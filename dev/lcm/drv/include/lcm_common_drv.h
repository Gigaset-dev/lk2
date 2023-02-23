/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#pragma once

#include <lcm_drv.h>
#include <lcm_util.h>

#define MTK_LCM_NAME_LENGTH (128)

/* LCM PHY TYPE*/
#define MTK_LCM_MIPI_DPHY (0)
#define MTK_LCM_MIPI_CPHY (1)
#define MTK_LCM_MIPI_PHY_COUNT (MTK_LCM_MIPI_CPHY + 1)

/* LCM_FUNC used for common operation */
#define MTK_LCM_FUNC_DBI (0)
#define MTK_LCM_FUNC_DPI (1)
#define MTK_LCM_FUNC_DSI (2)
#define MTK_LCM_FUNC_END (MTK_LCM_FUNC_DSI + 1)

/* function: update dts settings of panel resource for kernel boot up,
 * input: the target lcm name, phy type, and dts node
 * output: 0 for success, else for failed
 */
int mtk_lcm_drv_update_panel_for_kernel(char func,
        const char *lcm_name, int phy_type, void *fdt);

/* function: get dts settings of panel dconfig,
 * input: dts root, the lcm name and size for read back,
 * output: 0 for success, else for failed
 */
int mtk_lcm_drv_get_dconfig(const void *fdt, char func,
        char *lcm_name, unsigned int size);

/* function: parse lcm dts settings of specified lcm_name,
 * input: lcm_name is target lcm dts settings,
 * output: 0 for success, else for failed
 */
int mtk_lcm_drv_parse_lcm_dts(const void *fdt, char func, const char *lcm_name);

/* function: parse next lcm dts settings in lcm list,
 * this is used to the 1st time boot up of scan lcm list,
 * output:
 *     MTK_LCM_DSI_COUNT for lcm list end or failed to get a valid lcm settings,
 *     < MTK_LCM_DSI_COUNT for a valid lcm settings parsing done ,
 */
int mtk_lcm_drv_parse_next_lcm_dts(const void *fdt, char func, char *lcm_name, bool force);

/* function: update lcm boot up device name into storage,
 * input: lcm_name is target lcm dts name, limitation of 128 char
 * output: 0 for success, else for failed
 */
int mtk_lcm_drv_set_lcm_boot_env(char func, char *lcm_name);

/* function: read storage settings of lcm boot env,
 * input: the return buffer and size of lcm name
 * output: 0 for success, else for failed
 */
int mtk_lcm_drv_check_lcm_boot_env(char func, char *lcm_name, unsigned int size);

/* function: get lcm common driver,
 * input: func: DSI, DBI, DPI,
 * output: lcm_drv: the result pointer
 */
LCM_DRIVER *mtk_lcm_get_common_drv(char func);

/* function: lcm power control
 * input:
 *     func: DSI, DBI, DPI
 *     value: high, low
 */
int mtk_lcm_drv_power_on(unsigned int func);
int mtk_lcm_drv_power_off(unsigned int func);
int mtk_lcm_drv_power_reset(unsigned int func, unsigned int value);

/* function: delay of ms/us,
 * input: func: DSI/DBI/DPI, type: ms/us, time: period
 * output: 0 for success, else for failed
 */
int mtk_panel_delay(int func, char type, unsigned int time);

/*********** led interfaces define **********/
int _gate_ic_i2c_write_bytes(void *fdt, unsigned char addr, unsigned char value);
int _gate_ic_i2c_write_regs(void *fdt, unsigned char addr,
         unsigned char *value, unsigned short size);
void _gate_ic_i2c_panel_bias_enable(void *fdt, unsigned int power_status);
long gate_ic_power_status_setting(void *fdt, unsigned int value);
int _gate_ic_i2c_backlight_enable(void *fdt, unsigned int enable,
         unsigned int pwm_enable);
int _gate_ic_i2c_backight_level_set(void *fdt, unsigned int level);
/************************************/

