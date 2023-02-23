/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#pragma once

#include <compiler.h>
#include <kernel/mutex.h>
#include <lib/bio.h>
#include <stdbool.h>
#include <sys/types.h>

#define MMC_POWER_OFF       0
#define MMC_POWER_UP        1
#define MMC_POWER_ON        2

enum {
    EMMC_BOOT_PWR_RESET = 0,
    EMMC_BOOT_RST_N_SIG,
    EMMC_BOOT_PRE_IDLE_CMD
};

enum {
    RESP_NONE = 0,
    RESP_R1,
    RESP_R2,
    RESP_R3,
    RESP_R4,
    RESP_R5,
    RESP_R6,
    RESP_R7,
    RESP_R1B
};

struct mmc_csd {
    unsigned char  csd_struct;
    unsigned char  mmca_vsn;
    unsigned short cmdclass;            /* card command classes */
    unsigned int   max_dtr;             /* max. data transfer rate */
    unsigned int   read_blkbits;        /* max. read data block length */
    unsigned int   capacity;            /* card capacity */
};

struct mmc_ext_csd {
    unsigned int    sectors;
    unsigned int    hs_max_dtr;
    unsigned char   rev;
    unsigned char   boot_info;
    unsigned int    boot_part_sz;
    unsigned int    rpmb_sz;
    unsigned char   ddr_support;
    unsigned char   hs400_support;
    unsigned char   part_cfg;
    unsigned char   sec_support;
    unsigned char   reset_en;
    unsigned char   rel_wr_sec_c;
};

struct sd_scr {
    unsigned char   scr_struct;
    unsigned char   sda_vsn;
    unsigned char   data_bit_after_erase;
    unsigned char   security;
    unsigned char   bus_widths;
    unsigned char   sda_vsn3;
    unsigned char   ex_security;
    unsigned char   cmd_support;
};

#define SD_DRV_TYPE_B       (0)
#define SD_DRV_TYPE_A       (1<<0)
#define SD_DRV_TYPE_C       (1<<1)
#define SD_DRV_TYPE_D       (1<<2)

#define SD_MAX_CUR_200MA    (0)
#define SD_MAX_CUR_400MA    (1<<0)
#define SD_MAX_CUR_600MA    (1<<1)
#define SD_MAX_CUR_800MA    (1<<2)

struct sd_switch_caps {
    unsigned int    hs_max_dtr;
    unsigned int    ddr;
    unsigned int    drv_strength;
    unsigned int    max_cur;
    unsigned int    uhs_max_dtr;
    unsigned int    sd3_bus_mode;
};

#define MMC_CAP_4_BIT_DATA      (1U << 0) /* Can the host do 4 bit transfers */
#define MMC_CAP_MULTIWRITE      (1U << 1) /* Can accurately report bytes sent to card on error */
#define MMC_CAP_BYTEBLOCK       (1U << 2) /* Can do non-log2 block sizes */
#define MMC_CAP_MMC_HIGHSPEED   (1U << 3) /* Can do MMC high-speed timing */
#define MMC_CAP_SD_HIGHSPEED    (1U << 4) /* Can do SD high-speed timing */
#define MMC_CAP_8_BIT_DATA      (1U << 5) /* Can the host do 8 bit transfers */
#define MMC_CAP_SD_UHS1         (1U << 6) /* Can do SD ultra-high-speed timing */
#define MMC_CAP_DDR             (1U << 7) /* The host support dual data rate */
#define MMC_CAP_EMMC_HS200      (1U << 8) /* The host support dual data rate */
#define MMC_CAP_EMMC_HS400      (1U << 9) /* The host support dual data rate */

struct mmc_host {
    u8 id;
    struct mmc_card *card;
    u32 max_phys_segs;
    addr_t base;      /* host base address */
    addr_t base_top;  /* top pad control base address */
    u32 caps;         /* Host capabilities */
    u32 f_min;        /* host min. frequency */
    u32 f_max;        /* host max. frequency */
    u32 src_clk;      /* host clock speed */
    u32 cur_bus_clk;  /* SD/MS clock speed */
    u32 blklen;       /* block len */
    u32 ocr;          /* current ocr */
    u32 ocr_avail;    /* available ocr */
    u32 timeout_ns;   /* data timeout ns */
    u32 timeout_clks; /* data timeout clks */
    u8  pll_mux_clk;  /* the mux of system pll clock */
    u8  curr_part;    /* host current working partition */
    u32 intr_mask;    /* Interrupt mask */
    u32 cur_pwr;      /* current power voltage */
    u32  time_read;
    void *priv;       /* private data */
    mutex_t lock;     /* mutex lock for multi-thread */
    int (*blk_read)(struct mmc_host *host, u8 *dst, u32 src, u32 nblks);
    int (*blk_write)(struct mmc_host *host, u32 dst, u8 *src, u32 nblks);
};

#define MMC_TYPE_UNKNOWN    (0)          /* Unknown card */
#define MMC_TYPE_MMC        (0x00000001) /* MMC card */
#define MMC_TYPE_SD         (0x00000002) /* SD card */
#define MMC_TYPE_SDIO       (0x00000004) /* SDIO card */

/* MMC device */
struct mmc_card {
    struct mmc_host        *host;       /* the host this device belongs to */
    unsigned int            nblks;
    unsigned int            blklen;
    unsigned int            ocr;
    unsigned int            maxhz;
    unsigned int            uhs_mode;
    unsigned int            rca;        /* relative card address of device */
    unsigned int            type;       /* card type */
    unsigned int            sdio_funcs; /* number of SDIO functions */
    unsigned short          state;      /* (our) card state */
    unsigned short          ready;      /* card is ready or not */
    u32                     raw_csd[4]; /* raw card CSD */
    u32                     raw_scr[2]; /* raw card SCR */
    u8                      raw_ext_csd[512]; /* raw card EXT_CSD */
    struct mmc_csd          csd;        /* card specific */
    struct mmc_ext_csd      ext_csd;    /* mmc v4 extended card specific */
    struct sd_scr           scr;        /* extra SD information */
    struct sd_switch_caps   sw_caps;    /* switch (CMD6) caps */
    u8                      version;    /* the SD card version, 1.0, 2.0, or 3.0*/
};

struct mmc_command {
    u32 opcode;
    u32 arg;
    u32 rsptyp;
    u32 resp[4];
    u32 timeout;
    u32 retries;    /* max number of retries */
    u32 error;      /* command error */
};

struct mmc_data {
    u8  *buf;
    struct mmc_command *cmd;
    u32  blks;
    u32  timeout;   /* ms */
};

struct mmc_switch {
    u16 max_current;
    u16 grp6_info;
    u16 grp5_info;
    u16 grp4_info;
    u16 grp3_info;
    u16 grp2_info;
    u16 grp1_info;
    u8  grp6_result:4;
    u8  grp5_result:4;
    u8  grp4_result:4;
    u8  grp3_result:4;
    u8  grp2_result:4;
    u8  grp1_result:4;
    u8  ver;
    u16 grp6_busy;
    u16 grp5_busy;
    u16 grp4_busy;
    u16 grp3_busy;
    u16 grp2_busy;
    u16 grp1_busy;
    u8  rev[34];
};

#define mmc_card_mmc(c)             ((c)->type & MMC_TYPE_MMC)
#define mmc_card_sd(c)              ((c)->type & MMC_TYPE_SD)

#define mmc_card_set_host(c, h)      ((c)->host = (h))
#define mmc_card_set_unknown(c)     ((c)->type = MMC_TYPE_UNKNOWN)
#define mmc_card_set_mmc(c)         ((c)->type |= MMC_TYPE_MMC)
#define mmc_card_set_sd(c)          ((c)->type |= MMC_TYPE_SD)

#define mmc_card_present(c)         ((c)->state & MMC_STATE_PRESENT)
#define mmc_card_readonly(c)        ((c)->state & MMC_STATE_READONLY)
#define mmc_card_backyard(c)        ((c)->state & MMC_STATE_BACKYARD)
#define mmc_card_highspeed(c)       ((c)->state & MMC_STATE_HIGHSPEED)
#define mmc_card_uhs1(c)            ((c)->state & MMC_STATE_UHS1)
#define mmc_card_hs200(c)           ((c)->state & MMC_STATE_HS200)
#define mmc_card_hs400(c)           ((c)->state & MMC_STATE_HS400)
#define mmc_card_ddr(c)             ((c)->state & MMC_STATE_DDR)
#define mmc_card_blockaddr(c)       ((c)->state & MMC_STATE_BLOCKADDR)
#define mmc_card_highcaps(c)        ((c)->state & MMC_STATE_HIGHCAPS)

#define mmc_card_set_present(c)     ((c)->state |= MMC_STATE_PRESENT)
#define mmc_card_set_readonly(c)    ((c)->state |= MMC_STATE_READONLY)
#define mmc_card_set_backyard(c)    ((c)->state |= MMC_STATE_BACKYARD)
#define mmc_card_set_highspeed(c)   ((c)->state |= MMC_STATE_HIGHSPEED)
#define mmc_card_set_uhs1(c)        ((c)->state |= MMC_STATE_UHS1)
#define mmc_card_set_hs200(c)       ((c)->state |= MMC_STATE_HS200)
#define mmc_card_set_hs400(c)       ((c)->state |= MMC_STATE_HS400)
#define mmc_card_set_ddr(c)         ((c)->state |= MMC_STATE_DDR)
#define mmc_card_set_blockaddr(c)   ((c)->state |= MMC_STATE_BLOCKADDR)

#define mmc_card_clear_present(c)     ((c)->state &= ~MMC_STATE_PRESENT)
#define mmc_card_clear_readonly(c)    ((c)->state &= ~MMC_STATE_READONLY)
#define mmc_card_clear_highspeed(c)   ((c)->state &= ~MMC_STATE_HIGHSPEED)
#define mmc_card_clear_uhs1(c)        ((c)->state &= ~MMC_STATE_UHS1)
#define mmc_card_clear_hs200(c)       ((c)->state &= ~MMC_STATE_HS200)
#define mmc_card_clear_hs400(c)       ((c)->state &= ~MMC_STATE_HS400)
#define mmc_card_clear_ddr(c)         ((c)->state &= ~MMC_STATE_DDR)
#define mmc_card_clear_blockaddr(c)   ((c)->state &= ~MMC_STATE_BLOCKADDR)

#define mmc_card_clr_ddr(c)         ((c)->state &= ~MMC_STATE_DDR)
#define mmc_card_clr_speed_mode(c)  ((c)->state &= ~(MMC_STATE_HS400  \
        | MMC_STATE_HS200 | MMC_STATE_UHS1 | MMC_STATE_HIGHSPEED | MMC_STATE_BACKYARD))

#define mmc_card_name(c)            ((c)->cid.prod_name)

#define mmc_op_multi(op)    (((op) == MMC_CMD_READ_MULTIPLE_BLOCK) || \
        ((op) == MMC_CMD_WRITE_MULTIPLE_BLOCK))

struct mmc_card *emmc_init_stage1(bool *retry_opcond);
int emmc_init_stage2(struct mmc_card *card, bool retry_opcond);
int sdmmc_init(u8 id);
void emmc_init(void);
int mmc_dev_bwrite(struct mmc_card *card, unsigned long blknr,
    u32 blkcnt, const u8 *src);
int mmc_dev_bread(struct mmc_card *card, unsigned long blknr,
    u32 blkcnt, u8 *dst);
struct mmc_card *mmc_get_card(int id);
struct mmc_host *mmc_get_host(int id);
int mmc_set_part_config(struct mmc_card *card, u8 cfg);
#if defined(SDCARD_SUPPORT)
int sdcard_init(void);
int sdcard_deinit(void);
int mmc_get_sdcard_size(void);
#endif
