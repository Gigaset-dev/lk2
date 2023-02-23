/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */
#pragma once

#include <platform/mcupm_plat.h>


// Log @ CSRAM
#define MCUPM_EEM_LOG_ADDR        (SRAM_BASE + 0x00012800) // 0x0011_2800
#define MCUPM_EEM_LOG_SIZE        (0xC00)

#define MCUPM_PICACHU_LOG_ADDR    (SRAM_BASE + 0x00013400) // 0x0011_3400
#define MCUPM_PICACHU_LOG_SIZE    (0xC00)


