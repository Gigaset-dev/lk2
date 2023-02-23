/*
 * Copyright (c) 2020 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#define LOG_TAG "LCM"

#include <debug.h>
#include <mt_i2c.h>
#include <platform.h>

#include "lcm_drv.h"
#include "lcm_util.h"
#ifdef MTK_RT4831A_SUPPORT
#include "rt4831a_drv.h"
#endif

#ifdef MTK_ROUND_CORNER_SUPPORT
#include "data_hw_roundedpattern.h"
#endif

#define LCM_LOGI(string, args...)  dprintf(CRITICAL, "[LK/"LOG_TAG"]"string, ##args)
#define LCM_LOGD(string, args...)  dprintf(INFO, "[LK/"LOG_TAG"]"string, ##args)

#ifdef MTK_RUNTIME_SWITCH_FPS_SUPPORT
#define LCM_ID_NT36672E_JDI (0x0) // TO DO
#else
#define LCM_ID_NT36672E_JDI (0x41) // TO DO
#endif

static LCM_UTIL_FUNCS lcm_util;

#define SET_RESET_PIN(v, fdt)    (lcm_util.set_reset_pin(v, fdt))
#define MDELAY(n)       (lcm_util.mdelay(n))
#define UDELAY(n)       (lcm_util.udelay(n))


/* --------------------------------------------------------------------------- */
/* Local Functions */
/* --------------------------------------------------------------------------- */

#define dsi_set_cmdq_V2(cmd, count, ppara, force_update) \
        lcm_util.dsi_set_cmdq_V2(cmd, count, ppara, force_update)
#define dsi_set_cmdq(pdata, queue_size, force_update) \
        lcm_util.dsi_set_cmdq(pdata, queue_size, force_update)
#define wrtie_cmd(cmd) lcm_util.dsi_write_cmd(cmd)
#define write_regs(addr, pdata, byte_nums) \
        lcm_util.dsi_write_regs(addr, pdata, byte_nums)
#define read_reg(cmd) \
        lcm_util.dsi_dcs_read_lcm_reg(cmd)
#define read_reg_v2(cmd, buffer, buffer_size) \
        lcm_util.dsi_dcs_read_lcm_reg_v2(cmd, buffer, buffer_size)

/* --------------------------------------------------------------------------- */
/* Local Constants */
/* --------------------------------------------------------------------------- */
#define LCM_DSI_CMD_MODE                                    0
#define FRAME_WIDTH                                     (1080)
#define FRAME_HEIGHT                                    (2400)

#define REGFLAG_DELAY       0xFFFC
#define REGFLAG_UDELAY  0xFFFB
#define REGFLAG_END_OF_TABLE    0xFFFD
#define REGFLAG_RESET_LOW   0xFFFE
#define REGFLAG_RESET_HIGH  0xFFFF

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

struct LCM_setting_table {
    unsigned int cmd;
    unsigned char count;
    unsigned char para_list[64];
};

#if 0
static struct LCM_setting_table lcm_suspend_setting[] = {
    {0x28, 0, {} },
    {0x10, 0, {} },
    {REGFLAG_DELAY, 120, {} },
    {0x4F, 1, {0x01} },
    {REGFLAG_DELAY, 120, {} }
};
#endif

static struct LCM_setting_table init_setting[] = {

{0XFF, 1, {0X10} },
//REGR 0XFE, 0x10
{0XFB, 1, {0X01} },
{0XB0, 1, {0X00} },
//DSC off
{0XC0, 1, {0X00} },//JDI VESA
{0XC2, 2, {0X1B, 0XA0} },

{0XFF, 1, {0X20} },
//REGR 0XFE, 0x20
{0XFB, 1, {0X01} },
{0X01, 1, {0X66} },
{0X06, 1, {0X40} },
{0X07, 1, {0X38} },
{0X18, 1, {0X66} },
{0X1B, 1, {0X01} },
{0X2F, 1, {0X83} },
{0X5C, 1, {0X90} },
{0X5E, 1, {0XAA} },
{0X69, 1, {0X91} },
{0X95, 1, {0XD1} },
{0X96, 1, {0XD1} },
{0XF2, 1, {0X65} },
{0XF3, 1, {0X64} },
{0XF4, 1, {0X65} },
{0XF5, 1, {0X64} },
{0XF6, 1, {0X65} },
{0XF7, 1, {0X64} },
{0XF8, 1, {0X65} },
{0XF9, 1, {0X64} },

{0x89, 1, {0x15} },//VCOM
{0x8A, 1, {0x15} },//VCOM
{0x8D, 1, {0x15} },//VCOM
{0x8E, 1, {0x15} },//VCOM
{0x8F, 1, {0x15} },//VCOM
{0x91, 1, {0x15} },//VCOM

{0XFF, 1, {0X24} },
//REGR 0XFE, 0x24
{0XFB, 1, {0X01} },
{0X01, 1, {0X0F} },
{0X03, 1, {0X0C} },
{0X05, 1, {0X1D} },
{0X08, 1, {0X2F} },
{0X09, 1, {0X2E} },
{0X0A, 1, {0X2D} },
{0X0B, 1, {0X2C} },
{0X11, 1, {0X17} },
{0X12, 1, {0X13} },
{0X13, 1, {0X15} },
{0X15, 1, {0X14} },
{0X16, 1, {0X16} },
{0X17, 1, {0X18} },
{0X1B, 1, {0X01} },
{0X1D, 1, {0X1D} },
{0X20, 1, {0X2F} },
{0X21, 1, {0X2E} },
{0X22, 1, {0X2D} },
{0X23, 1, {0X2C} },
{0X29, 1, {0X17} },
{0X2A, 1, {0X13} },
{0X2B, 1, {0X15} },
{0X2F, 1, {0X14} },
{0X30, 1, {0X16} },
{0X31, 1, {0X18} },
{0X32, 1, {0X04} },
{0X34, 1, {0X10} },
{0X35, 1, {0X1F} },
{0X36, 1, {0X1F} },
{0X37, 1, {0X20} },
{0X4D, 1, {0X1B} },
{0X4E, 1, {0X4B} },
{0X4F, 1, {0X4B} },
{0X53, 1, {0X4B} },
{0X71, 1, {0X30} },
{0X79, 1, {0X11} },
{0X7A, 1, {0X82} },
{0X7B, 1, {0X96} },
{0X7D, 1, {0X04} },
{0X80, 1, {0X04} },
{0X81, 1, {0X04} },
{0X82, 1, {0X13} },
{0X84, 1, {0X31} },
{0X85, 1, {0X00} },
{0X86, 1, {0X00} },
{0X87, 1, {0X00} },
{0X90, 1, {0X13} },
{0X92, 1, {0X31} },
{0X93, 1, {0X00} },
{0X94, 1, {0X00} },
{0X95, 1, {0X00} },
{0X9C, 1, {0XF4} },
{0X9D, 1, {0X01} },
{0XA0, 1, {0X16} },
{0XA2, 1, {0X16} },
{0XA3, 1, {0X02} },
{0XA4, 1, {0X04} },
{0XA5, 1, {0X04} },
{0XC6, 1, {0XC0} },
{0XC9, 1, {0X00} },
{0XD9, 1, {0X80} },
{0XE9, 1, {0X02} },

{0XFF, 1, {0X25} },
//REGR 0XFE, 0x25
{0XFB, 1, {0X01} },
{0X0F, 1, {0X1B} },
{0X18, 1, {0X21} },
{0X19, 1, {0XE4} },
{0X21, 1, {0X40} },
{0X66, 1, {0XD8} },
{0X68, 1, {0X50} },
{0X69, 1, {0X10} },
{0X6B, 1, {0X00} },
{0X6D, 1, {0X0D} },
{0X6E, 1, {0X48} },
{0X72, 1, {0X41} },
{0X73, 1, {0X4A} },
{0X74, 1, {0XD0} },
{0X77, 1, {0X62} },
{0X79, 1, {0X77} },
{0X7D, 1, {0X40} },
{0X7E, 1, {0X1D} },
{0X7F, 1, {0X00} },
{0X80, 1, {0X04} },
{0X84, 1, {0X0D} },
{0XCF, 1, {0X80} },
{0XD6, 1, {0X80} },
{0XD7, 1, {0X80} },
{0XEF, 1, {0X20} },
{0XF0, 1, {0X84} },

{0XFF, 1, {0X26} },
//REGR 0XFE, 0x26
{0XFB, 1, {0X01} },
{0X15, 1, {0X04} },
{0X81, 1, {0X16} },
{0X83, 1, {0X02} },
{0X84, 1, {0X03} },
{0X85, 1, {0X01} },
{0X86, 1, {0X03} },
{0X87, 1, {0X01} },
{0X88, 1, {0X00} },
{0X8A, 1, {0X1A} },
{0X8B, 1, {0X11} },
{0X8C, 1, {0X24} },
{0X8E, 1, {0X42} },
{0X8F, 1, {0X11} },
{0X90, 1, {0X11} },
{0X91, 1, {0X11} },
{0X9A, 1, {0X80} },
{0X9B, 1, {0X04} },
{0X9C, 1, {0X00} },
{0X9D, 1, {0X00} },
{0X9E, 1, {0X00} },

{0XFF, 1, {0X27} },
//REGR 0XFE, 0x27
{0XFB, 1, {0X01} },
{0X01, 1, {0X60} },
{0X20, 1, {0X81} },
{0X21, 1, {0XE7} },
{0X25, 1, {0X82} },
{0X26, 1, {0X1F} },
{0X6E, 1, {0X00} },
{0X6F, 1, {0X00} },
{0X70, 1, {0X00} },
{0X71, 1, {0X00} },
{0X72, 1, {0X00} },
{0X75, 1, {0X00} },
{0X76, 1, {0X00} },
{0X77, 1, {0X00} },
{0X7D, 1, {0X09} },
{0X7E, 1, {0X5F} },
{0X80, 1, {0X23} },
{0X82, 1, {0X09} },
{0X83, 1, {0X5F} },
{0X88, 1, {0X01} },
{0X89, 1, {0X10} },
{0XA5, 1, {0X10} },
{0XA6, 1, {0X23} },
{0XA7, 1, {0X01} },
{0XB6, 1, {0X40} },
{0XE3, 1, {0X02} },
{0XE4, 1, {0XDA} },
{0XE5, 1, {0X01} },
{0XE6, 1, {0X6D} },
{0XE9, 1, {0X03} },
{0XEA, 1, {0X2F} },
{0XEB, 1, {0X01} },
{0XEC, 1, {0X98} },

{0XFF, 1, {0X2A} },
//REGR 0XFE, 0x2A
{0XFB, 1, {0X01} },
{0X00, 1, {0X91} },
{0X03, 1, {0X20} },
{0X07, 1, {0X52} },
{0X0A, 1, {0X70} },
{0X0D, 1, {0X40} },
{0X0E, 1, {0X02} },
{0X11, 1, {0XF0} },
{0X15, 1, {0X0E} },
{0X16, 1, {0XB6} },
{0X19, 1, {0X0E} },
{0X1A, 1, {0X8A} },
{0X1B, 1, {0X14} },
{0X1D, 1, {0X36} },
{0X1E, 1, {0X4F} },
{0X1F, 1, {0X51} },
{0X20, 1, {0X4F} },
{0X28, 1, {0XEC} },
{0X29, 1, {0X0C} },
{0X2A, 1, {0X05} },
{0X2D, 1, {0X06} },
{0X2F, 1, {0X02} },
{0X30, 1, {0X4A} },
{0X33, 1, {0X0E} },
{0X34, 1, {0XEE} },
{0X35, 1, {0X30} },
{0X36, 1, {0X06} },
{0X37, 1, {0XE9} },
{0X38, 1, {0X34} },
{0X39, 1, {0X02} },
{0X3A, 1, {0X4A} },
{0X45, 1, {0X0E} },
{0X46, 1, {0X40} },
{0X47, 1, {0X03} },
{0X4A, 1, {0XA0} },
{0X4E, 1, {0X0E} },
{0X4F, 1, {0XB6} },
{0X52, 1, {0X0E} },
{0X53, 1, {0X8A} },
{0X54, 1, {0X14} },
{0X56, 1, {0X36} },
{0X57, 1, {0X76} },
{0X58, 1, {0X76} },
{0X59, 1, {0X76} },
{0X60, 1, {0X80} },
{0X61, 1, {0XA0} },
{0X62, 1, {0X04} },
{0X63, 1, {0X33} },
{0X65, 1, {0X04} },
{0X66, 1, {0X01} },
{0X67, 1, {0X05} },
{0X68, 1, {0X40} },
{0X6A, 1, {0X4C} },
{0X6B, 1, {0XA2} },
{0X6C, 1, {0X21} },
{0X6D, 1, {0XBB} },
{0X6E, 1, {0X9F} },
{0X6F, 1, {0X23} },
{0X70, 1, {0XB9} },
{0X71, 1, {0X05} },
{0X7A, 1, {0X07} },
{0X7B, 1, {0X40} },
{0X7D, 1, {0X01} },
{0X7F, 1, {0X2C} },
{0X83, 1, {0X0E} },
{0X84, 1, {0XB6} },
{0X87, 1, {0X0E} },
{0X88, 1, {0X8A} },
{0X89, 1, {0X14} },
{0X8B, 1, {0X36} },
{0X8C, 1, {0X3A} },
{0X8D, 1, {0X3A} },
{0X8E, 1, {0X3A} },
{0X95, 1, {0X80} },
{0X96, 1, {0XFD} },
{0X97, 1, {0X14} },
{0X98, 1, {0X17} },
{0X99, 1, {0X01} },
{0X9A, 1, {0X08} },
{0X9B, 1, {0X02} },
{0X9C, 1, {0X4C} },
{0X9D, 1, {0XAF} },
{0X9F, 1, {0X6B} },
{0XA0, 1, {0XFF} },
{0XA2, 1, {0X40} },
{0XA3, 1, {0X6F} },
{0XA4, 1, {0XF9} },
{0XA5, 1, {0X45} },
{0XA6, 1, {0X6A} },
{0XA7, 1, {0X4C} },

{0XFF, 1, {0X2C} },
//REGR 0XFE, 0x2C
{0XFB, 1, {0X01} },
{0X00, 1, {0X02} },
{0X01, 1, {0X02} },
{0X02, 1, {0X02} },
{0X03, 1, {0X16} },
{0X04, 1, {0X16} },
{0X05, 1, {0X16} },
{0X0D, 1, {0X1F} },
{0X0E, 1, {0X1F} },
{0X16, 1, {0X1B} },
{0X17, 1, {0X4B} },
{0X18, 1, {0X4B} },
{0X19, 1, {0X4B} },
{0X2A, 1, {0X03} },
{0X4D, 1, {0X16} },
{0X4E, 1, {0X02} },
{0X4F, 1, {0X27} },
{0X53, 1, {0X02} },
{0X54, 1, {0X02} },
{0X55, 1, {0X02} },
{0X56, 1, {0X0E} },
{0X58, 1, {0X0E} },
{0X59, 1, {0X0E} },
{0X61, 1, {0X1F} },
{0X62, 1, {0X1F} },
{0X6A, 1, {0X14} },
{0X6B, 1, {0X34} },
{0X6C, 1, {0X34} },
{0X6D, 1, {0X34} },
{0X7E, 1, {0X03} },
{0X9D, 1, {0X0E} },
{0X9E, 1, {0X02} },
{0X9F, 1, {0X03} },


{0XFF, 1, {0X20} },
{0XFB, 1, {0X01} },
{0XB0, 16, {0x00, 0x00, 0x00, 0x15, 0x00, 0x3F, 0x00, 0x5F,
                0x00, 0x7E, 0x00, 0x97, 0x00, 0xAF, 0x00, 0xC3} },
{0XB1, 16, {0x00, 0xD7, 0x01, 0x0A, 0x01, 0x32, 0x01, 0x6F,
                0x01, 0x9E, 0x01, 0xE5, 0x02, 0x1D, 0x02, 0x1E} },
{0XB2, 16, {0x02, 0x56, 0x02, 0x94, 0x02, 0xBC, 0x02, 0xF1,
                0x03, 0x13, 0x03, 0x41, 0x03, 0x4F, 0x03, 0x5F} },
{0XB3, 14, {0x03, 0x71, 0x03, 0x84, 0x03, 0x99, 0x03, 0xB0, 0x03, 0xCA, 0x03, 0xD7, 0x00, 0x00} },
{0XB4, 16, {0x00, 0x00, 0x00, 0x17, 0x00, 0x46, 0x00, 0x69,
                0x00, 0x8C, 0x00, 0xA5, 0x00, 0xBE, 0x00, 0xD1} },
{0XB5, 16, {0x00, 0xE4, 0x01, 0x18, 0x01, 0x40, 0x01, 0x7C,
                0x01, 0xAA, 0x01, 0xF0, 0x02, 0x27, 0x02, 0x28} },
{0XB6, 16, {0x02, 0x5E, 0x02, 0x9B, 0x02, 0xC3, 0x02, 0xF6,
                0x03, 0x18, 0x03, 0x45, 0x03, 0x54, 0x03, 0x63} },
{0XB7, 14, {0x03, 0x75, 0x03, 0x87, 0x03, 0x9C, 0x03, 0xB2, 0x03, 0xCA, 0x03, 0xD7, 0x00, 0x00} },
{0XB8, 16, {0x00, 0x00, 0x00, 0x18, 0x00, 0x49, 0x00, 0x6B,
                0x00, 0x8E, 0x00, 0xA8, 0x00, 0xC1, 0x00, 0xD3} },
{0XB9, 16, {0x00, 0xE5, 0x01, 0x18, 0x01, 0x3F, 0x01, 0x7B,
                0x01, 0xA8, 0x01, 0xEC, 0x02, 0x24, 0x02, 0x26} },
{0XBA, 16, {0x02, 0x5A, 0x02, 0x97, 0x02, 0xC0, 0x02, 0xF4,
                0x03, 0x15, 0x03, 0x43, 0x03, 0x51, 0x03, 0x61} },
{0XBB, 14, {0x03, 0x72, 0x03, 0x85, 0x03, 0x9A, 0x03, 0xB1, 0x03, 0xCA, 0x03, 0xD7, 0x00, 0x00} },

{0xC6, 1, {0x00} },
{0xC7, 1, {0x00} },
{0xC8, 1, {0x00} },
{0xC9, 1, {0x00} },
{0xCA, 1, {0x00} },

{0xCB, 1, {0x00} },
{0xCC, 1, {0x00} },
{0xCD, 1, {0x00} },
{0xCE, 1, {0x00} },
{0xCF, 1, {0x00} },

{0xD0, 1, {0x00} },
{0xD1, 1, {0x00} },
{0xD2, 1, {0x00} },
{0xD3, 1, {0x00} },
{0xD4, 1, {0x00} },

{0xD5, 1, {0x00} },
{0xD6, 1, {0x00} },
{0xD7, 1, {0x00} },
{0xD8, 1, {0x00} },
{0xD9, 1, {0x00} },

{0xDA, 1, {0x00} },
{0xDB, 1, {0x00} },
{0xDC, 1, {0x00} },
{0xDD, 1, {0x00} },
{0xDE, 1, {0x00} },

{0xDF, 1, {0x00} },
{0xE0, 1, {0x00} },
{0xE1, 1, {0x00} },
{0xE2, 1, {0x00} },
{0xE3, 1, {0x00} },

{0xE4, 1, {0x00} },

{0xE5, 1, {0x00} },

{0xE6, 1, {0x00} },

{0xE7, 1, {0x00} },

{0xE8, 1, {0x00} },

{0xE9, 1, {0x00} },

{0xff, 1, {0x21} },
{0xfb, 1, {0x01} },
{0xb0, 16, {0x00, 0x00, 0x00, 0x15, 0x00, 0x3F, 0x00, 0x5F,
                0x00, 0x7E, 0x00, 0x97, 0x00, 0xAF, 0x00, 0xc3} },
{0xb1, 16, {0x00, 0xd7, 0x01, 0x0A, 0x01, 0x32, 0x01, 0x6F,
                0x01, 0x9E, 0x01, 0xE5, 0x02, 0x1d, 0x02, 0x1e} },
{0xb2, 16, {0x02, 0x56, 0x02, 0x94, 0x02, 0xBC, 0x02, 0xF1,
                0x03, 0x13, 0x03, 0x41, 0x03, 0x4F, 0x03, 0x5F} },
{0xb3, 14, {0x03, 0x71, 0x03, 0x84, 0x03, 0x99, 0x03, 0xB0, 0x03, 0xCA, 0x03, 0xD7, 0x00, 0x00} },
{0xb4, 16, {0x00, 0x00, 0x00, 0x17, 0x00, 0x46, 0x00, 0x69,
                0x00, 0x8C, 0x00, 0xa5, 0x00, 0xbE, 0x00, 0xd1} },
{0xb5, 16, {0x00, 0xe4, 0x01, 0x18, 0x01, 0x40, 0x01, 0x7C,
                0x01, 0xAA, 0x01, 0xf0, 0x02, 0x27, 0x02, 0x28} },
{0xb6, 16, {0x02, 0x5E, 0x02, 0x9B, 0x02, 0xC3, 0x02, 0xF6,
                0x03, 0x18, 0x03, 0x45, 0x03, 0x54, 0x03, 0x63} },
{0xb7, 14, {0x03, 0x75, 0x03, 0x87, 0x03, 0x9C, 0x03, 0xB2, 0x03, 0xCA, 0x03, 0xD7, 0x00, 0x00} },
{0xb8, 16, {0x00, 0x00, 0x00, 0x18, 0x00, 0x49, 0x00, 0x6B,
                0x00, 0x8E, 0x00, 0xa8, 0x00, 0xC1, 0x00, 0xd3} },
{0xb9, 16, {0x00, 0xe5, 0x01, 0x18, 0x01, 0x3F, 0x01, 0x7B,
                0x01, 0xa8, 0x01, 0xEC, 0x02, 0x24, 0x02, 0x26} },
{0xba, 16, {0x02, 0x5A, 0x02, 0x97, 0x02, 0xc0, 0x02, 0xF4,
                0x03, 0x15, 0x03, 0x43, 0x03, 0x51, 0x03, 0x61} },
{0xbb, 14, {0x03, 0x72, 0x03, 0x85, 0x03, 0x9A, 0x03, 0xB1, 0x03, 0xCA, 0x03, 0xD7, 0x00, 0x00} },


{0XFF, 1, {0XE0} },
//REGR 0XFE, 0xE0
{0XFB, 1, {0X01} },
{0X35, 1, {0X82} },

{0XFF, 1, {0XF0} },
//REGR 0XFE, 0xF0
{0XFB, 1, {0X01} },
{0X1C, 1, {0X01} },
{0X33, 1, {0X01} },
{0X5A, 1, {0X00} },
{0XD2, 1, {0X52} },

{0XFF, 1, {0XD0} },
//REGR 0XFE, 0xD0
{0XFB, 1, {0X01} },
{0X53, 1, {0X22} },
{0X54, 1, {0X02} },

{0XFF, 1, {0XC0} },
//CCMON
{0XFB, 1, {0X01} },
{0X9C, 1, {0X11} },
{0X9D, 1, {0X11} },

{0XFF, 1, {0X2B} },
{0XFB, 1, {0X01} },
{0XB7, 1, {0X08} },
{0XB8, 1, {0X0B} },

//CCMOFF
//CCMRUN
{0XFF, 1, {0X10} },
{0XFB, 1, {0X01} },
{0X35, 1, {0X01} },//TE Enable
{0X51, 1, {0XFF} },//Write_Display_Brightness
{0X53, 1, {0X0C} },//Write_CTRL_Display
{0X55, 1, {0X00} },//Write CABC

{0x11, 0, {} },
#ifndef LCM_SET_DISPLAY_ON_DELAY
{REGFLAG_DELAY, 120, {} },
/* Display On*/
{0x29, 0, {} },
#endif
};

#ifdef LCM_SET_DISPLAY_ON_DELAY
/* to reduce init time, we move 120ms delay to lcm_set_display_on() !! */
static struct LCM_setting_table set_display_on[] = {
        {0x29, 0, {} },
};
#endif

static struct LCM_setting_table bl_level[] = {
    {0x51, 1, {0xFF} },
    {REGFLAG_END_OF_TABLE, 0x00, {} }
};

static void push_table(struct LCM_setting_table *table,
                unsigned int count, unsigned char force_update)
{
    unsigned int i;

    for (i = 0; i < count; i++) {
        unsigned int cmd;

        cmd = table[i].cmd;

        switch (cmd) {
        case REGFLAG_DELAY:
            if (table[i].count <= 10)
                MDELAY(table[i].count);
            else
                MDELAY(table[i].count);
            break;

        case REGFLAG_UDELAY:
            UDELAY(table[i].count);
            break;

        case REGFLAG_END_OF_TABLE:
            break;
        default:
            dsi_set_cmdq_V2(cmd, table[i].count, table[i].para_list, force_update);
            break;
        }
    }
}

/* --------------------------------------------------------------------------- */
/* LCM Driver Implementations */
/* --------------------------------------------------------------------------- */

static void lcm_set_util_funcs(const LCM_UTIL_FUNCS *util)
{
    memcpy(&lcm_util, util, sizeof(LCM_UTIL_FUNCS));
}


static void lcm_get_params(LCM_PARAMS *params)
{
    memset(params, 0, sizeof(LCM_PARAMS));

    params->type = LCM_TYPE_DSI;

    params->width = FRAME_WIDTH;
    params->height = FRAME_HEIGHT;

#if (LCM_DSI_CMD_MODE)
    params->dsi.mode = CMD_MODE;
    params->dsi.switch_mode = SYNC_PULSE_VDO_MODE;
#else
    params->dsi.mode = SYNC_PULSE_VDO_MODE;
    params->dsi.switch_mode = CMD_MODE;
#endif
    params->dsi.switch_mode_enable = 0;

    /* DSI */
    /* Command mode setting */
    params->dsi.LANE_NUM = LCM_FOUR_LANE;
    /* The following defined the fomat for data coming from LCD engine. */
    params->dsi.data_format.color_order = LCM_COLOR_ORDER_RGB;
    params->dsi.data_format.trans_seq = LCM_DSI_TRANS_SEQ_MSB_FIRST;
    params->dsi.data_format.padding = LCM_DSI_PADDING_ON_LSB;
    params->dsi.data_format.format = LCM_DSI_FORMAT_RGB888;

    /* Highly depends on LCD driver capability. */
    params->dsi.packet_size = 256;
    /* video mode timing */

    params->dsi.PS = LCM_PACKED_PS_24BIT_RGB888;

    params->dsi.vertical_sync_active = 10;
    params->dsi.vertical_backporch = 10;
    params->dsi.vertical_frontporch = 54;
    params->dsi.vertical_frontporch_for_low_power = 840;
    params->dsi.vertical_active_line = FRAME_HEIGHT;

    params->dsi.horizontal_sync_active = 12;
    params->dsi.horizontal_backporch = 60;
    params->dsi.horizontal_frontporch = 76;
    params->dsi.horizontal_active_pixel = FRAME_WIDTH;
    params->dsi.ssc_disable = 1;

#if (LCM_DSI_CMD_MODE)
    params->dsi.PLL_CLOCK = 595;    /* this value must be in MTK suggested table */
#else
    params->dsi.PLL_CLOCK = 595;    /* this value must be in MTK suggested table */
#endif
    params->dsi.data_rate = 1190;

    params->dsi.clk_lp_per_line_enable = 0;
    params->dsi.esd_check_enable = 1;
    params->dsi.customization_esd_check_enable = 0;
    params->dsi.lcm_esd_check_table[0].cmd = 0x0A;
    params->dsi.lcm_esd_check_table[0].count = 1;
    params->dsi.lcm_esd_check_table[0].para_list[0] = 0x9C;

    //D2 <-> D3
    params->dsi.lane_swap_en = 1;
    params->dsi.lane_swap[MIPITX_PHY_PORT_0][MIPITX_PHY_LANE_0] = MIPITX_PHY_LANE_0;
    params->dsi.lane_swap[MIPITX_PHY_PORT_0][MIPITX_PHY_LANE_1] = MIPITX_PHY_LANE_1;
    params->dsi.lane_swap[MIPITX_PHY_PORT_0][MIPITX_PHY_LANE_2] = MIPITX_PHY_LANE_3;
    params->dsi.lane_swap[MIPITX_PHY_PORT_0][MIPITX_PHY_LANE_3] = MIPITX_PHY_LANE_2;
    params->dsi.lane_swap[MIPITX_PHY_PORT_0][MIPITX_PHY_LANE_CK] = MIPITX_PHY_LANE_CK;
    params->dsi.lane_swap[MIPITX_PHY_PORT_0][MIPITX_PHY_LANE_RX] = MIPITX_PHY_LANE_0;

#ifdef MTK_RUNTIME_SWITCH_FPS_SUPPORT
    params->dsi.fps = 60;
#endif

#ifdef MTK_ROUND_CORNER_SUPPORT
    params->round_corner_params.round_corner_en = 1;
    params->round_corner_params.full_content = 0;
    params->round_corner_params.h = ROUND_CORNER_H_TOP;
    params->round_corner_params.h_bot = ROUND_CORNER_H_BOT;
    params->round_corner_params.tp_size = sizeof(top_rc_pattern);
    params->round_corner_params.lt_addr = (void *)top_rc_pattern;
#endif
}
static int _lcm_i2c_write_bytes(u8 addr, u8 value, void *fdt)
{
    s32 ret = 0;
    u8 write_data[2];
    u8 len;
    int i2c_id, i2c_addr;

    /* get i2c id and addr */
    if (lcm_util_get_i2c_lcd_bias(&i2c_id, &i2c_addr, fdt) < 0)
        return -1;

    i2c_id = i2c_id & 0xff;
    i2c_addr = i2c_addr & 0xff;

    /* pkt data */
    write_data[0] = addr;
    write_data[1] = value;
    len = 2;

    ret = mtk_i2c_write((u8)i2c_id, (u8)i2c_addr, write_data, len, NULL);
    if (ret < 0)
        LCM_LOGI("[LCM][ERROR] %s: %d\n", __func__, ret);

    LCM_LOGI("[LCM][i2c write] %s: ch:%u,addr:0x%x,reg:0x%x,val:0x%x\n",
        __func__, i2c_id, i2c_addr, addr, value);

    return ret;
}

static void lcm_init_power(void *fdt)
{
    SET_RESET_PIN(0, fdt);
    MDELAY(100);
#ifdef MTK_RT4831A_SUPPORT
    if (gate_ic_power_status_setting(fdt, 1)) {
        LCM_LOGI("set_gpio_lcm_led_en not defined\n");
        return;
    }
    _gate_ic_i2c_panel_bias_enable(fdt, 1);
#else

    if (lcm_util.set_gpio_lcd_enp_bias) {
        lcm_util.set_gpio_lcd_enp_bias(1, fdt);
        _lcm_i2c_write_bytes(0x00, 0x0f, fdt);
        _lcm_i2c_write_bytes(0x01, 0x0f, fdt);

        LCM_LOGI("%s\n", __func__);

    } else
        LCM_LOGI("set_gpio_lcd_enp_bias not defined\n");
#endif
}

static void lcm_suspend_power(void)
{

}

static void lcm_resume_power(void)
{

}

#ifdef LCM_SET_DISPLAY_ON_DELAY
static u32 lcm_init_time;
static int is_display_on;
#endif

static void lcm_init(void *fdt)
{
    int ret = 0;

    SET_RESET_PIN(0, fdt);


#ifndef MACH_FPGA
#if 0
    /*config rt5081 register 0xB2[7:6]=0x3, that is set db_delay=4ms.*/
    ret = PMU_REG_MASK(0xB2, (0x3 << 6), (0x3 << 6));

    /* set AVDD 5.4v, (4v+28*0.05v) */
    /*ret = RT5081_write_byte(0xB3, (1 << 6) | 28);*/
    ret = PMU_REG_MASK(0xB3, 28, (0x3F << 0));
    if (ret < 0)
        LCM_LOGI("nt35695----tps6132----cmd=%0x--i2c write error----\n", 0xB3);
    else
        LCM_LOGI("nt35695----tps6132----cmd=%0x--i2c write success----\n", 0xB3);

    /* set AVEE */
    /*ret = RT5081_write_byte(0xB4, (1 << 6) | 28);*/
    ret = PMU_REG_MASK(0xB4, 28, (0x3F << 0));
    if (ret < 0)
        LCM_LOGI("nt35695----tps6132----cmd=%0x--i2c write error----\n", 0xB4);
    else
        LCM_LOGI("nt35695----tps6132----cmd=%0x--i2c write success----\n", 0xB4);

    /* enable AVDD & AVEE */
    /* 0x12--default value; bit3--Vneg; bit6--Vpos; */
    /*ret = RT5081_write_byte(0xB1, 0x12 | (1<<3) | (1<<6));*/
    ret = PMU_REG_MASK(0xB1, (1<<3) | (1<<6), (1<<3) | (1<<6));
    if (ret < 0)
        LCM_LOGI("nt35695----tps6132----cmd=%0x--i2c write error----\n", 0xB1);
    else
        LCM_LOGI("nt35695----tps6132----cmd=%0x--i2c write success----\n", 0xB1);

    MDELAY(15);
#endif
#endif

    MDELAY(15);
    SET_RESET_PIN(1, fdt);
    MDELAY(10);
    SET_RESET_PIN(0, fdt);
    MDELAY(10);

    SET_RESET_PIN(1, fdt);
    MDELAY(10);

    push_table(init_setting, sizeof(init_setting) / sizeof(struct LCM_setting_table), 1);

#ifdef LCM_SET_DISPLAY_ON_DELAY
    lcm_init_time = current_time();
    is_display_on = 0;
#endif
}

#ifdef LCM_SET_DISPLAY_ON_DELAY
static int lcm_set_display_on(void)
{
    u32 timeout, i = 0;

    if (is_display_on)
        return 0;

    /* we need to wait 120ms after lcm init to set display on */
    timeout = 120;

    while ((current_time() - lcm_init_time) <= timeout) {
        i++;
        MDELAY(1);
        if (i % 1000 == 0) {
            LCM_LOGI("nt36672e %s error: i=%u,lcm_init_time=%u,cur_time=%u\n", __func__,
                i, lcm_init_time, current_time());
        }
    }

    push_table(set_display_on, sizeof(set_display_on) / sizeof(struct LCM_setting_table), 1);

    is_display_on = 1;
    return 0;
}
#endif

static void lcm_suspend(void)
{
#if 0
    int ret;

    push_table(lcm_suspend_setting,
        sizeof(lcm_suspend_setting) / sizeof(struct LCM_setting_table), 1);
#ifndef MACH_FPGA
    /* enable AVDD & AVEE */
    /* 0x12--default value; bit3--Vneg; bit6--Vpos; */
    /*ret = RT5081_write_byte(0xB1, 0x12);*/
    ret = PMU_REG_MASK(0xB1, (0<<3) | (0<<6), (1<<3) | (1<<6));
    if (ret < 0)
        LCM_LOGI("nt36672e----cmd=%0x--i2c write error----\n", 0xB1);
    else
        LCM_LOGI("nt36672e----cmd=%0x--i2c write success----\n", 0xB1);

    MDELAY(5);

#endif
    SET_RESET_PIN(0);
    MDELAY(10);
#endif
}

static void lcm_resume(void)
{
    //lcm_init();
}

#if LCM_DSI_CMD_MODE == 1
static void lcm_update(unsigned int x, unsigned int y, unsigned int width, unsigned int height)
{
    unsigned int x0 = x;
    unsigned int y0 = y;
    unsigned int x1 = x0 + width - 1;
    unsigned int y1 = y0 + height - 1;

    unsigned char x0_MSB = ((x0 >> 8) & 0xFF);
    unsigned char x0_LSB = (x0 & 0xFF);
    unsigned char x1_MSB = ((x1 >> 8) & 0xFF);
    unsigned char x1_LSB = (x1 & 0xFF);
    unsigned char y0_MSB = ((y0 >> 8) & 0xFF);
    unsigned char y0_LSB = (y0 & 0xFF);
    unsigned char y1_MSB = ((y1 >> 8) & 0xFF);
    unsigned char y1_LSB = (y1 & 0xFF);

    unsigned int data_array[16];

#ifdef LCM_SET_DISPLAY_ON_DELAY
    lcm_set_display_on();
#endif

    data_array[0] = 0x00053902;
    data_array[1] = (x1_MSB << 24) | (x0_LSB << 16) | (x0_MSB << 8) | 0x2a;
    data_array[2] = (x1_LSB);
    dsi_set_cmdq(data_array, 3, 1);

    data_array[0] = 0x00053902;
    data_array[1] = (y1_MSB << 24) | (y0_LSB << 16) | (y0_MSB << 8) | 0x2b;
    data_array[2] = (y1_LSB);
    dsi_set_cmdq(data_array, 3, 1);

    data_array[0] = 0x002c3909;
    dsi_set_cmdq(data_array, 1, 0);
}
#else /* not LCM_DSI_CMD_MODE */

static void lcm_update(unsigned int x, unsigned int y, unsigned int width, unsigned int height)
{
#ifdef LCM_SET_DISPLAY_ON_DELAY
    lcm_set_display_on();
#endif
}
#endif

static unsigned int lcm_compare_id(void *fdt)
{

    unsigned int id = 0;
    unsigned char buffer[2];
    unsigned int array[16];

    LCM_LOGI("%s: enter\n", __func__);

    SET_RESET_PIN(1, fdt);
    MDELAY(10);
    SET_RESET_PIN(0, fdt);
    MDELAY(10);
    SET_RESET_PIN(1, fdt);
    MDELAY(10);

    array[0] = 0x00013700;    /* read id return two byte,version and id */
    dsi_set_cmdq(array, 1, 1);

    read_reg_v2(0xDA, buffer, 1);
    id = buffer[0];     /* we only need ID */

    LCM_LOGI("%s, nt3672e_id_jdi=0x%08x\n", __func__, id);

    if (id == LCM_ID_NT36672E_JDI)
        return 1;
    else
        return 0;

}


/* return TRUE: need recovery */
/* return FALSE: No need recovery */
static unsigned int lcm_esd_check(void)
{
    return FALSE;
}

static unsigned int lcm_ata_check(unsigned char *buffer)
{
#if 0
    unsigned int ret = 0;
    unsigned int x0 = FRAME_WIDTH / 4;
    unsigned int x1 = FRAME_WIDTH * 3 / 4;

    unsigned char x0_MSB = ((x0 >> 8) & 0xFF);
    unsigned char x0_LSB = (x0 & 0xFF);
    unsigned char x1_MSB = ((x1 >> 8) & 0xFF);
    unsigned char x1_LSB = (x1 & 0xFF);

    unsigned int data_array[3];
    unsigned char read_buf[4];

    LCM_LOGI("ATA check size = 0x%x, 0x%x, 0x%x, 0x%x\n", x0_MSB, x0_LSB, x1_MSB, x1_LSB);
    data_array[0] = 0x0005390A; /* HS packet */
    data_array[1] = (x1_MSB << 24) | (x0_LSB << 16) | (x0_MSB << 8) | 0x2a;
    data_array[2] = (x1_LSB);
    dsi_set_cmdq(data_array, 3, 1);

    data_array[0] = 0x00043700; /* read id return two byte,version and id */
    dsi_set_cmdq(data_array, 1, 1);

    read_reg_v2(0x2A, read_buf, 4);

    if ((read_buf[0] == x0_MSB) && (read_buf[1] == x0_LSB)
            && (read_buf[2] == x1_MSB) && (read_buf[3] == x1_LSB))
        ret = 1;
    else
        ret = 0;

    x0 = 0;
    x1 = FRAME_WIDTH - 1;

    x0_MSB = ((x0 >> 8) & 0xFF);
    x0_LSB = (x0 & 0xFF);
    x1_MSB = ((x1 >> 8) & 0xFF);
    x1_LSB = (x1 & 0xFF);

    data_array[0] = 0x0005390A; /* HS packet */
    data_array[1] = (x1_MSB << 24) | (x0_LSB << 16) | (x0_MSB << 8) | 0x2a;
    data_array[2] = (x1_LSB);
    dsi_set_cmdq(data_array, 3, 1);
    return ret;
#else
    return 0;
#endif
}

static void lcm_setbacklight_cmdq(void *handle, unsigned int level)
{

    LCM_LOGI("%s, nt36672e backlight: level = %d\n", __func__, level);

    bl_level[0].para_list[0] = level;

    push_table(bl_level, sizeof(bl_level) / sizeof(struct LCM_setting_table), 1);
}

static void lcm_setbacklight(unsigned int level)
{
    LCM_LOGI("%s, nt36672e backlight: level = %d\n", __func__, level);

    bl_level[0].para_list[0] = level;

    push_table(bl_level, sizeof(bl_level) / sizeof(struct LCM_setting_table), 1);
}

static void *lcm_switch_mode(int mode)
{
    return NULL;
}


LCM_DRIVER nt36672e_fhdp_dsi_vdo_60hz_jdi_dphy_lcm_drv = {
    .name = "nt36672e_fhdp_dsi_vdo_60hz_jdi_dphy_lcm_drv",
    .set_util_funcs = lcm_set_util_funcs,
    .get_params = lcm_get_params,
    .init = lcm_init,
    .suspend = lcm_suspend,
    .resume = lcm_resume,
    .compare_id = lcm_compare_id,
    .init_power = lcm_init_power,
    .resume_power = lcm_resume_power,
    .suspend_power = lcm_suspend_power,
    .esd_check = lcm_esd_check,
    .set_backlight = lcm_setbacklight,
    .ata_check = lcm_ata_check,
    .update = lcm_update,
    .switch_mode = lcm_switch_mode,
};
