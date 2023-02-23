/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#pragma once

enum sramrc_smc_type {
    SRAMRC_INIT = 0,
    SRAMRC_MAX_TIMEOUT,
    SRAMRC_REGISTER_ACCESS,
};

enum sramrc_smc_action {
    SRAMRC_ACT_READ = 0,
    SRAMRC_ACT_WRITE,
};
