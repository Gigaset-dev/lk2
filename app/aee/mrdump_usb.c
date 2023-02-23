/*
 * Copyright (c) 2019 MediaTek Inc.
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files
 * (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge,
 * publish, distribute, sublicense, and/or sell copies of the Software,
 * and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */
#include <debug.h>
#include <fastboot.h>
#include <fastboot_oem_cmd.h>
#include <fastboot_usb.h>
#include <malloc.h>
#include <platform.h>
#include <platform/mboot_params.h>
#include <platform/mrdump_expdb.h>
#include <platform/wdt.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <trace.h>

#include "aee.h"
#include "KEHeader.h"
#include "mrdump_private.h"

#define LOCAL_TRACE 0

/*
 * USB Connectivity
 * Note: for usb transmission
 *   QMU mode : MAX packet length: 63x1024 = 64512 byte. -> GPD_BUF_SIZE_ALIGN
 *   ZLP issue: EXSPACE should not be multiple size of 512 byte.
 */
#define EXSPACE      64256
#define MAX_RSP_SIZE 64
#define CON_TIMEOUT  5000

/* Flow control */
#define USB_RTS      "_RTS"
#define USB_CTS      "_CTS"
#define USB_FIN      "_FIN"
#define USBDONE      "DONE"

/*
 * Need to align 64 bytes to meet cache line length
 * 1. cmd[MAX_RSP_SIZE]
 * 2. data[EXSPACE]
 */
struct mrdump_usb_handle {
    char cmd[MAX_RSP_SIZE];
    uint8_t data[EXSPACE];
    unsigned int zipsize;
    unsigned int idx;
};

/* flow control of usb connection */
static bool usb_data_transfer(struct mrdump_usb_handle *handle, int length)
{
    int len;

    /* send RTS */
    memset(handle->cmd, 0, MAX_RSP_SIZE);
    len = snprintf(handle->cmd, MAX_RSP_SIZE, "%s", USB_RTS);
    len = usb_write(handle->cmd, strlen(handle->cmd));
    if (len > 0) {

        /* receive Clear-to-Send */
        memset(handle->cmd, 0, MAX_RSP_SIZE);
        len = usb_read(handle->cmd, MAX_RSP_SIZE);
        if ((len == (int)strlen(USB_CTS)) &&
            (!strncmp(handle->cmd, USB_CTS, strlen(USB_CTS)))) {
            /* send DATA */
            len = usb_write(handle->data, length);
            if (len > 0) {
                /* get FIN */
                memset(handle->cmd, 0, sizeof(handle->cmd));
                len = usb_read(handle->cmd, sizeof(handle->cmd));
                if ((len == (int)strlen(USB_FIN)) &&
                    (!strncmp(handle->cmd, USB_FIN, strlen(USB_FIN)))) {
                    return true;
                } else {
                    voprintf_error("%s: failed to get FIN.cmd<%p,%d><%x,%x,%x,%x>\n", __func__,
                        handle->cmd, len, handle->cmd[0], handle->cmd[1],
                        handle->cmd[2], handle->cmd[3]);
                }
            } else {
                voprintf_error("%s: send DATA error.\n", __func__);
            }
        } else {
            voprintf_error("%s: Not Clear-to-Send after RTS.cmd<%p,%d><%x,%x,%x,%x>\n", __func__,
                handle->cmd, len, handle->cmd[0], handle->cmd[1], handle->cmd[2], handle->cmd[3]);
        }
    } else {
        voprintf_error("%s: send RTS error.\n", __func__);
    }
    return false;
}

/* store data in pool (EXSPACE) and write when pool is full */
static int do_store_or_write(struct mrdump_usb_handle *handle, uint8_t *buf, uint32_t length)
{
    int total;
    unsigned int leftspace, mylen, reval;


    /* count for leftspace */
    total = EXSPACE;
    leftspace = total - handle->idx;

    /* check length */
    if (length > leftspace) {
        mylen = leftspace;
        reval = length - leftspace;
    } else {
        mylen = length;
        reval = 0;
    }

    /* store */
    while (mylen > 0) {
        handle->data[handle->idx] = *buf;
        handle->idx++;
        buf++;
        mylen--;
    }

    /* write */
    if (handle->idx == total) {
        if (!usb_data_transfer(handle, handle->idx)) {
            voprintf_error("%s: connection failed.(error idx: %d)\n",
            __func__, handle->idx);
            return -1;
        }
        handle->idx = 0;
    }

    return reval;
}

static int usb_write_cb(void *opaque_handle, void *buf, int size)
{
    unsigned int    len, moves;
    int             ret = 0;
    uint8_t         *ptr;

    struct mrdump_usb_handle *handle = opaque_handle;

    handle->zipsize += size;

    /* EOF, write the left Data in handle data buffer... */
    if ((buf == NULL) && (size == 0)) {

        /* MUST: a delay for the last transmission */
        mdelay(10);

        if (!usb_data_transfer(handle, handle->idx)) {
            voprintf_error("%s: connection failed.(error idx: %d)\n",
            __func__, handle->idx);
            return -1;
        }

        /* send "MRDUMP END Packet: pattern: MRDUMP08_DONE" */
        memset((void *)handle->data, 0, sizeof(handle->data));
        size = snprintf((char *)handle->data, sizeof(handle->data), "MRDUMP08_DONE");
        if (size < 0) {
            voprintf_error(" USB Dump: Make MRDUMP END Packet failed.\n");
            return -1;
        }
        if (usb_write(handle->data, strlen((char *)handle->data)) < 0) {
            voprintf_error(" USB Dump: Write MRDUMP ZLP failed.\n");
            return -1;
        }

        return 0;
    }

    /* buf should not be NULL if not EOF */
    if (buf == NULL)
        return -1;

    /* process of Store and write */
    len = size;
    ptr = (uint8_t *)buf;
    while (1) {
        ret = do_store_or_write(handle, ptr, len);
        if (ret < 0) {
            voprintf_error(" USB Dump: store and write failed.\n");
            return -1;
        } else if (ret == 0) {
            break;
        } else {
            moves = len - ret;
            ptr += moves;
            len = ret;
        }
    }

    return size;
}

int mrdump_usb_output(const struct mrdump_control_block *mrdump_cb,
              const struct kzip_addlist *memlist,
              const struct kzip_addlist *memlist_cmm)
{
    int i;
    struct kzip_addlist expdb_file[2] = {0};
    char *expdb_filename = NULL;
    unsigned int expdb_offset = 0, expdb_filesize = 0;
    bool ok = true;
    struct kzip_file *zf;
    struct mrdump_usb_handle *handle;

    voprintf_info("Output by USB\n");

    handle = memalign(64, sizeof(struct mrdump_usb_handle));
    if (handle == NULL) {
        LTRACEF_LEVEL(ALWAYS, "No enough memory.");
        return -1;
    }
    memset(handle, 0, sizeof(struct mrdump_usb_handle));

    mdelay(100);
    mtk_wdt_restart_timer();
    zf = kzip_open(handle, usb_write_cb);
    if (zf != NULL) {
        /* add SYS_COREDUMP */
        if (!kzip_add_file(zf, memlist, "SYS_COREDUMP"))
            ok = false;

        if (ok && !kzip_add_file(zf, memlist_cmm, "kernel.cmm")) {
            ok = false;
            mrdump_memlist_free_cmm(memlist_cmm);
        }
        if (!ok) {
            LTRACEF_LEVEL(ALWAYS, "failed to generate cmm\n");
            kzip_close(zf);
            return -1;
        }

        /* add file on expdb */
        for (i = 0; i < IPANIC_NR_SECTIONS; i++) {
            if (kedump_get_data_info(i, &expdb_filename, &expdb_offset, &expdb_filesize) == 0) {
                expdb_file[0].address = expdb_offset;
                expdb_file[0].size = expdb_filesize;
                expdb_file[0].type = EXPDB_FILE;
                expdb_file[0].memmap = 0;
                expdb_file[0].pageflags = 0;
                expdb_file[0].struct_page_size = 0;
                expdb_file[1].address = 0;
                expdb_file[1].size = 0;
                expdb_file[1].type = MEM_NO_MAP;
                expdb_file[1].memmap = 0;
                expdb_file[1].pageflags = 0;
                expdb_file[1].struct_page_size = 0;
                if (!kzip_add_file(zf, expdb_file, expdb_filename))
                    ok = false;
            }
        }

        /* close zipfile */
        kzip_close(zf);
        usb_write_cb(handle, NULL, 0); /* really write the last part */
        zf = NULL;
    } else {
        ok = false;
    }
    free(handle);

    mtk_wdt_restart_timer();

    return ok ? 0 : -1;
}


void mrdump_read_expdb(void *data, int len, u64 offset_src)
{
    bdev_t *dev_expdb;
    u64 part_size;
    int out_len;

    dev_expdb = bio_open_by_label(AEE_IPANIC_LABEL);
    if (!dev_expdb) {
        dprintf(CRITICAL, "kedump: Partition [%s] is not exist.\n", AEE_IPANIC_LABEL);
        return;
    }

    part_size = dev_expdb->total_size;

    if (part_size < offset_src) {
        dprintf(CRITICAL, "%s: partition size(%llx) is less then reserved (%llx)\n",
            __func__, part_size, offset_src);
        bio_close(dev_expdb);
        return;
    }

    if ((offset_src + len) > (part_size - MRDUMP_EXPDB_DRAM_KLOG_OFFSET)) {
        dprintf(CRITICAL, "%s: log size(0x%x) too big.\n", __func__, len);
        bio_close(dev_expdb);
        return;
    }

    out_len = bio_read(dev_expdb, (uchar *)data, offset_src, len);
    if (out_len != len) {
        dprintf(CRITICAL, "%s: read expdb fail\n", __func__);
        bio_close(dev_expdb);
        return;
    }
    bio_close(dev_expdb);
}


void cmd_oem_mrdump(const char *arg, void *data, unsigned int sz)
{
    if (strncmp(arg, " fullreset", 10) == 0) {
        dprintf(ALWAYS, "[FASTBOOT] mrdump reset!");
        fastboot_okay("");
        platform_halt(HALT_ACTION_REBOOT, HALT_REASON_POR); /* PMIC cold reset */
    }

    dprintf(ALWAYS, "[FASTBOOT] MRDUMP Dump Go!\n");
    fastboot_okay("");
    mrdump_run();
}

FASTBOOT_OEM_CMD_START(cmd_oem_mrdump)
    .cmd_str = "oem mrdump",
    .cmd_handler = cmd_oem_mrdump,
    .allowed_when_security_on = true,
    .forbidden_when_lock_on = false,
FASTBOOT_OEM_CMD_END
