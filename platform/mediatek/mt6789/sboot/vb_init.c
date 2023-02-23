/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#include <debug.h>
#include <oemkey.h>
#include <sboot.h>
#include <stdint.h>
#include <stdlib.h>
#include <verified_boot.h>
#include <verified_boot_common.h>
#include <verified_boot_error.h>

static uint8_t g_oemkey[OEM_PUBK_SZ] = {OEM_PUBK};

__WEAK int sec_set_oemkey(uint8_t *pubk, uint32_t size)
{
    return 0;
}

__WEAK int sec_func_init(uint64_t pl_start_addr)
{
    return 0;
}

static void plinfo_get_brom_header_block_size(uint64_t *pl_start_addr)
{
    *pl_start_addr = HEADER_BLOCK_SIZE_UFS;

    return;
}

int sec_lk_vb_init(void)
{
    int ret = 0;
#if (MTK_SECURITY_SW_SUPPORT)
    uint64_t pl_start_addr = 0;
    uint8_t *socid_addr = {0};

    socid_addr = sec_get_socid_addr();
    ret = sec_get_socid_from_pl_boottags(socid_addr, SOC_ID_LEN);
    if (ret)
        dprintf(INFO, "[SEC] Set SOCID to security library failed!\n");

    ret = sec_set_oemkey(g_oemkey, OEM_PUBK_SZ);
    if (ret)
        dprintf(CRITICAL, "[SEC] Fail to set oemkey. (0x%x)\n", ret);

    plinfo_get_brom_header_block_size(&pl_start_addr);
    ret = sec_func_init(pl_start_addr);
    if (ret)
        dprintf(CRITICAL, "[SEC] Fail to initialize sec function. (0x%x)\n", ret);
#endif

    return ret;
}
