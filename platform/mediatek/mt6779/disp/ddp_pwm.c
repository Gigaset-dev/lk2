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

#define LOG_TAG "PWM"
#include <string.h>
/* #include <platform/sync_write.h> */
#include <reg.h>
#include "disp_drv_platform.h"

#ifdef LED_SUPPORT
#include <platform/leds_config.h>
#endif
#include <debug.h>
#include <assert.h>
#include "ddp_reg.h"
#include "ddp_pwm.h"
#include "ddp_info.h"
#include "disp_drv_log.h"


extern int ddp_enable_module_clock(DISP_MODULE_ENUM module);

/* DISP_PWM_MUX setting */
#ifndef CLK_CFG_8
#define CLK_CFG_8 (IO_BASE+0xA0)
#endif

#ifndef CLK_CFG_8_SET
#define CLK_CFG_8_SET (IO_BASE+0xA4)
#endif

#ifndef CLK_CFG_8_CLR
#define CLK_CFG_8_CLR (IO_BASE+0xA8)
#endif

#define CLK_CFG_UPDATE1 (IO_BASE+0x08)
#define DISP_PWM_MUX_CK_UPDATE  (1)

#define PWM_MSG(fmt, arg...) DISPMSG("[PWM] " fmt "\n", ##arg)
#define PWM_DBG(fmt, arg...) DISPDBG("[PWM] " fmt "\n", ##arg)

#define PWM_REG_SET(reg, value) do { \
        writel(value, (volatile unsigned int *)(reg)); \
        PWM_DBG("set reg[0x%08x] = 0x%08x", (unsigned int)reg, (unsigned int)value); \
    } while (0)

#define PWM_REG_GET(reg) (*((volatile unsigned int *)reg))

#define PWM_DEFAULT_DIV_VALUE 0x0

#define pwm_get_reg_base(id) ((id) == DISP_PWM0 ? DISPSYS_PWM0_BASE : DISPSYS_PWM0_BASE)

#define index_of_pwm(id) ((id == DISP_PWM0) ? 0 : 1)

static int g_pwm_inited;
static disp_pwm_id_t g_pwm_main_id = DISP_PWM0;


void disp_pwm_clkmux_update(void)
{
    unsigned int regsrc = PWM_REG_GET(CLK_CFG_UPDATE1);

    regsrc = regsrc | (0x1 << DISP_PWM_MUX_CK_UPDATE);
    /* write 1 to update the change of pwm clock source selection */
    PWM_REG_SET(CLK_CFG_UPDATE1, regsrc);
}


void disp_pwm_init(disp_pwm_id_t id)
{
#ifdef LED_SUPPORT
    struct PWM_config *config_data;
#endif
    unsigned int pwm_div;
    unsigned int reg_base = pwm_get_reg_base(id);

    if (g_pwm_inited & id)
        return;

    if (id & DISP_PWM0)
        ddp_enable_module_clock(DISP_MODULE_PWM0);
    if (id & DISP_PWM1)
        ASSERT(0);

    pwm_div = PWM_DEFAULT_DIV_VALUE;
#ifdef LED_SUPPORT
    config_data = get_pwm_config_by_name("lcd-backlight");
    if (config_data != NULL) {
        if (config_data->clock_source >= 0 && config_data->clock_source <= 4) {
            unsigned int reg_val = PWM_REG_GET(CLK_CFG_8);

            PWM_REG_SET(CLK_CFG_8_CLR, 0x7);
            PWM_REG_SET(CLK_CFG_8_SET, (0x7 & (config_data->clock_source << 0)));
            PWM_DBG("%s : CLK_CFG_8 0x%x => 0x%x",
                 __func__, reg_val, PWM_REG_GET(CLK_CFG_8));
        }
        /*
         *After config clock source register, pwm hardware doesn't switch to new setting yet.
         *Api function "disp_pwm_clkmux_update" need to be called for trigger pwm hardware
         *to accept new clock source setting.
         */
        disp_pwm_clkmux_update();
        /* Some backlight chip/PMIC(e.g. MT6332) only accept slower clock */
        pwm_div = (config_data->div == 0) ? PWM_DEFAULT_DIV_VALUE : config_data->div;
        pwm_div &= 0x3FF;
        PWM_MSG("%s : PWM config data (%d,%d)",
             __func__, config_data->clock_source, config_data->div);
    }
#endif

    PWM_REG_SET(reg_base + DISP_PWM_CON_0_OFF, pwm_div << 16);

    PWM_REG_SET(reg_base + DISP_PWM_CON_1_OFF, 1023); /* 1024 levels */

    g_pwm_inited |= id;
}


disp_pwm_id_t disp_pwm_get_main(void)
{
    return g_pwm_main_id;
}


int disp_pwm_is_enabled(disp_pwm_id_t id)
{
    unsigned int reg_base = pwm_get_reg_base(id);
    return (PWM_REG_GET(reg_base + DISP_PWM_EN_OFF) & 0x1);
}


static void disp_pwm_set_enabled(disp_pwm_id_t id, int enabled)
{
    unsigned int reg_base = pwm_get_reg_base(id);
    if (enabled) {
        if (!disp_pwm_is_enabled(id)) {
            PWM_REG_SET(reg_base + DISP_PWM_EN_OFF, 0x1);
            PWM_MSG("disp_pwm_set_enabled: PWN_EN = 0x1");
        }
    } else {
        PWM_REG_SET(reg_base + DISP_PWM_EN_OFF, 0x0);
    }
}


/* For backward compatible */
int disp_bls_set_backlight(int level_256)
{
    int level_1024 = 0;

    if (level_256 <= 0)
        level_1024 = 0;
    else if (level_256 >= 255)
        level_1024 = 1023;
    else {
        level_1024 = (level_256 << 2) + 2;
    }

    return disp_pwm_set_backlight(disp_pwm_get_main(), level_1024);
}


static int disp_pwm_level_remap(disp_pwm_id_t id, int level_1024)
{
    return level_1024;
}


int disp_pwm_set_backlight(disp_pwm_id_t id, int level_1024)
{
    unsigned int reg_base;
    unsigned int offset;

    if ((DISP_PWM_ALL & id) == 0) {
        PWM_MSG("[ERROR] disp_pwm_set_backlight: invalid PWM ID = 0x%x", id);
        return -1;
    }

    disp_pwm_init(id);

    PWM_MSG("disp_pwm_set_backlight(id = 0x%x, level_1024 = %d)", id, level_1024);

    reg_base = pwm_get_reg_base(id);

    level_1024 = disp_pwm_level_remap(id, level_1024);

    PWM_REG_SET(reg_base + DISP_PWM_COMMIT_OFF, 0);
    PWM_REG_SET(reg_base + DISP_PWM_CON_1_OFF, (level_1024 << 16) | 0x3ff);

    if (level_1024 > 0) {
        disp_pwm_set_enabled(id, 1);
    } else {
        disp_pwm_set_enabled(id, 0); /* To save power */
    }

    PWM_REG_SET(reg_base + DISP_PWM_COMMIT_OFF, 1);
    PWM_REG_SET(reg_base + DISP_PWM_COMMIT_OFF, 0);

    for (offset = 0x0; offset <= 0x28; offset += 4) {
        PWM_DBG("reg[0x%08x] = 0x%08x", (reg_base + offset), PWM_REG_GET(reg_base + offset));
    }

    return 0;
}


