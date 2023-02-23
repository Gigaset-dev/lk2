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

#include <debug.h>
#include <platform/addressmap.h>
#include <pmic_wrap_common.h>

struct mt6779_pwrap_regs {
    u32 reserved1[776];
    u32 wacs2_cmd;
    u32 wacs2_rdata;
    u32 wacs2_vldclr;
    u32 reserved2[4];
};

static struct mt6779_pwrap_regs * const mtk_pwrap = (void *)PWRAP_BASE;

enum {
      WACS2 = 1 << 2
};

/* timeout setting */
enum {
      TIMEOUT_READ_US        = 255,
      TIMEOUT_WAIT_IDLE_US   = 255
};

enum {
      RDATA_WACS_RDATA_SHIFT = 0,
      RDATA_WACS_FSM_SHIFT   = 16,
      RDATA_WACS_REQ_SHIFT   = 19,
      RDATA_SYNC_IDLE_SHIFT  = 20,
      RDATA_INIT_DONE_SHIFT  = 22,
      RDATA_SYS_IDLE_SHIFT   = 23,
};

/* PMIC registers */
enum {
      PMIC_BASE                     = 0x0000,
      PMIC_SMT_CON1                 = PMIC_BASE + 0x0030,
      PMIC_DRV_CON1                 = PMIC_BASE + 0x0038,
      PMIC_FILTER_CON0              = PMIC_BASE + 0x0040,
      PMIC_GPIO_PULLEN0_CLR         = PMIC_BASE + 0x0098,
      PMIC_RG_SPI_CON0              = PMIC_BASE + 0x0408,
      PMIC_RG_SPI_RECORD0           = PMIC_BASE + 0x040A,
      PMIC_DEW_DIO_EN               = PMIC_BASE + 0x040C,
      PMIC_DEW_READ_TEST            = PMIC_BASE + 0x040E,
      PMIC_DEW_WRITE_TEST           = PMIC_BASE + 0x0410,
      PMIC_DEW_CRC_EN               = PMIC_BASE + 0x0414,
      PMIC_DEW_CRC_VAL              = PMIC_BASE + 0x0416,
      PMIC_DEW_RDDMY_NO             = PMIC_BASE + 0x0426,
      PMIC_CPU_INT_STA              = PMIC_BASE + 0x042E,
      PMIC_RG_SPI_CON2              = PMIC_BASE + 0x0432,
      PMIC_RG_SPI_CON3              = PMIC_BASE + 0x0434,
      PMIC_RG_SPI_CON4              = PMIC_BASE + 0x0436,
      PMIC_RG_SPI_CON5              = PMIC_BASE + 0x0438,
      PMIC_RG_SPI_CON6              = PMIC_BASE + 0x043A,
      PMIC_RG_SPI_CON7              = PMIC_BASE + 0x043C,
      PMIC_RG_SPI_CON8              = PMIC_BASE + 0x043E,
      PMIC_RG_SPI_CON13             = PMIC_BASE + 0x0448,
      PMIC_SPISLV_KEY               = PMIC_BASE + 0x044A,
      PMIC_PPCCTL0                  = PMIC_BASE + 0x0A08,
      PMIC_AUXADC_ADC17             = PMIC_BASE + 0x10AA,
      PMIC_AUXADC_ADC31             = PMIC_BASE + 0x10C6,
      PMIC_AUXADC_ADC32             = PMIC_BASE + 0x10C8,
      PMIC_AUXADC_ADC35             = PMIC_BASE + 0x10CE,
      PMIC_AUXADC_RQST0             = PMIC_BASE + 0x1108,
      PMIC_AUXADC_RQST1             = PMIC_BASE + 0x110A,
};

