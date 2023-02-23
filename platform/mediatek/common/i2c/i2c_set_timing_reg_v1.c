/*
 * Copyright (C) 2020 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#include <mt_i2c.h>
#include <platform_i2c.h>
#include "i2c.h"

const u16 mt_i2c_regs[] = {
    [OFFSET_DATA_PORT]             = 0x0,
    [OFFSET_SLAVE_ADDR]            = 0x4,
    [OFFSET_INTR_MASK]             = 0x8,
    [OFFSET_INTR_STAT]             = 0xc,
    [OFFSET_CONTROL]               = 0x10,
    [OFFSET_TRANSFER_LEN]          = 0x14,
    [OFFSET_TRANSAC_LEN]           = 0x18,
    [OFFSET_DELAY_LEN]             = 0x1c,
    [OFFSET_TIMING]                = 0x20,
    [OFFSET_START]                 = 0x24,
    [OFFSET_EXT_CONF]              = 0x28,
    [OFFSET_FIFO_STAT]             = 0x30,
    [OFFSET_FIFO_THRESH]           = 0x34,
    [OFFSET_FIFO_ADDR_CLR]         = 0x38,
    [OFFSET_IO_CONFIG]             = 0x40,
    [OFFSET_RSV_DEBUG]             = 0x44,
    [OFFSET_HS]                    = 0x48,
    [OFFSET_SOFTRESET]             = 0x50,
    [OFFSET_DCM_EN]                = 0x54,
    [OFFSET_PATH_DIR]              = 0x60,
    [OFFSET_DEBUGSTAT]             = 0x64,
    [OFFSET_DEBUGCTRL]             = 0x68,
    [OFFSET_TRANSFER_LEN_AUX]      = 0x6c,
    [OFFSET_CLOCK_DIV]             = 0x70,
    [OFFSET_SCL_HIGH_LOW_RATIO]    = 0x74,
    [OFFSET_HS_SCL_HIGH_LOW_RATIO] = 0x78,
    [OFFSET_SCL_MIS_COMP_POINT]    = 0x7C,
    [OFFSET_STA_STO_AC_TIMING]     = 0x80,
    [OFFSET_HS_STA_STO_AC_TIMING]  = 0x84,
    [OFFSET_SDA_TIMING]            = 0x88,
};

void i2c_set_timing_regs(struct mt_i2c *i2c)
{
    i2c_writel(i2c, OFFSET_CLOCK_DIV, (I2C_CLK_DIV - 1));
    i2c_writel(i2c, OFFSET_TIMING, i2c->timing_reg);
    i2c_writel(i2c, OFFSET_HS, i2c->high_speed_reg);
}
