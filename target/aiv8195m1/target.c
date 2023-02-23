/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */
#include <mmc_core.h>
#include <trace.h>

#define LOCAL_TRACE 0

struct mmc_card *card;
bool retry_opcond;

void target_early_init(void)
{
    card = emmc_init_stage1(&retry_opcond);
}

void target_init(void)
{
    emmc_init_stage2(card, retry_opcond);
}
