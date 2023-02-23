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
#include <debug.h>
#include <dtb.h>
#include <dt_table.h>
#include <err.h>
#include <libfdt.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <ufdt_overlay.h>

int32_t dtb_overlay(void *main_dtb_addr, uint32_t main_dtb_size,
                    void *dtbo_addr, uint32_t dtbo_size, void **merged_dtb)
{
    int ret = NO_ERROR;
    struct fdt_header *blob;
    struct fdt_header *merged_fdt;

    ASSERT(main_dtb_addr);
    ASSERT(main_dtb_size);
    ASSERT(dtbo_addr);
    ASSERT(dtbo_size);
    ASSERT(merged_dtb);

    blob = ufdt_install_blob(main_dtb_addr, main_dtb_size);
    if (blob == NULL) {
        dprintf(CRITICAL, "ufdt_install_blob() failed!\n");
        return ERR_INVALID_ARGS;
    }

    merged_fdt = ufdt_apply_overlay(blob, main_dtb_size, dtbo_addr, dtbo_size);
    if (merged_fdt == NULL) {
        dprintf(CRITICAL, "ufdt_apply_overlay() failed!\n");
        return ERR_INVALID_ARGS;
    }

    dprintf(CRITICAL, "after overlay, fdt size(%d)\n", fdt_totalsize(merged_fdt));

    *merged_dtb = (void *)merged_fdt;
    return ret;
}
