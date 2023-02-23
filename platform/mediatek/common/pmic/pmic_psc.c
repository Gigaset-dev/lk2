/*
 * Copyright (c) 2020 MediaTek Inc.
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
#include <platform_mtk.h>
#include <pmic_psc.h>

#define CLR_DELAY       62

static const struct pmic_psc_config *pmic_psc;

static u16 read_pmic_psc_reg(enum pmic_psc_reg_name reg_name)
{
    u16 val = 0;
    const struct pmic_psc_reg *reg = &pmic_psc->regs[reg_name];

    pmic_psc->read_field(reg->reg_addr, &val,
                         reg->reg_mask,
                         reg->reg_shift);
    return val;
}

static void set_pmic_psc_reg(enum pmic_psc_reg_name reg_name)
{
    const struct pmic_psc_reg *reg = &pmic_psc->regs[reg_name];

    pmic_psc->write_field(reg->reg_addr, 1,
                          reg->reg_mask,
                          reg->reg_shift);
}

static void clr_pmic_psc_reg(enum pmic_psc_reg_name reg_name)
{
    const struct pmic_psc_reg *reg = &pmic_psc->regs[reg_name];

    pmic_psc->write_field(reg->reg_addr, 0,
                          reg->reg_mask,
                          reg->reg_shift);
}

int is_pmic_long_press_reset(void)
{
    if (!pmic_psc)
        return ERR_NOT_CONFIGURED;
    return read_pmic_psc_reg(JUST_PWRKEY_RST);
}

int clr_pmic_long_press_reset(void)
{
    if (!pmic_psc)
        return ERR_NOT_CONFIGURED;
    set_pmic_psc_reg(CLR_JUST_RST);
    spin(CLR_DELAY);
    clr_pmic_psc_reg(CLR_JUST_RST);
    return 0;
}

int is_pmic_smart_reset(void)
{
    u16 val;

    if (!pmic_psc)
        return ERR_NOT_CONFIGURED;
    val = read_pmic_psc_reg(JUST_PWRKEY_RST);
    return (!val) & read_pmic_psc_reg(JUST_SMART_RST);
}

int clr_pmic_smart_reset(void)
{
    if (!pmic_psc)
        return ERR_NOT_CONFIGURED;
    set_pmic_psc_reg(RG_CLR_JUST_SMART_RST);
    spin(CLR_DELAY);
    clr_pmic_psc_reg(RG_CLR_JUST_SMART_RST);
    return 0;
}

int enable_pmic_smart_reset(bool enable)
{
    if (!pmic_psc)
        return ERR_NOT_CONFIGURED;

    if (pmic_psc->regs[RG_CPS_W_KEY].reg_addr)
        pmic_psc->write_field(pmic_psc->regs[RG_CPS_W_KEY].reg_addr, 0x4729,
                              pmic_psc->regs[RG_CPS_W_KEY].reg_mask,
                              pmic_psc->regs[RG_CPS_W_KEY].reg_shift);

    if (enable)
        set_pmic_psc_reg(RG_SMART_RST_MODE);
    else
        clr_pmic_psc_reg(RG_SMART_RST_MODE);

    if (pmic_psc->regs[RG_CPS_W_KEY].reg_addr)
        pmic_psc->write_field(pmic_psc->regs[RG_CPS_W_KEY].reg_addr, 0,
                              pmic_psc->regs[RG_CPS_W_KEY].reg_mask,
                              pmic_psc->regs[RG_CPS_W_KEY].reg_shift);
    return 0;
}

int enable_pmic_smart_reset_shutdown(bool enable)
{
    if (!pmic_psc)
        return ERR_NOT_CONFIGURED;

    if (pmic_psc->regs[RG_CPS_W_KEY].reg_addr)
        pmic_psc->write_field(pmic_psc->regs[RG_CPS_W_KEY].reg_addr, 0x4729,
                              pmic_psc->regs[RG_CPS_W_KEY].reg_mask,
                              pmic_psc->regs[RG_CPS_W_KEY].reg_shift);

    if (enable)
        set_pmic_psc_reg(RG_SMART_RST_SDN_EN);
    else
        clr_pmic_psc_reg(RG_SMART_RST_SDN_EN);

    if (pmic_psc->regs[RG_CPS_W_KEY].reg_addr)
        pmic_psc->write_field(pmic_psc->regs[RG_CPS_W_KEY].reg_addr, 0,
                              pmic_psc->regs[RG_CPS_W_KEY].reg_mask,
                              pmic_psc->regs[RG_CPS_W_KEY].reg_shift);
    return 0;
}

int platform_cold_reset(void)
{
    if (!pmic_psc)
        return ERR_NOT_CONFIGURED;
    /* some PMICs may not support cold reset */
    if (!pmic_psc->regs[RG_CRST].reg_addr)
        return ERR_NOT_SUPPORTED;
    set_pmic_psc_reg(RG_CRST);
    return 0;
}

int platform_power_hold(bool hold)
{
    if (!pmic_psc)
        return ERR_NOT_CONFIGURED;
    if (hold)
        set_pmic_psc_reg(RG_PWRHOLD);
    else
        clr_pmic_psc_reg(RG_PWRHOLD);
    return 0;
}

int pmic_get_chip_version(void)
{
    u16 val = 0;

    if (!pmic_psc)
        return ERR_NOT_CONFIGURED;
    pmic_psc->read_field(pmic_psc->regs[SWCID].reg_addr, &val,
                         0xff,
                         pmic_psc->regs[SWCID].reg_shift);

    return val;
}

u8 pmic_get_swreg(void)
{
    u8 val = 0;

    if (!pmic_psc)
        return ERR_NOT_CONFIGURED;

    pmic_psc->read_field(pmic_psc->regs[RG_RSV_SWREG].reg_addr, &val,
                         pmic_psc->regs[RG_RSV_SWREG].reg_mask,
                         pmic_psc->regs[RG_RSV_SWREG].reg_shift);
    return val;
}

int pmic_set_swreg(u8 val)
{
    if (!pmic_psc)
        return ERR_NOT_CONFIGURED;

    pmic_psc->write_field(pmic_psc->regs[RG_RSV_SWREG].reg_addr, val,
                          pmic_psc->regs[RG_RSV_SWREG].reg_mask,
                          pmic_psc->regs[RG_RSV_SWREG].reg_shift);
    return 0;
}

int pmic_psc_register(const struct pmic_psc_config *psc)
{
#ifndef PROJECT_TYPE_FPGA
    if (!psc || !psc->read_field || !psc->write_field)
        return ERR_INVALID_ARGS;
    pmic_psc = psc;
#endif
    return 0;
}
