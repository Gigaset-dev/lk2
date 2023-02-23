/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#pragma once

enum devapc_type {
    DEVAPC_INFRA = 0,
    DEVAPC_VLP,
    DEVAPC_ADSP,
    DEVAPC_MMINFRA,
    DEVAPC_MMUP,
    DEVAPC_NUM,
};

void devapc_protection_enable(enum devapc_type type);
