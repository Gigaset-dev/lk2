/*
 * Copyright (c) 2020 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */
#pragma once
#include <platform/addressmap.h>

#define SPM_DUMP_START        (SPM_BASE + 0x800)
#define SPM_DUMP_END          (SPM_BASE + 0x8B8)
#define SPM_BUF_LENGTH        0x1000

enum MT_SPM_SMC_UID {
        /* call from lk load_spm */
        MT_SPM_SMC_UID_FW_INIT = 0x5731,
};
