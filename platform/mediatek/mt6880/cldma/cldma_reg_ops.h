/*
 * Copyright (c) 2021 MediaTek Inc. All Rights Reserved.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#pragma once

/* BASE_ADDR - Reference info ------------------
 *  @CODA: CLDMA_INDMA_AO      CLDMA0_AO_INDMA_AO_AP  [0x1004_9000]<host Tx>
 *     CLDMA0_AO_INDMA_AO_MD [0x1004_A000]<sAP Tx>
 *     UL backup register for start and curr, enhance mode
 *  @CODA: CLDMA_INDMA_PD      CLDMA0_INDMA_PD_AP     [0x1021_D000]<host Tx>
 *     CLDMA0_INDMA_PD_MD     [0x1021_E000]<sAP Tx>
 *  @CODA: CLDMA_MISC_AO        CLDMA0_AO_MISC_AO_AP   [0x1004_9800]<host IRQ>
 *     CLDMA0_AO_MISC_AO_MD   [0x1004_A800]<sAP IRQ>
 *  @CODA: CLDMA_MISC_PD        CLDMA0_MSIC_PD_AP      [0x1021_D800]<host IRQ>
 *     CLDMA0_MISC_PD_MD      [0x1021_E800]<sAP IRQ>
 *  @CODA: CLDMA_OUTDMA_AO        CLDMA0_AO_OUTDMA_AO_AP [0x1004_9400]<host Rx>
 *     CLDMA0_AO_OUTDMA_AO_MD [0x1004_A400]<sAP Rx>
 *  @CODA: CLDMA_OUTDMA_PD        CLDMA0_OUTDMA_PD_AP    [0x1021_D400]<host Rx>
 *     CLDMA0_OUTDMA_PD_MD    [0x1021_E400]<sAP Rx>
 */

/* In comm LK, it need to access virtual address by IO_BASE(reference addressmap.h) */
/* power down domain for sAP */

#define CLDMA_PD_UL_BASE       CLDMA0_PD_BASE

/* CLDMA PD IN */
#define CLDMA_PD_DL_BASE       (CLDMA0_PD_BASE + 0x0400)

/* CLDMA PD OUT */
#define CLDMA_PD_MISC_BASE     (CLDMA0_PD_BASE + 0x0800)

/* CLDMA AO IN */
#define CLDMA_AO_UL_BASE       CLDMA0_AO_BASE

/* CLDMA AO OUT */
#define CLDMA_AO_DL_BASE       (CLDMA0_AO_BASE + 0x0400)

/* CLDMA MISC */
#define CLDMA_AO_MISC_BASE     (CLDMA0_AO_BASE + 0x0800)


/***********************************************************************
 * CLDMA PD DL
 ***********************************************************************/
#define REG_CLDMA_DL_ERR                (CLDMA_PD_DL_BASE + 0x0100)
#define REG_CLDMA_DL_START_CMD_WP       (CLDMA_PD_DL_BASE + 0x01BC)
#define REG_CLDMA_DL_STOP_CMD_WP        (CLDMA_PD_DL_BASE + 0x01C4)


/***********************************************************************
 * CLDMA PD UL
 ***********************************************************************/
#define REG_CLDMA_UL_START_ADDR_L(qno)    (CLDMA_PD_UL_BASE + 0x0004 + 0x0008 * qno)
#define REG_CLDMA_UL_START_ADDR_H(qno)    (CLDMA_PD_UL_BASE + 0x0008 + 0x0008 * qno)
#define REG_CLDMA_UL_CURR_ADDR_L(qno)     (CLDMA_PD_UL_BASE + 0x0044 + 0x0008 * qno)
#define REG_CLDMA_UL_CURR_ADDR_H(qno)     (CLDMA_PD_UL_BASE + 0x0048 + 0x0008 * qno)

#define REG_CLDMA_UL_STATUS               (CLDMA_PD_UL_BASE + 0x0084)
#define REG_CLDMA_UL_START_CMD_WP         (CLDMA_PD_UL_BASE + 0x0088)
#define REG_CLDMA_UL_STOP_CMD_WP          (CLDMA_PD_UL_BASE + 0x0090)
#define REG_CLDMA_UL_ERR                  (CLDMA_PD_UL_BASE + 0x0094)
#define REG_CLDMA_UL_CFG                  (CLDMA_PD_UL_BASE + 0x0098)

/***********************************************************************
 * CLDMA PD MISC
 ***********************************************************************/
/* UL part ----- */
#define REG_CLDMA_L2TISAR0                (CLDMA_PD_MISC_BASE + 0x0010)
#define REG_CLDMA_L2TISAR1                (CLDMA_PD_MISC_BASE + 0x0014)
#define REG_CLDMA_L3TISAR0                (CLDMA_PD_MISC_BASE + 0x0030)
#define REG_CLDMA_L3TISAR1                (CLDMA_PD_MISC_BASE + 0x0034)
#define REG_CLDMA_L3TISAR2                (CLDMA_PD_MISC_BASE + 0x00c0)

/* DL part ----- */
#define REG_CLDMA_L2RISAR0                (CLDMA_PD_MISC_BASE + 0x0050)
#define REG_CLDMA_L2RISAR1                (CLDMA_PD_MISC_BASE + 0x0054)
#define REG_CLDMA_L3RISAR0                (CLDMA_PD_MISC_BASE + 0x0070)
#define REG_CLDMA_L3RISAR1                (CLDMA_PD_MISC_BASE + 0x0074)


/***********************************************************************
 * CLDMA AO DL
 ***********************************************************************/
#define REG_CLDMA_DL_CFG                   (CLDMA_AO_DL_BASE + 0x0004)
#define REG_CLDMA_DL_START_ADDR_L(qno)     (CLDMA_AO_DL_BASE + 0x0078 + 0x0008 * qno)
#define REG_CLDMA_DL_START_ADDR_H(qno)     (CLDMA_AO_DL_BASE + 0x007C + 0x0008 * qno)
#define REG_CLDMA_DL_CURR_ADDR_L(qno)      (CLDMA_AO_DL_BASE + 0x00B8 + 0x0008 * qno)
#define REG_CLDMA_DL_CURR_ADDR_H(qno)      (CLDMA_AO_DL_BASE + 0x00BC + 0x0008 * qno)
#define REG_CLDMA_DL_STATUS                (CLDMA_AO_DL_BASE + 0x00F8)


/***********************************************************************
 * CLDMA AO MISC
 ***********************************************************************/
#define REG_CLDMA_DOMAIN_ID                (CLDMA_AO_MISC_BASE + 0x016C)


/***********************************************************************
 * CLDMA Register bit defination
 ***********************************************************************/
/* @ REG_CLDMA_DOMAIN_ID */
#define DOMAIN_ID_SAP                          0
#define DOMAIN_ID_PCIE                         5
#define DOMAIN_ID_MD                           7

#define DOMAIN_ID_LEFT_HAND_MASK               (0xFF)
#define DOMAIN_ID_RIGHT_HAND_MASK              (0xFF << 8)
#define DOMAIN_ID_PCIE_MASK                    (0xFF << 16)
#define DOMAIN_ID_MD_MASK                      (0xFF << 24)

#define DOMAIN_ID_LEFT_HAND_SHIFT              (0)
#define DOMAIN_ID_RIGHT_HAND_SHIFT             (8)
#define DOMAIN_ID_PCIE_SHIFT                   (16)
#define DOMAIN_ID_MD_SHIFT                     (24)

/* @ REG_CLDMA_UL_ERR */
#define CLDMA_TX_LDMU_ERR_STATUS                (1 << 0)
#define CLDMA_TX_LDMU_ERR_EXIT                  (1 << 1)

/* @ REG_CLDMA_L2TISAR0 */
#define CLDMA_TX_DONE_INT(qno)                  (1 << qno)
#define CLDMA_TX_QE_INT(qno)                    ((1 << 8) << qno)
#define CLDMA_TX_ERR_INT(qno)                   ((1 << 16) << qno)
#define CLDMA_TX_ACTIVE_START_ERR_INT(qno)      ((1 << 24) << qno)

/* @ REG_CLDMA_L3TISAR0 */
#define CLDMA_TX_GPD_LEN_ERR_INT(qno)           ((1 << 16) << qno)

/* @ REG_CLDMA_L3TISAR1 */
#define CLDMA_TX_GPD_64K_ERR_INT(qno)           (1 << qno)
#define CLDMA_TX_GPD_DATA_LEN_MIS_ERR_INT(qno)  ((1 << 16) << qno)

/* @ REG_CLDMA_L3TISAR2 */
#define CLDMA_TX_ILLEGALL_MEM_ACCESS_INT(qno)   ((1 << 8) << qno)

/* @ REG_CLDMA_DL_ERR */
#define CLDMA_RX_LDMU_ERR_STATUS                (1 << 0)
#define CLDMA_RX_LDMU_ERR_EXIT                        (1 << 1)

/* @ REG_CLDMA_L2RISAR0 */
#define CLDMA_RX_DONE_INT(qno)                  (1 << qno)
#define CLDMA_RX_QE_INT(qno)                    ((1 << 8) << qno)
#define CLDMA_RX_ERR_INT(qno)                   ((1 << 16) << qno)
#define CLDMA_RX_ACTIVE_START_ERR_INT(qno)      ((1 << 24) << qno)

/* @ REG_CLDMA_L2RISAR1 */
#define CLDMA_RX_INACTIVE_ERR_INT(qno)          (1 << qno)

/* @ REG_CLDMA_L3RISAR0 */
#define CLDMA_RX_GPD_LEN_ERR_INT(qno)           ((1 << 16) << qno)

/* @ REG_CLDMA_L3RISAR1 */
#define CLDMA_RX_ALLEN_ERR_INT(qno)             (1 << qno)
#define CLDMA_RX_ILLEGALL_MEM_ACCESS_INT(qno)   ((1 << 16) << qno)

/***********************************************************************
 * CLDMA Register operation API
 ***********************************************************************/
void cldma_reg_set_domain_id(void);
void cldma_reg_clr_tx_ldmu_err(void);
unsigned int cldma_reg_get_tx_ldmu_err(void);
void cldma_reg_clr_rx_ldmu_err(void);
unsigned int cldma_reg_get_rx_ldmu_err(void);
void cldma_reg_clr_tx_int(void);
void cldma_reg_clr_rx_int(void);
void cldma_reg_start_tx_que(unsigned int qno);
void cldma_reg_start_rx_que(unsigned int qno);
void cldma_reg_set_tx_start_addr(unsigned int qno, unsigned char *addr);
void cldma_reg_set_rx_start_addr(unsigned int qno, unsigned char *addr);
void cldma_reg_stop_tx_que(unsigned int qno);
void cldma_reg_stop_rx_que(unsigned int qno);
unsigned int cldma_reg_tx_error(unsigned int qno);
unsigned int cldma_reg_rx_error(unsigned int qno);
unsigned int cldma_reg_tx_active(unsigned int qno);
unsigned int cldma_reg_rx_active(unsigned int qno);
void cldma_reg_dump(void);

