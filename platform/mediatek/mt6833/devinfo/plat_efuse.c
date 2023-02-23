/*
 * Copyright (c) 2022 MediaTek Inc.
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
#define MT6368_VEFUSE_ANA_CON0          (0x1e0c)
#define MT6368_VEFUSE_ANA_CON1          (0x1e0d)
#define MT6368_LDO_VEFUSE_CON0          (0x1d07)

#define PMIC_RG_VEFUSE_VOCAL_ADDR       MT6368_VEFUSE_ANA_CON0
#define PMIC_RG_VEFUSE_VOCAL_MASK       0xF
#define PMIC_RG_VEFUSE_VOCAL_SHIFT      0

#define PMIC_RG_VEFUSE_VOSEL_ADDR       MT6368_VEFUSE_ANA_CON1
#define PMIC_RG_VEFUSE_VOSEL_MASK       0xF
#define PMIC_RG_VEFUSE_VOSEL_SHIFT      0


#define PMIC_RG_LDO_VEFUSE_EN_ADDR      MT6368_LDO_VEFUSE_CON0
#define PMIC_RG_LDO_VEFUSE_EN_MASK      0x1
#define PMIC_RG_LDO_VEFUSE_EN_SHIFT     0

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
    int model_index = 0;

    static const char * const model_name[] = {
        "MT6833(ENG)",
        "MT6833V/ZA",
        "MT6833V/NZA",
        "MT6833V/TZA",
        "MT6833V/TNZA",
        "MT6833V/MZA",
        "MT6833V/MNZA",
        "MT6833V/PNZA",

        "MT8771V/PZA",
        "MT8771V/NZA",
        "",             /* empty string */
    };

    switch (segment) {
    case 0x0:
        model_index = 0;//ENG
        break;
    case 0x1:
        model_index = 1;//ZA
        break;
    case 0x2:
        model_index = 2;//NZA
        break;
    case 0x10:
        model_index = 3;//TZA
        break;
    case 0x20:
        model_index = 4;//TNZA
        break;
    case 0x03:
        model_index = 5;//MZA
        break;
    case 0x04:
        model_index = 6;//MNZA
        break;
    case 0x06:
        model_index = 7;//PNZA
        break;
    default:
        model_index = 10;
        break;
    }

    dprintf(ALWAYS, "proj_code: 0x%x\n", get_devinfo_with_index(29) & 0x3FFF);
    dprintf(ALWAYS, "[LK] segment = 0x%x\n", segment);

    ret = snprintf(ptr, ptr_size, "%s", model_name[model_index]);
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
