/*
 * Copyright (c) 2020 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#pragma once

#include "DISP_MUTEX_c_header.h"

#define DISP_OVL_SEPARATE_MUTEX_ID (DISP_MUTEX_DDP_LAST+1)    /* other disp will not see mutex 4 */

#define DISP_REG_CONFIG_MUTEX_INTEN                    DISP0_MUTEX_INTEN
#define DISP1_REG_CONFIG_MUTEX_INTEN                   DISP1_MUTEX_INTEN
#define DISP_REG_CONFIG_MUTEX_INTSTA                DISP0_MUTEX_INTSTA
#define DISP1_REG_CONFIG_MUTEX_INTSTA               DISP1_MUTEX_INTSTA
#define DISP_REG_CONFIG_MUTEX_CFG                    DISP0_MUTEX_CFG
#define DISP1_REG_CONFIG_MUTEX_CFG                   DISP1_MUTEX_CFG
    #define CFG_FLD_HW_CG                                CFG_FLD_MUTEX_CFG

#define DISP_REG_CONFIG_MUTEX_REG_UPD_TIMEOUT        DISP_REG_UPD_TIMEOUT

#define DISP_REG_CONFIG_MUTEX0_EN                    DISP0_MUTEX0_EN
#define DISP1_REG_CONFIG_MUTEX0_EN                   DISP1_MUTEX0_EN
    #define EN_FLD_MUTEX0_EN                            DISP_MUTEX0_EN_FLD_MUTEX_EN

#define DISP_REG_CONFIG_MUTEX0_RST                    DISP0_MUTEX0_RST
#define DISP1_REG_CONFIG_MUTEX0_RST                   DISP1_MUTEX0_RST
    #define RST_FLD_MUTEX0_RST                            DISP_MUTEX0_RST_FLD_MUTEX_RST

#define DISP_REG_CONFIG_MUTEX0_SOF                    DISP0_MUTEX0_CTL
#define DISP1_REG_CONFIG_MUTEX0_SOF                   DISP1_MUTEX0_CTL
    #define SOF_FLD_MUTEX0_SOF                            DISP_MUTEX0_CTL_FLD_MUTEX_SOF
    #define SOF_FLD_MUTEX0_SOF_TIMING                    DISP_MUTEX0_CTL_FLD_MUTEX_SOF_TIMING
    #define SOF_FLD_MUTEX0_SOF_WAIT                        DISP_MUTEX0_CTL_FLD_MUTEX_SOF_WAIT
    #define SOF_FLD_MUTEX0_EOF                            DISP_MUTEX0_CTL_FLD_MUTEX_EOF
    #define SOF_FLD_MUTEX0_EOF_TIMING                    DISP_MUTEX0_CTL_FLD_MUTEX_EOF_TIMING
    #define SOF_FLD_MUTEX0_EOF_WAIT                        DISP_MUTEX0_CTL_FLD_MUTEX_EOF_WAIT

#define SOF_VAL_MUTEX0_SOF_SINGLE_MODE                    (0)
#define SOF_VAL_MUTEX0_SOF_FROM_DSI0                    (1)
#define SOF_VAL_MUTEX0_SOF_FROM_DPI                    (2)
#define SOF_VAL_MUTEX0_SOF_FROM_DSI1                        (3)
#define SOF_VAL_MUTEX0_EOF_SINGLE_MODE                    (0)
#define SOF_VAL_MUTEX0_EOF_FROM_DSI0                    (1)
#define SOF_VAL_MUTEX0_EOF_FROM_DPI                    (2)
#define SOF_VAL_MUTEX0_EOF_FROM_DSI1                        (3)

#define DISP_REG_CONFIG_MUTEX0_MOD0                    DISP0_MUTEX0_MOD0
#define DISP_REG_CONFIG_MUTEX0_MOD1                    DISP0_MUTEX0_MOD1

#define DISP1_REG_CONFIG_MUTEX0_MOD0                   DISP1_MUTEX0_MOD0
#define DISP1_REG_CONFIG_MUTEX0_MOD1                   DISP1_MUTEX0_MOD1

#define DISP_REG_CONFIG_DEBUG_OUT_SEL                DISP_MUTEX_DEBUG_OUT_SEL

#define DISP_REG_CONFIG_MUTEX_EN(n)                    (DISP_REG_CONFIG_MUTEX0_EN + (0x20 * (n)))
#define DISP_REG_CONFIG_MUTEX_RST(n)                (DISP_REG_CONFIG_MUTEX0_RST + (0x20 * (n)))
#define DISP_REG_CONFIG_MUTEX_MOD0(n)                (DISP_REG_CONFIG_MUTEX0_MOD0 + (0x20 * (n)))
#define DISP_REG_CONFIG_MUTEX_MOD1(n)                (DISP_REG_CONFIG_MUTEX0_MOD1 + (0x20 * (n)))
#define DISP_REG_CONFIG_MUTEX_SOF(n)                (DISP_REG_CONFIG_MUTEX0_SOF + (0x20 * (n)))

#define DISP1_REG_CONFIG_MUTEX_EN(n)                 (DISP1_REG_CONFIG_MUTEX0_EN + (0x20 * (n)))
#define DISP1_REG_CONFIG_MUTEX_RST(n)                (DISP1_REG_CONFIG_MUTEX0_RST + (0x20 * (n)))
#define DISP1_REG_CONFIG_MUTEX_MOD0(n)               (DISP1_REG_CONFIG_MUTEX0_MOD0 + (0x20 * (n)))
#define DISP1_REG_CONFIG_MUTEX_MOD1(n)               (DISP1_REG_CONFIG_MUTEX0_MOD1 + (0x20 * (n)))
#define DISP1_REG_CONFIG_MUTEX_SOF(n)                (DISP1_REG_CONFIG_MUTEX0_SOF + (0x20 * (n)))
