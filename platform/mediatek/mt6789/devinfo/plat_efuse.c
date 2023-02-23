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
#include <pmic_wrap_common.h>
#include <stdlib.h>
#include <string.h>
#include <trace.h>

#define udelay(x)   spin(x)
#define mdelay(x)   udelay((x) * 1000)

#define EFUSE_PMIC_CONTROL_ENABLE   1

/*******************************************
 * PMIC register definition
 *******************************************/
#define MT6366_VEFUSE_ANA_CON0          (0x1e98)
#define MT6366_LDO_VEFUSE_CON0          (0x1c44)

#define PMIC_RG_VEFUSE_VOCAL_ADDR       MT6366_VEFUSE_ANA_CON0
#define PMIC_RG_VEFUSE_VOCAL_MASK       0xF
#define PMIC_RG_VEFUSE_VOCAL_SHIFT      0

#define PMIC_RG_VEFUSE_VOSEL_ADDR       MT6366_VEFUSE_ANA_CON0
#define PMIC_RG_VEFUSE_VOSEL_MASK       0xF
#define PMIC_RG_VEFUSE_VOSEL_SHIFT      8


#define PMIC_RG_LDO_VEFUSE_EN_ADDR      MT6366_LDO_VEFUSE_CON0
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
    int ret;
    unsigned int segment = get_devinfo_with_index(30) & 0xFF;
    unsigned int platform = (get_devinfo_with_index(3) >> 30) & 0x1; /* 0: smartphone, 1: tablet */
    int model_index = 0;
    static const char * const model_name[] = {
        "MT6789(ENG)",  //0
        "MT6789V/CD",   //1
        "",             //2,empty string
    };
    static const char * const tb_model_name[] = {
        "MT8781V/CA",   //0
        "MT8781V/NA",   //1
        "",             //2,empty string
    };
    const char * const *p_model_name = model_name;

    switch (segment) {
    case 0x0:
        model_index = 0;
        break;
    case 0x1:
        model_index = 1;
        break;
    default:
        model_index = 2;
        break;
    }

    if (segment == 0 && platform) {
        unsigned int md1_dis = get_devinfo_with_index(6) & 0x1;

        if (!md1_dis)
                model_index = 0;
        else
                model_index = 1;
        p_model_name = tb_model_name;
    }

    dprintf(ALWAYS, "proj_code: 0x%x\n", get_devinfo_with_index(29) & 0x3FFF);
    dprintf(ALWAYS, "[LK] segment = 0x%x\n", segment);

    ret = snprintf(ptr, ptr_size, "%s", p_model_name[model_index]);
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
    unsigned int ret_val = 0;

#if EFUSE_PMIC_CONTROL_ENABLE
    /* 1.8V */
    pwrap_write_field((unsigned int)(PMIC_RG_VEFUSE_VOSEL_ADDR),
            (unsigned int)(0xC),
            (unsigned int)(PMIC_RG_VEFUSE_VOSEL_MASK),
            (unsigned int)(PMIC_RG_VEFUSE_VOSEL_SHIFT)
            );

    /* +0mV */
    pwrap_write_field((unsigned int)(PMIC_RG_VEFUSE_VOCAL_ADDR),
            (unsigned int)(0x0),
            (unsigned int)(PMIC_RG_VEFUSE_VOCAL_MASK),
            (unsigned int)(PMIC_RG_VEFUSE_VOCAL_SHIFT)
            );

    /* Fsource(VEFUSE) enabled */
    pwrap_write_field((unsigned int)(PMIC_RG_LDO_VEFUSE_EN_ADDR),
            (unsigned int)(0x1),
            (unsigned int)(PMIC_RG_LDO_VEFUSE_EN_MASK),
            (unsigned int)(PMIC_RG_LDO_VEFUSE_EN_SHIFT)
            );

    mdelay(10);

#endif
    return ret_val;
}

unsigned int efuse_fsource_close(void)
{
    unsigned int ret_val = 0;

#if EFUSE_PMIC_CONTROL_ENABLE
    /* Fsource(VEFUSE) disable */
    pwrap_write_field((unsigned int)(PMIC_RG_LDO_VEFUSE_EN_ADDR),
            (unsigned int)(0x0),
            (unsigned int)(PMIC_RG_LDO_VEFUSE_EN_MASK),
            (unsigned int)(PMIC_RG_LDO_VEFUSE_EN_SHIFT)
            );

    mdelay(10);

#endif
    return ret_val;
}

unsigned int efuse_fsource_is_enabled(void)
{
    unsigned int regVal = 0;

#if EFUSE_PMIC_CONTROL_ENABLE
    /*  Check Fsource(VEFUSE) Status */
    regVal = pwrap_read_field((unsigned int)(PMIC_RG_LDO_VEFUSE_EN_ADDR),
            (unsigned int)(PMIC_RG_LDO_VEFUSE_EN_MASK),
            (unsigned int)(PMIC_RG_LDO_VEFUSE_EN_SHIFT)
            );

    /* return 1 : fsource enabled  */
    /* return 0 : fsource disabled */

#endif
    return regVal == 1;
}
