/*
 * Copyright (c) 2020 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */
#include <asm.h>
#include <arch/ops.h>

#define LOC_SHIFT           24
#define CLIDR_FIELD_WIDTH   3
#define LEVEL_SHIFT         1
#define DCISW               0x0
#define DCCISW              0x1

/* will trash x0-x2, x4-x9, x11, x14, x16-x17 */
LOCAL_FUNCTION(do_dcsw_op)
        cbz     x3, exit
        adr     x14, dcsw_loop_table    // compute inner loop address
        add     x14, x14, x0, lsl #5    // inner loop is 8x32-bit instructions
        mov     x0, x9
        mov     w8, #1
loop1:
        add     x2, x10, x10, lsr #1    // work out 3x current cache level
        lsr     x1, x0, x2              // extract cache type bits from clidr
        and     x1, x1, #7              // mask the bits for current cache only
        cmp     x1, #2                  // see what cache we have at this level
        b.lt    level_done              // nothing to do if no cache or icache

        msr     csselr_el1, x10         // select current cache level in csselr
        isb                             // isb to sych the new cssr&csidr
        mrs     x1, ccsidr_el1          // read the new ccsidr
        and     x2, x1, #7              // extract the length of the cache lines
        add     x2, x2, #4              // add 4 (line length offset)
        mrs     x4, s3_0_c0_c7_2        // read memory model feature register
        ubfx    x4, x4, #20, #4         // check if Armv8.3-CCIDX implemented
        cmp     x4, #1                  // if support, then extract maximum
        ubfx    x4, x1, #3, #21         //   way number from bit[23:3]
        b.eq    1f                      // else
        ubfx    x4, x1, #3, #10         //   way number from bit[12:3]
1:      clz     w5, w4                  // bit position of way size increment
        lsl     w9, w4, w5              // w9 = aligned max way number
        lsl     w16, w8, w5             // w16 = way number loop decrement
        orr     w9, w10, w9             // w9 = combine way and cache number
        ubfx    x6, x1, #32, #24        // if support CCIDX, number of sets
        b.eq    2f                      //   extract from bit[55:32]
        ubfx    w6, w1, #13, #15        // else extrace from bit[27:13]
2:      lsl     w17, w8, w2             // w17 = set number loop decrement
        dsb     sy                      // barrier before we start this level
        br      x14                     // jump to DC operation specific loop

        .macro  dcsw_loop _op
loop2_\_op:
        lsl     w7, w6, w2              // w7 = aligned max set number

loop3_\_op:
        orr     w11, w9, w7             // combine cache, way and set number
        dc      \_op, x11
        subs    w7, w7, w17             // decrement set number
        b.ge    loop3_\_op

        subs    x9, x9, x16             // decrement way number
        b.ge    loop2_\_op

        b       level_done
.endm

level_done:
        add     x10, x10, #2            // increment cache number
        cmp     x3, x10
        b.gt    loop1
        msr     csselr_el1, xzr         // select cache level 0 in csselr
        dsb     sy                      // barrier to complete final cache operation
        isb
exit:
        ret

dcsw_loop_table:
        dcsw_loop isw
        dcsw_loop cisw
        dcsw_loop csw

/* will trash x3, x9, x10 */
.macro  dcsw_op shift, fw, ls
        mrs     x9, clidr_el1
        ubfx    x3, x9, \shift, \fw
        lsl     x3, x3, \ls
        mov     x10, xzr
        bl      do_dcsw_op
.endm

/* void arch_enable_cache(uint flags);
 * For EL1  only.
 */
FUNCTION(arch_enable_cache)
    stp     x29, x30, [sp, #-32]!
    stp     x24, x25, [sp, #16]

    mov     x25, x0
    /* check DCACHE flag */
    tst     x25, #DCACHE
    b.eq    .L__enable_icache
    mrs     x24, sctlr_el1
    tst     x24, #(1<<2)
    b.ne    .L__enable_icache

    /* invalidate dcache */
    mov     x0, #DCISW
    dcsw_op #LOC_SHIFT, #CLIDR_FIELD_WIDTH, #LEVEL_SHIFT

    /* enable dcache enable bit */
    orr     x24, x24, #(1<<2)
    msr     sctlr_el1, x24

.L__enable_icache:
    /* check ICACHE flag */
    tst     x25, #ICACHE
    b.eq    .L__done_enable
    mrs     x24, sctlr_el1
    tst     x24, #(1<<12)
    b.ne    .L__done_enable

    /* invalidate icache */
    dsb     sy
    ic      iallu
    dsb     sy
    isb

    /* enable icache enable bit */
    mrs     x24, sctlr_el1
    orr     x24, x24, #(1<<12)
    msr     sctlr_el1, x24

.L__done_enable:
    ldp     x24, x25, [sp, #16]
    ldp     x29, x30, [sp], #32
    ret

/* void arch_disable_cache(uint flags) */
/* only for el1 here */
FUNCTION(arch_disable_cache)
    stp     x29, x30, [sp, #-32]!
    str     x25, [sp, #16]

    mov     x25, x0
    /* check DCACHE flag */
    tst     x25, #DCACHE
    b.eq    .L__disable_icache
    mrs     x1, sctlr_el1
    tst     x1, #(1<<2)
    b.eq    .L__dcache_already_disabled

    /* disable dcache enable bit */
    bic     x1, x1, #(1<<2)
    msr     sctlr_el1, x1

    /* clean & invalidate dcache */
    mov     x0, #DCCISW
    b       .L__flush_dcache

.L__dcache_already_disabled:
    /* invalidate dcache */
    mov     x0, #DCISW
.L__flush_dcache:
    dcsw_op #LOC_SHIFT, #CLIDR_FIELD_WIDTH, #LEVEL_SHIFT

.L__disable_icache:
    /* check ICACHE flag */
    tst     x25, #ICACHE
    b.eq    .L__done_disable
    /* disable icache enable bit */
    mrs     x1, sctlr_el1
    bic     x1, x1, #(1<<12)
    msr     sctlr_el1, x1

    /* invalidate icache for PE to PoU */
    dsb     sy
    ic      iallu
    dsb     sy
    isb

.L__done_disable:
    ldr     x25, [sp, #16]
    ldp     x29, x30, [sp], #32
    ret
