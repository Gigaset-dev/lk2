/*
 * Copyright (c) 2009, Google Inc.
 * Copyright (c) 2019, MediaTek Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include <debug.h>
#include <fastboot_usb.h>
#include <lib/bio.h>
#include <kernel/event.h>
#include <kernel/thread.h>
#include <stdlib.h>
#include <string.h>
#include "bulk_process.h"
#include "flash_commands.h"
#include "sparse_format.h"
#include "sparse_state_machine.h"
#include "transfer.h"

#if defined MTK_ULTRA_FLASH

//#define TIME_STAMP gpt4_tick2time_ms(gpt4_get_current_tick())

#define CACHE_PADDING_SIZE STORAGE_BLOCK_SIZE
#define SYSOB_CACHE_PAGE (2 * 1024 * 1024U)
#define SYSOB_BIG_CACHE (2 * 1024 * 1024U)
#define SIGNAL_RESCHEDULE 0 // true; active. 0 passive.

/***********************************************************************************
 *                 DOWNLOAD ENGINE
 ***********************************************************************************/
enum engine_op_part {
    EOP_DATA_PROVIDER,
    EOP_DATA_CONSUMER,
};

struct cache {
    uint8_t *padding_buf;
    uint8_t *cache_buf;
    uint32_t padding_length; // sparse image boundary problem.
    uint32_t content_length; // if this is 0, indicate this the last package.
    event_t content_available;
    event_t cache_available;
};

struct engine_context {
    uint8_t *cache_base;
    struct cache dual_cache[2];
    uint32_t flipIdxR;         // receive buffer shift
    uint32_t flipIdxW;         // write buffer shift
    event_t thrR_end_ev;     // receive thread exit sync.
    event_t thrW_end_ev;     // write thread exit sync.
    status_t status_usb;     // if something is wrong, should exit.
    status_t status_storage; // if something is wrong, should exit.
};

static struct engine_context *ctx;

#define ALIGN_SZ (64)

#define MM_BASE get_available_ram_base()
#define MM_1ST_16M (((uint8_t *)get_available_ram_base()) + 3 * SYSOB_BIG_CACHE)

uint8_t *get_global_cache1(void)
{
    return MM_1ST_16M;
}

void init_engine_context(struct engine_context *tx)
{
    /*dual cache pattern:     | PADDING1 | CACHE1 || PADDING2 | CACHE2 |*/
    uint32_t CACHE_PAGE_SIZE = SYSOB_CACHE_PAGE;
    // uint32_t DCACHE_SIZE  = (2*CACHE_PAGE_SIZE+2*CACHE_PADDING_SIZE);

    tx->cache_base = (uint8_t *)get_available_ram_base();

    tx->dual_cache[0].padding_buf = tx->cache_base;
    tx->dual_cache[0].cache_buf = tx->cache_base + CACHE_PADDING_SIZE;
    tx->dual_cache[1].padding_buf =
        tx->cache_base + CACHE_PADDING_SIZE + CACHE_PAGE_SIZE;
    tx->dual_cache[1].cache_buf = tx->cache_base + CACHE_PADDING_SIZE +
        CACHE_PAGE_SIZE + CACHE_PADDING_SIZE;

    tx->dual_cache[0].padding_length = tx->dual_cache[1].padding_length = 0;

    // no data in cache
    event_init(&tx->dual_cache[0].content_available, 0, EVENT_FLAG_AUTOUNSIGNAL);
    event_init(&tx->dual_cache[1].content_available, 0, EVENT_FLAG_AUTOUNSIGNAL);

    // can receive from usb
    event_init(&tx->dual_cache[0].cache_available, 1, EVENT_FLAG_AUTOUNSIGNAL);
    event_init(&tx->dual_cache[1].cache_available, 1, EVENT_FLAG_AUTOUNSIGNAL);

    event_init(&tx->thrR_end_ev, 0, EVENT_FLAG_AUTOUNSIGNAL); // do not end.
    event_init(&tx->thrW_end_ev, 0, EVENT_FLAG_AUTOUNSIGNAL); // do not end.
    tx->status_usb = STATUS_OK;
    tx->status_storage = STATUS_OK;
    tx->flipIdxR = tx->flipIdxW = 0;
}

inline uint32_t cache_shift(uint32_t pre)
{
    return pre ^ 0x01;
}

void stop_engine(struct engine_context *tx, enum engine_op_part part)
{
    if (part == EOP_DATA_CONSUMER) {
        event_signal(&tx->dual_cache[0].cache_available, SIGNAL_RESCHEDULE);
        event_signal(&tx->dual_cache[1].cache_available, SIGNAL_RESCHEDULE);
        event_signal(&tx->thrR_end_ev, SIGNAL_RESCHEDULE);
    } else if (part == EOP_DATA_PROVIDER) {
        event_signal(&tx->dual_cache[0].content_available, SIGNAL_RESCHEDULE);
        event_signal(&tx->dual_cache[1].content_available, SIGNAL_RESCHEDULE);
        event_signal(&tx->thrW_end_ev, SIGNAL_RESCHEDULE);
    }
}

void destroy_engine(struct engine_context *tx)
{
    event_destroy(&tx->dual_cache[0].cache_available);
    event_destroy(&tx->dual_cache[1].cache_available);
    event_destroy(&tx->dual_cache[0].content_available);
    event_destroy(&tx->dual_cache[1].content_available);
    event_destroy(&tx->thrR_end_ev);
    event_destroy(&tx->thrW_end_ev);

    tx->cache_base = 0;
}
/***********************************************************************************
 *                 DOWNLOAD
 ***********************************************************************************/

static struct download_data_context *write_data_ctx;

status_t write_data(uint8_t *data, uint32_t length)
{
    int next_flip;
    static struct unsparse_status unsparse_status; // sparse image parsing used.
    static struct bulk_status bulk_status;         // bulk image parsing used.
    static bool is_sparse;
    uint64_t total_image_length = 0; // if sparse image, it should be size after unsparsed.

    if (data == 0)
        return STATUS_INVALID_PARAMETERS;

    if (write_data_ctx->first_run) {
        is_sparse = is_sparse_image(data, length);
        total_image_length = write_data_ctx->length_to_write;

        if (is_sparse) {
            init_unsparse_status(&(unsparse_status), write_data_ctx->part_info);
            total_image_length = unspared_size(data);
        } else {
            init_bulk_process_status(&(bulk_status), write_data_ctx->part_info);
            total_image_length = write_data_ctx->length_to_write;
        }

        if (total_image_length > write_data_ctx->part_info->max_size) {
            dprintf(ALWAYS, "size too large, space small.");
            dprintf(ALWAYS, "image length[0x%llx],partition max size[0x%llx]\n",
                 total_image_length, write_data_ctx->part_info->max_size);
            return STATUS_TOO_LARGE;
        }

        // The first run flag will be reset later after erase_before_download
        // finished.
        write_data_ctx->first_run = 0;
    }

    if (is_sparse) {
        next_flip = cache_shift(ctx->flipIdxR);

        if (length != 0) {
            write_sparse_data(&unsparse_status, data, length);
            if (unsparse_status.handle_status == STATUS_SPARSE_INCOMPLETE) {
                memcpy(ctx->dual_cache[next_flip].padding_buf +
                    (CACHE_PADDING_SIZE - unsparse_status.byte_to_process),
                       unsparse_status.buf, unsparse_status.byte_to_process);

                ctx->dual_cache[next_flip].padding_length =
                    unsparse_status.byte_to_process;
                unsparse_status.handle_status = STATUS_OK;
            } else if (unsparse_status.handle_status == STATUS_OK) {
                ctx->dual_cache[next_flip].padding_length = 0;
            }
        } else {
            // the last package.
            end_write_sparse_data(&unsparse_status);
        }
        return unsparse_status.handle_status;
    } else {
        if (length != 0)
            write_bulk_data(&bulk_status, data, length);
        else // the last package.
            end_write_bulk(&bulk_status);

        return bulk_status.handle_status;
    }
}

int write_storage_proc(void *arg)
{
    uint8_t *data = 0;
    uint32_t data_len = 0;

    for (;;) {
        event_wait(&(ctx->dual_cache[ctx->flipIdxR].content_available));

        if (FAIL(ctx->status_usb))
            goto exit;

        // if has something to write
        data = (uint8_t *)(ctx->dual_cache[ctx->flipIdxR].cache_buf);
        data_len = ctx->dual_cache[ctx->flipIdxR].content_length;

        data -= ctx->dual_cache[ctx->flipIdxR].padding_length;
        data_len += ctx->dual_cache[ctx->flipIdxR].padding_length;
        ctx->status_storage = write_data(data, data_len);

        if (ctx->status_storage != STATUS_OK) {
            // error
            dprintf(ALWAYS, "write data failed. handle_status(%d)\n", ctx->status_storage);
            goto exit;
        }

        // last package, should return;
        if (ctx->dual_cache[ctx->flipIdxR].content_length == 0)
            break;

        event_signal(&ctx->dual_cache[ctx->flipIdxR].cache_available,
                     SIGNAL_RESCHEDULE);
        // make this cache writeable again.

        ctx->flipIdxR = cache_shift(ctx->flipIdxR); // change next buffer.
    }

exit:
    stop_engine(ctx, EOP_DATA_CONSUMER);
    thread_exit(0);
    // never arrive here.
    return 0;
}

void read_usb_proc(uint64_t data_length)
{
    uint64_t bytes_already_read = 0;
    uint64_t bytes_to_read = 0;
    uint32_t CACHE_PAGE_SIZE = SYSOB_CACHE_PAGE;
    uint32_t TOTAL_CACHE_PAGE_SIZE = SYSOB_BIG_CACHE;

    while (bytes_already_read < data_length) {
        event_wait(&(ctx->dual_cache[ctx->flipIdxW].cache_available));

        ctx->dual_cache[ctx->flipIdxW].content_length = 0;
        uint32_t cache_offset = 0;

        while ((cache_offset < TOTAL_CACHE_PAGE_SIZE) &&
            (bytes_already_read < data_length)) {
            bytes_to_read = data_length - bytes_already_read;
            bytes_to_read =
                bytes_to_read >= CACHE_PAGE_SIZE ? CACHE_PAGE_SIZE : bytes_to_read;

            // ctx->dual_cache[ctx->flipIdxW].cache_buf+cache_offset);
            int r = usb_read(ctx->dual_cache[ctx->flipIdxW].cache_buf + cache_offset,
                (uint32_t)bytes_to_read);

            if ((r < 0) || ((uint32_t)r != (uint32_t)bytes_to_read)) {
                ctx->status_usb = STATUS_USB_ERR;
                dprintf(ALWAYS, "Read usb error. code 0x%x\n", ctx->status_usb);
                goto exit;
            }

            ctx->dual_cache[ctx->flipIdxW].content_length += bytes_to_read;
            bytes_already_read += bytes_to_read;
            cache_offset += bytes_to_read;

            // storage write error
            if (FAIL(ctx->status_usb) || FAIL(ctx->status_storage))
                goto exit;

            // display_progress("\rFlash: ", bytes_already_read,
            // write_data_ctx->length_to_write);
        }

        event_signal(&ctx->dual_cache[ctx->flipIdxW].content_available,
                     SIGNAL_RESCHEDULE);

        ctx->flipIdxW = cache_shift(ctx->flipIdxW); // change next buffer.
    }

    // last package.
    // must wait for this can write again. wait for storage write finish.
    event_wait(&(ctx->dual_cache[ctx->flipIdxW].cache_available));
    // notify finish info to storage write thread with zero length packet.
    ctx->dual_cache[ctx->flipIdxW].content_length = 0;
    event_signal(&ctx->dual_cache[ctx->flipIdxW].content_available,
                 SIGNAL_RESCHEDULE);

exit:
    stop_engine(ctx, EOP_DATA_PROVIDER);
    return;
}

// Big image and parallel transfer.
status_t download_data(uint64_t data_length, struct partition_info_struct *part_info)
{
    thread_t *thr;
    struct download_data_context data_ctx;
    struct engine_context engine_ctx;

#ifdef DUMP_SPEED
    uint32_t time_start = 0;
    uint32_t time_end = 0;
#endif

    write_data_ctx = &data_ctx;
    ctx = &engine_ctx;

    init_engine_context(ctx);
    init_download_data_context(write_data_ctx, data_length, part_info);

#ifdef DUMP_SPEED
    time_start = TIME_STAMP;
#endif

    thr = thread_create("write_storage_proc", write_storage_proc, 0,
                        HIGHEST_PRIORITY, 16 * 1024);

    if (!thr) {
        dprintf(ALWAYS, "create write_storage_proc thread failed.");
        return STATUS_THREAD;
    }

    thread_resume(thr);
    read_usb_proc(data_length);

    // wait for thread end.
    event_wait(&ctx->thrR_end_ev);
    event_wait(&ctx->thrW_end_ev);

#ifdef DUMP_SPEED
    time_end = TIME_STAMP;
#endif

    destroy_engine(ctx);

    return FAIL(ctx->status_storage) ? ctx->status_storage : ctx->status_usb;
}

#endif
