/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#include <debug.h>
#include <string.h>
#include <platform.h>

#include "ddp_dsi.h"
#include "ddp_manager.h"
#include "ddp_path.h"
#include "disp_drv_log.h"
#include "disp_drv_platform.h"
#include "disp_lcm.h"
#include "primary_display.h"

#define DDP_LK_BOOT
#ifndef HZ
#define HZ 100
#endif

#define LCM_MAX_WIDTH  1080
#define LCM_MAX_HEIGHT 1920

int primary_display_use_cmdq = CMDQ_DISABLE;
int primary_display_use_m4u = 1;
enum DISP_PRIMARY_PATH_MODE primary_display_mode = DIRECT_LINK_MODE;
static u32 disp_fb_bpp = 32;     ///ARGB8888
static u32 disp_fb_pages = 3;     ///Triple buffer
static u32 display_init;


//enum DDP_SCENARIO_ENUM ddp_scenario = DDP_SCENARIO_RDMA1_DISP;

struct _disp_dfo_item {
    char name[32];
    int  value;
};

#ifdef DFP_DISP
static _disp_dfo_item disp_dfo_setting[] = {
    {"LCM_FAKE_WIDTH",  0},
    {"LCM_FAKE_HEIGHT", 0},
    {"DISP_DEBUG_SWITCH",   0}
};
#endif


struct display_primary_path_context {
    int                         state;
    int                         need_trigger_overlay;
    enum DISP_PRIMARY_PATH_MODE  mode;
    unsigned int                last_vsync_tick;
    struct disp_lcm_handle      *plcm;
    void               *cmdq_handle_config;
    void               *cmdq_handle_trigger;
    void               *dpmgr_handle;
    void               *ovl2mem_path_handle;
};

#define pgc _get_context()

static struct display_primary_path_context *_get_context(void)
{
    static int is_context_inited;
    static struct display_primary_path_context g_context;

    if (!is_context_inited) {
        memset((void *)&g_context, 0, sizeof(struct display_primary_path_context));
        is_context_inited = 1;
    }

    return &g_context;
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

static int _build_path_direct_link(void)
{
    int ret = 0;
    enum DISP_MODULE_ENUM dst_module = 0;
    enum DDP_SCENARIO_ENUM scenario = DDP_SCENARIO_PRIMARY_DISP;
    LCM_PARAMS *lcm_param = NULL;

    DISPFUNC();
    pgc->mode = DIRECT_LINK_MODE;
    lcm_param = disp_lcm_get_params(pgc->plcm);

    pgc->dpmgr_handle = dpmgr_create_path(scenario, pgc->cmdq_handle_config);
    if (pgc->dpmgr_handle) {
        DISPCHECK("dpmgr create path SUCCESS(0x%08x)\n", (unsigned int)pgc->dpmgr_handle);
    } else {
        DISPCHECK("dpmgr create path FAIL\n");
        return -1;
    }

    dst_module = _get_dst_module_by_lcm(pgc->plcm);
    dpmgr_path_set_dst_module(pgc->dpmgr_handle, dst_module);
    DISPCHECK("dpmgr set dst module FINISHED(%s)\n", ddp_get_module_name(dst_module));

    dpmgr_set_lcm_utils(pgc->dpmgr_handle, pgc->plcm->drv, NULL);

    return ret;
}

static int _convert_disp_input_to_rdma(struct RDMA_CONFIG_STRUCT *dst,
            struct disp_input_config *src)
{
    if (src && dst) {
        dst->inputFormat = src->fmt;
        dst->address = src->addr;
        dst->width = src->src_w;
        dst->height = src->src_h;
        dst->pitch = src->src_pitch;

        return 0;
    } else {
        DISPERR("src(0x%08x) or dst(0x%08x) is null\n", (unsigned int)src, (unsigned int)dst);
        return -1;
    }
}

static int _convert_disp_input_to_ovl(struct OVL_CONFIG_STRUCT *dst, struct disp_input_config *src)
{
    if (src && dst) {
        dst->layer = src->layer;
        dst->layer_en = src->layer_en;
        dst->fmt = src->fmt;
        dst->addr = src->addr;
        dst->vaddr = src->vaddr;
        dst->src_x = src->src_x;
        dst->src_y = src->src_y;
        dst->src_w = src->src_w;
        dst->src_h = src->src_h;
        dst->src_pitch = src->src_pitch;
        dst->dst_x = src->dst_x;
        dst->dst_y = src->dst_y;
        dst->dst_w = src->dst_w;
        dst->dst_h = src->dst_h;
        dst->keyEn = src->keyEn;
        dst->key = src->key;
        dst->aen = src->aen;
        dst->alpha = src->alpha;

        dst->isDirty = src->isDirty;

        dst->buff_idx = src->buff_idx;
        dst->identity = src->identity;
        dst->connected_type = src->connected_type;
        dst->security = src->security;

        return 0;
    } else {
        DISPERR("src(0x%08x) or dst(0x%08x) is null\n", (unsigned int)src, (unsigned int)dst);
        return -1;
    }
}

int primary_display_is_lcm_connected(void)
{
    return pgc->plcm->is_connected;
}

const char *primary_display_get_lcm_name(void)
{
    return disp_lcm_get_name(pgc->plcm);
}

int primary_display_change_lcm_resolution(unsigned int width, unsigned int height)
{
    if (pgc->plcm) {
        DISPMSG("LCM Resolution will be changed, original: %dx%d, now: %dx%d\n",
                pgc->plcm->params->width, pgc->plcm->params->height, width, height);
        /*
         * align with 4 is the minimal check, to ensure we can boot up into kernel,
         * and could modify dfo setting again using meta tool
         * otherwise we will have a panic in lk(root cause unknown)
         */
        if (width > pgc->plcm->params->width || height > pgc->plcm->params->height
            || width == 0 || height == 0 || width % 4 || height % 2) {
            DISPERR("Invalid resolution: %dx%d\n", width, height);
            return -1;
        }

        if (primary_display_is_video_mode()) {
            DISPERR("Warning!!!Video Mode can't support multiple resolution!\n");
            return -1;
        }

        pgc->plcm->params->width = width;
        pgc->plcm->params->height = height;

        return 0;
    } else {
        return -1;
    }
}

int primary_display_init(void *fdt)
{
    DISPFUNC();
    enum DISP_STATUS ret = DISP_STATUS_OK;
    LCM_PARAMS *lcm_param = NULL;
    struct disp_ddp_path_config data_config;

    dpmgr_init();

    if (pgc->plcm == NULL)
        pgc->plcm = disp_lcm_probe(NULL, fdt);

    if (pgc->plcm == NULL) {
        DISPCHECK("disp_lcm_probe returns null\n");
        ret = DISP_STATUS_ERROR;
        goto done;
    } else {
        DISPCHECK("disp_lcm_probe SUCCESS\n");
    }


    lcm_param = disp_lcm_get_params(pgc->plcm);

    if (lcm_param == NULL) {
        DISPERR("get lcm params FAILED\n");
        ret = DISP_STATUS_ERROR;
        goto done;
    }

    /* insert DSC if needed */
    if (lcm_param->dsi.dsc_enable == 1)
        primary_disp_module_list_insert_dsc();

    if (primary_display_mode == DIRECT_LINK_MODE) {
        _build_path_direct_link();

        DISPCHECK("primary display is DIRECT LINK MODE\n");
    } else {
        DISPCHECK("primary display mode is WRONG\n");
    }

    dpmgr_path_set_video_mode(pgc->dpmgr_handle, primary_display_is_video_mode());

    dpmgr_path_init(pgc->dpmgr_handle, CMDQ_DISABLE);

    memset((void *)&data_config, 0, sizeof(struct disp_ddp_path_config));

    memcpy(&(data_config.dsi_config), &(lcm_param->dsi), sizeof(LCM_DSI_PARAMS));
#if defined(MTK_ROUND_CORNER_SUPPORT) && defined(DISP_HW_RC)
    memcpy(&(data_config.round_corner_params), &(lcm_param->round_corner_params),
            sizeof(LCM_ROUND_CORNER));
#endif

    data_config.dst_w = disp_lcm_width(pgc->plcm);
    data_config.dst_h = disp_lcm_height(pgc->plcm);
    if (lcm_param->type == LCM_TYPE_DSI) {
        if (lcm_param->dsi.data_format.format == LCM_DSI_FORMAT_RGB888)
            data_config.lcm_bpp = 24;
        else if (lcm_param->dsi.data_format.format == LCM_DSI_FORMAT_RGB565)
            data_config.lcm_bpp = 16;
        else if (lcm_param->dsi.data_format.format == LCM_DSI_FORMAT_RGB666)
            data_config.lcm_bpp = 18;
    } else if (lcm_param->type == LCM_TYPE_DPI) {
        if (lcm_param->dpi.format == LCM_DPI_FORMAT_RGB888)
            data_config.lcm_bpp = 24;
        else if (lcm_param->dpi.format == LCM_DPI_FORMAT_RGB565)
            data_config.lcm_bpp = 16;
        if (lcm_param->dpi.format == LCM_DPI_FORMAT_RGB666)
            data_config.lcm_bpp = 18;
    }
    data_config.dst_dirty = 1;

    ret = dpmgr_path_config(pgc->dpmgr_handle, &data_config, CMDQ_DISABLE);
    ret = disp_lcm_init(pgc->plcm, fdt);

    if (primary_display_is_video_mode()) {
        if (lcm_param->dsi.lfr_enable == 1)
            dpmgr_map_event_to_irq(pgc->dpmgr_handle, DISP_PATH_EVENT_IF_VSYNC,
                                    DDP_IRQ_DSI0_FRAME_DONE);
        else
            dpmgr_map_event_to_irq(pgc->dpmgr_handle, DISP_PATH_EVENT_IF_VSYNC,
                                    DDP_IRQ_RDMA0_DONE);
    }

    dpmgr_enable_event(pgc->dpmgr_handle, DISP_PATH_EVENT_IF_VSYNC);
    dpmgr_enable_event(pgc->dpmgr_handle, DISP_PATH_EVENT_FRAME_DONE);

    pgc->state = 1;
    display_init = 1;
done:

    return ret;
}

int primary_display_sw_init(bool isLCMConnected, void *fdt, char *plcm_name)
{
    DISPFUNC();
    enum DISP_STATUS ret = DISP_STATUS_OK;
    LCM_PARAMS *lcm_param = NULL;
    struct disp_ddp_path_config data_config;

    dpmgr_init();

    if (pgc->plcm == NULL) {
        if (plcm_name != NULL)
            dprintf(0, "%s, probe of lcm:%s\n", __func__, plcm_name);
        pgc->plcm = disp_lcm_probe(plcm_name, fdt);
    }

    if (pgc->plcm == NULL) {
        DISPCHECK("disp_lcm_probe returns null\n");
        ret = DISP_STATUS_ERROR;
        goto done;
    } else {
        DISPCHECK("disp_lcm_probe SUCCESS\n");
    }

    lcm_param = disp_lcm_get_params(pgc->plcm);

    if (lcm_param == NULL) {
        DISPERR("get lcm params FAILED\n");
        ret = DISP_STATUS_ERROR;
        goto done;
    }

    /* insert DSC if needed */
    if (lcm_param->dsi.dsc_enable == 1)
        primary_disp_module_list_insert_dsc();

    if (primary_display_mode == DIRECT_LINK_MODE) {
        _build_path_direct_link();

        DISPCHECK("primary display is DIRECT LINK MODE\n");
    }

    dpmgr_path_set_video_mode(pgc->dpmgr_handle, primary_display_is_video_mode());

    dpmgr_path_init(pgc->dpmgr_handle, CMDQ_DISABLE);

    memset((void *)&data_config, 0, sizeof(struct disp_ddp_path_config));

    memcpy(&(data_config.dsi_config), &(lcm_param->dsi), sizeof(LCM_DSI_PARAMS));
#if defined(MTK_ROUND_CORNER_SUPPORT) && defined(DISP_HW_RC)
    memcpy(&(data_config.round_corner_params), &(lcm_param->round_corner_params),
            sizeof(LCM_ROUND_CORNER));
#endif

    data_config.dst_w = disp_lcm_width(pgc->plcm);
    data_config.dst_h = disp_lcm_height(pgc->plcm);
    if (lcm_param->type == LCM_TYPE_DSI) {
        if (lcm_param->dsi.data_format.format == LCM_DSI_FORMAT_RGB888)
            data_config.lcm_bpp = 24;
        else if (lcm_param->dsi.data_format.format == LCM_DSI_FORMAT_RGB565)
            data_config.lcm_bpp = 16;
        else if (lcm_param->dsi.data_format.format == LCM_DSI_FORMAT_RGB666)
            data_config.lcm_bpp = 18;
    } else if (lcm_param->type == LCM_TYPE_DPI) {
        if (lcm_param->dpi.format == LCM_DPI_FORMAT_RGB888)
            data_config.lcm_bpp = 24;
        else if (lcm_param->dpi.format == LCM_DPI_FORMAT_RGB565)
            data_config.lcm_bpp = 16;
        if (lcm_param->dpi.format == LCM_DPI_FORMAT_RGB666)
            data_config.lcm_bpp = 18;
    }

    data_config.dst_dirty = 1;
    ret = dpmgr_path_config(pgc->dpmgr_handle, &data_config, CMDQ_DISABLE);

    pgc->plcm->is_connected = isLCMConnected;

    if (primary_display_is_video_mode()) {
        if (lcm_param->dsi.lfr_enable == 1)
            dpmgr_map_event_to_irq(pgc->dpmgr_handle, DISP_PATH_EVENT_IF_VSYNC,
                                    DDP_IRQ_DSI0_FRAME_DONE);
        else
            dpmgr_map_event_to_irq(pgc->dpmgr_handle, DISP_PATH_EVENT_IF_VSYNC,
                                    DDP_IRQ_RDMA0_DONE);
    }

    dpmgr_enable_event(pgc->dpmgr_handle, DISP_PATH_EVENT_IF_VSYNC);
    dpmgr_enable_event(pgc->dpmgr_handle, DISP_PATH_EVENT_FRAME_DONE);

    pgc->state = 1;
    display_init = 1;
done:

    return ret;
}

int primary_display_lcm_probe(char *plcm_name, void *fdt)
{
    enum DISP_STATUS ret = DISP_STATUS_OK;

    if (pgc->plcm == NULL) {
        pgc->plcm = disp_lcm_probe(plcm_name, fdt);
        DISPDBG("lcm handle is null, after probe:0x%08x\n", (unsigned int)pgc->plcm);
        if (pgc->plcm == NULL)
            ret = DISP_STATUS_ERROR;
    }

    return ret;
}

int primary_display_get_panel_index(void)
{
    return disp_lcm_get_dts_panel_index();
}

int primary_display_get_is_cphy(void)
{
    return disp_lcm_get_dts_phy_type();
}

// register rdma done event
int primary_display_wait_for_idle(void)
{
    enum DISP_STATUS ret = DISP_STATUS_OK;

    DISPFUNC();


    return ret;
}

int primary_display_wait_for_dump(void)
{
    return 0;
}

#if 0
static long int get_current_time_us(void)
{
    return get_timer(0);
}
#endif

int priamry_display_wait_for_vsync(void)
{
    int ret = 0;

    ret = dpmgr_wait_event(pgc->dpmgr_handle, DISP_PATH_EVENT_IF_VSYNC);
    if (ret == -2) {
        DISPCHECK("vsync for primary display path not enabled yet\n");
        return -1;
    }

//    pgc->last_vsync_tick = get_current_time_us();
    pgc->last_vsync_tick = 0;

    return 0;
}

int primary_display_suspend(void)
{
    enum DISP_STATUS ret = DISP_STATUS_OK;

    if (display_init == 0) {
        dprintf(CRITICAL, "skip %s before init\n", __func__);
        return -1;
    }

    DISPFUNC();
    disp_lcm_suspend(pgc->plcm);

    dpmgr_path_stop(pgc->dpmgr_handle, CMDQ_DISABLE);
    dpmgr_path_power_off(pgc->dpmgr_handle, CMDQ_DISABLE);
    pgc->state = 0;

    return ret;
}

int primary_display_resume(void)
{
    enum DISP_STATUS ret = DISP_STATUS_OK;

    DISPFUNC();


    disp_path_clock_on("Primary Display Path");
    pgc->state = 1;

    return ret;
}

int primary_display_is_alive(void)
{
    unsigned int temp = 0;

    DISPFUNC();
    temp = pgc->state;

    return temp;
}
int primary_display_is_sleepd(void)
{
    unsigned int temp = 0;

    DISPFUNC();
    temp = !pgc->state;

    return temp;
}

#if defined(MTK_ROUND_CORNER_SUPPORT) && !defined(DISP_HW_RC)
LCM_ROUND_CORNER *primary_display_get_corner_params(void)
{
    if (pgc->plcm == NULL)
        return NULL;

    if (pgc->plcm->params) {
        return &(pgc->plcm->params->round_corner_params);
    } else {
        DISPERR("lcm_params is null!\n");
        return NULL;
    }
}

int disp_get_round_corner_size(void)
{
    LCM_ROUND_CORNER *corner_params = primary_display_get_corner_params();
    int pitch = DISP_GetScreenWidth();
    int size = 0;

    if (corner_params) {
        if (corner_params->round_corner_en) {
            if (corner_params->full_content)
                size = (corner_params->h + corner_params->h_bot) * pitch * 2;
            else
                size = corner_params->h * pitch * 2 * 2;
        } else
            DISPERR("corner_params is null!\n");
    }

    return size;
}
#endif

int primary_display_get_width(void)
{
    if (pgc->plcm == NULL) {
        pgc->plcm = disp_lcm_probe(NULL, NULL);
        DISPDBG("%s: lcm handle is null, after probe:0x%08x\n",
            __func__, (unsigned int)pgc->plcm);
        if (pgc->plcm == NULL)
            return 0;
    }

    if (pgc->plcm->params) {
        return pgc->plcm->params->width;
    } else {
        DISPERR("%s, lcm_params is null!\n", __func__);
        return 0;
    }
}

int primary_display_get_align_width(void)
{
    int width;

    width = primary_display_get_width();

    return ALIGN_TO(width, MTK_FB_ALIGNMENT);
}

int primary_display_get_height(void)
{
    if (pgc->plcm == NULL) {
        pgc->plcm = disp_lcm_probe(NULL, NULL);
        DISPDBG("%s: lcm handle is null, after probe:0x%08x\n",
            __func__, (unsigned int)pgc->plcm);
        if (pgc->plcm == NULL)
            return 0;
    }

    if (pgc->plcm->params) {
        return pgc->plcm->params->height;
    } else {
        DISPERR("%s: lcm_params is null!\n", __func__);
        return 0;
    }
}

int primary_display_get_align_height(void)
{
    int height;

    height = primary_display_get_height();

    return ALIGN_TO(height, MTK_FB_ALIGNMENT);
}

void primary_display_diagnose(void)
{
    dpmgr_check_status(pgc->dpmgr_handle);
}

int primary_display_get_bpp(void)
{
    return disp_fb_bpp;
}

int primary_display_trigger(int blocking)
{
    int ret = 0;
    int x = 0, y = 0;
    int width = 0, height = 0;

    x = disp_lcm_x(pgc->plcm);
    y = disp_lcm_y(pgc->plcm);
    width = disp_lcm_width(pgc->plcm);
    height = disp_lcm_height(pgc->plcm);

    if (pgc->mode == DIRECT_LINK_MODE) {
        if (dpmgr_path_is_busy(pgc->dpmgr_handle)) {
            if (primary_display_is_video_mode())
                return 0;
            DSI0_WaitNotBusyAfterTrigger(DISP_MODULE_DSI0);
        }
        DISPCHECK("trigger mode: DIRECT_LINK\n");
        disp_lcm_update(pgc->plcm, x, y, width, height, 0);
        dpmgr_path_start(pgc->dpmgr_handle, primary_display_use_cmdq);
        if (primary_display_use_cmdq == CMDQ_DISABLE)
            dpmgr_path_trigger(pgc->dpmgr_handle, NULL, primary_display_use_cmdq);
    } else {
        DISPCHECK("primary display mode is WRONG(%d)\n", (unsigned int)pgc->mode);
    }

    primary_display_diagnose();
    return ret;
}

int primary_display_config_input(struct disp_input_config *input)
{
    int ret = 0;

    DISPFUNC();

    struct disp_ddp_path_config data_config;

    // all dirty should be cleared in dpmgr_path_get_last_config()
    memcpy((void *)&data_config, (void *)dpmgr_path_get_last_config(pgc->dpmgr_handle),
            sizeof(struct disp_ddp_path_config));

    if (pgc->mode == DIRECT_LINK_MODE) {
        if (dpmgr_path_is_busy(pgc->dpmgr_handle)) {
            if (primary_display_is_video_mode())
                dpmgr_wait_event_timeout(pgc->dpmgr_handle, DISP_PATH_EVENT_FRAME_DONE, HZ * 1);
        }
        ret = _convert_disp_input_to_ovl(&(data_config.ovl_config[input->layer]), input);
        data_config.ovl_dirty = 1;
        ret = dpmgr_path_config(pgc->dpmgr_handle, &data_config, primary_display_use_cmdq);
    } else {
        DISPCHECK("primary display mode is WRONG(%d)\n", (unsigned int)pgc->mode);
    }


    return ret;
}

int primary_display_is_video_mode(void)
{
    return disp_lcm_is_video_mode(pgc->plcm);
}

int primary_display_get_vsync_interval(void)
{
    int ret = 0;
    unsigned long long time0 = 0;
    unsigned long long time1 = 0;
    unsigned int lcd_time = 0;

    if (pgc->plcm->is_connected == false)
        goto fail;

    if (pgc->plcm->is_connected == false)
        goto fail;

    pgc->plcm->is_connected = false;

    ret = dpmgr_wait_event_timeout(pgc->dpmgr_handle, DISP_PATH_EVENT_IF_VSYNC, 1000);
    if (ret <= 0)
        goto fail;

    ret = dpmgr_wait_event_timeout(pgc->dpmgr_handle, DISP_PATH_EVENT_IF_VSYNC, 1000);
    if (ret <= 0)
        goto fail;

    // because we are polling irq status, so the first event should be ignored
    time0 = current_time_hires();
    //DISPMSG("vsync signaled:%d\n", gpt4_tick2time_us(gpt4_get_current_tick()));

    ret = dpmgr_wait_event_timeout(pgc->dpmgr_handle, DISP_PATH_EVENT_IF_VSYNC, 1000);
    if (ret <= 0)
        goto fail;
    //DISPMSG("vsync signaled:%d\n", gpt4_tick2time_us(gpt4_get_current_tick()));

    ret = dpmgr_wait_event_timeout(pgc->dpmgr_handle, DISP_PATH_EVENT_IF_VSYNC, 1000);
    if (ret > 0)
        time1 = current_time_hires();
    else
        goto fail;

    lcd_time = (time1 - time0)/2;
    pgc->plcm->is_connected = true;

    if (lcd_time != 0)
        return (100000000/lcd_time);
    else
        return (6000);
fail:
    DISPERR("wait event fail\n");
    return 0;
}

int primary_display_setbacklight(unsigned int level)
{
    int ret = 0;

    DISPMSG("lk %s level=%d\n", __func__, level);

    if (pgc->dpmgr_handle == NULL) {
        DISPCHECK("lk set backlight early!\n");
        return 0;
    }
    if (dpmgr_path_is_busy(pgc->dpmgr_handle)) {
        DISPCHECK("primary display path is busy\n");
        ret = dpmgr_wait_event_timeout(pgc->dpmgr_handle, DISP_PATH_EVENT_FRAME_DONE, HZ * 1);
        DISPCHECK("wait frame done ret:%d\n", ret);
    }
    disp_lcm_set_backlight(pgc->plcm, level);
    return 0;
}
////////////////////////////////////////////////////////////////////////

/***********************/
/*****Legacy DISP API*****/
/***********************/
u32 DISP_GetScreenWidth(void)
{
#ifndef MACH_FPGA_NO_DISPLAY
    return primary_display_get_width();
#else
    return LCM_MAX_WIDTH;
#endif
}

u32 DISP_GetScreenAlignWidth(void)
{
#ifndef MACH_FPGA_NO_DISPLAY
    return primary_display_get_align_width();
#else
    return ALIGN_TO(LCM_MAX_WIDTH, MTK_FB_ALIGNMENT);
#endif
}

u32 DISP_GetScreenHeight(void)
{
#ifndef MACH_FPGA_NO_DISPLAY
    return primary_display_get_height();
#else
    return LCM_MAX_HEIGHT;
#endif
}

u32 DISP_GetScreenAlignHeight(void)
{
#ifndef MACH_FPGA_NO_DISPLAY
    return primary_display_get_align_height();
#else
    return ALIGN_TO(LCM_MAX_HEIGHT, MTK_FB_ALIGNMENT);
#endif
}

u32 DISP_GetScreenBpp(void)
{
    return primary_display_get_bpp();
}

u32 DISP_GetPages(void)
{
    return disp_fb_pages;   // Triple Buffers
}

u32 DISP_GetFBRamSize(void)
{
    static u32 fbramSize;

    if (fbramSize == 0) {
        //width:1080->1088 * height:2300->2304 * 4Byte * 3FB
        fbramSize = DISP_GetScreenAlignWidth()
              * DISP_GetScreenAlignHeight()
              * ALIGN_TO(DISP_GetScreenBpp(), MTK_BITS_PER_BYTE)
              / MTK_BITS_PER_BYTE * DISP_GetPages();
    }

    return fbramSize;
}

unsigned int DISP_GetVRamSize(void)
{
    static u32 vramSize;
#if defined(MTK_ROUND_CORNER_SUPPORT) && !defined(DISP_HW_RC)
    unsigned int fb_ram_size = 0;
#endif

    //triple FB size + round corner size in byte
    if (vramSize == 0) {
        vramSize = DISP_GetFBRamSize();
        DISPDBG("^^ dsip total FB Size: %u bytes\n", vramSize);
#if defined(MTK_ROUND_CORNER_SUPPORT) && !defined(DISP_HW_RC)
        fb_ram_size = vramSize;
        vramSize += disp_get_round_corner_size();
        DISPDBG("^^ round corner Size: %u bytes\n", vramSize-fb_ram_size);
#endif

        vramSize = ALIGN_TO(vramSize, 0x10000);  // just align to 64KB is OK
        DISPDBG("^^ %s: %u bytes\n", __func__, vramSize);
    }

    return vramSize;
}

unsigned int DISP_GetMBRamSize(void)
{
    static u32 mramSize;
    u32 width, height, bpp;

    //mrdump size only in byte
    if (mramSize == 0) {
        width = DISP_GetScreenAlignWidth();
        height = DISP_GetScreenAlignHeight();
        bpp = ALIGN_TO(DISP_GetScreenBpp(), MTK_BITS_PER_BYTE);
        mramSize = width * (height / 8) * (bpp / MTK_BITS_PER_BYTE);

        DISPDBG("^^ %s: %u bytes\n", __func__, mramSize);
    }

    return mramSize;
}
