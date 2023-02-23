/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#pragma once

#include <stdint.h>

/* verified boot state */
#define BOOT_STATE_GREEN   (0)
#define BOOT_STATE_YELLOW  (1)
#define BOOT_STATE_ORANGE  (2)
#define BOOT_STATE_RED     (3)
#define MAX_IMG_NAME_LEN   (32)

uint32_t get_boot_state(void);
void set_boot_state(uint32_t boot_state);
void print_boot_state(void);
void set_boot_state_to_cmdline(void);
int show_warning(const char *img_name);
