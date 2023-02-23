/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#pragma once

#include <sys/types.h>

int dbgtop_drm_init(void *fdt);
int mtk_dbgtop_dram_reserved(int enable);
int mtk_dbgtop_is_reserve_ddr_enabled(void);
int mtk_dbgtop_dfd_count_en(int value);
int mtk_dbgtop_dfd_therm1_dis(int value);
int mtk_dbgtop_dfd_therm2_dis(int value);
int mtk_dbgtop_dfd_timeout(u32 value_abnormal, u32 value_normal);
int mtk_dbgtop_dfd_timeout_reset(void);
int mtk_dbgtop_drm_latch_en(int value);
