/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#pragma once

#define MSDC_CD_PIN_EN      (1U << 0)  /* card detection pin is wired   */
#define MSDC_WP_PIN_EN      (1U << 1)  /* write protection pin is wired */
#define MSDC_RST_PIN_EN     (1U << 2)  /* emmc reset pin is wired       */
#define MSDC_SDIO_IRQ       (1U << 3)  /* use internal sdio irq (bus)   */
#define MSDC_EXT_SDIO_IRQ   (1U << 4)  /* use external sdio irq         */
#define MSDC_REMOVABLE      (1U << 5)  /* removable slot                */
#define MSDC_SYS_SUSPEND    (1U << 6)  /* suspended by system           */
#define MSDC_HIGHSPEED      (1U << 7)  /* high-speed mode support       */
#define MSDC_UHS1           (1U << 8)  /* uhs-1 mode support            */
#define MSDC_DDR            (1U << 9)  /* ddr mode support              */
#define MSDC_HS200          (1U << 10) /* hs200 mode support(eMMC4.5)   */
#define MSDC_HS400          (1U << 11) /* hs200 mode support(eMMC5.0)   */

#define MSDC_SMPL_RISING        (0)
#define MSDC_SMPL_FALLING       (1)
#define MSDC_SMPL_SEPARATE      (2)


struct msdc_cust {
    unsigned char  clk_src;           /* host clock source             */
    unsigned char  cmd_edge;          /* command latch edge            */
    unsigned char  data_edge;         /* data latch edge               */
    unsigned char  clk_drv;           /* clock pad driving             */
    unsigned char  cmd_drv;           /* command pad driving           */
    unsigned char  dat_drv;           /* data pad driving              */
    unsigned char  rst_drv;           /* reset pin pad driving         */
    unsigned char  ds_drv;            /* ds pad driving                */
    unsigned char  data_pins;         /* data pins                     */
    unsigned int   data_offset;       /* data address offset           */
    unsigned int   flags;             /* hardware capability flags     */
};

struct msdc_cust get_msdc_capability(unsigned int id);
