/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#pragma once

#include <ctype.h>
#include <debug.h>
#include "lcm_gateic.h"
#include <malloc.h>
#include <platform.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef MTK_ROUND_CORNER_SUPPORT
#include "round_corner/lcm_rc.h"
#endif

#define MTK_LCM_MODE_UNIT (4)
#define MTK_LCM_NAME_DBI_KEY "mtk_dbi_"
#define MTK_LCM_NAME_DPI_KEY "mtk_dpi_"
#define MTK_LCM_NAME_DSI_KEY "mtk_dsi_"

/* mtk_lcm_delay_data
 * used to MTK_LCM_UTIL_TYPE_TDELAY
 * timeout: period for timeout
 * form: timeout count from when time
 */
struct mtk_lcm_delay_data {
    u32 timeout;
    u32 from;
};

/* mtk_lcm_dcs_lk_data
 * used to MTK_LCM_LK_TYPE_PREPARE_PARAM
 * data: dsi_set_cmdq required an array of u32
 * index: data index of the dsi_set_cmdq u32 array
 */
struct mtk_lcm_dcs_lk_data {
    u32 data;
    u32 index;
};

/* mtk_lcm_dcs_lk_data_bit
 * used to MTK_LCM_LK_TYPE_PREPARE_PARAM_?_BIT
 * data: each u32 data has 4 u8 data
 * index: data index of the dsi_set_cmdq u32 array
 * bit: data bit offset of the u32 data
 */
struct mtk_lcm_dcs_lk_data_bit {
    u8 data;
    u32 index;
    u8 bit;
};

/* mtk_lcm_dcs_cmd_data
 * used to MTK_LCM_CMD_TYPE_WRITE_BUFFER, MTK_LCM_CMD_TYPE_READ_BUFFER
 * cmd: the read/write command or address
 * data: the write data buffer, or returned read buffer
 * data_len: the data buffer length
 * start_id: for read command the returned data will be saved at which index of out buffer
 */
struct mtk_lcm_dcs_cmd_data {
    u8 cmd;
    u8 *data;
    unsigned int data_len;
    unsigned int start_id;
};

/* mtk_lcm_gpio_data
 * used to MTK_LCM_GPIO_TYPE_MODE, MTK_LCM_GPIO_TYPE_OUT
 * gpio_id: gpio index
 * data: the settings of gpio
 */
struct mtk_lcm_gpio_data {
    u8 gpio_id;
    u8 data;
};

/* mtk_lcm_cb_id_data
 * used to MTK_LCM_CB_TYPE_xx
 * id: the input parameter index
 * buffer_data: the ddic command data
 */
struct mtk_lcm_cb_id_data {
    unsigned int id_count;
    u8 *id;
    unsigned int data_count;
    u8 *buffer_data;
};

/* the union of lcm operation data*/
union mtk_lcm_ops_data_params {
    int util_data; /*used for MTK_LCM_UTIL_TYPE_xx*/
    struct mtk_lcm_delay_data delay_data;
    unsigned int lk_data_count;
    struct mtk_lcm_dcs_lk_data lk_data;
    struct mtk_lcm_dcs_lk_data_bit lk_data_bit;
    struct mtk_lcm_dcs_cmd_data cmd_data;
    struct mtk_lcm_gpio_data gpio_data;
    struct mtk_lcm_cb_id_data cb_id_data;
    void *cust_data; /*used for customization operation data*/
};

/* mtk_lcm_ops_data
 * used to save lcm operation cmd
 * func: MTK_LCM_FUNC_DSI/DBI/DPI
 * type: operation type from MTK_LCM_UTIL_TYPE_START to MTK_LCM_CUST_TYPE_END
 * size: the dts string length for parsing lcm operation
 * param: the parsing result of lcm operation params
 */
struct mtk_lcm_ops_data {
    struct list_node node;
    unsigned int func;
    unsigned int type;
    unsigned int size;
    union mtk_lcm_ops_data_params param;
};

/* mtk_lcm_ops_table
 * used to store the lcm operation commands
 * list: operation command list
 * size: operation command count
 */
struct mtk_lcm_ops_table {
    struct list_node list;
    unsigned int size;
};

/* mtk_lcm_ops_dbi
 * used to save the dbi operation list
 */
struct mtk_lcm_ops_dbi {
    unsigned int dbi_private_data;
};

/* mtk_lcm_ops_dpi
 * used to save the dpi operation list
 */
struct mtk_lcm_ops_dpi {
    unsigned int dpi_private_data;
};

/* mtk_lcm_ops_dsi
 * used to save the dsi operation list
 * xx_size: the operation count of xx function
 * xx: the operation data list of xx function
 */
struct mtk_lcm_ops_dsi {
    /* panel init & deinit */
    struct mtk_lcm_ops_table prepare;
    struct mtk_lcm_ops_table unprepare;

    /* panel backlight update*/
    unsigned int set_backlight_mask;
    struct mtk_lcm_ops_table set_backlight_cmdq;

    /* set display on*/
    struct mtk_lcm_ops_table set_display_on;

    /* lcm update*/
    struct mtk_lcm_ops_table lcm_update;

    /* panel compare id check*/
    unsigned int compare_id_value_length; /* the expected compare id length*/
    u8 *compare_id_value_data; /*the expected compare id value*/
    struct mtk_lcm_ops_table compare_id;

#if MTK_LCM_DEBUG_DUMP
    struct mtk_lcm_ops_table gpio_test;
#endif
};

struct mtk_lcm_ops {
    struct mtk_lcm_ops_dbi *dbi_ops;
    struct mtk_lcm_ops_dpi *dpi_ops;
    struct mtk_lcm_ops_dsi *dsi_ops;
};

struct mtk_lcm_pos {
    u8 msb;
    u8 lsb;
};

struct mtk_lcm_location {
    struct mtk_lcm_pos x0;
    struct mtk_lcm_pos x1;
    struct mtk_lcm_pos y0;
    struct mtk_lcm_pos y1;
};

/* customization callback of private panel operation
 * parse_params:
 *      used to save panel parameters parsed from dtsi
 * parse_ops:
 *      used to save panel operation cmd list parsed from dtsi
 * func:
 *      used to execute the customized operation cmd
 * dump:
 *      used to dump the customized settings in params (optional)
 * free:
 *    used to deallocate the memory buffer of panel parsing result
 */
struct mtk_panel_cust {
    unsigned int cust_enabled;
    int (*parse_params)(const void *fdt, int nodeoffset);
    int (*parse_ops)(unsigned int func,
        int type, u8 *data_in, unsigned int size_in,
        void *cust_data);
    int (*func)(struct mtk_lcm_ops_data *op,
        u8 *data, unsigned int size);
    void (*dump_params)(void);
    void (*dump_ops)(struct mtk_lcm_ops_data *op,
        const char *owner, unsigned int id);
    void (*free_ops)(unsigned int func);
    void (*free_params)(unsigned int func);
};

struct mtk_panel_misc_data {
    unsigned int is_fake;
    unsigned int fake_height;
    unsigned int fake_width;
    unsigned int voltage;
};

struct mtk_panel_resource {
    unsigned int version;
    LCM_PARAMS params;
    struct mtk_lcm_ops ops;
    struct mtk_panel_cust cust;
    struct mtk_panel_misc_data misc;
};

/* function: parse dts settings into u8
 * If the property is not found or the value is not set,
 * return default value 0.
 */
int lcm_fdt_getprop_u8(const void *fdt, int nodeoffset,
        const char *name, unsigned char *out_value);

/* function: parse dts settings into u32
 * If the property is not found or the value is not set,
 * return default value 0.
 */
int lcm_fdt_getprop_u32(const void *fdt, int nodeoffset,
        const char *name, unsigned int *out_value);

/* function: parse dts settings into u8 array
 * If the property is not found or the value is not set,
 * return default value 0.
 */
int lcm_fdt_getprop_u8_array_safe(const void *fdt, int nodeoffset,
        const char *name, unsigned char *out_value, unsigned int out_size);

int lcm_fdt_getprop_u8_array(const void *fdt, int nodeoffset,
        const char *name, unsigned char **out_value);

/* function: parse dts settings into u32 array
 * If the property is not found or the value is not set,
 * use default value 0.
 */
int lcm_fdt_getprop_u32_array(const void *fdt, int nodeoffset,
        const char *name, unsigned int *out_value, unsigned int out_size);

/* function: update dts settings of unsigned int
 * input:
 *    fdt: dts node
 *    nodeoffset: the target node
 *    node: the target property name
 *    value: the target property value
 * output:
 *    0 for success, else for failed
 */
int lcm_fdt_update_dts_value(void *fdt, int target_offset,
        const char *node, unsigned int value);

/* function: parse dts settings of string
 * If the property is not found or the value is not set,
 * out_value will be unchanged.
 */
int lcm_fdt_getprop_string(void *fdt, int nodeoffset,
        const char *name, char *out_value,
        unsigned int out_size);

/* function: update dts settings of string
 * input:
 *    fdt: dts node
 *    nodeoffset: the target node
 *    node: the target property name
 *    value: the target property value
 * output:
 *    0 for success, else for failed
 */
int lcm_fdt_update_dts_string(void *fdt, int nodeoffset,
        const char *node, const char *value);

/* function: update dts settings of node
 * input:
 *    fdt: dts node
 *    src_offset: the source node
 *    target_offset: the target node
 *    node: the target property name
 * output:
 *    0 for success, else for failed
 */
int lcm_fdt_update_dts_prop(void *fdt, int src_offset,
        int target_offset, const char *node);

int load_panel_resource_from_dts(const void *fdt, int nodeoffset,
        struct mtk_panel_resource *data);

int parse_lcm_ops_func(const void *fdt, int nodeoffset,
        struct mtk_lcm_ops_table *table,
        const char *func,
        unsigned int panel_type,
        struct mtk_panel_cust *cust, unsigned int phase);

/* function: execute lcm operations
 * input: table: lcm operation list
 *        table_size: lcm operation count
 *        panel_resource: lcm parameters
 *        data: the private data buffer of lcm operation
 *        size: the private data buffer size of lcm operation
 *        owner: the owner description
 * output: 0 for success, else of failed
 */
int mtk_panel_execute_operation(struct mtk_lcm_ops_table *table,
        const struct mtk_panel_resource *panel_resource,
        void *data, unsigned int size, const char *owner);

/* function: parse lcm parameters
 * input: fdt: dts, nodeoffset: dts node,
 *        params:the returned parsing result
 * output: 0 for success, else of failed
 */
int parse_lcm_params_dbi(const void *fdt, int nodeoffset,
        LCM_DBI_PARAMS *params, struct mtk_panel_misc_data *misc);
int parse_lcm_params_dpi(const void *fdt, int nodeoffset,
        LCM_DPI_PARAMS *params, struct mtk_panel_misc_data *misc);
int parse_lcm_params_dsi(const void *fdt, int nodeoffset,
        LCM_DSI_PARAMS *params, struct mtk_panel_misc_data *misc);

/* function: parse lcm operations
 * input: fdt: dts, nodeoffset: dts node,
 *        ops:the returned parsing result
 *        params: the lcm parameters already parsed
 * output: 0 for success, else of failed
 */
int parse_lcm_ops_dbi(const void *fdt, int nodeoffset,
        struct mtk_lcm_ops_dbi *ops,
        LCM_DBI_PARAMS *params,
        struct mtk_panel_cust *cust);
int parse_lcm_ops_dpi(const void *fdt, int nodeoffset,
        struct mtk_lcm_ops_dpi *ops,
        LCM_DPI_PARAMS *params,
        struct mtk_panel_cust *cust);
int parse_lcm_ops_dsi(const void *fdt, int nodeoffset,
        struct mtk_lcm_ops_dsi *ops,
        LCM_DSI_PARAMS *params,
        struct mtk_panel_cust *cust);

void free_lcm_params_dbi(LCM_DBI_PARAMS *params);
void free_lcm_params_dpi(LCM_DPI_PARAMS *params);
void free_lcm_params_dsi(LCM_DSI_PARAMS *params);
void free_lcm_ops_dbi(struct mtk_lcm_ops_dbi *ops);
void free_lcm_ops_dpi(struct mtk_lcm_ops_dpi *ops);
void free_lcm_ops_dsi(struct mtk_lcm_ops_dsi *ops);

/* function: dump dts settings of lcm driver*/
void dump_lcm_ops_func(struct mtk_lcm_ops_table *table,
        struct mtk_panel_cust *cust,
        const char *owner);
void dump_lcm_params_dsi(LCM_DSI_PARAMS *params,
         struct mtk_panel_cust *cust);
void dump_lcm_ops_dsi(struct mtk_lcm_ops_dsi *ops,
        LCM_DSI_PARAMS *params,
         struct mtk_panel_cust *cust);
void dump_lcm_params_dbi(LCM_DBI_PARAMS *params,
         struct mtk_panel_cust *cust);
void dump_lcm_ops_dbi(struct mtk_lcm_ops_dbi *ops,
        LCM_DBI_PARAMS *params,
         struct mtk_panel_cust *cust);
void dump_lcm_params_dpi(LCM_DPI_PARAMS *params,
         struct mtk_panel_cust *cust);
void dump_lcm_ops_dpi(struct mtk_lcm_ops_dpi *ops,
        LCM_DPI_PARAMS *params,
        struct mtk_panel_cust *cust);
void dump_lcm_params_basic(LCM_PARAMS *params);
#ifdef MTK_ROUND_CORNER_SUPPORT
void dump_lcm_round_corner_params(LCM_ROUND_CORNER *params);
#endif
void mtk_lcm_dump_all(char func, struct mtk_panel_resource *resource,
        struct mtk_panel_cust *cust);

/* function: dsi ddic write
 * input: data: the data buffer, force_update: force to do operation
 * output: 0 for success, else for failed
 */
int mtk_panel_dsi_dcs_write_buffer(struct mtk_lcm_dcs_cmd_data *data,
        unsigned char force_update);
int mtk_panel_dsi_dcs_write_lk(unsigned int *data,
        unsigned int data_len, unsigned char force_update);

/* function: dsi ddic write
 * input: data: the returned data buffer
 * output: 0 for success, else for failed
 */
int mtk_panel_dsi_dcs_read_buffer(struct mtk_lcm_dcs_cmd_data *data);

/* function: get lcm drvier count
 * output: the lcm driver count
 */
unsigned int mtk_lcm_get_drv_count(char func);
unsigned int mtk_lcm_get_dbi_count(void);
unsigned int mtk_lcm_get_dpi_count(void);
unsigned int mtk_lcm_get_dsi_count(void);

/* function: get lcm drvier name
 * input: the lcm index
 * output: lcm name of lcm index
 */
const char *mtk_lcm_get_drv_name(char func, unsigned int id);
const char *mtk_lcm_get_dbi_name(unsigned int id);
const char *mtk_lcm_get_dpi_name(unsigned int id);
const char *mtk_lcm_get_dsi_name(unsigned int id);

/* function: lcm power control
 * input:
 *     func: DSI, DBI, DPI
 *     value: high, low
 */
int mtk_lcm_dsi_power_on(void);
int mtk_lcm_dsi_power_off(void);
int mtk_lcm_dsi_power_reset(int value);

/* function: free lcm operation data
 * input: operation cmd list, and size
 */
void free_lcm_ops_table(struct mtk_lcm_ops_table *table,
        const char *func);
void free_lcm_resource(char func, struct mtk_panel_resource *data);

/* function: get lcm count
 * input: key word of lcm type
 * output: failed when <= 0
 */
int mtk_lcm_list_count(const char *key);

/* function: parsing lcm list name
 * input: out_value: used to save the lcm name buffer
 *       count: the lcm name
 *       key: key word of lcm type
 * output: the lcm count parsed successfully
 */
int mtk_lcm_list_parsing(char *out_value, unsigned int count, const char *key);

/* function: init lcm list
 * output: lcm count, failed when <= 0
 */
int mtk_lcm_list_init(char func);
int mtk_dbi_lcm_list_init(void);
int mtk_dpi_lcm_list_init(void);
int mtk_dsi_lcm_list_init(void);

