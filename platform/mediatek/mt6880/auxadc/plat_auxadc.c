/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */
#include <auxadc.h>
#include <debug.h>
#include <platform/sec_devinfo.h>
#include <stdlib.h>

#define ADC_OE_A_SHIFT      0
#define ADC_OE_A_MASK       (0x3ff << ADC_OE_A_SHIFT)
#define ADC_GE_A_SHIFT      10
#define ADC_GE_A_MASK       (0x3ff << ADC_GE_A_SHIFT)
#define ADC_CALI_EN_A_SHIFT 20
#define ADC_CALI_EN_A_MASK  (0x1 << ADC_CALI_EN_A_SHIFT)

struct auxadc_platform_info_t {
    uint32_t                            is_init;
    enum auxadc_platform_info_flag_t    flags;
    uint32_t                            cali_data;
    uint32_t                            cali_ge_a;
    uint32_t                            cali_oe_a;
    int32_t                             cali_ge;
    int32_t                             cali_oe;
};

struct auxadc_platform_info_t auxadc_platform_info = {0};

void auxadc_get_platform_data(void)
{
    uint32_t cali_data;
    uint32_t cali_ge_a = 0, cali_oe_a = 0;
    int32_t cali_ge, cali_oe;

    if (auxadc_platform_info.is_init)
        return;

    cali_data = get_devinfo_with_index(62);

    if (((cali_data & ADC_CALI_EN_A_MASK) >> ADC_CALI_EN_A_SHIFT) != 0) {
        cali_oe_a = (cali_data & ADC_OE_A_MASK) >> ADC_OE_A_SHIFT;
        cali_ge_a = ((cali_data & ADC_GE_A_MASK) >> ADC_GE_A_SHIFT);
        cali_ge = cali_ge_a - 512;
        cali_oe = cali_oe_a - 512;
        dprintf(INFO, "[AUXADC]: cali_ge = 0x%x, cali_oe = 0x%x\n",
            cali_ge, cali_oe);
    } else {
        cali_oe = 0;
        cali_ge = 0;
        dprintf(INFO, "[AUXADC]: No efuse data\n");
    }

    auxadc_platform_info.is_init = 1;
    auxadc_platform_info.flags |= AUXADC_CALI_EN;

    auxadc_platform_info.cali_data = cali_data;
    auxadc_platform_info.cali_ge_a = cali_ge_a;
    auxadc_platform_info.cali_oe_a = cali_oe_a;
    auxadc_platform_info.cali_ge = cali_ge;
    auxadc_platform_info.cali_oe = cali_oe;
}

enum auxadc_platform_info_flag_t auxadc_get_platform_flag(void)
{
    auxadc_get_platform_data();
    return auxadc_platform_info.flags;
}

int32_t auxadc_get_platform_cali(int32_t *cali_ge, int32_t *cali_oe)
{
    if ((!cali_ge) || (!cali_ge))
        return -1;

    auxadc_get_platform_data();
    *cali_ge = auxadc_platform_info.cali_ge;
    *cali_oe = auxadc_platform_info.cali_oe;

    return 0;

}
