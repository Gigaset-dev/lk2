/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#include "lcm_drv.h"

static char lcm_dbi_dconfig_list[][MTK_LCM_NAME_LENGTH] = {
    "<null>", //0
};

static char lcm_dpi_dconfig_list[][MTK_LCM_NAME_LENGTH] = {
    "<null>", //0
};

static char lcm_dsi_dconfig_list[][MTK_LCM_NAME_LENGTH] = {
    "<null>", //0
    "td4330_fhdp_dphy_vdo_truly", //1
    "td4330_fhdp_dphy_cmd_truly", //2
    "ft8756_fhdp_dphy_vdo_truly", //3
    "nt36672c_fhdp_dphy_vdo_shenchao_120hz", //4
    "nt36672c_fhdp_dphy_vdo_tianma_120hz", //5
    "nt36672c_fhdp_dphy_vdo_tianma_144hz", //6
    "nt36672e_fhdp_dphy_vdo_tianma_120hz_hfp", //7
    "nt36672e_fhdp_dphy_vdo_jdi_120hz", //8
    "nt36672e_fhdp_dphy_vdo_jdi_144hz", //9
    "nt36672e_fhdp_dphy_vdo_jdi_60hz", //10
    "nt36672e_fhdp_cphy_vdo_jdi_120hz", //11
    "r66451_fhdp_dphy_cmd_tianma_120hz", //12
    "r66451_fhdp_dphy_cmd_tianma_120hz_wa", //13
    "nt37701_fhdp_dphy_cmd_hx_120hz", //14
    "ana6705_wqhd_dphy_cmd_samsung_120hz", //15
    "ana6705_wqhd_dphy_cmd_samsung_120hz_msync20", //16
};

int do_get_dconfig_name(char *table, unsigned int size,
        unsigned int id, char *lcm_name)
{
    if (table == NULL || size <= id)
        return -1;

    snprintf(lcm_name, MTK_LCM_NAME_LENGTH - 1,
        "%s", table + id * MTK_LCM_NAME_LENGTH);
    lcm_name[MTK_LCM_NAME_LENGTH - 1] = '\0';

    return 0;
}

int mtk_lcm_get_dconfig_lcm_name(char func, unsigned int id, char *lcm_name)
{
    int ret = 0;

    if (id == 0)
        return -1;

    switch (func) {
    case MTK_LCM_FUNC_DBI:
        ret = do_get_dconfig_name(
                lcm_dbi_dconfig_list[0],
                sizeof(lcm_dbi_dconfig_list) / MTK_LCM_NAME_LENGTH,
                id, lcm_name);
        break;
    case MTK_LCM_FUNC_DPI:
        ret = do_get_dconfig_name(
                lcm_dpi_dconfig_list[0],
                sizeof(lcm_dpi_dconfig_list) / MTK_LCM_NAME_LENGTH,
                id, lcm_name);
        break;
    case MTK_LCM_FUNC_DSI:
        ret = do_get_dconfig_name(
                lcm_dsi_dconfig_list[0],
                sizeof(lcm_dsi_dconfig_list) / MTK_LCM_NAME_LENGTH,
                id, lcm_name);
        break;
    default:
        ret = -1;
        break;
    }

    return ret;
}

