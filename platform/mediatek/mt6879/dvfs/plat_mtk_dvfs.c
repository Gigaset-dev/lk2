/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#include <platform/sec_devinfo.h>

#define DEVINFO_IDX_CPU_LEVEL  7 //0x11F10050[7:0]
#define DEVINFO_OFF_CPU_LEVEL  0

unsigned int get_cpu_level(void)
{
    return ((get_devinfo_with_index(DEVINFO_IDX_CPU_LEVEL) >> DEVINFO_OFF_CPU_LEVEL) & 0xFF);
}
