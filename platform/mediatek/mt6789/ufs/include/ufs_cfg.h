/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#pragma once
/*--------------------------------------------------------------------------
 * FPGA Definition
 *--------------------------------------------------------------------------
 */

#ifdef PROJECT_TYPE_FPGA
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

#ifdef UFS_CFG_FPGA_PLATFORM
#define UFS_DEV_MAX_GEAR_RX     (1)
#define UFS_DEV_MAX_GEAR_TX     (1)
#else
#define UFS_DEV_MAX_GEAR_RX     (3)
#define UFS_DEV_MAX_GEAR_TX     (3)
#endif
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
