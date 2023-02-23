/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#pragma once

void ccci_md_bus_protection_en(void);
void ccci_md_bus_protection_disable(void);
unsigned int ccci_get_ap_rgu_wdt_non_rst_reg_val(void);
void ccci_set_ap_rgu_wdt_non_rst_reg_val(unsigned int new_val);
void ccci_using_ap_rgu_reset_md(void);
void ccci_md_special_init(void);
