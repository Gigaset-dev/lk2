/*
 * Copyright (c) 2020 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#pragma once

#include "ddp_info.h"
#define ARRAY_SIZE(arr) (countof(arr))

enum DDP_MODE {
    DDP_VIDEO_MODE = 0,
    DDP_CMD_MODE,
};

enum DDP_SCENARIO_ENUM {
    DDP_SCENARIO_PRIMARY_DISP = 0,
    DDP_SCENARIO_PRIMARY_BYPASS_PQ_DISP,    /* bypass pq module */
    DDP_SCENARIO_PRIMARY_RDMA0_COLOR0_DISP0, /* by pass ovl */
    DDP_SCENARIO_PRIMARY_RDMA0_DISP,
    DDP_SCENARIO_PRIMARY_OVL_MEMOUT,
    DDP_SCENARIO_PRIMARY0_DISP0_MDP_AAL4,
    DDP_SCENARIO_PRIMARY0_OVL_PQ_MEMOUT,
    DDP_SCENARIO_PRIMARY_ALL,
    DDP_SCENARIO_SUB_DISP,
    DDP_SCENARIO_SUB_RDMA1_DISP,
    DDP_SCENARIO_SUB_OVL2_2L_MEMOUT,
    DDP_SCENARIO_SUB_ALL,

    /*dual pipe*/
    DDP_SCENARIO_PRIMARY1_DISP,
    DDP_SCENARIO_PRIMARY_RDMA1_COLOR1_DISP1,
    DDP_SCENARIO_PRIMARY_RDMA1_COLOR1_DISP0,
    DDP_SCENARIO_PRIMARY1_OVL_MEMOUT,
    DDP_SCENARIO_PRIMARY1_DISP0_MDP_AAL5,
    DDP_SCENARIO_PRIMARY1_ALL,

    DDP_SCENARIO_PRIMARY_DISP_LEFT,
    DDP_SCENARIO_PRIMARY_DISP_RIGHT,
    DDP_SCENARIO_MAX
};

void ddp_connect_path(enum DDP_SCENARIO_ENUM scenario, void *handle);
void ddp_disconnect_path(enum DDP_SCENARIO_ENUM scenario, void *handle);
int ddp_get_module_num(enum DDP_SCENARIO_ENUM scenario);

void ddp_check_path(enum DDP_SCENARIO_ENUM scenario);
int ddp_mutex_set(int mutex_id, enum DDP_SCENARIO_ENUM scenario, enum DDP_MODE mode, void *handle);
int ddp_mutex_clear(int mutex_id, void *handle);
int ddp1_mutex_clear(int mutex_id, void *handle);
int ddp_mutex_enable(int mutex_id, enum DDP_SCENARIO_ENUM scenario, void *handle);
int ddp_mutex_disable(int mutex_id, enum DDP_SCENARIO_ENUM scenario, void *handle);
void ddp_check_mutex(int mutex_id, enum DDP_SCENARIO_ENUM scenario, enum DDP_MODE mode);
int ddp_mutex_reset(int mutex_id, void *handle);
int ddp_mutex_set_sof_wait(int mutex_id, void *handle, int wait);

int ddp_is_moudule_in_mutex(int mutex_id, enum DISP_MODULE_ENUM module);

int ddp_mutex_add_module(int mutex_id, enum DISP_MODULE_ENUM module, void *handle);
int ddp_mutex_add_module_by_scenario(int mutex_id, enum DDP_SCENARIO_ENUM scenario, void *handle);

int ddp_mutex_remove_module(int mutex_id, enum DISP_MODULE_ENUM module, void *handle);

int ddp_mutex_Interrupt_enable(int mutex_id, void *handle);

int ddp_mutex_Interrupt_disable(int mutex_id, void *handle);


enum DISP_MODULE_ENUM ddp_get_dst_module(enum DDP_SCENARIO_ENUM scenario);
int ddp_set_dst_module(enum DDP_SCENARIO_ENUM scenario, enum DISP_MODULE_ENUM dst_module);

int *ddp_get_scenario_list(enum DDP_SCENARIO_ENUM ddp_scenario);

int ddp_insert_module(enum DDP_SCENARIO_ENUM ddp_scenario, enum DISP_MODULE_ENUM place,
              enum DISP_MODULE_ENUM module);
int ddp_remove_module(enum DDP_SCENARIO_ENUM ddp_scenario, enum DISP_MODULE_ENUM module);

int ddp_is_scenario_on_primary(enum DDP_SCENARIO_ENUM scenario);

const char *ddp_get_scenario_name(enum DDP_SCENARIO_ENUM scenario);

int   ddp_path_top_clock_off(void);
int   ddp_path_top_clock_on(void);
int   ddp_path_m4u_off(void);
int   ddp_path_init(void);
void primary_disp_module_list_insert_dsc(void);
//should remove
int ddp_insert_config_allow_rec(void *handle);
int ddp_insert_config_dirty_rec(void *handle);
enum DDP_SCENARIO_ENUM ddp_get_dual_module(enum DDP_SCENARIO_ENUM ddp_scenario);
bool ddp_path_is_dual(enum DDP_SCENARIO_ENUM ddp_scenario);
