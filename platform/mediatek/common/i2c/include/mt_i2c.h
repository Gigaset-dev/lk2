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

#include <dev/i2c.h>
#include <err.h>
#include <kernel/mutex.h>
#include <stdbool.h>
#include <stdint.h>
#include <sys/types.h>

#define INTERNAL_PULL_UP   1
#define EXTERNAL_PULL_UP   0
#define PULL_UP            1
#define SMT_EN             1
#define DIR_IN             0

enum i2c_trans_st_rs {
    I2C_TRANS_STOP = 0,
    I2C_TRANS_REPEATED_START,
};

enum mt_trans_op {
    I2C_MASTER_NONE = 0,
    I2C_MASTER_WR = 1,
    I2C_MASTER_RD,
    I2C_MASTER_WRRD,
};

struct mt_trans_data {
    u16 trans_num;
    u16 data_size;
    u16 trans_len;
    u16 trans_auxlen;
};

struct mt_i2c {
    //==========set in i2c probe============//
    addr_t   base;    /* i2c base addr */
    u16      id;
    u16      irqnr;    /* i2c interrupt number */
    u16      irq_stat; /* i2c interrupt status */
    u32      clk;     /* host clock speed in khz */
    u32      pdn;     /*clock number*/
    //==========common data define============//
    enum     i2c_trans_st_rs st_rs;
    enum     mt_trans_op op;
    addr_t   pdmabase;
    u32      speed;   //The speed (khz)
    u32      msg_len;
    u8       *msg_buf;    /* pointer to msg data */
    /* The address of the slave device, 7bit, */
    /* the value include read/write bit. */
    u8       addr;
    u8       master_code;/* master code in HS mode */
    u8       mode;    /* ST/FS/HS mode */
    //==========reserved function============//
    u8       is_push_pull_enable; //IO push-pull or open-drain
    u8       is_clk_ext_disable;   //clk entend default enable
    u8       is_dma_enabled;   //Transaction via DMA instead of 8-byte FIFO
    u8       read_flag;//read,write,read_write
    bool     dma_en;
    bool     poll_en;
    bool     pushpull;//open drain
    bool     filter_msg;//filter msg error log
    struct mutex    bus_lock;//bus lock

    //==========define reg============//
    u16      timing_reg;
    u16      ltiming_reg;
    u16      high_speed_reg;
    u16      control_reg;
    struct   mt_trans_data trans_data;
};

//==========================================================================
// I2C Exported Function
//==========================================================================
s32 mtk_i2c_read(u8 bus_id, u8 device_addr, u8 *buffer,
    u32 len, void *reserve);
s32 mtk_i2c_write(u8 bus_id, u8 device_addr, u8 *buffer,
    u32 len, void *reserve);
s32 mtk_i2c_write_read(u8 bus_id, u8 device_addr, u8 *buffer,
    u32 write_len, u32 read_len, void *reserve);
void set_gpio_i2c_mode(int gpio_num, int gpio_mode, int internal_pull_en);

int i2c_hw_init(void *fdt);
