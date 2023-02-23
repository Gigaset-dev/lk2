/*
 *
 * Copyright (c) 2022 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#pragma once

#include <cust_msdc.h>
#include <msdc_cfg.h>

void msdc_card_power(struct mmc_host *host, u32 on);
void msdc_host_power(struct mmc_host *host, u32 on, u32 level);
void msdc_power(struct mmc_host *host, u8 mode);
void sd_card_vccq_on(void);

void msdc_set_driving_by_id(u32 id, struct msdc_cust *msdc_cap);
void msdc_set_ies_by_id(u32 id, int set_ies);
void msdc_set_sr_by_id(u32 id, int clk, int cmd, int dat, int rst, int ds);
void msdc_set_smt_by_id(u32 id, int set_smt);
void msdc_set_tdsel_by_id(u32 id, u32 flag, u32 value);
void msdc_set_rdsel_by_id(u32 id, u32 flag, u32 value);
void msdc_get_tdsel_by_id(u32 id, u32 *value);
void msdc_get_rdsel_by_id(u32 id, u32 *value);
void msdc_dump_clock_sts(struct mmc_host *host);
void msdc_dump_ldo_sts(struct mmc_host *host);
void msdc_dump_padctl_by_id(u32 id);
void msdc_pin_config_by_id(u32 id, u32 mode);
void msdc_set_pin_mode(struct mmc_host *host);
void msdc_set_tdsel_wrap(struct mmc_host *host);
void msdc_set_rdsel_wrap(struct mmc_host *host);
void msdc_gpio_and_pad_init(struct mmc_host *host);
void msdc_hs400_ds_delay(struct mmc_host *host);
int EFUSE_IS_EMMC_BOOT_BUS_WIDTH_8BIT(void);

/*******************************************************************************
 * CLOCK definition
 ******************************************************************************/
#ifdef FPGA_PLATFORM
#define MSDC_OP_SCLK            (10000000)
#define MSDC_SRC_CLK            (10000000)
#define MSDC_MAX_SCLK           (MSDC_SRC_CLK>>1)
#else
#define MSDC_OP_SCLK            (200000000)
#define MSDC_MAX_SCLK           (200000000)
#endif

#define MSDC_MIN_SCLK           (100000)

void msdc_clock(struct mmc_host *host, int on);
void msdc_config_clksrc(struct mmc_host *host, int clksrc);

#ifdef FPGA_PLATFORM
#define msdc_get_hclk(host, src)        MSDC_SRC_CLK
#else
extern u32 hclks_msdc0[];
extern u32 hclks_msdc1[];
extern u32 *hclks_msdc_all[];
#define msdc_get_hclk(id, src)          hclks_msdc_all[id][src]
#endif

/*******************************************************************************
 * Power Definition
 ******************************************************************************/
enum MSDC_POWER_VOL {
    VOL_DEFAULT,
    VOL_0900 = 900,
    VOL_1000 = 1000,
    VOL_1100 = 1100,
    VOL_1200 = 1200,
    VOL_1300 = 1300,
    VOL_1350 = 1350,
    VOL_1500 = 1500,
    VOL_1800 = 1800,
    VOL_2000 = 2000,
    VOL_2100 = 2100,
    VOL_2500 = 2500,
    VOL_2800 = 2800,
    VOL_2900 = 2900,
    VOL_3000 = 3000,
    VOL_3300 = 3300,
    VOL_3400 = 3400,
    VOL_3500 = 3500,
    VOL_3600 = 3600
};

enum MSDC_POWER {
    MSDC_VMC,
    MSDC_VMCH,
    MSDC_VEMC,
};

#define EMMC_VOL_ACTUAL         VOL_3000
#define SD_VOL_ACTUAL           VOL_3000

#define MSDC_PB0_DEFAULT        0x403C0007
#define MSDC_PB1_DEFAULT        0xFFE64309
#define MSDC_PB2_DEFAULT        0x14881803
