/*
 * Copyright (c) 2020 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#pragma once

#define DCONFIG_DTB_SIZE 131072

void dconfig_initenv(void);

const char *dconfig_getenv(const char *name);

void *dconfig_dtb_overlay(void *fdtbuf, int fdtbuf_size, int free_dt);

int dconfig_socid_match(void);
