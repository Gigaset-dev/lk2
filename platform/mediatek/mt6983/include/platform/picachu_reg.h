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
 *             MCUSYS register
 ******************************************************************************/
#define MCUCFG_CORE0_CONFIG0                 (MCUCFG_BASE + 0x0000) //RVBARADDR0_31_2
#define MCUCFG_CORE0_CONFIG1                 (MCUCFG_BASE + 0x0004) //RVBARADDR0_39_32
#define MCUCFG_CORE1_CONFIG0                 (MCUCFG_BASE + 0x0008) //RVBARADDR1_31_2
#define MCUCFG_CORE1_CONFIG1                 (MCUCFG_BASE + 0x000C) //RVBARADDR1_39_32
#define MCUCFG_CORE2_CONFIG0                 (MCUCFG_BASE + 0x0010) //RVBARADDR2_31_2
#define MCUCFG_CORE2_CONFIG1                 (MCUCFG_BASE + 0x0014) //RVBARADDR2_39_32
#define MCUCFG_CORE3_CONFIG0                 (MCUCFG_BASE + 0x0018) //RVBARADDR3_31_2
#define MCUCFG_CORE3_CONFIG1                 (MCUCFG_BASE + 0x001C) //RVBARADDR3_39_32
#define MCUCFG_CORE4_CONFIG0                 (MCUCFG_BASE + 0x0020) //RVBARADDR4_31_2
#define MCUCFG_CORE4_CONFIG1                 (MCUCFG_BASE + 0x0024) //RVBARADDR4_39_32
#define MCUCFG_CORE5_CONFIG0                 (MCUCFG_BASE + 0x0028) //RVBARADDR5_31_2
#define MCUCFG_CORE5_CONFIG1                 (MCUCFG_BASE + 0x002C) //RVBARADDR5_39_32
#define MCUCFG_CORE6_CONFIG0                 (MCUCFG_BASE + 0x0030) //RVBARADDR6_31_2
#define MCUCFG_CORE6_CONFIG1                 (MCUCFG_BASE + 0x0034) //RVBARADDR6_39_32
#define MCUCFG_CORE7_CONFIG0                 (MCUCFG_BASE + 0x0038) //RVBARADDR7_31_2
#define MCUCFG_CORE7_CONFIG1                 (MCUCFG_BASE + 0x003C) //RVBARADDR7_39_32

#define MCUCFG_L3_SHARE_CONFIG0              (MCUCFG_BASE + 0x0060) //L3$ Configuration Register
#define L3_SHARE_FULLNHALF                   (0)
#define L3_SHARE_EN                          (1)
#define L3_SHARE_PRE_EN                      (2)
#define L3_AUTO_RESP                         (3)

#define MCUSYS_CORE0_RESEREVED_REG0          (MCUCFG_BASE + 0xFF80)
#define MCUSYS_CORE1_RESEREVED_REG0          (MCUCFG_BASE + 0xFF88)
#define MCUSYS_CORE2_RESEREVED_REG0          (MCUCFG_BASE + 0xFF90)
#define MCUSYS_CORE3_RESEREVED_REG0          (MCUCFG_BASE + 0xFF98)
#define MCUSYS_CORE4_RESEREVED_REG0          (MCUCFG_BASE + 0xFFA0)
#define MCUSYS_CORE5_RESEREVED_REG0          (MCUCFG_BASE + 0xFFA8)
#define MCUSYS_CORE6_RESEREVED_REG0          (MCUCFG_BASE + 0xFFB0)
#define MCUSYS_CORE7_RESEREVED_REG0          (MCUCFG_BASE + 0xFFB8)

#define MCUSYS_MCUSYS_RESEREVED_REG0         (MCUCFG_BASE + 0xFFE0)
#define MCUSYS_MCUSYS_RESEREVED_REG1         (MCUCFG_BASE + 0xFFE4)
#define MCUSYS_MCUSYS_RESEREVED_REG2         (MCUCFG_BASE + 0xFFE8)
#define MCUSYS_MCUSYS_RESEREVED_REG3         (MCUCFG_BASE + 0xFFEC)


/*******************************************************************************
 *             EFUSE register
 ******************************************************************************/
#define EFUSE_COUNTER                        (27)

#define PICACHU_EFUSE_PTPOD0                 (IO_BASE + 0x01EE0580UL)
#define PICACHU_EFUSE_PTPOD1                 (IO_BASE + 0x01EE0584UL)
#define PICACHU_EFUSE_PTPOD2                 (IO_BASE + 0x01EE0588UL)
#define PICACHU_EFUSE_PTPOD3                 (IO_BASE + 0x01EE058CUL)
#define PICACHU_EFUSE_PTPOD4                 (IO_BASE + 0x01EE0590UL)
#define PICACHU_EFUSE_PTPOD5                 (IO_BASE + 0x01EE0594UL)
#define PICACHU_EFUSE_PTPOD6                 (IO_BASE + 0x01EE0598UL)
#define PICACHU_EFUSE_PTPOD7                 (IO_BASE + 0x01EE059CUL)
#define PICACHU_EFUSE_PTPOD8                 (IO_BASE + 0x01EE05A0UL)
#define PICACHU_EFUSE_PTPOD9                 (IO_BASE + 0x01EE05A4UL)
#define PICACHU_EFUSE_PTPOD10                (IO_BASE + 0x01EE05A8UL)
#define PICACHU_EFUSE_PTPOD11                (IO_BASE + 0x01EE05ACUL)
#define PICACHU_EFUSE_PTPOD12                (IO_BASE + 0x01EE05B0UL)
#define PICACHU_EFUSE_PTPOD13                (IO_BASE + 0x01EE05B4UL)
#define PICACHU_EFUSE_PTPOD14                (IO_BASE + 0x01EE05B8UL)
#define PICACHU_EFUSE_PTPOD15                (IO_BASE + 0x01EE05BCUL)
#define PICACHU_EFUSE_PTPOD16                (IO_BASE + 0x01EE05C0UL)
#define PICACHU_EFUSE_PTPOD17                (IO_BASE + 0x01EE05C4UL)
#define PICACHU_EFUSE_PTPOD18                (IO_BASE + 0x01EE05C8UL)
#define PICACHU_EFUSE_PTPOD19                (IO_BASE + 0x01EE05CCUL)
#define PICACHU_EFUSE_PTPOD20                (IO_BASE + 0x01EE05D0UL)
#define PICACHU_EFUSE_PTPOD21                (IO_BASE + 0x01EE05D4UL)
#define PICACHU_EFUSE_PTPOD22                (IO_BASE + 0x01EE05D8UL)
#define PICACHU_EFUSE_PTPOD23                (IO_BASE + 0x01EE05DCUL)
#define PICACHU_EFUSE_PTPOD24                (IO_BASE + 0x01EE05E0UL)
#define PICACHU_EFUSE_PTPOD25                (IO_BASE + 0x01EE05E4UL)
#define PICACHU_EFUSE_PTPOD26                (IO_BASE + 0x01EE05E8UL)

#define PICACHU_EFUSE_HRID0                  (IO_BASE + 0x01EE0140UL)
#define PICACHU_EFUSE_HRID1                  (IO_BASE + 0x01EE0144UL)
#define PICACHU_EFUSE_HRID2                  (IO_BASE + 0x01EE0148UL)
#define PICACHU_EFUSE_HRID3                  (IO_BASE + 0x01EE014CUL)

#define PICACHU_SEGMENT_ID_REG               (IO_BASE + 0x01EE04A0UL)

#define PICACHU_EFUSE_FAB_INFO0              (IO_BASE + 0x01EE07A0UL)
#define PICACHU_EFUSE_FAB_INFO1              (IO_BASE + 0x01EE07A4UL)
#define PICACHU_EFUSE_FAB_INFO2              (IO_BASE + 0x01EE07A8UL)
#define PICACHU_EFUSE_FAB_INFO3              (IO_BASE + 0x01EE07ACUL)
#define PICACHU_EFUSE_FAB_INFO4              (IO_BASE + 0x01EE07B0UL)
#define PICACHU_EFUSE_FAB_INFO5              (IO_BASE + 0x01EE07B4UL)
#define PICACHU_EFUSE_FAB_INFO6              (IO_BASE + 0x01EE07B8UL)
#define PICACHU_EFUSE_FAB_INFO7              (IO_BASE + 0x01EE07BCUL)
#define PICACHU_EFUSE_FAB_INFO8              (IO_BASE + 0x01EE07C0UL)
#define PICACHU_EFUSE_FAB_INFO9              (IO_BASE + 0x01EE07C4UL)
#define PICACHU_EFUSE_FAB_INFO10             (IO_BASE + 0x01EE07C8UL)
#define PICACHU_EFUSE_FAB_INFO11             (IO_BASE + 0x01EE07CCUL)
