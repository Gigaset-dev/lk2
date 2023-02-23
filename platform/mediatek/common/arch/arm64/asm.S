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
#include <asm.h>
#include <arch/asm_macros.h>

tmp  .req x9

.weak platform_elX_init
FUNCTION(platform_elX_init)
    ret

FUNCTION(arm64_elX_init)
    adrp tmp, __stack_end
    mov  sp, tmp

    push x0, x1
    push x2, x3
    push x29, x30

    bl   platform_elX_init

    pop  x29, x30
    pop  x2, x3
    pop  x0, x1

    ret

FUNCTION(arm64_chain_load)
    /* shuffle the args around */
    mov x5, x0
    mov x0, x1
    mov x1, x2
    mov x2, x3
    mov x3, x4
    mov x4, x5

#if WITH_KERNEL_VM
    /* disable MMU */
    mrs x5, sctlr_el1
    bic x5, x5, #0x1
    msr sctlr_el1, x5
    isb
#endif

    tlbi vmalle1
    br  x4
