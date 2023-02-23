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
#include <app/boot_mode.h>
#include <assert.h>
#include <atags.h>
#include <debug.h>
#include <lib/kcmdline.h>
#include <lib/pl_boottags.h>
#include <libfdt.h>
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
    uint32_t    meta_log_disable;
    uint32_t    fast_meta_gpio;
};

static struct meta_argument meta_arg;

/* Set androidboot.usbconfig for META/FACTORY mdoe */
void set_usbconfig(u8 boot_mode)
{
    uint32_t adb = !(meta_arg.meta_com_id & 0x0001);
    uint32_t elt = !!(meta_arg.meta_com_id & 0x0004);

    if (boot_mode == META_BOOT) {
        if (!adb && !elt) {
            /*only META*/
            kcmdline_append("androidboot.usbconfig=1 usbconfig=1");
        } else if (adb && !elt) {
            /*META + ADB*/
            kcmdline_append("androidboot.usbconfig=0 usbconfig=0");
        } else if (!adb && elt) {
            /*META + ELT*/
            kcmdline_append("androidboot.usbconfig=2 usbconfig=2");
        } else {
            /*META + ELT + ADB*/
            kcmdline_append("androidboot.usbconfig=3 usbconfig=3");
        }

        if ((meta_arg.meta_com_id & 0x0002) != 0)
            kcmdline_append("androidboot.mblogenable=0");
        else
            kcmdline_append("androidboot.mblogenable=1");
        LTRACEF("adb:%d elt:%d meta_com_id:%x\n", adb, elt, meta_arg.meta_com_id);
    } else if (boot_mode == FACTORY_BOOT) {
        if (!adb)
            kcmdline_append("androidboot.usbconfig=1 usbconfig=1");
        else
            kcmdline_append("androidboot.usbconfig=0 usbconfig=0");

        LTRACEF("adb:%d elt:%d\n", adb, elt);
    }

    if (meta_arg.meta_log_disable == 1)
        kcmdline_append("androidboot.meta_log_disable=1");
    else
        kcmdline_append("androidboot.meta_log_disable=0");
}

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

    if (get_boot_mode() == META_BOOT || get_boot_mode() == FACTORY_BOOT) {
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
