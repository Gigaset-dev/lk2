/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#include <assert.h>
#include <debug.h>
#include <dramc_common_v1.h>
#include <libfdt.h>
#include <lib/pl_boottags.h>
#include <platform/mboot_expdb.h>
#include <platform/mboot_params.h>
#include <set_fdt.h>
#include <trace.h>

static struct emi_info emi_info_arg;

static void pl_boottags_emi_info_hook(struct boot_tag *tag)
{
    if (tag->hdr.size - sizeof(tag->hdr) < sizeof(emi_info_arg)) {
        LTRACEF("%s Detect possible overrun access!\n", __func__);
        ASSERT(0);
    }
    memcpy(&emi_info_arg, &tag->data, sizeof(emi_info_arg));
}
PL_BOOTTAGS_INIT_HOOK(boot_tag_emi_info, BOOT_TAG_EMI_INFO, pl_boottags_emi_info_hook);

static void set_fdt_dramc(void *fdt)
{
    return;
}
SET_FDT_INIT_HOOK(set_fdt_dramc, set_fdt_dramc);

void get_emi_info(struct emi_info *info)
{
    if (info && !(info > &emi_info_arg - 1 && info < &emi_info_arg + 1))
        memcpy(info, &emi_info_arg, sizeof(struct emi_info));
}
