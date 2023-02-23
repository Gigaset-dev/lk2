/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */
#include <stdint.h>

int set_lock_state(uint32_t lock_state)
{
    return -1;
}

const char *get_lock_state_str(void)
{
    return "unlocked";
}

const char *get_warranty_str(void)
{
    return "voided";
}
