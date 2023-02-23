/*
 * Copyright (c) 2020 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#include <sys/types.h>

void __cpu_early_init(void)
{
    /* Set cntfrq 13M in arch/arm/arm/start.S
     * DO NOT use arm_write_cntfrq expending from GEN_CP15_REG_FUNCS(cntfrq, 0, c14, c0, 0),
     * since it would override r0 which is the address of pl_boottags. Therefore use r12 instead.
     */
    __asm__ volatile(
            "movw   r12, #23872\n\t"
            "movt   r12, #198\n\t"
            "mcr p15, 0, r12, c14, c0, 0\n\t"
            );
}
