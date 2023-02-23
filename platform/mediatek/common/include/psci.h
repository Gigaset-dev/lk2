/*
 * Copyright (c) 2020 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#pragma once

#include <sys/types.h>

/* PSCI v1.0 error code from section 5.2.2 */
enum {
    E_PSCI_SUCCESS              = 0,
    E_PSCI_NOT_SUPPORTED        = -1,
    E_PSCI_INVALID_PARAMETERS   = -2,
    E_PSCI_DENIED               = -3,
    E_PSCI_ALREADY_ON           = -4,
    E_PSCI_ON_PENDING           = -5,
    E_PSCI_INTERNAL_FAILURE     = -6,
    E_PSCI_NOT_PRESENT          = -7,
    E_PSCI_DISABLED             = -8,
    E_PSCI_INVALID_ADDRESS      = -9,
};

/*
 * power on target cpu
 *
 * param
 *      - target_cpu: the cpu to be powered on
 *      - ep: entrypoint of the target_cpu
 *      - context_id: 32 bits for SMC32 and 64 bits for SMC64. When the
 *                    target_cpu first enters the return non-secure exception
 *                    level, this value must be presented in X0 or R0.
 * return
 *      - E_PSCI_SUCCESS on success,
 *      - other error code on failure
 */
int psci_cpu_on(ulong target_cpu, ulong ep, ulong context_id);

/*
 * power down the calling cpu
 *
 * return
 *      - doesn't return on success,
 *      - E_PSCI_DENIED on failure
 */
int psci_cpu_off(void);
