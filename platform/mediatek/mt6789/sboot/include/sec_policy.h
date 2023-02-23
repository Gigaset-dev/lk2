/*
 * Copyright (c) 2022 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#pragma once

#include <stdbool.h>
#include <stdint.h>

struct policy_part_map {
    const char *part_name1;
    const char *part_name2;
    const char *part_name3;
    const char *part_name4;
    /* secure policy */
    uint32_t sec_sbcdis_lock_policy;   /*SBC :Disable, LOCK_STATE: Lock   */
    uint32_t sec_sbcdis_unlock_policy; /*SBC :Disable, LOCK_STATE: Unlock */
    uint32_t sec_sbcen_lock_policy;    /*SBC :Enable, LOCK_STATE: Lock    */
    uint32_t sec_sbcen_unlock_policy;  /*SBC :ENable, LOCK_STATE: Unlock  */
};

