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
#pragma once

#include <platform.h>
#include <stdbool.h>
#include <stdint.h>

#define WDT_MAX_TIMEOUT    30

enum {
    RGU_STAGE_MASK      = 0x7,
    RGU_STAGE_PRELOADER = 0x1,
    RGU_STAGE_LK        = 0x2,
    RGU_STAGE_KERNEL    = 0x3,
    RGU_STAGE_DA        = 0x4,
    RGU_STAGE_BL2_EXT   = 0x5,
    RGU_STAGE_AEE       = 0x6,
    RGU_STAGE_TFA       = 0x7
};

enum {
    MTK_WDT_NONRST2_BOOT_MASK       = 0xF,
    MTK_WDT_NONRST2_AEE_MASK        = 0xFF0,
    MTK_WDT_NONRST2_BOOT_CHARGER    = 1,
    MTK_WDT_NONRST2_BOOT_RECOVERY   = 2,
    MTK_WDT_NONRST2_BOOT_BOOTLOADER = 3,
    MTK_WDT_NONRST2_BOOT_DM_VERITY  = 4,
    MTK_WDT_NONRST2_BOOT_KPOC       = 5,
    MTK_WDT_NONRST2_BOOT_DDR_RSVD   = 6,
    MTK_WDT_NONRST2_BOOT_META       = 7,
    MTK_WDT_NONRST2_BOOT_RPMBPK     = 8,
    MTK_WDT_NONRST2_AEE_RSV_SHIFT   = 4,
    MTK_WDT_NONRST2_BYPASS_PWR_KEY  = 1 << 13,
    MTK_WDT_NONRST2_SSPM_RAISEV     = 1 << 16,
    MTK_WDT_NONRST2_VPROC_BEFORE    = 1 << 17,
    MTK_WDT_NONRST2_VPROC_AFTER     = 1 << 18,
    MTK_WDT_NONRST2_FLUSH_AFTER     = 1 << 19,
    MTK_WDT_NONRST2_RST_LAST        = 1 << 20,
    MTK_WDT_NONRST2_STAGE_OFS       = 29,
    MTK_WDT_NONRST2_LAST_STAGE_OFS  = 26
};

enum req_en {
    REQ_DIS,
    REQ_EN
};

enum req_mode {
    REQ_IRQ_MODE,
    REQ_RST_MODE
};

void mtk_wdt_config(bool enable);
void mtk_wdt_set_timeout_value(uint32_t seconds);

void mtk_wdt_raise_irq(void);
bool mtk_is_rst_from_da(void);
void mtk_wdt_restart_timer(void);
void mtk_wdt_reset(platform_halt_reason reason);
void mtk_wdt_set_restart_reason(uint8_t mode);
uint8_t mtk_wdt_get_restart_reason(void);
void mtk_wdt_set_aee_rsv(uint8_t value);
uint8_t mtk_wdt_get_aee_rsv(void);
const char *mtk_wdt_get_last_stage(void);
void mtk_wdt_mark_stage(uint32_t stage);
void mtk_wdt_pwr_latch(void);
void mtk_wdt_config_eint(enum req_en en, enum req_mode mode, int eint_num);
void mtk_wdt_config_sysrst(enum req_en en, enum req_mode mode);
void mtk_wdt_dfd_timeout(uint32_t tmo);
void mtk_wdt_dfd_timeout_en(enum req_en en);
unsigned int mtk_wdt_get_rgu_status(void);
void mtk_wdt_pre_init(void);
void mtk_wdt_isr_init(void);
