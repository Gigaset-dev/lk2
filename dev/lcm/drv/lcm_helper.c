/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#include "lcm_helper.h"

unsigned long long mtk_lcm_total_size;

int lcm_fdt_getprop_u8(const void *fdt, int nodeoffset,
        const char *name, unsigned char *out_value)
{
    int len = 0;
    const unsigned int *data;

    if (fdt == NULL || name == NULL || out_value == NULL) {
        LCM_MSG("%s: invalid parameters\n", __func__);
        return -EINVAL;
    }

    data = fdt_getprop(fdt, nodeoffset, name, &len);
    if (len > 0 && data != NULL) {
        len /= 4;
        *out_value = (unsigned char)fdt32_to_cpu(*data);
    } else if (len == 0) {
        *out_value = 0;
    } else {
        *out_value = 0;
#if MTK_LCM_DEBUG_DUMP
        LCM_MSG("%s:%d-\"%s\" cannot find, len:%d\n",
            __func__, nodeoffset, name, len);
#endif
    }

    return len;
}

int lcm_fdt_getprop_u32(const void *fdt, int nodeoffset,
        const char *name, unsigned int *out_value)
{
    int len = 0;
    const unsigned int *data;

    if (fdt == NULL || name == NULL || out_value == NULL) {
        LCM_MSG("%s: invalid parameters\n", __func__);
        return -EINVAL;
    }

    data = fdt_getprop(fdt, nodeoffset, name, &len);
    if (len > 0 && data != NULL) {
        len /= 4;
        *out_value = fdt32_to_cpu(*data);
    } else {
        *out_value = 0;
    }

    return len;
}

/*parsing from u32 array, and backup the parsing result*/
int lcm_fdt_getprop_u8_array_from_u32(const void *fdt, int nodeoffset,
        const char *name, unsigned char *out_value, unsigned int out_size)
{
    const unsigned int *data;
    int len = 0, i = 0;

    if (fdt == NULL || name == NULL ||
        out_value == NULL || out_size == 0) {
        LCM_MSG("%s: invalid parameters\n", __func__);
        return -EINVAL;
    }

    data = fdt_getprop(fdt, nodeoffset, name, &len);
    if (len > 0 && data != NULL) {
        len /= 4;
        if ((unsigned int)len > out_size) {
            LCM_MSG("%s: len:%d is out of size:%u\n",
                __func__, len, out_size);
            len = out_size;
        }
        for (i = 0; i < len; i++)
            *(out_value + i) = (unsigned char)((fdt32_to_cpu(*(data + i))) & 0xFF);
    } else {
        out_value[0] = '\0';
    }

    return len;
}

/*parsing from u8 array, and backup the parsing result*/
int lcm_fdt_getprop_u8_array_safe(const void *fdt, int nodeoffset,
        const char *name, unsigned char *out_value, unsigned int out_size)
{
    const unsigned char *data;
    int len = 0;
#if MTK_LCM_DEBUG_DUMP
    int i = 0, id = 0;
#endif

    if (fdt == NULL || name == NULL ||
        out_value == NULL || out_size == 0) {
        LCM_MSG("%s: invalid parameters\n", __func__);
        return -EINVAL;
    }

    data = (unsigned char *)fdt_getprop(fdt, nodeoffset, name, &len);
    if (len > 0 && data != NULL) {
        if ((unsigned int)len > out_size) {
            LCM_MSG("%s: len:%d is out of size:%u\n",
                __func__, len, out_size);
            len = out_size;
        }
        memcpy(out_value, data, len);
        if ((unsigned int)len < out_size)
            out_value[len] = '\0';
        else
            out_value[out_size - 1] = '\0';
#if MTK_LCM_DEBUG_DUMP
        for (i = id; i < len - 8; i += 8) {
             LCM_MSG("%s, data%d:0x%x,0x%x,0x%x,0x%x,0x%x,0x%x,0x%x,0x%x\n",
                 __func__, i, out_value[i], out_value[i+1],
                 out_value[i+2], out_value[i+3], out_value[i+4],
                 out_value[i+5], out_value[i+6], out_value[i+7]);
             if (i > id + 50 || i >= len - 16) {
                 LCM_MSG("%s, data:%d:0x%x,0x%x,0x%x,0x%x,0x%x\n",
                      __func__, len - 4, out_value[len-4],
                      out_value[len-3], out_value[len-2],
                      out_value[len-1], out_value[len]);
                 break;
             }
        }
#endif
    } else {
        out_value[0] = '\0';
    }

    return len;
}

/*parsing from u8 array, and do not backup the parsing result*/
int lcm_fdt_getprop_u8_array(const void *fdt, int nodeoffset,
        const char *name, unsigned char **out_value)
{
    int len = 0;

    if (fdt == NULL || name == NULL ||
        out_value == NULL) {
        LCM_MSG("%s: invalid parameters\n", __func__);
        return -EINVAL;
    }

    *out_value = (unsigned char *)fdt_getprop(fdt, nodeoffset, name, &len);
#if MTK_LCM_DEBUG_DUMP
    if (len > 0 && *out_value != NULL) {
        int i = 0, id = 0;

        for (i = id; i < len - 8; i += 8) {
             LCM_MSG("%s,data%d:0x%x,0x%x,0x%x,0x%x,0x%x,0x%x,0x%x,0x%x\n"
                  __func__, i, (*out_value)[i], (*out_value)[i+1],
                  (*out_value)[i+2], (*out_value)[i+3], (*out_value)[i+4],
                  (*out_value)[i+5], (*out_value)[i+6], (*out_value)[i+7]);
             if (i > id + 50 || i >= len - 16) {
                 LCM_MSG("%s,data:%d:0x%x,0x%x,0x%x,0x%x,0x%x\n",
                      __func__, len - 4, (*out_value)[len-4],
                      (*out_value)[len-3], (*out_value)[len-2],
                      (*out_value)[len-1], (*out_value)[len]);
                 break;
             }
        }
    } else {
        LCM_MSG("%s:%d-\"%s\" cannot find, len:%d\n",
            __func__, nodeoffset, name, len);
    }
#endif

    return len;
}


int lcm_fdt_getprop_u32_array(const void *fdt, int nodeoffset,
        const char *name, unsigned int *out_value, unsigned int out_size)
{
    int i;
    const unsigned int *data;
    int len = 0;

    if (fdt == NULL || name == NULL ||
        out_value == NULL || out_size == 0) {
        LCM_MSG("%s: invalid parameters\n", __func__);
        return -EINVAL;
    }

    data = fdt_getprop(fdt, nodeoffset, name, &len);
    if (len > 0 && data != NULL) {
        len /= 4;
        if ((unsigned int)len > out_size)
            len = out_size;
        for (i = 0; i < len; i++)
            *(out_value + i) = fdt32_to_cpu(*(data + i));
    } else {
        *out_value = 0;
    }

    return len;
}

int lcm_fdt_getprop_string(void *fdt, int nodeoffset,
        const char *name, char *out_value,
        unsigned int out_size)
{
    int namestroff = 0;
    const struct fdt_property *prop;
    const char *buf;
    int len = 0;

    if (fdt == NULL || name == NULL ||
        out_value == NULL || out_size == 0) {
        LCM_MSG("%s: invalid parameters\n", __func__);
        return -EINVAL;
    }
    prop = fdt_get_property(fdt, nodeoffset, name, &len);
    if (prop == NULL || (unsigned int)len > out_size) {
        LCM_MSG("%s,%d: invalid string prop,len:%d\n",
            __func__, __LINE__, len);
        return -EFAULT;
    }
    namestroff = fdt32_to_cpu(prop->nameoff);
    buf = fdt_string(fdt, namestroff);
    if (buf == NULL) {
        LCM_MSG("%s,%d: failed to get string value\n",
            __func__, __LINE__);
        return -EFAULT;
    }
    len = snprintf(out_value, out_size - 1, "%s", buf);
    if (len < 0 || (unsigned int)len >= out_size)
        LCM_MSG("%s, %d, snprintf failed\n", __func__, __LINE__);

    LCM_DBG("%s, %d, string value:%s, len:%d\n",
        __func__, __LINE__, out_value, len);

    return 0;
}

int lcm_fdt_update_dts_value(void *fdt, int target_offset,
        const char *node, unsigned int value)
{
    int ret = 0, phandle = 0;
#if MTK_LCM_DEBUG_DUMP
    unsigned int data = 0;
#endif

    if (fdt == NULL || node == NULL) {
        LCM_MSG("%s: invalid parameter\n", __func__);
        return -EINVAL;
    }

    if (target_offset < 0) {
        LCM_MSG("[%s] invalid node offset %d\n",
            __func__, target_offset);
        return -EINVAL;
    }

    phandle = cpu_to_fdt32(value);
    fdt_setprop(fdt, target_offset, node, &phandle, sizeof(phandle));

#if MTK_LCM_DEBUG_DUMP
    lcm_fdt_getprop_u32(fdt, target_offset, node, &data);
    LCM_DBG("update \"%s\" to 0x%x after lk set\n", node, data);
#endif

    return ret;
}

int lcm_fdt_update_dts_prop(void *fdt, int src_offset,
        int target_offset, const char *node)
{
    int ret = 0, phandle = 0;
#if MTK_LCM_DEBUG_DUMP
    unsigned int data = 0;
#endif

    if (fdt == NULL || node == NULL) {
        LCM_MSG("%s: invalid parameter\n", __func__);
        return -EINVAL;
    }

    if (src_offset < 0 || target_offset < 0) {
        LCM_MSG("[%s] invalid node offset %d, %d\n",
            __func__, src_offset, target_offset);
        return -EINVAL;
    }

    phandle = fdt_get_phandle(fdt, src_offset);
#if MTK_LCM_DEBUG_DUMP
    lcm_fdt_getprop_u32(fdt, target_offset, node, &data);
    LCM_DBG("default \"%s\" is 0x%x before lk set\n", node, data);
#endif

    phandle = cpu_to_fdt32(phandle);
    fdt_setprop(fdt, target_offset, node, &phandle, sizeof(phandle));

#if MTK_LCM_DEBUG_DUMP
    lcm_fdt_getprop_u32(fdt, target_offset, node, &data);
    LCM_DBG("update \"%s\" to 0x%x after lk set\n", node, data);
#endif

    return ret;
}

int lcm_fdt_update_dts_string(void *fdt, int nodeoffset,
        const char *node, const char *value)
{
    int ret = 0;
#if MTK_LCM_DEBUG_DUMP
    int len = 0;
    const char *temp, *temp2;
#endif

    if (fdt == NULL ||
        node == NULL || value == NULL) {
        LCM_MSG("%s: invalid parameter\n", __func__);
        return -EINVAL;
    }

    if (nodeoffset < 0) {
        LCM_MSG("[%s] invalid node offset %d\n",
            __func__, nodeoffset);
        return -EINVAL;
    }

#if MTK_LCM_DEBUG_DUMP
    temp = fdt_getprop(fdt, nodeoffset, node, &len);
    if (temp != NULL)
        LCM_DBG("default %s is %s\n", node, temp);
#endif

    fdt_setprop_string(fdt, nodeoffset, node, value);

#if MTK_LCM_DEBUG_DUMP
    temp2 = fdt_getprop(fdt, nodeoffset, node, &len);
    if (temp != NULL) {
        LCM_DBG("update %s to %s\n", node, temp2);
    } else {
        LCM_MSG("failed to update %s, %d\n", node, len);
        ret = -EFAULT;
    }
#endif

    return ret;
}

#if 0
int mtk_get_lcm_name(const void *fdt, int nodeoffset,
        char *name)
{
    int type_offset = 0;
    char *default_name = "unknown";
    int ret = 0, i = 0;

    if (fdt == NULL || name == NULL) {
        LCM_MSG("%s: invalid params\n", __func__);
        return -ENOMEM;
    }
    memset(params, 0x0, sizeof(LCM_PARAMS));

    ret = lcm_fdt_getprop_u8_array_safe(fdt, nodeoffset, "lcm-params-name",
            (u8 *)(name), MTK_LCM_NAME_LENGTH);
    if (ret > 0 && ret < MTK_LCM_NAME_LENGTH)
        name[ret] = '\0';
    else if (ret >= MTK_LCM_NAME_LENGTH)
        name[MTK_LCM_NAME_LENGTH - 1] = '\0';
    else {
        ret = snprintf(name,
            MTK_LCM_NAME_LENGTH - 1, "%s", default_name);
        if (ret < 0 || ret >= MTK_LCM_NAME_LENGTH)
            return -EFAULT;
        name[strlen(default_name)] = '\0';
    }

    return 0;
}
#endif

#ifdef MTK_ROUND_CORNER_SUPPORT
static int parse_lcm_params_round_corner(const void *fdt, int nodeoffset,
        LCM_ROUND_CORNER *corner)
{
    char pattern[MTK_LCM_NAME_LENGTH];
    int ret = 0;

    if (fdt == NULL || corner == NULL) {
        LCM_MSG("%s: invalid round corner\n", __func__);
        return -ENOMEM;
    }
    memset(corner, 0x0, sizeof(LCM_ROUND_CORNER));

    lcm_fdt_getprop_u32(fdt, nodeoffset, "lcm-params-lk-rc_round_corner_en",
            &corner->round_corner_en);
    lcm_fdt_getprop_u32(fdt, nodeoffset, "lcm-params-lk-rc_is_notch",
            &corner->is_notch);
    lcm_fdt_getprop_u32(fdt, nodeoffset, "lcm-params-lk-rc_full_content",
            &corner->full_content);

    lcm_fdt_getprop_u32(fdt, nodeoffset, "lcm-params-lk-rc_width",
            &corner->w);
    lcm_fdt_getprop_u32(fdt, nodeoffset, "lcm-params-lk-rc_height",
            &corner->h);
    lcm_fdt_getprop_u32(fdt, nodeoffset, "lcm-params-lk-rc_width_bot",
            &corner->w_bot);
    lcm_fdt_getprop_u32(fdt, nodeoffset, "lcm-params-lk-rc_height_bot",
            &corner->h_bot);

    lcm_fdt_getprop_u32(fdt, nodeoffset, "lcm-params-lk-rc_top_size",
            &corner->tp_size);
    lcm_fdt_getprop_u32(fdt, nodeoffset, "lcm-params-lk-rc_top_size_left",
            &corner->tp_size_l);
    lcm_fdt_getprop_u32(fdt, nodeoffset, "lcm-params-lk-rc_top_size_right",
            &corner->tp_size_r);
    lcm_fdt_getprop_u32(fdt, nodeoffset, "lcm-params-lk-rc_bottom_size",
            &corner->bt_size);

    ret = lcm_fdt_getprop_string(fdt, nodeoffset,
            "lcm-params-lk-rc_pattern_name", pattern, sizeof(pattern));
    if (ret < 0 || strlen(pattern) == 0) {
        LCM_MSG("%s,%d: invalid pattern:%d\n",
            __func__, __LINE__, ret);
        return -EFAULT;
    }

    LCM_MSG("%s, %d, corner pattern:%s\n",
        __func__, __LINE__, pattern);

    corner->lt_addr = mtk_lcm_get_rc_addr(pattern,
            RC_LEFT_TOP, &corner->tp_size);

    corner->rt_addr = mtk_lcm_get_rc_addr(pattern,
            RC_RIGHT_TOP, &corner->tp_size);

    corner->lt_addr_l = mtk_lcm_get_rc_addr(pattern,
            RC_LEFT_TOP_LEFT, &corner->tp_size_l);

    corner->lt_addr_r = mtk_lcm_get_rc_addr(pattern,
            RC_LEFT_TOP_RIGHT, &corner->tp_size_r);

    corner->lb_addr = mtk_lcm_get_rc_addr(pattern,
            RC_LEFT_BOTTOM, &corner->bt_size);

    corner->rb_addr = mtk_lcm_get_rc_addr(pattern,
            RC_RIGHT_BOTTOM, &corner->bt_size);

    return 0;
}
#endif

static int parse_lcm_params_basic(const void *fdt, int nodeoffset,
        int lk_offset, LCM_PARAMS *params)
{
    int ret = 0;
    unsigned int resolution[2] = {0};

    if (fdt == NULL || params == NULL) {
        LCM_MSG("%s: invalid lcm params\n", __func__);
        return -ENOMEM;
    }

    lcm_fdt_getprop_u32(fdt, nodeoffset, "lcm-params-types",
            &params->type);
    lcm_fdt_getprop_u32(fdt, lk_offset, "lcm-params-lk_ctrl",
            &params->ctrl);

    lcm_fdt_getprop_u32(fdt, lk_offset, "lcm-params-lk_lcm_if",
            &params->lcm_if);
    lcm_fdt_getprop_u32(fdt, lk_offset, "lcm-params-lk_lcm_cmd_if",
            &params->lcm_cmd_if);

    lcm_fdt_getprop_u32_array(fdt, nodeoffset, "lcm-params-resolution",
            &resolution[0], 2);
    params->width = resolution[0];
    params->height = resolution[1];

    lcm_fdt_getprop_u32(fdt, lk_offset, "lcm-params-lk_lcm_x",
            &params->lcm_x);
    lcm_fdt_getprop_u32(fdt, lk_offset, "lcm-params-lk_lcm_y",
            &params->lcm_y);

    lcm_fdt_getprop_u32_array(fdt, lk_offset, "lcm-params-lk_virtual_resolution",
            &resolution[0], 2);
    params->virtual_width = resolution[0];
    params->virtual_height = resolution[1];

    lcm_fdt_getprop_u32(fdt, lk_offset, "lcm-params-lk_io_select_mode",
            &params->io_select_mode);

    lcm_fdt_getprop_u32(fdt, nodeoffset, "lcm-params-physical_width",
            &params->physical_width);
    lcm_fdt_getprop_u32(fdt, nodeoffset, "lcm-params-physical_height",
            &params->physical_height);

    lcm_fdt_getprop_u32(fdt, lk_offset, "lcm-params-lk_od_table_size",
            &params->od_table_size);
    if (params->od_table_size > 0) {
        LCM_MALLOC(params->od_table, sizeof(u32) *
                (params->od_table_size + 1));
        if (params->od_table == NULL) {
            LCM_MSG("%s,%d: failed to allocate data\n",
                __func__, __LINE__);
            return -ENOMEM;
        }
        ret = lcm_fdt_getprop_u32_array(fdt, lk_offset, "lcm-params-lk_od_table",
                (u32 *)params->od_table, params->od_table_size);
        if (ret < 0 || (unsigned int)ret != params->od_table_size) {
            LCM_MSG("%s,%d: failed to get od table\n",
                __func__, __LINE__);
            return -EFAULT;
        }
    }

    return 0;
}

static int parse_lcm_params_dt_node(const void *fdt, int nodeoffset,
        LCM_PARAMS *params, struct mtk_panel_misc_data *misc)
{
#ifdef MTK_ROUND_CORNER_SUPPORT
    int rc_offset = 0;
#endif
    int lk_offset = 0;
    int type_offset = 0;
    int ret = 0;

    if (fdt == NULL) {
        LCM_MSG("%s: invalid lcm params\n", __func__);
        return -ENOMEM;
    }
    memset(params, 0x0, sizeof(LCM_PARAMS));

    lk_offset = fdt_subnode_offset(fdt, nodeoffset, "lcm-params-lk");
    if (lk_offset < 0) {
        LCM_MSG("%s: invalid lcm lk params, %d\n",
            __func__, lk_offset);
        return -EFAULT;
    }

#ifdef MTK_ROUND_CORNER_SUPPORT
    rc_offset = fdt_subnode_offset(fdt, nodeoffset,
            "lcm-params-lk-round-corner");
    if (rc_offset < 0) {
        LCM_MSG("%s: invalid lcm lk params round corner, %d\n",
            __func__, rc_offset);
        return -EFAULT;
    }
#endif

    parse_lcm_params_basic(fdt, nodeoffset,
            lk_offset, params);
#if MTK_LCM_DEBUG_DUMP
    dump_lcm_params_basic(params);
#endif

#ifdef MTK_ROUND_CORNER_SUPPORT
    if (rc_offset == NULL) {
        LCM_MSG("%s: invalid round corner params\n", __func__);
        return -EFAULT;
    }
    ret = parse_lcm_params_round_corner(fdt, rc_offset,
            &params->round_corner_params);
    if (ret)
        return ret;
#if MTK_LCM_DEBUG_DUMP
    dump_lcm_round_corner_params(&params->round_corner_params);
#endif
#endif

    switch (params->type) {
    case MTK_LCM_FUNC_DBI:
        type_offset = fdt_subnode_offset(fdt, nodeoffset,
                "lcm-params-dbi");
        if (type_offset < 0) {
            LCM_MSG("%s: invalid dbi params, %d\n", __func__, type_offset);
            return -EFAULT;
        }
        ret = parse_lcm_params_dbi(fdt, type_offset, &params->dbi, misc);
#if MTK_LCM_DEBUG_DUMP
        if (ret >= 0)
            dump_lcm_params_dbi(&params->dbi, NULL);
#endif
        if (ret < 0)
            free_lcm_params_dbi(&params->dbi);
        break;
    case MTK_LCM_FUNC_DPI:
        type_offset = fdt_subnode_offset(fdt, nodeoffset,
                "lcm-params-dpi");
        if (type_offset < 0) {
            LCM_MSG("%s: invalid dpi params, %d\n", __func__, type_offset);
            return -EFAULT;
        }
        ret = parse_lcm_params_dpi(fdt, type_offset, &params->dpi, misc);
#if MTK_LCM_DEBUG_DUMP
        if (ret >= 0)
            dump_lcm_params_dpi(&params->dpi, NULL);
#endif
        if (ret < 0)
            free_lcm_params_dpi(&params->dpi);
        break;
    case MTK_LCM_FUNC_DSI:
        type_offset = fdt_subnode_offset(fdt, nodeoffset,
                "lcm-params-dsi");
        if (type_offset < 0) {
            LCM_MSG("%s: invalid dsi params, %d\n", __func__, type_offset);
            return -EFAULT;
        }
        ret = parse_lcm_params_dsi(fdt, type_offset, &params->dsi, misc);
#if MTK_LCM_DEBUG_DUMP
        if (ret >= 0)
            dump_lcm_params_dsi(&params->dsi, NULL);
#endif
        if (ret < 0)
            free_lcm_params_dsi(&params->dsi);
        break;
    default:
        LCM_MSG("%s, invalid lcm type:%d\n",
            __func__, params->type);
        break;
    }

    return ret;
}

#define MTK_LCM_DATA_OFFSET (2)
static int parse_lcm_ops_func_util(struct mtk_lcm_ops_data *lcm_op, u8 *dts,
        unsigned int len)
{
    if (lcm_op == NULL ||
        dts == NULL) {
        LCM_MSG("%s,%d: invalid parameters\n",
            __func__, __LINE__);
        return -EINVAL;
    }

    switch (lcm_op->type) {
    case MTK_LCM_UTIL_TYPE_RESET:
        /* func type size data */
        lcm_op->param.util_data =  (unsigned int)dts[MTK_LCM_DATA_OFFSET];
        break;
    case MTK_LCM_UTIL_TYPE_MDELAY:
    case MTK_LCM_UTIL_TYPE_UDELAY:
    case MTK_LCM_UTIL_TYPE_TDELAY:
        /* func type size data */
        lcm_op->param.delay_data.timeout = (unsigned int)dts[MTK_LCM_DATA_OFFSET];
        break;
    case MTK_LCM_UTIL_TYPE_POWER_VOLTAGE:
        /* func type size addr level */
        lcm_op->param.util_data =  (unsigned int)dts[MTK_LCM_DATA_OFFSET];
        break;
    default:
        LCM_MSG("%s/%d: invalid type:0x%x\n",
            __FILE__, __LINE__, lcm_op->type);
        return -EINVAL;
    }

    return 0;
}

static int parse_lcm_ops_func_cmd(struct mtk_lcm_ops_data *lcm_op, u8 *dts,
        unsigned int len)
{
    if (lcm_op == NULL ||
        dts == NULL) {
        LCM_MSG("%s,%d: invalid parameters\n",
            __func__, __LINE__);
        return -EINVAL;
    }

    switch (lcm_op->type) {
    case MTK_LCM_LK_TYPE_PREPARE_PARAM_COUNT:
        if (lcm_op->size != 1) {
            LCM_MSG("%s,%d: invalid alignment, %u\n",
                __func__, __LINE__, lcm_op->size);
            return -ENOMEM;
        }
        lcm_op->param.lk_data_count = dts[MTK_LCM_DATA_OFFSET];
        break;
    case MTK_LCM_LK_TYPE_PREPARE_PARAM:
        if (lcm_op->size != 5) {
            LCM_MSG("%s,%d: invalid alignment, %u\n",
                __func__, __LINE__, lcm_op->size);
            return -ENOMEM;
        }
        lcm_op->param.lk_data.index = dts[MTK_LCM_DATA_OFFSET];
        lcm_op->param.lk_data.data = (dts[MTK_LCM_DATA_OFFSET + 1] << 24) |
            (dts[MTK_LCM_DATA_OFFSET + 2] << 16) |
            (dts[MTK_LCM_DATA_OFFSET + 3] << 8) |
            dts[MTK_LCM_DATA_OFFSET + 4];
        break;
    case MTK_LCM_LK_TYPE_PREPARE_PARAM_FIX_BIT:
        if (lcm_op->size != 3) {
            LCM_MSG("%s,%d: invalid alignment, %u\n",
                __func__, __LINE__, lcm_op->size);
            return -ENOMEM;
        }
        lcm_op->param.lk_data_bit.index = dts[MTK_LCM_DATA_OFFSET];
        lcm_op->param.lk_data_bit.bit = dts[MTK_LCM_DATA_OFFSET + 1];
        lcm_op->param.lk_data_bit.data = dts[MTK_LCM_DATA_OFFSET + 2];
        break;
    case MTK_LCM_LK_TYPE_PREPARE_PARAM_X0_MSB_BIT:
    case MTK_LCM_LK_TYPE_PREPARE_PARAM_X0_LSB_BIT:
    case MTK_LCM_LK_TYPE_PREPARE_PARAM_X1_MSB_BIT:
    case MTK_LCM_LK_TYPE_PREPARE_PARAM_X1_LSB_BIT:
    case MTK_LCM_LK_TYPE_PREPARE_PARAM_Y0_MSB_BIT:
    case MTK_LCM_LK_TYPE_PREPARE_PARAM_Y0_LSB_BIT:
    case MTK_LCM_LK_TYPE_PREPARE_PARAM_Y1_MSB_BIT:
    case MTK_LCM_LK_TYPE_PREPARE_PARAM_Y1_LSB_BIT:
        if (lcm_op->size != 3) {
            LCM_MSG("%s,%d: invalid alignment, %u\n",
                __func__, __LINE__, lcm_op->size);
            return -ENOMEM;
        }
        lcm_op->param.lk_data_bit.index = dts[MTK_LCM_DATA_OFFSET];
        lcm_op->param.lk_data_bit.bit = dts[MTK_LCM_DATA_OFFSET + 1];
        lcm_op->param.lk_data_bit.data = dts[MTK_LCM_DATA_OFFSET + 2]; /*default value*/
        break;
    case MTK_LCM_LK_TYPE_WRITE_PARAM:
    case MTK_LCM_LK_TYPE_WRITE_PARAM_UNFORCE:
        /* no parameters, in pair of MTK_LCM_LK_TYPE_PREPARE_COUNT*/
        break;
    case MTK_LCM_CMD_TYPE_WRITE_BUFFER:
        /* func type size cmd data0 data1 ... */
        LCM_MALLOC(lcm_op->param.cmd_data.data,
                lcm_op->size);
        if (lcm_op->param.cmd_data.data == NULL) {
            LCM_MSG("%s,%d: failed to allocate data\n",
                __func__, __LINE__);
            return -ENOMEM;
        }
        lcm_op->param.cmd_data.data_len = lcm_op->size - 1;
        lcm_op->param.cmd_data.cmd = dts[MTK_LCM_DATA_OFFSET];
        memcpy(lcm_op->param.cmd_data.data,
            dts + MTK_LCM_DATA_OFFSET + 1,
            lcm_op->size - 1);
        *(lcm_op->param.cmd_data.data + lcm_op->size - 1) = '\0';
        break;
    case MTK_LCM_CMD_TYPE_WRITE_CMD:
        /* func type size cmd data0 data1 ... */
        LCM_MALLOC(lcm_op->param.cmd_data.data,
                lcm_op->size);
        if (lcm_op->param.cmd_data.data == NULL) {
            LCM_MSG("%s,%d: failed to allocate data\n",
                __func__, __LINE__);
            return -ENOMEM;
        }
        lcm_op->param.cmd_data.cmd = dts[MTK_LCM_DATA_OFFSET];
        lcm_op->param.cmd_data.data_len = lcm_op->size - 1;
        memcpy(lcm_op->param.cmd_data.data,
            dts + MTK_LCM_DATA_OFFSET + 1,
            lcm_op->size - 1);
        *(lcm_op->param.cmd_data.data + lcm_op->size - 1) = '\0';
        break;
    case MTK_LCM_CMD_TYPE_READ_BUFFER:
    case MTK_LCM_CMD_TYPE_READ_CMD:
        /* func type size cmd data0 data1 ... */
        LCM_MALLOC(lcm_op->param.cmd_data.data,
                lcm_op->size - 2);
        if (lcm_op->param.cmd_data.data == NULL) {
            LCM_MSG("%s,%d: failed to allocate data\n",
                __func__, __LINE__);
            return -ENOMEM;
        }
        lcm_op->param.cmd_data.start_id = dts[MTK_LCM_DATA_OFFSET];
        lcm_op->param.cmd_data.data_len = dts[MTK_LCM_DATA_OFFSET + 1];
        lcm_op->param.cmd_data.cmd = dts[MTK_LCM_DATA_OFFSET + 2];
        *(lcm_op->param.cmd_data.data) = '\0';
        break;
    default:
        LCM_MSG("%s/%d: invalid type:0x%x\n",
            __FILE__, __LINE__, lcm_op->type);
        return -EINVAL;
    }
    return 0;
}

static int parse_lcm_ops_func_cb(struct mtk_lcm_ops_data *lcm_op, u8 *dts,
        unsigned int len)
{
    if (lcm_op == NULL ||
        dts == NULL) {
        LCM_MSG("%s,%d: invalid parameters\n",
            __func__, __LINE__);
        return -EINVAL;
    }

    switch (lcm_op->type) {
    case MTK_LCM_CB_TYPE_RUNTIME:
        /* func type size data0 data1 ... */
        LCM_MALLOC(lcm_op->param.cb_id_data.buffer_data,
                lcm_op->size + 1);
        if (lcm_op->param.cb_id_data.buffer_data == NULL) {
            LCM_MSG("%s,%d: failed to allocate data\n",
                __func__, __LINE__);
            return -ENOMEM;
        }
        memcpy(lcm_op->param.cb_id_data.buffer_data,
            dts + MTK_LCM_DATA_OFFSET, lcm_op->size);
        *(lcm_op->param.cb_id_data.buffer_data + lcm_op->size) = '\0';
        break;
    case MTK_LCM_CB_TYPE_RUNTIME_INPUT:
        /* func type size data0 data1 ... */
        lcm_op->param.cb_id_data.id_count = 1;
        lcm_op->param.cb_id_data.data_count = lcm_op->size - 1;
        LCM_MALLOC(lcm_op->param.cb_id_data.buffer_data,
                lcm_op->param.cb_id_data.data_count + 1);
        LCM_MALLOC(lcm_op->param.cb_id_data.id,
            lcm_op->param.cb_id_data.id_count + 1);
        if (lcm_op->param.cb_id_data.buffer_data == NULL ||
            lcm_op->param.cb_id_data.id == NULL) {
            LCM_MSG("%s,%d: failed to allocate data\n",
                __func__, __LINE__);
            return -ENOMEM;
        }
        lcm_op->param.cb_id_data.id[0] = dts[MTK_LCM_DATA_OFFSET];
        lcm_op->param.cb_id_data.id[1] = '\0';
        memcpy(lcm_op->param.cb_id_data.buffer_data,
            dts + MTK_LCM_DATA_OFFSET + 1,
            lcm_op->param.cb_id_data.data_count);
        *(lcm_op->param.cb_id_data.buffer_data +
            lcm_op->param.cb_id_data.data_count) = '\0';
        break;
    case MTK_LCM_CB_TYPE_RUNTIME_INPUT_MULTIPLE:
        /* func type size id_count id0, id1 ... data_count data0 data1 ... */
        /* parsing runtime input index */
        lcm_op->param.cb_id_data.id_count = dts[MTK_LCM_DATA_OFFSET];
        LCM_MALLOC(lcm_op->param.cb_id_data.id,
            lcm_op->param.cb_id_data.id_count + 1);
        if (lcm_op->param.cb_id_data.id == NULL) {
            LCM_MSG("%s,%d: failed to allocate id\n",
                __func__, __LINE__);
            return -ENOMEM;
        }
        memcpy(lcm_op->param.cb_id_data.id,
            dts + MTK_LCM_DATA_OFFSET + 1,
            lcm_op->param.cb_id_data.id_count);
        *(lcm_op->param.cb_id_data.id +
                lcm_op->param.cb_id_data.id_count) = '\0';

        /* parsing cmd parameters */
        lcm_op->param.cb_id_data.data_count = dts[MTK_LCM_DATA_OFFSET +
            lcm_op->param.cb_id_data.id_count + 1];
        LCM_MALLOC(lcm_op->param.cb_id_data.buffer_data,
            lcm_op->param.cb_id_data.data_count + 1);
        if (lcm_op->param.cb_id_data.buffer_data == NULL) {
            LCM_MSG("%s,%d: failed to allocate data\n",
                __func__, __LINE__);
            return -ENOMEM;
        }
        memcpy(lcm_op->param.cb_id_data.buffer_data,
            dts + MTK_LCM_DATA_OFFSET +
            lcm_op->param.cb_id_data.id_count + 2,
            lcm_op->param.cb_id_data.data_count);
        *(lcm_op->param.cb_id_data.buffer_data +
                lcm_op->param.cb_id_data.data_count) = '\0';
        break;
    default:
        LCM_MSG("%s/%d: invalid type:0x%x\n",
            __FILE__, __LINE__, lcm_op->type);
        return -EINVAL;
    }
    return 0;
}

static int parse_lcm_ops_func_gpio(struct mtk_lcm_ops_data *lcm_op, u8 *dts,
        unsigned int len)
{
    if (lcm_op == NULL ||
        dts == NULL) {
        LCM_MSG("%s,%d: invalid parameters\n",
            __func__, __LINE__);
        return -EINVAL;
    }

    switch (lcm_op->type) {
    case MTK_LCM_GPIO_TYPE_MODE:
    case MTK_LCM_GPIO_TYPE_DIR_OUTPUT:
    case MTK_LCM_GPIO_TYPE_DIR_INPUT:
    case MTK_LCM_GPIO_TYPE_OUT:
        if (lcm_op->size != 2) {
            LCM_MSG("%s,%d: invalid gpio size %u\n",
                __func__, __LINE__, lcm_op->size);
            return -EINVAL;
        }
        lcm_op->param.gpio_data.gpio_id = dts[MTK_LCM_DATA_OFFSET];
        lcm_op->param.gpio_data.data = dts[MTK_LCM_DATA_OFFSET + 1];
        break;
    default:
        LCM_MSG("%s/%d: invalid type:0x%x\n",
            __FILE__, __LINE__, lcm_op->type);
        return -EINVAL;
    }
    return 0;
}

static int parse_lcm_ops_func_cust(struct mtk_lcm_ops_data *lcm_op,
         u8 *dts, struct mtk_panel_cust *cust)
{
    if (lcm_op == NULL ||
        dts == NULL || cust == NULL)
        return -EINVAL;

    if (cust->cust_enabled == 0 ||
        cust->parse_ops == NULL)
        return -EINVAL;

    cust->parse_ops(lcm_op->func, lcm_op->type,
         dts, lcm_op->size, lcm_op->param.cust_data);

    return 0;
}

static int parse_lcm_ops_basic(struct mtk_lcm_ops_data *lcm_op, u8 *dts,
         struct mtk_panel_cust *cust, unsigned int len)
{
    int ret = 0;

    if (lcm_op == NULL || dts == NULL || len == 0) {
        LCM_MSG("%s %d: invalid cmd\n", __func__, __LINE__);
        return -EINVAL;
    }

    if (len - lcm_op->size < MTK_LCM_DATA_OFFSET) {
        LCM_MSG("%s,%d: dts length is not enough %u,%u\n",
            __func__, __LINE__, len, lcm_op->size);
        return -EINVAL;
    }

    if (lcm_op->type > MTK_LCM_UTIL_TYPE_START &&
        lcm_op->type < MTK_LCM_UTIL_TYPE_END)
        ret = parse_lcm_ops_func_util(lcm_op, dts, len);
    else if (lcm_op->type > MTK_LCM_CMD_TYPE_START &&
        lcm_op->type < MTK_LCM_CMD_TYPE_END)
        ret = parse_lcm_ops_func_cmd(lcm_op, dts, len);
    else if (lcm_op->type > MTK_LCM_LK_TYPE_START &&
        lcm_op->type < MTK_LCM_LK_TYPE_END)
        ret = parse_lcm_ops_func_cmd(lcm_op, dts, len);
    else if (lcm_op->type > MTK_LCM_CB_TYPE_START &&
        lcm_op->type < MTK_LCM_CB_TYPE_END)
        ret = parse_lcm_ops_func_cb(lcm_op, dts, len);
    else if (lcm_op->type > MTK_LCM_GPIO_TYPE_START &&
        lcm_op->type < MTK_LCM_GPIO_TYPE_END)
        ret = parse_lcm_ops_func_gpio(lcm_op, dts, len);
    else if (lcm_op->type > MTK_LCM_CUST_TYPE_START &&
        lcm_op->type < MTK_LCM_CUST_TYPE_END)
        ret = parse_lcm_ops_func_cust(lcm_op,
                dts, cust);
    else {
        LCM_MSG("%s/%d: invalid type:0x%x\n",
            __FILE__, __LINE__, lcm_op->type);
        return -EINVAL;
    }

    return ret;
}

int parse_lcm_ops_check(struct mtk_lcm_ops_data *ops, u8 *dts,
        unsigned int len, int curr_status, unsigned int phase)
{
    unsigned int i = 0, phase_tmp = 0;
    int phase_skip_flag = curr_status;

    if (ops == NULL || dts == NULL) {
        LCM_MSG("%s,%d: invalid parameters, len:%u\n",
            __func__, __LINE__, len);
        return phase_skip_flag;
    }

    if (len - ops->size < MTK_LCM_DATA_OFFSET) {
        LCM_MSG("%s,%d: dts length is not enough %u,%u\n",
            __func__, __LINE__, len, ops->size);
        return phase_skip_flag;
    }

    if (ops->type == MTK_LCM_PHASE_TYPE_START &&
        curr_status != MTK_LCM_PHASE_TYPE_START) {
        for (i = 0; i < ops->size; i++)
            phase_tmp |= dts[i + MTK_LCM_DATA_OFFSET];

        if ((phase_tmp & phase) != phase)
            phase_skip_flag = MTK_LCM_PHASE_TYPE_START;
    } else if (ops->type == MTK_LCM_PHASE_TYPE_END &&
        curr_status == MTK_LCM_PHASE_TYPE_START) {
        for (i = 0; i < ops->size; i++)
            phase_tmp |= dts[i + MTK_LCM_DATA_OFFSET];

        if ((phase_tmp & phase) != phase)
            phase_skip_flag = MTK_LCM_PHASE_TYPE_END;
    } else if (curr_status == MTK_LCM_PHASE_TYPE_END) {
        phase_skip_flag = 0;
    }

    return phase_skip_flag;
}

int parse_lcm_ops_func(const void *fdt, int nodeoffset,
        struct mtk_lcm_ops_table *table,
        const char *func,
        unsigned int panel_type,
        struct mtk_panel_cust *cust, unsigned int phase)
{
    unsigned int skip_count = 0, i = 0;
    u8 *tmp = NULL;
    int len = 0, tmp_len = 0, ret = 0;
    int phase_skip_flag = 0;
    struct mtk_lcm_ops_data *op = NULL;
    u8 *table_dts_buf = NULL;

    if (fdt == NULL || func == NULL ||
         panel_type >= MTK_LCM_FUNC_END) {
        LCM_MSG("%s:%d: invalid parameters\n", __FILE__, __LINE__);
        return -EFAULT;
    }

    len = lcm_fdt_getprop_u8_array(fdt, nodeoffset,
                func, &table_dts_buf);
    if (len == 0) {
        return 0;
    } else if (len < 0 || table_dts_buf == NULL) {
        LCM_MSG("%s, failed to get table dts, len:%d\n",
            __func__, len);
        return 0;
    } else {
#if MTK_LCM_DEBUG_DUMP
        LCM_MSG("%s: start to parse:%s, len:%u, phase:0x%x\n",
            __func__, func, len, phase);
#endif
    }

    list_initialize(&table->list);
    tmp = &table_dts_buf[0];
    while (len > MTK_LCM_DATA_OFFSET) {
        LCM_MALLOC(op, sizeof(struct mtk_lcm_ops_data));
        if (op == NULL) {
            LCM_MSG("%s, %d, failed to allocate op\n", __func__, __LINE__);
            return -ENOMEM;
        }

        op->func = panel_type;
        op->type = tmp[0];
        if (op->type == MTK_LCM_TYPE_END) {
            LCM_FREE(op, sizeof(struct mtk_lcm_ops_data));
            //LCM_DBG("%s: parsing end of %s, len:%d\n", __func__, func, len);
            len = 0;
            break;
        }

        op->size = tmp[1];
        tmp_len = MTK_LCM_DATA_OFFSET + op->size;
        phase_skip_flag = parse_lcm_ops_check(op,
                    tmp, len, phase_skip_flag, phase);

        if (phase_skip_flag == 0 &&
            op->type != MTK_LCM_PHASE_TYPE_START &&
            op->type != MTK_LCM_PHASE_TYPE_END) {
            ret = parse_lcm_ops_basic(op, tmp, cust, len);
            LCM_DBG("[%s+%d]>>>%u,type:%u,size:%u,dts:%u,op:%u,phase:0x%x,skip:%d\n",
                func, i, op->func, op->type,
                op->size, len, tmp_len,
                phase, phase_skip_flag);
            if (ret < 0) {
                LCM_MSG("[%s+%d] >>>func:%u,type:%u,size:%u,dts:%u,fail:%d\n",
                    func, i, op->func, op->type,
                    op->size, len, ret);
                LCM_FREE(op, sizeof(struct mtk_lcm_ops_data));
                return ret;
            }
            list_add_tail(&table->list, &op->node);
            table->size++;
        } else {
            LCM_DBG("[%s+%d] >>> func:%u,type:%u,sz:%u,skip(0x%x),phs(0x%x),dts:%u,op:%u\n",
                func, i, op->func, op->type, op->size,
                phase_skip_flag, phase, len, tmp_len);
            LCM_FREE(op, sizeof(struct mtk_lcm_ops_data));
            skip_count++;
        }

        if (tmp_len <= len) {
            tmp = tmp + tmp_len;
            len = len - tmp_len;
        } else {
            LCM_DBG("%s: parsing warning of %s, len:%d, op:%u\n", __func__, func, len, tmp_len);
            break;
        }
        i++;
    }

    if (len >= MTK_LCM_DATA_OFFSET)
        LCM_MSG("%s:%d:%s, total:%u,parsing:%u,skip:%u,last_dts:%u\n",
            __func__, __LINE__, func, i, table->size, skip_count, len);

    return table->size;
}

static int parse_lcm_ops_dt_node(const void *fdt, int nodeoffset,
        struct mtk_lcm_ops *ops, LCM_PARAMS *params,
        struct mtk_panel_cust *cust)
{
    int type_offset = 0;
    int ret = 0;

    if (fdt == NULL || ops == NULL || params == NULL) {
        LCM_MSG("%s: invalid lcm ops\n", __func__);
        return -ENOMEM;
    }

    switch (params->type) {
    case MTK_LCM_FUNC_DBI:
        type_offset = fdt_subnode_offset(fdt, nodeoffset, "lcm-ops-dbi");
        if (type_offset < 0) {
            LCM_MSG("%s:%d: failed to get lcm-ops-dbi, lcm:%d, type:%d\n",
                __func__, __LINE__, nodeoffset, type_offset);
            return -EFAULT;
        }
        LCM_MALLOC(ops->dbi_ops, sizeof(struct mtk_lcm_ops_dbi));
        if (ops->dbi_ops == NULL) {
            LCM_MSG("%s, %d, failed to allocate dbi ops\n", __func__, __LINE__);
            return -ENOMEM;
        }

        ret = parse_lcm_ops_dbi(fdt, type_offset,
                ops->dbi_ops, &params->dbi, cust);
        if (ret >= 0) {
#if MTK_LCM_DEBUG_DUMP
            LCM_MSG("%s, parse done:%d, LCM dump dbi ops:0x%lx, params:0x%lx\n",
                __func__, ret, (unsigned long)ops->dbi_ops, (unsigned long)(&params->dbi));
            dump_lcm_ops_dbi(ops->dbi_ops, &params->dbi, NULL);
#endif
        } else
            free_lcm_ops_dbi(ops->dbi_ops);
        break;
    case MTK_LCM_FUNC_DPI:
        type_offset = fdt_subnode_offset(fdt, nodeoffset, "lcm-ops-dpi");
        if (type_offset < 0) {
            LCM_MSG("%s:%d: failed to get lcm-ops-dpi, lcm:%d, type:%d\n",
                __func__, __LINE__, nodeoffset, type_offset);
            return -EFAULT;
        }
        LCM_MALLOC(ops->dpi_ops, sizeof(struct mtk_lcm_ops_dpi));
        if (ops->dpi_ops == NULL) {
            LCM_MSG("%s, %d, failed to allocate dpi ops\n", __func__, __LINE__);
            return -ENOMEM;
        }

        ret = parse_lcm_ops_dpi(fdt, type_offset,
                ops->dpi_ops, &params->dpi, cust);
        if (ret >= 0) {
#if MTK_LCM_DEBUG_DUMP
        LCM_MSG("%s, parse done:%d, LCM dump dpi ops:0x%lx, params:0x%lx\n",
            __func__, ret, (unsigned long)ops->dpi_ops, (unsigned long)(&params->dpi));
            dump_lcm_ops_dpi(ops->dpi_ops, &params->dpi, NULL);
#endif
        } else
            free_lcm_ops_dpi(ops->dpi_ops);
        break;
    case MTK_LCM_FUNC_DSI:
        type_offset = fdt_subnode_offset(fdt, nodeoffset, "lcm-ops-dsi");
        if (type_offset < 0) {
            LCM_MSG("%s:%d: failed to get lcm-ops-dsi, lcm:%d, type:%d\n",
                __func__, __LINE__, nodeoffset, type_offset);
            return -EFAULT;
        }
        LCM_MALLOC(ops->dsi_ops, sizeof(struct mtk_lcm_ops_dsi));
        if (ops->dsi_ops == NULL) {
            LCM_MSG("%s, %d, failed to allocate dsi ops\n", __func__, __LINE__);
            return -ENOMEM;
        }

        ret = parse_lcm_ops_dsi(fdt, type_offset,
                ops->dsi_ops, &params->dsi, cust);
        if (ret >= 0) {
#if MTK_LCM_DEBUG_DUMP
        LCM_MSG("%s, parse done:%d, LCM dump dsi ops:0x%lx, params:0x%lx\n",
            __func__, ret, (unsigned long)ops->dsi_ops, (unsigned long)(&params->dsi));
            dump_lcm_ops_dsi(ops->dsi_ops, &params->dsi, NULL);
#endif
        } else
            free_lcm_ops_dsi(ops->dsi_ops);
        break;
    default:
        LCM_MSG("%s, invalid lcm type:%d\n",
            __func__, params->type);
        ret = -EINVAL;
        break;
    }

    return ret;
}

int load_panel_resource_from_dts(const void *fdt, int nodeoffset,
        struct mtk_panel_resource *data)
{
    int ret = 0, sub_offset;

    if (fdt == NULL) {
        LCM_MSG("%s:%d: invalid fdt\n",
            __func__, __LINE__);
        return -EINVAL;
    }

    if (data == NULL) {
        LCM_MSG("%s:%d: invalid panel resource\n",
            __func__, __LINE__);
        return -EINVAL;
    }

    lcm_fdt_getprop_u32(fdt, nodeoffset, "lcm-version", &data->version);
    LCM_DBG("%s: version:%u, total_size:%lluByte\n",
        __func__, data->version, mtk_lcm_total_size);

    /* Load LCM parameters from DT */
    sub_offset = fdt_subnode_offset(fdt, nodeoffset, "lcm-params");
    if (sub_offset < 0) {
        LCM_MSG("%s:%d: failed to get lcm-params, lcm:%d, sub:%d\n",
            __func__, __LINE__, nodeoffset, sub_offset);
        return -EFAULT;
    }

    ret = parse_lcm_params_dt_node(fdt, sub_offset, &data->params, &data->misc);
    if (ret < 0)
        return ret;
    LCM_DBG("%s: parsing lcm-params, total_size:%lluByte\n",
        __func__, mtk_lcm_total_size);

    /* Load LCM customization settings from DT */
    if (data->cust.cust_enabled == 1 &&
        data->cust.parse_params != NULL) {
        LCM_MSG("%s: parsing cust settings, enable:%d\n",
            __func__, data->cust.cust_enabled);
        ret = data->cust.parse_params(fdt, sub_offset);
        if (ret < 0)
            LCM_MSG("%s, failed at cust parsing, %d\n",
                __func__, ret);
        LCM_DBG("%s: parsing lcm-params-cust, enable:%d, total_size:%lluByte\n",
            __func__, data->cust.cust_enabled, mtk_lcm_total_size);
    }

    /* Load LCM ops from DT */
    sub_offset = fdt_subnode_offset(fdt, nodeoffset, "lcm-ops");
    if (sub_offset < 0) {
        LCM_MSG("%s:%d: failed to get lcm-params, lcm:%d, sub:%d\n",
            __func__, __LINE__, nodeoffset, sub_offset);
        return -EFAULT;
    }
    ret = parse_lcm_ops_dt_node(fdt, sub_offset, &data->ops,
        &data->params, &data->cust);
    if (ret < 0) {
        LCM_MSG("%s, failed to parse operations, %d\n", __func__, ret);
        return ret;
    }
    LCM_DBG("%s: parsing lcm-ops, total_size:%lluByte\n",
        __func__, mtk_lcm_total_size);

    return 0;
}

static void mtk_get_func_name(unsigned int func, char *out)
{
    int ret = 0;
    if (out == NULL) {
        LCM_MSG("%s: invalid out buffer\n", __func__);
        return;
    }

    switch (func) {
    case MTK_LCM_FUNC_DBI:
        ret = snprintf(out, MTK_LCM_NAME_LENGTH - 1, "DBI");
        break;
    case MTK_LCM_FUNC_DPI:
        ret = snprintf(out, MTK_LCM_NAME_LENGTH - 1, "DPI");
        break;
    case MTK_LCM_FUNC_DSI:
        ret = snprintf(out, MTK_LCM_NAME_LENGTH - 1, "DSI");
        break;
    default:
        ret = snprintf(out, MTK_LCM_NAME_LENGTH - 1, "unknown");
        break;
    }
    if (ret < 0 || ret >= MTK_LCM_NAME_LENGTH)
        LCM_MSG("%s, failed, ret:%u\n", __func__, ret);
}

static void mtk_get_type_name(unsigned int type, char *out)
{
    int ret = 0;
    if (out == NULL) {
        LCM_MSG("%s: invalid out buffer\n", __func__);
        return;
    }

    switch (type) {
    case MTK_LCM_UTIL_TYPE_RESET:
        ret = snprintf(out, MTK_LCM_NAME_LENGTH - 1, "RESET");
        break;
    case MTK_LCM_UTIL_TYPE_MDELAY:
        ret = snprintf(out, MTK_LCM_NAME_LENGTH - 1, "MDELAY");
        break;
    case MTK_LCM_UTIL_TYPE_UDELAY:
        ret = snprintf(out, MTK_LCM_NAME_LENGTH - 1, "UDELAY");
        break;
    case MTK_LCM_UTIL_TYPE_TDELAY:
        ret = snprintf(out, MTK_LCM_NAME_LENGTH - 1, "TICK_DELAY");
        break;
    case MTK_LCM_UTIL_TYPE_POWER_VOLTAGE:
        ret = snprintf(out, MTK_LCM_NAME_LENGTH - 1,    "POWER_VOLTAGE");
        break;
    case MTK_LCM_UTIL_TYPE_POWER_ON:
        ret = snprintf(out, MTK_LCM_NAME_LENGTH - 1, "POWER_ON");
        break;
    case MTK_LCM_UTIL_TYPE_POWER_OFF:
        ret = snprintf(out, MTK_LCM_NAME_LENGTH - 1,    "POWER_OFF");
        break;
    case MTK_LCM_CMD_TYPE_WRITE_BUFFER:
        ret = snprintf(out, MTK_LCM_NAME_LENGTH - 1, "WRITE_BUF");
        break;
    case MTK_LCM_CMD_TYPE_WRITE_CMD:
        ret = snprintf(out, MTK_LCM_NAME_LENGTH - 1, "WRITE_CMD");
        break;
    case MTK_LCM_CMD_TYPE_READ_BUFFER:
        ret = snprintf(out, MTK_LCM_NAME_LENGTH - 1, "READ_BUF");
        break;
    case MTK_LCM_CMD_TYPE_READ_CMD:
        ret = snprintf(out, MTK_LCM_NAME_LENGTH - 1, "READ_CMD");
        break;
    case MTK_LCM_CB_TYPE_RUNTIME:
        ret = snprintf(out, MTK_LCM_NAME_LENGTH - 1, "CB_RUNTIME");
        break;
    case MTK_LCM_CB_TYPE_RUNTIME_INPUT:
        ret = snprintf(out, MTK_LCM_NAME_LENGTH - 1, "CB_RUNTIME_INOUT");
        break;
    case MTK_LCM_CB_TYPE_RUNTIME_INPUT_MULTIPLE:
        ret = snprintf(out, MTK_LCM_NAME_LENGTH - 1, "CB_RUNTIME_INOUT_MUL");
        break;
    case MTK_LCM_GPIO_TYPE_MODE:
        ret = snprintf(out, MTK_LCM_NAME_LENGTH - 1, "GPIO_MODE");
        break;
    case MTK_LCM_GPIO_TYPE_DIR_OUTPUT:
        ret = snprintf(out, MTK_LCM_NAME_LENGTH - 1, "GPIO_DIR_OUT");
        break;
    case MTK_LCM_GPIO_TYPE_DIR_INPUT:
        ret = snprintf(out, MTK_LCM_NAME_LENGTH - 1, "GPIO_DIR_IN");
        break;
    case MTK_LCM_GPIO_TYPE_OUT:
        ret = snprintf(out, MTK_LCM_NAME_LENGTH - 1, "GPIO_OUT");
        break;
    case MTK_LCM_LK_TYPE_PREPARE_PARAM_COUNT:
        ret = snprintf(out, MTK_LCM_NAME_LENGTH - 1, "LK_PREPARE_COUNT");
        break;
    case MTK_LCM_LK_TYPE_PREPARE_PARAM:
        ret = snprintf(out, MTK_LCM_NAME_LENGTH - 1, "LK_PREPARE_PARAM");
        break;
    case MTK_LCM_LK_TYPE_PREPARE_PARAM_FIX_BIT:
        ret = snprintf(out, MTK_LCM_NAME_LENGTH - 1, "LK_PREPARE_FIX");
        break;
    case MTK_LCM_LK_TYPE_PREPARE_PARAM_X0_MSB_BIT:
        ret = snprintf(out, MTK_LCM_NAME_LENGTH - 1, "LK_PREPARE_X0_MSB");
        break;
    case MTK_LCM_LK_TYPE_PREPARE_PARAM_X0_LSB_BIT:
        ret = snprintf(out, MTK_LCM_NAME_LENGTH - 1, "LK_PREPARE_X0_LSB");
        break;
    case MTK_LCM_LK_TYPE_PREPARE_PARAM_X1_MSB_BIT:
        ret = snprintf(out, MTK_LCM_NAME_LENGTH - 1, "LK_PREPARE_X1_MSB");
        break;
    case MTK_LCM_LK_TYPE_PREPARE_PARAM_X1_LSB_BIT:
        ret = snprintf(out, MTK_LCM_NAME_LENGTH - 1, "LK_PREPARE_X1_LSB");
        break;
    case MTK_LCM_LK_TYPE_PREPARE_PARAM_Y0_MSB_BIT:
        ret = snprintf(out, MTK_LCM_NAME_LENGTH - 1, "LK_PREPARE_Y0_MSB");
        break;
    case MTK_LCM_LK_TYPE_PREPARE_PARAM_Y0_LSB_BIT:
        ret = snprintf(out, MTK_LCM_NAME_LENGTH - 1, "LK_PREPARE_Y0_LSB");
        break;
    case MTK_LCM_LK_TYPE_PREPARE_PARAM_Y1_MSB_BIT:
        ret = snprintf(out, MTK_LCM_NAME_LENGTH - 1, "LK_PREPARE_Y1_MSB");
        break;
    case MTK_LCM_LK_TYPE_PREPARE_PARAM_Y1_LSB_BIT:
        ret = snprintf(out, MTK_LCM_NAME_LENGTH - 1, "LK_PREPARE_Y1_LSB");
        break;
    case MTK_LCM_LK_TYPE_WRITE_PARAM:
        ret = snprintf(out, MTK_LCM_NAME_LENGTH - 1, "LK_WRITE_PARAM");
        break;
    case MTK_LCM_LK_TYPE_WRITE_PARAM_UNFORCE:
        ret = snprintf(out, MTK_LCM_NAME_LENGTH - 1, "LK_WRITE_PARAM_UNFORCE");
        break;
    default:
        if (type > MTK_LCM_CUST_TYPE_START &&
            type < MTK_LCM_CUST_TYPE_END)
            ret = snprintf(out, MTK_LCM_NAME_LENGTH - 1,
                "CUST-%d", type);
        ret = snprintf(out, MTK_LCM_NAME_LENGTH - 1,
                "unknown");
        break;
    }

    if (ret < 0 || ret >= MTK_LCM_NAME_LENGTH)
        LCM_MSG("%s, failed, ret:%u\n", __func__, ret);
}

static void dump_lcm_ops_func_util(struct mtk_lcm_ops_data *lcm_op,
    const char *owner, unsigned int id)
{
    char func_name[MTK_LCM_NAME_LENGTH] = { 0 };
    char type_name[MTK_LCM_NAME_LENGTH] = { 0 };

    if (lcm_op == NULL || owner == NULL)
        return;

    switch (lcm_op->type) {
    case MTK_LCM_UTIL_TYPE_RESET:
    case MTK_LCM_UTIL_TYPE_MDELAY:
    case MTK_LCM_UTIL_TYPE_UDELAY:
    case MTK_LCM_UTIL_TYPE_TDELAY:
    case MTK_LCM_UTIL_TYPE_POWER_VOLTAGE:
        mtk_get_func_name(lcm_op->func, &func_name[0]);
        mtk_get_type_name(lcm_op->type, &type_name[0]);
        LCM_MSG("[%s-%u]: func:%s, type:%s, dts_size:%u, data:%u\n",
            owner, id, func_name, type_name,
            lcm_op->size, lcm_op->param.util_data);
        break;
    case MTK_LCM_UTIL_TYPE_POWER_ON:
    case MTK_LCM_UTIL_TYPE_POWER_OFF:
        break;
    default:
        LCM_MSG("%s:[%s-%u]: invalid type:%u\n",
            __func__, owner, id, lcm_op->type);
        break;
    }
}

static void dump_lcm_ops_func_cmd(struct mtk_lcm_ops_data *lcm_op,
    const char *owner, unsigned int id)
{
    char func_name[MTK_LCM_NAME_LENGTH] = { 0 };
    char type_name[MTK_LCM_NAME_LENGTH] = { 0 };
    unsigned int i = 0;

    if (lcm_op == NULL || owner == NULL)
        return;

    mtk_get_func_name(lcm_op->func, &func_name[0]);
    mtk_get_type_name(lcm_op->type, &type_name[0]);
    switch (lcm_op->type) {
    case MTK_LCM_CMD_TYPE_WRITE_BUFFER:
        LCM_MSG("[%s-%u]: func:%s, type:%s, dts_size:%u, cmd:0x%x, para_count:%u\n",
            owner, id, func_name, type_name,
            lcm_op->size,
            lcm_op->param.cmd_data.cmd,
            lcm_op->param.cmd_data.data_len);
        for (i = 0; i < lcm_op->param.cmd_data.data_len; i += 4) {
            LCM_MSG("[%s-%u][data%u~%u]>>> 0x%x 0x%x 0x%x 0x%x\n",
                owner, id, i, i + 3,
                lcm_op->param.cmd_data.data[i],
                lcm_op->param.cmd_data.data[i + 1],
                lcm_op->param.cmd_data.data[i + 2],
                lcm_op->param.cmd_data.data[i + 3]);
        }
        break;
    case MTK_LCM_CMD_TYPE_WRITE_CMD:
        LCM_MSG("[%s-%u]: func:%s, type:%s, dts_size:%u, cmd:0x%x, data_len:0x%x, startid:%u\n",
            owner, id, func_name, type_name, lcm_op->size,
            lcm_op->param.cmd_data.cmd,
            lcm_op->param.cmd_data.data_len,
            lcm_op->param.cmd_data.start_id);
        for (i = 0; i < lcm_op->param.cmd_data.data_len; i += 4) {
            LCM_MSG("[%s-%u][data%u~%u]>>> 0x%x 0x%x 0x%x 0x%x\n",
                owner, id, i, i + 3,
                lcm_op->param.cmd_data.data[i],
                lcm_op->param.cmd_data.data[i + 1],
                lcm_op->param.cmd_data.data[i + 2],
                lcm_op->param.cmd_data.data[i + 3]);
        }
        break;
    case MTK_LCM_CMD_TYPE_READ_BUFFER:
    case MTK_LCM_CMD_TYPE_READ_CMD:
        LCM_MSG("[%s-%u]: func:%s, type:%s, dts_size:%u, cmd:0x%x, data_len:0x%x, startid:%u\n",
            owner, id, func_name, type_name, lcm_op->size,
            lcm_op->param.cmd_data.cmd,
            lcm_op->param.cmd_data.data_len,
            lcm_op->param.cmd_data.start_id);
        break;
    default:
        LCM_MSG("%s:[%s-%u]: invalid type:%u\n",
            __func__, owner, id, lcm_op->type);
        break;
    }
}

static void dump_lcm_ops_func_cb(struct mtk_lcm_ops_data *lcm_op,
        const char *owner, unsigned int id)
{
    char func_name[MTK_LCM_NAME_LENGTH] = { 0 };
    char type_name[MTK_LCM_NAME_LENGTH] = { 0 };
    unsigned int i = 0;

    if (lcm_op == NULL || owner == NULL)
        return;

    mtk_get_func_name(lcm_op->func, &func_name[0]);
    mtk_get_type_name(lcm_op->type, &type_name[0]);
    switch (lcm_op->type) {
    case MTK_LCM_CB_TYPE_RUNTIME:
        LCM_MSG("[%s-%u]: func:%s, type:%s, dts_size:%u\n",
            owner, id, func_name, type_name, lcm_op->size);
        for (i = 0; i < lcm_op->size; i += 4) {
            LCM_MSG("[%s-%u][data%u~%u]>>> 0x%x 0x%x 0x%x 0x%x\n",
                owner, id, i, i + 3,
                lcm_op->param.cb_id_data.buffer_data[i],
                lcm_op->param.cb_id_data.buffer_data[i + 1],
                lcm_op->param.cb_id_data.buffer_data[i + 2],
                lcm_op->param.cb_id_data.buffer_data[i + 3]);
        }
        break;
    case MTK_LCM_CB_TYPE_RUNTIME_INPUT:
        LCM_MSG("[%s-%u]: func:%s, type:%s, dts_size:%u, id:%u\n",
            owner, id, func_name, type_name,
            lcm_op->size, lcm_op->param.cb_id_data.id[0]);
        for (i = 0; i < lcm_op->size - 1; i += 4) {
            LCM_MSG("[%s-%u][data%u~%u]>>> 0x%x 0x%x 0x%x 0x%x\n",
                owner, id, i, i + 3,
                lcm_op->param.cb_id_data.buffer_data[i],
                lcm_op->param.cb_id_data.buffer_data[i + 1],
                lcm_op->param.cb_id_data.buffer_data[i + 2],
                lcm_op->param.cb_id_data.buffer_data[i + 3]);
        }
        break;
    case MTK_LCM_CB_TYPE_RUNTIME_INPUT_MULTIPLE:
        LCM_MSG("[%s-%u]: func:%s, type:%s, dts_size:%u, id_count:%u, data_count:%u\n",
            owner, id, func_name, type_name,
            lcm_op->size, lcm_op->param.cb_id_data.id_count,
            lcm_op->param.cb_id_data.data_count);
        for (i = 0; (unsigned int)i <
            ROUNDUP(lcm_op->param.cb_id_data.id_count, 4); i += 4) {
            LCM_MSG("[%s-%u][id%u~%u]>>> 0x%x 0x%x 0x%x 0x%x\n",
                owner, id, i, i + 3,
                lcm_op->param.cb_id_data.id[i],
                lcm_op->param.cb_id_data.id[i + 1],
                lcm_op->param.cb_id_data.id[i + 2],
                lcm_op->param.cb_id_data.id[i + 3]);
        }
        for (i = 0; (unsigned int)i <
            ROUNDUP(lcm_op->param.cb_id_data.data_count, 4); i += 4) {
            LCM_MSG("[%s-%u][data%u~%u]>>> 0x%x 0x%x 0x%x 0x%x\n",
                owner, id, i, i + 3,
                lcm_op->param.cb_id_data.buffer_data[i],
                lcm_op->param.cb_id_data.buffer_data[i + 1],
                lcm_op->param.cb_id_data.buffer_data[i + 2],
                lcm_op->param.cb_id_data.buffer_data[i + 3]);
        }
        break;
    default:
        LCM_MSG("%s:[%s-%u]: invalid type:%u\n",
            __func__, owner, id, lcm_op->type);
        break;
    }
}

static void dump_lcm_ops_func_gpio(struct mtk_lcm_ops_data *lcm_op,
        const char *owner, unsigned int id)
{
    char func_name[MTK_LCM_NAME_LENGTH] = { 0 };
    char type_name[MTK_LCM_NAME_LENGTH] = { 0 };

    if (lcm_op == NULL || owner == NULL)
        return;

    switch (lcm_op->type) {
    case MTK_LCM_GPIO_TYPE_MODE:
    case MTK_LCM_GPIO_TYPE_DIR_OUTPUT:
    case MTK_LCM_GPIO_TYPE_DIR_INPUT:
    case MTK_LCM_GPIO_TYPE_OUT:
        mtk_get_func_name(lcm_op->func, &func_name[0]);
        mtk_get_type_name(lcm_op->type, &type_name[0]);
        LCM_MSG("[%s-%u]: func:%s, type:%s, dts_size:%u, gpio:%u, data:0x%x\n",
            owner, id, func_name, type_name, lcm_op->size,
            lcm_op->param.gpio_data.gpio_id,
            lcm_op->param.gpio_data.data);
        break;
    default:
        LCM_MSG("%s:[%s-%u]: invalid type:%u\n",
            __func__, owner, id, lcm_op->type);
        break;
    }
}

static void dump_lcm_ops_func_lk_prepare(struct mtk_lcm_ops_data *lcm_op,
        const char *owner, unsigned int id)
{
    char func_name[MTK_LCM_NAME_LENGTH] = { 0 };
    char type_name[MTK_LCM_NAME_LENGTH] = { 0 };

    if (lcm_op == NULL || owner == NULL)
        return;

    mtk_get_func_name(lcm_op->func, &func_name[0]);
    mtk_get_type_name(lcm_op->type, &type_name[0]);
    switch (lcm_op->type) {
    case MTK_LCM_LK_TYPE_PREPARE_PARAM_COUNT:
        LCM_MSG("[%s-%u]: func:%s, type:%s, dts_size:%u, count:%u\n",
            owner, id, func_name, type_name, lcm_op->size,
            lcm_op->param.lk_data_count);
        break;
    case MTK_LCM_LK_TYPE_PREPARE_PARAM:
        LCM_MSG("[%s-%u]: func:%s, type:%s, dts_size:%u, index:%u, data:0x%x\n",
            owner, id, func_name, type_name, lcm_op->size,
            lcm_op->param.lk_data.index,
            lcm_op->param.lk_data.data);
        break;
    case MTK_LCM_LK_TYPE_PREPARE_PARAM_FIX_BIT:
    case MTK_LCM_LK_TYPE_PREPARE_PARAM_X0_MSB_BIT:
    case MTK_LCM_LK_TYPE_PREPARE_PARAM_X0_LSB_BIT:
    case MTK_LCM_LK_TYPE_PREPARE_PARAM_X1_MSB_BIT:
    case MTK_LCM_LK_TYPE_PREPARE_PARAM_X1_LSB_BIT:
    case MTK_LCM_LK_TYPE_PREPARE_PARAM_Y0_MSB_BIT:
    case MTK_LCM_LK_TYPE_PREPARE_PARAM_Y0_LSB_BIT:
    case MTK_LCM_LK_TYPE_PREPARE_PARAM_Y1_MSB_BIT:
    case MTK_LCM_LK_TYPE_PREPARE_PARAM_Y1_LSB_BIT:
        LCM_MSG("[%s-%u]: func:%s, type:%s, dts_size:%u, index:%u, bit:%u data:0x%x\n",
            owner, id, func_name, type_name, lcm_op->size,
            lcm_op->param.lk_data_bit.index,
            lcm_op->param.lk_data_bit.bit,
            lcm_op->param.lk_data_bit.data);
        break;
    case MTK_LCM_LK_TYPE_WRITE_PARAM:
    case MTK_LCM_LK_TYPE_WRITE_PARAM_UNFORCE:
        LCM_MSG("[%s-%u]: func:%s, type:%s, dts_size:%u\n",
            owner, id, func_name, type_name, lcm_op->size);
        break;
    default:
        LCM_MSG("%s:[%s-%u]: invalid type:%u\n",
            __func__, owner, id, lcm_op->type);
        break;
    }
}

void dump_lcm_ops_func(struct mtk_lcm_ops_table *table,
        struct mtk_panel_cust *cust,
        const char *owner)
{
    struct mtk_lcm_ops_data *lcm_op = NULL;
    char owner_tmp[MTK_LCM_NAME_LENGTH] = { 0 };
    unsigned int i = 0;
    int ret = 0;

    if (owner == NULL)
        ret = snprintf(&owner_tmp[0], MTK_LCM_NAME_LENGTH - 1, "unknown");
    else
        ret = snprintf(&owner_tmp[0], MTK_LCM_NAME_LENGTH - 1, "%s", owner);

    if (ret < 0 || ret >= MTK_LCM_NAME_LENGTH)
        LCM_MSG("%s, failed, ret:%u\n", __func__, ret);

    if (table == NULL || table->size == 0) {
        LCM_MSG("%s: invalid table of \"%s\"\n", __func__, owner_tmp);
        return;
    }

    LCM_MSG("-------------%s(%u): 0x%lx-----------\n",
        owner_tmp, table->size, (unsigned long)table);
    list_for_every_entry(&table->list, lcm_op, struct mtk_lcm_ops_data, node) {
        if (lcm_op->type > MTK_LCM_UTIL_TYPE_START &&
            lcm_op->type < MTK_LCM_UTIL_TYPE_END)
            dump_lcm_ops_func_util(lcm_op, owner_tmp, i);
        else if (lcm_op->type > MTK_LCM_CMD_TYPE_START &&
            lcm_op->type < MTK_LCM_CMD_TYPE_END)
            dump_lcm_ops_func_cmd(lcm_op, owner_tmp, i);
        else if (lcm_op->type > MTK_LCM_CB_TYPE_START &&
            lcm_op->type < MTK_LCM_CB_TYPE_END)
            dump_lcm_ops_func_cb(lcm_op, owner_tmp, i);
        else if (lcm_op->type > MTK_LCM_GPIO_TYPE_START &&
            lcm_op->type < MTK_LCM_GPIO_TYPE_END)
            dump_lcm_ops_func_gpio(lcm_op, owner_tmp, i);
        else if (lcm_op->type > MTK_LCM_LK_TYPE_START &&
            lcm_op->type < MTK_LCM_LK_TYPE_END)
            dump_lcm_ops_func_lk_prepare(lcm_op, owner_tmp, i);
        else if (lcm_op->type > MTK_LCM_CUST_TYPE_START &&
            lcm_op->type < MTK_LCM_CUST_TYPE_END) {
            if (cust != NULL &&
                cust->cust_enabled != 0 &&
                cust->dump_ops != NULL)
                cust->dump_ops(lcm_op, owner_tmp, i);
        } else
            LCM_MSG("%s:[%s-%u]: invalid type:%u\n",
                __func__, owner, i, lcm_op->type);
        i++;
    }
}

static void mtk_lcm_tick_delay(unsigned int delay, lk_time_t from)
{
#ifdef LCM_SET_DISPLAY_ON_DELAY
    u32 i = 0;

    while (current_time() - from < delay) {
        i++;
        if (i % 1000 == 0)
            LCM_MSG("%s: i:%u, from_tick:%u, cur_tick:%u, delay:%u\n",
                __func__, i, from,
                current_time(), delay);
    }
    LCM_MSG("%s: delay:%u, from:%u, current:%u\n",
        __func__, delay, from, current_time());
#endif
}

int mtk_panel_delay(int func, char type, unsigned int time)
{
    int ret = 0;

    switch (func) {
    case MTK_LCM_FUNC_DBI:
        ret = mtk_lcm_dbi_delay(type, time);
        break;
    case MTK_LCM_FUNC_DPI:
        ret = mtk_lcm_dpi_delay(type, time);
        break;
    case MTK_LCM_FUNC_DSI:
        ret = mtk_lcm_dsi_delay(type, time);
        break;
    default:
        LCM_MSG("%s, invalid func:%d\n", __func__, func);
        ret = -EINVAL;
        break;
    }

    return ret;
}

static int mtk_set_location_data(struct mtk_lcm_ops_data *op,
        struct mtk_lcm_location *locate,
        unsigned int *data_array, unsigned int data_size,
        struct list_node *list)
{
    unsigned int id = 0, data = 0, off = 0;
    struct mtk_lcm_ops_data *lcm_op = op;
    char func_name[MTK_LCM_NAME_LENGTH] = { 0 };
    char type_name[MTK_LCM_NAME_LENGTH] = { 0 };
    bool skip = false, write = false;

    if (lcm_op == NULL || data_size == 0 || list == NULL)
        return -EINVAL;

    memset(data_array, 0x0, data_size);

    while (lcm_op != NULL) {
        mtk_get_func_name(lcm_op->func, &func_name[0]);
        mtk_get_type_name(lcm_op->type, &type_name[0]);
        LCM_DBG("%s, %d, func:%s/%u, type:%s/%u\n",
            __func__, __LINE__, func_name, lcm_op->func,
            type_name, lcm_op->type);
        switch (lcm_op->type) {
        case MTK_LCM_LK_TYPE_PREPARE_PARAM_FIX_BIT:
            off = lcm_op->param.lk_data_bit.bit;
            data = lcm_op->param.lk_data_bit.data << off;
            id = lcm_op->param.lk_data_bit.index;
            break;
        case MTK_LCM_LK_TYPE_PREPARE_PARAM:
            data = lcm_op->param.lk_data.data;
            id = lcm_op->param.lk_data.index;
            break;
        case MTK_LCM_LK_TYPE_PREPARE_PARAM_X0_MSB_BIT:
            if (locate == NULL) {
                LCM_MSG("%s,%d: invalid location\n",
                    __func__, __LINE__);
                return -EINVAL;
            }
            off = lcm_op->param.lk_data_bit.bit;
            data = (unsigned int)locate->x0.msb << off;
            id = lcm_op->param.lk_data_bit.index;
            break;
        case MTK_LCM_LK_TYPE_PREPARE_PARAM_X0_LSB_BIT:
            if (locate == NULL) {
                LCM_MSG("%s,%d: invalid location\n",
                    __func__, __LINE__);
                return -EINVAL;
            }
            off = lcm_op->param.lk_data_bit.bit;
            data = (unsigned int)locate->x0.lsb << off;
            id = lcm_op->param.lk_data_bit.index;
            break;
        case MTK_LCM_LK_TYPE_PREPARE_PARAM_X1_MSB_BIT:
            if (locate == NULL) {
                LCM_MSG("%s,%d: invalid location\n",
                    __func__, __LINE__);
                return -EINVAL;
            }
            off = lcm_op->param.lk_data_bit.bit;
            data = (unsigned int)locate->x1.msb << off;
            id = lcm_op->param.lk_data_bit.index;
            break;
        case MTK_LCM_LK_TYPE_PREPARE_PARAM_X1_LSB_BIT:
            if (locate == NULL) {
                LCM_MSG("%s,%d: invalid location\n",
                    __func__, __LINE__);
                return -EINVAL;
            }
            off = lcm_op->param.lk_data_bit.bit;
            data = (unsigned int)locate->x1.lsb << off;
            id = lcm_op->param.lk_data_bit.index;
            break;
        case MTK_LCM_LK_TYPE_PREPARE_PARAM_Y0_MSB_BIT:
            if (locate == NULL) {
                LCM_MSG("%s,%d: invalid location\n",
                    __func__, __LINE__);
                return -EINVAL;
            }
            off = lcm_op->param.lk_data_bit.bit;
            data = (unsigned int)locate->y0.msb << off;
            id = lcm_op->param.lk_data_bit.index;
            break;
        case MTK_LCM_LK_TYPE_PREPARE_PARAM_Y0_LSB_BIT:
            if (locate == NULL) {
                LCM_MSG("%s,%d: invalid location\n",
                    __func__, __LINE__);
                return -EINVAL;
            }
            off = lcm_op->param.lk_data_bit.bit;
            data = (unsigned int)locate->y0.lsb << off;
            id = lcm_op->param.lk_data_bit.index;
            break;
        case MTK_LCM_LK_TYPE_PREPARE_PARAM_Y1_MSB_BIT:
            if (locate == NULL) {
                LCM_MSG("%s,%d: invalid location\n",
                    __func__, __LINE__);
                return -EINVAL;
            }
            off = lcm_op->param.lk_data_bit.bit;
            data = (unsigned int)locate->y1.msb << off;
            id = lcm_op->param.lk_data_bit.index;
            break;
        case MTK_LCM_LK_TYPE_PREPARE_PARAM_Y1_LSB_BIT:
            if (locate == NULL) {
                LCM_MSG("%s,%d: invalid location\n",
                    __func__, __LINE__);
                return -EINVAL;
            }
            off = lcm_op->param.lk_data_bit.bit;
            data = (unsigned int)locate->y1.lsb << off;
            id = lcm_op->param.lk_data_bit.index;
            break;
        case MTK_LCM_LK_TYPE_WRITE_PARAM:
        case MTK_LCM_LK_TYPE_WRITE_PARAM_UNFORCE:
            write = true;
            goto end;
        default:
            skip = true;
            break;
        }

        if (skip == false) {
            if (id >= data_size) {
                LCM_MSG("%s, func:%s/%u, type:%s/%u, invalid index:%u, size:%u\n",
                    __func__, func_name, lcm_op->func,
                    type_name, lcm_op->type, id, data_size);
                return -EINVAL;
            }
            if (((data_array[id] >> off) & 0xFF) != 0)
                LCM_MSG("%s, func:%s/%u, type:%s/%u, data[%u] off:%u has been set:0x%x\n",
                    __func__, func_name, lcm_op->func,
                    type_name, lcm_op->type, id, off, data_array[id]);
            data_array[id] |= data;
        } else
            skip = false;

        lcm_op = list_next_type(list, &lcm_op->node, struct mtk_lcm_ops_data, node);
    }
end:
    if (write == false)
        LCM_MSG("%s, didn't get write\n", __func__);

    return 0;
}

int mtk_execute_func_cmd(
        struct mtk_lcm_ops_data *lcm_op,
        void *data, unsigned int size_out,
        struct list_node *list)
{
    int ret = 0;
    static unsigned int *lk_data;
    static unsigned int lk_data_count;
#if MTK_LCM_DEBUG_DUMP
    unsigned int i = 0;
#endif

    if (lcm_op == NULL) {
        LCM_MSG("%s %d: invalid cmd\n", __func__, __LINE__);
        return -EINVAL;
    }

    switch (lcm_op->type) {
    case MTK_LCM_LK_TYPE_PREPARE_PARAM_COUNT:
        LCM_MALLOC(lk_data, sizeof(u32) * lcm_op->param.lk_data_count);
        if (lk_data == NULL) {
            LCM_MSG("%s,%d: failed to allocate data array\n",
                __func__, __LINE__);
            return -ENOMEM;
        }
        ret = mtk_set_location_data(lcm_op, (struct mtk_lcm_location *)data,
                lk_data, lcm_op->param.lk_data_count, list);
        if (ret < 0) {
            LCM_MSG("%s: failed to set localtion data:%u\n",
                __func__, ret);
            LCM_FREE(lk_data, sizeof(u32) * lcm_op->param.lk_data_count);
            return ret;
        }
        lk_data_count = lcm_op->param.lk_data_count;
#if MTK_LCM_DEBUG_DUMP
        LCM_MSG("%s,%d: prepare %u params\n", __func__, __LINE__, lk_data_count);
        for (i = 0; i < lk_data_count; i++)
            LCM_MSG("%s,%d: >>> param[%u]=0x%08x\n", __func__, __LINE__, i, lk_data[i]);
#endif
        break;
    case MTK_LCM_LK_TYPE_WRITE_PARAM:
        if (lk_data == NULL || lk_data_count == 0) {
            LCM_MSG("%s: invalid lk data para, count:%u\n",
                __func__, lk_data_count);
            return -EINVAL;
        }
#if MTK_LCM_DEBUG_DUMP
        LCM_MSG("%s, %d, lk_data_count:%u\n", __func__, __LINE__, lk_data_count);
        for (i = 0; i < lk_data_count; i++)
            LCM_MSG("%s, %d, i:%u, data:0x%08x\n", __func__, __LINE__, i, lk_data[i]);
#endif
        ret = mtk_panel_dsi_dcs_write_lk(&lk_data[0], lk_data_count, 1);
        lk_data_count = 0;
        LCM_FREE(lk_data, sizeof(u32) * lcm_op->param.lk_data_count);
        break;
    case MTK_LCM_LK_TYPE_WRITE_PARAM_UNFORCE:
        if (lk_data == NULL || lk_data_count == 0) {
            LCM_MSG("%s: invalid lk data para, count:%u\n",
                __func__, lk_data_count);
            return -EINVAL;
        }
#if MTK_LCM_DEBUG_DUMP
        LCM_MSG("%s, %d, lk_data_count:%u\n", __func__, __LINE__, lk_data_count);
        for (i = 0; i < lk_data_count; i++)
            LCM_MSG("%s, %d, i:%u, data:0x%08x\n", __func__, __LINE__, i, lk_data[i]);
#endif
        ret = mtk_panel_dsi_dcs_write_lk(&lk_data[0], lk_data_count, 0);
        lk_data_count = 0;
        LCM_FREE(lk_data, sizeof(u32) * lcm_op->param.lk_data_count);
        break;
    case MTK_LCM_LK_TYPE_PREPARE_PARAM:
    case MTK_LCM_LK_TYPE_PREPARE_PARAM_FIX_BIT:
    case MTK_LCM_LK_TYPE_PREPARE_PARAM_X0_MSB_BIT:
    case MTK_LCM_LK_TYPE_PREPARE_PARAM_X0_LSB_BIT:
    case MTK_LCM_LK_TYPE_PREPARE_PARAM_X1_MSB_BIT:
    case MTK_LCM_LK_TYPE_PREPARE_PARAM_X1_LSB_BIT:
    case MTK_LCM_LK_TYPE_PREPARE_PARAM_Y0_MSB_BIT:
    case MTK_LCM_LK_TYPE_PREPARE_PARAM_Y0_LSB_BIT:
    case MTK_LCM_LK_TYPE_PREPARE_PARAM_Y1_MSB_BIT:
    case MTK_LCM_LK_TYPE_PREPARE_PARAM_Y1_LSB_BIT:
        /* handling in mtk_lcm_set_location */
        break;
    case MTK_LCM_CMD_TYPE_WRITE_BUFFER:
    case MTK_LCM_CMD_TYPE_WRITE_CMD:
        ret = mtk_panel_dsi_dcs_write_buffer(
                    &lcm_op->param.cmd_data, 1);
        break;
    case MTK_LCM_CMD_TYPE_READ_BUFFER:
    case MTK_LCM_CMD_TYPE_READ_CMD:
    {
        u8 *data_temp = data;
        unsigned int offset = lcm_op->param.cmd_data.start_id;
        unsigned int len = lcm_op->param.cmd_data.data_len;
        unsigned int i = 0;

        if (data_temp == NULL ||
            size_out < offset + len) {
            LCM_MSG("%s: out buffer is not enough, size_out:%u, off:%u, len:%u\n",
                __func__, size_out, offset, len);
            return -ENOMEM;
        }

        LCM_MALLOC(lcm_op->param.cmd_data.data, len + 1);
        if (lcm_op->param.cmd_data.data == NULL) {
            LCM_MSG("%s,%d: failed to allocate data, len:%u\n",
                __func__, __LINE__, len);
            return -ENOMEM;
        }

        ret = mtk_panel_dsi_dcs_read_buffer(
                    &lcm_op->param.cmd_data);

        for (i = 0; i < len; i++)
            data_temp[i + offset] = lcm_op->param.cmd_data.data[i];
        break;
    }
    default:
        ret = -EINVAL;
        LCM_MSG("%s: invalid func:%u\n", __func__, lcm_op->type);
        break;
    }

    return ret;
}

int mtk_lcm_drv_power_reset(unsigned int func, unsigned int value)
{
    int ret = 0;

    switch (func) {
    case MTK_LCM_FUNC_DSI:
        ret = mtk_lcm_dsi_power_reset(value);
        break;
    case MTK_LCM_FUNC_DBI:
    case MTK_LCM_FUNC_DPI:
        LCM_MSG("%s, not support func:%d\n", __func__, func);
        break;
    default:
        LCM_MSG("%s, invalid func:%d\n", __func__, func);
        break;
    }

    return ret;
}

int mtk_lcm_drv_power_on(unsigned int func)
{
    int ret = 0;

    switch (func) {
    case MTK_LCM_FUNC_DSI:
        ret = mtk_lcm_dsi_power_on();
        break;
    case MTK_LCM_FUNC_DBI:
    case MTK_LCM_FUNC_DPI:
        LCM_MSG("%s, not support func:%d\n", __func__, func);
        break;
    default:
        LCM_MSG("%s, invalid func:%d\n", __func__, func);
        break;
    }

    return ret;
}

int mtk_lcm_drv_power_off(unsigned int func)
{
    int ret = 0;

    switch (func) {
    case MTK_LCM_FUNC_DSI:
        ret = mtk_lcm_dsi_power_off();
        break;
    case MTK_LCM_FUNC_DBI:
    case MTK_LCM_FUNC_DPI:
        LCM_MSG("%s, not support func:%d\n", __func__, func);
        break;
    default:
        LCM_MSG("%s, invalid func:%d\n", __func__, func);
        break;
    }

    return ret;
}

int mtk_execute_func_util(struct mtk_lcm_ops_data *lcm_op, lk_time_t from)
{
    unsigned int data = 0;
    int ret = 0;

    if (lcm_op == NULL) {
        LCM_MSG("%s %d: invalid parameters\n",
            __func__, __LINE__);
        return -EINVAL;
    }
    data = lcm_op->param.util_data;

    switch (lcm_op->type) {
    case MTK_LCM_UTIL_TYPE_RESET:
        ret = mtk_lcm_drv_power_reset(lcm_op->func, data);
        if (ret < 0)
            LCM_MSG("%s, reset failed, %d\n",
                __func__, ret);
        break;
    case MTK_LCM_UTIL_TYPE_POWER_ON:
        ret = mtk_lcm_drv_power_on(lcm_op->func);
        if (ret < 0)
            LCM_MSG("%s, power on failed, %d\n",
                __func__, ret);
        break;
    case MTK_LCM_UTIL_TYPE_POWER_OFF:
        ret = mtk_lcm_drv_power_off(lcm_op->func);
        if (ret < 0)
            LCM_MSG("%s, power off failed, %d\n",
                __func__, ret);
        break;
    case MTK_LCM_UTIL_TYPE_POWER_VOLTAGE:
        ret = mtk_lcm_gateic_set_voltage(data, lcm_op->func);
        if (ret < 0)
            LCM_MSG("%s, set voltage:%u failed, %d\n",
                __func__, data, ret);
        break;
    case MTK_LCM_UTIL_TYPE_MDELAY:
    case MTK_LCM_UTIL_TYPE_UDELAY:
        mtk_panel_delay(lcm_op->func, lcm_op->type,
            lcm_op->param.delay_data.timeout);
        break;
    case MTK_LCM_UTIL_TYPE_TDELAY:
        mtk_lcm_tick_delay(lcm_op->param.delay_data.timeout,
                from);
        break;
    default:
        ret = -EINVAL;
        LCM_MSG("%s: invalid type:%u\n",
            __func__, lcm_op->type);
        break;
    }

    return ret;
}

int mtk_execute_func_gpio(struct mtk_lcm_ops_data *lcm_op,
        const char **lcm_pinctrl_name,
        unsigned int pinctrl_count)
{
    unsigned int id = 0, data = 0;
    int ret = 0;

    if (lcm_op == NULL) {
        LCM_MSG("%s %d: invalid cmd\n",
            __func__, __LINE__);
        return -EINVAL;
    }

    id = lcm_op->param.gpio_data.gpio_id;
    data = lcm_op->param.gpio_data.data;
    if (id >= pinctrl_count) {
        LCM_MSG("%s %d: invalid parameter:%u-%u\n",
            __func__, __LINE__, id, pinctrl_count);
        return -EINVAL;
    }

    switch (lcm_op->type) {
    case MTK_LCM_GPIO_TYPE_MODE:
        ret = mt_set_gpio_mode(id, data);
        break;
    case MTK_LCM_GPIO_TYPE_DIR_OUTPUT:
        ret = mt_set_gpio_dir(id, 1);
        break;
    case MTK_LCM_GPIO_TYPE_DIR_INPUT:
        ret = mt_set_gpio_dir(id, 0);
        break;
    case MTK_LCM_GPIO_TYPE_OUT:
        ret = mt_set_gpio_out(id, data);
        break;
    default:
        ret = -EINVAL;
        LCM_MSG("%s: invalid type:%u\n",
            __func__, lcm_op->type);
        break;
    }

    return ret;
}

int mtk_is_lcm_read_ops(unsigned int type)
{
    if (type == MTK_LCM_CMD_TYPE_READ_CMD ||
        type == MTK_LCM_CMD_TYPE_READ_BUFFER)
        return 1;

    return 0;
}

int mtk_panel_execute_operation(struct mtk_lcm_ops_table *table,
        const struct mtk_panel_resource *panel_resource,
        void *data, unsigned int data_size, const char *owner)
{
    struct mtk_lcm_ops_data *op = NULL;
    unsigned int i = 0;
    char owner_tmp[MTK_LCM_NAME_LENGTH] = { 0 };
    char func_name[MTK_LCM_NAME_LENGTH] = { 0 };
    char type_name[MTK_LCM_NAME_LENGTH] = { 0 };
    int ret = 0;

    if (owner == NULL)
        ret = snprintf(&owner_tmp[0], MTK_LCM_NAME_LENGTH - 1, "unknown");
    else
        ret = snprintf(&owner_tmp[0], MTK_LCM_NAME_LENGTH - 1, "%s", owner);
    if (ret < 0 || ret >= MTK_LCM_NAME_LENGTH)
        LCM_MSG("%s, snprintf failed, ret:%d\n", __func__, ret);

    if (table == NULL ||
        table->size == 0 ||
        table->size >= MTK_PANEL_TABLE_OPS_COUNT) {
        LCM_MSG("%s: %s, invalid table size:%u\n",
            __func__, owner_tmp, table->size);
        return 0;
    }

    list_for_every_entry(&table->list, op, struct mtk_lcm_ops_data, node) {
        if (op == NULL) {
            LCM_MSG("%s: owner_tmp:%s, invalid op\n",
                __func__, owner_tmp);
            ret = -EINVAL;
            break;
        }

        mtk_get_func_name(op->func, &func_name[0]);
        if (op->func != MTK_LCM_FUNC_DSI) {
            LCM_MSG("%s %d: not support:%s-%d\n",
                __func__, __LINE__, func_name, op->func);
            return -EINVAL;
        }

        mtk_get_type_name(op->type, &type_name[0]);
#if MTK_LCM_DEBUG_DUMP
        LCM_MSG("%s: [%s+%u]: func:%s/%u, type:%s/%u\n",
            __func__, owner_tmp, i,
            func_name, op->func,
            type_name, op->type);
#endif
        if (op->type > MTK_LCM_UTIL_TYPE_START &&
            op->type < MTK_LCM_UTIL_TYPE_END) {
            if (data == NULL)
                ret = mtk_execute_func_util(op, 0);
            else
                ret = mtk_execute_func_util(op, *(lk_time_t *)data);
        } else if (op->type > MTK_LCM_CMD_TYPE_START &&
            op->type < MTK_LCM_CMD_TYPE_END)
            ret = mtk_execute_func_cmd(op, (u8 *)data, data_size, &table->list);
        else if (op->type > MTK_LCM_LK_TYPE_START &&
            op->type < MTK_LCM_LK_TYPE_END)
            ret = mtk_execute_func_cmd(op, (u8 *)data, data_size, &table->list);
#if 0
        else if (op->type > MTK_LCM_GPIO_TYPE_START &&
            op->type < MTK_LCM_GPIO_TYPE_END)
            mtk_execute_func_gpio(op,
                     panel_resource->ops.dsi_ops->lcm_pinctrl_name,
                     panel_resource->ops.dsi_ops->lcm_pinctrl_count);
#endif
        else if (op->type > MTK_LCM_CUST_TYPE_START &&
            op->type < MTK_LCM_CUST_TYPE_END) {
             if (panel_resource->cust.cust_enabled == 0 ||
                panel_resource->cust.func == NULL)
                return -EINVAL;
             ret = panel_resource->cust.func(op, (u8 *)data, data_size);
        } else {
            ret = -EINVAL;
        }


        if (ret < 0) {
            LCM_MSG("%s: [%s+%u] func:%s/%u, type:%s/%u, failed:%d\n",
                __func__, owner_tmp, i,
                func_name, op->func,
                type_name, op->type, ret);
            break;
        }
        i++;
    }

    //LCM_DBG("%s:%s total:%u --\n", __func__, owner_tmp, i);
    return ret;
}

int mtk_panel_get_i2c_lcd_bias(const void *fdt, int *id, int *addr)
{
    int offset;
    unsigned int *data = NULL;
    int len = 0;

    if (fdt == NULL) {
        LCM_MSG("%s: failed to get fdt\n", __func__);
        return -EINVAL;
    }
    offset = fdt_node_offset_by_compatible(fdt,
            -1, "mediatek,i2c_lcd_bias");
    if (offset < 0) {
        LCM_MSG("%s: failed to get i2c_lcd_bias node, %d\n",
            __func__, offset);
        return -EFAULT;
    }

    data = (unsigned int *)fdt_getprop(fdt,
            offset, "reg", &len);
    if (data == NULL || len <= 0) {
        LCM_MSG("%s: failed to get i2c_lcd_bias reg, len:%d\n",
            __func__, len);
        return -EFAULT;
    }
    *addr = fdt32_to_cpu(*data);

    data = (unsigned int *)fdt_getprop(fdt,
            offset, "id", &len);
    if (data == NULL || len <= 0) {
        LCM_MSG("%s: get i2c_lcd_bias id fail, len:%d\n",
            __func__, len);
        return -EFAULT;
    }
    *id = fdt32_to_cpu(*data);

    LCM_DBG("%s, %d, id:0x%x, addr:0x%x\n",
         __func__, __LINE__, *id, *addr);
    return 0;
}

int mtk_panel_get_pin(const void *fdt, const char *pin_name)
{
    int offset, idx, phandle;
    unsigned int *data = NULL;
    int len = 0, ret = 0;
    char pinctrl_name[128] = {'\0'};

    if (fdt == NULL) {
        LCM_MSG("%s: failed to get fdt\n", __func__);
        return -EINVAL;
    }

    offset = fdt_node_offset_by_compatible(fdt,
            -1, "mediatek,mtkfb");
    if (offset < 0) {
        LCM_MSG("%s: get fdt mtkfb node fail, %d\n",
            __func__, offset);
        return -1;
    }

    idx = fdt_stringlist_search(fdt,
            offset, "pinctrl-names", pin_name);
    if (idx < 0) {
        LCM_MSG("%s: get idx fail. pinctrl_name:%s, idx:%d\n",
            __func__, pinctrl_name, idx);
        return -1;
    }

    ret = snprintf(pinctrl_name, (sizeof(pinctrl_name) - 1),
        "pinctrl-%d", idx);
    if (ret < 0 || (size_t)ret >= sizeof(pinctrl_name))
        LCM_MSG("%s, snprintf failed, ret:%d\n", __func__, ret);
    data = (unsigned int *)fdt_getprop(fdt,
            offset, pinctrl_name, &len);
    if (data == NULL || len <= 0) {
        LCM_MSG("%s: get lcm pinctrl fail. pinctrl_name:%s, len:%d\n",
            __func__, pinctrl_name, len);
        return -1;
    }
    phandle = fdt32_to_cpu(*data);

    offset = fdt_node_offset_by_phandle(fdt,
            phandle);
    if (offset < 0) {
        LCM_MSG("%s: get lcm pinctrl fail. phandle:0x%x, %d\n",
            __func__, phandle, offset);
        return -1;
    }

    offset = fdt_subnode_offset(fdt,
            offset, "pins_cmd_dat");
    if (offset < 0) {
        LCM_MSG("%s: get pins_cmd_dat node fail, %d\n",
            __func__, offset);
        return -1;
    }

    data = (unsigned int *)fdt_getprop(fdt,
            offset, "pinmux", &len);
    if (data == NULL || len <= 0) {
        LCM_MSG("%s: get pinmux fail, len:%d\n",
            __func__, len);
        return -1;
    }

    /* Pin define format: 0~7->pin status, 8~15->pin id */
    return (fdt32_to_cpu(*data) >> 8);
}

void dump_lcm_params_basic(LCM_PARAMS *params)
{
    unsigned int i = 0;

    if (params == NULL) {
        LCM_MSG("%s: invalid lcm params\n", __func__);
        return;
    }

    LCM_MSG("=========== LCM DUMP: basic params ==============\n");
    LCM_MSG("type=%u, ctrl=%u, lcm_if=%u lcm_cmd_if=%u\n",
        params->type, params->ctrl,
        params->lcm_if, params->lcm_cmd_if);

    LCM_MSG("resolution=(%u,%u) lcm_pos=(%u,%u)\n",
        params->width, params->height,
        params->lcm_x, params->lcm_y);

    LCM_MSG("virtual_resolution=(%u,%u) io_mode=%u\n",
        params->virtual_width, params->virtual_height,
        params->io_select_mode);

    LCM_MSG("physical_resolution=(%u,%u) od_table_size=%u, od_table:\n",
        params->physical_width, params->physical_height,
        params->od_table_size);

    if (params->od_table_size > 0) {
        if (params->od_table != NULL) {
            for (i = 0; i < params->od_table_size; i++) {
                LCM_MSG("%u ",
                    *((unsigned int *)params->od_table + i));
                if (i % 21 == 0)
                    LCM_MSG("\n");
            }
        }
    }

    LCM_MSG("================================================\n");
    return;
}

#ifdef MTK_ROUND_CORNER_SUPPORT
/* dump dsi fps params */
void dump_lcm_round_corner_params(LCM_ROUND_CORNER *params)
{
    unsigned int i = 0;

    LCM_MSG("=========== LCM DUMP: round corner params ==============\n");
    LCM_MSG("enable=%u, width=%u-%u, height=%u-%u\n",
        params->round_corner_en,
        params->w, params->w_bot,
        params->h, params->h_bot);
    LCM_MSG("notch=%u, full_content=%u, top_size=(%u,l:%u,r:%u) bottom_size=%u\n",
        params->is_notch, params->full_content,
        params->tp_size, params->tp_size_l,
        params->tp_size_r, params->bt_size);

    LCM_MSG("top(lt=0x%lx,lt_l=0x%lx,lt_r=0x%lx)\n",
        (unsigned long)params->lt_addr,
        (unsigned long)params->lt_addr_l,
        (unsigned long)params->lt_addr_r);

    LCM_MSG("bottom(lb=0x%lx,rb=0x%lx)\n",
        (unsigned long)params->lb_addr,
        (unsigned long)params->rb_addr);

    LCM_MSG("=================================================\n");
}
#endif

void mtk_lcm_dump_all(char func, struct mtk_panel_resource *resource,
        struct mtk_panel_cust *cust)
{
    dump_lcm_params_basic(&resource->params);
#ifdef MTK_ROUND_CORNER_SUPPORT
    dump_lcm_round_corner_params(&resource->params.round_corner_params);
#endif

    switch (func) {
    case MTK_LCM_FUNC_DBI:
        dump_lcm_params_dbi(&resource->params.dbi, &resource->cust);
        dump_lcm_ops_dbi(resource->ops.dbi_ops,
                &resource->params.dbi, &resource->cust);
        break;
    case MTK_LCM_FUNC_DPI:
        dump_lcm_params_dpi(&resource->params.dpi, &resource->cust);
        dump_lcm_ops_dpi(resource->ops.dpi_ops,
                &resource->params.dpi, &resource->cust);
        break;
    case MTK_LCM_FUNC_DSI:
        dump_lcm_params_dsi(&resource->params.dsi, &resource->cust);
        dump_lcm_ops_dsi(resource->ops.dsi_ops,
                &resource->params.dsi, &resource->cust);
        break;
    default:
        LCM_MSG("%s, invalid func:%d\n", __func__, func);
        break;
    }
}

unsigned int mtk_lcm_get_drv_count(char func)
{
    switch (func) {
    case MTK_LCM_FUNC_DBI:
        return mtk_lcm_get_dbi_count();
    case MTK_LCM_FUNC_DPI:
        return mtk_lcm_get_dpi_count();
    case MTK_LCM_FUNC_DSI:
        return mtk_lcm_get_dsi_count();
    default:
        LCM_MSG("%s, invalid func:%d\n", __func__, func);
        break;
    }

    return 0;
}

const char *mtk_lcm_get_drv_name(char func, unsigned int id)
{
    switch (func) {
    case MTK_LCM_FUNC_DBI:
        return mtk_lcm_get_dbi_name(id);
    case MTK_LCM_FUNC_DPI:
        return mtk_lcm_get_dpi_name(id);
    case MTK_LCM_FUNC_DSI:
        return mtk_lcm_get_dsi_name(id);
    default:
        LCM_MSG("%s, invalid func:%d\n", __func__, func);
        break;
    }

    return NULL;
}

static void free_lcm_ops_data(struct mtk_lcm_ops_data *lcm_op)
{
    if (lcm_op == NULL)
        return;

    switch (lcm_op->type) {
    case MTK_LCM_CMD_TYPE_WRITE_BUFFER:
    case MTK_LCM_CMD_TYPE_WRITE_CMD:
        if (lcm_op->param.cmd_data.data != NULL &&
            lcm_op->size > 0) {
            LCM_FREE(lcm_op->param.cmd_data.data,
                 lcm_op->size);
            lcm_op->size = 0;
        }
        break;
    case MTK_LCM_CMD_TYPE_READ_BUFFER:
    case MTK_LCM_CMD_TYPE_READ_CMD:
        if (lcm_op->param.cmd_data.data != NULL &&
            lcm_op->size > 2) {
            LCM_FREE(lcm_op->param.cmd_data.data,
                lcm_op->size - 2);
            lcm_op->size = 0;
        }
        break;
    case MTK_LCM_CB_TYPE_RUNTIME:
        if (lcm_op->param.cb_id_data.buffer_data != NULL &&
            lcm_op->size > 0) {
            LCM_FREE(lcm_op->param.cb_id_data.buffer_data,
                lcm_op->size + 1);
            lcm_op->size = 0;
        }
        break;
    case MTK_LCM_CB_TYPE_RUNTIME_INPUT:
    case MTK_LCM_CB_TYPE_RUNTIME_INPUT_MULTIPLE:
        if (lcm_op->param.cb_id_data.id != NULL &&
            lcm_op->param.cb_id_data.id_count > 0) {
            LCM_FREE(lcm_op->param.cb_id_data.id,
                lcm_op->param.cb_id_data.id_count + 1);
            lcm_op->param.cb_id_data.id_count = 0;
        }
        if (lcm_op->param.cb_id_data.buffer_data != NULL &&
            lcm_op->param.cb_id_data.data_count > 0) {
            LCM_FREE(lcm_op->param.cb_id_data.buffer_data,
                lcm_op->param.cb_id_data.data_count + 1);
            lcm_op->param.cb_id_data.data_count = 0;
        }
        break;
    default:
        LCM_DBG("%s, %d, invalid cmd:%u\n",
             __func__, __LINE__, lcm_op->type);
        break;
    }
}

void free_lcm_ops_table(struct mtk_lcm_ops_table *table, const char *func)
{
    struct list_node *t_node = NULL;
    struct mtk_lcm_ops_data *op = NULL;

    if (table == NULL || table->size == 0)
        return;

    while (list_is_empty(&table->list) == false) {
        t_node = list_remove_tail(&table->list);
        op = containerof(t_node, struct mtk_lcm_ops_data, node);
        free_lcm_ops_data(op);
        LCM_FREE(op, sizeof(struct mtk_lcm_ops_data));
    }
    table->size = 0;

    if (func != NULL)
        LCM_DBG("%s--, %d, func:%s, total_size:%lluByte\n",
            __func__, __LINE__, func, mtk_lcm_total_size);
    else
        LCM_DBG("%s--, %d, func:unknown, total_size:%lluByte\n",
            __func__, __LINE__, mtk_lcm_total_size);
}

static void free_lcm_params_basic(LCM_PARAMS *params)
{
    if (params == NULL)
        return;

    if (params->od_table_size > 0 &&
        params->od_table != NULL) {
        LCM_FREE(params->od_table, sizeof(u32) *
                (params->od_table_size + 1));
        params->od_table_size = 0;
    }
}

static void free_lcm_params(char func, LCM_PARAMS *params)
{
    if (params == NULL)
        return;

    switch (func) {
    case MTK_LCM_FUNC_DBI:
        free_lcm_params_dbi(&params->dbi);
        break;
    case MTK_LCM_FUNC_DPI:
        free_lcm_params_dpi(&params->dpi);
        break;
    case MTK_LCM_FUNC_DSI:
        free_lcm_params_dsi(&params->dsi);
        break;
    default:
        break;
    }
    free_lcm_params_basic(params);
}

static void free_lcm_ops(char func, struct mtk_lcm_ops *ops)
{
    if (ops == NULL)
        return;

    switch (func) {
    case MTK_LCM_FUNC_DBI:
        free_lcm_ops_dbi(ops->dbi_ops);
        ops->dbi_ops = NULL;
        break;
    case MTK_LCM_FUNC_DPI:
        free_lcm_ops_dpi(ops->dpi_ops);
        ops->dpi_ops = NULL;
        break;
    case MTK_LCM_FUNC_DSI:
        free_lcm_ops_dsi(ops->dsi_ops);
        ops->dsi_ops = NULL;
        break;
    default:
        break;
    }
}

void free_lcm_resource(char func, struct mtk_panel_resource *data)
{
    if (data == NULL)
        return;

    LCM_DBG("%s, %d++\n", __func__, __LINE__);
    free_lcm_ops(func, &data->ops);
    free_lcm_params(func, &data->params);

    if (data->cust.cust_enabled == 1) {
        if (data->cust.free_ops != NULL)
            data->cust.free_ops(func);
        if (data->cust.free_params != NULL)
            data->cust.free_params(func);
    }
    LCM_FREE(data, sizeof(struct mtk_panel_resource));
}

int mtk_lcm_list_count(const char *key)
{
    unsigned int count = 0, i = 0;
    char *lcm_define = NULL;
    char *p_start = NULL;
    unsigned int length = strlen(MTK_LCM_LIST_SUPPORT) + 1;
    unsigned int key_len = 0;

    /*back up lcm list define*/
    if (length <= key_len || key == NULL) {
        LCM_MSG("%s, invalid lcm list,%u or key\n", __func__, length);
        return -EFAULT;
    }
    key_len = strlen(key) + 1;
    LCM_MSG("%s, lcm defines:\"%s\", len:%u, key:\"%s\", len:%u\n",
        __func__, MTK_LCM_LIST_SUPPORT, length, key, key_len);

    LCM_MALLOC(lcm_define, length + 1);
    if (lcm_define == NULL) {
        LCM_MSG("%s, failed to allocate buffer\n", __func__);
        return -ENOMEM;
    }

    p_start = strncpy(lcm_define, MTK_LCM_LIST_SUPPORT, length);
    if (p_start == NULL) {
        LCM_MSG("%s, strcpy failed, length:%u\n",
            __func__, length);
        goto end;
    }
    for (i = 0; i < length; i++)
        lcm_define[i] = tolower(lcm_define[i]);

    /*calculate lcm count*/
    do {
        p_start = strstr(p_start, key);
        if (p_start == NULL)
            break;
        count++;
        p_start = strchr(p_start, ',');
    } while (p_start != NULL && strlen(p_start) > key_len);

end:
    LCM_FREE(lcm_define, length + 1);
    return count;
}

int mtk_lcm_list_parsing(char *out_value, unsigned int count, const char *key)
{
    unsigned int i = 0;
    char *lcm_define = NULL;
    char *p_start = NULL, *p_end = NULL;
    unsigned int length = strlen(MTK_LCM_LIST_SUPPORT) + 1;
    unsigned int key_len = 0;
    int size = 0;

    /*back up lcm list define*/
    if (length <= key_len || count == 0 ||
        out_value == NULL || key == NULL) {
        LCM_MSG("%s, invalid lcm list,len:%u, count:%u\n",
            __func__, length, count);
        return -EFAULT;
    }
    key_len = strlen(key) + 1;

    LCM_MALLOC(lcm_define, length + 1);
    if (lcm_define == NULL) {
        LCM_MSG("%s, failed to allocate buffer\n", __func__);
        return -ENOMEM;
    }

    p_start = strncpy(lcm_define, MTK_LCM_LIST_SUPPORT, length);
    if (p_start == NULL) {
        LCM_MSG("%s, strcpy failed, length:%u\n",
            __func__, length);
        goto end;
    }
    for (i = 0; i < length; i++)
        lcm_define[i] = tolower(lcm_define[i]);

    /*update lcm list*/
    for (i = 0; i < count; i++) {
        p_start = strstr(p_start, key);
        if (p_start == NULL) {
            LCM_MSG("%s, failed to get next lcm define, i:%u\n",
                __func__, i);
            count = i;
            break;
        }
        p_end = strchr(p_start, ',');
        if (p_end == NULL)
            size = strlen(p_start) - key_len + 1;
        else
            size = p_end - p_start - key_len + 1;

        if (size < 0 || size >= MTK_LCM_NAME_LENGTH) {
            LCM_MSG("%s, i:%u, invalid lcm name size, size:%d\n",
                __func__, i, size);
            strncpy(out_value + i * MTK_LCM_NAME_LENGTH,
                "<null>", sizeof("<null>"));
        } else {
            strncpy(out_value + i * MTK_LCM_NAME_LENGTH,
                p_start + key_len - 1, size);
            LCM_DBG("%s, i:%u, update lcm name:\"%s\", size:%d\n",
                __func__, i,
                out_value + i * MTK_LCM_NAME_LENGTH, size);
        }
        p_start = p_end;
    }

end:
    LCM_FREE(lcm_define, length + 1);
    return count;
}

int mtk_lcm_list_init(char func)
{
    int ret = 0;

    switch (func) {
    case MTK_LCM_FUNC_DBI:
        ret = mtk_dbi_lcm_list_init();
        break;
    case MTK_LCM_FUNC_DPI:
        ret = mtk_dpi_lcm_list_init();
        break;
    case MTK_LCM_FUNC_DSI:
        ret = mtk_dsi_lcm_list_init();
        break;
    default:
        ret = -EINVAL;
        break;
    }

    return ret;
}

