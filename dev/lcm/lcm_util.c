/*
* Copyright (c) 2019 MediaTek Inc.
*
* Permission is hereby granted, free of charge, to any person obtaining
* a copy of this software and associated documentation files
* (the "Software"), to deal in the Software without restriction,
* including without limitation the rights to use, copy, modify, merge,
* publish, distribute, sublicense, and/or sell copies of the Software,
* and to permit persons to whom the Software is furnished to do so,
* subject to the following conditions:
*
* The above copyright notice and this permission notice shall be
* included in all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
* TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
* SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include <debug.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <platform.h>
/* #include <platform/mt_typedefs.h> */

#include "lcm_define.h"
#include "lcm_drv.h"
#include "lcm_util.h"

#if DEVICE_TREE_SUPPORT
#include <libfdt.h>

int lcm_util_get_i2c_lcd_bias(int *id, int *addr, void *fdt)
{
    int offset;
    unsigned int *data = NULL;
    int len = 0;

    if (fdt == NULL)
        return -1;

    offset = fdt_node_offset_by_compatible(fdt, -1, "mediatek,i2c_lcd_bias");
    if (offset < 0) {
        dprintf(0, "get fdt i2c_lcd_bias node fail\n");
        return -1;
    }
    data = (unsigned int *)fdt_getprop(fdt, offset, "reg", &len);
    if (!data || !len) {
        dprintf(0, "get i2c_lcd_bias reg fail\n");
        return -1;
    }
    *addr = fdt32_to_cpu(*(unsigned int *)data);
    offset = fdt_parent_offset(fdt, offset);
    data = (unsigned int *)fdt_getprop(fdt, offset, "id", &len);
    if (!data || !len) {
        dprintf(0, "get i2c_lcd_bias id fail\n");
        return -1;
    }
    *id = fdt32_to_cpu(*(unsigned int *)data);
    return 0;
}

int lcm_util_get_pin(const char *pin_name, void *fdt)
{
    int offset, idx, phandle;
    unsigned int *data = NULL;
    int len = 0;
    char pinctrl_name[128] = {'\0'};

    if (fdt == NULL)
        return -1;

    offset = fdt_node_offset_by_compatible(fdt, -1, "mediatek,mtkfb");
    if (offset < 0) {
        dprintf(INFO, "get fdt mtkfb node fail\n");
        return -1;
    }
    idx = fdt_stringlist_search(fdt, offset, "pinctrl-names", pin_name);
    if (idx < 0) {
        dprintf(INFO, "get idx fail. pinctrl_name:%s\n", pinctrl_name);
        return -1;
    }
    snprintf(pinctrl_name, (sizeof(pinctrl_name) - 1), "pinctrl-%d", idx);
    data = (unsigned int *)fdt_getprop(fdt, offset, pinctrl_name, &len);
    if (!data || !len) {
        dprintf(INFO, "get lcm pinctrl fail. pinctrl_name:%s\n", pinctrl_name);
        return -1;
    }
    phandle = fdt32_to_cpu(*(unsigned int *)data);

    offset = fdt_node_offset_by_phandle(fdt, phandle);
    if (offset < 0) {
        dprintf(INFO, "get lcm pinctrl fail. phandle:0x%x\n", phandle);
        return -1;
    }

    offset = fdt_subnode_offset(fdt, offset, "pins_cmd_dat");
    if (offset < 0) {
        dprintf(INFO, "get pins_cmd_dat node fail\n");
        return -1;
    }
    data = (unsigned int *)fdt_getprop(fdt, offset, "pinmux", &len);
    if (!data || !len) {
        dprintf(INFO, "get pinmux fail\n");
        return -1;
    }

    /* Pin define format: 0~7->pin status, 8~15->pin id */
    return fdt32_to_cpu(*(unsigned int *)data) >> 8;
}
#else
int lcm_util_get_i2c_lcd_bias(int *id, int *addr, void *fdt)
{
    return -1;
}
int lcm_util_get_pin(const char *pin_name, void *fdt)
{
    return -1;
}
#endif

#if defined(MTK_LCM_DEVICE_TREE_SUPPORT)
static LCM_STATUS _lcm_util_check_data(char type, const LCM_DATA_T1 *t1)
{
    switch (type) {
    case LCM_UTIL_RESET:
        switch (t1->data) {
        case LCM_UTIL_RESET_LOW:
        case LCM_UTIL_RESET_HIGH:
            break;

        default:
            return LCM_STATUS_ERROR;
        }
        break;

    case LCM_UTIL_MDELAY:
    case LCM_UTIL_UDELAY:
        // no limitation
        break;

    default:
        return LCM_STATUS_ERROR;
    }


    return LCM_STATUS_OK;
}


static LCM_STATUS _lcm_util_check_write_cmd_v1(const LCM_DATA_T5 *t5)
{
    if (t5 == NULL) {
        return LCM_STATUS_ERROR;
    }

    if (t5->size == 0) {
        return LCM_STATUS_ERROR;
    }

    return LCM_STATUS_OK;
}


static LCM_STATUS _lcm_util_check_write_cmd_v2(const LCM_DATA_T3 *t3)
{
    if (t3 == NULL) {
        return LCM_STATUS_ERROR;
    }

    return LCM_STATUS_OK;
}


static LCM_STATUS _lcm_util_check_write_cmd_v23(const LCM_DATA_T3 *t3)
{
    if (t3 == NULL)
        return LCM_STATUS_ERROR;

    return LCM_STATUS_OK;
}


static LCM_STATUS _lcm_util_check_read_cmd_v2(const LCM_DATA_T4 *t4)
{
    if (t4 == NULL) {
        return LCM_STATUS_ERROR;
    }

    return LCM_STATUS_OK;
}


LCM_STATUS lcm_util_set_data(const LCM_UTIL_FUNCS *lcm_util, char type, LCM_DATA_T1 *t1)
{
    // check parameter is valid
    if (LCM_STATUS_OK == _lcm_util_check_data(type, t1)) {
        switch (type) {
        case LCM_UTIL_RESET:
            lcm_util->set_reset_pin((unsigned int)t1->data);
            break;

        case LCM_UTIL_MDELAY:
            lcm_util->mdelay((unsigned int)t1->data);
            break;

        case LCM_UTIL_UDELAY:
            lcm_util->udelay((unsigned int)t1->data);
            break;

        default:
            dprintf(0, "[LCM][ERROR] %s: %d \n", __func__, (unsigned int)type);
            return LCM_STATUS_ERROR;
        }
    } else {
        dprintf(0, "[LCM][ERROR] %s: 0x%x, 0x%x \n", __func__, (unsigned int)type, (unsigned int)t1->data);
        return LCM_STATUS_ERROR;
    }

    return LCM_STATUS_OK;
}


LCM_STATUS lcm_util_set_write_cmd_v1(const LCM_UTIL_FUNCS *lcm_util, LCM_DATA_T5 *t5, unsigned char force_update)
{
    unsigned int i;
    unsigned int cmd[32];

    // check parameter is valid
    if (LCM_STATUS_OK == _lcm_util_check_write_cmd_v1(t5)) {
        memset(cmd, 0x0, sizeof(unsigned int) * 32);
        for (i = 0; i < t5->size; i++) {
            cmd[i] = (t5->cmd[i*4+3] << 24) | (t5->cmd[i*4+2] << 16) | (t5->cmd[i*4+1] << 8) | (t5->cmd[i*4]);
        }
        lcm_util->dsi_set_cmdq(cmd, (unsigned int)t5->size, force_update);
    } else {
        dprintf(0, "[LCM][ERROR] %s: 0x%p, %d, %d \n", __func__, t5->cmd, (unsigned int)t5->size, force_update);
        return LCM_STATUS_ERROR;
    }

    return LCM_STATUS_OK;
}


LCM_STATUS lcm_util_set_write_cmd_v2(const LCM_UTIL_FUNCS *lcm_util, LCM_DATA_T3 *t3, unsigned char force_update)
{
    // check parameter is valid
    if (LCM_STATUS_OK == _lcm_util_check_write_cmd_v2(t3)) {
        lcm_util->dsi_set_cmdq_V2((unsigned char)t3->cmd, (unsigned char)t3->size, (unsigned char *)t3->data, force_update);
    } else {
        dprintf(0, "[LCM][ERROR] %s: 0x%x, %d, 0x%p, %d \n", __func__, (unsigned int)t3->cmd, (unsigned int)t3->size, t3->data, force_update);
        return LCM_STATUS_ERROR;
    }

    return LCM_STATUS_OK;
}


LCM_STATUS lcm_util_set_write_cmd_v23(const LCM_UTIL_FUNCS *lcm_util, void *handle, LCM_DATA_T3 *t3,
                                      unsigned char force_update)
{
    /* check parameter is valid */
    if (LCM_STATUS_OK == _lcm_util_check_write_cmd_v23(t3))
        lcm_util->dsi_set_cmdq_V23(handle, (unsigned char)t3->cmd, (unsigned char)t3->size,
                                   (unsigned char *)t3->data, force_update);
    else {
        pr_debug("[LCM][ERROR] %s/%d: 0x%x, %d, 0x%p\n", __func__, __LINE__, t3->cmd,
                 t3->size, t3->data);
        return LCM_STATUS_ERROR;
    }

    return LCM_STATUS_OK;
}


LCM_STATUS lcm_util_set_read_cmd_v2(const LCM_UTIL_FUNCS *lcm_util, LCM_DATA_T4 *t4, unsigned int *compare)
{
    if (compare == NULL) {
        dprintf(0, "[LCM][ERROR] %s: NULL parameter \n", __func__);
        return LCM_STATUS_ERROR;
    }

    *compare = 0;

    // check parameter is valid
    if (LCM_STATUS_OK == _lcm_util_check_read_cmd_v2(t4)) {
        unsigned char buffer[4];

        lcm_util->dsi_dcs_read_lcm_reg_v2((unsigned char)t4->cmd, buffer, 4);

        if (buffer[t4->location] == (unsigned char)t4->data) {
            *compare = 1;
        }
    } else {
        dprintf(0, "[LCM][ERROR] %s: 0x%x, %d, 0x%x \n", __func__, (unsigned int)t4->cmd, (unsigned int)t4->location, (unsigned int)t4->data);
        return LCM_STATUS_ERROR;
    }

    return LCM_STATUS_OK;
}
#endif

