/*
 * Copyright (c) 2022 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#pragma once

enum {
    KEY_TYPE_RPMB = 0,
    KEY_TYPE_FDE
};

unsigned int sec_key_derive(unsigned int key_type,
                            unsigned char *output_key,
                            unsigned int output_key_size);
