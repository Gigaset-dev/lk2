/*
 * Copyright (c) 2020 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#define LOG_TAG "ddp_manager"
#include <malloc.h>

#include "ddp_reg.h"

#include "ddp_path.h"
#include "ddp_dump.h"
#include "ddp_manager.h"
#include "ddp_log.h"


#define DDP_MAX_MANAGER_HANDLE (DISP_MUTEX_DDP_COUNT+DISP_MUTEX_DDP_FIRST)
#define DEFAULT_IRQ_EVENT_SCENARIO (3)

struct DPMGR_WQ_HANDLE {
    unsigned int       init;
    enum DISP_PATH_EVENT    event;
    //wait_queue_head_t  wq;
    unsigned long long data;
};

struct DDP_IRQ_EVENT_MAPPING {
    enum DDP_IRQ_BIT         irq_bit;
};

struct ddp_path_handle_t {
    void                    *cmdqhandle;
    int                     hwmutexid;
    int                     power_sate;
    enum DDP_MODE                mode;
    //struct mutex            mutex_lock;
    struct DDP_IRQ_EVENT_MAPPING   irq_event_map[DISP_PATH_EVENT_NUM];
    struct DPMGR_WQ_HANDLE         wq_list[DISP_PATH_EVENT_NUM];
    enum DDP_SCENARIO_ENUM       scenario;
    struct disp_ddp_path_config    last_config;
};

struct DDP_MANAGER_CONTEXT {
    int                     handle_cnt;
    int                     mutex_idx;
    int                     power_sate;
    //struct mutex            mutex_lock;
    enum DISP_MODULE_ENUM        module_usage_table[DISP_MODULE_NUM];
    struct ddp_path_handle_t *module_path_table[DISP_MODULE_NUM];
    struct ddp_path_handle_t *handle_pool[DDP_MAX_MANAGER_HANDLE];
};

static struct ddp_path_handle_t g_handle;

static struct DDP_IRQ_EVENT_MAPPING ddp_irq_event_list[DEFAULT_IRQ_EVENT_SCENARIO]
                [DISP_PATH_EVENT_NUM] = {
    {
        //ovl0 path
        {DDP_IRQ_RDMA0_START         }, /*FRAME_START*/
        {DDP_IRQ_RDMA0_DONE          }, /*FRAME_DONE*/
        {DDP_IRQ_RDMA0_REG_UPDATE    }, /*FRAME_REG_UPDATE*/
        {DDP_IRQ_RDMA0_TARGET_LINE   }, /*FRAME_TARGET_LINE*/
        {DDP_IRQ_UNKNOWN}, /*FRAME_COMPLETE*/
        {DDP_IRQ_RDMA0_TARGET_LINE   }, /*FRAME_STOP*/
        {DDP_IRQ_RDMA0_REG_UPDATE    }, /*IF_CMD_DONE*/
        {DDP_IRQ_DSI0_EXT_TE         }, /*IF_VSYNC*/
        {DDP_IRQ_UNKNOWN              }, /*AAL_TRIGER*/
        {DDP_IRQ_UNKNOWN              }, /*COLOR_TRIGER*/
    },
};

static struct DDP_MANAGER_CONTEXT *_get_context(void)
{
    static int is_context_inited;
    static struct DDP_MANAGER_CONTEXT context;

    if (!is_context_inited) {
        memset((void *)&context, 0, sizeof(struct DDP_MANAGER_CONTEXT));
        context.mutex_idx = (1 << DISP_MUTEX_DDP_COUNT) - 1;
        //mutex_init(&context.mutex_lock);
        is_context_inited = 1;
    }
    return &context;
}

static int path_top_clock_off(void)
{
    int i = 0;
    struct DDP_MANAGER_CONTEXT *context = _get_context();

    if (context->power_sate) {
        for (i = 0; i < DDP_MAX_MANAGER_HANDLE; i++) {
            if (context->handle_pool[i] != NULL && context->handle_pool[i]->power_sate != 0)
                return 0;
        }
        context->power_sate = 0;
        ddp_path_top_clock_off();
    }
    return 0;
}

static int path_top_clock_on(void)
{
    struct DDP_MANAGER_CONTEXT *context = _get_context();

    if (!context->power_sate) {
        context->power_sate = 1;
        ddp_path_top_clock_on();
    }
    return 0;
}

static struct ddp_path_handle_t *find_handle_by_module(enum DISP_MODULE_ENUM module)
{
    return _get_context()->module_path_table[module];
}

static int dpmgr_module_notify(enum DISP_MODULE_ENUM module, enum DISP_PATH_EVENT event)
{
    struct ddp_path_handle_t *handle = find_handle_by_module(module);

    return dpmgr_signal_event(handle, event);
}

static int assign_default_irqs_table(enum DDP_SCENARIO_ENUM scenario,
                                        struct DDP_IRQ_EVENT_MAPPING *irq_events)
{
    int idx = 0;

    memcpy(irq_events, ddp_irq_event_list[idx], sizeof(ddp_irq_event_list[idx]));
    return 0;
}

static int acquire_free_bit(unsigned int total)
{
    int free_id = 0;

    while (total) {
        if (total & 0x1)
            return free_id;

        total >>= 1;
        ++free_id;
    }
    return -1;
}

static int acquire_mutex(enum DDP_SCENARIO_ENUM scenario)
{
    //primay use mutex 0
    int mutex_id = 0;
    struct DDP_MANAGER_CONTEXT *content = _get_context();
    int mutex_idx_free = content->mutex_idx;

    ASSERT(scenario >= 0  && scenario < DDP_SCENARIO_MAX);
    while (mutex_idx_free) {
        if (mutex_idx_free & 0x1) {
            content->mutex_idx &= (~(0x1 << mutex_id));
            mutex_id += DISP_MUTEX_DDP_FIRST;
            break;
        }
        mutex_idx_free >>= 1;
        ++mutex_id;
    }
    ASSERT(mutex_id < (DISP_MUTEX_DDP_FIRST+DISP_MUTEX_DDP_COUNT));
    DISP_LOG_I("scenario %s acquire mutex %d , left mutex 0x%x!\n",
               ddp_get_scenario_name(scenario), mutex_id, content->mutex_idx);
    return mutex_id;
}

static int release_mutex(int mutex_idx)
{
    struct DDP_MANAGER_CONTEXT *content = _get_context();

    ASSERT(mutex_idx < (DISP_MUTEX_DDP_FIRST+DISP_MUTEX_DDP_COUNT));
    content->mutex_idx |= 1 << (mutex_idx-DISP_MUTEX_DDP_FIRST);
    DISP_LOG_I("release mutex %d , left mutex 0x%x!\n",
               mutex_idx, content->mutex_idx);
    return 0;
}

int dpmgr_path_set_video_mode(void *dp_handle, int is_vdo_mode)
{
    struct ddp_path_handle_t *handle = NULL;

    ASSERT(dp_handle != NULL);
    handle = (struct ddp_path_handle_t *)dp_handle;
    handle->mode = is_vdo_mode ?  DDP_VIDEO_MODE : DDP_CMD_MODE;
    DISP_LOG_I("set scenario %s mode %s\n", ddp_get_scenario_name(handle->scenario),
               is_vdo_mode ? "Video Mode":"Cmd Mode");
    return 0;
}

static int _dpmgr_path_connect(enum DDP_SCENARIO_ENUM scenario, void *handle)
{
    int i = 0, k = 0, module = 0;
    int *modules = NULL;
    int module_num = 0;

    enum DDP_SCENARIO_ENUM scn[2] = { DDP_SCENARIO_MAX, DDP_SCENARIO_MAX };
    bool has_dual_path = false;

    scn[0] = scenario;
    has_dual_path = ddp_path_is_dual(scn[0]);
    if (has_dual_path)
        scn[1] = ddp_get_dual_module(scn[0]);

    for (k = 0; k <= has_dual_path; k++) {
        ddp_connect_path(scn[k], handle);

        modules = ddp_get_scenario_list(scn[k]);
        module_num = ddp_get_module_num(scn[k]);

        for (i = 0; i < module_num; i++) {
            module = modules[i];
            if (ddp_modules_driver[module] && ddp_modules_driver[module]->connect) {
                int prev = i == 0 ? DISP_MODULE_UNKNOWN : modules[i - 1];
                int next = i == module_num - 1 ? DISP_MODULE_UNKNOWN : modules[i + 1];

                ddp_modules_driver[module]->connect(module, prev, next, 1, handle);
            }
        }
    }

    return 0;
}

static int _dpmgr_path_disconnect(enum DDP_SCENARIO_ENUM scenario, void *handle)
{
    int i = 0, module;
    int *modules = ddp_get_scenario_list(scenario);
    int module_num = ddp_get_module_num(scenario);

    ddp_disconnect_path(scenario, handle);

    for (i = 0; i < module_num; i++) {
        module = modules[i];
        if (ddp_modules_driver[module] && ddp_modules_driver[module]->connect) {
            int prev = i == 0 ? DISP_MODULE_UNKNOWN : modules[i - 1];
            int next = i == module_num - 1 ? DISP_MODULE_UNKNOWN : modules[i + 1];

            ddp_modules_driver[module]->connect(module, prev, next, 1, handle);
        }
    }

    return 0;
}

void *dpmgr_create_path(enum DDP_SCENARIO_ENUM scenario, void *cmdq_handle)
{
    int i = 0;
    int module_name;

    struct ddp_path_handle_t *path_handle = NULL;
    int *modules = ddp_get_scenario_list(scenario);
    int module_num = ddp_get_module_num(scenario);
    struct DDP_MANAGER_CONTEXT *content = _get_context();

    //path_handle = kzalloc(sizeof(uint8_t*) * sizeof(struct ddp_path_handle_t), GFP_KERNEL);
    memset((void *)(&g_handle), 0, sizeof(struct ddp_path_handle_t));
    path_handle = &g_handle;
    if (path_handle != NULL) {
        path_handle->cmdqhandle = cmdq_handle;
        path_handle->scenario = scenario;
        path_handle->hwmutexid = acquire_mutex(scenario);
        assign_default_irqs_table(scenario, path_handle->irq_event_map);
        DISP_LOG_I("create handle 0x%x on scenario %s\n", (unsigned int)path_handle,
                        ddp_get_scenario_name(scenario));
        for (i = 0; i < module_num; i++) {
            module_name = modules[i];
            content->module_usage_table[module_name]++;
            content->module_path_table[module_name] = path_handle;
        }
        content->handle_cnt++;
        content->handle_pool[path_handle->hwmutexid] = path_handle;
    } else {
        DISP_LOG_E("Fail to create handle on scenario %s\n", ddp_get_scenario_name(scenario));
    }
    return path_handle;
}

int dpmgr_destroy_path(void *dp_handle)
{
    int i = 0;
    int module_name;

    ASSERT(dp_handle != NULL);
    struct ddp_path_handle_t *handle = (struct ddp_path_handle_t *)dp_handle;
    int *modules = ddp_get_scenario_list(handle->scenario);
    int module_num = ddp_get_module_num(handle->scenario);
    struct DDP_MANAGER_CONTEXT *content = _get_context();

    DISP_LOG_I("destroy path handle 0x%x on scenario %s\n", (unsigned int)handle,
                    ddp_get_scenario_name(handle->scenario));
    if (handle != NULL) {
        release_mutex(handle->hwmutexid);
        _dpmgr_path_disconnect(handle->scenario, handle->cmdqhandle);
        for (i = 0; i < module_num; i++) {
            module_name = modules[i];
            content->module_usage_table[module_name]--;
            content->module_path_table[module_name] = NULL;
        }
        content->handle_cnt--;
        ASSERT(content->handle_cnt >= 0);
        content->handle_pool[handle->hwmutexid] = NULL;
        // kfree(handle);
    }
    return 0;
}

int dpmgr_path_add_dump(void *dp_handle, enum ENGINE_DUMP engine, int encmdq)
{
    ASSERT(dp_handle != NULL);
    struct ddp_path_handle_t *handle = (struct ddp_path_handle_t *)dp_handle;

    ASSERT(handle->scenario == DDP_SCENARIO_PRIMARY_DISP);
    void *cmdqHandle = encmdq ? handle->cmdqhandle : NULL;

    enum DISP_MODULE_ENUM wdma = ddp_is_scenario_on_primary(handle->scenario) ?
                                                        DISP_MODULE_WDMA0 : DISP_MODULE_WDMA0;
    // update contxt
    struct DDP_MANAGER_CONTEXT *context = _get_context();

    context->module_usage_table[wdma]++;
    context->module_path_table[wdma] = handle;
    handle->scenario = DDP_SCENARIO_PRIMARY_ALL;
    // update connected
    _dpmgr_path_connect(handle->scenario, cmdqHandle);
    ddp_mutex_set(handle->hwmutexid, handle->scenario, handle->mode, cmdqHandle);

    //wdma just need start.
    if (ddp_modules_driver[wdma] != 0) {
        if (ddp_modules_driver[wdma]->init != 0)
            ddp_modules_driver[wdma]->init(wdma, cmdqHandle);

        if (ddp_modules_driver[wdma]->start != 0)
            ddp_modules_driver[wdma]->start(wdma, cmdqHandle);
    }
    return 0;
}

int dpmgr_path_remove_dump(void *dp_handle, int encmdq)
{
    ASSERT(dp_handle != NULL);
    struct ddp_path_handle_t *handle = (struct ddp_path_handle_t *)dp_handle;

    ASSERT(handle->scenario == DDP_SCENARIO_PRIMARY_DISP ||
                    handle->scenario == DDP_SCENARIO_PRIMARY_ALL);
    void *cmdqHandle = encmdq ? handle->cmdqhandle : NULL;
    enum DISP_MODULE_ENUM wdma = ddp_is_scenario_on_primary(handle->scenario) ?
                                                    DISP_MODULE_WDMA0 : DISP_MODULE_WDMA0;
    // update contxt
    struct DDP_MANAGER_CONTEXT *context = _get_context();

    context->module_usage_table[wdma]--;
    context->module_path_table[wdma] = 0;
    //wdma just need stop
    if (ddp_modules_driver[wdma] != 0) {
        if (ddp_modules_driver[wdma]->stop != 0)
            ddp_modules_driver[wdma]->stop(wdma, cmdqHandle);

        if (ddp_modules_driver[wdma]->deinit != 0)
            ddp_modules_driver[wdma]->deinit(wdma, cmdqHandle);
    }
    _dpmgr_path_disconnect(DDP_SCENARIO_PRIMARY_OVL_MEMOUT, cmdqHandle);

    handle->scenario = DDP_SCENARIO_PRIMARY_DISP;
    // update connected
    _dpmgr_path_connect(handle->scenario, cmdqHandle);
    ddp_mutex_set(handle->hwmutexid, handle->scenario, handle->mode, cmdqHandle);

    return 0;
}

int dpmgr_path_set_dst_module(void *dp_handle, enum DISP_MODULE_ENUM dst_module)
{
    ASSERT(dp_handle != NULL);
    struct ddp_path_handle_t *handle = (struct ddp_path_handle_t *)dp_handle;

    ASSERT((handle->scenario >= 0  && handle->scenario < DDP_SCENARIO_MAX));
    DISP_LOG_I("set dst module on scenario %s, module %s\n",
               ddp_get_scenario_name(handle->scenario), ddp_get_module_name(dst_module));
    return ddp_set_dst_module(handle->scenario, dst_module);
}

enum DISP_MODULE_ENUM dpmgr_path_get_dst_module(void *dp_handle)
{
    ASSERT(dp_handle != NULL);
    struct ddp_path_handle_t *handle = (struct ddp_path_handle_t *)dp_handle;

    ASSERT((handle->scenario >= 0  && handle->scenario < DDP_SCENARIO_MAX));
    enum DISP_MODULE_ENUM dst_module = ddp_get_dst_module(handle->scenario);

    DISP_LOG_I("get dst module on scenario %s, module %s\n",
               ddp_get_scenario_name(handle->scenario), ddp_get_module_name(dst_module));
    return dst_module;
}

int  dpmgr_path_init(void *dp_handle, int encmdq)
{
    int i = 0;
    int module_name;

    ASSERT(dp_handle != NULL);
    struct ddp_path_handle_t *handle = (struct ddp_path_handle_t *)dp_handle;
    int *modules = ddp_get_scenario_list(handle->scenario);
    int module_num = ddp_get_module_num(handle->scenario);
    void *cmdqHandle = encmdq ? handle->cmdqhandle : NULL;
    enum DDP_SCENARIO_ENUM scn[2] = { DDP_SCENARIO_MAX, DDP_SCENARIO_MAX };
    bool has_dual_path = false;
    int k = 0;

    DISP_LOG_I("path init on scenario %s\n", ddp_get_scenario_name(handle->scenario));

    #ifndef MACH_FPGA
    //open top clock
    path_top_clock_on();
    #endif

    //turn off m4u
    ddp_path_m4u_off();
    //init ddp_path structure
    ddp_path_init();

    /* now we start to init this path */
    //seting mutex
    ddp_mutex_set(handle->hwmutexid,
                  handle->scenario,
                  handle->mode,
                  cmdqHandle);
    //connect path;
    _dpmgr_path_connect(handle->scenario, cmdqHandle);

    scn[0] = handle->scenario;
    has_dual_path = ddp_path_is_dual(scn[0]);
    if (has_dual_path)
        scn[1] = ddp_get_dual_module(scn[0]);

    // each module init
    for (k = 0; k <= has_dual_path; k++) {
        modules = ddp_get_scenario_list(scn[k]);
        module_num = ddp_get_module_num(scn[k]);

        for (i = 0; i < module_num; i++) {
            module_name = modules[i];
            if (ddp_modules_driver[module_name] != 0) {
                if (ddp_modules_driver[module_name]->init != 0)
                    ddp_modules_driver[module_name]->init(module_name, cmdqHandle);

                if (ddp_modules_driver[module_name]->set_listener != 0)
                    ddp_modules_driver[module_name]->set_listener(module_name, dpmgr_module_notify);
            }
        }
    }
    //after init this path will power on;
    handle->power_sate = 1;
    return 0;
}

int  dpmgr_path_deinit(void *dp_handle, int encmdq)
{
    int i = 0;
    int module_name;

    ASSERT(dp_handle != NULL);
    struct ddp_path_handle_t *handle = (struct ddp_path_handle_t *)dp_handle;
    int *modules = ddp_get_scenario_list(handle->scenario);
    int module_num = ddp_get_module_num(handle->scenario);
    void *cmdqHandle = encmdq ? handle->cmdqhandle : NULL;

    DISP_LOG_I("path deinit on scenario %s\n", ddp_get_scenario_name(handle->scenario));
    ddp_mutex_clear(handle->hwmutexid, cmdqHandle);
    _dpmgr_path_disconnect(handle->scenario, cmdqHandle);
    for (i = 0; i < module_num; i++) {
        module_name = modules[i];
        if (ddp_modules_driver[module_name] != 0) {
            if (ddp_modules_driver[module_name]->deinit != 0) {
                DISP_LOG_V("scenario %s deinit module  %s\n",
                           ddp_get_scenario_name(handle->scenario),
                           ddp_get_module_name(module_name));
                ddp_modules_driver[module_name]->deinit(module_name, cmdqHandle);
            }
            if (ddp_modules_driver[module_name]->set_listener != 0)
                ddp_modules_driver[module_name]->set_listener(module_name, NULL);
        }
    }
    handle->power_sate = 0;
    //close top clock when last path init
    path_top_clock_off();
    return 0;
}

int dpmgr_path_start(void *dp_handle, int encmdq)
{
    int i = 0;
    int module_name;

    ASSERT(dp_handle != NULL);
    struct ddp_path_handle_t *handle = (struct ddp_path_handle_t *)dp_handle;
    int *modules = ddp_get_scenario_list(handle->scenario);
    int module_num = ddp_get_module_num(handle->scenario);
    void *cmdqHandle = encmdq ? handle->cmdqhandle : NULL;
    enum DDP_SCENARIO_ENUM scn[2] = { DDP_SCENARIO_MAX, DDP_SCENARIO_MAX };
    bool has_dual_path = false;
    int k = 0;

    DISP_LOG_V("path start on scenario %s\n", ddp_get_scenario_name(handle->scenario));

    scn[0] = handle->scenario;
        has_dual_path = ddp_path_is_dual(scn[0]);
    if (has_dual_path)
        scn[1] = ddp_get_dual_module(scn[0]);

    for (k = 0; k <= has_dual_path; k++) {
        modules = ddp_get_scenario_list(scn[k]);
        module_num = ddp_get_module_num(scn[k]);

        for (i = 0; i < module_num; i++) {
            module_name = modules[i];
            if (ddp_modules_driver[module_name] != 0) {
                if (ddp_modules_driver[module_name]->start != 0)
                    ddp_modules_driver[module_name]->start(module_name, cmdqHandle);
            }
        }
    }
    return 0;
}

int dpmgr_path_stop(void *dp_handle, int encmdq)
{

    int i = 0;
    int module_name;

    ASSERT(dp_handle != NULL);
    struct ddp_path_handle_t *handle = (struct ddp_path_handle_t *)dp_handle;
    int *modules = ddp_get_scenario_list(handle->scenario);
    int module_num = ddp_get_module_num(handle->scenario);
    void *cmdqHandle = encmdq ? handle->cmdqhandle : NULL;

    DISP_LOG_I("path stop on scenario %s\n", ddp_get_scenario_name(handle->scenario));
    for (i = 0; i < module_num; i++) {
        module_name = modules[i];
        if (ddp_modules_driver[module_name] != 0) {
            if (ddp_modules_driver[module_name]->stop != 0) {
                DISP_LOG_V("scenario %s  stop module %s\n", ddp_get_scenario_name(handle->scenario),
                           ddp_get_module_name(module_name));
                ddp_modules_driver[module_name]->stop(module_name, cmdqHandle);
            }
        }
    }
    return 0;
}

int dpmgr_path_reset(void *dp_handle, int encmdq)
{
    int i = 0;
    int module_name;

    ASSERT(dp_handle != NULL);
    struct ddp_path_handle_t *handle = (struct ddp_path_handle_t *)dp_handle;
    int *modules = ddp_get_scenario_list(handle->scenario);
    int module_num = ddp_get_module_num(handle->scenario);
    void *cmdqHandle = encmdq ? handle->cmdqhandle : NULL;

    DISP_LOG_I("path reset on scenario %s\n", ddp_get_scenario_name(handle->scenario));
    for (i = 0; i < module_num; i++) {
        module_name = modules[i];
        if (ddp_modules_driver[module_name] != 0) {
            if (ddp_modules_driver[module_name]->reset != 0) {
                DISP_LOG_V("scenario %s  reset module %s\n",
                           ddp_get_scenario_name(handle->scenario),
                           ddp_get_module_name(module_name));
                ddp_modules_driver[module_name]->reset(module_name, cmdqHandle);
            }
        }
    }
    return 0;
}

static unsigned int dpmgr_is_PQ(enum DISP_MODULE_ENUM module)
{
    unsigned int isPQ = 0;

    switch (module) {
    case DISP_MODULE_COLOR0:
    case DISP_MODULE_CCORR0:
    case DISP_MODULE_AAL0:
    case DISP_MODULE_MDP_AAL4:
    case DISP_MODULE_GAMMA0:
    case DISP_MODULE_DITHER0:
    case DISP_MODULE_COLOR1:
    case DISP_MODULE_CCORR1:
    case DISP_MODULE_AAL1:
    case DISP_MODULE_MDP_AAL5:
    case DISP_MODULE_GAMMA1:
    case DISP_MODULE_DITHER1:
        isPQ = 1;
        break;
    default:
        isPQ = 0;
    }

    return isPQ;
}

struct tile_roi {
    u32 src_x;
    u32 src_y;
    u32 addr;
    enum DpColorFormat fmt;
    u32 pitch;
    u32 x;
    u32 y;
    u32 w;
    u32 h;
    u32 bg_w;
    u32 bg_h;
};

int dpmgr_path_convert_rdma_roi_to_layer(struct tile_roi *layer_roi,
                    const struct RDMA_CONFIG_STRUCT rdma_config,
                    const struct disp_ddp_path_config *pconfig)
{
    layer_roi->addr = rdma_config.address;
    layer_roi->fmt = rdma_config.inputFormat;
    layer_roi->pitch = rdma_config.pitch;
    layer_roi->w = rdma_config.width;
    layer_roi->h = rdma_config.height;

    return 0;
}

int dpmgr_path_convert_pipe_roi_to_rdma(struct RDMA_CONFIG_STRUCT *rdma_config,
                    const struct tile_roi pipe_roi)
{
    rdma_config->address = pipe_roi.addr;
    rdma_config->inputFormat = pipe_roi.fmt;
    rdma_config->pitch = pipe_roi.pitch;
    rdma_config->width = pipe_roi.w;
    rdma_config->height = pipe_roi.h;

    return 0;
}

int dpmgr_path_convert_ovl_roi_to_layer(struct tile_roi *layer_roi,
                    const struct OVL_CONFIG_STRUCT ovl_config,
                    const struct disp_ddp_path_config *pconfig)
{
    layer_roi->src_x = ovl_config.src_x;
    layer_roi->src_y = ovl_config.src_y;
    layer_roi->addr = ovl_config.addr;
    layer_roi->fmt = ovl_config.fmt;
    layer_roi->pitch = ovl_config.src_pitch;
    layer_roi->x = ovl_config.dst_x;
    layer_roi->y = ovl_config.dst_y;
    layer_roi->w = ovl_config.src_w;
    layer_roi->h = ovl_config.src_h;
    layer_roi->bg_w = pconfig->dst_w;
    layer_roi->bg_h = pconfig->dst_h;

    return 0;
}

int dpmgr_path_convert_pipe_roi_to_ovl(struct OVL_CONFIG_STRUCT *lconfig,
                       const struct tile_roi pipe_roi)
{
    lconfig->src_x = pipe_roi.src_x;
    lconfig->src_y = pipe_roi.src_y;
    lconfig->addr = pipe_roi.addr;
    lconfig->fmt = pipe_roi.fmt;
    lconfig->src_pitch = pipe_roi.pitch;
    lconfig->dst_x = pipe_roi.x;
    lconfig->dst_y = pipe_roi.y;
    lconfig->dst_w = pipe_roi.w;
    lconfig->dst_h = pipe_roi.h;

    return 0;
}

int dpmgr_path_split_layer_roi_to_pipe_roi(struct tile_roi *pipe_roi, struct tile_roi *layer_roi,
                       u32 left_bg_w, u32 right_bg_w, u32 is_left)
{
    u32 layer_dst_x1 = 0, layer_dst_x2 = 0;
    u32 bg_boundary_x = 0;

    layer_dst_x1 = layer_roi->x;
    layer_dst_x2 = layer_roi->x + layer_roi->w - 1;

    /* update pipe src_x, x, w, bg_w */
    if (is_left) {
        if (left_bg_w == 0) {
            DDPDBG("%s:left pipe:no bg ROI\n", __func__);
            return -1;
        }

        bg_boundary_x = left_bg_w - 1;
        pipe_roi->bg_w = left_bg_w;

        if (bg_boundary_x < layer_dst_x1) {
            pipe_roi->w = 0;
            pipe_roi->x = 0;
            pipe_roi->src_x = 0;
        } else {
            pipe_roi->x = layer_dst_x1;

            if (bg_boundary_x < layer_dst_x2)
                pipe_roi->w = bg_boundary_x - layer_dst_x1 + 1;
            else
                pipe_roi->w = layer_roi->w;

            pipe_roi->src_x = layer_roi->src_x;
        }
    } else if (!is_left) {
        if (right_bg_w == 0) {
            DDPDBG("%s:right pipe:no bg ROI\n", __func__);
            return -1;
        }

        bg_boundary_x = layer_roi->bg_w - right_bg_w;
        pipe_roi->bg_w = right_bg_w;

        if (layer_dst_x2 < bg_boundary_x) {
            pipe_roi->x = 0;
            pipe_roi->w = 0;
            pipe_roi->src_x = 0;
        } else {
            if (bg_boundary_x < layer_dst_x1) {
                pipe_roi->x = layer_dst_x1 - bg_boundary_x;
                pipe_roi->w = layer_roi->w;
                pipe_roi->src_x = layer_roi->src_x;
            } else {
                pipe_roi->x = 0;
                pipe_roi->w = layer_dst_x2 - bg_boundary_x + 1;

                if (layer_roi->addr)
                    pipe_roi->src_x = layer_roi->src_x + (bg_boundary_x - layer_dst_x1);
            }
        }
    }
    pipe_roi->src_y = layer_roi->src_y;
    pipe_roi->addr = layer_roi->addr;
    pipe_roi->fmt = layer_roi->fmt;
    pipe_roi->pitch = layer_roi->pitch;
    pipe_roi->y = layer_roi->y;
    pipe_roi->h = layer_roi->h;
    pipe_roi->bg_h = layer_roi->bg_h;

    return 0;
}

int dpmgr_path_set_dual_config(struct disp_ddp_path_config *src_config,
                   struct disp_ddp_path_config *dst_config, int is_left)
{
    int i = 0;

    memcpy(dst_config, src_config, sizeof(struct disp_ddp_path_config));

    if (src_config->rdma_dirty) {
        struct tile_roi layer_roi = { 0 };
        struct tile_roi pipe_roi = { 0 };
        u32 left_w = src_config->dst_w, right_w = 0;
        struct RDMA_CONFIG_STRUCT *s_rdma_config = NULL, *d_rdma_config = NULL;

        s_rdma_config = &src_config->rdma_config;
        d_rdma_config = &dst_config->rdma_config;

        if (src_config->is_dual) {
            left_w = s_rdma_config->width / 2;
            right_w = s_rdma_config->height / 2;
        }

        dpmgr_path_convert_rdma_roi_to_layer(&layer_roi, *d_rdma_config,
                             src_config);
        dpmgr_path_split_layer_roi_to_pipe_roi(&pipe_roi, &layer_roi,
                               left_w, right_w, is_left);
        dpmgr_path_convert_pipe_roi_to_rdma(d_rdma_config, pipe_roi);

        if (!is_left)
            dst_config->ovl_layer_scanned = 0;
    }

    if (src_config->ovl_dirty) {
        for (i = 0; i < TOTAL_OVL_LAYER_NUM; i++) {
            struct tile_roi layer_roi = { 0 };
            struct tile_roi pipe_roi = { 0 };
            u32 left_bg_w = src_config->dst_w, right_bg_w = 0;
            struct OVL_CONFIG_STRUCT *s_lconfig = NULL;
            struct OVL_CONFIG_STRUCT *d_lconfig = NULL;

            if (!dst_config->ovl_config[i].layer_en)
                continue;

            s_lconfig = &src_config->ovl_config[i];
            d_lconfig = &dst_config->ovl_config[i];

            if (src_config->is_dual) {
                left_bg_w = src_config->dst_w / 2;
                right_bg_w = src_config->dst_w / 2;
            }

            dpmgr_path_convert_ovl_roi_to_layer(&layer_roi, *d_lconfig,
                                src_config);
            dpmgr_path_split_layer_roi_to_pipe_roi(&pipe_roi, &layer_roi,
                                   left_bg_w, right_bg_w, is_left);
            dpmgr_path_convert_pipe_roi_to_ovl(d_lconfig, pipe_roi);

            if (!d_lconfig->dst_w)
                d_lconfig->layer_en = 0;
        }

        if (!is_left)
            dst_config->ovl_layer_scanned = 0;
    }

    return 0;
}

int dpmgr_path_config(void *dp_handle, struct disp_ddp_path_config *config, int encmdq)
{
    int i = 0;
    int module_name;

    ASSERT(dp_handle != NULL);
    struct ddp_path_handle_t *handle = (struct ddp_path_handle_t *)dp_handle;
    int *modules = ddp_get_scenario_list(handle->scenario);
    int module_num = ddp_get_module_num(handle->scenario);
    void *cmdqHandle = encmdq ? handle->cmdqhandle : NULL;
    bool has_dual_path = false;
    enum DDP_SCENARIO_ENUM scn[2] = { DDP_SCENARIO_MAX, DDP_SCENARIO_MAX };
    struct disp_ddp_path_config *path_config = NULL;
    int k = 0;

    DISP_LOG_V("path config ovl %d, rdma %d, wdma %d, dst %d on handle 0x%x scenario %s\n",
               config->ovl_dirty,
               config->rdma_dirty,
               config->wdma_dirty,
               config->dst_dirty,
               (unsigned int)handle,
               ddp_get_scenario_name(handle->scenario));

    scn[0] = handle->scenario;
    has_dual_path = ddp_path_is_dual(scn[0]);
    if (has_dual_path)
        scn[1] = ddp_get_dual_module(scn[0]);

    if (has_dual_path) {
        path_config = malloc(sizeof(struct disp_ddp_path_config));
        if (!path_config) {
            DISP_LOG_E("path_config NULL!\n");
            return -1;
        }
        config->is_dual = true;
    }

    memcpy(&handle->last_config, config, sizeof(struct disp_ddp_path_config));
    for (k = 0; k <= has_dual_path; k++) {
        modules = ddp_get_scenario_list(scn[k]);
        module_num = ddp_get_module_num(scn[k]);

        if (has_dual_path) {
            dpmgr_path_set_dual_config(&handle->last_config, path_config, !k);
            config = path_config;
        }

        if (has_dual_path)
            path_config->dst_w /= 2;

        for (i = 0; i < module_num; i++) {
            module_name = modules[i];
            if (ddp_modules_driver[module_name] != 0) {
                if (ddp_modules_driver[module_name]->config != 0)
                    ddp_modules_driver[module_name]->config(module_name, config, cmdqHandle);

                /*
                 * now all PQ modules are bypassed by itself in LK, don't need this,
                 * just for future using
                 */
                if (dpmgr_is_PQ(module_name) == 1) {
                    if (ddp_modules_driver[module_name]->bypass != NULL)
                        ddp_modules_driver[module_name]->bypass(module_name, 1);
                }
            }
        }
    }

    if (has_dual_path)
        free(path_config);

    return 0;
}

struct disp_ddp_path_config *dpmgr_path_get_last_config(void *dp_handle)
{
    ASSERT(dp_handle != NULL);
    struct ddp_path_handle_t *handle = (struct ddp_path_handle_t *)dp_handle;

    handle->last_config.ovl_dirty  = 0;
    handle->last_config.rdma_dirty = 0;
    handle->last_config.wdma_dirty = 0;
    handle->last_config.dst_dirty = 0;
    return &handle->last_config;
}

int dpmgr_path_build_cmdq(void *dp_handle, void *trigger_loop_handle,
                            enum CMDQ_STATE state)
{
    int i = 0;
    int module_name;

    ASSERT(dp_handle != NULL);
    struct ddp_path_handle_t *handle = (struct ddp_path_handle_t *)dp_handle;
    int *modules = ddp_get_scenario_list(handle->scenario);
    int module_num = ddp_get_module_num(handle->scenario);

    DISP_LOG_D("path build cmdq on scenario %s\n", ddp_get_scenario_name(handle->scenario));
    for (i = 0; i < module_num; i++) {
        module_name = modules[i];
        if (ddp_modules_driver[module_name] != 0) {
            if (ddp_modules_driver[module_name]->build_cmdq != 0) {
                DISP_LOG_D("%s build cmdq, state=%d\n", ddp_get_module_name(module_name), state);
                ddp_modules_driver[module_name]->build_cmdq(module_name, trigger_loop_handle,
                                                            state);
            }
        }
    }
    return 0;
}

int dpmgr_path_trigger(void *dp_handle, void *trigger_loop_handle, int encmdq)
{
    ASSERT(dp_handle != NULL);
    struct ddp_path_handle_t *handle = (struct ddp_path_handle_t *)dp_handle;

    DISP_LOG_V("%s on scenario %s\n", __func__, ddp_get_scenario_name(handle->scenario));

    int i = 0;
    int *modules = ddp_get_scenario_list(handle->scenario);
    int module_num = ddp_get_module_num(handle->scenario);
    int module_name;

    ddp_mutex_enable(handle->hwmutexid, handle->scenario, trigger_loop_handle);

    for (i = 0; i < module_num; i++) {
        module_name = modules[i];
        if (ddp_modules_driver[module_name] != 0) {
            if (ddp_modules_driver[module_name]->trigger != 0)
                ddp_modules_driver[module_name]->trigger(module_name, trigger_loop_handle);
        }
    }
    return 0;
}

int dpmgr_path_flush(void *dp_handle, int encmdq)
{
    ASSERT(dp_handle != NULL);
    struct ddp_path_handle_t *handle = (struct ddp_path_handle_t *)dp_handle;
    void *cmdqHandle = encmdq ? handle->cmdqhandle : NULL;

    DISP_LOG_I("path flush on scenario %s\n", ddp_get_scenario_name(handle->scenario));
    return ddp_mutex_enable(handle->hwmutexid, handle->scenario, cmdqHandle);
}

int dpmgr_path_power_off(void *dp_handle, enum CMDQ_SWITCH encmdq)
{
    int i = 0;
    int module_name;

    ASSERT(dp_handle != NULL);
    struct ddp_path_handle_t *handle = (struct ddp_path_handle_t *)dp_handle;
    int *modules = ddp_get_scenario_list(handle->scenario);
    int module_num = ddp_get_module_num(handle->scenario);

    DISP_LOG_I("path power off on scenario %s\n", ddp_get_scenario_name(handle->scenario));
    for (i = 0; i < module_num; i++) {
        module_name = modules[i];
        if (ddp_modules_driver[module_name] != 0) {
            if (ddp_modules_driver[module_name]->power_off != 0) {
                DISP_LOG_I(" %s power off\n", ddp_get_module_name(module_name));
                ddp_modules_driver[module_name]->power_off(module_name,
                                                            encmdq?handle->cmdqhandle:NULL);
            }
        }
    }
    handle->power_sate = 0;
    path_top_clock_off();
    return 0;
}

int dpmgr_path_power_on(void *dp_handle, enum CMDQ_SWITCH encmdq)
{
    int i = 0;
    int module_name;

    ASSERT(dp_handle != NULL);
    struct ddp_path_handle_t *handle = (struct ddp_path_handle_t *)dp_handle;
    int *modules = ddp_get_scenario_list(handle->scenario);
    int module_num = ddp_get_module_num(handle->scenario);

    DISP_LOG_I("path power on scenario %s\n", ddp_get_scenario_name(handle->scenario));
    path_top_clock_on();
    for (i = 0; i < module_num; i++) {
        module_name = modules[i];
        if (ddp_modules_driver[module_name] != 0) {
            if (ddp_modules_driver[module_name]->power_on != 0) {
                DISP_LOG_I("%s power on\n", ddp_get_module_name(module_name));
                ddp_modules_driver[module_name]->power_on(module_name,
                                                            encmdq?handle->cmdqhandle:NULL);
            }
        }
    }
    //modules on this path will resume power on;
    handle->power_sate = 1;
    return 0;
}

int dpmgr_path_set_parameter(void *dp_handle, int io_evnet, void *data)
{
    return 0;
}

int dpmgr_path_get_parameter(void *dp_handle, int io_evnet, void *data)
{
    return 0;
}

int dpmgr_path_is_idle(void *dp_handle)
{
    ASSERT(dp_handle != NULL);
    return !dpmgr_path_is_busy(dp_handle);
}

int dpmgr_path_is_busy(void *dp_handle)
{
    int i = 0;
    int module_name;

    ASSERT(dp_handle != NULL);
    struct ddp_path_handle_t *handle = (struct ddp_path_handle_t *)dp_handle;
    int *modules = ddp_get_scenario_list(handle->scenario);
    int module_num = ddp_get_module_num(handle->scenario);

    DISP_LOG_V("path check busy on scenario %s\n", ddp_get_scenario_name(handle->scenario));
    for (i = module_num-1; i >= 0; i--) {
        module_name = modules[i];
        if (ddp_modules_driver[module_name] != 0) {
            if (ddp_modules_driver[module_name]->is_busy != 0) {
                if (ddp_modules_driver[module_name]->is_busy(module_name)) {
                    DISP_LOG_V("%s is busy\n", ddp_get_module_name(module_name));
                    return 1;
                }
            }
        }
    }
    return 0;
}

int dpmgr_set_lcm_utils(void *dp_handle, void *lcm_drv, void *fdt)
{
    int i = 0;
    int module_name;

    ASSERT(dp_handle != NULL);
    struct ddp_path_handle_t *handle = (struct ddp_path_handle_t *)dp_handle;
    int *modules = ddp_get_scenario_list(handle->scenario);
    int module_num = ddp_get_module_num(handle->scenario);

    DISP_LOG_V("path set lcm drv handle 0x%x\n", (unsigned int)handle);
    for (i = 0; i < module_num; i++) {
        module_name = modules[i];
        if (ddp_modules_driver[module_name] != 0) {
            if ((ddp_modules_driver[module_name]->set_lcm_utils != 0) && lcm_drv) {
                DISP_LOG_I("%s set lcm utils\n", ddp_get_module_name(module_name));
                ddp_modules_driver[module_name]->set_lcm_utils(module_name, lcm_drv, fdt);
            }
        }
    }
    return 0;
}

int dpmgr_enable_event(void *dp_handle, enum DISP_PATH_EVENT event)
{
    ASSERT(dp_handle != NULL);
    struct ddp_path_handle_t *handle = (struct ddp_path_handle_t *)dp_handle;
    struct DPMGR_WQ_HANDLE *wq_handle = &handle->wq_list[event];

    DISP_LOG_I("enable event on scenario %s, event %d, irtbit 0x%x\n",
               ddp_get_scenario_name(handle->scenario),
               event,
               handle->irq_event_map[event].irq_bit);
    if (!wq_handle->init) {
        //init_waitqueue_head(&(wq_handle->wq));
        wq_handle->init = 1;
        wq_handle->data = 0;
        wq_handle->event = event;
    }
    return 0;
}

int dpmgr_map_event_to_irq(void *dp_handle, enum DISP_PATH_EVENT event,
                            enum DDP_IRQ_BIT irq_bit)
{
    ASSERT(dp_handle != NULL);
    struct ddp_path_handle_t *handle = (struct ddp_path_handle_t *)dp_handle;
    struct DDP_IRQ_EVENT_MAPPING *irq_table = handle->irq_event_map;

    if (event < DISP_PATH_EVENT_NUM) {
        DISP_LOG_I("map event %d to irq 0x%x on scenario %s\n", event, irq_bit,
                    ddp_get_scenario_name(handle->scenario));
        irq_table[event].irq_bit = irq_bit;
        return 0;
    }
    DISP_LOG_E("fail to map event %d to irq 0x%x on scenario %s\n", event, irq_bit,
                    ddp_get_scenario_name(handle->scenario));
    return -1;
}


int dpmgr_disable_event(void *dp_handle, enum DISP_PATH_EVENT event)
{
    ASSERT(dp_handle != NULL);
    struct ddp_path_handle_t *handle = (struct ddp_path_handle_t *)dp_handle;

    DISP_LOG_I("disable event %d on scenario %s\n",
                event, ddp_get_scenario_name(handle->scenario));
    struct DPMGR_WQ_HANDLE *wq_handle = &handle->wq_list[event];

    wq_handle->init = 0;
    wq_handle->data = 0;
    return 0;
}

int dpmgr_check_status(void *dp_handle)
{
    int i = 0, k = 0;
    int module_name;

    ASSERT(dp_handle != NULL);
    struct ddp_path_handle_t *handle = (struct ddp_path_handle_t *)dp_handle;
    int *modules = ddp_get_scenario_list(handle->scenario);
    int module_num = ddp_get_module_num(handle->scenario);
    enum DDP_SCENARIO_ENUM scn[2] = { DDP_SCENARIO_MAX, DDP_SCENARIO_MAX };
    bool has_dual_path = false;

    DISP_LOG_I("%s: %s, module_num %d\n", __func__,
               ddp_get_scenario_name(handle->scenario), module_num);
    ddp_check_path(handle->scenario);
    ddp_check_mutex(handle->hwmutexid, handle->scenario,
                    handle->mode);
    //ddp_check_signal(handle->scenario);
    scn[0] = handle->scenario;
    has_dual_path = ddp_path_is_dual(scn[0]);
    if (has_dual_path)
        scn[1] = ddp_get_dual_module(scn[0]);

    for (k = 0; k <= has_dual_path; k++) {
        modules = ddp_get_scenario_list(scn[k]);
        module_num = ddp_get_module_num(scn[k]);

        DDPDUMP("path:\n");
        for (i = 0; i < module_num; i++) {
            module_name = modules[i];
            ddp_dump_analysis(module_name);

            if (i == DISP_MODULE_DSI0 || i == DISP_MODULE_DSI1 || i == DISP_MODULE_DSIDUAL) {
                if (ddp_modules_driver[i] != 0) {
                    if (ddp_modules_driver[i]->dump_info != 0)
                        ddp_modules_driver[i]->dump_info(i, 1);
                }
            }
        }
    }

    ddp_dump_analysis(DISP_MODULE_CONFIG);
    ddp_dump_analysis(DISP_MODULE_MUTEX);
    for (k = 0; k <= has_dual_path; k++) {
        modules = ddp_get_scenario_list(scn[k]);
        module_num = ddp_get_module_num(scn[k]);
        for (i = 0; i < module_num; i++) {
            module_name = modules[i];
            ddp_dump_reg(module_name);
        }
    }
    ddp_dump_reg(DISP_MODULE_CONFIG);
    ddp_dump_reg(DISP_MODULE_MUTEX);
    return 0;
}

int dpmgr_wait_event_timeout(void *dp_handle, enum DISP_PATH_EVENT event, int timeout)
{
    int ret = -1;

    ASSERT(dp_handle != NULL);
    struct ddp_path_handle_t *handle = (struct ddp_path_handle_t *)dp_handle;
    struct DPMGR_WQ_HANDLE *wq_handle = &handle->wq_list[event];
    struct DDP_IRQ_EVENT_MAPPING *irqmap = handle->irq_event_map;
    enum DISP_MODULE_ENUM module = IRQBIT_MODULE(irqmap[event].irq_bit);
    int q_bit = IRQBIT_BIT(irqmap[event].irq_bit);

    if (wq_handle->init) {
        //DISP_LOG_V("wait event %d timeout %d on scenario %s\n", event, timeout,
        //               ddp_get_scenario_name(handle->scenario));
        //should poling
        if (ddp_modules_driver[module] != 0 && ddp_modules_driver[module]->polling_irq != 0)
            ret = ddp_modules_driver[module]->polling_irq(module, q_bit, timeout);

        if (ret > 0) {
            //DISP_LOG_V("received event  %d  left time %d on scenario %s\n", event, ret,
            //                 ddp_get_scenario_name(handle->scenario));

        } else {
            DISP_LOG_E("wait %d timeout ret %d on scenario %d\n", event, ret, handle->scenario);
            dpmgr_check_status(handle);
        }
        return ret;
    }
    DISP_LOG_E("wait event %d not initialized on scenario %s\n",
                event, ddp_get_scenario_name(handle->scenario));
    return ret;
}

int dpmgr_wait_event(void *dp_handle, enum DISP_PATH_EVENT event)
{
    int ret = -1;

    ASSERT(dp_handle != NULL);
    struct ddp_path_handle_t *handle = (struct ddp_path_handle_t *)dp_handle;
    struct DPMGR_WQ_HANDLE *wq_handle = &handle->wq_list[event];
    struct DDP_IRQ_EVENT_MAPPING *irqmap = handle->irq_event_map;
    enum DISP_MODULE_ENUM module = IRQBIT_MODULE(irqmap[event].irq_bit);
    int q_bit = IRQBIT_BIT(irqmap[event].irq_bit);

    if (wq_handle->init) {
        DISP_LOG_V("wait event %d on scenario %s\n", event,
                        ddp_get_scenario_name(handle->scenario));
        if (ddp_modules_driver[module] != 0 && ddp_modules_driver[module]->polling_irq != 0)
            ret = ddp_modules_driver[module]->polling_irq(module, q_bit, -1);

        DISP_LOG_V("received event  %d on scenario %s\n", event,
                        ddp_get_scenario_name(handle->scenario));
        return ret;
    }
    DISP_LOG_E("wait event %d not initialized on scenario %s\n",
                event, ddp_get_scenario_name(handle->scenario));
    return ret;
}

int dpmgr_signal_event(void *dp_handle, enum DISP_PATH_EVENT event)
{
    ASSERT(dp_handle != NULL);
    struct ddp_path_handle_t *handle = (struct ddp_path_handle_t *)dp_handle;

    if (handle->wq_list[event].init) {
        //wq_handle->data = sched_clock();
        DISP_LOG_V("wake up evnet %d on scenario %s\n",
                    event, ddp_get_scenario_name(handle->scenario));
        //wake_up_interruptible(&(handle->wq_list[event].wq));
    }
    return 0;
}

int dpmgr_init(void)
{
    DISP_LOG_I("ddp manager init\n");
    ddp_init_modules_driver();
    return 0;
}
