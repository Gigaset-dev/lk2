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

#include <ufs_cfg.h>
#include "ufs_platform.h"
#include "ufs_hcd.h"
#include "ufs_utils.h"
#include <platform/round.h>

void ufs_init_mphy(void)
{
    unsigned int N;
    unsigned int reg = 0;
    unsigned int temp = 0;

    /*
     * Support t12 UFS TX Deemphasis by Software.
     * RG_MP_TX_RDN_TRIM_CODE      0x11FA9000  ,  Bit [12:8]
     * RG_MP_TX0_DRV_MASTER_3P5DB  0x11FA9008  ,  Bit [4:0]
     * RG_MP_TX0_DRV_SLAVE_3P5DB   0x11FA9008  ,  Bit [7:5]
     * RG_MP_TX0_DRV_MASTER_6DB    0x11FA9008  ,  Bit [12:8]
     * RG_MP_TX0_DRV_SLAVE_6DB 0x11FA9008  ,  Bit [15:13]
     */

    /* N = RG_MP_TX0_RDN_TRIM_CODE<12:8> + 9 */
    reg = readl(UFS_MPHY_BASE + 0x9000);
    N = ((reg & (0x00001F00)) >> 8) + 9;

    /* RG_MP_TX0_DRV_MASTER_3P5DB = round(N*(5/6)) - 9 */
    temp = DIV_ROUND_CLOSEST(N*5, 6) - 9;
    reg = temp & 0x0000001F;

    /* RG_MP_TX0_DRV_SLAVE_3P5DB = N - round(N*(5/6)) - 3 */
    temp = N - DIV_ROUND_CLOSEST(N*5, 6) - 3;
    reg |= ((temp << 5) & 0x000000E0);

    /* RG_MP_TX0_DRV_MASTER_6DB = round(N*(3/4)) - 9 */
    temp = DIV_ROUND_CLOSEST(N*3, 4) - 9;
    reg |= ((temp << 8) & 0x00001F00);

    /* RG_MP_TX0_DRV_SLAVE_6DB = N - round(N*(3/4)) - 3 */
    temp = N - DIV_ROUND_CLOSEST(N*3, 4) - 3;
    reg |= ((temp << 13) & 0x0000E000);

    writel(((readl(UFS_MPHY_BASE + 0x9008) & 0xFFFF0000) | reg), (UFS_MPHY_BASE + 0x9008));
}

