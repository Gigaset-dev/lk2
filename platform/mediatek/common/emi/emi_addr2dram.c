/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#include <compiler.h>
#include <debug.h>
#include <emi_addr2dram.h>
#include <libfdt.h>
#include <reg.h>

#include "emi_addr2dram_v1.h"
#include "emi_addr2dram_v2.h"
#include "emi_util.h"

struct emicen_of_id {
    char *compatible;
    int a2d_ver;
};

static struct emicen_of_id emicen_of_ids[] = {
    { .compatible = "mediatek,common-emicen", .a2d_ver = 1 },
    { .compatible = "mediatek,mt6873-emicen", .a2d_ver = 1 },
    { .compatible = "mediatek,mt6877-emicen", .a2d_ver = 2 },
    { .compatible = NULL, .a2d_ver = -1 }
};

__WEAK int emi_get_addr2dram_version(void)
{
    return 0;
}

int emi_addr2dram(unsigned long addr, struct emi_addr_map *map)
{
    static int version = -1;
    void *fdt;
    int array_len, off, i;

    if (version < 0) {
        array_len = countof(emicen_compatible);
        fdt = emi_get_fdt();

        if (fdt) {
            for (i = 0; i < array_len; i++) {
                off = fdt_path_offset(fdt, emicen_compatible[i]);
                if (off >= 0)
                    break;
            }
            if (i >= array_len) {
                EMI_DBG("%s couldn't find the emicen node\n", __func__);
                return -1;
            }

            array_len = countof(emicen_of_ids);
            for (i = 0; i < array_len; i++) {
                if (!fdt_node_check_compatible(fdt, off, emicen_of_ids[i].compatible)) {
                    version = emicen_of_ids[i].a2d_ver;
                    break;
                }
            }
        } else
            version = emi_get_addr2dram_version();
    }

    if (version < 0) {
        EMI_DBG("%s couldn't match the compatible\n", __func__);
        return -1;
    } else
        EMI_DBG("%s version %d\n", __func__, version);

    if (version == 1)
        return emi_addr2dram_v1(addr, map);
    else if (version == 2)
        return emi_addr2dram_v2(addr, map);
    else
        return -1;
}
