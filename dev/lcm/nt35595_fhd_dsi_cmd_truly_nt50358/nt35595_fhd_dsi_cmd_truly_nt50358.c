/*
 * Copyright (c) 2020 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#define LOG_TAG "LCM"

#include <debug.h>
#include <platform.h>

#include "lcm_drv.h"
#include "lcm_util.h"

#define LCM_LOGI(string, args...)  dprintf(0, "[LK/"LOG_TAG"]"string, ##args)
#define LCM_LOGD(string, args...)  dprintf(1, "[LK/"LOG_TAG"]"string, ##args)

static LCM_UTIL_FUNCS lcm_util;

#define SET_RESET_PIN(v, fdt)    (lcm_util.set_reset_pin(v, fdt))
#define MDELAY(n)        (lcm_util.mdelay(n))
#define UDELAY(n)        (lcm_util.udelay(n))

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


/* static unsigned char lcd_id_pins_value = 0xFF; */
static const unsigned char LCD_MODULE_ID = 0x01;
#define LCM_DSI_CMD_MODE   1
#define FRAME_WIDTH        (1080)
#define FRAME_HEIGHT       (1920)



#define REGFLAG_DELAY           0xFFFC
#define REGFLAG_UDELAY          0xFFFB
#define REGFLAG_END_OF_TABLE    0xFFFD
#define REGFLAG_RESET_LOW       0xFFFE
#define REGFLAG_RESET_HIGH      0xFFFF

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

/* i2c control start */
#define LCM_I2C_MODE    ST_MODE
#define LCM_I2C_SPEED   100
#define LCM_ID_TD4330  0x02

#if 0
static struct mt_i2c_t tps65132_i2c;
static int _lcm_i2c_write_bytes(kal_uint8 addr, kal_uint8 value)
{
    kal_int32 ret_code = I2C_OK;
    kal_uint8 write_data[2];
    kal_uint16 len;
    int id, i2c_addr;

    write_data[0] = addr;
    write_data[1] = value;

    if (lcm_util_get_i2c_lcd_bias(&id, &i2c_addr) < 0)
        return -1;
    tps65132_i2c.id = (U16)id;
    tps65132_i2c.addr = (U16)i2c_addr;
    tps65132_i2c.mode = LCM_I2C_MODE;
    tps65132_i2c.speed = LCM_I2C_SPEED;
    len = 2;

    ret_code = i2c_write(&tps65132_i2c, write_data, len);
    if (ret_code < 0)
        dprintf(0, "[LCM][ERROR] %s: %d\n", __func__, ret_code);

    return ret_code;
}
#endif

/* i2c control end */

struct LCM_setting_table {
    unsigned int cmd;
    unsigned char count;
    unsigned char para_list[64];
};

static struct LCM_setting_table lcm_suspend_setting[] = {
    {0x28, 0, {} },
    {0x10, 0, {} },
    {REGFLAG_DELAY, 120, {} },
    {0x4F, 1, {0x01} },
    {REGFLAG_DELAY, 120, {} }
};

static struct LCM_setting_table init_setting[] = {
    {0xFF, 1, {0x24} },    //  Return  To  CMD1
    {0x6E, 1, {0x10} },    //  Return  To  CMD1
    {0xFB, 1, {0x01} },    //  Return  To  CMD1
    {0xFF, 1, {0x10} },    //  Return  To  CMD1

    {0xFF, 1, {0x10} },    //  Return  To  CMD1
    {REGFLAG_UDELAY, 1, {} },
#if (LCM_DSI_CMD_MODE)
    {0xBB, 1, {0x10} },
#else
    {0xBB, 1, {0x03} },
#endif
    {0x3B, 5, {0x03, 0x0A, 0x0A, 0x0A, 0x0A} },
    {0x53, 1, {0x24} },
    {0x55, 1, {0x00} },
    {0x5E, 1, {0x00} },
    {0x11, 0, {} },
    {REGFLAG_DELAY, 120, {} },
    {0xFF, 1, {0x24} },
    {REGFLAG_UDELAY, 1, {} },
    {0xFB, 1, {0x01} },
    {0x9D, 1, {0xB0} },
    {0x72, 1, {0x00} },
    {0x93, 1, {0x04} },
    {0x94, 1, {0x04} },
    {0x9B, 1, {0x0F} },
    {0x8A, 1, {0x33} },
    {0x86, 1, {0x1B} },
    {0x87, 1, {0x39} },
    {0x88, 1, {0x1B} },
    {0x89, 1, {0x39} },
    {0x8B, 1, {0xF4} },
    {0x8C, 1, {0x01} },
    {0x90, 1, {0x79} },
    {0x91, 1, {0x4C} },
    /* modify to 0x77 to see whether fps is higher */
    /* {0x92, 1, {0x79} }, */
    {0x92, 1, {0x77} },
    {0x95, 1, {0xE4} },

    {0xDE, 1, {0xFF} },
    {0xDF, 1, {0x82} },

    {0x00, 1, {0x0F} },
    {0x01, 1, {0x00} },
    {0x02, 1, {0x00} },
    {0x03, 1, {0x00} },
    {0x04, 1, {0x0B} },
    {0x05, 1, {0x0C} },
    {0x06, 1, {0x00} },
    {0x07, 1, {0x00} },
    {0x08, 1, {0x00} },
    {0x09, 1, {0x00} },
    {0x0A, 1, {0X03} },
    {0x0B, 1, {0X04} },
    {0x0C, 1, {0x01} },
    {0x0D, 1, {0x13} },
    {0x0E, 1, {0x15} },
    {0x0F, 1, {0x17} },
    {0x10, 1, {0x0F} },
    {0x11, 1, {0x00} },
    {0x12, 1, {0x00} },
    {0x13, 1, {0x00} },
    {0x14, 1, {0x0B} },
    {0x15, 1, {0x0C} },
    {0x16, 1, {0x00} },
    {0x17, 1, {0x00} },
    {0x18, 1, {0x00} },
    {0x19, 1, {0x00} },
    {0x1A, 1, {0x03} },
    {0x1B, 1, {0X04} },
    {0x1C, 1, {0x01} },
    {0x1D, 1, {0x13} },
    {0x1E, 1, {0x15} },
    {0x1F, 1, {0x17} },

    {0x20, 1, {0x09} },
    {0x21, 1, {0x01} },
    {0x22, 1, {0x00} },
    {0x23, 1, {0x00} },
    {0x24, 1, {0x00} },
    {0x25, 1, {0x6D} },
    {0x26, 1, {0x00} },
    {0x27, 1, {0x00} },

    {0x2F, 1, {0x02} },
    {0x30, 1, {0x04} },
    {0x31, 1, {0x49} },
    {0x32, 1, {0x23} },
    {0x33, 1, {0x01} },
    {0x34, 1, {0x00} },
    {0x35, 1, {0x69} },
    {0x36, 1, {0x00} },
    {0x37, 1, {0x2D} },
    {0x38, 1, {0x08} },
    {0x39, 1, {0x00} },
    {0x3A, 1, {0x69} },

    {0x29, 1, {0x58} },
    {0x2A, 1, {0x16} },

    {0x5B, 1, {0x00} },
    {0x5F, 1, {0x75} },
    {0x63, 1, {0x00} },
    {0x67, 1, {0x04} },

    {0x7B, 1, {0x80} },
    {0x7C, 1, {0xD8} },
    {0x7D, 1, {0x60} },
    {0x7E, 1, {0x10} },
    {0x7F, 1, {0x19} },
    {0x80, 1, {0x00} },
    {0x81, 1, {0x06} },
    {0x82, 1, {0x03} },
    {0x83, 1, {0x00} },
    {0x84, 1, {0x03} },
    {0x85, 1, {0x07} },
    {0x74, 1, {0x10} },
    {0x75, 1, {0x19} },
    {0x76, 1, {0x06} },
    {0x77, 1, {0x03} },

    {0x78, 1, {0x00} },
    {0x79, 1, {0x00} },
    {0x99, 1, {0x33} },
    {0x98, 1, {0x00} },
    {0xB3, 1, {0x28} },
    {0xB4, 1, {0x05} },
    {0xB5, 1, {0x10} },

    {0xFF, 1, {0x20} },
    {REGFLAG_UDELAY, 1, {} },
    {0x00, 1, {0x01} },
    {0x01, 1, {0x55} },
    {0x02, 1, {0x45} },
    {0x03, 1, {0x55} },
    {0x05, 1, {0x50} },
    {0x06, 1, {0x9E} },
    {0x07, 1, {0xA8} },
    {0x08, 1, {0x0C} },
    {0x0B, 1, {0x96} },
    {0x0C, 1, {0x96} },
    {0x0E, 1, {0x00} },
    {0x0F, 1, {0x00} },
    {0x11, 1, {0x29} },
    {0x12, 1, {0x29} },
    {0x13, 1, {0x03} },
    {0x14, 1, {0x0A} },
    {0x15, 1, {0x99} },
    {0x16, 1, {0x99} },
    {0x6D, 1, {0x44} },
    {0x58, 1, {0x05} },
    {0x59, 1, {0x05} },
    {0x5A, 1, {0x05} },
    {0x5B, 1, {0x05} },
    {0x5C, 1, {0x00} },
    {0x5D, 1, {0x00} },
    {0x5E, 1, {0x00} },
    {0x5F, 1, {0x00} },

    {0x1B, 1, {0x39} },
    {0x1C, 1, {0x39} },
    {0x1D, 1, {0x47} },

    {0xFF, 1, {0x20} },
    {REGFLAG_UDELAY, 1, {} },
    /* R+   , 1, {} }, */
    {0x75, 1, {0x00} },
    {0x76, 1, {0x00} },
    {0x77, 1, {0x00} },
    {0x78, 1, {0x22} },
    {0x79, 1, {0x00} },
    {0x7A, 1, {0x46} },
    {0x7B, 1, {0x00} },
    {0x7C, 1, {0x5C} },
    {0x7D, 1, {0x00} },
    {0x7E, 1, {0x76} },
    {0x7F, 1, {0x00} },
    {0x80, 1, {0x8D} },
    {0x81, 1, {0x00} },
    {0x82, 1, {0xA6} },
    {0x83, 1, {0x00} },
    {0x84, 1, {0xB8} },
    {0x85, 1, {0x00} },
    {0x86, 1, {0xC7} },
    {0x87, 1, {0x00} },
    {0x88, 1, {0xF6} },
    {0x89, 1, {0x01} },
    {0x8A, 1, {0x1D} },
    {0x8B, 1, {0x01} },
    {0x8C, 1, {0x54} },
    {0x8D, 1, {0x01} },
    {0x8E, 1, {0x81} },
    {0x8F, 1, {0x01} },
    {0x90, 1, {0xCB} },
    {0x91, 1, {0x02} },
    {0x92, 1, {0x05} },
    {0x93, 1, {0x02} },
    {0x94, 1, {0x07} },
    {0x95, 1, {0x02} },
    {0x96, 1, {0x47} },
    {0x97, 1, {0x02} },
    {0x98, 1, {0x82} },
    {0x99, 1, {0x02} },
    {0x9A, 1, {0xAB} },
    {0x9B, 1, {0x02} },
    {0x9C, 1, {0xDC} },
    {0x9D, 1, {0x03} },
    {0x9E, 1, {0x01} },
    {0x9F, 1, {0x03} },
    {0xA0, 1, {0x3A} },
    {0xA2, 1, {0x03} },
    {0xA3, 1, {0x56} },
    {0xA4, 1, {0x03} },
    {0xA5, 1, {0x6D} },
    {0xA6, 1, {0x03} },
    {0xA7, 1, {0x89} },
    {0xA9, 1, {0x03} },
    {0xAA, 1, {0xA3} },
    {0xAB, 1, {0x03} },
    {0xAC, 1, {0xC9} },
    {0xAD, 1, {0x03} },
    {0xAE, 1, {0xDD} },
    {0xAF, 1, {0x03} },
    {0xB0, 1, {0xF5} },
    {0xB1, 1, {0x03} },
    {0xB2, 1, {0xFF} },
    /* R-   , 1, {} }, */
    {0xB3, 1, {0x00} },
    {0xB4, 1, {0x00} },
    {0xB5, 1, {0x00} },
    {0xB6, 1, {0x22} },
    {0xB7, 1, {0x00} },
    {0xB8, 1, {0x46} },
    {0xB9, 1, {0x00} },
    {0xBA, 1, {0x5C} },
    {0xBB, 1, {0x00} },
    {0xBC, 1, {0x76} },
    {0xBD, 1, {0x00} },
    {0xBE, 1, {0x8D} },
    {0xBF, 1, {0x00} },
    {0xC0, 1, {0xA6} },
    {0xC1, 1, {0x00} },
    {0xC2, 1, {0xB8} },
    {0xC3, 1, {0x00} },
    {0xC4, 1, {0xC7} },
    {0xC5, 1, {0x00} },
    {0xC6, 1, {0xF6} },
    {0xC7, 1, {0x01} },
    {0xC8, 1, {0x1D} },
    {0xC9, 1, {0x01} },
    {0xCA, 1, {0x54} },
    {0xCB, 1, {0x01} },
    {0xCC, 1, {0x81} },
    {0xCD, 1, {0x01} },
    {0xCE, 1, {0xCB} },
    {0xCF, 1, {0x02} },
    {0xD0, 1, {0x05} },
    {0xD1, 1, {0x02} },
    {0xD2, 1, {0x07} },
    {0xD3, 1, {0x02} },
    {0xD4, 1, {0x47} },
    {0xD5, 1, {0x02} },
    {0xD6, 1, {0x82} },
    {0xD7, 1, {0x02} },
    {0xD8, 1, {0xAB} },
    {0xD9, 1, {0x02} },
    {0xDA, 1, {0xDC} },
    {0xDB, 1, {0x03} },
    {0xDC, 1, {0x01} },
    {0xDD, 1, {0x03} },
    {0xDE, 1, {0x3A} },
    {0xDF, 1, {0x03} },
    {0xE0, 1, {0x56} },
    {0xE1, 1, {0x03} },
    {0xE2, 1, {0x6D} },
    {0xE3, 1, {0x03} },
    {0xE4, 1, {0x89} },
    {0xE5, 1, {0x03} },
    {0xE6, 1, {0xA3} },
    {0xE7, 1, {0x03} },
    {0xE8, 1, {0xC9} },
    {0xE9, 1, {0x03} },
    {0xEA, 1, {0xDD} },
    {0xEB, 1, {0x03} },
    {0xEC, 1, {0xF5} },
    {0xED, 1, {0x03} },
    {0xEE, 1, {0xFF} },
    /* G+   , 1, {} }, */
    {0xEF, 1, {0x00} },
    {0xF0, 1, {0x00} },
    {0xF1, 1, {0x00} },
    {0xF2, 1, {0x22} },
    {0xF3, 1, {0x00} },
    {0xF4, 1, {0x46} },
    {0xF5, 1, {0x00} },
    {0xF6, 1, {0x5C} },
    {0xF7, 1, {0x00} },
    {0xF8, 1, {0x76} },
    {0xF9, 1, {0x00} },
    {0xFA, 1, {0x8D} },

    {0xFF, 1, {0x21} },
    {REGFLAG_UDELAY, 1, {} },
    {0x00, 1, {0x00} },
    {0x01, 1, {0xA6} },
    {0x02, 1, {0x00} },
    {0x03, 1, {0xB8} },
    {0x04, 1, {0x00} },
    {0x05, 1, {0xC7} },
    {0x06, 1, {0x00} },
    {0x07, 1, {0xF6} },
    {0x08, 1, {0x01} },
    {0x09, 1, {0x1D} },
    {0x0A, 1, {0x01} },
    {0x0B, 1, {0x54} },
    {0x0C, 1, {0x01} },
    {0x0D, 1, {0x81} },
    {0x0E, 1, {0x01} },
    {0x0F, 1, {0xCB} },
    {0x10, 1, {0x02} },
    {0x11, 1, {0x05} },
    {0x12, 1, {0x02} },
    {0x13, 1, {0x07} },
    {0x14, 1, {0x02} },
    {0x15, 1, {0x47} },
    {0x16, 1, {0x02} },
    {0x17, 1, {0x82} },
    {0x18, 1, {0x02} },
    {0x19, 1, {0xAB} },
    {0x1A, 1, {0x02} },
    {0x1B, 1, {0xDC} },
    {0x1C, 1, {0x03} },
    {0x1D, 1, {0x01} },
    {0x1E, 1, {0x03} },
    {0x1F, 1, {0x3A} },
    {0x20, 1, {0x03} },
    {0x21, 1, {0x56} },
    {0x22, 1, {0x03} },
    {0x23, 1, {0x6D} },
    {0x24, 1, {0x03} },
    {0x25, 1, {0x89} },
    {0x26, 1, {0x03} },
    {0x27, 1, {0xA3} },
    {0x28, 1, {0x03} },
    {0x29, 1, {0xC9} },
    {0x2A, 1, {0x03} },
    {0x2B, 1, {0xDD} },
    {0x2D, 1, {0x03} },
    {0x2F, 1, {0xF5} },
    {0x30, 1, {0x03} },
    {0x31, 1, {0xFF} },
    /* G-   , 1, {} }, */
    {0x32, 1, {0x00} },
    {0x33, 1, {0x00} },
    {0x34, 1, {0x00} },
    {0x35, 1, {0x22} },
    {0x36, 1, {0x00} },
    {0x37, 1, {0x46} },
    {0x38, 1, {0x00} },
    {0x39, 1, {0x5C} },
    {0x3A, 1, {0x00} },
    {0x3B, 1, {0x76} },
    {0x3D, 1, {0x00} },
    {0x3F, 1, {0x8D} },
    {0x40, 1, {0x00} },
    {0x41, 1, {0xA6} },
    {0x42, 1, {0x00} },
    {0x43, 1, {0xB8} },
    {0x44, 1, {0x00} },
    {0x45, 1, {0xC7} },
    {0x46, 1, {0x00} },
    {0x47, 1, {0xF6} },
    {0x48, 1, {0x01} },
    {0x49, 1, {0x1D} },
    {0x4A, 1, {0x01} },
    {0x4B, 1, {0x54} },
    {0x4C, 1, {0x01} },
    {0x4D, 1, {0x81} },
    {0x4E, 1, {0x01} },
    {0x4F, 1, {0xCB} },
    {0x50, 1, {0x02} },
    {0x51, 1, {0x05} },
    {0x52, 1, {0x02} },
    {0x53, 1, {0x07} },
    {0x54, 1, {0x02} },
    {0x55, 1, {0x47} },
    {0x56, 1, {0x02} },
    {0x58, 1, {0x82} },
    {0x59, 1, {0x02} },
    {0x5A, 1, {0xAB} },
    {0x5B, 1, {0x02} },
    {0x5C, 1, {0xDC} },
    {0x5D, 1, {0x03} },
    {0x5E, 1, {0x01} },
    {0x5F, 1, {0x03} },
    {0x60, 1, {0x3A} },
    {0x61, 1, {0x03} },
    {0x62, 1, {0x56} },
    {0x63, 1, {0x03} },
    {0x64, 1, {0x6D} },
    {0x65, 1, {0x03} },
    {0x66, 1, {0x89} },
    {0x67, 1, {0x03} },
    {0x68, 1, {0xA3} },
    {0x69, 1, {0x03} },
    {0x6A, 1, {0xC9} },
    {0x6B, 1, {0x03} },
    {0x6C, 1, {0xDD} },
    {0x6D, 1, {0x03} },
    {0x6E, 1, {0xF5} },
    {0x6F, 1, {0x03} },
    {0x70, 1, {0xFF} },
    /* B+   , 1, {} }, */
    {0x71, 1, {0x00} },
    {0x72, 1, {0x00} },
    {0x73, 1, {0x00} },
    {0x74, 1, {0x22} },
    {0x75, 1, {0x00} },
    {0x76, 1, {0x46} },
    {0x77, 1, {0x00} },
    {0x78, 1, {0x5C} },
    {0x79, 1, {0x00} },
    {0x7A, 1, {0x76} },
    {0x7B, 1, {0x00} },
    {0x7C, 1, {0x8D} },
    {0x7D, 1, {0x00} },
    {0x7E, 1, {0xA6} },
    {0x7F, 1, {0x00} },
    {0x80, 1, {0xB8} },
    {0x81, 1, {0x00} },
    {0x82, 1, {0xC7} },
    {0x83, 1, {0x00} },
    {0x84, 1, {0xF6} },
    {0x85, 1, {0x01} },
    {0x86, 1, {0x1D} },
    {0x87, 1, {0x01} },
    {0x88, 1, {0x54} },
    {0x89, 1, {0x01} },
    {0x8A, 1, {0x81} },
    {0x8B, 1, {0x01} },
    {0x8C, 1, {0xCB} },
    {0x8D, 1, {0x02} },
    {0x8E, 1, {0x05} },
    {0x8F, 1, {0x02} },
    {0x90, 1, {0x07} },
    {0x91, 1, {0x02} },
    {0x92, 1, {0x47} },
    {0x93, 1, {0x02} },
    {0x94, 1, {0x82} },
    {0x95, 1, {0x02} },
    {0x96, 1, {0xAB} },
    {0x97, 1, {0x02} },
    {0x98, 1, {0xDC} },
    {0x99, 1, {0x03} },
    {0x9A, 1, {0x01} },
    {0x9B, 1, {0x03} },
    {0x9C, 1, {0x3A} },
    {0x9D, 1, {0x03} },
    {0x9E, 1, {0x56} },
    {0x9F, 1, {0x03} },
    {0xA0, 1, {0x6D} },
    {0xA2, 1, {0x03} },
    {0xA3, 1, {0x89} },
    {0xA4, 1, {0x03} },
    {0xA5, 1, {0xA3} },
    {0xA6, 1, {0x03} },
    {0xA7, 1, {0xC9} },
    {0xA9, 1, {0x03} },
    {0xAA, 1, {0xDD} },
    {0xAB, 1, {0x03} },
    {0xAC, 1, {0xF5} },
    {0xAD, 1, {0x03} },
    {0xAE, 1, {0xFF} },
    /* B-   , 1, {} }, */
    {0xAF, 1, {0x00} },
    {0xB0, 1, {0x00} },
    {0xB1, 1, {0x00} },
    {0xB2, 1, {0x22} },
    {0xB3, 1, {0x00} },
    {0xB4, 1, {0x46} },
    {0xB5, 1, {0x00} },
    {0xB6, 1, {0x5C} },
    {0xB7, 1, {0x00} },
    {0xB8, 1, {0x76} },
    {0xB9, 1, {0x00} },
    {0xBA, 1, {0x8D} },
    {0xBB, 1, {0x00} },
    {0xBC, 1, {0xA6} },
    {0xBD, 1, {0x00} },
    {0xBE, 1, {0xB8} },
    {0xBF, 1, {0x00} },
    {0xC0, 1, {0xC7} },
    {0xC1, 1, {0x00} },
    {0xC2, 1, {0xF6} },
    {0xC3, 1, {0x01} },
    {0xC4, 1, {0x1D} },
    {0xC5, 1, {0x01} },
    {0xC6, 1, {0x54} },
    {0xC7, 1, {0x01} },
    {0xC8, 1, {0x81} },
    {0xC9, 1, {0x01} },
    {0xCA, 1, {0xCB} },
    {0xCB, 1, {0x02} },
    {0xCC, 1, {0x05} },
    {0xCD, 1, {0x02} },
    {0xCE, 1, {0x07} },
    {0xCF, 1, {0x02} },
    {0xD0, 1, {0x47} },
    {0xD1, 1, {0x02} },
    {0xD2, 1, {0x82} },
    {0xD3, 1, {0x02} },
    {0xD4, 1, {0xAB} },
    {0xD5, 1, {0x02} },
    {0xD6, 1, {0xDC} },
    {0xD7, 1, {0x03} },
    {0xD8, 1, {0x01} },
    {0xD9, 1, {0x03} },
    {0xDA, 1, {0x3A} },
    {0xDB, 1, {0x03} },
    {0xDC, 1, {0x56} },
    {0xDD, 1, {0x03} },
    {0xDE, 1, {0x6D} },
    {0xDF, 1, {0x03} },
    {0xE0, 1, {0x89} },
    {0xE1, 1, {0x03} },
    {0xE2, 1, {0xA3} },
    {0xE3, 1, {0x03} },
    {0xE4, 1, {0xC9} },
    {0xE5, 1, {0x03} },
    {0xE6, 1, {0xDD} },
    {0xE7, 1, {0x03} },
    {0xE8, 1, {0xF5} },
    {0xE9, 1, {0x03} },
    {0xEA, 1, {0xFF} },

    {0xFF, 1, {0x21} },
    {REGFLAG_UDELAY, 1, {} },
    {0xEB, 1, {0x30} },
    {0xEC, 1, {0x17} },
    {0xED, 1, {0x20} },
    {0xEE, 1, {0x0F} },
    {0xEF, 1, {0x1F} },
    {0xF0, 1, {0x0F} },
    {0xF1, 1, {0x0F} },
    {0xF2, 1, {0x07} },

    {0xFF, 1, {0x23} },
    {REGFLAG_UDELAY, 1, {} },
    {0x08, 1, {0x04} },

    /* image.first */
    {0xFF, 1, {0x10} },
    {REGFLAG_UDELAY, 1, {} },
    {0x35, 1, {0x00} },
    {0x29, 0, {} },
    /* {0x51, 1, {0xFF} },     //      write   display brightness */
};

#ifdef LCM_SET_DISPLAY_ON_DELAY
/* to reduce init time, we move 120ms delay to lcm_set_display_on() !! */
static struct LCM_setting_table set_display_on[] = {
    {0x29, 0, {} },
    {REGFLAG_DELAY, 100, {} }, //Delay 100ms
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
    unsigned int cmd;

    for (i = 0; i < count; i++) {
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
            dsi_set_cmdq_V2(cmd, table[i].count,
                table[i].para_list, force_update);
        }
    }
}

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

    params->dsi.vertical_sync_active = 2;
    params->dsi.vertical_backporch = 8;
    params->dsi.vertical_frontporch = 10;
    params->dsi.vertical_active_line = FRAME_HEIGHT;

    params->dsi.horizontal_sync_active = 10;
    params->dsi.horizontal_backporch = 20;
    params->dsi.horizontal_frontporch = 40;
    params->dsi.horizontal_active_pixel = FRAME_WIDTH;
    params->dsi.ssc_disable = 1;
#ifndef PROJECT_TYPE_FPGA
#if (LCM_DSI_CMD_MODE)
    params->dsi.PLL_CLOCK = 500;    /* this value must be in MTK suggested table */
#else
    params->dsi.PLL_CLOCK = 450;    /* this value must be in MTK suggested table */
#endif
#else
    params->dsi.PLL_CLOCK = 80;
#endif

    params->dsi.clk_lp_per_line_enable = 0;
    params->dsi.esd_check_enable = 1;
    params->dsi.customization_esd_check_enable = 0;
    params->dsi.lcm_esd_check_table[0].cmd = 0x53;
    params->dsi.lcm_esd_check_table[0].count = 1;
    params->dsi.lcm_esd_check_table[0].para_list[0] = 0x24;

}


static void lcm_init_power(void *fdt)
{
    if (lcm_util.set_gpio_lcd_enp_bias) {
        lcm_util.set_gpio_lcd_enp_bias(1, fdt);
        //_lcm_i2c_write_bytes(0x0, 0xf);
        //_lcm_i2c_write_bytes(0x1, 0xf);
    } else
        LCM_LOGI("set_gpio_lcd_enp_bias not defined\n");
}

static void lcm_suspend_power(void)
{
#if 0
    SET_RESET_PIN(0);
    if (lcm_util.set_gpio_lcd_enp_bias)
        lcm_util.set_gpio_lcd_enp_bias(0);
    else
        LCM_LOGI("set_gpio_lcd_enp_bias not defined\n");
#endif
}

static void lcm_resume_power(void)
{
#if 0
    SET_RESET_PIN(0);
    lcm_init_power();
#endif
}

#ifdef LCM_SET_DISPLAY_ON_DELAY
static u32 lcm_init_time;
static int is_display_on;
#endif

static void lcm_init(void *fdt)
{

    SET_RESET_PIN(1, fdt);
    MDELAY(1);
    SET_RESET_PIN(0, fdt);
    MDELAY(10);

    SET_RESET_PIN(1, fdt);
    MDELAY(10);

    push_table(init_setting, sizeof(init_setting) /
            sizeof(struct LCM_setting_table), 1);
#ifdef LCM_SET_DISPLAY_ON_DELAY
    lcm_init_time = current_time();
    is_display_on = 0;
#endif
    LCM_LOGI("%s -\n", __func__);
}

#ifdef LCM_SET_DISPLAY_ON_DELAY
static int lcm_set_display_on(void)
{
    u32 timeout, i = 0;

    if (is_display_on)
        return 0;

    /* we need to wait 200ms after lcm init to set display on */
    timeout = 120;

    while ((current_time() - lcm_init_time) <= timeout) {
        i++;
        MDELAY(1);
        if (i % 1000 == 0) {
            LCM_LOGI("td4320 %s error: i=%u,lcm_init_time=%u,cur_time=%u\n", __func__, i,
                lcm_init_time, current_time());
        }
    }

    push_table(set_display_on, sizeof(set_display_on) /
           sizeof(struct LCM_setting_table), 1);

    is_display_on = 1;
    return 0;
}
#endif

static void lcm_suspend(void)
{
#if 0
    push_table(lcm_suspend_setting, sizeof(lcm_suspend_setting) /
            sizeof(struct LCM_setting_table), 1);
    //SET_RESET_PIN(0);
    MDELAY(10);
#endif
}

static void lcm_resume(void)
{
    //lcm_init();
}

#if 1
static void lcm_update(unsigned int x, unsigned int y, unsigned int width, unsigned int height)
{
#ifdef LCM_SET_DISPLAY_ON_DELAY
    lcm_set_display_on();
#endif
}
#else
static void lcm_update(unsigned int x, unsigned int y, unsigned int width, unsigned int height)
{
#ifdef LCM_SET_DISPLAY_ON_DELAY
    lcm_set_display_on();
#endif
}
#endif

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

    LCM_LOGI("ATA check size = 0x%x,0x%x,0x%x,0x%x\n",
        x0_MSB, x0_LSB, x1_MSB, x1_LSB);
    data_array[0] = 0x0005390A; /* HS packet */
    data_array[1] = (x1_MSB << 24) | (x0_LSB << 16) | (x0_MSB << 8) | 0x2a;
    data_array[2] = (x1_LSB);
    dsi_set_cmdq(data_array, 3, 1);

    data_array[0] = 0x00043700; /* read id return two byte,version and id */
    dsi_set_cmdq(data_array, 1, 1);

    read_reg_v2(0x2A, read_buf, 4);

    if ((read_buf[0] == x0_MSB) && (read_buf[1] == x0_LSB) &&
        (read_buf[2] == x1_MSB) && (read_buf[3] == x1_LSB))
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

    LCM_LOGI("%s,nt35595 backlight: level = %d\n", __func__, level);

    bl_level[0].para_list[0] = level;

    push_table(bl_level, sizeof(bl_level) /
            sizeof(struct LCM_setting_table), 1);
}

static void lcm_setbacklight(unsigned int level)
{
    LCM_LOGI("%s,nt35595 backlight: level = %d\n", __func__, level);

    bl_level[0].para_list[0] = level;

    push_table(bl_level, sizeof(bl_level) /
            sizeof(struct LCM_setting_table), 1);
}

#if 0
#define LCM_ID_NT35595 (0x95)

static unsigned int lcm_compare_id(void)
{
    unsigned int id = 0;
    unsigned char buffer[2];
    unsigned int array[16];

    SET_RESET_PIN(1);
    SET_RESET_PIN(0);
    MDELAY(1);

    SET_RESET_PIN(1);
    MDELAY(20);

    array[0] = 0x00023700;  /* read id return two byte,version and id */
    dsi_set_cmdq(array, 1, 1);

    read_reg_v2(0xF4, buffer, 2);
    id = buffer[0];     /* we only need ID */

    LCM_LOGI("%s,nt35595 debug: nt35595 id = 0x%08x\n", __func__, id);

    if (id == LCM_ID_NT35595)
        return 1;
    else
        return 0;

}
#endif

LCM_DRIVER nt35595_fhd_dsi_cmd_truly_nt50358_lcm_drv = {
    .name = "nt35595_fhd_dsi_cmd_truly_nt50358_drv",
    .set_util_funcs = lcm_set_util_funcs,
    .get_params = lcm_get_params,
    .init = lcm_init,
    .suspend = lcm_suspend,
    .resume = lcm_resume,
    .init_power = lcm_init_power,
    //.compare_id = lcm_compare_id,
    .resume_power = lcm_resume_power,
    .suspend_power = lcm_suspend_power,
    .set_backlight = lcm_setbacklight,
    .ata_check = lcm_ata_check,
    .update = lcm_update,
};
