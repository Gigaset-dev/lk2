/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */
#pragma once
#include <platform/addressmap.h>

#ifdef PROJECT_TYPE_FPGA
#define BOOT_TRAP_MASK  (0xFFFFFFFF)
#define TRAP_TEST_RD    (IO_BASE + 0x00001ed0)
#define BOOT_FROM_PNAND (0x80800000)
#define BOOT_FROM_EMMC  (0x80800010)
#define BOOT_FROM_SNAND (0x80800020)
#else
#define BOOT_TRAP_MASK  (0x00380000)
#define TRAP_TEST_RD    (GPIO_BASE + 0x000006f0)
#define BOOT_FROM_PNAND (0x00000000)
#define BOOT_FROM_EMMC  (0x00080000)
#define BOOT_FROM_SNAND (0x00100000)
#endif
