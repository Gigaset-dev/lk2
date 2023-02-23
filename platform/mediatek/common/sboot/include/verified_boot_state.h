/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#pragma once

#include <stdint.h>

/* SEC-BOOT state */
#define ATTR_SBOOT_DISABLE                  0x00
#define ATTR_SBOOT_ENABLE                   0x11
#define ATTR_SBOOT_ONLY_ENABLE_ON_SCHIP     0x22

/* S-USBDL state */
#define ATTR_SUSBDL_DISABLE                 0x00
#define ATTR_SUSBDL_ENABLE                  0x11
#define ATTR_SUSBDL_ONLY_ENABLE_ON_SCHIP    0x22

/* verified boot state */
#define BOOT_STATE_GREEN   0x0
#define BOOT_STATE_YELLOW  0x1
#define BOOT_STATE_ORANGE  0x2
#define BOOT_STATE_RED     0x3

int get_sboot_state(uint32_t *sboot_state);

