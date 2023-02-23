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
#include <app/mt_boot.h>
#include <assert.h>
#include <atags.h>
#include <lib/pl_boottags.h>
#include <libfdt.h>
#include <partition_utils.h>
#include <platform/boot_mode.h>
#include <set_fdt.h>
#include <string.h>

#define ATAG_BOOT     0x41000802

/* boot information */
struct tag_boot {
    u32 bootmode;
};

void set_fdt_atag_boot(void *fdt)
{
    #define LEN (4)
    int offset, ret;
    unsigned int buf[LEN] = {0};

    buf[0] = tag_size(tag_boot);
    buf[1] = ATAG_BOOT;
    buf[2] = get_boot_mode();
    buf[3] = partition_get_bootdev_type();
    offset = fdt_path_offset(fdt, "/chosen");
    ASSERT(offset >= 0);
    ret = fdt_setprop(fdt, offset, "atag,boot", buf, sizeof(buf));
    if (ret)
        ASSERT(0);
}
SET_FDT_INIT_HOOK(set_fdt_atag_boot, set_fdt_atag_boot);

void set_fdt_firmware(void *fdt)
{
    int offset, ret;
    const char *name, *value;

    offset = fdt_path_offset(fdt, "/firmware/android");
    if (offset < 0) {
        offset = fdt_path_offset(fdt, "/firmware");
        if (offset < 0) {
            /* Add subnode "firmware" in root */
            offset = fdt_add_subnode(fdt, 0, "firmware");
            if (offset < 0) {
                dprintf(CRITICAL, "Warning: can't add firmware node in device tree\n");
                return;
            }
        }

        /* Add subnode "android" in "/firmware" */
        offset = fdt_add_subnode(fdt, offset, "android");
        if (offset < 0) {
            dprintf(CRITICAL, "Warning: can't add firmware/android node in device tree\n");
            return;
        }
    }

    /* Add properties in "/firmware/android" */
    name = "compatible";
    value = "android,firmware";
    ret = fdt_setprop_string(fdt, offset, name, value);
    if (ret)
        ASSERT(0);

    name = "serialno";
    ret = fdt_setprop_string(fdt, offset, name, get_serialno());
    if (ret)
        ASSERT(0);

    name = "mode";

    switch (get_boot_mode()) {
    case META_BOOT:
        value = "meta";
        break;
    case KERNEL_POWER_OFF_CHARGING_BOOT:
        value = "charger";
        break;
    case FACTORY_BOOT:
        value = "factory";
        break;
    case RECOVERY_BOOT:
        value = "recovery";
        break;
    default:
        value = "normal";
        break;
    }

    ret = fdt_setprop_string(fdt, offset, name, value);
    if (ret)
        ASSERT(0);
}
SET_FDT_INIT_HOOK(set_fdt_firmware, set_fdt_firmware);
