/*
 * Copyright (c) 2019 MediaTek Inc.
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files
 * (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge,
 * publish, distribute, sublicense, and/or sell copies of the Software,
 * and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
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

int get_policy_entry_idx(const char *part_name);
bool get_vfy_policy(const char *part_name);
bool get_dl_policy(const char *part_name);

