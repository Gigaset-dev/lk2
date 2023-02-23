/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#pragma once

/**************************************************************
 * ATAG porting
 **************************************************************/
/*device information*/
#define ATAG_DEVINFO_DATA_SIZE    220
#define EFUSE_SELF_BLOW_RESULT_IDX  49
#define HRID_SIZE 4

int get_devinfo_model_name(char *ptr, size_t ptr_size);

/******************************************************************************
 * EXPORT DATA/FUNCTION
 ******************************************************************************/
unsigned int internal_get_devinfo_with_index(unsigned int index);
unsigned int get_devinfo_with_index(unsigned int index);
void devinfo_init_mode(char *mode);

/******************************************************************************
 * EFUSE RUN-TIME BLOWING FUNCTIONS
 ******************************************************************************/
unsigned int efuse_fsource_set(void);
unsigned int efuse_fsource_close(void);
unsigned int efuse_fsource_is_enabled(void);

void efuse_wdt_restart(void);
