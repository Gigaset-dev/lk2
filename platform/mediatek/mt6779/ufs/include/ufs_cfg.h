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
/*--------------------------------------------------------------------------
 * FPGA Definition
 *--------------------------------------------------------------------------
 */

#ifdef MACH_FPGA
#define UFS_CFG_FPGA_PLATFORM
#endif

/*--------------------------------------------------------------------------*/
/* UFS Driver Features                                                      */
/*--------------------------------------------------------------------------*/

/*
 * Defined:     Use slow UFS device speed for safe bring-up. Used during development stage only.
 * Non-defined: Use high UFS device speed for performance. Shall be default configuration.
 */

/* #define UFS_CFG_SAFE_BRING_UP */

/*--------------------------------------------------------------------------
 * Common Definition
 *--------------------------------------------------------------------------
 */

/* UFS PA power modes */
enum {
    FAST_MODE       = 1,
    SLOW_MODE       = 2,
    FASTAUTO_MODE   = 4,
    SLOWAUTO_MODE   = 5,
    UNCHANGED       = 7,
};

/* UFS PA TX/RX Frequency Series */
enum {
    PA_HS_MODE_A    = 1,
    PA_HS_MODE_B    = 2,
};

/* UFS device default power mode, gear, lane configurations */
#ifndef UFS_CFG_SAFE_BRING_UP

#define UFS_DEV_MAX_GEAR_RX     (1)
#define UFS_DEV_MAX_GEAR_TX     (1)
#define UFS_DEV_MAX_LANE_RX     (1)
#define UFS_DEV_MAX_LANE_TX     (1)
#define UFS_DEV_DEFAULT_HS_RATE (PA_HS_MODE_B)
#define UFS_DEV_DEFAULT_PWR_RX  (FAST_MODE)
#define UFS_DEV_DEFAULT_PWR_TX  (FAST_MODE)

#else

/* Use PWM-G4 for safe bring-up case */
#define UFS_DEV_MAX_GEAR_RX     (4)
#define UFS_DEV_MAX_GEAR_TX     (4)
#define UFS_DEV_MAX_LANE_RX     (1)
#define UFS_DEV_MAX_LANE_TX     (1)
#define UFS_DEV_DEFAULT_HS_RATE (PA_HS_MODE_A)
#define UFS_DEV_DEFAULT_PWR_RX  (SLOW_MODE)
#define UFS_DEV_DEFAULT_PWR_TX  (SLOW_MODE)

#endif
