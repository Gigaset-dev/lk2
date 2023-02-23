/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#include "gz_boot_tag.h"
#include <lib/pl_boottags.h>
#include <string.h>

/* GZ variables */
#define EL2_BOOT_DISABLE (1 << 0)
static uint32_t gz_configs = EL2_BOOT_DISABLE;
static uint64_t gz_exec_start_offset;
static uint64_t gz_reserved_mem_size;

uint32_t is_el2_enabled(void)
{
    if (gz_configs & EL2_BOOT_DISABLE)
        return 0; /* el2 is disabled */

    return 1; /* el2 is enabled */
}

uint64_t get_el2_exec_start_offset(void)
{
    return gz_exec_start_offset;
}

uint64_t get_el2_reserved_mem_size(void)
{
    return gz_reserved_mem_size;
}

/* gz info */
static void pl_boottags_gz_info_hook(struct boot_tag *tag)
{
    struct boot_tag_gz_info *p = (struct boot_tag_gz_info *)&tag->data;

    gz_configs = p->gz_configs;
}
PL_BOOTTAGS_INIT_HOOK(pl_boottags_gz_info_hook,
                      BOOT_TAG_GZ_INFO,
                      pl_boottags_gz_info_hook);

/* gz platform */
static void pl_boottags_gz_plat_hook(struct boot_tag *tag)
{
    struct boot_tag_gz_platform *p = (struct boot_tag_gz_platform *)&tag->data;

    /* do copy here to prevent un-aligned access for 64-bit memory */
    memcpy(&gz_exec_start_offset, &p->exec_start_offset, sizeof(uint64_t));

    /* 32-bit, access directly */
    gz_reserved_mem_size = p->reserve_mem_size & 0xFFFFFFFFUL;
}
PL_BOOTTAGS_INIT_HOOK(pl_boottags_gz_plat_hook,
                      BOOT_TAG_GZ_PLAT,
                      pl_boottags_gz_plat_hook);
