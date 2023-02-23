/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#include <assert.h>
#include <kernel/vm.h>
#include <lib/pl_boottags.h>
#include <platform/plat_dbg_info.h>
#include <string.h>
#include <trace.h>

#define LOCAL_TRACE 0

static struct boot_tag_plat_dbg_info plat_dbg_info_arg;

static void pl_boottags_plat_dbg_info_hook(struct boot_tag *tag)
{
    if (tag->hdr.size - sizeof(tag->hdr) < sizeof(plat_dbg_info_arg)) {
        LTRACEF("%s Detect possible overrun access!\n", __func__);
        ASSERT(0);
    }
    memcpy(&plat_dbg_info_arg, (struct boot_tag_plat_dbg_info *)&tag->data,
           sizeof(plat_dbg_info_arg));
}
PL_BOOTTAGS_INIT_HOOK(boot_tag_plat_dbg_info, BOOT_TAG_PLAT_DBG_INFO,
                      pl_boottags_plat_dbg_info_hook);

void *get_dbg_info_base(unsigned int key)
{
    unsigned int i;
    void *val;

    for (i = 0; i < plat_dbg_info_arg.info_max; i++) {
        if (plat_dbg_info_arg.info[i].key == key) {
            val = paddr_to_kvaddr(plat_dbg_info_arg.info[i].base);
                return val;
        }
    }

    return 0;
}

unsigned int get_dbg_info_size(unsigned int key)
{
    unsigned int i;

    for (i = 0; i < plat_dbg_info_arg.info_max; i++) {
        if (plat_dbg_info_arg.info[i].key == key)
            return plat_dbg_info_arg.info[i].size;
    }

    return 0;
}
