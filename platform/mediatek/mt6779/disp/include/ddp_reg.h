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

/* #include <platform/mt_typedefs.h> */
#include <stdlib.h>
/* #include <platform/sync_write.h> */
#include <reg.h>

#include <platform/addressmap.h>

typedef void *cmdqRecHandle;


//TDODO: get base reg addr from system header
#define DISP_INDEX_OFFSET (DISP_OVL0_2L_BASE - DISP_OVL0_BASE) /* used by ovl */
#define DISP_RDMA_INDEX_OFFSET (DISP_RDMA1_BASE - DISP_RDMA0_BASE) /* used by rdma */

#define DISPSYS_OVL0_BASE               DISP_OVL0_BASE
#define DISPSYS_OVL0_2L_BASE            DISP_OVL0_2L_BASE
#define DISPSYS_OVL1_2L_BASE            DISP_OVL1_2L_BASE
#define DISPSYS_WDMA0_BASE              DISP_WDMA0_BASE
#define DISPSYS_RDMA0_BASE              DISP_RDMA0_BASE
#define DISPSYS_COLOR0_BASE             DISP_COLOR0_BASE
#define DISPSYS_AAL0_BASE               DISP_AAL0_BASE
#define DISPSYS_GAMMA0_BASE             DISP_GAMMA0_BASE
#define DISPSYS_SPLIT0_BASE             (0)
#define DISPSYS_DSI0_BASE               DSI0_BASE
#define DISPSYS_DITHER0_BASE            DISP_DITHER0_BASE
#define DISPSYS_DSI1_BASE               (0)
#define DISPSYS_PWM0_BASE               DISP_PWM0_BASE
#define DISPSYS_PWM1_BASE               (0)
#define DISPSYS_MUTEX_BASE              MM_MUTEX_BASE
#define DISPSYS_SMI_LARB0_BASE          SMI_LARB0_BASE
#define DISPSYS_SMI_LARB1_BASE          SMI_LARB1_BASE
#define DISPSYS_SMI_COMMON_BASE         SMI_COMMON_BASE
#define DISPSYS_CCORR0_BASE             DISP_CCORR0_BASE
#define DISPSYS_DPI_BASE                DPI_BASE
#define DISPSYS_MIPITX0_BASE            MIPI_TX0_BASE
#define DISPSYS_MIPITX1_BASE            MIPI_TX1_BASE
#define DISPSYS_CONFIG_BASE             MMSYS_CONFIG_BASE
#define DISPSYS_POSTMASK_BASE           DISP_POSTMASK_BASE
#define DISPSYS_APMIXED_BASE            APMIXED_BASE

// ---------------------------------------------------------------------------
//  Type Casting
// ---------------------------------------------------------------------------

#define AS_INT32(x)     (*(INT32 *)(x))
#define AS_INT16(x)     (*(INT16 *)(x))
#define AS_INT8(x)      (*(INT8  *)(x))

#define AS_UINT32(x)    (*(u32 *)(x))
#define AS_UINT16(x)    (*(u16 *)(x))
#define AS_UINT8(x)     (*(u8  *)(x))


// ---------------------------------------------------------------------------
//  Register Manipulations
// ---------------------------------------------------------------------------
/*
#define READ_REGISTER_UINT32(reg) \
    (*(volatile u32 * const)(reg))

#define WRITE_REGISTER_UINT32(reg, val) \
    (*(volatile u32 * const)(reg)) = (val)

#define READ_REGISTER_UINT16(reg) \
    (*(volatile u16 * const)(reg))

#define WRITE_REGISTER_UINT16(reg, val) \
    (*(volatile u16 * const)(reg)) = (val)

#define READ_REGISTER_UINT8(reg) \
    (*(volatile u8 * const)(reg))

#define WRITE_REGISTER_UINT8(reg, val) \
    (*(volatile u8 * const)(reg)) = (val)
*/

// ---------------------------------------------------------------------------
//  Register Field Access
// ---------------------------------------------------------------------------

#define REG_FLD(width, shift) \
    ((unsigned int)((((width) & 0xFF) << 16) | ((shift) & 0xFF)))

#define REG_FLD_MSB_LSB(msb, lsb) REG_FLD((msb) - (lsb) + 1, (lsb))

#define REG_FLD_WIDTH(field) \
    ((unsigned int)(((field) >> 16) & 0xFF))

#define REG_FLD_SHIFT(field) \
    ((unsigned int)((field) & 0xFF))

#define REG_FLD_MASK(field) \
    ((unsigned int)((1ULL << REG_FLD_WIDTH(field)) - 1) << REG_FLD_SHIFT(field))

#define REG_FLD_VAL(field, val) \
    (((val) << REG_FLD_SHIFT(field)) & REG_FLD_MASK(field))

#define REG_FLD_VAL_GET(field, regval) \
    (((regval) & REG_FLD_MASK(field)) >> REG_FLD_SHIFT(field))


#define DISP_REG_GET(reg32) (*(volatile unsigned int *)(reg32))
#define DISP_REG_GET_FIELD(field, reg32) ((*(volatile unsigned int *)(reg32) & REG_FLD_MASK(field)) >> REG_FLD_SHIFT(field))

/* polling register until masked bit is 1 */
#define DDP_REG_POLLING(reg32, mask) \
    do { \
        while (!((DISP_REG_GET(reg32))&mask))\
            ; \
    } while (0)

/* Polling register until masked bit is 0 */
#define DDP_REG_POLLING_NEG(reg32, mask) \
    do { \
        while ((DISP_REG_GET(reg32))&mask)\
            ; \
    } while (0)

#define DISP_CPU_REG_SET(reg32, val) writel(val, (volatile unsigned int *)(reg32))
#define DISP_CPU_REG_SET_FIELD(field, reg32, val)  \
    do {                                \
           writel((*(volatile unsigned int *)(reg32) & ~REG_FLD_MASK(field))|REG_FLD_VAL((field), (val)), reg32);  \
    } while (0)

#define DISP_REG_MASK(handle, reg32, val, mask)     \
        do { \
            writel((unsigned int)(readl(reg32)&~(mask))|(val), (volatile unsigned long *)(reg32)); \
        } while (0)

#define DISP_REG_SET(handle, reg32, val) \
        do { \
            writel(val, (volatile unsigned int *)(reg32));\
        } while (0)


#define DISP_REG_SET_FIELD(handle, field, reg32, val)  \
        do {  \
             writel((*(volatile unsigned int *)(reg32) & ~REG_FLD_MASK(field))|REG_FLD_VAL((field), (val)), reg32);  \
        } while (0)

#define DISP_REG_CMDQ_POLLING(handle, reg32, val, mask) \
    do { \
        while ((DISP_REG_GET(reg32) & (mask)) != ((val) & (mask)))\
            ; \
    } while (0)

#include "ddp_reg_mmsys.h"
#include "ddp_reg_mutex.h"
#include "ddp_reg_ovl.h"
#include "ddp_reg_dsi.h"
#include "ddp_reg_mipi.h"
#include "ddp_reg_dma.h"
#include "ddp_reg_pq.h"
#include "ddp_reg_postmask.h"

