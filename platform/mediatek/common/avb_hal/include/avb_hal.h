/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#pragma once
#include <avb_ops.h>

#define PERSIST_PART_NAME               "seccfg"
#define PERSIST_VALUE_OFFSET            0x200
#define PERSIST_UNSPECIFIED_OFFSET      0xFFFFFFFFFFFFFFFF

AvbIOResult avb_hal_get_size_of_partition(AvbOps *ops,
                                          const char *partition,
                                          uint64_t *out_size_num_bytes);
AvbIOResult avb_hal_get_abs_offset(const char *partition, int64_t offset, uint64_t *abs_offset);
AvbOps *avb_hal_get_operations(void);
