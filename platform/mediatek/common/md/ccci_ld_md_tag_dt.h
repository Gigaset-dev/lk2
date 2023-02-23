/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#pragma once

int ccci_lk_tag_info_init(void);
int ccci_insert_tag_inf(const char *name, char *data, unsigned int size);
int ccci_find_tag_inf(const char *name, char *buf, unsigned int size);

void ccci_update_err_by_md(int md_id, int error);
void ccci_update_md_load_bitmap(int md_id);
void ccci_update_common_err_info(int error);
