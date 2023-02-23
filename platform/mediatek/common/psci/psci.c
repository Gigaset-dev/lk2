/*
 * Copyright (c) 2020 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#include <debug.h>
#include <smc.h>
#include <sys/types.h>

#define PSCI_CPU_OFF_FUNC_ID    0x84000002
#define PSCI_CPU_ON_FUNC_ID_32  0x84000003
#define PSCI_CPU_ON_FUNC_ID_64  0xc4000003

#if ARCH_ARM64
#define PSCI_CPU_ON_FUNC_ID     PSCI_CPU_ON_FUNC_ID_64
#else
#define PSCI_CPU_ON_FUNC_ID     PSCI_CPU_ON_FUNC_ID_32
#endif

int psci_cpu_on(ulong target_cpu, ulong ep, ulong context_id)
{
    struct __smccc_res res;

    __smc_conduit(PSCI_CPU_ON_FUNC_ID, target_cpu, ep, context_id, 0, 0, 0, 0,
                  &res);
    return (int)res.a0;
}

int psci_cpu_off(void)
{
    struct __smccc_res res;

    __smc_conduit(PSCI_CPU_OFF_FUNC_ID, 0, 0, 0, 0, 0, 0, 0, &res);
    return (int)res.a0;
}
