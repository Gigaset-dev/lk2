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

/*
 * M-TX Configuration Attributes
 */
#define TX_MODE                              0x0021
#define TX_HSRATE_SERIES                     0x0022
#define TX_HSGEAR                            0x0023
#define TX_PWMGEAR                           0x0024
#define TX_AMPLITUDE                         0x0025
#define TX_HS_SLEWRATE                       0x0026
#define TX_SYNC_SOURCE                       0x0027
#define TX_HS_SYNC_LENGTH                    0x0028
#define TX_HS_PREPARE_LENGTH                 0x0029
#define TX_LS_PREPARE_LENGTH                 0x002A
#define TX_HIBERN8_CONTROL                   0x002B
#define TX_LCC_ENABLE                        0x002C
#define TX_PWM_BURST_CLOSURE_EXTENSION       0x002D
#define TX_BYPASS_8B10B_ENABLE               0x002E
#define TX_DRIVER_POLARITY                   0x002F
#define TX_HS_UNTERMINATED_LINE_DRIVE_ENABLE 0x0030
#define TX_LS_TERMINATED_LINE_DRIVE_ENABLE   0x0031
#define TX_LCC_SEQUENCER                     0x0032
#define TX_MIN_ACTIVATETIME                  0x0033
#define TX_PWM_G6_G7_SYNC_LENGTH             0x0034

/*
 * M-RX Configuration Attributes
 */
#define RX_MODE                         0x00A1
#define RX_HSRATE_SERIES                0x00A2
#define RX_HSGEAR                       0x00A3
#define RX_PWMGEAR                      0x00A4
#define RX_LS_TERMINATED_ENABLE         0x00A5
#define RX_HS_UNTERMINATED_ENABLE       0x00A6
#define RX_ENTER_HIBERN8                0x00A7
#define RX_BYPASS_8B10B_ENABLE          0x00A8
#define RX_TERMINATION_FORCE_ENABLE     0x0089

#define is_mphy_tx_attr(attr)           (attr < RX_MODE)
/*
 * PHY Adpater attributes
 */
#define PA_ACTIVETXDATALANES    0x1560
#define PA_ACTIVERXDATALANES    0x1580
#define PA_TXTRAILINGCLOCKS     0x1564
#define PA_PHY_TYPE             0x1500
#define PA_AVAILTXDATALANES     0x1520
#define PA_AVAILRXDATALANES     0x1540
#define PA_MINRXTRAILINGCLOCKS  0x1543
#define PA_TXPWRSTATUS          0x1567
#define PA_RXPWRSTATUS          0x1582
#define PA_TXFORCECLOCK         0x1562
#define PA_TXPWRMODE            0x1563
#define PA_LEGACYDPHYESCDL      0x1570
#define PA_MAXTXSPEEDFAST       0x1521
#define PA_MAXTXSPEEDSLOW       0x1522
#define PA_MAXRXSPEEDFAST       0x1541
#define PA_MAXRXSPEEDSLOW       0x1542
#define PA_TXLINKSTARTUPHS      0x1544
#define PA_TXSPEEDFAST          0x1565
#define PA_TXSPEEDSLOW          0x1566
#define PA_REMOTEVERINFO        0x15A0
#define PA_TXGEAR               0x1568
#define PA_TXTERMINATION        0x1569
#define PA_HSSERIES             0x156A
#define PA_PWRMODE              0x1571
#define PA_RXGEAR               0x1583
#define PA_RXTERMINATION        0x1584
#define PA_MAXRXPWMGEAR         0x1586
#define PA_MAXRXHSGEAR          0x1587
#define PA_RXHSUNTERMCAP        0x15A5
#define PA_RXLSTERMCAP          0x15A6
#define PA_PACPREQTIMEOUT       0x1590
#define PA_PACPREQEOBTIMEOUT    0x1591
#define PA_HIBERN8TIME          0x15A7
#define PA_LOCALVERINFO         0x15A9
#define PA_TACTIVATE            0x15A8
#define PA_GRANULARITY          0x15AA
#define PA_PACPFRAMECOUNT       0x15C0
#define PA_PACPERRORCOUNT       0x15C1
#define PA_PHYTESTCONTROL       0x15C2
#define PA_PWRMODEUSERDATA0     0x15B0
#define PA_PWRMODEUSERDATA1     0x15B1
#define PA_PWRMODEUSERDATA2     0x15B2
#define PA_PWRMODEUSERDATA3     0x15B3
#define PA_PWRMODEUSERDATA4     0x15B4
#define PA_PWRMODEUSERDATA5     0x15B5
#define PA_PWRMODEUSERDATA6     0x15B6
#define PA_PWRMODEUSERDATA7     0x15B7
#define PA_PWRMODEUSERDATA8     0x15B8
#define PA_PWRMODEUSERDATA9     0x15B9
#define PA_PWRMODEUSERDATA10    0x15BA
#define PA_PWRMODEUSERDATA11    0x15BB
#define PA_CONNECTEDTXDATALANES 0x1561
#define PA_CONNECTEDRXDATALANES 0x1581
#define PA_SCRAMBLING           0x1585
#define PA_LOGICALLANEMAP       0x15A1
#define PA_SLEEPNOCONFIGTIME    0x15A2
#define PA_STALLNOCONFIGTIME    0x15A3
#define PA_SAVECONFIGTIME       0x15A4
#define PA_TXHSADAPTTYPE        0x15D4
#define PA_LOCALTXLCCENABLE     0x155E

/* Adpat type for PA_TXHSADAPTTYPE attribute */
#define PA_REFRESH_ADAPT        0x00
#define PA_INITIAL_ADAPT        0x01
#define PA_NO_ADAPT             0x03

#define PA_GRANULARITY_MIN_VAL  1
#define PA_GRANULARITY_MAX_VAL  6

enum ufs_pwm_gear_tag {
    UFS_PWM_DONT_CHANGE,    /* Don't change Gear */
    UFS_PWM_G1,             /* PWM Gear 1 (default for reset) */
    UFS_PWM_G2,             /* PWM Gear 2 */
    UFS_PWM_G3,             /* PWM Gear 3 */
    UFS_PWM_G4,             /* PWM Gear 4 */
    UFS_PWM_G5,             /* PWM Gear 5 */
    UFS_PWM_G6,             /* PWM Gear 6 */
    UFS_PWM_G7,             /* PWM Gear 7 */
};

enum ufs_hs_gear_tag {
    UFS_HS_DONT_CHANGE, /* Don't change Gear */
    UFS_HS_G1,          /* HS Gear 1 (default for reset) */
    UFS_HS_G2,          /* HS Gear 2 */
    UFS_HS_G3,          /* HS Gear 3 */
    UFS_HS_G4,          /* HS Gear 4 */
};

/*
 * Data Link Layer Attributes
 */
#define DL_TC0TXFCTHRESHOLD     0x2040
#define DL_FC0PROTTIMEOUTVAL    0x2041
#define DL_TC0REPLAYTIMEOUTVAL  0x2042
#define DL_AFC0REQTIMEOUTVAL    0x2043
#define DL_AFC0CREDITTHRESHOLD  0x2044
#define DL_TC0OUTACKTHRESHOLD   0x2045
#define DL_TC1TXFCTHRESHOLD     0x2060
#define DL_FC1PROTTIMEOUTVAL    0x2061
#define DL_TC1REPLAYTIMEOUTVAL  0x2062
#define DL_AFC1REQTIMEOUTVAL    0x2063
#define DL_AFC1CREDITTHRESHOLD  0x2064
#define DL_TC1OUTACKTHRESHOLD   0x2065
#define DL_TXPREEMPTIONCAP      0x2000
#define DL_TC0TXMAXSDUSIZE      0x2001
#define DL_TC0RXINITCREDITVAL   0x2002
#define DL_TC0TXBUFFERSIZE      0x2005
#define DL_PEERTC0PRESENT       0x2046
#define DL_PEERTC0RXINITCREVAL  0x2047
#define DL_TC1TXMAXSDUSIZE      0x2003
#define DL_TC1RXINITCREDITVAL   0x2004
#define DL_TC1TXBUFFERSIZE      0x2006
#define DL_PEERTC1PRESENT       0x2066
#define DL_PEERTC1RXINITCREVAL  0x2067

/*
 * Network Layer Attributes
 */
#define N_DEVICEID          0x3000
#define N_DEVICEID_VALID    0x3001
#define N_TC0TXMAXSDUSIZE   0x3020
#define N_TC1TXMAXSDUSIZE   0x3021

/*
 * Transport Layer Attributes
 */
#define T_NUMCPORTS         0x4000
#define T_NUMTESTFEATURES   0x4001
#define T_CONNECTIONSTATE   0x4020
#define T_PEERDEVICEID      0x4021
#define T_PEERCPORTID       0x4022
#define T_TRAFFICCLASS      0x4023
#define T_PROTOCOLID        0x4024
#define T_CPORTFLAGS        0x4025
#define T_TXTOKENVALUE      0x4026
#define T_RXTOKENVALUE      0x4027
#define T_LOCALBUFFERSPACE  0x4028
#define T_PEERBUFFERSPACE   0x4029
#define T_CREDITSTOSEND     0x402A
#define T_CPORTMODE         0x402B
#define T_TC0TXMAXSDUSIZE   0x4060
#define T_TC1TXMAXSDUSIZE   0x4061

#define VENDOR_MPHYCFGUPDT            0xD085
#define VENDOR_DEBUGCLOCKENABLE       0xD0A1
#define VENDOR_DEEPSTALLCFG           0xD0A2
#define VENDOR_SAVEPOWERCONTROL       0xD0A6
#define VENDOR_UNIPROVERSION          0xD0A7
#define VENDOR_AUTOBURSTENDCTRL       0xD0B5
#define VENDOR_POWERSTATE             0xD083
#define VENDOR_UNIPROPOWERDOWNCONTROL 0xD0A8

/* VENDOR_DEBUGCLOCKENABLE */
enum {
    TX_SYMBOL_CLK_REQ_FORCE = 5,
};

/* VENDOR_SAVEPOWERCONTROL */
enum {
    RX_SYMBOL_CLK_GATE_EN   = 0,
    SYS_CLK_GATE_EN         = 2,
    TX_CLK_GATE_EN          = 3,
};
