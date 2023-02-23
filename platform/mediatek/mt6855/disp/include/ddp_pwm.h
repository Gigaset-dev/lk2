/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#pragma once

enum disp_pwm_id_t {
    DISP_PWM0 = 0x1,
    DISP_PWM1 = 0x2,
    DISP_PWM_ALL = (DISP_PWM0 | DISP_PWM1)
};


enum disp_pwm_id_t disp_pwm_get_main(void);

void disp_pwm_init(enum disp_pwm_id_t id);

int disp_pwm_is_enabled(enum disp_pwm_id_t id);

int disp_pwm_set_backlight(enum disp_pwm_id_t id, int level_1024);

/* For backward compatible */
int disp_bls_set_backlight(int level_256);
