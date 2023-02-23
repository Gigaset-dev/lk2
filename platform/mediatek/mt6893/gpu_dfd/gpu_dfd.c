/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#include <debug.h>
#include <gpu_dfd.h>
#include <kernel/vm.h>
#include <mtcmos.h>
#include <mblock.h>
#include <platform/addressmap.h>
#include <platform/dbgtop.h>
#include <platform/mboot_expdb.h>
#include <reg.h>
#include <stdio.h>
#include <string.h>
#include <trace.h>

#define GPU_BRING_UP 1
#define LOCAL_TRACE  0

#define udelay(x)                    spin(x)

/*
 * IO_BASE     : 0x1000_0000
 * GPU_DFD_BASE: 0x13E0_0000 (0x13E0_0000 ~ 0x13EF_FFFF)
 */

#define GPU_DFD_BASE                 (IO_BASE + 0x03E00000)
#define GPU_DFD_DATA_SIZE            0x100000

/*
 * INFRACFG_AO_BASE: 0x1000_1000
 * MFG_MISC_CON    : 0x1000_1600
 */
#define INFRACFG_AO_BASE             (IO_BASE + 0x00001000)
#define MFG_MISC_CON                 (INFRACFG_AO_BASE + 0x600)
#define MFG_DFD_COMPLETE             (1 << 18)
#define MFG_DFD_TRIGGERED            (1 << 19)

/*
 * MFGCFG_BASE  : 0x13FB_F000
 * MFG_DFD_CON_0: 0x13FB_FA00
 */
#define MFGCFG_BASE                  (IO_BASE + 0x03fbf000)
#define MFG_DFD_CON_0                (MFGCFG_BASE + 0xA00)

#define MFG_DFD_DUMP_SETTING         (0x0F000011)

/*
 * TOPRGU_BASE : 0x1000_7000
 * WDT_SWSYSRST: 0x1000_7018
 */
#define TOPRGU_BASE                  (IO_BASE + 0x00007000)
#define WDT_SWSYSRST                 (TOPRGU_BASE + 0x018)
#define MFG_RST                      (1 << 2)

#define MFG_RST_SETTING              (0x88000000 + MFG_RST)
#define MFG_RST_RELEASE_SETTING      (0x88000000)

/*
 * DRM_DEBUG_BASE: 0x1000_D000
 * DRM_MFG_REG   : 0x1000_D060
 */
#define DRM_DEBUG_BASE               (IO_BASE + 0x0000D000)
#define DRM_MFG_REG                  (DRM_DEBUG_BASE + 0x60)

#define MFG_PWR_OFF_DISABLE_SETTING  (0x77000000)
#define MFG_PWR_ON_ENABLE_SETTING    (0x77000003)

/*
 * MTK_SLEEP_BASE: 0x1000_6000
 * PWR_STATUS:     0x1000_616c
 * PWR_STATUS_2ND: 0x1000_6170
 * MFG0_PWR_CON:   0x1000_6308
 * MFG1_PWR_CON:   0x1000_630C
 */
#define MTK_SLEEP_BASE               (IO_BASE + 0x00006000)
#define PWR_STATUS                   (MTK_SLEEP_BASE + 0x16C)
#define PWR_STATUS_2ND               (MTK_SLEEP_BASE + 0x170)
#define MFG0_PWR_CON                 (MTK_SLEEP_BASE + 0x308)
#define MFG1_PWR_CON                 (MTK_SLEEP_BASE + 0x30C)

void dump_gpu_dfd_debug_info(void)
{
    unsigned int status = readl(MFG_MISC_CON);
    unsigned int pwr = readl(DRM_MFG_REG);

    LTRACEF("[GPU_DFD]%s: GPU DFD MFG_MISC_CON:0x%x, DRM_MFG_REG:0x%x\n",
        __func__, status, pwr);

    LTRACEF("[GPU_DFD]%s: PWR_STATUS[0x1000616c]:0x%x, PWR_STATUS_2ND[0x10006170]:0x%x\n",
        __func__, readl(PWR_STATUS), readl(PWR_STATUS_2ND));
    LTRACEF(" MFG0_PWR_CON[0x10006308]:0x%x, MFG1_PWR_CON[0x1000630C]:0x%x\n",
        readl(MFG0_PWR_CON), readl(MFG1_PWR_CON));
}

static void save_gpu_dfd_data(CALLBACK dev_write)
{
    unsigned int mfg_misc_con = 0;
    unsigned int mfg_dfd_con_0 = 0;
    unsigned int *dfd_buffer = NULL;
    unsigned int i;

    LTRACEF("[GPU_DFD]%s: dump dfd data start\n", __func__);
    /* check if GPU DFD triggered */
    mfg_misc_con = readl(MFG_MISC_CON);
    if ((mfg_misc_con & MFG_DFD_TRIGGERED) != MFG_DFD_TRIGGERED) {
        LTRACEF("[GPU_DFD]%s: was not triggered (0x%x)\n", __func__, mfg_misc_con);
        return;
    }

    dump_gpu_dfd_debug_info();
    /* power on GPU */
    LTRACEF("[GPU_DFD]%s: Begin GPU0 MTCMOS\n", __func__);
    spm_mtcmos_ctrl_mfg0_enable();
    LTRACEF("[GPU_DFD]%s: end GPU0 MTCMOS\n", __func__);
    dump_gpu_dfd_debug_info();

    LTRACEF("[GPU_DFD]%s: Begin GPU1 MTCMOS\n", __func__);
    spm_mtcmos_ctrl_mfg1_enable();
    LTRACEF("[GPU_DFD]%s: end GPU1 MTCMOS\n", __func__);
    dump_gpu_dfd_debug_info();

    /* check if GPU DFD complete */
    if ((mfg_misc_con & MFG_DFD_COMPLETE) != MFG_DFD_COMPLETE) {
        LTRACEF("[GPU_DFD]%s: was not complete (0x%x)\n", __func__, mfg_misc_con);
        goto mfg_power_off;
    }

    /* config dfd dump setting for data readout */
    mfg_dfd_con_0 = readl(MFG_DFD_CON_0);
    LTRACEF("[GPU_DFD]%s: MFG_DFD_CON_0:0x%x\n", __func__, mfg_dfd_con_0);

    LTRACEF("[GPU_DFD]%s: write MFG_DFD_DUMP_SETTING\n", __func__);
    writel(MFG_DFD_DUMP_SETTING, MFG_DFD_CON_0);

    /* dump GPU DFD data */
    dfd_buffer = malloc(GPU_DFD_DATA_SIZE);
    if (dfd_buffer == NULL) {
        LTRACEF("[GPU_DFD]%s: malloc fail\n", __func__);
        goto mfg_power_off;
    }

    for (i = 0; i < (GPU_DFD_DATA_SIZE >> 2); i++)
        *(dfd_buffer + i) = readl(GPU_DFD_BASE + i * 0x4);

    LTRACEF("[GPU_DFD]%s: GPU_DFD[0x%x] = 0x%x\n", __func__, 0, dfd_buffer[0]);
    LTRACEF("[GPU_DFD]%s: GPU_DFD[0x%x] = 0x%x\n", __func__, 4, dfd_buffer[1]);
    LTRACEF("[GPU_DFD]%s: dump complete\n", __func__);

    /* write DFD data to DB */
    dev_write(vaddr_to_paddr(dfd_buffer), GPU_DFD_DATA_SIZE);
    free(dfd_buffer);

mfg_power_off:

    /* disable gpu dfd */
    LTRACEF("[GPU_DFD]%s: disable MFG_DFD_CON_0\n", __func__);
    writel(0x0, MFG_DFD_CON_0);

    /* clear wdt_mfg_pwr_on */
    LTRACEF("[GPU_DFD]%s: DRM_MFG_REG, wdt_mfg_pwr_on = 0\n", __func__);
    writel(MFG_PWR_OFF_DISABLE_SETTING, DRM_MFG_REG);

    /* power off GPU */
    dump_gpu_dfd_debug_info();
    LTRACEF("[GPU_DFD]%s: GPU DFD Begin GPU1 MTCMOS off\n", __func__);
    spm_mtcmos_ctrl_mfg1_disable();
    LTRACEF("[GPU_DFD]%s: GPU DFD End GPU1 MTCMOS off\n", __func__);
    dump_gpu_dfd_debug_info();

    LTRACEF("[GPU_DFD]%s: GPU DFD Begin GPU0 MTCMOS off\n", __func__);
    spm_mtcmos_ctrl_mfg0_disable();
    LTRACEF("[GPU_DFD]%s: GPU DFD End GPU0 MTCMOS off\n", __func__);
    dump_gpu_dfd_debug_info();

    LTRACEF("[GPU_DFD]%s: dump dfd data end\n", __func__);

    return;
}
#if !GPU_BRING_UP
AEE_EXPDB_INIT_HOOK(SYS_GPU_DFD, GPU_DFD_DATA_SIZE, save_gpu_dfd_data);
#endif

static unsigned int gpu_dfd_clear(void)
{
    /*
     * gpu hard reset to clear the gpu status for normal reboot
     *
     * hard reset GPU with following steps:
     * a. reset mfg
     * b. Wait 10us;
     * c. release reset
     */
    LTRACEF("[GPU_DFD]%s: gpu hard reset\n", __func__);
    writel(MFG_RST_SETTING, WDT_SWSYSRST);
    udelay(10);
    writel(MFG_RST_RELEASE_SETTING, WDT_SWSYSRST);

    /* do fake power on GPU */
    LTRACEF("[GPU_DFD]%s: do fake power on\n", __func__);
    dump_gpu_dfd_debug_info();
    spm_mtcmos_ctrl_mfg0_enable();
    dump_gpu_dfd_debug_info();
    spm_mtcmos_ctrl_mfg1_enable();
    dump_gpu_dfd_debug_info();

    /* disable gpu dfd */
    LTRACEF("[GPU_DFD]%s: disable MFG_DFD_CON_0\n", __func__);
    writel(0x0, MFG_DFD_CON_0);

    /* clear wdt_mfg_pwr_on */
    LTRACEF("[GPU_DFD]%s: DRM_MFG_REG, wdt_mfg_pwr_on = 0\n", __func__);
    writel(MFG_PWR_OFF_DISABLE_SETTING, DRM_MFG_REG);

    /* power off GPU */
    dump_gpu_dfd_debug_info();
    LTRACEF("[GPU_DFD]%s: GPU DFD Begin GPU1 MTCMOS off\n", __func__);
    spm_mtcmos_ctrl_mfg1_disable();
    LTRACEF("[GPU_DFD]%s: GPU DFD End GPU1 MTCMOS off\n", __func__);

    LTRACEF("[GPU_DFD]%s: GPU DFD Begin GPU0 MTCMOS off\n", __func__);
    spm_mtcmos_ctrl_mfg0_disable();
    LTRACEF("[GPU_DFD]%s: GPU DFD End GPU0 MTCMOS off\n", __func__);

    return 1;
}

void gpu_dfd_init(void)
{
    unsigned int mfg_misc_con = 0;
    unsigned int drm_mfg_reg = 0;

#if GPU_BRING_UP
    LTRACEF("[GPU_DFD]%s: GPU bring up state, no need to init\n", __func__);
    return;
#endif

    mfg_misc_con = readl(MFG_MISC_CON);
    if ((mfg_misc_con & MFG_DFD_TRIGGERED) != MFG_DFD_TRIGGERED) {
        dprintf(CRITICAL, "[GPU_DFD]%s: DFD was not triggered (0x%x)\n", __func__, mfg_misc_con);
    } else {
        dprintf(CRITICAL, "[GPU_DFD]%s: GPU DFD was triggered, start clear.\n", __func__);
        gpu_dfd_clear();
    }

    /* Enable power latch for mfg when dfd trigger */
    drm_mfg_reg = readl(DRM_MFG_REG);
    drm_mfg_reg = writel((drm_mfg_reg | MFG_PWR_ON_ENABLE_SETTING), DRM_MFG_REG);
    LTRACEF("[GPU_DFD]%s: drm_mfg_reg = 0x%x.\n", __func__, drm_mfg_reg);

    /*
     * Set the required timeout value of DFD HW dump before RGU reset
     * dbgtop driver will take the maximum as timeout value.
     */
    mtk_dbgtop_dfd_timeout(0x3E8, 0);

    return;
}
