/*
 * Copyright (c) 2021 MediaTek Inc.
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
static void mmc_commandline_bootdevice(void)
{
    int ret = 0;
    char cmdline_buf[CMDLINE_BUF_SIZE];
#ifndef MSDC1_EMMC
    ret = snprintf(cmdline_buf, CMDLINE_BUF_SIZE,
        "androidboot.boot_devices=bootdevice,soc/%08x.mmc,%08x.mmc",
            MSDC0_BASE, MSDC0_BASE);
#else
    ret = snprintf(cmdline_buf, CMDLINE_BUF_SIZE,
        "androidboot.boot_devices=bootdevice,soc/%08x.mmc,%08x.mmc",
            MSDC1_BASE, MSDC1_BASE);
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
#define TRAP_UFS_FIRST  (1U << 11) /* bit 11 0: UFS, bit 11 1: eMMC */

void update_mmc_status(void *fdt)
{
    int node, ret = 0;
    char status[] = "disabled";

    if (fdt == NULL) {
        dprintf(CRITICAL, "kernel fdt is NULL!\n");
        goto out;
    }

    node = fdt_node_offset_by_compatible(fdt, -1, "mediatek,common-mmc");
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
    }

out:
    return;
}

SET_FDT_INIT_HOOK(update_mmc_status, update_mmc_status);
#endif
////////////////////////////////////////////////////////////////////////////////
//
// Section 1. Power Control -- Common for ASIC and FPGA
//
////////////////////////////////////////////////////////////////////////////////
#if !defined(FPGA_PLATFORM)
void msdc_dump_ldo_sts(struct mmc_host *host)
{
    u8 vmc_en = 0, vmc_vol = 0, vmc_cal = 0, vmch_en = 0, vmch_vol = 0, vmch_cal = 0;

    if (host->id == 1) {
        subpmic_read_interface(SUBPMIC_CHAN_LDO, REG_VMC_EN,
            &vmc_en, MASK_LDO3_EN, SHIFT_LDO3_EN);
        subpmic_read_interface(SUBPMIC_CHAN_LDO, REG_VMC_VOSEL,
            &vmc_vol, MASK_VMC_VOSEL, SHIFT_VMC_VOSEL);
        subpmic_read_interface(SUBPMIC_CHAN_LDO, REG_VMC_VOSEL_CAL,
            &vmc_cal, MASK_VMC_VOSEL_CAL, SHIFT_VMC_VOSEL_CAL);
        dprintf(CRITICAL,
            " VMC_EN=0x%x, VMC_VOL=0x%x [4b'0100(1V8),4b'1011(3V)], VMC_CAL=0x%x\n",
            vmc_en, vmc_vol, vmc_cal);
        subpmic_read_interface(SUBPMIC_CHAN_LDO, REG_VMCH_EN,
            &vmch_en, MASK_LDO5_EN, SHIFT_LDO5_EN);
        subpmic_read_interface(SUBPMIC_CHAN_LDO, REG_VMCH_VOSEL,
            &vmch_vol, MASK_VMCH_VOSEL, SHIFT_VMCH_VOSEL);
        subpmic_read_interface(SUBPMIC_CHAN_LDO, REG_VMCH_VOSEL_CAL,
            &vmch_cal, MASK_VMCH_VOSEL_CAL, SHIFT_VMCH_VOSEL_CAL);
        dprintf(CRITICAL,
            " VMCH_EN=0x%x, VMCH_VOL=0x%x [3b'011(3V)], VMCH_CAL=0x%x\n",
            vmch_en, vmch_vol, vmch_cal);
    }
}

u32 hwPowerOn(enum MSDC_POWER powerId, enum MSDC_POWER_VOL powerVolt)
{
    u32 ret;

    switch (powerId) {
    case MSDC_VMC:
        if (powerVolt == VOL_3300 || powerVolt == VOL_3000)
            subpmic_config_interface(SUBPMIC_CHAN_LDO, REG_VMC_VOSEL,
                VMC_VOSEL_3V, MASK_VMC_VOSEL, SHIFT_VMC_VOSEL);
        else if (powerVolt == VOL_1800) {
            subpmic_config_interface(SUBPMIC_CHAN_LDO, REG_VMC_VOSEL,
                VMC_VOSEL_1V8, MASK_VMC_VOSEL, SHIFT_VMC_VOSEL);
            subpmic_config_interface(SUBPMIC_CHAN_LDO, REG_VMC_VOSEL_CAL, 6,
                MASK_VMC_VOSEL_CAL, SHIFT_VMC_VOSEL_CAL);
        } else
            dprintf(CRITICAL,
                "Not support to Set VMC power to %d\n", powerVolt);

        subpmic_config_interface(SUBPMIC_CHAN_LDO, REG_VMC_EN,
            1, MASK_VMC_EN, SHIFT_VMC_EN);
        break;

    case MSDC_VMCH:
        if (powerVolt == VOL_3000) {
            subpmic_config_interface(SUBPMIC_CHAN_LDO, REG_VMCH_VOSEL,
                VMCH_VOSEL_3V, MASK_VMCH_VOSEL, SHIFT_VMCH_VOSEL);
            if (SD_VOL_ACTUAL != VOL_3000) {
                subpmic_config_interface(SUBPMIC_CHAN_LDO,
                    REG_VMCH_VOSEL_CAL,
                    VMCH_VOSEL_CAL_mV(SD_VOL_ACTUAL - VOL_3000),
                    MASK_VMCH_VOSEL_CAL, SHIFT_VMCH_VOSEL_CAL);
            }
        } else if (powerVolt == VOL_3300) {
            subpmic_config_interface(SUBPMIC_CHAN_LDO, REG_VMCH_VOSEL,
                VMCH_VOSEL_3V3, MASK_VMCH_VOSEL, SHIFT_VMCH_VOSEL);
        } else {
            dprintf(CRITICAL,
                "Not support to Set VMCH power to %d\n", powerVolt);
        }
        subpmic_config_interface(SUBPMIC_CHAN_LDO, REG_VMCH_EN, 1,
            MASK_VMCH_EN, SHIFT_VMCH_EN);
        break;

    default:
        dprintf(CRITICAL, "Not support to Set %d power on\n", powerId);
        break;
    }

    mdelay(100); /* requires before voltage stable */

    return 0;
}

u32 hwPowerDown(enum MSDC_POWER powerId)
{
    switch (powerId) {
    case MSDC_VMC:
        subpmic_config_interface(SUBPMIC_CHAN_LDO,
            REG_VMC_EN, 0, MASK_VMC_EN, SHIFT_VMC_EN);
        break;
    case MSDC_VMCH:
        subpmic_config_interface(SUBPMIC_CHAN_LDO,
            REG_VMCH_EN, 0, MASK_VMCH_EN, SHIFT_VMCH_EN);
        break;
    default:
        dprintf(CRITICAL, "Not support to Set %d power down\n", powerId);
        break;
    }

    return 0;
}

static u32 msdc_ldo_power(u32 on, enum MSDC_POWER powerId,
    enum MSDC_POWER_VOL powerVolt, u32 *status)
{
    if (on) { // want to power on
        if (*status == 0) {  // can power on
            dprintf(INFO, "msdc LDO<%d> power on<%d>\n", powerId, powerVolt);
            hwPowerOn(powerId, powerVolt);
            *status = powerVolt;
        } else if (*status == powerVolt) {
            dprintf(INFO, "msdc LDO<%d><%d> power on again!\n", powerId, powerVolt);
        } else { // for sd3.0 later
            dprintf(INFO, "msdc LDO<%d> change<%d> to <%d>\n", powerId, *status, powerVolt);
            hwPowerDown(powerId);
            hwPowerOn(powerId, powerVolt);
            *status = powerVolt;
        }
    } else {  // want to power off
        if (*status != 0) {  // has been powerred on
            dprintf(INFO, "msdc LDO<%d> power off\n", powerId);
            hwPowerDown(powerId);
            *status = 0;
        } else {
            dprintf(INFO, "LDO<%d> not power on\n", powerId);
        }
    }

    return 0;
}
#endif

void msdc_card_power(struct mmc_host *host, u32 on)
{
    //Only CTP support power on/off for verification purose.
    //Preload and LK need not touch power since it is default on
    dprintf(INFO, "[SD%d] Turn %s card power\n", host->id, on ? "on" : "off");
    switch (host->id) {
    case 0:
        //default on,no need change;
        break;
    case 1:
        msdc_ldo_power(on, MSDC_VMCH, VOL_3000, &g_msdc1_flash);
        mdelay(10);
        break;
    default:
        break;
    }
}

void msdc_host_power(struct mmc_host *host, u32 on, u32 level)
{
    //Only CTP support power on/off for verification purose.
    //Preload and LK need not touch power since it is default on
    u32 card_on = on;
    struct msdc_cust msdc_cap = get_msdc_capability(host->id);

    if (host->id != 0)
        host->cur_pwr = level;

    msdc_set_tdsel_wrap(host);
    msdc_set_rdsel_wrap(host);
    msdc_set_driving(host, &msdc_cap);

    dprintf(INFO, "[SD%d] Turn %s host power\n", host->id, on ? "on" : "off");

    switch (host->id) {
    case 0:
        //default on,no need change;
        break;
    case 1:
        msdc_ldo_power(on, MSDC_VMC, level, &g_msdc1_io);
        mdelay(10);
        break;
    default:
        break;
    }
}

void msdc_power(struct mmc_host *host, u8 mode)
{
    if (mode == MMC_POWER_ON || mode == MMC_POWER_UP) {
#if !defined(FPGA_PLATFORM)
        msdc_pin_config(host, MSDC_PIN_PULL_UP);
#endif
        msdc_host_power(host, 1, host->cur_pwr);
        msdc_card_power(host, 1);
        msdc_clock(host, 1);
    } else {
        msdc_clock(host, 0);
        msdc_host_power(host, 0, host->cur_pwr);
        msdc_card_power(host, 0);
#if !defined(FPGA_PLATFORM)
        msdc_pin_config(host, MSDC_PIN_PULL_DOWN);
#endif

    }
    mmc_commandline_bootdevice();
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
#if !defined(FPGA_PLATFORM)
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
    if (host->id == 0) {
        dprintf(ALWAYS, "MSDC0 HCLK_MUX[0x1000_0080][1:0] = %d, pdn = %d, ",
                /* mux at bits 1~0 */
                (MSDC_READ32(IO_BASE + 0x80) >> 0) & 3,
                /* pdn at bit 7 */
                (MSDC_READ32(IO_BASE + 0x80) >> 7) & 1);
        dprintf(ALWAYS, "CLK_MUX[0x1000_0080][10:8] = %d, pdn = %d, ",
                /* mux at bits 10~8 */
                (MSDC_READ32(IO_BASE + 0x80) >> 8) & 7,
                /* pdn at bit 15 */
                (MSDC_READ32(IO_BASE + 0x80) >> 15) & 1);
        dprintf(ALWAYS, "CLK_CG[0x1000_1094] bit2 = %d, bit6 = %d\n",
                /* cg at bit 2, 6 */
                (MSDC_READ32(INFRACFG_AO_BASE + 0x94) >> 2) & 1,
                (MSDC_READ32(INFRACFG_AO_BASE + 0x94) >> 6) & 1);
    } else if (host->id == 1) {
        dprintf(ALWAYS, "CLK_MUX[0x1000_0080][18:16] = %d, pdn = %d, ",
                /* mux at bits 18~16 */
                (MSDC_READ32(IO_BASE + 0x80) >> 16) & 7,
                /* pdn at bit 23 */
                (MSDC_READ32(IO_BASE + 0x80) >> 23) & 1);
        dprintf(ALWAYS, "CLK_CG[0x1000_1094] bit4 = %d, bit16 = %d\n",
                /* cg at bit 4, 16 */
                (MSDC_READ32(INFRACFG_AO_BASE + 0x94) >> 4) & 1,
                (MSDC_READ32(INFRACFG_AO_BASE + 0x94) >> 16) & 1);
    }
#endif
}
#endif

void msdc_clock(struct mmc_host *host, int on)
{
    /*
     * Only CTP need enable/disable clock
     * Preloader will turn on clock with predefined clock source
     * and module need not to touch clock setting.
     * LK will use preloader's setting and need not to touch clock setting.
     */
    if (on) {
        if (host->id == 1) {
            MSDC_SET_BIT32(MSDC1_CLOCK_UNGATE_REG, MSDC1_CLOCK_CG);
            MSDC_SET_BIT32(MSDC1_CLOCK_UNGATE_REG, MSDC1_CLOCK_SRC_CG);
        }
    } else {
        if (host->id == 1) {
            MSDC_SET_BIT32(MSDC1_CLOCK_GATE_REG, MSDC1_CLOCK_CG);
            MSDC_SET_BIT32(MSDC1_CLOCK_GATE_REG, MSDC1_CLOCK_SRC_CG);
        }
    }

#if !defined(FPGA_PLATFORM)
    msdc_dump_clock_sts(host);

#endif
}

/* perloader will pre-set msdc pll and the mux channel of msdc pll */
/* note: pll will not changed */
void msdc_config_clksrc(struct mmc_host *host, int clksrc)
{
#if !defined(FPGA_PLATFORM)
    if (host->id == 0) {
        host->pll_mux_clk = MSDC0_CLKSRC_DEFAULT;
        host->src_clk = msdc_get_hclk(host->id, MSDC0_CLKSRC_DEFAULT);
    } else if (host->id == 1) {
        host->pll_mux_clk = MSDC1_CLKSRC_DEFAULT;
        host->src_clk = msdc_get_hclk(host->id, MSDC1_CLKSRC_DEFAULT);
    }
#else
    host->src_clk = msdc_get_hclk(host->id, clksrc);
#endif

}



/**************************************************************/
/* Section 3: GPIO and Pad                    */
/**************************************************************/
#if !defined(FPGA_PLATFORM)
void msdc_dump_padctl_by_id(u32 id)
{
#if defined(MTK_MSDC_BRINGUP_DEBUG)
    if (id == 0) {
        dprintf(ALWAYS, "MSDC0 MODE22 [%lx] = 0x%X\tshould: 0x1???????\n",
                MSDC0_GPIO_MODE22, MSDC_READ32(MSDC0_GPIO_MODE22));
        dprintf(ALWAYS, "MSDC0 MODE23 [%lx] = 0x%X\tshould: 0x11111111\n",
                MSDC0_GPIO_MODE23, MSDC_READ32(MSDC0_GPIO_MODE23));
        dprintf(ALWAYS, "MSDC0 MODE24 [%lx] = 0x%X\tshould: 0x?????111\n",
                MSDC0_GPIO_MODE24, MSDC_READ32(MSDC0_GPIO_MODE24));
        dprintf(ALWAYS, "MSDC0 IES [%lx] = 0x%X\tshould: 0x3FFC????\n",
                MSDC0_GPIO_IES, MSDC_READ32(MSDC0_GPIO_IES));
        dprintf(ALWAYS, "MSDC0 SMT [%lx] = 0x%X\tshould: 0x??????7C\n",
                MSDC0_GPIO_SMT, MSDC_READ32(MSDC0_GPIO_SMT));
        dprintf(ALWAYS, "MSDC0 TDSEL0 [%lx] = 0x%X",
                MSDC0_GPIO_TDSEL0,
                MSDC_READ32(MSDC0_GPIO_TDSEL0));
        dprintf(ALWAYS, "should: 0x00000000\n");
        dprintf(ALWAYS, "MSDC0 RDSEL0 [%lx] = 0x%X, [%lx] = 0x%X\n",
                MSDC0_GPIO_RDSEL0,
                MSDC_READ32(MSDC0_GPIO_RDSEL0),
                MSDC0_GPIO_RDSEL0_1,
                MSDC_READ32(MSDC0_GPIO_RDSEL0_1));
        dprintf(ALWAYS, "should: 0x00000000 , should: 0x00000000\n");
        dprintf(ALWAYS, "MSDC0 DRV0 [%lx] = 0x%X\n", MSDC0_GPIO_DRV0,
                MSDC_READ32(MSDC0_GPIO_DRV0));
        dprintf(ALWAYS, "should: 0x?9249249, should: 0x???????9\n");
        dprintf(ALWAYS, "PUPD/R1/R0: dat/cmd:0/0/1, clk/dst: 1/1/0\n");
        dprintf(ALWAYS, "MSDC0 PUPD0 [%lx] = 0x%X\tshould: 0x?????401\n",
                MSDC0_GPIO_PUPD0,
                MSDC_READ32(MSDC0_GPIO_PUPD0));
        dprintf(ALWAYS, "MSDC0 R0 [%lx] = 0x%X\tshould: 0x?????BFE\n",
                MSDC0_GPIO_R0,
                MSDC_READ32(MSDC0_GPIO_R0));
        dprintf(ALWAYS, "MSDC0 R1 [%lx] = 0x%X\tshould: 0x?????401\n",
                MSDC0_GPIO_R1,
                MSDC_READ32(MSDC0_GPIO_R1));
    } else if (id == 1) {
        dprintf(ALWAYS, "MSDC1_A_GPIO_MISC [%lx] =0x%8x\tshould: bit[9]=0\n",
            MSDC1_GPIO_MISC, MSDC_READ32(MSDC1_GPIO_MISC));
        dprintf(ALWAYS, "MSDC1 MODE6 [%lx] = 0x%X\tshould: 0x11111???\n",
                MSDC1_GPIO_MODE6, MSDC_READ32(MSDC1_GPIO_MODE6));
        dprintf(ALWAYS, "MSDC1 MODE7 [%lx] = 0x%X\tshould: 0x???????1\n",
                MSDC1_GPIO_MODE7, MSDC_READ32(MSDC1_GPIO_MODE7));
        dprintf(ALWAYS, "MSDC1 IES [%lx] = 0x%X\t 21-16bits should: 2b111111\n",
            MSDC1_GPIO_IES, MSDC_READ32(MSDC1_GPIO_IES));
        dprintf(ALWAYS, "MSDC1 SMT [%lx] = 0x%X\t 8-6bits should: 2b111\n",
            MSDC1_GPIO_SMT, MSDC_READ32(MSDC1_GPIO_SMT));
        dprintf(ALWAYS, "MSDC1 TDSEL0 [%lx] = 0x%X, [%lx] =0x%X\n",
            MSDC1_GPIO_TDSEL0,
            MSDC_READ32(MSDC1_GPIO_TDSEL0),
            MSDC1_GPIO_TDSEL0_1,
            MSDC_READ32(MSDC1_GPIO_TDSEL0_1));
        dprintf(ALWAYS, "should 1.8v: sleep: TBD, awake: TBD\n");
        dprintf(ALWAYS, "MSDC1 RDSEL0 [%lx] = 0x%X\n",
            MSDC1_GPIO_RDSEL0, MSDC_READ32(MSDC1_GPIO_RDSEL0));
        dprintf(ALWAYS, "1.8V: TBD, 2.9v: TBD\n");
        dprintf(ALWAYS, "MSDC1 DRV0 [%lx] = 0x%X\n",
            MSDC1_GPIO_DRV0, MSDC_READ32(MSDC1_GPIO_DRV0));
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
    if (host->id == 0) {
        MSDC_SET_FIELD(MSDC0_GPIO_MODE22, 0xF0000000, 0x1);
        MSDC_SET_FIELD(MSDC0_GPIO_MODE23, 0xFFFFFFFF, 0x11111111);
        MSDC_SET_FIELD(MSDC0_GPIO_MODE24, 0x00000FFF, 0x111);
    } else if (host->id == 1) {
        MSDC_SET_FIELD(MSDC1_GPIO_MISC, MSDC1_PIN_MUX_SEL_MASK_A, 0x0);
        MSDC_SET_FIELD(MSDC1_GPIO_MODE6, 0xFFFFF000, 0x11111);
        MSDC_SET_FIELD(MSDC1_GPIO_MODE7, 0x0000000F, 0x1);
    }
}

void msdc_set_ies_by_id(u32 id, int set_ies)
{
    if (id == 0)
        MSDC_SET_FIELD(MSDC0_GPIO_IES, MSDC0_IES_ALL_MASK,
            (set_ies ? 0xFFF : 0));
    else if (id == 1)
        MSDC_SET_FIELD(MSDC1_GPIO_IES, MSDC1_IES_ALL_MASK,
            (set_ies ? 0x3F : 0));
}

void msdc_set_smt_by_id(u32 id, int set_smt)
{
    if (id == 0)
        MSDC_SET_FIELD(MSDC0_GPIO_SMT, MSDC0_SMT_ALL_MASK, (set_smt ? 0xF : 0));
    else if (id == 1)
        MSDC_SET_FIELD(MSDC1_GPIO_SMT, MSDC1_SMT_ALL_MASK, (set_smt ? 0x7 : 0));
}

void msdc_set_tdsel_by_id(u32 id, u32 flag, u32 value)
{
    u32 cust_val;

    if (id == 0) {
        if (flag == MSDC_TDRDSEL_CUST)
            cust_val = value;
        else
            cust_val = 0;
        MSDC_SET_FIELD(MSDC0_GPIO_TDSEL0, MSDC0_TDSEL0_CMD_MASK, cust_val);
        MSDC_SET_FIELD(MSDC0_GPIO_TDSEL0, MSDC0_TDSEL0_DAT_MASK, cust_val);
        MSDC_SET_FIELD(MSDC0_GPIO_TDSEL0, MSDC0_TDSEL0_CLK_MASK, cust_val);
        MSDC_SET_FIELD(MSDC0_GPIO_TDSEL0, MSDC0_TDSEL0_DSL_MASK, cust_val);
    } else if (id == 1) {
        if (flag == MSDC_TDRDSEL_CUST)
            cust_val = value;
        else
            cust_val = 0;
        MSDC_SET_FIELD(MSDC1_GPIO_TDSEL0_1, MSDC1_TDSEL0_DAT_MASK, cust_val);
        MSDC_SET_FIELD(MSDC1_GPIO_TDSEL0, MSDC1_TDSEL0_CMD_MASK, cust_val);
        MSDC_SET_FIELD(MSDC1_GPIO_TDSEL0, MSDC1_TDSEL0_CLK_MASK, cust_val);
    }
}

void msdc_set_rdsel_by_id(u32 id, u32 flag, u32 value)
{
    u32 cust_val;

    if (id == 0) {
        if (flag == MSDC_TDRDSEL_CUST)
            cust_val = value;
        else
            cust_val = 0;
        MSDC_SET_FIELD(MSDC0_GPIO_RDSEL0, MSDC0_RDSEL0_CMD_MASK, cust_val);
        MSDC_SET_FIELD(MSDC0_GPIO_RDSEL0, MSDC0_RDSEL0_DAT_MASK, cust_val);
        MSDC_SET_FIELD(MSDC0_GPIO_RDSEL0, MSDC0_RDSEL0_CLK_MASK, cust_val);
        MSDC_SET_FIELD(MSDC0_GPIO_RDSEL0, MSDC0_RDSEL0_DSL_MASK, cust_val);
        MSDC_SET_FIELD(MSDC0_GPIO_RDSEL0_1, MSDC0_RDSEL0_RSTB_MASK, cust_val);
    } else if (id == 1) {
        if (flag == MSDC_TDRDSEL_CUST)
            cust_val = value;
        else
            cust_val = 0;
        MSDC_SET_FIELD(MSDC1_GPIO_RDSEL0, MSDC1_RDSEL0_CMD_MASK, cust_val);
        MSDC_SET_FIELD(MSDC1_GPIO_RDSEL0, MSDC1_RDSEL0_DAT_MASK, cust_val);
        MSDC_SET_FIELD(MSDC1_GPIO_RDSEL0, MSDC1_RDSEL0_CLK_MASK, cust_val);
    }
}

void msdc_get_tdsel_by_id(u32 id, u32 *value)
{
    if (id == 0)
        MSDC_GET_FIELD(MSDC0_GPIO_TDSEL0, MSDC0_TDSEL0_CMD_MASK, *value);
    else if (id == 1)
        MSDC_GET_FIELD(MSDC1_GPIO_TDSEL0, MSDC1_TDSEL0_CMD_MASK, *value);
}

void msdc_get_rdsel_by_id(u32 id, u32 *value)
{
    if (id == 0)
        MSDC_GET_FIELD(MSDC0_GPIO_RDSEL0, MSDC0_RDSEL0_CMD_MASK, *value);
    else if (id == 1)
        MSDC_GET_FIELD(MSDC1_GPIO_RDSEL0, MSDC1_RDSEL0_CMD_MASK, *value);
}

void msdc_set_sr_by_id(u32 id, int clk, int cmd, int dat, int rst, int ds)
{
    /*SR not supported*/
    return;
}

void msdc_set_driving_by_id(u32 id, struct msdc_cust *msdc_cap)
{
    if (id == 0) {
        MSDC_SET_FIELD(MSDC0_GPIO_DRV0, MSDC0_DRV0_DSL_MASK, msdc_cap->ds_drv);
        MSDC_SET_FIELD(MSDC0_GPIO_DRV0, MSDC0_DRV0_DAT_MASK, msdc_cap->dat_drv);
        MSDC_SET_FIELD(MSDC0_GPIO_DRV0, MSDC0_DRV0_CMD_MASK, msdc_cap->cmd_drv);
        MSDC_SET_FIELD(MSDC0_GPIO_DRV0, MSDC0_DRV0_CLK_MASK, msdc_cap->clk_drv);
    } else if (id == 1) {
        MSDC_SET_FIELD(MSDC1_GPIO_DRV0, MSDC1_DRV0_CMD_MASK, msdc_cap->cmd_drv);
        MSDC_SET_FIELD(MSDC1_GPIO_DRV0, MSDC1_DRV0_CLK_MASK, msdc_cap->clk_drv);
        MSDC_SET_FIELD(MSDC1_GPIO_DRV0, MSDC1_DRV0_DAT_MASK, msdc_cap->dat_drv);
    }
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
            MSDC_SET_FIELD(MSDC0_GPIO_R1, MSDC0_R1_ALL_MASK, 0xFFF);
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
}

/**************************************************************/
/* Section 4: MISC                                            */
/**************************************************************/
/* make sure the pad is msdc mode */
#if !defined(FPGA_PLATFORM)
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
#endif
#endif

#define MSDC0_SDC_ADV_CFG0 (MSDC0_BASE + 0x64)
#define MSDC1_SDC_ADV_CFG0 (MSDC1_BASE + 0x64)
#define MSDC_SPM_REQ_EN    (7U << 22) /* bit 22:24 msdc spm req enable bits */
/* Enable msdc HW SPM resource request, otherwise SPM always see msdc res req siganl busy */
void msdc_spm_hw_res_req_en(void)
{
    MSDC_SET_FIELD(MSDC0_SDC_ADV_CFG0, MSDC_SPM_REQ_EN, 0x7);
    MSDC_SET_FIELD(MSDC1_SDC_ADV_CFG0, MSDC_SPM_REQ_EN, 0x7);
}

#if !defined(FPGA_PLATFORM)
/* Apply HS400 DS delay */
void msdc_hs400_ds_delay(struct mmc_host *host)
{
    writel(0x14814, TOP_EMMC50_PAD_DS_TUNE); /* Apply 2731 DS delay setting */
}

/* Efuse to decide 8bit or 1bit booting */
int EFUSE_IS_EMMC_BOOT_BUS_WIDTH_8BIT(void)
{
    return 1;
}
#endif
