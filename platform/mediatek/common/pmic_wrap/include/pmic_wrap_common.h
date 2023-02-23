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

#include <assert.h>
#include <debug.h>
#include <pmic_wrap.h>

#define PWRAPTAG                "[PWRAP] "
#define pwrap_err(fmt, arg ...) \
        dprintf(CRITICAL, PWRAPTAG "ERROR,line=%d" fmt, __LINE__, ## arg)

/* external API */
s32 check_idle(void *addr, u32 expected);
s32 pwrap_wacs2(u32 write, u16 adr, u16 wdata, u16 *rdata);
static inline s32 pwrap_read(u16 addr, u16 *rdata)
{
    return pwrap_wacs2(0, addr, 0, rdata);
}

static inline s32 pwrap_write(u16 addr, u16 wdata)
{
    return pwrap_wacs2(1, addr, wdata, 0);
}

static inline u16 pwrap_read_field(u16 reg, u16 mask, u16 shift)
{
    u16 rdata;
    s32 ret;

    ret = pwrap_read(reg, &rdata);
    if (ret != 0)
        ASSERT(0);

    return (rdata >> shift) & mask;
}

static inline void pwrap_write_field(u16 reg, u16 val, u16 mask, u16 shift)
{
    u16 data;
    s32 ret;

    ret = pwrap_read(reg, &data);
    if (ret != 0)
        ASSERT(0);

    data = data & ~(mask << shift);
    data |= (val << shift);
    pwrap_write(reg, data);
}

/* dewrapper defaule value */
enum {
      DEFAULT_VALUE_READ_TEST  = 0x5aa5,
      WRITE_TEST_VALUE         = 0xa55a
};

/* manual commnd */
enum {
      OP_WR    = 0x1,
      OP_CSH   = 0x0,
      OP_CSL   = 0x1,
      OP_OUTS  = 0x8,
};

enum {
      RDATA_WACS_RDATA_MASK = 0xffff,
      RDATA_WACS_FSM_MASK   = 0x7,
      RDATA_WACS_REQ_MASK   = 0x1,
      RDATA_SYNC_IDLE_MASK  = 0x1,
      RDATA_INIT_DONE_MASK  = 0x1,
      RDATA_SYS_IDLE_MASK   = 0x1,
};

/* WACS_FSM */
enum {
      WACS_FSM_IDLE     = 0x00,
      WACS_FSM_REQ      = 0x02,
      WACS_FSM_WFDLE    = 0x04, /* wait for dle, wait for read data done */
      WACS_FSM_WFVLDCLR = 0x06, /* finish read data, wait for valid flag
                                 * clearing
                                 */
};

enum {
      WACS_INIT_DONE    = 0x01,
      WACS_SYNC_IDLE    = 0x01,
      WACS_SYNC_BUSY    = 0x00
};

enum {
      RDATA_WRAP_CH_DLE_RESTCNT_SHIFT = 0,
      RDATA_WRAP_FSM_SHIFT            = 11,
};

enum {
      RDATA_WRAP_CH_DLE_RESTCNT_MASK = 0x7,
      RDATA_WRAP_FSM_MASK            = 0xf,
};

enum {
      WRAP_IDLE = 0x0,
};

/* error information flag */
enum {
      E_PWR_INVALID_ARG             = 1,
      E_PWR_INVALID_RW              = 2,
      E_PWR_INVALID_ADDR            = 3,
      E_PWR_INVALID_WDAT            = 4,
      E_PWR_INVALID_OP_MANUAL       = 5,
      E_PWR_NOT_IDLE_STATE          = 6,
      E_PWR_NOT_INIT_DONE           = 7,
      E_PWR_NOT_INIT_DONE_READ      = 8,
      E_PWR_WAIT_IDLE_TIMEOUT       = 9,
      E_PWR_WAIT_IDLE_TIMEOUT_READ  = 10,
      E_PWR_INIT_SIDLY_FAIL         = 11,
      E_PWR_RESET_TIMEOUT           = 12,
      E_PWR_TIMEOUT                 = 13,
      E_PWR_INIT_RESET_SPI          = 20,
      E_PWR_INIT_SIDLY              = 21,
      E_PWR_INIT_REG_CLOCK          = 22,
      E_PWR_INIT_ENABLE_PMIC        = 23,
      E_PWR_INIT_DIO                = 24,
      E_PWR_INIT_CIPHER             = 25,
      E_PWR_INIT_WRITE_TEST         = 26,
      E_PWR_INIT_ENABLE_CRC         = 27,
      E_PWR_INIT_ENABLE_DEWRAP      = 28,
      E_PWR_INIT_ENABLE_EVENT       = 29,
      E_PWR_READ_TEST_FAIL          = 30,
      E_PWR_WRITE_TEST_FAIL         = 31,
      E_PWR_SWITCH_DIO              = 32
};

typedef u32 (*loop_condition_fp)(u32);

static inline u32 wait_for_fsm_vldclr(u32 x)
{
    return ((x >> RDATA_WACS_FSM_SHIFT) & RDATA_WACS_FSM_MASK) !=
            WACS_FSM_WFVLDCLR;
}

static inline u32 wait_for_fsm_idle(u32 x)
{
    return ((x >> RDATA_WACS_FSM_SHIFT) & RDATA_WACS_FSM_MASK) !=
            WACS_FSM_IDLE;
}

