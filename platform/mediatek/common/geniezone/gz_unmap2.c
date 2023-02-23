/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#include <assert.h>
#include <debug.h>
#ifdef WITH_PLATFORM_MEDIATEK_COMMON_EMI
#include <emi_mpu.h>
#endif
#include <hvc.h>
#include <kernel/vm.h>
#include <platform_mtk.h>
#include <smc_id_table.h>
#include <stdlib.h>
#include <sys/types.h>

#ifndef GZ_TAG
#define GZ_TAG "[GZ_UNMAP2]"
#endif

/* UNMAP2 Debugging */
#define UNMAP2_DEBUG     0
#define NO_DUMP          0
#define DUMP_EL1S2_LK    (1 << 0)
#define DUMP_EL1S2_LINUX (1 << 1)
#define DUMP_EL2S1_GZ    (1 << 2)

#if UNMAP2_DEBUG == 0
#define UNMAP2_DUMP_PAGETABLE 0
#else
#define UNMAP2_DUMP_PAGETABLE (DUMP_EL1S2_LK + DUMP_EL1S2_LINUX)
#endif

/* SEARCH in EMI MPU region info */
#define SEARCH_IN_EMIMPU      1

#if UNMAP2_DEBUG == 1
static void show_unmap_region_info(void)
{
    unsigned int count = check_ns_cpu_perm();
    struct unmap_region *p = get_region_info();

    dprintf(INFO, " -=#( Hello GZ UNMAP2 - Region Info)#=-\n");
    dprintf(INFO, " -------------------------\n");
    for (unsigned int i = 0; i < count; i++, p++) {
        dprintf(INFO, " [SR:%02u][enable:%u][%llx][%llx]\n",
                p->region, p->enable, p->start, p->end);
    }
    dprintf(INFO, " Total num of region: %d\n", count);
    dprintf(INFO, " -------------------------\n");
}
#endif

/*
 * Parameters definition (__hvc_conduit)
 *
 * function id: SMC ID
 * arg0: struct unmap_region -> count -> reserved_num
 * arg1: struct unmap_region -> struct unmap_region unmap_region_info[]
 * arg2: ROUNDDOWN 4k align start address of arg1
 * arg3: size of unmap_region_info[]
 * arg4: ROUNDUP 4k align size of unmap_region_info[]
 *
 * arg5: MTK_STD_LK_EMIMPU_REGION_INFO : 0(No Dump), 1(Dump PageTable)
 *       MTK_STD_LK_SECURE_REGION_QUERY: Query Address
 *
 * arg6: 0(SEARCH_IN_MBLOCK), 1(SEARCH_IN_EMIMPU)
 *
 * res:  res.a0 as the result. 0: success
 */

void gz_unmap2(void)
{
#ifdef WITH_PLATFORM_MEDIATEK_COMMON_EMI
    struct __smccc_res res;

#if UNMAP2_DEBUG == 1
    show_unmap_region_info();
#endif

    /* check unmap_region_info count */
    unsigned int count = check_ns_cpu_perm();

    if (count == 0) {
        dprintf(INFO, "%s: unmap_region is not ready yet!\n", GZ_TAG);
        return;
    }

    /* HVC to do unmap */
    ulong reserved_num = (ulong)count;

    ulong reserved_t_pa_addr =
        (ulong)vaddr_to_paddr((void *)get_region_info());

    ulong reserved_t_size =
        (ulong)(sizeof(struct unmap_region) * reserved_num);

    dprintf(INFO, "%s: HVC unmap_region_info count=%lu, addr=0x%lx(0x%lx), size=0x%lx(0x%lx)\n",
                  GZ_TAG, reserved_num,
                  reserved_t_pa_addr, ROUNDDOWN(reserved_t_pa_addr, PAGE_SIZE),
                  reserved_t_size, ROUNDUP(reserved_t_size, PAGE_SIZE));

    __hvc_conduit(MTK_STD_LK_EMIMPU_REGION_INFO,
                  reserved_num,
                  reserved_t_pa_addr,
                  ROUNDDOWN(reserved_t_pa_addr, PAGE_SIZE),
                  reserved_t_size,
                  ROUNDUP(reserved_t_size, PAGE_SIZE),
                  UNMAP2_DUMP_PAGETABLE, 0, &res);

    if (res.a0) {
        dprintf(CRITICAL, "%s: unmap failed! (%lx, %lx, %lx, %lx)\n",
                GZ_TAG, res.a0, res.a1, res.a2, res.a3);
        dprintf(CRITICAL, "%s: PIC: owners related to SR(%u) -- not 4k align or overlay\n",
                GZ_TAG, (unsigned int)res.a2);
        assert(0);
    } else {
        dprintf(INFO, "%s: done!\n", GZ_TAG);
    }

    /* Check violation and clear */
    paddr_t addr = check_violation_for_unmap();

    if (addr) {
        __hvc_conduit(MTK_STD_LK_SECURE_REGION_QUERY,
                      reserved_num,
                      reserved_t_pa_addr,
                      ROUNDDOWN(reserved_t_pa_addr, PAGE_SIZE),
                      reserved_t_size,
                      ROUNDUP(reserved_t_size, PAGE_SIZE),
                      (ulong)addr, SEARCH_IN_EMIMPU, &res);

        if ((res.a0 == 0) && (res.a1 == 1)) {
            dprintf(INFO, "%s: clear (0x%lx) violation\n", GZ_TAG, (ulong)addr);
            clear_violation();
        } else {
            dprintf(CRITICAL, "%s: no volation in secure region! (0x%lx)(%lx, %lx, %lx, %lx)\n",
                    GZ_TAG, (ulong)addr, res.a0, res.a1, res.a2, res.a3);
        }
    }
#else
    dprintf(INFO, "%s: Not supported w/o WITH_PLATFORM_MEDIATEK_COMMON_EMI\n", GZ_TAG);
#endif
}
