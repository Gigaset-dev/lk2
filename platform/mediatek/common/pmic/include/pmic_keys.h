/*
 * Copyright (c) 2019 MediaTek Inc.
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files
 * (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge,
 * publish, distribute, sublicense, and/or sell copies of the Software,
 * and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#pragma once

enum pmic_key {
    POWER_KEY,
    HOME_KEY,
    HOME_KEY2,
    PMIC_KEY_MAX,
};

enum pmic_key_reg_name {
    PWRKEY_DEB,
    HOMEKEY_DEB,
    HOMEKEY2_DEB,
    RG_INT_STATUS_PWRKEY,
    RG_INT_STATUS_HOMEKEY,
    RG_INT_STATUS_HOMEKEY_2,
};

struct pmic_key_reg {
    u16 reg_addr;
    u16 reg_mask;
    u16 reg_shift;
};

struct pmic_key_config {
    int (*read_field)(u16 reg, u16 *val, u16 mask, u16 shift);
    int (*write_field)(u16 reg, u16 val, u16 mask, u16 shift);
    const struct pmic_key_reg *regs;
};

#define PMIC_KEY_REG(_reg_name, addr, shift) \
    [_reg_name] = {         \
        .reg_addr = addr,   \
        .reg_mask = 0x1,    \
        .reg_shift = shift, \
    }

int is_pmic_key_pressed(enum pmic_key key);
int get_pmic_key_event_status(enum pmic_key key);
int clr_pmic_key_event_status(enum pmic_key key);
int pmic_key_register(const struct pmic_key_config *key_cfg);
