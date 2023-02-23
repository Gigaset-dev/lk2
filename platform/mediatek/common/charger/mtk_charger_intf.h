/*
 * Copyright (c) 2019 MediaTek Inc.
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files
 * (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge,
 * publish, distribute, sublicense, and/or sell copies of the Software,
 * and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#pragma once

#include <compiler.h>
#include <debug.h>
#include <stdbool.h>
#include <string.h>

int mtk_charger_init(void);
int mtk_charger_dump_register(void);
int mtk_charger_enable_charging(bool enable);
int mtk_charger_get_vbus(unsigned int *vbus);
int mtk_charger_get_ichg(unsigned int *ichg);
int mtk_charger_set_ichg(unsigned int ichg);
int mtk_charger_get_aicr(unsigned int *aicr);
int mtk_charger_set_aicr(unsigned int aicr);
int mtk_charger_set_mivr(unsigned int mivr);
int mtk_charger_enable_power_path(bool enable);
int mtk_charger_reset_pumpx(bool reset);
int mtk_charger_reset_wdt(void);
int mtk_charger_sw_reset(void);
int mtk_charger_enable_wdt(bool en);
int mtk_charger_check_charging_mode(void);
int mtk_charger_check_battery_plugout_once(bool *plugout_once);
