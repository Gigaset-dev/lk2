/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#include "lcm_drv.h"
#include <sysenv.h>

static struct mtk_panel_context ctx_dpi;
static LCM_UTIL_FUNCS lcm_util_dpi;

int mtk_lcm_dpi_get_dconfig_panel(const void *fdt,
        char *lcm_name, unsigned int size)
{
    return 0;
}

int mtk_panel_register_dpi_customization_callback(
        struct mtk_panel_cust *cust)
{
    struct mtk_panel_cust *target_cust = NULL;

    if (ctx_dpi.panel_resource == NULL) {
        LCM_MALLOC(ctx_dpi.panel_resource, sizeof(struct mtk_panel_resource));
        if (ctx_dpi.panel_resource == NULL) {
            LCM_MSG("%s: failed to allocate panel resource\n", __func__);
            return -ENOMEM;
        }
        memset(ctx_dpi.panel_resource, 0, sizeof(struct mtk_panel_resource));
    }

    target_cust = &ctx_dpi.panel_resource->cust;
    if (target_cust->cust_enabled != 0) {
        LCM_MSG("%s %d: cust callback has already been registered\n",
            __func__, __LINE__);
        return -EINVAL;
    }

    target_cust->parse_params = cust->parse_params;
    target_cust->parse_ops = cust->parse_ops;
    target_cust->func = cust->func;
    target_cust->dump_params = cust->dump_params;
    target_cust->dump_ops = cust->dump_ops;
    target_cust->cust_enabled = 1;

    return 0;
}

int mtk_lcm_dpi_parse_lcm_dts(const void *fdt, const char *lcm_name)
{
    char node[MTK_LCM_NAME_LENGTH] = {0};
    int offset = 0, ret = 0;

    if (fdt == NULL)
        panic("lk driver fdt is NULL!\n");

    if (lcm_name == NULL) {
        LCM_MSG("%s:%d: invalid lcm name\n", __FILE__, __LINE__);
        return -EINVAL;
    }

    LCM_MSG("%s: parse gateic dts\n", __func__);
    ret = mtk_lcm_gateic_drv_init(fdt, MTK_LCM_FUNC_DPI, lcm_name);
    if (ret != 0) {
        LCM_MSG("%s, failed to init gateic driver, ret:%d\n",
            __func__, ret);
        return ret;
    }

    ret = snprintf(node, MTK_LCM_NAME_LENGTH - 1, "mediatek,%s", lcm_name);
    if (ret < 0 || ret >= MTK_LCM_NAME_LENGTH)
        LCM_MSG("%s, snprintf failed, ret:%d\n", __func__, ret);
    LCM_MSG("%s: parse lcm dts: %s\n", __func__, node);
    offset = fdt_node_offset_by_compatible(fdt, -1, node);
    if (offset < 0) {
        LCM_MSG("%s, failed to get lcm dts node:%s, ret:%d\n",
            __func__, node, ret);
        return offset;
    }

    ret = load_panel_resource_from_dts(fdt, offset, ctx_dpi.panel_resource);
    if (ret != 0 || ctx_dpi.panel_resource == NULL) {
        LCM_MSG("%s, failed to load lcm resource from dts, ret:%d\n",
            __func__, ret);
        return ret;
    }
    ctx_dpi.parsing = 1;
    return 0;
}

int mtk_lcm_dpi_parse_next_lcm_dts(const void *fdt, char *lcm_name, bool force)
{
    static unsigned int id;
    const char *temp;
    int ret = 0;

    if (force == true) {
        id = 0;
        ret = mtk_lcm_list_init(MTK_LCM_FUNC_DPI);
        if (ret <= 0) {
            LCM_MSG("%s, failed to init lcm list, ret:%d\n",
                __func__, ret);
            return ret;
        }
    }


retry:
    if (id >= mtk_lcm_get_drv_count(MTK_LCM_FUNC_DPI)) {
        LCM_DBG("%s:no more lcm dts\n", __func__);
        return -ENOMEM;
    }

    ctx_dpi.parsing = 0;
    LCM_FREE(ctx_dpi.panel_resource, sizeof(struct mtk_panel_resource));

    temp = mtk_lcm_get_drv_name(MTK_LCM_FUNC_DPI, id);
    if (temp == NULL) {
        LCM_MSG("%s:failed to get lcm name\n", __func__);
        goto retry;
    }
    LCM_MSG("%s: parse lcm dts:%s, id:%u\n", __func__, temp, id);
    ret = mtk_lcm_dpi_parse_lcm_dts(fdt, temp);
    if (ret != 0) {
        LCM_MSG("%s: failed to parse lcm dts:%s, id:%u, ret:%d\n",
            __func__, temp, id, ret);
        id++;
        goto retry;
    }

    strncpy(lcm_name, temp, (size_t)(MTK_LCM_NAME_LENGTH - 1));
    return id++;
}

int mtk_lcm_dpi_delay(char type, unsigned int time)
{
    switch (type) {
    case MTK_LCM_UTIL_TYPE_MDELAY:
        if (lcm_util_dpi.mdelay)
            lcm_util_dpi.mdelay(time);
        break;
    case MTK_LCM_UTIL_TYPE_UDELAY:
        if (lcm_util_dpi.udelay)
            lcm_util_dpi.udelay(time);
        break;
    default:
        LCM_MSG("[LCM][ERROR] %s: %d\n", __func__, (unsigned int)type);
        return -EFAULT;
    }

    return 0;
}

int mtk_lcm_dpi_update_panel_for_kernel(const char *lcm_name,
        int phy_type, void *fdt)
{
    return 0;
}

LCM_DRIVER mtk_lcm_dpi_drv = {
    .name           = "mediatek,lcm-dpi-common_driver",
};
