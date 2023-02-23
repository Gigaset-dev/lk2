/*
 * Copyright (c) 2022 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#pragma once

#define PLAT_DFD_MCU_BUF_LENGTH    0x1000
#define DFD_STATUS_RETURN          0x0C53E0A4

void plat_dfd_mcu_init(void);
int plat_dfd_mcu_get(void **data, int *len);
void plat_dfd_mcu_put(void **data);
