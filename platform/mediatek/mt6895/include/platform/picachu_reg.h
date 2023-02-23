/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */
#pragma once

/*******************************************************************************
 *                        Base Address Define
 ******************************************************************************/
#if LK_AS_BL2_EXT
#include <platform/addressmap.h>
#else
#define SRAM_BASE                            (0x00100000UL)
#define MCUCFG_BASE                          (0x0C000000UL)
#define IO_BASE                              (0x10000000UL)
#endif


/*******************************************************************************
 *                        CSRAM Define
 ******************************************************************************/
#define CSRAM_START                          (SRAM_BASE + 0x0012E00UL)
#define CSRAM_END                            (SRAM_BASE + 0x0013C00UL)


/*******************************************************************************
 *             EFUSE register
 ******************************************************************************/

#define PICACHU_EFUSE_HRID0                  (IO_BASE + 0x01F10140UL)
#define PICACHU_EFUSE_HRID1                  (IO_BASE + 0x01F10144UL)
#define PICACHU_EFUSE_HRID2                  (IO_BASE + 0x01F10148UL)
#define PICACHU_EFUSE_HRID3                  (IO_BASE + 0x01F1014CUL)


