/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#pragma once

#include <stdint.h>
#include <avb_slot_verify.h>

uint32_t collect_rot_info(AvbSlotVerifyData *slot_data);
void send_root_of_trust_info(void);
void set_rot_boot_state(uint32_t state);

