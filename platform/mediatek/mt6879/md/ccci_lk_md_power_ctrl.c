/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#include <chip_id.h>
#include <debug.h>
#include <mtcmos.h>
#include <smc.h>
#include <smc_id_table.h>
#include <platform/addressmap.h>
#include <platform/reg.h>
#include <platform/sec_devinfo.h>

#include "ccci_ld_md_log.h"
#include "ccci_ld_md_sec.h"

#define LOCAL_TRACE 0

#define POWERON_CONFIG_EN       (SPM_BASE + 0x0000)
#define SPM_POWER_ON_VAL1       (SPM_BASE + 0x0008)
#define SPM_404_WAIT_REG        (SPM_BASE + 0x0404)


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

static int ccci_md_pll_sec(ulong stage)
{
    struct __smccc_res res;

    ALWAYS_LOG("Trigger MD pll setting\n");
    __smc_conduit(MTK_SIP_BL_CCCI_CONTROL, MD_POWER_CONFIG, MD_LK_BOOT_PLAT,
        stage, 0, 0, 0, 0, &res);
    ALWAYS_LOG("set_stage=0x%lx, status1=0x%lx, status2=0x%lx\n", stage, res.a0, res.a1);

    if (res.a0)
        return -1;
    return 0;
}

static int delay_and_wait(int wait_us, int set_val, int wait_val)
{
#define WAITE_UNIT   (10)
    int wait_counter = 1000000/WAITE_UNIT; // 1s

    int ret = 0;
    unsigned int val = 0;

    spin(wait_us);
    write32(SPM_404_WAIT_REG, set_val);

    ALWAYS_LOG("[POWER ON] polling after %dus wait\n", wait_us);

    do {
        if ((read32(SPM_404_WAIT_REG) & 0x10) == wait_val)
            break;
        spin(WAITE_UNIT);
        wait_counter--;
    } while (wait_counter);

    ALWAYS_LOG("[POWER ON] polling end after %dus wait, 0x%x, 0x%x\n",
        wait_us, wait_counter, read32(SPM_404_WAIT_REG));

    return 0;
}

static void set_pll_wa_flow(void)
{
    unsigned long stage = 0;
    int ret;
#define WAITE_UNIT   (10)
    int wait_time = 1000000/WAITE_UNIT;

    for (stage = 0; stage < 3; stage++) {
        ret = ccci_md_pll_sec(stage);
        switch (stage) {
        case 0:
            delay_and_wait(90, 1, 0x10);
            break;
        case 1:
            delay_and_wait(60, 0, 0x00);
            delay_and_wait(250, 1, 0x10);
            break;
        case 2:
            delay_and_wait(500, 0, 0x00);
            break;
        }
    }

}

void ccci_md_power_on(void)
{
    ALWAYS_LOG("[Power On]Enter MD power on\n");
    /* 1. pmic in pre-loader by pmic */
    /* 2. power on MD MTCMOS by spm */
    spm_mtcmos_ctrl_md1_enable();
    ALWAYS_LOG("[Power On]MD MTCMOS power on done!\n");

    set_pll_wa_flow();
    ALWAYS_LOG("[Power On]MD set PLL done!\n");
}

void ccci_test_md_power_on(void)
{
    ALWAYS_LOG("[Power On]Enter test md power on mode\n");
    /* 0. for MD RF */
    write32(POWERON_CONFIG_EN, 0x0B160001);
    setbits32(SPM_POWER_ON_VAL1, (0x1 << 14));
    /* 1. pmic in pre-loader by pmic */
    /* 2. power on MD MTCMOS by spm */
    spm_mtcmos_ctrl_md1_enable();
    ALWAYS_LOG("[Power On]MD MTCMOS power on done!\n");
}
