/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */
#include <app/boot_mode.h>
#include <assert.h>
#include <atags.h>
#include <debug.h>
#include <libfdt.h>
#include <lib/pl_boottags.h>
#include <platform/boot_mode.h>
#include <set_fdt.h>
#include <string.h>
#include <trace.h>
#include <uart_api.h>

#define LOCAL_TRACE 2

#define BOOT_TAG_META_COM  0x88610003
#define ATAG_META_COM      0x41000803

struct meta_argument {
    uint32_t    meta_com_type;  /* identify meta via uart or usb */
    uint32_t    meta_com_id;    /* multiple meta need to know com port id */
    uint32_t    meta_uart_port; /* identify meta uart port */
};

static struct meta_argument meta_arg;

static void pl_boottags_meta_com_hook(struct boot_tag *tag)
{
    memcpy(&meta_arg, &tag->data, sizeof(meta_arg));
}
PL_BOOTTAGS_INIT_HOOK(meta_com, BOOT_TAG_META_COM, pl_boottags_meta_com_hook);

void set_fdt_atag_meta(void *fdt)
{
    #define LEN (5)
    int offset, ret;
    unsigned int buf[LEN] = {0};

    if (get_boot_mode() == META_BOOT) {
        buf[0] = tag_size(meta_argument);
        buf[1] = ATAG_META_COM;
        buf[2] = meta_arg.meta_com_type;
        buf[3] = meta_arg.meta_com_id;
        buf[4] = uart_base_to_id(meta_arg.meta_uart_port);
        offset = fdt_path_offset(fdt, "/chosen");
        ASSERT(offset >= 0);
        ret = fdt_setprop(fdt, offset, "atag,meta", buf, sizeof(buf));
        if (ret)
            ASSERT(0);

        LTRACEF("meta com type = %d\n", meta_arg.meta_com_type);
        LTRACEF("meta com id = %d\n", meta_arg.meta_com_id);
        LTRACEF("meta uart port = %d\n", meta_arg.meta_uart_port);
    }
}
SET_FDT_INIT_HOOK(set_fdt_atag_meta, set_fdt_atag_meta);
