/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#pragma once

#include <platform/addressmap.h>

/* Read/write byte limitation, by platform */
#define PMIF_BYTECNT_MAX    2

/* indicate which number SW channel start, by project */
#define PMIF_SWINF_0_CHAN_NO    8
/* MD: 0, security domain: 1, AP: 2 */
#define PMIF_AP_SWINF_NO    2

#define PMIF_MPU_DT_NODE_NAME "/spmi_pmif_mpu@1c015900"
#define PMIF_MPU_DT_PROP_NAME "mediatek,pmic_all_rgn_en_lk"

enum {
    PMIF_SPI,
    PMIF_SPMI,
};

struct mt6983_pmif_regs {
    u32 init_done;       /* 0x000 */
    u32 reserved0_0[8];
    u32 inf_en;          /* 0x024 */
    u32 reserved0_1[74];
    u32 arb_en;          /* 0x150 */
    u32 reserved0_2[153];
    u32 cmdissue_en;     /* 0x3B8 */
    u32 reserved0_3[305];
    u32 swinf_acc;       /* 0x880 */
    u32 swinf_wdata;     /* 0x884 */
    u32 reserved1[3];
    u32 swinf_rdata;     /* 0x894 */
    u32 reserved2[3];
    u32 swinf_vld_clr;   /* 0x8A4 */
    u32 swinf_sta;       /* 0x8A8 */
    u32 reserved3[21];
    u32 mpu_ctrl;        /* 0x900 */
    u32 reserved4[43];
    u32 pmic_all_rgn_en; /* 0x9b0 */
};

static struct mt6983_pmif_regs * const mtk_pmif = (void *)PMIF_SPMI_BASE;
