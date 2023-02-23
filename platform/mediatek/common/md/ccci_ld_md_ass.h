/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#pragma once

int ccci_main_img_parsing(unsigned char *base, int size, char **hdr_ptr);
int ccci_dsp_img_parsing(unsigned char *base, int size, unsigned int offset);
int ccci_get_md_version(int md_id, char buf[], int size);
