/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#include <arch/asm_macros.h>
#include <asm.h>

tmp  .req x9

FUNCTION(__hvc_conduit)
    hvc #0

    ldr tmp, [sp]
    stp x0, x1, [tmp], #16
    stp x2, x3, [tmp]

    ret
