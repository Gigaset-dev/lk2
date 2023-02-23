/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */
#include <app/boot_mode.h>
#include <app/gem_boot.h>
#include <atags.h>
#include <assert.h>
#include <lib/pl_boottags.h>
#include <libfdt.h>
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
    buf[3] = 2; // UFS. TODO: to use storage API
    offset = fdt_path_offset(fdt, "/chosen");
    ASSERT(offset >= 0);
    ret = fdt_setprop(fdt, offset, "atag,boot", buf, sizeof(buf));
    if (ret)
        ASSERT(0);
}
SET_FDT_INIT_HOOK(set_fdt_atag_boot, set_fdt_atag_boot);
