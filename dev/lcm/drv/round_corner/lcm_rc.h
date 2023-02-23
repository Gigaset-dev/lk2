/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#pragma once

#include <debug.h>
#include <lcm_common_drv.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include "../lcm_settings.h"
#include "../lcm_log.h"

struct rc_pattern {
    unsigned int size;
    void *addr;
};

struct mtk_lcm_rc_pattern {
    char name[MTK_LCM_NAME_LENGTH];
    struct rc_pattern left_top;
    struct rc_pattern left_top_left;
    struct rc_pattern left_top_right;
    struct rc_pattern right_top;
    struct rc_pattern left_bottom;
    struct rc_pattern right_bottom;
};

enum mtk_lcm_rc_locate {
    RC_LEFT_TOP = 0,
    RC_LEFT_TOP_LEFT,
    RC_LEFT_TOP_RIGHT,
    RC_RIGHT_TOP,
    RC_LEFT_BOTTOM,
    RC_RIGHT_BOTTOM,
};


/* function: get round corner pattern address
 * input:
 *   index: pattern id parsing from panel dts settings
 *   locate: pattern location
 *   size: rc pattern data size
 * output: round corner pattern address
 */
const void *mtk_lcm_get_rc_addr(char *name, enum mtk_lcm_rc_locate locate,
        unsigned int *size);
