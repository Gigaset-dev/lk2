/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#pragma once

#include "efi.h"

enum {
    PART_RAW,
    PART_EXT4,
    PART_F2FS,
};

/* Boot device type */
enum {
    BOOTDEV_NAND,
    BOOTDEV_SDMMC,
    BOOTDEV_UFS
};

int partition_get_type(const char *name, int *p_type);
int partition_get_name_list(char **name_list, int *count);
int partition_update_table(char *part_name, int delta_lba);
int partition_update_gpt_table(uint8_t *gpt_raw, unsigned int size);
int partition_get_uuid_by_name(const char *name, char **ret);
void partition_table_dump(void);
int partition_get_bootdev_type(void);
int partition_get_active_preloader(char *part_name, const int name_size);
