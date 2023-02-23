/*
 * Copyright (c) 2020 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */
#include <arch/arch_ops.h>
#include <arch/mp.h>
#include <arch/mp_mediatek.h>
#include <arch/ops.h>
#include <assert.h>
#include <compiler.h>
#include <debug.h>
#include <dev/interrupt/arm_gic.h>
#include <err.h>
#include <kernel/mp.h>
#include <kernel/thread.h>
#include <lk/init.h>
#include <platform/interrupts.h>
#include <psci.h>
#include <sys/types.h>
#include <trace.h>

#if WITH_SMP

#define LOCAL_TRACE 0

/*
 * [platform smp note]
 * Platform should add platform specific stuff to support smp:
 * 1. define following SMP constants in platform specific makefile,
 *      SMP_MAX_CPUS:
 *          Max number of cpus to be used
 *      SMP_CPU_CLUSTER_SHIFT:
 *          Bits to shift for cluster to form linear cpu id.
 *          For example, if at most 4 cores in a cluster, set this to 2.
 * 2. add "struct smp_cpu_info smp_cpu" definition in platform code, and
 *    override mp_get_smp_cpu_info().
 * 3. implement void platform_quiesce(void) to call plat_mp_off().
 * 4. optional: add PLAT_GIC_IPI_BASE to GLOBAL_DEFINES constant in makefile
 *    if platform required IPI base other than 0.
 */

/* set PLAT_GIC_IPI_BASE to non-secure interrupt base */
#if !defined(PLAT_GIC_IPI_BASE)
#define PLAT_GIC_IPI_BASE   0
#endif

#define MAX_IPI_NUM 15
enum {
    MTK_PLAT_MP_IPI_GENERIC = MP_IPI_GENERIC,
    MTK_PLAT_MP_IPI_RESCHEDULE = MP_IPI_RESCHEDULE,
    MTK_PLAT_MP_IPI_CUSTOM_START,
};

STATIC_ASSERT(MTK_PLAT_MP_IPI_CUSTOM_START <= MAX_IPI_NUM);

__WEAK const struct smp_cpu_info *mp_get_smp_cpu_info(void)
{
    PANIC_UNIMPLEMENTED;
    return NULL;
}

status_t plat_mp_send_ipi(mp_cpu_mask_t target, uint ipi)
{
    LTRACEF_LEVEL(2, "target 0x%x, ipi %u\n", target, ipi);

    uint gic_ipi_num = ipi + PLAT_GIC_IPI_BASE;

    /* filter out targets outside of the range of cpus we care about */
    target &= ((1UL << SMP_MAX_CPUS) - 1);
    if (target != 0) {
        LTRACEF_LEVEL(2, "target 0x%x, gic_ipi %u\n", target, gic_ipi_num);
        arm_gic_sgi(gic_ipi_num, ARM_GIC_SGI_FLAG_NS, target);
    }

    return NO_ERROR;
}

status_t arch_mp_send_ipi(mp_cpu_mask_t target, mp_ipi_t ipi)
{
    return plat_mp_send_ipi(target, ipi);
}

void arch_mp_init_percpu(void)
{
    register_int_handler(MTK_PLAT_MP_IPI_GENERIC + PLAT_GIC_IPI_BASE,
            &arm_ipi_generic_handler, 0);
    register_int_handler(MTK_PLAT_MP_IPI_RESCHEDULE + PLAT_GIC_IPI_BASE,
            &arm_ipi_reschedule_handler, 0);

    unmask_interrupt(MTK_PLAT_MP_IPI_GENERIC + PLAT_GIC_IPI_BASE);
    unmask_interrupt(MTK_PLAT_MP_IPI_RESCHEDULE + PLAT_GIC_IPI_BASE);
}

static int cpu_off_thread(void *arg)
{
    int ret;

    arch_disable_ints();

    THREAD_LOCK(state);
    mp_set_curr_cpu_active(false);
    THREAD_UNLOCK(state);

    /* executed by the to-be-off cpu */
    ret = psci_cpu_off(); /* should never return */
    DEBUG_ASSERT(ret && 0);

    return -1;
}

static void plat_mp_on(uint level)
{
    const struct smp_cpu_info *smp_cpu = mp_get_smp_cpu_info();
    uint32_t local_cpu = arch_curr_cpu_num();
    uint32_t cpu_on_mask;
    int i, ret;

    DEBUG_ASSERT(smp_cpu != NULL);

    cpu_on_mask = smp_cpu->cpu_on_mask & ~(1U << local_cpu);

    for (i = 0; i < SMP_MAX_CPUS; i++) {
        if (cpu_on_mask & (1 << i)) {
            ret = psci_cpu_on(smp_cpu->id[i], MEMBASE + KERNEL_LOAD_OFFSET, 0);
            LTRACEF("cpu=0x%x, ret=%d, entry=%x\n",
                    smp_cpu->id[i], ret, MEMBASE + KERNEL_LOAD_OFFSET);
        }
    }
}

void plat_mp_off(void)
{
    uint32_t local_cpu = arch_curr_cpu_num();
    mp_cpu_mask_t target = 0;
    thread_t *t;
    uint32_t cpu;

    for (cpu = 0; cpu < SMP_MAX_CPUS; cpu++) {
        if (mp_is_cpu_active(cpu) && (cpu != local_cpu)) {
            t = thread_create("cpu_off_thread", &cpu_off_thread, NULL,
                              DEFAULT_PRIORITY, DEFAULT_STACK_SIZE);
            thread_set_pinned_cpu(t, cpu);
            thread_detach_and_resume(t);
            target |= (1 << cpu);
        }
    }

    plat_mp_send_ipi(target, MTK_PLAT_MP_IPI_RESCHEDULE);
}

static int dummy_thread(void *arg)
{
    /* do nothing, just to let the boot cpu go away from the current thread */
    return 0;
}

void plat_mp_bootcpu_handover(uint32_t cpuid)
{
    uint32_t curr_cpu = arch_curr_cpu_num();
    thread_t *curr_thread;

    DEBUG_ASSERT(!arch_ints_disabled());

    if (curr_cpu == cpuid)
        return;

    if (!mp_is_cpu_active(cpuid)) {
        LTRACEF("handover cpu=%u is not active.\n", cpuid);
        return;
    }

    /* switch boot cpu to new one */
    curr_thread = get_current_thread();
    thread_set_pinned_cpu(curr_thread, cpuid);
    thread_t *dummy_t = thread_create("dummy_thread", &dummy_thread, NULL,
            DEFAULT_PRIORITY, DEFAULT_STACK_SIZE);
    thread_detach_and_resume(dummy_t);
    LTRACEF("new bootcpu=%u\n", arch_curr_cpu_num());
}

LK_INIT_HOOK(plat_mp_on, &plat_mp_on, LK_INIT_LEVEL_THREADING);
#endif

