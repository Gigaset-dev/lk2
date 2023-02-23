/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#pragma once

/**********************************************************************
 * Platform interfaces for common code,
 * which should be implemented by each platform.
 **********************************************************************/
void *medmcu_dram_alloc(void **phy, unsigned int *size);
int verify_load_medmcu_image(const char *part_name, void *addr_phy,
                             void *addr);
void medmcu_emi_protect_enable(void *addr);
void disable_medmcu_hw(void);

