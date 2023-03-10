/*
 * Copyright (c) 2020 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#define LOG_TAG "DSI"

#include "ddp_info.h"
//#include <platform/mt_typedefs.h>
#include <stdlib.h>
#include <stdbool.h>
//#include <platform/mt_gpt.h>
//#include <platform/sync_write.h>
#include <reg.h>
#include "disp_drv_platform.h"
#include "disp_drv_log.h"
#include "ddp_manager.h"
#include "ddp_reg.h"
#include "ddp_dsi.h"
#include "ddp_log.h"
//#include <platform/ddp_dump.h>
//#include <platform/ddp_path.h>
#include <debug.h>
#include <string.h>

//#include <platform/mt_gpio.h>
#ifdef GPIO_SUPPORT
#include <gpio_api.h>
#endif
#include <lcm_util.h>
#ifdef MTK_LCM_COMMON_DRIVER_SUPPORT
#include <lcm_common_drv.h>
#endif

#define ENABLE_DSI_INTERRUPT 0
#define DSI_MODULE_BEGIN(x)     (x == DISP_MODULE_DSIDUAL ? 0 : DSI_MODULE_to_ID(x))
#define DSI_MODULE_END(x)       (x == DISP_MODULE_DSIDUAL ? 1 : DSI_MODULE_to_ID(x))
#define DSI_MODULE_to_ID(x)     (x == DISP_MODULE_DSI0 ? 0 : 1)
#define DIFF_CLK_LANE_LP 0x10

static int dsi_reg_op_debug;
//static int mipi_reg_op_debug = 1;
static int s_isDsiPowerOn;

/*****************************************************************************/
enum MIPITX_PAD_VALUE {
    PAD_D2P_T0A = 0,
    PAD_D2N_T0B,
    PAD_D0P_T0C,
    PAD_D0N_T1A,
    PAD_CKP_T1B,
    PAD_CKN_T1C,
    PAD_D1P_T2A,
    PAD_D1N_T2B,
    PAD_D3P_T2C,
    PAD_D3N_XXX,
    PAD_NUM
};

#define DSI_OUTREG32(cmdq, addr, val) \
do { \
    if (dsi_reg_op_debug) \
        DISPMSG("[dsi/reg]0x%p=0x%08x, cmdq:0x%p\n", (void *)addr, val, cmdq);\
    writel(val, addr); \
} while (0)

#define DSI_MASKREG32(cmdq, REG, MASK, VALUE)    DISP_REG_MASK((cmdq), (REG), (VALUE), (MASK))


#define DSI_OUTREGBIT(cmdq, TYPE, REG, bit, value)  \
    {\
    if (cmdq) { \
        do { \
        } while (0);  \
    } else { \
        do {\
                TYPE r = *((TYPE *)&readl(&REG));    \
                r.bit = value;    \
                DSI_OUTREG32(cmdq, &REG, AS_UINT32(&r));      \
                } while (0)\
         }      \
    }

#ifdef MACH_FPGA
#define MIPITX_INREGBIT(addr, field) (0)

#define MIPITX_OUTREG32(addr, val) \
do { \
    if (dsi_reg_op_debug) \
        DDPMSG("[mipitx/reg]%p=0x%08x\n", (void *)addr, val); \
    if (0) \
        writel(val, addr); \
} while (0)

#define MIPITX_OUTREGBIT(addr, field, value)  \
do {    \
    unsigned int val = 0; \
    if (0) \
        val = readl(addr);\
    val = (val & ~REG_FLD_MASK(field)) | (REG_FLD_VAL((field), (value))); \
    MIPITX_OUTREG32(addr, val);   \
} while (0)

#else
#define MIPITX_INREGBIT(addr, field) (REG_FLD_VAL((field), readl(addr)))

#define MIPITX_OUTREG32(addr, val) \
do {\
    if (dsi_reg_op_debug) {    \
        DDPMSG("[mipitx/wreg]%p=0x%08x\n", (void *)addr, val);\
    } \
    writel(val, addr);\
} while (0)

#define MIPITX_OUTREGBIT(addr, field, value)  \
do {    \
    unsigned int val = 0; \
    val = readl(addr); \
    val = (val & ~REG_FLD_MASK(field)) | (REG_FLD_VAL((field), (value))); \
    MIPITX_OUTREG32(addr, val);      \
} while (0)
#endif

#define DSI_INREG32(type, addr)                                               \
        ({                                                                   \
            unsigned int var = 0;                                            \
            union p_regs                                                     \
            {                                                                \
                type p_reg;                                                  \
                unsigned int *p_uint;                                       \
            } p_temp1;                                                        \
            p_temp1.p_reg  = (type)(addr);                                   \
            var = readl(p_temp1.p_uint);                                   \
            var;                                                             \
        })

#define DSI_READREG32(type, dst, src)                                \
            {                                                                    \
                union p_regs                                                     \
                {                                                                \
                    type p_reg;                                                  \
                    unsigned int *p_uint;                                       \
                } p_temp1, p_temp2;                                                \
                p_temp1.p_reg  = (type)(dst);                                \
                p_temp2.p_reg  = (type)(src);                                \
                writel(readl(p_temp2.p_uint), p_temp1.p_uint);               \
            }

struct t_dsi_context {
    void           *handle;
    bool            enable;
    struct DSI_REGS_TYPE        regBackup;
    unsigned int    cmdq_size;
    LCM_DSI_PARAMS  dsi_params;
    //high frame rate
    unsigned int data_phy_cycle;
    unsigned int HS_TRAIL;
};

struct mipitx_impedance {
    unsigned int efuse_block_num;
    unsigned int block_bits_num;
    unsigned int efuse_base;
    unsigned int offset_start;
};

struct t_dsi_context _dsi_context[DSI_INTERFACE_NUM];
static struct DSI_REGS *const DSI_REG[2] = {(struct DSI_REGS *)(DISPSYS_DSI0_BASE),
                                           (struct DSI_REGS *)(DISPSYS_DSI1_BASE)};
static unsigned long const DSI_PHY_REG[2] = {(unsigned long)(DISPSYS_MIPITX0_BASE),
                                             (unsigned long)(DISPSYS_MIPITX1_BASE)};
static struct DSI_CMDQ_REGS *const DSI_CMDQ_REG[2] = {
                                           (struct DSI_CMDQ_REGS *)(DISPSYS_DSI0_BASE+0x200),
                                           (struct DSI_CMDQ_REGS *)(DISPSYS_DSI1_BASE+0x200)};

static struct DSI_VM_CMDQ_REGS *const DSI_VM_CMD_REG[2] = {
                                           (struct DSI_VM_CMDQ_REGS *)(DISPSYS_DSI0_BASE + 0x134),
                                           (struct DSI_VM_CMDQ_REGS *)(DISPSYS_DSI1_BASE + 0x134)};
static LCM_UTIL_FUNCS lcm_utils_dsi0;
static LCM_UTIL_FUNCS lcm_utils_dsi1;
static LCM_UTIL_FUNCS lcm_utils_dsidual;
//static int dsi0_te_enable = 0;
//static int dsi1_te_enable = 0;
//static int dsidual_te_enable = 0;

static void _DSI_INTERNAL_IRQ_Handler(enum DISP_MODULE_ENUM module, unsigned int param)
{}

static enum DSI_STATUS DSI_Reset(enum DISP_MODULE_ENUM module, void *cmdq)
{
    int i = 0;

    for (i = DSI_MODULE_BEGIN(module); i <= DSI_MODULE_END(module); i++) {
        //DSI_OUTREGBIT(cmdq, struct DSI_COM_CTRL_REG,DSI_REG[i]->DSI_COM_CTRL,DSI_RESET, 1);
        //DSI_OUTREGBIT(cmdq, struct DSI_COM_CTRL_REG,DSI_REG[i]->DSI_COM_CTRL,DSI_RESET,0);
        DISP_REG_SET_FIELD(cmdq, DSI_COM_CON_FLD_DSI_RESET,
                            DSI_REG_BASE[i] + DISP_REG_DSI_COM_CON, 1);
        DISP_REG_SET_FIELD(cmdq, DSI_COM_CON_FLD_DSI_RESET,
                            DSI_REG_BASE[i] + DISP_REG_DSI_COM_CON, 0);
    }

    return DSI_STATUS_OK;
}

static int _dsi_is_video_mode(enum DISP_MODULE_ENUM module)
{
    int i = 0;
    /*struct DSI_MODE_CTRL_REG tmpreg;*/

    for (i = DSI_MODULE_BEGIN(module); i <= DSI_MODULE_END(module); i++) {
        if (DSI_REG[i]->DSI_MODE_CTRL.MODE == CMD_MODE)
            return 0;
        else
            return 1;
    }
    return 0;
}

static enum DSI_STATUS DSI_SetMode(enum DISP_MODULE_ENUM module, void *cmdq, unsigned int mode)
{
    int i = 0;

    for (i = DSI_MODULE_BEGIN(module); i <= DSI_MODULE_END(module); i++) {
        /*DSI_OUTREGBIT(cmdq, struct DSI_MODE_CTRL_REG,DSI_REG[i]->DSI_MODE_CTRL, MODE, mode);*/
        DISP_REG_SET_FIELD(cmdq,
                        DSI_MODE_CON_FLD_MODE_CON, DSI_REG_BASE[i] + DISP_REG_DSI_MODE_CON, mode);
    }

    return DSI_STATUS_OK;
}

static void DSI_WaitForNotBusy(enum DISP_MODULE_ENUM module, void *cmdq)
{
    /*DISPFUNC();*/
    int i = 0, cnt = 0;
    unsigned int tmp = 0;

    /*...dsi video is always in busy state...*/
    if (_dsi_is_video_mode(module))
        return;

    i = DSI_MODULE_BEGIN(module);
    while (1) {
        tmp = readl(DSI_REG_BASE[i] + DISP_REG_DSI_INTSTA);
        /* spin(1000); just polling, busy delay is not needed*/
        if (!(tmp & 0x80000000))
            break;
        if (!(++cnt % 1000)) {
            DDPERR("%s: %d\n", __func__, cnt);
            DSI_DumpRegisters(module, 1);
        }
    }
}

void DSI0_WaitNotBusyAfterTrigger(enum DISP_MODULE_ENUM module)
{
    int i = 0, cnt = 0;
    int delay_in_us = 50;
    unsigned int tmp = 0;

    /*...dsi video is always in busy state...*/
    if (_dsi_is_video_mode(module))
        return;

    DDPMSG("%s+\n", __func__);

    i = DSI_MODULE_BEGIN(module);
    while (cnt < (1000 * 1000 / delay_in_us)) { /* poll 1s */
        tmp = readl(DSI_REG_BASE[i] + DISP_REG_DSI_INTSTA);
        spin(delay_in_us);
        if (!(tmp & 0x80000000)) {
            DDPMSG("%s- cnt:%d\n", __func__, cnt);
            return;
        }
        if (!(++cnt % (20 * 1000 / delay_in_us)))
            DDPERR("%s: still wait cnt:%d\n", __func__, cnt);
    }
    DDPMSG("%s- cannot wait not busy\n", __func__);
    DSI_DumpRegisters(module, 1);
}

void DSI_lane0_ULP_mode(enum DISP_MODULE_ENUM module, void *cmdq, bool enter)
{
    int i = 0;

    ASSERT(cmdq == NULL);

    for (i = DSI_MODULE_BEGIN(module); i <= DSI_MODULE_END(module); i++) {
        if (enter) {
            //DSI_OUTREGBIT(cmdq, struct DSI_PHY_LD0CON_REG,
            //                   DSI_REG[i]->DSI_PHY_LD0CON, L0_RM_TRIG_EN, 0);
            DISP_REG_SET_FIELD(cmdq, DSI_PHY_LD0CON_FLD_L0_RM_TRIG_EN,
                                DSI_REG_BASE[i] + DISP_REG_DSI_PHY_LD0CON, 0);
            spin(1000);
            //DSI_OUTREGBIT(cmdq, struct DSI_PHY_LD0CON_REG,
            //                   DSI_REG[i]->DSI_PHY_LD0CON, L0_ULPM_EN, 0);
            //DSI_OUTREGBIT(cmdq, struct DSI_PHY_LD0CON_REG,
            //                   DSI_REG[i]->DSI_PHY_LD0CON, L0_ULPM_EN, 1);
            DISP_REG_SET_FIELD(cmdq, DSI_PHY_LD0CON_FLD_L0_ULPM_EN,
                                DSI_REG_BASE[i] + DISP_REG_DSI_PHY_LD0CON, 0);
            DISP_REG_SET_FIELD(cmdq, DSI_PHY_LD0CON_FLD_L0_ULPM_EN,
                                DSI_REG_BASE[i] + DISP_REG_DSI_PHY_LD0CON, 1);
        } else {
            //DSI_OUTREGBIT(cmdq, struct DSI_PHY_LD0CON_REG,
            //                   DSI_REG[i]->DSI_PHY_LD0CON, L0_ULPM_EN, 0);
            DISP_REG_SET_FIELD(cmdq, DSI_PHY_LD0CON_FLD_L0_ULPM_EN,
                                DSI_REG_BASE[i] + DISP_REG_DSI_PHY_LD0CON, 0);
            spin(1000);
            //DSI_OUTREGBIT(cmdq, struct DSI_PHY_LD0CON_REG,
            //                   DSI_REG[i]->DSI_PHY_LD0CON, L0_WAKEUP_EN, 1);
            DISP_REG_SET_FIELD(cmdq, DSI_PHY_LD0CON_FLD_L0_WAKEUP_EN,
                                DSI_REG_BASE[i] + DISP_REG_DSI_PHY_LD0CON, 1);
            spin(1000);
            //DSI_OUTREGBIT(cmdq, struct DSI_PHY_LD0CON_REG,
            //                   DSI_REG[i]->DSI_PHY_LD0CON, L0_WAKEUP_EN, 0);
            DISP_REG_SET_FIELD(cmdq, DSI_PHY_LD0CON_FLD_L0_WAKEUP_EN,
                                DSI_REG_BASE[i] + DISP_REG_DSI_PHY_LD0CON, 0);
            spin(1000);
        }
    }
}

void DSI_clk_ULP_mode(enum DISP_MODULE_ENUM module, void *cmdq, bool enter)
{
    int i = 0;

    ASSERT(cmdq == NULL);

    for (i = DSI_MODULE_BEGIN(module); i <= DSI_MODULE_END(module); i++) {
        if (enter) {
            //DSI_OUTREGBIT(cmdq, struct DSI_PHY_LCCON_REG,
            //                   DSI_REG[i]->DSI_PHY_LCCON, LC_ULPM_EN, 0);
            //DSI_OUTREGBIT(cmdq, struct DSI_PHY_LCCON_REG,
            //                   DSI_REG[i]->DSI_PHY_LCCON, LC_ULPM_EN, 1);
            DISP_REG_SET_FIELD(cmdq, DSI_PHY_LCCON_FLD_LC_ULPM_EN,
                                DSI_REG_BASE[i] + DISP_REG_DSI_PHY_LCCON, 0);
            DISP_REG_SET_FIELD(cmdq, DSI_PHY_LCCON_FLD_LC_ULPM_EN,
                                DSI_REG_BASE[i] + DISP_REG_DSI_PHY_LCCON, 1);
            spin(1000);
        } else {
            //DSI_OUTREGBIT(cmdq, struct DSI_PHY_LCCON_REG,
            //                   DSI_REG[i]->DSI_PHY_LCCON, LC_ULPM_EN, 0);
            DISP_REG_SET_FIELD(cmdq, DSI_PHY_LCCON_FLD_LC_ULPM_EN,
                                DSI_REG_BASE[i] + DISP_REG_DSI_PHY_LCCON, 0);
            spin(1000);

            //DSI_OUTREGBIT(cmdq, struct DSI_PHY_LCCON_REG,
            //                   DSI_REG[i]->DSI_PHY_LCCON, LC_WAKEUP_EN, 1);
            DISP_REG_SET_FIELD(cmdq, DSI_PHY_LCCON_FLD_LC_WAKEUP_EN,
                                DSI_REG_BASE[i] + DISP_REG_DSI_PHY_LCCON, 1);
            spin(1000);

            //DSI_OUTREGBIT(cmdq, struct DSI_PHY_LCCON_REG,
            //                   DSI_REG[i]->DSI_PHY_LCCON, LC_WAKEUP_EN, 0);
            DISP_REG_SET_FIELD(cmdq, DSI_PHY_LCCON_FLD_LC_WAKEUP_EN,
                                DSI_REG_BASE[i] + DISP_REG_DSI_PHY_LCCON, 0);
            spin(1000);
        }
    }
}

/**
 * DSI_enter_ULPS
 *
 * 1. disable DSI high-speed clock
 * 2. Data lane enter ultra-low power mode
 * 3. Clock lane enter ultra-low power mode
 * 4. wait DSI sleepin irq (timeout interval ?)
 * 5. clear lane_num
 */
void DSI_enter_ULPS(enum DISP_MODULE_ENUM module)
{
    int i = 0;
    int cnt = 0;

    /* DSI_clk_HS_mode(module, NULL, FALSE); */
    for (i = DSI_MODULE_BEGIN(module); i <= DSI_MODULE_END(module); i++) {
        ASSERT(DSI_REG[i]->DSI_PHY_LD0CON.L0_ULPM_EN == 0);
        ASSERT(DSI_REG[i]->DSI_PHY_LCCON.LC_ULPM_EN == 0);

        //DSI_OUTREGBIT(NULL, struct DSI_INT_ENABLE_REG,
        //                   DSI_REG[i]->DSI_INTEN, SLEEPIN_ULPS_INT_EN, 0);
        //DSI_OUTREGBIT(NULL, struct DSI_INT_STATUS_REG,
        //                   DSI_REG[i]->DSI_INTSTA, SLEEPIN_DONE, 0);
        DISP_REG_SET_FIELD(NULL, DSI_INTEN_FLD_SLEEPIN_ULPS_INT_EN,
                            DSI_REG_BASE[i] + DISP_REG_DSI_INTEN, 0);
        DISP_REG_SET_FIELD(NULL, DSI_INTSTA_FLD_SLEEPIN_ULPS_INT_EN_FLAG,
                            DSI_REG_BASE[i] + DISP_REG_DSI_INTSTA, 0);

        //DSI_OUTREGBIT(NULL, struct DSI_PHY_LD0CON_REG,
        //                   DSI_REG[i]->DSI_PHY_LD0CON, Lx_ULPM_AS_L0, 1);
        //DSI_OUTREGBIT(NULL, struct DSI_PHY_LD0CON_REG,
        //                   DSI_REG[i]->DSI_PHY_LD0CON, L0_ULPM_EN, 1);
        //DSI_OUTREGBIT(NULL, struct DSI_PHY_LCCON_REG,
        //                   DSI_REG[i]->DSI_PHY_LCCON, LC_ULPM_EN, 1);
        DISP_REG_SET_FIELD(NULL, DSI_PHY_LD0CON_FLD_LX_ULPM_AS_L0,
                            DSI_REG_BASE[i] + DISP_REG_DSI_PHY_LD0CON, 1);
        DISP_REG_SET_FIELD(NULL, DSI_PHY_LD0CON_FLD_L0_ULPM_EN,
                            DSI_REG_BASE[i] + DISP_REG_DSI_PHY_LD0CON, 1);
        DISP_REG_SET_FIELD(NULL, DSI_PHY_LCCON_FLD_LC_ULPM_EN,
                            DSI_REG_BASE[i] + DISP_REG_DSI_PHY_LCCON, 1);

        while (DISP_REG_GET_FIELD(DSI_INTSTA_FLD_SLEEPIN_ULPS_INT_EN_FLAG,
               DSI_REG_BASE[i] + DISP_REG_DSI_INTSTA) == 0) {
            if (!(++cnt % 100))
                DDPERR("polling SLEEPIN_DONE %d\n", cnt);
        }

        //DSI_OUTREGBIT(NULL, struct DSI_INT_ENABLE_REG,
        //                   DSI_REG[i]->DSI_INTEN, SLEEPIN_ULPS_INT_EN, 0);
        //DSI_OUTREGBIT(NULL, struct DSI_INT_STATUS_REG,
        //                   DSI_REG[i]->DSI_INTSTA, SLEEPIN_DONE, 0);
        DISP_REG_SET_FIELD(NULL, DSI_INTEN_FLD_SLEEPIN_ULPS_INT_EN,
                            DSI_REG_BASE[i] + DISP_REG_DSI_INTEN, 0);
        DISP_REG_SET_FIELD(NULL, DSI_INTSTA_FLD_SLEEPIN_ULPS_INT_EN_FLAG,
                            DSI_REG_BASE[i] + DISP_REG_DSI_INTSTA, 0);

        // clear lane_num when enter ulps
        // DSI_OUTREGBIT(NULL, struct DSI_TXRX_CTRL_REG,
        //                   DSI_REG[i]->DSI_TXRX_CTRL, LANE_NUM, 0);
        DISP_REG_SET_FIELD(NULL, DSI_TXRX_CON_FLD_LANE_NUM,
                            DSI_REG_BASE[i] + DISP_REG_DSI_TXRX_CON, 0);

    }
}

/**
 * DSI_exit_ULPS
 *
 * 1. set DSI sleep out mode
 * 2. set wakeup prd according to current MIPI frequency
 * 3. recovery lane number
 * 4. sleep out start
 * 4. wait DSI sleepout irq (timeout interval ?)
 */
void DSI_exit_ULPS(enum DISP_MODULE_ENUM module)
{
    int i = 0, cnt = 0;
    unsigned int lane_num_bitvalue = 0;
    unsigned int pll_clock = _dsi_context[i].dsi_params.PLL_CLOCK;
    /* wake_up_prd * 1024 * cycle time > 1ms */
    int wake_up_prd = (pll_clock * 2 * 1000) / (1024 * 8) + 0x1;

    for (i = DSI_MODULE_BEGIN(module); i <= DSI_MODULE_END(module); i++) {
        if (_dsi_context[i].dsi_params.IsCphy) {
            DISP_REG_SET_FIELD(NULL, DSI_CPHY_CON0_CPHY_EN,
                                DSI_REG_BASE[i] + DISP_REG_DSI_CPHY_CON0, 1);
            wake_up_prd = (pll_clock * 2 * 1000) / (1024 * 7) + 0x1;
        }

        // DSI_OUTREGBIT(NULL, struct DSI_PHY_LD0CON_REG,
            // DSI_REG[i]->DSI_PHY_LD0CON, Lx_ULPM_AS_L0, 1);
        // DSI_OUTREGBIT(NULL, struct DSI_INT_ENABLE_REG,
            // DSI_REG[i]->DSI_INTEN, SLEEPOUT_DONE, 0);
        // DSI_OUTREGBIT(NULL, struct DSI_INT_STATUS_REG,
            // DSI_REG[i]->DSI_INTSTA, SLEEPOUT_DONE, 0);
        // DSI_OUTREGBIT(NULL, struct DSI_MODE_CTRL_REG,
            // DSI_REG[i]->DSI_MODE_CTRL, SLEEP_MODE, 1);
        // DSI_OUTREGBIT(NULL, struct DSI_TIME_CON0_REG,
            // DSI_REG[i]->DSI_TIME_CON0, UPLS_WAKEUP_PRD,
            // wake_up_prd);
        DISP_REG_SET_FIELD(NULL, DSI_PHY_LD0CON_FLD_LX_ULPM_AS_L0,
                            DSI_REG_BASE[i] + DISP_REG_DSI_PHY_LD0CON, 1);
        DISP_REG_SET_FIELD(NULL, DSI_INTEN_FLD_SLEEPOUT_DONE_INT_EN,
                            DSI_REG_BASE[i] + DISP_REG_DSI_INTEN, 0);
        DISP_REG_SET_FIELD(NULL, DSI_INTSTA_FLD_SLEEPOUT_DONE_INT_FLAG,
                            DSI_REG_BASE[i] + DISP_REG_DSI_INTSTA, 0);
        DISP_REG_SET_FIELD(NULL, DSI_MODE_CON_FLD_SLEEP_MODE,
                            DSI_REG_BASE[i] + DISP_REG_DSI_MODE_CON, 1);
        DISP_REG_SET_FIELD(NULL, DSI_TIME_CON0_FLD_ULPS_WAKEUP_PRD,
                            DSI_REG_BASE[i] + DISP_REG_DSI_TIME_CON0,
                            wake_up_prd);

        switch (_dsi_context[i].dsi_params.LANE_NUM) {
        case LCM_ONE_LANE:
            lane_num_bitvalue = 0x1;
            break;
        case LCM_TWO_LANE:
            lane_num_bitvalue = 0x3;
            break;
        case LCM_THREE_LANE:
            lane_num_bitvalue = 0x7;
            break;
        case LCM_FOUR_LANE:
            lane_num_bitvalue = 0xF;
            break;
        default:
            break;
        }

        // DSI_OUTREGBIT(NULL, struct DSI_TXRX_CTRL_REG,
            // DSI_REG[i]->DSI_TXRX_CTRL, LANE_NUM, lane_num_bitvalue);
        DISP_REG_SET_FIELD(NULL, DSI_TXRX_CON_FLD_LANE_NUM,
                            DSI_REG_BASE[i] + DISP_REG_DSI_TXRX_CON,
                            lane_num_bitvalue);

        // DSI_OUTREGBIT(NULL, struct DSI_START_REG,
            // DSI_REG[i]->DSI_START, SLEEPOUT_START, 0);
        // DSI_OUTREGBIT(NULL, struct DSI_START_REG,
            // DSI_REG[i]->DSI_START, SLEEPOUT_START, 1);
        DISP_REG_SET_FIELD(NULL, DSI_START_FLD_SLEEPOUT_START,
                            DSI_REG_BASE[i] + DISP_REG_DSI_START, 0);
        DISP_REG_SET_FIELD(NULL, DSI_START_FLD_SLEEPOUT_START,
                            DSI_REG_BASE[i] + DISP_REG_DSI_START, 1);

        while (DISP_REG_GET_FIELD(DSI_INTSTA_FLD_SLEEPOUT_DONE_INT_FLAG,
               DSI_REG_BASE[i] + DISP_REG_DSI_INTSTA) == 0) {
            if (!(++cnt % 100))
                DDPERR("polling SLEEPOUT_DONE %d\n", cnt);
        }
    }

    for (i = DSI_MODULE_BEGIN(module); i <= DSI_MODULE_END(module); i++) {
        // DSI_OUTREGBIT(NULL, struct DSI_INT_ENABLE_REG,
            // DSI_REG[i]->DSI_INTEN, SLEEPOUT_DONE, 0);
        // DSI_OUTREGBIT(NULL, struct DSI_INT_STATUS_REG,
            // DSI_REG[i]->DSI_INTSTA, SLEEPOUT_DONE, 0);
        // DSI_OUTREGBIT(NULL, struct DSI_START_REG,
            // DSI_REG[i]->DSI_START, SLEEPOUT_START, 0);
        // DSI_OUTREGBIT(NULL, struct DSI_MODE_CTRL_REG,
            // DSI_REG[i]->DSI_MODE_CTRL, SLEEP_MODE, 0);
        DISP_REG_SET_FIELD(NULL, DSI_INTEN_FLD_SLEEPOUT_DONE_INT_EN,
                            DSI_REG_BASE[i] + DISP_REG_DSI_INTEN, 0);
        DISP_REG_SET_FIELD(NULL, DSI_INTSTA_FLD_SLEEPOUT_DONE_INT_FLAG,
                            DSI_REG_BASE[i] + DISP_REG_DSI_INTSTA, 0);
        DISP_REG_SET_FIELD(NULL, DSI_START_FLD_SLEEPOUT_START,
                            DSI_REG_BASE[i] + DISP_REG_DSI_START, 0);
        DISP_REG_SET_FIELD(NULL, DSI_MODE_CON_FLD_SLEEP_MODE,
                            DSI_REG_BASE[i] + DISP_REG_DSI_MODE_CON, 0);
    }
}


bool DSI_clk_HS_state(enum DISP_MODULE_ENUM module, void *cmdq)
{
    int i = 0;
    struct DSI_PHY_LCCON_REG tmpreg;

    for (i = DSI_MODULE_BEGIN(module); i <= DSI_MODULE_END(module); i++) {
        DSI_READREG32(struct DSI_PHY_LCCON_REG*, &tmpreg,
        &DSI_REG[i]->DSI_PHY_LCCON);
        return tmpreg.LC_HS_TX_EN ? true : false;
    }

    return  true;
}

void DSI_clk_HS_mode(enum DISP_MODULE_ENUM module, void *cmdq, bool enter)
{
    int i = 0;

    for (i = DSI_MODULE_BEGIN(module); i <= DSI_MODULE_END(module); i++) {
        if (enter) {
#ifdef MACH_FPGA
            /* fpga must set this */
            DISP_REG_SET(NULL, DISP_REG_DSI_PHY_LCPAT + DSI_REG_BASE[i], 0x55);
#endif
            DISP_REG_SET_FIELD(cmdq, DSI_PHY_LCCON_FLD_LC_HSTX_EN,
                                DSI_REG_BASE[i] + DISP_REG_DSI_PHY_LCCON, 1);
        } else if (!enter) {
            //DSI_OUTREGBIT(cmdq, struct DSI_PHY_LCCON_REG, DSI_REG[i]->DSI_PHY_LCCON,
            //                    LC_HS_TX_EN, 0);
            DISP_REG_SET_FIELD(cmdq, DSI_PHY_LCCON_FLD_LC_HSTX_EN,
                                DSI_REG_BASE[i] + DISP_REG_DSI_PHY_LCCON, 0);
        }
    }
    return;
}

const char *_dsi_cmd_mode_parse_state(unsigned int state)
{
    switch (state) {
    case 0x0001:
        return "idle";
    case 0x0002:
        return "Reading command queue for header";
    case 0x0004:
        return "Sending type-0 command";
    case 0x0008:
        return "Waiting frame data from RDMA for type-1 command";
    case 0x0010:
        return "Sending type-1 command";
    case 0x0020:
        return "Sending type-2 command";
    case 0x0040:
        return "Reading command queue for type-2 data";
    case 0x0080:
        return "Sending type-3 command";
    case 0x0100:
        return "Sending BTA";
    case 0x0200:
        return "Waiting RX-read data";
    case 0x0400:
        return "Waiting SW RACK for RX-read data";
    case 0x0800:
        return "Waiting TE";
    case 0x1000:
        return "Get TE";
    case 0x2000:
        return "Waiting SW RACK for TE";
    case 0x4000:
        return "Waiting external TE";
    case 0x8000:
        return "Get external TE";
    default:
        return "unknown";
    }
    return "unknown";
}

static const char *_dsi_vdo_mode_parse_state(unsigned int state)
{
    switch (state) {
    case 0x0001:
        return "Video mode idle";
    case 0x0002:
        return "Sync start packet";
    case 0x0004:
        return "Hsync active";
    case 0x0008:
        return "Sync end packet";
    case 0x0010:
        return "Hsync back porch";
    case 0x0020:
        return "Video data period";
    case 0x0040:
        return "Hsync front porch";
    case 0x0080:
        return "BLLP";
    case 0x0100:
        return "--";
    case 0x0200:
        return "Mix mode using command mode transmission";
    case 0x0400:
        return "Command transmission in BLLP";
    default:
        return "unknown";
    }

    return "unknown";
}

enum DSI_STATUS DSI_DumpRegisters(enum DISP_MODULE_ENUM module, int level)
{
    u32 i = 0;
    u32 k = 0;

    DDPDUMP("== DISP DSI REGS ==\n");
    if (level >= 0) {
        for (i = DSI_MODULE_BEGIN(module); i <= DSI_MODULE_END(module); i++) {
            unsigned int DSI_DBG8_Status;
            unsigned int DSI_DBG9_Status;
            addr_t dsi_base_addr = (addr_t)DSI_REG[i];

            if (DSI_REG[i]->DSI_MODE_CTRL.MODE == CMD_MODE) {
                unsigned int DSI_DBG6_Status = (readl(dsi_base_addr + 0x160)) & 0xffff;

                DDPDUMP("DSI%d state6(cmd mode):%s\n",
                    i, _dsi_cmd_mode_parse_state(DSI_DBG6_Status));
            } else {
                unsigned int DSI_DBG7_Status = (readl(dsi_base_addr + 0x164)) & 0xff;

                DDPDUMP("DSI%d state7(vdo mode):%s\n",
                    i, _dsi_vdo_mode_parse_state(DSI_DBG7_Status));
            }
            DSI_DBG8_Status = (readl(dsi_base_addr + 0x168)) & 0x3fff;
            DDPDUMP("DSI%d state8 WORD_COUNTER(cmd mode):%d\n", i, DSI_DBG8_Status);
            DSI_DBG9_Status = (readl(dsi_base_addr + 0x16C)) & 0x3fffff;
            DDPDUMP("DSI%d state9 LINE_COUNTER(cmd mode):%d\n", i, DSI_DBG9_Status);
        }
    }
    if (level >= 1) {
        for (i = DSI_MODULE_BEGIN(module); i <= DSI_MODULE_END(module); i++) {
            addr_t dsi_base_addr = (addr_t)DSI_REG[i];
#ifndef MACH_FPGA
            addr_t mipi_base_addr = (addr_t)DSI_PHY_REG[i];
#endif

            DDPDUMP("== DSI%d REGS ==\n", i);
            for (k = 0; k < 0x400; k += 16) {
                DDPDUMP("0x%04x: 0x%08x 0x%08x 0x%08x 0x%08x\n", k,
                    readl(dsi_base_addr + k),
                    readl(dsi_base_addr + k + 0x4),
                    readl(dsi_base_addr + k + 0x8),
                    readl(dsi_base_addr + k + 0xc));
            }

            DDPDUMP("- DSI%d CMD REGS -\n", i);
            for (k = 0; k < 32; k += 16) { /* only dump first 32 bytes cmd */
                DDPDUMP("0x%04x: 0x%08x 0x%08x 0x%08x 0x%08x\n", k,
                    readl((dsi_base_addr + 0x200 + k)),
                    readl((dsi_base_addr + 0x200 + k + 0x4)),
                    readl((dsi_base_addr + 0x200 + k + 0x8)),
                    readl((dsi_base_addr + 0x200 + k + 0xc)));
            }

#ifndef MACH_FPGA
            DDPDUMP("== DSI_PHY%d REGS ==\n", i);
            for (k = 0; k < 0x6A0; k += 16) {
                DDPDUMP("0x%04x: 0x%08x 0x%08x 0x%08x 0x%08x\n", k,
                    readl((mipi_base_addr + k)),
                    readl((mipi_base_addr + k + 0x4)),
                    readl((mipi_base_addr + k + 0x8)),
                    readl((mipi_base_addr + k + 0xc)));
            }
#endif
        }
    }

    return DSI_STATUS_OK;
}

static const char *dsi_mode_spy(LCM_DSI_MODE_CON mode)
{
    switch (mode) {
    case CMD_MODE:
        return "CMD_MODE";
    case SYNC_PULSE_VDO_MODE:
        return "SYNC_PULSE_VDO_MODE";
    case SYNC_EVENT_VDO_MODE:
        return "SYNC_EVENT_VDO_MODE";
    case BURST_VDO_MODE:
        return "BURST_VDO_MODE";
    default:
        return "unknown";
    }
}

void dsi_analysis(enum DISP_MODULE_ENUM module)
{
    int i = 0;

    DDPDUMP("== DISP DSI ANALYSIS ==\n");
    for (i = DSI_MODULE_BEGIN(module); i <= DSI_MODULE_END(module); i++) {
        DDPDUMP("DSI%d Start:%x, Busy:%d, DSI_DUAL_EN:%d, MODE:%s, High Speed:%d, FSM State:%s\n",
            i, DSI_REG[i]->DSI_START.DSI_START, DSI_REG[i]->DSI_INTSTA.BUSY,
            DSI_REG[i]->DSI_COM_CTRL.DSI_DUAL_EN, dsi_mode_spy(DSI_REG[i]->DSI_MODE_CTRL.MODE),
            DSI_REG[i]->DSI_PHY_LCCON.LC_HS_TX_EN,
            _dsi_cmd_mode_parse_state(DSI_REG[i]->DSI_STATE_DBG6.CMTRL_STATE));

        DDPDUMP("DSI%d IRQ,RD_RDY:%d, CMD_DONE:%d, SLEEPOUT_DONE:%d\n",
            i, DSI_REG[i]->DSI_INTSTA.RD_RDY, DSI_REG[i]->DSI_INTSTA.CMD_DONE,
            DSI_REG[i]->DSI_INTSTA.SLEEPOUT_DONE);

        DDPDUMP("DSI%d IRQ,TE_RDY:%d, VM_CMD_DONE:%d, VM_DONE:%d\n",
            i, DSI_REG[i]->DSI_INTSTA.TE_RDY, DSI_REG[i]->DSI_INTSTA.VM_CMD_DONE,
            DSI_REG[i]->DSI_INTSTA.VM_DONE);

        DDPDUMP("DSI%d Lane Num:%d, Ext_TE_EN:%d, Ext_TE_Edge:%d, HSTX_CKLP_EN:%d\n", i,
            DSI_REG[i]->DSI_TXRX_CTRL.LANE_NUM,
            DSI_REG[i]->DSI_TXRX_CTRL.EXT_TE_EN,
            DSI_REG[i]->DSI_TXRX_CTRL.EXT_TE_EDGE,
            DSI_REG[i]->DSI_TXRX_CTRL.HSTX_CKLP_EN);

        DDPDUMP("DSI%d LFR En:%d, LFR MODE:%d, LFR TYPE:%d, LFR SKIP NUMBER:%d\n", i,
            DSI_REG[i]->DSI_LFR_CON.LFR_EN,
            DSI_REG[i]->DSI_LFR_CON.LFR_MODE,
            DSI_REG[i]->DSI_LFR_CON.LFR_TYPE, DSI_REG[i]->DSI_LFR_CON.LFR_SKIP_NUM);
    }
}



enum DSI_STATUS DSI_SleepOut(enum DISP_MODULE_ENUM module, void *cmdq)
{
    int i = 0;
    int wake_up_prd;

    DISPFUNC();
    /* wake_up_prd *1024*cycle time > 1ms */
    unsigned int data_rate = _dsi_context[i].dsi_params.data_rate != 0 ?
                            _dsi_context[i].dsi_params.data_rate :
                            _dsi_context[i].dsi_params.PLL_CLOCK * 2;
    wake_up_prd = (data_rate * 1000) / (1024 * 8) + 0x1;
    for (i = DSI_MODULE_BEGIN(module); i <= DSI_MODULE_END(module); i++) {
        //DSI_OUTREGBIT(cmdq, struct DSI_MODE_CTRL_REG,DSI_REG[i]->DSI_MODE_CTRL,SLEEP_MODE, 1);
        //DSI_OUTREGBIT(cmdq, struct DSI_TIME_CON0_REG, DSI_REG[i]->DSI_TIME_CON0,
        //                UPLS_WAKEUP_PRD, wake_up_prd);  // cycle to 1ms for 520MHz
        DISP_REG_SET_FIELD(cmdq, DSI_MODE_CON_FLD_SLEEP_MODE,
                        DSI_REG_BASE[i] + DISP_REG_DSI_MODE_CON, 1);
        DISP_REG_SET_FIELD(cmdq, DSI_TIME_CON0_FLD_ULPS_WAKEUP_PRD,
                        DSI_REG_BASE[i] + DISP_REG_DSI_TIME_CON0, wake_up_prd);
    }

    return DSI_STATUS_OK;
}

enum DSI_STATUS DSI_Wakeup(enum DISP_MODULE_ENUM module, void *cmdq)
{
    int i = 0;

    DISPFUNC();
    for (i = DSI_MODULE_BEGIN(module); i <= DSI_MODULE_END(module); i++) {
        //DSI_OUTREGBIT(cmdq, struct DSI_START_REG,DSI_REG[i]->DSI_START,SLEEPOUT_START,0);
        //DSI_OUTREGBIT(cmdq, struct DSI_START_REG,DSI_REG[i]->DSI_START,SLEEPOUT_START, 1);
        DISP_REG_SET_FIELD(cmdq, DSI_START_FLD_SLEEPOUT_START,
                        DSI_REG_BASE[i] + DISP_REG_DSI_START, 0);
        DISP_REG_SET_FIELD(cmdq, DSI_START_FLD_SLEEPOUT_START,
                        DSI_REG_BASE[i] + DISP_REG_DSI_START, 1);
        spin(1000);

        //DSI_OUTREGBIT(cmdq, struct DSI_START_REG,DSI_REG[i]->DSI_START,SLEEPOUT_START,0);
        //DSI_OUTREGBIT(cmdq, struct DSI_MODE_CTRL_REG,DSI_REG[i]->DSI_MODE_CTRL,SLEEP_MODE,0);
        DISP_REG_SET_FIELD(cmdq, DSI_START_FLD_SLEEPOUT_START,
                        DSI_REG_BASE[i] + DISP_REG_DSI_START, 0);
        DISP_REG_SET_FIELD(cmdq, DSI_MODE_CON_FLD_SLEEP_MODE,
                        DSI_REG_BASE[i] + DISP_REG_DSI_MODE_CON, 0);
    }

    return DSI_STATUS_OK;
}

enum DSI_STATUS DSI_BackupRegisters(enum DISP_MODULE_ENUM module, void *cmdq)
{
    int i = 0;
    struct DSI_REGS_TYPE *regs = NULL;

    for (i = DSI_MODULE_BEGIN(module); i <= DSI_MODULE_END(module); i++) {
        regs = (void *)&(_dsi_context[i].regBackup);
        DSI_OUTREG32(cmdq, &regs->DSI_INTEN,
                 DISP_REG_GET(DSI_REG_BASE[i] + DISP_REG_DSI_INTEN));
        DSI_OUTREG32(cmdq, (void *)&_dsi_context[i].regBackup.DSI_MODE_CTRL,
                 DISP_REG_GET(DSI_REG_BASE[i] + DISP_REG_DSI_MODE_CON));
        DSI_OUTREG32(cmdq, &regs->DSI_TXRX_CTRL,
                 DISP_REG_GET(DSI_REG_BASE[i] + DISP_REG_DSI_TXRX_CON));
        DSI_OUTREG32(cmdq, (void *)&regs->DSI_PSCTRL,
                 DISP_REG_GET(DSI_REG_BASE[i] + DISP_REG_DSI_PSCON));

        DSI_OUTREG32(cmdq, (void *)&regs->DSI_VSA_NL,
                 AS_UINT32(DSI_REG_BASE[i] + DISP_REG_DSI_VSA_NL));
        DSI_OUTREG32(cmdq, (void *)&regs->DSI_VBP_NL,
                 AS_UINT32(DSI_REG_BASE[i] + DISP_REG_DSI_VBP_NL));
        DSI_OUTREG32(cmdq, (void *)&regs->DSI_VFP_NL,
                 AS_UINT32(DSI_REG_BASE[i] + DISP_REG_DSI_VFP_NL));
        DSI_OUTREG32(cmdq, (void *)&regs->DSI_VACT_NL,
                 AS_UINT32(DSI_REG_BASE[i] + DISP_REG_DSI_VACT_NL));

        DSI_OUTREG32(cmdq, (void *)&regs->DSI_HSA_WC,
                 AS_UINT32(DSI_REG_BASE[i] + DISP_REG_DSI_HSA_WC));
        DSI_OUTREG32(cmdq, (void *)&regs->DSI_HBP_WC,
                 AS_UINT32(DSI_REG_BASE[i] + DISP_REG_DSI_HBP_WC));
        DSI_OUTREG32(cmdq, (void *)&regs->DSI_HFP_WC,
                 AS_UINT32(DSI_REG_BASE[i] + DISP_REG_DSI_HFP_WC));
        DSI_OUTREG32(cmdq, (void *)&regs->DSI_BLLP_WC,
                 AS_UINT32(DSI_REG_BASE[i] + DISP_REG_DSI_BLLP_WC));

        DSI_OUTREG32(cmdq, (void *)&regs->DSI_HSTX_CKL_WC,
                 AS_UINT32(DSI_REG_BASE[i] + DISP_REG_DSI_HSTX_CKLP_WC));
        DSI_OUTREG32(cmdq, (void *)&regs->DSI_MEM_CONTI,
                 AS_UINT32(DSI_REG_BASE[i] + DISP_REG_DSI_MEM_CONTI));

        DSI_OUTREG32(cmdq, (void *)&regs->DSI_PHY_TIMECON0,
                 AS_UINT32(DSI_REG_BASE[i] + DISP_REG_DSI_PHY_TIMCON0));
        DSI_OUTREG32(cmdq, (void *)&regs->DSI_PHY_TIMECON1,
                 AS_UINT32(DSI_REG_BASE[i] + DISP_REG_DSI_PHY_TIMCON1));
        DSI_OUTREG32(cmdq, (void *)&regs->DSI_PHY_TIMECON2,
                 AS_UINT32(DSI_REG_BASE[i] + DISP_REG_DSI_PHY_TIMCON2));
        DSI_OUTREG32(cmdq, &regs->DSI_PHY_TIMECON3,
                 AS_UINT32(DSI_REG_BASE[i] + DISP_REG_DSI_PHY_TIMCON3));
        DSI_OUTREG32(cmdq, (void *)&regs->DSI_VM_CMD_CON,
                 AS_UINT32(DSI_REG_BASE[i] + DISP_REG_DSI_VM_CMD_CON));
    }

    return DSI_STATUS_OK;
}

enum DSI_STATUS DSI_RestoreRegisters(enum DISP_MODULE_ENUM module, void *cmdq)
{

    int i = 0;
    struct DSI_REGS_TYPE *regs = NULL;

    for (i = DSI_MODULE_BEGIN(module); i <= DSI_MODULE_END(module); i++) {
        regs = &(_dsi_context[i].regBackup);

        DSI_OUTREG32(cmdq, DSI_REG_BASE[i] + DISP_REG_DSI_INTEN, AS_UINT32(&regs->DSI_INTEN));
        DSI_OUTREG32(cmdq, DSI_REG_BASE[i] + DISP_REG_DSI_MODE_CON,
                 AS_UINT32(&regs->DSI_MODE_CTRL));
        DSI_OUTREG32(cmdq, DSI_REG_BASE[i] + DISP_REG_DSI_TXRX_CON,
                 AS_UINT32(&regs->DSI_TXRX_CTRL) & 0xFFFFFFC3);
        DSI_OUTREG32(cmdq, DSI_REG_BASE[i] + DISP_REG_DSI_PSCON, AS_UINT32(&regs->DSI_PSCTRL));

        DSI_OUTREG32(cmdq, DSI_REG_BASE[i] + DISP_REG_DSI_VSA_NL, AS_UINT32(&regs->DSI_VSA_NL));
        DSI_OUTREG32(cmdq, DSI_REG_BASE[i] + DISP_REG_DSI_VBP_NL, AS_UINT32(&regs->DSI_VBP_NL));
        DSI_OUTREG32(cmdq, DSI_REG_BASE[i] + DISP_REG_DSI_VFP_NL, AS_UINT32(&regs->DSI_VFP_NL));
        DSI_OUTREG32(cmdq, DSI_REG_BASE[i] + DISP_REG_DSI_VACT_NL, AS_UINT32(&regs->DSI_VACT_NL));

        DSI_OUTREG32(cmdq, DSI_REG_BASE[i] + DISP_REG_DSI_HSA_WC, AS_UINT32(&regs->DSI_HSA_WC));
        DSI_OUTREG32(cmdq, DSI_REG_BASE[i] + DISP_REG_DSI_HBP_WC, AS_UINT32(&regs->DSI_HBP_WC));
        DSI_OUTREG32(cmdq, DSI_REG_BASE[i] + DISP_REG_DSI_HFP_WC, AS_UINT32(&regs->DSI_HFP_WC));
        DSI_OUTREG32(cmdq, DSI_REG_BASE[i] + DISP_REG_DSI_BLLP_WC, AS_UINT32(&regs->DSI_BLLP_WC));

        DSI_OUTREG32(cmdq, DSI_REG_BASE[i] + DISP_REG_DSI_HSTX_CKLP_WC,
                 AS_UINT32(&regs->DSI_HSTX_CKL_WC));
        DSI_OUTREG32(cmdq, DSI_REG_BASE[i] + DISP_REG_DSI_MEM_CONTI,
                 AS_UINT32(&regs->DSI_MEM_CONTI));

        DSI_OUTREG32(cmdq,  DSI_REG_BASE[i] + DISP_REG_DSI_PHY_TIMCON0,
                 AS_UINT32(&regs->DSI_PHY_TIMECON0));
        DSI_OUTREG32(cmdq,  DSI_REG_BASE[i] + DISP_REG_DSI_PHY_TIMCON1,
                 AS_UINT32(&regs->DSI_PHY_TIMECON1));
        DSI_OUTREG32(cmdq,  DSI_REG_BASE[i] + DISP_REG_DSI_PHY_TIMCON2,
                 AS_UINT32(&regs->DSI_PHY_TIMECON2));
        DSI_OUTREG32(cmdq,  DSI_REG_BASE[i] + DISP_REG_DSI_PHY_TIMCON3,
                 AS_UINT32(&regs->DSI_PHY_TIMECON3));
        DSI_OUTREG32(cmdq,  DSI_REG_BASE[i] + DISP_REG_DSI_VM_CMD_CON,
                AS_UINT32(&regs->DSI_VM_CMD_CON));

        DDPMSG("%s VM_CMD_EN %d TS_VFP_EN %d\n", __func__,
            REG_FLD_VAL_GET(DSI_VM_CMD_CON_FLD_VM_CMD_EN,
                AS_UINT32(&regs->DSI_VM_CMD_CON)),
            REG_FLD_VAL_GET(DSI_VM_CMD_CON_FLD_TS_VFP_EN,
                AS_UINT32(&regs->DSI_VM_CMD_CON)));
    }
    return DSI_STATUS_OK;
}

void DSI_DPHY_clk_switch(enum DISP_MODULE_ENUM module, void *cmdq, int on)
{
#ifndef MACH_FPGA
    int i = 0;

    ASSERT(cmdq == NULL);

    if (on) {
        i = DSI_MODULE_BEGIN(module);
        DSI_PHY_clk_setting(module, cmdq, &(_dsi_context[i].dsi_params));
    } else {
        for (i = DSI_MODULE_BEGIN(module); i <= DSI_MODULE_END(module); i++) {
        /* disable mipi clock */
        /* step 0 */
        /* PLL DISABLE */
        MIPITX_OUTREGBIT(DSI_PHY_REG[i]+MIPITX_PLL_CON1, FLD_RG_DSI_PLL_EN, 0);

        /* step 1 */
        /* SDM_RWR_ON / SDM_ISO_EN */
        MIPITX_OUTREGBIT(DSI_PHY_REG[i]+MIPITX_PLL_PWR, FLD_AD_DSI_PLL_SDM_ISO_EN, 1);
        MIPITX_OUTREGBIT(DSI_PHY_REG[i]+MIPITX_PLL_PWR, FLD_AD_DSI_PLL_SDM_PWR_ON, 0);

        /* Switch ON each Lane */
        MIPITX_OUTREGBIT(DSI_PHY_REG[i]+MIPITX_D0_SW_CTL_EN,
            FLD_DSI_D0_SW_CTL_EN, 1);
        MIPITX_OUTREGBIT(DSI_PHY_REG[i]+MIPITX_D1_SW_CTL_EN,
            FLD_DSI_D1_SW_CTL_EN, 1);
        MIPITX_OUTREGBIT(DSI_PHY_REG[i]+MIPITX_D2_SW_CTL_EN,
            FLD_DSI_D2_SW_CTL_EN, 1);
        MIPITX_OUTREGBIT(DSI_PHY_REG[i]+MIPITX_D3_SW_CTL_EN,
            FLD_DSI_D3_SW_CTL_EN, 1);
        MIPITX_OUTREGBIT(DSI_PHY_REG[i]+MIPITX_CK_SW_CTL_EN,
            FLD_DSI_CK_SW_CTL_EN, 1);

        /* step 2 */
        MIPITX_OUTREG32(DSI_PHY_REG[i]+MIPITX_LANE_CON, 0x3FFF0180); /* BG_LPF_EN=0 */
        MIPITX_OUTREG32(DSI_PHY_REG[i]+MIPITX_LANE_CON, 0x3FFF0100); /* BG_CORE_EN=1 */

        /* spin(1000); */
        }
    }
#endif
}

void DSI_CPHY_clk_switch(enum DISP_MODULE_ENUM module, void *cmdq, int on)
{
#ifndef MACH_FPGA
    int i = 0;

    /* can't use cmdq for this */
    ASSERT(cmdq == NULL);

    if (on) {
        DSI_PHY_clk_setting(module, cmdq, &(_dsi_context[i].dsi_params));
    } else {
        for (i = DSI_MODULE_BEGIN(module); i <= DSI_MODULE_END(module); i++) {
            /* disable mipi clock */
            /* step 0 */
            /* PLL DISABLE */
            MIPITX_OUTREGBIT(DSI_PHY_REG[i]+MIPITX_PLL_CON1, FLD_RG_DSI_PLL_EN,    0);

            /* step 1 */
            /* SDM_RWR_ON / SDM_ISO_EN */
            MIPITX_OUTREGBIT(DSI_PHY_REG[i]+MIPITX_PLL_PWR, FLD_AD_DSI_PLL_SDM_ISO_EN, 1);
            MIPITX_OUTREGBIT(DSI_PHY_REG[i]+MIPITX_PLL_PWR, FLD_AD_DSI_PLL_SDM_PWR_ON, 0);

            /* step 2 */
            MIPITX_OUTREG32(DSI_PHY_REG[i]+MIPITX_LANE_CON, 0x3FFF0080); /* BG_LPF_EN=0 */
            MIPITX_OUTREG32(DSI_PHY_REG[i]+MIPITX_LANE_CON, 0x3FFF0000); /* BG_CORE_EN=1 */
            /* spin(1000); */
        }
    }
#endif
}

void DSI_PHY_clk_switch(enum DISP_MODULE_ENUM module, void *cmdq, int on)
{
    int i = 0;

    for (i = DSI_MODULE_BEGIN(module); i <= DSI_MODULE_END(module); i++) {
        if (_dsi_context[i].dsi_params.IsCphy)
            DSI_CPHY_clk_switch(module, cmdq, on);
        else
            DSI_DPHY_clk_switch(module, cmdq, on);
    }
}

enum DSI_STATUS DSI_BIST_Pattern_Test(enum DISP_MODULE_ENUM module, void *cmdq, bool enable,
                                        unsigned int color)
{
    int i = 0;
    void *temp;

    for (i = DSI_MODULE_BEGIN(module); i <= DSI_MODULE_END(module); i++) {
        if (enable) {
            DSI_OUTREG32(cmdq, &DSI_REG[i]->DSI_BIST_PATTERN, color);
            /* DSI_OUTREG32(&DSI_REG->DSI_BIST_CON, AS_UINT32(&temp_reg)); */
            /* DSI_OUTREGBIT(DSI_BIST_CON_REG, DSI_REG->DSI_BIST_CON, SELF_PAT_MODE, 1); */
            //DSI_OUTREGBIT(cmdq, struct DSI_BIST_CON_REG, DSI_REG[i]->DSI_BIST_CON,
            //          SELF_PAT_MODE, 1);
            DISP_REG_SET_FIELD(cmdq, DSI_BIST_CON_FLD_SELF_PAT_MODE,
                                DSI_REG_BASE[i] + DISP_REG_DSI_BIST_CON, 1);
            DISPMSG("%s SELF_PAT_MODE\n", __func__);

            if (!_dsi_is_video_mode(module)) {
                struct DSI_T0_INS t0;

                t0.CONFG = 0x09;
                t0.Data_ID = 0x39;
                t0.Data0 = 0x2c;
                t0.Data1 = 0;
                temp = &t0;

                //DSI_OUTREG32(cmdq, &DSI_CMDQ_REG[i]->data[0], AS_UINT32((void *)&t0));
                DSI_OUTREG32(cmdq, &DSI_CMDQ_REG[i]->data[0], AS_UINT32(temp));
                DSI_OUTREG32(cmdq, DSI_REG_BASE[i] + DISP_REG_DSI_CMDQ_CON, 1);

                /* DSI_OUTREGBIT(DSI_START_REG,DSI_REG->DSI_START,DSI_START,0); */
                DSI_OUTREG32(cmdq, DSI_REG_BASE[i] + DISP_REG_DSI_START, 0);
                DSI_OUTREG32(cmdq, DSI_REG_BASE[i] + DISP_REG_DSI_START, 1);
                /* DSI_OUTREGBIT(DSI_START_REG,DSI_REG->DSI_START,DSI_START, 1); */
            }
        } else {
            /* if disable dsi pattern, need enable mutex, can't just start dsi */
            /* so we just disable pattern bit, do not start dsi here */
            /* DSI_WaitForNotBusy(module, cmdq); */
            /* DSI_OUTREGBIT(cmdq, DSI_BIST_CON_REG, DSI_REG[i]->DSI_BIST_CON, SELF_PAT_MODE, 0); */
            DSI_OUTREG32(cmdq, DSI_REG_BASE[i] + DISP_REG_DSI_BIST_CON, 0x00);
        }

    }
    return DSI_STATUS_OK;
}

void DSI_Config_DPHY_VDO_Timing(enum DISP_MODULE_ENUM module, void *cmdq,
                                LCM_DSI_PARAMS *dsi_params)
{
    int i = 0;
    //unsigned int line_byte;
    unsigned int horizontal_sync_active_byte;
    unsigned int horizontal_backporch_byte;
    unsigned int horizontal_frontporch_byte;
    unsigned int horizontal_bllp_byte;
    unsigned int dsiTmpBufBpp;

    DISPFUNC();

    for (i = DSI_MODULE_BEGIN(module); i <= DSI_MODULE_END(module); i++) {
        if (dsi_params->data_format.format == LCM_DSI_FORMAT_RGB565)
            dsiTmpBufBpp = 2;
        else
            dsiTmpBufBpp = 3;

        DSI_OUTREG32(cmdq, DSI_REG_BASE[i] + DISP_REG_DSI_VSA_NL,
                        dsi_params->vertical_sync_active);
        DSI_OUTREG32(cmdq, DSI_REG_BASE[i] + DISP_REG_DSI_VBP_NL,
                        dsi_params->vertical_backporch);
        DSI_OUTREG32(cmdq, DSI_REG_BASE[i] + DISP_REG_DSI_VFP_NL,
                        dsi_params->vertical_frontporch);
        DSI_OUTREG32(cmdq, DSI_REG_BASE[i] + DISP_REG_DSI_VACT_NL,
                        dsi_params->vertical_active_line);

        // line_byte =
            // (dsi_params->horizontal_sync_active + dsi_params->horizontal_backporch +
             // dsi_params->horizontal_frontporch +
             // dsi_params->horizontal_active_pixel) * dsiTmpBufBpp;
        horizontal_sync_active_byte =
            (dsi_params->horizontal_sync_active * dsiTmpBufBpp - 4);

        if (dsi_params->mode == SYNC_EVENT_VDO_MODE
            || dsi_params->mode == BURST_VDO_MODE
            || dsi_params->switch_mode == SYNC_EVENT_VDO_MODE
            || dsi_params->switch_mode == BURST_VDO_MODE) {
            ASSERT((dsi_params->horizontal_backporch +
                dsi_params->horizontal_sync_active) * dsiTmpBufBpp > 9);
            horizontal_backporch_byte =
                ((dsi_params->horizontal_backporch +
                  dsi_params->horizontal_sync_active) * dsiTmpBufBpp - 10);
        } else {
            ASSERT(dsi_params->horizontal_sync_active * dsiTmpBufBpp > 9);
            horizontal_sync_active_byte =
                (dsi_params->horizontal_sync_active * dsiTmpBufBpp - 10);

            ASSERT(dsi_params->horizontal_backporch * dsiTmpBufBpp > 9);
            horizontal_backporch_byte =
                (dsi_params->horizontal_backporch * dsiTmpBufBpp - 10);
        }

        ASSERT(dsi_params->horizontal_frontporch * dsiTmpBufBpp > 11);
        horizontal_frontporch_byte =
            (dsi_params->horizontal_frontporch * dsiTmpBufBpp - 12);
        horizontal_bllp_byte = (dsi_params->horizontal_bllp * dsiTmpBufBpp);

        DSI_OUTREG32(cmdq, DSI_REG_BASE[i] + DISP_REG_DSI_HSA_WC,
                 ALIGN_TO((horizontal_sync_active_byte), 4));
        DSI_OUTREG32(cmdq, DSI_REG_BASE[i] + DISP_REG_DSI_HBP_WC,
                 ALIGN_TO((horizontal_backporch_byte), 4));
        DSI_OUTREG32(cmdq, DSI_REG_BASE[i] + DISP_REG_DSI_HFP_WC,
                 ALIGN_TO((horizontal_frontporch_byte), 4));
        DSI_OUTREG32(cmdq, DSI_REG_BASE[i] + DISP_REG_DSI_BLLP_WC,
                 ALIGN_TO((horizontal_bllp_byte), 4));
    }
}

void DSI_Config_CPHY_VDO_Timing(enum DISP_MODULE_ENUM module, void *cmdq,
                                LCM_DSI_PARAMS *dsi_params)
{
    int i = 0;
    //unsigned int line_byte;
    unsigned int horizontal_sync_active_byte;
    unsigned int horizontal_backporch_byte;
    unsigned int horizontal_frontporch_byte;
    unsigned int horizontal_bllp_byte;
    unsigned int dsiTmpBufBpp;
    unsigned int lane_num = 0;
    unsigned int data_phy_cycle;

    DISPFUNC();

    for (i = DSI_MODULE_BEGIN(module); i <= DSI_MODULE_END(module); i++) {
        if (dsi_params->data_format.format == LCM_DSI_FORMAT_RGB565)
            dsiTmpBufBpp = 2;
        else
            dsiTmpBufBpp = 3;

        switch (_dsi_context[i].dsi_params.LANE_NUM) {
        case LCM_ONE_LANE:
            lane_num = 1;
            break;
        case LCM_TWO_LANE:
            lane_num = 2;
            break;
        case LCM_THREE_LANE:
            lane_num = 3;
            break;
        case LCM_FOUR_LANE:
            lane_num = 4;
            break;
        default:
            break;
        }

        DSI_OUTREG32(cmdq, DSI_REG_BASE[i] + DISP_REG_DSI_VSA_NL,
                        dsi_params->vertical_sync_active);
        DSI_OUTREG32(cmdq, DSI_REG_BASE[i] + DISP_REG_DSI_VBP_NL,
                        dsi_params->vertical_backporch);
        DSI_OUTREG32(cmdq, DSI_REG_BASE[i] + DISP_REG_DSI_VFP_NL,
                        dsi_params->vertical_frontporch);
        DSI_OUTREG32(cmdq, DSI_REG_BASE[i] + DISP_REG_DSI_VACT_NL,
                        dsi_params->vertical_active_line);

        horizontal_sync_active_byte =
            (dsi_params->horizontal_sync_active * dsiTmpBufBpp - 4);

        if (dsi_params->mode == SYNC_EVENT_VDO_MODE
            || dsi_params->mode == BURST_VDO_MODE
            || dsi_params->switch_mode == SYNC_EVENT_VDO_MODE
            || dsi_params->switch_mode == BURST_VDO_MODE) {
            ASSERT((dsi_params->horizontal_backporch +
                dsi_params->horizontal_sync_active) * dsiTmpBufBpp > 9);
            horizontal_backporch_byte =
                ((dsi_params->horizontal_backporch +
                  dsi_params->horizontal_sync_active) * dsiTmpBufBpp - 10);
        } else {
            if (dsi_params->horizontal_sync_active * dsiTmpBufBpp < 10
                * lane_num + 26 + 5)
                horizontal_sync_active_byte = 4;
            else
                horizontal_sync_active_byte =
                        (dsi_params->horizontal_sync_active *
                         dsiTmpBufBpp - 10 * lane_num - 26);

            if (dsi_params->horizontal_backporch * dsiTmpBufBpp < 12
                * lane_num + 26 + 5)
                horizontal_backporch_byte = 4;
            else
                horizontal_backporch_byte =
                        (dsi_params->horizontal_backporch *
                         dsiTmpBufBpp - 12 * lane_num - 26);
        }
        // data_phy_cycle = DISP_REG_GET_FIELD(DSI_PHY_TIMCON0_FLD_DA_HS_PREP,
                                // DSI_REG_BASE[i] + DISP_REG_DSI_PHY_TIMCON0) +
            // DISP_REG_GET_FIELD(DSI_PHY_TIMCON0_FLD_DA_HS_ZERO,
                    // DSI_REG_BASE[i] + DISP_REG_DSI_PHY_TIMCON0) +
            // DISP_REG_GET_FIELD(DSI_PHY_TIMCON1_FLD_DA_HS_EXIT,
                    // DSI_REG_BASE[i] + DISP_REG_DSI_PHY_TIMCON1) +
            // DISP_REG_GET_FIELD(DSI_PHY_TIMCON0_FLD_LPX, DSI_REG_BASE[i]
                    // + DISP_REG_DSI_PHY_TIMCON0) + 5;
        data_phy_cycle = _dsi_context[i].data_phy_cycle;

        if (dsi_params->horizontal_frontporch * dsiTmpBufBpp < 8 *
                lane_num + 28 + 2 * data_phy_cycle * lane_num  + 9)
            horizontal_frontporch_byte = 8;
        else if ((dsi_params->horizontal_frontporch * dsiTmpBufBpp > 8 *
                lane_num + 28 + 2 * data_phy_cycle * lane_num  + 8) &&
            (dsi_params->horizontal_frontporch * dsiTmpBufBpp < 8 *
                lane_num + 28 + 2 * data_phy_cycle * lane_num + 2 *
            (_dsi_context[i].HS_TRAIL + 1) * lane_num - 6 * lane_num - 12))
            horizontal_frontporch_byte = 2 * (_dsi_context[i].HS_TRAIL + 1) *
                lane_num - 6 * lane_num - 12;
        else
            horizontal_frontporch_byte = dsi_params->horizontal_frontporch *
              dsiTmpBufBpp - 8 * lane_num - 28 - 2 * data_phy_cycle * lane_num;
        horizontal_bllp_byte = (lane_num * 16);

        DSI_OUTREG32(cmdq, DSI_REG_BASE[i] + DISP_REG_DSI_HSA_WC,
                 ALIGN_TO((horizontal_sync_active_byte), 2));
        DSI_OUTREG32(cmdq, DSI_REG_BASE[i] + DISP_REG_DSI_HBP_WC,
                 ALIGN_TO((horizontal_backporch_byte), 2));
        DSI_OUTREG32(cmdq, DSI_REG_BASE[i] + DISP_REG_DSI_HFP_WC,
                 ALIGN_TO((horizontal_frontporch_byte), 2));
        DSI_OUTREG32(cmdq, DSI_REG_BASE[i] + DISP_REG_DSI_BLLP_WC,
                ALIGN_TO((horizontal_bllp_byte), 2));
    }
}

void DSI_Config_VDO_Timing(enum DISP_MODULE_ENUM module,
                           void *cmdq, LCM_DSI_PARAMS *dsi_params)
{
    int i = 0;

    for (i = DSI_MODULE_BEGIN(module); i <= DSI_MODULE_END(module); i++) {
        if (_dsi_context[i].dsi_params.IsCphy)
            DSI_Config_CPHY_VDO_Timing(module, cmdq, dsi_params);
        else
            DSI_Config_DPHY_VDO_Timing(module, cmdq, dsi_params);
    }
}


void DSI_PHY_CLK_LP_PerLine_config(enum DISP_MODULE_ENUM module, void *cmdq,
                                    LCM_DSI_PARAMS *dsi_params)
{
    int i;
    void *temp;

     // struct DSI_PHY_TIMCON0_REG timcon0; // LPX
     // struct DSI_PHY_TIMCON2_REG timcon2; // CLK_HS_TRAIL, CLK_HS_ZERO
     // struct DSI_PHY_TIMCON3_REG timcon3; // CLK_HS_EXIT, CLK_HS_POST, CLK_HS_PREP
     // struct DSI_HSA_WC_REG          hsa;
     // struct DSI_HBP_WC_REG          hbp;
     // struct DSI_HFP_WC_REG          hfp, new_hfp;
     // struct DSI_BLLP_WC_REG         bllp;
     // struct DSI_PSCTRL_REG          ps;

    u32 timcon0; // LPX
    u32 timcon2; // CLK_HS_TRAIL, CLK_HS_ZERO
    u32 timcon3; // CLK_HS_EXIT, CLK_HS_POST, CLK_HS_PREP
    u32 hsa;
    u32 hbp;
    u32 hfp, new_hfp;
    u32 bllp;
    u32 ps;
    u32 hstx_ckl_wc = 0;
    u32 new_hstx_ckl_wc = 0;
    u32 v_a, v_b, v_c, lane_num;
    LCM_DSI_MODE_CON dsi_mode;

    for (i = DSI_MODULE_BEGIN(module); i <= DSI_MODULE_END(module); i++) {
        lane_num =  dsi_params->LANE_NUM;
        dsi_mode =  dsi_params->mode;

        if (dsi_mode == CMD_MODE)
            continue;


        // vdo mode
        temp = &hsa;
        DSI_OUTREG32(cmdq, temp, AS_UINT32(DSI_REG_BASE[i] + DISP_REG_DSI_HSA_WC));
        temp = &hbp;
        DSI_OUTREG32(cmdq, temp, AS_UINT32(DSI_REG_BASE[i] + DISP_REG_DSI_HBP_WC));
        temp = &hfp;
        DSI_OUTREG32(cmdq, temp, AS_UINT32(DSI_REG_BASE[i] + DISP_REG_DSI_HFP_WC));
        temp = &bllp;
        DSI_OUTREG32(cmdq, temp, AS_UINT32(DSI_REG_BASE[i] + DISP_REG_DSI_BLLP_WC));
        temp = &ps;
        DSI_OUTREG32(cmdq, temp, AS_UINT32(DSI_REG_BASE[i] + DISP_REG_DSI_PSCON));
        temp = &hstx_ckl_wc;
        DSI_OUTREG32(cmdq, temp, AS_UINT32(DSI_REG_BASE[i] + DISP_REG_DSI_HSTX_CKLP_WC));
        temp = &timcon0;
        DSI_OUTREG32(cmdq, temp, AS_UINT32(DSI_REG_BASE[i] + DISP_REG_DSI_PHY_TIMCON0));
        temp = &timcon2;
        DSI_OUTREG32(cmdq, temp, AS_UINT32(DSI_REG_BASE[i] + DISP_REG_DSI_PHY_TIMCON2));
        temp = &timcon3;
        DSI_OUTREG32(cmdq, temp, AS_UINT32(DSI_REG_BASE[i] + DISP_REG_DSI_PHY_TIMCON3));

        // 1. sync_pulse_mode
        //    Total    WC(A) = HSA_WC + HBP_WC + HFP_WC + PS_WC + 32
        //    CLK init WC(B) = (CLK_HS_EXIT + LPX + CLK_HS_PREP + CLK_HS_ZERO) * lane_num
        //    CLK end  WC(C) = (CLK_HS_POST + CLK_HS_TRAIL) * lane_num
        //    HSTX_CKLP_WC = A - B
        //    Limitation: B + C < HFP_WC
        if (dsi_mode == SYNC_PULSE_VDO_MODE) {

            //v_a = hsa.HSA_WC + hbp.HBP_WC + hfp.HFP_WC + ps.DSI_PS_WC +32;
            //v_b = (timcon3.CLK_HS_EXIT + timcon0.LPX + timcon3.CLK_HS_PRPR
            //          + timcon2.CLK_ZERO) * lane_num;
            //v_c = (timcon3.CLK_HS_POST + timcon2.CLK_TRAIL)* lane_num;
            v_a = REG_FLD_VAL_GET(DSI_HSA_WC_FLD_DSI_HSA_WC, (u32)hsa)
                  + REG_FLD_VAL_GET(DSI_HBP_WC_FLD_DSI_HBP_WC, (u32)hbp)
                + REG_FLD_VAL_GET(DSI_HFP_WC_FLD_DSI_HFP_WC, (u32)hfp)
                  + REG_FLD_VAL_GET(DSI_PSCON_FLD_DSI_PS_WC, (u32)ps)
                + 32;
            v_b = (REG_FLD_VAL_GET(DSI_PHY_TIMCON3_FLD_CLK_HS_EXIT, timcon3)
                + REG_FLD_VAL_GET(DSI_PHY_TIMCON0_FLD_LPX, timcon0)
                + REG_FLD_VAL_GET(DSI_PHY_TIMCON3_FLD_CLK_HS_PREP, timcon3)
                + REG_FLD_VAL_GET(DSI_PHY_TIMCON2_FLD_CLK_HS_ZERO, timcon2)) * lane_num;
            v_c = (REG_FLD_VAL_GET(DSI_PHY_TIMCON3_FLD_CLK_HS_POST, timcon3)
                + REG_FLD_VAL_GET(DSI_PHY_TIMCON2_FLD_CLK_HS_TRAIL, timcon2)) * lane_num;

            DISPCHECK("===>v_a-v_b=0x%x,HSTX_CKLP_WC=0x%x\n", (v_a - v_b), hstx_ckl_wc);
            DISPCHECK("===>v_b+v_c=0x%x,HFP_WC=0x%x\n", (v_b+v_c), hfp);
            DISPCHECK("===>Will Reconfig in order to fulfill LP clock lane per line\n");

            //B+C < HFP ,here diff is 0x10;
            DSI_OUTREG32(cmdq, DSI_REG_BASE[i] + DISP_REG_DSI_HFP_WC, (v_b+v_c+DIFF_CLK_LANE_LP));
            temp = &new_hfp;
            DSI_OUTREG32(cmdq, temp, AS_UINT32(DSI_REG_BASE[i] + DISP_REG_DSI_HFP_WC));
            //v_a = hsa.HSA_WC + hbp.HBP_WC + new_hfp.HFP_WC + ps.DSI_PS_WC +32;

            v_a = REG_FLD_VAL_GET(DSI_HSA_WC_FLD_DSI_HSA_WC, hsa)
                + REG_FLD_VAL_GET(DSI_HBP_WC_FLD_DSI_HBP_WC, hbp)
                + REG_FLD_VAL_GET(DSI_HFP_WC_FLD_DSI_HFP_WC, new_hfp)
                + REG_FLD_VAL_GET(DSI_PSCON_FLD_DSI_PS_WC, ps) + 32;
            DSI_OUTREG32(cmdq, DSI_REG_BASE[i] + DISP_REG_DSI_HSTX_CKLP_WC, (v_a - v_b));
            DSI_OUTREG32(cmdq, &new_hstx_ckl_wc,
                            AS_UINT32(DSI_REG_BASE[i] + DISP_REG_DSI_HSTX_CKLP_WC));
            //DISPCHECK("===>new HSTX_CKL_WC=0x%x, HFP_WC=0x%x\n", new_hstx_ckl_wc, new_hfp.HFP_WC);
            DISPCHECK("===>new HSTX_CKL_WC=0x%x, HFP_WC=0x%x\n", new_hstx_ckl_wc,
                                            REG_FLD_VAL_GET(DSI_HFP_WC_FLD_DSI_HFP_WC, new_hfp));
        }
        // 2. sync_event_mode
        //    Total    WC(A) = HBP_WC + HFP_WC + PS_WC + 26
        //    CLK init WC(B) = (CLK_HS_EXIT + LPX + CLK_HS_PREP + CLK_HS_ZERO) * lane_num
        //    CLK end  WC(C) = (CLK_HS_POST + CLK_HS_TRAIL) * lane_num
        //    HSTX_CKLP_WC = A - B
        //    Limitation: B + C < HFP_WC
        else if (dsi_mode == SYNC_EVENT_VDO_MODE) {
            //v_a = hbp.HBP_WC + hfp.HFP_WC + ps.DSI_PS_WC +26;
            //v_b = (timcon3.CLK_HS_EXIT + timcon0.LPX + timcon3.CLK_HS_PRPR + timcon2.CLK_ZERO)
            //        * lane_num;
            //v_c = (timcon3.CLK_HS_POST + timcon2.CLK_TRAIL)* lane_num;
            v_a = REG_FLD_VAL_GET(DSI_HBP_WC_FLD_DSI_HBP_WC, hbp)
                + REG_FLD_VAL_GET(DSI_HFP_WC_FLD_DSI_HFP_WC, hfp)
                + REG_FLD_VAL_GET(DSI_PSCON_FLD_DSI_PS_WC, ps) + 26;
            v_b = (REG_FLD_VAL_GET(DSI_PHY_TIMCON3_FLD_CLK_HS_EXIT, timcon3)
                + REG_FLD_VAL_GET(DSI_PHY_TIMCON0_FLD_LPX, timcon0)
                + REG_FLD_VAL_GET(DSI_PHY_TIMCON3_FLD_CLK_HS_PREP, timcon3)
                + REG_FLD_VAL_GET(DSI_PHY_TIMCON2_FLD_CLK_HS_ZERO, timcon2)) * lane_num;
            v_c = (REG_FLD_VAL_GET(DSI_PHY_TIMCON3_FLD_CLK_HS_POST, timcon3)
                + REG_FLD_VAL_GET(DSI_PHY_TIMCON2_FLD_CLK_HS_TRAIL, timcon2)) * lane_num;


            DISPCHECK("===>v_a-v_b=0x%x,HSTX_CKLP_WC=0x%x\n", (v_a - v_b), hstx_ckl_wc);
            DISPCHECK("===>v_b+v_c=0x%x,HFP_WC=0x%x\n", (v_b+v_c), hfp);
            DISPCHECK("===>Will Reconfig in order to fulfill LP clock lane per line\n");

            //B+C < HFP ,here diff is 0x10;
            DSI_OUTREG32(cmdq, DSI_REG_BASE[i] + DISP_REG_DSI_HFP_WC, (v_b+v_c+DIFF_CLK_LANE_LP));

            temp = &new_hfp;
            DSI_OUTREG32(cmdq, temp, AS_UINT32(DSI_REG_BASE[i] + DISP_REG_DSI_HFP_WC));
            //v_a = hbp.HBP_WC + new_hfp.HFP_WC + ps.DSI_PS_WC +26;
            v_a = REG_FLD_VAL_GET(DSI_HBP_WC_FLD_DSI_HBP_WC, hbp)
                + REG_FLD_VAL_GET(DSI_HFP_WC_FLD_DSI_HFP_WC, new_hfp)
                + REG_FLD_VAL_GET(DSI_PSCON_FLD_DSI_PS_WC, ps) + 26;

            DSI_OUTREG32(cmdq, DSI_REG_BASE[i] + DISP_REG_DSI_HSTX_CKLP_WC, (v_a - v_b));
            DSI_OUTREG32(cmdq, &new_hstx_ckl_wc,
                            AS_UINT32(DSI_REG_BASE[i] + DISP_REG_DSI_HSTX_CKLP_WC));
            //DISPCHECK("===>new HSTX_CKL_WC=0x%x, HFP_WC=0x%x\n", new_hstx_ckl_wc, new_hfp.HFP_WC);
            DISPCHECK("===>new HSTX_CKL_WC=0x%x, HFP_WC=0x%x\n", new_hstx_ckl_wc,
                        REG_FLD_VAL_GET(DSI_HFP_WC_FLD_DSI_HFP_WC, new_hfp));

        }
        // 3. burst_mode
        //    Total    WC(A) = HBP_WC + HFP_WC + PS_WC + BLLP_WC + 32
        //    CLK init WC(B) = (CLK_HS_EXIT + LPX + CLK_HS_PREP + CLK_HS_ZERO) * lane_num
        //    CLK end  WC(C) = (CLK_HS_POST + CLK_HS_TRAIL) * lane_num
        //    HSTX_CKLP_WC = A - B
        //    Limitation: B + C < HFP_WC
        else if (dsi_mode == BURST_VDO_MODE) {
            //v_a = hbp.HBP_WC + hfp.HFP_WC + ps.DSI_PS_WC +bllp.BLLP_WC+32;
            //v_b = (timcon3.CLK_HS_EXIT + timcon0.LPX + timcon3.CLK_HS_PRPR + timcon2.CLK_ZERO)
            //            * lane_num;
            //v_c = (timcon3.CLK_HS_POST + timcon2.CLK_TRAIL)* lane_num;
            v_a = REG_FLD_VAL_GET(DSI_HBP_WC_FLD_DSI_HBP_WC, hbp)
                + REG_FLD_VAL_GET(DSI_HFP_WC_FLD_DSI_HFP_WC, hfp)
                + REG_FLD_VAL_GET(DSI_PSCON_FLD_DSI_PS_WC, ps)
                + REG_FLD_VAL_GET(DSI_BLLP_WC_FLD_DSI_BLLP_WC, bllp) + 32;
            v_b = (REG_FLD_VAL_GET(DSI_PHY_TIMCON3_FLD_CLK_HS_EXIT, timcon3)
                + REG_FLD_VAL_GET(DSI_PHY_TIMCON3_FLD_CLK_HS_PREP, timcon3)
                + REG_FLD_VAL_GET(DSI_PHY_TIMCON2_FLD_CLK_HS_ZERO, timcon2)) * lane_num;
            v_c = (REG_FLD_VAL_GET(DSI_PHY_TIMCON3_FLD_CLK_HS_POST, timcon3)
                + REG_FLD_VAL_GET(DSI_PHY_TIMCON2_FLD_CLK_HS_TRAIL, timcon2)) * lane_num;

            DISPCHECK("===>v_a-v_b=0x%x,HSTX_CKLP_WC=0x%x\n", (v_a - v_b), hstx_ckl_wc);
            DISPCHECK("===>v_b+v_c=0x%x,HFP_WC=0x%x\n", (v_b+v_c), hfp);
            DISPCHECK("===>Will Reconfig in order to fulfill LP clock lane per line\n");

            //B+C < HFP ,here diff is 0x10;
            DSI_OUTREG32(cmdq, DSI_REG_BASE[i] + DISP_REG_DSI_HFP_WC, (v_b+v_c+DIFF_CLK_LANE_LP));
            DSI_OUTREG32(cmdq, &new_hfp, AS_UINT32(DSI_REG_BASE[i] + DISP_REG_DSI_HFP_WC));
            //v_a = hbp.HBP_WC + new_hfp.HFP_WC + ps.DSI_PS_WC +bllp.BLLP_WC+32;
            v_a = REG_FLD_VAL_GET(DSI_HBP_WC_FLD_DSI_HBP_WC, hbp)
                + REG_FLD_VAL_GET(DSI_HFP_WC_FLD_DSI_HFP_WC, new_hfp)
                + REG_FLD_VAL_GET(DSI_PSCON_FLD_DSI_PS_WC, ps)
                + REG_FLD_VAL_GET(DSI_BLLP_WC_FLD_DSI_BLLP_WC, bllp) + 32;
            DSI_OUTREG32(cmdq, DSI_REG_BASE[i] + DISP_REG_DSI_HSTX_CKLP_WC, (v_a - v_b));
            DSI_OUTREG32(cmdq, &new_hstx_ckl_wc,
                            AS_UINT32(DSI_REG_BASE[i] + DISP_REG_DSI_HSTX_CKLP_WC));
            //DISPCHECK("===>new HSTX_CKL_WC=0x%x, HFP_WC=0x%x\n", new_hstx_ckl_wc, new_hfp.HFP_WC);
            DISPCHECK("===>new HSTX_CKL_WC=0x%x, HFP_WC=0x%x\n", new_hstx_ckl_wc,
                        REG_FLD_VAL_GET(DSI_HFP_WC_FLD_DSI_HFP_WC, new_hfp));
        }
    }

}

int _dsi_ps_type_to_bpp(LCM_PS_TYPE ps)
{
    switch (ps) {
    case LCM_PACKED_PS_16BIT_RGB565:
        return 2;
    case LCM_LOOSELY_PS_18BIT_RGB666:
        return 3;
    case LCM_PACKED_PS_24BIT_RGB888:
        return 3;
    case LCM_PACKED_PS_18BIT_RGB666:
        return 3;
    }
    return 0;
}

enum DSI_STATUS DSI_PS_Control(enum DISP_MODULE_ENUM module, void *cmdq, LCM_DSI_PARAMS *dsi_params,
                                 int w, int h)
{
    int i = 0;
    unsigned int params_ps;
    unsigned int ps_sel_bitvalue = 0;
    unsigned int ps_wc_adjust = 0;
    unsigned int ps_wc = 0;
    unsigned int val = 0;

    DISPFUNC();
    params_ps = dsi_params->PS;
    ASSERT(params_ps <= PACKED_PS_18BIT_RGB666);

    if ((int)(dsi_params->PS) > (int)(LOOSELY_PS_18BIT_RGB666))
        ps_sel_bitvalue = (5 - dsi_params->PS);
    else
        ps_sel_bitvalue = dsi_params->PS;

    if (module == DISP_MODULE_DSIDUAL)
        w = w / 2;

    for (i = DSI_MODULE_BEGIN(module); i <= DSI_MODULE_END(module); i++) {
        //DSI_OUTREGBIT(cmdq, struct DSI_VACT_NL_REG, DSI_REG[i]->DSI_VACT_NL, VACT_NL, h);
        DISP_REG_SET_FIELD(cmdq, DSI_VACT_NL_FLD_VACT_NL,
                            DSI_REG_BASE[i] + DISP_REG_DSI_VACT_NL, h);
        if (dsi_params->ufoe_enable && dsi_params->ufoe_params.lr_mode_en != 1) {
            if (dsi_params->ufoe_params.compress_ratio == 3) {
                unsigned int ufoe_internal_width = w + w % 4;

                if (ufoe_internal_width % 3 == 0) {
                    ps_wc = (ufoe_internal_width / 3) * _dsi_ps_type_to_bpp(dsi_params->PS);
                } else {
                    unsigned int temp_w = ufoe_internal_width / 3 + 1;

                    temp_w = ((temp_w % 2) == 1) ? (temp_w + 1) : temp_w;
                    ps_wc = temp_w  * _dsi_ps_type_to_bpp(dsi_params->PS);
                }
            } else { /* 1/2 */
                ps_wc = (w + w % 4) / 2 * _dsi_ps_type_to_bpp(dsi_params->PS);
            }
        } else if (dsi_params->dsc_enable) {
            ps_wc =
                ((dsi_params->dsc_params.chunk_size + 2) / 3) * 3;
            if (dsi_params->dsc_params.slice_mode == 1)
                ps_wc *= 2;
        } else {
            ps_wc = w * _dsi_ps_type_to_bpp(dsi_params->PS);
        }

        if (ps_wc_adjust)
            ps_wc *= dsi_params->packet_size_mult;

#if 0
        DSI_OUTREGBIT(cmdq, struct DSI_PSCTRL_REG, DSI_REG[i]->DSI_PSCTRL, DSI_PS_WC, ps_wc);
        DSI_OUTREGBIT(cmdq, struct DSI_PSCTRL_REG, DSI_REG[i]->DSI_PSCTRL, DSI_PS_SEL,
                  ps_sel_bitvalue);
#endif

        DISP_REG_SET_FIELD(cmdq, DSI_PSCON_FLD_DSI_PS_WC, DSI_REG_BASE[i] + DISP_REG_DSI_PSCON,
                            ps_wc);

        if (dsi_params->dsc_enable)
            ps_sel_bitvalue = 5;

        DISP_REG_SET_FIELD(cmdq, DSI_PSCON_FLD_DSI_PS_SEL, DSI_REG_BASE[i] + DISP_REG_DSI_PSCON,
                            ps_sel_bitvalue);

        if (dsi_params->dsc_enable)
            val = (h << 16) + ps_wc / 3;
        else
            val = (h << 16) + w;
        DSI_OUTREG32(cmdq, DSI_REG_BASE[i] + DISP_REG_DSI_SIZE_CON, val);
    }

    return DSI_STATUS_OK;
}

enum DSI_STATUS DSI_TXRX_Control(enum DISP_MODULE_ENUM module, void *cmdq,
                                    LCM_DSI_PARAMS *dsi_params)
{
    int i = 0;
    unsigned int lane_num_bitvalue = 0;
    int lane_num = dsi_params->LANE_NUM;
    int vc_num = 0;
    bool null_packet_en = false;
    bool dis_eotp_en = false;
    bool hstx_cklp_en = dsi_params->cont_clock ? false : true;
    int max_return_size = 0;

    switch (lane_num) {
    case LCM_ONE_LANE:
        lane_num_bitvalue = 0x1;
        break;
    case LCM_TWO_LANE:
        lane_num_bitvalue = 0x3;
        break;
    case LCM_THREE_LANE:
        lane_num_bitvalue = 0x7;
        break;
    case LCM_FOUR_LANE:
        lane_num_bitvalue = 0xF;
        break;
    }

    for (i = DSI_MODULE_BEGIN(module); i <= DSI_MODULE_END(module); i++) {
        // DSI_OUTREGBIT(cmdq, struct DSI_TXRX_CTRL_REG,
            // DSI_REG[i]->DSI_TXRX_CTRL, VC_NUM, vc_num);
        // DSI_OUTREGBIT(cmdq, struct DSI_TXRX_CTRL_REG,
            // DSI_REG[i]->DSI_TXRX_CTRL, DIS_EOT, dis_eotp_en);
        // DSI_OUTREGBIT(cmdq, struct DSI_TXRX_CTRL_REG,
            // DSI_REG[i]->DSI_TXRX_CTRL, BLLP_EN, null_packet_en);
        // DSI_OUTREGBIT(cmdq, struct DSI_TXRX_CTRL_REG,
            // DSI_REG[i]->DSI_TXRX_CTRL, MAX_RTN_SIZE, max_return_size);
        // DSI_OUTREGBIT(cmdq, struct DSI_TXRX_CTRL_REG,
            // DSI_REG[i]->DSI_TXRX_CTRL, HSTX_CKLP_EN, hstx_cklp_en);
        // DSI_OUTREGBIT(cmdq, struct DSI_TXRX_CTRL_REG,
            // DSI_REG[i]->DSI_TXRX_CTRL, LANE_NUM, lane_num_bitvalue);

        DISP_REG_SET_FIELD(cmdq, DSI_TXRX_CON_FLD_VC_NUM,
                            DSI_REG_BASE[i] + DISP_REG_DSI_TXRX_CON, vc_num);
        DISP_REG_SET_FIELD(cmdq, DSI_TXRX_CON_FLD_HSTX_DIS_EOT,
                            DSI_REG_BASE[i] + DISP_REG_DSI_TXRX_CON, dis_eotp_en);
        DISP_REG_SET_FIELD(cmdq, DSI_TXRX_CON_FLD_HSTX_BLLP_EN,
                            DSI_REG_BASE[i] + DISP_REG_DSI_TXRX_CON, null_packet_en);
        DISP_REG_SET_FIELD(cmdq, DSI_TXRX_CON_FLD_MAX_RTN_SIZE,
                            DSI_REG_BASE[i] + DISP_REG_DSI_TXRX_CON, max_return_size);
        DISP_REG_SET_FIELD(cmdq, DSI_TXRX_CON_FLD_HSTX_CKLP_EN,
                            DSI_REG_BASE[i] + DISP_REG_DSI_TXRX_CON, hstx_cklp_en);
        DISP_REG_SET_FIELD(cmdq, DSI_TXRX_CON_FLD_LANE_NUM,
                            DSI_REG_BASE[i] + DISP_REG_DSI_TXRX_CON, lane_num_bitvalue);

        DSI_OUTREG32(cmdq, DSI_REG_BASE[i] + DISP_REG_DSI_MEM_CONTI, DSI_WMEM_CONTI);
        if (dsi_params->mode == CMD_MODE
            || (dsi_params->mode != CMD_MODE && dsi_params->eint_disable)) {
            if (dsi_params->ext_te_edge == LCM_POLARITY_FALLING) {
                /*use ext te falling edge */
                //DSI_OUTREGBIT(cmdq, struct DSI_TXRX_CTRL_REG, DSI_REG[i]->DSI_TXRX_CTRL,
                    //      EXT_TE_EDGE, 1);
                DISP_REG_SET_FIELD(cmdq, DSI_TXRX_CON_FLD_EXT_TE_EDGE_SEL,
                                    DSI_REG_BASE[i] + DISP_REG_DSI_TXRX_CON, 1);
            }
            //DSI_OUTREGBIT(cmdq, struct DSI_TXRX_CTRL_REG, DSI_REG[i]->DSI_TXRX_CTRL,
            //                EXT_TE_EN, 1);
            DISP_REG_SET_FIELD(cmdq, DSI_TXRX_CON_FLD_EXT_TE_EN,
                                DSI_REG_BASE[i] + DISP_REG_DSI_TXRX_CON, 1);
        }
    }
    return DSI_STATUS_OK;
}

void DSI_CPHY_clk_setting(enum DISP_MODULE_ENUM module, void *cmdq, LCM_DSI_PARAMS *dsi_params)
{
    int i = 0;
    unsigned int pcw_ratio = 0;
    unsigned int pcw = 0;
    unsigned int posdiv = 0;
    unsigned int prediv = 0;
    unsigned int data_Rate = dsi_params->data_rate != 0 ?
                                    dsi_params->data_rate : dsi_params->PLL_CLOCK * 2;
    enum MIPITX_PAD_VALUE pad_mapping[MIPITX_PHY_LANE_NUM]
            = {PAD_D2P_T0A, PAD_D0N_T1A, PAD_D1P_T2A,
                PAD_D1P_T2A, PAD_D1P_T2A, PAD_D1P_T2A};

#ifdef LK_FILL_MIPI_IMPEDANCE
    unsigned int j = 0;
    unsigned int mipitx_base = 0, mipitx_base2 = 0;
    unsigned int mipitx_addr = 0, mipitx_addr2 = 0;
    unsigned int mipitx_value = 0;
    unsigned int tmp = 0, tmp2 = 0;
    unsigned int tmp_base = 0;
    int k;

    const struct mipitx_impedance m_mipitx_impedance[3] = {
        /* fill mipitx impedance MSB->LSB */
        {5, 5, 0x11C10190, 27},
        {4, 3, 0x11C10194, 9},
        {1, 3, 0x11C10198, 29},
    };
#endif

    for (i = DSI_MODULE_BEGIN(module); i <= DSI_MODULE_END(module); i++) {
        //MIPITX_OUTREG32(DSI_PHY_REG[i]+MIPITX_IMPENDANCE_2, 0x00001010);

        /*set volate*/
        MIPITX_OUTREG32(DSI_PHY_REG[i]+MIPITX_VOLTAGE_SEL, 0x44441344);

        /*set lane swap*/
        if (dsi_params->lane_swap_en) {
            DISPCHECK("MIPITX Lane Swap Enabled for DSI Port %d\n", i);
            DISPCHECK("MIPITX Lane Swap mapping: %d|%d|%d|%d|%d|%d\n",
                dsi_params->lane_swap[i][MIPITX_PHY_LANE_0],
                dsi_params->lane_swap[i][MIPITX_PHY_LANE_1],
                dsi_params->lane_swap[i][MIPITX_PHY_LANE_2],
                dsi_params->lane_swap[i][MIPITX_PHY_LANE_3],
                dsi_params->lane_swap[i][MIPITX_PHY_LANE_CK],
                dsi_params->lane_swap[i][MIPITX_PHY_LANE_RX]);
            MIPITX_OUTREGBIT(DSI_PHY_REG[i]+MIPITX_PHY_SEL0, FLD_MIPI_TX_PHY0_SEL,
                pad_mapping[dsi_params->lane_swap[i][MIPITX_PHY_LANE_0]]);
            MIPITX_OUTREGBIT(DSI_PHY_REG[i]+MIPITX_PHY_SEL0, FLD_MIPI_TX_PHY1AB_SEL,
                pad_mapping[dsi_params->lane_swap[i][MIPITX_PHY_LANE_0]]+1);

            /* ENABLE CPHY*/
            MIPITX_OUTREGBIT(DSI_PHY_REG[i]+MIPITX_PHY_SEL0, FLD_MIPI_TX_CPHY_EN, 0x1);

            /* CPHY_LANE_T0 */
            MIPITX_OUTREGBIT(DSI_PHY_REG[i]+MIPITX_PHY_SEL0, FLD_MIPI_TX_PHY2_SEL,
                pad_mapping[dsi_params->lane_swap[i][MIPITX_PHY_LANE_0]]);
            MIPITX_OUTREGBIT(DSI_PHY_REG[i]+MIPITX_PHY_SEL0, FLD_MIPI_TX_CPHY0BC_SEL,
                pad_mapping[dsi_params->lane_swap[i][MIPITX_PHY_LANE_0]]+1);
            MIPITX_OUTREGBIT(DSI_PHY_REG[i]+MIPITX_PHY_SEL0, FLD_MIPI_TX_PHY0_SEL,
                pad_mapping[dsi_params->lane_swap[i][MIPITX_PHY_LANE_0]]+2);

            /* CPHY_LANE_T1 */
            MIPITX_OUTREGBIT(DSI_PHY_REG[i]+MIPITX_PHY_SEL0, FLD_MIPI_TX_PHY1AB_SEL,
                pad_mapping[dsi_params->lane_swap[i][MIPITX_PHY_LANE_1]]);
            MIPITX_OUTREGBIT(DSI_PHY_REG[i]+MIPITX_PHY_SEL0, FLD_MIPI_TX_PHYC_SEL,
                pad_mapping[dsi_params->lane_swap[i][MIPITX_PHY_LANE_1]]+1);
            MIPITX_OUTREGBIT(DSI_PHY_REG[i]+MIPITX_PHY_SEL0, FLD_MIPI_TX_CPHY1CA_SEL,
                pad_mapping[dsi_params->lane_swap[i][MIPITX_PHY_LANE_1]]+2);

            /* CPHY_LANE_T2 */
            MIPITX_OUTREGBIT(DSI_PHY_REG[i]+MIPITX_PHY_SEL0, FLD_MIPI_TX_PHY1_SEL,
                pad_mapping[dsi_params->lane_swap[i][MIPITX_PHY_LANE_2]]);
            MIPITX_OUTREGBIT(DSI_PHY_REG[i]+MIPITX_PHY_SEL1, FLD_MIPI_TX_PHY2BC_SEL,
                pad_mapping[dsi_params->lane_swap[i][MIPITX_PHY_LANE_2]]+1);
            MIPITX_OUTREGBIT(DSI_PHY_REG[i]+MIPITX_PHY_SEL1, FLD_MIPI_TX_PHY3_SEL,
                pad_mapping[dsi_params->lane_swap[i][MIPITX_PHY_LANE_2]]+2);

            /* LPRX SETTING */
            MIPITX_OUTREGBIT(DSI_PHY_REG[i]+MIPITX_PHY_SEL1, FLD_MIPI_TX_LPRX0AB_SEL,
                pad_mapping[dsi_params->lane_swap[i][MIPITX_PHY_LANE_0]]);
            MIPITX_OUTREGBIT(DSI_PHY_REG[i]+MIPITX_PHY_SEL1, FLD_MIPI_TX_LPRX0BC_SEL,
                pad_mapping[dsi_params->lane_swap[i][MIPITX_PHY_LANE_0]]+1);
            MIPITX_OUTREGBIT(DSI_PHY_REG[i]+MIPITX_PHY_SEL1, FLD_MIPI_TX_LPRX0CA_SEL,
                pad_mapping[dsi_params->lane_swap[i][MIPITX_PHY_LANE_0]]+2);

            /* HS SETTING */
            MIPITX_OUTREGBIT(DSI_PHY_REG[i]+MIPITX_PHY_SEL1, FLD_MIPI_TX_CPHY0_HS_SEL,
                pad_mapping[dsi_params->lane_swap[i][MIPITX_PHY_LANE_0]]/3);
            MIPITX_OUTREGBIT(DSI_PHY_REG[i]+MIPITX_PHY_SEL1, FLD_MIPI_TX_CPHY1_HS_SEL,
                pad_mapping[dsi_params->lane_swap[i][MIPITX_PHY_LANE_1]]/3);
            MIPITX_OUTREGBIT(DSI_PHY_REG[i]+MIPITX_PHY_SEL1, FLD_MIPI_TX_CPHY2_HS_SEL,
                pad_mapping[dsi_params->lane_swap[i][MIPITX_PHY_LANE_2]]/3);

            /* HS_DATA_SETTING */
            MIPITX_OUTREGBIT(DSI_PHY_REG[i]+MIPITX_PHY_SEL2, FLD_MIPI_TX_PHY2_HSDATA_SEL,
                pad_mapping[dsi_params->lane_swap[i][MIPITX_PHY_LANE_0]]+2);
            MIPITX_OUTREGBIT(DSI_PHY_REG[i]+MIPITX_PHY_SEL2, FLD_MIPI_TX_CPHY0BC_HSDATA_SEL,
                pad_mapping[dsi_params->lane_swap[i][MIPITX_PHY_LANE_0]]);
            MIPITX_OUTREGBIT(DSI_PHY_REG[i]+MIPITX_PHY_SEL2, FLD_MIPI_TX_PHY0_HSDATA_SEL,
                pad_mapping[dsi_params->lane_swap[i][MIPITX_PHY_LANE_0]]+1);
            MIPITX_OUTREGBIT(DSI_PHY_REG[i]+MIPITX_PHY_SEL2, FLD_MIPI_TX_PHY1AB_HSDATA_SEL,
                pad_mapping[dsi_params->lane_swap[i][MIPITX_PHY_LANE_1]]);
            MIPITX_OUTREGBIT(DSI_PHY_REG[i]+MIPITX_PHY_SEL2, FLD_MIPI_TX_PHYC_HSDATA_SEL,
                pad_mapping[dsi_params->lane_swap[i][MIPITX_PHY_LANE_1]]+1);
            MIPITX_OUTREGBIT(DSI_PHY_REG[i]+MIPITX_PHY_SEL2, FLD_MIPI_TX_CPHY1CA_HSDATA_SEL,
                pad_mapping[dsi_params->lane_swap[i][MIPITX_PHY_LANE_1]]+2);
            MIPITX_OUTREGBIT(DSI_PHY_REG[i]+MIPITX_PHY_SEL2, FLD_MIPI_TX_PHY1_HSDATA_SEL,
                pad_mapping[dsi_params->lane_swap[i][MIPITX_PHY_LANE_2]]+2);
            MIPITX_OUTREGBIT(DSI_PHY_REG[i]+MIPITX_PHY_SEL2, FLD_MIPI_TX_PHY2BC_HSDATA_SEL,
                pad_mapping[dsi_params->lane_swap[i][MIPITX_PHY_LANE_2]]);
            MIPITX_OUTREGBIT(DSI_PHY_REG[i]+MIPITX_PHY_SEL3, FLD_MIPI_TX_PHY3_HSDATA_SEL,
                pad_mapping[dsi_params->lane_swap[i][MIPITX_PHY_LANE_2]]+1);
        } else {
            MIPITX_OUTREG32(DSI_PHY_REG[i]+MIPITX_PHY_SEL0, 0x65432101);
            MIPITX_OUTREG32(DSI_PHY_REG[i]+MIPITX_PHY_SEL1, 0x24210987);
            MIPITX_OUTREG32(DSI_PHY_REG[i]+MIPITX_PHY_SEL2, 0x68543102);
            MIPITX_OUTREG32(DSI_PHY_REG[i]+MIPITX_PHY_SEL3, 0x00000007);
        }
    }
#ifdef LK_FILL_MIPI_IMPEDANCE
    /* fill mipitx impedance */
    for (i = 0; i < 3; i++) {
        if (i == 0) {
            mipitx_base = DSI_PHY_REG[0] + MIPITX_D2P_RTCODE0;
            mipitx_base2 = DSI_PHY_REG[0] + MIPITX_D2N_RTCODE0;
        } else {
            mipitx_base = DSI_PHY_REG[0] + MIPITX_D2P_RT_DEM_CODE;
            mipitx_base2 = DSI_PHY_REG[0] + MIPITX_D2N_RT_DEM_CODE;
        }

        mipitx_value = readl(m_mipitx_impedance[i].efuse_base);
        for (j = 0; j < m_mipitx_impedance[i].efuse_block_num; j++) {
            if (i == 0) {
                tmp = (mipitx_value>>(m_mipitx_impedance[i].offset_start -
                    j * m_mipitx_impedance[i].block_bits_num))&0x1F;
            } else
                tmp = (mipitx_value>>(m_mipitx_impedance[i].offset_start -
                    j * m_mipitx_impedance[i].block_bits_num))&0x7;

            /* fill value into mipitx reg */
            mipitx_addr = mipitx_base;
            mipitx_addr2 = mipitx_base2;
            for (k = 0; k < m_mipitx_impedance[i].block_bits_num; k++) {
                if (tmp) {
                    if (i == 0) {
                        MIPITX_OUTREG32(mipitx_addr, (tmp >> k)&0x1);
                        MIPITX_OUTREG32(mipitx_addr2, (tmp >> k)&0x1);
                    } else {
                        MIPITX_OUTREG32(mipitx_addr, ((tmp >> k)&0x1)<<(k*8));
                        MIPITX_OUTREG32(mipitx_addr2, ((tmp >> k)&0x1)<<(k*8));
                    }
                }
                if (i == 0) {
                    mipitx_addr += 0x4;
                    mipitx_addr2 += 0x4;
                }
            }

            /* update mipitx base */
            mipitx_base += 0x100;
            mipitx_base2 += 0x100;
        }
    }

#if 0
    for (i = 0; i < 3; i++) {
        tmp_base = m_mipitx_impedance[i].efuse_base;
        DDPDUMP("TMP[0x%08x]: 0x%08x\n", tmp_base,
            readl((tmp_base)));
    }

    for (i = 0; i < 10; i++) {
        if (i < 5) {
            mipitx_addr = DSI_PHY_REG[0] + MIPITX_D2P_RTCODE0;
            mipitx_addr2 = DSI_PHY_REG[0] + MIPITX_D2N_RTCODE0;
        } else {
            mipitx_addr = DSI_PHY_REG[0] + MIPITX_D2P_RT_DEM_CODE;
            mipitx_addr2 = DSI_PHY_REG[0] + MIPITX_D2N_RT_DEM_CODE;
        }

        if (i < 5)
            k = i * 0x100;
        else
            k = (i - 5) * 0x100;

        if (i < 5) {
            DDPDUMP("MIPI_TX[0x%08x]: 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x\n", mipitx_addr + k,
                    readl((mipitx_addr + k)),
                    readl((mipitx_addr + k + 0x4)),
                    readl((mipitx_addr + k + 0x8)),
                    readl((mipitx_addr + k + 0xC)),
                    readl((mipitx_addr + k + 0x10)));
            DDPDUMP("MIPI_TX2[0x%08x]: 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x\n", mipitx_addr2 + k,
                    readl((mipitx_addr2 + k)),
                    readl((mipitx_addr2 + k + 0x4)),
                    readl((mipitx_addr2 + k + 0x8)),
                    readl((mipitx_addr2 + k + 0xC)),
                    readl((mipitx_addr2 + k + 0x10)));
        } else {
            DDPDUMP("MIPI_TX[0x%08x]: 0x%08x\n", mipitx_addr + k,
                    readl((mipitx_addr + k)));
            DDPDUMP("MIPI_TX[0x%08x]: 0x%08x\n", mipitx_addr2 + k,
                    readl((mipitx_addr2 + k)));
        }
    }
#endif
#endif
    /* MIPI INIT */
    for (i = DSI_MODULE_BEGIN(module); i <= DSI_MODULE_END(module); i++) {
        /* step 0 */
        MIPITX_OUTREG32(DSI_PHY_REG[i]+MIPITX_PRESERVED, 0x0);
        /* BG_LPF_EN / BG_CORE_EN */
        MIPITX_OUTREG32(DSI_PHY_REG[i]+MIPITX_PLL_CON4, 0x00FF12E0);
        MIPITX_OUTREG32(DSI_PHY_REG[i]+MIPITX_LANE_CON, 0x3FFF0088); /* BG_LPF_EN=0 BG_CORE_EN=1 */
        spin(1000); /* 1us */
        MIPITX_OUTREG32(DSI_PHY_REG[i]+MIPITX_LANE_CON, 0x3FFF00C8); /* BG_LPF_EN=1 */

        /* step 1 */
        /* SDM_RWR_ON / SDM_ISO_EN */
        MIPITX_OUTREGBIT(DSI_PHY_REG[i]+MIPITX_PLL_PWR, FLD_AD_DSI_PLL_SDM_PWR_ON, 1);
        spin(1000); /* 1us */
        MIPITX_OUTREGBIT(DSI_PHY_REG[i]+MIPITX_PLL_PWR, FLD_AD_DSI_PLL_SDM_ISO_EN, 0);


        if (data_Rate != 0) {
            unsigned int tmp = 0;

            if (data_Rate > 2500) {
                DISPCHECK("mipitx Data Rate exceed limitation(%d)\n", data_Rate);
                ASSERT(0);
            } else if (data_Rate >= 2000) { /* 2G ~ 2.5G */
                pcw_ratio = 1;
                posdiv    = 0;
                prediv    = 0;
            } else if (data_Rate >= 1000) { /* 1G ~ 2G */
                pcw_ratio = 2;
                posdiv    = 1;
                prediv    = 0;
            } else if (data_Rate >= 500) { /* 500M ~ 1G */
                pcw_ratio = 4;
                posdiv    = 2;
                prediv    = 0;
            } else if (data_Rate > 250) { /* 250M ~ 500M */
                pcw_ratio = 8;
                posdiv    = 3;
                prediv    = 0;
            } else if (data_Rate >= 125) { /* 125M ~ 250M */
                pcw_ratio = 16;
                posdiv    = 4;
                prediv    = 0;
            } else {
                DISPCHECK("dataRate is too low(%d)\n", data_Rate);
                ASSERT(0);
            }

            /* step 3 */
            /* PLL PCW config */
            /*
             * PCW bit 24~30 = floor(pcw)
             * PCW bit 16~23 = (pcw - floor(pcw))*256
             * PCW bit 8~15 = (pcw*256 - floor(pcw)*256)*256
             * PCW bit 8~15 = (pcw*256*256 - floor(pcw)*256*256)*256
             */
            // pcw = data_Rate*4*txdiv/(26*2);//Post DIV =4, so need data_Rate*4

            pcw = data_Rate * pcw_ratio / 26;
            tmp = ((pcw & 0xFF) << 24) | (((256 * (data_Rate * pcw_ratio % 26) / 26) & 0xFF) << 16)
                | (((256 * (256 * (data_Rate * pcw_ratio % 26) % 26) / 26) & 0xFF) << 8)
                | ((256 * (256 * (256 * (data_Rate * pcw_ratio % 26) % 26) % 26) / 26) & 0xFF);
            MIPITX_OUTREG32(DSI_PHY_REG[i]+MIPITX_PLL_CON0, tmp);

            MIPITX_OUTREGBIT(DSI_PHY_REG[i]+MIPITX_PLL_CON1, FLD_RG_DSI_PLL_POSDIV, posdiv);
        }
        /* step 4 */
        /* PLL EN */
        spin(1000); /* 30ns */
        MIPITX_OUTREGBIT(DSI_PHY_REG[i]+MIPITX_PLL_CON1, FLD_RG_DSI_PLL_EN, 1);

        spin(1000); /* 20us */
    }

}

void DSI_DPHY_clk_setting(enum DISP_MODULE_ENUM module, void *cmdq, LCM_DSI_PARAMS *dsi_params)
{
    int i = 0;
    unsigned int j = 0;
    unsigned int data_Rate = dsi_params->data_rate != 0 ?
                                dsi_params->data_rate : dsi_params->PLL_CLOCK * 2;
    unsigned int pcw_ratio = 0;
    unsigned int pcw = 0;
    unsigned int posdiv    = 0;
    //unsigned int prediv    = 0;
    unsigned int delta1 = 5; /* Delta1 is SSC range, default is 0%~-5% */
    unsigned int pdelta1 = 0;
#ifdef LK_FILL_MIPI_IMPEDANCE
    const struct mipitx_impedance m_mipitx_impedance[3] = {
        /* fill mipitx impedance MSB->LSB */
        {5, 5, 0x11C10190, 27},
        {4, 3, 0x11C10194, 9},
        {1, 3, 0x11C10198, 29},
    };
#endif
    enum MIPITX_PAD_VALUE pad_mapping[MIPITX_PHY_LANE_NUM]
        = {PAD_D0P_T0C, PAD_D1P_T2A, PAD_D2P_T0A,
            PAD_D3P_T2C, PAD_CKP_T1B, PAD_CKP_T1B};

    DISPFUNC();

    /* DPHY SETTING */

    /* MIPITX lane swap setting */
    for (i = DSI_MODULE_BEGIN(module); i <= DSI_MODULE_END(module); i++) {
        /* step 0 MIPITX lane swap setting */
        if (dsi_params->lane_swap_en) {
            DISPCHECK("MIPITX Lane Swap Enabled for DSI Port %d\n", i);
            DISPCHECK("MIPITX Lane Swap mapping: %d|%d|%d|%d|%d|%d\n",
                dsi_params->lane_swap[i][MIPITX_PHY_LANE_0],
                dsi_params->lane_swap[i][MIPITX_PHY_LANE_1],
                dsi_params->lane_swap[i][MIPITX_PHY_LANE_2],
                dsi_params->lane_swap[i][MIPITX_PHY_LANE_3],
                dsi_params->lane_swap[i][MIPITX_PHY_LANE_CK],
                dsi_params->lane_swap[i][MIPITX_PHY_LANE_RX]);

            /* CKMODE_EN */
            for (j = MIPITX_PHY_LANE_0; j < MIPITX_PHY_LANE_CK; j++) {
                if (dsi_params->lane_swap[i][j] == MIPITX_PHY_LANE_CK)
                    break;
            }
            switch (j) {
            case MIPITX_PHY_LANE_0:
                MIPITX_OUTREGBIT(DSI_PHY_REG[i]+MIPITX_D0_CKMODE_EN, FLD_DSI_D0_CKMODE_EN, 1);
                break;
            case MIPITX_PHY_LANE_1:
                MIPITX_OUTREGBIT(DSI_PHY_REG[i]+MIPITX_D1_CKMODE_EN, FLD_DSI_D1_CKMODE_EN, 1);
                break;
            case MIPITX_PHY_LANE_2:
                MIPITX_OUTREGBIT(DSI_PHY_REG[i]+MIPITX_D2_CKMODE_EN, FLD_DSI_D2_CKMODE_EN, 1);
                break;
            case MIPITX_PHY_LANE_3:
                MIPITX_OUTREGBIT(DSI_PHY_REG[i]+MIPITX_D3_CKMODE_EN, FLD_DSI_D3_CKMODE_EN, 1);
                break;
            case MIPITX_PHY_LANE_CK:
                MIPITX_OUTREGBIT(DSI_PHY_REG[i]+MIPITX_CK_CKMODE_EN, FLD_DSI_CK_CKMODE_EN, 1);
                break;
            default:
                break;
            }
            /* LANE_0 */
            MIPITX_OUTREGBIT(DSI_PHY_REG[i]+MIPITX_PHY_SEL0, FLD_MIPI_TX_PHY0_SEL,
                pad_mapping[dsi_params->lane_swap[i][MIPITX_PHY_LANE_0]]);
            MIPITX_OUTREGBIT(DSI_PHY_REG[i]+MIPITX_PHY_SEL0, FLD_MIPI_TX_PHY1AB_SEL,
                pad_mapping[dsi_params->lane_swap[i][MIPITX_PHY_LANE_0]]+1);

            /* LANE_1 */
            MIPITX_OUTREGBIT(DSI_PHY_REG[i]+MIPITX_PHY_SEL0, FLD_MIPI_TX_PHY1_SEL,
                pad_mapping[dsi_params->lane_swap[i][MIPITX_PHY_LANE_1]]);
            MIPITX_OUTREGBIT(DSI_PHY_REG[i]+MIPITX_PHY_SEL1, FLD_MIPI_TX_PHY2BC_SEL,
                pad_mapping[dsi_params->lane_swap[i][MIPITX_PHY_LANE_1]]+1);

            /* LANE_2 */
            MIPITX_OUTREGBIT(DSI_PHY_REG[i]+MIPITX_PHY_SEL0, FLD_MIPI_TX_PHY2_SEL,
                pad_mapping[dsi_params->lane_swap[i][MIPITX_PHY_LANE_2]]);
            MIPITX_OUTREGBIT(DSI_PHY_REG[i]+MIPITX_PHY_SEL0, FLD_MIPI_TX_CPHY0BC_SEL,
                pad_mapping[dsi_params->lane_swap[i][MIPITX_PHY_LANE_2]]+1);

            /* LANE_3 */
            MIPITX_OUTREGBIT(DSI_PHY_REG[i]+MIPITX_PHY_SEL1, FLD_MIPI_TX_PHY3_SEL,
                pad_mapping[dsi_params->lane_swap[i][MIPITX_PHY_LANE_3]]);
            MIPITX_OUTREGBIT(DSI_PHY_REG[i]+MIPITX_PHY_SEL1, FLD_MIPI_TX_CPHYXXX_SEL,
                pad_mapping[dsi_params->lane_swap[i][MIPITX_PHY_LANE_3]]+1);

            /* CK LANE */
            MIPITX_OUTREGBIT(DSI_PHY_REG[i]+MIPITX_PHY_SEL0, FLD_MIPI_TX_PHYC_SEL,
                pad_mapping[dsi_params->lane_swap[i][MIPITX_PHY_LANE_CK]]);
            MIPITX_OUTREGBIT(DSI_PHY_REG[i]+MIPITX_PHY_SEL0, FLD_MIPI_TX_CPHY1CA_SEL,
                pad_mapping[dsi_params->lane_swap[i][MIPITX_PHY_LANE_CK]]+1);

            /* LPRX SETTING */
            MIPITX_OUTREGBIT(DSI_PHY_REG[i]+MIPITX_PHY_SEL1, FLD_MIPI_TX_LPRX0AB_SEL,
                 pad_mapping[dsi_params->lane_swap[i][MIPITX_PHY_LANE_RX]]);
            MIPITX_OUTREGBIT(DSI_PHY_REG[i]+MIPITX_PHY_SEL1, FLD_MIPI_TX_LPRX0BC_SEL,
                 pad_mapping[dsi_params->lane_swap[i][MIPITX_PHY_LANE_RX]]+1);

            /* HS_DATA SETTING */
            MIPITX_OUTREGBIT(DSI_PHY_REG[i]+MIPITX_PHY_SEL2, FLD_MIPI_TX_PHY2_HSDATA_SEL,
                 pad_mapping[dsi_params->lane_swap[i][MIPITX_PHY_LANE_2]]);
            MIPITX_OUTREGBIT(DSI_PHY_REG[i]+MIPITX_PHY_SEL2, FLD_MIPI_TX_PHY0_HSDATA_SEL,
                 pad_mapping[dsi_params->lane_swap[i][MIPITX_PHY_LANE_0]]);
            MIPITX_OUTREGBIT(DSI_PHY_REG[i]+MIPITX_PHY_SEL2, FLD_MIPI_TX_PHYC_HSDATA_SEL,
                 pad_mapping[dsi_params->lane_swap[i][MIPITX_PHY_LANE_CK]]);
            MIPITX_OUTREGBIT(DSI_PHY_REG[i]+MIPITX_PHY_SEL2, FLD_MIPI_TX_PHY1_HSDATA_SEL,
                 pad_mapping[dsi_params->lane_swap[i][MIPITX_PHY_LANE_1]]);
            MIPITX_OUTREGBIT(DSI_PHY_REG[i]+MIPITX_PHY_SEL3, FLD_MIPI_TX_PHY3_HSDATA_SEL,
                 pad_mapping[dsi_params->lane_swap[i][MIPITX_PHY_LANE_3]]);

        } else {
            MIPITX_OUTREGBIT(DSI_PHY_REG[i]+MIPITX_CK_CKMODE_EN, FLD_DSI_CK_CKMODE_EN, 1);
        }
    }

#ifdef LK_FILL_MIPI_IMPEDANCE
    /* fill mipitx impedance */
    for (i = 0; i < 3; i++) {
        if (i == 0) {
            mipitx_base = DSI_PHY_REG[0] + MIPITX_D2P_RTCODE0;
            mipitx_base2 = DSI_PHY_REG[0] + MIPITX_D2N_RTCODE0;
        } else {
            mipitx_base = DSI_PHY_REG[0] + MIPITX_D2P_RT_DEM_CODE;
            mipitx_base2 = DSI_PHY_REG[0] + MIPITX_D2N_RT_DEM_CODE;
        }

        mipitx_value = readl(m_mipitx_impedance[i].efuse_base);
        for (j = 0; j < m_mipitx_impedance[i].efuse_block_num; j++) {
            if (i == 0)
                tmp = (mipitx_value>>(m_mipitx_impedance[i].offset_start -
                    j * m_mipitx_impedance[i].block_bits_num))&0x1F;
            else
                tmp = (mipitx_value>>(m_mipitx_impedance[i].offset_start -
                    j * m_mipitx_impedance[i].block_bits_num))&0x7;

            /* fill value into mipitx reg */
            mipitx_addr = mipitx_base;
            mipitx_addr2 = mipitx_base2;
            for (k = 0; k < m_mipitx_impedance[i].block_bits_num; k++) {
                if (tmp) {
                    if (i == 0) {
                        MIPITX_OUTREG32(mipitx_addr, (tmp >> k)&0x1);
                        MIPITX_OUTREG32(mipitx_addr2, (tmp >> k)&0x1);
                    } else {
                        MIPITX_OUTREG32(mipitx_addr, ((tmp >> k)&0x1)<<(k*8));
                        MIPITX_OUTREG32(mipitx_addr2, ((tmp >> k)&0x1)<<(k*8));
                    }
                }
                if (i == 0) {
                    mipitx_addr += 0x4;
                    mipitx_addr2 += 0x4;
                }
            }

            /* update mipitx base */
            mipitx_base += 0x100;
            mipitx_base2 += 0x100;
        }
    }

#if 0
    for (i = 0; i < 3; i++) {
        tmp_base = m_mipitx_impedance[i].efuse_base;
        DDPDUMP("TMP[0x%08x]: 0x%08x\n", tmp_base,
            readl((tmp_base)));
    }

    for (i = 0; i < 10; i++) {
        if (i < 5) {
            mipitx_addr = DSI_PHY_REG[0] + MIPITX_D2P_RTCODE0;
            mipitx_addr2 = DSI_PHY_REG[0] + MIPITX_D2N_RTCODE0;
        } else {
            mipitx_addr = DSI_PHY_REG[0] + MIPITX_D2P_RT_DEM_CODE;
            mipitx_addr2 = DSI_PHY_REG[0] + MIPITX_D2N_RT_DEM_CODE;
        }

        if (i < 5)
            k = i * 0x100;
        else
            k = (i - 5) * 0x100;

        if (i < 5) {
            DDPDUMP("MIPI_TX[0x%08x]: 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x\n",
                    mipitx_addr + k,
                    readl((mipitx_addr + k)),
                    readl((mipitx_addr + k + 0x4)),
                    readl((mipitx_addr + k + 0x8)),
                    readl((mipitx_addr + k + 0xC)),
                    readl((mipitx_addr + k + 0x10)));
            DDPDUMP("MIPI_TX2[0x%08x]: 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x\n",
                    mipitx_addr2 + k,
                    readl((mipitx_addr2 + k)),
                    readl((mipitx_addr2 + k + 0x4)),
                    readl((mipitx_addr2 + k + 0x8)),
                    readl((mipitx_addr2 + k + 0xC)),
                    readl((mipitx_addr2 + k + 0x10)));
        } else {
            DDPDUMP("MIPI_TX[0x%08x]: 0x%08x\n", mipitx_addr + k,
                    readl((mipitx_addr + k)));
            DDPDUMP("MIPI_TX[0x%08x]: 0x%08x\n", mipitx_addr2 + k,
                    readl((mipitx_addr2 + k)));
        }
    }
#endif
#endif

    /* MIPI INIT */
    for (i = DSI_MODULE_BEGIN(module); i <= DSI_MODULE_END(module); i++) {
#if 0
        MIPITX_OUTREGBIT(struct MIPITX_DSI_IMPENDANCE_0_REG,
            DSI_PHY_REG[i]->MIPITX_DSI_IMPENDANCE_0, RG_DSI0_D2P_RT_CODE,
            impendance0[i] & 0x1F);
        MIPITX_OUTREGBIT(struct MIPITX_DSI_IMPENDANCE_0_REG,
            DSI_PHY_REG[i]->MIPITX_DSI_IMPENDANCE_0, RG_DSI0_D2N_RT_CODE,
            (impendance0[i] >> 8) & 0x1F);
        MIPITX_OUTREGBIT(struct MIPITX_DSI_IMPENDANCE_0_REG,
            DSI_PHY_REG[i]->MIPITX_DSI_IMPENDANCE_0, RG_DSI0_D0P_RT_CODE,
            (impendance0[i] >> 16) & 0x1F);
        MIPITX_OUTREGBIT(struct MIPITX_DSI_IMPENDANCE_0_REG,
            DSI_PHY_REG[i]->MIPITX_DSI_IMPENDANCE_0, RG_DSI0_D0N_RT_CODE,
            (impendance0[i] >> 24) & 0x1F);
        MIPITX_OUTREGBIT(struct MIPITX_DSI_IMPENDANCE_1_REG,
            DSI_PHY_REG[i]->MIPITX_DSI_IMPENDANCE_1, RG_DSI0_CKP_RT_CODE,
            impendance1[i] & 0x1F);
        MIPITX_OUTREGBIT(struct MIPITX_DSI_IMPENDANCE_1_REG,
            DSI_PHY_REG[i]->MIPITX_DSI_IMPENDANCE_1, RG_DSI0_CKN_RT_CODE,
            (impendance1[i] >> 8) & 0x1F);
        MIPITX_OUTREGBIT(struct MIPITX_DSI_IMPENDANCE_1_REG,
            DSI_PHY_REG[i]->MIPITX_DSI_IMPENDANCE_1, RG_DSI0_D1P_RT_CODE,
            (impendance1[i] >> 16) & 0x1F);
        MIPITX_OUTREGBIT(struct MIPITX_DSI_IMPENDANCE_1_REG,
            DSI_PHY_REG[i]->MIPITX_DSI_IMPENDANCE_1, RG_DSI0_D1N_RT_CODE,
            (impendance1[i] >> 24) & 0x1F);
        MIPITX_OUTREGBIT(struct MIPITX_DSI_IMPENDANCE_2_REG,
            DSI_PHY_REG[i]->MIPITX_DSI_IMPENDANCE_2, RG_DSI0_D3P_RT_CODE,
            impendance2[i] & 0x1F);
        MIPITX_OUTREGBIT(struct MIPITX_DSI_IMPENDANCE_2_REG,
            DSI_PHY_REG[i]->MIPITX_DSI_IMPENDANCE_2, RG_DSI0_D3N_RT_CODE,
            (impendance2[i] >> 8) & 0x1F);
#endif
        /* step 0 */
        MIPITX_OUTREG32(DSI_PHY_REG[i]+MIPITX_PRESERVED, 0x0);
        /* RG_DSI0_PLL_IBIAS = 0*/
        MIPITX_OUTREG32(DSI_PHY_REG[i]+MIPITX_PLL_CON4, 0x00FF12E0);
        /* BG_LPF_EN / BG_CORE_EN */
        MIPITX_OUTREG32(DSI_PHY_REG[i]+MIPITX_LANE_CON, 0x3FFF0180); /* BG_LPF_EN=0 BG_CORE_EN=1 */
        spin(1000);
        MIPITX_OUTREG32(DSI_PHY_REG[i]+MIPITX_LANE_CON, 0x3FFF0080); /* BG_LPF_EN=1 */

        /* Switch OFF each Lane */
        MIPITX_OUTREGBIT(DSI_PHY_REG[i]+MIPITX_D0_SW_CTL_EN,
            FLD_DSI_D0_SW_CTL_EN, 0);
        MIPITX_OUTREGBIT(DSI_PHY_REG[i]+MIPITX_D1_SW_CTL_EN,
            FLD_DSI_D1_SW_CTL_EN, 0);
        MIPITX_OUTREGBIT(DSI_PHY_REG[i]+MIPITX_D2_SW_CTL_EN,
            FLD_DSI_D2_SW_CTL_EN, 0);
        MIPITX_OUTREGBIT(DSI_PHY_REG[i]+MIPITX_D3_SW_CTL_EN,
            FLD_DSI_D3_SW_CTL_EN, 0);
        MIPITX_OUTREGBIT(DSI_PHY_REG[i]+MIPITX_CK_SW_CTL_EN,
            FLD_DSI_CK_SW_CTL_EN, 0);

        /* step 1 */
        /* SDM_RWR_ON / SDM_ISO_EN */
        MIPITX_OUTREGBIT(DSI_PHY_REG[i]+MIPITX_PLL_PWR, FLD_AD_DSI_PLL_SDM_PWR_ON, 1);
        spin(1000); /* 1us */
        MIPITX_OUTREGBIT(DSI_PHY_REG[i]+MIPITX_PLL_PWR, FLD_AD_DSI_PLL_SDM_ISO_EN, 0);

        if (data_Rate != 0) {
            unsigned int tmp = 0;

            if (data_Rate > 2500) {
                DISPERR("mipitx Data Rate exceed limitation(%d)\n", data_Rate);
                ASSERT(0);
            } else if (data_Rate >= 2000) { /* 2G ~ 2.5G */
                pcw_ratio = 1;
                posdiv      = 0;
                //prediv      = 0;
            } else if (data_Rate >= 1000) { /* 1G ~ 2G */
                pcw_ratio = 2;
                posdiv      = 1;
                //prediv      = 0;
            } else if (data_Rate >= 500) { /* 500M ~ 1G */
                pcw_ratio = 4;
                posdiv      = 2;
                //prediv      = 0;
            } else if (data_Rate > 250) { /* 250M ~ 500M */
                pcw_ratio = 8;
                posdiv      = 3;
                //prediv      = 0;
            } else if (data_Rate >= 125) { /* 125M ~ 250M */
                pcw_ratio = 16;
                posdiv      = 4;
                //prediv      = 0;
            } else {
                DISPERR("dataRate is too low(%d)\n", data_Rate);
                ASSERT(0);
            }

            /* step 3 */
            /* PLL PCW config */
            /**
             * PCW bit 24~30 = floor(pcw)
             * PCW bit 16~23 = (pcw - floor(pcw))*256
             * PCW bit 8~15 = (pcw*256 - floor(pcw)*256)*256
             * PCW bit 8~15 = (pcw*256*256 - floor(pcw)*256*256)*256
             */
            /* pcw = data_Rate*4*txdiv/(26*2);//Post DIV =4, so need data_Rate*4 */

            pcw = data_Rate * pcw_ratio / 26;
            tmp = ((pcw & 0xFF) << 24) | (((256 * (data_Rate * pcw_ratio % 26) / 26) & 0xFF) << 16)
                | (((256 * (256 * (data_Rate * pcw_ratio % 26) % 26) / 26) & 0xFF) << 8)
                | ((256 * (256 * (256 * (data_Rate * pcw_ratio % 26) % 26) % 26) / 26) & 0xFF);
            MIPITX_OUTREG32(DSI_PHY_REG[i]+MIPITX_PLL_CON0, tmp);

            MIPITX_OUTREGBIT(DSI_PHY_REG[i]+MIPITX_PLL_CON1, FLD_RG_DSI_PLL_POSDIV, posdiv);

            /* SSC config */
            if (dsi_params->ssc_disable != 1) {
                MIPITX_OUTREGBIT(DSI_PHY_REG[i]+MIPITX_PLL_CON2, FLD_RG_DSI_PLL_SDM_SSC_PH_INIT,
                                    1);
                MIPITX_OUTREGBIT(DSI_PHY_REG[i]+MIPITX_PLL_CON2, FLD_RG_DSI_PLL_SDM_SSC_PRD,
                                    0x1B1);

                delta1 = (dsi_params->ssc_range == 0) ? delta1 : dsi_params->ssc_range;
                ASSERT(delta1 <= 8);
                pdelta1 = (delta1 * (data_Rate / 2) * pcw_ratio * 262144 + 281664) / 563329;

                MIPITX_OUTREGBIT(DSI_PHY_REG[i]+MIPITX_PLL_CON3, FLD_RG_DSI_PLL_SDM_SSC_DELTA,
                                    pdelta1);
                MIPITX_OUTREGBIT(DSI_PHY_REG[i]+MIPITX_PLL_CON3, FLD_RG_DSI_PLL_SDM_SSC_DELTA1,
                    pdelta1);
                DDPMSG("PLL config:data_rate=%d,pcw_ratio=%d,delta1=%d,pdelta1=0x%x\n",
                    data_Rate, pcw_ratio, delta1, pdelta1);
            }

        }
        /* step 4 */
        /* PLL EN */
        spin(1000);
        MIPITX_OUTREGBIT(DSI_PHY_REG[i]+MIPITX_PLL_CON1, FLD_RG_DSI_PLL_EN, 1);

        spin(1000);
    }

    for (i = DSI_MODULE_BEGIN(module); i <= DSI_MODULE_END(module); i++) {
        if ((data_Rate != 0) && (dsi_params->ssc_disable != 1))
            MIPITX_OUTREGBIT(DSI_PHY_REG[i]+MIPITX_PLL_CON2, FLD_RG_DSI_PLL_SDM_SSC_EN, 1);
        else
            MIPITX_OUTREGBIT(DSI_PHY_REG[i]+MIPITX_PLL_CON2, FLD_RG_DSI_PLL_SDM_SSC_EN, 0);

    }
}

void DSI_PHY_clk_setting(enum DISP_MODULE_ENUM module, void *cmdq, LCM_DSI_PARAMS *dsi_params)
{
    int i = 0;

    for (i = DSI_MODULE_BEGIN(module); i <= DSI_MODULE_END(module); i++) {
        if (_dsi_context[i].dsi_params.IsCphy)
            DSI_CPHY_clk_setting(module, cmdq, dsi_params);
        else
            DSI_DPHY_clk_setting(module, cmdq, dsi_params);
    }
}

#define NS_TO_CYCLE(n, c) ((n) / (c))
#define CHK_SWITCH(a, b)  ((a == 0) ? b : a)

void DSI_CPHY_TIMCONFIG(enum DISP_MODULE_ENUM module, void *cmdq, LCM_DSI_PARAMS *dsi_params)
{
    struct DSI_PHY_TIMCON0_REG timcon0;
    struct DSI_PHY_TIMCON1_REG timcon1;
    struct DSI_PHY_TIMCON2_REG timcon2;
    struct DSI_PHY_TIMCON3_REG timcon3;
    int i = 0;
    unsigned int lane_no;
    unsigned int cycle_time = 0;
    unsigned int ui = 0;
    unsigned int hs_trail;
    //unsigned char timcon_temp;

#ifdef MACH_FPGA
    return 0;
#endif
    lane_no = dsi_params->LANE_NUM;
    if (dsi_params->PLL_CLOCK != 0) {
        ui = 1000 / (dsi_params->PLL_CLOCK * 2) + 0x01;
        cycle_time = 8000 / (dsi_params->PLL_CLOCK * 2) + 0x01;
        DISPCHECK("[DISP]-LK-%s, pll=%d, CycleTime =%d(ns), UnitInterval =%d(ns). , lane# = %d\n",
                   __func__, dsi_params->PLL_CLOCK, cycle_time, ui, lane_no);
    } else {
        DISPERR("[dsi_dsi.c] PLL clock should not be 0!!!\n");
        ASSERT(0);
    }

    //  div2_real=div2 ? div2*0x02 : 0x1;
    //cycle_time = (1000 * div2 * div1 * pre_div * post_div)/ (fbk_sel * (fbk_div+0x01) * 26) + 1;
    //ui = (1000 * div2 * div1 * pre_div * post_div)/ (fbk_sel * (fbk_div+0x01) * 26 * 2) + 1;

    hs_trail = (dsi_params->HS_TRAIL == 0) ?
         32 : dsi_params->HS_TRAIL;

    // +3 is recommended from designer becauase of HW latency
    timcon0.HS_TRAIL = hs_trail;

    timcon0.HS_PRPR = (dsi_params->HS_PRPR == 0) ?
        (NS_TO_CYCLE(dsi_params->PLL_CLOCK * 2 * 50.5, 7000) + 1) :
        dsi_params->HS_PRPR;

    timcon0.HS_ZERO = (dsi_params->HS_ZERO == 0) ?
        48 : dsi_params->HS_ZERO;

    timcon0.LPX = (dsi_params->LPX == 0) ?
        (NS_TO_CYCLE(dsi_params->PLL_CLOCK * 2 * 75, 7000)
        + 0x01) : dsi_params->LPX;

    //  timcon1.TA_SACK     = (dsi_params->TA_SACK == 0) ? 1 : dsi_params->TA_SACK;
    timcon1.TA_GET = (dsi_params->TA_GET == 0) ?
        (0x5 * timcon0.LPX) : dsi_params->TA_GET;
    timcon1.TA_SURE = (dsi_params->TA_SURE == 0) ?
        (0x3 * timcon0.LPX / 0x2) : dsi_params->TA_SURE;
    timcon1.TA_GO = (dsi_params->TA_GO == 0) ?
        (0x4 * timcon0.LPX) : dsi_params->TA_GO;
    // --------------------------------------------------------------
    //  NT35510 need fine tune timing
    //  Data_hs_exit = 60 ns + 128UI
    //  Clk_post = 60 ns + 128 UI.
    // --------------------------------------------------------------
    timcon1.DA_HS_EXIT  = (dsi_params->DA_HS_EXIT == 0) ?
        (NS_TO_CYCLE(dsi_params->PLL_CLOCK * 2 * 112.5, 7000) + 1) : dsi_params->DA_HS_EXIT;
    timcon2.CLK_TRAIL = ((dsi_params->CLK_TRAIL == 0) ?
        NS_TO_CYCLE(0x60, cycle_time) : dsi_params->CLK_TRAIL) + 0x01;

    // CLK_TRAIL can't be 1.
    if (timcon2.CLK_TRAIL < 2)
        timcon2.CLK_TRAIL = 2;

    //  timcon2.LPX_WAIT    = (dsi_params->LPX_WAIT == 0) ? 1 : dsi_params->LPX_WAIT;
    timcon2.CONT_DET = dsi_params->CONT_DET;
    timcon2.CLK_ZERO = (dsi_params->CLK_ZERO == 0) ?
        NS_TO_CYCLE(0x190, cycle_time) : dsi_params->CLK_ZERO;

    timcon3.CLK_HS_PRPR = (dsi_params->CLK_HS_PRPR == 0) ?
        NS_TO_CYCLE(0x40, cycle_time) : dsi_params->CLK_HS_PRPR;
    if (timcon3.CLK_HS_PRPR < 1)
        timcon3.CLK_HS_PRPR = 1;
    timcon3.CLK_HS_EXIT = (dsi_params->CLK_HS_EXIT == 0) ?
        (0x2 * timcon0.LPX) : dsi_params->CLK_HS_EXIT;
    timcon3.CLK_HS_POST = (dsi_params->CLK_HS_POST == 0) ?
        NS_TO_CYCLE((0x60 + 0x34 * ui), cycle_time) :
        dsi_params->CLK_HS_POST;

    DISP_LOG_PRINT(ANDROID_LOG_INFO, "DSI",
        "[DISP] - LK - DSI_PHY_TIMCONFIG, HS_TRAIL = %d, HS_ZERO = %d, HS_PRPR = %d, LPX = %d, TA_GET = %d, TA_SURE = %d, TA_GO = %d, CLK_TRAIL = %d, CLK_ZERO = %d, CLK_HS_PRPR = %d\n",
        timcon0.HS_TRAIL, timcon0.HS_ZERO, timcon0.HS_PRPR, timcon0.LPX, timcon1.TA_GET,
        timcon1.TA_SURE, timcon1.TA_GO, timcon2.CLK_TRAIL, timcon2.CLK_ZERO,
        timcon3.CLK_HS_PRPR);
    DISP_LOG_PRINT(ANDROID_LOG_INFO, "DSI",
        "CLK_HS_POST=%d,CLK_HS_EXIT=%d,CLK_TRAIL=%d\n",
        timcon3.CLK_HS_POST, timcon3.CLK_HS_EXIT, timcon2.CLK_TRAIL);

    for (i = DSI_MODULE_BEGIN(module); i <= DSI_MODULE_END(module); i++) {
        _dsi_context[i].data_phy_cycle = timcon0.HS_PRPR + timcon0.HS_ZERO +
            timcon1.DA_HS_EXIT + timcon0.LPX + 5;
        _dsi_context[i].HS_TRAIL = timcon0.HS_TRAIL;
        DISP_REG_SET_FIELD(cmdq, DSI_PHY_TIMCON0_FLD_LPX,
                        DSI_REG_BASE[i] + DISP_REG_DSI_PHY_TIMCON0, timcon0.LPX);
        DISP_REG_SET_FIELD(cmdq, DSI_PHY_TIMCON0_FLD_DA_HS_PREP,
                        DSI_REG_BASE[i] + DISP_REG_DSI_PHY_TIMCON0, timcon0.HS_PRPR);
        DISP_REG_SET_FIELD(cmdq, DSI_PHY_TIMCON0_FLD_DA_HS_ZERO,
                        DSI_REG_BASE[i] + DISP_REG_DSI_PHY_TIMCON0, timcon0.HS_ZERO);
        DISP_REG_SET_FIELD(cmdq, DSI_PHY_TIMCON0_FLD_DA_HS_TRAIL,
                        DSI_REG_BASE[i] + DISP_REG_DSI_PHY_TIMCON0, timcon0.HS_TRAIL);

        DISP_REG_SET_FIELD(cmdq, DSI_PHY_TIMCON1_FLD_TA_GO,
                        DSI_REG_BASE[i] + DISP_REG_DSI_PHY_TIMCON1, timcon1.TA_GO);
        DISP_REG_SET_FIELD(cmdq, DSI_PHY_TIMCON1_FLD_TA_SURE,
                        DSI_REG_BASE[i] + DISP_REG_DSI_PHY_TIMCON1, timcon1.TA_SURE);
        DISP_REG_SET_FIELD(cmdq, DSI_PHY_TIMCON1_FLD_TA_GET,
                        DSI_REG_BASE[i] + DISP_REG_DSI_PHY_TIMCON1, timcon1.TA_GET);
        DISP_REG_SET_FIELD(cmdq, DSI_PHY_TIMCON1_FLD_DA_HS_EXIT,
                        DSI_REG_BASE[i] + DISP_REG_DSI_PHY_TIMCON1, timcon1.DA_HS_EXIT);

        DISP_REG_SET_FIELD(cmdq, DSI_PHY_TIMCON2_FLD_CONT_DET,
                        DSI_REG_BASE[i] + DISP_REG_DSI_PHY_TIMCON2, timcon2.CONT_DET);
        DISP_REG_SET_FIELD(cmdq, DSI_PHY_TIMCON2_FLD_DA_HS_SYNC,
                        DSI_REG_BASE[i] + DISP_REG_DSI_PHY_TIMCON2, 0x06);
        DISP_REG_SET_FIELD(cmdq, DSI_PHY_TIMCON2_FLD_CLK_HS_ZERO,
                        DSI_REG_BASE[i] + DISP_REG_DSI_PHY_TIMCON2, timcon2.CLK_ZERO);
        DISP_REG_SET_FIELD(cmdq, DSI_PHY_TIMCON2_FLD_CLK_HS_TRAIL,
                        DSI_REG_BASE[i] + DISP_REG_DSI_PHY_TIMCON2, timcon2.CLK_TRAIL);

        DISP_REG_SET_FIELD(cmdq, DSI_PHY_TIMCON3_FLD_CLK_HS_PREP,
                        DSI_REG_BASE[i] + DISP_REG_DSI_PHY_TIMCON3, timcon3.CLK_HS_PRPR);
        DISP_REG_SET_FIELD(cmdq, DSI_PHY_TIMCON3_FLD_CLK_HS_POST,
                        DSI_REG_BASE[i] + DISP_REG_DSI_PHY_TIMCON3, timcon3.CLK_HS_POST);
        DISP_REG_SET_FIELD(cmdq, DSI_PHY_TIMCON3_FLD_CLK_HS_EXIT,
                        DSI_REG_BASE[i] + DISP_REG_DSI_PHY_TIMCON3, timcon3.CLK_HS_EXIT);

        DSI_OUTREG32(cmdq, &DSI_REG[i]->DSI_CPHY_CON0, 0x012c0003);

        DISPMSG("%s, 0x%08x,0x%08x,0x%08x,0x%08x\n", __func__, readl(DISP_DSI0_BASE+0x110),
                    readl(DISP_DSI0_BASE+0x114), readl(DISP_DSI0_BASE+0x118),
                    readl(DISP_DSI0_BASE+0x11c));
    }

}

void DSI_DPHY_TIMCONFIG(enum DISP_MODULE_ENUM module, void *cmdq, LCM_DSI_PARAMS *dsi_params)
{
#ifdef MACH_FPGA
    int i = 0;

    /* sync from cmm */
    for (i = DSI_MODULE_BEGIN(module); i <= DSI_MODULE_END(module); i++) {
        DSI_OUTREG32(cmdq, &DSI_REG[i]->DSI_PHY_TIMECON0, 0x02000102);
        DSI_OUTREG32(cmdq, &DSI_REG[i]->DSI_PHY_TIMECON1, 0x010a0308);
        DSI_OUTREG32(cmdq, &DSI_REG[i]->DSI_PHY_TIMECON2, 0x02000100);
        DSI_OUTREG32(cmdq, &DSI_REG[i]->DSI_PHY_TIMECON3, 0x00010701);

        DISPCHECK("%s, 0x%08x,0x%08x,0x%08x,0x%08x\n", __func__,
              readl(&DSI_REG[i]->DSI_PHY_TIMECON0),
              readl(&DSI_REG[i]->DSI_PHY_TIMECON1),
              readl(&DSI_REG[i]->DSI_PHY_TIMECON2),
              readl(&DSI_REG[i]->DSI_PHY_TIMECON3));
    }
    return;

#else
    int i = 0;
    unsigned int lane_no;
    unsigned int cycle_time = 0, ui = 0;
    unsigned int lpx, hs_prpr, hs_zero, hs_trail;
    unsigned int ta_get, ta_sure, ta_go, da_hs_exit;
    unsigned int clk_zero, clk_trail, da_hs_sync;
    unsigned int clk_hs_prpr, clk_hs_exit, clk_hs_post;
    unsigned int cont_det;
    unsigned int value;

    lane_no = dsi_params->LANE_NUM;
    if (dsi_params->data_rate != 0) {
        ui = 1000 / dsi_params->data_rate + 0x01;
        cycle_time = 8000 / dsi_params->data_rate + 0x01;
        DISPCHECK("[DISP]-LK-%s, pll=%d, CycleTime=%d(ns), UnitInterval=%d(ns). , lane#=%d\n",
                   __func__, dsi_params->PLL_CLOCK, cycle_time, ui, lane_no);
    } else if (dsi_params->PLL_CLOCK != 0) {
        dsi_params->data_rate = dsi_params->PLL_CLOCK * 2;
        ui = 1000 / (dsi_params->PLL_CLOCK * 2) + 0x01;
        cycle_time = 8000 / (dsi_params->PLL_CLOCK * 2) + 0x01;
        DISPCHECK("[DISP]-LK- %s, pll=%d, CycleTime=%d(ns), UnitInterval=%d(ns). , lane#=%d\n",
                   __func__, dsi_params->PLL_CLOCK, cycle_time, ui, lane_no);
    } else {
        DISPERR("[dsi_dsi.c] PLL clock should not be 0!!!\n");
        ASSERT(0);
    }

    ui = 1000 / dsi_params->data_rate + 0x01;
    cycle_time = 8000 / dsi_params->data_rate + 0x01;

    lpx = NS_TO_CYCLE(dsi_params->data_rate * 0x4B, 0x1F40) + 0x1;
    hs_prpr = NS_TO_CYCLE((0x40 + 0x5 * ui), cycle_time) + 0x1;
    hs_zero = NS_TO_CYCLE((0xC8 + 0x0A * ui), cycle_time);
    hs_zero = hs_zero > hs_prpr ? hs_zero - hs_prpr : hs_zero;
    hs_trail = NS_TO_CYCLE((0x4 * ui + 0x50) *
        dsi_params->data_rate, 0x1F40) + 0x1;

    ta_get = 5 * lpx;
    ta_sure = 3 * lpx / 2;
    ta_go = 4 * lpx;
    da_hs_exit = 2 * lpx;

    clk_zero = NS_TO_CYCLE(0x190, cycle_time);
    clk_trail = NS_TO_CYCLE(0x64 * dsi_params->data_rate, 0x1F40) + 0x1;
    da_hs_sync = 0x1;
    cont_det = 0x3;

    clk_hs_prpr = NS_TO_CYCLE(0x50 * dsi_params->data_rate, 0x1F40);
    clk_hs_exit = 2 * lpx;
    clk_hs_post = NS_TO_CYCLE(0x60 + 0x34 * ui, cycle_time);

    lpx = CHK_SWITCH(dsi_params->LPX, lpx);
    hs_prpr = CHK_SWITCH(dsi_params->HS_PRPR, hs_prpr);
    hs_zero = CHK_SWITCH(dsi_params->HS_ZERO, hs_zero);
    hs_trail = CHK_SWITCH(dsi_params->HS_TRAIL, hs_trail);

    ta_get = CHK_SWITCH(dsi_params->TA_GET, ta_get);
    ta_sure = CHK_SWITCH(dsi_params->TA_SURE, ta_sure);
    ta_go = CHK_SWITCH(dsi_params->TA_GO, ta_go);
    da_hs_exit = CHK_SWITCH(dsi_params->DA_HS_EXIT, da_hs_exit);

    clk_zero = CHK_SWITCH(dsi_params->CLK_ZERO, clk_zero);
    clk_trail = CHK_SWITCH(dsi_params->CLK_TRAIL, clk_trail);

    clk_hs_prpr = CHK_SWITCH(dsi_params->CLK_HS_PRPR, clk_hs_prpr);
    clk_hs_exit = CHK_SWITCH(dsi_params->CLK_HS_EXIT, clk_hs_exit);
    clk_hs_post = CHK_SWITCH(dsi_params->CLK_HS_POST, clk_hs_post);

CONFIG_REG:
    for (i = DSI_MODULE_BEGIN(module); i <= DSI_MODULE_END(module); i++) {
        value = REG_FLD_VAL(FLD_LPX, lpx)
            | REG_FLD_VAL(FLD_HS_PREP, hs_prpr)
            | REG_FLD_VAL(FLD_HS_ZERO, hs_zero)
            | REG_FLD_VAL(FLD_HS_TRAIL, hs_trail);
        DSI_OUTREG32(NULL, &DSI_REG[i]->DSI_PHY_TIMECON0, value);

        value = REG_FLD_VAL(FLD_TA_GO, ta_go)
            | REG_FLD_VAL(FLD_TA_SURE, ta_sure)
            | REG_FLD_VAL(FLD_TA_GET, ta_get)
            | REG_FLD_VAL(FLD_DA_HS_EXIT, da_hs_exit);
        DSI_OUTREG32(NULL, &DSI_REG[i]->DSI_PHY_TIMECON1, value);

        value = REG_FLD_VAL(FLD_CONT_DET, cont_det)
            | REG_FLD_VAL(FLD_DA_HS_SYNC, da_hs_sync)
            | REG_FLD_VAL(FLD_CLK_HS_ZERO, clk_zero)
            | REG_FLD_VAL(FLD_CLK_HS_TRAIL, clk_trail);
        DSI_OUTREG32(NULL, &DSI_REG[i]->DSI_PHY_TIMECON2, value);

        value = REG_FLD_VAL(FLD_CLK_HS_PREP, clk_hs_prpr)
            | REG_FLD_VAL(FLD_CLK_HS_POST, clk_hs_post)
            | REG_FLD_VAL(FLD_CLK_HS_EXIT, clk_hs_exit);
        DSI_OUTREG32(NULL, &DSI_REG[i]->DSI_PHY_TIMECON3, value);
    }
#endif
}

void DSI_PHY_TIMCONFIG(enum DISP_MODULE_ENUM module, void *cmdq, LCM_DSI_PARAMS *dsi_params)
{
    int i = 0;

    for (i = DSI_MODULE_BEGIN(module); i <= DSI_MODULE_END(module); i++) {
        if (_dsi_context[i].dsi_params.IsCphy)
            DSI_CPHY_TIMCONFIG(module, cmdq, dsi_params);
        else
            DSI_DPHY_TIMCONFIG(module, cmdq, dsi_params);
    }
}

enum DSI_STATUS DSI_Start(enum DISP_MODULE_ENUM module, void *cmdq)
{
    int i = 0;

    if (module != DISP_MODULE_DSIDUAL) {
        for (i = DSI_MODULE_BEGIN(module); i <= DSI_MODULE_END(module); i++) {
            //DSI_OUTREGBIT(cmdq, struct DSI_START_REG, DSI_REG[i]->DSI_START, DSI_START, 0);
            //DSI_OUTREGBIT(cmdq, struct DSI_START_REG, DSI_REG[i]->DSI_START, DSI_START, 1);
            DISP_REG_SET_FIELD(cmdq, DSI_START_FLD_DSI_START,
                            DSI_REG_BASE[i] + DISP_REG_DSI_START, 0);
            DISP_REG_SET_FIELD(cmdq, DSI_START_FLD_DSI_START,
                            DSI_REG_BASE[i] + DISP_REG_DSI_START, 1);
        }
    } else {
        /* TODO: do we need this? */
        //DSI_OUTREGBIT(cmdq, struct DSI_START_REG, DSI_REG[0]->DSI_START, DSI_START, 0);
        //DSI_OUTREGBIT(cmdq, struct DSI_START_REG, DSI_REG[0]->DSI_START, DSI_START, 1);
        DISP_REG_SET_FIELD(cmdq, DSI_START_FLD_DSI_START,
                            DSI_REG_BASE[0] + DISP_REG_DSI_START, 0);
        DISP_REG_SET_FIELD(cmdq, DSI_START_FLD_DSI_START,
                            DSI_REG_BASE[0] + DISP_REG_DSI_START, 1);
    }

    return DSI_STATUS_OK;
}

enum DSI_STATUS DSI_EnableVM_CMD(enum DISP_MODULE_ENUM module, void *cmdq)
{
    if (module == DISP_MODULE_DSI0 || module == DISP_MODULE_DSIDUAL) {
        //DSI_OUTREGBIT(cmdq, struct DSI_START_REG,DSI_REG[0]->DSI_START,VM_CMD_START,0);
        //DSI_OUTREGBIT(cmdq, struct DSI_START_REG,DSI_REG[0]->DSI_START,VM_CMD_START, 1);
        DISP_REG_SET_FIELD(cmdq, DSI_START_FLD_VM_CMD_START,
                            DSI_REG_BASE[0] + DISP_REG_DSI_START, 0);
        DISP_REG_SET_FIELD(cmdq, DSI_START_FLD_VM_CMD_START,
                            DSI_REG_BASE[0] + DISP_REG_DSI_START, 1);
    }
    return DSI_STATUS_OK;
}


/// return value: the data length we got
u32 DSI_dcs_read_lcm_reg_v2(enum DISP_MODULE_ENUM module, void *cmdq, u8 cmd, u8 *buffer,
                            u8 buffer_size)
{
    int d = 0;
    u32 max_try_count = 5;
    u32 recv_data_cnt = 0; // reture value
    unsigned int read_timeout_ms; // used for polling rd_rdy
    unsigned char packet_type;
    struct DSI_RX_DATA_REG read_data0;
    struct DSI_RX_DATA_REG read_data1;
    struct DSI_RX_DATA_REG read_data2;
    struct DSI_RX_DATA_REG read_data3;
    struct DSI_T0_INS t0;
    struct DSI_T0_INS t1;
    void *temp;

    DISPFUNC();
#if ENABLE_DSI_INTERRUPT
    static const long WAIT_TIMEOUT = 2 * HZ;    // 2 sec
    long ret;
#endif

    for (d = DSI_MODULE_BEGIN(module); d <= DSI_MODULE_END(module); d++) {
        if (DSI_REG[d]->DSI_MODE_CTRL.MODE) {
            // only support cmd mode read
            DISPDBG("DSI Read Fail: DSI Mode is %d\n", DSI_REG[d]->DSI_MODE_CTRL.MODE);
            return 0;
        }

        if (buffer == NULL || buffer_size == 0) {
            // illegal parameters
            DISPERR("DSI Read Fail: buffer=0x%p and buffer_size=%d\n", buffer, buffer_size);
            return 0;
        }

        do {
            if (max_try_count == 0) {
                DISPERR("DSI Read Fail: try 5 times\n");
                return 0;
            }
            max_try_count--;
            recv_data_cnt = 0;
            read_timeout_ms = 20;

            // 1. wait dsi not busy => can't read if dsi busy
            DSI_WaitForNotBusy(module, cmdq);
            // 2. Check rd_rdy & cmd_done irq
#if 0
/* LK does not rely on INTEN, use polling to handle event  */
            if (DSI_REG[d]->DSI_INTEN.RD_RDY == 0) {
                //DSI_OUTREGBIT(cmdq, struct DSI_INT_ENABLE_REG, DSI_REG[d]->DSI_INTEN,
                //          RD_RDY, 1);
                DISP_REG_SET_FIELD(cmdq, DSI_INTEN_FLD_LPRX_RD_RDY_INT_EN,
                                    DSI_REG_BASE[d] + DISP_REG_DSI_INTEN, 1);

            }
            if (DSI_REG[d]->DSI_INTEN.CMD_DONE == 0) {
                //DSI_OUTREGBIT(cmdq, struct DSI_INT_ENABLE_REG, DSI_REG[d]->DSI_INTEN,
                         // CMD_DONE, 1);
                DISP_REG_SET_FIELD(cmdq, DSI_INTEN_FLD_CMD_DONE_INT_EN,
                                    DSI_REG_BASE[d] + DISP_REG_DSI_INTEN, 1);

            }
#endif
            if (DSI_REG[d]->DSI_INTSTA.RD_RDY != 0
                || DSI_REG[d]->DSI_INTSTA.CMD_DONE != 0) {
                /* dump cmdq & rxdata */
                {
                    unsigned int i;

                    DISPCHECK("Last DSI Read Why not clear irq???\n");
                    DISPCHECK("DSI_CMDQ_SIZE  : %d\n",
                          AS_UINT32(DSI_REG_BASE[d] + DISP_REG_DSI_CMDQ_CON));
                    for (i = 0; i < DSI_REG[d]->DSI_CMDQ_SIZE.CMDQ_SIZE; i++) {
                        DISPCHECK("DSI_CMDQ_DATA%d : 0x%08x\n", i,
                              AS_UINT32(&DSI_CMDQ_REG[d]->data[i]));
                    }
                    DISPCHECK("DSI_RX_DATA0   : 0x%08x\n",
                          AS_UINT32(DSI_REG_BASE[d] + DISP_REG_DSI_RX_DATA03));
                    DISPCHECK("DSI_RX_DATA1   : 0x%08x\n",
                          AS_UINT32(DSI_REG_BASE[d] + DISP_REG_DSI_RX_DATA47));
                    DISPCHECK("DSI_RX_DATA2   : 0x%08x\n",
                          AS_UINT32(DSI_REG_BASE[d] +  DISP_REG_DSI_RX_DATA8B));
                    DISPCHECK("DSI_RX_DATA3   : 0x%08x\n",
                          AS_UINT32(DSI_REG_BASE[d] + DISP_REG_DSI_RX_DATAC));
                }
                /* clear irq */
                //DSI_OUTREGBIT(cmdq, struct DSI_INT_STATUS_REG, DSI_REG[d]->DSI_INTSTA,
                //          RD_RDY, 0);
                //DSI_OUTREGBIT(cmdq, struct DSI_INT_STATUS_REG, DSI_REG[d]->DSI_INTSTA,
                //          CMD_DONE, 0);
                DISP_REG_SET_FIELD(cmdq, DSI_INTSTA_FLD_LPRX_RD_RDY_INT_FLAG,
                                    DSI_REG_BASE[d] + DISP_REG_DSI_INTSTA, 0);
                DISP_REG_SET_FIELD(cmdq, DSI_INTSTA_FLD_CMD_DONE_INT_FLAG,
                                    DSI_REG_BASE[d] + DISP_REG_DSI_INTSTA, 0);
            }
            /* 3. Send cmd */
            t0.CONFG = 0x04;    /* /BTA */
            /* / 0xB0 is used to distinguish DCS cmd or Gerneric cmd, is that Right??? */
            t0.Data_ID =
                (cmd <
                 0xB0) ? DSI_DCS_READ_PACKET_ID : DSI_GERNERIC_READ_LONG_PACKET_ID;
            t0.Data0 = cmd;
            t0.Data1 = 0;
            /* set max return size */
            t1.CONFG = 0x00;
            t1.Data_ID = 0x37;
            t1.Data0 = buffer_size <= 10 ? buffer_size : 10;
            t1.Data1 = 0;
            temp = &t1;

            DSI_OUTREG32(cmdq, &DSI_CMDQ_REG[d]->data[0], AS_UINT32(temp));
            temp = &t0;
            DSI_OUTREG32(cmdq, &DSI_CMDQ_REG[d]->data[1], AS_UINT32(temp));
            DSI_OUTREG32(cmdq, DSI_REG_BASE[d] + DISP_REG_DSI_CMDQ_CON, 2);

            DSI_OUTREG32(cmdq, DSI_REG_BASE[d] + DISP_REG_DSI_START, 0);
            DSI_OUTREG32(cmdq, DSI_REG_BASE[d] + DISP_REG_DSI_START, 1);
            DSI_OUTREG32(cmdq, DSI_REG_BASE[d] + DISP_REG_DSI_START, 0);

            /* / the following code is to */
            /* / 1: wait read ready */
            /* / 2: ack read ready */
            /* / 3: wait for CMDQ_DONE(interrupt handler do this op) */
            /* / 4: read data */
#if ENABLE_DSI_INTERRUPT
            ret = wait_event_interruptible_timeout(_dsi_dcs_read_wait_queue,
                                                    !_IsEngineBusy(), WAIT_TIMEOUT);
            if (ret == 0) {
                DISP_LOG_PRINT(ANDROID_LOG_INFO, "DSI",
                                " Wait for DSI engine read ready timeout!!!\n");
                DSI_DumpRegisters(module, 2);
                //DSI_OUTREGBIT(cmdq, struct DSI_RACK_REG,DSI_REG[d]->DSI_RACK,DSI_RACK, 1);
                DISP_REG_SET_FIELD(cmdq, DSI_RX_RACK_FLD_RACK,
                                    DSI_REG_BASE[d] + DISP_REG_DSI_RX_RACK, 1);
                DSI_Reset();
                return 0;
            }
#else
            // wait read ready
            DISP_LOG_PRINT(ANDROID_LOG_INFO, "DSI", " Start polling DSI read ready!!!\n");
            while (DISP_REG_GET_FIELD(DSI_INTSTA_FLD_LPRX_RD_RDY_INT_FLAG,
                   DSI_REG_BASE[d] + DISP_REG_DSI_INTSTA) == 0) {
                ///keep polling
                spin(1000);
                read_timeout_ms--;

                if (read_timeout_ms == 0) {
                    DISP_LOG_PRINT(ANDROID_LOG_INFO, "DSI",
                                    " DSI Read Fail: Polling DSI read ready timeout!!!\n");
                    DSI_DumpRegisters(module, 2);

                    ///do necessary reset here
                    //DSI_OUTREGBIT(cmdq, struct DSI_RACK_REG,DSI_REG[d]->DSI_RACK,DSI_RACK, 1);
                    DISP_REG_SET_FIELD(cmdq, DSI_RX_RACK_FLD_RACK,
                                        DSI_REG_BASE[d] + DISP_REG_DSI_RX_RACK, 1);
                    DSI_Reset(module, cmdq);
                    return 0;
                }
            }

            DISP_LOG_PRINT(ANDROID_LOG_INFO, "DSI",
                            " End polling DSI read ready!!!\n");
            // ack read ready
            //DSI_OUTREGBIT(cmdq, struct DSI_RACK_REG,DSI_REG[d]->DSI_RACK,DSI_RACK, 1);
            DISP_REG_SET_FIELD(cmdq, DSI_RX_RACK_FLD_RACK,
                                DSI_REG_BASE[d] + DISP_REG_DSI_RX_RACK, 1);
            // clear read ready irq
            //DSI_OUTREGBIT(cmdq, struct DSI_INT_STATUS_REG,DSI_REG[d]->DSI_INTSTA,RD_RDY,0);
            DISP_REG_SET_FIELD(cmdq, DSI_INTSTA_FLD_LPRX_RD_RDY_INT_FLAG,
                                DSI_REG_BASE[d] + DISP_REG_DSI_INTSTA, 0);
            // wait dsi cmd done
            read_timeout_ms = 20;
            while (DISP_REG_GET_FIELD(DSI_INTSTA_FLD_CMD_DONE_INT_FLAG,
                   DSI_REG_BASE[d] + DISP_REG_DSI_INTSTA) == 0) {
                ///keep polling
                spin(1000);
                read_timeout_ms--;
                if (read_timeout_ms == 0) {
                    DISP_LOG_PRINT(ANDROID_LOG_INFO, "DSI",
                                    " DSI Read Fail: Polling DSI cmd done timeout!!!\n");
                    DSI_DumpRegisters(module, 2);
                    ///do necessary reset here
                    //DSI_OUTREGBIT(cmdq, struct DSI_RACK_REG,DSI_REG[d]->DSI_RACK,DSI_RACK, 1);
                    DISP_REG_SET_FIELD(cmdq, DSI_RX_RACK_FLD_RACK,
                                        DSI_REG_BASE[d] + DISP_REG_DSI_RX_RACK, 1);
                    DSI_Reset(module, cmdq);
                    return 0;
                }
            }
            // clear cmd done irq
            //DSI_OUTREGBIT(cmdq, struct DSI_INT_STATUS_REG,DSI_REG[d]->DSI_INTSTA,CMD_DONE,0);
            DISP_REG_SET_FIELD(cmdq, DSI_INTSTA_FLD_CMD_DONE_INT_FLAG,
                                DSI_REG_BASE[d] + DISP_REG_DSI_INTSTA, 0);
#endif

            DSI_OUTREG32(cmdq, &read_data0, AS_UINT32(DSI_REG_BASE[d] + DISP_REG_DSI_RX_DATA03));
            DSI_OUTREG32(cmdq, &read_data1, AS_UINT32(DSI_REG_BASE[d] + DISP_REG_DSI_RX_DATA47));
            DSI_OUTREG32(cmdq, &read_data2, AS_UINT32(DSI_REG_BASE[d] + DISP_REG_DSI_RX_DATA8B));
            DSI_OUTREG32(cmdq, &read_data3, AS_UINT32(DSI_REG_BASE[d] + DISP_REG_DSI_RX_DATAC));

            {
                unsigned int i;

                DISPCHECK("DSI read begin i = %d --------------------\n",
                      5 - max_try_count);
                DISPCHECK("DSI_RX_STA     : 0x%08x\n",
                      AS_UINT32(DSI_REG_BASE[d] + DISP_REG_DSI_RX_TRIG_STA));
                DISPCHECK("DSI_CMDQ_SIZE  : %d\n",
                      AS_UINT32(DSI_REG_BASE[d] + DISP_REG_DSI_CMDQ_CON));
                for (i = 0; i < DSI_REG[d]->DSI_CMDQ_SIZE.CMDQ_SIZE; i++) {
                    DISPCHECK("DSI_CMDQ_DATA%d : 0x%08x\n", i,
                          AS_UINT32(&DSI_CMDQ_REG[d]->data[i]));
                }
                DISPCHECK("DSI_RX_DATA0   : 0x%08x\n",
                      AS_UINT32(DSI_REG_BASE[d] + DISP_REG_DSI_RX_DATA03));
                DISPCHECK("DSI_RX_DATA1   : 0x%08x\n",
                      AS_UINT32(DSI_REG_BASE[d] + DISP_REG_DSI_RX_DATA47));
                DISPCHECK("DSI_RX_DATA2   : 0x%08x\n",
                      AS_UINT32(DSI_REG_BASE[d] + DISP_REG_DSI_RX_DATA8B));
                DISPCHECK("DSI_RX_DATA3   : 0x%08x\n",
                      AS_UINT32(DSI_REG_BASE[d] + DISP_REG_DSI_RX_DATAC));
                DISPCHECK("DSI read end ----------------------------\n");
            }

            packet_type = read_data0.byte0;

            DISPCHECK("DSI read packet_type is 0x%x\n", packet_type);

            /* 0x02: acknowledge & error report */
            /* 0x11: generic short read response(1 byte return) */
            /* 0x12: generic short read response(2 byte return) */
            /* 0x1a: generic long read response */
            /* 0x1c: dcs long read response */
            /* 0x21: dcs short read response(1 byte return) */
            /* 0x22: dcs short read response(2 byte return) */
            if (packet_type == 0x1A || packet_type == 0x1C) {
                recv_data_cnt = read_data0.byte1 + read_data0.byte2 * 16;
                if (recv_data_cnt > 10) {
                    DISPCHECK
                        ("DSI read long packet data exceeds 4 bytes return size: %d\n",
                         recv_data_cnt);
                    recv_data_cnt = 10;
                }

                if (recv_data_cnt > buffer_size) {
                    DISPCHECK
                        ("DSI read long packet data exceeds buffer size return size %d\n",
                         recv_data_cnt);
                    recv_data_cnt = buffer_size;
                }
                DISPCHECK("DSI read long packet size: %d\n", recv_data_cnt);

                if (recv_data_cnt <= 4) {
                    memcpy((void *)buffer, (void *)&read_data1, recv_data_cnt);
                } else if (recv_data_cnt <= 8) {
                    memcpy((void *)buffer, (void *)&read_data1, 4);
                    memcpy((void *)buffer + 4, (void *)&read_data2,
                           recv_data_cnt - 4);
                } else {
                    memcpy((void *)buffer, (void *)&read_data1, 4);
                    memcpy((void *)buffer + 4, (void *)&read_data2, 4);
                    memcpy((void *)buffer + 8, (void *)&read_data2,
                           recv_data_cnt - 8);
                }
            } else if (packet_type == 0x11 || packet_type == 0x12 ||
                    packet_type == 0x21 || packet_type == 0x22) {
                if (packet_type == 0x11 || packet_type == 0x21)
                    recv_data_cnt = 1;
                else
                    recv_data_cnt = 2;
                if (recv_data_cnt > buffer_size) {
                    DISPCHECK
                        ("DSI read short packet data exceeds buffer size: %d\n",
                         buffer_size);
                    recv_data_cnt = buffer_size;
                    memcpy((void *)buffer, (void *)&read_data0.byte1,
                           recv_data_cnt);
                } else {
                    memcpy((void *)buffer, (void *)&read_data0.byte1,
                           recv_data_cnt);
                }

            } else if (packet_type == 0x02) {
                DISPCHECK("read return type is 0x02, re-read\n");
            } else {
                DISPCHECK("read return type is non-recognite, type = 0x%x\n",
                      packet_type);
                return 0;
            }
        } while (packet_type == 0x02);
        /*
         * here: we may receive a ACK packet which packet type is 0x02
         * (incdicates some error happened)
         * therefore we try re-read again until no ACK packet
         * But: if it is a good way to keep re-trying ???
         */
    }

    return recv_data_cnt;
}

/// return value: the data length we got
u32 DSI_dcs_read_lcm_reg_v3(enum DISP_MODULE_ENUM module, void *cmdq,
    char *out, struct dsi_cmd_desc *cmds, unsigned int len)
{
    int d = 0;
    u32 max_try_count = 5;
    u32 recv_data_cnt = 0; // reture value
    unsigned int read_timeout_ms; // used for polling rd_rdy
    unsigned char packet_type;
    struct DSI_RX_DATA_REG read_data0;
    struct DSI_RX_DATA_REG read_data1;
    struct DSI_RX_DATA_REG read_data2;
    struct DSI_RX_DATA_REG read_data3;
    struct DSI_T0_INS t0;
    struct DSI_T0_INS t1;
    void *temp;

#if ENABLE_DSI_INTERRUPT
    static const long WAIT_TIMEOUT = 2 * HZ;    // 2 sec
    long ret;
#endif
    u8 cmd, buffer_size, *buffer;
    unsigned char virtual_channel;

    DISPFUNC();

    buffer = (u8 *)out;
    buffer_size = (u8)len;
    cmd = (u8)cmds->dtype;
    virtual_channel = (unsigned char)cmds->vc;

    virtual_channel = ((virtual_channel << 6) | 0x3F);

    for (d = DSI_MODULE_BEGIN(module); d <= DSI_MODULE_END(module); d++) {
        if (DSI_REG[d]->DSI_MODE_CTRL.MODE) {
            // only support cmd mode read
            DISPDBG("DSI Read Fail: DSI Mode is %d\n", DSI_REG[d]->DSI_MODE_CTRL.MODE);
            return 0;
        }

        if (buffer == NULL || buffer_size == 0) {
            // illegal parameters
            DISPERR("DSI Read Fail: buffer=0x%p and buffer_size=%d\n", buffer, buffer_size);
            return 0;
        }

        do {
            if (max_try_count == 0) {
                DISPERR("DSI Read Fail: try 5 times\n");
                return 0;
            }
            max_try_count--;
            recv_data_cnt = 0;
            read_timeout_ms = 20;

            // 1. wait dsi not busy => can't read if dsi busy
            DSI_WaitForNotBusy(module, cmdq);
            // 2. Check rd_rdy & cmd_done irq
            if (DSI_REG[d]->DSI_INTEN.RD_RDY == 0) {
                //DSI_OUTREGBIT(cmdq, struct DSI_INT_ENABLE_REG, DSI_REG[d]->DSI_INTEN,
                //          RD_RDY, 1);
                DISP_REG_SET_FIELD(cmdq, DSI_INTEN_FLD_LPRX_RD_RDY_INT_EN,
                                    DSI_REG_BASE[d] + DISP_REG_DSI_INTEN, 1);

            }
            if (DSI_REG[d]->DSI_INTEN.CMD_DONE == 0) {
                //DSI_OUTREGBIT(cmdq, struct DSI_INT_ENABLE_REG, DSI_REG[d]->DSI_INTEN,
                         // CMD_DONE, 1);
                DISP_REG_SET_FIELD(cmdq, DSI_INTEN_FLD_CMD_DONE_INT_EN,
                                    DSI_REG_BASE[d] + DISP_REG_DSI_INTEN, 1);

            }
            if (DSI_REG[d]->DSI_INTSTA.RD_RDY != 0
                || DSI_REG[d]->DSI_INTSTA.CMD_DONE != 0) {
                /* dump cmdq & rxdata */
                {
                    unsigned int i;

                    DISPCHECK("Last DSI Read Why not clear irq???\n");
                    DISPCHECK("DSI_CMDQ_SIZE  : %d\n",
                          AS_UINT32(DSI_REG_BASE[d] + DISP_REG_DSI_CMDQ_CON));
                    for (i = 0; i < DSI_REG[d]->DSI_CMDQ_SIZE.CMDQ_SIZE; i++) {
                        DISPCHECK("DSI_CMDQ_DATA%d : 0x%08x\n", i,
                              AS_UINT32(&DSI_CMDQ_REG[d]->data[i]));
                    }
                    DISPCHECK("DSI_RX_DATA0   : 0x%08x\n",
                          AS_UINT32(DSI_REG_BASE[d] + DISP_REG_DSI_RX_DATA03));
                    DISPCHECK("DSI_RX_DATA1   : 0x%08x\n",
                          AS_UINT32(DSI_REG_BASE[d] + DISP_REG_DSI_RX_DATA47));
                    DISPCHECK("DSI_RX_DATA2   : 0x%08x\n",
                          AS_UINT32(DSI_REG_BASE[d] +  DISP_REG_DSI_RX_DATA8B));
                    DISPCHECK("DSI_RX_DATA3   : 0x%08x\n",
                          AS_UINT32(DSI_REG_BASE[d] + DISP_REG_DSI_RX_DATAC));
                }
                /* clear irq */
                //DSI_OUTREGBIT(cmdq, struct DSI_INT_STATUS_REG, DSI_REG[d]->DSI_INTSTA,
                //          RD_RDY, 0);
                //DSI_OUTREGBIT(cmdq, struct DSI_INT_STATUS_REG, DSI_REG[d]->DSI_INTSTA,
                //          CMD_DONE, 0);
                DISP_REG_SET_FIELD(cmdq, DSI_INTSTA_FLD_LPRX_RD_RDY_INT_FLAG,
                                    DSI_REG_BASE[d] + DISP_REG_DSI_INTSTA, 0);
                DISP_REG_SET_FIELD(cmdq, DSI_INTSTA_FLD_CMD_DONE_INT_FLAG,
                                    DSI_REG_BASE[d] + DISP_REG_DSI_INTSTA, 0);
            }
            /* 3. Send cmd */
            t0.CONFG = 0x04;    /* /BTA */
            /* / 0xB0 is used to distinguish DCS cmd or Gerneric cmd, is that Right??? */
            t0.Data_ID =
                (cmd <
                 0xB0) ? DSI_DCS_READ_PACKET_ID : DSI_GERNERIC_READ_LONG_PACKET_ID;
            t0.Data_ID = t0.Data_ID & virtual_channel;
            t0.Data0 = cmd;
            t0.Data1 = 0;
            /* set max return size */
            t1.CONFG = 0x00;
            t1.Data_ID = 0x37;
            t1.Data_ID = t1.Data_ID & virtual_channel;
            t1.Data0 = buffer_size <= 10 ? buffer_size : 10;
            t1.Data1 = 0;
            temp = &t1;

            DSI_OUTREG32(cmdq, &DSI_CMDQ_REG[d]->data[0], AS_UINT32(temp));
            temp = &t0;
            DSI_OUTREG32(cmdq, &DSI_CMDQ_REG[d]->data[1], AS_UINT32(temp));
            DSI_OUTREG32(cmdq, DSI_REG_BASE[d] + DISP_REG_DSI_CMDQ_CON, 2);

            DSI_OUTREG32(cmdq, DSI_REG_BASE[d] + DISP_REG_DSI_START, 0);
            DSI_OUTREG32(cmdq, DSI_REG_BASE[d] + DISP_REG_DSI_START, 1);
            DSI_OUTREG32(cmdq, DSI_REG_BASE[d] + DISP_REG_DSI_START, 0);

            /* / the following code is to */
            /* / 1: wait read ready */
            /* / 2: ack read ready */
            /* / 3: wait for CMDQ_DONE(interrupt handler do this op) */
            /* / 4: read data */
#if ENABLE_DSI_INTERRUPT
            ret = wait_event_interruptible_timeout(_dsi_dcs_read_wait_queue,
                                                    !_IsEngineBusy(), WAIT_TIMEOUT);
            if (ret == 0) {
                DISP_LOG_PRINT(ANDROID_LOG_INFO, "DSI",
                                " Wait for DSI engine read ready timeout!!!\n");
                DSI_DumpRegisters(module, 2);
                //DSI_OUTREGBIT(cmdq, struct DSI_RACK_REG,DSI_REG[d]->DSI_RACK,DSI_RACK, 1);
                DISP_REG_SET_FIELD(cmdq, DSI_RX_RACK_FLD_RACK,
                                    DSI_REG_BASE[d] + DISP_REG_DSI_RX_RACK, 1);
                DSI_Reset();
                return 0;
            }
#else
            // wait read ready
            DISP_LOG_PRINT(ANDROID_LOG_INFO, "DSI", " Start polling DSI read ready!!!\n");
            while (DISP_REG_GET_FIELD(DSI_INTSTA_FLD_LPRX_RD_RDY_INT_FLAG,
                   DSI_REG_BASE[d] + DISP_REG_DSI_INTSTA) == 0) {
                ///keep polling
                spin(1000);
                read_timeout_ms--;

                if (read_timeout_ms == 0) {
                    DISP_LOG_PRINT(ANDROID_LOG_INFO, "DSI",
                                    " DSI Read Fail: Polling DSI read ready timeout!!!\n");
                    DSI_DumpRegisters(module, 2);

                    ///do necessary reset here
                    //DSI_OUTREGBIT(cmdq, struct DSI_RACK_REG,DSI_REG[d]->DSI_RACK,DSI_RACK, 1);
                    DISP_REG_SET_FIELD(cmdq, DSI_RX_RACK_FLD_RACK,
                                        DSI_REG_BASE[d] + DISP_REG_DSI_RX_RACK, 1);
                    DSI_Reset(module, cmdq);
                    return 0;
                }
            }

            DISP_LOG_PRINT(ANDROID_LOG_INFO, "DSI", " End polling DSI read ready!!!\n");
            // ack read ready
            //DSI_OUTREGBIT(cmdq, struct DSI_RACK_REG,DSI_REG[d]->DSI_RACK,DSI_RACK, 1);
            DISP_REG_SET_FIELD(cmdq, DSI_RX_RACK_FLD_RACK,
                                DSI_REG_BASE[d] + DISP_REG_DSI_RX_RACK, 1);
            // clear read ready irq
            //DSI_OUTREGBIT(cmdq, struct DSI_INT_STATUS_REG,DSI_REG[d]->DSI_INTSTA,RD_RDY,0);
            DISP_REG_SET_FIELD(cmdq, DSI_INTSTA_FLD_LPRX_RD_RDY_INT_FLAG,
                                DSI_REG_BASE[d] + DISP_REG_DSI_INTSTA, 0);
            // wait dsi cmd done
            read_timeout_ms = 20;
            while (DISP_REG_GET_FIELD(DSI_INTSTA_FLD_CMD_DONE_INT_FLAG,
                   DSI_REG_BASE[d] + DISP_REG_DSI_INTSTA) == 0) {
                ///keep polling
                spin(1000);
                read_timeout_ms--;
                if (read_timeout_ms == 0) {
                    DISP_LOG_PRINT(ANDROID_LOG_INFO, "DSI",
                                    " DSI Read Fail: Polling DSI cmd done timeout!!!\n");
                    DSI_DumpRegisters(module, 2);
                    ///do necessary reset here
                    //DSI_OUTREGBIT(cmdq, struct DSI_RACK_REG,DSI_REG[d]->DSI_RACK,DSI_RACK, 1);
                    DISP_REG_SET_FIELD(cmdq, DSI_RX_RACK_FLD_RACK,
                                        DSI_REG_BASE[d] + DISP_REG_DSI_RX_RACK, 1);
                    DSI_Reset(module, cmdq);
                    return 0;
                }
            }
            // clear cmd done irq
            //DSI_OUTREGBIT(cmdq, struct DSI_INT_STATUS_REG,DSI_REG[d]->DSI_INTSTA,CMD_DONE,0);
            DISP_REG_SET_FIELD(cmdq, DSI_INTSTA_FLD_CMD_DONE_INT_FLAG,
                                DSI_REG_BASE[d] + DISP_REG_DSI_INTSTA, 0);
#endif

            DSI_OUTREG32(cmdq, &read_data0, AS_UINT32(DSI_REG_BASE[d] + DISP_REG_DSI_RX_DATA03));
            DSI_OUTREG32(cmdq, &read_data1, AS_UINT32(DSI_REG_BASE[d] + DISP_REG_DSI_RX_DATA47));
            DSI_OUTREG32(cmdq, &read_data2, AS_UINT32(DSI_REG_BASE[d] + DISP_REG_DSI_RX_DATA8B));
            DSI_OUTREG32(cmdq, &read_data3, AS_UINT32(DSI_REG_BASE[d] + DISP_REG_DSI_RX_DATAC));

            {
                unsigned int i;

                DISPCHECK("DSI read begin i = %d --------------------\n",
                      5 - max_try_count);
                DISPCHECK("DSI_RX_STA     : 0x%08x\n",
                      AS_UINT32(DSI_REG_BASE[d] + DISP_REG_DSI_RX_TRIG_STA));
                DISPCHECK("DSI_CMDQ_SIZE  : %d\n",
                      AS_UINT32(DSI_REG_BASE[d] + DISP_REG_DSI_CMDQ_CON));
                for (i = 0; i < DSI_REG[d]->DSI_CMDQ_SIZE.CMDQ_SIZE; i++) {
                    DISPCHECK("DSI_CMDQ_DATA%d : 0x%08x\n", i,
                          AS_UINT32(&DSI_CMDQ_REG[d]->data[i]));
                }
                DISPCHECK("DSI_RX_DATA0   : 0x%08x\n",
                      AS_UINT32(DSI_REG_BASE[d] + DISP_REG_DSI_RX_DATA03));
                DISPCHECK("DSI_RX_DATA1   : 0x%08x\n",
                      AS_UINT32(DSI_REG_BASE[d] + DISP_REG_DSI_RX_DATA47));
                DISPCHECK("DSI_RX_DATA2   : 0x%08x\n",
                      AS_UINT32(DSI_REG_BASE[d] + DISP_REG_DSI_RX_DATA8B));
                DISPCHECK("DSI_RX_DATA3   : 0x%08x\n",
                      AS_UINT32(DSI_REG_BASE[d] + DISP_REG_DSI_RX_DATAC));
                DISPCHECK("DSI read end ----------------------------\n");
            }

            packet_type = read_data0.byte0;

            DISPCHECK("DSI read packet_type is 0x%x\n", packet_type);

            /* 0x02: acknowledge & error report */
            /* 0x11: generic short read response(1 byte return) */
            /* 0x12: generic short read response(2 byte return) */
            /* 0x1a: generic long read response */
            /* 0x1c: dcs long read response */
            /* 0x21: dcs short read response(1 byte return) */
            /* 0x22: dcs short read response(2 byte return) */
            if (packet_type == 0x1A || packet_type == 0x1C) {
                recv_data_cnt = read_data0.byte1 + read_data0.byte2 * 16;
                if (recv_data_cnt > 10) {
                    DISPCHECK
                        ("DSI read long packet data exceeds 4 bytes return size: %d\n",
                         recv_data_cnt);
                    recv_data_cnt = 10;
                }

                if (recv_data_cnt > buffer_size) {
                    DISPCHECK
                        ("DSI read long packet data exceeds buffer size return size %d\n",
                         recv_data_cnt);
                    recv_data_cnt = buffer_size;
                }
                DISPCHECK("DSI read long packet size: %d\n", recv_data_cnt);

                if (recv_data_cnt <= 4) {
                    memcpy((void *)buffer, (void *)&read_data1, recv_data_cnt);
                } else if (recv_data_cnt <= 8) {
                    memcpy((void *)buffer, (void *)&read_data1, 4);
                    memcpy((void *)buffer + 4, (void *)&read_data2,
                           recv_data_cnt - 4);
                } else {
                    memcpy((void *)buffer, (void *)&read_data1, 4);
                    memcpy((void *)buffer + 4, (void *)&read_data2, 4);
                    memcpy((void *)buffer + 8, (void *)&read_data2,
                           recv_data_cnt - 8);
                }
            } else if (packet_type == 0x11 || packet_type == 0x12 ||
                    packet_type == 0x21 || packet_type == 0x22) {
                if (packet_type == 0x11 || packet_type == 0x21)
                    recv_data_cnt = 1;
                else
                    recv_data_cnt = 2;
                if (recv_data_cnt > buffer_size) {
                    DISPCHECK
                        ("DSI read short packet data exceeds buffer size: %d\n",
                         buffer_size);
                    recv_data_cnt = buffer_size;
                    memcpy((void *)buffer, (void *)&read_data0.byte1,
                           recv_data_cnt);
                } else {
                    memcpy((void *)buffer, (void *)&read_data0.byte1,
                           recv_data_cnt);
                }

            } else if (packet_type == 0x02) {
                DISPCHECK("read return type is 0x02, re-read\n");
            } else {
                DISPCHECK("read return type is non-recognite, type = 0x%x\n",
                      packet_type);
                return 0;
            }
        } while (packet_type == 0x02);
        /*
         * here: we may receive a ACK packet which packet type is 0x02
         * (incdicates some error happened)
         * therefore we try re-read again until no ACK packet
         * But: if it is a good way to keep re-trying ???
         */
    }

    return recv_data_cnt;
}

void DSI_set_cmdq_V2(enum DISP_MODULE_ENUM module, void *cmdq, unsigned int cmd,
                    unsigned char count, unsigned char *para_list, unsigned char force_update)
{
    u32 i = 0;
    int d = 0;
    addr_t goto_addr, mask_para, set_para;
    struct DSI_T0_INS t0;
    struct DSI_T2_INS t2;
    void *temp;

    //DISPFUNC();
    for (d = DSI_MODULE_BEGIN(module); d <= DSI_MODULE_END(module); d++) {
        if (DSI_REG[d]->DSI_MODE_CTRL.MODE != 0) {
            //not in cmd mode
            struct DSI_VM_CMD_CON_REG vm_cmdq;

            memset(&vm_cmdq, 0, sizeof(struct DSI_VM_CMD_CON_REG));

            DSI_READREG32(struct DSI_VM_CMD_CON_REG *, &vm_cmdq, &DSI_REG[d]->DSI_VM_CMD_CON);
            if (cmd < 0xB0) {
                if (count > 1) {
                    vm_cmdq.LONG_PKT = 1;
                    vm_cmdq.CM_DATA_ID = DSI_DCS_LONG_PACKET_ID;
                    vm_cmdq.CM_DATA_0 = count + 1;

                    //DISP_REG_SET_FIELD(cmdq, DSI_VM_CMD_CON_FLD_LONG_PKT,
                    //                    DSI_REG_BASE[d] + DISP_REG_DSI_VM_CMD_CON, 1);
                    //DISP_REG_SET_FIELD(cmdq, DSI_VM_CMD_CON_FLD_CM_DATA_ID,
                    //                    DSI_REG_BASE[d] + DISP_REG_DSI_VM_CMD_CON,
                    //                    DSI_DCS_LONG_PACKET_ID);
                    //DISP_REG_SET_FIELD(cmdq, DSI_VM_CMD_CON_FLD_CM_DATA_0,
                    //                    DSI_REG_BASE[d] + DISP_REG_DSI_VM_CMD_CON, count+1);

                    temp = &vm_cmdq;
                    DSI_OUTREG32(cmdq, DSI_REG_BASE[d] + DISP_REG_DSI_VM_CMD_CON, AS_UINT32(temp));

                    goto_addr = (addr_t)(&DSI_VM_CMD_REG[d]->data[0].byte0);
                    mask_para = (0xFF<<((goto_addr&0x3)*8));
                    set_para = (cmd<<((goto_addr&0x3)*8));
                    DSI_MASKREG32(cmdq, goto_addr&(~0x3), mask_para, set_para);

                    for (i = 0; i < count; i++) {
                        goto_addr = (addr_t)(&DSI_VM_CMD_REG[d]->data[0].byte1) + i;
                        mask_para = (0xFF<<((goto_addr&0x3)*8));
                        set_para = (para_list[i]<<((goto_addr&0x3)*8));
                        DSI_MASKREG32(cmdq, goto_addr&(~0x3), mask_para, set_para);
                    }
                } else {
                    vm_cmdq.LONG_PKT = 0;
                    vm_cmdq.CM_DATA_0 = cmd;
                    if (count) {
                        vm_cmdq.CM_DATA_ID = DSI_DCS_SHORT_PACKET_ID_1;
                        vm_cmdq.CM_DATA_1 = para_list[0];
                    } else {
                        vm_cmdq.CM_DATA_ID = DSI_DCS_SHORT_PACKET_ID_0;
                        vm_cmdq.CM_DATA_1 = 0;
                    }
                    temp = &vm_cmdq;
                    DSI_OUTREG32(cmdq, DSI_REG_BASE[d] + DISP_REG_DSI_VM_CMD_CON, AS_UINT32(temp));
                }
            } else {
                if (count > 1) {
                    vm_cmdq.LONG_PKT = 1;
                    vm_cmdq.CM_DATA_ID = DSI_GERNERIC_LONG_PACKET_ID;
                    vm_cmdq.CM_DATA_0 = count + 1;
                    temp = &vm_cmdq;
                    DSI_OUTREG32(cmdq, DSI_REG_BASE[d] + DISP_REG_DSI_VM_CMD_CON, AS_UINT32(temp));

                    goto_addr = (addr_t)(&DSI_VM_CMD_REG[d]->data[0].byte0);
                    mask_para = (0xFF<<((goto_addr&0x3)*8));
                    set_para = (cmd<<((goto_addr&0x3)*8));
                    DSI_MASKREG32(cmdq, goto_addr&(~0x3), mask_para, set_para);

                    for (i = 0; i < count; i++) {
                        goto_addr = (addr_t)(&DSI_VM_CMD_REG[d]->data[0].byte1) + i;
                        mask_para = (0xFF<<((goto_addr&0x3)*8));
                        set_para = (para_list[i]<<((goto_addr&0x3)*8));
                        DSI_MASKREG32(cmdq, goto_addr&(~0x3), mask_para, set_para);
                    }
                } else {
                    vm_cmdq.LONG_PKT = 0;
                    vm_cmdq.CM_DATA_0 = cmd;
                    if (count) {
                        vm_cmdq.CM_DATA_ID = DSI_GERNERIC_SHORT_PACKET_ID_2;
                        vm_cmdq.CM_DATA_1 = para_list[0];
                    } else {
                        vm_cmdq.CM_DATA_ID = DSI_GERNERIC_SHORT_PACKET_ID_1;
                        vm_cmdq.CM_DATA_1 = 0;
                    }
                    temp = &vm_cmdq;
                    DSI_OUTREG32(cmdq, DSI_REG_BASE[d] + DISP_REG_DSI_VM_CMD_CON, AS_UINT32(temp));
                }
            }
        } else {
#ifdef ENABLE_DSI_ERROR_REPORT
            if ((para_list[0] & 1)) {
                memset(_dsi_cmd_queue, 0, sizeof(_dsi_cmd_queue));
                memcpy(_dsi_cmd_queue, para_list, count);
                _dsi_cmd_queue[(count+3)/4*4] = 0x4;
                count = (count+3)/4*4 + 4;
                para_list = (unsigned char *) _dsi_cmd_queue;
            } else {
                para_list[0] |= 4;
            }
#endif
            DSI_WaitForNotBusy(module, cmdq);

            if (cmd < 0xB0) {
                if (count > 1) {
                    t2.CONFG = 2;
                    t2.Data_ID = DSI_DCS_LONG_PACKET_ID;
                    t2.WC16 = count + 1;
                    temp = &t2;
                    DSI_OUTREG32(cmdq, &DSI_CMDQ_REG[d]->data[0], AS_UINT32(temp));

                    goto_addr = (addr_t)(&DSI_CMDQ_REG[d]->data[1].byte0);
                    mask_para = (0xFF<<((goto_addr&0x3)*8));
                    set_para = (cmd<<((goto_addr&0x3)*8));
                    DSI_MASKREG32(cmdq, goto_addr&(~0x3), mask_para, set_para);

                    for (i = 0; i < count; i++) {
                        goto_addr = (addr_t)(&DSI_CMDQ_REG[d]->data[1].byte1) + i;
                        mask_para = (0xFF<<((goto_addr&0x3)*8));
                        set_para = (para_list[i]<<((goto_addr&0x3)*8));
                        DSI_MASKREG32(cmdq, goto_addr&(~0x3), mask_para, set_para);
                    }

                    DSI_OUTREG32(cmdq, DSI_REG_BASE[d] + DISP_REG_DSI_CMDQ_CON, 2+(count)/4);
                } else {
                    t0.CONFG = 0;
                    t0.Data0 = cmd;
                    if (count) {
                        t0.Data_ID = DSI_DCS_SHORT_PACKET_ID_1;
                        t0.Data1 = para_list[0];
                    } else {
                        t0.Data_ID = DSI_DCS_SHORT_PACKET_ID_0;
                        t0.Data1 = 0;
                    }
                    temp = &t0;

                    DSI_OUTREG32(cmdq, &DSI_CMDQ_REG[d]->data[0], AS_UINT32(temp));
                    DSI_OUTREG32(cmdq, DSI_REG_BASE[d] + DISP_REG_DSI_CMDQ_CON, 1);
                }
            } else {
                if (count > 1) {
                    t2.CONFG = 2;
                    t2.Data_ID = DSI_GERNERIC_LONG_PACKET_ID;
                    t2.WC16 = count + 1;
                    temp = &t2;
                    DSI_OUTREG32(cmdq, &DSI_CMDQ_REG[d]->data[0], AS_UINT32(temp));

                    goto_addr = (addr_t)(&DSI_CMDQ_REG[d]->data[1].byte0);
                    mask_para = (0xFF<<((goto_addr&0x3)*8));
                    set_para = (cmd<<((goto_addr&0x3)*8));
                    DSI_MASKREG32(cmdq, goto_addr&(~0x3), mask_para, set_para);

                    for (i = 0; i < count; i++) {
                        goto_addr = (addr_t)(&DSI_CMDQ_REG[d]->data[1].byte1) + i;
                        mask_para = (0xFF<<((goto_addr&0x3)*8));
                        set_para = (para_list[i]<<((goto_addr&0x3)*8));
                        DSI_MASKREG32(cmdq, goto_addr&(~0x3), mask_para, set_para);
                    }

                    DSI_OUTREG32(cmdq, DSI_REG_BASE[d] + DISP_REG_DSI_CMDQ_CON, 2+(count)/4);

                } else {
                    t0.CONFG = 0;
                    t0.Data0 = cmd;
                    if (count) {
                        t0.Data_ID = DSI_GERNERIC_SHORT_PACKET_ID_2;
                        t0.Data1 = para_list[0];
                    } else {
                        t0.Data_ID = DSI_GERNERIC_SHORT_PACKET_ID_1;
                        t0.Data1 = 0;
                    }
                    temp = &t0;
                    DSI_OUTREG32(cmdq, &DSI_CMDQ_REG[d]->data[0], AS_UINT32(temp));
                    DSI_OUTREG32(cmdq, DSI_REG_BASE[d] + DISP_REG_DSI_CMDQ_CON, 1);
                }
            }
        }
    }

    if (((module != DISP_MODULE_DSI1) && (DSI_REG[0]->DSI_MODE_CTRL.MODE != 0))
        || ((module == DISP_MODULE_DSI1) && (DSI_REG[1]->DSI_MODE_CTRL.MODE != 0))) {
         //not in cmd mode
        /* start DSI VM CMDQ */
        if (force_update)
            DSI_EnableVM_CMD(module, cmdq);
    } else {
        if (force_update) {
            if (module == DISP_MODULE_DSIDUAL)
                DISP_REG_SET_FIELD(cmdq, DSI_COM_CON_FLD_DSI_DUAL_EN,
                                DSI_REG_BASE[1] + DISP_REG_DSI_COM_CON, 1);

            DSI_Start(module, cmdq);
            DSI_WaitForNotBusy(module, cmdq);
            if (module == DISP_MODULE_DSIDUAL)
                DISP_REG_SET_FIELD(cmdq, DSI_COM_CON_FLD_DSI_DUAL_EN,
                                DSI_REG_BASE[1] + DISP_REG_DSI_COM_CON, 0);
        }
    }
}


void DSI_set_cmdq_V3(enum DISP_MODULE_ENUM module, void *cmdq, LCM_setting_table_V3 *para_tbl,
                        unsigned int size, unsigned char force_update)
{
    u32 i;
    //u32 layer, layer_state, lane_num;
    addr_t goto_addr, mask_para, set_para;
    //u32 fbPhysAddr, fbVirAddr;
    struct DSI_T0_INS t0;
    //struct DSI_T1_INS t1;
    struct DSI_T2_INS t2;
    void *temp;

    u32 index = 0;

    unsigned char data_id, cmd, count;
    unsigned char *para_list;
    u32 d;

    for (d = DSI_MODULE_BEGIN(module); d <= DSI_MODULE_END(module); d++) {
        do {
            data_id = para_tbl[index].id;
            cmd = para_tbl[index].cmd;
            count = para_tbl[index].count;
            para_list = para_tbl[index].para_list;

            if (data_id == REGFLAG_ESCAPE_ID && cmd == REGFLAG_DELAY_MS_V3) {
                spin(1000*count);
                DISPMSG("DISP/DSI %s[%d]. Delay %d (ms)\n", __func__, index, count);

                continue;
            }
            if (DSI_REG[d]->DSI_MODE_CTRL.MODE != 0) {
                //not in cmd mode
                struct DSI_VM_CMD_CON_REG vm_cmdq;

                temp = &vm_cmdq;
                writel(AS_UINT32(DSI_REG_BASE[d] + DISP_REG_DSI_VM_CMD_CON), temp);
                DISPMSG("set cmdq in VDO mode\n");
                if (count > 1) {
                    vm_cmdq.LONG_PKT = 1;
                    vm_cmdq.CM_DATA_ID = data_id;
                    vm_cmdq.CM_DATA_0 = count + 1;
                    temp = &vm_cmdq;
                    writel(AS_UINT32(temp), DSI_REG_BASE[d] + DISP_REG_DSI_VM_CMD_CON);

                    goto_addr = (addr_t)(&DSI_VM_CMD_REG[d]->data[0].byte0);
                    mask_para = (0xFF<<((goto_addr&0x3)*8));
                    set_para = (cmd<<((goto_addr&0x3)*8));
                    writel((readl(goto_addr&(~0x3)) & ~(mask_para)) | (set_para), goto_addr&(~0x3));

                    for (i = 0; i < count; i++) {
                        goto_addr = (addr_t)(&DSI_VM_CMD_REG[d]->data[0].byte1) + i;
                        mask_para = (0xFF<<((goto_addr&0x3)*8));
                        set_para = (para_list[i]<<((goto_addr&0x3)*8));
                        writel((readl(goto_addr&(~0x3)) & ~(mask_para)) | (set_para),
                                goto_addr&(~0x3));
                    }
                } else {
                    vm_cmdq.LONG_PKT = 0;
                    vm_cmdq.CM_DATA_0 = cmd;
                    if (count) {
                        vm_cmdq.CM_DATA_ID = data_id;
                        vm_cmdq.CM_DATA_1 = para_list[0];
                    } else {
                        vm_cmdq.CM_DATA_ID = data_id;
                        vm_cmdq.CM_DATA_1 = 0;
                    }
                    temp = &vm_cmdq;
                    writel(AS_UINT32(temp), DSI_REG_BASE[d] + DISP_REG_DSI_VM_CMD_CON);
                }

                if (force_update)
                    DSI_EnableVM_CMD(module, cmdq);

            } else {
                DSI_WaitForNotBusy(module, cmdq);

                writel(0, &DSI_CMDQ_REG[d]->data[0]);

                if (count > 1) {
                    t2.CONFG = 2;
                    t2.Data_ID = data_id;
                    t2.WC16 = count + 1;
                    temp = &t2;
                    //DSI_OUTREG32(cmdq, &DSI_CMDQ_REG[d]->data[0].byte0, AS_UINT32(temp));
                    DSI_OUTREG32(cmdq, &DSI_CMDQ_REG[d]->data[0], AS_UINT32(temp));

                    goto_addr = (addr_t)(&DSI_CMDQ_REG[d]->data[1].byte0);
                    mask_para = (0xFFu<<((goto_addr&0x3u)*8));
                    set_para = (cmd<<((goto_addr&0x3u)*8));
                    DSI_MASKREG32(cmdq, goto_addr&(~(0x3u)), mask_para, set_para);

                    for (i = 0; i < count; i++) {
                        goto_addr = (addr_t)(&DSI_CMDQ_REG[d]->data[1].byte1) + i;
                        mask_para = (0xFFu<<((goto_addr&0x3u)*8));
                        set_para = (para_list[i]<<((goto_addr&0x3u)*8));
                        DSI_MASKREG32(cmdq, goto_addr&(~(0x3u)), mask_para,
                                        set_para);
                    }

                    DSI_OUTREG32(cmdq, DSI_REG_BASE[d] + DISP_REG_DSI_CMDQ_CON, 2+(count)/4);
                } else {
                    t0.CONFG = 0;
                    t0.Data0 = cmd;
                    if (count) {
                        t0.Data_ID = data_id;
                        t0.Data1 = para_list[0];
                    } else {
                        t0.Data_ID = data_id;
                        t0.Data1 = 0;
                    }
                    temp = &t0;
                    DSI_OUTREG32(cmdq, &DSI_CMDQ_REG[d]->data[0], AS_UINT32(temp));
                    DSI_OUTREG32(cmdq, DSI_REG_BASE[d] + DISP_REG_DSI_CMDQ_CON, 1);
                }

                if (force_update) {
                    DSI_Start(module, cmdq);
                    DSI_WaitForNotBusy(module, cmdq);
                }

            }
        } while (++index < size);
    }
}

void DSI_set_cmdq_V4(enum DISP_MODULE_ENUM module, void *cmdq,
    struct dsi_cmd_desc *cmds)
{
    u32 i = 0;
    int d = 0;
    addr_t goto_addr, mask_para, set_para;
    struct DSI_T0_INS t0;
    struct DSI_T2_INS t2;
    void *temp;
    unsigned int cmd;
    unsigned char count;
    unsigned char *para_list;
    unsigned char virtual_channel;

    //DISPFUNC();

    cmd = cmds->dtype;
    count = (unsigned char)cmds->dlen;
    para_list = (unsigned char *)cmds->payload;
    virtual_channel = (unsigned char)cmds->vc;

    virtual_channel = ((virtual_channel << 6) | 0x3F);

    if (cmds->link_state == 0)
        /* Switch to HS mode*/
        DSI_clk_HS_mode(module, cmdq, true);

    for (d = DSI_MODULE_BEGIN(module); d <= DSI_MODULE_END(module); d++) {
        if (DSI_REG[d]->DSI_MODE_CTRL.MODE != 0) {
            //not in cmd mode
            struct DSI_VM_CMD_CON_REG vm_cmdq;

            memset(&vm_cmdq, 0, sizeof(struct DSI_VM_CMD_CON_REG));

            DSI_READREG32(struct DSI_VM_CMD_CON_REG *, &vm_cmdq, &DSI_REG[d]->DSI_VM_CMD_CON);
            if (cmd < 0xB0) {
                if (count > 1) {
                    vm_cmdq.LONG_PKT = 1;
                    vm_cmdq.CM_DATA_ID = DSI_DCS_LONG_PACKET_ID;
                    vm_cmdq.CM_DATA_ID =
                        vm_cmdq.CM_DATA_ID & virtual_channel;
                    vm_cmdq.CM_DATA_0 = count + 1;

                    //DISP_REG_SET_FIELD(cmdq, DSI_VM_CMD_CON_FLD_LONG_PKT,
                    //                    DSI_REG_BASE[d] + DISP_REG_DSI_VM_CMD_CON, 1);
                    //DISP_REG_SET_FIELD(cmdq, DSI_VM_CMD_CON_FLD_CM_DATA_ID,
                    //                    DSI_REG_BASE[d] + DISP_REG_DSI_VM_CMD_CON,
                    //                    DSI_DCS_LONG_PACKET_ID);
                    //DISP_REG_SET_FIELD(cmdq, DSI_VM_CMD_CON_FLD_CM_DATA_0,
                    //                    DSI_REG_BASE[d] + DISP_REG_DSI_VM_CMD_CON,
                    //                      count+1);

                    temp = &vm_cmdq;
                    DSI_OUTREG32(cmdq, DSI_REG_BASE[d] + DISP_REG_DSI_VM_CMD_CON, AS_UINT32(temp));

                    goto_addr = (addr_t)(&DSI_VM_CMD_REG[d]->data[0].byte0);
                    mask_para = (0xFF<<((goto_addr&0x3)*8));
                    set_para = (cmd<<((goto_addr&0x3)*8));
                    DSI_MASKREG32(cmdq, goto_addr&(~0x3), mask_para, set_para);

                    for (i = 0; i < count; i++) {
                        goto_addr = (addr_t)(&DSI_VM_CMD_REG[d]->data[0].byte1) + i;
                        mask_para = (0xFF<<((goto_addr&0x3)*8));
                        set_para = (para_list[i]<<((goto_addr&0x3)*8));
                        DSI_MASKREG32(cmdq, goto_addr&(~0x3), mask_para, set_para);
                    }
                } else {
                    vm_cmdq.LONG_PKT = 0;
                    vm_cmdq.CM_DATA_0 = cmd;
                    if (count) {
                        vm_cmdq.CM_DATA_ID = DSI_DCS_SHORT_PACKET_ID_1;
                        vm_cmdq.CM_DATA_ID =
                            vm_cmdq.CM_DATA_ID & virtual_channel;
                        vm_cmdq.CM_DATA_1 = para_list[0];
                    } else {
                        vm_cmdq.CM_DATA_ID = DSI_DCS_SHORT_PACKET_ID_0;
                        vm_cmdq.CM_DATA_ID =
                            vm_cmdq.CM_DATA_ID & virtual_channel;
                        vm_cmdq.CM_DATA_1 = 0;
                    }
                    temp = &vm_cmdq;
                    DSI_OUTREG32(cmdq, DSI_REG_BASE[d] + DISP_REG_DSI_VM_CMD_CON, AS_UINT32(temp));
                }
            } else {
                if (count > 1) {
                    vm_cmdq.LONG_PKT = 1;
                    vm_cmdq.CM_DATA_ID = DSI_GERNERIC_LONG_PACKET_ID;
                    vm_cmdq.CM_DATA_ID =
                        vm_cmdq.CM_DATA_ID & virtual_channel;
                    vm_cmdq.CM_DATA_0 = count + 1;
                    temp = &vm_cmdq;
                    DSI_OUTREG32(cmdq, DSI_REG_BASE[d] + DISP_REG_DSI_VM_CMD_CON, AS_UINT32(temp));

                    goto_addr = (addr_t)(&DSI_VM_CMD_REG[d]->data[0].byte0);
                    mask_para = (0xFF<<((goto_addr&0x3)*8));
                    set_para = (cmd<<((goto_addr&0x3)*8));
                    DSI_MASKREG32(cmdq, goto_addr&(~0x3), mask_para, set_para);

                    for (i = 0; i < count; i++) {
                        goto_addr = (addr_t)(&DSI_VM_CMD_REG[d]->data[0].byte1) + i;
                        mask_para = (0xFF<<((goto_addr&0x3)*8));
                        set_para = (para_list[i]<<((goto_addr&0x3)*8));
                        DSI_MASKREG32(cmdq, goto_addr&(~0x3), mask_para, set_para);
                    }
                } else {
                    vm_cmdq.LONG_PKT = 0;
                    vm_cmdq.CM_DATA_0 = cmd;
                    if (count) {
                        vm_cmdq.CM_DATA_ID = DSI_GERNERIC_SHORT_PACKET_ID_2;
                        vm_cmdq.CM_DATA_ID =
                            vm_cmdq.CM_DATA_ID & virtual_channel;
                        vm_cmdq.CM_DATA_1 = para_list[0];
                    } else {
                        vm_cmdq.CM_DATA_ID = DSI_GERNERIC_SHORT_PACKET_ID_1;
                        vm_cmdq.CM_DATA_ID =
                            vm_cmdq.CM_DATA_ID & virtual_channel;
                        vm_cmdq.CM_DATA_1 = 0;
                    }
                    temp = &vm_cmdq;
                    DSI_OUTREG32(cmdq, DSI_REG_BASE[d] + DISP_REG_DSI_VM_CMD_CON, AS_UINT32(temp));
                }
            }
        } else {
#ifdef ENABLE_DSI_ERROR_REPORT
            if ((para_list[0] & 1)) {
                memset(_dsi_cmd_queue, 0, sizeof(_dsi_cmd_queue));
                memcpy(_dsi_cmd_queue, para_list, count);
                _dsi_cmd_queue[(count+3)/4*4] = 0x4;
                count = (count+3)/4*4 + 4;
                para_list = (unsigned char *) _dsi_cmd_queue;
            } else {
                para_list[0] |= 4;
            }
#endif
            DSI_WaitForNotBusy(module, cmdq);

            if (cmd < 0xB0) {
                if (count > 1) {
                    t2.CONFG = 2;
                    if (cmds->link_state == 0)
                        /* HS Tx transmission */
                        t2.CONFG = t2.CONFG | 0x08;
                    t2.Data_ID = DSI_DCS_LONG_PACKET_ID;
                    t2.Data_ID = t2.Data_ID & virtual_channel;
                    t2.WC16 = count + 1;
                    temp = &t2;
                    DSI_OUTREG32(cmdq, &DSI_CMDQ_REG[d]->data[0], AS_UINT32(temp));

                    goto_addr = (addr_t)(&DSI_CMDQ_REG[d]->data[1].byte0);
                    mask_para = (0xFF<<((goto_addr&0x3)*8));
                    set_para = (cmd<<((goto_addr&0x3)*8));
                    DSI_MASKREG32(cmdq, goto_addr&(~0x3), mask_para, set_para);

                    for (i = 0; i < count; i++) {
                        goto_addr = (addr_t)(&DSI_CMDQ_REG[d]->data[1].byte1) + i;
                        mask_para = (0xFF<<((goto_addr&0x3)*8));
                        set_para = (para_list[i]<<((goto_addr&0x3)*8));
                        DSI_MASKREG32(cmdq, goto_addr&(~0x3), mask_para, set_para);
                    }

                    DSI_OUTREG32(cmdq, DSI_REG_BASE[d] + DISP_REG_DSI_CMDQ_CON, 2+(count)/4);
                } else {
                    t0.CONFG = 0;
                    if (cmds->link_state == 0)
                        /* HS Tx transmission */
                        t0.CONFG = t0.CONFG | 0x08;
                    t0.Data0 = cmd;
                    if (count) {
                        t0.Data_ID = DSI_DCS_SHORT_PACKET_ID_1;
                        t0.Data_ID = t0.Data_ID & virtual_channel;
                        t0.Data1 = para_list[0];
                    } else {
                        t0.Data_ID = DSI_DCS_SHORT_PACKET_ID_0;
                        t0.Data_ID = t0.Data_ID & virtual_channel;
                        t0.Data1 = 0;
                    }
                    temp = &t0;

                    DSI_OUTREG32(cmdq, &DSI_CMDQ_REG[d]->data[0], AS_UINT32(temp));
                    DSI_OUTREG32(cmdq, DSI_REG_BASE[d] + DISP_REG_DSI_CMDQ_CON, 1);
                }
            } else {
                if (count > 1) {
                    t2.CONFG = 2;
                    if (cmds->link_state == 0)
                        /* HS Tx transmission */
                        t2.CONFG = t2.CONFG | 0x08;
                    t2.Data_ID = DSI_GERNERIC_LONG_PACKET_ID;
                    t2.Data_ID = t2.Data_ID & virtual_channel;
                    t2.WC16 = count + 1;
                    temp = &t2;
                    DSI_OUTREG32(cmdq, &DSI_CMDQ_REG[d]->data[0], AS_UINT32(temp));

                    goto_addr = (addr_t)(&DSI_CMDQ_REG[d]->data[1].byte0);
                    mask_para = (0xFF<<((goto_addr&0x3)*8));
                    set_para = (cmd<<((goto_addr&0x3)*8));
                    DSI_MASKREG32(cmdq, goto_addr&(~0x3), mask_para, set_para);

                    for (i = 0; i < count; i++) {
                        goto_addr = (addr_t)(&DSI_CMDQ_REG[d]->data[1].byte1) + i;
                        mask_para = (0xFF<<((goto_addr&0x3)*8));
                        set_para = (para_list[i]<<((goto_addr&0x3)*8));
                        DSI_MASKREG32(cmdq, goto_addr&(~0x3), mask_para, set_para);
                    }

                    DSI_OUTREG32(cmdq, DSI_REG_BASE[d] + DISP_REG_DSI_CMDQ_CON, 2+(count)/4);

                } else {
                    t0.CONFG = 0;
                    if (cmds->link_state == 0)
                        /* HS Tx transmission */
                        t0.CONFG = t0.CONFG | 0x08;
                    t0.Data0 = cmd;
                    if (count) {
                        t0.Data_ID = DSI_GERNERIC_SHORT_PACKET_ID_2;
                        t0.Data_ID = t0.Data_ID & virtual_channel;
                        t0.Data1 = para_list[0];
                    } else {
                        t0.Data_ID = DSI_GERNERIC_SHORT_PACKET_ID_1;
                        t0.Data_ID = t0.Data_ID & virtual_channel;
                        t0.Data1 = 0;
                    }
                    temp = &t0;
                    DSI_OUTREG32(cmdq, &DSI_CMDQ_REG[d]->data[0], AS_UINT32(temp));
                    DSI_OUTREG32(cmdq, DSI_REG_BASE[d] + DISP_REG_DSI_CMDQ_CON, 1);
                }
            }
        }
    }

    //not in cmd mode
    if (((module != DISP_MODULE_DSI1) && (DSI_REG[0]->DSI_MODE_CTRL.MODE != 0))
        || ((module == DISP_MODULE_DSI1) && (DSI_REG[1]->DSI_MODE_CTRL.MODE != 0))) {
        /* start DSI VM CMDQ */
        DSI_EnableVM_CMD(module, cmdq);
    } else {
        if (module == DISP_MODULE_DSIDUAL)
            DISP_REG_SET_FIELD(cmdq, DSI_COM_CON_FLD_DSI_DUAL_EN,
                                DSI_REG_BASE[1] + DISP_REG_DSI_COM_CON, 1);

        DSI_Start(module, cmdq);
        DSI_WaitForNotBusy(module, cmdq);

        if (module == DISP_MODULE_DSIDUAL)
            DISP_REG_SET_FIELD(cmdq, DSI_COM_CON_FLD_DSI_DUAL_EN,
                                DSI_REG_BASE[1] + DISP_REG_DSI_COM_CON, 0);
    }

    /* Revert to LP mode */
    if (cmds->link_state == 0)
        /* Switch to HS mode*/
        DSI_clk_HS_mode(module, cmdq, false);

}


void DSI_set_cmdq(enum DISP_MODULE_ENUM module, void *cmdq, unsigned int *pdata,
                    unsigned int queue_size, unsigned char force_update)
{
    DISPFUNC();

    unsigned int j = 0;
    int i = 0;
    const char *module_name = ddp_get_module_name(module);

    DISPCHECK("%s, module=%s, cmdq=0x%p\n", __func__, module_name, cmdq);

    for (i = DSI_MODULE_BEGIN(module); i <= DSI_MODULE_END(module); i++) {
        if (DSI_REG[i]->DSI_MODE_CTRL.MODE != 0) {
#if 0
            //not in cmd mode
            DSI_VM_CMD_CON_REG vm_cmdq;

            writel(AS_UINT32(&DSI_REG[i]->DSI_VM_CMD_CON), &vm_cmdq);
            DISPMSG("set cmdq in VDO mode\n");
            if (queue_size > 1) {
                //long packet
                vm_cmdq.LONG_PKT = 1;
                vm_cmdq.CM_DATA_ID = ((pdata[0] >> 8) & 0xFF);
                vm_cmdq.CM_DATA_0 = ((pdata[0] >> 16) & 0xFF);
                vm_cmdq.CM_DATA_1 = 0;
                writel(AS_UINT32(&vm_cmdq), &DSI_REG[i]->DSI_VM_CMD_CON);
                for (j = 0; j < queue_size-1; j++)
                    writel(AS_UINT32((pdata+j+1)), &DSI_VM_CMD_REG->data[j]);

            } else {
                vm_cmdq.LONG_PKT = 0;
                vm_cmdq.CM_DATA_ID = ((pdata[0] >> 8) & 0xFF);
                vm_cmdq.CM_DATA_0 = ((pdata[0] >> 16) & 0xFF);
                vm_cmdq.CM_DATA_1 = ((pdata[0] >> 24) & 0xFF);
                writel(AS_UINT32(&vm_cmdq), &DSI_REG->DSI_VM_CMD_CON);
            }
#endif
        } else {
            ASSERT(queue_size <= 32);
            DSI_WaitForNotBusy(module, cmdq);
#ifdef ENABLE_DSI_ERROR_REPORT
            if ((pdata[0] & 1)) {
                memcpy(_dsi_cmd_queue, pdata, queue_size*4);
                _dsi_cmd_queue[queue_size++] = 0x4;
                pdata = (unsigned int *) _dsi_cmd_queue;
            } else {
                pdata[0] |= 4;
            }
#endif

            for (j = 0; j < queue_size; j++)
                DSI_OUTREG32(cmdq, &DSI_CMDQ_REG[i]->data[j], AS_UINT32((pdata+j)));

            DSI_OUTREG32(cmdq, DSI_REG_BASE[i] + DISP_REG_DSI_CMDQ_CON, queue_size);

            for (j = 0; j < queue_size; j++)
                DISPMSG("[DISP] - kernel - %s. DSI_CMDQ+%04x : 0x%08x\n", __func__,
                            j*4, readl(DISP_DSI0_BASE + 0x200 + j*4));

        }
    }

    if ((module != DISP_MODULE_DSI1 && DSI_REG[0]->DSI_MODE_CTRL.MODE != 0)
        || (module == DISP_MODULE_DSI1 && DSI_REG[1]->DSI_MODE_CTRL.MODE != 0)) {
    //not in cmd mode
#if 0
        //start DSI VM CMDQ
        if (force_update) {
            MMProfileLogEx(MTKFB_MMP_Events.DSICmd, MMProfileFlagStart,
                        *(unsigned int *)(&DSI_VM_CMD_REG->data[0]),
                        *(unsigned int *)(&DSI_VM_CMD_REG->data[1]));
            DSI_EnableVM_CMD();

            //must wait VM CMD done?
            MMProfileLogEx(MTKFB_MMP_Events.DSICmd, MMProfileFlagEnd,
                        *(unsigned int *)(&DSI_VM_CMD_REG->data[2]),
                        *(unsigned int *)(&DSI_VM_CMD_REG->data[3]));
        }
#endif
    } else {
        if (force_update) {
            if (module == DISP_MODULE_DSIDUAL)
                DISP_REG_SET_FIELD(cmdq, DSI_COM_CON_FLD_DSI_DUAL_EN,
                                DSI_REG_BASE[1] + DISP_REG_DSI_COM_CON, 1);

            DSI_Start(module, cmdq);
            DSI_WaitForNotBusy(module, cmdq);

            if (module == DISP_MODULE_DSIDUAL)
                DISP_REG_SET_FIELD(cmdq, DSI_COM_CON_FLD_DSI_DUAL_EN,
                                DSI_REG_BASE[1] + DISP_REG_DSI_COM_CON, 0);
        }
    }
}

void _copy_dsi_params(LCM_DSI_PARAMS *src, LCM_DSI_PARAMS *dst)
{
    memcpy((LCM_DSI_PARAMS *)dst, (LCM_DSI_PARAMS *)src, sizeof(LCM_DSI_PARAMS));
}

int ddp_dsi_init(enum DISP_MODULE_ENUM module, void *cmdq)
{
    enum DSI_STATUS ret = DSI_STATUS_OK;
    int i = 0;

    DISPFUNC();

    //DSI_OUTREG32(cmdq, 0x10000048, 0x80000000);
    ddp_enable_module_clock(module);
    for (i = DSI_MODULE_BEGIN(module); i <= DSI_MODULE_END(module); i++) {
        memset(&_dsi_context[i], 0, sizeof(_dsi_context[i]));
        DISPCHECK("dsi%d init finished\n", i);
    }

    return ret;
}

int ddp_dsi_deinit(enum DISP_MODULE_ENUM module, void *cmdq)
{
    int i = 0;

    DSI_SetMode(module, NULL, CMD_MODE);
    DSI_clk_HS_mode(module, NULL, false);
    DSI_enter_ULPS(module);
    ddp_disable_module_clock(module);
    DSI_PHY_clk_switch(module, NULL, false);
    for (i = DSI_MODULE_BEGIN(module); i <= DSI_MODULE_END(module); i++) {
        memset(&_dsi_context[i], 0, sizeof(_dsi_context[i]));
        DISPCHECK("dsi%d init finished\n", i);
    }

    return 0;
}

void _dump_dsi_params(LCM_DSI_PARAMS *dsi_config)
{
    //int i = 0;

    if (dsi_config) {
        switch (dsi_config->mode) {
        case CMD_MODE:
            DISPCHECK("[DDPDSI] DSI Mode: CMD_MODE\n");
            break;
        case SYNC_PULSE_VDO_MODE:
            DISPCHECK("[DDPDSI] DSI Mode: SYNC_PULSE_VDO_MODE\n");
            break;
        case SYNC_EVENT_VDO_MODE:
            DISPCHECK("[DDPDSI] DSI Mode: SYNC_EVENT_VDO_MODE\n");
            break;
        case BURST_VDO_MODE:
            DISPCHECK("[DDPDSI] DSI Mode: BURST_VDO_MODE\n");
            break;
        default:
            DISPCHECK("[DDPDSI] DSI Mode: Unknown\n");
            break;
        }

        DISPCHECK("[DDPDSI] LANE_NUM: %d,data_format:(%d,%d,%d,%d)\n", dsi_config->LANE_NUM,
                    dsi_config->data_format.color_order, dsi_config->data_format.format,
                    dsi_config->data_format.padding, dsi_config->data_format.trans_seq);


        DISPCHECK("[DDPDSI] vact:%d, vbp:%d, vfp:%d, vact_line:%d\n",
                    dsi_config->vertical_sync_active, dsi_config->vertical_backporch,
                    dsi_config->vertical_frontporch, dsi_config->vertical_active_line);
        DISPCHECK("[DDPDSI] hact:%d, hbp:%d, hfp:%d, hblank:%d\n",
                    dsi_config->horizontal_sync_active, dsi_config->horizontal_backporch,
                    dsi_config->horizontal_frontporch, dsi_config->horizontal_blanking_pixel);
        DISPCHECK("[DDPDSI] pll_select:%d, pll_div1:%d, pll_div2:%d, fbk_div:%d, fbk_sel:%d\n",
                    dsi_config->pll_select, dsi_config->pll_div1, dsi_config->pll_div2,
                    dsi_config->fbk_div, dsi_config->fbk_sel);
        DISPCHECK("[DDPDSI] rg_bir:%d, rg_bic:%d, rg_bp:%d, PLL_CLOCK:%d, dsi_clk:%d\n",
                    dsi_config->rg_bir, dsi_config->rg_bic,  dsi_config->rg_bp,
                    dsi_config->PLL_CLOCK, dsi_config->dsi_clock);
        DISPCHECK("[DDPDSI] ssc_range:%d, ssc_disable:%d, compatibility_for_nvk:%d, cont_clk:%d\n",
                    dsi_config->ssc_range, dsi_config->ssc_disable,
                    dsi_config->compatibility_for_nvk, dsi_config->cont_clock);
        DISPCHECK("[DDPDSI] lcm_ext_te_enable:%d, noncont_clk:%d, noncont_clock_period:%d\n",
                    dsi_config->lcm_ext_te_enable, dsi_config->noncont_clock,
                    dsi_config->noncont_clock_period);
    }

    return;
}

void DSI_Set_LFR(enum DISP_MODULE_ENUM module, void *cmdq, unsigned int mode,
                 unsigned int type, unsigned int enable, unsigned int skip_num)
{
    //LFR_MODE 0 disable,1 static mode ,2 dynamic mode 3,both
    unsigned int i = 0;

    for (i = DSI_MODULE_BEGIN(module); i <= DSI_MODULE_END(module); i++) {
        //DSI_OUTREGBIT(cmdq, struct DSI_LFR_CON_REG,DSI_REG[i]->DSI_LFR_CON,
        //                LFR_MODE, mode);
        //DSI_OUTREGBIT(cmdq, struct DSI_LFR_CON_REG,DSI_REG[i]->DSI_LFR_CON,
        //                LFR_TYPE, type);
        //DSI_OUTREGBIT(cmdq, struct DSI_LFR_CON_REG,DSI_REG[i]->DSI_LFR_CON,
        //                LFR_EN, enable);
        //DSI_OUTREGBIT(cmdq, struct DSI_LFR_CON_REG,DSI_REG[i]->DSI_LFR_CON,
        //                LFR_UPDATE, 1);
        //DSI_OUTREGBIT(cmdq, struct DSI_LFR_CON_REG,DSI_REG[i]->DSI_LFR_CON,
        //                LFR_VSE_DIS, 0);
        //DSI_OUTREGBIT(cmdq, struct DSI_LFR_CON_REG,DSI_REG[i]->DSI_LFR_CON,
        //                LFR_SKIP_NUM, skip_num);

        DISP_REG_SET_FIELD(cmdq, DSI_LFR_CON_FLD_DSI_LFR_MODE,
                            DSI_REG_BASE[i] + DISP_REG_DSI_LFR_CON, mode);
        DISP_REG_SET_FIELD(cmdq, DSI_LFR_CON_FLD_DSI_LFR_TYPE,
                            DSI_REG_BASE[i] + DISP_REG_DSI_LFR_CON, type);
        DISP_REG_SET_FIELD(cmdq, DSI_LFR_CON_FLD_DSI_LFR_EN,
                            DSI_REG_BASE[i] + DISP_REG_DSI_LFR_CON, enable);
        DISP_REG_SET_FIELD(cmdq, DSI_LFR_CON_FLD_DSI_LFR_UPDATE,
                            DSI_REG_BASE[i] + DISP_REG_DSI_LFR_CON, 1);
        DISP_REG_SET_FIELD(cmdq, DSI_LFR_CON_FLD_DSI_LFR_VSE_DIS,
                            DSI_REG_BASE[i] + DISP_REG_DSI_LFR_CON, 0);
        DISP_REG_SET_FIELD(cmdq, DSI_LFR_CON_FLD_DSI_LFR_SKIP_NUM,
                            DSI_REG_BASE[i] + DISP_REG_DSI_LFR_CON, skip_num);
    }
}

void DSI_LFR_UPDATE(enum DISP_MODULE_ENUM module, void *cmdq)
{
    unsigned int i = 0;

    for (i = DSI_MODULE_BEGIN(module); i <= DSI_MODULE_END(module); i++) {
        //DSI_OUTREGBIT(cmdq, struct DSI_LFR_CON_REG,
        //                    DSI_REG[i]->DSI_LFR_CON,LFR_UPDATE,0);
        //DSI_OUTREGBIT(cmdq, struct DSI_LFR_CON_REG,
        //                    DSI_REG[i]->DSI_LFR_CON,LFR_UPDATE, 1);
        DISP_REG_SET_FIELD(cmdq, DSI_LFR_CON_FLD_DSI_LFR_UPDATE,
                            DSI_REG_BASE[i] + DISP_REG_DSI_LFR_CON, 0);
        DISP_REG_SET_FIELD(cmdq, DSI_LFR_CON_FLD_DSI_LFR_UPDATE,
                            DSI_REG_BASE[i] + DISP_REG_DSI_LFR_CON, 1);
    }
}

void DSI_Set_VM_CMD(enum DISP_MODULE_ENUM module, void *cmdq)
{

    if (module == DISP_MODULE_DSI0 || module == DISP_MODULE_DSIDUAL) {
        //DSI_OUTREGBIT(cmdq, struct DSI_VM_CMD_CON_REG,DSI_REG[0]->DSI_VM_CMD_CON,
        //                    TS_VFP_EN, 1);
        //DSI_OUTREGBIT(cmdq, struct DSI_VM_CMD_CON_REG,DSI_REG[0]->DSI_VM_CMD_CON,
        //                    VM_CMD_EN, 1);
        DISP_REG_SET_FIELD(cmdq, DSI_VM_CMD_CON_FLD_TS_VFP_EN,
                            DSI_REG_BASE[0] + DISP_REG_DSI_VM_CMD_CON, 1);
        DISP_REG_SET_FIELD(cmdq, DSI_VM_CMD_CON_FLD_VM_CMD_EN,
                            DSI_REG_BASE[0] + DISP_REG_DSI_VM_CMD_CON, 1);
    }

    return;
}

int ddp_dsi_config(enum DISP_MODULE_ENUM module, struct disp_ddp_path_config *config,
                    void *cmdq_handle)
{
    int i = 0;
    static int cnt;

    DISPFUNC();

    if (!config->dst_dirty)
        return 0;

    if (config->dsi_config.output_mode == MTK_PANEL_DUAL_PORT) {
        if (module == DISP_MODULE_DSI0)
            module = DISP_MODULE_DSIDUAL;
        else if (module == DISP_MODULE_DSI1)
            return 0;
    }

    for (i = DSI_MODULE_BEGIN(module); i <= DSI_MODULE_END(module); i++) {
        _copy_dsi_params(&(config->dsi_config), &(_dsi_context[i].dsi_params));
        _dump_dsi_params(&(_dsi_context[i].dsi_params));
    }

    DSI_PHY_clk_setting(module, NULL, &(config->dsi_config));

    /* the first power up, don't need to WakeUp !! */
    if (cnt++)
        DSI_exit_ULPS(module);

    DSI_TXRX_Control(module, NULL, &(config->dsi_config));
    if (config->dsi_config.output_mode == MTK_PANEL_DUAL_PORT)
        DSI_PS_Control(module, NULL, &(config->dsi_config), config->dst_w * 2, config->dst_h);
    else
        DSI_PS_Control(module, NULL, &(config->dsi_config), config->dst_w, config->dst_h);
    DSI_PHY_TIMCONFIG(module, NULL, &(config->dsi_config));

    if (config->dsi_config.mode != CMD_MODE || ((config->dsi_config.switch_mode_enable == 1)
        && (config->dsi_config.switch_mode != CMD_MODE))) {
        DSI_Config_VDO_Timing(module, NULL, &(config->dsi_config));
        DSI_Set_VM_CMD(module, cmdq_handle);
        if (config->dsi_config.mode != CMD_MODE && config->dsi_config.lfr_enable) {
            unsigned int  mode = config->dsi_config.lfr_mode;
            unsigned int  type = config->dsi_config.lfr_type;
            unsigned int  skip_num = config->dsi_config.lfr_skip_num;
            unsigned int  enable = config->dsi_config.lfr_enable;
             DISPMSG("lfr en %d mode= %d type=%d,skip_num %d\n", enable, mode, type, skip_num);
            DSI_Set_LFR(module, cmdq_handle, mode, type, enable, skip_num);
        }
    }

    // Enable clk low power per Line ;
    if (config->dsi_config.clk_lp_per_line_enable)
        DSI_PHY_CLK_LP_PerLine_config(module, NULL, &(config->dsi_config));

    DSI_BackupRegisters(module, cmdq_handle);
    return 0;
}

//int ddp_dsi_stop(enum DISP_MODULE_ENUM module, struct disp_path_config_struct_ex *config,
//                    void *cmdq_handle)
int ddp_dsi_stop(enum DISP_MODULE_ENUM module, void *cmdq_handle)
{
    //ths caller should call wait_event_or_idle for frame stop event then.
    if (_dsi_is_video_mode(module))
        DSI_SetMode(module, cmdq_handle, CMD_MODE);

    return 0;
}

int ddp_dsi_reset(enum DISP_MODULE_ENUM module, void *cmdq_handle)
{
    DSI_Reset(module, cmdq_handle);

    return 0;
}

int ddp_dsi_power_on(enum DISP_MODULE_ENUM module, void *cmdq_handle)
{
    int i = 0;
    int ret = 0;
    static int first_in = 1;

    if (!s_isDsiPowerOn) {
        if (module == DISP_MODULE_DSI0 || module == DISP_MODULE_DSI1) {
            ddp_enable_module_clock(module);

            if (ret > 0)
                DISP_LOG_PRINT(ANDROID_LOG_WARN, "DSI", "DSI0 power manager API return false\n");
        }

        s_isDsiPowerOn = true;
    }

    for (i = DSI_MODULE_BEGIN(module); i <= DSI_MODULE_END(module); i++) {
        if (_dsi_context[i].dsi_params.mode == CMD_MODE) {
            DSI_PHY_clk_switch(module, NULL, true);

            // restore dsi register
            DSI_RestoreRegisters(module, NULL);

            // enable sleep-out mode, skip first in
            if (!first_in)
                DSI_SleepOut(module, NULL);
            else
                first_in = 0;

            // enter wakeup
            DSI_Wakeup(module, NULL);

            DSI_Reset(module, NULL);
        } else {
            // initialize clock setting
            DSI_PHY_clk_switch(module, NULL, true);

            // restore dsi register
            DSI_RestoreRegisters(module, NULL);

            // enable sleep-out mode, skip first in
            if (!first_in)
                DSI_SleepOut(module, NULL);
            else
                first_in = 0;

            // enter wakeup
            DSI_Wakeup(module, NULL);
            DSI_clk_HS_mode(module, NULL, false);

            DSI_Reset(module, NULL);
        }
    }

    return DSI_STATUS_OK;
}


int ddp_dsi_power_off(enum DISP_MODULE_ENUM module, void *cmdq_handle)
{
    int i = 0;
    int ret = 0;

    if (!s_isDsiPowerOn) {
        if (module == DISP_MODULE_DSI0 || module == DISP_MODULE_DSI1) {
            ddp_disable_module_clock(module);

            if (ret > 0)
                DISP_LOG_PRINT(ANDROID_LOG_WARN, "DSI0", "DSI0 power manager API return false\n");
        }
        s_isDsiPowerOn = true;
    }

    for (i = DSI_MODULE_BEGIN(module); i <= DSI_MODULE_END(module); i++) {
        if (_dsi_context[i].dsi_params.mode == CMD_MODE) {
            // no need this, we will make dsi is in idle when ddp_dsi_stop() returns
            //DSI_CHECK_RET(DSI_WaitForNotBusy(module, NULL));
            DSI_CHECK_RET(DSI_BackupRegisters(module, NULL));

            // disable HS mode
            DSI_clk_HS_mode(module, NULL, false);
            // enter ULPS mode
            DSI_lane0_ULP_mode(module, NULL, 1);
            DSI_clk_ULP_mode(module, NULL, 1);
            // disable mipi pll
            DSI_PHY_clk_switch(module, NULL, false);
        } else {
            // backup dsi register
            // no need this, we will make dsi is in idle when ddp_dsi_stop() returns
            //DSI_CHECK_RET(DSI_WaitForNotBusy());
            DSI_BackupRegisters(module, NULL);

            // disable HS mode
            DSI_clk_HS_mode(module, NULL, false);
            // enter ULPS mode
            DSI_lane0_ULP_mode(module, NULL, 1);
            DSI_clk_ULP_mode(module, NULL, 1);

            // disable mipi pll
            DSI_PHY_clk_switch(module, NULL, false);
        }
    }

    return DSI_STATUS_OK;
}

int ddp_dsi_is_busy(enum DISP_MODULE_ENUM module)
{
    int i = 0;
    int busy = 0;
    struct DSI_INT_STATUS_REG status;

    DISPFUNC();

    for (i = DSI_MODULE_BEGIN(module); i <= DSI_MODULE_END(module); i++) {
        status = DSI_REG[i]->DSI_INTSTA;

        if (status.BUSY)
            busy++;
    }

    return busy;
}

int ddp_dsi_is_idle(enum DISP_MODULE_ENUM module)
{
    return !ddp_dsi_is_busy(module);
}

int ddp_dsi_dump(enum DISP_MODULE_ENUM module, int level)
{
    DSI_DumpRegisters(module, level);
    return 0;
}

int ddp_dsi_start(enum DISP_MODULE_ENUM module, void *cmdq)
{
    int i = 0;

    if (_dsi_context[0].dsi_params.output_mode == MTK_PANEL_DUAL_PORT) {
        if (module == DISP_MODULE_DSI0)
            module = DISP_MODULE_DSIDUAL;
        else if (module == DISP_MODULE_DSI1)
            return 0;
    }

#ifdef LK_BYPASS_SHADOW_REG
    for (i = DSI_MODULE_BEGIN(module); i <= DSI_MODULE_END(module); i++) {
        //DSI_OUTREGBIT(cmdq, struct DSI_SHADOW_DEBUG_REG,
        //                    DSI_REG[i]->DSI_SHADOW_DEBUG, BYPASS_SHADOW, 1);
        //DSI_OUTREGBIT(cmdq, struct DSI_SHADOW_DEBUG_REG,
        //                    DSI_REG[i]->DSI_SHADOW_DEBUG, READ_WORKING, 1);
        DISP_REG_SET_FIELD(cmdq, DSI_SHADOW_DEBUG_FLD_BYPASS_SHADOW,
                            DSI_REG_BASE[i] + DISP_REG_DSI_SHADOW_DEBUG, 1);
        DISP_REG_SET_FIELD(cmdq, DSI_SHADOW_DEBUG_FLD_READ_WORKING,
                            DSI_REG_BASE[i] + DISP_REG_DSI_SHADOW_DEBUG, 1);
    }
#endif

    if (module == DISP_MODULE_DSIDUAL) {
        //DSI_OUTREGBIT(cmdq, struct DSI_START_REG,DSI_REG[0]->DSI_START,DSI_START,0);
        //DSI_OUTREGBIT(cmdq, struct DSI_START_REG,DSI_REG[1]->DSI_START,DSI_START,0);
        DISP_REG_SET_FIELD(cmdq, DSI_START_FLD_DSI_START, DSI_REG_BASE[0] + DISP_REG_DSI_START, 0);
        DISP_REG_SET_FIELD(cmdq, DSI_START_FLD_DSI_START, DSI_REG_BASE[1] + DISP_REG_DSI_START, 0);

        if (_dsi_context[0].dsi_params.mode != CMD_MODE) {
            //DSI_OUTREGBIT(cmdq, struct DSI_COM_CTRL_REG,
            //                DSI_REG[0]->DSI_COM_CTRL, DSI_DUAL_EN, 1);
            //DSI_OUTREGBIT(cmdq, struct DSI_COM_CTRL_REG,
            //                DSI_REG[1]->DSI_COM_CTRL, DSI_DUAL_EN, 1);
            //DISP_REG_SET_FIELD(cmdq, DSI_COM_CON_FLD_DSI_DUAL_EN,
            //                DSI_REG_BASE[0] + DISP_REG_DSI_COM_CON, 1);
            DISP_REG_SET_FIELD(cmdq, DSI_COM_CON_FLD_DSI_DUAL_EN,
                                DSI_REG_BASE[1] + DISP_REG_DSI_COM_CON, 1);
        }

        DSI_SetMode(module, cmdq, _dsi_context[0].dsi_params.mode);
        DSI_clk_HS_mode(module, cmdq, true);
    } else if (module == DISP_MODULE_DSI0 || module == DISP_MODULE_DSI1) {
        DISPFUNC();
        i = DSI_MODULE_BEGIN(module);
        DSI_SetMode(module, cmdq, _dsi_context[i].dsi_params.mode);
        DSI_clk_HS_mode(module, cmdq, true);
    }
    return 0;
}

int ddp_dsi_trigger(enum DISP_MODULE_ENUM module, void *cmdq)
{
    int i = 0;
    unsigned int data_array[1];

#if 0
    //dsi pattern
    DSI_BIST_Pattern_Test(module, NULL, 1, 0x00ffff00);
    DISPMSG("make it hang after dsi pattern\n");
    while (1)
        ;
#endif
    i = DSI_MODULE_BEGIN(module);
    if (_dsi_context[i].dsi_params.mode == CMD_MODE) {
    /*test*/
        // DSI_OUTREG32(cmdq, &DSI_REG[0]->DSI_BIST_PATTERN, 0x00ffff00);
        // DSI_OUTREG32(cmdq, &DSI_REG[0]->DSI_BIST_CON, 0x00000040);

        data_array[0] = 0x002c3909;
        DSI_set_cmdq(module, cmdq, data_array, 1, 0);

        if (module == DISP_MODULE_DSIDUAL) {
            /*
             * DSI1 is only used for triggering video data; thus pull up DSI_DUAL_EN,
             * and pull down DSI_DUAL_EN after triggering video data is done.
             */
            //DSI_OUTREGBIT(cmdq, struct DSI_START_REG, DSI_REG[0]->DSI_START,
            //                    DSI_START, 0);
            //DSI_OUTREGBIT(cmdq, struct DSI_START_REG, DSI_REG[1]->DSI_START,
            //                    DSI_START, 0);
            //DSI_OUTREGBIT(cmdq, struct DSI_COM_CTRL_REG, DSI_REG[0]->DSI_COM_CTRL,
            //                    DSI_DUAL_EN, 1);
            //DSI_OUTREGBIT(cmdq, struct DSI_COM_CTRL_REG, DSI_REG[1]->DSI_COM_CTRL,
            //                    DSI_DUAL_EN, 1);
            DISP_REG_SET_FIELD(cmdq, DSI_START_FLD_DSI_START,
                                DSI_REG_BASE[0] + DISP_REG_DSI_START, 0);
            DISP_REG_SET_FIELD(cmdq, DSI_START_FLD_DSI_START,
                                DSI_REG_BASE[1] + DISP_REG_DSI_START, 0);
            //DISP_REG_SET_FIELD(cmdq, DSI_COM_CON_FLD_DSI_DUAL_EN,
            //                  DSI_REG_BASE[0] + DISP_REG_DSI_COM_CON, 1);
            DISP_REG_SET_FIELD(cmdq, DSI_COM_CON_FLD_DSI_DUAL_EN,
                                DSI_REG_BASE[1] + DISP_REG_DSI_COM_CON, 1);

        }
    }

    DSI_Start(module, cmdq);

    if (module == DISP_MODULE_DSIDUAL && _dsi_context[i].dsi_params.mode == CMD_MODE) {
        /* Reading one reg is only used for delay in order to pull down DSI_DUAL_EN. */
        readl(DISP_DSI0_BASE + 0xc);

        //DSI_OUTREGBIT(cmdq, struct DSI_COM_CTRL_REG, DSI_REG[0]->DSI_COM_CTRL,
        //                    DSI_DUAL_EN, 0);
        //DSI_OUTREGBIT(cmdq, struct DSI_COM_CTRL_REG, DSI_REG[1]->DSI_COM_CTRL,
        //                    DSI_DUAL_EN, 0);
        //DISP_REG_SET_FIELD(cmdq, DSI_COM_CON_FLD_DSI_DUAL_EN,
        //                    DSI_REG_BASE[0] + DISP_REG_DSI_COM_CON, 0);
        DISP_REG_SET_FIELD(cmdq, DSI_COM_CON_FLD_DSI_DUAL_EN,
                            DSI_REG_BASE[1] + DISP_REG_DSI_COM_CON, 0);
    }

    return 0;
}

static void lcm_set_reset_pin(u32 value, void *fdt)
{

#ifndef GPIO_SUPPORT
    writel(value, MMSYS_CONFIG_BASE+0x180);
    DISPMSG("reg set lcm rst pin\n");
#else
#ifdef MTK_LCM_COMMON_DRIVER_SUPPORT
    int ret = 0;

    ret = mtk_lcm_drv_power_reset(MTK_LCM_FUNC_DSI, value);
    if (ret != 0) {
        DDPMSG("%s, gate ic reset err, %d\n",
            __func__, ret);
        return;
    }
#else
    int rst_pin;

    rst_pin = lcm_util_get_pin("lcm_rst_out0_gpio", fdt);
    if (rst_pin < 0) {
    #ifdef MT6873_GPIO_HARD_CODE
        rst_pin = GPIO42;
    #else
        return;
    #endif
    }

    DISPMSG("gpio set lcm rst pin\n");
    //rst_pin |= PIN_ENCRYPT;
    mt_set_gpio_mode(rst_pin, GPIO_MODE_00);
    mt_set_gpio_dir(rst_pin, GPIO_DIR_OUT);
    if (value)
        mt_set_gpio_out(rst_pin, GPIO_OUT_ONE);
    else
        mt_set_gpio_out(rst_pin, GPIO_OUT_ZERO);
#endif
#endif
}

static void lcm_udelay(u32 us)
{
    spin(us);
}

static void lcm_mdelay(u32 ms)
{
    spin(ms * 1000);
}

void DSI_set_cmdq_V2_Wrapper_DSI0(unsigned int cmd, unsigned char count,
                                    unsigned char *para_list, unsigned char force_update)
{
    DSI_set_cmdq_V2(DISP_MODULE_DSI0, NULL, cmd, count, para_list, force_update);
}

void DSI_set_cmdq_V2_Wrapper_DSI1(unsigned int cmd, unsigned char count,
                                    unsigned char *para_list, unsigned char force_update)
{
    DSI_set_cmdq_V2(DISP_MODULE_DSI1, NULL, cmd, count, para_list, force_update);
}

void DSI_set_cmdq_V11_wrapper_DSI0(void *cmdq, unsigned int *pdata,
                                    unsigned int queue_size, unsigned char force_update)
{
    DSI_set_cmdq(DISP_MODULE_DSI0, cmdq, pdata, queue_size, force_update);
}

void DSI_set_cmdq_V11_wrapper_DSI1(void *cmdq, unsigned int *pdata,
                                    unsigned int queue_size, unsigned char force_update)
{
    DSI_set_cmdq(DISP_MODULE_DSI1, cmdq, pdata, queue_size, force_update);
}

void DSI_set_cmdq_V2_Wrapper_DSIDual(unsigned int cmd, unsigned char count,
                                       unsigned char *para_list, unsigned char force_update)
{
    DSI_set_cmdq_V2(DISP_MODULE_DSIDUAL, NULL, cmd, count, para_list, force_update);
}

void DSI_set_cmdq_V2_DSI0(void *cmdq, unsigned int cmd, unsigned char count,
                            unsigned char *para_list, unsigned char force_update)
{
    DSI_set_cmdq_V2(DISP_MODULE_DSI0, cmdq, cmd, count, para_list, force_update);
}

void DSI_set_cmdq_V2_DSI1(void *cmdq, unsigned int cmd, unsigned char count,
                            unsigned char *para_list, unsigned char force_update)
{
    DSI_set_cmdq_V2(DISP_MODULE_DSI1, cmdq, cmd, count, para_list, force_update);
}

void DSI_set_cmdq_V2_DSIDual(void *cmdq, unsigned int cmd, unsigned char count,
                             unsigned char *para_list, unsigned char force_update)
{
    DSI_set_cmdq_V2(DISP_MODULE_DSIDUAL, cmdq, cmd, count, para_list, force_update);
}

void DSI_set_cmdq_V4_DSI0(void *cmdq, struct dsi_cmd_desc *cmds)
{
    DSI_set_cmdq_V4(DISP_MODULE_DSI0, cmdq, cmds);
}

void DSI_set_cmdq_V4_DSI1(void *cmdq, struct dsi_cmd_desc *cmds)
{
    DSI_set_cmdq_V4(DISP_MODULE_DSI1, cmdq, cmds);
}

void DSI_set_cmdq_V4_DSIDual(void *cmdq, struct dsi_cmd_desc *cmds)
{
    DSI_set_cmdq_V4(DISP_MODULE_DSIDUAL, cmdq, cmds);
}

void DSI_set_cmdq_V3_Wrapper_DSI0(LCM_setting_table_V3 *para_tbl, unsigned int size,
                                    unsigned char force_update)
{
    DSI_set_cmdq_V3(DISP_MODULE_DSI0, NULL, para_tbl, size, force_update);
}

void DSI_set_cmdq_V3_Wrapper_DSI1(LCM_setting_table_V3 *para_tbl, unsigned int size,
                                    unsigned char force_update)
{
    DSI_set_cmdq_V3(DISP_MODULE_DSI1, NULL, para_tbl, size, force_update);
}

void DSI_set_cmdq_V3_Wrapper_DSIDual(LCM_setting_table_V3 *para_tbl, unsigned int size,
                                        unsigned char force_update)
{
    DSI_set_cmdq_V3(DISP_MODULE_DSIDUAL, NULL, para_tbl, size, force_update);
}

void DSI_set_cmdq_wrapper_DSI0(unsigned int *pdata, unsigned int queue_size,
                                unsigned char force_update)
{
    DSI_set_cmdq(DISP_MODULE_DSI0, NULL, pdata, queue_size, force_update);
}

void DSI_set_cmdq_wrapper_DSI1(unsigned int *pdata, unsigned int queue_size,
                                unsigned char force_update)
{
    DSI_set_cmdq(DISP_MODULE_DSI1, NULL, pdata, queue_size, force_update);
}

void DSI_set_cmdq_wrapper_DSIDual(unsigned int *pdata, unsigned int queue_size,
                                    unsigned char force_update)
{
    DSI_set_cmdq(DISP_MODULE_DSIDUAL, NULL, pdata, queue_size, force_update);
}

unsigned int DSI_dcs_read_lcm_reg_v2_wrapper_DSI0(u8 cmd, u8 *buffer, u8 buffer_size)
{
    return DSI_dcs_read_lcm_reg_v2(DISP_MODULE_DSI0, NULL, cmd, buffer, buffer_size);
}

unsigned int DSI_dcs_read_lcm_reg_v2_wrapper_DSI1(u8 cmd, u8 *buffer, u8 buffer_size)
{
    return DSI_dcs_read_lcm_reg_v2(DISP_MODULE_DSI1, NULL, cmd, buffer, buffer_size);
}

unsigned int DSI_dcs_read_lcm_reg_v2_wrapper_DSIDUAL(u8 cmd, u8 *buffer, u8 buffer_size)
{
    return DSI_dcs_read_lcm_reg_v2(DISP_MODULE_DSIDUAL, NULL, cmd, buffer, buffer_size);
}

unsigned int DSI_dcs_read_lcm_reg_v3_wrapper_DSI0(char *out,
        struct dsi_cmd_desc *cmds, unsigned int len)
{
    return DSI_dcs_read_lcm_reg_v3(DISP_MODULE_DSI0, NULL,
            out, cmds, len);
}

unsigned int DSI_dcs_read_lcm_reg_v3_wrapper_DSI1(char *out,
        struct dsi_cmd_desc *cmds, unsigned int len)
{
    return DSI_dcs_read_lcm_reg_v3(DISP_MODULE_DSI1, NULL,
            out, cmds, len);
}

unsigned int DSI_dcs_read_lcm_reg_v3_wrapper_DSIDUAL(char *out,
        struct dsi_cmd_desc *cmds, unsigned int len)
{
    return DSI_dcs_read_lcm_reg_v3(DISP_MODULE_DSIDUAL, NULL,
            out, cmds, len);
}

long lcd_enp_bias_setting(unsigned int value, void *fdt)
{
    long ret = 0;

#ifdef MACH_FPGA
    DDPMSG("In FPGA stage, no need to control gate power ic by gpio\n");
#else

#ifndef GPIO_SUPPORT
#else
#ifdef MTK_LCM_COMMON_DRIVER_SUPPORT
    if (value != 0)
        ret = mtk_lcm_drv_power_on(MTK_LCM_FUNC_DSI);
    else
        ret = mtk_lcm_drv_power_off(MTK_LCM_FUNC_DSI);

    if (ret != 0) {
        DDPMSG("%s, gate ic power %s failed, %ld\n",
            __func__, (value ? "on" : "off"), ret);

        return ret;
    }
#else
    int lcd_enp_pin, lcd_enn_pin;

    lcd_enp_pin = lcm_util_get_pin("lcd_bias_enp0_gpio", fdt);
    lcd_enn_pin = lcm_util_get_pin("lcd_bias_enn0_gpio", fdt);
    if (lcd_enp_pin < 0 || lcd_enn_pin < 0) {
    #ifdef MT6873_GPIO_HARD_CODE
        lcd_enp_pin = GPIO28;
        lcd_enn_pin = GPIO29;
    #else
        return -1;
    #endif

    }

#if 1
    /* Wait dts ok */
    //lcd_enp_pin |= PIN_ENCRYPT;
    //lcd_enn_pin |= PIN_ENCRYPT;
    if (value) { /* power on gate power ic */
        mt_set_gpio_mode(lcd_enp_pin, GPIO_MODE_00);
        mt_set_gpio_dir(lcd_enp_pin, GPIO_DIR_OUT);
        mt_set_gpio_out(lcd_enp_pin, GPIO_OUT_ONE);

        lcm_mdelay(2);

        mt_set_gpio_mode(lcd_enn_pin, GPIO_MODE_00);
        mt_set_gpio_dir(lcd_enn_pin, GPIO_DIR_OUT);
        mt_set_gpio_out(lcd_enn_pin, GPIO_OUT_ONE);
    } else { /* power off gate power ic */
        mt_set_gpio_mode(lcd_enn_pin, GPIO_MODE_00);
        mt_set_gpio_dir(lcd_enn_pin, GPIO_DIR_OUT);
        mt_set_gpio_out(lcd_enn_pin, GPIO_OUT_ZERO);

        lcm_mdelay(1);

        mt_set_gpio_mode(lcd_enp_pin, GPIO_MODE_00);
        mt_set_gpio_dir(lcd_enp_pin, GPIO_DIR_OUT);
        mt_set_gpio_out(lcd_enp_pin, GPIO_OUT_ZERO);
    }
#endif
#endif
#endif
#endif

    return ret;
}

int ddp_dsi_set_lcm_utils(enum DISP_MODULE_ENUM module, LCM_DRIVER *lcm_drv, void *fdt)
{
    LCM_UTIL_FUNCS *utils = NULL;

    if (lcm_drv == NULL) {
        DISPERR("lcm_drv is null\n");
        return -1;
    }

    if (module == DISP_MODULE_DSI0) {
        utils = (LCM_UTIL_FUNCS *)&lcm_utils_dsi0;
    } else if (module == DISP_MODULE_DSI1) {
        utils = (LCM_UTIL_FUNCS *)&lcm_utils_dsi1;
    } else if (module == DISP_MODULE_DSIDUAL) {
        utils = (LCM_UTIL_FUNCS *)&lcm_utils_dsidual;
    } else {
        DISPERR("wrong module: %d\n", module);
        return -1;
    }

    utils->set_reset_pin    = lcm_set_reset_pin;
    utils->udelay           = lcm_udelay;
    utils->mdelay           = lcm_mdelay;

    if (module == DISP_MODULE_DSI0) {
        LCM_PARAMS lcm_param;

        lcm_drv->get_params(&lcm_param);
        if (lcm_param.dsi.output_mode == MTK_PANEL_DUAL_PORT) {
            if (lcm_param.lcm_cmd_if == LCM_INTERFACE_DSI0) {
                utils->dsi_set_cmdq = DSI_set_cmdq_wrapper_DSI0;
                utils->dsi_set_cmdq_V2 = DSI_set_cmdq_V2_Wrapper_DSI0;
                utils->dsi_set_cmdq_V3 = DSI_set_cmdq_V3_Wrapper_DSI0;
                utils->dsi_dcs_read_lcm_reg_v2 = DSI_dcs_read_lcm_reg_v2_wrapper_DSI0;
                utils->mipi_dsi_cmds_tx = DSI_set_cmdq_V4_DSI0;
                utils->mipi_dsi_cmds_rx = DSI_dcs_read_lcm_reg_v3_wrapper_DSI0;
            } else if (lcm_param.lcm_cmd_if == LCM_INTERFACE_DSI1) {
                utils->dsi_set_cmdq = DSI_set_cmdq_wrapper_DSI1;
                utils->dsi_set_cmdq_V2 = DSI_set_cmdq_V2_Wrapper_DSI1;
                utils->dsi_set_cmdq_V3 = DSI_set_cmdq_V3_Wrapper_DSI1;
                utils->dsi_dcs_read_lcm_reg_v2 = DSI_dcs_read_lcm_reg_v2_wrapper_DSI1;
                utils->mipi_dsi_cmds_tx = DSI_set_cmdq_V4_DSI1;
                utils->mipi_dsi_cmds_rx = DSI_dcs_read_lcm_reg_v3_wrapper_DSI1;
            } else {
                utils->dsi_set_cmdq = DSI_set_cmdq_wrapper_DSIDual;
                utils->dsi_set_cmdq_V2 = DSI_set_cmdq_V2_Wrapper_DSIDual;
                utils->dsi_set_cmdq_V3 = DSI_set_cmdq_V3_Wrapper_DSIDual;
                utils->dsi_dcs_read_lcm_reg_v2 = DSI_dcs_read_lcm_reg_v2_wrapper_DSI0;
                utils->mipi_dsi_cmds_tx = DSI_set_cmdq_V4_DSIDual;
                utils->mipi_dsi_cmds_rx = DSI_dcs_read_lcm_reg_v3_wrapper_DSI0;
            }
        } else {
            utils->dsi_set_cmdq = DSI_set_cmdq_wrapper_DSI0;
            utils->dsi_set_cmdq_V2 = DSI_set_cmdq_V2_Wrapper_DSI0;
            utils->dsi_set_cmdq_V3 = DSI_set_cmdq_V3_Wrapper_DSI0;
            utils->dsi_dcs_read_lcm_reg_v2 = DSI_dcs_read_lcm_reg_v2_wrapper_DSI0;
            utils->dsi_set_cmdq_V22 = DSI_set_cmdq_V2_DSI0;
            utils->dsi_set_cmdq_V11 = DSI_set_cmdq_V11_wrapper_DSI0;
            utils->mipi_dsi_cmds_tx = DSI_set_cmdq_V4_DSI0;
            utils->mipi_dsi_cmds_rx = DSI_dcs_read_lcm_reg_v3_wrapper_DSI0;
        }
    } else if (module == DISP_MODULE_DSI1) {
        utils->dsi_set_cmdq = DSI_set_cmdq_wrapper_DSI1;
        utils->dsi_set_cmdq_V2 = DSI_set_cmdq_V2_Wrapper_DSI1;
        utils->dsi_set_cmdq_V3 = DSI_set_cmdq_V3_Wrapper_DSI1;
        utils->dsi_dcs_read_lcm_reg_v2 = DSI_dcs_read_lcm_reg_v2_wrapper_DSI1;
        utils->dsi_set_cmdq_V22 = DSI_set_cmdq_V2_DSI1;
        utils->dsi_set_cmdq_V11 = DSI_set_cmdq_V11_wrapper_DSI1;
        utils->mipi_dsi_cmds_tx = DSI_set_cmdq_V4_DSI1;
        utils->mipi_dsi_cmds_rx = DSI_dcs_read_lcm_reg_v3_wrapper_DSI1;
    } else if (module == DISP_MODULE_DSIDUAL) {
        // TODO: Ugly workaround, hope we can found better resolution
        LCM_PARAMS lcm_param;

        lcm_drv->get_params(&lcm_param);

        if (lcm_param.lcm_cmd_if == LCM_INTERFACE_DSI0) {
            utils->dsi_set_cmdq = DSI_set_cmdq_wrapper_DSI0;
            utils->dsi_set_cmdq_V2 = DSI_set_cmdq_V2_Wrapper_DSI0;
            utils->dsi_set_cmdq_V3 = DSI_set_cmdq_V3_Wrapper_DSI0;
            utils->dsi_dcs_read_lcm_reg_v2 = DSI_dcs_read_lcm_reg_v2_wrapper_DSI0;
            utils->mipi_dsi_cmds_tx = DSI_set_cmdq_V4_DSI0;
            utils->mipi_dsi_cmds_rx = DSI_dcs_read_lcm_reg_v3_wrapper_DSI0;
        } else if (lcm_param.lcm_cmd_if == LCM_INTERFACE_DSI1) {
            utils->dsi_set_cmdq = DSI_set_cmdq_wrapper_DSI1;
            utils->dsi_set_cmdq_V2 = DSI_set_cmdq_V2_Wrapper_DSI1;
            utils->dsi_set_cmdq_V3 = DSI_set_cmdq_V3_Wrapper_DSI1;
            utils->dsi_dcs_read_lcm_reg_v2  = DSI_dcs_read_lcm_reg_v2_wrapper_DSI1;
            utils->mipi_dsi_cmds_tx = DSI_set_cmdq_V4_DSI1;
            utils->mipi_dsi_cmds_rx = DSI_dcs_read_lcm_reg_v3_wrapper_DSI1;
        } else {
            utils->dsi_set_cmdq = DSI_set_cmdq_wrapper_DSIDual;
            utils->dsi_set_cmdq_V2 = DSI_set_cmdq_V2_Wrapper_DSIDual;
            utils->dsi_set_cmdq_V3 = DSI_set_cmdq_V3_Wrapper_DSIDual;
            utils->dsi_dcs_read_lcm_reg_v2 = DSI_dcs_read_lcm_reg_v2_wrapper_DSIDUAL;
            utils->mipi_dsi_cmds_tx = DSI_set_cmdq_V4_DSIDual;
            utils->mipi_dsi_cmds_rx = DSI_dcs_read_lcm_reg_v3_wrapper_DSIDUAL;
        }
    }

#ifndef MACH_FPGA
#if 0
    utils->set_gpio_out = mt_set_gpio_out;
    utils->set_gpio_mode = mt_set_gpio_mode;
    utils->set_gpio_dir = mt_set_gpio_dir;
    utils->set_gpio_pull_enable = (int (*)(unsigned int, unsigned char))mt_set_gpio_pull_enable;
#endif
#endif

    utils->set_gpio_lcd_enp_bias = lcd_enp_bias_setting;

    lcm_drv->set_util_funcs(utils);

    return 0;
}

static int ddp_dsi_polling_irq(enum DISP_MODULE_ENUM module, int bit, int timeout)
{
    //int i = 0;
    unsigned int cnt  = 0;
    addr_t irq_reg_base = 0;
    unsigned int reg_val = 0;

    if (module == DISP_MODULE_DSI0 || module == DISP_MODULE_DSIDUAL)
        irq_reg_base = (addr_t)(&DSI_REG[0]->DSI_INTSTA);

    //DISPCHECK("dsi polling irq, module=%d, bit=0x%08x, timeout=%d, irq_regbase=0x%08x\n",
    //            module, bit, timeout, irq_reg_base);

    if (timeout <= 0) {
        while ((DISP_REG_GET(irq_reg_base) & bit) == 0)
            ;
        cnt = 1;
    } else {
        // time need to update
        cnt = timeout*1000/100;
        while (cnt > 0) {
            cnt--;
            reg_val = DISP_REG_GET(irq_reg_base);
            //DISPMSG("reg_val=0x%08x\n", reg_val);
            if (reg_val & bit) {
                DSI_OUTREG32(NULL, irq_reg_base, ~reg_val);
                break;
            }
            spin(100);
        }
    }

    DISPMSG("DSI polling interrupt ret =%d\n", cnt);

    if (cnt == 0)
        DSI_DumpRegisters(module, 2);

    return cnt;
}

struct DDP_MODULE_DRIVER ddp_driver_dsi0 = {
    .module         = DISP_MODULE_DSI0,
    .init           = ddp_dsi_init,
    .deinit         = ddp_dsi_deinit,
    .config         = ddp_dsi_config,
    .trigger        = ddp_dsi_trigger,
    .start          = ddp_dsi_start,
    .stop           = ddp_dsi_stop,
    .reset          = ddp_dsi_reset,
    .power_on       = ddp_dsi_power_on,
    .power_off      = ddp_dsi_power_off,
    .is_idle        = ddp_dsi_is_idle,
    .is_busy        = ddp_dsi_is_busy,
    .dump_info      = ddp_dsi_dump,
    .set_lcm_utils  = ddp_dsi_set_lcm_utils,
    .polling_irq    = ddp_dsi_polling_irq
};

struct DDP_MODULE_DRIVER ddp_driver_dsi1 = {
    .module         = DISP_MODULE_DSI1,
    .init           = ddp_dsi_init,
    .deinit         = ddp_dsi_deinit,
    .config         = ddp_dsi_config,
    .trigger        = ddp_dsi_trigger,
    .start          = ddp_dsi_start,
    .stop           = ddp_dsi_stop,
    .reset          = ddp_dsi_reset,
    .power_on       = ddp_dsi_power_on,
    .power_off      = ddp_dsi_power_off,
    .is_idle        = ddp_dsi_is_idle,
    .is_busy        = ddp_dsi_is_busy,
    .dump_info      = ddp_dsi_dump,
    .set_lcm_utils  = ddp_dsi_set_lcm_utils,
    .polling_irq    = ddp_dsi_polling_irq
};

struct DDP_MODULE_DRIVER ddp_driver_dsidual = {
    .module         = DISP_MODULE_DSIDUAL,
    .init           = ddp_dsi_init,
    .deinit         = ddp_dsi_deinit,
    .config         = ddp_dsi_config,
    .trigger        = ddp_dsi_trigger,
    .start          = ddp_dsi_start,
    .stop           = ddp_dsi_stop,
    .reset          = ddp_dsi_reset,
    .power_on       = ddp_dsi_power_on,
    .power_off      = ddp_dsi_power_off,
    .is_idle        = ddp_dsi_is_idle,
    .is_busy        = ddp_dsi_is_busy,
    .dump_info      = ddp_dsi_dump,
    .set_lcm_utils  = ddp_dsi_set_lcm_utils,
    .polling_irq    = ddp_dsi_polling_irq
};
