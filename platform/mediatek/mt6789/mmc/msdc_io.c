/*
 * Copyright (c) 2022 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#include <cust_msdc.h>
#include <err.h>
#include <errno.h>
#include <kernel/event.h>
#include <libfdt.h>
#include <mmc_core.h>
#include <msdc_io_layout.h>
#include <set_fdt.h>
#include <lib/kcmdline.h>
#include <lk/init.h>
#include <partition_utils.h>
#include <stdbool.h>
#include <subpmic.h>

#include "msdc_io.h"

//#define MTK_MSDC_BRINGUP_DEBUG

u32 g_msdc0_io;
u32 g_msdc1_io;

u32 g_msdc0_flash;
u32 g_msdc1_flash;

#define CMDLINE_BUF_SIZE 128

/*
 * Setting kernel command line of boot device
 */
void mmc_commandline_bootdevice(void)
{
    int ret = 0;
    char cmdline_buf[CMDLINE_BUF_SIZE];
#ifndef MSDC1_EMMC
    ret = snprintf(cmdline_buf, CMDLINE_BUF_SIZE,
        "androidboot.boot_devices=bootdevice,soc/%08zx.mmc,%08zx.mmc",
        (MSDC0_BASE - KERNEL_ASPACE_BASE), (MSDC0_BASE - KERNEL_ASPACE_BASE));
#else
    ret = snprintf(cmdline_buf, CMDLINE_BUF_SIZE,
        "androidboot.boot_devices=bootdevice,soc/%08zx.mmc,%08zx.mmc",
        (MSDC1_BASE - KERNEL_ASPACE_BASE), (MSDC1_BASE - KERNEL_ASPACE_BASE));
#endif
    if (ret < 0) {
        dprintf(ALWAYS, "Partition append bootdevice to command line fail");
        return;
    }

    ret = kcmdline_append(cmdline_buf);
    if (ret != NO_ERROR) {
        dprintf(ALWAYS, "kcmdline append %s failed, rc=%d\n", cmdline_buf, ret);
        return;
    }
    dprintf(ALWAYS, "kcmdline append %s\n", cmdline_buf);
    return;
}

#if !defined(FPGA_PLATFORM)
#define GPIO_BANK       (GPIO_BASE + 0x6F0)
#define TRAP_UFS_FIRST  (0x1U << 10) /* bit 10 0: UFS, 1: eMMC */

/* restore msdc pin mode to default value */
void msdc_pin_mode_deinit(void)
{
    MSDC_SET_FIELD(MSDC0_GPIO_MODE6, 0xFF000000, 0x0);
    MSDC_SET_FIELD(MSDC0_GPIO_MODE7, 0xFFFFFFFF, 0x0);
    MSDC_SET_FIELD(MSDC0_GPIO_MODE8, 0x000000FF, 0x0);
}

void update_mmc_dts(void *fdt)
{
    int node, ret = 0;
    char status[] = "disabled";
    char data[32] = {0};

    if (fdt == NULL) {
        dprintf(CRITICAL, "kernel fdt is NULL!\n");
        goto out;
    }

    node = fdt_node_offset_by_compatible(fdt, -1, "mediatek,mt6789-mmc");
    if (node < 0) {
        dprintf(CRITICAL, "%s:fail to find node(%d)\n",
            __func__, node);
        goto out;
    }

    if ((MSDC_READ32(GPIO_BANK) & TRAP_UFS_FIRST) == 0) {
        dprintf(ALWAYS, "%s:disable mmc0 when ufs boot\n", __func__);
        ret = fdt_setprop(fdt, node, "status", status, sizeof(status));
        if (ret)
            dprintf(CRITICAL, "%s:fail to set status to disabled(%d)\n",
                __func__, ret);
        msdc_pin_mode_deinit();
    } else {
        node = fdt_path_offset(fdt, "/aliases");
        if (node < 0) {
            dprintf(CRITICAL, "%s:fail to find aliases node(%d)\n",
                __func__, node);
            goto out;
        }
        ret = snprintf(data, sizeof(data), "/soc/mmc@%08zx", (MSDC0_BASE - KERNEL_ASPACE_BASE));
        if (ret < 0) {
            dprintf(CRITICAL, "%s:fail to filling mmc alias string\n", __func__);
            goto out;
        }
        ret = fdt_setprop_string(fdt, node, "mmc0", data);
        dprintf(ALWAYS, "%s:set mmc alias as %s\n", __func__, data);
        if (ret)
            dprintf(CRITICAL, "%s:fail to set mmc alias(%d)\n", __func__, ret);
    }

out:
    return;
}
SET_FDT_INIT_HOOK(update_mmc_dts, update_mmc_dts);

#endif
////////////////////////////////////////////////////////////////////////////////
//
// Section 1. Power Control -- Common for ASIC and FPGA
//
////////////////////////////////////////////////////////////////////////////////
#ifndef FPGA_PLATFORM
void msdc_dump_ldo_sts(struct mmc_host *host)
{
}
#endif

void msdc_card_power(struct mmc_host *host, u32 on)
{
    /* power is default on */
}

void msdc_host_power(struct mmc_host *host, u32 on, u32 level)
{
    /* power is default on */
}

void msdc_power(struct mmc_host *host, u8 mode)
{
    if (mode == MMC_POWER_ON || mode == MMC_POWER_UP) {
        msdc_pin_config(host, MSDC_PIN_PULL_UP);
        msdc_host_power(host, 1, host->cur_pwr);
        msdc_card_power(host, 1);
        msdc_clock(host, 1);
    } else {
        msdc_clock(host, 0);
        msdc_host_power(host, 0, host->cur_pwr);
        msdc_card_power(host, 0);
        msdc_pin_config(host, MSDC_PIN_PULL_DOWN);
    }
}

/* MT2712EVB, GPIO67 to control SD VCCQ, output H --> 3.3V, output L --> 1.8V */
/* set to 3.3V */
void sd_card_vccq_on(void)
{
    /* Need porting if needed */
}

/**************************************************************/
/* Section 2: Clock                    */
/**************************************************************/
#ifdef FPGA_PLATFORM
void msdc_config_clksrc(struct mmc_host *host, int clksrc)
{
    host->src_clk = msdc_get_hclk(host->id, clksrc);
}
void msdc_dump_clock_sts(struct mmc_host *host)
{
}
#else
u32 hclks_msdc0[] = { MSDC0_SRC_0, MSDC0_SRC_1, MSDC0_SRC_2, MSDC0_SRC_3,
                      MSDC0_SRC_4, MSDC0_SRC_5
                    };

/* msdc1/2 clock source reference value is 200M */
u32 hclks_msdc1[] = { MSDC1_SRC_0, MSDC1_SRC_1, MSDC1_SRC_2, MSDC1_SRC_3,
                      MSDC1_SRC_4
                    };

u32 *hclks_msdc_all[] = {
    hclks_msdc0,
    hclks_msdc1,
};

void msdc_dump_clock_sts(struct mmc_host *host)
{
#if defined(MTK_MSDC_BRINGUP_DEBUG)
    dprintf(ALWAYS, "MSDC0 HCLK_MUX[0x1000_0070][25:24] = %d, pdn = %d,",
        /* mux at bits 25~24 */
        (MSDC_READ32(TOPCLK_BASE + 0x70) >> 24) & 3,
        /* pdn at bit 31 */
        (MSDC_READ32(TOPCLK_BASE + 0x70) >> 31) & 1);
    dprintf(ALWAYS, "CLK_MUX[0x1000_0080][2:0] = %d, pdn = %d,",
        /* mux at bits 2~0 */
        (MSDC_READ32(TOPCLK_BASE + 0x80) >> 0) & 7,
        /* pdn at bit 7 */
        (MSDC_READ32(TOPCLK_BASE + 0x80) >> 7) & 1);
    dprintf(ALWAYS, "CLK_CG[0x1000_1094] bit2 = %d, bit6 = %d, bit17 = %d\n",
        /* cg at bit 2, 6, 17 */
        (MSDC_READ32(INFRACFG_AO_BASE + 0x94) >> 2) & 1,
        (MSDC_READ32(INFRACFG_AO_BASE + 0x94) >> 6) & 1,
        (MSDC_READ32(INFRACFG_AO_BASE + 0x94) >> 17) & 1);
#endif
}

/* perloader will pre-set msdc pll and the mux channel of msdc pll */
/* note: pll will not changed */
void msdc_config_clksrc(struct mmc_host *host, int clksrc)
{
    if (host->id == 0) {
        host->pll_mux_clk = MSDC0_CLKSRC_DEFAULT;
        host->src_clk = msdc_get_hclk(host->id, MSDC0_CLKSRC_DEFAULT);
    } else if (host->id == 1) {
        host->pll_mux_clk = MSDC1_CLKSRC_DEFAULT;
        host->src_clk = msdc_get_hclk(host->id, MSDC1_CLKSRC_DEFAULT);
    }
}
#endif

void msdc_clock(struct mmc_host *host, int on)
{
    /* clock is default on */
    msdc_dump_clock_sts(host);
}

/**************************************************************/
/* Section 3: GPIO and Pad                    */
/**************************************************************/
void msdc_dump_padctl_by_id(u32 id)
{
#if defined(MTK_MSDC_BRINGUP_DEBUG)
    if (id == 0) {
        dprintf(ALWAYS, "MSDC0 MODE6[%x] = 0x%X\tshould: 0x11??????\n",
            MSDC0_GPIO_MODE6, MSDC_READ32(MSDC0_GPIO_MODE6));
        dprintf(ALWAYS, "MSDC0 MODE7[%x] = 0x%X\tshould: 0x11111111\n",
            MSDC0_GPIO_MODE7, MSDC_READ32(MSDC0_GPIO_MODE7));
        dprintf(ALWAYS, "MSDC0 MODE8[%x] = 0x%X\tshould: 0x??????11\n",
            MSDC0_GPIO_MODE8, MSDC_READ32(MSDC0_GPIO_MODE8));
        dprintf(ALWAYS, "MSDC0 IES   [%x] = 0x%X\tshould: 0x???FFF??\n",
            MSDC0_GPIO_IES, MSDC_READ32(MSDC0_GPIO_IES));
        dprintf(ALWAYS, "MSDC0 SMT   [%x] = 0x%X\tshould: 0x???7FF8?\n",
            MSDC0_GPIO_SMT, MSDC_READ32(MSDC0_GPIO_SMT));
        dprintf(ALWAYS, "MSDC0 TDSEL0[%x] = 0x%X\tshould: 0x0???????\n",
            MSDC0_GPIO_TDSEL0,
            MSDC_READ32(MSDC0_GPIO_TDSEL0));
        dprintf(ALWAYS, "MSDC0 TDSEL1[%x] = 0x%X\tshould: 0x????????\n",
            MSDC0_GPIO_TDSEL1,
            MSDC_READ32(MSDC0_GPIO_TDSEL1));
        dprintf(ALWAYS, "MSDC0 TDSEL2[%x] = 0x%X\tshould: 0x00000???\n",
            MSDC0_GPIO_TDSEL2,
            MSDC_READ32(MSDC0_GPIO_TDSEL2));
        dprintf(ALWAYS, "MSDC0 RDSEL0[%x] = 0x%X,[%x] = 0x%X\tshould: 0x????0000\n",
            MSDC0_GPIO_RDSEL0,
            MSDC_READ32(MSDC0_GPIO_RDSEL0));
        dprintf(ALWAYS, "MSDC0 RDSEL1[%x] = 0x%X,[%x] = 0x%X\tshould: 0x00000000\n",
            MSDC0_GPIO_RDSEL1,
            MSDC_READ32(MSDC0_GPIO_RDSEL1));
        dprintf(ALWAYS, "MSDC0 RDSEL2[%x] = 0x%X,[%x] = 0x%X\tshould: 0x??000000\n",
            MSDC0_GPIO_RDSEL2,
            MSDC_READ32(MSDC0_GPIO_RDSEL2));
        dprintf(ALWAYS, "MSDC0 DRV0  [%x] = 0x%X\tshould: 0x?9249???\n",
            MSDC0_GPIO_DRV0,
            MSDC_READ32(MSDC0_GPIO_DRV0));
        dprintf(ALWAYS, "MSDC0 DRV1  [%x] = 0x%X\tshould: 0x????9249\n",
            MSDC0_GPIO_DRV1,
            MSDC_READ32(MSDC0_GPIO_DRV1));
        dprintf(ALWAYS, "PUPD/R1/R0: dat/cmd:0/0/1, clk/dst: 1/1/0\n");
        dprintf(ALWAYS, "MSDC0 PUPD0 [%x] = 0x%X\tshould: 0x?????401\n",
            MSDC0_GPIO_PUPD0,
            MSDC_READ32(MSDC0_GPIO_PUPD0));
        dprintf(ALWAYS, "MSDC0 R0 [%x] = 0x%X\tshould: 0x?????BFE\n",
            MSDC0_GPIO_R0,
            MSDC_READ32(MSDC0_GPIO_R0));
        dprintf(ALWAYS, "MSDC0 R1 [%x] = 0x%X\tshould: 0x?????401\n",
            MSDC0_GPIO_R1,
            MSDC_READ32(MSDC0_GPIO_R1));
    } else if (id == 1) {
        dprintf(ALWAYS, "MSDC1 MODE10 [%lx] = 0x%X\tshould: 0x111111??\n",
                MSDC1_GPIO_MODE10, MSDC_READ32(MSDC1_GPIO_MODE10));
        dprintf(ALWAYS, "MSDC1 IES [%lx] = 0x%X\t 8-13bits should: 2b111111\n",
            MSDC1_GPIO_IES, MSDC_READ32(MSDC1_GPIO_IES));
        dprintf(ALWAYS, "MSDC1 SMT [%lx] = 0x%X\t 6-11bits should: 2b111111\n",
            MSDC1_GPIO_SMT, MSDC_READ32(MSDC1_GPIO_SMT));
        dprintf(ALWAYS, "MSDC1 TDSEL0 [%lx] = 0x%X, [%lx] =0x%X\n",
            MSDC1_GPIO_TDSEL0,
            MSDC_READ32(MSDC1_GPIO_TDSEL0));
        dprintf(ALWAYS, "should 1.8v: sleep: TBD, awake: TBD\n");
        dprintf(ALWAYS, "MSDC1 RDSEL0 [%lx] = 0x%X\n",
            MSDC1_GPIO_RDSEL0, MSDC_READ32(MSDC1_GPIO_RDSEL0));
        dprintf(ALWAYS, "MSDC1 RDSEL1 [%lx] = 0x%X\n",
            MSDC1_GPIO_RDSEL1, MSDC_READ32(MSDC1_GPIO_RDSEL1));
        dprintf(ALWAYS, "1.8V: TBD, 2.9v: TBD\n");
        dprintf(ALWAYS, "MSDC1 DRV0 [%lx] = 0x%X\n",
            MSDC1_GPIO_DRV0, MSDC_READ32(MSDC1_GPIO_DRV0));
        dprintf(ALWAYS, "MSDC1 DRV0 [%lx] = 0x%X\n",
            MSDC1_GPIO_DRV1, MSDC_READ32(MSDC1_GPIO_DRV1));
        dprintf(ALWAYS, "MSDC1 PUPD0 [%lx] = 0x%X\tshould: 0x??????01\n",
            MSDC1_GPIO_PUPD0, MSDC_READ32(MSDC1_GPIO_PUPD0));
        dprintf(ALWAYS, "MSDC1 R0 [%lx] = 0x%X\tshould: 0x??????00\n",
            MSDC1_GPIO_R0, MSDC_READ32(MSDC1_GPIO_R0));
        dprintf(ALWAYS, "MSDC1 R1 [%lx] = 0x%X\tshould: 0x??????3F\n",
            MSDC1_GPIO_R1, MSDC_READ32(MSDC1_GPIO_R1));
    }
#endif
}

void msdc_set_pin_mode(struct mmc_host *host)
{
#ifndef FPGA_PLATFORM
    if (host->id == 0) {
        MSDC_SET_FIELD(MSDC0_GPIO_MODE6, 0xFF000000, 0x11);
        MSDC_SET_FIELD(MSDC0_GPIO_MODE7, 0xFFFFFFFF, 0x11111111);
        MSDC_SET_FIELD(MSDC0_GPIO_MODE8, 0x000000FF, 0x11);
    } else if (host->id == 1) {
        MSDC_SET_FIELD(MSDC1_GPIO_MODE10, 0xFFFFFF00, 0x111111);
    }
#endif
}

void msdc_set_ies_by_id(u32 id, int set_ies)
{
#ifndef FPGA_PLATFORM
    if (id == 0)
        MSDC_SET_FIELD(MSDC0_GPIO_IES, MSDC0_IES_ALL_MASK,
            (set_ies ? 0xFFF : 0));
    else if (id == 1)
        MSDC_SET_FIELD(MSDC1_GPIO_IES, MSDC1_IES_ALL_MASK,
            (set_ies ? 0x3F : 0));
#endif
}

void msdc_set_smt_by_id(u32 id, int set_smt)
{
#ifndef FPGA_PLATFORM
    if (id == 0)
        MSDC_SET_FIELD(MSDC0_GPIO_SMT, MSDC0_SMT_ALL_MASK, (set_smt ? 0xFFF : 0));
    else if (id == 1)
        MSDC_SET_FIELD(MSDC1_GPIO_SMT, MSDC1_SMT_ALL_MASK, (set_smt ? 0x3F : 0));
#endif
}

void msdc_set_tdsel_by_id(u32 id, u32 flag, u32 value)
{
#if !defined(FPGA_PLATFORM)
    u32 cust_val;

    if (id == 0) {
        if (flag == MSDC_TDRDSEL_CUST)
            cust_val = value;
        else
            cust_val = 0;
        MSDC_SET_FIELD(MSDC0_GPIO_TDSEL1, MSDC0_TDSEL1_CMD_MASK, cust_val);
        MSDC_SET_FIELD(MSDC0_GPIO_TDSEL0, MSDC0_TDSEL0_CLK_MASK, cust_val);
        MSDC_SET_FIELD(MSDC0_GPIO_TDSEL1, MSDC0_TDSEL1_DAT0_MASK, cust_val);
        MSDC_SET_FIELD(MSDC0_GPIO_TDSEL1, MSDC0_TDSEL1_DAT1_MASK, cust_val);
        MSDC_SET_FIELD(MSDC0_GPIO_TDSEL1, MSDC0_TDSEL1_DAT2_MASK, cust_val);
        MSDC_SET_FIELD(MSDC0_GPIO_TDSEL1, MSDC0_TDSEL1_DAT3_MASK, cust_val);
        MSDC_SET_FIELD(MSDC0_GPIO_TDSEL1, MSDC0_TDSEL1_DAT4_MASK, cust_val);
        MSDC_SET_FIELD(MSDC0_GPIO_TDSEL1, MSDC0_TDSEL1_DAT5_MASK, cust_val);
        MSDC_SET_FIELD(MSDC0_GPIO_TDSEL1, MSDC0_TDSEL1_DAT6_MASK, cust_val);
        MSDC_SET_FIELD(MSDC0_GPIO_TDSEL2, MSDC0_TDSEL2_DAT7_MASK, cust_val);
        MSDC_SET_FIELD(MSDC0_GPIO_TDSEL2, MSDC0_TDSEL2_DSL_MASK, cust_val);
        MSDC_SET_FIELD(MSDC0_GPIO_TDSEL2, MSDC0_TDSEL2_RSTB_MASK, cust_val);
    } else if (id == 1) {
        if (flag == MSDC_TDRDSEL_CUST)
            cust_val = value;
        else
            cust_val = 0;
        MSDC_SET_FIELD(MSDC1_GPIO_TDSEL0, MSDC1_TDSEL0_DAT0_MASK, cust_val);
        MSDC_SET_FIELD(MSDC1_GPIO_TDSEL0, MSDC1_TDSEL0_DAT1_MASK, cust_val);
        MSDC_SET_FIELD(MSDC1_GPIO_TDSEL0, MSDC1_TDSEL0_DAT2_MASK, cust_val);
        MSDC_SET_FIELD(MSDC1_GPIO_TDSEL0, MSDC1_TDSEL0_DAT3_MASK, cust_val);
        MSDC_SET_FIELD(MSDC1_GPIO_TDSEL0, MSDC1_TDSEL0_CMD_MASK, cust_val);
        MSDC_SET_FIELD(MSDC1_GPIO_TDSEL0, MSDC1_TDSEL0_CLK_MASK, cust_val);
    }
#endif
}

void msdc_set_rdsel_by_id(u32 id, u32 flag, u32 value)
{
#if !defined(FPGA_PLATFORM)
    u32 cust_val;

    if (id == 0) {
        if (flag == MSDC_TDRDSEL_CUST)
            cust_val = value;
        else
            cust_val = 0;
        MSDC_SET_FIELD(MSDC0_GPIO_RDSEL0, MSDC0_RDSEL0_CMD_MASK, cust_val);
        MSDC_SET_FIELD(MSDC0_GPIO_RDSEL0, MSDC0_RDSEL0_CLK_MASK, cust_val);
        MSDC_SET_FIELD(MSDC0_GPIO_RDSEL0, MSDC0_RDSEL0_DAT0_MASK, cust_val);
        MSDC_SET_FIELD(MSDC0_GPIO_RDSEL1, MSDC0_RDSEL1_DAT1_MASK, cust_val);
        MSDC_SET_FIELD(MSDC0_GPIO_RDSEL1, MSDC0_RDSEL1_DAT2_MASK, cust_val);
        MSDC_SET_FIELD(MSDC0_GPIO_RDSEL1, MSDC0_RDSEL1_DAT3_MASK, cust_val);
        MSDC_SET_FIELD(MSDC0_GPIO_RDSEL1, MSDC0_RDSEL1_DAT4_MASK, cust_val);
        MSDC_SET_FIELD(MSDC0_GPIO_RDSEL1, MSDC0_RDSEL1_DAT5_MASK, cust_val);
        MSDC_SET_FIELD(MSDC0_GPIO_RDSEL2, MSDC0_RDSEL2_DAT6_MASK, cust_val);
        MSDC_SET_FIELD(MSDC0_GPIO_RDSEL2, MSDC0_RDSEL2_DAT7_MASK, cust_val);
        MSDC_SET_FIELD(MSDC0_GPIO_RDSEL2, MSDC0_RDSEL2_RSTB_MASK, cust_val);
        MSDC_SET_FIELD(MSDC0_GPIO_RDSEL2, MSDC0_RDSEL2_DSL_MASK, cust_val);
    } else if (id == 1) {
        if (flag == MSDC_TDRDSEL_CUST)
            cust_val = value;
        else
            cust_val = 0;
        MSDC_SET_FIELD(MSDC1_GPIO_RDSEL0, MSDC1_RDSEL0_CMD_MASK, cust_val);
        MSDC_SET_FIELD(MSDC1_GPIO_RDSEL0, MSDC1_RDSEL0_CLK_MASK, cust_val);
        MSDC_SET_FIELD(MSDC1_GPIO_RDSEL0, MSDC1_RDSEL0_DAT0_MASK, cust_val);
        MSDC_SET_FIELD(MSDC1_GPIO_RDSEL0, MSDC1_RDSEL0_DAT1_MASK, cust_val);
        MSDC_SET_FIELD(MSDC1_GPIO_RDSEL0, MSDC1_RDSEL0_DAT2_MASK, cust_val);
        MSDC_SET_FIELD(MSDC1_GPIO_RDSEL1, MSDC1_RDSEL1_DAT3_MASK, cust_val);
    }
#endif
}

void msdc_get_tdsel_by_id(u32 id, u32 *value)
{
#ifndef FPGA_PLATFORM
    if (id == 0)
        MSDC_GET_FIELD(MSDC0_GPIO_TDSEL1, MSDC0_TDSEL1_CMD_MASK, *value);
    else if (id == 1)
        MSDC_GET_FIELD(MSDC1_GPIO_TDSEL0, MSDC1_TDSEL0_CMD_MASK, *value);
#endif
}

void msdc_get_rdsel_by_id(u32 id, u32 *value)
{
#ifndef FPGA_PLATFORM
    if (id == 0)
        MSDC_GET_FIELD(MSDC0_GPIO_RDSEL0, MSDC0_RDSEL0_CMD_MASK, *value);
    else if (id == 1)
        MSDC_GET_FIELD(MSDC1_GPIO_RDSEL0, MSDC1_RDSEL0_CMD_MASK, *value);
#endif
}

void msdc_set_sr_by_id(u32 id, int clk, int cmd, int dat, int rst, int ds)
{
    /*SR not supported*/
    return;
}

void msdc_set_driving_by_id(u32 id, struct msdc_cust *msdc_cap)
{
#ifndef FPGA_PLATFORM
    if (id == 0) {
        MSDC_SET_FIELD(MSDC0_GPIO_DRV1, MSDC0_DRV1_DSL_MASK,
            msdc_cap->ds_drv);
        MSDC_SET_FIELD(MSDC0_GPIO_DRV0, MSDC0_DRV0_DAT0_MASK,
            msdc_cap->dat_drv);
        MSDC_SET_FIELD(MSDC0_GPIO_DRV0, MSDC0_DRV0_DAT1_MASK,
            msdc_cap->dat_drv);
        MSDC_SET_FIELD(MSDC0_GPIO_DRV0, MSDC0_DRV0_DAT2_MASK,
            msdc_cap->dat_drv);
        MSDC_SET_FIELD(MSDC0_GPIO_DRV0, MSDC0_DRV0_DAT3_MASK,
            msdc_cap->dat_drv);
        MSDC_SET_FIELD(MSDC0_GPIO_DRV1, MSDC0_DRV1_DAT4_MASK,
            msdc_cap->dat_drv);
        MSDC_SET_FIELD(MSDC0_GPIO_DRV1, MSDC0_DRV1_DAT5_MASK,
            msdc_cap->dat_drv);
        MSDC_SET_FIELD(MSDC0_GPIO_DRV1, MSDC0_DRV1_DAT6_MASK,
            msdc_cap->dat_drv);
        MSDC_SET_FIELD(MSDC0_GPIO_DRV1, MSDC0_DRV1_DAT7_MASK,
            msdc_cap->dat_drv);
        MSDC_SET_FIELD(MSDC0_GPIO_DRV0, MSDC0_DRV0_CMD_MASK,
            msdc_cap->cmd_drv);
        MSDC_SET_FIELD(MSDC0_GPIO_DRV0, MSDC0_DRV0_CLK_MASK,
            msdc_cap->clk_drv);
    } else if (id == 1) {
        MSDC_SET_FIELD(MSDC1_GPIO_DRV0, MSDC1_DRV0_CMD_MASK, msdc_cap->cmd_drv);
        MSDC_SET_FIELD(MSDC1_GPIO_DRV0, MSDC1_DRV0_CLK_MASK, msdc_cap->clk_drv);
        MSDC_SET_FIELD(MSDC1_GPIO_DRV0, MSDC1_DRV0_DAT0_MASK, msdc_cap->dat_drv);
        MSDC_SET_FIELD(MSDC1_GPIO_DRV0, MSDC1_DRV0_DAT1_MASK, msdc_cap->dat_drv);
        MSDC_SET_FIELD(MSDC1_GPIO_DRV1, MSDC1_DRV1_DAT2_MASK, msdc_cap->dat_drv);
        MSDC_SET_FIELD(MSDC1_GPIO_DRV1, MSDC1_DRV1_DAT3_MASK, msdc_cap->dat_drv);
    }
#endif
}

/* msdc pin config
 * MSDC0
 * PUPD/R1/R0
 * 0/0/0: High-Z
 * 0/0/1: Pull-up with 10Kohm
 * 0/1/0: Pull-up with 50Kohm
 * 0/1/1: Pull-up with 10Kohm//50Kohm
 * 1/0/0: High-Z
 * 1/0/1: Pull-down with 10Kohm
 * 1/1/0: Pull-down with 50Kohm
 * 1/1/1: Pull-down with 10Kohm//50Kohm
 */
void msdc_pin_config_by_id(u32 id, u32 mode)
{
#ifndef FPGA_PLATFORM
    if (id == 0) {
        /* 1. don't pull CLK high;
         * 2. Don't toggle RST to prevent from entering boot mode
         */
        if (mode == MSDC_PIN_PULL_NONE) {
            /* Switch MSDC0_* to no ohm PU */
            MSDC_SET_FIELD(MSDC0_GPIO_PUPD0, MSDC0_PUPD_ALL_MASK, 0x0);
            MSDC_SET_FIELD(MSDC0_GPIO_R0, MSDC0_R0_ALL_MASK, 0x0);
            MSDC_SET_FIELD(MSDC0_GPIO_R1, MSDC0_R1_ALL_MASK, 0x0);
        } else if (mode == MSDC_PIN_PULL_DOWN) {
            /* Switch MSDC0_* to 50K ohm PD */
            MSDC_SET_FIELD(MSDC0_GPIO_PUPD0, MSDC0_PUPD_ALL_MASK, 0x7FF);
            MSDC_SET_FIELD(MSDC0_GPIO_R0, MSDC0_R0_ALL_MASK, 0x0);
            MSDC_SET_FIELD(MSDC0_GPIO_R1, MSDC0_R1_ALL_MASK, 0x7FF);
        } else if (mode == MSDC_PIN_PULL_UP) {
            /*
             * Switch MSDC0_CLK to 50K ohm PD,
             * MSDC0_CMD/MSDC0_DAT* to 10K ohm PU,
             * MSDC0_DSL to 50K ohm PD
             */
            MSDC_SET_FIELD(MSDC0_GPIO_PUPD0, MSDC0_PUPD_ALL_MASK, 0x401);
            MSDC_SET_FIELD(MSDC0_GPIO_R0, MSDC0_R0_ALL_MASK, 0xBFE);
            MSDC_SET_FIELD(MSDC0_GPIO_R1, MSDC0_R1_ALL_MASK, 0x401);
        }
    } else if (id == 1) {
        if (mode == MSDC_PIN_PULL_NONE) {
            /* Switch MSDC1_* to no ohm PU */
            MSDC_SET_FIELD(MSDC1_GPIO_PUPD0, MSDC1_PUPD_ALL_MASK, 0x0);
            MSDC_SET_FIELD(MSDC1_GPIO_R0, MSDC1_R0_ALL_MASK, 0x0);
            MSDC_SET_FIELD(MSDC1_GPIO_R1, MSDC1_R1_ALL_MASK, 0x0);
        } else if (mode == MSDC_PIN_PULL_DOWN) {
            /* Switch MSDC1_* to 50K ohm PD */
            MSDC_SET_FIELD(MSDC1_GPIO_PUPD0, MSDC1_PUPD_ALL_MASK, 0x3F);
            MSDC_SET_FIELD(MSDC1_GPIO_R0, MSDC1_R0_ALL_MASK, 0x0);
            MSDC_SET_FIELD(MSDC1_GPIO_R1, MSDC1_R1_ALL_MASK, 0x3F);
        } else if (mode == MSDC_PIN_PULL_UP) {
            /* Switch MSDC1_CLK to 50K ohm PD,
             * MSDC1_CMD/MSDC1_DAT* to 50K ohm PU
             */
            MSDC_SET_FIELD(MSDC1_GPIO_PUPD0, MSDC1_PUPD_ALL_MASK, 0x1);
            MSDC_SET_FIELD(MSDC1_GPIO_R0, MSDC1_R0_ALL_MASK, 0x0);
            MSDC_SET_FIELD(MSDC1_GPIO_R1, MSDC1_R1_ALL_MASK, 0x3F);
        }
    }
#endif
}

/**************************************************************/
/* Section 4: MISC                                            */
/**************************************************************/
/* make sure the pad is msdc mode */
void msdc_set_tdsel_wrap(struct mmc_host *host)
{
    if (host->cur_pwr == VOL_1800)
        msdc_set_tdsel_by_id(host->id, MSDC_TDRDSEL_1V8, 0);
    else
        msdc_set_tdsel_by_id(host->id, MSDC_TDRDSEL_3V, 0);
}

void msdc_set_rdsel_wrap(struct mmc_host *host)
{
    if (host->cur_pwr == VOL_1800)
        msdc_set_rdsel_by_id(host->id, MSDC_TDRDSEL_1V8, 0);
    else
        msdc_set_rdsel_by_id(host->id, MSDC_TDRDSEL_3V, 0);
}

void msdc_gpio_and_pad_init(struct mmc_host *host)
{
    struct msdc_cust msdc_cap = get_msdc_capability(host->id);

    /* set smt enable */
    msdc_set_smt(host, 1);

    /* set pull enable */
    msdc_pin_config(host, MSDC_PIN_PULL_UP);

    /* set gpio to msdc mode */
    msdc_set_pin_mode(host);

    /* set ies enable */
    msdc_set_ies(host, 1);

    /* set driving */
    msdc_set_driving(host, &msdc_cap);

    /* set tdsel and rdsel */
    msdc_set_tdsel_wrap(host);
    msdc_set_rdsel_wrap(host);

    msdc_dump_padctl_by_id(host->id);
}

#define MSDC0_SDC_ADV_CFG0 (MSDC0_BASE + 0x64)
#define MSDC1_SDC_ADV_CFG0 (MSDC1_BASE + 0x64)
#define MSDC_SPM_REQ_EN    (7U << 22) /* bit 22:24 msdc spm req enable bits */
/* Enable msdc HW SPM resource request, otherwise SPM always see msdc res req siganl busy */
void msdc_spm_hw_res_req_en(void)
{
    MSDC_SET_FIELD(MSDC0_SDC_ADV_CFG0, MSDC_SPM_REQ_EN, 0x7);
    MSDC_SET_FIELD(MSDC1_SDC_ADV_CFG0, MSDC_SPM_REQ_EN, 0x7);
}

/* Apply HS400 DS delay */
void msdc_hs400_ds_delay(struct mmc_host *host)
{
#if !defined(FPGA_PLATFORM)
    writel(0x14814, TOP_EMMC50_PAD_DS_TUNE); /* Apply 2731 DS delay setting */
#endif
}

/* Efuse to decide 8bit or 1bit booting */
int EFUSE_IS_EMMC_BOOT_BUS_WIDTH_8BIT(void)
{
    return 1;
}
