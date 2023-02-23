/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#include <assert.h>
#include <bits.h>
#include <debug.h>
#include <dramc_common_v2.h>
#include <emi_lastemi_v1.h>
#include <kernel/vm.h>
#include <libfdt.h>
#include <platform/boot_mode.h>
#include <platform/mboot_expdb.h>
#include <reg.h>

#include "emi_util.h"

static struct emi_info emi_info_arg;
static struct emi_lastemi_data *lastemi_data;

__WEAK int emi_get_lastemi_data_without_dts(struct emi_lastemi_data *data)
{
    return 0;
}

int emi_lastemi_init(void *fdt)
{
    unsigned int dram_type;
    unsigned int cen_num = 0;
    unsigned int *emicen_data, *emichn_data;
    int offset, len;
    unsigned int array_len;
    unsigned int i;
    struct emi_lastemi_data *info;

    get_emi_info(&emi_info_arg);

    assert(emi_info_arg.magic_tail == 0x20190831);

    info = (struct emi_lastemi_data *)malloc(sizeof(*info));
    if (!info)
        return -1;

    if (fdt) {
        /* Get emicen info */
        array_len = countof(emicen_compatible);
        for (i = 0; i < array_len; i++) {
            offset = fdt_path_offset(fdt, emicen_compatible[i]);
            if (offset >= 0)
                break;
        }

        if (offset < 0) {
            EMI_DBG("%s: couldn't find emicen node: %d\n", __func__, offset);
            goto fail;
        }

        emicen_data = (unsigned int *)fdt_getprop(fdt, offset, "reg", &len);
        if (!emicen_data || !len) {
            EMI_DBG("%s: couldn't find emicen property reg\n", __func__);
            goto fail;
        }

        cen_num = len/sizeof(unsigned int)/4;

        info->base = (struct base_addesss *)malloc(sizeof(struct base_addesss) * cen_num);
        if (info->base == NULL) {
            EMI_DBG("%s:  malloc base fail\n", __func__);
            goto fail;
        }

        /* Get emichn info */
        array_len = countof(emichn_compatible);
        for (i = 0; i < array_len; i++) {
            offset = fdt_path_offset(fdt, emichn_compatible[i]);
            if (offset >= 0)
                break;
        }

        if (offset < 0) {
            EMI_DBG("%s: couldn't find emichn node: %d\n", __func__, offset);
            goto fail;
        }

        emichn_data = (unsigned int *)fdt_getprop(fdt, offset, "reg", &len);
        if (!emichn_data || !len) {
            EMI_DBG("%s: couldn't find emichn property reg\n", __func__);
            goto fail;
        }
        info->chn_num = (unsigned int)(cpu_to_fdt32(emi_info_arg.ch_cnt) >> 24);

        /* Get base address */
        emicen_data++;
        emichn_data++;
        for (i = 0; i < cen_num; i++) {
            info->base[i].cen_base = fdt32_to_cpu(*emicen_data) + KERNEL_ASPACE_BASE;
            info->base[i].chn_base[0] = fdt32_to_cpu(*emichn_data) + KERNEL_ASPACE_BASE;//CH0 CH2
            emichn_data += 4;
            info->base[i].chn_base[1] = fdt32_to_cpu(*emichn_data) + KERNEL_ASPACE_BASE;//CH1 CH3
            emichn_data += 4;
            emicen_data += 4;
        }
    } else {
        if (!emi_get_lastemi_data_without_dts(info)) {
            EMI_DBG("%s: couldn't find property lastemi information\n", __func__);
            goto fail;
        }
    }

    /* Get dram type */
    /* dram_type is mapping to DRAM_DRAM_TYPE_T
     * typedef enum
     * {
     *   TYPE_DDR1 = 1,
     *   TYPE_LPDDR2,
     *   TYPE_LPDDR3,
     *   TYPE_PCDDR3,
     *   TYPE_LPDDR4,
     *   TYPE_LPDDR4X,
     *   TYPE_LPDDR4P,
     *   TYPE_LPDDR5
     * } DRAM_DRAM_TYPE_T;
     */
    dram_type = (unsigned int)(cpu_to_fdt32(emi_info_arg.dram_type) >> 24);

    /* Set initial data in header */
    info->header.lastemi_magic = 0x12345678;
    info->header.lastemi_version = 1;
    info->header.nr_cen_emi = cen_num;
    info->header.total_latch_count = TOTAL_LATCH_COUNT;
    info->header.max_result_count_cen_emi = MAX_RESULT_COUNT_CEN;
    info->header.max_result_count_chn_emi = MAX_RESULT_COUNT_CHN;
    info->header.lastemi_dram_type = dram_type;
    info->header.lastemi_magic_l = 0x12345678;

    lastemi_data = info;

    return 0;

fail:
    free(info);
    return -1;
}

static int emi_lastemi_dump(char *buf, int *wp)
{
    unsigned int i, j, k, n, m;
    unsigned int val;

    EMI_DBG("%s: lastemi dump start\n", __func__);
    if (buf == NULL || wp == NULL)
        return -1;

    if (!lastemi_data)
        return -1;

    if (*wp + sizeof(struct emi_lastemi_header) >= LATCH_LASTEMI_LENGTH)
        return -1;

    memcpy(buf + *wp, &lastemi_data->header, sizeof(struct emi_lastemi_header));
    *wp += sizeof(struct emi_lastemi_header);

    for (i = 0; i < TOTAL_LATCH_COUNT; i++) {
        for (j = 0; j < lastemi_data->header.nr_cen_emi; j++) {
            /* CEN EMI dump */
            for (k = 0; k < MAX_RESULT_COUNT_CEN; k++) {
                val = readl(lastemi_data->base[j].cen_base + CTRL_OFFSET);
                val &= 0x7;
                val |= ((k  + i * MAX_RESULT_COUNT_CEN) << 3);
                writel(val, lastemi_data->base[j].cen_base + CTRL_OFFSET);
                val = readl(lastemi_data->base[j].cen_base + RESULT_OFFSET);
                if ((*wp + sizeof(val)) >= LATCH_LASTEMI_LENGTH)
                    return -1;
                memcpy(buf + *wp, &val,  sizeof(val));
                *wp += sizeof(val);
                #if LACTH_LOG_ENABLE
                EMI_DBG("TEST %s (%d  + %d * %d) + 0x10000000 * %d = 0x%x\n",
                        __func__, k, i, MAX_RESULT_COUNT_CEN, j, val);
                #endif
            }
            /* CHN EMI dump for each CEN EMI */
            for (m = 0; m < 2; m++) {
                EMI_DBG("TEST chn 0x%lx\n", lastemi_data->base[j].chn_base[m]);
                for (n = 0; n < MAX_RESULT_COUNT_CHN; n++) {
                    val = readl(lastemi_data->base[j].chn_base[m] + CTRL_OFFSET);
                    val &= 0x7;
                    val |= ((n + i * MAX_RESULT_COUNT_CHN) << 3);
                    writel(val, lastemi_data->base[j].chn_base[m] + CTRL_OFFSET);
                    val = readl(lastemi_data->base[j].chn_base[m] + RESULT_OFFSET);
                    if ((*wp + sizeof(val)) >= LATCH_LASTEMI_LENGTH)
                        return -1;
                    memcpy(buf + *wp, &val,  sizeof(val));
                    *wp += sizeof(val);
                    #if LACTH_LOG_ENABLE
                    EMI_DBG("TEST %s (%d + %d * %d) + 0x10000 * %d + 0x10000000 * %d = 0x%x\n",
                            __func__, n, i, MAX_RESULT_COUNT_CHN, m, j, val);
                    #endif
                }
            }
        }
    }

    return 0;
}

int emi_lastemi_config(int enable)
{
    unsigned int i, j;
    unsigned int val;

    if (!lastemi_data)
        return -1;

    if (enable) {
        for (i = 0; i < lastemi_data->header.nr_cen_emi; i++) {
            val = readl(lastemi_data->base[i].cen_base + CTRL_OFFSET);
            val |= (1 << 1);
            writel(val, lastemi_data->base[i].cen_base + CTRL_OFFSET);
            EMI_DBG("TEST %s enable:%d cen_base:%d rg:0x%lx\n",
                    __func__, enable, i, lastemi_data->base[i].cen_base + CTRL_OFFSET);
            for (j = 0; j < 2; j++) {
                val = readl(lastemi_data->base[i].chn_base[j] + CTRL_OFFSET);
                val |= (1 << 1);
                writel(val, lastemi_data->base[i].chn_base[j] + CTRL_OFFSET);
                EMI_DBG("TEST %s enable:%d chn_base:%d rg:0x%lx\n",
                        __func__, enable, j, lastemi_data->base[i].chn_base[j] + CTRL_OFFSET);
            }
        }
    } else {
        for (i = 0; i < lastemi_data->header.nr_cen_emi; i++) {
            val = readl(lastemi_data->base[i].cen_base + CTRL_OFFSET);
            bitmap_clear(&val, 1);
            writel(val, lastemi_data->base[i].cen_base + CTRL_OFFSET);
            EMI_DBG("TEST %s disable:%d cen_base:%d rg:0x%lx\n", __func__,
                    enable, i, lastemi_data->base[i].cen_base + CTRL_OFFSET);

            for (j = 0; j < 2; j++) {
                val = readl(lastemi_data->base[i].chn_base[j] + CTRL_OFFSET);
                bitmap_clear(&val, 1);
                writel(val, lastemi_data->base[i].chn_base[j] + CTRL_OFFSET);
                EMI_DBG("TEST %s disable:%d chn_base:%d rg:0x%lx\n",
                        __func__, enable, j, lastemi_data->base[i].chn_base[j] + CTRL_OFFSET);
            }
        }
    }
    EMI_DBG("%s: lastemi dump end\n", __func__);
    return 0;
}

int emi_lastemi_get(void **data, int *len)
{
    int ret;
    *len = 0;
    *data = malloc(LATCH_LASTEMI_LENGTH);
    if (*data == NULL)
        return 0;

    ret = emi_lastemi_config(1);
    if (ret)
        return 0;

    ret = emi_lastemi_dump(*data, len);

    if (ret < 0 || *len > LATCH_LASTEMI_LENGTH) {
        *len = MIN(*len, LATCH_LASTEMI_LENGTH);
        return ret;
    }

    return 1;
}

void emi_lastemi_put(void **data)
{
    free(*data);
}

static void save_emi_lastemi_data(CALLBACK dev_write)
{
    char *buf = NULL;
    int ret;
    int len = 0;
    bool result = false;
    /* Save latch buffer */
    ret = emi_lastemi_get((void **)&buf, &len);
    if (ret && (buf != NULL)) {
        if (len > 0)
            result = dev_write(vaddr_to_paddr(buf), len);
    }
    if (!result)
        EMI_DBG("%s: lastemi dump fail\n", __func__);
    if (buf != NULL)
        emi_lastemi_put((void **)&buf);

    EMI_DBG("%s: lastemi dump end\n", __func__);
}

AEE_EXPDB_INIT_HOOK(SYS_LASTEMI_RAW, LATCH_LASTEMI_LENGTH, save_emi_lastemi_data);
