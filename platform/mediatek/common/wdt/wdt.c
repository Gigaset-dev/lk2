/*
 * Copyright (c) 2019 MediaTek Inc.
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files
 * (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge,
 * publish, distribute, sublicense, and/or sell copies of the Software,
 * and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */
#include <compiler.h>
#include <libfdt.h>
#include <mt_gic.h>
#include <platform.h>
#include <platform/addressmap.h>
#include <platform/interrupts.h>
#include <platform/irq.h>
#include <platform/log_store_lk.h>
#include <platform/mboot_params.h>
#include <platform/reg.h>
#include <platform/wdt.h>
#include <set_fdt.h>
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <debug.h>

#define WDT_DTS_NAME           "/watchdog"
#define WDT_DTS_NAME_SOC       "/soc/watchdog"
#define WDT_DT_NODE_PROP_NAME  "apwdt_en"

struct mtk_wdt_regs {
    uint32_t wdt_mode;
    uint32_t wdt_length;
    uint32_t wdt_restart;
    uint32_t wdt_status;
    uint32_t wdt_interval;
    uint32_t wdt_swrst;
    uint32_t wdt_swsysrst;
    uint32_t wdt_swsysrst_pulse;
    uint32_t wdt_nonrst;
    uint32_t wdt_nonrst2;
    uint32_t reserved1[2];
    uint32_t wdt_req_mode;
    uint32_t wdt_req_irq;
    uint32_t wdt_ext_req_con;
    uint32_t reserved2[1];
    uint32_t wdt_debug_ctl;
    uint32_t wdt_latch_ctl;
    uint32_t wdt_latch_ctl2;
    uint32_t reserved3[15];
    uint32_t wdt_sysdbg_deg_en1;
    uint32_t wdt_sysdbg_deg_en2;
    uint32_t reserved4[5];
    uint32_t wdt_pwr_latch;
};

STATIC_ASSERT(__offsetof(struct mtk_wdt_regs, wdt_swrst) == 0x14);
STATIC_ASSERT(__offsetof(struct mtk_wdt_regs, wdt_latch_ctl2) == 0x48);
STATIC_ASSERT(__offsetof(struct mtk_wdt_regs, wdt_sysdbg_deg_en1) == 0x88);
STATIC_ASSERT(__offsetof(struct mtk_wdt_regs, wdt_pwr_latch) == 0xa4);

static struct mtk_wdt_regs *const mtk_wdt = (void *)RGU_BASE;
static unsigned int g_rgu_status;

enum {
    MTK_WDT_MODE_KEY        = 0x22000000,
    MTK_WDT_MODE_DUAL_MODE  = 1 << 6,
    MTK_WDT_MODE_IRQ        = 1 << 3,
    MTK_WDT_MODE_EXTEN      = 1 << 2,
    MTK_WDT_MODE_EXT_POL    = 1 << 1,
    MTK_WDT_MODE_ENABLE     = 1 << 0
};

enum {
    MTK_WDT_STATUS_SPM_THERMAL_RST = 1 << 0,
    MTK_WDT_STATUS_SPMWDT_RST      = 1 << 1,
    MTK_WDT_STATUS_EINT_RST        = 1 << 2,
    MTK_WDT_STATUS_SYSRST_RST      = 1 << 3,
    MTK_WDT_STATUS_DVFSP_RST       = 1 << 4,
    MTK_WDT_STATUS_MCUPM_RST       = 1 << 5,
    MTK_WDT_STATUS_SSPM_RST        = 1 << 16,
    MTK_WDT_STATUS_MDDBG_RST       = 1 << 17,
    MTK_WDT_STATUS_THERMAL_RST     = 1 << 18,
    MTK_WDT_STATUS_DEBUGWDT_RST    = 1 << 19,
    MTK_WDT_STATUS_SECURITY_RST    = 1 << 28,
    MTK_WDT_STATUS_IRQWDT_RST      = 1 << 29,
    MTK_WDT_STATUS_SWWDT_RST       = 1 << 30,
    MTK_WDT_STATUS_HWWDT_RST       = 1 << 31
};

enum {
    RE_BOOT_REASON_UNKNOWN          = 0,
    RE_BOOT_BY_WDT_HW              = 1 << 0,
    RE_BOOT_BY_WDT_SW              = 1 << 1,
    RE_BOOT_WITH_INTTERUPT         = 1 << 2,
    RE_BOOT_BY_SPM_THERMAL         = 1 << 3,
    RE_BOOT_BY_SPM                 = 1 << 4,
    RE_BOOT_BY_THERMAL_DIRECT      = 1 << 5,
    RE_BOOT_BY_DEBUG               = 1 << 6,
    RE_BOOT_BY_SECURITY            = 1 << 7,
    RE_BOOT_BY_EINT                = 1 << 8,
    RE_BOOT_BY_SYSRST              = 1 << 9,
    RE_BOOT_BY_SSPM_RST            = 1 << 10,
    RE_BOOT_BY_PMIC_FULL_RST       = 1 << 11,
    RE_BOOT_BY_MCUPM_RST           = 1 << 12,
    RE_BOOT_ABNORMAL               = 0xF0
};

enum {
    MTK_WDT_SWRST_KEY       = 0x1209,
};

enum {
    MTK_WDT_RESTART_KEY     = 0x1971,
};

enum {
    MTK_WDT_PWR_LATCH_KEY    = 0x66000000,
    MTK_WDT_PWR_LATCH_READY  = 1 << 0
};

enum {
    MTK_WDT_REQ_MODE_KEY       = 0x33000000,
    MTK_WDT_REQ_MODE_DEFAULT   = 0x3F00F2,
    MTK_WDT_REQ_MODE_EINT_EN   = 1 << 2,
    MTK_WDT_REQ_MODE_SYSRST_EN = 1 << 3
};

enum {
    MTK_WDT_REQ_IRQ_KEY        = 0x44000000,
    MTK_WDT_REQ_IRQ_DEFAULT    = 0x3F00F2,
    MTK_WDT_REQ_IRQ_EINT_EN    = 1 << 2,
    MTK_WDT_REQ_IRQ_SYSRST_EN  = 1 << 3
};

enum {
    MTK_WDT_EXT_REQ_CONF_EN    = 1 << 0,
    MTK_WDT_EXT_REQ_CONF_SHIFT = 4,
    MTK_WDT_EXT_REQ_CONF_MASK  = 0xF
};

enum {
    MTK_LATCH_CTL_KEY          = 0x95000000,
    MTK_RG_LATH_EN             = 1 << 0,
    MTK_RG_MCU_LATCH_SELECT    = 1 << 1,
    MTK_RG_SPM_LATCH_SELECT    = 1 << 2,
    MTK_RG_MCU_LATH_EN         = 1 << 4,
    MTK_RG_SPM_LATH_EN         = 1 << 5,
    MTK_RG_DRAMC_LATH_EN       = 1 << 6,
    MTK_RG_MPO_EXT_OFF_EN      = 1 << 8,
    MTK_RG_GPU_EXT_OFF_EN      = 1 << 9,
    MTK_RG_MD_EXT_OFF_EN       = 1 << 10,
    MTK_RG_DRAMC_RD_TEST_EN    = 1 << 11,
    MTK_RG_DRAMC_RDWT_TEST_EN  = 1 << 12,
    MTK_RG_DVFSRC_LATCH_EN     = 1 << 13,/* obsolete */
    MTK_RG_EMI_LATCH_EN        = 1 << 14,
    MTK_RG_DEBUGSYS_LATCH_EN   = 1 << 17,
};

enum {
    MTK_WDT_LATCH_CTL2_KEY     = 0x95000000,
    MTK_WDT_LATCH_CTL2_DFD_EN  = 1 << 17,
    MTK_WDT_DFD_TIMEOUT_MASK   = 0x1FFFF,
    MTK_WDT_LATCH_CTL2_KEY2    = 0x90000000,
    MTK_WDT_DFD_TIMEOUT_MASK2  = 0xFFFFFFF,
};

enum {
    MTK_WDT_SYSDBG_DEG_EN1_KEY = 0x1B2A,
    MTK_WDT_SYSDBG_DEG_EN2_KEY = 0x4F59
};

void mtk_wdt_set_restart_reason(uint8_t mode)
{
    uint32_t tmp;

    tmp = read32(&mtk_wdt->wdt_nonrst2);
    tmp &= ~MTK_WDT_NONRST2_BOOT_MASK;
    tmp |= mode;
    write32(&mtk_wdt->wdt_nonrst2, tmp);
}

uint8_t mtk_wdt_get_restart_reason(void)
{
    static uint8_t rgu_restart_reason = MTK_WDT_NONRST2_BOOT_MASK;

    if (rgu_restart_reason == MTK_WDT_NONRST2_BOOT_MASK) {
        uint32_t tmp;

        tmp = read32(&mtk_wdt->wdt_nonrst2);
        rgu_restart_reason = (uint8_t)(tmp & MTK_WDT_NONRST2_BOOT_MASK);
        tmp &= ~MTK_WDT_NONRST2_BOOT_MASK;
        write32(&mtk_wdt->wdt_nonrst2, tmp);
    }
    return rgu_restart_reason;
}

void mtk_wdt_set_aee_rsv(uint8_t value)
{
    uint32_t tmp;

    tmp = read32(&mtk_wdt->wdt_nonrst2);
    tmp &= ~MTK_WDT_NONRST2_AEE_MASK;
    tmp |= ((uint32_t)value) << MTK_WDT_NONRST2_AEE_RSV_SHIFT;
    write32(&mtk_wdt->wdt_nonrst2, tmp);
}

uint8_t mtk_wdt_get_aee_rsv(void)
{
    uint32_t aee_rsv = 0;

    aee_rsv = read32(&mtk_wdt->wdt_nonrst2);
    aee_rsv &= MTK_WDT_NONRST2_AEE_MASK;
    return (uint8_t)(aee_rsv >> MTK_WDT_NONRST2_AEE_RSV_SHIFT);
}

int mtk_wdt_request_en_set(int mark_bit, int en)
{
    unsigned int tmp;

    if ((mark_bit != MTK_WDT_STATUS_SSPM_RST) &&
        (mark_bit != MTK_WDT_STATUS_SYSRST_RST) &&
        (mark_bit != MTK_WDT_STATUS_EINT_RST) &&
        (mark_bit != MTK_WDT_STATUS_SPM_THERMAL_RST))
        return -1;

    tmp = read32(&mtk_wdt->wdt_req_mode);
    tmp |= MTK_WDT_REQ_MODE_KEY;

    if (en == REQ_EN)
        tmp |= (mark_bit);
    if (en == REQ_DIS)
        tmp &= ~(mark_bit);

    write32(&mtk_wdt->wdt_req_mode, tmp);

    return 0;
}

int mtk_wdt_request_mode_set(int mark_bit, int mode)
{
    unsigned int tmp;

    if ((mark_bit != MTK_WDT_STATUS_SSPM_RST) &&
        (mark_bit != MTK_WDT_STATUS_SYSRST_RST) &&
        (mark_bit != MTK_WDT_STATUS_EINT_RST) &&
        (mark_bit != MTK_WDT_STATUS_SPM_THERMAL_RST))
        return -1;

    tmp = read32(&mtk_wdt->wdt_req_irq);
    tmp |= MTK_WDT_REQ_IRQ_KEY;

    if (mode == REQ_IRQ_MODE)
        tmp |= (mark_bit);
    if (mode == REQ_RST_MODE)
        tmp &= ~(mark_bit);

    write32(&mtk_wdt->wdt_req_irq, tmp);

    return 0;
}

bool mtk_is_rst_from_da(void)
{
    unsigned int reg;
    unsigned int last_stage;

    reg = read32(&mtk_wdt->wdt_nonrst2);

    last_stage = (reg >> MTK_WDT_NONRST2_LAST_STAGE_OFS) & RGU_STAGE_MASK;

    if (last_stage == RGU_STAGE_DA)
        return true;

    return false;
}

static void dump_rgu_rgs(void)
{
    /* Dump RGU regisers */
    dprintf(ALWAYS, "MODE:          0x%x\n", read32(&mtk_wdt->wdt_mode));
    dprintf(ALWAYS, "STA:           0x%x\n", read32(&mtk_wdt->wdt_status));
    dprintf(ALWAYS, "LENGTH:        0x%x\n", read32(&mtk_wdt->wdt_length));
    dprintf(ALWAYS, "INTERVAL:      0x%x\n", read32(&mtk_wdt->wdt_interval));
    dprintf(ALWAYS, "SWSYSRST:      0x%x\n", read32(&mtk_wdt->wdt_swsysrst));
    dprintf(ALWAYS, "LATCH_CTL:     0x%x\n", read32(&mtk_wdt->wdt_latch_ctl));
    dprintf(ALWAYS, "NONRST_REG:    0x%x\n", read32(&mtk_wdt->wdt_nonrst));
    dprintf(ALWAYS, "NONRST_REG2:   0x%x\n", read32(&mtk_wdt->wdt_nonrst2));
    dprintf(ALWAYS, "DEBUG_CTL:     0x%x\n", read32(&mtk_wdt->wdt_debug_ctl));
    dprintf(ALWAYS, "REQ_MODE:      0x%x\n", read32(&mtk_wdt->wdt_req_mode));
    dprintf(ALWAYS, "REQ_IRQ_EN:    0x%x\n", read32(&mtk_wdt->wdt_req_irq));
}

static void mtk_wdt_check_last_stage(void)
{
    unsigned int reg;
    unsigned int last_stage;

    /* check reboot source */
    reg = read32(&mtk_wdt->wdt_nonrst2);

    dprintf(ALWAYS, "\n rst from: ");

    last_stage = (reg >> MTK_WDT_NONRST2_STAGE_OFS) & RGU_STAGE_MASK;

    if (last_stage == RGU_STAGE_PRELOADER)
        dprintf(ALWAYS, "pl\n");
    else if (last_stage == RGU_STAGE_LK)
        dprintf(ALWAYS, "lk\n");
    else if (last_stage == RGU_STAGE_KERNEL)
        dprintf(ALWAYS, "kernel\n");
    else if (last_stage == RGU_STAGE_DA)
        dprintf(ALWAYS, "DA\n");
    else if (last_stage == RGU_STAGE_BL2_EXT)
        dprintf(ALWAYS, "BL2EXT\n");
    else if (last_stage == RGU_STAGE_AEE)
        dprintf(ALWAYS, "AEE\n");
    else if (last_stage == RGU_STAGE_TFA)
        dprintf(ALWAYS, "TFA\n");
    else
        dprintf(ALWAYS, "?\n");

    reg = (reg & ~(RGU_STAGE_MASK << MTK_WDT_NONRST2_LAST_STAGE_OFS)) |
        (last_stage << MTK_WDT_NONRST2_LAST_STAGE_OFS);

    write32(&mtk_wdt->wdt_nonrst2, reg);
}

const char *mtk_wdt_get_last_stage(void)
{
    uint32_t reg;
    uint32_t last_stage;

    reg = read32(&mtk_wdt->wdt_nonrst2);

    last_stage = (reg >> MTK_WDT_NONRST2_LAST_STAGE_OFS) & RGU_STAGE_MASK;

    if (last_stage == RGU_STAGE_PRELOADER)
        return "rst from: pl";
    else if (last_stage == RGU_STAGE_LK)
        return "rst from: lk";
    else if (last_stage == RGU_STAGE_KERNEL)
        return "rst from: kernel";
    else if (last_stage == RGU_STAGE_DA)
        return "rst from: DA";
    else if (last_stage == RGU_STAGE_BL2_EXT)
        return "rst from: BL2EXT";
    else if (last_stage == RGU_STAGE_AEE)
        return "rst from: AEE";
    else if (last_stage == RGU_STAGE_TFA)
        return "rst from: TFA";

    return "rst from: unknown";
}

void mtk_wdt_mark_stage(uint32_t stage)
{
    uint32_t reg = read32(&mtk_wdt->wdt_nonrst2);

    reg = (reg & ~(RGU_STAGE_MASK << MTK_WDT_NONRST2_STAGE_OFS))
            | (stage << MTK_WDT_NONRST2_STAGE_OFS);

    write32(&mtk_wdt->wdt_nonrst2, reg);
}

void mtk_wdt_config(bool enable)
{
    const uint32_t config = MTK_WDT_MODE_ENABLE |
                            MTK_WDT_MODE_EXTEN |
                            MTK_WDT_MODE_IRQ |
                            MTK_WDT_MODE_DUAL_MODE;

    clrsetbits32(&mtk_wdt->wdt_mode,
                 MTK_WDT_MODE_EXT_POL | config,
                 MTK_WDT_MODE_KEY | ((enable) ? config : 0));

    if (enable)
        mtk_wdt_restart_timer();
}

void mtk_wdt_set_timeout_value(uint32_t seconds)
{
    /* sec * 32768 / 512 = sec * 64 = sec * 1 << 6 */
    uint32_t timeout = (seconds * (1 << 6));

    /*
     * TimeOut = BitField 15:5
     * Key     = BitField  4:0 = 0x08
     */
    write32(&mtk_wdt->wdt_length, ((timeout << 5) | 0x8));
}

void mtk_wdt_restart_timer(void)
{
    write32(&mtk_wdt->wdt_restart, MTK_WDT_RESTART_KEY);
}

void mtk_wdt_raise_irq(void)
{
    mtk_wdt_restart_timer();
    write32(&mtk_wdt->wdt_swrst, MTK_WDT_SWRST_KEY);
}

static bool mtk_wdt_is_v2_dfd(void)
{
    write32(&mtk_wdt->wdt_latch_ctl2, MTK_WDT_LATCH_CTL2_KEY);
    if (read32(&mtk_wdt->wdt_latch_ctl2))
        return true;

    return false;
}

void mtk_wdt_reset(platform_halt_reason reason)
{
    uint32_t reg = read32(&mtk_wdt->wdt_mode);

    if (reason == HALT_REASON_SW_RESET) {
        /*
         * Disable DFD for normal reboot
         */
        if (mtk_wdt_is_v2_dfd())
            mtk_wdt_dfd_timeout(0);
        else
            mtk_wdt_dfd_timeout_en(0);
    }

    reg = (reg & ~(MTK_WDT_MODE_DUAL_MODE | MTK_WDT_MODE_IRQ | MTK_WDT_MODE_ENABLE)) |
           MTK_WDT_MODE_KEY;

    write32(&mtk_wdt->wdt_mode, reg);

    mtk_wdt_request_en_set(MTK_WDT_STATUS_EINT_RST | MTK_WDT_STATUS_SYSRST_RST, REQ_DIS);

    write32(&mtk_wdt->wdt_swrst, MTK_WDT_SWRST_KEY);
}

void mtk_wdt_pwr_latch(void)
{
    write32(&mtk_wdt->wdt_pwr_latch,
            MTK_WDT_PWR_LATCH_KEY | MTK_WDT_PWR_LATCH_READY);
}

void mtk_wdt_config_eint(enum req_en en, enum req_mode mode, int eint_num)
{
    uint32_t reg = read32(&mtk_wdt->wdt_req_mode);

    if (en == REQ_EN) {
        uint32_t eint = (eint_num & MTK_WDT_EXT_REQ_CONF_MASK) << MTK_WDT_EXT_REQ_CONF_SHIFT;

        write32(&mtk_wdt->wdt_ext_req_con, eint | MTK_WDT_EXT_REQ_CONF_EN);
        reg |= MTK_WDT_REQ_MODE_EINT_EN;
    } else {
        write32(&mtk_wdt->wdt_ext_req_con, 0);
        reg &= ~MTK_WDT_REQ_MODE_EINT_EN;
    }
    write32(&mtk_wdt->wdt_req_mode, reg | MTK_WDT_REQ_MODE_KEY);

    reg = read32(&mtk_wdt->wdt_req_irq);
    if (mode == REQ_IRQ_MODE)
        reg |= MTK_WDT_REQ_IRQ_EINT_EN;
    else
        reg &= ~MTK_WDT_REQ_IRQ_EINT_EN;
    write32(&mtk_wdt->wdt_req_irq, reg | MTK_WDT_REQ_IRQ_KEY);
}

void mtk_wdt_config_sysrst(enum req_en en, enum req_mode mode)
{
    uint32_t reg = read32(&mtk_wdt->wdt_req_mode);

    if (en == REQ_EN) {
        write32(&mtk_wdt->wdt_sysdbg_deg_en1, MTK_WDT_SYSDBG_DEG_EN1_KEY);
        write32(&mtk_wdt->wdt_sysdbg_deg_en2, MTK_WDT_SYSDBG_DEG_EN2_KEY);
        reg |= MTK_WDT_REQ_MODE_SYSRST_EN;
    } else {
        write32(&mtk_wdt->wdt_sysdbg_deg_en1, 0);
        write32(&mtk_wdt->wdt_sysdbg_deg_en2, 0);
        reg &= ~MTK_WDT_REQ_MODE_SYSRST_EN;
    }
    write32(&mtk_wdt->wdt_req_mode, reg | MTK_WDT_REQ_MODE_KEY);

    reg = read32(&mtk_wdt->wdt_req_irq);
    if (mode == REQ_IRQ_MODE)
        reg |= MTK_WDT_REQ_IRQ_SYSRST_EN;
    else
        reg &= ~MTK_WDT_REQ_IRQ_SYSRST_EN;
    write32(&mtk_wdt->wdt_req_irq, reg | MTK_WDT_REQ_IRQ_KEY);
}

void mtk_wdt_dfd_timeout(uint32_t tmo)
{
    uint32_t latch_ctrl2 = read32(&mtk_wdt->wdt_latch_ctl2);

    if (mtk_wdt_is_v2_dfd()) {
        dprintf(ALWAYS, "RGU DFD V2\n");

        tmo = tmo & MTK_WDT_DFD_TIMEOUT_MASK2;
        latch_ctrl2 &= (~MTK_WDT_DFD_TIMEOUT_MASK2);
        latch_ctrl2 |= (tmo | MTK_WDT_LATCH_CTL2_KEY2);
    } else {
        tmo = tmo & MTK_WDT_DFD_TIMEOUT_MASK;
        latch_ctrl2 &= (~MTK_WDT_DFD_TIMEOUT_MASK);
        latch_ctrl2 |= (tmo | MTK_WDT_LATCH_CTL2_KEY);
    }

    write32(&mtk_wdt->wdt_latch_ctl2, latch_ctrl2);
}

void mtk_wdt_dfd_timeout_en(enum req_en en)
{
    if (mtk_wdt_is_v2_dfd())
        return;
    else {
        uint32_t latch_ctrl2 = read32(&mtk_wdt->wdt_latch_ctl2);

        if (en)
            latch_ctrl2 |= (MTK_WDT_LATCH_CTL2_DFD_EN | MTK_WDT_LATCH_CTL2_KEY);
        else {
            latch_ctrl2 &= (~MTK_WDT_LATCH_CTL2_DFD_EN);
            latch_ctrl2 |= MTK_WDT_LATCH_CTL2_KEY;
        }

        write32(&mtk_wdt->wdt_latch_ctl2, latch_ctrl2);
    }
}

static unsigned int mtk_wdt_get_status(void)
{
    static unsigned int wdt_sta;
    static unsigned int wdt_sta_handled;

    if (wdt_sta_handled == 0) {
        wdt_sta = read32(&mtk_wdt->wdt_status);
        wdt_sta |= read32(&mtk_wdt->wdt_nonrst);
        wdt_sta_handled = 1;
    }

    return wdt_sta;
}

static void g_rgu_status_parsing(void)
{
    unsigned int wdt_sta = 0, nonrst2 = 0;

    wdt_sta = mtk_wdt_get_status();
    nonrst2 = read32(&mtk_wdt->wdt_nonrst2);

    if ((wdt_sta & MTK_WDT_STATUS_HWWDT_RST) &&
        !(wdt_sta & MTK_WDT_STATUS_SWWDT_RST)) {
        /* Time out reboot always by pass power key */
        g_rgu_status = RE_BOOT_BY_WDT_HW;
    } else if (wdt_sta & MTK_WDT_STATUS_SWWDT_RST) {
        g_rgu_status = RE_BOOT_BY_WDT_SW;
    } else {
        g_rgu_status = RE_BOOT_REASON_UNKNOWN;
    }

    if (wdt_sta & MTK_WDT_STATUS_IRQWDT_RST)
        g_rgu_status |= RE_BOOT_WITH_INTTERUPT;

    if (wdt_sta & MTK_WDT_STATUS_SPM_THERMAL_RST)
        g_rgu_status |= RE_BOOT_BY_SPM_THERMAL;

    if (wdt_sta & MTK_WDT_STATUS_SPMWDT_RST)
        g_rgu_status |= RE_BOOT_BY_SPM;

    if (wdt_sta & MTK_WDT_STATUS_THERMAL_RST)
        g_rgu_status |= RE_BOOT_BY_THERMAL_DIRECT;

    if (wdt_sta & MTK_WDT_STATUS_DEBUGWDT_RST)
        g_rgu_status |= RE_BOOT_BY_DEBUG;

    if (wdt_sta & MTK_WDT_STATUS_SECURITY_RST)
        g_rgu_status |= RE_BOOT_BY_SECURITY;

    if (wdt_sta & MTK_WDT_STATUS_SYSRST_RST)
        g_rgu_status |= RE_BOOT_BY_SYSRST;

    if (wdt_sta & MTK_WDT_STATUS_SSPM_RST)
        g_rgu_status |= RE_BOOT_BY_SSPM_RST;

    if (wdt_sta & MTK_WDT_STATUS_MCUPM_RST)
        g_rgu_status |= RE_BOOT_BY_MCUPM_RST;

    /* SW workaround to avoid EINT_RST be triggered with other RST behavior */
    if ((wdt_sta & MTK_WDT_STATUS_EINT_RST) && (g_rgu_status == RE_BOOT_REASON_UNKNOWN))
        g_rgu_status |= RE_BOOT_BY_EINT;
    else if ((read32(&mtk_wdt->wdt_req_irq) & MTK_WDT_REQ_IRQ_EINT_EN) &&
        (g_rgu_status == RE_BOOT_WITH_INTTERUPT) &&
        (wdt_sta & MTK_WDT_STATUS_EINT_RST))
        g_rgu_status |= RE_BOOT_BY_EINT;

    dprintf(ALWAYS, "parse g_rgu_status: %d (0x%x)\n", g_rgu_status, g_rgu_status);
}

unsigned int mtk_wdt_get_rgu_status(void)
{
    return g_rgu_status;
}

void mtk_wdt_pre_init(void)
{
    unsigned int nonrst, wdt_ctrl;

    /* get last stage and mark current stage to preloader */
    mtk_wdt_check_last_stage();

    dump_rgu_rgs();

    g_rgu_status_parsing();

    write32(&mtk_wdt->wdt_nonrst2, read32(&mtk_wdt->wdt_nonrst2) & ~
       (MTK_WDT_NONRST2_SSPM_RAISEV | MTK_WDT_NONRST2_VPROC_BEFORE |
        MTK_WDT_NONRST2_VPROC_AFTER | MTK_WDT_NONRST2_FLUSH_AFTER  |
        MTK_WDT_NONRST2_RST_LAST));

    nonrst = read32(&mtk_wdt->wdt_nonrst);
    if (nonrst & ~(MTK_WDT_STATUS_SWWDT_RST))
        nonrst = nonrst | mtk_wdt_get_status();
    else
        nonrst = mtk_wdt_get_status();
    write32(&mtk_wdt->wdt_nonrst, nonrst & ~(MTK_WDT_STATUS_SWWDT_RST));
    dprintf(INFO, "Set NONRST_REG to 0x%x\n", nonrst);

    wdt_ctrl = read32(&mtk_wdt->wdt_latch_ctl);
    wdt_ctrl |= MTK_LATCH_CTL_KEY;
    wdt_ctrl |= MTK_RG_LATH_EN;
    wdt_ctrl |= MTK_RG_MCU_LATH_EN | MTK_RG_SPM_LATH_EN;
    wdt_ctrl |= MTK_RG_GPU_EXT_OFF_EN | MTK_RG_MD_EXT_OFF_EN;
    wdt_ctrl &= ~(MTK_RG_MCU_LATCH_SELECT | MTK_RG_SPM_LATCH_SELECT);
    wdt_ctrl |= MTK_RG_DRAMC_LATH_EN;
    wdt_ctrl |= MTK_RG_DEBUGSYS_LATCH_EN;
    wdt_ctrl |= (MTK_RG_DRAMC_RD_TEST_EN | MTK_RG_DRAMC_RDWT_TEST_EN);
    write32(&mtk_wdt->wdt_latch_ctl, wdt_ctrl);

    dprintf(INFO, "%s: MTK_WDT_REQ_MODE(%x), MTK_WDT_REQ_IRQ_EN(%x)\n", __func__,
        read32(&mtk_wdt->wdt_req_mode), read32(&mtk_wdt->wdt_req_irq));
}

static void mtk_wdt_doe_setup(void *fdt)
{
    int apwdt_en = 1;
    int len, offset = 0;
    unsigned int *data = NULL;

    offset = fdt_path_offset(fdt, WDT_DTS_NAME);
    if (offset < 0) {
        dprintf(ALWAYS, "Failed to find %s in dtb\n", WDT_DTS_NAME);
        offset = fdt_path_offset(fdt, WDT_DTS_NAME_SOC);
        if (offset < 0) {
            dprintf(ALWAYS, "Failed to find %s in dtb\n", WDT_DTS_NAME_SOC);
            return;
        }
    }

    data = (unsigned int *)fdt_getprop(fdt, offset, WDT_DT_NODE_PROP_NAME, &len);
    if (len <= 0 || !data) {
        dprintf(ALWAYS, "Fail to found property %s\n", WDT_DT_NODE_PROP_NAME);
    } else
        apwdt_en = fdt32_to_cpu(*(unsigned int *)data);


    if (apwdt_en == 0) {
        /* disable wdt timeout */
        mtk_wdt_config(false);

        /* disable all reset source's reset and irq mode */
        write32(&mtk_wdt->wdt_req_mode, MTK_WDT_REQ_MODE_KEY);
        write32(&mtk_wdt->wdt_req_irq, MTK_WDT_REQ_IRQ_KEY);
        dprintf(ALWAYS, "apwdt is disabled by doe\n");
    }
}
SET_FDT_INIT_HOOK(mtk_wdt_doe_setup, mtk_wdt_doe_setup);

static enum handler_return mtk_wdt_isr(void *arg)
{
    dprintf(CRITICAL, "mtk wdt isr\n");
    /* dump rgu status */
    dump_rgu_rgs();

#if LK_AS_AEE
    mtk_wdt_set_aee_rsv(AEE_EXP_TYPE_AEE_LK_CRASH);
#endif

    /* flush log_store buffer cache */
    logstore_flush_header_cache();

    /* do RGU reset */
    mtk_wdt_reset(HALT_REASON_SW_WATCHDOG);
    dprintf(ALWAYS, "WDT_isr: spinning forever...\n");
    for (;;)
        ;
    return INT_NO_RESCHEDULE;
}

void mtk_wdt_isr_init(void)
{
    mt_irq_set_sens(WDT_IRQ_ID, GIC_LEVEL_SENSITIVE);
    mt_irq_set_polarity(WDT_IRQ_ID, GIC_LOW_POLARITY);
    register_int_handler(WDT_IRQ_ID, mtk_wdt_isr, NULL);
    unmask_interrupt(WDT_IRQ_ID);
}
