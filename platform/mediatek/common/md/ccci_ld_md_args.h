/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#pragma once

void ccci_get_boot_args_from_dt(void *fdt);
int ccci_get_mem_limit_align_map(const char name[], unsigned long long *low_bound,
    unsigned long long *limit, unsigned long *align, unsigned int *mapping);
int ccci_get_md_enabled(void);
