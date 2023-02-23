/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#pragma once

#define DCONFIG_PART "boot_para"

int load_dconfig_partition(const char *img_name, void *buffer, size_t buffer_size,
                           uint8_t socid[SOCID_LEN]);

