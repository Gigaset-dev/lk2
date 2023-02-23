/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the License file or at
 * https://opensource.org/license/MIT
 */

#pragma once

#include <stdint.h>

enum PWR_STA {
    STA_POWER_DOWN = 0,
    STA_POWER_ON = 1,
};

int spm_mtcmos_ctrl_md1(enum PWR_STA state);
int spm_mtcmos_ctrl_dis(enum PWR_STA state);
int spm_mtcmos_ctrl_audio(enum PWR_STA state);
int spm_mtcmos_ctrl_netsys(enum PWR_STA state);
