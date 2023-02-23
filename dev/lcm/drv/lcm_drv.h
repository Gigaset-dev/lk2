/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#pragma once

#include "lcm_helper.h"

extern LCM_DRIVER mtk_lcm_dbi_drv;
extern LCM_DRIVER mtk_lcm_dpi_drv;
extern LCM_DRIVER mtk_lcm_dsi_drv;

struct mtk_panel_context {
    /* status of dts parsing */
    unsigned int parsing;
    char lcm_name[MTK_LCM_NAME_LENGTH];
    unsigned int enabled;
    unsigned int display_on;
    lk_time_t lcm_init_tick;

    /* gate ic operations */
    struct mtk_gateic_funcs gateic_ops;

    /* panel params and operations parsed from dtsi */
    struct mtk_panel_resource *panel_resource;
};

/* register customization callback of panel operation
 * func: MTK_LCM_FUNC_DBI/DPI/DSI
 * cust: customized function callback
 * return: 0 for success, else for failed
 */
int mtk_panel_register_drv_customization_callback(char func,
        struct mtk_panel_cust *cust);
int mtk_panel_register_dbi_customization_callback(
        struct mtk_panel_cust *cust);
int mtk_panel_register_dpi_customization_callback(
        struct mtk_panel_cust *cust);
int mtk_panel_register_dsi_customization_callback(
        struct mtk_panel_cust *cust);

/* function: update dts settings of panel resource for kernel boot up,
 * input: the target lcm name, phy type, and dts node
 * output: 0 for success, else for failed
 */
int mtk_lcm_dbi_update_panel_for_kernel(const char *lcm_name,
        int phy_type, void *fdt);
int mtk_lcm_dpi_update_panel_for_kernel(const char *lcm_name,
        int phy_type, void *fdt);
int mtk_lcm_dsi_update_panel_for_kernel(const char *lcm_name,
        int phy_type, void *fdt);

/* function: get dts settings of panel dconfig,
 * input: dts root, the lcm name and size for read back,
 * output: 0 for success, else for failed
 */
int mtk_lcm_dbi_get_dconfig_panel(const void *fdt,
        char *lcm_name, unsigned int size);
int mtk_lcm_dpi_get_dconfig_panel(const void *fdt,
        char *lcm_name, unsigned int size);
int mtk_lcm_dsi_get_dconfig_panel(const void *fdt,
        char *lcm_name, unsigned int size);

/* function: parse lcm dts settings of specified lcm_name,
 * input: lcm_name is target lcm dts settings,
 * output: 0 for success, else for failed
 */
int mtk_lcm_dbi_parse_lcm_dts(const void *fdt, const char *lcm_name);
int mtk_lcm_dpi_parse_lcm_dts(const void *fdt, const char *lcm_name);
int mtk_lcm_dsi_parse_lcm_dts(const void *fdt, const char *lcm_name);

/* function: parse next lcm dts settings in lcm list,
 * this is used to the 1st time boot up of scan lcm list,
 * output:
 *     MTK_LCM_DSI_COUNT for lcm list end or failed to get a valid lcm settings,
 *     < MTK_LCM_DSI_COUNT for a valid lcm settings parsing done ,
 */
int mtk_lcm_dbi_parse_next_lcm_dts(const void *fdt, char *lcm_name, bool force);
int mtk_lcm_dpi_parse_next_lcm_dts(const void *fdt, char *lcm_name, bool force);
int mtk_lcm_dsi_parse_next_lcm_dts(const void *fdt, char *lcm_name, bool force);

/* function: update lcm boot up device name into storage,
 * input: lcm_name is target lcm dts name, limitation of 128 char
 * output: 0 for success, else for failed
 */
int mtk_lcm_dsi_set_lcm_boot_env(char *lcm_name);

/* function: read storage settings of lcm boot env,
 * input: the return buffer and size of lcm name
 * output: 0 for success, else for failed
 */
int mtk_lcm_dsi_check_lcm_boot_env(char *lcm_name, unsigned int size);

/* function: get lcm name with dconfig index
 * input: func: DSI/DBI/DPI, id: dconfig index
 * output: 0 for success, else for failed
 */
int mtk_lcm_get_dconfig_lcm_name(char func, unsigned int id, char *lcm_name);
