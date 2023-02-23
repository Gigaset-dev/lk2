/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#include <assert.h>
#include <debug.h>
#include <libfdt.h>
#include <lib/kcmdline.h>
#include <mblock.h>
#ifdef WITH_PLATFORM_MEDIATEK_COMMON_RNG
#include <rng.h>
#endif
#include <set_fdt.h>
#include <trace.h>
#define LOCAL_TRACE 0

/*
 * USER :      LK_DEBUGLEVEL = 0
 * USERDEBUG : LK_DEBUGLEVEL = 1
 * ENG :       LK_DEBUGLEVEL = 2
 */

static bool stack_depot_enable;

static void kernel_mm_slub_debug_cmd_init(void *fdt, int chosen_offset)
{
    const void *status;
    int status_len;

    status = fdt_getprop(fdt, chosen_offset,
        "slub_debug-status", &status_len);
    if (status && !strcmp(status, "okay"))
        kcmdline_append("slub_debug=OFZPU");

}

static void kernel_hwtag_kasan_cmd_init(void *fdt, int chosen_offset)
{
    const void *status;
    int status_len;

    status = fdt_getprop(fdt, chosen_offset,
        "hwtag_kasan", &status_len);
    if (status) {
        if (!strcmp(status, "sync")) {
            stack_depot_enable = true;
            kcmdline_append("kasan.mode=sync");
            kcmdline_append("kasan.stacktrace=on");
        } else if (!strcmp(status, "async")) {
            kcmdline_append("kasan.mode=async");
            kcmdline_append("kasan.stacktrace=off");
        } else if (!strcmp(status, "off")) {
            kcmdline_append("kasan.mode=off");
        }
    }
}

static void kernel_mm_page_owner_cmd_init(void *fdt, int chosen_offset)
{
    const void *status;
    int status_len;

    status = fdt_getprop(fdt, chosen_offset,
        "page_owner-status", &status_len);
    if (status && !strcmp(status, "okay")) {
        stack_depot_enable = true;
        kcmdline_append("page_owner=on");
    }

}

static void kernel_mm_cmd_init(void *fdt, int chosen_offset)
{

#if LK_DEBUGLEVEL > 0
   /* init panic_on_taint for BAD page case */
    kcmdline_append("panic_on_taint=20");

    /* add kasan backtrace support */
    stack_depot_enable = true;
#endif

    /* init slub_debug cmdline */
    kernel_mm_slub_debug_cmd_init(fdt, chosen_offset);

    /* init page_owner cmdline */
    kernel_mm_page_owner_cmd_init(fdt, chosen_offset);
}

static void kernel_kaslr_seed_init(void *fdt, int chosen_offset)
{
    const void *status;
    int status_len;
    u32 seed[2] = {0};
    int ret;

    status = fdt_getprop(fdt, chosen_offset,
        "kaslr-status", &status_len);
    if (status && !strcmp(status, "disabled")) {
        kcmdline_append("nokaslr");
        return;
    }

    status = fdt_getprop(fdt, chosen_offset, "kaslr-seed", &status_len);
    /* get random kaslr-seed if it is defined in the dtb */
    if (status) {
        if (status_len != sizeof(u64)) {
            LTRACEF_LEVEL(ALWAYS, "incorrect kaslr-seed length=%d\n",
                status_len);
            ASSERT(0);
            return;
        }

#ifdef WITH_PLATFORM_MEDIATEK_COMMON_RNG
        get_rnd(&seed[0]);
        get_rnd(&seed[1]);
#else
        LTRACEF_LEVEL(ALWAYS, "rng is not supported, so seed is null\n");
#endif

        ret = fdt_setprop(fdt, chosen_offset, "kaslr-seed", seed, status_len);
        if (ret) {
            ASSERT(0);
            return;
        }
    }
}

static void kernel_mte_cmd_init(void *fdt, int chosen_offset)
{
    const struct reserved_t *mte_mblock;
    const void *status;
    int status_len;
    bool mte_suppress;

    mte_mblock = mblock_query_reserved_by_name("security_booker_tag", 0);
    if (!mte_mblock) {
        kcmdline_append("arm64.nomte");
        return;
    }

    /* Default:
     *   user/userdebug: by project option: MTK_MTE_CMD_SUPPRESS
     *   eng: enabled
     */
    #if (LK_DEBUGLEVEL < 2) && (MTK_MTE_CMD_SUPPRESS == 1)
        mte_suppress = true;
    #else
        mte_suppress = false;
    #endif

    /* For debug, modify if DoE is configured */
    status = fdt_getprop(fdt, chosen_offset,
        "mte-status", &status_len);
    if (status) {
        if (!strcmp(status, "on"))
            mte_suppress = false;
        else if (!strcmp(status, "off"))
            mte_suppress = true;
    }
    if (mte_suppress)
        kcmdline_append("arm64.nomte");

    return;
}

static void kernel_debug_cmd_init(void *fdt)
{
    const void *kmemleak_status;
    int status_len, offset;

    offset = fdt_path_offset(fdt, "/chosen");
    if (offset < 0) {
        LTRACEF_LEVEL(ALWAYS, "failed to get fdt offset\n");
        return;
    }

    /* init kmemleak cmdline */
    kmemleak_status = fdt_getprop(fdt, offset,
                                        "kmemleak-status", &status_len);
    if (kmemleak_status && !strcmp(kmemleak_status, "okay"))
            kcmdline_append("kmemleak=on");

    /* init kernel mm debug  cmdline */
    kernel_mm_cmd_init(fdt, offset);

    /* init kaslr seed */
    kernel_kaslr_seed_init(fdt, offset);

    /* init hwtag kasan cmdline */
    kernel_hwtag_kasan_cmd_init(fdt, offset);

    /* init stack_depot cmdline */
    if (stack_depot_enable)
        kcmdline_append("stack_depot_disable=off");

    /* init MTE cmdline */
    kernel_mte_cmd_init(fdt, offset);
}
SET_FDT_INIT_HOOK(kernel_debug_cmd_init, kernel_debug_cmd_init);

