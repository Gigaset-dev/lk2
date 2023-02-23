/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#pragma once

#define MMC_BLOCK_BITS_SHFT             (9)
#define MMC_BLOCK_SIZE                  (1 << MMC_BLOCK_BITS_SHFT)
#define MMC_MAX_BLOCK_SIZE              (1 << MMC_BLOCK_BITS_SHFT)

#define SDIO_MAX_FUNCS                  (7)

#define SD_CMD_BIT                      (1 << 7)
#define SD_CMD_APP_BIT                  (1 << 8)
#define SD_CMD_AUTO_BIT                 (1 << 9)

/* MMC command numbers */
#define MMC_CMD_GO_IDLE_STATE           (0)              /* bc.   no response */
#define MMC_CMD_SEND_OP_COND            (1)              /* bcr.  R3          */
#define MMC_CMD_ALL_SEND_CID            (2)              /* bcr.  R2          */
#define MMC_CMD_SET_RELATIVE_ADDR       (3)              /* ac.   R1          */
#define MMC_CMD_SET_DSR                 (4)              /* bc.   no response */
#define MMC_CMD_SLEEP_AWAKE             (5)              /* ac.   R1b         */
#define MMC_CMD_SWITCH                  (6)              /* ac.   R1b         */
#define MMC_CMD_SELECT_CARD             (7)              /* ac.   R1/R1b      */
#define MMC_CMD_SEND_EXT_CSD            (8)              /* adtc. R1          */
#define MMC_CMD_SEND_CSD                (9)              /* ac.   R2          */
#define MMC_CMD_SEND_CID                (10)             /* ac.   R2          */
#define MMC_CMD_READ_DAT_UNTIL_STOP     (11)             /* adtc. R1          */
#define MMC_CMD_STOP_TRANSMISSION       (12)             /* ac.   R1/R1b      */
#define MMC_CMD_SEND_STATUS             (13)             /* ac.   R1          */
#define MMC_CMD_BUSTEST_R               (14)             /* adtc. R1          */
#define MMC_CMD_GO_INACTIVE_STATE       (15)             /* ac.   no response */
#define MMC_CMD_SET_BLOCKLEN            (16)             /* ac.   R1          */
#define MMC_CMD_READ_SINGLE_BLOCK       (17)             /* adtc. R1          */
#define MMC_CMD_READ_MULTIPLE_BLOCK     (18)             /* adtc. R1          */
#define MMC_CMD_BUSTEST_W               (19)             /* adtc. R1          */
#define MMC_CMD_WRITE_DAT_UNTIL_STOP    (20)             /* adtc. R1          */
#define MMC_CMD21                       (21)             /* adtc. R1  Sandisk  */
#define MMC_CMD_SET_BLOCK_COUNT         (23)             /* ac.   R1          */
#define MMC_CMD_WRITE_BLOCK             (24)             /* adtc. R1          */
#define MMC_CMD_WRITE_MULTIPLE_BLOCK    (25)             /* adtc. R1          */
#define MMC_CMD_PROGRAM_CID             (26)             /* adtc. R1          */
#define MMC_CMD_PROGRAM_CSD             (27)             /* adtc. R1          */

#define MMC_CMD_SET_WRITE_PROT          (28)             /* ac.   R1b         */
#define MMC_CMD_CLR_WRITE_PROT          (29)             /* ac.   R1b         */
#define MMC_CMD_SEND_WRITE_PROT         (30)             /* adtc. R1          */
#define MMC_CMD_SEND_WRITE_PROT_TYPE    (31)             /* adtc. R1          */
#define MMC_CMD_ERASE_WR_BLK_START      (32)
#define MMC_CMD_ERASE_WR_BLK_END        (33)
#define MMC_CMD_ERASE_GROUP_START       (35)             /* ac.   R1          */
#define MMC_CMD_ERASE_GROUP_END         (36)             /* ac.   R1          */
#define MMC_CMD_ERASE                   (38)             /* ac.   R1b         */
#define MMC_CMD_FAST_IO                 (39)             /* ac.   R4          */
#define MMC_CMD_GO_IRQ_STATE            (40)             /* bcr.  R5          */
#define MMC_CMD_LOCK_UNLOCK             (42)             /* adtc. R1          */
#define MMC_CMD50                       (50)             /* adtc. R1 Sandisk */
#define MMC_CMD_APP_CMD                 (55)             /* ac.   R1          */
#define MMC_CMD_GEN_CMD                 (56)             /* adtc. R1          */

/* SD Card command numbers */
#define SD_CMD_SEND_RELATIVE_ADDR       (3 | SD_CMD_BIT)
#define SD_CMD_SWITCH                   (6 | SD_CMD_BIT)
#define SD_CMD_SEND_IF_COND             (8 | SD_CMD_BIT)
#define SD_CMD_VOL_SWITCH               (11 | SD_CMD_BIT)
#define SD_CMD_SEND_TUNING_BLOCK        (19 | SD_CMD_BIT)
#define SD_CMD_SPEED_CLASS_CTRL         (20 | SD_CMD_BIT)

#define SD_ACMD_SET_BUSWIDTH            (6  | SD_CMD_APP_BIT)
#define SD_ACMD_SD_STATUS               (13 | SD_CMD_APP_BIT)
#define SD_ACMD_SEND_NR_WR_BLOCKS       (22 | SD_CMD_APP_BIT)
#define SD_ACMD_SET_WR_ERASE_CNT        (23 | SD_CMD_APP_BIT)
#define SD_ACMD_SEND_OP_COND            (41 | SD_CMD_APP_BIT)
#define SD_ACMD_SET_CLR_CD              (42 | SD_CMD_APP_BIT)
#define SD_ACMD_SEND_SCR                (51 | SD_CMD_APP_BIT)

/* SDIO Card command numbers */
#define SD_IO_SEND_OP_COND              (5 | SD_CMD_BIT) /* bcr. R4           */
#define SD_IO_RW_DIRECT                 (52 | SD_CMD_BIT)/* ac.  R5           */
#define SD_IO_RW_EXTENDED               (53 | SD_CMD_BIT)/* adtc. R5          */

/* platform dependent command */
#define SD_ATOCMD_STOP_TRANSMISSION     (12 | SD_CMD_AUTO_BIT)
#define SD_ATOCMD_SET_BLOCK_COUNT       (23 | SD_CMD_AUTO_BIT)

#define MMC_VDD_145_150 0x00000001  /* VDD voltage 1.45 - 1.50 */
#define MMC_VDD_150_155 0x00000002  /* VDD voltage 1.50 - 1.55 */
#define MMC_VDD_155_160 0x00000004  /* VDD voltage 1.55 - 1.60 */
#define MMC_VDD_160_165 0x00000008  /* VDD voltage 1.60 - 1.65 */
#define MMC_VDD_165_170 0x00000010  /* VDD voltage 1.65 - 1.70 */
//Add this line by reference to include/linux/mmc/host.h in linux kernel
#define MMC_VDD_165_195 0x00000080  /* VDD voltage 1.65 - 1.95 */
#define MMC_VDD_17_18   0x00000020  /* VDD voltage 1.7 - 1.8 */
#define MMC_VDD_18_19   0x00000040  /* VDD voltage 1.8 - 1.9 */
#define MMC_VDD_19_20   0x00000080  /* VDD voltage 1.9 - 2.0 */
#define MMC_VDD_20_21   0x00000100  /* VDD voltage 2.0 ~ 2.1 */
#define MMC_VDD_21_22   0x00000200  /* VDD voltage 2.1 ~ 2.2 */
#define MMC_VDD_22_23   0x00000400  /* VDD voltage 2.2 ~ 2.3 */
#define MMC_VDD_23_24   0x00000800  /* VDD voltage 2.3 ~ 2.4 */
#define MMC_VDD_24_25   0x00001000  /* VDD voltage 2.4 ~ 2.5 */
#define MMC_VDD_25_26   0x00002000  /* VDD voltage 2.5 ~ 2.6 */
#define MMC_VDD_26_27   0x00004000  /* VDD voltage 2.6 ~ 2.7 */
#define MMC_VDD_27_28   0x00008000  /* VDD voltage 2.7 ~ 2.8 */
#define MMC_VDD_27_36   0x00FF8000  /* VDD voltage 2.7 ~ 3.6 */
#define MMC_VDD_28_29   0x00010000  /* VDD voltage 2.8 ~ 2.9 */
#define MMC_VDD_29_30   0x00020000  /* VDD voltage 2.9 ~ 3.0 */
#define MMC_VDD_30_31   0x00040000  /* VDD voltage 3.0 ~ 3.1 */
#define MMC_VDD_31_32   0x00080000  /* VDD voltage 3.1 ~ 3.2 */
#define MMC_VDD_32_33   0x00100000  /* VDD voltage 3.2 ~ 3.3 */
#define MMC_VDD_33_34   0x00200000  /* VDD voltage 3.3 ~ 3.4 */
#define MMC_VDD_34_35   0x00400000  /* VDD voltage 3.4 ~ 3.5 */
#define MMC_VDD_35_36   0x00800000  /* VDD voltage 3.5 ~ 3.6 */
#define MMC_CARD_BUSY   0x80000000  /* Card Power up status bit */

#define EMMC_VER_50   (50)
#define EMMC_VER_45   (45)
#define EMMC_VER_44   (44)
#define EMMC_VER_43   (43)
#define EMMC_VER_42   (42)
#define SD_VER_10     (10)
#define SD_VER_20     (20)
#define SD_VER_30     (30)

#define MMC_ERR_NONE          0
#define MMC_ERR_TIMEOUT       1
#define MMC_ERR_BADCRC        2
#define MMC_ERR_FIFO          3
#define MMC_ERR_FAILED        4
#define MMC_ERR_INVALID       5
#define MMC_ERR_CMDTUNEFAIL   6
#define MMC_ERR_READTUNEFAIL  7
#define MMC_ERR_WRITETUNEFAIL 8
#define MMC_ERR_CMD_TIMEOUT   9
#define MMC_ERR_CMD_RSPCRC    10
#define MMC_ERR_ACMD_TIMEOUT  11
#define MMC_ERR_ACMD_RSPCRC   12
#define MMC_ERR_AXI_RSPCRC    13
#define MMC_ERR_UNEXPECT      14
#define MMC_ERR_RETRY         15

#define MMC_BUS_WIDTH_1     0
#define MMC_BUS_WIDTH_4     2

#define SD_BUS_WIDTH_1      0
#define SD_BUS_WIDTH_4      2

#define MMC_STATE_PRESENT       (1<<0)      /* present in sysfs */
#define MMC_STATE_READONLY      (1<<1)      /* card is read-only */
#define MMC_STATE_HIGHSPEED     (1<<2)      /* card is in high speed mode */
#define MMC_STATE_BLOCKADDR     (1<<3)      /* card uses block-addressing */
#define MMC_STATE_HIGHCAPS      (1<<4)
#define MMC_STATE_UHS1          (1<<5)      /* card is in ultra high speed mode */
#define MMC_STATE_DDR           (1<<6)      /* card is in ddr mode */
#define MMC_STATE_HS200         (1<<7)
#define MMC_STATE_HS400         (1<<8)
#define MMC_STATE_BACKYARD      (1<<9)

#define R1_OUT_OF_RANGE         (1UL << 31) /* er, c */
#define R1_ADDRESS_ERROR        (1 << 30)   /* erx, c */
#define R1_BLOCK_LEN_ERROR      (1 << 29)   /* er, c */
#define R1_ERASE_SEQ_ERROR      (1 << 28)   /* er, c */
#define R1_ERASE_PARAM          (1 << 27)   /* ex, c */
#define R1_WP_VIOLATION         (1 << 26)   /* erx, c */
#define R1_CARD_IS_LOCKED       (1 << 25)   /* sx, a */
#define R1_LOCK_UNLOCK_FAILED   (1 << 24)   /* erx, c */
#define R1_COM_CRC_ERROR        (1 << 23)   /* er, b */
#define R1_ILLEGAL_COMMAND      (1 << 22)   /* er, b */
#define R1_CARD_ECC_FAILED      (1 << 21)   /* ex, c */
#define R1_CC_ERROR             (1 << 20)   /* erx, c */
#define R1_ERROR                (1 << 19)   /* erx, c */
#define R1_UNDERRUN             (1 << 18)   /* ex, c */
#define R1_OVERRUN              (1 << 17)   /* ex, c */
#define R1_CID_CSD_OVERWRITE    (1 << 16)   /* erx, c, CID/CSD overwrite */
#define R1_WP_ERASE_SKIP        (1 << 15)   /* sx, c */
#define R1_CARD_ECC_DISABLED    (1 << 14)   /* sx, a */
#define R1_ERASE_RESET          (1 << 13)   /* sr, c */
#define R1_STATUS(x)            (x & 0xFFFFE000)
#define R1_CURRENT_STATE(x)     ((x & 0x00001E00) >> 9) /* sx, b (4 bits) */
#define R1_READY_FOR_DATA       (1 << 8)    /* sx, a */
#define R1_SWITCH_ERROR         (1 << 7)    /* ex, b */
#define R1_URGENT_BKOPS         (1 << 6)    /* sr, a */
#define R1_APP_CMD              (1 << 5)    /* sr, c */

/*
 * Card Command Classes (CCC)
 */
#define CCC_BASIC               (1<<0)  /* (0) Basic protocol functions */
/* (CMD0,1,2,3,4,7,9,10,12,13,15) */
#define CCC_STREAM_READ         (1<<1)  /* (1) Stream read commands */
/* (CMD11) */
#define CCC_BLOCK_READ          (1<<2)  /* (2) Block read commands */
/* (CMD16,17,18) */
#define CCC_STREAM_WRITE        (1<<3)  /* (3) Stream write commands */
/* (CMD20) */
#define CCC_BLOCK_WRITE         (1<<4)  /* (4) Block write commands */
/* (CMD16,24,25,26,27) */
#define CCC_ERASE               (1<<5)  /* (5) Ability to erase blocks */
/* (CMD32,33,34,35,36,37,38,39) */
#define CCC_WRITE_PROT          (1<<6)  /* (6) Able to write protect blocks */
/* (CMD28,29,30) */
#define CCC_LOCK_CARD           (1<<7)  /* (7) Able to lock down card */
/* (CMD16,CMD42) */
#define CCC_APP_SPEC            (1<<8)  /* (8) Application specific */
/* (CMD55,56,57,ACMD*) */
#define CCC_IO_MODE             (1<<9)  /* (9) I/O mode */
/* (CMD5,39,40,52,53) */
#define CCC_SWITCH              (1<<10) /* (10) High speed switch */
/* (CMD6,34,35,36,37,50) */
/* (11) Reserved */
/* (CMD?) */

/*
 * CSD field definitions
 */

#define CSD_STRUCT_VER_1_0  0   /* Valid for system specification 1.0 - 1.2 */
#define CSD_STRUCT_VER_1_1  1   /* Valid for system specification 1.4 - 2.2 */
#define CSD_STRUCT_VER_1_2  2   /* Valid for system specification 3.1 - 3.2 - 3.31 - 4.0 - 4.1 */
#define CSD_STRUCT_EXT_CSD  3   /* Version is coded in CSD_STRUCTURE in EXT_CSD */

#define CSD_SPEC_VER_0      0   /* Implements system specification 1.0 - 1.2 */
#define CSD_SPEC_VER_1      1   /* Implements system specification 1.4 */
#define CSD_SPEC_VER_2      2   /* Implements system specification 2.0 - 2.2 */
#define CSD_SPEC_VER_3      3   /* Implements system specification 3.1 - 3.2 - 3.31 */
#define CSD_SPEC_VER_4      4   /* Implements system specification 4.0 - 4.1 */

/*
 * EXT_CSD fields
 */

#define EXT_CSD_BADBLK_MGMT             134 /* R/W */
#define EXT_CSD_ENH_START_ADDR          136 /* R/W 4 bytes */
#define EXT_CSD_ENH_SIZE_MULT           140 /* R/W 3 bytes */
#define EXT_CSD_GP1_SIZE_MULT           143 /* R/W 3 bytes */
#define EXT_CSD_GP2_SIZE_MULT           146 /* R/W 3 bytes */
#define EXT_CSD_GP3_SIZE_MULT           149 /* R/W 3 bytes */
#define EXT_CSD_GP4_SIZE_MULT           152 /* R/W 3 bytes */
#define EXT_CSD_PART_SET_COMPL          155 /* R/W */
#define EXT_CSD_PART_ATTR               156 /* R/W 3 bytes */
#define EXT_CSD_MAX_ENH_SIZE_MULT       157 /* R/W 3 bytes */
#define EXT_CSD_PART_SUPPORT            160 /* R */
#define EXT_CSD_HPI_MGMT                161 /* R/W/E_P (4.41) */
#define EXT_CSD_RST_N_FUNC              162 /* R/W */
#define EXT_CSD_BKOPS_EN                163 /* R/W (4.41) */
#define EXT_CSD_BKOPS_START             164 /* W/E_P (4.41) */
#define EXT_CSD_WR_REL_PARAM            166 /* R (4.41) */
#define EXT_CSD_WR_REL_SET              167 /* R/W (4.41) */
#define EXT_CSD_RPMB_SIZE_MULT          168 /* R */
#define EXT_CSD_FW_CONFIG               169 /* R/W */
#define EXT_CSD_USR_WP                  171 /* R/W, R/W/C_P & R/W/E_P */
#define EXT_CSD_BOOT_WP                 173 /* R/W, R/W/C_P */
#define EXT_CSD_ERASE_GRP_DEF           175 /* R/W/E */
#define EXT_CSD_BOOT_BUS_WIDTH          177 /* R/W/E */
#define EXT_CSD_BOOT_CONFIG_PROT        178 /* R/W & R/W/C_P */
#define EXT_CSD_PART_CFG                179 /* R/W/E & R/W/E_P */
#define EXT_CSD_ERASED_MEM_CONT         181 /* R */
#define EXT_CSD_BUS_WIDTH               183 /* R/W */
#define EXT_CSD_HS_TIMING               185 /* R/W */
#define EXT_CSD_PWR_CLASS               187 /* R/W/E_P */
#define EXT_CSD_CMD_SET_REV             189 /* R */
#define EXT_CSD_CMD_SET                 191 /* R/W/E_P */
#define EXT_CSD_REV                     192 /* R */
#define EXT_CSD_STRUCT                  194 /* R */
#define EXT_CSD_CARD_TYPE               196 /* RO */
#define EXT_CSD_OUT_OF_INTR_TIME        198 /* R (4.41) */
#define EXT_CSD_PART_SWITCH_TIME        199 /* R (4.41) */
#define EXT_CSD_PWR_CL_52_195           200 /* R */
#define EXT_CSD_PWR_CL_26_195           201 /* R */
#define EXT_CSD_PWR_CL_52_360           202 /* R */
#define EXT_CSD_PWR_CL_26_360           203 /* R */
#define EXT_CSD_MIN_PERF_R_4_26         205 /* R */
#define EXT_CSD_MIN_PERF_W_4_26         206 /* R */
#define EXT_CSD_MIN_PERF_R_8_26_4_25    207 /* R */
#define EXT_CSD_MIN_PERF_W_8_26_4_25    208 /* R */
#define EXT_CSD_MIN_PERF_R_8_52         209 /* R */
#define EXT_CSD_MIN_PERF_W_8_52         210 /* R */
#define EXT_CSD_SEC_CNT                 212 /* RO, 4 bytes */
#define EXT_CSD_S_A_TIMEOUT             217 /* R */
#define EXT_CSD_S_C_VCCQ                219 /* R */
#define EXT_CSD_S_C_VCC                 220 /* R */
#define EXT_CSD_HC_WP_GPR_SIZE          221 /* R */
#define EXT_CSD_REL_WR_SEC_C            222 /* R */
#define EXT_CSD_ERASE_TIMEOUT_MULT      223 /* R */
#define EXT_CSD_HC_ERASE_GRP_SIZE       224 /* R */
#define EXT_CSD_ACC_SIZE                225 /* R */
#define EXT_CSD_BOOT_SIZE_MULT          226 /* R */
#define EXT_CSD_BOOT_INFO               228 /* R */
#define EXT_CSD_SEC_TRIM_MULT           229 /* R */
#define EXT_CSD_SEC_ERASE_MULT          230 /* R */
#define EXT_CSD_SEC_FEATURE_SUPPORT     231 /* R */
#define EXT_CSD_TRIM_MULT               232 /* R */
#define EXT_CSD_MIN_PERF_DDR_R_8_52     234 /* R */
#define EXT_CSD_MIN_PERF_DDR_W_8_52     235 /* R */
#define EXT_CSD_PWR_CL_DDR_52_195       238 /* R */
#define EXT_CSD_PWR_CL_DDR_52_360       239 /* R */
#define EXT_CSD_INI_TIMEOUT_AP          241 /* R */
#define EXT_CSD_CORRECT_PRG_SECTS_NUM   242 /* R, 4 bytes (4.41) */
#define EXT_CSD_BKOPS_STATUS            246 /* R (4.41) */
#define EXT_CSD_BKOPS_SUPP              502 /* R (4.41) */
#define EXT_CSD_HPI_FEATURE             503 /* R (4.41) */
#define EXT_CSD_S_CMD_SET               504 /* R */

/*
 * EXT_CSD field definitions
 */

/* SEC_FEATURE_SUPPORT[231] */
#define EXT_CSD_SEC_FEATURE_ER_EN       (1<<0)
#define EXT_CSD_SEC_FEATURE_BD_BLK_EN   (1<<2)
#define EXT_CSD_SEC_FEATURE_GB_CL_EN    (1<<4)

/* BOOT_INFO[228] */
#define EXT_CSD_BOOT_INFO_ALT_BOOT      (1<<0)
#define EXT_CSD_BOOT_INFO_DDR_BOOT      (1<<1)
#define EXT_CSD_BOOT_INFO_HS_BOOT       (1<<2)

#define EXT_CSD_CMD_SET_NORMAL          (1<<0)
#define EXT_CSD_CMD_SET_SECURE          (1<<1)
#define EXT_CSD_CMD_SET_CPSECURE        (1<<2)

#define EXT_CSD_CARD_TYPE_26            (1<<0)  /* Card can run at 26MHz */
#define EXT_CSD_CARD_TYPE_52            (1<<1)  /* Card can run at 52MHz */
#define EXT_CSD_CARD_TYPE_DDR_52        (1<<2)  /* Card can run at DDR 52MHz@1.8V or 3V */
#define EXT_CSD_CARD_TYPE_DDR_52_1_2V   (1<<3)  /* Card can run at DDR 52MHz@1.2V */
#define EXT_CSD_CARD_TYPE_HS200_1_8V    (1<<4)  /* Card can run at 200MHz@ 1.8V*/
#define EXT_CSD_CARD_TYPE_HS200_1_2V    (1<<5)  /* Card can run at 200MHz@ 1.2V*/
#define EXT_CSD_CARD_TYPE_HS400_1_8V    (1<<6)  /* Card can run at 200MHz@ 1.8V*/
#define EXT_CSD_CARD_TYPE_HS400_1_2V    (1<<7)  /* Card can run at 200MHz@ 1.2V*/

/* BUS_WIDTH[183] */
#define EXT_CSD_BUS_WIDTH_1             (0) /* Card is in 1 bit mode */
#define EXT_CSD_BUS_WIDTH_4             (1) /* Card is in 4 bit mode */
#define EXT_CSD_BUS_WIDTH_8             (2) /* Card is in 8 bit mode */
#define EXT_CSD_BUS_WIDTH_4_DDR         (5) /* Card is in 4 bit mode + DDR */
#define EXT_CSD_BUS_WIDTH_8_DDR         (6) /* Card is in 8 bit mode + DDR */

/* high speed timing */
#define EXT_CSD_HS_TIMEING_BACKWARDS    (0) /* selecting backwards compatibility interface timing */
#define EXT_CSD_HS_TIMEING_HS           (1) /* selecting high speed  interface timing */
#define EXT_CSD_HS_TIMEING_HS200        (2) /* selecting hs200 interface timing */
#define EXT_CSD_HS_TIMEING_HS400        (3) /* selecting hs400 interface timing */

/* ERASED_MEM_CONT[181] */
#define EXT_CSD_ERASED_MEM_CONT_0       (0)
#define EXT_CSD_ERASED_MEM_CONT_1       (1)

/* PARTITION CONFIG[179] */
#define EXT_CSD_PART_CFG_DEFT_PART      (0)
#define EXT_CSD_PART_CFG_BOOT_PART_1    (1)
#define EXT_CSD_PART_CFG_BOOT_PART_2    (2)
#define EXT_CSD_PART_CFG_RPMB_PART      (3)
#define EXT_CSD_PART_CFG_GP_PART_1      (4)
#define EXT_CSD_PART_CFG_GP_PART_2      (5)
#define EXT_CSD_PART_CFG_GP_PART_3      (6)
#define EXT_CSD_PART_CFG_GP_PART_4      (7)
#define EXT_CSD_PART_CFG_EN_NO_BOOT     (0 << 3)
#define EXT_CSD_PART_CFG_EN_BOOT_PART_1 (1 << 3)
#define EXT_CSD_PART_CFG_EN_BOOT_PART_2 (2 << 3)
#define EXT_CSD_PART_CFG_EN_USER_AREA   (7 << 3)
#define EXT_CSD_PART_CFG_EN_NO_ACK      (0 << 6)
#define EXT_CSD_PART_CFG_EN_ACK         (1 << 6)

/* BOOT_CONFIG_PROT[178] */
#define EXT_CSD_EN_PWR_BOOT_CFG_PROT    (1)
#define EXT_CSD_EN_PERM_BOOT_CFG_PROT   (1<<4)  /* Carefully */

/* BOOT_BUS_WIDTH[177] */
#define EXT_CSD_BOOT_BUS_WIDTH_1        (0)
#define EXT_CSD_BOOT_BUS_WIDTH_4        (1)
#define EXT_CSD_BOOT_BUS_WIDTH_8        (2)
#define EXT_CSD_BOOT_BUS_RESET          (1 << 2)

#define EXT_CSD_BOOT_BUS_MODE_DEFT      (0 << 3)
#define EXT_CSD_BOOT_BUS_MODE_HS        (1 << 3)
#define EXT_CSD_BOOT_BUS_MODE_DDR       (2 << 3)

/* ERASE_GROUP_DEF[175] */
#define EXT_CSD_ERASE_GRP_DEF_EN        (1)

/* BOOT_WP[173] */
#define EXT_CSD_BOOT_WP_EN_PWR_WP       (1)
#define EXT_CSD_BOOT_WP_EN_PERM_WP      (1 << 2)
#define EXT_CSD_BOOT_WP_DIS_PERM_WP     (1 << 4)
#define EXT_CSD_BOOT_WP_DIS_PWR_WP      (1 << 6)

/* USER_WP[171] */
#define EXT_CSD_USR_WP_EN_PWR_WP        (1)
#define EXT_CSD_USR_WP_EN_PERM_WP       (1<<2)
#define EXT_CSD_USR_WP_DIS_PWR_WP       (1<<3)
#define EXT_CSD_USR_WP_DIS_PERM_WP      (1<<4)
#define EXT_CSD_USR_WP_DIS_CD_PERM_WP   (1<<6)
#define EXT_CSD_USR_WP_DIS_PERM_PWD     (1<<7)

/* RST_n_FUNCTION[162] */
#define EXT_CSD_RST_N_TEMP_DIS          (0)
#define EXT_CSD_RST_N_PERM_EN           (1) /* carefully */
#define EXT_CSD_RST_N_PERM_DIS          (2) /* carefully */

/* PARTITIONING_SUPPORT[160] */
#define EXT_CSD_PART_SUPPORT_PART_EN     (1)
#define EXT_CSD_PART_SUPPORT_ENH_ATTR_EN (1<<1)

/* PARTITIONS_ATTRIBUTE[156] */
#define EXT_CSD_PART_ATTR_ENH_USR       (1<<0)
#define EXT_CSD_PART_ATTR_ENH_1         (1<<1)
#define EXT_CSD_PART_ATTR_ENH_2         (1<<2)
#define EXT_CSD_PART_ATTR_ENH_3         (1<<3)
#define EXT_CSD_PART_ATTR_ENH_4         (1<<4)

/* PARTITION_SETTING_COMPLETED[156] */
#define EXT_CSD_PART_SET_COMPL_BIT      (1<<0)

/*
 * MMC_SWITCH access modes
 */

#define MMC_SWITCH_MODE_CMD_SET     0x00    /* Change the command set */
#define MMC_SWITCH_MODE_SET_BITS    0x01    /* Set bits which are 1 in value */
#define MMC_SWITCH_MODE_CLEAR_BITS  0x02    /* Clear bits which are 1 in value */
#define MMC_SWITCH_MODE_WRITE_BYTE  0x03    /* Set target to value */

#define MMC_SWITCH_MODE_SDR12       0
#define MMC_SWITCH_MODE_SDR25       1
#define MMC_SWITCH_MODE_SDR50       2
#define MMC_SWITCH_MODE_SDR104      3
#define MMC_SWITCH_MODE_DDR50       4

#define MMC_SWITCH_MODE_DRV_TYPE_B  0
#define MMC_SWITCH_MODE_DRV_TYPE_A  1
#define MMC_SWITCH_MODE_DRV_TYPE_C  2
#define MMC_SWITCH_MODE_DRV_TYPE_D  3

#define MMC_SWITCH_MODE_CL_200MA    0
#define MMC_SWITCH_MODE_CL_400MA    1
#define MMC_SWITCH_MODE_CL_600MA    2
#define MMC_SWITCH_MODE_CL_800MA    3

/*
 * MMC_ERASE arguments
 */
#define MMC_ERASE_SECURE_REQ        (1 << 31)
#define MMC_ERASE_GC_REQ            (1 << 15)
#define MMC_ERASE_DISCARD           (3 << 0)
#define MMC_ERASE_TRIM              (1 << 0)
#define MMC_ERASE_NORMAL            (0)

#define HOST_BUS_WIDTH_1            (1)
#define HOST_BUS_WIDTH_4            (4)
#define HOST_BUS_WIDTH_8            (8)

#define EMMC_BOOT_PULL_CMD_MODE     (0)
#define EMMC_BOOT_RST_CMD_MODE      (1)

