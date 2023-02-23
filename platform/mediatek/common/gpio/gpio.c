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

#include <platform/gpio.h>
#include <platform/reg.h>
#include <debug.h>

#include "gpio_api.h"
#include "gpio_const.h"
#include "gpio_cfg.h"

/******************************************************************************
 * Enumeration/Structure
 ******************************************************************************/
struct mt_gpio_obj {
    struct GPIO_REGS *reg;
};
static struct mt_gpio_obj gpio_dat = {
    .reg  = (struct GPIO_REGS *)(GPIO_BASE),
};

/*---------------------------------------------------------------------------*/
s32 mt_set_gpio_dir(enum GPIO_PIN pin, enum GPIO_DIR dir)
{
    u32 pos;
    u32 bit;
    struct mt_gpio_obj *obj = &gpio_dat;

    if (pin >= MAX_GPIO_PIN)
        return -ERINVAL;

    if (dir >= GPIO_DIR_MAX)
        return -ERINVAL;

    pos = pin / MAX_GPIO_REG_BITS;
    bit = pin % MAX_GPIO_REG_BITS;

    /* To reduce memory usage, we don't use DIR_addr[] array*/
    if (dir == GPIO_DIR_IN)
        GPIO_WR32(&obj->reg->dir[pos].rst, 1L << bit);
    else
        GPIO_WR32(&obj->reg->dir[pos].set, 1L << bit);

    return RSUCCESS;
}
/*---------------------------------------------------------------------------*/
s32 mt_get_gpio_drv(enum GPIO_PIN pin)
{
    u32 reg;
    u32 val, mask;

    if ((pin >= MAX_GPIO_PIN) || (pin <= GPIO_UNSUPPORTED))
        return -ERINVAL;

    if (DRV_offset[pin].offset == (s8)-1)
        return GPIO_DRV_UNSUPPORTED;

    reg = GPIO_RD32(DRV_addr[pin].addr);
    reg = reg >> DRV_offset[pin].offset;
    mask = (1 << DRV_width[pin].width) - 1;
    val = reg & mask;

    return val;
}
/*---------------------------------------------------------------------------*/
s32 mt_get_gpio_dir(enum GPIO_PIN pin)
{
    u32 pos;
    u32 bit;
    u32 reg;
    struct mt_gpio_obj *obj = &gpio_dat;

    if ((pin >= MAX_GPIO_PIN) || (pin <= GPIO_UNSUPPORTED))
        return -ERINVAL;

    pos = pin / MAX_GPIO_REG_BITS;
    bit = pin % MAX_GPIO_REG_BITS;

    /* To reduce memory usage, we don't use DIR_addr[] array*/
    reg = GPIO_RD32(&obj->reg->dir[pos].val);
    return (((reg & (1L << bit)) != 0) ? 1 : 0);
}
/*---------------------------------------------------------------------------*/
s32 mt_set_gpio_drv(enum GPIO_PIN pin, enum GPIO_DRV drv)
{
    u32 reg;
    u32 val, mask;

    if ((pin >= MAX_GPIO_PIN) || (pin <= GPIO_UNSUPPORTED))
        return -ERINVAL;

    if (DRV_offset[pin].offset == (s8)-1)
        return GPIO_DRV_UNSUPPORTED;

    mask = ((1 << DRV_width[pin].width) - 1);
    /* check if set value beyond supported width */
    if ((u32)drv > mask)
        return -ERINVAL;

    reg = GPIO_RD32(DRV_addr[pin].addr);
    val = reg & ~(mask << DRV_offset[pin].offset);
    val |= (drv << DRV_offset[pin].offset);

    GPIO_WR32(DRV_addr[pin].addr, val);

    return RSUCCESS;
}
/*---------------------------------------------------------------------------*/
s32 mt_set_gpio_out(enum GPIO_PIN pin, enum GPIO_OUT output)
{
    u32 pos;
    u32 bit;
    struct mt_gpio_obj *obj = &gpio_dat;

    if ((pin >= MAX_GPIO_PIN) || (pin <= GPIO_UNSUPPORTED))
        return -ERINVAL;

    if (output >= GPIO_OUT_MAX)
        return -ERINVAL;

    pos = pin / MAX_GPIO_REG_BITS;
    bit = pin % MAX_GPIO_REG_BITS;

    /* To reduce memory usage, we don't use DATAOUT_addr[] array*/
    if (output == GPIO_OUT_ZERO)
        GPIO_WR32(&obj->reg->dout[pos].rst, 1L << bit);
    else
        GPIO_WR32(&obj->reg->dout[pos].set, 1L << bit);

    return RSUCCESS;
}
/*---------------------------------------------------------------------------*/
s32 mt_get_gpio_out(enum GPIO_PIN pin)
{
    u32 pos;
    u32 bit;
    u32 reg;
    struct mt_gpio_obj *obj = &gpio_dat;

    if ((pin >= MAX_GPIO_PIN) || (pin <= GPIO_UNSUPPORTED))
        return -ERINVAL;

    pos = pin / MAX_GPIO_REG_BITS;
    bit = pin % MAX_GPIO_REG_BITS;

    /* To reduce memory usage, we don't use DATAOUT_addr[] array*/
    reg = GPIO_RD32(&obj->reg->dout[pos].val);
    return (((reg & (1L << bit)) != 0) ? 1 : 0);
}
/*---------------------------------------------------------------------------*/
s32 mt_get_gpio_in(enum GPIO_PIN pin)
{
    u32 pos;
    u32 bit;
    u32 reg;
    struct mt_gpio_obj *obj = &gpio_dat;

    if ((pin >= MAX_GPIO_PIN) || (pin <= GPIO_UNSUPPORTED))
        return -ERINVAL;

    pos = pin / MAX_GPIO_REG_BITS;
    bit = pin % MAX_GPIO_REG_BITS;

    /* To reduce memory usage, we don't use DIN_addr[] array*/
    reg = GPIO_RD32(&obj->reg->din[pos].val);
    return (((reg & (1L << bit)) != 0) ? 1 : 0);
}
/*---------------------------------------------------------------------------*/
s32 mt_set_gpio_mode(enum GPIO_PIN pin, enum GPIO_MODE mode)
{
    unsigned long pos;
    unsigned long bit;
    unsigned long data;
    struct mt_gpio_obj *obj = &gpio_dat;
    u32 mask;

    if ((pin >= MAX_GPIO_PIN) || (pin <= GPIO_UNSUPPORTED))
        return -ERINVAL;

    if (mode >= GPIO_MODE_MAX)
        return -ERINVAL;

#ifdef SUPPORT_MODE_MWR
    mask = (1L << (GPIO_MODE_BITS - 1)) - 1;
#else
    mask = (1L << GPIO_MODE_BITS) - 1;
#endif

    mode = mode & mask;
    pos = pin / MAX_GPIO_MODE_PER_REG;
    bit = (pin % MAX_GPIO_MODE_PER_REG) * GPIO_MODE_BITS;

#ifdef SUPPORT_MODE_MWR
    data = (1L << (bit + GPIO_MODE_BITS - 1)) | (mode << bit);
    GPIO_WR32(&obj->reg->mode[pos]._align1, data);
#else
    data = GPIO_RD32(&obj->reg->mode[pos].val);
    data &= (~(mask << bit));
    data |= (mode << bit);
    GPIO_WR32(&obj->reg->mode[pos].val, data);
#endif

    return RSUCCESS;
}
/*---------------------------------------------------------------------------*/
s32 mt_get_gpio_mode(enum GPIO_PIN pin)
{
    unsigned long pos;
    unsigned long bit;
    unsigned long data;
    u32 mask;
    struct mt_gpio_obj *obj = &gpio_dat;

    if ((pin >= MAX_GPIO_PIN) || (pin <= GPIO_UNSUPPORTED))
        return -ERINVAL;

#ifdef SUPPORT_MODE_MWR
    mask = (1L << (GPIO_MODE_BITS - 1)) - 1;
#else
    mask = (1L << GPIO_MODE_BITS) - 1;
#endif

    pos = pin / MAX_GPIO_MODE_PER_REG;
    bit = (pin % MAX_GPIO_MODE_PER_REG) * GPIO_MODE_BITS;

    data = GPIO_RD32(&obj->reg->mode[pos].val);
    return (data >> bit) & mask;
}
/*---------------------------------------------------------------------------*/
s32 mt_set_gpio_smt(enum GPIO_PIN pin, enum GPIO_SMT enable)
{
    if ((pin >= MAX_GPIO_PIN) || (pin <= GPIO_UNSUPPORTED))
        return -ERINVAL;

    if (SMT_offset[pin].offset == (s8)-1)
        return GPIO_SMT_UNSUPPORTED;

    if (enable == GPIO_SMT_DISABLE)
        GPIO_CLR_BITS(SMT_addr[pin].addr, 1L << SMT_offset[pin].offset);
    else
        GPIO_SET_BITS(SMT_addr[pin].addr, 1L << SMT_offset[pin].offset);

    return RSUCCESS;
}
/*---------------------------------------------------------------------------*/
s32 mt_get_gpio_smt(enum GPIO_PIN pin)
{
    u32 reg;

    if ((pin >= MAX_GPIO_PIN) || (pin <= GPIO_UNSUPPORTED))
        return -ERINVAL;

    if (SMT_offset[pin].offset == (s8)-1)
        return GPIO_SMT_UNSUPPORTED;

    reg = GPIO_RD32(SMT_addr[pin].addr);
    return ((reg & (1L << SMT_offset[pin].offset)) ? 1 : 0);
}
/*---------------------------------------------------------------------------*/
s32 mt_set_gpio_ies(enum GPIO_PIN pin, enum GPIO_IES enable)
{
    if ((pin >= MAX_GPIO_PIN) || (pin <= GPIO_UNSUPPORTED))
        return -ERINVAL;

    if (IES_offset[pin].offset == (s8)-1)
        return GPIO_IES_UNSUPPORTED;

    if (enable == GPIO_IES_DISABLE)
        GPIO_CLR_BITS(IES_addr[pin].addr, 1L << IES_offset[pin].offset);
    else
        GPIO_SET_BITS(IES_addr[pin].addr, 1L << IES_offset[pin].offset);

    return RSUCCESS;
}
/*---------------------------------------------------------------------------*/
s32 mt_get_gpio_ies(enum GPIO_PIN pin)
{
    u32 reg;

    if ((pin >= MAX_GPIO_PIN) || (pin <= GPIO_UNSUPPORTED))
        return -ERINVAL;

    if (IES_offset[pin].offset == (s8)-1)
        return GPIO_IES_UNSUPPORTED;

    reg = GPIO_RD32(IES_addr[pin].addr);
    return ((reg & (1L << IES_offset[pin].offset)) ? 1 : 0);
}
/*---------------------------------------------------------------------------*/
s32 mt_set_gpio_pull_enable(enum GPIO_PIN pin, enum GPIO_PULL_EN en)
{
    if ((pin >= MAX_GPIO_PIN) || (pin <= GPIO_UNSUPPORTED))
        return -ERINVAL;

#if defined(HAS_PUPD)
    if ((PU_offset[pin].offset == (s8)-1) && (PUPD_offset[pin].offset == (s8)-1))
        return GPIO_PULL_EN_UNSUPPORTED;

    if (en == GPIO_PULL_DISABLE) {
        if (PU_offset[pin].offset == (s8)-1) {
            GPIO_CLR_BITS(R0_addr[pin].addr, 1L << R0_offset[pin].offset);
            GPIO_CLR_BITS(R1_addr[pin].addr, 1L << R1_offset[pin].offset);
        } else {
            GPIO_CLR_BITS(PU_addr[pin].addr, 1L << PU_offset[pin].offset);
            GPIO_CLR_BITS(PD_addr[pin].addr, 1L << PD_offset[pin].offset);
        }
    } else if (en == GPIO_PULL_ENABLE) {
        if (PU_offset[pin].offset == (s8)-1) {
            /* For PUPD+R0+R1 Type, mt_set_gpio_pull_enable does not know
             * which one between PU and PD shall be enabled.
             * Use R0 to guarantee at one resistor is set when lk
             * apply default setting
             */
            GPIO_SET_BITS(R0_addr[pin].addr, 1L << R0_offset[pin].offset);
            GPIO_CLR_BITS(R1_addr[pin].addr, 1L << R1_offset[pin].offset);
        } else {
            /* For PU + PD Ttype, mt_set_gpio_pull_enable does not know
             * which one between PU and PD shall be enabled.
             * Use mt_set_gpio_pull_select() or GPIO_PULL_ENABLE_Rx instead.
             */
             return GPIO_PULL_EN_UNSUPPORTED;
        }
    } else if (en == GPIO_PULL_ENABLE_R0) {
        if (PUPD_offset[pin].offset == (s8)-1)
            return GPIO_PULL_EN_UNSUPPORTED;
        /* IOConfig does not support MWR operation, so use CLR + SET */
        GPIO_SET_BITS(R0_addr[pin].addr, 1L << R0_offset[pin].offset);
        GPIO_CLR_BITS(R1_addr[pin].addr, 1L << R1_offset[pin].offset);
    } else if (en == GPIO_PULL_ENABLE_R1) {
        if (PUPD_offset[pin].offset == (s8)-1)
            return GPIO_PULL_EN_UNSUPPORTED;
        /* IOConfig does not support MWR operation, so use CLR + SET */
        GPIO_CLR_BITS(R0_addr[pin].addr, 1L << R0_offset[pin].offset);
        GPIO_SET_BITS(R1_addr[pin].addr, 1L << R1_offset[pin].offset);
    } else if (en == GPIO_PULL_ENABLE_R0R1) {
        if (PUPD_offset[pin].offset == (s8)-1)
            return GPIO_PULL_EN_UNSUPPORTED;
        GPIO_SET_BITS(R0_addr[pin].addr, 1L << R0_offset[pin].offset);
        GPIO_SET_BITS(R1_addr[pin].addr, 1L << R1_offset[pin].offset);
    } else {
        return -ERINVAL;
    }

    return RSUCCESS;
#elif defined(HAS_PULLSEL_PULLEN)
    int reg;

    if (-1 != PULLEN_offset[pin].offset) {
        reg = GPIO_RD32(PULLEN_addr[pin].addr);
        if (en == GPIO_PULL_DISABLE)
            reg &= (~(1 << (PULLEN_offset[pin].offset)));
        else
            reg |= (1 << (PULLEN_offset[pin].offset));
        GPIO_WR32(PULLEN_addr[pin].addr, reg);
    }
    if (-1 != PUPD_offset[pin].offset) {
        reg = GPIO_RD32(PUPD_addr[pin].addr);
        if (en == GPIO_PULL_DISABLE) {
            reg &= (~(1 << (R1_offset[pin].offset)));
            reg &= (~(1 << (R0_offset[pin].offset)));
        } else {
            reg &= (~(1 << (R1_offset[pin].offset)));
            reg |= (1 << (R0_offset[pin].offset));
        }
        GPIO_WR32(PUPD_addr[pin].addr, reg);
    }
    return RSUCCESS;
#else
    return -ERINVAL;
#endif
}
/*---------------------------------------------------------------------------*/
s32 mt_get_gpio_pull_enable(enum GPIO_PIN pin)
{
    if ((pin >= MAX_GPIO_PIN) || (pin <= GPIO_UNSUPPORTED))
        return -ERINVAL;

#if defined(HAS_PUPD)
    u32 reg;
    u32 reg2;

    if ((PU_offset[pin].offset == (s8)-1) && (PUPD_offset[pin].offset == (s8)-1))
        return GPIO_PULL_EN_UNSUPPORTED;

    if (PU_offset[pin].offset == (s8)-1) {
        reg = (GPIO_RD32(R0_addr[pin].addr) >> R0_offset[pin].offset) & 1;
        reg2 = (GPIO_RD32(R1_addr[pin].addr) >> R1_offset[pin].offset) & 1;
        return ((reg | reg2) ? 1 : 0);
    } else if (PUPD_offset[pin].offset == (s8)-1) {
        reg = (GPIO_RD32(PU_addr[pin].addr) >> PU_offset[pin].offset) & 1;
        reg2 = (GPIO_RD32(PD_addr[pin].addr) >> PD_offset[pin].offset) & 1;
        return ((reg | reg2) ? 1 : 0);
    }
#elif defined(HAS_PULLSEL_PULLEN)
    int res;

    res = mt_get_gpio_pull_select(pin);

    if (res == 2)
        return 0;/*disable */
    if (1 == res || 0 == res)
        return 1;/*enable */
    if (-1 == res)
        return -1;
    return -ERWRAPPER;
#endif

    return -ERINVAL;
}
/*---------------------------------------------------------------------------*/
s32 mt_set_gpio_pull_select(enum GPIO_PIN pin, enum GPIO_PULL sel)
{
    if ((pin >= MAX_GPIO_PIN) || (pin <= GPIO_UNSUPPORTED))
        return -ERINVAL;

#if defined(HAS_PUPD)
    if ((PU_offset[pin].offset == (s8)-1) && (PUPD_offset[pin].offset == (s8)-1))
        return GPIO_PULL_UNSUPPORTED;

    if (sel == GPIO_NO_PULL) {
        mt_set_gpio_pull_enable(pin, GPIO_PULL_DISABLE);
        if (PU_offset[pin].offset == (s8)-1)
            GPIO_SET_BITS(PUPD_addr[pin].addr, 1L << PUPD_offset[pin].offset);
        /* *ELSE* do nothting since already modified in mt_set_gpio_pull_enable_chip */
    } else if (sel == GPIO_PULL_UP) {
        if (PU_offset[pin].offset == (s8)-1)
            GPIO_CLR_BITS(PUPD_addr[pin].addr, 1L << PUPD_offset[pin].offset);
        else {
            GPIO_SET_BITS(PU_addr[pin].addr, 1L << PU_offset[pin].offset);
            GPIO_CLR_BITS(PD_addr[pin].addr, 1L << PD_offset[pin].offset);
        }
    } else if (sel == GPIO_PULL_DOWN) {
        if (PU_offset[pin].offset == (s8)-1)
            GPIO_SET_BITS(PUPD_addr[pin].addr, 1L << PUPD_offset[pin].offset);
        else {
            GPIO_CLR_BITS(PU_addr[pin].addr, 1L << PU_offset[pin].offset);
            GPIO_SET_BITS(PD_addr[pin].addr, 1L << PD_offset[pin].offset);
        }
    } else {
        return -ERINVAL;
    }

    return RSUCCESS;
#elif defined(HAS_PULLSEL_PULLEN)
    int reg;

    if (-1 != PULLSEL_offset[pin].offset) {
        reg = GPIO_RD32(PULLSEL_addr[pin].addr);
        if (sel == GPIO_PULL_UP)
            reg |= (1 << (PULLSEL_offset[pin].offset));
        else if (sel == GPIO_PULL_DOWN)
            reg &= (~(1 << (PULLSEL_offset[pin].offset)));
        GPIO_WR32(PULLSEL_addr[pin].addr, reg);
    }
    if (-1 != PUPD_offset[pin].offset) {
        reg = GPIO_RD32(PUPD_addr[pin].addr);
        reg &= (~(1 << (R1_offset[pin].offset)));
        reg |= (1 << (R0_offset[pin].offset));
        if (sel == GPIO_PULL_UP)
            reg &= (~(1 << (PUPD_offset[pin].offset)));
        else if (sel == GPIO_PULL_DOWN)
            reg |= (1 << (PUPD_offset[pin].offset));
        GPIO_WR32(PUPD_addr[pin].addr, reg);
    }
    return RSUCCESS;
#else
    return -ERINVAL;
#endif
}
/*---------------------------------------------------------------------------*/
/* get pull-up or pull-down, regardless of resistor value */
s32 mt_get_gpio_pull_select(enum GPIO_PIN pin)
{
    u32 data;
    u32 data1;
    u32 r0;
    u32 r1;
    u32 pupd;

    if ((pin >= MAX_GPIO_PIN) || (pin <= GPIO_UNSUPPORTED))
        return -ERINVAL;

#if defined(HAS_PUPD)
    if ((PU_offset[pin].offset == (s8)-1) && (PUPD_offset[pin].offset == (s8)-1))
        return GPIO_PULL_UNSUPPORTED;

    if (PU_offset[pin].offset == (s8)-1) {
        data = GPIO_RD32(PUPD_addr[pin].addr);
        pupd = data & (1L << (PUPD_offset[pin].offset));
        r0 = data & (1L << (R0_offset[pin].offset));
        r1 = data & (1L << (R1_offset[pin].offset));
        if (r0 == 0 && r1 == 0)
            return GPIO_NO_PULL;/*High Z(no pull) */
            else if (pupd == 0)
                return GPIO_PULL_UP;/* pull up */
            else if (pupd == 1)
                return GPIO_PULL_DOWN;/* pull down */
    } else if (PUPD_offset[pin].offset == (s8)-1) {
        data = (GPIO_RD32(PU_addr[pin].addr) >> PU_offset[pin].offset) & 1;
        data1 = (GPIO_RD32(PD_addr[pin].addr) >> PD_offset[pin].offset) & 1;
        if (data)
            return GPIO_PULL_UP;
        else if (data1)
            return GPIO_PULL_DOWN;
        else
            return GPIO_NO_PULL;
    }
#elif defined(HAS_PULLSEL_PULLEN)
    if (-1 != PUPD_offset[pin].offset) {
        data = GPIO_RD32(PUPD_addr[pin].addr);
        pupd = data & (1L << (PUPD_offset[pin].offset));
        r0 = data & (1L << (R0_offset[pin].offset));
        r1 = data & (1L << (R1_offset[pin].offset));
        if (r0 == 0 && r1 == 0)
            return GPIO_NO_PULL;/*High Z(no pull) */
            else if (pupd == 0)
                return GPIO_PULL_UP;/* pull up */
            else if (pupd == 1)
                return GPIO_PULL_DOWN;/* pull down */
    }

    if (-1 != PULLEN_offset[pin].offset) {
        data = GPIO_RD32(PULLEN_addr[pin].addr);
        data1 = GPIO_RD32(PULLSEL_addr[pin].addr);
        if (0 == (data & (1L << (PULLEN_offset[pin].offset))))
            return GPIO_NO_PULL;
        else if (0 == (data1 & (1L << (PULLSEL_offset[pin].offset))))
            return GPIO_PULL_DOWN;
        else
            return GPIO_PULL_UP;
    }
#endif

    return -ERINVAL;
}
