/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */
/*****************************************************************************
 *
 *****************************************************************************/

#include <debug.h>
#include <err.h>
#include <stdlib.h>
#include <string.h>


/*****************************************************************************
 *
 *****************************************************************************/
char picachu_partition_name[] = "pi_img";
int LOW_BAT_VOLT = 3700;


/*****************************************************************************
 *
 *****************************************************************************/
int __WEAK picachu_check_bat_low_voltage(void)
{
    return 0;
}

const char *dconfig_str = "";
char __WEAK *picachu_dconfig_getenv(char *name)
{
    return (char *)dconfig_str;
}

int __WEAK picachu_check_doe(const char *doe, const char *str)
{
    return 0;
}

int __WEAK picachu_get_doe_v_offset(char *doe_str)
{
    return 0;
}

int __WEAK picachu_voltage_calibration_via_doe(void)
{
    return 0;
}

/*****************************************************************************
 *
 *****************************************************************************/

int __WEAK is_aging_flavor_load(void)
{
    return 0;
}

int __WEAK is_slt_load(void)
{
    return 0;
}

/*****************************************************************************
 *
 *****************************************************************************/

void start_picachu(void);
status_t platform_load_picachu(void)
{
    /* Call picachu main code, which will then load pi_img.img */
    start_picachu();

    return NO_ERROR;
}


