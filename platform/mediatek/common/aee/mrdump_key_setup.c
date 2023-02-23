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

#include <debug.h>
#include <errno.h>
#ifdef WITH_PLATFORM_MEDIATEK_COMMON_GPIO
#include <gpio_api.h>
#endif
#include <libfdt.h>
#include <lk/init.h>
#include <platform/mrdump.h>
#include <platform/wdt.h>
#ifdef WITH_PLATFORM_MEDIATEK_COMMON_PMIC
#include <pmic_psc.h>
#endif
#include <set_fdt.h>
#include <stdlib.h>
#include <stdio.h>
#include <trace.h>

#define LOCAL_TRACE 1
#define ARRAY_SIZE(arr) (countof(arr))

static void mrdump_key_select_eint(int mrdump_key_pin)
{
#ifdef WITH_PLATFORM_MEDIATEK_COMMON_GPIO
    mt_set_gpio_pull_enable(mrdump_key_pin, GPIO_PULL_UP);
    mt_set_gpio_dir(mrdump_key_pin, GPIO_DIR_IN);
    mt_set_gpio_mode(mrdump_key_pin, GPIO_MODE_00);
#endif
}

static int select_rgp_pin(int eint_pin)
{
#ifdef MRDUMP_KEY_RGU_REMAP_ENABLE
    int i;

    for (i = 0; i < ARRAY_SIZE(gpio_rgu_remap_table); i++) {
        if (eint_pin == gpio_rgu_remap_table[i].gpio_num) {
            LTRACEF_LEVEL(CRITICAL, "get rgu_pin(%d) for eint_pin(%d)\n",
                                        gpio_rgu_remap_table[i].rgu_num, eint_pin);
            return gpio_rgu_remap_table[i].rgu_num;
        }
    }
    LTRACEF_LEVEL(CRITICAL, "can't get rgu_pin for eint_pin(%d)\n", eint_pin);
    return -1;
#else
    return eint_pin;
#endif
}

static void mrdump_key_apply(char *force_trigger,  char *rst_mode,
        int mrdump_key_pin)
{
    int rgu_pin;

    if (!rst_mode || !force_trigger) {
        LTRACEF_LEVEL(CRITICAL, "no rst_mode or force_trigger\n");
        return;
    }

    if (!strncmp("SYSRST", force_trigger, strlen("SYSRST"))) {
        if (!strncmp(rst_mode, "IRQ", strlen("IRQ"))) {
            mtk_wdt_config_sysrst(REQ_EN, REQ_IRQ_MODE);
#ifdef WITH_PLATFORM_MEDIATEK_COMMON_PMIC
            enable_pmic_smart_reset(1);
            enable_pmic_smart_reset_shutdown(1);
#endif
        } else if (!strncmp(rst_mode, "RST", strlen("RST"))) {
            mtk_wdt_config_sysrst(REQ_EN, REQ_RST_MODE);
#ifdef WITH_PLATFORM_MEDIATEK_COMMON_PMIC
            enable_pmic_smart_reset(1);
            enable_pmic_smart_reset_shutdown(1);
#endif
        } else
            LTRACEF_LEVEL(CRITICAL, "bad rst_mode=%s\n", rst_mode);
    } else if (!strncmp("EINT", force_trigger, strlen("EINT"))) {

        /* initialize gpio setting */
        mrdump_key_select_eint(mrdump_key_pin);
        /* select correct rgu pin */
        rgu_pin = select_rgp_pin(mrdump_key_pin);
        if (rgu_pin >= 0) {
            if (!strncmp(rst_mode, "IRQ", strlen("IRQ")))
                mtk_wdt_config_eint(REQ_EN, REQ_IRQ_MODE, rgu_pin);
            else if (!strncmp(rst_mode, "RST", strlen("RST")))
                mtk_wdt_config_eint(REQ_EN, REQ_RST_MODE, rgu_pin);
        }
    } else
        LTRACEF_LEVEL(CRITICAL, "no valid trigger info\n");
}

static void mrdump_key_fdt_init(void *fdt)
{
    const char mrdump_key_compatible[] = "mediatek, mrdump_ext_rst-eint";
    int offset, len;
    const void *data;
    const char *force_trigger;
    const char *rst_mode;
    int *interrupts = NULL;
    int mrdump_key_pin;
#if LK_DEBUGLEVEL > 1
    force_trigger = "SYSRST";
#else
    force_trigger = "EINT";
#endif
    rst_mode = "IRQ";

    if (!fdt) {
        LTRACEF_LEVEL(CRITICAL, "fdt is NULL\n");
        return;
    }

    offset = fdt_node_offset_by_compatible(fdt, -1, mrdump_key_compatible);
    if (offset < 0) {
        LTRACEF_LEVEL(CRITICAL, "%s not found\n", mrdump_key_compatible);
        return;
    }

    data = fdt_getprop(fdt, offset, "force_mode", &len);
    if (data) {
        LTRACEF_LEVEL(CRITICAL, "force_mode = %s\n", (char *)data);
        force_trigger = (const char *)data;
    } else
        LTRACEF_LEVEL(CRITICAL, "no force_mode attribute default=%s\n", force_trigger);

    data = fdt_getprop(fdt, offset, "mode", &len);
    if (data) {
        LTRACEF_LEVEL(CRITICAL, "mode = %s\n", (char *)data);
        rst_mode = (const char *)data;
    } else
        LTRACEF_LEVEL(CRITICAL, "no mode attribute default=%s\n", rst_mode);

    data = fdt_getprop(fdt, offset, "interrupts", &len);
    if (data) {
        LTRACEF_LEVEL(CRITICAL, "interrupts = 0x%x\n", *((int *)data));
        interrupts = (int *)data;
        mrdump_key_pin = (*interrupts/0x1000000);
        LTRACEF_LEVEL(CRITICAL, " mrdump_key_pin = EINT%d\n", mrdump_key_pin);
    } else
        LTRACEF_LEVEL(CRITICAL, "no interrupts attribute\n");

    mrdump_key_apply((char *)force_trigger, (char *)rst_mode, mrdump_key_pin);
}
SET_FDT_INIT_HOOK(mrdump_key_fdt_init, mrdump_key_fdt_init);
