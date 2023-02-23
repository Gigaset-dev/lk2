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

#include <debug.h>
#include <err.h>
#include <lib/pl_boottags.h>
#include <libfdt.h>
#include <mtk_charger.h>
#include <pmic_dlpt.h>
#include <set_fdt.h>
#include <trace.h>

#define LOCAL_TRACE     0

#ifndef mdelay
#define mdelay(x)       spin((x) * 1000)
#endif

#define BOOT_TAG_DLPT_INFO          0x8861002A

#define DUMMY_LOAD_DELAY_MS         8
#define CHARGER_DISABLE_DELAY_MS    50

#define VBAT_DIFF_MAX       2000
#define VBAT_DIFF_MIN       40
#define IBAT_DIFF_MAX       2000
#define IBAT_DIFF_MIN       700
#define RAC_MIN             30

struct boot_tag_dlpt_info {
    unsigned int imix_r;
};

static struct boot_tag_dlpt_info dlpt_info = {
    .imix_r = 90,
};
PL_BOOTTAGS_TO_BE_UPDATED(dlpt_info, BOOT_TAG_DLPT_INFO, &dlpt_info);

static const struct pmic_dlpt_dev *dlpt_dev;

static void pl_boottags_dlpt_info_hook(struct boot_tag *tag)
{
    memcpy(&dlpt_info, &tag->data, sizeof(struct boot_tag_dlpt_info));
}
PL_BOOTTAGS_INIT_HOOK(dlpt_info, BOOT_TAG_DLPT_INFO, pl_boottags_dlpt_info_hook);

static void set_fdt_imix_r(void *fdt)
{
    int offset;

    offset = fdt_node_offset_by_compatible(fdt, -1, "mediatek,pmic-auxadc");
    if (offset < 0) {
        dprintf(CRITICAL, "pmic-auxadc node not found, ret=%d\n", offset);
        return;
    }
    offset = fdt_subnode_offset(fdt, offset, "imix_r");
    if (offset < 0) {
        dprintf(CRITICAL, "imix_r node not found, ret=%d\n", offset);
        return;
    }
    fdt_setprop_u32(fdt, offset, "val", dlpt_info.imix_r);
}
SET_FDT_INIT_HOOK(set_fdt_imix_r, set_fdt_imix_r);

static int get_rac_val(void)
{
    u32 volt_1 = 0, volt_2 = 0, curr_1 = 0, curr_2 = 0;
    int rac_val;
    bool retry_state = false;
    int retry_count = 0;

    do {
        /* Trigger ADC PTIM mode to get VBAT and current */
        do_ptim(&volt_1, &curr_1);
        /* enable dummy load */
        dlpt_dev->enable_dummy_load(true);
        mdelay(DUMMY_LOAD_DELAY_MS);
        /* Trigger ADC PTIM mode again to get new VBAT and current */
        do_ptim(&volt_2, &curr_2);
        /* disable dummy load */
        dlpt_dev->enable_dummy_load(false);
        mdelay(DUMMY_LOAD_DELAY_MS);
        /* Calculate Rac */
        if (curr_2 - curr_1 >= IBAT_DIFF_MIN
            && curr_2 - curr_1 <= IBAT_DIFF_MAX
            && volt_1 - volt_2 >= VBAT_DIFF_MIN
            && volt_1 - volt_2 <= VBAT_DIFF_MAX) {
            /* rac_val unit is m-ohm */
            rac_val = (int)((volt_1 - volt_2) * 1000) / (curr_2 - curr_1);
            if (rac_val < 0)
                rac_val = -rac_val;
            if (rac_val < RAC_MIN) {
                rac_val = ERR_NOT_VALID;
                dprintf(CRITICAL, "bypass due to Rac < 50mOhm\n");
            }
        } else {
            rac_val = ERR_NOT_VALID;
            dprintf(CRITICAL, "bypass due to v_diff or c_diff over range\n");
        }
        LTRACEF("v1=%d,v2=%d,c1=%d,c2=%d,rac_val=%d,retry=%d,v_diff=%d,c_diff=%d\n",
                volt_1, volt_2, curr_1, curr_2, rac_val,
                retry_count, (volt_1 - volt_2), (curr_2 - curr_1));
        retry_count++;
        if (retry_count < 3 && rac_val == ERR_NOT_VALID)
            retry_state = true;
        else
            retry_state = false;
    } while (retry_state == true);

    return rac_val;
}

int do_ptim(u32 *vbat, u32 *ibat)
{
    if (!dlpt_dev || !dlpt_dev->imp_conv)
        return ERR_NOT_CONFIGURED;
    return dlpt_dev->imp_conv(vbat, ibat);
}

void calc_dlpt_imix_r(void)
{
    int rac_val[5];
    int i;
    int validcnt = 0;
    int min = 1000, max = 0;
    unsigned int rac_val_sum = 0;

    charger_enable_charging(false);
    charger_enable_power_path(false);
    mdelay(CHARGER_DISABLE_DELAY_MS);

    /* remove Max/min rac_val and average it */
    for (i = 0; i < 5; i++) {
        rac_val[i] = get_rac_val();
        if (rac_val[i] <= min && rac_val[i] != ERR_NOT_VALID)
            min = rac_val[i];
        if (rac_val[i] >= max)
            max = rac_val[i];
        if (rac_val[i] != ERR_NOT_VALID) {
            rac_val_sum += rac_val[i];
            validcnt++;
        }
    }
    if (validcnt >= 4) {
        rac_val_sum = rac_val_sum - min - max;
        dlpt_info.imix_r = rac_val_sum / (validcnt - 2);
    } else if (validcnt != 0) {
        dlpt_info.imix_r = rac_val_sum / validcnt;
    }
    LTRACEF("rac_val:%d,%d,%d,%d,%d [%d:%d:%d], imix_r:%d\n",
            rac_val[0], rac_val[1], rac_val[2], rac_val[3], rac_val[4],
            min, max, validcnt, dlpt_info.imix_r);
    charger_enable_power_path(true);
    charger_enable_charging(true);
}

int pmic_dlpt_device_register(const struct pmic_dlpt_dev *dev)
{
#ifndef PROJECT_TYPE_FPGA
    if (!dev)
        return ERR_INVALID_ARGS;
    dlpt_dev = dev;
#endif

    return 0;
}

