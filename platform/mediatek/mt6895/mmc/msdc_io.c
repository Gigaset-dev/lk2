/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#include <cust_msdc.h>
#include <err.h>
#include <errno.h>
#include <kernel/event.h>
#include <lib/kcmdline.h>
#include <lk/init.h>
#include <mmc_core.h>
#include <msdc_io_layout.h>
#include <stdbool.h>

#include "msdc_io.h"

#define MTK_MSDC_BRINGUP_DEBUG

#ifdef MSDC1_EMMC

#define CMDLINE_BUF_SIZE 128

/*
 * Setting kernel command line of boot device
 */
void mmc_commandline_bootdevice(void)
{
    int ret = 0;
    char cmdline_buf[CMDLINE_BUF_SIZE];
#ifndef MSDC1_EMMC
    ret = snprintf(cmdline_buf, CMDLINE_BUF_SIZE,
        "loop.max_part=7 androidboot.boot_devices=bootdevice,soc/%08zx.mmc,%08zx.mmc",
            (MSDC0_BASE - KERNEL_ASPACE_BASE), (MSDC0_BASE - KERNEL_ASPACE_BASE));
#else
    ret = snprintf(cmdline_buf, CMDLINE_BUF_SIZE,
        "loop.max_part=7 androidboot.boot_devices=bootdevice,soc/%08zx.mmc,%08zx.mmc",
            (MSDC1_BASE - KERNEL_ASPACE_BASE), (MSDC1_BASE - KERNEL_ASPACE_BASE));
#endif
    if (ret < 0) {
        dprintf(ALWAYS, "Partition append bootdevice to command line fail");
        return;
    }

    ret = kcmdline_append(cmdline_buf);
    if (ret != NO_ERROR) {
        dprintf(ALWAYS, "kcmdline append %s failed, rc=%d\n", cmdline_buf, ret);
        return;
    }
    dprintf(ALWAYS, "kcmdline append %s\n", cmdline_buf);
    return;
}

#endif

////////////////////////////////////////////////////////////////////////////////
//
// Section 1. Power Control -- Common for ASIC and FPGA
//
////////////////////////////////////////////////////////////////////////////////
#ifndef FPGA_PLATFORM
void msdc_dump_ldo_sts(struct mmc_host *host)
{
}
#endif

void msdc_card_power(struct mmc_host *host, u32 on)
{
    /* power is default on */
}

void msdc_host_power(struct mmc_host *host, u32 on, u32 level)
{
    /* power is default on */
}

void msdc_power(struct mmc_host *host, u8 mode)
{
    if (mode == MMC_POWER_ON || mode == MMC_POWER_UP) {
        msdc_pin_config(host, MSDC_PIN_PULL_UP);
        msdc_host_power(host, 1, host->cur_pwr);
        msdc_card_power(host, 1);
        msdc_clock(host, 1);
    } else {
        msdc_clock(host, 0);
        msdc_host_power(host, 0, host->cur_pwr);
        msdc_card_power(host, 0);
        msdc_pin_config(host, MSDC_PIN_PULL_DOWN);
    }
}

void sd_card_vccq_on(void)
{
    /* Need porting if needed */
}

/**************************************************************/
/* Section 2: Clock                    */
/**************************************************************/
#ifdef FPGA_PLATFORM
void msdc_config_clksrc(struct mmc_host *host, int clksrc)
{
    host->src_clk = msdc_get_hclk(host->id, clksrc);
}

#else

/* msdc1 clock source reference value is 200M */
u32 hclks_msdc1[] = { MSDC1_SRC_0, MSDC1_SRC_1, MSDC1_SRC_2, MSDC1_SRC_3, MSDC1_SRC_4 };

u32 *hclks_msdc_all[] = {
    NULL,
    hclks_msdc1,
};

void msdc_dump_clock_sts(struct mmc_host *host)
{
}

/* perloader will pre-set msdc pll and the mux channel of msdc pll */
/* note: pll will not changed */
void msdc_config_clksrc(struct mmc_host *host, int clksrc)
{
    host->pll_mux_clk = MSDC1_CLKSRC_DEFAULT;
    host->src_clk = msdc_get_hclk(host->id, MSDC1_CLKSRC_DEFAULT);
}

#endif

void msdc_clock(struct mmc_host *host, int on)
{
    /* clock is default on */
    msdc_dump_clock_sts(host);
}


/**************************************************************/
/* Section 3: GPIO and Pad                    */
/**************************************************************/
#ifndef FPGA_PLATFORM
void msdc_dump_padctl_by_id(u32 id)
{
}

void msdc_set_pin_mode(struct mmc_host *host)
{
}

void msdc_set_ies_by_id(u32 id, int set_ies)
{
}

void msdc_set_smt_by_id(u32 id, int set_smt)
{
}

void msdc_set_tdsel_by_id(u32 id, u32 flag, u32 value)
{
}

void msdc_set_rdsel_by_id(u32 id, u32 flag, u32 value)
{
}

void msdc_get_tdsel_by_id(u32 id, u32 *value)
{
}

void msdc_get_rdsel_by_id(u32 id, u32 *value)
{
}

void msdc_set_sr_by_id(u32 id, int clk, int cmd, int dat, int rst, int ds)
{
}

void msdc_set_driving_by_id(u32 id, struct msdc_cust *msdc_cap)
{
}

void msdc_pin_config_by_id(u32 id, u32 mode)
{
}

/**************************************************************/
/* Section 4: MISC                                            */
/**************************************************************/
/* make sure the pad is msdc mode */
void msdc_set_tdsel_wrap(struct mmc_host *host)
{
    if (host->cur_pwr == VOL_1800)
        msdc_set_tdsel_by_id(host->id, MSDC_TDRDSEL_1V8, 0);
    else
        msdc_set_tdsel_by_id(host->id, MSDC_TDRDSEL_3V, 0);
}

void msdc_set_rdsel_wrap(struct mmc_host *host)
{
    if (host->cur_pwr == VOL_1800)
        msdc_set_rdsel_by_id(host->id, MSDC_TDRDSEL_1V8, 0);
    else
        msdc_set_rdsel_by_id(host->id, MSDC_TDRDSEL_3V, 0);
}

void msdc_gpio_and_pad_init(struct mmc_host *host)
{
    struct msdc_cust msdc_cap = get_msdc_capability(host->id);

    /* set smt enable */
    msdc_set_smt(host, 1);

    /* set pull enable */
    msdc_pin_config(host, MSDC_PIN_PULL_UP);

    /* set gpio to msdc mode */
    msdc_set_pin_mode(host);

    /* set ies enable */
    msdc_set_ies(host, 1);

    /* set driving */
    msdc_set_driving(host, &msdc_cap);

    /* set tdsel and rdsel */
    msdc_set_tdsel_wrap(host);
    msdc_set_rdsel_wrap(host);

    msdc_dump_padctl_by_id(host->id);
}

/* Apply HS400 DS delay */
void msdc_hs400_ds_delay(struct mmc_host *host)
{
    writel(0x14814, TOP_EMMC50_PAD_DS_TUNE); /* Apply 2731 DS delay setting */
}

/* Efuse to decide 8bit or 1bit booting */
int EFUSE_IS_EMMC_BOOT_BUS_WIDTH_8BIT(void)
{
    return 1;
}
#endif
