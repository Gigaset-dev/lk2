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

#define DISP_OVL_SEPARATE_MUTEX_ID                  (DISP_MUTEX_DDP_LAST + 1)
#define DISP_REG_CONFIG_MUTEX_INTEN                 (DISPSYS_MUTEX_BASE + 0x000)
#define DISP_REG_CONFIG_MUTEX_INTSTA                (DISPSYS_MUTEX_BASE + 0x004)
#define DISP_REG_CONFIG_MUTEX_CFG                   (DISPSYS_MUTEX_BASE + 0x008)
#define DISP_REG_CONFIG_MUTEX_UPD_TIMEOUT           (DISPSYS_MUTEX_BASE + 0x00C)
#define DISP_REG_CONFIG_MUTEX_REG_COMMIT0           (DISPSYS_MUTEX_BASE + 0x010)
#define DISP_REG_CONFIG_MUTEX_INTEN_1               (DISPSYS_MUTEX_BASE + 0x018)
#define DISP_REG_CONFIG_MUTEX_INTSTA_1              (DISPSYS_MUTEX_BASE + 0x01C)

#define DISP_REG_CONFIG_MUTEX0_EN                   (DISPSYS_MUTEX_BASE + 0x020)
#define EN_FLD_MUTEX0_EN                            REG_FLD(1, 0)
#define DISP_REG_CONFIG_MUTEX0_GET                  (DISPSYS_MUTEX_BASE + 0x024)
#define GET_FLD_MUTEX0_GET                          REG_FLD(1, 0)
#define GET_FLD_INT_MUTEX0_EN                       REG_FLD(1, 1)
#define DISP_REG_CONFIG_MUTEX0_RST                  (DISPSYS_MUTEX_BASE + 0x028)
#define RST_FLD_MUTEX0_RST                          REG_FLD(1, 0)
#define DISP_REG_CONFIG_MUTEX0_SOF                  (DISPSYS_MUTEX_BASE + 0x02C)
#define SOF_FLD_MUTEX0_SOF                          REG_FLD(3, 0)
#define SOF_FLD_MUTEX0_SOF_TIMING                   REG_FLD(2, 3)
#define SOF_FLD_MUTEX0_SOF_WAIT                     REG_FLD(1, 5)
#define SOF_FLD_MUTEX0_EOF                          REG_FLD(3, 6)
#define SOF_FLD_MUTEX0_FOF_TIMING                   REG_FLD(2, 9)
#define SOF_FLD_MUTEX0_EOF_WAIT                     REG_FLD(1, 11)
#define DISP_REG_CONFIG_MUTEX0_MOD0                 (DISPSYS_MUTEX_BASE + 0x030)

#define DISP_REG_CONFIG_MUTEX1_EN                   (DISPSYS_MUTEX_BASE + 0x040)
#define DISP_REG_CONFIG_MUTEX1_GET                  (DISPSYS_MUTEX_BASE + 0x044)
#define DISP_REG_CONFIG_MUTEX1_RST                  (DISPSYS_MUTEX_BASE + 0x048)
#define DISP_REG_CONFIG_MUTEX1_SOF                  (DISPSYS_MUTEX_BASE + 0x04C)
#define DISP_REG_CONFIG_MUTEX1_MOD0                 (DISPSYS_MUTEX_BASE + 0x050)

#define DISP_REG_CONFIG_MUTEX2_EN                   (DISPSYS_MUTEX_BASE + 0x060)
#define DISP_REG_CONFIG_MUTEX2_GET                  (DISPSYS_MUTEX_BASE + 0x064)
#define DISP_REG_CONFIG_MUTEX2_RST                  (DISPSYS_MUTEX_BASE + 0x068)
#define DISP_REG_CONFIG_MUTEX2_SOF                  (DISPSYS_MUTEX_BASE + 0x06C)
#define DISP_REG_CONFIG_MUTEX2_MOD0                 (DISPSYS_MUTEX_BASE + 0x070)

#define DISP_REG_CONFIG_MUTEX3_EN                   (DISPSYS_MUTEX_BASE + 0x080)
#define DISP_REG_CONFIG_MUTEX3_GET                  (DISPSYS_MUTEX_BASE + 0x084)
#define DISP_REG_CONFIG_MUTEX3_RST                  (DISPSYS_MUTEX_BASE + 0x088)
#define DISP_REG_CONFIG_MUTEX3_SOF                  (DISPSYS_MUTEX_BASE + 0x08C)
#define DISP_REG_CONFIG_MUTEX3_MOD0                 (DISPSYS_MUTEX_BASE + 0x090)

#define DISP_REG_CONFIG_MUTEX4_EN                   (DISPSYS_MUTEX_BASE + 0x0A0)
#define DISP_REG_CONFIG_MUTEX4_GET                  (DISPSYS_MUTEX_BASE + 0x0A4)
#define DISP_REG_CONFIG_MUTEX4_RST                  (DISPSYS_MUTEX_BASE + 0x0A8)
#define DISP_REG_CONFIG_MUTEX4_SOF                  (DISPSYS_MUTEX_BASE + 0x0AC)
#define DISP_REG_CONFIG_MUTEX4_MOD0                 (DISPSYS_MUTEX_BASE + 0x0B0)

#define DISP_REG_CONFIG_MUTEX5_EN                   (DISPSYS_MUTEX_BASE + 0x0C0)
#define DISP_REG_CONFIG_MUTEX5_GET                  (DISPSYS_MUTEX_BASE + 0x0C4)
#define DISP_REG_CONFIG_MUTEX5_RST                  (DISPSYS_MUTEX_BASE + 0x0C8)
#define DISP_REG_CONFIG_MUTEX5_SOF                  (DISPSYS_MUTEX_BASE + 0x0CC)
#define DISP_REG_CONFIG_MUTEX5_MOD0                 (DISPSYS_MUTEX_BASE + 0x0D0)

#define DISP_REG_CONFIG_MUTEX6_EN                   (DISPSYS_MUTEX_BASE + 0x0E0)
#define DISP_REG_CONFIG_MUTEX6_GET                  (DISPSYS_MUTEX_BASE + 0x0E4)
#define DISP_REG_CONFIG_MUTEX6_RST                  (DISPSYS_MUTEX_BASE + 0x0E8)
#define DISP_REG_CONFIG_MUTEX6_SOF                  (DISPSYS_MUTEX_BASE + 0x0EC)
#define DISP_REG_CONFIG_MUTEX6_MOD0                 (DISPSYS_MUTEX_BASE + 0x0F0)

#define DISP_REG_CONFIG_MUTEX7_EN                   (DISPSYS_MUTEX_BASE + 0x100)
#define DISP_REG_CONFIG_MUTEX7_GET                  (DISPSYS_MUTEX_BASE + 0x104)
#define DISP_REG_CONFIG_MUTEX7_RST                  (DISPSYS_MUTEX_BASE + 0x108)
#define DISP_REG_CONFIG_MUTEX7_SOF                  (DISPSYS_MUTEX_BASE + 0x10C)
#define DISP_REG_CONFIG_MUTEX7_MOD0                 (DISPSYS_MUTEX_BASE + 0x110)

#define DISP_REG_CONFIG_DEBUG_OUT_SEL               (DISPSYS_MUTEX_BASE + 0x30C)
#define DEBUG_OUT_SEL_FLD_DEBUG_OUT_SEL                 REG_FLD(2, 0)

#define DISP_REG_CONFIG_MUTEX_EN(n)                 (DISP_REG_CONFIG_MUTEX0_EN + (0x20 * (n)))
#define DISP_REG_CONFIG_MUTEX_GET(n)                (DISP_REG_CONFIG_MUTEX0_GET + (0x20 * (n)))
#define DISP_REG_CONFIG_MUTEX_RST(n)                (DISP_REG_CONFIG_MUTEX0_RST + (0x20 * (n)))
#define DISP_REG_CONFIG_MUTEX_MOD0(n)               (DISP_REG_CONFIG_MUTEX0_MOD0 + (0x20 * (n)))
#define DISP_REG_CONFIG_MUTEX_MOD1(n)               (DISP_REG_CONFIG_MUTEX0_MOD0 + (0x20 * (n)))

#define DISP_REG_CONFIG_MUTEX_SOF(n)                (DISP_REG_CONFIG_MUTEX0_SOF + (0x20 * (n)))

#define SOF_VAL_MUTEX0_SOF_SINGLE_MODE                  (0)
#define SOF_VAL_MUTEX0_SOF_FROM_DSI0                    (1)
#define SOF_VAL_MUTEX0_SOF_FROM_DSI1                    (2)
#define SOF_VAL_MUTEX0_SOF_FROM_DPI                     (3)
#define SOF_VAL_MUTEX0_SOF_FROM_RESERVED                (5)
#define SOF_VAL_MUTEX0_EOF_DISABLE                      (0)
#define SOF_VAL_MUTEX0_EOF_FROM_DSI0                    (1)
#define SOF_VAL_MUTEX0_EOF_FROM_DSI1                    (2)
#define SOF_VAL_MUTEX0_EOF_FROM_DPI                     (3)

