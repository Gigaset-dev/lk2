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
#include <pmic_regu.h>
#include <stdlib.h>
#include <string.h>
#include <trace.h>

#define LOCAL_TRACE 0
#define EFUSE_PMIC_CONTROL_ENABLE   1


#define udelay(x)   spin(x)
#define mdelay(x)   udelay((x) * 1000)

/**************************************************************
 * Module name porting
 **************************************************************/
int get_devinfo_model_name(char *ptr, size_t ptr_size)
{
    int ret;
    unsigned int segment = get_devinfo_with_index(7) & 0xFF;
    int model_index = 0;
    const char * const model_name[] = {
        "MT6890(ENG)"
    };

    switch (segment) {
    case 0:
        model_index = 0;
        break;
    default:
        dprintf(CRITICAL, "segment not support\n");
        return 1;
    }

    unsigned int proj_code = get_devinfo_with_index(29) & 0x3FFF;

    dprintf(CRITICAL, "proj_code: 0x%x\n", proj_code);
    dprintf(CRITICAL, "[LK] segment = 0x%x\n", segment);

    ret = snprintf(ptr, ptr_size, "%s", model_name[model_index]);
    if (ret < 0)
        dprintf(CRITICAL, "Fail to copy model name(%d)\n", ret);

    return 0;
}

/****************************************************
 * Fsource
 * return 0 : success
 ****************************************************/
unsigned int efuse_fsource_set(void)
{
#if EFUSE_PMIC_CONTROL_ENABLE

    /* 1.8V */
    vefuse_set_vol(0x4);

    /* +0mV */
    vefuse_set_cal(0);

    /* Fsource(VEFUSE) enabled */
    vefuse_en(1);

    mdelay(10);

#endif
    return 0;
}

unsigned int efuse_fsource_close(void)
{

#if EFUSE_PMIC_CONTROL_ENABLE

    /* Fsource(VEFUSE) disable */
    vefuse_en(0);

    mdelay(10);

#endif
    return 0;
}

unsigned int efuse_fsource_is_enabled(void)
{
    unsigned int regVal = 0;

#if EFUSE_PMIC_CONTROL_ENABLE

    /*  Check Fsource(VEFUSE) Status */
    regVal = vefuse_is_enabled();

    /* return 1 : fsource enabled, return 0 : fsource disabled */
#endif

    return regVal;
}
