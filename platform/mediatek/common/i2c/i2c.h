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

#pragma once
#include <arch/arch_ops.h>
#include <debug.h>
#include <mt_i2c.h>
#include <sys/types.h>

#define ERR 0
#define MSG 1
#define LOG 2

/* write barrir to prevent compilation optimization */
#define I2C_MB()       smp_mb()
#define I2C_M_RD       0x0001

#define I2C_WRITE_FAIL_HS_NACKERR           0xA013
#define I2C_WRITE_FAIL_ACKERR               0xA014
#define I2C_WRITE_FAIL_TIMEOUT              0xA015

#define I2C_LOCK_TIMEOUT                    1

#define I2C_TRANSAC_START                   0x0001
#define I2C_SOFT_RST                        0x0001
#define I2C_SLAVE_ADDR_CLR                  0x0000
#define I2C_FIFO_ADDR_CLR                   0x0005

#define I2C_IO_CONFIG_PUSH_PULL             0x0000
#define I2C_IO_CONFIG_OPEN_DRAIN            0x0003
#define I2C_DELAY_LEN                       0x0002

//CONTROL
#define I2C_CONTROL_RS                  (0x1 << 1)
#define I2C_CONTROL_DMA_EN              (0x1 << 2)
#define I2C_CONTROL_CLK_EXT_EN          (0x1 << 3)
#define I2C_CONTROL_DIR_CHANGE          (0x1 << 4)
#define I2C_CONTROL_ACKERR_DET_EN       (0x1 << 5)
#define I2C_CONTROL_TRANSFER_LEN_CHANGE (0x1 << 6)
#define I2C_CONTROL_WRAPPER             (0x1 << 0)

#define I2C_HS_NACKERR            (1 << 2)
#define I2C_ACKERR                (1 << 1)
#define I2C_TRANSAC_COMP          (1 << 0)

#define MAX_ST_MODE_SPEED         100  /* khz */
#define MAX_FS_MODE_SPEED         400  /* khz */
#define MAX_HS_MODE_SPEED         3400 /* khz */

/* Max(65535) aligned to 4 bytes = 65532 */
#define MAX_DMA_TRANS_SIZE        65532
#define MAX_DMA_TRANS_NUM         256

#define MAX_SAMPLE_CNT_DIV        8
#define MAX_STEP_CNT_DIV          64
#define MAX_HS_STEP_CNT_DIV       8

#define DMA_ADDRESS_HIGH          (0xC0000000)

/*********************register operation**************************/
enum I2C_REGS_OFFSET {
    OFFSET_DATA_PORT,
    OFFSET_SLAVE_ADDR,
    OFFSET_INTR_MASK,
    OFFSET_INTR_STAT,
    OFFSET_CONTROL,
    OFFSET_TRANSFER_LEN,
    OFFSET_TRANSAC_LEN,
    OFFSET_DELAY_LEN,
    OFFSET_TIMING,
    OFFSET_START,
    OFFSET_EXT_CONF,
    OFFSET_FIFO_STAT,
    OFFSET_FIFO_THRESH,
    OFFSET_FIFO_ADDR_CLR,
    OFFSET_IO_CONFIG,
    OFFSET_RSV_DEBUG,
    OFFSET_HS,
    OFFSET_SOFTRESET,
    OFFSET_DCM_EN,
    OFFSET_PATH_DIR,
    OFFSET_DEBUGSTAT,
    OFFSET_DEBUGCTRL,
    OFFSET_TRANSFER_LEN_AUX,
    OFFSET_CLOCK_DIV,
    OFFSET_LTIMING,
    OFFSET_SCL_HIGH_LOW_RATIO,
    OFFSET_HS_SCL_HIGH_LOW_RATIO,
    OFFSET_SCL_MIS_COMP_POINT,
    OFFSET_STA_STO_AC_TIMING,
    OFFSET_HS_STA_STO_AC_TIMING,
    OFFSET_SDA_TIMING,
    OFFSET_MCU_INTR,
    OFFSET_MULTI_DMA,
};

enum DMA_REGS_OFFSET {
    OFFSET_INT_FLAG       = 0x0,
    OFFSET_INT_EN         = 0x04,
    OFFSET_EN             = 0x08,
    OFFSET_RST            = 0x0C,
    OFFSET_CON            = 0x18,
    OFFSET_TX_MEM_ADDR    = 0x1C,
    OFFSET_RX_MEM_ADDR    = 0x20,
    OFFSET_TX_LEN         = 0x24,
    OFFSET_RX_LEN         = 0x28,
};

struct mt_i2c_dts_info {
    u32 clock_source;
    u32 i2c_nr;
    u32 sda_gpio_num[16];
    u32 scl_gpio_num[16];
    u32 i2c_mode[16]; /*set gpio i2c mode*/
    u32 internal_pull_en[16]; /*internal pull up/external pull up*/
};

extern const u16 mt_i2c_regs[];
void i2c_writel(struct mt_i2c *i2c, u8 offset, u16 value);
u32 i2c_readl(struct mt_i2c *i2c, u16 offset);
void i2c_set_timing_regs(struct mt_i2c *i2c);
