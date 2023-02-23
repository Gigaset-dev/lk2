/*
 * Copyright (c) 2020 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */
#pragma once

#include <sys/types.h>

#if WITH_SMP

struct smp_cpu_info {
    uint id[SMP_MAX_CPUS];  /* Bit[0:23] of mpidr for each core */
    uint cpu_on_mask;       /* Cpu mask to indicate which cpu to be on.
                             * Bit 0: don't care.
                             * For example: 0x11 (or 0x10) to bring up cpu 4.
                             */
};

/* power off non boot cpus */
void plat_mp_off(void);

/* handover bootcpu to cpu */
void plat_mp_bootcpu_handover(uint32_t cpu);

#endif
