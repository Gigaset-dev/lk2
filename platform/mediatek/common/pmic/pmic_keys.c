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
#include <pmic_keys.h>

static const struct pmic_key_config *pmic_key;

static u16 read_pmic_key_reg(enum pmic_key_reg_name reg_name)
{
    u16 val = 0;
    const struct pmic_key_reg *reg = &pmic_key->regs[reg_name];

    pmic_key->read_field(reg->reg_addr, &val,
                         reg->reg_mask,
                         reg->reg_shift);
    return val;
}

static void set_pmic_key_reg(enum pmic_key_reg_name reg_name)
{
    const struct pmic_key_reg *reg = &pmic_key->regs[reg_name];

    pmic_key->write_field(reg->reg_addr, 1,
                          reg->reg_mask,
                          reg->reg_shift);
}

static void clr_pmic_key_reg(enum pmic_key_reg_name reg_name)
{
    const struct pmic_key_reg *reg = &pmic_key->regs[reg_name];

    pmic_key->write_field(reg->reg_addr, 0,
                          reg->reg_mask,
                          reg->reg_shift);
}

int is_pmic_key_pressed(enum pmic_key key)
{
    u16 val;

    if (!pmic_key)
        return ERR_NOT_CONFIGURED;
    switch (key) {
    case POWER_KEY:
        val = read_pmic_key_reg(PWRKEY_DEB);
        break;
    case HOME_KEY:
        val = read_pmic_key_reg(HOMEKEY_DEB);
        break;
    case HOME_KEY2:
        val = read_pmic_key_reg(HOMEKEY2_DEB);
        break;
    default:
        return ERR_INVALID_ARGS;
    }
    return val ? 0 : 1;
}

int get_pmic_key_event_status(enum pmic_key key)
{
    u16 val;

    if (!pmic_key)
        return ERR_NOT_CONFIGURED;
    switch (key) {
    case POWER_KEY:
        val = read_pmic_key_reg(RG_INT_STATUS_PWRKEY);
        break;
    case HOME_KEY:
        val = read_pmic_key_reg(RG_INT_STATUS_HOMEKEY);
        break;
    case HOME_KEY2:
        val = read_pmic_key_reg(RG_INT_STATUS_HOMEKEY_2);
        break;
    default:
        return ERR_INVALID_ARGS;
    }
    return val;
}

int clr_pmic_key_event_status(enum pmic_key key)
{
    if (!pmic_key)
        return ERR_NOT_CONFIGURED;
    switch (key) {
    case POWER_KEY:
        set_pmic_key_reg(RG_INT_STATUS_PWRKEY);
        break;
    case HOME_KEY:
        set_pmic_key_reg(RG_INT_STATUS_HOMEKEY);
        break;
    case HOME_KEY2:
        set_pmic_key_reg(RG_INT_STATUS_HOMEKEY_2);
        break;
    default:
        return ERR_INVALID_ARGS;
    }
    return 0;
}

int pmic_key_register(const struct pmic_key_config *key_cfg)
{
#ifndef PROJECT_TYPE_FPGA
    if (!key_cfg || !key_cfg->read_field || !key_cfg->write_field)
        return ERR_INVALID_ARGS;
    pmic_key = key_cfg;
#endif
    return 0;
}
