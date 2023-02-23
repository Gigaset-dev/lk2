/*
 * Copyright (c) 2021 MediaTek Inc. All Rights Reserved.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#pragma once

#include <stdbool.h>
#include <sys/types.h>

/***********************************************************************
 * Prototypes for the APIs.
 ***********************************************************************/
int cldma_put_data(unsigned char *buf, unsigned int len, lk_time_t timeout);
int cldma_get_data(unsigned char *buf, unsigned int len, lk_time_t timeout);
void cldma_init(void);

