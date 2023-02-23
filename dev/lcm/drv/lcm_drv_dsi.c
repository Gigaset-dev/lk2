/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#include "lcm_drv.h"
#include <sysenv.h>

static struct mtk_panel_context ctx_dsi;
static LCM_UTIL_FUNCS lcm_util_dsi;

#define MTK_LCM_DSI_BOOT_UP_DEVICE "mediatek-lcm-dsi-boot-up-device"

int mtk_lcm_dsi_power_reset(int value)
{
    int ret = 0;

    ret = mtk_lcm_gateic_reset(!!value, MTK_LCM_FUNC_DSI);
    if (ret < 0)
        LCM_MSG("%s:failed to reset panel\n",
            __func__);

    return ret;
}

int mtk_lcm_dsi_power_on(void)
{
    int ret = 0;

    LCM_DBG("%s++\n", __func__);

    ret = mtk_lcm_gateic_power_on(MTK_LCM_FUNC_DSI);
    if (ret < 0)
        LCM_MSG("%s, failed to power on, %d\n", __func__, ret);

    LCM_MSG("%s, voltage:%u\n", __func__,
        ctx_dsi.panel_resource->misc.voltage);
    if (ctx_dsi.panel_resource->misc.voltage != 0) {
        ret = mtk_lcm_gateic_set_voltage(
                ctx_dsi.panel_resource->misc.voltage,
                MTK_LCM_FUNC_DSI);
        if (ret != 0)
            LCM_MSG("%s, failed to set gateic voltage:%u, %d\n",
                __func__, ctx_dsi.panel_resource->misc.voltage, ret);
    }

    LCM_MSG("%s--, voltage:%u, %d\n",
        __func__, ctx_dsi.panel_resource->misc.voltage, ret);
    return ret;
}

int mtk_lcm_dsi_power_off(void)
{
    long ret = 0;

    LCM_DBG("%s++\n", __func__);

    ret = mtk_lcm_dsi_power_reset(0);
    if (ret < 0)
        LCM_MSG("%s:failed to reset panel %ld\n",
            __func__, ret);

    ret = mtk_lcm_gateic_power_off(MTK_LCM_FUNC_DSI);

    if (ret < 0)
        LCM_MSG("%s, failed to power off, %ld\n", __func__, ret);

#ifdef LCM_SET_DISPLAY_ON_DELAY
    ctx_dsi.display_on = 0;
#endif
    LCM_DBG("%s--, %ld\n", __func__, ret);
    return ret;
}

int mtk_panel_register_dsi_customization_callback(
        struct mtk_panel_cust *cust)
{
    struct mtk_panel_cust *target_cust = NULL;

    LCM_MSG("%s ++\n", __func__);
    if (ctx_dsi.panel_resource == NULL) {
        LCM_MALLOC(ctx_dsi.panel_resource, sizeof(struct mtk_panel_resource));
        if (ctx_dsi.panel_resource == NULL) {
            LCM_MSG("%s: failed to allocate panel resource\n", __func__);
            return -ENOMEM;
        }
    }

    target_cust = &ctx_dsi.panel_resource->cust;
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
    LCM_MSG("%s --\n", __func__);

    return 0;
}

int mtk_lcm_dsi_get_dconfig_panel(const void *fdt,
        char *lcm_name, unsigned int size)
{
    int ret = 0, nodeoffset = 0;
    unsigned int dconfig_id = 0;
    char lcm[MTK_LCM_NAME_LENGTH] = { 0 };
    char buf[MTK_LCM_NAME_LENGTH];

    if (lcm_name == NULL || size == 0 || fdt == NULL) {
        LCM_MSG("%s: invalid parameter\n", __func__);
        return -EINVAL;
    }

    ret = snprintf(buf, MTK_LCM_NAME_LENGTH - 1, "/soc/dsi");
    if (ret < 0 || ret >= MTK_LCM_NAME_LENGTH)
        LCM_MSG("%s, snprintf failed, ret:%d\n", __func__, ret);
    nodeoffset = fdt_path_offset(fdt, buf);
    if (nodeoffset < 0) {
        ret = snprintf(buf, MTK_LCM_NAME_LENGTH - 1, "/dsi");
        if (ret < 0 || ret >= MTK_LCM_NAME_LENGTH)
            LCM_MSG("%s, snprintf failed, ret:%d\n", __func__, ret);
    }

    nodeoffset = fdt_path_offset(fdt, buf);
    if (nodeoffset < 0) {
        LCM_MSG("[%s] get dsi node failed, buf:%s, %d\n",
            __func__, buf, nodeoffset);
        return -EFAULT;
    }

    ret = lcm_fdt_getprop_u32(fdt, nodeoffset,
        "panel_dconfig", &dconfig_id);
    if (ret < 0) {
        LCM_MSG("[%s] get dconfig failed, nodeoffset:%u, ret:%d\n",
            __func__, nodeoffset, ret);
        return -EEXIST;
    }

    ret = mtk_lcm_get_dconfig_lcm_name(MTK_LCM_FUNC_DSI,
                dconfig_id, &lcm[0]);
    if (ret < 0 || strlen(lcm) == 0 ||
        strcmp(lcm, "<null>") == 0) {
        LCM_MSG("%s, %d invalid id:%u, ret:%d\n",
            __func__, __LINE__, dconfig_id, ret);
        return -EINVAL;
    }
    if (size > MTK_LCM_NAME_LENGTH)
        size = MTK_LCM_NAME_LENGTH;

    ret = snprintf(lcm_name, size - 1, "%s", lcm);
    if (ret < 0 || (unsigned int)ret >= size)
        LCM_MSG("%s, snprintf failed, ret:%d\n", __func__, ret);

    lcm_name[size - 1] = '\0';
    LCM_MSG("%s: %d, dconfig id:%u, lcm:\"%s\"\n",
        __func__, __LINE__, dconfig_id, lcm_name);

    return 0;
}

int mtk_lcm_dsi_set_lcm_boot_env(char *lcm_name)
{
    int ret = 0;

    if (lcm_name == NULL || strlen(lcm_name) >= MTK_LCM_NAME_LENGTH) {
        LCM_MSG("%s: invalid parameter\n", __func__);
        return -EINVAL;
    }
    ret = set_env((char *)MTK_LCM_DSI_BOOT_UP_DEVICE, lcm_name);
    if (ret < 0) {
        LCM_MSG("%s: failed to set lcm boot env:%s, %d\n",
            __func__, lcm_name, ret);
        return ret;
    }

    LCM_MSG("%s: update lcm boot env:%s\n", __func__, lcm_name);
    return 0;
}

int mtk_lcm_dsi_check_lcm_boot_env(char *lcm_name, unsigned int size)
{
    int ret = 0;
    if (lcm_name == NULL || size == 0) {
        LCM_MSG("%s: invalid parameter\n", __func__);
        return -EINVAL;
    }

    ret = snprintf(lcm_name, MTK_LCM_NAME_LENGTH - 1, "%s",
        get_env((char *)MTK_LCM_DSI_BOOT_UP_DEVICE));
    if (ret < 0 || ret >= MTK_LCM_NAME_LENGTH)
        LCM_MSG("%s, snprintf failed, ret:%d\n", __func__, ret);

    if (lcm_name == NULL || strlen(lcm_name) == 0 ||
        strcmp(lcm_name, "<null>") == 0) {
        if (strlen(lcm_name) > 0)
            lcm_name[0] = '\0';
        return -EEXIST;
     }
    lcm_name[size - 1] = '\0';
    LCM_DBG("%s: get lcm boot env: \"%s\", size:%u\n",
        __func__, lcm_name, size);

    return 0;
}

static int mtk_lcm_dsi_update_panel_resource(const char *lcm_name,
        void *fdt)
{
    char src_name[MTK_LCM_NAME_LENGTH] = {0};
    int ret = 0, target_offset = 0, src_offset = 0;

    ret = snprintf(&src_name[0], MTK_LCM_NAME_LENGTH - 1, "mediatek,%s", lcm_name);
    if (ret < 0 || ret >= MTK_LCM_NAME_LENGTH)
        LCM_MSG("%s, snprintf failed, ret:%d\n", __func__, ret);
    LCM_MSG("[%s] src:\"%s\", lcm:\"%s\"\n",
        __func__, src_name, lcm_name);

    src_offset = fdt_node_offset_by_compatible(fdt, -1, src_name);
    if (src_offset <= 0) {
        LCM_MSG("[%s] get panel node of \"%s\"failed,%d\n",
            __func__, src_name, src_offset);
        return -EINVAL;
    }
    target_offset = fdt_node_offset_by_compatible(fdt, -1,
            "mediatek,mtk-drm-panel-drv-dsi");
    if (target_offset < 0) {
        LCM_MSG("[%s] get panel node failed,%d\n",
            __func__, target_offset);
        return -EFAULT;
    }

    ret = lcm_fdt_update_dts_prop(fdt, src_offset,
            target_offset, "panel_resource");
    if (ret)
        LCM_MSG("%s, failed to update panel_resource:\"%s\", %d\n",
            __func__, src_name, ret);

    return ret;
}

static int mtk_lcm_dsi_update_phy_type(int phy_type,
        void *fdt)
{
    char phy_name[MTK_LCM_NAME_LENGTH] = {0};
    int ret = 0, target_offset = 0;

    switch (phy_type) {
    case MTK_LCM_MIPI_DPHY:
        ret = snprintf(phy_name, sizeof(phy_name) - 1,
            "mediatek,%s-mipi-tx", SUB_PLATFORM);
        break;
    case MTK_LCM_MIPI_CPHY:
        ret = snprintf(phy_name, sizeof(phy_name) - 1,
            "mediatek,%s-mipi-tx-cphy", SUB_PLATFORM);
        break;
    default:
        LCM_MSG("%s, invalid phy type:%d\n", __func__, phy_type);
        return -EINVAL;
    }

    if (ret < 0 || (size_t)ret >= sizeof(phy_name))
        LCM_MSG("%s, snprintf failed, ret:%d\n", __func__, ret);
    if (strlen(phy_name) > 0) {
        LCM_MSG("%s, %d, phy type:%d, name:%s\n",
            __func__, __LINE__, phy_type, phy_name);
        target_offset = fdt_node_offset_by_compatible(fdt, -1,
                "mediatek,mipi_tx_config0");
        if (target_offset < 0) {
            LCM_MSG("[%s] get mipi0 node, failed,%d\n",
                __func__, target_offset);
            return -EFAULT;
        }
        ret = lcm_fdt_update_dts_string(fdt, target_offset,
                "compatible", phy_name);
        if (ret)
            LCM_MSG("%s, failed to update phy name:%s of mipi0, %d\n",
                __func__, phy_name, ret);

        target_offset = fdt_node_offset_by_compatible(fdt, -1,
                "mediatek,mipi_tx_config1");
        if (target_offset < 0) {
            LCM_MSG("[%s] get mipi1 node, failed,%d\n",
                __func__, target_offset);
            return 0; //mipi1 may not existed
        }
        ret = lcm_fdt_update_dts_string(fdt, target_offset,
                "compatible", phy_name);
        if (ret)
            LCM_MSG("%s, failed to update phy name:%s of mipi1, %d\n",
                __func__, phy_name, ret);
    }

    return 0;
}

static int mtk_lcm_dsi_update_touch_info(const char *lcm_name,
        void *fdt)
{
    char touch_name[MTK_LCM_NAME_LENGTH] = {0};
    int ret = 0, target_offset = 0;

    ret = snprintf(touch_name, sizeof(touch_name) - 1,
        "mediatek,touch-panel");
    if (ret < 0 || (size_t)ret >= sizeof(touch_name))
        LCM_MSG("%s, snprintf failed, ret:%d\n", __func__, ret);
    target_offset = fdt_node_offset_by_compatible(fdt, -1,
            touch_name);
    if (target_offset < 0) {
        LCM_MSG("[%s] get touch node failed,%d, %s\n",
            __func__, target_offset, touch_name);
        return -EFAULT;
    }

    LCM_MSG("[%s] touch:%s\n", __func__, touch_name);

    ret = lcm_fdt_update_dts_value(fdt, target_offset, "lcm-height",
            ctx_dsi.panel_resource->params.height);
    if (ret)
        LCM_MSG("%s, %d, failed to update touch node:\"%s\", %d\n",
            __func__, __LINE__, touch_name, ret);

    ret = lcm_fdt_update_dts_value(fdt, target_offset, "lcm-width",
            ctx_dsi.panel_resource->params.width);
    if (ret)
        LCM_MSG("%s, %d, failed to update touch node:\"%s\", %d\n",
            __func__, __LINE__, touch_name, ret);

    ret = lcm_fdt_update_dts_value(fdt, target_offset, "lcm-is-fake",
            ctx_dsi.panel_resource->misc.is_fake);
    if (ret)
        LCM_MSG("%s, %d, failed to update touch node:\"%s\", %d\n",
            __func__, __LINE__, touch_name, ret);

    ret = lcm_fdt_update_dts_value(fdt, target_offset, "lcm-fake-height",
            ctx_dsi.panel_resource->misc.fake_height);
    if (ret)
        LCM_MSG("%s, %d, failed to update touch node:\"%s\", %d\n",
            __func__, __LINE__, touch_name, ret);

    ret = lcm_fdt_update_dts_value(fdt, target_offset, "lcm-fake-width",
            ctx_dsi.panel_resource->misc.fake_width);
    if (ret)
        LCM_MSG("%s, %d, failed to update touch node:\"%s\", %d\n",
            __func__, __LINE__, touch_name, ret);

    ret = lcm_fdt_update_dts_string(fdt, target_offset,
            "lcm-name", lcm_name);
    if (ret)
        LCM_MSG("%s, %d, failed to update touch node:\"%s\", %d\n",
            __func__, __LINE__, touch_name, ret);
    return ret;
}

int mtk_lcm_dsi_update_panel_for_kernel(const char *lcm_name,
        int phy_type, void *fdt)
{
    if (fdt == NULL || lcm_name == NULL) {
        LCM_MSG("%s, invalid parameters\n",
            __func__);
        return -EFAULT;
    }

    mtk_lcm_dsi_update_panel_resource(lcm_name, fdt);
    mtk_lcm_dsi_update_phy_type(phy_type, fdt);
    mtk_lcm_dsi_update_touch_info(lcm_name, fdt);

    return 0;
}

int mtk_lcm_dsi_parse_lcm_dts(const void *fdt, const char *lcm_name)
{
    char node[MTK_LCM_NAME_LENGTH] = {0};
    int offset = 0, ret = 0;
    lk_time_t start = current_time();

    if (fdt == NULL)
        panic("lk driver fdt is NULL!\n");

    if (lcm_name == NULL) {
        LCM_MSG("%s:%d: invalid lcm name\n", __FILE__, __LINE__);
        return -EINVAL;
    }

    LCM_DBG("%s ++, \"%s\"\n", __func__, lcm_name);
    if (ctx_dsi.parsing == 1) {
        free_lcm_resource(MTK_LCM_FUNC_DSI, ctx_dsi.panel_resource);
        ctx_dsi.panel_resource = NULL;
        LCM_MSG("%s, free previous lcm:\"%s\", total_size:%lluByte\n",
            __func__, ctx_dsi.lcm_name,
            mtk_lcm_total_size);
        memset(&ctx_dsi.lcm_name[0], 0, MTK_LCM_NAME_LENGTH);
        ctx_dsi.parsing = 0;
    }

    ret = mtk_lcm_gateic_drv_init(fdt, MTK_LCM_FUNC_DSI, lcm_name);
    if (ret < 0) {
        LCM_MSG("%s, failed to init gateic driver, ret:%d\n",
            __func__, ret);
        return ret;
    }

    ret = snprintf(node, MTK_LCM_NAME_LENGTH - 1, "mediatek,%s", lcm_name);
    if (ret < 0 || ret >= MTK_LCM_NAME_LENGTH)
        LCM_MSG("%s, snprintf failed, ret:%d\n", __func__, ret);
    offset = fdt_node_offset_by_compatible(fdt, -1, node);
    if (offset < 0) {
        LCM_MSG("%s, failed to get lcm dts node:%s, ret:%d\n",
            __func__, node, ret);
        return offset;
    }

    if (ctx_dsi.panel_resource == NULL) {
        LCM_MALLOC(ctx_dsi.panel_resource, sizeof(struct mtk_panel_resource));
        if (ctx_dsi.panel_resource == NULL) {
            LCM_MSG("%s: failed to allocate panel resource\n", __func__);
            return -ENOMEM;
        }
    }
    ret = load_panel_resource_from_dts(fdt, offset, ctx_dsi.panel_resource);
    if (ret < 0) {
        LCM_MSG("%s, failed to load lcm resource from dts, ret:%d\n",
            __func__, ret);
        if (ctx_dsi.panel_resource != NULL) {
            free_lcm_resource(MTK_LCM_FUNC_DSI, ctx_dsi.panel_resource);
            ctx_dsi.panel_resource = NULL;
            LCM_MSG("%s: free previous panel resource, total_size:%llu\n",
                __func__, mtk_lcm_total_size);
        }
        return ret;
    }

    ctx_dsi.parsing = 1;
    ret = snprintf(&ctx_dsi.lcm_name[0], MTK_LCM_NAME_LENGTH - 1, "%s", lcm_name);
    if (ret < 0 || ret >= MTK_LCM_NAME_LENGTH)
        LCM_MSG("%s, snprintf failed, ret:%d\n", __func__, ret);
    ctx_dsi.lcm_name[MTK_LCM_NAME_LENGTH - 1] = '\0';

    LCM_MSG("%s:\"%s\" parsing done, total_size:%lluByte, cost:%dms\n", __func__, lcm_name,
            mtk_lcm_total_size, current_time() - start);
    return 0;
}

int mtk_lcm_dsi_parse_next_lcm_dts(const void *fdt, char *lcm_name, bool force)
{
    static unsigned int id;
    const char *temp;
    int ret = 0;

    if (force == true) {
        id = 0;
        ret = mtk_lcm_list_init(MTK_LCM_FUNC_DSI);
        if (ret <= 0) {
            LCM_MSG("%s, failed to init lcm list, ret:%d\n",
                __func__, ret);
            return ret;
        }
    }

retry:
    LCM_DBG("%s, id:%u, count:%u\n", __func__, id,
        mtk_lcm_get_drv_count(MTK_LCM_FUNC_DSI));
    if (id >= mtk_lcm_get_drv_count(MTK_LCM_FUNC_DSI)) {
        LCM_DBG("%s:no more lcm dts\n", __func__);
        return -ENOMEM;
    }

    if (ctx_dsi.panel_resource != NULL) {
        free_lcm_resource(MTK_LCM_FUNC_DSI, ctx_dsi.panel_resource);
        ctx_dsi.panel_resource = NULL;
        LCM_MSG("%s: free previous panel resource, total_size:%llu\n",
            __func__, mtk_lcm_total_size);
    }
    memset(&ctx_dsi.lcm_name[0], 0, MTK_LCM_NAME_LENGTH);
    ctx_dsi.parsing = 0;

    temp = mtk_lcm_get_drv_name(MTK_LCM_FUNC_DSI, id);
    if (temp == NULL) {
        LCM_MSG("%s:failed to get lcm name\n", __func__);
        id++;
        goto retry;
    }
    LCM_MSG("%s: parse lcm dts:\"%s\", id:%u\n", __func__, temp, id);
    ret = mtk_lcm_dsi_parse_lcm_dts(fdt, temp);
    if (ret < 0) {
        LCM_MSG("%s: failed to parse lcm dts:%s, id:%u, ret:%d\n",
            __func__, temp, id, ret);
        id++;
        goto retry;
    }

    if (lcm_name != NULL) {
        strncpy(lcm_name, temp, (size_t)(MTK_LCM_NAME_LENGTH - 1));
        LCM_DBG("%s, %d: lcm%u \"%s\" parsing done, total_size:%lluByte\n",
        __func__, __LINE__, id, lcm_name,
        mtk_lcm_total_size);
    } else {
        LCM_MSG("%s, %d: lcm \"%s\" parsing done without return\n",
        __func__, __LINE__, temp);
    }

    id++;
    return id - 1;
}

int mtk_lcm_dsi_delay(char type, unsigned int time)
{
    switch (type) {
    case MTK_LCM_UTIL_TYPE_MDELAY:
        if (lcm_util_dsi.mdelay)
            lcm_util_dsi.mdelay(time);
        break;
    case MTK_LCM_UTIL_TYPE_UDELAY:
        if (lcm_util_dsi.udelay)
            lcm_util_dsi.udelay(time);
        break;
    default:
        LCM_MSG("[LCM][ERROR] %s: %d\n", __func__, (unsigned int)type);
        return -EFAULT;
    }

    return 0;
}

int mtk_panel_dsi_dcs_write_lk(unsigned int *data,
        unsigned int data_len, unsigned char force_update)
{
    if (data == NULL) {
        LCM_MSG("[LCM][ERROR] %s: invalid data, len:%u\n",
            __func__, data_len);
        return -EFAULT;
    }

    if (lcm_util_dsi.dsi_set_cmdq == NULL) {
        LCM_MSG("[LCM][ERROR] %s: invalid util\n",
            __func__);
        return -EFAULT;
    }

    lcm_util_dsi.dsi_set_cmdq(data, (int)data_len, force_update);

    return 0;
}

int mtk_panel_dsi_dcs_write_buffer_runtime(unsigned char *data, unsigned int size,
        unsigned char force_update)
{
    if (data == NULL || size == 0) {
        LCM_MSG("[LCM][ERROR] %s: invalid data, size:%u\n",
            __func__, size);
        return -EFAULT;
    }

    if (lcm_util_dsi.dsi_set_cmdq_V2 == NULL)
        return -EFAULT;

    lcm_util_dsi.dsi_set_cmdq_V2(data[0],
            (unsigned char)(size - 1), &data[1], force_update);
    return 0;
}

int mtk_panel_dsi_dcs_write_buffer(struct mtk_lcm_dcs_cmd_data *data,
        unsigned char force_update)
{
    if (data == NULL) {
        LCM_MSG("[LCM][ERROR] %s: invalid data\n", __func__);
        return -EFAULT;
    }

    if (lcm_util_dsi.dsi_set_cmdq_V2 == NULL)
        return -EFAULT;

    lcm_util_dsi.dsi_set_cmdq_V2((unsigned char)data->cmd,
                (unsigned char)data->data_len,
                (unsigned char *)data->data, force_update);
    return 0;
}

int mtk_panel_dsi_dcs_read_buffer(struct mtk_lcm_dcs_cmd_data *data)
{
    if (data == NULL) {
        LCM_MSG("[LCM][ERROR] %s: invalid data\n", __func__);
        return -EFAULT;
    }

    if (lcm_util_dsi.dsi_dcs_read_lcm_reg_v2 == NULL)
        return -EFAULT;

    lcm_util_dsi.dsi_dcs_read_lcm_reg_v2((unsigned char)data->cmd,
                data->data, data->data_len);
    return 0;
}

static void mtk_lcm_drv_get_params(LCM_PARAMS *params)
{
    if (params == NULL) {
        LCM_MSG("[LCM][ERROR] %s: NULL parameter\n", __func__);
        return;
    }

    if (ctx_dsi.parsing == 0) {
        LCM_MSG("[LCM][ERROR] %s: DTS is not parsed\n", __func__);
        return;
    }

    memset(params, 0, sizeof(LCM_PARAMS));
    memcpy(params, &(ctx_dsi.panel_resource->params), sizeof(LCM_PARAMS));
}

static void mtk_lcm_drv_init(void *fdt)
{
    struct mtk_lcm_ops_dsi *ops = ctx_dsi.panel_resource->ops.dsi_ops;
    int ret = 0;

    LCM_MSG("%s++\n", __func__);
    if (ctx_dsi.enabled != 0)
        return;

    if (ops == NULL)
        return;

    ret = mtk_lcm_gateic_power_on(MTK_LCM_FUNC_DSI);
    if (ret < 0) {
        LCM_MSG("%s, gate ic power on failed, %d\n",
            __func__, ret);
        return;
    }

    ret = mtk_panel_execute_operation(&ops->prepare,
            ctx_dsi.panel_resource,
            NULL, 0, "panel_prepare");

    if (ret < 0)
        LCM_MSG("%s,%d: failed to do panel prepare, %d\n",
            __func__, __LINE__, ret);

    ctx_dsi.enabled = 1;
#ifdef LCM_SET_DISPLAY_ON_DELAY
    ctx_dsi.lcm_init_tick = current_time();
    ctx_dsi.display_on = 0;
#endif
    LCM_MSG("%s--, tick:%u\n", __func__, ctx_dsi.lcm_init_tick);
}

static void mtk_lcm_drv_suspend(void)
{
    struct mtk_lcm_ops_dsi *ops = ctx_dsi.panel_resource->ops.dsi_ops;
    int ret = 0;

    if (ctx_dsi.parsing == 0) {
        LCM_MSG("[LCM][ERROR] %s: DTS is not parsed\n", __func__);
        return;
    }

    if (ctx_dsi.enabled == 0 || ops == NULL) {
        LCM_MSG("[LCM][ERROR] %s: didn't enabled\n", __func__);
        return;
    }

    LCM_DBG("%s++\n", __func__);
    ret = mtk_panel_execute_operation(&ops->unprepare,
            ctx_dsi.panel_resource,
            NULL, 0, "panel_unprepare");

    if (ret < 0) {
        LCM_MSG("%s,%d: failed to do panel unprepare, %d\n",
            __func__, __LINE__, ret);
        return;
    }

    ctx_dsi.enabled = 0;
    ret = mtk_lcm_dsi_power_reset(0);
    if (ret < 0) {
        LCM_MSG("%s:failed to reset panel %d\n",
            __func__, ret);
        return;
    }
    LCM_DBG("%s--, %d\n", __func__, ret);
}

static void mtk_lcm_drv_resume(void)
{
    LCM_DBG("%s++\n", __func__);
    //mtk_lcm_drv_init(NULL);
    LCM_DBG("%s--\n", __func__);
}

static void mtk_lcm_drv_init_power(void *fdt)
{
    mtk_lcm_dsi_power_on();
}

static void mtk_lcm_drv_suspend_power(void)
{
    mtk_lcm_dsi_power_off();
}

static void mtk_lcm_drv_resume_power(void)
{
    int ret = 0;

    LCM_DBG("%s++\n", __func__);
    ret = mtk_lcm_dsi_power_reset(0);
    if (ret < 0) {
        LCM_MSG("%s:failed to reset panel %d\n",
            __func__, ret);
        return;
    }

    mtk_lcm_dsi_power_on();

#ifdef LCM_SET_DISPLAY_ON_DELAY
    ctx_dsi.display_on = 0;
#endif
    LCM_DBG("%s--, %d\n", __func__, ret);
}

#ifdef LCM_SET_DISPLAY_ON_DELAY
static int lcm_set_display_on(void)
{
    struct mtk_lcm_ops_dsi *ops = ctx_dsi.panel_resource->ops.dsi_ops;
    int ret = 0;

    if (ctx_dsi.display_on == 1)
        return 0;

    LCM_DBG("%s++\n", __func__);
    if (ops == NULL)
        return -EINVAL;

    ret = mtk_panel_execute_operation(&ops->set_display_on,
            ctx_dsi.panel_resource,
            &ctx_dsi.lcm_init_tick, 0, "set_display_on");

    if (ret < 0) {
        LCM_MSG("%s,%d: failed to do panel set_display_on, %d\n",
            __func__, __LINE__, ret);
        return ret;
    }

    ctx_dsi.display_on = 1;
    LCM_MSG("%s--, %d\n", __func__, ret);
    return 0;
}
#endif

static void mtk_lcm_drv_update(unsigned int x, unsigned int y,
                       unsigned int width, unsigned int height)
{
    struct mtk_lcm_ops_dsi *ops = ctx_dsi.panel_resource->ops.dsi_ops;
    int ret = 0;
    struct mtk_lcm_location locate;
    unsigned int x0 = x;
    unsigned int y0 = y;
    unsigned int x1 = x0 + width - 1;
    unsigned int y1 = y0 + height - 1;

    LCM_DBG("%s++, x0:0x%x, y0:0x%x, width:%u, height:%u, x1:0x%x, y1:0x%x\n",
         __func__, x0, y0, width, height, x1, y1);
    locate.x0.msb = ((x0>>8)&0xFF);
    locate.x0.lsb = (x0&0xFF);
    locate.x1.msb = ((x1>>8)&0xFF);
    locate.x1.lsb = (x1&0xFF);
    locate.y0.msb = ((y0>>8)&0xFF);
    locate.y0.lsb = (y0&0xFF);
    locate.y1.msb = ((y1>>8)&0xFF);
    locate.y1.lsb = (y1&0xFF);

    if (ctx_dsi.parsing == 0) {
        LCM_MSG("[LCM][ERROR] %s: DTS is not parsed\n", __func__);
        return;
    }

#ifdef LCM_SET_DISPLAY_ON_DELAY
    lcm_set_display_on();
#endif

    ret = mtk_panel_execute_operation(&ops->lcm_update,
            ctx_dsi.panel_resource,
            &locate, 0, "lcm_update");

    if (ret < 0) {
        LCM_MSG("%s,%d: failed to do panel set_display_on, %d\n",
            __func__, __LINE__, ret);
        return;
    }

    LCM_DBG("%s--\n", __func__);
}

static int do_panel_set_backlight(unsigned int level, struct mtk_lcm_ops_table *table)
{
    u8 *data = NULL;
    struct mtk_lcm_ops_data *op = NULL;
    unsigned int id = -1, j = 0;
    size_t size = 0, count = 0;
    unsigned int *mask = NULL;

    list_for_every_entry(&table->list, op, struct mtk_lcm_ops_data, node) {
        data = op->param.cb_id_data.buffer_data;
        size = op->param.cb_id_data.data_count;
        count = op->param.cb_id_data.id_count;
        LCM_MALLOC(mask, count);
        if (mask == NULL) {
            LCM_MSG("%s:failed to allocate mask buffer, count:%u\n",
                __func__, (unsigned int)count);
            return -ENOMEM;
        }

        for (j = 0; j < count; j++) {
            id = op->param.cb_id_data.id[j];
            if (id >= size) {
                LCM_MSG("%s:invalid backlight level id:%u of table:%u\n",
                    __func__, id, (unsigned int)size);
                LCM_FREE(mask, count);
                return -EINVAL;
            }

            mask[j] = data[id]; //backup backlight mask
            data[id] &= (level >> (8 * (count - j - 1)));
        }

        mtk_panel_dsi_dcs_write_buffer_runtime(data, size, 1);

        /* restore backlight mask*/
        for (j = 0; j < count; j++) {
            id = op->param.cb_id_data.id[j];
            if (id >= size)
                continue;
            data[id] = mask[j];
        }
        LCM_FREE(mask, count);

    }

    return 0;
}

static int panel_set_backlight(unsigned int level, struct mtk_lcm_ops_table *table,
    unsigned int mode)
{
    if (level > 255)
        level = 255;

    if (table == NULL || table->size == 0)
        return 0;

    return  do_panel_set_backlight(level * mode / 255, table);
}

static void mtk_lcm_drv_setbacklight(unsigned int level)
{
    struct mtk_lcm_ops_dsi *ops = NULL;
    struct mtk_lcm_ops_table *table = NULL;
    unsigned int mode = 0;
    int ret = 0;

    LCM_DBG("%s+ level:%u\n", __func__, level);
    if (ctx_dsi.panel_resource == NULL ||
        ctx_dsi.panel_resource->ops.dsi_ops == NULL) {
        LCM_MSG("%s, invalid ctx or panel resource\n", __func__);
        return;
    }

    ops = ctx_dsi.panel_resource->ops.dsi_ops;
    if (ops == NULL ||
        ops->set_backlight_cmdq.size == 0)
        return;

    table = &ops->set_backlight_cmdq;
    mode = ops->set_backlight_mask;

    ret = panel_set_backlight(level, table, mode);

    LCM_MSG("%s-, level:%u, mode:%u, ret:%d\n",
        __func__, level, mode, ret);
    return;
}

static unsigned int mtk_lcm_drv_compare_id(void *fdt)
{
    struct mtk_lcm_ops_dsi *ops = ctx_dsi.panel_resource->ops.dsi_ops;
    int ret = 1;
    unsigned int i = 0;
    u8 *id = NULL;

    LCM_DBG("%s++\n", __func__);
    if (ctx_dsi.parsing == 0) {
        LCM_MSG("[LCM][ERROR] %s: panel is not initialized\n", __func__);
        return 0;
    }

    if (ops->compare_id_value_length == 0) {
        LCM_MSG("[LCM][WARN] %s: no compare id value\n", __func__);
        return 0;
    }

    LCM_MALLOC(id, ops->compare_id_value_length);
    if (id == NULL) {
        LCM_MSG("%s,%d: failed to allocate data\n",
            __func__, __LINE__);
        return 0;
    }
    ret = mtk_panel_execute_operation(&ops->compare_id,
            ctx_dsi.panel_resource,
            id, ops->compare_id_value_length, "compare_id");

    if (ret < 0) {
        LCM_MSG("%s,%d: failed to get compare id, %d\n",
            __func__, __LINE__, ret);
        ret = 0;
        goto end;
    }

    for (i = 0; i < ops->compare_id_value_length; i++) {
        if (id[i] != ops->compare_id_value_data[i]) {
            LCM_MSG("%s: failed, i:%d, id:0x%x, exepct:0x%x\n",
                __func__, i, id[i], ops->compare_id_value_data[i]);
            ret = 0;
            goto end;
        }
    }
    ret = 1;

end:
    LCM_FREE(id, ops->compare_id_value_length);

    LCM_DBG("%s--, ret:%d\n", __func__, ret);
    return ret;
}

static void mtk_lcm_drv_set_util_funcs(const LCM_UTIL_FUNCS *util)
{
    memcpy(&lcm_util_dsi, util, sizeof(LCM_UTIL_FUNCS));
}

static unsigned int mtk_lcm_drv_ata_check(unsigned char *buffer)
{
    LCM_DBG("%s++\n", __func__);
    return 1;
    LCM_DBG("%s--\n", __func__);
}

LCM_DRIVER mtk_lcm_dsi_drv = {
    .name           = "mediatek,lcm-dsi-common_driver",
    .set_util_funcs = mtk_lcm_drv_set_util_funcs,
    .get_params     = mtk_lcm_drv_get_params,
    .init           = mtk_lcm_drv_init,
    .suspend        = mtk_lcm_drv_suspend,
    .resume         = mtk_lcm_drv_resume,
    .init_power     = mtk_lcm_drv_init_power,
    .resume_power   = mtk_lcm_drv_resume_power,
    .suspend_power  = mtk_lcm_drv_suspend_power,
    .compare_id = mtk_lcm_drv_compare_id,
    .set_backlight  = mtk_lcm_drv_setbacklight,
    .update         = mtk_lcm_drv_update,
    .ata_check = mtk_lcm_drv_ata_check,
};
