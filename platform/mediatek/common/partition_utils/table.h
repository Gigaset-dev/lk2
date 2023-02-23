/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#pragma once

#include "efi.h"

struct gpt_entry *get_entry(void);
struct gpt_header *get_header(void);
struct part_meta_info *get_info(void);
int get_part_count(void);
int get_entry_by_name(const char *name, struct gpt_entry **ret);
int part_entry_update(struct gpt_header *header, struct gpt_entry *entry_list);
int partition_reset_tbl(void);
void table_dump(void);
int get_uuid_by_name(const char *name, char **uuid);
