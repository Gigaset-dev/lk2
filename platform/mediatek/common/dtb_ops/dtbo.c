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
#include <assert.h>
#include <compiler.h>
#include <debug.h>
#include <dtb.h>
#include <dt_table.h>
#include <err.h>
#include <libfdt.h>
#include <load_image.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

/* global variable to send androidboot.dtbo_idx */
static uint32_t dtbo_idx = 0xff;
uint32_t get_dtbo_index(void)
{
    if (dtbo_idx == 0xff)
        panic("dtbo_idx is not inited!\n");
    return dtbo_idx;
}

/* default implementation, mtdbo indexing and table parsing could be customized */
__WEAK int custom_get_mdtbo_index(void)
{
    return 0;
}

static int32_t default_parse_dtbo_tbl(struct dt_table_header *dt_tbl_hdr_buffer,
        uint32_t selected_idx, uint32_t *dtbo_entry_idx_out,
        uint32_t *dtbo_size, uint32_t *dtbo_offset)
{
    struct dt_table_entry *dt_entry_head;
    struct dt_table_entry *dt_entry_selected;
    uint32_t dt_entry_cnt;
    uint32_t dtbo_entry_idx;

    ASSERT(dt_tbl_hdr_buffer);
    ASSERT(dtbo_entry_idx_out);
    ASSERT(dtbo_size);
    ASSERT(dtbo_offset);

    /* sanity check */
    if (fdt32_to_cpu(dt_tbl_hdr_buffer->magic) != DT_TABLE_MAGIC) {
        dprintf(CRITICAL, "dt_table_header magic is not correct:0x%x\n",
            fdt_magic(dt_tbl_hdr_buffer));
        return ERR_NOT_VALID;
    }

    dt_entry_cnt = fdt32_to_cpu(dt_tbl_hdr_buffer->dt_entry_count);
    dt_entry_head = (struct dt_table_entry *)(dt_tbl_hdr_buffer + 1);
    /*
     * traverse dt_table_entry list, compare
     * dt_table_entry->id with hw_board_idx
     */
    for (dtbo_entry_idx = 0; dtbo_entry_idx < dt_entry_cnt ; dtbo_entry_idx++) {
        if (selected_idx == fdt32_to_cpu((dt_entry_head + dtbo_entry_idx)->id))
            break;
    }
    /* traverse end */

    if (dtbo_entry_idx >= dt_entry_cnt) {
        dprintf(CRITICAL, "Set dtbo_entry_idx to 0: dtbo_entry_idx %d >= num_of_dtbo %d.\n",
            dtbo_entry_idx, dt_entry_cnt);
        dtbo_entry_idx = 0;
    }

    *dtbo_entry_idx_out = dtbo_entry_idx;
    dt_entry_selected = dt_entry_head + dtbo_entry_idx;
    *dtbo_size = fdt32_to_cpu(dt_entry_selected->dt_size);
    *dtbo_offset = fdt32_to_cpu(dt_entry_selected->dt_offset);

    return NO_ERROR;
}

int32_t parse_dtbo(void *buf, parse_dtbo_func cb, void **dtbo_ptr, uint32_t *dtbo_size)
{
    int32_t ret = NO_ERROR;
    parse_dtbo_func table_parser;
    uint32_t dtbo_offset;
    uint32_t _dtbo_size;
    void *dtbo_buffer = NULL;
    void *selected_dtbo_body_ptr;

    if (cb)
        table_parser = cb;
    else
        table_parser = default_parse_dtbo_tbl;

    /* select dtbo via selected index */
    ret = (*table_parser)((struct dt_table_header *)buf,
                custom_get_mdtbo_index(),
                &dtbo_idx,
                (uint32_t *)&_dtbo_size,
                (uint32_t *)&dtbo_offset);
    if (ret < 0)
        goto end;

    dtbo_buffer = malloc(_dtbo_size);
    if (dtbo_buffer == NULL) {
        ret = ERR_NO_MEMORY;
        goto end;
    }

    *dtbo_size = _dtbo_size;
    selected_dtbo_body_ptr = buf + dtbo_offset;
    memcpy(dtbo_buffer, selected_dtbo_body_ptr, _dtbo_size);
    *dtbo_ptr = dtbo_buffer;

end:
    return ret;
}

int32_t load_dtbo(const char *part_name, const char *img_name,
            parse_dtbo_func cb, void **dtbo_ptr, uint32_t *dtbo_size)
{
    void *dtbo_total_buffer = NULL;
    int32_t ret = NO_ERROR;

    dtbo_total_buffer = malloc(DTBO_MAX_SIZE);
    if (dtbo_total_buffer == NULL) {
        ret = ERR_NO_MEMORY;
        goto end;
    }

    ret = load_partition(part_name, img_name, dtbo_total_buffer, DTBO_MAX_SIZE);
    if (ret < 0)
        goto end;

    ret = parse_dtbo(dtbo_total_buffer, cb, dtbo_ptr, dtbo_size);
    if (ret < 0)
        dprintf(CRITICAL, "dtbo parse fail\n");

end:
    if (dtbo_total_buffer)
        free(dtbo_total_buffer);

    return ret;
}
