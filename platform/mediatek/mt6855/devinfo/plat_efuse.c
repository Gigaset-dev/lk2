/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#include <debug.h>
#include <errno.h>
#include <platform/sec_devinfo.h>
#include <platform/wdt.h>
#include <spmi_common.h>
#include <stdlib.h>
#include <string.h>
#include <trace.h>

#define udelay(x)   spin(x)
#define mdelay(x)   udelay((x) * 1000)

static struct spmi_device *spmi_sdev;

/*******************************************
 * PMIC register definition
 *******************************************/

#define MTxxxx_VEFUSE_ANA_CON0          (0x1e0c)
#define MTxxxx_VEFUSE_ANA_CON1          (0x1e0d)
#define MTxxxx_LDO_VEFUSE_CON0          (0x1c87)

#define PMIC_RG_VEFUSE_VOCAL_ADDR       MTxxxx_VEFUSE_ANA_CON0
#define PMIC_RG_VEFUSE_VOCAL_MASK       0xF
#define PMIC_RG_VEFUSE_VOCAL_SHIFT      0

#define PMIC_RG_VEFUSE_VOSEL_ADDR       MTxxxx_VEFUSE_ANA_CON1
#define PMIC_RG_VEFUSE_VOSEL_MASK       0xF
#define PMIC_RG_VEFUSE_VOSEL_SHIFT      0


#define PMIC_RG_LDO_VEFUSE_EN_ADDR      MTxxxx_LDO_VEFUSE_CON0
#define PMIC_RG_LDO_VEFUSE_EN_MASK      0x1
#define PMIC_RG_LDO_VEFUSE_EN_SHIFT     0

/*
 * SEL table
 *
 * 4'b0000 :1.2V
 * 4'b0001 :1.3V
 * 4'b0010 :1.5V
 * 4'b0011 :1.7V
 * 4'b0100 :1.8V
 * 4'b0101 :2.0V
 * 4'b0110 :2.1V
 * 4'b0111 :2.2V
 * 4'b1000 :2.7V
 * 4'b1001 :2.8V
 * 4'b1010 :2.9V
 * 4'b1011 :3.0V
 * 4'b1100 :3.1V
 * 4'b1101 :3.3V
 * 4'b1110 :3.4V
 * 4'b1111 :3.5V
 *
 * CAL table
 *
 * 4'b0000: +00mV
 * 4'b0001: +10mV
 * 4'b0010: +20mV
 * 4'b0011: +30mV
 * 4'b0100: +40mV
 * 4'b0101: +50mV
 * 4'b0110: +60mV
 * 4'b0111: +70mV
 * 4'b1000: +80mV
 * 4'b1001: +90mV
 * 4'b1010: +100mV
 */

/**************************************************************
 * devinfo API
 **************************************************************/

unsigned int get_devinfo_with_index(unsigned int index)
{
    return internal_get_devinfo_with_index(index);
}

/**************************************************************
 * Module name porting
 **************************************************************/
int get_devinfo_model_name(char *ptr, size_t ptr_size)
{
    /* 0: smartphone, 1: tablet */
    int ret;
    unsigned int segment = get_devinfo_with_index(30) & 0xFF;
    static const char * const model_name[] = {
        "MT6855_ENG",   //0
        "MT6855V/AZA",  //1
        "",             //2, empty
    };

    if (segment > 1)
        segment = 0;

    dprintf(ALWAYS, "proj_code: 0x%x\n", get_devinfo_with_index(29) & 0x3FFF);
    dprintf(ALWAYS, "[LK] segment = 0x%x\n", segment);

    ret = snprintf(ptr, ptr_size, "%s", model_name[segment]);
    if (ret < 0)
        dprintf(ALWAYS, "Fail to copy model name(%d)\n", ret);

    return 0;
}

/**************************************************************
 * WDT
 **************************************************************/
void efuse_wdt_restart(void)
{
    mtk_wdt_restart_timer();
}

/****************************************************
 * Fsource
 * return 0 : success
 ****************************************************/
unsigned int efuse_fsource_set(void)
{
    int ret = 0;

    spmi_sdev = get_spmi_device(SPMI_MASTER_1, SPMI_SLAVE_5);
    if (!spmi_sdev) {
        dprintf(ALWAYS, "%s: get spmi device fail\n", __func__);
        return 1;
    }

    /* 1.8V */
    ret = spmi_ext_register_writel_field(spmi_sdev,
            (unsigned int)(PMIC_RG_VEFUSE_VOSEL_ADDR),
            (unsigned int)(0x4),
            (unsigned int)(PMIC_RG_VEFUSE_VOSEL_MASK),
            (unsigned int)(PMIC_RG_VEFUSE_VOSEL_SHIFT));
    if (ret < 0) {
        dprintf(ALWAYS, "SPMI write fail at (%s), addr: %d, ret: %d\n",
                 __func__, PMIC_RG_VEFUSE_VOSEL_ADDR, ret);
        return 1;
    }

    /* +0mV */
    ret = spmi_ext_register_writel_field(spmi_sdev,
            (unsigned int)(PMIC_RG_VEFUSE_VOCAL_ADDR),
            (unsigned int)(0x0),
            (unsigned int)(PMIC_RG_VEFUSE_VOCAL_MASK),
            (unsigned int)(PMIC_RG_VEFUSE_VOCAL_SHIFT));
    if (ret < 0) {
        dprintf(ALWAYS, "SPMI write fail at (%s), addr: %d, ret: %d\n",
                 __func__, PMIC_RG_VEFUSE_VOCAL_ADDR, ret);
        return 1;
    }

    /* Fsource(VEFUSE) enabled */
    ret = spmi_ext_register_writel_field(spmi_sdev,
            (unsigned int)(PMIC_RG_LDO_VEFUSE_EN_ADDR),
            (unsigned int)(0x1),
            (unsigned int)(PMIC_RG_LDO_VEFUSE_EN_MASK),
            (unsigned int)(PMIC_RG_LDO_VEFUSE_EN_SHIFT));
    if (ret < 0) {
        dprintf(ALWAYS, "SPMI write fail at (%s), addr: %d, ret: %d\n",
                 __func__, PMIC_RG_LDO_VEFUSE_EN_ADDR, ret);
        return 1;
    }

    mdelay(10);

    return 0;
}

unsigned int efuse_fsource_close(void)
{
    /* Fsource(VEFUSE) disable */
    int ret = spmi_ext_register_writel_field(spmi_sdev,
            (unsigned int)(PMIC_RG_LDO_VEFUSE_EN_ADDR),
            (unsigned int)(0x0),
            (unsigned int)(PMIC_RG_LDO_VEFUSE_EN_MASK),
            (unsigned int)(PMIC_RG_LDO_VEFUSE_EN_SHIFT));
    if (ret < 0) {
        dprintf(ALWAYS, "SPMI write fail at (%s), addr: %d, ret: %d\n",
                 __func__, PMIC_RG_LDO_VEFUSE_EN_ADDR, ret);
        return 1;
    }

    mdelay(10);
    return 0;
}

unsigned int efuse_fsource_is_enabled(void)
{
    u8 regVal = 0;

    /*  Check Fsource(VEFUSE) Status */
    int ret = spmi_ext_register_readl_field(spmi_sdev,
            (unsigned int)(PMIC_RG_LDO_VEFUSE_EN_ADDR),
            &regVal,
            (unsigned int)(PMIC_RG_LDO_VEFUSE_EN_MASK),
            (unsigned int)(PMIC_RG_LDO_VEFUSE_EN_SHIFT));
    if (ret < 0) {
        dprintf(ALWAYS, "SPMI read fail at (%s), addr: %d, ret: %d\n",
                __func__, PMIC_RG_LDO_VEFUSE_EN_ADDR, ret);
    }

    /* return 1 : fsource enabled  */
    /* return 0 : fsource disabled */

    return regVal == 1;
}
