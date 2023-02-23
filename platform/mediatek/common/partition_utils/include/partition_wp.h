/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#pragma once
// Important: do not change the enum below.
// They are used as assigned value.
enum {
    WP_PERMANENT = 0,
    WP_POWER_ON,
    WP_PERMANENT_AWP,
    WP_TEMPORARY,
    WP_DISABLE,
};

int partition_write_prot_set(const char *start_part,
    const char *end_part, int type);
