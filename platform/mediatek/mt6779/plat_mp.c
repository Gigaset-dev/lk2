/*
 * Copyright (c) 2020 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#include <arch/mp_mediatek.h>
#include <sys/types.h>

#if WITH_SMP

const struct smp_cpu_info smp_cpu = {
    .id = { 0x0, 0x100, 0x200, 0x300, 0x400, 0x500, 0x600, 0x700 },
    .cpu_on_mask = SMP_CPU_ON_MASK,
};

const struct smp_cpu_info *mp_get_smp_cpu_info(void)
{
    return &smp_cpu;
}

#endif
