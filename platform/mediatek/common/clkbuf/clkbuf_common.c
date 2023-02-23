/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */
#include <assert.h>
#include <clkbuf_common.h>
#include <debug.h>
#include <spmi_common.h>

void clk_buf_set_voter(unsigned int spmi_master, unsigned int spmi_slave,
        unsigned int addr_l, unsigned int addr_h,
        unsigned int vote_l_mask, unsigned int vote_l_shift,
        unsigned int vote_h_mask, unsigned int vote_h_shift)
{
#if !defined(CLKBUF_SPMI_BRINGUP)
    struct spmi_device *dev = get_spmi_device(spmi_master, spmi_slave);
    unsigned char val = 0;

    ASSERT(dev);

    spmi_ext_register_readl(dev, addr_l, &val, 1);
    val = val | (vote_l_mask << vote_l_shift);
    spmi_ext_register_writel(dev, addr_l, &val, 1);

    spmi_ext_register_readl(dev, addr_h, &val, 1);
    val = val | (vote_h_mask << vote_h_shift);
    spmi_ext_register_writel(dev, addr_h, &val, 1);
#if defined(CLKBUF_DEBUG)
    spmi_ext_register_readl(dev, addr_l, &val, 1);
    dprintf(CRITICAL, "[CLKBUF] vote addr: 0x%x config to 0x%x\n",
            addr_l, val);

    spmi_ext_register_readl(dev, addr_h, &val, 1);
    dprintf(CRITICAL, "[CLKBUF] vote addr: 0x%x config to 0x%x\n",
            addr_h, val);
#endif /* defined(CLKBUF_DEBUG) */
#endif /* !defined(CLKBUF_SPMI_BRINGUP) */
}

void clk_buf_mask_voter(unsigned int spmi_master, unsigned int spmi_slave,
        unsigned int addr_l, unsigned int addr_h,
        unsigned int vote_l_mask, unsigned int vote_h_mask,
        unsigned int vote_l_shift, unsigned int vote_h_shift)
{
#if !defined(CLKBUF_SPMI_BRINGUP)
    struct spmi_device *dev = get_spmi_device(spmi_master, spmi_slave);
    unsigned char val = 0;

    ASSERT(dev);

    spmi_ext_register_readl(dev, addr_l, &val, 1);
    val = val & (~(vote_l_mask << vote_l_shift));
    spmi_ext_register_writel(dev, addr_l, &val, 1);

    spmi_ext_register_readl(dev, addr_h, &val, 1);
    val = val & (~(vote_h_mask << vote_h_shift));
    spmi_ext_register_writel(dev, addr_h, &val, 1);
#if defined(CLKBUF_DEBUG)
    spmi_ext_register_readl(dev, addr_l, &val, 1);
    dprintf(CRITICAL, "[CLKBUF] vote addr: 0x%x config to 0x%x\n",
            addr_l, val);

    spmi_ext_register_readl(dev, addr_h, &val, 1);
    dprintf(CRITICAL, "[CLKBUF] vote addr: 0x%x config to 0x%x\n",
            addr_h, val);
#endif /* defined(CLKBUF_DEBUG) */
#endif /* !defined(CLKBUF_SPMI_BRINGUP) */
}

void clk_buf_disable_xo(unsigned int spmi_master, unsigned int spmi_slave,
        unsigned int mode_addr, unsigned int en_addr,
        unsigned int mode_mask, unsigned int mode_shift,
        unsigned int en_mask, unsigned int en_shift)
{
#if !defined(CLKBUF_SPMI_BRINGUP)
    struct spmi_device *dev = get_spmi_device(spmi_master, spmi_slave);
    unsigned char val = 0;

    ASSERT(dev);

    spmi_ext_register_readl(dev, mode_addr, &val, 1);
    val = val & (~(mode_mask << mode_shift));
    if (mode_addr == en_addr)
        val = val & (~(en_mask << en_shift));

    spmi_ext_register_writel(dev, mode_addr, &val, 1);

    if (mode_addr != en_addr) {
        spmi_ext_register_readl(dev, en_addr, &val, 1);
        val = val & (~(en_mask << en_shift));
        spmi_ext_register_writel(dev, en_addr, &val, 1);
    }

#if defined(CLKBUF_DEBUG)
    spmi_ext_register_readl(dev, mode_addr, &val, 1);
    dprintf(CRITICAL, "[CLKBUF] mode_addr: 0x%x, val: 0x%x\n",
            mode_addr, val);
    if (mode_addr != en_addr) {
        spmi_ext_register_readl(dev, en_addr, &val, 1);
        dprintf(CRITICAL, "[CLKBUF] en_addr: 0x%x, val: 0x%x\n",
                en_addr, val);
    }
#endif /* defined(CLKBUF_DEBUG) */
#endif /* !defined(CLKBUF_SPMI_BRINGUP) */
}

