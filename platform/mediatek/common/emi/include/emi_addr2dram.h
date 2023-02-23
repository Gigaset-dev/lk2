/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#pragma once

struct emi_addr_map {
    int emi;
    int channel;
    int rank;
    int bank;
    int row;
    int column;
};

/*
 * mtk_addr2dram - Translate a physical address to a DRAM-point-of-view map
 * @fdt - device tree
 * @addr - input physical address
 * @map - output map stored in struct emi_addr_map
 *
 * Return 0 on success, -1 on failures.
 *
 * There is no code comment for the translation. This is intended since
 * the fomular of translation is derived from the implementation of EMI.
 */
int emi_addr2dram(unsigned long addr, struct emi_addr_map *map);
