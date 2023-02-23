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

#if !IS_64BIT
#include <arch/arm/mmu.h>
#endif

#define SZ_2M  0x200000
#define SZ_4G  0x100000000

#define SHIFT_2M  (21)
#define SHIFT_16M (24)

#if IS_64BIT
#define AEE_MAP_SIZE  SZ_2M
#define AEE_MAP_ALIGN SHIFT_2M
#else
#define AEE_MAP_SIZE  SUPERSECTION_SIZE
#define AEE_MAP_ALIGN SHIFT_16M
#endif

/* ram_console/mrdump_cblock DRAM offset */
#define MRDUMP_CB_DRAM_OFF   0X2000
#define MRDUMP_CB_SIZE       0x8000
#define RAM_CONSOLE_DRAM_OFF 0x1000
#define RAM_CONSOLE_SIZE     0x1000

#define AEE_ENABLE_NO   0
#define AEE_ENABLE_MINI 1
#define AEE_ENABLE_FULL 2

#define AEE_DEFAULT_SETTING AEE_ENABLE_MINI

int aee_check_enable(void);
int mrdump_check_enable(void);
