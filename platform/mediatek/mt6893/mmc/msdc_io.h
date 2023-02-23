/*
 *
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#pragma once

#include <msdc.h>

/*******************************************************************************
 *PINMUX and GPIO definition
 ******************************************************************************/

#define MSDC_PIN_PULL_NONE      (0)
#define MSDC_PIN_PULL_DOWN      (1)
#define MSDC_PIN_PULL_UP        (2)
#define MSDC_PIN_KEEP           (3)

/* add pull down/up mode define */
#define MSDC_GPIO_PULL_UP       (0)
#define MSDC_GPIO_PULL_DOWN     (1)

#define MSDC_TDRDSEL_SLEEP      (0)
#define MSDC_TDRDSEL_3V         (1)
#define MSDC_TDRDSEL_1V8        (2)
#define MSDC_TDRDSEL_CUST       (3)

/*--------------------------------------------------------------------------*/
/* MSDC0~3 GPIO and IO Pad Configuration Base                               */
/*--------------------------------------------------------------------------*/
#define MSDC_GPIO_BASE          GPIO_BASE

#define MSDC0_IO_PAD_BASE       (IOCFG_TM_BASE)
#define MSDC1_IO_PAD_BASE       (IOCFG_LM_BASE)

/*--------------------------------------------------------------------------*/
/* MSDC GPIO Related Register                                               */
/*--------------------------------------------------------------------------*/
/* MSDC0 */
#define MSDC0_GPIO_MODE22       (MSDC_GPIO_BASE + 0x460)
#define MSDC0_GPIO_MODE23       (MSDC_GPIO_BASE + 0x470)
#define MSDC0_GPIO_MODE24       (MSDC_GPIO_BASE + 0x480)
#define MSDC0_GPIO_DRV0         (MSDC0_IO_PAD_BASE + 0x00)
#define MSDC0_GPIO_IES          (MSDC0_IO_PAD_BASE + 0x50)
#define MSDC0_GPIO_PUPD0        (MSDC0_IO_PAD_BASE + 0x80)
#define MSDC0_GPIO_R0           (MSDC0_IO_PAD_BASE + 0xA0)
#define MSDC0_GPIO_R1           (MSDC0_IO_PAD_BASE + 0xB0)
#define MSDC0_GPIO_RDSEL0       (MSDC0_IO_PAD_BASE + 0xC0)
#define MSDC0_GPIO_RDSEL0_1     (MSDC0_IO_PAD_BASE + 0xD0)
#define MSDC0_GPIO_SMT          (MSDC0_IO_PAD_BASE + 0xF0)
#define MSDC0_GPIO_TDSEL0       (MSDC0_IO_PAD_BASE + 0x100)

/* MSDC1 */
#define MSDC1_GPIO_MISC         (MSDC_GPIO_BASE + 0x600)
#define MSDC1_GPIO_MODE6        (MSDC_GPIO_BASE + 0x360)
#define MSDC1_GPIO_MODE7        (MSDC_GPIO_BASE + 0x370)
#define MSDC1_GPIO_DRV0         (MSDC1_IO_PAD_BASE + 0x00)
#define MSDC1_GPIO_IES          (MSDC1_IO_PAD_BASE + 0x20)
#define MSDC1_GPIO_PUPD0        (MSDC1_IO_PAD_BASE + 0x40)
#define MSDC1_GPIO_R0           (MSDC1_IO_PAD_BASE + 0x60)
#define MSDC1_GPIO_R1           (MSDC1_IO_PAD_BASE + 0x70)
#define MSDC1_GPIO_RDSEL0       (MSDC1_IO_PAD_BASE + 0x80)
#define MSDC1_GPIO_SMT          (MSDC1_IO_PAD_BASE + 0x90)
#define MSDC1_GPIO_SR           (MSDC1_IO_PAD_BASE + 0xA0)
#define MSDC1_GPIO_TDSEL0       (MSDC1_IO_PAD_BASE + 0xB0)
#define MSDC1_GPIO_TDSEL0_1     (MSDC1_IO_PAD_BASE + 0xC0)

/*
 * MSDC0 GPIO and PAD register and bitfields definition
 */
/* MSDC0_GPIO_MODE22, 001b is msdc mode*/
#define MSDC0_MODE_CMD_MASK     (0x7 << 28)

/* MSDC0_GPIO_MODE23, 001b is msdc mode */
#define MSDC0_MODE_DSL_MASK     (0x7 << 28)
#define MSDC0_MODE_DAT7_MASK    (0x7 << 24)
#define MSDC0_MODE_DAT5_MASK    (0x7 << 20)
#define MSDC0_MODE_DAT1_MASK    (0x7 << 16)
#define MSDC0_MODE_DAT6_MASK    (0x7 << 12)
#define MSDC0_MODE_DAT4_MASK    (0x7 << 8)
#define MSDC0_MODE_DAT2_MASK    (0x7 << 4)
#define MSDC0_MODE_DAT0_MASK    (0x7 << 0)

/* MSDC0_GPIO_MODE24, 001b is msdc mode */
#define MSDC0_MODE_RSTB_MASK    (0x7 << 8)
#define MSDC0_MODE_DAT3_MASK    (0x7 << 4)
#define MSDC0_MODE_CLK_MASK     (0x7 << 0)

/* MSDC0 IES mask */
#define MSDC0_IES_RSTB_MASK     (0x1 << 29)
#define MSDC0_IES_DSL_MASK      (0x1 << 28)
#define MSDC0_IES_DAT_MASK      (0xFF << 20)
#define MSDC0_IES_CMD_MASK      (0x1 << 19)
#define MSDC0_IES_CLK_MASK      (0x1 << 18)
#define MSDC0_IES_ALL_MASK      (0xFFF << 18)
/* MSDC0 SMT mask */
#define MSDC0_SMT_DSL_MASK      (0x1 << 5)
#define MSDC0_SMT_DAT_MASK      (0x1 << 4)
#define MSDC0_SMT_CMD_MASK      (0x1 << 3)
#define MSDC0_SMT_CLK_MASK      (0x1 << 2)
#define MSDC0_SMT_ALL_MASK      (0xF << 2)
/* MSDC0 TDSEL0 mask */
#define MSDC0_TDSEL0_DSL_MASK   (0xF << 24)
#define MSDC0_TDSEL0_DAT_MASK   (0xF << 20)
#define MSDC0_TDSEL0_CMD_MASK   (0xF << 16)
#define MSDC0_TDSEL0_CLK_MASK   (0xF << 12)
#define MSDC0_TDSEL0_ALL_MASK   (0xFFFF << 12)
/* MSDC0 RDSEL0 mask */
#define MSDC0_RDSEL0_RSTB_MASK  (0x3F << 0)
#define MSDC0_RDSEL0_DSL_MASK   (0x3F << 24)
#define MSDC0_RDSEL0_DAT_MASK   (0x3F << 18)
#define MSDC0_RDSEL0_CMD_MASK   (0x3F << 12)
#define MSDC0_RDSEL0_CLK_MASK   (0x3F << 6)
#define MSDC0_RDSEL0_ALL_MASK_0 (0xFFFFFF << 6)
#define MSDC0_RDSEL0_ALL_MASK_1 (0x3F << 0)
/* MSDC0 DRV0 mask */
#define MSDC0_DRV0_DSL_MASK     (0x7 << 15)
#define MSDC0_DRV0_DAT_MASK     (0x7 << 12)
#define MSDC0_DRV0_CMD_MASK     (0x7 << 9)
#define MSDC0_DRV0_CLK_MASK     (0x7 << 6)
#define MSDC0_DRV0_ALL_MASK     (0xFFF << 6)
/* MSDC0 PUPD mask*/
#define MSDC0_PUPD_RSTB_MASK    (0x1  << 11)
#define MSDC0_PUPD_DSL_MASK     (0x1  << 10)
#define MSDC0_PUPD_DAT7_MASK    (0x1  << 9)
#define MSDC0_PUPD_DAT6_MASK    (0x1  << 8)
#define MSDC0_PUPD_DAT5_MASK    (0x1  << 7)
#define MSDC0_PUPD_DAT4_MASK    (0x1  << 6)
#define MSDC0_PUPD_DAT3_MASK    (0x1  << 5)
#define MSDC0_PUPD_DAT2_MASK    (0x1  << 4)
#define MSDC0_PUPD_DAT1_MASK    (0x1  << 3)
#define MSDC0_PUPD_DAT0_MASK    (0x1  << 2)
#define MSDC0_PUPD_CMD_MASK     (0x1  << 1)
#define MSDC0_PUPD_CLK_MASK     (0x1  << 0)
#define MSDC0_PUPD_DAT_MASK     (0xFF << 2)
#define MSDC0_PUPD_ALL_MASK     (0x7FF << 0)
/* MSDC0 R0 mask*/
#define MSDC0_R0_RSTB_MASK      (0x1  << 11)
#define MSDC0_R0_DSL_MASK       (0x1  << 10)
#define MSDC0_R0_DAT7_MASK      (0x1  << 9)
#define MSDC0_R0_DAT6_MASK      (0x1  << 8)
#define MSDC0_R0_DAT5_MASK      (0x1  << 7)
#define MSDC0_R0_DAT4_MASK      (0x1  << 6)
#define MSDC0_R0_DAT3_MASK      (0x1  << 5)
#define MSDC0_R0_DAT2_MASK      (0x1  << 4)
#define MSDC0_R0_DAT1_MASK      (0x1  << 3)
#define MSDC0_R0_DAT0_MASK      (0x1  << 2)
#define MSDC0_R0_CMD_MASK       (0x1  << 1)
#define MSDC0_R0_CLK_MASK       (0x1  << 0)
#define MSDC0_R0_DAT_MASK       (0xFF << 2)
#define MSDC0_R0_ALL_MASK       (0x7FF << 0)
/* MSDC0 R1 mask*/
#define MSDC0_R1_RSTB_MASK      (0x1  << 11)
#define MSDC0_R1_DSL_MASK       (0x1  << 10)
#define MSDC0_R1_DAT7_MASK      (0x1  << 9)
#define MSDC0_R1_DAT6_MASK      (0x1  << 8)
#define MSDC0_R1_DAT5_MASK      (0x1  << 7)
#define MSDC0_R1_DAT4_MASK      (0x1  << 6)
#define MSDC0_R1_DAT3_MASK      (0x1  << 5)
#define MSDC0_R1_DAT2_MASK      (0x1  << 4)
#define MSDC0_R1_DAT1_MASK      (0x1  << 3)
#define MSDC0_R1_DAT0_MASK      (0x1  << 2)
#define MSDC0_R1_CMD_MASK       (0x1  << 1)
#define MSDC0_R1_CLK_MASK       (0x1  << 0)
#define MSDC0_R1_DAT_MASK       (0xFF << 2)
#define MSDC0_R1_ALL_MASK       (0x7FF << 0)

/*
 * MSDC1 GPIO and PAD register and bitfields definition
 */
/* MSDC1_GPIO_MODE6, 0001b is msdc mode */

/* MSDC1_A_PIN_MUX_SEL */
#define MSDC1_PIN_MUX_SEL_MASK_A (0x1 << 9)

/* MSDC1 IES mask*/
#define MSDC1_IES_ALL_MASK       (0x3F << 16)

/* MSDC1 SMT mask*/
#define MSDC1_SMT_ALL_MASK       (0x7 <<  6)

/* MSDC1 TDSEL0 mask*/
#define MSDC1_TDSEL0_DAT_MASK    (0xF << 0)
#define MSDC1_TDSEL0_CMD_MASK    (0xF << 28)
#define MSDC1_TDSEL0_CLK_MASK    (0xF << 24)

/* MSDC1 RDSEL0 mask*/
#define MSDC1_RDSEL0_DAT_MASK    (0x3F << 24)
#define MSDC1_RDSEL0_CMD_MASK    (0x3F << 18)
#define MSDC1_RDSEL0_CLK_MASK    (0x3F << 12)
#define MSDC1_RDSEL0_ALL_MASK    (0x3FFFF << 12)
/* MSDC1 DRV0 mask*/
#define MSDC1_DRV0_DAT_MASK      (0x7 << 24)
#define MSDC1_DRV0_CMD_MASK      (0x7 << 21)
#define MSDC1_DRV0_CLK_MASK      (0x7 << 18)
#define MSDC1_DRV0_ALL_MASK      (0x1FF << 18)
/* MSDC1 PUPD mask*/
#define MSDC1_PUPD_DAT3_MASK     (0x1 << 5)
#define MSDC1_PUPD_DAT2_MASK     (0x1 << 4)
#define MSDC1_PUPD_DAT1_MASK     (0x1 << 3)
#define MSDC1_PUPD_DAT0_MASK     (0x1 << 2)
#define MSDC1_PUPD_CMD_MASK      (0x1 << 1)
#define MSDC1_PUPD_CLK_MASK      (0x1 << 0)
#define MSDC1_PUPD_ALL_MASK      (0x3F << 0)
/* MSDC1 R0 mask*/
#define MSDC1_R0_DAT3_MASK       (0x1 << 5)
#define MSDC1_R0_DAT2_MASK       (0x1 << 4)
#define MSDC1_R0_DAT1_MASK       (0x1 << 3)
#define MSDC1_R0_DAT0_MASK       (0x1 << 2)
#define MSDC1_R0_CMD_MASK        (0x1 << 1)
#define MSDC1_R0_CLK_MASK        (0x1 << 0)
#define MSDC1_R0_ALL_MASK        (0x3F << 0)
/* MSDC1 R1 mask*/
#define MSDC1_R1_DAT3_MASK       (0x1 << 5)
#define MSDC1_R1_DAT2_MASK       (0x1 << 4)
#define MSDC1_R1_DAT1_MASK       (0x1 << 3)
#define MSDC1_R1_DAT0_MASK       (0x1 << 2)
#define MSDC1_R1_CMD_MASK        (0x1 << 1)
#define MSDC1_R1_CLK_MASK        (0x1 << 0)
#define MSDC1_R1_ALL_MASK        (0x3F << 0)

/*
 * MSDC Driving Strength Definition: specify as gear instead of driving
 */
#define MSDC_DRVN_GEAR0         0x0
#define MSDC_DRVN_GEAR1         0x1
#define MSDC_DRVN_GEAR2         0x2
#define MSDC_DRVN_GEAR3         0x3
#define MSDC_DRVN_GEAR4         0x4
#define MSDC_DRVN_GEAR5         0x5
#define MSDC_DRVN_GEAR6         0x6
#define MSDC_DRVN_GEAR7         0x7
#define MSDC_DRVN_DONT_CARE     0x0

#define msdc_set_driving(host, msdc_cap) \
    msdc_set_driving_by_id(host->id, msdc_cap)

#define msdc_set_ies(host, set_ies) \
    msdc_set_ies_by_id(host->id, set_ies)

#define msdc_set_sr(host, clk, cmd, dat, rst, ds) \
    msdc_set_sr_by_id(host->id, clk, cmd, dat, rst, ds)

#define msdc_set_smt(host, set_smt) \
    msdc_set_smt_by_id(host->id, set_smt)

#define msdc_set_tdsel(host, flag, value) \
    msdc_set_tdsel_by_id(host->id, flag, value)

#define msdc_set_rdsel(host, flag, value) \
    msdc_set_rdsel_by_id(host->id, flag, value)

#define msdc_get_tdsel(host, value) \
    msdc_get_tdsel_by_id(host->id, value)

#define msdc_get_rdsel(host, value) \
    msdc_get_rdsel_by_id(host->id, value)

#define msdc_dump_padctl(host) \
    msdc_dump_padctl_by_id(host->id)

#define msdc_pin_config(host, mode) \
    msdc_pin_config_by_id(host->id, mode)


/*******************************************************************************
 * CLOCK definition
 ******************************************************************************/

/* each PLL have different gears for select
 * software can used mux interface from clock management module to select
 */
enum {
    MSDC0_CLKSRC_26MHZ  = 0,
    MSDC0_CLKSRC_400MHZ,
    MSDC0_CLKSRC_200MHZ,
    MSDC0_CLKSRC_156MHZ,
    MSDC0_CLKSRC_182MHZ,
    MSDC0_CLKSRC_312MHZ,
    MSDC0_CLKSRC_MAX
};

enum {
    MSDC1_CLKSRC_26MHZ   = 0,
    MSDC1_CLKSRC_208MHZ,
    MSDC1_CLKSRC_182MHZ,
    MSDC1_CLKSRC_156MHZ,
    MSDC1_CLKSRC_200MHZ,
    MSDC1_CLKSRC_MAX
};

#define MSDC0_CLKSRC_DEFAULT    MSDC0_CLKSRC_400MHZ
#define MSDC1_CLKSRC_DEFAULT    MSDC1_CLKSRC_200MHZ
#define MSDC_CLKSRC_DEFAULT(ID) MSDC##ID_CLKSRC_DEFAULT

#define MSDCPLL_FREQ            384000000

#define MSDC0_SRC_0             26000000
#define MSDC0_SRC_1             MSDCPLL_FREQ
#define MSDC0_SRC_2             (MSDCPLL_FREQ/2)
#define MSDC0_SRC_3             156000000
#define MSDC0_SRC_4             182000000
#define MSDC0_SRC_5             312000000

#define MSDC1_SRC_0             26000000
#define MSDC1_SRC_1             208000000
#define MSDC1_SRC_2             182000000
#define MSDC1_SRC_3             156000000
#define MSDC1_SRC_4             (MSDCPLL_FREQ/2)

/*set cg bit*/
#define MSDC0_CLOCK_GATE_REG    (INFRACFG_AO_BASE + 0x088)
#define MSDC1_CLOCK_GATE_REG    (INFRACFG_AO_BASE + 0x088)
/*clr cg bit*/
#define MSDC0_CLOCK_UNGATE_REG  (INFRACFG_AO_BASE + 0x08c)
#define MSDC1_CLOCK_UNGATE_REG  (INFRACFG_AO_BASE + 0x08c)

#define MSDC0_CLOCK_CG          (1 << 2)
#define MSDC0_CLOCK_SRC_CG      (1 << 6)
#define MSDC1_CLOCK_CG          (1 << 4)
#define MSDC1_CLOCK_SRC_CG      (1 << 16)


/*******************************************************************************
 * Power Definition
 ******************************************************************************/
#define REG_VEMC_VOSEL_CAL      PMIC_RG_VEMC_VOCAL_ADDR
#define MASK_VEMC_VOSEL_CAL     PMIC_RG_VEMC_VOCAL_MASK
#define SHIFT_VEMC_VOSEL_CAL    PMIC_RG_VEMC_VOCAL_SHIFT
#define FIELD_VEMC_VOSEL_CAL    (MASK_VEMC_VOSEL_CAL \
                    << SHIFT_VEMC_VOSEL_CAL)

#define REG_VEMC_VOSEL          PMIC_RG_VEMC_VOSEL_ADDR
#define MASK_VEMC_VOSEL         PMIC_RG_VEMC_VOSEL_MASK
#define SHIFT_VEMC_VOSEL        PMIC_RG_VEMC_VOSEL_SHIFT
#define FIELD_VEMC_VOSEL        (MASK_VEMC_VOSEL << SHIFT_VEMC_VOSEL)

#define REG_VEMC_EN             PMIC_RG_LDO_VEMC_EN_ADDR
#define MASK_VEMC_EN            PMIC_RG_LDO_VEMC_EN_MASK
#define SHIFT_VEMC_EN           PMIC_RG_LDO_VEMC_EN_SHIFT
#define FIELD_VEMC_EN           (MASK_VEMC_EN << SHIFT_VEMC_EN)

#define REG_VMC_VOSEL           0x9
#define MASK_VMC_VOSEL          0xF0
#define SHIFT_VMC_VOSEL         4

#define REG_VMC_VOSEL_CAL       0x9
#define MASK_VMC_VOSEL_CAL      0x0F
#define SHIFT_VMC_VOSEL_CAL     0

#define REG_VMC_EN              0x5
#define MASK_VMC_EN             0x40
#define SHIFT_VMC_EN            6

#define MASK_LDO3_EN            0x04
#define SHIFT_LDO3_EN           2

#define REG_VMCH_VOSEL          0xF
#define MASK_VMCH_VOSEL         0x70
#define SHIFT_VMCH_VOSEL        4

#define REG_VMCH_VOSEL_CAL      0xF
#define MASK_VMCH_VOSEL_CAL     0x0F
#define SHIFT_VMCH_VOSEL_CAL    0

#define REG_VMCH_EN             0xB
#define MASK_VMCH_EN            0x40
#define SHIFT_VMCH_EN           6

#define MASK_LDO5_EN            0x04
#define SHIFT_LDO5_EN           2

#define VEMC_VOSEL_CAL_mV(cal)  ((cal >= 0) ? ((cal)/10) : 0)
#define VEMC_VOSEL_2V9          (2)
#define VEMC_VOSEL_3V           (3)
#define VEMC_VOSEL_3V3          (5)
#define VMC_VOSEL_CAL_mV(cal)   ((cal >= 0) ? ((cal)/10) : 0)
#define VMC_VOSEL_1V8           (4) /* Acutally 1V86 */
#define VMC_VOSEL_2V9           (10)
#define VMC_VOSEL_3V            (11)
#define VMC_VOSEL_3V3           (13)
#define VMCH_VOSEL_CAL_mV(cal)  ((cal >= 0) ? ((cal)/10) : 0)
#define VMCH_VOSEL_2V9          (2)
#define VMCH_VOSEL_3V           (3)
#define VMCH_VOSEL_3V3          (5)

#define EMMC_VOL_ACTUAL         VOL_3000
#define SD_VOL_ACTUAL           VOL_3000


/*******************************************************************************
 * Power Definition
 ******************************************************************************/
#define CMD_RSP_TA_CNTR_DEFAULT         0
#define WRDAT_CRCS_TA_CNTR_DEFAULT      0
#define BUSY_MA_DEFAULT                 1

#define CRCSTSENSEL_HS400_DEFAULT       3
#define RESPSTENSEL_HS400_DEFAULT       3
#define CRCSTSENSEL_HS_DEFAULT          1
#define RESPSTENSEL_HS_DEFAULT          1
#define CRCSTSENSEL_FPGA_DEFAULT        0
