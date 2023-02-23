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
#define GPUDFD_TAG                   "[GPU_DFD]"

/*
 * IO_BASE     : 0x1000_0000
 * GPU_DFD_BASE: 0x13E0_0000 (0x13E0_0000 ~ 0x13F1_1FFF)
 */

#define GPU_DFD_BASE                 (IO_BASE + 0x03E00000)
#define GPU_DFD_DATA_SIZE            0x112000

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
 * TOPRGU_BASE : 0x1C00_7000
 * WDT_SWSYSRST: 0x1C00_7200
 */
#define TOPRGU_BASE                  (IO_BASE + 0x0C007000)
#define WDT_MFG_REG                  (TOPRGU_BASE + 0x068)
#define WDT_SWSYSRST                 (TOPRGU_BASE + 0x200)

/* todo: need to check how to software reset GPU IP*/
#define MFG_RST                      (1 << 2)
#define MFG_RST_SETTING              (0x88000000 + MFG_RST)
#define MFG_RST_RELEASE_SETTING      (0x88000000)

/*
 * DRM_DEBUG_BASE: 0x1000_D000
 * DRM_MFG_REG   : 0x1000_D060
 */
#define DRM_DEBUG_BASE               (IO_BASE + 0xD000)
#define DRM_DEBUG_CTL                (DRM_DEBUG_BASE + 0x30)
#define DRM_LATCH_CTL                (DRM_DEBUG_BASE + 0x40)
#define DRM_LATCH_CTL2               (DRM_DEBUG_BASE + 0x44)
#define DRM_MFG_REG                  (DRM_DEBUG_BASE + 0x60)

#define MFG_PWR_ON_DISABLE_SETTING   (0x77000000)
#define MFG_PWR_ON_ENABLE_SETTING    (0x77000001)

static void dump_gpu_dfd_register(void)
{
    LTRACEF("%s: DRM_DEBUG_CTL  (0x%X) = 0x%08x\n",
        GPUDFD_TAG, (unsigned int)DRM_DEBUG_CTL, readl(DRM_DEBUG_CTL));
    LTRACEF("%s: DRM_LATCH_CTL  (0x%X) = 0x%08x\n",
        GPUDFD_TAG, (unsigned int)DRM_LATCH_CTL, readl(DRM_LATCH_CTL));
    LTRACEF("%s: DRM_LATCH_CTL2 (0x%X) = 0x%08x\n",
        GPUDFD_TAG, (unsigned int)DRM_LATCH_CTL2, readl(DRM_LATCH_CTL2));
    LTRACEF("%s: DRM_MFG_REG    (0x%X) = 0x%08x\n",
        GPUDFD_TAG, (unsigned int)DRM_MFG_REG, readl(DRM_MFG_REG));
    LTRACEF("%s: WDT_SWSYSRST   (0x%X) = 0x%08x\n",
        GPUDFD_TAG, (unsigned int)WDT_SWSYSRST, readl(WDT_SWSYSRST));
    LTRACEF("%s: MFG_MISC_CON   (0x%X) = 0x%08x\n",
        GPUDFD_TAG, (unsigned int)MFG_MISC_CON, readl(MFG_MISC_CON));

    return;
}

static void save_gpu_dfd_data(CALLBACK dev_write)
{
    unsigned int mfg_misc_con = 0;
    unsigned int *dfd_buffer = NULL;
    unsigned int i;

    LTRACEF("%s: dump dfd data start\n", GPUDFD_TAG);
    dump_gpu_dfd_register();

    /* check if GPU DFD triggered */
    mfg_misc_con = readl(MFG_MISC_CON);
    if ((mfg_misc_con & MFG_DFD_TRIGGERED) != MFG_DFD_TRIGGERED) {
        LTRACEF("%s: was not triggered (0x%08x)\n", GPUDFD_TAG, mfg_misc_con);
        return;
    }

    /* check if GPU DFD complete */
    if ((mfg_misc_con & MFG_DFD_COMPLETE) != MFG_DFD_COMPLETE) {
        LTRACEF("%s: was not complete (0x%08x)\n", GPUDFD_TAG, mfg_misc_con);
        goto clear_dfd_status;
    }

    /* config dfd dump setting for data readout */
    writel(MFG_DFD_DUMP_SETTING, MFG_DFD_CON_0);
    LTRACEF("%s: After config dfd dump setting, MFG_DFD_CON_0 = 0x%08x\n",
        GPUDFD_TAG, readl(MFG_DFD_CON_0));

    /* dump GPU DFD data */
    dfd_buffer = malloc(GPU_DFD_DATA_SIZE);
    if (dfd_buffer == NULL) {
        LTRACEF("%s: malloc fail\n", GPUDFD_TAG);
        goto clear_dfd_status;
    }

    for (i = 0; i < (GPU_DFD_DATA_SIZE >> 2); i++)
        *(dfd_buffer + i) = readl(GPU_DFD_BASE + i * 0x4);

    /* check DFD data header */
    for (i = 0; i < 8; i++)
        LTRACEF("%s: [0x%x] = 0x%08x\n",
            GPUDFD_TAG, (unsigned int) (GPU_DFD_BASE + i*4), dfd_buffer[i]);

    LTRACEF("%s: dump complete\n", GPUDFD_TAG);

    /* write DFD data to DB */
    dev_write(vaddr_to_paddr(dfd_buffer), GPU_DFD_DATA_SIZE);
    free(dfd_buffer);

clear_dfd_status:
    /* disable gpu dfd */
    LTRACEF("%s: disable MFG_DFD_CON_0\n", GPUDFD_TAG);
    writel(0x0, MFG_DFD_CON_0);

    /* clear wdt_mfg_pwr_on */
    LTRACEF("%s: DRM_MFG_REG, wdt_mfg_pwr_on = 0\n", GPUDFD_TAG);
    writel(MFG_PWR_ON_DISABLE_SETTING, DRM_MFG_REG);

    LTRACEF("%s: WDT_MFG_REG = 0x%08x\n", GPUDFD_TAG, readl(WDT_MFG_REG));

    LTRACEF("%s: dump dfd data end\n", GPUDFD_TAG);

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
    LTRACEF("%s: gpu hard reset\n", GPUDFD_TAG);
    writel(MFG_RST_SETTING, WDT_SWSYSRST);
    udelay(10);
    writel(MFG_RST_RELEASE_SETTING, WDT_SWSYSRST);

    /* disable gpu dfd */
    LTRACEF("%s: disable MFG_DFD_CON_0\n", GPUDFD_TAG);
    writel(0x0, MFG_DFD_CON_0);

    return;
}

void gpu_dfd_init(void)
{
    unsigned int val = 0;

#if GPU_BRING_UP
    LTRACEF("%s: GPU bring up state, no need to init\n", GPUDFD_TAG);
    return;
#endif
    val = readl(MFG_MISC_CON);
    if ((val & MFG_DFD_TRIGGERED) != MFG_DFD_TRIGGERED) {
        dprintf(CRITICAL, "%s: DFD was not triggered (0x%x)\n", GPUDFD_TAG, val);
    } else {
        dprintf(CRITICAL, "%s: GPU DFD was triggered, start clear.\n", GPUDFD_TAG);
        gpu_dfd_clear();
    }

    /* Set top rgu */
    writel(0x22000005, TOPRGU_BASE);

    /* GPU Latch DRM_LATCH_CTL[7] */
    val = (0x95 << 24) | (readl(DRM_LATCH_CTL) | 0x91);
    writel(val, DRM_LATCH_CTL);

    /* Disable MCU Legacy Latch */
    val = (0x59 << 24) | (readl(DRM_DEBUG_CTL) & 0xFFFFFFFD);
    writel(val, DRM_DEBUG_CTL);

    LTRACEF("%s: before dbgtop setting\n", GPUDFD_TAG);
    dump_gpu_dfd_register();

#if DBGTOP_READY
   /*
    * Set the required timeout value of DFD HW dump before RGU reset
    * dbgtop driver will take the maximum as timeout value.
    *
    * HW: writel(0x950807D0, DRM_LATCH_CTL2);
    * [18:0] dfd timeout: 0x7D0 = 1000ms
    */

    if (mtk_dbgtop_dfd_timeout(0x7D0, 0) < 0) {
        dprintf(CRITICAL, "%s: set dfd timeout fail\n", __func__);
        return;
    }

    if (mtk_dbgtop_dfd_count_en(1) < 0) {
        dprintf(CRITICAL, "%s: start dfd count fail\n", __func__);
        return;
    }
#endif

    LTRACEF("%s before DRM_MFG_REG setting\n", GPUDFD_TAG);
    dump_gpu_dfd_register();

    /* Enable power latch for mfg when dfd trigger */
    val = readl(DRM_MFG_REG);
    if ((val & 0x1) != 0x1) {
        writel(MFG_PWR_ON_DISABLE_SETTING, DRM_MFG_REG);
        LTRACEF("%s after MFG_PWR_ON_DISABLE_SETTING\n", GPUDFD_TAG);
        dump_gpu_dfd_register();
        udelay(10);
        writel(MFG_PWR_ON_ENABLE_SETTING, DRM_MFG_REG);
    }

    LTRACEF("%s after DRM_MFG_REG setting\n", GPUDFD_TAG);
    dump_gpu_dfd_register();

    return;
}
