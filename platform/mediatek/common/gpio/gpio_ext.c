/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#include <gpio_api.h>
#include <gpio_ext_api.h>
#include <spmi_common.h>
#include <sys/types.h>

#include "gpio_ext.h"
#include "mt6373_gpio.h"

/*---------------------------------------------------------------------------*/
struct mt_gpio_ext_dev *mt_gpio_ext_get_dev(int dev_id)
{
    if (dev_id == MT_GPIO_EXT_MT6373)
        return mt_gpio_get_dev_mt6373();

    return NULL;
}
/*---------------------------------------------------------------------------*/
s32 mt_gpio_ext_set_drv(int dev_id, u32 pin, u32 drv)
{
    const struct gpio_attr_addr *attr_addr;
    struct mt_gpio_ext_dev *gpio_dev;
    s32 ret;

    gpio_dev = mt_gpio_ext_get_dev(dev_id);

    if ((pin < gpio_dev->min_pin_no) || (pin > gpio_dev->max_pin_no))
        return -ERINVAL;

    attr_addr = &gpio_dev->attr_addr[GPIO_ATTR_DRV];

    if (attr_addr->offset[pin - gpio_dev->min_pin_no] == (s8)-1)
        return GPIO_DRV_UNSUPPORTED;

    if (drv > gpio_dev->drv_mask)
        return -ERINVAL;

    ret = spmi_ext_register_writel_field(gpio_dev->dev,
                attr_addr->addr[pin - gpio_dev->min_pin_no],
                (u8)drv, gpio_dev->drv_mask,
                attr_addr->offset[pin - gpio_dev->min_pin_no]);

    return ret;
}
/*---------------------------------------------------------------------------*/
s32 mt_gpio_ext_get_drv(int dev_id, u32 pin)
{
    const struct gpio_attr_addr *attr_addr;
    struct mt_gpio_ext_dev *gpio_dev;
    u8 val = 0;
    s32 ret;

    gpio_dev = mt_gpio_ext_get_dev(dev_id);

    if ((pin < gpio_dev->min_pin_no) || (pin > gpio_dev->max_pin_no))
        return -ERINVAL;

    attr_addr = &gpio_dev->attr_addr[GPIO_ATTR_DRV];

    if (attr_addr->offset[pin - gpio_dev->min_pin_no] == (s8)-1)
        return GPIO_DRV_UNSUPPORTED;

    ret = spmi_ext_register_readl_field(gpio_dev->dev,
                attr_addr->addr[pin - gpio_dev->min_pin_no],
                &val, gpio_dev->drv_mask,
                attr_addr->offset[pin - gpio_dev->min_pin_no]);

    if (ret < 0)
        return ret;

    return (s32)val;
}

/*---------------------------------------------------------------------------*/
s32 mt_gpio_ext_set_dir(int dev_id, u32 pin, u32 dir)
{
    const struct gpio_attr_addr *attr_addr;
    struct mt_gpio_ext_dev *gpio_dev;
    u8 val = (u8)!!dir;
    s32 ret;

    gpio_dev = mt_gpio_ext_get_dev(dev_id);

    if ((pin < gpio_dev->min_pin_no) || (pin > gpio_dev->max_pin_no))
        return -ERINVAL;

    attr_addr = &gpio_dev->attr_addr[GPIO_ATTR_DIR];

    if (attr_addr->offset[pin - gpio_dev->min_pin_no] == (s8)-1)
        return GPIO_DIR_UNSUPPORTED;

    ret = spmi_ext_register_writel_field(gpio_dev->dev,
                attr_addr->addr[pin - gpio_dev->min_pin_no],
                val, 0x1,
                attr_addr->offset[pin - gpio_dev->min_pin_no]);

    return ret;
}
/*---------------------------------------------------------------------------*/
s32 mt_gpio_ext_get_dir(int dev_id, u32 pin)
{
    const struct gpio_attr_addr *attr_addr;
    struct mt_gpio_ext_dev *gpio_dev;
    u8 val = 0;
    s32 ret;

    gpio_dev = mt_gpio_ext_get_dev(dev_id);

    if ((pin < gpio_dev->min_pin_no) || (pin > gpio_dev->max_pin_no))
        return -ERINVAL;

    attr_addr = &gpio_dev->attr_addr[GPIO_ATTR_DIR];

    if (attr_addr->offset[pin - gpio_dev->min_pin_no] == (s8)-1)
        return GPIO_DIR_UNSUPPORTED;

    ret = spmi_ext_register_readl_field(gpio_dev->dev,
                attr_addr->addr[pin - gpio_dev->min_pin_no],
                &val, 0x1,
                attr_addr->offset[pin - gpio_dev->min_pin_no]);

    if (ret < 0)
        return ret;

    return (s32)val;
}
/*---------------------------------------------------------------------------*/
s32 mt_gpio_ext_set_out(int dev_id, u32 pin, u32 output)
{
    const struct gpio_attr_addr *attr_addr;
    struct mt_gpio_ext_dev *gpio_dev;
    u8 val = (u8)!!output;
    s32 ret;

    gpio_dev = mt_gpio_ext_get_dev(dev_id);

    if ((pin < gpio_dev->min_pin_no) || (pin > gpio_dev->max_pin_no))
        return -ERINVAL;

    attr_addr = &gpio_dev->attr_addr[GPIO_ATTR_DO];

    if (attr_addr->offset[pin - gpio_dev->min_pin_no] == (s8)-1)
        return GPIO_OUT_UNSUPPORTED;

    ret = spmi_ext_register_writel_field(gpio_dev->dev,
                attr_addr->addr[pin - gpio_dev->min_pin_no],
                val, 0x1,
                attr_addr->offset[pin - gpio_dev->min_pin_no]);

    return ret;
}
/*---------------------------------------------------------------------------*/
s32 mt_gpio_ext_get_out(int dev_id, u32 pin)
{
    const struct gpio_attr_addr *attr_addr;
    struct mt_gpio_ext_dev *gpio_dev;
    u8 val = 0;
    s32 ret;

    gpio_dev = mt_gpio_ext_get_dev(dev_id);

    if ((pin < gpio_dev->min_pin_no) || (pin > gpio_dev->max_pin_no))
        return -ERINVAL;

    attr_addr = &gpio_dev->attr_addr[GPIO_ATTR_DO];

    if (attr_addr->offset[pin - gpio_dev->min_pin_no] == (s8)-1)
        return GPIO_OUT_UNSUPPORTED;

    ret = spmi_ext_register_readl_field(gpio_dev->dev,
                attr_addr->addr[pin - gpio_dev->min_pin_no],
                &val, 0x1,
                attr_addr->offset[pin - gpio_dev->min_pin_no]);

    if (ret < 0)
        return ret;

    return (s32)val;
}
/*---------------------------------------------------------------------------*/
s32 mt_gpio_ext_get_in(int dev_id, u32 pin)
{
    const struct gpio_attr_addr *attr_addr;
    struct mt_gpio_ext_dev *gpio_dev;
    u8 val = 0;
    s32 ret;

    gpio_dev = mt_gpio_ext_get_dev(dev_id);

    if ((pin < gpio_dev->min_pin_no) || (pin > gpio_dev->max_pin_no))
        return -ERINVAL;

    attr_addr = &gpio_dev->attr_addr[GPIO_ATTR_DI];

    if (attr_addr->offset[pin - gpio_dev->min_pin_no] == (s8)-1)
        return GPIO_IN_UNSUPPORTED;

    ret = spmi_ext_register_readl_field(gpio_dev->dev,
                attr_addr->addr[pin - gpio_dev->min_pin_no],
                &val, 0x1,
                attr_addr->offset[pin - gpio_dev->min_pin_no]);

    if (ret < 0)
        return ret;

    return (s32)val;
}
/*---------------------------------------------------------------------------*/
s32 mt_gpio_ext_set_mode(int dev_id, u32 pin, u32 mode)
{
    const struct gpio_attr_addr *attr_addr;
    struct mt_gpio_ext_dev *gpio_dev;
    s32 ret;

    gpio_dev = mt_gpio_ext_get_dev(dev_id);

    if ((pin < gpio_dev->min_pin_no) || (pin > gpio_dev->max_pin_no))
        return -ERINVAL;

    attr_addr = &gpio_dev->attr_addr[GPIO_ATTR_MODE];

    if (attr_addr->offset[pin - gpio_dev->min_pin_no] == (s8)-1)
        return GPIO_MODE_UNSUPPORTED;

    if (mode > gpio_dev->mode_mask)
        return -ERINVAL;

    ret = spmi_ext_register_writel_field(gpio_dev->dev,
                attr_addr->addr[pin - gpio_dev->min_pin_no],
                (u8)mode, gpio_dev->mode_mask,
                attr_addr->offset[pin - gpio_dev->min_pin_no]);

    return ret;
}
/*---------------------------------------------------------------------------*/
s32 mt_gpio_ext_get_mode(int dev_id, u32 pin)
{
    const struct gpio_attr_addr *attr_addr;
    struct mt_gpio_ext_dev *gpio_dev;
    u8 val = 0;
    s32 ret;

    gpio_dev = mt_gpio_ext_get_dev(dev_id);

    if ((pin < gpio_dev->min_pin_no) || (pin > gpio_dev->max_pin_no))
        return -ERINVAL;

    attr_addr = &gpio_dev->attr_addr[GPIO_ATTR_MODE];

    if (attr_addr->offset[pin - gpio_dev->min_pin_no] == (s8)-1)
        return GPIO_MODE_UNSUPPORTED;

    ret = spmi_ext_register_readl_field(gpio_dev->dev,
                attr_addr->addr[pin - gpio_dev->min_pin_no],
                &val, gpio_dev->mode_mask,
                attr_addr->offset[pin - gpio_dev->min_pin_no]);

    if (ret < 0)
        return ret;

    return (s32)val;
}
/*---------------------------------------------------------------------------*/
s32 mt_gpio_ext_set_smt(int dev_id, u32 pin, u32 enable)
{
    const struct gpio_attr_addr *attr_addr;
    struct mt_gpio_ext_dev *gpio_dev;
    u8 val = (u8)!!enable;
    s32 ret;

    gpio_dev = mt_gpio_ext_get_dev(dev_id);

    if ((pin < gpio_dev->min_pin_no) || (pin > gpio_dev->max_pin_no))
        return -ERINVAL;

    attr_addr = &gpio_dev->attr_addr[GPIO_ATTR_SMT];

    if (attr_addr->offset[pin - gpio_dev->min_pin_no] == (s8)-1)
        return GPIO_SMT_UNSUPPORTED;

    ret = spmi_ext_register_writel_field(gpio_dev->dev,
                attr_addr->addr[pin - gpio_dev->min_pin_no],
                val, 0x1,
                attr_addr->offset[pin - gpio_dev->min_pin_no]);

    return ret;
}
/*---------------------------------------------------------------------------*/
s32 mt_gpio_ext_get_smt(int dev_id, u32 pin)
{
    const struct gpio_attr_addr *attr_addr;
    struct mt_gpio_ext_dev *gpio_dev;
    u8 val = 0;
    s32 ret;

    gpio_dev = mt_gpio_ext_get_dev(dev_id);

    if ((pin < gpio_dev->min_pin_no) || (pin > gpio_dev->max_pin_no))
        return -ERINVAL;

    attr_addr = &gpio_dev->attr_addr[GPIO_ATTR_SMT];

    if (attr_addr->offset[pin - gpio_dev->min_pin_no] == (s8)-1)
        return GPIO_SMT_UNSUPPORTED;

    ret = spmi_ext_register_readl_field(gpio_dev->dev,
                attr_addr->addr[pin - gpio_dev->min_pin_no],
                &val, 0x1,
                attr_addr->offset[pin - gpio_dev->min_pin_no]);

    if (ret < 0)
        return ret;

    return (s32)val;
}
/*---------------------------------------------------------------------------*/
s32 mt_gpio_ext_set_pull_enable(int dev_id, u32 pin, u32 en)
{
    const struct gpio_attr_addr *attr_addr;
    struct mt_gpio_ext_dev *gpio_dev;
    u8 val = (u8)!!en;
    s32 ret;

    gpio_dev = mt_gpio_ext_get_dev(dev_id);

    if ((pin < gpio_dev->min_pin_no) || (pin > gpio_dev->max_pin_no))
        return -ERINVAL;

    attr_addr = &gpio_dev->attr_addr[GPIO_ATTR_PULLEN];

    if (attr_addr->offset[pin - gpio_dev->min_pin_no] == (s8)-1)
        return GPIO_PULL_EN_UNSUPPORTED;

    ret = spmi_ext_register_writel_field(gpio_dev->dev,
                attr_addr->addr[pin - gpio_dev->min_pin_no],
                val, 0x1,
                attr_addr->offset[pin - gpio_dev->min_pin_no]);

    return ret;
}
/*---------------------------------------------------------------------------*/
s32 mt_gpio_ext_get_pull_enable(int dev_id, u32 pin)
{
    const struct gpio_attr_addr *attr_addr;
    struct mt_gpio_ext_dev *gpio_dev;
    u8 val = 0;
    s32 ret;

    gpio_dev = mt_gpio_ext_get_dev(dev_id);

    if ((pin < gpio_dev->min_pin_no) || (pin > gpio_dev->max_pin_no))
        return -ERINVAL;

    attr_addr = &gpio_dev->attr_addr[GPIO_ATTR_PULLEN];

    if (attr_addr->offset[pin - gpio_dev->min_pin_no] == (s8)-1)
        return GPIO_PULL_EN_UNSUPPORTED;

    ret = spmi_ext_register_readl_field(gpio_dev->dev,
                attr_addr->addr[pin - gpio_dev->min_pin_no],
                &val, 0x1,
                attr_addr->offset[pin - gpio_dev->min_pin_no]);

    if (ret < 0)
        return ret;

    return (s32)val;
}
/*---------------------------------------------------------------------------*/
s32 mt_gpio_ext_set_pull_select(int dev_id, u32 pin, u32 sel)
{
    const struct gpio_attr_addr *attr_addr;
    struct mt_gpio_ext_dev *gpio_dev;
    u8 val = (u8)!!sel;
    s32 ret;

    gpio_dev = mt_gpio_ext_get_dev(dev_id);

    if ((pin < gpio_dev->min_pin_no) || (pin > gpio_dev->max_pin_no))
        return -ERINVAL;

    attr_addr = &gpio_dev->attr_addr[GPIO_ATTR_PULLSEL];

    if (attr_addr->offset[pin - gpio_dev->min_pin_no] == (s8)-1)
        return GPIO_PULL_UNSUPPORTED;

    ret = spmi_ext_register_writel_field(gpio_dev->dev,
                attr_addr->addr[pin - gpio_dev->min_pin_no],
                val, 0x1,
                attr_addr->offset[pin - gpio_dev->min_pin_no]);

    return ret;
}
/*---------------------------------------------------------------------------*/
/* get pull-up or pull-down, regardless of resistor value */
s32 mt_gpio_ext_get_pull_select(int dev_id, u32 pin)
{
    const struct gpio_attr_addr *attr_addr;
    struct mt_gpio_ext_dev *gpio_dev;
    u8 val = 0;
    s32 ret;

    gpio_dev = mt_gpio_ext_get_dev(dev_id);

    if ((pin < gpio_dev->min_pin_no) || (pin > gpio_dev->max_pin_no))
        return -ERINVAL;

    attr_addr = &gpio_dev->attr_addr[GPIO_ATTR_PULLSEL];

    if (attr_addr->offset[pin - gpio_dev->min_pin_no] == (s8)-1)
        return GPIO_PULL_UNSUPPORTED;

    ret = spmi_ext_register_readl_field(gpio_dev->dev,
                attr_addr->addr[pin - gpio_dev->min_pin_no],
                &val, 0x1,
                attr_addr->offset[pin - gpio_dev->min_pin_no]);

    if (ret < 0)
        return ret;

    return (s32)val;
}
