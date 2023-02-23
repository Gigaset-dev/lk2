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

#include <debug.h>
#include <err.h>
#include <lk/init.h>
#include <pmic_keys.h>
#include <pmic_wrap_common.h>

#include "registers.h"

static const struct pmic_key_reg mt6359_key_regs[] = {
    PMIC_KEY_REG(PWRKEY_DEB, MT6359_TOPSTATUS, 1),
    PMIC_KEY_REG(HOMEKEY_DEB, MT6359_TOPSTATUS, 3),
    PMIC_KEY_REG(RG_INT_STATUS_PWRKEY, MT6359_PSC_TOP_INT_STATUS0, 0),
    PMIC_KEY_REG(RG_INT_STATUS_HOMEKEY, MT6359_PSC_TOP_INT_STATUS0, 1),
};

static int mt6359_key_read_field(u16 reg, u16 *val, u16 mask, u16 shift)
{
    *val = pwrap_read_field(reg, mask, shift);

    return 0;
}

static const struct pmic_key_config mt6359_key_cfg = {
    .read_field = mt6359_key_read_field,
    .write_field = pwrap_write_field,
    .regs = mt6359_key_regs,
};

static void mt6359_keys_init(uint level)
{
    pmic_key_register(&mt6359_key_cfg);
}

LK_INIT_HOOK(mt6359_keys, mt6359_keys_init, LK_INIT_LEVEL_PLATFORM_EARLY);
