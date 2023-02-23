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

#include <errno.h>
#include <string.h>
#include <trace.h>

#include "mtk_charger_intf.h"

#define LOCAL_TRACE 0

int __WEAK mtk_charger_init(void)
{
    LTRACEF("%s: not ready\n", __func__);
    return 0;
}

int __WEAK mtk_charger_dump_register(void)
{
    LTRACEF("%s: not ready\n", __func__);
    return 0;
}

int __WEAK mtk_charger_enable_charging(bool enable)
{
    LTRACEF("%s: not ready\n", __func__);
    return 0;
}

int __WEAK mtk_charger_get_ichg(unsigned int *ichg)
{
    LTRACEF("%s: not ready\n", __func__);
    return 0;
}

int __WEAK mtk_charger_set_ichg(unsigned int ichg)
{
    LTRACEF("%s: not ready\n", __func__);
    return 0;
}

int __WEAK mtk_charger_get_aicr(unsigned int *aicr)
{
    LTRACEF("%s: not ready\n", __func__);
    return 0;
}

int __WEAK mtk_charger_set_aicr(unsigned int aicr)
{
    LTRACEF("%s: not ready\n", __func__);
    return 0;
}

int __WEAK mtk_charger_get_adc(int chan, unsigned int *data)
{
    LTRACEF("%s: not ready\n", __func__);
    return 0;
}

int __WEAK mtk_charger_get_vbus(unsigned int *vbus)
{
    LTRACEF("%s: not ready\n", __func__);
    return 0;
}

int __WEAK mtk_charger_set_mivr(unsigned int mivr)
{
    LTRACEF("%s: not ready\n", __func__);
    return 0;
}

int __WEAK mtk_charger_enable_power_path(bool enable)
{
    LTRACEF("%s: not ready\n", __func__);
    return 0;
}

int __WEAK mtk_charger_reset_pumpx(bool reset)
{
    LTRACEF("%s: not ready\n", __func__);
    return 0;
}

int __WEAK mtk_charger_enable_wdt(bool en)
{
    LTRACEF("%s: not ready\n", __func__);
    return 0;
}

int __WEAK mtk_charger_reset_wdt(void)
{
    LTRACEF("%s: not ready\n", __func__);
    return 0;
}

int __WEAK mtk_charger_sw_reset(void)
{
    LTRACEF("%s: not ready\n", __func__);
    return 0;
}

int __WEAK mtk_charger_check_charging_mode(void)
{
    LTRACEF("%s: not ready\n", __func__);
    return 0;
}

int __WEAK mtk_charger_check_battery_plugout_once(bool *plugout_once)
{
    LTRACEF("%s: not ready\n", __func__);
    return 0;
}
