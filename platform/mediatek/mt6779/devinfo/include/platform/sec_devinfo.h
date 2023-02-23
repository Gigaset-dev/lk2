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

/******************************************************************************
 * EFUSE RUN-TIME BLOWING INDEX DEFINITIONS
 ******************************************************************************/
/*
 * NOTE: C_DATx is Multi-Bit ECC field and can only be blown once. If you blow
 *       it more than one time. Error code S_EFUSE_MULTI_ECC_FIELD_BLOWN_TWICE
 *       will be returned.
 */
#define EFUSE_IDX_C_DAT0            0
#define EFUSE_IDX_C_DAT1            1
#define EFUSE_IDX_C_DAT2            2
#define EFUSE_IDX_C_DAT3            3
#define EFUSE_IDX_C_DAT4            4
#define EFUSE_IDX_C_DAT5            5
#define EFUSE_IDX_C_CTRL0           6
#define EFUSE_IDX_C_CTRL1           7

/******************************************************************************
 * EFUSE RUN-TIME BLOWING ERROR CODE
 ******************************************************************************/
#define S_EFUSE_DONE                        0
#define S_EFUSE_BLOW_ERROR                  0x00100000
#define S_EFUSE_PMIC_ERR                    0x00020000
#define S_EFUSE_FIELD_NOT_SUPPORTED         0x00001000
#define S_EFUSE_INPUT_DATA_NOT_VALID        0x00002000
#define S_EFUSE_MULTI_ECC_FIELD_BLOWN_TWICE 0x00004000

/**************************************************************
 * ATAG porting
 **************************************************************/
/*device information*/
#define ATAG_DEVINFO_DATA_SIZE    200
#define EFUSE_SELF_BLOW_RESULT_IDX  49

int get_devinfo_model_name(char *ptr, size_t ptr_size);

/******************************************************************************
 * EXPORT DATA/FUNCTION
 ******************************************************************************/
unsigned int get_devinfo_with_index(unsigned int index);

/******************************************************************************
 * EFUSE RUN-TIME BLOWING FUNCTIONS
 ******************************************************************************/
unsigned int efuse_runtime_read(u32 index, u32 *data);
unsigned int efuse_runtime_blow(u32 index, u32 data);

unsigned int efuse_fsource_set(void);
unsigned int efuse_fsource_close(void);
unsigned int efuse_fsource_is_enabled(void);

