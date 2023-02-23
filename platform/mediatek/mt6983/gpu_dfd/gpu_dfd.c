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
#include <platform/mboot_expdb.h>
#include <reg.h>
#include <stdio.h>
#include <string.h>
#include <trace.h>

#define GPU_BRING_UP 1
#define DBGTOP_READY 1
#define LOCAL_TRACE  0

#if DBGTOP_READY
#include <platform/dbgtop.h>
#endif

#define udelay(x)                    spin(x)

/*
 * IO_BASE     : 0x1000_0000
 * GPU_DFD_BASE: 0x13D0_0000 (0x13D0_0000 ~ 0x13F1_2FFF)
 */

#define GPU_DFD_BASE                 (IO_BASE + 0x03D00000)
#define GPU_DFD_DATA_SIZE            0x213000

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

#define MFG_DFD_DUMP_SETTING         (0x0F100011)

/*
 * TOPRGU_BASE : 0x1C00_7000
 * WDT_SWSYSRST: 0x1C00_7200
 */
#define TOPRGU_BASE                  (IO_BASE + 0x0C007000)
#define WDT_SWSYSRST                 (TOPRGU_BASE + 0x200)

/* todo: need to check how to software reset GPU IP*/
#define MFG_RST                      (1 << 2)
#define MFG_RST_SETTING              (0x88000000 + MFG_RST)
#define MFG_RST_RELEASE_SETTING      (0x88000000)

/*
 * DRM_DEBUG_BASE: 0x11FF_0000
 * DRM_MFG_REG   : 0x11FF_0060
 */
#define DRM_DEBUG_BASE               (IO_BASE + 0x01FF0000)
#define DRM_DEBUG_CTL                (DRM_DEBUG_BASE + 0x30)
#define DRM_LATCH_CTL                (DRM_DEBUG_BASE + 0x40)
#define DRM_LATCH_CTL2               (DRM_DEBUG_BASE + 0x44)
#define DRM_MFG_REG                  (DRM_DEBUG_BASE + 0x60)

#define MFG_PWR_ON_DISABLE_SETTING   (0x77000000)
#define MFG_PWR_ON_ENABLE_SETTING    (0x77000001)

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

    /* check if GPU DFD complete */
    if ((mfg_misc_con & MFG_DFD_COMPLETE) != MFG_DFD_COMPLETE) {
        LTRACEF("[GPU_DFD]%s: was not complete (0x%x)\n", __func__, mfg_misc_con);
        goto clear_dfd_status;
    }

    /* config dfd dump setting for data readout */
    writel(MFG_DFD_DUMP_SETTING, MFG_DFD_CON_0);

    /* dump GPU DFD data */
    dfd_buffer = malloc(GPU_DFD_DATA_SIZE);
    if (dfd_buffer == NULL) {
        LTRACEF("[GPU_DFD]%s: malloc fail\n", __func__);
        goto clear_dfd_status;
    }

    for (i = 0; i < (GPU_DFD_DATA_SIZE >> 2); i++)
        *(dfd_buffer + i) = readl(GPU_DFD_BASE + i * 0x4);

    LTRACEF("[GPU_DFD]%s: GPU_DFD[0x%x] = 0x%x\n", __func__, 0, dfd_buffer[0]);
    LTRACEF("[GPU_DFD]%s: GPU_DFD[0x%x] = 0x%x\n", __func__, 4, dfd_buffer[1]);
    LTRACEF("[GPU_DFD]%s: dump complete\n", __func__);

    /* write DFD data to DB */
    dev_write(vaddr_to_paddr(dfd_buffer), GPU_DFD_DATA_SIZE);
    free(dfd_buffer);

clear_dfd_status:
    /* disable gpu dfd */
    LTRACEF("[GPU_DFD]%s: disable MFG_DFD_CON_0\n", __func__);
    writel(0x0, MFG_DFD_CON_0);

    /* clear wdt_mfg_pwr_on */
    LTRACEF("[GPU_DFD]%s: DRM_MFG_REG, wdt_mfg_pwr_on = 0\n", __func__);
    writel(MFG_PWR_ON_DISABLE_SETTING, DRM_MFG_REG);

    LTRACEF("[GPU_DFD]%s: dump dfd data end\n", __func__);

    return;
}
#if !GPU_BRING_UP
AEE_EXPDB_INIT_HOOK(SYS_GPU_DFD, GPU_DFD_DATA_SIZE, save_gpu_dfd_data);
#endif

static void gpu_dfd_clear(void)
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

    /* disable gpu dfd */
    LTRACEF("[GPU_DFD]%s: disable MFG_DFD_CON_0\n", __func__);
    writel(0x0, MFG_DFD_CON_0);

    return;
}

void gpu_dfd_init(void)
{
    unsigned int mfg_misc_con = 0;
    unsigned int drm_mfg_reg = 0;
    unsigned int val = 0;

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

    val = (0x95 << 24) | (readl(DRM_LATCH_CTL) | 0x91);
    writel(val, DRM_LATCH_CTL);

    val = (0x59 << 24) | (readl(DRM_DEBUG_CTL) & 0xFFFFFFFD);
    writel(val, DRM_DEBUG_CTL);

#if DBGTOP_READY
   /*
     * Set the required timeout value of DFD HW dump before RGU reset
     * dbgtop driver will take the maximum as timeout value.
     *
     * HW: writel(0x950807D0, DRM_LATCH_CTL2);
     * [18:0] dfd timeout: 0x7D0 = 1000ms
     */

    if (mtk_dbgtop_dfd_timeout(0x7D0, 0) < 0) {
        dprintf(CRITICAL, "%s: set dfd timeout fail: %d\n", __func__);
        return;
    }

    if (mtk_dbgtop_dfd_count_en(1) < 0) {
        dprintf(CRITICAL, "%s: start dfd count fail: %d\n", __func__);
        return;
    }
#endif

    /* Enable power latch for mfg when dfd trigger */
    writel(MFG_PWR_ON_DISABLE_SETTING, DRM_MFG_REG);
    udelay(10);
    writel(MFG_PWR_ON_ENABLE_SETTING, DRM_MFG_REG);

    return;
}
