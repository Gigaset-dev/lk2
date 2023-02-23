/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#include "lcm_drv.h"

int mtk_lcm_drv_parse_lcm_dts(const void *fdt, char func, const char *lcm_name)
{
    int ret = 0;

    switch (func) {
    case MTK_LCM_FUNC_DBI:
        ret = mtk_lcm_dbi_parse_lcm_dts(fdt, lcm_name);
        break;
    case MTK_LCM_FUNC_DPI:
        ret = mtk_lcm_dpi_parse_lcm_dts(fdt, lcm_name);
        break;
    case MTK_LCM_FUNC_DSI:
        ret = mtk_lcm_dsi_parse_lcm_dts(fdt, lcm_name);
        break;
    default:
        ret = -EINVAL;
        break;
    }

    return ret;
}

int mtk_lcm_drv_parse_next_lcm_dts(const void *fdt, char func, char *lcm_name, bool force)
{
    int ret = 0;

    switch (func) {
    case MTK_LCM_FUNC_DBI:
        ret = mtk_lcm_dbi_parse_next_lcm_dts(fdt, lcm_name, force);
        break;
    case MTK_LCM_FUNC_DPI:
        ret = mtk_lcm_dpi_parse_next_lcm_dts(fdt, lcm_name, force);
        break;
    case MTK_LCM_FUNC_DSI:
        ret = mtk_lcm_dsi_parse_next_lcm_dts(fdt, lcm_name, force);
        break;
    default:
        LCM_MSG("%s, invalid func:%d\n", __func__, func);
        ret = -EINVAL;
        break;
    }

    return ret;
}

int mtk_lcm_drv_check_lcm_boot_env(char func, char *lcm_name, unsigned int size)
{
    int ret = 0;

    switch (func) {
    case MTK_LCM_FUNC_DBI:
        break;
    case MTK_LCM_FUNC_DPI:
        break;
    case MTK_LCM_FUNC_DSI:
        ret = mtk_lcm_dsi_check_lcm_boot_env(lcm_name, size);
        break;
    default:
        LCM_MSG("%s, invalid func:%d\n", __func__, func);
        ret = -EINVAL;
        break;
    }

    return ret;
}

int mtk_lcm_drv_set_lcm_boot_env(char func, char *lcm_name)
{
    int ret = 0;

    switch (func) {
    case MTK_LCM_FUNC_DBI:
        break;
    case MTK_LCM_FUNC_DPI:
        break;
    case MTK_LCM_FUNC_DSI:
        ret = mtk_lcm_dsi_set_lcm_boot_env(lcm_name);
        break;
    default:
        LCM_MSG("%s, invalid func:%d\n", __func__, func);
        ret = -EINVAL;
        break;
    }

    return ret;
}

LCM_DRIVER *mtk_lcm_get_common_drv(char func)
{
    LCM_DRIVER *lcm_drv = NULL;

    switch (func) {
    case MTK_LCM_FUNC_DBI:
        lcm_drv = &mtk_lcm_dbi_drv;
        break;
    case MTK_LCM_FUNC_DPI:
        lcm_drv = &mtk_lcm_dpi_drv;
        break;
    case MTK_LCM_FUNC_DSI:
        lcm_drv = &mtk_lcm_dsi_drv;
        break;
    default:
        LCM_MSG("%s, invalid func:%d\n", __func__, func);
        break;
    }

    return lcm_drv;
}

int mtk_lcm_drv_update_panel_for_kernel(char func,
        const char *lcm_name, int phy_type, void *fdt)
{
    int ret = 0;

    if (fdt == NULL) {
        LCM_MSG("%s, invalid fdt, get fdt automatically\n",
            __func__);
        return -EINVAL;
    }

    switch (func) {
    case MTK_LCM_FUNC_DBI:
        ret = mtk_lcm_dbi_update_panel_for_kernel(lcm_name,
                phy_type, fdt);
        break;
    case MTK_LCM_FUNC_DPI:
        ret = mtk_lcm_dpi_update_panel_for_kernel(lcm_name,
                phy_type, fdt);
        break;
    case MTK_LCM_FUNC_DSI:
        ret = mtk_lcm_dsi_update_panel_for_kernel(lcm_name,
                phy_type, fdt);
        break;
    default:
        LCM_MSG("%s, invalid func:%d\n", __func__, func);
        ret = -EINVAL;
        break;
    }

    return ret;
}

int mtk_lcm_drv_get_dconfig(const void *fdt, char func,
        char *lcm_name, unsigned int size)
{
    int ret = 0;

    if (fdt == NULL) {
        LCM_MSG("%s, invalid fdt, get fdt automatically\n",
            __func__);
        return -EFAULT;
    }

    switch (func) {
    case MTK_LCM_FUNC_DBI:
        ret = mtk_lcm_dbi_get_dconfig_panel(fdt, lcm_name, size);
        break;
    case MTK_LCM_FUNC_DPI:
        ret = mtk_lcm_dpi_get_dconfig_panel(fdt, lcm_name, size);
        break;
    case MTK_LCM_FUNC_DSI:
        ret = mtk_lcm_dsi_get_dconfig_panel(fdt, lcm_name, size);
        break;
    default:
        LCM_MSG("%s, invalid func:%d\n", __func__, func);
        ret = -EINVAL;
        break;
    }

    return ret;
}

int mtk_panel_register_drv_customization_callback(char func,
        struct mtk_panel_cust *cust)
{
    int ret = 0;

    switch (func) {
    case MTK_LCM_FUNC_DBI:
        ret = mtk_panel_register_dbi_customization_callback(cust);
        break;
    case MTK_LCM_FUNC_DPI:
        ret = mtk_panel_register_dpi_customization_callback(cust);
        break;
    case MTK_LCM_FUNC_DSI:
        ret = mtk_panel_register_dsi_customization_callback(cust);
        break;
    default:
        LCM_MSG("%s, invalid func:%d\n", __func__, func);
        ret = -EINVAL;
        break;
    }

    return ret;
}
