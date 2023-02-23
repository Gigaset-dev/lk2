/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#pragma once

/*******************************************
 * EFUSE RUN-TIME BLOWING INDEX DEFINITIONS
 *******************************************/
#define EFUSE_IDX_C_DAT0            0
#define EFUSE_IDX_C_DAT1            1
#define EFUSE_IDX_C_DAT2            2
#define EFUSE_IDX_C_DAT3            3
#define EFUSE_IDX_C_DAT4            4
#define EFUSE_IDX_C_DAT5            5
#define EFUSE_IDX_C_CTRL0           6
#define EFUSE_IDX_C_CTRL1           7
#define EFUSE_IDX_SEC_CTRL          8

/*******************************************
 * EFUSE RUN-TIME BLOWING ERROR CODE
 *******************************************/
#define S_EFUSE_DONE                        0
#define S_EFUSE_BLOW_ERROR                  0x00100000
#define S_EFUSE_PMIC_ERR                    0x00020000
#define S_EFUSE_FIELD_NOT_SUPPORTED         0x00001000
#define S_EFUSE_INPUT_DATA_NOT_VALID        0x00002000
#define S_EFUSE_MULTI_ECC_FIELD_BLOWN_TWICE 0x00004000

/*******************************************
 * EFUSE RUN-TIME BLOWING FUNCTIONS
 *******************************************/
u32 efuse_runtime_read(u32 index, u32 *data);
u32 efuse_runtime_blow(u32 index, u32 data);
