/*
 * Copyright (c) 2020 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#include <compiler.h>
#include <lk/init.h>
#include <pmif_common.h>
#include <spmi_common.h>

#define SPMI_GROUP_ID   0xB

static struct pmif mt6873_pmif_m_arb = {
    .swinf_ch_start = PMIF_SWINF_0_CHAN_NO,
    .swinf_no = PMIF_AP_SWINF_NO,
    .mstid = SPMI_MASTER_0,
    .pmifid = PMIF_SPMI,
    .read_cmd = pmif_spmi_read_cmd,
    .write_cmd = pmif_spmi_write_cmd,
};

static struct spmi_device mt6873_spmi_dev[] = {
    {
        .slvid = SPMI_SLAVE_6,
        .grpiden = 0x1 << SPMI_GROUP_ID,
        .mstid = SPMI_MASTER_0,
        .hwcid_addr = 0x0009,
        .hwcid_val = 0x15,
        .swcid_addr = 0x000B,
        .swcid_val = 0x15,
        .wpk_key_addr = 0x3A8,
        .wpk_key_val = 0x15,
        .wpk_key_h_val = 0x63,
        .tma_key_addr = 0x39F,
        .tma_key_val = 0xEA,
        .tma_key_h_val = 0x9C,
        .pmif_arb = &mt6873_pmif_m_arb,
    }, {
        .slvid = SPMI_SLAVE_7,
        .grpiden = 0x1 << SPMI_GROUP_ID,
        .mstid = SPMI_MASTER_0,
        .hwcid_addr = 0x0009,
        .hwcid_val = 0x15,
        .swcid_addr = 0x000B,
        .swcid_val = 0x15,
        .wpk_key_addr = 0x3A8,
        .wpk_key_val = 0x15,
        .wpk_key_h_val = 0x63,
        .tma_key_addr = 0x39F,
        .tma_key_val = 0xEA,
        .tma_key_h_val = 0x9C,
        .pmif_arb = &mt6873_pmif_m_arb,
    }, {
        .slvid = SPMI_SLAVE_3,
        .grpiden = 0x1 << SPMI_GROUP_ID,
        .mstid = SPMI_MASTER_0,
        .hwcid_addr = 0x0009,
        .hwcid_val = 0x15,
        .swcid_addr = 0x000B,
        .swcid_val = 0x15,
        .wpk_key_addr = 0x3A8,
        .wpk_key_val = 0x15,
        .wpk_key_h_val = 0x63,
        .tma_key_addr = 0x39F,
        .tma_key_val = 0xEA,
        .tma_key_h_val = 0x9C,
        .pmif_arb = &mt6873_pmif_m_arb,
    }
};

static void platform_pmif_spmi_init(uint level)
{
    spmi_device_register(mt6873_spmi_dev, countof(mt6873_spmi_dev));
}

LK_INIT_HOOK(spmi, platform_pmif_spmi_init, LK_INIT_LEVEL_PLATFORM_EARLY - 1);
