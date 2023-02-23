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
#include <assert.h>
#include <auxadc.h>
#include <platform/addressmap.h>
#include <platform/reg.h>
#include <platform/wait.h>

struct mtk_auxadc_regs {
    uint32_t con0;
    uint32_t con1;
    uint32_t con1_set;
    uint32_t con1_clr;
    uint32_t con2;
    uint32_t data[16];
    uint32_t reserved[16];
    uint32_t misc;
};

static struct mtk_auxadc_regs *const mtk_auxadc = (void *)AUXADC_BASE;

__WEAK enum auxadc_platform_info_flag_t auxadc_get_platform_flag(void)
{
    return AUXADC_MISC_EN;
}

__WEAK int32_t auxadc_get_platform_cali(int32_t *cali_ge, int32_t *cali_oe)
{
    if ((!cali_ge) || (!cali_ge))
        return -1;

    *cali_ge = 0;
    *cali_oe = 0;

    return 0;
}


static uint32_t auxadc_get_rawdata(int channel, enum auxadc_platform_info_flag_t flags)
{
    uint32_t value;

    if (flags & AUXADC_MISC_EN)
        setbits32(&mtk_auxadc->misc, 1 << 14);

    /* step1 check con2 if auxadc is busy */
    ASSERT(wait_ms(!(read32(&mtk_auxadc->con2) & 0x1), 300));

    /* step2 clear bit */
    clrbits32(&mtk_auxadc->con1, 1 << channel);

    /* step3 read channel and make sure old ready bit = 0 */
    ASSERT(wait_ms(!(read32(&mtk_auxadc->data[channel]) & (1 << 12)), 300));

    /* step4 set bit  to trigger sample */
    setbits32(&mtk_auxadc->con1, 1 << channel);

    /* step5 read channel and make sure ready bit = 1 */
    ASSERT(wait_ms(read32(&mtk_auxadc->data[channel]) & (1 << 12), 300));

    /* step6 read data */
    value = read32(&mtk_auxadc->data[channel]) & 0x0FFF;

    if (flags & AUXADC_MISC_EN)
        clrbits32(&mtk_auxadc->misc, 1 << 14);

    return value;
}

/* return mv */
int auxadc_get_voltage(unsigned int channel)
{
    uint32_t raw_value;
    enum auxadc_platform_info_flag_t flags;
    int32_t cali_ge;
    int32_t cali_oe;

    ASSERT(channel < 16);

    flags = auxadc_get_platform_flag();
    auxadc_get_platform_cali(&cali_ge, &cali_oe);

    /* 1.5V in 4096 steps */
    raw_value = auxadc_get_rawdata(channel, flags);

    if (flags & AUXADC_CALI_EN) {
        raw_value = (4096 * (raw_value - cali_oe)) /
                   (4096 + cali_ge);
    }

    return (int)((int64_t)raw_value * 1500 / 4096);
}
