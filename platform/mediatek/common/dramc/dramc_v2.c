/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#include <assert.h>
#include <debug.h>
#include <dramc_common_v2.h>
#include <libfdt.h>
#include <lib/pl_boottags.h>
#include <platform/mboot_expdb.h>
#include <platform/mboot_params.h>
#include <set_fdt.h>
#include <trace.h>

#define LOCAL_TRACE   2

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
    int ret = 0;
    int offset;
    unsigned int dram_type;
    unsigned int support_ch_cnt;
    unsigned int ch_cnt;
    unsigned int rk_cnt;
    unsigned int mr_cnt;
    unsigned int freq_cnt;
    unsigned int rk_size[DRAMC_MAX_RK];
    unsigned int mr_info[DRAMC_MR_CNT * 2];
    unsigned int freq_step[DRAMC_FREQ_CNT];

    if (!fdt)
        return;

    offset = fdt_path_offset(fdt, "/dramc");
    if (offset < 0) {
        offset = fdt_path_offset(fdt, "/soc/dramc");
        if (offset < 0) {
            dprintf(CRITICAL, "[%s] get_path_offset fail\n", __func__);
            return;
        }
    }

    // prepare fdt data
    dram_type = cpu_to_fdt32((unsigned int)(emi_info_arg.dram_type));
    support_ch_cnt = cpu_to_fdt32((unsigned int)(emi_info_arg.support_ch_cnt));
    ch_cnt = cpu_to_fdt32((unsigned int)(emi_info_arg.ch_cnt));
    rk_cnt = cpu_to_fdt32((unsigned int)(emi_info_arg.rk_cnt));
    mr_cnt = cpu_to_fdt32((unsigned int)(emi_info_arg.mr_cnt));
    freq_cnt = cpu_to_fdt32((unsigned int)(emi_info_arg.freq_cnt));

    LTRACEF("offset = %d\n", offset);
    LTRACEF("dram_type original data = %d\n", (unsigned int)(emi_info_arg.dram_type));

    /* unit of rank size: 1Gb (128MB) */
    for (unsigned int idx = 0; idx < DRAMC_MAX_RK; ++idx) {
        if (idx < rk_cnt)
            rk_size[idx] = cpu_to_fdt32((emi_info_arg.rk_size[idx] >> 27) & 0xFFFFFFFF);
        else
            rk_size[idx] = cpu_to_fdt32(0);
    }

    for (unsigned int idx = 0; idx < DRAMC_MR_CNT; ++idx) {
        int fdt_idx = idx * 2;
        int fdt_val = fdt_idx + 1;

        if (idx < mr_cnt) {
            mr_info[fdt_idx] = cpu_to_fdt32(
                    (unsigned int)(emi_info_arg.mr_info[idx].mr_index));
            mr_info[fdt_val] = cpu_to_fdt32(
                    (unsigned int)(emi_info_arg.mr_info[idx].mr_value));
        } else {
            mr_info[fdt_idx] = cpu_to_fdt32(0);
            mr_info[fdt_val] = cpu_to_fdt32(0);
        }
    }

    for (unsigned int idx = 0; idx < DRAMC_FREQ_CNT; ++idx) {
        if (idx < freq_cnt)
            freq_step[idx] = cpu_to_fdt32((unsigned int)(emi_info_arg.freq_step[idx]));
        else
            freq_step[idx] = cpu_to_fdt32(0);
    }

    /* pass parameter to kernel */
    ret = fdt_setprop(fdt, offset, "dram_type", &dram_type, sizeof(dram_type));
    if (ret < 0)
        ASSERT(0);

    ret = fdt_setprop(fdt, offset, "support_ch_cnt", &support_ch_cnt, sizeof(support_ch_cnt));
    if (ret < 0)
        ASSERT(0);

    ret = fdt_setprop(fdt, offset, "ch_cnt", &ch_cnt, sizeof(ch_cnt));
    if (ret < 0)
        ASSERT(0);

    ret = fdt_setprop(fdt, offset, "rk_cnt", &rk_cnt, sizeof(rk_cnt));
    if (ret < 0)
        ASSERT(0);

    ret = fdt_setprop(fdt, offset, "mr_cnt", &mr_cnt, sizeof(mr_cnt));
    if (ret < 0)
        ASSERT(0);

    ret = fdt_setprop(fdt, offset, "freq_cnt", &freq_cnt, sizeof(freq_cnt));
    if (ret < 0)
        ASSERT(0);

    ret = fdt_setprop(fdt, offset, "rk_size", &rk_size, sizeof(rk_size));
    if (ret < 0)
        ASSERT(0);

    ret = fdt_setprop(fdt, offset, "mr", &mr_info, sizeof(mr_info));
    if (ret < 0)
        ASSERT(0);

    ret = fdt_setprop(fdt, offset, "freq_step", &freq_step, sizeof(freq_step));
    if (ret < 0)
        ASSERT(0);
}
SET_FDT_INIT_HOOK(set_fdt_dramc, set_fdt_dramc);

void get_emi_info(struct emi_info *info)
{
    if (info && !(info > &emi_info_arg - 1 && info < &emi_info_arg + 1))
        memcpy(info, &emi_info_arg, sizeof(struct emi_info));
}
