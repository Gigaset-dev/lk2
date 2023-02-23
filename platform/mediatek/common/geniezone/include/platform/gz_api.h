/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#pragma once

/* gz_boot_tag */
uint32_t is_el2_enabled(void);
uint64_t get_el2_exec_start_offset(void);
uint64_t get_el2_reserved_mem_size(void);

/* gz_unmap2 */
void gz_unmap2(void);
