/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#include <compiler.h>
#include <debug.h>
#include <err.h>
#include <smc.h>
#include <smc_id_table.h>
#include <spmi_common.h>
#include <trace.h>

#include "apusys_rv.h"
#include "apupw.h"

#define LOCAL_TRACE 0

/* Below reg_name has to 1-1 mapping DTS's name */
static const char *reg_name[APUPW_MAX_REGS] = {
    "sys_vlp", "sys_spm", "apu_rcx", "apu_vcore", "apu_md32_mbox",
    "apu_rpc", "apu_pcu", "apu_ao_ctl", "apu_pll", "apu_acc",
    "apu_are0", "apu_are1", "apu_are2",
    "apu_acx0", "apu_acx0_rpc_lite", "apu_acx1", "apu_acx1_rpc_lite",
};

static const unsigned int reg_addr[APUPW_MAX_REGS] = {
    SYS_VLP, SYS_SPM, APU_RCX, APU_VCORE, APU_MD32_MBOX,
    APU_RPC, APU_PCU, APU_AO_CTL, APU_PLL, APU_ACC,
    APU_ARE0, APU_ARE1, APU_ARE2,
    APU_ACX0, APU_ACX0_RPC_LITE, APU_ACX1, APU_ACX1_RPC_LITE,
};

static struct apu_power apupw;

static void apupw_secure_init(uint32_t op)
{
    struct __smccc_res smccc_res;

    LTRACE_ENTRY;

    __smc_conduit(MTK_SIP_BL_APUSYS_CONTROL, MTK_APUSYS_SMC_OP_APUSYS_PWR_INIT,
        op, 0, 0, 0, 0, 0, &smccc_res);

    LTRACE_EXIT;
}

static void get_pll_pcw(uint32_t clk_rate, uint32_t *r1, uint32_t *r2)
{
    unsigned int fvco = clk_rate;
    unsigned int pcw_val;
    unsigned int postdiv_val = 1;
    unsigned int postdiv_reg = 0;

    while (fvco <= 1500) {
        postdiv_val = postdiv_val << 1;
        postdiv_reg = postdiv_reg + 1;
        fvco = fvco << 1;
    }

    pcw_val = (fvco * 1 << 14) / 26;

    if (postdiv_reg == 0) { //Fvco * 2 with post_divider = 2
        pcw_val = pcw_val * 2;
        postdiv_val = postdiv_val << 1;
        postdiv_reg = postdiv_reg + 1;
    } //Post divider is 1 is not available

    *r1 = postdiv_reg;
    *r2 = pcw_val;
}

static void __apu_pll_init(void)
{
    // need to 1-1 in order mapping to these two array
    uint32_t pll_base_arr[] = {MNOC_PLL_BASE, UP_PLL_BASE,
                               MDLA_PLL_BASE, MVPU_PLL_BASE
                              };
    int32_t pll_freq_out[] = {1480, 1850, 1850, 2040}; // MHz, then div 2
    uint32_t pcw_val, posdiv_val;
    int pll_arr_size = countof(pll_base_arr);
    int pll_idx;

    LTRACE_ENTRY;

    // Step4. Initial PLL setting

    for (pll_idx = 0 ; pll_idx < pll_arr_size ; pll_idx++) {
        // PCW value always from hopping function: ofs 0x100
        apu_setl(0x1 << 0, apupw.regs[apu_pll]
                 + pll_base_arr[pll_idx] + PLL1UPLL_FHCTL_HP_EN);

        // Hopping function reset release: ofs 0x10C
        apu_setl(0x1 << 0, apupw.regs[apu_pll]
                 + pll_base_arr[pll_idx] + PLL1UPLL_FHCTL_RST_CON);

        // Hopping function clock enable: ofs 0x108
        apu_setl(0x1 << 0, apupw.regs[apu_pll]
                 + pll_base_arr[pll_idx] + PLL1UPLL_FHCTL_CLK_CON);

        // Hopping function enable: ofs 0x114
        apu_setl((0x1 << 0) | (0x1 << 2), apupw.regs[apu_pll]
                 + pll_base_arr[pll_idx] + PLL1UPLL_FHCTL0_CFG);

        posdiv_val = 0;
        pcw_val = 0;
        get_pll_pcw(pll_freq_out[pll_idx], &posdiv_val, &pcw_val);

        // POSTDIV: ofs 0x000C , [26:24] RG_PLL_POSDIV
        // 3'b000: /1 , 3'b001: /2 , 3'b010: /4
        // 3'b011: /8 , 3'b100: /16
        apu_clearl(((0x1 << 26) | (0x1 << 25) | (0x1 << 24)),
                   apupw.regs[apu_pll] + pll_base_arr[pll_idx] + PLL1U_PLL1_CON1);

        apu_setl(((0x1 << 31) | (posdiv_val << 24) | pcw_val),
                 apupw.regs[apu_pll] + pll_base_arr[pll_idx] + PLL1U_PLL1_CON1);

        // PCW register: ofs 0x011C
        // [31] FHCTL0_PLL_TGL_ORG
        // [21:0] FHCTL0_PLL_ORG set to PCW value
        apu_writel(((0x1 << 31) | pcw_val),
                   apupw.regs[apu_pll]
                   + pll_base_arr[pll_idx] + PLL1UPLL_FHCTL0_DDS);
    }

    LTRACE_EXIT;
}

static void __apu_acc_init(void)
{
    char buf[32];

    LTRACE_ENTRY;

    // Step6. Initial ACC setting (@ACC)

    /* mnoc clk setting */
    // CGEN_SOC
    apu_writel(0x00000004, apupw.regs[apu_acc] + APU_ACC_CONFG_CLR0);
    // HW_CTRL_EN, SEL_APU_DIV2
    apu_writel(0x00008400, apupw.regs[apu_acc] + APU_ACC_CONFG_SET0);

    /* iommu clk setting */
    // CGEN_SOC
    apu_writel(0x00000004, apupw.regs[apu_acc] + APU_ACC_CONFG_CLR1);
    // HW_CTRL_EN, SEL_APU_DIV2
    apu_writel(0x00008400, apupw.regs[apu_acc] + APU_ACC_CONFG_SET1);

    /* mvpu clk setting */
    // CGEN_SOC
    apu_writel(0x00000004, apupw.regs[apu_acc] + APU_ACC_CONFG_CLR2);
    // HW_CTRL_EN, SEL_APU_DIV2
    apu_writel(0x00008400, apupw.regs[apu_acc] + APU_ACC_CONFG_SET2);
    // CLK_REQ_SW_EN
    apu_writel(0x00000100, apupw.regs[apu_acc] + APU_ACC_AUTO_CTRL_SET2);

    /* mdla clk setting */
    // CGEN_SOC
    apu_writel(0x00000004, apupw.regs[apu_acc] + APU_ACC_CONFG_CLR3);
    // HW_CTRL_EN, SEL_APU_DIV2
    apu_writel(0x00008400, apupw.regs[apu_acc] + APU_ACC_CONFG_SET3);
    // CLK_REQ_SW_EN
    apu_writel(0x00000100, apupw.regs[apu_acc] + APU_ACC_AUTO_CTRL_SET3);

    /* clk invert setting */
    // MVPU1_CLK_INV_EN MVPU3_CLK_INV_EN MVPU5_CLK_INV_EN
    // MDLA1_CLK_INV_EN MDLA3_CLK_INV_EN
    // MDLA5_CLK_INV_EN MDLA7_CLK_INV_EN
    apu_writel(0x0000AAA8, apupw.regs[apu_acc] + APU_ACC_CLK_INV_EN_SET);

    LTRACE_EXIT;
}

static void vapu_buck_off(void)
{
    struct spmi_device *mt6373_dev = NULL;
    uint32_t en_seq_off = (0x1 << BUCK_VAPU_PMIC_REG_EN_SHIFT);
    uint32_t en_seq_ret;

    mt6373_dev = get_spmi_device(SPMI_MASTER_1, SPMI_SLAVE_5);
    if (!mt6373_dev) {
        dprintf(CRITICAL, "%s fail to get spmi device : %d-%d\n",
                __func__, SPMI_MASTER_1, SPMI_SLAVE_5);
        return;
    }

    spmi_ext_register_readl(mt6373_dev,
                            BUCK_VAPU_PMIC_REG_EN_ADDR, &en_seq_ret, 1);
    LTRACEF_LEVEL(1, "%s get ret : 0x%08x %u\n",
            __func__, en_seq_ret,
            (en_seq_ret >> BUCK_VAPU_PMIC_REG_EN_SHIFT) & 0x1);

    spmi_ext_register_writel(mt6373_dev,
                             BUCK_VAPU_PMIC_REG_EN_CLR_ADDR, &en_seq_off, 1);

    spmi_ext_register_readl(mt6373_dev,
                            BUCK_VAPU_PMIC_REG_EN_ADDR, &en_seq_ret, 1);
    LTRACEF_LEVEL(1, "%s get ret : 0x%08x %u\n",
            __func__, en_seq_ret,
            (en_seq_ret >> BUCK_VAPU_PMIC_REG_EN_SHIFT) & 0x1);
}

static void __apu_buck_off_cfg(void)
{
    LTRACE_ENTRY;

    // Step11. Roll back to Buck off stage

    // a. Setup Buck control signal
    //  The following setting need to in order,
    //  and wait 1uS before setup next control signal
    // APU_BUCK_PROT_REQ
    apu_writel(0x00004000, apupw.regs[apu_rpc] + APU_RPC_HW_CON);
    udelay(1);
    // SRAM_AOC_LHENB
    apu_writel(0x00000010, apupw.regs[apu_rpc] + APU_RPC_HW_CON);
    udelay(1);
    // SRAM_AOC_ISO
    apu_writel(0x00000040, apupw.regs[apu_rpc] + APU_RPC_HW_CON);
    udelay(1);
    // APU_BUCK_ELS_EN
    apu_writel(0x00000400, apupw.regs[apu_rpc] + APU_RPC_HW_CON);
    udelay(1);
    // APU_BUCK_RST_B
    apu_writel(0x00002000, apupw.regs[apu_rpc] + APU_RPC_HW_CON);
    udelay(1);

    // b. Manually turn off Buck (by configuring register in PMIC)
    // move to probe last line
    vapu_buck_off();

    // Step12. After APUsys is finished, update the following register to 1,
    //     ARE will use this information to ensure the SRAM in ARE is
    //     trusted or not
    //     apusys_initial_done
    apupw_secure_init(1);

    LTRACE_EXIT;
}

/*
 * low 32-bit data for PMIC control
 *  APU_PCU_PMIC_TAR_BUF1 (or APU_PCU_BUCK_ON_DAT0_L)
 *  [31:16] offset to update
 *  [15:00] data to update
 *
 * high 32-bit data for PMIC control
 *  APU_PCU_PMIC_TAR_BUF2 (or APU_PCU_BUCK_ON_DAT0_H)
 *  [2:0] cmd_op, read:0x3 , write:0x7
 *  [3]: pmifid,
 *  [7:4]: slvid
 *  [8]: bytecnt
 */
static void __apu_pcu_init(void)
{
    uint32_t cmd_op_w = 0x7;
    uint32_t pmif_id = 0x0;
    uint32_t slave_id = SUB_PMIC_ID;
    uint32_t en_set_offset = BUCK_VAPU_PMIC_REG_EN_SET_ADDR;
    uint32_t en_clr_offset = BUCK_VAPU_PMIC_REG_EN_CLR_ADDR;
    uint32_t en_shift = BUCK_VAPU_PMIC_REG_EN_SHIFT;

    LTRACE_ENTRY;

    // auto buck enable
    apu_writel((0x1 << 16), apupw.regs[apu_pcu] + APU_PCUTOP_CTRL_SET);

    // Step1. enable auto buck on/off function of command0
    // [0]: cmd0 enable auto ON, [4]: cmd0 enable auto OFF
    apu_writel(0x11, (void *)(apupw.regs[apu_pcu] + APU_PCU_BUCK_STEP_SEL));

    // Step2. fill-in command0 for vapu auto buck ON
    apu_writel((en_set_offset << 16) | (0x1 << en_shift),
               apupw.regs[apu_pcu] + APU_PCU_BUCK_ON_DAT0_L);
    apu_writel((slave_id << 4) | (pmif_id << 3) | cmd_op_w,
               apupw.regs[apu_pcu] + APU_PCU_BUCK_ON_DAT0_H);

    // APU_PCU_BUCK_ON_DAT0_L=0x02410040
    // APU_PCU_BUCK_ON_DAT0_H=0x00000057

    // Step3. fill-in command0 for vapu auto buck OFF
    apu_writel((en_clr_offset << 16) | (0x1 << en_shift),
               apupw.regs[apu_pcu] + APU_PCU_BUCK_OFF_DAT0_L);
    apu_writel((slave_id << 4) | (pmif_id << 3) | cmd_op_w,
               apupw.regs[apu_pcu] + APU_PCU_BUCK_OFF_DAT0_H);

    // Step4. update buck settle time for vapu by SEL0
    apu_writel(VAPU_BUCK_ON_SETTLE_TIME,
               apupw.regs[apu_pcu] + APU_PCU_BUCK_ON_SLE0);

    LTRACE_EXIT;
}

static void __apu_rpclite_init(void)
{
    uint32_t sleep_type_offset[] = {0x0208, 0x020C, 0x0210, 0x0214,
                                    0x0218, 0x021C, 0x0220, 0x0224
                                   };
    enum apupw_reg rpc_lite_base[CLUSTER_NUM];
    int ofs_arr_size = countof(sleep_type_offset);
    int acx_idx, ofs_idx;

    LTRACE_ENTRY;

    rpc_lite_base[0] = apu_acx0_rpc_lite;

    if (CLUSTER_NUM == 2)
        rpc_lite_base[1] = apu_acx1_rpc_lite;

    for (acx_idx = 0 ; acx_idx < CLUSTER_NUM ; acx_idx++) {
        for (ofs_idx = 0 ; ofs_idx < ofs_arr_size ; ofs_idx++) {
            // Memory setting
            apu_clearl((0x1 << 1),
                       apupw.regs[rpc_lite_base[acx_idx]]
                       + sleep_type_offset[ofs_idx]);
        }

        // Control setting
        apu_setl(0x0000009E, apupw.regs[rpc_lite_base[acx_idx]]
                 + APU_RPC_TOP_SEL);
    }

    LTRACE_EXIT;
}

static void __apu_rpc_init(void)
{
    LTRACE_ENTRY;

    // Step7. RPC: memory types (sleep or PD type)
    // RPC: iTCM in uP need to setup to sleep type
    apu_clearl((0x1 << 1), apupw.regs[apu_rpc] + 0x0200);

    // Step9. RPCtop initial
    // RPC
    apu_setl(0x1800531E, apupw.regs[apu_rpc] + APU_RPC_TOP_SEL);
    // BUCK_PROT_SEL
    apu_setl((0x1 << 20), apupw.regs[apu_rpc] + APU_RPC_TOP_SEL_1);

    LTRACE_EXIT;
}

static int __apu_are_init(void)
{
    u32 tmp = 0;
    int ret = 0;
    int val = 0;
    int are_id = 0;
    int idx = 0;
    uint32_t are_entry2_cfg_h[] = {0x000F0000, 0x000F0000, 0x000F0000};
    uint32_t are_entry2_cfg_l[] = {0x001F0705, 0x001F0707, 0x001F0707};
    int polling_cnt = 0;

    LTRACE_ENTRY;

    // Step10. ARE initial

    /* TINFO="Wait for sare1 fsm to transition to IDLE" */
    while ((apu_readl(apupw.regs[apu_are2] + 0x48) & 0x1) != 0x1) {
        udelay(5);
        if (polling_cnt++ > 20) {
            ret = -1;
            break;
        }
    }

    if (ret) {
        dprintf(CRITICAL, "%s timeout to wait sram1 fsm to idle, ret %d\n",
                __func__, ret);
    }

    for (are_id = apu_are0, idx = 0; are_id <= apu_are2; are_id++, idx++) {

        /* ARE entry 0 initial */
        apu_writel(0x01234567, apupw.regs[are_id]
                   + APU_ARE_ETRY0_SRAM_H);
        apu_writel(0x89ABCDEF, apupw.regs[are_id]
                   + APU_ARE_ETRY0_SRAM_L);

        /* ARE entry 1 initial */
        apu_writel(0xFEDCBA98, apupw.regs[are_id]
                   + APU_ARE_ETRY1_SRAM_H);
        apu_writel(0x76543210, apupw.regs[are_id]
                   + APU_ARE_ETRY1_SRAM_L);

        /* ARE entry 2 initial */
        apu_writel(are_entry2_cfg_h[idx], apupw.regs[are_id]
                   + APU_ARE_ETRY2_SRAM_H);
        apu_writel(are_entry2_cfg_l[idx], apupw.regs[are_id]
                   + APU_ARE_ETRY2_SRAM_L);

        /* dummy read ARE entry2 H/L sram */
        tmp = readl(apupw.regs[are_id] + APU_ARE_ETRY2_SRAM_H);
        tmp = readl(apupw.regs[are_id] + APU_ARE_ETRY2_SRAM_L);

        /* update ARE sram */
        apu_writel(0x00000004, apupw.regs[are_id] + APU_ARE_INI_CTRL);
    }

    LTRACE_EXIT;

    return 0;
}

static void __apu_aoc_init(void)
{
    LTRACE_ENTRY;

    // Step1. Switch APU AOC control signal from SW register to HW path (RPC)
    //        rpc_sram_ctrl_mux_sel
    // Step2. Manually disable Buck els enable @SOC
    //        (disable SW mode to manually control Buck on/off)
    //        vapu_ext_buck_iso
    apupw_secure_init(0);

    // Step3. Roll back to APU Buck on stage
    //  The following setting need to in order
    //  and wait 1uS before setup next control signal
    // APU_BUCK_ELS_EN
    apu_writel(0x00000800, apupw.regs[apu_rpc] + APU_RPC_HW_CON);
    udelay(1);

    // APU_BUCK_RST_B
    apu_writel(0x00001000, apupw.regs[apu_rpc] + APU_RPC_HW_CON);
    udelay(1);

    // APU_BUCK_PROT_REQ
    apu_writel(0x00008000, apupw.regs[apu_rpc] + APU_RPC_HW_CON);
    udelay(1);

    // SRAM_AOC_ISO
    apu_writel(0x00000080, apupw.regs[apu_rpc] + APU_RPC_HW_CON);
    udelay(1);

    LTRACE_EXIT;
}

static int init_hw_setting(void)
{
    __apu_aoc_init();
    __apu_pcu_init();
    __apu_rpc_init();
    __apu_rpclite_init();
    __apu_are_init();
    __apu_pll_init();
    __apu_acc_init();
    __apu_buck_off_cfg();

    return 0;
}

/*
 * init apusys power hw here and ensure the following sequence:
 * apusys_power    - LK_INIT_LEVEL_PLATFORM
 * apusys_devapc   - LK_INIT_LEVEL_APPS - 1
 * spm_devapc      - mt_boot_init
 * vlp_devapc      - mt_boot_init
 */
void apusys_power_init(void)
{
    int idx = 0;

    for (idx = 0; idx < APUPW_MAX_REGS; idx++) {
        apupw.regs[idx] = IO_BASE + 0x09000000 + reg_addr[idx];
        apupw.phy_addr[idx] = 0x10000000 + 0x09000000 + reg_addr[idx];
    }

    init_hw_setting();

    LTRACEF_LEVEL(1, "%s initial done\n", __func__);
}
