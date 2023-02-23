/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#pragma once

#include "DISP_MUTEX_c_header.h"

#define DISP_OVL_SEPARATE_MUTEX_ID             (DISP_MUTEX_DDP_LAST+1)

#define DISP_REG_CONFIG_MUTEX_INTEN            DISP_MUTEX_INTEN
#define DISP_REG_CONFIG_MUTEX_INTSTA           DISP_MUTEX_INTSTA
#define DISP_REG_CONFIG_MUTEX_CFG              DISP_MUTEX_CFG
    #define CFG_FLD_HW_CG                       CFG_FLD_MUTEX_CFG

#define DISP_REG_CONFIG_MUTEX_REG_UPD_TIMEOUT  DISP_REG_UPD_TIMEOUT

#define DISP_REG_CONFIG_MUTEX0_EN              DISP_MUTEX0_EN
    #define EN_FLD_MUTEX0_EN                    DISP_MUTEX0_EN_FLD_MUTEX_EN

#define DISP_REG_CONFIG_MUTEX0_RST             DISP_MUTEX0_RST
    #define RST_FLD_MUTEX0_RST                  DISP_MUTEX0_RST_FLD_MUTEX_RST

#define DISP_REG_CONFIG_MUTEX0_SOF             DISP_MUTEX0_CTL
    #define SOF_FLD_MUTEX0_SOF                  DISP_MUTEX0_CTL_FLD_MUTEX_SOF
    #define SOF_FLD_MUTEX0_SOF_TIMING           DISP_MUTEX0_CTL_FLD_MUTEX_SOF_TIMING
    #define SOF_FLD_MUTEX0_SOF_WAIT             DISP_MUTEX0_CTL_FLD_MUTEX_SOF_WAIT
    #define SOF_FLD_MUTEX0_EOF                  DISP_MUTEX0_CTL_FLD_MUTEX_EOF
    #define SOF_FLD_MUTEX0_EOF_TIMING           DISP_MUTEX0_CTL_FLD_MUTEX_EOF_TIMING
    #define SOF_FLD_MUTEX0_EOF_WAIT             DISP_MUTEX0_CTL_FLD_MUTEX_EOF_WAIT

#define SOF_VAL_MUTEX0_SOF_SINGLE_MODE         (0)
#define SOF_VAL_MUTEX0_SOF_FROM_DSI0           (1)
#define SOF_VAL_MUTEX0_SOF_FROM_DSI1           (2)
#define SOF_VAL_MUTEX0_SOF_FROM_DPI            (3)
#define SOF_VAL_MUTEX0_EOF_SINGLE_MODE         (0)
#define SOF_VAL_MUTEX0_EOF_FROM_DSI0           (1)
#define SOF_VAL_MUTEX0_EOF_FROM_DSI1           (2)
#define SOF_VAL_MUTEX0_EOF_FROM_DPI            (3)

#define DISP_REG_CONFIG_MUTEX0_MOD0            DISP_MUTEX0_MOD0
#define DISP_REG_CONFIG_MUTEX0_MOD1            DISP_MUTEX0_MOD1

#define DISP_REG_CONFIG_DEBUG_OUT_SEL          DISP_MUTEX_DEBUG_OUT_SEL

#define DISP_REG_CONFIG_MUTEX_EN(n)            (DISP_REG_CONFIG_MUTEX0_EN + (0x20 * (n)))
#define DISP_REG_CONFIG_MUTEX_RST(n)           (DISP_REG_CONFIG_MUTEX0_RST + (0x20 * (n)))
#define DISP_REG_CONFIG_MUTEX_MOD0(n)          (DISP_REG_CONFIG_MUTEX0_MOD0 + (0x20 * (n)))
#define DISP_REG_CONFIG_MUTEX_MOD1(n)          (DISP_REG_CONFIG_MUTEX0_MOD1 + (0x20 * (n)))
#define DISP_REG_CONFIG_MUTEX_SOF(n)           (DISP_REG_CONFIG_MUTEX0_SOF + (0x20 * (n)))
