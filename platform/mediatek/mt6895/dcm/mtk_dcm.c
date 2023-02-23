/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#include <arch/ops.h>
#include <debug.h>
#include <libfdt.h>
#include <mtk_dcm.h>
#include <mtk_dcm_autogen.h>
#include <platform/addressmap.h>
#include <platform/reg.h>
#include <reg.h>
#include <smc.h>
#include <smc_id_table.h>
#include <stdlib.h>
#include <string.h>
#include <trace.h>

#define LOCAL_TRACE       0

#define late_initcall(a)

#define IOMEM(a) (a)

#define MCUSYS_SET_IN_ATF        0

#define dcm_smc_msg(init_type, res) \
        __smc_conduit(MTK_SIP_KERNEL_DCM, init_type, \
            0, 0, 0, 0, 0, 0, &res)
#define dcm_smc_read_cnt(type) \
        __smc_conduit(MTK_SIP_KERNEL_DCM, type, \
            1, 0, 0, 0, 0, 0, &res)

#define __raw_readl(addr)           read32(addr)
#define reg_read(addr)              __raw_readl(IOMEM(addr))
#define reg_write(addr, val)        writel((val), ((void *)addr))

#define MCUSYS_SMC_WRITE(addr, val) reg_write(addr, val)
#define MCSI_SMC_WRITE(addr, val)   mcsi_reg_write(val, (addr##_PHYS & 0xFFFF))
#define MCSI_SMC_READ(addr)         mcsi_reg_read(addr##_PHYS & 0xFFFF)
#define dcm_smc_msg_send(msg, res)  dcm_smc_msg(msg, res)

#define dcm_err(fmt, args...)       LTRACEF("[DCM]"fmt, ##args)
#define dcm_warn(fmt, args...)      LTRACEF("[DCM]"fmt, ##args)
#define dcm_info(fmt, args...)      LTRACEF("[DCM]"fmt, ##args)
#define dcm_dbg(fmt, args...)       LTRACEF("[DCM]"fmt, ##args)
#define dcm_ver(fmt, args...)       LTRACEF("[DCM]"fmt, ##args)

#define REG_DUMP(addr) dcm_info("%-30s(0x%08lx): 0x%x\n", #addr, addr, reg_read(addr))
#define SECURE_REG_DUMP(addr) dcm_info("%-30s(0x%08x): 0x%08lx\n", \
            #addr, addr, mcsi_reg_read(addr##_PHYS & 0xFFFF))

/** global **/
static short dcm_initiated;
#ifdef CTRL_BIGCORE_DCM_IN_KERNEL
static short dcm_cpu_cluster_stat;
#endif

static unsigned int all_dcm_type = (ARMCORE_DCM_TYPE | MCUSYS_DCM_TYPE
                    | INFRA_DCM_TYPE | BIG_CORE_DCM_TYPE | PERI_DCM_TYPE
                    | VLP_DCM_TYPE | MCUSYS_ACP_DCM_TYPE
                    | MCUSYS_ADB_DCM_TYPE | MCUSYS_BUS_DCM_TYPE
                    | MCUSYS_CBIP_DCM_TYPE | MCUSYS_CORE_DCM_TYPE
                    | MCUSYS_IO_DCM_TYPE | MCUSYS_CPC_PBI_DCM_TYPE
                    | MCUSYS_CPC_TURBO_DCM_TYPE | MCUSYS_STALL_DCM_TYPE
                    | MCUSYS_APB_DCM_TYPE);

#if defined(BUSDVT_ONLY_MD)
static unsigned int init_dcm_type = BUSDVT_DCM_TYPE;
#else
static unsigned int init_dcm_type = (ARMCORE_DCM_TYPE | MCUSYS_DCM_TYPE
                    | INFRA_DCM_TYPE | BIG_CORE_DCM_TYPE | PERI_DCM_TYPE
                    | VLP_DCM_TYPE | MCUSYS_ACP_DCM_TYPE
                    | MCUSYS_ADB_DCM_TYPE | MCUSYS_BUS_DCM_TYPE
                    | MCUSYS_CBIP_DCM_TYPE | MCUSYS_CORE_DCM_TYPE
                    | MCUSYS_IO_DCM_TYPE | MCUSYS_CPC_PBI_DCM_TYPE
                    | MCUSYS_CPC_TURBO_DCM_TYPE | MCUSYS_STALL_DCM_TYPE
                    | MCUSYS_APB_DCM_TYPE);
#endif

#define DCM_NODE "mediatek,mt6895-dcm"

/*****************************************
 * following is implementation per DCM module.
 * 1. per-DCM function is 1-argu with ON/OFF/MODE option.
 *****************************************/

/* TODO: Fix*/
int dcm_topckg(int on)
{
    return 0;
}

void dcm_infracfg_ao_emi_indiv(int on)
{
}

static int dcm_infra_preset(int on)
{
    return 0;
}

int dcm_infra(int on)
{
    dcm_infracfg_ao_aximem_bus_dcm(on);
    dcm_infracfg_ao_infra_bus_dcm(on);
    dcm_infracfg_ao_infra_rx_p2p_dcm(on);
    dcm_infra_ao_bcrm_infra_bus_dcm(on);

    return 0;
}

int dcm_peri(int on)
{
    dcm_peri_ao_bcrm_peri_bus_dcm(on);
    return 0;
}

int dcm_mcusys_acp(int on)
{
    return 0;
}

int dcm_mcusys_adb(int on)
{
    return 0;
}

int dcm_mcusys_bus(int on)
{
    return 0;
}

int dcm_mcusys_cbip(int on)
{
    return 0;
}

int dcm_mcusys_core(int on)
{
    return 0;
}

int dcm_mcusys_io(int on)
{
    return 0;
}

int dcm_mcusys_cpc_pbi(int on)
{
    return 0;
}

int dcm_mcusys_cpc_turbo(int on)
{
    return 0;
}

int dcm_mcusys_stall(int on)
{
    return 0;
}

int dcm_mcusys_apb(int on)
{
    return 0;
}

int dcm_vlp(int on)
{
    dcm_vlp_ao_bcrm_vlp_bus_dcm(on);
    return 0;
}

int dcm_armcore(int on)
{
    dcm_mp_cpusys_top_cpu_pll_div_0_dcm(on);
    dcm_mp_cpusys_top_cpu_pll_div_1_dcm(on);
    dcm_mp_cpusys_top_cpu_pll_div_2_dcm(on);

    return 0;
}

int dcm_mcusys(int on)
{
    dcm_mp_cpusys_top_adb_dcm(on);
    dcm_mp_cpusys_top_apb_dcm(on);
    dcm_mp_cpusys_top_bus_pll_div_dcm(on);
    dcm_mp_cpusys_top_core_stall_dcm(on);
    dcm_mp_cpusys_top_cpubiu_dcm(on);
    dcm_mp_cpusys_top_fcm_stall_dcm(on);
    dcm_mp_cpusys_top_last_cor_idle_dcm(on);
    dcm_mp_cpusys_top_misc_dcm(on);
    dcm_mp_cpusys_top_mp0_qdcm(on);
    dcm_cpccfg_reg_emi_wfifo(on);

    return 0;
}

static int dcm_mcusys_preset(int on)
{
    return 0;
}

int dcm_big_core(int on)
{
    return 0;
}

int dcm_gic_sync(int on)
{
    return 0;
}

int dcm_last_core(int on)
{
    return 0;
}

int dcm_rgu(int on)
{
    return 0;
}

int dcm_dramc_ao(int on)
{
    return 0;
}

int dcm_ddrphy(int on)
{

    return 0;
}

int dcm_emi(int on)
{

    return 0;
}

int dcm_lpdma(int on)
{
    return 0;
}

static int dcm_pwrap(int on)
{
    return 0;
}

static int dcm_mcsi_preset(int on)
{
    return 0;
}

static int dcm_mcsi(int on)
{
    return 0;
}

static int dcm_busdvt(int on)
{
    return 0;
}
/*****************************************************/
struct DCM {
    int current_state;
    int saved_state;
    int disable_refcnt;
    int default_state;
    int (*func)(int on);
    void (*preset_func)(void);
    int typeid;
    const char *name;
};


/*
 * "DoE" relys on this array definition and order. Don't modify/change this array.
 */
static struct DCM dcm_array[NR_DCM_TYPE] = {
    {
     .typeid = ARMCORE_DCM_TYPE,
     .name = "ARMCORE_DCM",
     .func = dcm_armcore,
     .current_state = ARMCORE_DCM_MODE1,
     .default_state = ARMCORE_DCM_MODE1,
     .disable_refcnt = 0,
     },
    {
     .typeid = MCUSYS_DCM_TYPE,
     .name = "MCUSYS_DCM",
     .func = dcm_mcusys,
     .current_state = MCUSYS_DCM_ON,
     .default_state = MCUSYS_DCM_ON,
     .disable_refcnt = 0,
     },
    {
     .typeid = INFRA_DCM_TYPE,
     .name = "INFRA_DCM",
     .func = dcm_infra,
     .current_state = INFRA_DCM_ON,
     .default_state = INFRA_DCM_ON,
     .disable_refcnt = 0,
     },
    {
     .typeid = PERI_DCM_TYPE,
     .name = "PERI_DCM",
     .func = dcm_peri,
     .current_state = PERI_DCM_ON,
     .default_state = PERI_DCM_ON,
     .disable_refcnt = 0,
     },
    {
     .typeid = MCUSYS_ACP_DCM_TYPE,
     .name = "MCU_ACP_DCM",
     .func = dcm_mcusys_acp,
     .current_state = MCUSYS_ACP_DCM_ON,
     .default_state = MCUSYS_ACP_DCM_ON,
     .disable_refcnt = 0,
    },
    {
     .typeid = MCUSYS_ADB_DCM_TYPE,
     .name = "MCU_ADB_DCM",
     .func = dcm_mcusys_adb,
     .current_state = MCUSYS_ADB_DCM_ON,
     .default_state = MCUSYS_ADB_DCM_ON,
     .disable_refcnt = 0,
    },
    {
     .typeid = MCUSYS_BUS_DCM_TYPE,
     .name = "MCU_BUS_DCM",
     .func = dcm_mcusys_bus,
     .current_state = MCUSYS_BUS_DCM_ON,
     .default_state = MCUSYS_BUS_DCM_ON,
     .disable_refcnt = 0,
    },
    {
     .typeid = MCUSYS_CBIP_DCM_TYPE,
     .name = "MCU_CBIP_DCM",
     .func = dcm_mcusys_cbip,
     .current_state = MCUSYS_CBIP_DCM_ON,
     .default_state = MCUSYS_CBIP_DCM_ON,
     .disable_refcnt = 0,
    },
    {
     .typeid = MCUSYS_CORE_DCM_TYPE,
     .name = "MCU_CORE_DCM",
     .func = dcm_mcusys_core,
     .current_state = MCUSYS_CORE_DCM_ON,
     .default_state = MCUSYS_CORE_DCM_ON,
     .disable_refcnt = 0,
    },
    {
     .typeid = MCUSYS_IO_DCM_TYPE,
     .name = "MCU_IO_DCM",
     .func = dcm_mcusys_io,
     .current_state = MCUSYS_IO_DCM_ON,
     .default_state = MCUSYS_IO_DCM_ON,
     .disable_refcnt = 0,
    },
    {
     .typeid = MCUSYS_CPC_PBI_DCM_TYPE,
     .name = "MCU_CPC_PBI_DCM",
     .func = dcm_mcusys_cpc_pbi,
     .current_state = MCUSYS_CPC_PBI_DCM_ON,
     .default_state = MCUSYS_CPC_PBI_DCM_ON,
     .disable_refcnt = 0,
    },
    {
     .typeid = MCUSYS_CPC_TURBO_DCM_TYPE,
     .name = "MCU_CPC_TURBO_DCM",
     .func = dcm_mcusys_cpc_turbo,
     .current_state = MCUSYS_CPC_TURBO_DCM_ON,
     .default_state = MCUSYS_CPC_TURBO_DCM_ON,
     .disable_refcnt = 0,
    },
    {
     .typeid = MCUSYS_STALL_DCM_TYPE,
     .name = "MCU_STALL_DCM",
     .func = dcm_mcusys_stall,
     .current_state = MCUSYS_STALL_DCM_ON,
     .default_state = MCUSYS_STALL_DCM_ON,
     .disable_refcnt = 0,
    },
    {
     .typeid = MCUSYS_APB_DCM_TYPE,
     .name = "MCU_APB_DCM",
     .func = dcm_mcusys_apb,
     .current_state = MCUSYS_APB_DCM_ON,
     .default_state = MCUSYS_APB_DCM_ON,
     .disable_refcnt = 0,
    },
    {
     .typeid = VLP_DCM_TYPE,
     .name = "VLP_DCM",
     .func = dcm_vlp,
     .current_state = VLP_DCM_ON,
     .default_state = VLP_DCM_ON,
     .disable_refcnt = 0,
    },
    {
     .typeid = EMI_DCM_TYPE,
     .name = "EMI_DCM",
     .func = dcm_emi,
     .current_state = EMI_DCM_ON,
     .default_state = EMI_DCM_ON,
     .disable_refcnt = 0,
     },
    {
     .typeid = DRAMC_DCM_TYPE,
     .name = "DRAMC_DCM",
     .func = dcm_dramc_ao,
     .current_state = DRAMC_AO_DCM_ON,
     .default_state = DRAMC_AO_DCM_ON,
     .disable_refcnt = 0,
     },
    {
     .typeid = DDRPHY_DCM_TYPE,
     .name = "DDRPHY_DCM",
     .func = dcm_ddrphy,
     .current_state = DDRPHY_DCM_ON,
     .default_state = DDRPHY_DCM_ON,
     .disable_refcnt = 0,
     },
    {
     .typeid = BIG_CORE_DCM_TYPE,
     .name = "BIG_CORE_DCM",
     .func = dcm_big_core,
     .current_state = BIG_CORE_DCM_ON,
     .default_state = BIG_CORE_DCM_ON,
     .disable_refcnt = 0,
     },
    {
     .typeid = GIC_SYNC_DCM_TYPE,
     .name = "GIC_SYNC_DCM",
     .func = dcm_gic_sync,
     .current_state = GIC_SYNC_DCM_OFF,
     .default_state = GIC_SYNC_DCM_OFF,
     .disable_refcnt = 0,
     },
    {
     .typeid = LAST_CORE_DCM_TYPE,
     .name = "LAST_CORE_DCM",
     .func = dcm_last_core,
     .current_state = LAST_CORE_DCM_ON,
     .default_state = LAST_CORE_DCM_ON,
     .disable_refcnt = 0,
     },
    {
     .typeid = RGU_DCM_TYPE,
     .name = "RGU_DCM",
     .func = dcm_rgu,
     .current_state = RGU_DCM_ON,
     .default_state = RGU_DCM_ON,
     .disable_refcnt = 0,
     },
    {
     .typeid = TOPCKG_DCM_TYPE,
     .name = "TOPCKG_DCM",
     .func = dcm_topckg,
     .current_state = TOPCKG_DCM_ON,
     .default_state = TOPCKG_DCM_ON,
     .disable_refcnt = 0,
     },
    {
     .typeid = LPDMA_DCM_TYPE,
     .name = "LPDMA_DCM",
     .func = dcm_lpdma,
     .current_state = LPDMA_DCM_ON,
     .default_state = LPDMA_DCM_ON,
     .disable_refcnt = 0,
     },
     {
     .typeid = MCSI_DCM_TYPE,
     .name = "MCSI_DCM",
     .func = dcm_mcsi,
     .current_state = MCSI_DCM_ON,
     .default_state = MCSI_DCM_ON,
     .disable_refcnt = 0,
     },
    {
     .typeid = BUSDVT_DCM_TYPE,
     .name = "MCSI_DCM",
     .func = dcm_busdvt,
     .current_state = BUSDVT_DCM_ON,
     .default_state = BUSDVT_DCM_ON,
     .disable_refcnt = 0,
     },
};

/*****************************************
 * DCM driver will provide regular APIs :
 * 1. dcm_restore(type) to recovery CURRENT_STATE before any power-off reset.
 * 2. dcm_set_default(type) to reset as cold-power-on init state.
 * 3. dcm_disable(type) to disable all dcm.
 * 4. dcm_set_state(type) to set dcm state.
 * 5. dcm_dump_state(type) to show CURRENT_STATE.
 * 6. /sys/power/dcm_state interface:  'restore', 'disable', 'dump', 'set'. 4 commands.
 *
 * spsecified APIs for workaround:
 * 1. (definitely no workaround now)
 *****************************************/
static void dcm_set_default(unsigned int type)
{
    int i;
    struct DCM *dcm;
#if MCUSYS_SET_IN_ATF
    struct __smccc_res res;
#endif /* MCUSYS_SET_IN_ATF */

#ifndef ENABLE_DCM_IN_LK
    dcm_warn("[%s]type:0x%X, init_dcm_type=0x%X\n", __func__, type, init_dcm_type);
#else
    dcm_warn("[%s]type:0x%X, init_dcm_type=0x%X, INIT_DCM_TYPE_BY_K=0x%X\n",
        __func__, type, init_dcm_type, INIT_DCM_TYPE_BY_K);
#endif

    for (i = 0, dcm = &dcm_array[0]; i < NR_DCM_TYPE; i++, dcm++) {
        if (type & dcm->typeid) {
            dcm->saved_state = dcm->default_state;
            dcm->current_state = dcm->default_state;
            dcm->disable_refcnt = 0;
#ifdef ENABLE_DCM_IN_LK
            if (!INIT_DCM_TYPE_BY_K && dcm->typeid) {
#endif
                if (dcm->preset_func)
                    dcm->preset_func();
                dcm->func(dcm->current_state);
#ifdef ENABLE_DCM_IN_LK
            }
#endif

            dcm_info("[%s 0x%X] current state:%d (%d)\n",
                dcm->name, dcm->typeid, dcm->current_state,
                dcm->disable_refcnt);
        }
    }

#if MCUSYS_SET_IN_ATF
    dcm_smc_msg_send(init_dcm_type, res);
#endif /* MCUSYS_SET_IN_ATF */
}

static void dcm_set_state(unsigned int type, int state)
{
    int i;
    struct DCM *dcm;
    unsigned int init_dcm_type_pre = init_dcm_type;
#if MCUSYS_SET_IN_ATF
    struct __smccc_res res;
#endif /* MCUSYS_SET_IN_ATF */

    dcm_warn("[%s]type:0x%X, set:%d, init_dcm_type_pre=0x%X\n",
         __func__, type, state, init_dcm_type_pre);

    for (i = 0, dcm = &dcm_array[0]; type && (i < NR_DCM_TYPE); i++, dcm++) {
        if (type & dcm->typeid) {
            type &= ~(dcm->typeid);

            dcm->saved_state = state;
            if (dcm->disable_refcnt == 0) {
                if (state)
                    init_dcm_type |= dcm->typeid;
                else
                    init_dcm_type &= ~(dcm->typeid);

                dcm->current_state = state;
                dcm->func(dcm->current_state);
            }

            dcm_info("[%s 0x%X] current state:%d (%d)\n",
                 dcm->name, dcm->typeid, dcm->current_state,
                 dcm->disable_refcnt);

        }
    }

    if (init_dcm_type_pre != init_dcm_type) {
        dcm_warn("[%s]type:0x%X, set:%d, init_dcm_type=0x%X->0x%X\n",
             __func__, type, state, init_dcm_type_pre, init_dcm_type);
#if MCUSYS_SET_IN_ATF
        dcm_smc_msg_send(init_dcm_type, res);
#endif /* MCUSYS_SET_IN_ATF */
    }
}


static void dcm_disable(unsigned int type)
{
    int i;
    struct DCM *dcm;
    unsigned int init_dcm_type_pre = init_dcm_type;
#if MCUSYS_SET_IN_ATF
    struct __smccc_res res;
#endif /* MCUSYS_SET_IN_ATF */

    dcm_warn("[%s]type:0x%X\n", __func__, type);

    for (i = 0, dcm = &dcm_array[0]; type && (i < NR_DCM_TYPE); i++, dcm++) {
        if (type & dcm->typeid) {
            type &= ~(dcm->typeid);

            dcm->current_state = DCM_OFF;
            if (dcm->disable_refcnt++ == 0)
                init_dcm_type &= ~(dcm->typeid);
            dcm->func(dcm->current_state);

            dcm_info("[%s 0x%X] current state:%d (%d)\n",
                 dcm->name, dcm->typeid, dcm->current_state,
                 dcm->disable_refcnt);

        }
    }

    if (init_dcm_type_pre != init_dcm_type) {
        dcm_warn("[%s]type:0x%X, init_dcm_type=0x%X->0x%X\n",
             __func__, type, init_dcm_type_pre, init_dcm_type);
#if MCUSYS_SET_IN_ATF
        dcm_smc_msg_send(init_dcm_type, res);
#endif /* MCUSYS_SET_IN_ATF */
    }

}

static void dcm_restore(unsigned int type)
{
    int i;
    struct DCM *dcm;
    unsigned int init_dcm_type_pre = init_dcm_type;
#if MCUSYS_SET_IN_ATF
    struct __smccc_res res;
#endif /* MCUSYS_SET_IN_ATF */

    dcm_warn("[%s]type:0x%X\n", __func__, type);

    for (i = 0, dcm = &dcm_array[0]; type && (i < NR_DCM_TYPE); i++, dcm++) {
        if (type & dcm->typeid) {
            type &= ~(dcm->typeid);

            if (dcm->disable_refcnt > 0)
                dcm->disable_refcnt--;
            if (dcm->disable_refcnt == 0) {
                if (dcm->saved_state)
                    init_dcm_type |= dcm->typeid;
                else
                    init_dcm_type &= ~(dcm->typeid);

                dcm->current_state = dcm->saved_state;
                dcm->func(dcm->current_state);
            }

            dcm_info("[%s 0x%X] current state:%d (%d)\n",
                 dcm->name, dcm->typeid, dcm->current_state,
                 dcm->disable_refcnt);

        }
    }

    if (init_dcm_type_pre != init_dcm_type) {
        dcm_warn("[%s]type:0x%X, init_dcm_type=0x%X->0x%X\n",
             __func__, type, init_dcm_type_pre, init_dcm_type);
#if MCUSYS_SET_IN_ATF
        dcm_smc_msg_send(init_dcm_type, res);
#endif /*MCUSYS_SET_IN_ATF */
    }
}


static void dcm_dump_state(int type)
{
    int i;
    struct DCM *dcm;

    dcm_info("\n******** dcm dump state *********\n");
    for (i = 0, dcm = &dcm_array[0]; i < NR_DCM_TYPE; i++, dcm++) {
        if (type & dcm->typeid) {
            dcm_info("[%s 0x%X] current state:%d (%d)\n",
                 dcm->name, dcm->typeid, dcm->current_state,
                 dcm->disable_refcnt);
        }
    }
}

static void dcm_dump_regs(void)
{
    dcm_info("\n******** dcm dump register *********\n");

    REG_DUMP(MP_CPUSYS_TOP_CPU_PLLDIV_CFG0);
    REG_DUMP(MP_CPUSYS_TOP_CPU_PLLDIV_CFG1);
    REG_DUMP(MP_CPUSYS_TOP_CPU_PLLDIV_CFG2);
    REG_DUMP(MP_CPUSYS_TOP_BUS_PLLDIV_CFG);
    REG_DUMP(MP_CPUSYS_TOP_MCSIC_DCM0);
    REG_DUMP(MP_CPUSYS_TOP_MP_ADB_DCM_CFG0);
    REG_DUMP(MP_CPUSYS_TOP_MP_ADB_DCM_CFG4);
    REG_DUMP(MP_CPUSYS_TOP_MP_MISC_DCM_CFG0);
    REG_DUMP(MP_CPUSYS_TOP_MCUSYS_DCM_CFG0);
    REG_DUMP(CPCCFG_REG_EMI_WFIFO);
    REG_DUMP(MP_CPUSYS_TOP_MP0_DCM_CFG0);
    REG_DUMP(MP_CPUSYS_TOP_MP0_DCM_CFG7);
    REG_DUMP(INFRA_BUS_DCM_CTRL);
    REG_DUMP(P2P_RX_CLK_ON);
    REG_DUMP(INFRA_AXIMEM_IDLE_BIT_EN_0);
    REG_DUMP(VDNR_DCM_TOP_INFRA_PAR_BUS_u_INFRA_PAR_BUS_CTRL_0);
    REG_DUMP(VDNR_DCM_TOP_INFRA_PAR_BUS_u_INFRA_PAR_BUS_CTRL_1);
    REG_DUMP(VDNR_DCM_TOP_INFRA_PAR_BUS_u_INFRA_PAR_BUS_CTRL_2);
    REG_DUMP(VDNR_DCM_TOP_PERI_PAR_BUS_u_PERI_PAR_BUS_CTRL_0);
    REG_DUMP(VDNR_DCM_TOP_PERI_PAR_BUS_u_PERI_PAR_BUS_CTRL_1);
    REG_DUMP(VDNR_DCM_TOP_VLP_PAR_BUS_u_VLP_PAR_BUS_CTRL_0);
}

static int mt_dcm_dts_map(void)
{
    return 0;
}

static unsigned int disabled_by_dts(void *fdt, const char *prop)
{
    int nodeoffset;
    unsigned int *data = NULL;
    int len = 0;

    if (fdt == NULL)
        panic("lk driver fdt is NULL!\n");

    nodeoffset = fdt_node_offset_by_compatible(fdt, -1, DCM_NODE);
    if (nodeoffset < 0) {
        dcm_warn("[DCM] mediatek,dcm not found in DTS!\n");
        return 0;
    }

    data = (unsigned int *) fdt_getprop(fdt, nodeoffset, prop, &len);
    if (data == NULL) {
        dcm_warn("[DCM] get property info fail in DTS.\n");
        return 0;
    }

    return fdt32_to_cpu(*(unsigned int *)data);
}


int mt_dcm_init(void *fdt)
{
    unsigned int mcu_disabled = 0;
    unsigned int infra_disabled = 0;

#ifdef DCM_BRINGUP
    dcm_warn("%s: skipped for bring up\n", __func__);
    return 0;
#endif

    if (mt_dcm_dts_map()) {
        dcm_err("%s: failed due to DTS failed\n", __func__);
        return -1;
    }

    mcu_disabled = disabled_by_dts(fdt, "mcu_disable");
    infra_disabled = disabled_by_dts(fdt, "infra_disable");
#ifndef DCM_DEFAULT_ALL_OFF
    /* for DOE(debug 2.0) support */
    /* get dts node from DTS */
    if (mcu_disabled && infra_disabled) {
        /* "MCU DCMs" includes ARMCORE_DCM_TYPE, MCUSYS_DCM_TYPE, */
        /* and STALL_DCM_TYPE */
        dcm_warn("[DCM] Infra DCM is disabled by DTS\n");
        dcm_warn("[DCM] MCU DCM is disabled by DTS\n");
        dcm_set_state(ARMCORE_DCM_TYPE | MCUSYS_DCM_TYPE
                | STALL_DCM_TYPE | BIG_CORE_DCM_TYPE
                | INFRA_DCM_TYPE, DCM_OFF);
    } else if (mcu_disabled) {
        dcm_warn("[DCM] MCU DCM is disabled by DTS\n");
        dcm_set_state(ARMCORE_DCM_TYPE | MCUSYS_DCM_TYPE
                | STALL_DCM_TYPE | BIG_CORE_DCM_TYPE,
                DCM_OFF);
    } else if (infra_disabled) {
        dcm_warn("[DCM] Infra DCM is disabled by DTS\n");
        dcm_set_state(INFRA_DCM_TYPE, DCM_OFF);
    }
    dcm_set_default(init_dcm_type);    /** enable init dcm **/

#else /* DCM_DEFAULT_ALL_OFF */
    dcm_set_state(all_dcm_type, DCM_OFF);
#endif /* #ifndef DCM_DEFAULT_ALL_OFF */

    dcm_dump_regs();

    dcm_initiated = 1;

    return 0;
}
late_initcall(mt_dcm_init);

/**** public APIs *****/
void mt_dcm_disable(void)
{
    if (!dcm_initiated)
        return;

    dcm_disable(all_dcm_type);
}

void mt_dcm_restore(void)
{
    if (!dcm_initiated)
        return;

    dcm_restore(all_dcm_type);
}
