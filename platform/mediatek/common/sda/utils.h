/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#pragma once

static inline unsigned int extract_n2mbits(unsigned int input, unsigned int n, unsigned int m)
{
    /*
     * 1. ~0 = 1111 1111 1111 1111 1111 1111 1111 1111
     * 2. ~0 << (m - n + 1) = 1111 1111 1111 1111 1100 0000 0000 0000
     * // assuming we are extracting 14 bits, the +1 is added for inclusive selection
     * 3. ~(~0 << (m - n + 1)) = 0000 0000 0000 0000 0011 1111 1111 1111
     */
    int mask;

    if (n > m) {
        n = n + m;
        m = n - m;
        n = n - m;
    }
    mask = ~(~0U << (m - n + 1));
    return (input >> n) & mask;
}
