/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */
#include <cust_msdc.h>
#include <gpio_api.h>
#include <kernel/event.h>
#include <mmc_core.h>
#include <msdc_io_layout.h>
#include <stdbool.h>

#include "msdc_io.h"

////////////////////////////////////////////////////////////////////////////////
//
// Section 1. Power Control -- Common for ASIC and FPGA
//
////////////////////////////////////////////////////////////////////////////////
#if defined(FPGA_PLATFORM)
#define PWR_GPIO        (0x10000E84) //Select Power On/Off
#define PWR_GPIO_EO     (0x10000E88) //Select Output/Input Direction

#define PWR_MASK_CARD   (0x1 << 8)   //Vcc
#define PWR_MASK_VOL_18 (0x1 << 9)   //CLK/CMD/DAT for eMMC/SD +  Vccq for eMMC
#define PWR_MASK_VOL_33 (0x1 << 10)  //CLK/CMD/DAT for eMMC/SD +  Vccq for eMMC
#define PWR_MASK_L4     (0x1 << 11)
#define PWR_MSDC        (PWR_MASK_CARD | PWR_MASK_VOL_18 | PWR_MASK_VOL_33 | PWR_MASK_L4)

#define MSDC_MAX_NUM    1

//#define FPGA_GPIO_DEBUG
static void msdc_clr_gpio(u32 bits)
{
    u32 l_val = 0;
    char *name;

    switch (bits) {
    case PWR_MASK_CARD:
        name = "card pwr";
        break;
    case PWR_MASK_VOL_18:
        name = "SD/eMMC bus pwer 1.8V";
        break;
    case PWR_MASK_VOL_33:
        name = "SD/eMMC bus pwer 3.3V";
        break;
    case PWR_MASK_L4:
        name = "L4";
        break;
    default:
        dprintf(ALWAYS, "[%s:%s]invalid value: 0x%x\n", __FILE__, __func__, bits);
        return;
    }

    MSDC_GET_FIELD(PWR_GPIO_EO, bits, l_val);
    //dprintf(ALWAYS, "====%s====%d\n", name, l_val);
    if (l_val == 0) {
        dprintf(ALWAYS, "check me! gpio for %s is input\n", name);
        MSDC_SET_FIELD(PWR_GPIO_EO, bits, 1);
    }
    /* check for set before */
    if (bits &  MSDC_READ32(PWR_GPIO)) {
        dprintf(ALWAYS, "clear %s\n", name);
        MSDC_SET_FIELD(PWR_GPIO, bits, 0);
        //l_val = MSDC_READ32(PWR_GPIO);
    }

#ifdef FPGA_GPIO_DEBUG
    u32 val = 0;

    val = MSDC_READ32(PWR_GPIO);
    dprintf(ALWAYS, "[clr]PWR_GPIO[8-11]:0x%x\n", val);
    val = MSDC_READ32(PWR_GPIO_EO);
    dprintf(ALWAYS, "[clr]GPIO_DIR[8-11]:0x%x\n", val);
#endif
}

static void msdc_set_gpio(u32 bits)
{
    u32 l_val = 0;
    char *name;

    switch (bits) {
    case PWR_MASK_CARD:
        name = "card pwr";
        break;
    case PWR_MASK_VOL_18:
        name = "SD/eMMC bus pwer 1.8V";
        break;
    case PWR_MASK_VOL_33:
        name = "SD/eMMC bus pwer 3.3V";
        break;
    case PWR_MASK_L4:
        name = "L4";
        break;
    default:
        dprintf(ALWAYS, "[%s]invalid parm: 0x%x\n", __func__, bits);
        return;
    }

    MSDC_GET_FIELD(PWR_GPIO_EO, bits, l_val);
    //dprintf(ALWAYS, "====%s====%d\n", name, l_val);
    if (l_val == 0) {
        dprintf(ALWAYS, "check me! gpio for %s is input\n", name);
        MSDC_SET_FIELD(PWR_GPIO_EO, bits, 1);
    }
    /* check for set before */
    if ((bits &  MSDC_READ32(PWR_GPIO)) == 0) {
        dprintf(ALWAYS, "Set %s\n", name);
        MSDC_SET_FIELD(PWR_GPIO, bits, 1);
        //l_val = MSDC_READ32(PWR_GPIO);
    }

#ifdef FPGA_GPIO_DEBUG
    u32 val = 0;

    val = MSDC_READ32(PWR_GPIO);
    dprintf(ALWAYS, "[set]PWR_GPIO[8-11]:0x%x\n", val);
    val = MSDC_READ32(PWR_GPIO_EO);
    dprintf(ALWAYS, "[set]GPIO_DIR[8-11]:0x%x\n", val);
#endif
}
#endif

#if defined(FPGA_PLATFORM)
void msdc_card_power(struct mmc_host *host, u32 on)
{
    dprintf(INFO, "[SD%d] Turn %s card power\n", host->id, on ? "on" : "off");
    switch (host->id) {
    case 0:
        if (on) {
            //On V6/V7 FPGA, card power need to be on/off
            //On HAPS FPGA, card power maybe always on.
            //Left this code for safety prevent from card power not always not.
            msdc_set_gpio(PWR_MASK_CARD);
        } else {
            msdc_clr_gpio(PWR_MASK_CARD);
        }
        spin(10000);
        break;
    default:
        //No MSDC1 in FPGA
        break;
    }

    //add for fpga debug
    msdc_set_gpio(PWR_MASK_L4);
}

void msdc_host_power(struct mmc_host *host, u32 on, u32 level)
{
    dprintf(INFO, "[SD%d] Turn %s host power\n", host->id, on ? "on" : "off");

    // GPO[3:2] = {LVL_PWR33, LVL_PWR18};
    msdc_clr_gpio(PWR_MASK_VOL_18);
    msdc_clr_gpio(PWR_MASK_VOL_33);

    switch (host->id) {
    case 0:
        if (on) {
            if (level == VOL_1800)
                msdc_set_gpio(PWR_MASK_VOL_18);
            else
                msdc_set_gpio(PWR_MASK_VOL_33);
        }
        spin(10000);
        break;
    default:
        //No MSDC1 in FPGA
        break;
    }

    msdc_set_gpio(PWR_MASK_L4);
}
#else
void msdc_card_power(struct mmc_host *host, u32 on)
{
    //Only CTP support power on/off for verification purose.
    //Preload and LK need not touch power since it is default on
    dprintf(INFO, "[SD%d] Turn %s card power\n", host->id, on ? "on" : "off");
}

void msdc_host_power(struct mmc_host *host, u32 on, u32 level)
{
    //Only CTP support power on/off for verification purose.
    //Preload and LK need not touch power since it is default on
}

void msdc_dump_ldo_sts(struct mmc_host *host)
{
}
#endif

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
#if !defined(FPGA_PLATFORM)
u32 hclks_msdc0[] = {MSDC0_SRC_0, MSDC0_SRC_1, MSDC0_SRC_2, MSDC0_SRC_3,
             MSDC0_SRC_4, MSDC0_SRC_5};

/* msdc1/2 clock source reference value is 200M */
u32 hclks_msdc1[] = {MSDC1_SRC_0, MSDC1_SRC_1, MSDC1_SRC_2, MSDC1_SRC_3,
             MSDC1_SRC_4};

u32 *hclks_msdc_all[] = {
    hclks_msdc0,
    hclks_msdc1,
};

void msdc_dump_clock_sts(struct mmc_host *host)
{
#if defined(MTK_MSDC_BRINGUP_DEBUG)
    dprintf(ALWAYS, "MSDC0 HCLK_MUX[0x1000_0080][1:0] = %d, pdn = %d, ",
        /* mux at bits 1~0 */
        (MSDC_READ32(IO_BASE + 0x80) >> 0) & 3,
        /* pdn at bit 7 */
        (MSDC_READ32(IO_BASE + 0x80) >> 7) & 1);
    dprintf(ALWAYS, "CLK_MUX[0x1000_0080][10:8] = %d, pdn = %d, "
        /* mux at bits 8~10 */
        (MSDC_READ32(IO_BASE + 0x80) >> 8) & 7,
        /* pdn at bit 15 */
        (MSDC_READ32(IO_BASE + 0x80) >> 15) & 1);
    dprintf(ALWAYS, "CLK_CG[0x1000_1094] bit2 = %d, bit6 %d\n",
        /* cg at bit 2, 6 */
        (MSDC_READ32(IO_BASE + 0x94) >> 2) & 1,
        (MSDC_READ32(IO_BASE + 0x94) >> 6) & 1);
    dprintf(ALWAYS, "MSDC1 CLK_MUX[0x1000_0080][18:16] = %d, pdn = %d, ",
        /* mux at bits 16~18 */
        (MSDC_READ32(IO_BASE + 0x80) >> 16) & 7,
        /* pdn at bit 23 */
        (MSDC_READ32(IO_BASE + 0x80) >> 23) & 1);
    dprintf(ALWAYS, "CLK_CG[0x1000_1094] bit4 = %d, bit16 %d\n",
        /* cg at bit 4, 16 */
        (MSDC_READ32(IO_BASE + 0x94) >> 4) & 1,
        (MSDC_READ32(IO_BASE + 0x94) >> 16) & 1);
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
#if !defined(FPGA_PLATFORM)
    msdc_dump_clock_sts(host);
#endif
}

/* perloader will pre-set msdc pll and the mux channel of msdc pll */
/* note: pll will not changed */
void msdc_config_clksrc(struct mmc_host *host, int clksrc)
{
#if !defined(FPGA_PLATFORM)
    host->pll_mux_clk = MSDC0_CLKSRC_DEFAULT;
    host->src_clk = msdc_get_hclk(host->id, MSDC0_CLKSRC_DEFAULT);
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
        dprintf(ALWAYS, "MSDC0 MODE0[%x] = 0x%X\tshould: 0x1???????\n",
            MSDC0_GPIO_MODE0, MSDC_READ32(MSDC0_GPIO_MODE0));
        dprintf(ALWAYS, "MSDC0 MODE1[%x] = 0x%X\tshould: 0x11111111\n",
                MSDC0_GPIO_MODE1, MSDC_READ32(MSDC0_GPIO_MODE1));
        dprintf(ALWAYS, "MSDC0 IES   [%x] = 0x%X\tshould: 0x3FFC????\n",
                MSDC0_GPIO_IES_ADDR, MSDC_READ32(MSDC0_GPIO_IES_ADDR));
        dprintf(ALWAYS, "MSDC0 SMT   [%x] = 0x%X\tshould: 0x??????7C\n",
                MSDC0_GPIO_SMT_ADDR, MSDC_READ32(MSDC0_GPIO_SMT_ADDR));
        dprintf(ALWAYS, "MSDC0 TDSEL0[%x] = 0x%X\tshould: 0x????0000\n",
                MSDC0_GPIO_TDSEL0_ADDR,
                MSDC_READ32(MSDC0_GPIO_TDSEL0_ADDR));
        dprintf(ALWAYS, "MSDC0 TDSEL1[%x] = 0x%X\tshould: 0x????0000\n",
                MSDC0_GPIO_TDSEL1_ADDR,
                MSDC_READ32(MSDC0_GPIO_TDSEL1_ADDR));
        dprintf(ALWAYS, "MSDC0 RDSEL0[%x] = 0x%X\tshould: 0x00000000\n",
                MSDC0_GPIO_RDSEL0_ADDR,
                MSDC_READ32(MSDC0_GPIO_RDSEL0_ADDR));
        dprintf(ALWAYS, "MSDC0 RDSEL1[%x] = 0x%X\tshould: 0x00000000\n",
                MSDC0_GPIO_RDSEL1_ADDR,
                MSDC_READ32(MSDC0_GPIO_RDSEL1_ADDR));
        dprintf(ALWAYS, "MSDC0 RDSEL2[%x] = 0x%X\tshould: 0x00000000\n",
                MSDC0_GPIO_RDSEL2_ADDR,
                MSDC_READ32(MSDC0_GPIO_RDSEL2_ADDR));
        dprintf(ALWAYS, "MSDC0 DRV0  [%x] = 0x%X\tshould: 0x?9249249\n",
                MSDC0_GPIO_DRV0_ADDR,
                MSDC_READ32(MSDC0_GPIO_DRV0_ADDR));
        dprintf(ALWAYS, "MSDC0 DRV1  [%x] = 0x%X\tshould: 0x???????9\n",
            MSDC0_GPIO_DRV1_ADDR,
            MSDC_READ32(MSDC0_GPIO_DRV1_ADDR));
        dprintf(ALWAYS, "PUPD/R1/R0: dat/cmd:0/0/1, clk/dst: 1/1/0\n");
        dprintf(ALWAYS, "MSDC0 PUPD0 [%x] = 0x%X\tshould: 0x?????401\n",
            MSDC0_GPIO_PUPD0_ADDR,
            MSDC_READ32(MSDC0_GPIO_PUPD0_ADDR));
        dprintf(ALWAYS, "MSDC0 R0 [%x] = 0x%X\tshould: 0x?????3FE\n",
            MSDC0_GPIO_R0_ADDR,
            MSDC_READ32(MSDC0_GPIO_R0_ADDR));
        dprintf(ALWAYS, "MSDC0 R1 [%x] = 0x%X\tshould: 0x?????401\n",
            MSDC0_GPIO_R1_ADDR,
            MSDC_READ32(MSDC0_GPIO_R1_ADDR));
#if (MSDC_MAX_NUM > 1)
    } else if (id == 1) {
        dprintf(ALWAYS, "MSDC1 MODE16 [%x] = 0x%X\tshould: 0x11??????\n",
            MSDC1_GPIO_MODE16, MSDC_READ32(MSDC1_GPIO_MODE16));
        dprintf(ALWAYS, "MSDC1 MODE17 [%x] = 0x%X\tshould: 0x????1111\n",
            MSDC1_GPIO_MODE17, MSDC_READ32(MSDC1_GPIO_MODE17));
        dprintf(ALWAYS, "MSDC1 IES    [%x] = 0x%X\tshould: 0x??????FC\n",
            MSDC1_GPIO_IES_ADDR, MSDC_READ32(MSDC1_GPIO_IES_ADDR));
        dprintf(ALWAYS, "MSDC1 SMT    [%x] = 0x%X\tshould: 0x??????1C\n",
            MSDC1_GPIO_SMT_ADDR, MSDC_READ32(MSDC1_GPIO_SMT_ADDR));
        dprintf(ALWAYS, "MSDC1 TDSEL0 [%x] = 0x%X\tshould: 0x????000?\n",
            MSDC1_GPIO_TDSEL0_ADDR,
            MSDC_READ32(MSDC1_GPIO_TDSEL0_ADDR));
        dprintf(ALWAYS, "should 1.8v: sleep: TBD, awake: TBD\n");
        dprintf(ALWAYS, "MSDC1 RDSEL0 [%x] = 0x%X\tshould: 0x???0000?\n",
            MSDC1_GPIO_RDSEL0_ADDR,
            MSDC_READ32(MSDC1_GPIO_RDSEL0_ADDR));
        dprintf(ALWAYS, "1.8V: TBD, 2.9v: TBD\n");
        dprintf(ALWAYS, "MSDC1 DRV0   [%x] = 0x%X\tshould: 0x?????92?\n",
            MSDC1_GPIO_DRV0_ADDR,
            MSDC_READ32(MSDC1_GPIO_DRV0_ADDR));
        dprintf(ALWAYS, "MSDC1 PUPD0  [%x] = 0x%X\tshould: 0x??????01\n",
            MSDC1_GPIO_PUPD0_ADDR,
            MSDC_READ32(MSDC1_GPIO_PUPD0_ADDR));
        dprintf(ALWAYS, "MSDC1 R0  [%x] = 0x%X\tshould: 0x??????00\n",
            MSDC1_GPIO_R0_ADDR,
            MSDC_READ32(MSDC1_GPIO_R0_ADDR));
        dprintf(ALWAYS, "MSDC1 R1  [%x] = 0x%X\tshould: 0x??????3F\n",
            MSDC1_GPIO_R1_ADDR,
            MSDC_READ32(MSDC1_GPIO_R1_ADDR));
#endif
    }
#endif
}

void msdc_set_pin_mode(struct mmc_host *host)
{
    if (host->id == 0) {
        MSDC_SET_FIELD(MSDC0_GPIO_MODE0, 0xFFFF0000, 0x1111);
        MSDC_SET_FIELD(MSDC0_GPIO_MODE1, 0xFFFFFFFF, 0x11111111);
#if (MSDC_MAX_NUM > 1)
    } else if (host->id == 1) {
        MSDC_SET_FIELD(MSDC1_GPIO_MODE0, 0xFF000000, 0x11);
        MSDC_SET_FIELD(MSDC1_GPIO_MODE1, 0x0000FFFF, 0x1111);
#endif
    }
}

void msdc_set_ies_by_id(u32 id, int set_ies)
{
    if (id == 0) {
        MSDC_SET_FIELD(MSDC0_GPIO_IES_ADDR, MSDC0_IES_ALL_MASK,
            (set_ies ? 0xFFF : 0));
#if (MSDC_MAX_NUM > 1)
    } else if (id == 1) {
        MSDC_SET_FIELD(MSDC1_GPIO_IES_ADDR, MSDC1_IES_ALL_MASK,
            (set_ies ? 0x3F : 0));
#endif
    }
}

void msdc_set_smt_by_id(u32 id, int set_smt)
{
    if (id == 0) {
        MSDC_SET_FIELD(MSDC0_GPIO_SMT_ADDR, MSDC0_SMT_ALL_MASK,
            (set_smt ? 0xFFF : 0));
#if (MSDC_MAX_NUM > 1)
    } else if (id == 1) {
        MSDC_SET_FIELD(MSDC1_GPIO_SMT_ADDR, MSDC1_SMT_ALL_MASK,
            (set_smt ? 0x7 : 0));
#endif
    }
}

void msdc_set_tdsel_by_id(u32 id, u32 flag, u32 value)
{
    u32 cust_val;

    if (id == 0) {
        if (flag == MSDC_TDRDSEL_CUST)
            cust_val = value;
        else
            cust_val = 0;
        MSDC_SET_FIELD(MSDC0_GPIO_TDSEL0_ADDR, MSDC0_TDSEL0_CMD_MASK,
                   cust_val);
        MSDC_SET_FIELD(MSDC0_GPIO_TDSEL0_ADDR, MSDC0_TDSEL0_DAT0_MASK,
                   cust_val);
        MSDC_SET_FIELD(MSDC0_GPIO_TDSEL0_ADDR, MSDC0_TDSEL0_DAT1_MASK,
                   cust_val);
        MSDC_SET_FIELD(MSDC0_GPIO_TDSEL0_ADDR, MSDC0_TDSEL0_DAT2_MASK,
                   cust_val);
        MSDC_SET_FIELD(MSDC0_GPIO_TDSEL0_ADDR, MSDC0_TDSEL0_DAT3_MASK,
                   cust_val);
        MSDC_SET_FIELD(MSDC0_GPIO_TDSEL0_ADDR, MSDC0_TDSEL0_DAT4_MASK,
                   cust_val);
        MSDC_SET_FIELD(MSDC0_GPIO_TDSEL0_ADDR, MSDC0_TDSEL0_DAT5_MASK,
                   cust_val);
        MSDC_SET_FIELD(MSDC0_GPIO_TDSEL1_ADDR, MSDC0_TDSEL1_DAT6_MASK,
                   cust_val);
        MSDC_SET_FIELD(MSDC0_GPIO_TDSEL1_ADDR, MSDC0_TDSEL1_DAT7_MASK,
                   cust_val);
        MSDC_SET_FIELD(MSDC0_GPIO_TDSEL0_ADDR, MSDC0_TDSEL0_CLK_MASK,
                   cust_val);
        MSDC_SET_FIELD(MSDC0_GPIO_TDSEL1_ADDR, MSDC0_TDSEL1_DSL_MASK,
                   cust_val);
#if (MSDC_MAX_NUM > 1)
    } else if (id == 1) {
        if (flag == MSDC_TDRDSEL_CUST)
            cust_val = value;
        else
            cust_val = 0;
        MSDC_SET_FIELD(MSDC1_GPIO_TDSEL0_ADDR, MSDC1_TDSEL0_DAT_MASK,
                   cust_val);
        MSDC_SET_FIELD(MSDC1_GPIO_TDSEL0_ADDR, MSDC1_TDSEL0_CMD_MASK,
                   cust_val);
        MSDC_SET_FIELD(MSDC1_GPIO_TDSEL0_ADDR, MSDC1_TDSEL0_CLK_MASK,
                   cust_val);
#endif
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
        MSDC_SET_FIELD(MSDC0_GPIO_RDSEL0_ADDR, MSDC0_RDSEL0_CLK_MASK,
                   cust_val);
        MSDC_SET_FIELD(MSDC0_GPIO_RDSEL0_ADDR, MSDC0_RDSEL0_CMD_MASK,
                   cust_val);
        MSDC_SET_FIELD(MSDC0_GPIO_RDSEL0_ADDR, MSDC0_RDSEL0_DAT0_MASK,
                   cust_val);
        MSDC_SET_FIELD(MSDC0_GPIO_RDSEL0_ADDR, MSDC0_RDSEL0_DAT1_MASK,
                   cust_val);
        MSDC_SET_FIELD(MSDC0_GPIO_RDSEL0_ADDR, MSDC0_RDSEL0_DAT2_MASK,
                   cust_val);
        MSDC_SET_FIELD(MSDC0_GPIO_RDSEL1_ADDR, MSDC0_RDSEL1_DAT3_MASK,
                   cust_val);
        MSDC_SET_FIELD(MSDC0_GPIO_RDSEL1_ADDR, MSDC0_RDSEL1_DAT4_MASK,
                   cust_val);
        MSDC_SET_FIELD(MSDC0_GPIO_RDSEL1_ADDR, MSDC0_RDSEL1_DAT5_MASK,
                   cust_val);
        MSDC_SET_FIELD(MSDC0_GPIO_RDSEL1_ADDR, MSDC0_RDSEL1_DAT6_MASK,
                   cust_val);
        MSDC_SET_FIELD(MSDC0_GPIO_RDSEL1_ADDR, MSDC0_RDSEL1_DAT7_MASK,
                   cust_val);
        MSDC_SET_FIELD(MSDC0_GPIO_RDSEL2_ADDR, MSDC0_RDSEL2_DSL_MASK,
                   cust_val);
        MSDC_SET_FIELD(MSDC0_GPIO_RDSEL2_ADDR, MSDC0_RDSEL2_RSTB_MASK,
                   cust_val);
#if (MSDC_MAX_NUM > 1)
    } else if (id == 1) {
        if (flag == MSDC_TDRDSEL_CUST)
            cust_val = value;
        else
            cust_val = 0;
        MSDC_SET_FIELD(MSDC1_GPIO_RDSEL0_ADDR, MSDC1_RDSEL0_CMD_MASK,
                   cust_val);
        MSDC_SET_FIELD(MSDC1_GPIO_RDSEL0_ADDR, MSDC1_RDSEL0_DAT_MASK,
                   cust_val);
        MSDC_SET_FIELD(MSDC1_GPIO_RDSEL0_ADDR, MSDC1_RDSEL0_CLK_MASK,
                   cust_val);
#endif
    }
}

void msdc_get_tdsel_by_id(u32 id, u32 *value)
{
    if (id == 0) {
        MSDC_GET_FIELD(MSDC0_GPIO_TDSEL0_ADDR, MSDC0_TDSEL0_CMD_MASK,
                   *value);
#if (MSDC_MAX_NUM > 1)
    } else if (id == 1) {
        MSDC_GET_FIELD(MSDC1_GPIO_TDSEL0_ADDR, MSDC1_TDSEL0_CMD_MASK,
                   *value);
#endif
    }
}

void msdc_get_rdsel_by_id(u32 id, u32 *value)
{
    if (id == 0) {
        MSDC_GET_FIELD(MSDC0_GPIO_RDSEL0_ADDR, MSDC0_RDSEL0_CMD_MASK,
                   *value);
#if (MSDC_MAX_NUM > 1)
    } else if (id == 1) {
        MSDC_GET_FIELD(MSDC1_GPIO_RDSEL0_ADDR, MSDC1_RDSEL0_CMD_MASK,
                   *value);
#endif
    }
}

void msdc_set_sr_by_id(u32 id, int clk, int cmd, int dat, int rst, int ds)
{
    /*SR not supported*/
    return;
}

void msdc_set_driving_by_id(u32 id, struct msdc_cust *msdc_cap, bool sd_18)
{
    if (id == 0) {
        MSDC_SET_FIELD(MSDC0_GPIO_DRV1_ADDR, MSDC0_DRV1_DSL_MASK,
                   msdc_cap->ds_drv);
        MSDC_SET_FIELD(MSDC0_GPIO_DRV1_ADDR, MSDC0_DRV1_RSTB_MASK,
                   msdc_cap->rst_drv);
        MSDC_SET_FIELD(MSDC0_GPIO_DRV0_ADDR, MSDC0_DRV0_DAT0_MASK,
                   msdc_cap->dat_drv);
        MSDC_SET_FIELD(MSDC0_GPIO_DRV0_ADDR, MSDC0_DRV0_DAT1_MASK,
                   msdc_cap->dat_drv);
        MSDC_SET_FIELD(MSDC0_GPIO_DRV0_ADDR, MSDC0_DRV0_DAT2_MASK,
                   msdc_cap->dat_drv);
        MSDC_SET_FIELD(MSDC0_GPIO_DRV0_ADDR, MSDC0_DRV0_DAT3_MASK,
                   msdc_cap->dat_drv);
        MSDC_SET_FIELD(MSDC0_GPIO_DRV0_ADDR, MSDC0_DRV0_DAT4_MASK,
                   msdc_cap->dat_drv);
        MSDC_SET_FIELD(MSDC0_GPIO_DRV0_ADDR, MSDC0_DRV0_DAT5_MASK,
                   msdc_cap->dat_drv);
        MSDC_SET_FIELD(MSDC0_GPIO_DRV0_ADDR, MSDC0_DRV0_DAT6_MASK,
                   msdc_cap->dat_drv);
        MSDC_SET_FIELD(MSDC0_GPIO_DRV0_ADDR, MSDC0_DRV0_DAT7_MASK,
                   msdc_cap->dat_drv);
        MSDC_SET_FIELD(MSDC0_GPIO_DRV0_ADDR, MSDC0_DRV0_CMD_MASK,
                   msdc_cap->cmd_drv);
        MSDC_SET_FIELD(MSDC0_GPIO_DRV0_ADDR, MSDC0_DRV0_CLK_MASK,
                   msdc_cap->clk_drv);
#if (MSDC_MAX_NUM > 1)
    } else if (id == 1) {
        MSDC_SET_FIELD(MSDC1_GPIO_DRV0_ADDR, MSDC1_DRV0_CMD_MASK,
                   msdc_cap->cmd_drv);
        MSDC_SET_FIELD(MSDC1_GPIO_DRV0_ADDR, MSDC1_DRV0_CLK_MASK,
                   msdc_cap->clk_drv);
        MSDC_SET_FIELD(MSDC1_GPIO_DRV0_ADDR, MSDC1_DRV0_DAT_MASK,
                   msdc_cap->dat_drv);
#endif
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
            MSDC_SET_FIELD(MSDC0_GPIO_PUPD0_ADDR, MSDC0_PUPD_ALL_MASK, 0x0);
            MSDC_SET_FIELD(MSDC0_GPIO_R0_ADDR, MSDC0_R0_ALL_MASK, 0x0);
            MSDC_SET_FIELD(MSDC0_GPIO_R1_ADDR, MSDC0_R1_ALL_MASK, 0x0);
        } else if (mode == MSDC_PIN_PULL_DOWN) {
            /* Switch MSDC0_* to 50K ohm PD */
            MSDC_SET_FIELD(MSDC0_GPIO_PUPD0_ADDR, MSDC0_PUPD_ALL_MASK, 0x7FF);
            MSDC_SET_FIELD(MSDC0_GPIO_R0_ADDR, MSDC0_R0_ALL_MASK, 0x0);
            MSDC_SET_FIELD(MSDC0_GPIO_R1_ADDR, MSDC0_R1_ALL_MASK, 0x7FF);
        } else if (mode == MSDC_PIN_PULL_UP) {
            /* Switch MSDC0_CLK to 50K ohm PD,
             * MSDC0_CMD/MSDC0_DAT* to 10K ohm PU,
             * MSDC0_DSL to 50K ohm PD
             */
            MSDC_SET_FIELD(MSDC0_GPIO_PUPD0_ADDR, MSDC0_PUPD_ALL_MASK, 0x401);
            MSDC_SET_FIELD(MSDC0_GPIO_R0_ADDR, MSDC0_R0_ALL_MASK, 0x3FE);
            MSDC_SET_FIELD(MSDC0_GPIO_R1_ADDR, MSDC0_R1_ALL_MASK, 0x401);
        }
#if (MSDC_MAX_NUM > 1)
    } else if (id == 1) {
        if (mode == MSDC_PIN_PULL_NONE) {
            /* Switch MSDC1_* to no ohm PU */
            MSDC_SET_FIELD(MSDC1_GPIO_PUPD0_ADDR, MSDC1_PUPD_ALL_MASK, 0x0);
            MSDC_SET_FIELD(MSDC1_GPIO_R0_ADDR, MSDC1_R0_ALL_MASK, 0x0);
            MSDC_SET_FIELD(MSDC1_GPIO_R1_ADDR, MSDC1_R1_ALL_MASK, 0x0);
        } else if (mode == MSDC_PIN_PULL_DOWN) {
            /* Switch MSDC1_* to 50K ohm PD */
            MSDC_SET_FIELD(MSDC1_GPIO_PUPD0_ADDR, MSDC1_PUPD_ALL_MASK, 0x3F);
            MSDC_SET_FIELD(MSDC1_GPIO_R0_ADDR, MSDC1_R0_ALL_MASK, 0x0);
            MSDC_SET_FIELD(MSDC1_GPIO_R1_ADDR, MSDC1_R1_ALL_MASK, 0x3F);
        } else if (mode == MSDC_PIN_PULL_UP) {
            /*
             * Switch MSDC1_CLK to 50K ohm PD,
             * MSDC1_CMD/MSDC1_DAT* to 50K ohm PU
             */
            MSDC_SET_FIELD(MSDC1_GPIO_PUPD0_ADDR, MSDC1_PUPD_ALL_MASK, 0x1);
            MSDC_SET_FIELD(MSDC1_GPIO_R0_ADDR, MSDC1_R0_ALL_MASK, 0x0);
            MSDC_SET_FIELD(MSDC1_GPIO_R1_ADDR, MSDC1_R1_ALL_MASK, 0x3F);
        }
#endif
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
    //msdc_set_ies(host, 1);

    /* set driving */
    msdc_set_driving(host, &msdc_cap, (host->cur_pwr == VOL_1800));

    /* set tdsel and rdsel */
    //msdc_set_tdsel_wrap(host);
    //msdc_set_rdsel_wrap(host);

    //msdc_dump_padctl_by_id(host->id);
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
    MSDC_WRITE32(TOP_EMMC50_PAD_DS_TUNE, 0x14814); /* Apply 2731 DS delay setting */
}

/* Efuse to decide 8bit or 1bit booting */
int EFUSE_IS_EMMC_BOOT_BUS_WIDTH_8BIT(void)
{
    return 1;
}
#endif
