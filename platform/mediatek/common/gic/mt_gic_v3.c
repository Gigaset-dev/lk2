/*
 * Copyright (c) 2012-2019 Travis Geiselbrecht
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

#include <dev/interrupt/arm_gic.h>
#include <platform/gic.h>
#include <platform/mcucfg.h>
#include <platform/reg.h>
#include <arch/arch_ops.h>
#include <mt_gic.h>

void mt_irq_set_polarity(unsigned int irq, unsigned int polarity)
{
#if !defined(MTK_POL_DEPRECATED)
    unsigned int value;
    unsigned int index, offset;

    // peripheral device's IRQ line is using GIC's SPI, and line ID >= GIC_PRIVATE_SIGNALS
    if (irq < 32)
        return;

    offset = (irq - 32) & 0x1F;
    index = (irq - 32) >> 5;
    value = (polarity == GIC_LOW_POLARITY) ? 1 : 0;
    clrsetbits32(&mcucfg->int_pol_ctl[index], 1 << offset, value << offset);
#endif
}

void mt_irq_set_sens(unsigned int irq, unsigned int sens)
{
    unsigned int config;

    if (sens == GIC_EDGE_SENSITIVE) {
        config = GIC_READ(GIC_DIST_BASE + GIC_DIST_CONFIG + (irq / 16) * 4);
        config |= (0x2 << (irq % 16) * 2);
        GIC_WRITE(GIC_DIST_BASE + GIC_DIST_CONFIG + (irq / 16) * 4, config);
    } else {
        config = GIC_READ(GIC_DIST_BASE + GIC_DIST_CONFIG + (irq / 16) * 4);
        config &= ~(0x2 << (irq % 16) * 2);
        GIC_WRITE(GIC_DIST_BASE + GIC_DIST_CONFIG + (irq / 16) * 4, config);
    }
    DSB;
}

void clear_sec_pol_ctl_en(void)
{
#if defined(SECURE_WORLD) && !defined(MTK_POL_DEPRECATED)
    unsigned int i;

    /* total 19 polarity ctrl registers */
    for (i = 0; i < NR_INT_POL_CTL; i++)
        write32(&mcucfg->sec_pol_ctl_en[i], 0);
#endif
}
