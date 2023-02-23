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

#pragma once

#include "ddp_info.h"


typedef enum {
    DDP_VIDEO_MODE = 0,
    DDP_CMD_MODE,
} DDP_MODE;

typedef enum {
    DDP_SCENARIO_PRIMARY_DISP = 0,
    DDP_SCENARIO_PRIMARY_RDMA0_COLOR0_DISP,
    DDP_SCENARIO_PRIMARY_RDMA0_DISP,
    DDP_SCENARIO_PRIMARY_OVL_MEMOUT,
    DDP_SCENARIO_PRIMARY_ALL,
    DDP_SCENARIO_SUB_DISP,
    DDP_SCENARIO_SUB_RDMA1_DISP,
    DDP_SCENARIO_SUB_OVL_MEMOUT,
    DDP_SCENARIO_SUB_ALL,
    DDP_SCENARIO_MAX
} DDP_SCENARIO_ENUM;

void ddp_connect_path(DDP_SCENARIO_ENUM scenario, void *handle);
void ddp_disconnect_path(DDP_SCENARIO_ENUM scenario, void *handle);
int ddp_get_module_num(DDP_SCENARIO_ENUM scenario);

void ddp_check_path(DDP_SCENARIO_ENUM scenario);
int ddp_mutex_set(int mutex_id, DDP_SCENARIO_ENUM scenario, DDP_MODE mode, void *handle);
int ddp_mutex_clear(int mutex_id, void *handle);
int ddp_mutex_enable(int mutex_id, DDP_SCENARIO_ENUM scenario, void *handle);
int ddp_mutex_disable(int mutex_id, DDP_SCENARIO_ENUM scenario, void *handle);
int ddp_mutex_get(int mutex_id, DDP_SCENARIO_ENUM scenario, void *handle);
int ddp_mutex_release(int mutex_id, DDP_SCENARIO_ENUM scenario, void *handle);
void ddp_check_mutex(int mutex_id, DDP_SCENARIO_ENUM scenario, DDP_MODE mode);
int ddp_mutex_reset(int mutex_id, void *handle);
int ddp_mutex_set_sof_wait(int mutex_id, cmdqRecHandle handle, int wait);

int ddp_is_moudule_in_mutex(int mutex_id, DISP_MODULE_ENUM module);

int ddp_mutex_add_module(int mutex_id, DISP_MODULE_ENUM module, void *handle);

int ddp_mutex_remove_module(int mutex_id, DISP_MODULE_ENUM module, void *handle);

int ddp_mutex_Interrupt_enable(int mutex_id, void *handle);

int ddp_mutex_Interrupt_disable(int mutex_id, void *handle);


DISP_MODULE_ENUM ddp_get_dst_module(DDP_SCENARIO_ENUM scenario);
int ddp_set_dst_module(DDP_SCENARIO_ENUM scenario, DISP_MODULE_ENUM dst_module);

int *ddp_get_scenario_list(DDP_SCENARIO_ENUM ddp_scenario);

int ddp_insert_module(DDP_SCENARIO_ENUM ddp_scenario, DISP_MODULE_ENUM place,
                      DISP_MODULE_ENUM module);
int ddp_remove_module(DDP_SCENARIO_ENUM ddp_scenario, DISP_MODULE_ENUM module);

int ddp_is_scenario_on_primary(DDP_SCENARIO_ENUM scenario);

const char *ddp_get_scenario_name(DDP_SCENARIO_ENUM scenario);

int   ddp_path_top_clock_off(void);
int   ddp_path_top_clock_on(void);
int   ddp_path_m4u_off(void);
int   ddp_path_init(void);

//should remove
int ddp_insert_config_allow_rec(void *handle);
int ddp_insert_config_dirty_rec(void *handle);
const char *ddp_get_mutex_sof_name(unsigned int regval);

