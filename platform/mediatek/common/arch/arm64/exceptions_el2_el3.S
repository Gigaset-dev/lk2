/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */
#include <asm.h>
#include <arch/arm64/mmu.h>
#include <arch/asm_macros.h>

/* use x9 ~ x15 as scratch registers */
tmp     .req x9

#define ESR_EC_SHIFT    26
#define ESR_EC_LENGTH   6
#define EC_AARCH64_HVC  0x16
#define EC_AARCH64_SMC  0x17

.weak mtk_sip
FUNCTION(mtk_sip)
    ret

.section .text.boot.vectab
.align 12

/*
 * The next boot stage after lk can be ATF (lk as bl2 bootloader), linux
 * kernel or hypervisor (lk as bl33 bootloader). Different entry execution
 * level is required for each next boot stage,
 *      - ATF: from EL3
 *      - linux kernel: from EL2 or EL1
 *      - hypervisor: from EL2
 * It's necessary for lk to return to its beginning entry level before jumping
 * to next boot stage.
 *
 * SMC or HVC will be used for this purpose, thus we install only the exception
 * vector to handle sync exception from lower exception level.
 *
 * [TODO] add rest exception vectors to catch unhandled exceptions.
 */
.Lel2_or_el3_exception_base:
FUNCTION(arm64_el2_or_el3_exception_base)
/* exceptions from lower EL, running arm64 */
.org 0x400
LOCAL_FUNCTION(arm64_sync_exc_lower_el_64)
#if WITH_KERNEL_VM
    mov     tmp, sp
    and     sp, tmp, #~(~0 << MMU_KERNEL_SIZE_SHIFT)
#endif
    mrs     tmp, CurrentEL
    cmp     tmp, #(0b11 << 2)   /* in EL3? */
    b.ne    .LnotEL3
    mrs     tmp, esr_el3
    b       .Lcheck_ec

.LnotEL3:
    cmp     tmp, #(0b10 << 2)   /* in EL2? */
    b.ne    .Lunhandled_sync_exc
    mrs     tmp, esr_el2

.Lcheck_ec:
    ubfx    tmp, tmp, #ESR_EC_SHIFT, #ESR_EC_LENGTH
    cmp     tmp, #EC_AARCH64_SMC
    b.eq    .Lsip_handler
    cmp     tmp, #EC_AARCH64_HVC
    b.ne    .Lunhandled_sync_exc

.Lsip_handler:
    b       mtk_sip

.Lunhandled_sync_exc:
    b       .

