/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */
#pragma once

extern uint32_t g_boot_state;

/* verified boot state */
#define BOOT_STATE_GREEN   0x0
#define BOOT_STATE_YELLOW  0x1
#define BOOT_STATE_ORANGE  0x2
#define BOOT_STATE_RED     0x3

/* lock state */
#define DEVICE_STATE_UNLOCKED 0x0
#define DEVICE_STATE_LOCKED   0x1

int print_boot_state(void);
int yellow_state_warning(void);
int orange_state_warning(void);
int red_state_warning(const char *img_name);
int show_warning(const char *img_name);

int set_boot_state_to_cmdline();
int vboot_state_result(uint32_t bootimg_type);