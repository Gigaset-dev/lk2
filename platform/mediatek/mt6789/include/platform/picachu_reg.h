/*
 * Copyright (c) 2022 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */
#pragma once

/*******************************************************************************
 *                        Base Address Define
 ******************************************************************************/
#include <platform/addressmap.h>


/*******************************************************************************
 *             CSRAM Define
 ******************************************************************************/
#define CSRAM_START                          (SRAM_BASE + 0x00013400UL)
#define CSRAM_END                            (SRAM_BASE + 0x00014000UL)


/*******************************************************************************
 *             EFUSE register
 ******************************************************************************/
#define PICACHU_EFUSE_HRID0                  (IO_BASE + 0x01C10140UL)
#define PICACHU_EFUSE_HRID1                  (IO_BASE + 0x01C10144UL)
#define PICACHU_EFUSE_HRID2                  (IO_BASE + 0x01C10148UL)
#define PICACHU_EFUSE_HRID3                  (IO_BASE + 0x01C1014CUL)

