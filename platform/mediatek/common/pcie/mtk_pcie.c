/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#include <platform/addressmap.h>
#include <platform/mtk_pcie.h>
#include <reg.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

/* PCIe Register Map */
#define PCIE_LTSSM_STATUS      (PCIE0_MAC_BASE + 0x150)
#define PCIE_LTSSM_L0          0x10
#define PCIE_LTSSM_L0S         0x11
#define PCIE_LTSSM_L1_IDLE     0x13
#define PCIE_LTSSM_L2_IDLE     0x14
#define PCIE_LTSSM_HOTRESET    0x1a
#define PCIE_LTSSM_VAL(reg)    ((reg >> 24) & 0x1f)

#define PCIE_LINK_STATUS     (PCIE0_MAC_BASE + 0x154)
#define PCIE_DATA_LINK_UP    (1 << 8)

#define PCIE_DIPC_REG        (PCIE0_MAC_BASE + 0xd1c)
#define PCIE_DIPC_STAGE_LB    0
#define PCIE_DIPC_STAGE_HB    2
#define PCIE_DIPC_BROM_LB     4
#define PCIE_DIPC_BROM_HB     6
#define PCIE_DIPC_LK_LB       8
#define PCIE_DIPC_LK_HB       10
#define PCIE_DIPC_LINUX_LB    12
#define PCIE_DIPC_LINUX_HB    15
#define PCIE_DIPC_PORT_LB     16
#define PCIE_DIPC_PORT_HB     25
#define PCIE_RESET_TYPE_LB    26
#define PCIE_RESET_TYPE_HB    27
#define PCIE_DIPC_HOST_LB     28
#define PCIE_DIPC_HOST_HB     31

#define PCIE_COLD_RESET       1
#define PCIE_WARM_RESET       2

/* MHCCIF Register Map */
#define MHCCIF_SW_TCHNUM      (MHCCIF_EP_BASE + 0x0c)
#define MHCCIF_CHANNEL        5

#ifndef BIT
#define BIT(bit) (1UL << (bit))
#endif
#ifndef GENMASK
#define BITS_PER_LONG 32
#define GENMASK(h, l) \
    ((u32)(((~0UL) << (l)) & (~0UL >> (BITS_PER_LONG - 1 - (h)))))
#endif

static void mtk_reg_set_field(u32 reg, int h, int l, u32 data)
{
    int val;

    val = readl(reg);
    val &= ~GENMASK(h, l);
    val |= ((data << l) & GENMASK(h, l));
    writel(val, reg);
}

static int mtk_pcie_set_dipc_data(u32 data)
{
    writel(data, PCIE_DIPC_REG);

    return 0;
}

static int mtk_pcie_set_stage_data(u32 data)
{
    mtk_reg_set_field(PCIE_DIPC_REG, PCIE_DIPC_STAGE_HB,
                      PCIE_DIPC_STAGE_LB, data);

    return 0;
}

static int mtk_pcie_set_event_data(u32 data)
{
    mtk_reg_set_field(PCIE_DIPC_REG, PCIE_DIPC_LK_HB,
                      PCIE_DIPC_LK_LB, data);
    return 0;
}

static int mtk_pcie_get_ltssm(void)
{
    int ltssm;

    ltssm = readl(PCIE_LTSSM_STATUS);
    return PCIE_LTSSM_VAL(ltssm);
}

bool pcie_link_is_l0(void)
{
    return mtk_pcie_get_ltssm() == PCIE_LTSSM_L0 ? 1 : 0;
}

bool pcie_link_is_l1(void)
{
    return mtk_pcie_get_ltssm() == PCIE_LTSSM_L1_IDLE ? 1 : 0;
}

bool pcie_link_is_l2(void)
{
    return mtk_pcie_get_ltssm() == PCIE_LTSSM_L2_IDLE ? 1 : 0;
}

bool pcie_is_linkup(void)
{
    int val;

    val = readl(PCIE_LINK_STATUS);
    if (val & PCIE_DATA_LINK_UP)
        return pcie_link_is_l0();

    return false;
}

int mtk_pcie_set_port_data(u32 data)
{
    mtk_reg_set_field(PCIE_DIPC_REG, PCIE_DIPC_PORT_HB,
                      PCIE_DIPC_PORT_LB, data);

    return 0;
}

void pcie_dipc_write_lk(u32 event, u32 stage)
{
    mtk_pcie_set_event_data(event);
    mtk_pcie_set_stage_data(stage);

    /* Send irq to host */
    writel(MHCCIF_CHANNEL, MHCCIF_SW_TCHNUM);
}

void mtk_pcie_set_warm_reset(void)
{
    mtk_reg_set_field(PCIE_DIPC_REG, PCIE_RESET_TYPE_HB,
                      PCIE_RESET_TYPE_LB, PCIE_WARM_RESET);
}

void mtk_pcie_set_cold_reset(void)
{
    mtk_reg_set_field(PCIE_DIPC_REG, PCIE_RESET_TYPE_HB,
                      PCIE_RESET_TYPE_LB, PCIE_COLD_RESET);
}
