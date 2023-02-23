/*
 * Copyright (c) 2020 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#include <stdbool.h>
#include <debug.h>
#include <lcm_drv.h>
#include <lcm_define.h>
#include "ddp_manager.h"
#include "disp_lcm.h"
#include "disp_drv_platform.h"
#include "disp_drv_log.h"
#include <assert.h>
#include <libfdt.h>
#ifdef MTK_LCM_COMMON_DRIVER_SUPPORT
#include <lcm_common_drv.h>
#endif

// This macro and arrya is designed for multiple LCM support
// for multiple LCM, we should assign I/F Port id in lcm driver, such as DPI0, DSI0/1
static struct disp_lcm_handle _disp_lcm_driver[MAX_LCM_NUMBER] = {{0} };
static LCM_PARAMS _disp_lcm_params;

#ifndef MTK_LCM_COMMON_DRIVER_SUPPORT
int _lcm_count(void)
{
    return lcm_count;
}
#endif

int _is_lcm_inited(struct disp_lcm_handle *plcm)
{
    if (plcm) {
        if (plcm->params && plcm->drv)
            return 1;
    }

    DISPERR("WARNING, invalid lcm handle: 0x%08x\n", (unsigned int)plcm);
    return 0;
}

LCM_PARAMS *_get_lcm_params_by_handle(struct disp_lcm_handle *plcm)
{
    if (plcm)
        return plcm->params;

    DISPERR("WARNING, invalid lcm handle: 0x%08x\n", (unsigned int)plcm);
    return NULL;
}

LCM_DRIVER *_get_lcm_driver_by_handle(struct disp_lcm_handle *plcm)
{
    if (plcm)
        return plcm->drv;

    DISPERR("WARNING, invalid lcm handle: 0x%08x\n", (unsigned int)plcm);
    return NULL;
}

static enum DISP_MODULE_ENUM _get_dst_module_by_lcm(struct disp_lcm_handle *plcm)
{
    if (plcm == NULL) {
        DISPERR("plcm is null\n");
        return DISP_MODULE_UNKNOWN;
    }

    if (plcm->params->type == LCM_TYPE_DSI) {
        if (plcm->lcm_if_id == LCM_INTERFACE_DSI0)
            return DISP_MODULE_DSI0;
        else if (plcm->lcm_if_id == LCM_INTERFACE_DSI1)
            return DISP_MODULE_DSI1;
        else if (plcm->lcm_if_id == LCM_INTERFACE_DSI_DUAL)
            return DISP_MODULE_DSIDUAL;
        else
            return DISP_MODULE_DSI0;
    } else if (plcm->params->type == LCM_TYPE_DPI) {
        return DISP_MODULE_UNKNOWN;
    } else {
        DISPERR("can't find primary path dst module\n");
        return DISP_MODULE_UNKNOWN;
    }
}


void *_display_interface_path_init(struct disp_lcm_handle *plcm, void *fdt)
{
    enum DISP_MODULE_ENUM dst_module = DISP_MODULE_UNKNOWN;
    struct disp_ddp_path_config data_config;
    void *handle = NULL;

    DISPFUNC();

    if (plcm == NULL) {
        DISPERR("plcm is null\n");
        return NULL;
    }
    dpmgr_init();
    handle = dpmgr_create_path(DDP_SCENARIO_PRIMARY_RDMA0_DISP, NULL);
    if (handle) {
        DISPCHECK("dpmgr create path SUCCESS(0x%08x)\n", (unsigned int)handle);
    } else {
        DISPCHECK("dpmgr create path FAIL\n");
        return NULL;
    }

    dst_module = _get_dst_module_by_lcm(plcm);
    dpmgr_path_set_dst_module(handle, dst_module);
    DISPCHECK("dpmgr set dst module FINISHED(%s)\n", ddp_get_module_name(dst_module));


    dpmgr_set_lcm_utils(handle, plcm->drv, fdt);
    dpmgr_path_init(handle, CMDQ_DISABLE);

    memset((void *)&data_config, 0, sizeof(struct disp_ddp_path_config));

    memcpy(&(data_config.dsi_config), &(plcm->params->dsi), sizeof(LCM_DSI_PARAMS));
#if defined(MTK_ROUND_CORNER_SUPPORT) && defined(DISP_HW_RC)
    memcpy(&(data_config.round_corner_params), &(plcm->params->round_corner_params),
            sizeof(LCM_ROUND_CORNER));
#endif

    data_config.dst_w = plcm->params->width;
    data_config.dst_h = plcm->params->height;
    data_config.dst_dirty = 1;
    data_config.dsi_config = plcm->params->dsi;

    dpmgr_path_config(handle, &data_config, CMDQ_DISABLE);

    return handle;
}

int _display_interface_path_deinit(void *handle)
{
    DISPFUNC();
    if (handle == NULL) {
        DISPERR("handle is null\n");
        return -1;
    }

    dpmgr_path_deinit(handle, CMDQ_DISABLE);
    dpmgr_destroy_path(handle);

    return 0;
}

void _dump_lcm_info(struct disp_lcm_handle *plcm)
{
    LCM_DRIVER *l = NULL;
    LCM_PARAMS *p = NULL;

    if (plcm == NULL) {
        DISPERR("plcm is null\n");
        return;
    }

    l = plcm->drv;
    p = plcm->params;

    DISPCHECK("******** dump lcm driver information ********\n");

    if (l && p) {
        DISPCHECK("[LCM], name: %s\n", l->name);

        DISPCHECK("[LCM] resolution: %d x %d\n", p->width, p->height);
        DISPCHECK("[LCM] physical size: %d x %d\n", p->physical_width, p->physical_height);
        DISPCHECK("[LCM] physical size: %d x %d\n", p->physical_width, p->physical_height);
        DISPCHECK("[LCM] lcm_if:%d, cmd_if:%d\n", p->lcm_if, p->lcm_cmd_if);
#ifdef MTK_ROUND_CORNER_SUPPORT
        DISPCHECK("[LCM][RC] en:%d, w:%d, h:%d\n", p->round_corner_params.round_corner_en,
                    p->round_corner_params.w, p->round_corner_params.h);
        DISPCHECK("[LCM][RC] size:%d, addr:0x%p\n", p->round_corner_params.tp_size,
                    p->round_corner_params.lt_addr);
#endif

        switch (p->lcm_if) {
        case LCM_INTERFACE_DSI0:
            DISPCHECK("[LCM] interface: DSI0\n");
            break;
        case LCM_INTERFACE_DSI1:
            DISPCHECK("[LCM] interface: DSI1\n");
            break;
        case LCM_INTERFACE_DPI0:
            DISPCHECK("[LCM] interface: DPI0\n");
            break;
        case LCM_INTERFACE_DPI1:
            DISPCHECK("[LCM] interface: DPI1\n");
            break;
        case LCM_INTERFACE_DBI0:
            DISPCHECK("[LCM] interface: DBI0\n");
            break;
        default:
            DISPCHECK("[LCM] interface: unknown\n");
            break;
        }

        switch (p->type) {
        case LCM_TYPE_DBI:
            DISPCHECK("[LCM] Type: DBI\n");
            break;
        case LCM_TYPE_DSI:
            DISPCHECK("[LCM] Type: DSI\n");
            break;
        case LCM_TYPE_DPI:
            DISPCHECK("[LCM] Type: DPI\n");
            break;
        default:
            DISPCHECK("[LCM] TYPE: unknown\n");
            break;
        }

        if (p->type == LCM_TYPE_DSI) {
            switch (p->dsi.mode) {
            case CMD_MODE:
                DISPCHECK("[LCM] DSI Mode: CMD_MODE\n");
                break;
            case SYNC_PULSE_VDO_MODE:
                DISPCHECK("[LCM] DSI Mode: SYNC_PULSE_VDO_MODE\n");
                break;
            case SYNC_EVENT_VDO_MODE:
                DISPCHECK("[LCM] DSI Mode: SYNC_EVENT_VDO_MODE\n");
                break;
            case BURST_VDO_MODE:
                DISPCHECK("[LCM] DSI Mode: BURST_VDO_MODE\n");
                break;
            default:
                DISPCHECK("[LCM] DSI Mode: Unknown\n");
                break;
            }
        }

        if (p->type == LCM_TYPE_DSI) {
            DISPCHECK("[LCM] LANE_NUM: %d,data_format:(%d,%d,%d,%d)\n", p->dsi.LANE_NUM,
                p->dsi.data_format.color_order, p->dsi.data_format.format,
                p->dsi.data_format.padding, p->dsi.data_format.trans_seq);
#ifdef MY_TODO
#error
#endif
            DISPCHECK("[LCM] vact:%d,vbp:%d,vfp:%d,vact_line:%d\n",
                        p->dsi.vertical_sync_active, p->dsi.vertical_backporch,
                        p->dsi.vertical_frontporch, p->dsi.vertical_active_line);
            DISPCHECK("[LCM] hact:%d,hbp:%d,hfp:%d,hblank:%d,hactive:%d\n",
                        p->dsi.horizontal_sync_active, p->dsi.horizontal_backporch,
                        p->dsi.horizontal_frontporch, p->dsi.horizontal_blanking_pixel,
                        p->dsi.horizontal_active_pixel);
            DISPCHECK("[LCM] pll_select:%d,pll_div1:%d,pll_div2:%d,fbk_div:%d,fbk_sel:%d\n",
                        p->dsi.pll_select, p->dsi.pll_div1, p->dsi.pll_div2, p->dsi.fbk_div,
                        p->dsi.fbk_sel);
            DISPCHECK("[LCM] rg_bir:%d,rg_bic:%d,rg_bp:%d,PLL_CLK:%d,dsi_clk:%d\n",
                        p->dsi.rg_bir, p->dsi.rg_bic, p->dsi.rg_bp,
                        p->dsi.PLL_CLOCK, p->dsi.dsi_clock);
            DISPCHECK("[LCM] ssc_range:%d,ssc_disable:%d,compatibility_for_nvk:%d,cont_clk:%d\n",
                        p->dsi.ssc_range, p->dsi.ssc_disable, p->dsi.compatibility_for_nvk,
                        p->dsi.cont_clock);
            DISPCHECK("[LCM] lcm_ext_te_enable:%d,noncont_clk:%d,noncont_clk_period:%d\n",
                        p->dsi.lcm_ext_te_enable, p->dsi.noncont_clock,
                        p->dsi.noncont_clock_period);
        }
    }

    return;
}

// TODO: remove this workaround!!!
extern u32 DSI_dcs_read_lcm_reg_v2(enum DISP_MODULE_ENUM module, void *cmdq, u8 cmd, u8 *buffer,
                                    u8 buffer_size);

#ifdef MTK_LCM_COMMON_DRIVER_SUPPORT
static char dts_panel_resource[MTK_LCM_NAME_LENGTH] = {0};
void disp_lcm_set_dts_panel_resource(char *lcm_name)
{
    int ret = 0;

    if (lcm_name == NULL)
        return;

    ret = snprintf(&dts_panel_resource[0],
        MTK_LCM_NAME_LENGTH - 1, "%s", lcm_name);
    if (ret < 0 || ret >= MTK_LCM_NAME_LENGTH)
        DISPCHECK("%s, %d snprintf failed, %d\n",
            __func__, __LINE__, ret);
}
#endif

static int dts_panel_index = 1;
void disp_lcm_set_dts_panel_index(int index)
{
    dts_panel_index = index;
}

int disp_lcm_get_dts_panel_index(void)
{
    return dts_panel_index;
}

static int dts_is_cphy;
void disp_lcm_set_dts_phy_type(int phy_type)
{
    dts_is_cphy = phy_type;
}

int disp_lcm_get_dts_phy_type(void)
{
    return dts_is_cphy;
}

#ifdef MTK_LCM_COMMON_DRIVER_SUPPORT
static int disp_lcm_check_compare_id(void *fdt, LCM_DRIVER *lcm_drv, char *lcm_name)
{
    int ret = 0;
    void *handle = NULL;
    struct disp_lcm_handle *plcm = NULL;
    LCM_PARAMS *lcm_params = NULL;

    plcm = malloc(sizeof(struct disp_lcm_handle));
    if (plcm == NULL) {
        DISPERR("%s, failed to allocate lcm handler\n", __func__);
        return 0;
    }
    memset((void *)plcm, 0, sizeof(struct disp_lcm_handle));

    lcm_params = malloc(sizeof(LCM_PARAMS));
    if (lcm_params == NULL) {
        DISPERR("%s, failed to allocate lcm params\n", __func__);
        free(plcm);
        return 0;
    }
    memset((void *)lcm_params, 0, sizeof(LCM_PARAMS));

    lcm_drv->get_params(lcm_params);
    plcm->drv = lcm_drv;
    plcm->lcm_if_id = lcm_params->lcm_if;
    plcm->params = lcm_params;
    DISPCHECK("%s: we will check lcm: %s, type:%u\n",
        __func__, lcm_name, lcm_params->type);

    if (lcm_drv->compare_id == NULL) {
        DISPERR("%s: lcm not support compare id\n", __func__);
        ret = 0;
        goto FAIL2;
    }
    handle = _display_interface_path_init(plcm, fdt);
    if (handle == NULL) {
        DISPERR("%s: _display_interface_path_init returns NULL\n", __func__);
        ret = 0;
        goto FAIL2;
    }

    if (lcm_drv->init_power)
        lcm_drv->init_power(fdt);

    if (lcm_drv->compare_id(fdt) != 1) {
        DISPERR("%s: failed of compare id\n", __func__);
        goto FAIL1;
    }

    ret = 1;

FAIL1:
    _display_interface_path_deinit(handle);

FAIL2:
    DISPCHECK("%s: we have finished checking compare id of \"%s\", ret=%d\n",
        __func__, lcm_name, ret);
    free(lcm_params);
    free(plcm);
    return ret;
}
#endif

// if lcm handle already probed, should return lcm handle directly
struct disp_lcm_handle *disp_lcm_probe(char *plcm_name, void *fdt)
{
    DISPFUNC();

    bool isLCMFound = false;
    bool isLCMConnected = false;
    LCM_DRIVER *lcm_drv = NULL;
    LCM_PARAMS *lcm_param = NULL;
    struct disp_lcm_handle *plcm = NULL;

#if defined(MTK_LCM_COMMON_DRIVER_SUPPORT)
    char lcm_name[MTK_LCM_NAME_LENGTH] = {0};
    char func = MTK_LCM_FUNC_DSI;
    int ret = 0;
    bool force = true;
    bool is_checked = false;

    if (fdt == NULL) {
        DISPERR("%s: invalid fdt\n", __func__);
        return NULL;
    }

    lcm_drv = mtk_lcm_get_common_drv(func);
    if (lcm_drv == NULL) {
        DISPERR("%s: failed to get common driver of %d\n",
            __func__, func);
        return NULL;
    }

    if (plcm_name != NULL) {
        snprintf(lcm_name, MTK_LCM_NAME_LENGTH - 1, "%s", plcm_name);
        is_checked = true;
        DISPCHECK("%s, %d, bl2_ext send lcm info:\"%s\", is_checked=%d\n",
            __func__, __LINE__, lcm_name, is_checked);
    } else if (mtk_lcm_drv_get_dconfig(fdt, func, &lcm_name[0],
        MTK_LCM_NAME_LENGTH) == 0) {
        isLCMFound = true; //we will not scan lcm list with dconfig
        DISPMSG("%s, %d, got dconfig panel:\"%s\"\n",
            __func__, __LINE__, lcm_name);
    } else if (mtk_lcm_drv_check_lcm_boot_env(func, &lcm_name[0],
        MTK_LCM_NAME_LENGTH) == 0) {
        DISPCHECK("%s, %d, got boot up device:\"%s\"\n",
            __func__, __LINE__, lcm_name);
    }

    if (strlen(&lcm_name[0]) > 0) {
        ret = mtk_lcm_drv_parse_lcm_dts(fdt, func, &lcm_name[0]);
        if (ret != 0) {
            DISPERR("%s: failed to parse boot env dts:%s, ret:%d\n",
                __func__, lcm_name, ret);
        } else if (is_checked == true ||
                   disp_lcm_check_compare_id(fdt, lcm_drv, &lcm_name[0]) == 1) {
            DISPCHECK("%s, %d: we will use panel:\"%s\", is_checked:%d\n",
                __func__, __LINE__, lcm_name, is_checked);
            isLCMFound = true;
        }
    } else {
        DISPERR("%s, %d, failed to get boot up device\n",
            __func__, __LINE__);
    }

    if (isLCMFound == true)
        goto end;

    while (mtk_lcm_drv_parse_next_lcm_dts(fdt, func, &lcm_name[0], force) >= 0) {
        force = false;
        if (disp_lcm_check_compare_id(fdt, lcm_drv, &lcm_name[0]) == 1) {
            DISPCHECK("%s, %d: we will use panel:\"%s\"\n",
                __func__, __LINE__, lcm_name);
            isLCMFound = true;
            ret = mtk_lcm_drv_set_lcm_boot_env(func, &lcm_name[0]);
            if (ret != 0)
                DISPERR("%s: failed to update lcm boot env of %s\n",
                    __func__, lcm_name);
            break;
        }
    }

    if (isLCMFound == false) {
        if (strlen(lcm_name) > 0) {
            /* evb w/o panel DTB should also boot up correctly*/
            isLCMFound = true;
            DISPERR("%s, %d, we have check all lcm list, but failed, default use:%s\n",
                __func__, __LINE__, lcm_name);
        } else
            DISPERR("%s, %d, we have check all lcm list, but failed, no default panel\n",
                __func__, __LINE__);
    }

#else

    if (_lcm_count() == 0) {
        DISPERR("no lcm driver defined in linux kernel driver\n");
        return NULL;
    } else if (_lcm_count() == 1) {
        lcm_drv = lcm_driver_list[0];
        isLCMFound = true;
    } else if (plcm_name != NULL) {
        int i = 0;

        for (i = 0; i < _lcm_count(); i++) {
            lcm_drv = lcm_driver_list[i];
            if (!strcmp(lcm_drv->name, plcm_name)) {
                isLCMFound = true;
                disp_lcm_set_dts_panel_index(i + 1);
                break;
            }
        }
    } else {
        int i = 0;
        void *handle = NULL;
        struct disp_lcm_handle hlcm;
        struct disp_lcm_handle *plcm = &hlcm;
        LCM_PARAMS hlcm_param;

        for (i = 0; i < _lcm_count(); i++) {
            memset((void *)&hlcm, 0, sizeof(struct disp_lcm_handle));
            memset((void *)&hlcm_param, 0, sizeof(LCM_PARAMS));

            lcm_drv = lcm_driver_list[i];
            lcm_drv->get_params(&hlcm_param);
            plcm->drv = lcm_drv;
            plcm->params = &hlcm_param;
            plcm->lcm_if_id = plcm->params->lcm_if;
            dprintf(0, "we will check lcm: %s, i=%d\n", lcm_drv->name, i);
            handle = _display_interface_path_init(plcm, fdt);
            if (handle == NULL) {
                DISPERR("_display_interface_path_init returns NULL\n");
                goto FAIL;
            }
            if (lcm_drv->init_power)
                lcm_drv->init_power(fdt);

            if (lcm_drv->compare_id != NULL) {
                if (lcm_drv->compare_id(fdt) != 0) {
                    isLCMFound = true;
                    _display_interface_path_deinit(handle);
                    dprintf(0, "we will use lcm: %s\n", lcm_drv->name);
                    disp_lcm_set_dts_panel_index(i+1);
                    break;
                }
            }
            _display_interface_path_deinit(handle);
        }

        if (isLCMFound == false) {
            dprintf(0, "we have checked all lcm driver, but no lcm found\n");
            lcm_drv = lcm_driver_list[0];
            isLCMFound = true;
            disp_lcm_set_dts_panel_index(1);
        }
    }
#endif

end:
    if (isLCMFound == false) {
        DISPERR("FATAL ERROR!!!No LCM Driver defined\n");
        return NULL;
    }

    plcm = &_disp_lcm_driver[0];
    lcm_param = &_disp_lcm_params;
    if (plcm && lcm_param) {
        plcm->params = lcm_param;
        plcm->drv = lcm_drv;
    } else {
        DISPERR("FATAL ERROR!!!kzalloc plcm and plcm->params failed\n");
        goto FAIL;
    }


    plcm->drv->get_params(plcm->params);
    plcm->lcm_if_id = plcm->params->lcm_if;
    // below code is for lcm driver forward compatible
    if (plcm->params->type == LCM_TYPE_DSI && plcm->params->lcm_if == LCM_INTERFACE_NOTDEFINED)
        plcm->lcm_if_id = LCM_INTERFACE_DSI0;
    if (plcm->params->type == LCM_TYPE_DPI && plcm->params->lcm_if == LCM_INTERFACE_NOTDEFINED)
        plcm->lcm_if_id = LCM_INTERFACE_DPI0;
    if (plcm->params->type == LCM_TYPE_DBI && plcm->params->lcm_if == LCM_INTERFACE_NOTDEFINED)
        plcm->lcm_if_id = LCM_INTERFACE_DBI0;

    plcm->is_connected = isLCMConnected;

    if (plcm->params->dsi.IsCphy)
        disp_lcm_set_dts_phy_type(plcm->params->dsi.IsCphy);

#if defined(MTK_LCM_COMMON_DRIVER_SUPPORT)
    disp_lcm_set_dts_panel_resource(&lcm_name[0]);
#endif
    _dump_lcm_info(plcm);
    return plcm;

FAIL:

    return NULL;
}

//int ddp_dsi_dump(enum DISP_MODULE_ENUM module, int level);
//int DSI_BIST_Pattern_Test(enum DISP_MODULE_ENUM module, void *cmdq,
//                                   bool enable, unsigned int color);
//int ddp_dsi_start(enum DISP_MODULE_ENUM module, void *cmdq);

int disp_lcm_init(struct disp_lcm_handle *plcm, void *fdt)
{
    DISPFUNC();
    LCM_DRIVER *lcm_drv = NULL;
    bool isLCMConnected = false;

    if (_is_lcm_inited(plcm)) {
        lcm_drv = plcm->drv;

        if (lcm_drv->init_power) {
            DISPCHECK("lcm init_power\n");
            lcm_drv->init_power(fdt);
        }

        if (lcm_drv->init) {
            if (!disp_lcm_is_inited(plcm)) {
                DISPCHECK("lcm init\n");
                lcm_drv->init(fdt);
            }
        } else {
            DISPERR("FATAL ERROR, lcm_drv->init is null\n");
            return -1;
        }

#ifndef MACH_FPGA
        if (plcm->params->type == LCM_TYPE_DSI) {
            int ret = 0;
            char buffer = 0;

            ret = DSI_dcs_read_lcm_reg_v2(_get_dst_module_by_lcm(plcm), NULL, 0x0A,
                                                (u8 *)&buffer, 1);
            DISPMSG("read from lcm 0x0A: %d\n", buffer);
            if (ret == 0) {
                isLCMConnected = 0;
                DISPMSG("lcm is not connected\n");
            } else {
                isLCMConnected = 1;
                DISPMSG("lcm is connected\n");
            }
        }
        if (plcm->params->dsi.edp_panel == 1)
            isLCMConnected = 1;

#else
        isLCMConnected = 1;
#endif
        plcm->is_connected = isLCMConnected;

        //ddp_dsi_start(DISP_MODULE_DSI0, NULL);
        //DSI_BIST_Pattern_Test(DISP_MODULE_DSI0,NULL,true, 0x00ffff00);
        //ddp_dsi_dump(DISP_MODULE_DSI0, 2);
        return 0;
    } else {
        DISPERR("plcm is null\n");
        return -1;
    }
}

LCM_PARAMS *disp_lcm_get_params(struct disp_lcm_handle *plcm)
{
    DISPFUNC();

    if (_is_lcm_inited(plcm))
        return plcm->params;
    else
        return NULL;
}

const char *disp_lcm_get_name(struct disp_lcm_handle *plcm)
{
    DISPFUNC();

#ifdef MTK_LCM_COMMON_DRIVER_SUPPORT
    return &dts_panel_resource[0];
#else
    if (_is_lcm_inited(plcm))
        return plcm->drv->name;
    else
        return NULL;
#endif
}

int disp_lcm_width(struct disp_lcm_handle *plcm)
{
    LCM_PARAMS *plcm_param = NULL;
    int w = 0;

    if (_is_lcm_inited(plcm))
        plcm_param = _get_lcm_params_by_handle(plcm);
    if (plcm_param == NULL) {
        DISPERR("plcm_param is null\n");
        return 0;
    }

    w = plcm_param->virtual_width;
    if (w == 0)
        w = plcm_param->width;

    return w;
}

int disp_lcm_height(struct disp_lcm_handle *plcm)
{
    LCM_PARAMS *plcm_param = NULL;
    int h = 0;

    if (_is_lcm_inited(plcm))
        plcm_param = _get_lcm_params_by_handle(plcm);
    if (plcm_param == NULL) {
        DISPERR("plcm_param is null\n");
        return 0;
    }

    h = plcm_param->virtual_height;
    if (h == 0)
        h = plcm_param->height;

    return h;
}

int disp_lcm_x(struct disp_lcm_handle *plcm)
{
    LCM_PARAMS *plcm_param = NULL;

    if (_is_lcm_inited(plcm))
        plcm_param = _get_lcm_params_by_handle(plcm);
    if (plcm_param == NULL) {
        DISPERR("plcm_param is null\n");
        return 0;
    }
    return plcm_param->lcm_x;
}

int disp_lcm_y(struct disp_lcm_handle *plcm)
{
    LCM_PARAMS *plcm_param = NULL;

    if (_is_lcm_inited(plcm))
        plcm_param = _get_lcm_params_by_handle(plcm);
    if (plcm_param == NULL) {
        DISPERR("plcm_param is null\n");
        return 0;
    }
    return plcm_param->lcm_y;
}


LCM_INTERFACE_ID disp_lcm_get_interface_id(struct disp_lcm_handle *plcm)
{
    DISPFUNC();

    if (_is_lcm_inited(plcm))
        return plcm->lcm_if_id;

    return LCM_INTERFACE_NOTDEFINED;
}

int disp_lcm_update(struct disp_lcm_handle *plcm, int x, int y, int w, int h, int force)
{

    LCM_DRIVER *lcm_drv = NULL;
    LCM_PARAMS *plcm_param = NULL;

    if (_is_lcm_inited(plcm)) {
        plcm_param = _get_lcm_params_by_handle(plcm);
        if (plcm_param == NULL)
            return -1;
        DISPCHECK("(x=%d,y=%d),(w=%d,h=%d)\n", x, y, w, h);
        lcm_drv = plcm->drv;
        if (lcm_drv->update) {
            lcm_drv->update(x, y, w, h);
        } else {
            if (disp_lcm_is_video_mode(plcm))
                ;// do nothing
            else
                DISPERR("FATAL ERROR, lcm is cmd mode lcm_drv->update is null\n");

            return -1;
        }

        return 0;
    }

    DISPERR("lcm_drv is null\n");
    return -1;

}


int disp_lcm_esd_check(struct disp_lcm_handle *plcm)
{
    DISPFUNC();
    LCM_DRIVER *lcm_drv = NULL;

    if (_is_lcm_inited(plcm)) {
        lcm_drv = plcm->drv;
        if (lcm_drv->esd_check) {
            lcm_drv->esd_check();
        } else {
            DISPERR("FATAL ERROR, lcm_drv->esd_check is null\n");
            return -1;
        }

        return 0;
    }

    DISPERR("lcm_drv is null\n");
    return -1;

}



int disp_lcm_esd_recover(struct disp_lcm_handle *plcm)
{
    DISPFUNC();
    LCM_DRIVER *lcm_drv = NULL;

    if (_is_lcm_inited(plcm)) {
        lcm_drv = plcm->drv;
        if (lcm_drv->esd_recover) {
            lcm_drv->esd_recover();
        } else {
            DISPERR("FATAL ERROR, lcm_drv->esd_check is null\n");
            return -1;
        }

        return 0;
    }
    DISPERR("lcm_drv is null\n");
    return -1;
}

int disp_lcm_suspend(struct disp_lcm_handle *plcm)
{
    DISPFUNC();
    LCM_DRIVER *lcm_drv = NULL;

    if (_is_lcm_inited(plcm)) {
        lcm_drv = plcm->drv;
        if (lcm_drv->suspend) {
            lcm_drv->suspend();
        } else {
            DISPERR("FATAL ERROR, lcm_drv->suspend is null\n");
            return -1;
        }

        if (lcm_drv->suspend_power)
            lcm_drv->suspend_power();

        return 0;
    }
    DISPERR("lcm_drv is null\n");
    return -1;

}

int disp_lcm_resume(struct disp_lcm_handle *plcm)
{
    DISPFUNC();
    LCM_DRIVER *lcm_drv = NULL;

    if (_is_lcm_inited(plcm)) {
        lcm_drv = plcm->drv;

        if (lcm_drv->resume_power)
            lcm_drv->resume_power();

        if (lcm_drv->resume) {
            lcm_drv->resume();
        } else {
            DISPERR("FATAL ERROR, lcm_drv->resume is null\n");
            return -1;
        }

        return 0;
    }
    DISPERR("lcm_drv is null\n");
    return -1;
}



#ifdef MY_TODO
#error "maybe CABC can be moved into lcm_ioctl??"
#endif
int disp_lcm_set_backlight(struct disp_lcm_handle *plcm, int level)
{
    DISPFUNC();
    LCM_DRIVER *lcm_drv = NULL;

    if (_is_lcm_inited(plcm)) {
        lcm_drv = plcm->drv;
        if (lcm_drv->set_backlight) {
            lcm_drv->set_backlight(level);
        } else {
            DISPERR("FATAL ERROR, lcm_drv->esd_check is null\n");
            return -1;
        }

        return 0;
    }
    DISPERR("lcm_drv is null\n");
    return -1;
}




int disp_lcm_ioctl(struct disp_lcm_handle *plcm, LCM_IOCTL ioctl, unsigned int arg)
{
    return 0;
}

int disp_lcm_is_inited(struct disp_lcm_handle *plcm)
{
    if (_is_lcm_inited(plcm))
        return plcm->is_inited;

    return 0;
}

int disp_lcm_is_video_mode(struct disp_lcm_handle *plcm)
{
    DISPFUNC();
    LCM_PARAMS *lcm_param = NULL;

    if (_is_lcm_inited(plcm))
        lcm_param =  plcm->params;
    else
        ASSERT(0);

    switch (lcm_param->type) {
    case LCM_TYPE_DBI:
        return false;
        break;
    case LCM_TYPE_DSI:
        break;
    case LCM_TYPE_DPI:
        return true;
        break;
    default:
        DISPERR("[LCM] TYPE: unknown\n");
        break;
    }

    if (lcm_param->type == LCM_TYPE_DSI) {
        switch (lcm_param->dsi.mode) {
        case CMD_MODE:
            return false;
            break;
        case SYNC_PULSE_VDO_MODE:
        case SYNC_EVENT_VDO_MODE:
        case BURST_VDO_MODE:
            return true;
            break;
        default:
            DISPERR("[LCM] DSI Mode: Unknown\n");
            break;
        }
    }

    ASSERT(0);

    return 0;
}

