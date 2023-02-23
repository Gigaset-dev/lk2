/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#pragma once

enum pmic_regu_reg_name {
    RG_LDO_VEFUSE_EN,
    RG_VEFUSE_VOCAL,
    RG_VEFUSE_VOSEL,
    DA_VEFUSE_B_EN,
};

struct pmic_regu_reg {
    u16 reg_addr;
    u8 reg_mask;
    u8 reg_shift;
};

#define PMIC_REGU_REG(_reg_name, addr, shift) \
    [_reg_name] = {         \
        .reg_addr = addr,   \
        .reg_mask = 0x1,    \
        .reg_shift = shift, \
    }

int vefuse_set_vol(unsigned char vosel);
int vefuse_set_cal(unsigned char vocal);
int vefuse_en(unsigned char en);
unsigned char vefuse_is_enabled(void);
