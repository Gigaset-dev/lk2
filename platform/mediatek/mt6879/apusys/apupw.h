/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#pragma once
#include <platform/addressmap.h>
#include <platform/reg.h>

enum t_acx_id {
    ACX0 = 0,
    ACX1,
    CLUSTER_NUM,
    RCX,
};

enum apupw_reg {
    sys_vlp,
    sys_spm,
    apu_rcx,
    apu_vcore,
    apu_md32_mbox,
    apu_rpc,
    apu_pcu,
    apu_ao_ctl,
    apu_pll,
    apu_acc,
    apu_are0,
    apu_are1,
    apu_are2,
    apu_acx0,
    apu_acx0_rpc_lite,
    apu_acx1,
    apu_acx1_rpc_lite,
    APUPW_MAX_REGS,
};

struct apu_power {
    void *regs[APUPW_MAX_REGS];
    unsigned int phy_addr[APUPW_MAX_REGS];
};

#define SYS_VLP                 0x000000 // sys_vlp
#define SYS_SPM                 0x000000 // sys_spm
#define APU_RCX                 0x020000 // apu_rcx
#define APU_VCORE               0x0e0000 // apu_vcore
#define APU_MD32_MBOX           0x0e1000 // apu_md32_mbox 0
#define APU_RPC                 0x0f0000 // apu_rpc
#define APU_PCU                 0x0f1000 // apu_pcu
#define APU_AO_CTL              0x0f2000 // apu_ao_ctl
#define APU_PLL                 0x0f3000 // apu_pll
#define APU_ACC                 0x0f4000 // apu_acc
#define APU_ARE0                0x0f6000 // apu_are0
#define APU_ARE1                0x0f7000 // apu_are1
#define APU_ARE2                0x0f8000 // apu_are2
#define APU_ACX0                0x100000 // apu_acx0
#define APU_ACX0_RPC_LITE       0x140000 // apu_acx0_rpc_lite
#define APU_ACX1                0x200000 // apu_acx1
#define APU_ACX1_RPC_LITE       0x240000 // apu_acx1_rpc_lite

/* RPC offset define */
#define APU_RPC_TOP_CON           0x0000
#define APU_RPC_TOP_SEL           0x0004
#define APU_RPC_SW_FIFO_WE        0x0008
#define APU_RPC_IO_DEBUG          0x000C
#define APU_RPC_STATUS            0x0014
#define APU_RPC_TOP_SEL_1         0x0018
#define APU_RPC_HW_CON            0x001C
#define APU_RPC_LITE_CON          0x0020
#define APU_RPC_INTF_PWR_RDY_REG  0x0040
#define APU_RPC_INTF_PWR_RDY      0x0044

/* APU PLL4H offset define */
#define PLL4H_GRP0_CON    0x004
#define PLL4H_PLL1_CON0    0x008    //APUPLL; MDLA use
#define PLL4H_PLL1_CON1    0x00C
#define PLL4H_PLL1_CON3    0x014
#define PLL4H_PLL2_CON0    0x018    //NPUPLL; MVUP use
#define PLL4H_PLL2_CON1    0x01C
#define PLL4H_PLL2_CON3    0x024
#define PLL4H_PLL3_CON0    0x028    //APUPLL1; MNOC, uP use
#define PLL4H_PLL3_CON1    0x02C
#define PLL4H_PLL3_CON3    0x034
#define PLL4H_PLL4_CON0    0x038    //APUPLL2; parking use only
#define PLL4H_PLL4_CON1    0x03C
#define PLL4H_PLL4_CON3    0x044
#define PLL4H_FQMTR_CON0    0x200
#define PLL4H_FQMTR_CON1    0x204
#define PLL4HPLL_FHCTL_HP_EN    0xE00
#define PLL4HPLL_FHCTL_UNITSLOPE_EN    0xE04
#define PLL4HPLL_FHCTL_CLK_CON    0xE08
#define PLL4HPLL_FHCTL_RST_CON    0xE0C
#define PLL4HPLL_FHCTL_SLOPE0    0xE10
#define PLL4HPLL_FHCTL_SLOPE1    0xE14
#define PLL4HPLL_FHCTL_DSSC_CFG    0xE18
#define PLL4HPLL_FHCTL_DSSC0_CON    0xE1C
#define PLL4HPLL_FHCTL_DSSC1_CON    0xE20
#define PLL4HPLL_FHCTL_DSSC2_CON    0xE24
#define PLL4HPLL_FHCTL_DSSC3_CON    0xE28
#define PLL4HPLL_FHCTL_DSSC4_CON    0xE2C
#define PLL4HPLL_FHCTL_DSSC5_CON    0xE30
#define PLL4HPLL_FHCTL_DSSC6_CON    0xE34
#define PLL4HPLL_FHCTL_DSSC7_CON    0xE38
#define PLL4HPLL_FHCTL0_CFG    0xE3C
#define PLL4HPLL_FHCTL0_UPDNLMT    0xE40
#define PLL4HPLL_FHCTL0_DDS    0xE44
#define PLL4HPLL_FHCTL0_DVFS    0xE48
#define PLL4HPLL_FHCTL0_MON    0xE4C
#define PLL4HPLL_FHCTL1_CFG    0xE50
#define PLL4HPLL_FHCTL1_UPDNLMT    0xE54
#define PLL4HPLL_FHCTL1_DDS    0xE58
#define PLL4HPLL_FHCTL1_DVFS    0xE5C
#define PLL4HPLL_FHCTL1_MON    0xE60
#define PLL4HPLL_FHCTL2_CFG    0xE64
#define PLL4HPLL_FHCTL2_UPDNLMT    0xE68
#define PLL4HPLL_FHCTL2_DDS    0xE6C
#define PLL4HPLL_FHCTL2_DVFS    0xE70
#define PLL4HPLL_FHCTL2_MON    0xE74
#define PLL4HPLL_FHCTL3_CFG    0xE78
#define PLL4HPLL_FHCTL3_UPDNLMT    0xE7C
#define PLL4HPLL_FHCTL3_DDS    0xE80
#define PLL4HPLL_FHCTL3_DVFS    0xE84
#define PLL4HPLL_FHCTL3_MON    0xE88

/* ACC offset define */
#define APU_ACC_CONFG_SET0      0x0000
#define APU_ACC_CONFG_SET1      0x0004
#define APU_ACC_CONFG_SET2      0x0008
#define APU_ACC_CONFG_SET3      0x000C
#define APU_ACC_CONFG_CLR0      0x0040
#define APU_ACC_CONFG_CLR1      0x0044
#define APU_ACC_CONFG_CLR2      0x0048
#define APU_ACC_CONFG_CLR3      0x004C
#define APU_ACC_FM_CONFG_SET    0x00C0
#define APU_ACC_FM_CONFG_CLR    0x00C4
#define APU_ACC_FM_SEL          0x00C8
#define APU_ACC_FM_CNT          0x00CC
#define APU_ACC_CLK_EN_SET      0x00E0
#define APU_ACC_CLK_EN_CLR      0x00E4
#define APU_ACC_CLK_INV_EN_SET  0x00E8
#define APU_ACC_CLK_INV_EN_CLR  0x00EC
#define APU_ACC_AUTO_CONFG0     0x0100
#define APU_ACC_AUTO_CONFG1     0x0104
#define APU_ACC_AUTO_CONFG2     0x0108
#define APU_ACC_AUTO_CONFG3     0x010C
#define APU_ACC_AUTO_CTRL_SET0  0x0120
#define APU_ACC_AUTO_CTRL_SET1  0x0124
#define APU_ACC_AUTO_CTRL_SET2  0x0128
#define APU_ACC_AUTO_CTRL_SET3  0x012C
#define APU_ACC_AUTO_CTRL_CLR0  0x0140
#define APU_ACC_AUTO_CTRL_CLR1  0x0144
#define APU_ACC_AUTO_CTRL_CLR2  0x0148
#define APU_ACC_AUTO_CTRL_CLR3  0x014C
#define APU_ACC_AUTO_STATUS0    0x0160
#define APU_ACC_AUTO_STATUS1    0x0164
#define APU_ACC_AUTO_STATUS2    0x0168

#define APU_ACC_AUTO_STATUS3    0x016C

/* ARE offset define */
#define APU_ARE_INI_CTRL        0x0000
#define APU_ARE_SRAM_CON        0x0004
#define APU_ARE_CONFG0          0x0040
#define APU_ARE_CONFG1          0x0044
#define APU_ARE_GLO_FSM         0x0048
#define APU_ARE_APB_FSM         0x004C
#define APU_ARE_ETRY0_SRAM_H    0x0C00
#define APU_ARE_ETRY0_SRAM_L    0x0800
#define APU_ARE_ETRY1_SRAM_H    0x0C04
#define APU_ARE_ETRY1_SRAM_L    0x0804
#define APU_ARE_ETRY2_SRAM_H    0x0C08
#define APU_ARE_ETRY2_SRAM_L    0x0808

/* vcore offset define */
#define APUSYS_VCORE_CG_CON     0x0000
#define APUSYS_VCORE_CG_SET     0x0004
#define APUSYS_VCORE_CG_CLR     0x0008
#define APUSYS_VCORE_SW_RST     0x000C

/* rcx offset define */
#define APU_RCX_CG_CON          0x0000
#define APU_RCX_CG_SET          0x0004
#define APU_RCX_CG_CLR          0x0008
#define APU_RCX_SW_RST          0x000C

/* acx 0/1 offset define */
#define APU_ACX_CONN_CG_CON     0x3C000
#define APU_ACX_CONN_CG_CLR     0x3C008
#define APU_ACX_MVPU_CG_CON     0x2B000
#define APU_ACX_MVPU_CG_CLR     0x2B008
#define APU_ACX_MVPU_SW_RST     0x2B00C
#define APU_ACX_MVPU_RV55_CTRL0 0x2B018
#define APU_ACX_MDLA0_CG_CON    0x30000
#define APU_ACX_MDLA0_CG_CLR    0x30008
#define APU_ACX_MDLA1_CG_CON    0x34000
#define APU_ACX_MDLA1_CG_CLR    0x34008

// vlp offset define
#define APUSYS_AO_CTRL_ADDR (0x200)

// spm offset define
#define APUSYS_BUCK_ISOLATION       (0x39C)

// mbox offset define (for data exchange with remote)
#define SPARE0_MBOX_DUMMY_0_ADDR        0x640   // mbox6_dummy0
#define SPARE0_MBOX_DUMMY_1_ADDR        0x644   // mbox6_dummy1
#define SPARE0_MBOX_DUMMY_2_ADDR        0x648   // mbox6_dummy2
#define SPARE0_MBOX_DUMMY_3_ADDR        0x64C   // mbox6_dummy3
#define SPARE0_MBOX_DUMMY_4_ADDR    0x740   // mbox7_dummy0

// PCU initial data
#define APU_PCUTOP_CTRL_SET     0x0

// PCU initial data
#define TOP_VRCTL_VR0_EN        0x240
#define TOP_VRCTL_VR0_EN_SET    0x241
#define TOP_VRCTL_VR0_EN_CLR    0x242
#define MT6368_PMIC_RG_BUCK_VBUCK3_EN_ADDR    TOP_VRCTL_VR0_EN
#define MT6368_PMIC_RG_BUCK_VBUCK3_EN_ADDR_SET    TOP_VRCTL_VR0_EN_SET
#define MT6368_PMIC_RG_BUCK_VBUCK3_EN_ADDR_CLR    TOP_VRCTL_VR0_EN_CLR
#define MT6368_PMIC_RG_BUCK_VBUCK3_EN_SHIFT    (3)
#define BUCK_VAPU_PMIC_REG_EN_ADDR    MT6368_PMIC_RG_BUCK_VBUCK3_EN_ADDR
#define BUCK_VAPU_PMIC_REG_EN_SET_ADDR    MT6368_PMIC_RG_BUCK_VBUCK3_EN_ADDR_SET
#define BUCK_VAPU_PMIC_REG_EN_CLR_ADDR    MT6368_PMIC_RG_BUCK_VBUCK3_EN_ADDR_CLR
#define BUCK_VAPU_PMIC_REG_EN_SHIFT    MT6368_PMIC_RG_BUCK_VBUCK3_EN_SHIFT
#define MT6363_SLAVE_ID    (0x4)
#define MT6368_SLAVE_ID    (0x5)
#define MAIN_PMIC_ID    MT6363_SLAVE_ID
#define SUB_PMIC_ID    MT6368_SLAVE_ID
#define APU_PCU_BUCK_STEP_SEL    0x0030
#define APU_PCU_BUCK_ON_DAT0_L    0x0080
#define APU_PCU_BUCK_ON_DAT0_H    0x0084
#define APU_PCU_BUCK_OFF_DAT0_L    0x00A0
#define APU_PCU_BUCK_OFF_DAT0_H    0x00A4
#define APU_PCU_BUCK_ON_SLE0    0x00C0
#define VAPU_BUCK_ON_SETTLE_TIME    0x12C    //300us

#define udelay(x)   spin(x)

static inline void apu_writel(const unsigned int val, void *regs)
{
    writel(val, regs);
    /* make sure all the write instructions are done */
    // smp_mb();
}

static inline u32 apu_readl(void *regs)
{
    return readl(regs);
}

static inline void apu_setl(const unsigned int val, void *regs)
{
    apu_writel((readl(regs) | val), regs);
}

static inline void apu_clearl(const unsigned int val, void *regs)
{
    apu_writel((readl(regs) & ~val), regs);
}

void apusys_power_init(void);
