/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#include <debug.h>
#include <oemkey.h>
#include <stdint.h>
#include <stdlib.h>
#include <verified_boot.h>

#define HEADER_BLOCK_SIZE_UFS (4096)

uint8_t g_oemkey[OEM_PUBK_SZ] = {OEM_PUBK};

static void plinfo_get_brom_header_block_size(uint64_t *pl_start_addr)
{
    //hardcode here since lk2 only support UFS device temporarily.
    *pl_start_addr = HEADER_BLOCK_SIZE_UFS;

    return;
}

int sec_lk_vb_init(void)
{
    int ret = 0;
    uint64_t pl_start_addr = 0;

    ret = seclib_set_oemkey(g_oemkey, OEM_PUBK_SZ);
    if (ret != 0)
        dprintf(CRITICAL, "[SEC] Fail to set oemkey. (0x%x)\n", ret);
    else
        dprintf(CRITICAL, "[SEC] Finish to set oemkey.");

    plinfo_get_brom_header_block_size(&pl_start_addr);
    ret = sec_func_init(pl_start_addr);
    if (ret != 0)
        dprintf(CRITICAL, "[SEC] Fail to initialize sec function. (0x%x)\n", ret);

    return ret;
}
