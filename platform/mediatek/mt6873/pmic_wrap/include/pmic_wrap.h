/*
 * Copyright (c) 2020 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#pragma once

#include <platform/addressmap.h>
#include <pmic_wrap_common.h>

struct mtk_pwrap_regs {
    u32 reserved1[800];
    u32 swinf_acc;
    u32 swinf_wdata;
    u32 reserved2[3];
    u32 swinf_rdata;
    u32 reserved3[3];
    u32 swinf_vld_clr;
    u32 swinf_sta;
    u32 reserved4[4];
};

static struct mtk_pwrap_regs * const mtk_pwrap = (void *)PWRAP_BASE;

/* timeout setting */
enum {
      TIMEOUT_READ_US        = 255,
      TIMEOUT_WAIT_IDLE_US   = 255
};

enum {
      RDATA_WACS_RDATA_SHIFT = 0,
      RDATA_WACS_FSM_SHIFT   = 16,
      RDATA_WACS_REQ_SHIFT   = 19,
      RDATA_SYNC_IDLE_SHIFT  = 20,
      RDATA_INIT_DONE_SHIFT  = 22,
      RDATA_SYS_IDLE_SHIFT   = 23,
};
