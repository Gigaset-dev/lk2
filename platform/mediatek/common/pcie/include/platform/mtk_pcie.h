/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#pragma once

#include <stdbool.h>
#include <sys/types.h>

/* LK information */
#define LK_NORMAL       0x00
#define LK_POST_DUMP    0x01
#define LK_REBOOT       0x07
#define LK_EVENT_MASK   0x07

/* Device stage information */
#define DEV_INITIALIZE    0x00
#define DEV_BROM_DL_MODE  0x01
#define DEV_BROM_DL_ACT   0x02
#define DEV_LK_BRANCH     0x03
#define DEV_LINUX_BRANCH  0x04
#define DEV_PL_DA         0x05    /* Reserved */

int mtk_pcie_set_port_data(u32 data);
void pcie_dipc_write_lk(u32 event, u32 stage);

/* Interfaces for LK */
#define dipc_set_lk_normal(stage)    pcie_dipc_write_lk(LK_NORMAL, stage)
#define dipc_set_lk_post(stage)      pcie_dipc_write_lk(LK_POST_DUMP, stage)
#define dipc_set_lk_reboot(stage)    pcie_dipc_write_lk(LK_REBOOT, stage)

/* Interfaces to query PCIe link status */
bool pcie_link_is_l0(void);
bool pcie_link_is_l1(void);
bool pcie_link_is_l2(void);
bool pcie_is_linkup(void);
void mtk_pcie_set_cold_reset(void);
void mtk_pcie_set_warm_reset(void);
