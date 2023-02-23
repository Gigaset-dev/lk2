/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#pragma once

#include <debug.h>
#include <dev/timer/arm_generic.h>
#include <errno.h>
#include <gpio_api.h>
#include <lcm_common_drv.h>
#include "lcm_log.h"
#include "lcm_settings.h"
#include <libfdt.h>
#include <list.h>
#include <mt_i2c.h>
#include <string.h>

extern unsigned long long mtk_lcm_total_size;
extern void mdelay(unsigned long msec);
extern void udelay(unsigned long usec);
extern unsigned long long mtk_lcm_total_size;

#define MTK_LCM_DEBUG_DUMP (0)
#define LCM_MALLOC(buf, size) \
do { \
    buf = malloc(ROUNDUP(size, 4)); \
    if (buf != NULL) {\
         mtk_lcm_total_size += size; \
         memset(buf, 0, ROUNDUP(size, 4)); \
    } \
} while (0)

#define LCM_FREE(buf, size) \
do { \
    if (mtk_lcm_total_size >= ROUNDUP(size, 4)) \
        mtk_lcm_total_size -= ROUNDUP(size, 4); \
    else \
        mtk_lcm_total_size = 0; \
    free(buf); \
    buf = NULL; \
} while (0)

#define LCM_MALLOC(buf, size) \
do { \
    buf = malloc(ROUNDUP(size, 4)); \
    if (buf != NULL) {\
         mtk_lcm_total_size += size; \
         memset(buf, 0, ROUNDUP(size, 4)); \
    } \
} while (0)

#define LCM_FREE(buf, size) \
do { \
    if (mtk_lcm_total_size >= ROUNDUP(size, 4)) \
        mtk_lcm_total_size -= ROUNDUP(size, 4); \
    else \
        mtk_lcm_total_size = 0; \
    free(buf); \
    buf = NULL; \
} while (0)

struct mtk_gateic_data {
    struct mt_i2c client;
    unsigned int init;
    unsigned int ref;
    unsigned int backlight_status;
    unsigned int backlight_level;
    unsigned int backlight_mode;
    /* for gate IC of gpio control */
    int reset_gpio;
    int bias_pos_gpio;
    int bias_neg_gpio;
    unsigned int lcm_count;
    char **lcm_list;
};

struct mtk_gateic_funcs {
    /* power on with default voltage */
    int (*power_on)(void);

    /* power off panel */
    int (*power_off)(void);

    /* function: update voltage settings as panel spec requirement
     * input: the voltage level required by panel spec,
     * mention that gate IC will automatically transfer
     * the input of voltage level to HW settings.
     */
    int (*set_voltage)(unsigned int level);

    /* reset panel */
    int (*reset)(int on);
    /* get i2c client */
    struct mt_i2c *(*get_i2c_client)(void);

    /* enable backlight */
    int (*enable_backlight)(unsigned int enable, unsigned int pwm_enable);

    /* set backlight */
    int (*set_backlight)(unsigned int level);
};

/* function: register gateic ops of panel driver
 * input: gateic ops: panel driver pointer, func: DBI, DPI, DSI
 * output: 0 for success; !0 for failed
 * mention that gatic driver should be probed before panel driver,
 * else this function will be failed.
 */
int mtk_lcm_gateic_register(struct mtk_gateic_funcs *ops, char func);

/* function: get gateic ops of panel driver
 * input: func: DBI, DPI, DSI
 * output: 0 for success; !0 for failed
 */
struct mtk_gateic_funcs *mtk_lcm_gateic_get(char func);

/* function: panel power on
 * input: func: DBI, DPI, DSI
 * output: 0 for success; !0 for failed
 */
int mtk_lcm_gateic_power_on(char func);

/* function: panel power off
 * input: func: DBI, DPI, DSI
 * output: 0 for success; !0 for failed
 */
int mtk_lcm_gateic_power_off(char func);

/* function: panel power control
 * input: func: DBI, DPI, DSI, value:0 for off, 1 for on
 * output: 0 for success; !0 for failed
 */
int mtk_lcm_power_status_setting(unsigned int value, char func);

/* function: set panel power voltage
 * input: func: DBI, DPI, DSI
 * output: 0 for success; !0 for failed
 */
int mtk_lcm_gateic_set_voltage(unsigned int level, char func);

/* function: reset panel
 * input: func: DBI, DPI, DSI
 * output: 0 for success; !0 for failed
 */
int mtk_lcm_gateic_reset(int on, char func);

/* function: set backlight
 * input: func: DBI, DPI, DSI, brightness level
 * output: 0 for success; !0 for failed
 */
int mtk_lcm_gateic_set_backlight(unsigned int level, char func);

/* function: enable backlight
 * input: func: DBI, DPI, DSI,
 *        enbale: 1 for enable, 0 for disable
 *        pwm_enable: set backlight by pwm or not
 * output: 0 for success; !0 for failed
 */
int mtk_lcm_gateic_enable_backlight(unsigned int enable,
        unsigned int pwm_enable, char func);

/* function: write i2c data
 * input: addr: i2c address, value: i2c value
 * output: 0 for success; !0 for failed
 */
int mtk_panel_i2c_write_bytes(struct mt_i2c *client,
        unsigned char addr, unsigned char value);

/* function: write i2c data buffer
 * input: addr: i2c address,
 *        value: data buffer
 *        size: data size
 * output: 0 for success; !0 for failed
 */
int mtk_panel_i2c_write_multiple_bytes(struct mt_i2c *client,
        unsigned char addr, unsigned char *value,
        unsigned int size);

/* function: write gateic data
 * input: addr: i2c address,
 *        value: i2c value
 *        func: DBI, DPI, DSI
 * output: 0 for success; !0 for failed
 */
int mtk_lcm_gateic_write_bytes(unsigned char addr,
         unsigned char value, char func);

/* function: write gateic data buffer
 * input: addr: i2c address,
 *        value: data buffer
 *        size: data size
 *        func: DBI, DPI, DSI
 * output: 0 for success; !0 for failed
 */
int mtk_lcm_gateic_write_multiple_bytes(unsigned char addr,
         unsigned char *value, unsigned int size, char func);

/* function: get gpio/pinctrl settings
 * input: pin_name: the name of gpio dts node
 * output: 0 for success; !0 for failed
 */
int mtk_panel_get_pin(const void *fdt, const char *pin_name);

/* function: get i2c dts settings
 * input: id: the returned i2c index, addr: the returned i2c address
 * output: 0 for success, else for failed
 */
int mtk_panel_get_i2c_lcd_bias(const void *fdt, int *id, int *addr);

/* function: delay of ms/us,
 * input: func: DSI/DBI/DPI, type: ms/us, time: period
 * output: 0 for success, else for failed
 */
int mtk_lcm_dbi_delay(char type, unsigned int time);
int mtk_lcm_dpi_delay(char type, unsigned int time);
int mtk_lcm_dsi_delay(char type, unsigned int time);

/* function: check if lcm is supported by current gateic
 * input: lcm_name, gateic supported panel list and panel count
 * output: false for no, true for yes
 */
bool mtk_gateic_match_lcm_list(const char *lcm_name,
    char **list, unsigned int count, const char *gateic_name);

/*********** gate ic list define **********/
/* gate ic driver
 * list the gate ic driver init function here
 */
#ifdef MTK_LCM_GATEIC_RT4801H_SUPPORT
int rt4801h_gateic_init(const void *fdt, char func, const char *lcm_name);
#endif
#ifdef MTK_LCM_GATEIC_RT4831A_SUPPORT
int rt4831a_gateic_init(const void *fdt, char func, const char *lcm_name);
#endif
/****************************************/

/* function: init gateic driver
 * input: func: DBI, DPI, DSI
 * output: 0 for success; !0 for failed
 */
int mtk_lcm_gateic_drv_init(const void *fdt, char func, const char *lcm_name);

