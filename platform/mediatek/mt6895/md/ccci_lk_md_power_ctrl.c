/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#include <debug.h>
#include <mtcmos.h>
#include <platform/addressmap.h>
#include <platform/reg.h>
#include <platform/sec_devinfo.h>

#include "ccci_ld_md_log.h"

#define LOCAL_TRACE 0

/* only for disable modem project */

#define ccci_write32(a, v)    writel(v, a)
#define ccci_read32(a)        readl(a)

static int md1_revert_sequencer_setting(void)
{
    int count = 0;

    ALWAYS_LOG("[POWER OFF] %s start\n", __func__);

    /* disable sequencer */
    ccci_write32(AOC_BASE + 0x204, 0x0);
    ALWAYS_LOG("[POWER OFF] disable sequencer done\n");

    /* retry 1000 * 1ms = 1s*/
    while (1) {
    /* wait sequencer done */
        if (ccci_read32(AOC_BASE + 0x310) == 0x1010001) {
            ALWAYS_LOG("[POWER OFF] wait sequencer done\n");
            break;
        }
        count++;
        spin(1000);
        if (count == 1000) {
            ALWAYS_LOG("[POWER OFF] wait seq fail, 0x200=0x%x,0x204=0x%x,0x208=0x%x,0x310=0x%x\n",
                ccci_read32(AOC_BASE + 0x200), ccci_read32(AOC_BASE + 0x204),
                ccci_read32(AOC_BASE + 0x208), ccci_read32(AOC_BASE + 0x310));
            return -2;
        }
    }

    /* revert mux of sequencer to AOC1.0 */
    ccci_write32(AOC_BASE + 0x208, 0x5000D);

    ALWAYS_LOG("[POWER OFF] %s end\n", __func__);

    return 0;
}

static int md1_enable_sequencer_setting(void)
{
    int count = 0;

    ALWAYS_LOG("[POWER OFF] %s start\n", __func__);

    ccci_write32(AOC_BASE + 0x280, 0x0);
    /* enable sequencer */
    ccci_write32(AOC_BASE + 0x204, 0x1);
    ALWAYS_LOG("[POWER OFF] %s enable sequencer done\n", __func__);

    /* retry 1000 * 1ms = 1s*/
    while (1) {
        /* wait sequencer done */
        if (ccci_read32(AOC_BASE + 0x310) == 0x4040040) {
            ALWAYS_LOG("[POWER OFF] wait sequencer done\n");
            break;
        }
        count++;
        spin(1000);
        if (count == 1000) {
            ALWAYS_LOG("[POWER OFF] wait seq fail, 0x200=0x%x,0x204=0x%x,0x208=0x%x,0x310=0x%x\n",
                ccci_read32(AOC_BASE + 0x200), ccci_read32(AOC_BASE + 0x204),
                ccci_read32(AOC_BASE + 0x208), ccci_read32(AOC_BASE + 0x310));
            return -2;
        }
    }

    ALWAYS_LOG("[POWER OFF] %s end\n", __func__);

    return 0;
}

static int md_cd_topclkgen_off(void)
{
    unsigned int reg_value;

    ALWAYS_LOG("[POWER OFF]%s start\n", __func__);

    reg_value = ccci_read32(TOPCLK_BASE);
    reg_value |= ((1<<8) | (1<<9));

    ccci_write32(TOPCLK_BASE, reg_value);

    ALWAYS_LOG("[POWER OFF]%s end: set md1_clk_mod = 0x%x\n",
        __func__, ccci_read32(TOPCLK_BASE));

    return 0;
}

static void internal_md_power_down(void)
{
    /* step1.revert sequencer setting to AOC2.5 */
    md1_revert_sequencer_setting();

    ALWAYS_LOG("[Power Off] Start, call spm_mtcmos_ctrl_md1_disable\n");
    /* step2.power off md mtcmos */
    spm_mtcmos_ctrl_md1_disable();
    ALWAYS_LOG("[Power Off] spm_mtcmos_ctrl_md1_disable done\n");

    /* step3.enable sequencer */
    md1_enable_sequencer_setting();

    /* step4.disable MD input 26M&32K */
    md_cd_topclkgen_off();
}

int ccci_get_md_dev_en(void)
{
    unsigned int val;

    val = get_devinfo_with_index(6);
    ALWAYS_LOG("[Power On]Dev en:%d\n", val);

    if ((val & (0x1 << 0)) == 0)
        return 1;
    return 0;
}

void ccci_md_power_off(void)
{
    if (ccci_get_md_dev_en())
        internal_md_power_down();
    else
        ALWAYS_LOG("MD effused, no need power off\n");
}

void ccci_md_power_on(void)
{
    ALWAYS_LOG("[Power On]Enter MD power on\n");
    /* 1. pmic in pre-loader by pmic */
    /* 2. power on MD MTCMOS by spm */
    spm_mtcmos_ctrl_md1_enable();
    ALWAYS_LOG("[Power On]MD MTCMOS power on done!\n");
}

void ccci_test_md_power_on(void)
{
    ALWAYS_LOG("[Power On]Enter test md power on mode\n");
    /* 1. pmic in pre-loader by pmic */
    /* 2. power on MD MTCMOS by spm */
    spm_mtcmos_ctrl_md1_enable();
    ALWAYS_LOG("[Power On]MD MTCMOS power on done!\n");
}
