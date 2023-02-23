/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#pragma once

#define LCM_MSG(string, args...) dprintf(CRITICAL, "[LCM]"string, ##args)
#define LCM_INFO(string, args...) dprintf(INFO, "[LCM]"string, ##args)
#define LCM_DBG(string, args...) dprintf(4, "[LCM]"string, ##args)

