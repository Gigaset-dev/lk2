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
#include <emi_lastslc.h>
#include <kernel/vm.h>
#include <libfdt.h>
#include <platform/emi.h>
#include <platform/mboot_expdb.h>
#include <reg.h>

#include "emi_util.h"

static struct emi_lastslc_header {
    unsigned int lastslc_magic;
    unsigned int lastslc_version;
    unsigned int total_latch_count;
    unsigned int nr_cen_emi;
    unsigned int max_result_count_slc;
    unsigned int lastslc_magic_l;
};

static struct emi_lastslc_base {
    unsigned long slc_base;
};

static struct emi_lastslc_data {
    struct emi_lastslc_header header;
    struct emi_lastslc_base *base;
};

static struct emi_info emi_info_arg;
static struct emi_lastslc_data *lastslc_data;

int emi_lastslc_init(void *fdt)
{
    unsigned int cen_num = 0;
    unsigned int *emicen_data;
    int offset, len;
    unsigned int array_len;
    unsigned int i;
    struct emi_lastslc_data *info;

    EMI_DBG("%s: lastslc init start\n", __func__);

    if (!fdt) {
        EMI_DBG("%s: couldn't find property lastslc information\n", __func__);
        return -1;
    }

    get_emi_info(&emi_info_arg);

    assert(emi_info_arg.magic_tail == 0x20190831);

    info = (struct emi_lastslc_data *)malloc(sizeof(*info));
    if (!info)
        goto fail;

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

    cen_num = len / sizeof(unsigned int) / 4;

    /* info */
    info->base = (struct emi_lastslc_base *)malloc(sizeof(struct emi_lastslc_base) * cen_num);
    if (info->base == NULL) {
        EMI_DBG("%s:  malloc base fail\n", __func__);
        goto fail;
    }

    /* Get base address */
    info->base[0].slc_base = SLC_PMU_REG_BASE + KERNEL_ASPACE_BASE;
    info->base[1].slc_base = SUB_SLC_PMU_REG_BASE + KERNEL_ASPACE_BASE;

    /* Set initial data in header */
    info->header.lastslc_magic = 0x12345678;
    info->header.lastslc_version = 1;
    info->header.nr_cen_emi = cen_num;
    info->header.total_latch_count = SLC_TOTAL_LATCH_COUNT;
    info->header.max_result_count_slc = SLC_MAX_RESULT_COUNT;
    info->header.lastslc_magic_l = 0x12345678;

    lastslc_data = info;
    EMI_DBG("%s: lastslc init end\n", __func__);
    return 0;

fail:
    if (info) {
        if (info->base)
            free(info->base);
        free(info);
    }

    return -1;
}

static int emi_lastslc_dump(char *buf, int *wp)
{
    unsigned int i, j, k;
    unsigned int val;
    unsigned long slc_parity_addr;

    EMI_DBG("%s: lastslc dump start\n", __func__);
    if (buf == NULL || wp == NULL)
        return -1;
    if (!lastslc_data)
        return -1;
    if (*wp + sizeof(struct emi_lastslc_header) >= LATCH_LASTSLC_LENGTH)
        return -1;
    memcpy(buf + *wp, &lastslc_data->header, sizeof(struct emi_lastslc_header));
    *wp += sizeof(struct emi_lastslc_header);

    /*Latch start*/
    for (i = 0; i < SLC_TOTAL_LATCH_COUNT; i++) {
        unsigned long slc_ctrl_addr[lastslc_data->header.nr_cen_emi];
        unsigned long slc_dbg_ch0_addr;
        unsigned long slc_dbg_ch1_addr;

        for (j = 0; j < lastslc_data->header.nr_cen_emi; j++) {
            slc_ctrl_addr[j] = lastslc_data->base[j].slc_base + SLC_CTRL_OFFSET;

            /* Write SLC_CTRL to select SLC_DBG_MUX[10:9] */
            val = readl(slc_ctrl_addr[j]);
            val &= ~(0x600);
            val |= i << 9;
            writel(val, slc_ctrl_addr[j]);
            val = readl(slc_ctrl_addr[j]);

            if ((*wp + sizeof(val)) >= LATCH_LASTSLC_LENGTH)
                return -1;
            memcpy(buf + *wp, &val,  sizeof(val));
            *wp += sizeof(val);

            /* Latch debug value */
            for (k = 0; k < SLC_MAX_RESULT_COUNT; k++) {
                /* Set DBG_MUX[8:3] */
                val = readl(slc_ctrl_addr[j]);
                val &= ~(0x1f8);
                val |= k << 3;
                writel(val, slc_ctrl_addr[j]);

                slc_dbg_ch0_addr = lastslc_data->base[j].slc_base + SLC_MUX_RESULT_OFFSET1;
                slc_dbg_ch1_addr = lastslc_data->base[j].slc_base + SLC_MUX_RESULT_OFFSET2;

                /* Latch value */
                val = readl(slc_dbg_ch0_addr);
                if ((*wp + sizeof(val)) >= LATCH_LASTSLC_LENGTH)
                    return -1;
                memcpy(buf + *wp, &val,  sizeof(val));
                *wp += sizeof(val);
                #if LACTH_LOG_ENABLE
                EMI_DBG("TEST %s: Latch%d: cen_num:%d count:%d lastslc mux_result1: 0x%x\n",
                        __func__, i+1, j, k, val);
                #endif

                val = readl(slc_dbg_ch1_addr);
                if ((*wp + sizeof(val)) >= LATCH_LASTSLC_LENGTH)
                    return -1;
                memcpy(buf + *wp, &val,  sizeof(val));
                *wp += sizeof(val);
                #if LACTH_LOG_ENABLE
                EMI_DBG("TEST %s: Latch%d: cen_num:%d count:%d lastslc mux_result2: 0x%x\n",
                        __func__, i+1, j, k, val);
                #endif
            }
        }
    }
    EMI_DBG("%s: lastslc dump end\n", __func__);

    EMI_DBG("%s: slc parity dump start\n", __func__);
    for (j = 0; j < lastslc_data->header.nr_cen_emi; j++) {
        slc_parity_addr = lastslc_data->base[j].slc_base + SLC_SRAM_PARITY_ERR;

        val = readl(slc_parity_addr);
        if ((*wp + sizeof(val)) >= LATCH_LASTSLC_LENGTH)
            return -1;
        memcpy(buf + *wp, &val,  sizeof(val));
        *wp += sizeof(val);

        slc_parity_addr = lastslc_data->base[j].slc_base + SLC_ERROR_STATUS;
        for (k = 0; k <= SLC_ERROR_STATUS_LENGTH; k += 4) {
            val = readl(slc_parity_addr + k);
            if ((*wp + sizeof(val)) >= LATCH_LASTSLC_LENGTH)
                return -1;
            memcpy(buf + *wp, &val,  sizeof(val));
            *wp += sizeof(val);
        }
    }
    EMI_DBG("%s: slc parity dump end\n", __func__);
    return 0;
}


static int emi_lastslc_get(void **data, int *len)
{
    int ret;
    *len = 0;
    *data = malloc(LATCH_LASTSLC_LENGTH);
    if (*data == NULL)
        return 0;

    ret = emi_lastslc_dump(*data, len);
    if (ret < 0 || *len > LATCH_LASTSLC_LENGTH) {
        *len = MIN(*len, LATCH_LASTSLC_LENGTH);
        return ret;
    }
    return 1;
}

static void emi_lastslc_put(void **data)
{
    free(*data);
}

static void save_emi_lastslc_data(CALLBACK dev_write)
{
    char *buf = NULL;
    int ret;
    int len = 0;
    bool result = false;
    /* Save latch buffer */
    ret = emi_lastslc_get((void **)&buf, &len);
    if (ret && (buf != NULL)) {
        if (len > 0)
            result = dev_write(vaddr_to_paddr(buf), len);
    }
    if (!result)
        EMI_DBG("%s: lastslc dump fail\n", __func__);
    if (buf != NULL)
        emi_lastslc_put((void **)&buf);

    EMI_DBG("%s: lastslc dump end\n", __func__);
}

AEE_EXPDB_INIT_HOOK(SYS_LASTSLC_RAW, LATCH_LASTSLC_LENGTH, save_emi_lastslc_data);
