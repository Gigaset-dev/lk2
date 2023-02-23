/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */
#pragma once


/*****************************************************************************
 *
 *****************************************************************************/
int picachu_check_bat_low_voltage(void);
int picachu_voltage_calibration_via_doe(void);
int picachu_check_doe(char *doe, const char *str);

int is_aging_flavor_load(void);
int is_slt_load(void);

void lut_update_vf_table(
    const unsigned int cluster_id,
    const unsigned int point_id,
    const unsigned int freq,
    const unsigned int volt);
