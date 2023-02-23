/*
 * Copyright (c) 2020 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#ifndef REG_BASE_C_MODULE
// ----------------- DISP_MUTEX Bit Field Definitions -------------------

#define PACKING

PACKING union REG_INTEN
{
    PACKING struct
    {
        unsigned int MUTEX_INTEN               : 32;
    } Bits;
    u32 Raw;
};

PACKING union REG_INTSTA
{
    PACKING struct
    {
        unsigned int MUTEX_INTSTA              : 32;
    } Bits;
    u32 Raw;
};

PACKING union REG_CFG
{
    PACKING struct
    {
        unsigned int MUTEX_CFG                 : 1;
        unsigned int rsv_1                     : 31;
    } Bits;
    u32 Raw;
};

PACKING union REG_DISP_MUTEX0_EN
{
    PACKING struct
    {
        unsigned int MUTEX_EN                  : 1;
        unsigned int rsv_1                     : 31;
    } Bits;
    u32 Raw;
};

PACKING union REG_DISP_MUTEX0_RST
{
    PACKING struct
    {
        unsigned int MUTEX_RST                 : 1;
        unsigned int rsv_1                     : 31;
    } Bits;
    u32 Raw;
};

PACKING union REG_DISP_MUTEX0_CTL
{
    PACKING struct
    {
        unsigned int MUTEX_SOF                 : 3;
        unsigned int MUTEX_SOF_TIMING          : 2;
        unsigned int MUTEX_SOF_WAIT            : 1;
        unsigned int MUTEX_EOF                 : 3;
        unsigned int MUTEX_EOF_TIMING          : 2;
        unsigned int MUTEX_EOF_WAIT            : 1;
        unsigned int rsv_12                    : 20;
    } Bits;
    u32 Raw;
};

PACKING union REG_DISP_MUTEX0_MOD0
{
    PACKING struct
    {
        unsigned int MUTEX_MOD0                : 32;
    } Bits;
    u32 Raw;
};

PACKING union REG_DISP_MUTEX0_MOD1
{
    PACKING struct
    {
        unsigned int MUTEX_MOD1                : 32;
    } Bits;
    u32 Raw;
};

PACKING union REG_DISP_MUTEX1_EN
{
    PACKING struct
    {
        unsigned int MUTEX_EN                  : 1;
        unsigned int rsv_1                     : 31;
    } Bits;
    u32 Raw;
};

PACKING union REG_DISP_MUTEX1_RST
{
    PACKING struct
    {
        unsigned int MUTEX_RST                 : 1;
        unsigned int rsv_1                     : 31;
    } Bits;
    u32 Raw;
};

PACKING union REG_DISP_MUTEX1_CTL
{
    PACKING struct
    {
        unsigned int MUTEX_SOF                 : 3;
        unsigned int MUTEX_SOF_TIMING          : 2;
        unsigned int MUTEX_SOF_WAIT            : 1;
        unsigned int MUTEX_EOF                 : 3;
        unsigned int MUTEX_EOF_TIMING          : 2;
        unsigned int MUTEX_EOF_WAIT            : 1;
        unsigned int rsv_12                    : 20;
    } Bits;
    u32 Raw;
};

PACKING union REG_DISP_MUTEX1_MOD0
{
    PACKING struct
    {
        unsigned int MUTEX_MOD0                : 32;
    } Bits;
    u32 Raw;
};

PACKING union REG_DISP_MUTEX2_EN
{
    PACKING struct
    {
        unsigned int MUTEX_EN                  : 1;
        unsigned int rsv_1                     : 31;
    } Bits;
    u32 Raw;
};

PACKING union REG_DISP_MUTEX2_RST
{
    PACKING struct
    {
        unsigned int MUTEX_RST                 : 1;
        unsigned int rsv_1                     : 31;
    } Bits;
    u32 Raw;
};

PACKING union REG_DISP_MUTEX2_CTL
{
    PACKING struct
    {
        unsigned int MUTEX_SOF                 : 3;
        unsigned int MUTEX_SOF_TIMING          : 2;
        unsigned int MUTEX_SOF_WAIT            : 1;
        unsigned int MUTEX_EOF                 : 3;
        unsigned int MUTEX_EOF_TIMING          : 2;
        unsigned int MUTEX_EOF_WAIT            : 1;
        unsigned int rsv_12                    : 20;
    } Bits;
    u32 Raw;
};

PACKING union REG_DISP_MUTEX2_MOD0
{
    PACKING struct
    {
        unsigned int MUTEX_MOD0                : 32;
    } Bits;
    u32 Raw;
};

PACKING union REG_DISP_MUTEX3_EN
{
    PACKING struct
    {
        unsigned int MUTEX_EN                  : 1;
        unsigned int rsv_1                     : 31;
    } Bits;
    u32 Raw;
};

PACKING union REG_DISP_MUTEX3_RST
{
    PACKING struct
    {
        unsigned int MUTEX_RST                 : 1;
        unsigned int rsv_1                     : 31;
    } Bits;
    u32 Raw;
};

PACKING union REG_DISP_MUTEX3_CTL
{
    PACKING struct
    {
        unsigned int MUTEX_SOF                 : 3;
        unsigned int MUTEX_SOF_TIMING          : 2;
        unsigned int MUTEX_SOF_WAIT            : 1;
        unsigned int MUTEX_EOF                 : 3;
        unsigned int MUTEX_EOF_TIMING          : 2;
        unsigned int MUTEX_EOF_WAIT            : 1;
        unsigned int rsv_12                    : 20;
    } Bits;
    u32 Raw;
};

PACKING union REG_DISP_MUTEX3_MOD0
{
    PACKING struct
    {
        unsigned int MUTEX_MOD0                : 32;
    } Bits;
    u32 Raw;
};

PACKING union REG_DISP_MUTEX4_EN
{
    PACKING struct
    {
        unsigned int MUTEX_EN                  : 1;
        unsigned int rsv_1                     : 31;
    } Bits;
    u32 Raw;
};

PACKING union REG_DISP_MUTEX4_RST
{
    PACKING struct
    {
        unsigned int MUTEX_RST                 : 1;
        unsigned int rsv_1                     : 31;
    } Bits;
    u32 Raw;
};

PACKING union REG_DISP_MUTEX4_CTL
{
    PACKING struct
    {
        unsigned int MUTEX_SOF                 : 3;
        unsigned int MUTEX_SOF_TIMING          : 2;
        unsigned int MUTEX_SOF_WAIT            : 1;
        unsigned int MUTEX_EOF                 : 3;
        unsigned int MUTEX_EOF_TIMING          : 2;
        unsigned int MUTEX_EOF_WAIT            : 1;
        unsigned int rsv_12                    : 20;
    } Bits;
    u32 Raw;
};

PACKING union REG_DISP_MUTEX4_MOD0
{
    PACKING struct
    {
        unsigned int MUTEX_MOD0                : 32;
    } Bits;
    u32 Raw;
};

PACKING union REG_DISP_MUTEX5_EN
{
    PACKING struct
    {
        unsigned int MUTEX_EN                  : 1;
        unsigned int rsv_1                     : 31;
    } Bits;
    u32 Raw;
};

PACKING union REG_DISP_MUTEX5_RST
{
    PACKING struct
    {
        unsigned int MUTEX_RST                 : 1;
        unsigned int rsv_1                     : 31;
    } Bits;
    u32 Raw;
};

PACKING union REG_DISP_MUTEX5_CTL
{
    PACKING struct
    {
        unsigned int MUTEX_SOF                 : 3;
        unsigned int MUTEX_SOF_TIMING          : 2;
        unsigned int MUTEX_SOF_WAIT            : 1;
        unsigned int MUTEX_EOF                 : 3;
        unsigned int MUTEX_EOF_TIMING          : 2;
        unsigned int MUTEX_EOF_WAIT            : 1;
        unsigned int rsv_12                    : 20;
    } Bits;
    u32 Raw;
};

PACKING union REG_DISP_MUTEX5_MOD0
{
    PACKING struct
    {
        unsigned int MUTEX_MOD0                : 32;
    } Bits;
    u32 Raw;
};

PACKING union REG_DISP_MUTEX6_EN
{
    PACKING struct
    {
        unsigned int MUTEX_EN                  : 1;
        unsigned int rsv_1                     : 31;
    } Bits;
    u32 Raw;
};

PACKING union REG_DISP_MUTEX6_RST
{
    PACKING struct
    {
        unsigned int MUTEX_RST                 : 1;
        unsigned int rsv_1                     : 31;
    } Bits;
    u32 Raw;
};

PACKING union REG_DISP_MUTEX6_CTL
{
    PACKING struct
    {
        unsigned int MUTEX_SOF                 : 3;
        unsigned int MUTEX_SOF_TIMING          : 2;
        unsigned int MUTEX_SOF_WAIT            : 1;
        unsigned int MUTEX_EOF                 : 3;
        unsigned int MUTEX_EOF_TIMING          : 2;
        unsigned int MUTEX_EOF_WAIT            : 1;
        unsigned int rsv_12                    : 20;
    } Bits;
    u32 Raw;
};

PACKING union REG_DISP_MUTEX6_MOD0
{
    PACKING struct
    {
        unsigned int MUTEX_MOD0                : 32;
    } Bits;
    u32 Raw;
};

PACKING union REG_DISP_MUTEX7_EN
{
    PACKING struct
    {
        unsigned int MUTEX_EN                  : 1;
        unsigned int rsv_1                     : 31;
    } Bits;
    u32 Raw;
};

PACKING union REG_DISP_MUTEX7_RST
{
    PACKING struct
    {
        unsigned int MUTEX_RST                 : 1;
        unsigned int rsv_1                     : 31;
    } Bits;
    u32 Raw;
};

PACKING union REG_DISP_MUTEX7_CTL
{
    PACKING struct
    {
        unsigned int MUTEX_SOF                 : 3;
        unsigned int MUTEX_SOF_TIMING          : 2;
        unsigned int MUTEX_SOF_WAIT            : 1;
        unsigned int MUTEX_EOF                 : 3;
        unsigned int MUTEX_EOF_TIMING          : 2;
        unsigned int MUTEX_EOF_WAIT            : 1;
        unsigned int rsv_12                    : 20;
    } Bits;
    u32 Raw;
};

PACKING union REG_DISP_MUTEX7_MOD0
{
    PACKING struct
    {
        unsigned int MUTEX_MOD0                : 32;
    } Bits;
    u32 Raw;
};

PACKING union REG_DISP_MUTEX8_EN
{
    PACKING struct
    {
        unsigned int MUTEX_EN                  : 1;
        unsigned int rsv_1                     : 31;
    } Bits;
    u32 Raw;
};

PACKING union REG_DISP_MUTEX8_RST
{
    PACKING struct
    {
        unsigned int MUTEX_RST                 : 1;
        unsigned int rsv_1                     : 31;
    } Bits;
    u32 Raw;
};

PACKING union REG_DISP_MUTEX8_CTL
{
    PACKING struct
    {
        unsigned int MUTEX_SOF                 : 3;
        unsigned int MUTEX_SOF_TIMING          : 2;
        unsigned int MUTEX_SOF_WAIT            : 1;
        unsigned int MUTEX_EOF                 : 3;
        unsigned int MUTEX_EOF_TIMING          : 2;
        unsigned int MUTEX_EOF_WAIT            : 1;
        unsigned int rsv_12                    : 20;
    } Bits;
    u32 Raw;
};

PACKING union REG_DISP_MUTEX8_MOD0
{
    PACKING struct
    {
        unsigned int MUTEX_MOD0                : 32;
    } Bits;
    u32 Raw;
};

PACKING union REG_DISP_MUTEX9_EN
{
    PACKING struct
    {
        unsigned int MUTEX_EN                  : 1;
        unsigned int rsv_1                     : 31;
    } Bits;
    u32 Raw;
};

PACKING union REG_DISP_MUTEX9_RST
{
    PACKING struct
    {
        unsigned int MUTEX_RST                 : 1;
        unsigned int rsv_1                     : 31;
    } Bits;
    u32 Raw;
};

PACKING union REG_DISP_MUTEX9_CTL
{
    PACKING struct
    {
        unsigned int MUTEX_SOF                 : 3;
        unsigned int MUTEX_SOF_TIMING          : 2;
        unsigned int MUTEX_SOF_WAIT            : 1;
        unsigned int MUTEX_EOF                 : 3;
        unsigned int MUTEX_EOF_TIMING          : 2;
        unsigned int MUTEX_EOF_WAIT            : 1;
        unsigned int rsv_12                    : 20;
    } Bits;
    u32 Raw;
};

PACKING union REG_DISP_MUTEX9_MOD0
{
    PACKING struct
    {
        unsigned int MUTEX_MOD0                : 32;
    } Bits;
    u32 Raw;
};

PACKING union REG_DEBUG_OUT_SEL
{
    PACKING struct
    {
        unsigned int DEBUG_OUT_SEL             : 2;
        unsigned int rsv_2                     : 14;
    } Bits;
    u32 Raw;
};

// ----------------- DISP_MUTEX  Grouping Definitions -------------------
PACKING struct MUTEX_STS{
    PACKING union REG_INTSTA                      DISP_MUTEX_INTSTA;   //14001004
};

struct MUTEX_START {
    union REG_DISP_MUTEX0_EN              DISP_MUTEX0_EN;      //14001020
    union REG_DISP_MUTEX1_EN              DISP_MUTEX1_EN;      //14001040
    union REG_DISP_MUTEX2_EN              DISP_MUTEX2_EN;      //14001060
    union REG_DISP_MUTEX3_EN              DISP_MUTEX3_EN;      //14001080
    union REG_DISP_MUTEX4_EN              DISP_MUTEX4_EN;      //140010A0
    union REG_DISP_MUTEX5_EN              DISP_MUTEX5_EN;      //140010C0
    union REG_DISP_MUTEX6_EN              DISP_MUTEX6_EN;      //140010E0
    union REG_DISP_MUTEX7_EN              DISP_MUTEX7_EN;      //14001100
    union REG_DISP_MUTEX8_EN              DISP_MUTEX8_EN;      //14001120
    union REG_DISP_MUTEX9_EN              DISP_MUTEX9_EN;      //14001140
};

struct MUTEX_CFG {
    union REG_INTEN                       DISP_MUTEX_INTEN;    //14001000
    union REG_CFG                         DISP_MUTEX_CFG;      //14001008
    union REG_DISP_MUTEX0_RST             DISP_MUTEX0_RST;     //14001028
    union REG_DISP_MUTEX0_CTL             DISP_MUTEX0_CTL;     //1400102C
    union REG_DISP_MUTEX0_MOD0            DISP_MUTEX0_MOD0;    //14001030
    union REG_DISP_MUTEX0_MOD1            DISP_MUTEX0_MOD1;    //14001034
    union REG_DISP_MUTEX1_RST             DISP_MUTEX1_RST;     //14001048
    union REG_DISP_MUTEX1_CTL             DISP_MUTEX1_CTL;     //1400104C
    union REG_DISP_MUTEX1_MOD0            DISP_MUTEX1_MOD0;    //14001050
    union REG_DISP_MUTEX2_RST             DISP_MUTEX2_RST;     //14001068
    union REG_DISP_MUTEX2_CTL             DISP_MUTEX2_CTL;     //1400106C
    union REG_DISP_MUTEX2_MOD0            DISP_MUTEX2_MOD0;    //14001070
    union REG_DISP_MUTEX3_RST             DISP_MUTEX3_RST;     //14001088
    union REG_DISP_MUTEX3_CTL             DISP_MUTEX3_CTL;     //1400108C
    union REG_DISP_MUTEX3_MOD0            DISP_MUTEX3_MOD0;    //14001090
    union REG_DISP_MUTEX4_RST             DISP_MUTEX4_RST;     //140010A8
    union REG_DISP_MUTEX4_CTL             DISP_MUTEX4_CTL;     //140010AC
    union REG_DISP_MUTEX4_MOD0            DISP_MUTEX4_MOD0;    //140010B0
    union REG_DISP_MUTEX5_RST             DISP_MUTEX5_RST;     //140010C8
    union REG_DISP_MUTEX5_CTL             DISP_MUTEX5_CTL;     //140010CC
    union REG_DISP_MUTEX5_MOD0            DISP_MUTEX5_MOD0;    //140010D0
    union REG_DISP_MUTEX6_RST             DISP_MUTEX6_RST;     //140010E8
    union REG_DISP_MUTEX6_CTL             DISP_MUTEX6_CTL;     //140010EC
    union REG_DISP_MUTEX6_MOD0            DISP_MUTEX6_MOD0;    //140010F0
    union REG_DISP_MUTEX7_RST             DISP_MUTEX7_RST;     //14001108
    union REG_DISP_MUTEX7_CTL             DISP_MUTEX7_CTL;     //1400110C
    union REG_DISP_MUTEX7_MOD0            DISP_MUTEX7_MOD0;    //14001110
    union REG_DISP_MUTEX8_RST             DISP_MUTEX8_RST;     //14001128
    union REG_DISP_MUTEX8_CTL             DISP_MUTEX8_CTL;     //1400112C
    union REG_DISP_MUTEX8_MOD0            DISP_MUTEX8_MOD0;    //14001130
    union REG_DISP_MUTEX9_RST             DISP_MUTEX9_RST;     //14001148
    union REG_DISP_MUTEX9_CTL             DISP_MUTEX9_CTL;     //1400114C
    union REG_DISP_MUTEX9_MOD0            DISP_MUTEX9_MOD0;    //14001150
    union REG_DEBUG_OUT_SEL               DISP_MUTEX_DEBUG_OUT_SEL; //130C
};

// ----------------- DISP_MUTEX Register Definition -------------------
/* modify by RD */
struct DISP_MUTEX_REGS {
    union REG_INTEN                       DISP_MUTEX_INTEN;        //14016000
    union REG_INTSTA                      DISP_MUTEX_INTSTA;       //14016004
    union REG_CFG                         DISP_MUTEX_CFG;          //14016008
    u32                          rsv_100C[5];              // 100C..101C
    union REG_DISP_MUTEX0_EN              DISP_MUTEX0_EN;          //14016020
    u32                          rsv_1024;                 // 1024
    union REG_DISP_MUTEX0_RST             DISP_MUTEX0_RST;         //14016028
    union REG_DISP_MUTEX0_CTL             DISP_MUTEX0_CTL;         //1401602C
    union REG_DISP_MUTEX0_MOD0            DISP_MUTEX0_MOD0;        //14016030
    union REG_DISP_MUTEX0_MOD1            DISP_MUTEX0_MOD1;        //14016034
    u32                          rsv_6034[2];              //14016038..603C
    union REG_DISP_MUTEX1_EN              DISP_MUTEX1_EN;          //14016040
    u32                          rsv_1044;                 // 1044
    union REG_DISP_MUTEX1_RST             DISP_MUTEX1_RST;         //14016048
    union REG_DISP_MUTEX1_CTL             DISP_MUTEX1_CTL;         //1401604C
    union REG_DISP_MUTEX1_MOD0            DISP_MUTEX1_MOD0;        //14016050
    u32                          rsv_6054[3];              //14016054..605C
    union REG_DISP_MUTEX2_EN              DISP_MUTEX2_EN;          //14016060
    u32                          rsv_1064;                 // 1064
    union REG_DISP_MUTEX2_RST             DISP_MUTEX2_RST;         //14016068
    union REG_DISP_MUTEX2_CTL             DISP_MUTEX2_CTL;         //1401606C
    union REG_DISP_MUTEX2_MOD0            DISP_MUTEX2_MOD0;        //14016070
    u32                          rsv_6074[3];              //14016074..607C
    union REG_DISP_MUTEX3_EN              DISP_MUTEX3_EN;          //14016080
    u32                          rsv_1084;                 // 1084
    union REG_DISP_MUTEX3_RST             DISP_MUTEX3_RST;         //14016088
    union REG_DISP_MUTEX3_CTL             DISP_MUTEX3_CTL;         //1401608C
    union REG_DISP_MUTEX3_MOD0            DISP_MUTEX3_MOD0;        //14016090
    u32                          rsv_6094[3];              //14016094..609C
    union REG_DISP_MUTEX4_EN              DISP_MUTEX4_EN;          //140160A0
    u32                          rsv_10A4;                 // 10A4
    union REG_DISP_MUTEX4_RST             DISP_MUTEX4_RST;         //140160A8
    union REG_DISP_MUTEX4_CTL             DISP_MUTEX4_CTL;         //140160AC
    union REG_DISP_MUTEX4_MOD0            DISP_MUTEX4_MOD0;        //140160B0
    u32                          rsv_60B4[3];              //140160B4..60BC
    union REG_DISP_MUTEX5_EN              DISP_MUTEX5_EN;          //140160C0
    u32                          rsv_10C4;                 // 10C4
    union REG_DISP_MUTEX5_RST             DISP_MUTEX5_RST;         //140160C8
    union REG_DISP_MUTEX5_CTL             DISP_MUTEX5_CTL;         //140160CC
    union REG_DISP_MUTEX5_MOD0            DISP_MUTEX5_MOD0;        //140160D0
    u32                          rsv_60D4[3];              //140160D4..60DC
    union REG_DISP_MUTEX6_EN              DISP_MUTEX6_EN;          //140160E0
    u32                          rsv_10E4;                 // 10E4
    union REG_DISP_MUTEX6_RST             DISP_MUTEX6_RST;         //140160E8
    union REG_DISP_MUTEX6_CTL             DISP_MUTEX6_CTL;         //140160EC
    union REG_DISP_MUTEX6_MOD0            DISP_MUTEX6_MOD0;        //140160F0
    u32                          rsv_60F4[3];              //140160F4..60FC
    union REG_DISP_MUTEX7_EN              DISP_MUTEX7_EN;          //14016100
    u32                          rsv_1104;                 // 1104
    union REG_DISP_MUTEX7_RST             DISP_MUTEX7_RST;         //14016108
    union REG_DISP_MUTEX7_CTL             DISP_MUTEX7_CTL;         //1401610C
    union REG_DISP_MUTEX7_MOD0            DISP_MUTEX7_MOD0;        //14016110
    u32                          rsv_6114[3];              //14016114..611C
    union REG_DISP_MUTEX8_EN              DISP_MUTEX8_EN;          //14016120
    u32                          rsv_1124;                 // 1124
    union REG_DISP_MUTEX8_RST             DISP_MUTEX8_RST;         //14016128
    union REG_DISP_MUTEX8_CTL             DISP_MUTEX8_CTL;         //1401612C
    union REG_DISP_MUTEX8_MOD0            DISP_MUTEX8_MOD0;        //14016130
    u32                          rsv_6134[3];              //14016134..613C
    union REG_DISP_MUTEX9_EN              DISP_MUTEX9_EN;          //14016140
    u32                          rsv_1144;                 // 1144
    union REG_DISP_MUTEX9_RST             DISP_MUTEX9_RST;         //14016148
    union REG_DISP_MUTEX9_CTL             DISP_MUTEX9_CTL;         //1401614C
    union REG_DISP_MUTEX9_MOD0            DISP_MUTEX9_MOD0;        //14016150
    u32                          rsv_1154[110];            // 1154..1308
    union REG_DEBUG_OUT_SEL               DISP_MUTEX_DEBUG_OUT_SEL; //1401630C
};

// ---------- DISP_MUTEX Enum Definitions      ----------
// ---------- DISP_MUTEX C Macro Definitions   ----------
extern struct DISP_MUTEX_REGS *g_DISP_MUTEX_BASE;

#define DISP_MUTEX_BASE                         (g_DISP_MUTEX_BASE)

#define DISP_MUTEX_INTEN                        (addr_t)(&DISP_MUTEX_BASE->DISP_MUTEX_INTEN) // 1000
#define DISP_MUTEX_INTSTA                       (addr_t)(&DISP_MUTEX_BASE->DISP_MUTEX_INTSTA) // 1004
#define DISP_MUTEX_CFG                          (addr_t)(&DISP_MUTEX_BASE->DISP_MUTEX_CFG) // 1008
#define DISP_MUTEX0_EN                          (addr_t)(&DISP_MUTEX_BASE->DISP_MUTEX0_EN) // 6020
#define DISP_MUTEX0_RST                         (addr_t)(&DISP_MUTEX_BASE->DISP_MUTEX0_RST) // 6028
#define DISP_MUTEX0_CTL                         (addr_t)(&DISP_MUTEX_BASE->DISP_MUTEX0_CTL) // 602C
#define DISP_MUTEX0_MOD0                        (addr_t)(&DISP_MUTEX_BASE->DISP_MUTEX0_MOD0) // 6030
#define DISP_MUTEX0_MOD1                        (addr_t)(&DISP_MUTEX_BASE->DISP_MUTEX0_MOD1) // 6034
#define DISP_MUTEX1_EN                          (addr_t)(&DISP_MUTEX_BASE->DISP_MUTEX1_EN) // 6040
#define DISP_MUTEX1_RST                         (addr_t)(&DISP_MUTEX_BASE->DISP_MUTEX1_RST) // 6048
#define DISP_MUTEX1_CTL                         (addr_t)(&DISP_MUTEX_BASE->DISP_MUTEX1_CTL) // 604C
#define DISP_MUTEX1_MOD0                        (addr_t)(&DISP_MUTEX_BASE->DISP_MUTEX1_MOD0) // 6050
#define DISP_MUTEX2_EN                          (addr_t)(&DISP_MUTEX_BASE->DISP_MUTEX2_EN) // 6060
#define DISP_MUTEX2_RST                         (addr_t)(&DISP_MUTEX_BASE->DISP_MUTEX2_RST) // 6068
#define DISP_MUTEX2_CTL                         (addr_t)(&DISP_MUTEX_BASE->DISP_MUTEX2_CTL) // 606C
#define DISP_MUTEX2_MOD0                        (addr_t)(&DISP_MUTEX_BASE->DISP_MUTEX2_MOD0) // 6070
#define DISP_MUTEX3_EN                          (addr_t)(&DISP_MUTEX_BASE->DISP_MUTEX3_EN) // 6080
#define DISP_MUTEX3_RST                         (addr_t)(&DISP_MUTEX_BASE->DISP_MUTEX3_RST) // 6088
#define DISP_MUTEX3_CTL                         (addr_t)(&DISP_MUTEX_BASE->DISP_MUTEX3_CTL) // 608C
#define DISP_MUTEX3_MOD0                        (addr_t)(&DISP_MUTEX_BASE->DISP_MUTEX3_MOD0) // 6090
#define DISP_MUTEX4_EN                          (addr_t)(&DISP_MUTEX_BASE->DISP_MUTEX4_EN) // 60A0
#define DISP_MUTEX4_RST                         (addr_t)(&DISP_MUTEX_BASE->DISP_MUTEX4_RST) // 60A8
#define DISP_MUTEX4_CTL                         (addr_t)(&DISP_MUTEX_BASE->DISP_MUTEX4_CTL) // 60AC
#define DISP_MUTEX4_MOD0                        (addr_t)(&DISP_MUTEX_BASE->DISP_MUTEX4_MOD0) // 60B0
#define DISP_MUTEX5_EN                          (addr_t)(&DISP_MUTEX_BASE->DISP_MUTEX5_EN) // 60C0
#define DISP_MUTEX5_RST                         (addr_t)(&DISP_MUTEX_BASE->DISP_MUTEX5_RST) // 60C8
#define DISP_MUTEX5_CTL                         (addr_t)(&DISP_MUTEX_BASE->DISP_MUTEX5_CTL) // 60CC
#define DISP_MUTEX5_MOD0                        (addr_t)(&DISP_MUTEX_BASE->DISP_MUTEX5_MOD0) // 60D0
#define DISP_MUTEX6_EN                          (addr_t)(&DISP_MUTEX_BASE->DISP_MUTEX6_EN) // 60E0
#define DISP_MUTEX6_RST                         (addr_t)(&DISP_MUTEX_BASE->DISP_MUTEX6_RST) // 60E8
#define DISP_MUTEX6_CTL                         (addr_t)(&DISP_MUTEX_BASE->DISP_MUTEX6_CTL) // 60EC
#define DISP_MUTEX6_MOD0                        (addr_t)(&DISP_MUTEX_BASE->DISP_MUTEX6_MOD0) // 60F0
#define DISP_MUTEX7_EN                          (addr_t)(&DISP_MUTEX_BASE->DISP_MUTEX7_EN) // 6100
#define DISP_MUTEX7_RST                         (addr_t)(&DISP_MUTEX_BASE->DISP_MUTEX7_RST) // 6108
#define DISP_MUTEX7_CTL                         (addr_t)(&DISP_MUTEX_BASE->DISP_MUTEX7_CTL) // 610C
#define DISP_MUTEX7_MOD0                        (addr_t)(&DISP_MUTEX_BASE->DISP_MUTEX7_MOD0) // 6110
#define DISP_MUTEX8_EN                          (addr_t)(&DISP_MUTEX_BASE->DISP_MUTEX8_EN) // 6120
#define DISP_MUTEX8_RST                         (addr_t)(&DISP_MUTEX_BASE->DISP_MUTEX8_RST) // 6128
#define DISP_MUTEX8_CTL                         (addr_t)(&DISP_MUTEX_BASE->DISP_MUTEX8_CTL) // 612C
#define DISP_MUTEX8_MOD0                        (addr_t)(&DISP_MUTEX_BASE->DISP_MUTEX8_MOD0) // 6130
#define DISP_MUTEX9_EN                          (addr_t)(&DISP_MUTEX_BASE->DISP_MUTEX9_EN) // 6140
#define DISP_MUTEX9_RST                         (addr_t)(&DISP_MUTEX_BASE->DISP_MUTEX9_RST) // 6148
#define DISP_MUTEX9_CTL                         (addr_t)(&DISP_MUTEX_BASE->DISP_MUTEX9_CTL) // 614C
#define DISP_MUTEX9_MOD0                        (addr_t)(&DISP_MUTEX_BASE->DISP_MUTEX9_MOD0) // 6150
#define DEBUG_OUT_SEL                           (addr_t)(&DISP_MUTEX_BASE->DEBUG_OUT_SEL) // 130C

#endif


#define INTEN_FLD_MUTEX_INTEN                          REG_FLD(32, 0)

#define INTSTA_FLD_MUTEX_INTSTA                        REG_FLD(32, 0)

#define CFG_FLD_MUTEX_CFG                              REG_FLD(1, 0)

#define DISP_MUTEX0_EN_FLD_MUTEX_EN                    REG_FLD(1, 0)

#define DISP_MUTEX0_RST_FLD_MUTEX_RST                  REG_FLD(1, 0)

#define DISP_MUTEX0_CTL_FLD_MUTEX_EOF_WAIT             REG_FLD(1, 11)
#define DISP_MUTEX0_CTL_FLD_MUTEX_EOF_TIMING           REG_FLD(2, 9)
#define DISP_MUTEX0_CTL_FLD_MUTEX_EOF                  REG_FLD(3, 6)
#define DISP_MUTEX0_CTL_FLD_MUTEX_SOF_WAIT             REG_FLD(1, 5)
#define DISP_MUTEX0_CTL_FLD_MUTEX_SOF_TIMING           REG_FLD(2, 3)
#define DISP_MUTEX0_CTL_FLD_MUTEX_SOF                  REG_FLD(3, 0)

#define DISP_MUTEX0_MOD0_FLD_MUTEX_MOD0                REG_FLD(32, 0)

#define DISP_MUTEX1_EN_FLD_MUTEX_EN                    REG_FLD(1, 0)

#define DISP_MUTEX1_RST_FLD_MUTEX_RST                  REG_FLD(1, 0)

#define DISP_MUTEX1_CTL_FLD_MUTEX_EOF_WAIT             REG_FLD(1, 11)
#define DISP_MUTEX1_CTL_FLD_MUTEX_EOF_TIMING           REG_FLD(2, 9)
#define DISP_MUTEX1_CTL_FLD_MUTEX_EOF                  REG_FLD(3, 6)
#define DISP_MUTEX1_CTL_FLD_MUTEX_SOF_WAIT             REG_FLD(1, 5)
#define DISP_MUTEX1_CTL_FLD_MUTEX_SOF_TIMING           REG_FLD(2, 3)
#define DISP_MUTEX1_CTL_FLD_MUTEX_SOF                  REG_FLD(3, 0)

#define DISP_MUTEX1_MOD0_FLD_MUTEX_MOD0                REG_FLD(32, 0)

#define DISP_MUTEX2_EN_FLD_MUTEX_EN                    REG_FLD(1, 0)

#define DISP_MUTEX2_RST_FLD_MUTEX_RST                  REG_FLD(1, 0)

#define DISP_MUTEX2_CTL_FLD_MUTEX_EOF_WAIT             REG_FLD(1, 11)
#define DISP_MUTEX2_CTL_FLD_MUTEX_EOF_TIMING           REG_FLD(2, 9)
#define DISP_MUTEX2_CTL_FLD_MUTEX_EOF                  REG_FLD(3, 6)
#define DISP_MUTEX2_CTL_FLD_MUTEX_SOF_WAIT             REG_FLD(1, 5)
#define DISP_MUTEX2_CTL_FLD_MUTEX_SOF_TIMING           REG_FLD(2, 3)
#define DISP_MUTEX2_CTL_FLD_MUTEX_SOF                  REG_FLD(3, 0)

#define DISP_MUTEX2_MOD0_FLD_MUTEX_MOD0                REG_FLD(32, 0)

#define DISP_MUTEX3_EN_FLD_MUTEX_EN                    REG_FLD(1, 0)

#define DISP_MUTEX3_RST_FLD_MUTEX_RST                  REG_FLD(1, 0)

#define DISP_MUTEX3_CTL_FLD_MUTEX_EOF_WAIT             REG_FLD(1, 11)
#define DISP_MUTEX3_CTL_FLD_MUTEX_EOF_TIMING           REG_FLD(2, 9)
#define DISP_MUTEX3_CTL_FLD_MUTEX_EOF                  REG_FLD(3, 6)
#define DISP_MUTEX3_CTL_FLD_MUTEX_SOF_WAIT             REG_FLD(1, 5)
#define DISP_MUTEX3_CTL_FLD_MUTEX_SOF_TIMING           REG_FLD(2, 3)
#define DISP_MUTEX3_CTL_FLD_MUTEX_SOF                  REG_FLD(3, 0)

#define DISP_MUTEX3_MOD0_FLD_MUTEX_MOD0                REG_FLD(32, 0)

#define DISP_MUTEX4_EN_FLD_MUTEX_EN                    REG_FLD(1, 0)

#define DISP_MUTEX4_RST_FLD_MUTEX_RST                  REG_FLD(1, 0)

#define DISP_MUTEX4_CTL_FLD_MUTEX_EOF_WAIT             REG_FLD(1, 11)
#define DISP_MUTEX4_CTL_FLD_MUTEX_EOF_TIMING           REG_FLD(2, 9)
#define DISP_MUTEX4_CTL_FLD_MUTEX_EOF                  REG_FLD(3, 6)
#define DISP_MUTEX4_CTL_FLD_MUTEX_SOF_WAIT             REG_FLD(1, 5)
#define DISP_MUTEX4_CTL_FLD_MUTEX_SOF_TIMING           REG_FLD(2, 3)
#define DISP_MUTEX4_CTL_FLD_MUTEX_SOF                  REG_FLD(3, 0)

#define DISP_MUTEX4_MOD0_FLD_MUTEX_MOD0                REG_FLD(32, 0)

#define DISP_MUTEX5_EN_FLD_MUTEX_EN                    REG_FLD(1, 0)

#define DISP_MUTEX5_RST_FLD_MUTEX_RST                  REG_FLD(1, 0)

#define DISP_MUTEX5_CTL_FLD_MUTEX_EOF_WAIT             REG_FLD(1, 11)
#define DISP_MUTEX5_CTL_FLD_MUTEX_EOF_TIMING           REG_FLD(2, 9)
#define DISP_MUTEX5_CTL_FLD_MUTEX_EOF                  REG_FLD(3, 6)
#define DISP_MUTEX5_CTL_FLD_MUTEX_SOF_WAIT             REG_FLD(1, 5)
#define DISP_MUTEX5_CTL_FLD_MUTEX_SOF_TIMING           REG_FLD(2, 3)
#define DISP_MUTEX5_CTL_FLD_MUTEX_SOF                  REG_FLD(3, 0)

#define DISP_MUTEX5_MOD0_FLD_MUTEX_MOD0                REG_FLD(32, 0)

#define DISP_MUTEX6_EN_FLD_MUTEX_EN                    REG_FLD(1, 0)

#define DISP_MUTEX6_RST_FLD_MUTEX_RST                  REG_FLD(1, 0)

#define DISP_MUTEX6_CTL_FLD_MUTEX_EOF_WAIT             REG_FLD(1, 11)
#define DISP_MUTEX6_CTL_FLD_MUTEX_EOF_TIMING           REG_FLD(2, 9)
#define DISP_MUTEX6_CTL_FLD_MUTEX_EOF                  REG_FLD(3, 6)
#define DISP_MUTEX6_CTL_FLD_MUTEX_SOF_WAIT             REG_FLD(1, 5)
#define DISP_MUTEX6_CTL_FLD_MUTEX_SOF_TIMING           REG_FLD(2, 3)
#define DISP_MUTEX6_CTL_FLD_MUTEX_SOF                  REG_FLD(3, 0)

#define DISP_MUTEX6_MOD0_FLD_MUTEX_MOD0                REG_FLD(32, 0)

#define DISP_MUTEX7_EN_FLD_MUTEX_EN                    REG_FLD(1, 0)

#define DISP_MUTEX7_RST_FLD_MUTEX_RST                  REG_FLD(1, 0)

#define DISP_MUTEX7_CTL_FLD_MUTEX_EOF_WAIT             REG_FLD(1, 11)
#define DISP_MUTEX7_CTL_FLD_MUTEX_EOF_TIMING           REG_FLD(2, 9)
#define DISP_MUTEX7_CTL_FLD_MUTEX_EOF                  REG_FLD(3, 6)
#define DISP_MUTEX7_CTL_FLD_MUTEX_SOF_WAIT             REG_FLD(1, 5)
#define DISP_MUTEX7_CTL_FLD_MUTEX_SOF_TIMING           REG_FLD(2, 3)
#define DISP_MUTEX7_CTL_FLD_MUTEX_SOF                  REG_FLD(3, 0)

#define DISP_MUTEX7_MOD0_FLD_MUTEX_MOD0                REG_FLD(32, 0)

#define DISP_MUTEX8_EN_FLD_MUTEX_EN                    REG_FLD(1, 0)

#define DISP_MUTEX8_RST_FLD_MUTEX_RST                  REG_FLD(1, 0)

#define DISP_MUTEX8_CTL_FLD_MUTEX_EOF_WAIT             REG_FLD(1, 11)
#define DISP_MUTEX8_CTL_FLD_MUTEX_EOF_TIMING           REG_FLD(2, 9)
#define DISP_MUTEX8_CTL_FLD_MUTEX_EOF                  REG_FLD(3, 6)
#define DISP_MUTEX8_CTL_FLD_MUTEX_SOF_WAIT             REG_FLD(1, 5)
#define DISP_MUTEX8_CTL_FLD_MUTEX_SOF_TIMING           REG_FLD(2, 3)
#define DISP_MUTEX8_CTL_FLD_MUTEX_SOF                  REG_FLD(3, 0)

#define DISP_MUTEX8_MOD0_FLD_MUTEX_MOD0                REG_FLD(32, 0)

#define DISP_MUTEX9_EN_FLD_MUTEX_EN                    REG_FLD(1, 0)

#define DISP_MUTEX9_RST_FLD_MUTEX_RST                  REG_FLD(1, 0)

#define DISP_MUTEX9_CTL_FLD_MUTEX_EOF_WAIT             REG_FLD(1, 11)
#define DISP_MUTEX9_CTL_FLD_MUTEX_EOF_TIMING           REG_FLD(2, 9)
#define DISP_MUTEX9_CTL_FLD_MUTEX_EOF                  REG_FLD(3, 6)
#define DISP_MUTEX9_CTL_FLD_MUTEX_SOF_WAIT             REG_FLD(1, 5)
#define DISP_MUTEX9_CTL_FLD_MUTEX_SOF_TIMING           REG_FLD(2, 3)
#define DISP_MUTEX9_CTL_FLD_MUTEX_SOF                  REG_FLD(3, 0)

#define DISP_MUTEX9_MOD0_FLD_MUTEX_MOD0                REG_FLD(32, 0)

#define DEBUG_OUT_SEL_FLD_DEBUG_OUT_SEL                REG_FLD(2, 0)

#define INTEN_GET_MUTEX_INTEN(reg32)            REG_FLD_GET(INTEN_FLD_MUTEX_INTEN, (reg32))

#define INTSTA_GET_MUTEX_INTSTA(reg32)          REG_FLD_GET(INTSTA_FLD_MUTEX_INTSTA, (reg32))

#define CFG_GET_MUTEX_CFG(reg32)                REG_FLD_GET(CFG_FLD_MUTEX_CFG, (reg32))

#define DISP_MUTEX0_EN_GET_MUTEX_EN(reg32)      REG_FLD_GET(DISP_MUTEX0_EN_FLD_MUTEX_EN, (reg32))

#define DISP_MUTEX0_RST_GET_MUTEX_RST(reg32)    REG_FLD_GET(DISP_MUTEX0_RST_FLD_MUTEX_RST, (reg32))

#define DISP_MUTEX0_CTL_GET_MUTEX_EOF_WAIT(reg32)                                       \
                REG_FLD_GET(DISP_MUTEX0_CTL_FLD_MUTEX_EOF_WAIT, (reg32))
#define DISP_MUTEX0_CTL_GET_MUTEX_EOF_TIMING(reg32)                                     \
                REG_FLD_GET(DISP_MUTEX0_CTL_FLD_MUTEX_EOF_TIMING, (reg32))
#define DISP_MUTEX0_CTL_GET_MUTEX_EOF(reg32)                                            \
                REG_FLD_GET(DISP_MUTEX0_CTL_FLD_MUTEX_EOF, (reg32))
#define DISP_MUTEX0_CTL_GET_MUTEX_SOF_WAIT(reg32)                                       \
                REG_FLD_GET(DISP_MUTEX0_CTL_FLD_MUTEX_SOF_WAIT, (reg32))
#define DISP_MUTEX0_CTL_GET_MUTEX_SOF_TIMING(reg32)                                     \
                REG_FLD_GET(DISP_MUTEX0_CTL_FLD_MUTEX_SOF_TIMING, (reg32))
#define DISP_MUTEX0_CTL_GET_MUTEX_SOF(reg32)                                            \
                REG_FLD_GET(DISP_MUTEX0_CTL_FLD_MUTEX_SOF, (reg32))

#define DISP_MUTEX0_MOD0_GET_MUTEX_MOD0(reg32)                                          \
                REG_FLD_GET(DISP_MUTEX0_MOD0_FLD_MUTEX_MOD0, (reg32))

#define DISP_MUTEX1_EN_GET_MUTEX_EN(reg32)                                              \
                REG_FLD_GET(DISP_MUTEX1_EN_FLD_MUTEX_EN, (reg32))

#define DISP_MUTEX1_RST_GET_MUTEX_RST(reg32)                                            \
                REG_FLD_GET(DISP_MUTEX1_RST_FLD_MUTEX_RST, (reg32))

#define DISP_MUTEX1_CTL_GET_MUTEX_EOF_WAIT(reg32)                                       \
                REG_FLD_GET(DISP_MUTEX1_CTL_FLD_MUTEX_EOF_WAIT, (reg32))
#define DISP_MUTEX1_CTL_GET_MUTEX_EOF_TIMING(reg32)                                     \
                REG_FLD_GET(DISP_MUTEX1_CTL_FLD_MUTEX_EOF_TIMING, (reg32))
#define DISP_MUTEX1_CTL_GET_MUTEX_EOF(reg32)                                            \
                REG_FLD_GET(DISP_MUTEX1_CTL_FLD_MUTEX_EOF, (reg32))
#define DISP_MUTEX1_CTL_GET_MUTEX_SOF_WAIT(reg32)                                       \
                REG_FLD_GET(DISP_MUTEX1_CTL_FLD_MUTEX_SOF_WAIT, (reg32))
#define DISP_MUTEX1_CTL_GET_MUTEX_SOF_TIMING(reg32)                                     \
                REG_FLD_GET(DISP_MUTEX1_CTL_FLD_MUTEX_SOF_TIMING, (reg32))
#define DISP_MUTEX1_CTL_GET_MUTEX_SOF(reg32)                                            \
                REG_FLD_GET(DISP_MUTEX1_CTL_FLD_MUTEX_SOF, (reg32))

#define DISP_MUTEX1_MOD0_GET_MUTEX_MOD0(reg32)                                          \
                REG_FLD_GET(DISP_MUTEX1_MOD0_FLD_MUTEX_MOD0, (reg32))

#define DISP_MUTEX2_EN_GET_MUTEX_EN(reg32)                                              \
                REG_FLD_GET(DISP_MUTEX2_EN_FLD_MUTEX_EN, (reg32))

#define DISP_MUTEX2_RST_GET_MUTEX_RST(reg32)                                            \
                REG_FLD_GET(DISP_MUTEX2_RST_FLD_MUTEX_RST, (reg32))

#define DISP_MUTEX2_CTL_GET_MUTEX_EOF_WAIT(reg32)                                       \
                REG_FLD_GET(DISP_MUTEX2_CTL_FLD_MUTEX_EOF_WAIT, (reg32))
#define DISP_MUTEX2_CTL_GET_MUTEX_EOF_TIMING(reg32)                                     \
                REG_FLD_GET(DISP_MUTEX2_CTL_FLD_MUTEX_EOF_TIMING, (reg32))
#define DISP_MUTEX2_CTL_GET_MUTEX_EOF(reg32)                                            \
                REG_FLD_GET(DISP_MUTEX2_CTL_FLD_MUTEX_EOF, (reg32))
#define DISP_MUTEX2_CTL_GET_MUTEX_SOF_WAIT(reg32)                                       \
                REG_FLD_GET(DISP_MUTEX2_CTL_FLD_MUTEX_SOF_WAIT, (reg32))
#define DISP_MUTEX2_CTL_GET_MUTEX_SOF_TIMING(reg32)                                     \
                REG_FLD_GET(DISP_MUTEX2_CTL_FLD_MUTEX_SOF_TIMING, (reg32))
#define DISP_MUTEX2_CTL_GET_MUTEX_SOF(reg32)                                            \
                REG_FLD_GET(DISP_MUTEX2_CTL_FLD_MUTEX_SOF, (reg32))

#define DISP_MUTEX2_MOD0_GET_MUTEX_MOD0(reg32)                                          \
                REG_FLD_GET(DISP_MUTEX2_MOD0_FLD_MUTEX_MOD0, (reg32))

#define DISP_MUTEX3_EN_GET_MUTEX_EN(reg32)                                              \
                REG_FLD_GET(DISP_MUTEX3_EN_FLD_MUTEX_EN, (reg32))

#define DISP_MUTEX3_RST_GET_MUTEX_RST(reg32)                                            \
                REG_FLD_GET(DISP_MUTEX3_RST_FLD_MUTEX_RST, (reg32))

#define DISP_MUTEX3_CTL_GET_MUTEX_EOF_WAIT(reg32)                                       \
                REG_FLD_GET(DISP_MUTEX3_CTL_FLD_MUTEX_EOF_WAIT, (reg32))
#define DISP_MUTEX3_CTL_GET_MUTEX_EOF_TIMING(reg32)                                     \
                REG_FLD_GET(DISP_MUTEX3_CTL_FLD_MUTEX_EOF_TIMING, (reg32))
#define DISP_MUTEX3_CTL_GET_MUTEX_EOF(reg32)                                            \
                REG_FLD_GET(DISP_MUTEX3_CTL_FLD_MUTEX_EOF, (reg32))
#define DISP_MUTEX3_CTL_GET_MUTEX_SOF_WAIT(reg32)                                       \
                REG_FLD_GET(DISP_MUTEX3_CTL_FLD_MUTEX_SOF_WAIT, (reg32))
#define DISP_MUTEX3_CTL_GET_MUTEX_SOF_TIMING(reg32)                                     \
                REG_FLD_GET(DISP_MUTEX3_CTL_FLD_MUTEX_SOF_TIMING, (reg32))
#define DISP_MUTEX3_CTL_GET_MUTEX_SOF(reg32)                                            \
                REG_FLD_GET(DISP_MUTEX3_CTL_FLD_MUTEX_SOF, (reg32))

#define DISP_MUTEX3_MOD0_GET_MUTEX_MOD0(reg32)                                          \
                REG_FLD_GET(DISP_MUTEX3_MOD0_FLD_MUTEX_MOD0, (reg32))

#define DISP_MUTEX4_EN_GET_MUTEX_EN(reg32)                                              \
                REG_FLD_GET(DISP_MUTEX4_EN_FLD_MUTEX_EN, (reg32))

#define DISP_MUTEX4_RST_GET_MUTEX_RST(reg32)                                            \
                REG_FLD_GET(DISP_MUTEX4_RST_FLD_MUTEX_RST, (reg32))

#define DISP_MUTEX4_CTL_GET_MUTEX_EOF_WAIT(reg32)                                       \
                REG_FLD_GET(DISP_MUTEX4_CTL_FLD_MUTEX_EOF_WAIT, (reg32))
#define DISP_MUTEX4_CTL_GET_MUTEX_EOF_TIMING(reg32)                                     \
                REG_FLD_GET(DISP_MUTEX4_CTL_FLD_MUTEX_EOF_TIMING, (reg32))
#define DISP_MUTEX4_CTL_GET_MUTEX_EOF(reg32)                                            \
                REG_FLD_GET(DISP_MUTEX4_CTL_FLD_MUTEX_EOF, (reg32))
#define DISP_MUTEX4_CTL_GET_MUTEX_SOF_WAIT(reg32)                                       \
                REG_FLD_GET(DISP_MUTEX4_CTL_FLD_MUTEX_SOF_WAIT, (reg32))
#define DISP_MUTEX4_CTL_GET_MUTEX_SOF_TIMING(reg32)                                     \
                REG_FLD_GET(DISP_MUTEX4_CTL_FLD_MUTEX_SOF_TIMING, (reg32))
#define DISP_MUTEX4_CTL_GET_MUTEX_SOF(reg32)                                            \
                REG_FLD_GET(DISP_MUTEX4_CTL_FLD_MUTEX_SOF, (reg32))

#define DISP_MUTEX4_MOD0_GET_MUTEX_MOD0(reg32)                                          \
                REG_FLD_GET(DISP_MUTEX4_MOD0_FLD_MUTEX_MOD0, (reg32))

#define DISP_MUTEX5_EN_GET_MUTEX_EN(reg32)                                              \
                REG_FLD_GET(DISP_MUTEX5_EN_FLD_MUTEX_EN, (reg32))

#define DISP_MUTEX5_RST_GET_MUTEX_RST(reg32)                                            \
                REG_FLD_GET(DISP_MUTEX5_RST_FLD_MUTEX_RST, (reg32))

#define DISP_MUTEX5_CTL_GET_MUTEX_EOF_WAIT(reg32)                                       \
                REG_FLD_GET(DISP_MUTEX5_CTL_FLD_MUTEX_EOF_WAIT, (reg32))
#define DISP_MUTEX5_CTL_GET_MUTEX_EOF_TIMING(reg32)                                     \
                REG_FLD_GET(DISP_MUTEX5_CTL_FLD_MUTEX_EOF_TIMING, (reg32))
#define DISP_MUTEX5_CTL_GET_MUTEX_EOF(reg32)                                            \
                REG_FLD_GET(DISP_MUTEX5_CTL_FLD_MUTEX_EOF, (reg32))
#define DISP_MUTEX5_CTL_GET_MUTEX_SOF_WAIT(reg32)                                       \
                REG_FLD_GET(DISP_MUTEX5_CTL_FLD_MUTEX_SOF_WAIT, (reg32))
#define DISP_MUTEX5_CTL_GET_MUTEX_SOF_TIMING(reg32)                                     \
                REG_FLD_GET(DISP_MUTEX5_CTL_FLD_MUTEX_SOF_TIMING, (reg32))
#define DISP_MUTEX5_CTL_GET_MUTEX_SOF(reg32)                                            \
                REG_FLD_GET(DISP_MUTEX5_CTL_FLD_MUTEX_SOF, (reg32))

#define DISP_MUTEX5_MOD0_GET_MUTEX_MOD0(reg32)                                          \
                REG_FLD_GET(DISP_MUTEX5_MOD0_FLD_MUTEX_MOD0, (reg32))

#define DISP_MUTEX6_EN_GET_MUTEX_EN(reg32)                                              \
                REG_FLD_GET(DISP_MUTEX6_EN_FLD_MUTEX_EN, (reg32))

#define DISP_MUTEX6_RST_GET_MUTEX_RST(reg32)                                            \
                REG_FLD_GET(DISP_MUTEX6_RST_FLD_MUTEX_RST, (reg32))

#define DISP_MUTEX6_CTL_GET_MUTEX_EOF_WAIT(reg32)                                       \
                REG_FLD_GET(DISP_MUTEX6_CTL_FLD_MUTEX_EOF_WAIT, (reg32))
#define DISP_MUTEX6_CTL_GET_MUTEX_EOF_TIMING(reg32)                                     \
                REG_FLD_GET(DISP_MUTEX6_CTL_FLD_MUTEX_EOF_TIMING, (reg32))
#define DISP_MUTEX6_CTL_GET_MUTEX_EOF(reg32)                                            \
                REG_FLD_GET(DISP_MUTEX6_CTL_FLD_MUTEX_EOF, (reg32))
#define DISP_MUTEX6_CTL_GET_MUTEX_SOF_WAIT(reg32)                                       \
                REG_FLD_GET(DISP_MUTEX6_CTL_FLD_MUTEX_SOF_WAIT, (reg32))
#define DISP_MUTEX6_CTL_GET_MUTEX_SOF_TIMING(reg32)                                     \
                REG_FLD_GET(DISP_MUTEX6_CTL_FLD_MUTEX_SOF_TIMING, (reg32))
#define DISP_MUTEX6_CTL_GET_MUTEX_SOF(reg32)                                            \
                REG_FLD_GET(DISP_MUTEX6_CTL_FLD_MUTEX_SOF, (reg32))

#define DISP_MUTEX6_MOD0_GET_MUTEX_MOD0(reg32)                                          \
                REG_FLD_GET(DISP_MUTEX6_MOD0_FLD_MUTEX_MOD0, (reg32))

#define DISP_MUTEX7_EN_GET_MUTEX_EN(reg32)                                              \
                REG_FLD_GET(DISP_MUTEX7_EN_FLD_MUTEX_EN, (reg32))

#define DISP_MUTEX7_RST_GET_MUTEX_RST(reg32)                                            \
                REG_FLD_GET(DISP_MUTEX7_RST_FLD_MUTEX_RST, (reg32))

#define DISP_MUTEX7_CTL_GET_MUTEX_EOF_WAIT(reg32)                                       \
                REG_FLD_GET(DISP_MUTEX7_CTL_FLD_MUTEX_EOF_WAIT, (reg32))
#define DISP_MUTEX7_CTL_GET_MUTEX_EOF_TIMING(reg32)                                     \
                REG_FLD_GET(DISP_MUTEX7_CTL_FLD_MUTEX_EOF_TIMING, (reg32))
#define DISP_MUTEX7_CTL_GET_MUTEX_EOF(reg32)                                            \
                REG_FLD_GET(DISP_MUTEX7_CTL_FLD_MUTEX_EOF, (reg32))
#define DISP_MUTEX7_CTL_GET_MUTEX_SOF_WAIT(reg32)                                       \
                REG_FLD_GET(DISP_MUTEX7_CTL_FLD_MUTEX_SOF_WAIT, (reg32))
#define DISP_MUTEX7_CTL_GET_MUTEX_SOF_TIMING(reg32)                                     \
                REG_FLD_GET(DISP_MUTEX7_CTL_FLD_MUTEX_SOF_TIMING, (reg32))
#define DISP_MUTEX7_CTL_GET_MUTEX_SOF(reg32)                                            \
                REG_FLD_GET(DISP_MUTEX7_CTL_FLD_MUTEX_SOF, (reg32))

#define DISP_MUTEX7_MOD0_GET_MUTEX_MOD0(reg32)                                          \
                REG_FLD_GET(DISP_MUTEX7_MOD0_FLD_MUTEX_MOD0, (reg32))

#define DISP_MUTEX8_EN_GET_MUTEX_EN(reg32)                                              \
                REG_FLD_GET(DISP_MUTEX8_EN_FLD_MUTEX_EN, (reg32))

#define DISP_MUTEX8_RST_GET_MUTEX_RST(reg32)                                            \
                REG_FLD_GET(DISP_MUTEX8_RST_FLD_MUTEX_RST, (reg32))

#define DISP_MUTEX8_CTL_GET_MUTEX_EOF_WAIT(reg32)                                       \
                REG_FLD_GET(DISP_MUTEX8_CTL_FLD_MUTEX_EOF_WAIT, (reg32))
#define DISP_MUTEX8_CTL_GET_MUTEX_EOF_TIMING(reg32)                                     \
                REG_FLD_GET(DISP_MUTEX8_CTL_FLD_MUTEX_EOF_TIMING, (reg32))
#define DISP_MUTEX8_CTL_GET_MUTEX_EOF(reg32)                                            \
                REG_FLD_GET(DISP_MUTEX8_CTL_FLD_MUTEX_EOF, (reg32))
#define DISP_MUTEX8_CTL_GET_MUTEX_SOF_WAIT(reg32)                                       \
                REG_FLD_GET(DISP_MUTEX8_CTL_FLD_MUTEX_SOF_WAIT, (reg32))
#define DISP_MUTEX8_CTL_GET_MUTEX_SOF_TIMING(reg32)                                     \
                REG_FLD_GET(DISP_MUTEX8_CTL_FLD_MUTEX_SOF_TIMING, (reg32))
#define DISP_MUTEX8_CTL_GET_MUTEX_SOF(reg32)                                            \
                REG_FLD_GET(DISP_MUTEX8_CTL_FLD_MUTEX_SOF, (reg32))

#define DISP_MUTEX8_MOD0_GET_MUTEX_MOD0(reg32)                                          \
                REG_FLD_GET(DISP_MUTEX8_MOD0_FLD_MUTEX_MOD0, (reg32))

#define DISP_MUTEX9_EN_GET_MUTEX_EN(reg32)                                              \
                REG_FLD_GET(DISP_MUTEX9_EN_FLD_MUTEX_EN, (reg32))

#define DISP_MUTEX9_RST_GET_MUTEX_RST(reg32)                                            \
                REG_FLD_GET(DISP_MUTEX9_RST_FLD_MUTEX_RST, (reg32))

#define DISP_MUTEX9_CTL_GET_MUTEX_EOF_WAIT(reg32)                                       \
                REG_FLD_GET(DISP_MUTEX9_CTL_FLD_MUTEX_EOF_WAIT, (reg32))
#define DISP_MUTEX9_CTL_GET_MUTEX_EOF_TIMING(reg32)                                     \
                REG_FLD_GET(DISP_MUTEX9_CTL_FLD_MUTEX_EOF_TIMING, (reg32))
#define DISP_MUTEX9_CTL_GET_MUTEX_EOF(reg32)                                            \
                REG_FLD_GET(DISP_MUTEX9_CTL_FLD_MUTEX_EOF, (reg32))
#define DISP_MUTEX9_CTL_GET_MUTEX_SOF_WAIT(reg32)                                       \
                REG_FLD_GET(DISP_MUTEX9_CTL_FLD_MUTEX_SOF_WAIT, (reg32))
#define DISP_MUTEX9_CTL_GET_MUTEX_SOF_TIMING(reg32)                                     \
                REG_FLD_GET(DISP_MUTEX9_CTL_FLD_MUTEX_SOF_TIMING, (reg32))
#define DISP_MUTEX9_CTL_GET_MUTEX_SOF(reg32)                                            \
                REG_FLD_GET(DISP_MUTEX9_CTL_FLD_MUTEX_SOF, (reg32))

#define DISP_MUTEX9_MOD0_GET_MUTEX_MOD0(reg32)                                          \
                REG_FLD_GET(DISP_MUTEX9_MOD0_FLD_MUTEX_MOD0, (reg32))

#define DEBUG_OUT_SEL_GET_DEBUG_OUT_SEL(reg32)                                          \
                REG_FLD_GET(DEBUG_OUT_SEL_FLD_DEBUG_OUT_SEL, (reg32))

#define INTEN_SET_MUTEX_INTEN(reg32, val)                                               \
                REG_FLD_SET(INTEN_FLD_MUTEX_INTEN, (reg32), (val))

#define INTSTA_SET_MUTEX_INTSTA(reg32, val)                                             \
                REG_FLD_SET(INTSTA_FLD_MUTEX_INTSTA, (reg32), (val))

#define CFG_SET_MUTEX_CFG(reg32, val)                                                   \
                REG_FLD_SET(CFG_FLD_MUTEX_CFG, (reg32), (val))

#define DISP_MUTEX0_EN_SET_MUTEX_EN(reg32, val)                                         \
                REG_FLD_SET(DISP_MUTEX0_EN_FLD_MUTEX_EN, (reg32), (val))

#define DISP_MUTEX0_RST_SET_MUTEX_RST(reg32, val)                                       \
                REG_FLD_SET(DISP_MUTEX0_RST_FLD_MUTEX_RST, (reg32), (val))

#define DISP_MUTEX0_CTL_SET_MUTEX_EOF_WAIT(reg32, val)                                  \
                REG_FLD_SET(DISP_MUTEX0_CTL_FLD_MUTEX_EOF_WAIT, (reg32), (val))
#define DISP_MUTEX0_CTL_SET_MUTEX_EOF_TIMING(reg32, val)                                \
                REG_FLD_SET(DISP_MUTEX0_CTL_FLD_MUTEX_EOF_TIMING, (reg32), (val))
#define DISP_MUTEX0_CTL_SET_MUTEX_EOF(reg32, val)                                       \
                REG_FLD_SET(DISP_MUTEX0_CTL_FLD_MUTEX_EOF, (reg32), (val))
#define DISP_MUTEX0_CTL_SET_MUTEX_SOF_WAIT(reg32, val)                                  \
                REG_FLD_SET(DISP_MUTEX0_CTL_FLD_MUTEX_SOF_WAIT, (reg32), (val))
#define DISP_MUTEX0_CTL_SET_MUTEX_SOF_TIMING(reg32, val)                                \
                REG_FLD_SET(DISP_MUTEX0_CTL_FLD_MUTEX_SOF_TIMING, (reg32), (val))
#define DISP_MUTEX0_CTL_SET_MUTEX_SOF(reg32, val)                                       \
                REG_FLD_SET(DISP_MUTEX0_CTL_FLD_MUTEX_SOF, (reg32), (val))

#define DISP_MUTEX0_MOD0_SET_MUTEX_MOD0(reg32, val)                                     \
                REG_FLD_SET(DISP_MUTEX0_MOD0_FLD_MUTEX_MOD0, (reg32), (val))

#define DISP_MUTEX1_EN_SET_MUTEX_EN(reg32, val)                                         \
                REG_FLD_SET(DISP_MUTEX1_EN_FLD_MUTEX_EN, (reg32), (val))

#define DISP_MUTEX1_RST_SET_MUTEX_RST(reg32, val)                                       \
                REG_FLD_SET(DISP_MUTEX1_RST_FLD_MUTEX_RST, (reg32), (val))

#define DISP_MUTEX1_CTL_SET_MUTEX_EOF_WAIT(reg32, val)                                  \
                REG_FLD_SET(DISP_MUTEX1_CTL_FLD_MUTEX_EOF_WAIT, (reg32), (val))
#define DISP_MUTEX1_CTL_SET_MUTEX_EOF_TIMING(reg32, val)                                \
                REG_FLD_SET(DISP_MUTEX1_CTL_FLD_MUTEX_EOF_TIMING, (reg32), (val))
#define DISP_MUTEX1_CTL_SET_MUTEX_EOF(reg32, val)                                       \
                REG_FLD_SET(DISP_MUTEX1_CTL_FLD_MUTEX_EOF, (reg32), (val))
#define DISP_MUTEX1_CTL_SET_MUTEX_SOF_WAIT(reg32, val)                                  \
                REG_FLD_SET(DISP_MUTEX1_CTL_FLD_MUTEX_SOF_WAIT, (reg32), (val))
#define DISP_MUTEX1_CTL_SET_MUTEX_SOF_TIMING(reg32, val)                                \
                REG_FLD_SET(DISP_MUTEX1_CTL_FLD_MUTEX_SOF_TIMING, (reg32), (val))
#define DISP_MUTEX1_CTL_SET_MUTEX_SOF(reg32, val)                                       \
                REG_FLD_SET(DISP_MUTEX1_CTL_FLD_MUTEX_SOF, (reg32), (val))

#define DISP_MUTEX1_MOD0_SET_MUTEX_MOD0(reg32, val)                                     \
                REG_FLD_SET(DISP_MUTEX1_MOD0_FLD_MUTEX_MOD0, (reg32), (val))

#define DISP_MUTEX2_EN_SET_MUTEX_EN(reg32, val)                                         \
                REG_FLD_SET(DISP_MUTEX2_EN_FLD_MUTEX_EN, (reg32), (val))

#define DISP_MUTEX2_RST_SET_MUTEX_RST(reg32, val)                                       \
                REG_FLD_SET(DISP_MUTEX2_RST_FLD_MUTEX_RST, (reg32), (val))

#define DISP_MUTEX2_CTL_SET_MUTEX_EOF_WAIT(reg32, val)                                  \
                REG_FLD_SET(DISP_MUTEX2_CTL_FLD_MUTEX_EOF_WAIT, (reg32), (val))
#define DISP_MUTEX2_CTL_SET_MUTEX_EOF_TIMING(reg32, val)                                \
                REG_FLD_SET(DISP_MUTEX2_CTL_FLD_MUTEX_EOF_TIMING, (reg32), (val))
#define DISP_MUTEX2_CTL_SET_MUTEX_EOF(reg32, val)                                       \
                REG_FLD_SET(DISP_MUTEX2_CTL_FLD_MUTEX_EOF, (reg32), (val))
#define DISP_MUTEX2_CTL_SET_MUTEX_SOF_WAIT(reg32, val)                                  \
                REG_FLD_SET(DISP_MUTEX2_CTL_FLD_MUTEX_SOF_WAIT, (reg32), (val))
#define DISP_MUTEX2_CTL_SET_MUTEX_SOF_TIMING(reg32, val)                                \
                REG_FLD_SET(DISP_MUTEX2_CTL_FLD_MUTEX_SOF_TIMING, (reg32), (val))
#define DISP_MUTEX2_CTL_SET_MUTEX_SOF(reg32, val)                                       \
                REG_FLD_SET(DISP_MUTEX2_CTL_FLD_MUTEX_SOF, (reg32), (val))

#define DISP_MUTEX2_MOD0_SET_MUTEX_MOD0(reg32, val)                                     \
                REG_FLD_SET(DISP_MUTEX2_MOD0_FLD_MUTEX_MOD0, (reg32), (val))

#define DISP_MUTEX3_EN_SET_MUTEX_EN(reg32, val)                                         \
                REG_FLD_SET(DISP_MUTEX3_EN_FLD_MUTEX_EN, (reg32), (val))

#define DISP_MUTEX3_RST_SET_MUTEX_RST(reg32, val)                                       \
                REG_FLD_SET(DISP_MUTEX3_RST_FLD_MUTEX_RST, (reg32), (val))

#define DISP_MUTEX3_CTL_SET_MUTEX_EOF_WAIT(reg32, val)                                  \
                REG_FLD_SET(DISP_MUTEX3_CTL_FLD_MUTEX_EOF_WAIT, (reg32), (val))
#define DISP_MUTEX3_CTL_SET_MUTEX_EOF_TIMING(reg32, val)                                \
                REG_FLD_SET(DISP_MUTEX3_CTL_FLD_MUTEX_EOF_TIMING, (reg32), (val))
#define DISP_MUTEX3_CTL_SET_MUTEX_EOF(reg32, val)                                       \
                REG_FLD_SET(DISP_MUTEX3_CTL_FLD_MUTEX_EOF, (reg32), (val))
#define DISP_MUTEX3_CTL_SET_MUTEX_SOF_WAIT(reg32, val)                                  \
                REG_FLD_SET(DISP_MUTEX3_CTL_FLD_MUTEX_SOF_WAIT, (reg32), (val))
#define DISP_MUTEX3_CTL_SET_MUTEX_SOF_TIMING(reg32, val)                                \
                REG_FLD_SET(DISP_MUTEX3_CTL_FLD_MUTEX_SOF_TIMING, (reg32), (val))
#define DISP_MUTEX3_CTL_SET_MUTEX_SOF(reg32, val)                                       \
                REG_FLD_SET(DISP_MUTEX3_CTL_FLD_MUTEX_SOF, (reg32), (val))

#define DISP_MUTEX3_MOD0_SET_MUTEX_MOD0(reg32, val)                                     \
                REG_FLD_SET(DISP_MUTEX3_MOD0_FLD_MUTEX_MOD0, (reg32), (val))

#define DISP_MUTEX4_EN_SET_MUTEX_EN(reg32, val)                                         \
                REG_FLD_SET(DISP_MUTEX4_EN_FLD_MUTEX_EN, (reg32), (val))

#define DISP_MUTEX4_RST_SET_MUTEX_RST(reg32, val)                                       \
                REG_FLD_SET(DISP_MUTEX4_RST_FLD_MUTEX_RST, (reg32), (val))

#define DISP_MUTEX4_CTL_SET_MUTEX_EOF_WAIT(reg32, val)                                  \
                REG_FLD_SET(DISP_MUTEX4_CTL_FLD_MUTEX_EOF_WAIT, (reg32), (val))
#define DISP_MUTEX4_CTL_SET_MUTEX_EOF_TIMING(reg32, val)                                \
                REG_FLD_SET(DISP_MUTEX4_CTL_FLD_MUTEX_EOF_TIMING, (reg32), (val))
#define DISP_MUTEX4_CTL_SET_MUTEX_EOF(reg32, val)                                       \
                REG_FLD_SET(DISP_MUTEX4_CTL_FLD_MUTEX_EOF, (reg32), (val))
#define DISP_MUTEX4_CTL_SET_MUTEX_SOF_WAIT(reg32, val)                                  \
                REG_FLD_SET(DISP_MUTEX4_CTL_FLD_MUTEX_SOF_WAIT, (reg32), (val))
#define DISP_MUTEX4_CTL_SET_MUTEX_SOF_TIMING(reg32, val)                                \
                REG_FLD_SET(DISP_MUTEX4_CTL_FLD_MUTEX_SOF_TIMING, (reg32), (val))
#define DISP_MUTEX4_CTL_SET_MUTEX_SOF(reg32, val)                                       \
                REG_FLD_SET(DISP_MUTEX4_CTL_FLD_MUTEX_SOF, (reg32), (val))

#define DISP_MUTEX4_MOD0_SET_MUTEX_MOD0(reg32, val)                                     \
                REG_FLD_SET(DISP_MUTEX4_MOD0_FLD_MUTEX_MOD0, (reg32), (val))

#define DISP_MUTEX5_EN_SET_MUTEX_EN(reg32, val)                                         \
                REG_FLD_SET(DISP_MUTEX5_EN_FLD_MUTEX_EN, (reg32), (val))

#define DISP_MUTEX5_RST_SET_MUTEX_RST(reg32, val)                                       \
                REG_FLD_SET(DISP_MUTEX5_RST_FLD_MUTEX_RST, (reg32), (val))

#define DISP_MUTEX5_CTL_SET_MUTEX_EOF_WAIT(reg32, val)                                  \
                REG_FLD_SET(DISP_MUTEX5_CTL_FLD_MUTEX_EOF_WAIT, (reg32), (val))
#define DISP_MUTEX5_CTL_SET_MUTEX_EOF_TIMING(reg32, val)                                \
                REG_FLD_SET(DISP_MUTEX5_CTL_FLD_MUTEX_EOF_TIMING, (reg32), (val))
#define DISP_MUTEX5_CTL_SET_MUTEX_EOF(reg32, val)                                       \
                REG_FLD_SET(DISP_MUTEX5_CTL_FLD_MUTEX_EOF, (reg32), (val))
#define DISP_MUTEX5_CTL_SET_MUTEX_SOF_WAIT(reg32, val)                                  \
                REG_FLD_SET(DISP_MUTEX5_CTL_FLD_MUTEX_SOF_WAIT, (reg32), (val))
#define DISP_MUTEX5_CTL_SET_MUTEX_SOF_TIMING(reg32, val)                                \
                REG_FLD_SET(DISP_MUTEX5_CTL_FLD_MUTEX_SOF_TIMING, (reg32), (val))
#define DISP_MUTEX5_CTL_SET_MUTEX_SOF(reg32, val)                                       \
                REG_FLD_SET(DISP_MUTEX5_CTL_FLD_MUTEX_SOF, (reg32), (val))

#define DISP_MUTEX5_MOD0_SET_MUTEX_MOD0(reg32, val)                                     \
                REG_FLD_SET(DISP_MUTEX5_MOD0_FLD_MUTEX_MOD0, (reg32), (val))

#define DISP_MUTEX6_EN_SET_MUTEX_EN(reg32, val)                                         \
                REG_FLD_SET(DISP_MUTEX6_EN_FLD_MUTEX_EN, (reg32), (val))

#define DISP_MUTEX6_RST_SET_MUTEX_RST(reg32, val)                                       \
                REG_FLD_SET(DISP_MUTEX6_RST_FLD_MUTEX_RST, (reg32), (val))

#define DISP_MUTEX6_CTL_SET_MUTEX_EOF_WAIT(reg32, val)                                  \
                REG_FLD_SET(DISP_MUTEX6_CTL_FLD_MUTEX_EOF_WAIT, (reg32), (val))
#define DISP_MUTEX6_CTL_SET_MUTEX_EOF_TIMING(reg32, val)                                \
                REG_FLD_SET(DISP_MUTEX6_CTL_FLD_MUTEX_EOF_TIMING, (reg32), (val))
#define DISP_MUTEX6_CTL_SET_MUTEX_EOF(reg32, val)                                       \
                REG_FLD_SET(DISP_MUTEX6_CTL_FLD_MUTEX_EOF, (reg32), (val))
#define DISP_MUTEX6_CTL_SET_MUTEX_SOF_WAIT(reg32, val)                                  \
                REG_FLD_SET(DISP_MUTEX6_CTL_FLD_MUTEX_SOF_WAIT, (reg32), (val))
#define DISP_MUTEX6_CTL_SET_MUTEX_SOF_TIMING(reg32, val)                                \
                REG_FLD_SET(DISP_MUTEX6_CTL_FLD_MUTEX_SOF_TIMING, (reg32), (val))
#define DISP_MUTEX6_CTL_SET_MUTEX_SOF(reg32, val)                                       \
                REG_FLD_SET(DISP_MUTEX6_CTL_FLD_MUTEX_SOF, (reg32), (val))

#define DISP_MUTEX6_MOD0_SET_MUTEX_MOD0(reg32, val)                                     \
                REG_FLD_SET(DISP_MUTEX6_MOD0_FLD_MUTEX_MOD0, (reg32), (val))

#define DISP_MUTEX7_EN_SET_MUTEX_EN(reg32, val)                                         \
                REG_FLD_SET(DISP_MUTEX7_EN_FLD_MUTEX_EN, (reg32), (val))

#define DISP_MUTEX7_RST_SET_MUTEX_RST(reg32, val)                                       \
                REG_FLD_SET(DISP_MUTEX7_RST_FLD_MUTEX_RST, (reg32), (val))

#define DISP_MUTEX7_CTL_SET_MUTEX_EOF_WAIT(reg32, val)                                  \
                REG_FLD_SET(DISP_MUTEX7_CTL_FLD_MUTEX_EOF_WAIT, (reg32), (val))
#define DISP_MUTEX7_CTL_SET_MUTEX_EOF_TIMING(reg32, val)                                \
                REG_FLD_SET(DISP_MUTEX7_CTL_FLD_MUTEX_EOF_TIMING, (reg32), (val))
#define DISP_MUTEX7_CTL_SET_MUTEX_EOF(reg32, val)                                       \
                REG_FLD_SET(DISP_MUTEX7_CTL_FLD_MUTEX_EOF, (reg32), (val))
#define DISP_MUTEX7_CTL_SET_MUTEX_SOF_WAIT(reg32, val)                                  \
                REG_FLD_SET(DISP_MUTEX7_CTL_FLD_MUTEX_SOF_WAIT, (reg32), (val))
#define DISP_MUTEX7_CTL_SET_MUTEX_SOF_TIMING(reg32, val)                                \
                REG_FLD_SET(DISP_MUTEX7_CTL_FLD_MUTEX_SOF_TIMING, (reg32), (val))
#define DISP_MUTEX7_CTL_SET_MUTEX_SOF(reg32, val)                                       \
                REG_FLD_SET(DISP_MUTEX7_CTL_FLD_MUTEX_SOF, (reg32), (val))

#define DISP_MUTEX7_MOD0_SET_MUTEX_MOD0(reg32, val)                                     \
                REG_FLD_SET(DISP_MUTEX7_MOD0_FLD_MUTEX_MOD0, (reg32), (val))

#define DISP_MUTEX8_EN_SET_MUTEX_EN(reg32, val)                                         \
                REG_FLD_SET(DISP_MUTEX8_EN_FLD_MUTEX_EN, (reg32), (val))

#define DISP_MUTEX8_RST_SET_MUTEX_RST(reg32, val)                                       \
                REG_FLD_SET(DISP_MUTEX8_RST_FLD_MUTEX_RST, (reg32), (val))

#define DISP_MUTEX8_CTL_SET_MUTEX_EOF_WAIT(reg32, val)                                  \
                REG_FLD_SET(DISP_MUTEX8_CTL_FLD_MUTEX_EOF_WAIT, (reg32), (val))
#define DISP_MUTEX8_CTL_SET_MUTEX_EOF_TIMING(reg32, val)                                \
                REG_FLD_SET(DISP_MUTEX8_CTL_FLD_MUTEX_EOF_TIMING, (reg32), (val))
#define DISP_MUTEX8_CTL_SET_MUTEX_EOF(reg32, val)                                       \
                REG_FLD_SET(DISP_MUTEX8_CTL_FLD_MUTEX_EOF, (reg32), (val))
#define DISP_MUTEX8_CTL_SET_MUTEX_SOF_WAIT(reg32, val)                                  \
                REG_FLD_SET(DISP_MUTEX8_CTL_FLD_MUTEX_SOF_WAIT, (reg32), (val))
#define DISP_MUTEX8_CTL_SET_MUTEX_SOF_TIMING(reg32, val)                                \
                REG_FLD_SET(DISP_MUTEX8_CTL_FLD_MUTEX_SOF_TIMING, (reg32), (val))
#define DISP_MUTEX8_CTL_SET_MUTEX_SOF(reg32, val)                                       \
                REG_FLD_SET(DISP_MUTEX8_CTL_FLD_MUTEX_SOF, (reg32), (val))

#define DISP_MUTEX8_MOD0_SET_MUTEX_MOD0(reg32, val)                                     \
                REG_FLD_SET(DISP_MUTEX8_MOD0_FLD_MUTEX_MOD0, (reg32), (val))

#define DISP_MUTEX9_EN_SET_MUTEX_EN(reg32, val)                                         \
                REG_FLD_SET(DISP_MUTEX9_EN_FLD_MUTEX_EN, (reg32), (val))

#define DISP_MUTEX9_RST_SET_MUTEX_RST(reg32, val)                                       \
                REG_FLD_SET(DISP_MUTEX9_RST_FLD_MUTEX_RST, (reg32), (val))

#define DISP_MUTEX9_CTL_SET_MUTEX_EOF_WAIT(reg32, val)                                  \
                REG_FLD_SET(DISP_MUTEX9_CTL_FLD_MUTEX_EOF_WAIT, (reg32), (val))
#define DISP_MUTEX9_CTL_SET_MUTEX_EOF_TIMING(reg32, val)                                \
                REG_FLD_SET(DISP_MUTEX9_CTL_FLD_MUTEX_EOF_TIMING, (reg32), (val))
#define DISP_MUTEX9_CTL_SET_MUTEX_EOF(reg32, val)                                       \
                REG_FLD_SET(DISP_MUTEX9_CTL_FLD_MUTEX_EOF, (reg32), (val))
#define DISP_MUTEX9_CTL_SET_MUTEX_SOF_WAIT(reg32, val)                                  \
                REG_FLD_SET(DISP_MUTEX9_CTL_FLD_MUTEX_SOF_WAIT, (reg32), (val))
#define DISP_MUTEX9_CTL_SET_MUTEX_SOF_TIMING(reg32, val)                                \
                REG_FLD_SET(DISP_MUTEX9_CTL_FLD_MUTEX_SOF_TIMING, (reg32), (val))
#define DISP_MUTEX9_CTL_SET_MUTEX_SOF(reg32, val)                                       \
                REG_FLD_SET(DISP_MUTEX9_CTL_FLD_MUTEX_SOF, (reg32), (val))

#define DISP_MUTEX9_MOD0_SET_MUTEX_MOD0(reg32, val)                                     \
                REG_FLD_SET(DISP_MUTEX9_MOD0_FLD_MUTEX_MOD0, (reg32), (val))

#define DEBUG_OUT_SEL_SET_DEBUG_OUT_SEL(reg32, val)                                     \
                REG_FLD_SET(DEBUG_OUT_SEL_FLD_DEBUG_OUT_SEL, (reg32), (val))

#define INTEN_VAL_MUTEX_INTEN(val)              REG_FLD_VAL(INTEN_FLD_MUTEX_INTEN, (val))

#define INTSTA_VAL_MUTEX_INTSTA(val)            REG_FLD_VAL(INTSTA_FLD_MUTEX_INTSTA, (val))

#define CFG_VAL_MUTEX_CFG(val)                  REG_FLD_VAL(CFG_FLD_MUTEX_CFG, (val))

#define DISP_MUTEX0_EN_VAL_MUTEX_EN(val)        REG_FLD_VAL(DISP_MUTEX0_EN_FLD_MUTEX_EN, (val))

#define DISP_MUTEX0_RST_VAL_MUTEX_RST(val)      REG_FLD_VAL(DISP_MUTEX0_RST_FLD_MUTEX_RST, (val))

#define DISP_MUTEX0_CTL_VAL_MUTEX_EOF_WAIT(val)                                         \
                REG_FLD_VAL(DISP_MUTEX0_CTL_FLD_MUTEX_EOF_WAIT, (val))
#define DISP_MUTEX0_CTL_VAL_MUTEX_EOF_TIMING(val)                                       \
                REG_FLD_VAL(DISP_MUTEX0_CTL_FLD_MUTEX_EOF_TIMING, (val))
#define DISP_MUTEX0_CTL_VAL_MUTEX_EOF(val)                                              \
                REG_FLD_VAL(DISP_MUTEX0_CTL_FLD_MUTEX_EOF, (val))
#define DISP_MUTEX0_CTL_VAL_MUTEX_SOF_WAIT(val)                                         \
                REG_FLD_VAL(DISP_MUTEX0_CTL_FLD_MUTEX_SOF_WAIT, (val))
#define DISP_MUTEX0_CTL_VAL_MUTEX_SOF_TIMING(val)                                       \
                REG_FLD_VAL(DISP_MUTEX0_CTL_FLD_MUTEX_SOF_TIMING, (val))
#define DISP_MUTEX0_CTL_VAL_MUTEX_SOF(val)                                              \
                REG_FLD_VAL(DISP_MUTEX0_CTL_FLD_MUTEX_SOF, (val))

#define DISP_MUTEX0_MOD0_VAL_MUTEX_MOD0(val)    REG_FLD_VAL(DISP_MUTEX0_MOD0_FLD_MUTEX_MOD0, (val))

#define DISP_MUTEX1_EN_VAL_MUTEX_EN(val)        REG_FLD_VAL(DISP_MUTEX1_EN_FLD_MUTEX_EN, (val))

#define DISP_MUTEX1_RST_VAL_MUTEX_RST(val)      REG_FLD_VAL(DISP_MUTEX1_RST_FLD_MUTEX_RST, (val))

#define DISP_MUTEX1_CTL_VAL_MUTEX_EOF_WAIT(val)                                         \
                REG_FLD_VAL(DISP_MUTEX1_CTL_FLD_MUTEX_EOF_WAIT, (val))
#define DISP_MUTEX1_CTL_VAL_MUTEX_EOF_TIMING(val)                                       \
                REG_FLD_VAL(DISP_MUTEX1_CTL_FLD_MUTEX_EOF_TIMING, (val))
#define DISP_MUTEX1_CTL_VAL_MUTEX_EOF(val)                                              \
                REG_FLD_VAL(DISP_MUTEX1_CTL_FLD_MUTEX_EOF, (val))
#define DISP_MUTEX1_CTL_VAL_MUTEX_SOF_WAIT(val)                                         \
                REG_FLD_VAL(DISP_MUTEX1_CTL_FLD_MUTEX_SOF_WAIT, (val))
#define DISP_MUTEX1_CTL_VAL_MUTEX_SOF_TIMING(val)                                       \
                REG_FLD_VAL(DISP_MUTEX1_CTL_FLD_MUTEX_SOF_TIMING, (val))
#define DISP_MUTEX1_CTL_VAL_MUTEX_SOF(val)                                              \
                REG_FLD_VAL(DISP_MUTEX1_CTL_FLD_MUTEX_SOF, (val))

#define DISP_MUTEX1_MOD0_VAL_MUTEX_MOD0(val)    REG_FLD_VAL(DISP_MUTEX1_MOD0_FLD_MUTEX_MOD0, (val))

#define DISP_MUTEX2_EN_VAL_MUTEX_EN(val)        REG_FLD_VAL(DISP_MUTEX2_EN_FLD_MUTEX_EN, (val))

#define DISP_MUTEX2_RST_VAL_MUTEX_RST(val)      REG_FLD_VAL(DISP_MUTEX2_RST_FLD_MUTEX_RST, (val))

#define DISP_MUTEX2_CTL_VAL_MUTEX_EOF_WAIT(val)                                         \
                REG_FLD_VAL(DISP_MUTEX2_CTL_FLD_MUTEX_EOF_WAIT, (val))
#define DISP_MUTEX2_CTL_VAL_MUTEX_EOF_TIMING(val)                                       \
                REG_FLD_VAL(DISP_MUTEX2_CTL_FLD_MUTEX_EOF_TIMING, (val))
#define DISP_MUTEX2_CTL_VAL_MUTEX_EOF(val)                                              \
                REG_FLD_VAL(DISP_MUTEX2_CTL_FLD_MUTEX_EOF, (val))
#define DISP_MUTEX2_CTL_VAL_MUTEX_SOF_WAIT(val)                                         \
                REG_FLD_VAL(DISP_MUTEX2_CTL_FLD_MUTEX_SOF_WAIT, (val))
#define DISP_MUTEX2_CTL_VAL_MUTEX_SOF_TIMING(val)                                       \
                REG_FLD_VAL(DISP_MUTEX2_CTL_FLD_MUTEX_SOF_TIMING, (val))
#define DISP_MUTEX2_CTL_VAL_MUTEX_SOF(val)                                              \
                REG_FLD_VAL(DISP_MUTEX2_CTL_FLD_MUTEX_SOF, (val))

#define DISP_MUTEX2_MOD0_VAL_MUTEX_MOD0(val)    REG_FLD_VAL(DISP_MUTEX2_MOD0_FLD_MUTEX_MOD0, (val))

#define DISP_MUTEX3_EN_VAL_MUTEX_EN(val)        REG_FLD_VAL(DISP_MUTEX3_EN_FLD_MUTEX_EN, (val))

#define DISP_MUTEX3_RST_VAL_MUTEX_RST(val)      REG_FLD_VAL(DISP_MUTEX3_RST_FLD_MUTEX_RST, (val))

#define DISP_MUTEX3_CTL_VAL_MUTEX_EOF_WAIT(val)                                         \
                REG_FLD_VAL(DISP_MUTEX3_CTL_FLD_MUTEX_EOF_WAIT, (val))
#define DISP_MUTEX3_CTL_VAL_MUTEX_EOF_TIMING(val)                                       \
                REG_FLD_VAL(DISP_MUTEX3_CTL_FLD_MUTEX_EOF_TIMING, (val))
#define DISP_MUTEX3_CTL_VAL_MUTEX_EOF(val)                                              \
                REG_FLD_VAL(DISP_MUTEX3_CTL_FLD_MUTEX_EOF, (val))
#define DISP_MUTEX3_CTL_VAL_MUTEX_SOF_WAIT(val)                                         \
                REG_FLD_VAL(DISP_MUTEX3_CTL_FLD_MUTEX_SOF_WAIT, (val))
#define DISP_MUTEX3_CTL_VAL_MUTEX_SOF_TIMING(val)                                       \
                REG_FLD_VAL(DISP_MUTEX3_CTL_FLD_MUTEX_SOF_TIMING, (val))
#define DISP_MUTEX3_CTL_VAL_MUTEX_SOF(val)                                              \
                REG_FLD_VAL(DISP_MUTEX3_CTL_FLD_MUTEX_SOF, (val))

#define DISP_MUTEX3_MOD0_VAL_MUTEX_MOD0(val)    REG_FLD_VAL(DISP_MUTEX3_MOD0_FLD_MUTEX_MOD0, (val))

#define DISP_MUTEX4_EN_VAL_MUTEX_EN(val)        REG_FLD_VAL(DISP_MUTEX4_EN_FLD_MUTEX_EN, (val))

#define DISP_MUTEX4_RST_VAL_MUTEX_RST(val)      REG_FLD_VAL(DISP_MUTEX4_RST_FLD_MUTEX_RST, (val))

#define DISP_MUTEX4_CTL_VAL_MUTEX_EOF_WAIT(val)                                         \
                REG_FLD_VAL(DISP_MUTEX4_CTL_FLD_MUTEX_EOF_WAIT, (val))
#define DISP_MUTEX4_CTL_VAL_MUTEX_EOF_TIMING(val)                                       \
                REG_FLD_VAL(DISP_MUTEX4_CTL_FLD_MUTEX_EOF_TIMING, (val))
#define DISP_MUTEX4_CTL_VAL_MUTEX_EOF(val)                                              \
                REG_FLD_VAL(DISP_MUTEX4_CTL_FLD_MUTEX_EOF, (val))
#define DISP_MUTEX4_CTL_VAL_MUTEX_SOF_WAIT(val)                                         \
                REG_FLD_VAL(DISP_MUTEX4_CTL_FLD_MUTEX_SOF_WAIT, (val))
#define DISP_MUTEX4_CTL_VAL_MUTEX_SOF_TIMING(val)                                       \
                REG_FLD_VAL(DISP_MUTEX4_CTL_FLD_MUTEX_SOF_TIMING, (val))
#define DISP_MUTEX4_CTL_VAL_MUTEX_SOF(val)                                              \
                REG_FLD_VAL(DISP_MUTEX4_CTL_FLD_MUTEX_SOF, (val))

#define DISP_MUTEX4_MOD0_VAL_MUTEX_MOD0(val)    REG_FLD_VAL(DISP_MUTEX4_MOD0_FLD_MUTEX_MOD0, (val))

#define DISP_MUTEX5_EN_VAL_MUTEX_EN(val)        REG_FLD_VAL(DISP_MUTEX5_EN_FLD_MUTEX_EN, (val))

#define DISP_MUTEX5_RST_VAL_MUTEX_RST(val)      REG_FLD_VAL(DISP_MUTEX5_RST_FLD_MUTEX_RST, (val))

#define DISP_MUTEX5_CTL_VAL_MUTEX_EOF_WAIT(val)                                         \
                REG_FLD_VAL(DISP_MUTEX5_CTL_FLD_MUTEX_EOF_WAIT, (val))
#define DISP_MUTEX5_CTL_VAL_MUTEX_EOF_TIMING(val)                                       \
                REG_FLD_VAL(DISP_MUTEX5_CTL_FLD_MUTEX_EOF_TIMING, (val))
#define DISP_MUTEX5_CTL_VAL_MUTEX_EOF(val)                                              \
                REG_FLD_VAL(DISP_MUTEX5_CTL_FLD_MUTEX_EOF, (val))
#define DISP_MUTEX5_CTL_VAL_MUTEX_SOF_WAIT(val)                                         \
                REG_FLD_VAL(DISP_MUTEX5_CTL_FLD_MUTEX_SOF_WAIT, (val))
#define DISP_MUTEX5_CTL_VAL_MUTEX_SOF_TIMING(val)                                       \
                REG_FLD_VAL(DISP_MUTEX5_CTL_FLD_MUTEX_SOF_TIMING, (val))
#define DISP_MUTEX5_CTL_VAL_MUTEX_SOF(val)                                              \
                REG_FLD_VAL(DISP_MUTEX5_CTL_FLD_MUTEX_SOF, (val))

#define DISP_MUTEX5_MOD0_VAL_MUTEX_MOD0(val)    REG_FLD_VAL(DISP_MUTEX5_MOD0_FLD_MUTEX_MOD0, (val))

#define DISP_MUTEX6_EN_VAL_MUTEX_EN(val)        REG_FLD_VAL(DISP_MUTEX6_EN_FLD_MUTEX_EN, (val))

#define DISP_MUTEX6_RST_VAL_MUTEX_RST(val)      REG_FLD_VAL(DISP_MUTEX6_RST_FLD_MUTEX_RST, (val))

#define DISP_MUTEX6_CTL_VAL_MUTEX_EOF_WAIT(val)                                         \
                REG_FLD_VAL(DISP_MUTEX6_CTL_FLD_MUTEX_EOF_WAIT, (val))
#define DISP_MUTEX6_CTL_VAL_MUTEX_EOF_TIMING(val)                                       \
                REG_FLD_VAL(DISP_MUTEX6_CTL_FLD_MUTEX_EOF_TIMING, (val))
#define DISP_MUTEX6_CTL_VAL_MUTEX_EOF(val)                                              \
                REG_FLD_VAL(DISP_MUTEX6_CTL_FLD_MUTEX_EOF, (val))
#define DISP_MUTEX6_CTL_VAL_MUTEX_SOF_WAIT(val)                                         \
                REG_FLD_VAL(DISP_MUTEX6_CTL_FLD_MUTEX_SOF_WAIT, (val))
#define DISP_MUTEX6_CTL_VAL_MUTEX_SOF_TIMING(val)                                       \
                REG_FLD_VAL(DISP_MUTEX6_CTL_FLD_MUTEX_SOF_TIMING, (val))
#define DISP_MUTEX6_CTL_VAL_MUTEX_SOF(val)                                              \
                REG_FLD_VAL(DISP_MUTEX6_CTL_FLD_MUTEX_SOF, (val))

#define DISP_MUTEX6_MOD0_VAL_MUTEX_MOD0(val)    REG_FLD_VAL(DISP_MUTEX6_MOD0_FLD_MUTEX_MOD0, (val))

#define DISP_MUTEX7_EN_VAL_MUTEX_EN(val)        REG_FLD_VAL(DISP_MUTEX7_EN_FLD_MUTEX_EN, (val))

#define DISP_MUTEX7_RST_VAL_MUTEX_RST(val)      REG_FLD_VAL(DISP_MUTEX7_RST_FLD_MUTEX_RST, (val))

#define DISP_MUTEX7_CTL_VAL_MUTEX_EOF_WAIT(val)                                         \
                REG_FLD_VAL(DISP_MUTEX7_CTL_FLD_MUTEX_EOF_WAIT, (val))
#define DISP_MUTEX7_CTL_VAL_MUTEX_EOF_TIMING(val)                                       \
                REG_FLD_VAL(DISP_MUTEX7_CTL_FLD_MUTEX_EOF_TIMING, (val))
#define DISP_MUTEX7_CTL_VAL_MUTEX_EOF(val)                                              \
                REG_FLD_VAL(DISP_MUTEX7_CTL_FLD_MUTEX_EOF, (val))
#define DISP_MUTEX7_CTL_VAL_MUTEX_SOF_WAIT(val)                                         \
                REG_FLD_VAL(DISP_MUTEX7_CTL_FLD_MUTEX_SOF_WAIT, (val))
#define DISP_MUTEX7_CTL_VAL_MUTEX_SOF_TIMING(val)                                       \
                REG_FLD_VAL(DISP_MUTEX7_CTL_FLD_MUTEX_SOF_TIMING, (val))
#define DISP_MUTEX7_CTL_VAL_MUTEX_SOF(val)                                              \
                REG_FLD_VAL(DISP_MUTEX7_CTL_FLD_MUTEX_SOF, (val))

#define DISP_MUTEX7_MOD0_VAL_MUTEX_MOD0(val)    REG_FLD_VAL(DISP_MUTEX7_MOD0_FLD_MUTEX_MOD0, (val))

#define DISP_MUTEX8_EN_VAL_MUTEX_EN(val)        REG_FLD_VAL(DISP_MUTEX8_EN_FLD_MUTEX_EN, (val))

#define DISP_MUTEX8_RST_VAL_MUTEX_RST(val)      REG_FLD_VAL(DISP_MUTEX8_RST_FLD_MUTEX_RST, (val))

#define DISP_MUTEX8_CTL_VAL_MUTEX_EOF_WAIT(val)                                         \
                REG_FLD_VAL(DISP_MUTEX8_CTL_FLD_MUTEX_EOF_WAIT, (val))
#define DISP_MUTEX8_CTL_VAL_MUTEX_EOF_TIMING(val)                                       \
                REG_FLD_VAL(DISP_MUTEX8_CTL_FLD_MUTEX_EOF_TIMING, (val))
#define DISP_MUTEX8_CTL_VAL_MUTEX_EOF(val)                                              \
                REG_FLD_VAL(DISP_MUTEX8_CTL_FLD_MUTEX_EOF, (val))
#define DISP_MUTEX8_CTL_VAL_MUTEX_SOF_WAIT(val)                                         \
                REG_FLD_VAL(DISP_MUTEX8_CTL_FLD_MUTEX_SOF_WAIT, (val))
#define DISP_MUTEX8_CTL_VAL_MUTEX_SOF_TIMING(val)                                       \
                REG_FLD_VAL(DISP_MUTEX8_CTL_FLD_MUTEX_SOF_TIMING, (val))
#define DISP_MUTEX8_CTL_VAL_MUTEX_SOF(val)                                              \
                REG_FLD_VAL(DISP_MUTEX8_CTL_FLD_MUTEX_SOF, (val))

#define DISP_MUTEX8_MOD0_VAL_MUTEX_MOD0(val)    REG_FLD_VAL(DISP_MUTEX8_MOD0_FLD_MUTEX_MOD0, (val))

#define DISP_MUTEX9_EN_VAL_MUTEX_EN(val)        REG_FLD_VAL(DISP_MUTEX9_EN_FLD_MUTEX_EN, (val))

#define DISP_MUTEX9_RST_VAL_MUTEX_RST(val)      REG_FLD_VAL(DISP_MUTEX9_RST_FLD_MUTEX_RST, (val))

#define DISP_MUTEX9_CTL_VAL_MUTEX_EOF_WAIT(val)                                         \
                REG_FLD_VAL(DISP_MUTEX9_CTL_FLD_MUTEX_EOF_WAIT, (val))
#define DISP_MUTEX9_CTL_VAL_MUTEX_EOF_TIMING(val)                                       \
                REG_FLD_VAL(DISP_MUTEX9_CTL_FLD_MUTEX_EOF_TIMING, (val))
#define DISP_MUTEX9_CTL_VAL_MUTEX_EOF(val)                                              \
                REG_FLD_VAL(DISP_MUTEX9_CTL_FLD_MUTEX_EOF, (val))
#define DISP_MUTEX9_CTL_VAL_MUTEX_SOF_WAIT(val)                                         \
                REG_FLD_VAL(DISP_MUTEX9_CTL_FLD_MUTEX_SOF_WAIT, (val))
#define DISP_MUTEX9_CTL_VAL_MUTEX_SOF_TIMING(val)                                       \
                REG_FLD_VAL(DISP_MUTEX9_CTL_FLD_MUTEX_SOF_TIMING, (val))
#define DISP_MUTEX9_CTL_VAL_MUTEX_SOF(val)                                              \
                REG_FLD_VAL(DISP_MUTEX9_CTL_FLD_MUTEX_SOF, (val))

#define DISP_MUTEX9_MOD0_VAL_MUTEX_MOD0(val)    REG_FLD_VAL(DISP_MUTEX9_MOD0_FLD_MUTEX_MOD0, (val))

#define DEBUG_OUT_SEL_VAL_DEBUG_OUT_SEL(val)    REG_FLD_VAL(DEBUG_OUT_SEL_FLD_DEBUG_OUT_SEL, (val))

#ifdef __cplusplus
}
#endif
