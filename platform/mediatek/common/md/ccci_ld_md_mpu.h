/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#pragma once

enum md_mpu_region {
    MD_MCU_RO_HW_RO = 0,
    MD_MCU_RW_HW_RW,
    MD_MCU_RW_HW_RO,
    MD_MCU_RO_HW_RW,
    MD_DSP_RO,
    MD_DSP_RW,
    MD_DRDI,
    MD_TRACE_TOP,
    MD_CONSYS,
    MD_C_SMEM,
    MD_NC_SMEM,
    MD_SEC_SMEM,
};

int ccci_update_md_mpu_cfg_tbl(void *blk_tbl[], unsigned int num);
