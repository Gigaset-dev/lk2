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
#define IO_BASE                              (0x10000000UL)
#endif



/*******************************************************************************
 *             CSRAM Define
 ******************************************************************************/
#define CSRAM_START                          (SRAM_BASE + 0x00013400UL)
#define CSRAM_END                            (SRAM_BASE + 0x00014000UL)


/*******************************************************************************
 *             EFUSE register
 ******************************************************************************/
#define EFUSE_COUNTER                        (16)

#define PICACHU_EFUSE_PTPOD0                 (IO_BASE + 0x01F10580UL)
#define PICACHU_EFUSE_PTPOD1                 (IO_BASE + 0x01F10584UL)
#define PICACHU_EFUSE_PTPOD2                 (IO_BASE + 0x01F10588UL)
#define PICACHU_EFUSE_PTPOD3                 (IO_BASE + 0x01F1058CUL)
#define PICACHU_EFUSE_PTPOD4                 (IO_BASE + 0x01F10590UL)
#define PICACHU_EFUSE_PTPOD5                 (IO_BASE + 0x01F10594UL)
#define PICACHU_EFUSE_PTPOD6                 (IO_BASE + 0x01F10598UL)
#define PICACHU_EFUSE_PTPOD7                 (IO_BASE + 0x01F1059CUL)
#define PICACHU_EFUSE_PTPOD8                 (IO_BASE + 0x01F105A0UL)
#define PICACHU_EFUSE_PTPOD9                 (IO_BASE + 0x01F105A4UL)
#define PICACHU_EFUSE_PTPOD10                (IO_BASE + 0x01F105A8UL)
#define PICACHU_EFUSE_PTPOD11                (IO_BASE + 0x01F105ACUL)
#define PICACHU_EFUSE_PTPOD12                (IO_BASE + 0x01F105B0UL)
#define PICACHU_EFUSE_PTPOD13                (IO_BASE + 0x01F105B4UL)
#define PICACHU_EFUSE_PTPOD14                (IO_BASE + 0x01F105B8UL)
#define PICACHU_EFUSE_PTPOD15                (IO_BASE + 0x01F105BCUL)

#define PICACHU_EFUSE_HRID0                  (IO_BASE + 0x01F10140UL)
#define PICACHU_EFUSE_HRID1                  (IO_BASE + 0x01F10144UL)
#define PICACHU_EFUSE_HRID2                  (IO_BASE + 0x01F10148UL)
#define PICACHU_EFUSE_HRID3                  (IO_BASE + 0x01F1014CUL)

#define PICACHU_SEGMENT_ID_REG               (IO_BASE + 0x01F104A0UL)

#define PICACHU_EFUSE_FAB_INFO0              (IO_BASE + 0x01F107A0UL)
#define PICACHU_EFUSE_FAB_INFO1              (IO_BASE + 0x01F107A4UL)
#define PICACHU_EFUSE_FAB_INFO2              (IO_BASE + 0x01F107A8UL)
#define PICACHU_EFUSE_FAB_INFO3              (IO_BASE + 0x01F107ACUL)
#define PICACHU_EFUSE_FAB_INFO4              (IO_BASE + 0x01F107B0UL)
#define PICACHU_EFUSE_FAB_INFO5              (IO_BASE + 0x01F107B4UL)
#define PICACHU_EFUSE_FAB_INFO6              (IO_BASE + 0x01F107B8UL)
#define PICACHU_EFUSE_FAB_INFO7              (IO_BASE + 0x01F107BCUL)
#define PICACHU_EFUSE_FAB_INFO8              (IO_BASE + 0x01F107C0UL)
#define PICACHU_EFUSE_FAB_INFO9              (IO_BASE + 0x01F107C4UL)
#define PICACHU_EFUSE_FAB_INFO10             (IO_BASE + 0x01F107C8UL)
#define PICACHU_EFUSE_FAB_INFO11             (IO_BASE + 0x01F107CCUL)
