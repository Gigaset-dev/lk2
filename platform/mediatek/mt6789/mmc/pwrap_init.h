/* Copyright Statement:
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein is
 * confidential and proprietary to MediaTek Inc. and/or its licensors. Without
 * the prior written permission of MediaTek inc. and/or its licensors, any
 * reproduction, modification, use or disclosure of MediaTek Software, and
 * information contained herein, in whole or in part, shall be strictly
 * prohibited.
 *
 * MediaTek Inc. (C) 2019. All rights reserved.
 *
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
 * RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO RECEIVER
 * ON AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL
 * WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR
 * NONINFRINGEMENT. NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH
 * RESPECT TO THE SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY,
 * INCORPORATED IN, OR SUPPLIED WITH THE MEDIATEK SOFTWARE, AND RECEIVER AGREES
 * TO LOOK ONLY TO SUCH THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO.
 * RECEIVER EXPRESSLY ACKNOWLEDGES THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO
 * OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES CONTAINED IN MEDIATEK
 * SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK SOFTWARE
 * RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S
 * ENTIRE AND CUMULATIVE LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE
 * RELEASED HEREUNDER WILL BE, AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE
 * MEDIATEK SOFTWARE AT ISSUE, OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE
 * CHARGE PAID BY RECEIVER TO MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 *
 * The following software/firmware and/or related documentation ("MediaTek
 * Software") have been modified by MediaTek Inc. All revisions are subject to
 * any receiver's applicable license agreements with MediaTek Inc.
 */
#ifndef __PMIC_WRAP_INIT_H__
#define __PMIC_WRAP_INIT_H__

/****** SW ENV define *************************************/
#define PMIC_WRAP_PRELOADER      1
#define PMIC_WRAP_LK             0
#define PMIC_WRAP_KERNEL         0
#define PMIC_WRAP_SCP            0
#define PMIC_WRAP_CTP            0

#define PMIC_WRAP_DEBUG
#define PMIC_WRAP_SUPPORT
#define PMIC_WRAP_CRC_SUPPORT
/* #define PMIC_WRAP_MATCH_SUPPORT */
#define PMIC_WRAP_HW_MONITOR_SET 1  /* valid value: 1~8 */
#define PMIC_WRAP_GENIEZONE_PMIF_SUPPORT

/****** For BringUp. if BringUp doesn't had PMIC, need open this ***********/
#if (PMIC_WRAP_PRELOADER)
	#if CFG_FPGA_PLATFORM
		#define PMIC_WRAP_NO_PMIC
	#else
		/* #define PWRAP_TIMEOUT */
	#endif
#elif (PMIC_WRAP_LK)
	#if defined(MACH_FPGA)
		#define PMIC_WRAP_NO_PMIC
	#else
		#define PWRAP_TIMEOUT
	#endif
#elif (PMIC_WRAP_KERNEL)
	#if defined(CONFIG_MTK_FPGA) || defined(CONFIG_FPGA_EARLY_PORTING)
		#define PMIC_WRAP_NO_PMIC
	#else
		/* #define PWRAP_TIMEOUT */
	#endif
#elif (PMIC_WRAP_CTP)
	#if defined(CONFIG_MTK_FPGA)
		#define PMIC_WRAP_NO_PMIC
	#else
		/* #define PWRAP_TIMEOUT */
	#endif
#else
	#define PWRAP_TIMEOUT
#endif

#define MTK_PLATFORM_MT6359      1
#define PMIC_WRAP_ULPOSC_CAL     0

/******  SW ENV header define *****************************/


/*****************************************************************/

/*******************macro for  regsister@PMIC *******************************/


/*******************start ---external API********************************/


/******  DEBUG marco define *******************************/

/**********************************************************/

/***********  platform info, PMIC info ********************/
#define PMIF_REG_RANGE         (0x774)
#define PMICSPI_MST_REG_RANGE  (0x80)

#define CLK_26M_PRD            (3846)
#define CLK_ULPOSC_PRD         (3846)

#define DEFAULT_VALUE_READ_TEST                 (0x5aa5)
#define DEFAULT_VALUE_WRITE_TEST                (0xa55a)
#define PWRAP_WRITE_TEST_VALUE                  (0x1234)
#define PWRAP_EXT_WRITE_TEST_VALUE              (0x4321)

/*********** bit shift for geniezone ********************/
#define GET_WACS1_INIT_DONE1_SHIFT              (15)
#define GET_SYS_IDLE1_SHIFT                     (17)

/**********************************************************/

#define DISABLE_ALL     (0)

/* HIPRIS_ARB */
/*
#define MDINF           (1 << 0)
#define WACS0		(1 << 1)
#define WACS1		(1 << 2)
#define WACS2		(1 << 4)
#define DVFSINF		(1 << 3)
#define STAUPD		(1 << 5)
#define GPSINF		(1 << 6)
*/

/* MUX SEL */
#define WRAPPER_MODE    (0)
#define MANUAL_MODE     (1)

#define DEFAULT_CMD             0
#define DEFAULT_SLVID           0
#define DEFAULT_BYTECNT         0

/* macro for PMIF SPI */
#define PMIF_SPI
#define PMIF_SPI_PMIFID                  0

#define PMIF_SPI_HWINF_NUM               8
#define PMIF_SPI_MAIN_SWINF_NUM          4
#define PMIF_SPI_SWINF_NUM               (PMIF_SPI_MAIN_SWINF_NUM + 2)
#define PMIF_SPI_CHAN_NUM                21
#define PMIF_SPI_MD_SWINF_NO             0
#define PMIF_SPI_AP_SECURE_SWINF_NO      1
#define PMIF_SPI_AP_SWINF_NO             2
#define PMIF_SPI_SWINF_0_CHAN_NO         8
#define PMIF_SPI_SWINF_3_CHAN_NO         11
#define PMIF_SPI_MD_SWINF_CHAN_NO        (PMIF_SPI_SWINF_0_CHAN_NO + PMIF_SPI_MD_SWINF_NO)
#define PMIF_SPI_AP_SECURE_SWINF_CHAN_NO (PMIF_SPI_SWINF_0_CHAN_NO + PMIF_SPI_AP_SECURE_SWINF_NO)
#define PMIF_SPI_AP_SWINF_CHAN_NO        (PMIF_SPI_SWINF_0_CHAN_NO + PMIF_SPI_AP_SWINF_NO)

/* macro for MAN_RDATA FSM */
#define MAN_FSM_NO_REQ             (0x00)
#define MAN_FSM_IDLE               (0x00)
#define MAN_FSM_REQ                (0x02)
#define MAN_FSM_WFDLE              (0x04) /* wait for idle, wait for read data done */
#define MAN_FSM_WFVLDCLR           (0x06)

/* macro for WACS_FSM */
#define WACS_FSM_IDLE               (0x00)
#define WACS_FSM_REQ                (0x02) /* request in process */
#define WACS_FSM_WFDLE              (0x04) /* wait for idle, wait for read data done */
#define WACS_FSM_WFVLDCLR           (0x06) /* finish read data , wait for valid flag clearing */
#define WACS_INIT_DONE              (0x01)
#define WACS_SYNC_IDLE              (0x01)
#define WACS_SYNC_BUSY              (0x00)

/**** timeout time, unit :us ***********/
#define TIMEOUT_RESET           (0x2710) /* 10000us */
#define TIMEOUT_READ            (0x2710) /* 10000us */
#define TIMEOUT_WAIT_IDLE       (0x2710) /* 10000us */

/*-----macro for manual commnd ---------------------------------*/
#define OP_WR    (0x1)
#define OP_RD    (0x0)
#define OP_CSH   (0x0)
#define OP_CSL   (0x1)
#define OP_CK    (0x2)
#define OP_OUTS  (0x8)
#define OP_OUTD  (0x9)
#define OP_OUTQ  (0xA)
#define OP_INS   (0xC)
#define OP_INS0  (0xD)
#define OP_IND   (0xE)
#define OP_INQ   (0xF)
#define OP_OS2IS (0x10)
#define OP_OS2ID (0x11)
#define OP_OS2IQ (0x12)
#define OP_OD2IS (0x13)
#define OP_OD2ID (0x14)
#define OP_OD2IQ (0x15)
#define OP_OQ2IS (0x16)
#define OP_OQ2ID (0x17)
#define OP_OQ2IQ (0x18)
#define OP_OSNIS (0x19)
#define OP_ODNID (0x1A)

/******************Error handle *****************************/
#define E_PWR_INVALID_ARG               (1)
#define E_PWR_INVALID_RW                (2)
#define E_PWR_INVALID_ADDR              (3)
#define E_PWR_INVALID_WDAT              (4)
#define E_PWR_INVALID_OP_MANUAL         (5)
#define E_PWR_NOT_IDLE_STATE            (6)
#define E_PWR_NOT_INIT_DONE             (7)
#define E_PWR_NOT_INIT_DONE_READ        (8)
#define E_PWR_WAIT_IDLE_TIMEOUT         (9)
#define E_PWR_WAIT_IDLE_TIMEOUT_READ    (10)
#define E_PWR_INIT_SIDLY_FAIL           (11)
#define E_PWR_RESET_TIMEOUT             (12)
#define E_PWR_TIMEOUT                   (13)
#define E_PWR_INVALID_SWINF             (14)
#define E_PWR_INVALID_CMD               (15)
#define E_PWR_INVALID_PMIFID            (16)
#define E_PWR_INVALID_SLVID             (17)
#define E_PWR_INVALID_BYTECNT           (18)
#define E_PWR_INIT_RESET_SPI            (20)
#define E_PWR_INIT_SIDLY                (21)
#define E_PWR_INIT_REG_CLOCK            (22)
#define E_PWR_INIT_ENABLE_PMIC          (23)
#define E_PWR_INIT_DIO                  (24)
#define E_PWR_INIT_CIPHER               (25)
#define E_PWR_INIT_WRITE_TEST           (26)
#define E_PWR_INIT_ENABLE_CRC           (27)
#define E_PWR_INIT_ENABLE_DEWRAP        (28)
#define E_PWR_READ_TEST_FAIL            (30)
#define E_PWR_WRITE_TEST_FAIL           (31)
#define E_PWR_SWITCH_DIO                (32)

/*-----macro for read/write register -------------------------------------*/
#define READ_REGISTER_UINT32(reg) \
    (*(volatile unsigned int * const)(reg))
#define INREG32(x)          READ_REGISTER_UINT32((unsigned int *)(x))
#define DRV_Reg32(addr)             INREG32(addr)

#define WRAP_RD32(addr)            DRV_Reg32(addr)
#define WRAP_WR32(addr,data)       ((*(volatile unsigned int *)(addr)) = (unsigned int)data)
#define WRAP_SET_BIT(BS,REG)       ((*(volatile unsigned int*)(REG)) |= (unsigned int)(BS))
#define WRAP_CLR_BIT(BS,REG)       ((*(volatile unsigned int*)(REG)) &= ~((unsigned int)(BS)))


/**************** end ---external API***********************************/

/************* macro for spi geniezone ******************************/
#define PWRAP_BASE					PMIF_SPI_BASE

/************* macro for spi clock config ******************************/
#define CLK_SPI_CK_26M					0x1

/************* macro for spi io config ******************************/
//#define IOCFG_LM_BASE					(0x11E20000)
#define IOCFG_LM_DRV_CFG4				(IOCFG_LM_BASE+0x040)
#define IOCFG_LM_DRV_CFG4_SET				(IOCFG_LM_BASE+0x044)
#define IOCFG_LM_DRV_CFG4_CLR				(IOCFG_LM_BASE+0x048)
/************* macro for APB Module modem_temp_share ****************/
#define MODEM_TEMP_SHARE_BASE			 	(TOPCKGEN_BASE+0x18000)
#define MODEM_TEMP_SHARE_CTRL			 	(MODEM_TEMP_SHARE_BASE+0x00)

#endif /*__PMIC_WRAP_INIT_H__*/
