/*
 * Copyright (c) 2020 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#pragma once

#define MT6315_HW_OFF   0
#define MT6315_HW_LP    1

int mt6315_vbuck_lp_setting(int slvid, u8 buck_id, bool en, bool cfg);
int mt6315_device_register(int mstid, int slvid);
