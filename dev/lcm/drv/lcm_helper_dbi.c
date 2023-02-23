/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#include "lcm_helper.h"

static char *mtk_dbi_lcm_list;
static unsigned int mtk_dbi_lcm_count;

int mtk_dbi_lcm_list_init(void)
{
    int count = 0;

    count = mtk_lcm_list_count(MTK_LCM_NAME_DBI_KEY);
    if (count <= 0)
        return count;

    LCM_MALLOC(mtk_dbi_lcm_list, count * MTK_LCM_NAME_LENGTH);
    if (mtk_dbi_lcm_list == NULL) {
        LCM_MSG("%s, failed to allocate buffer, count:%u\n",
            __func__, count);
        return -ENOMEM;
    }

    mtk_dbi_lcm_count = mtk_lcm_list_parsing(mtk_dbi_lcm_list,
                 count, MTK_LCM_NAME_DBI_KEY);

    return mtk_dbi_lcm_count;
}

unsigned int mtk_lcm_get_dbi_count(void)
{
    return mtk_dbi_lcm_count;
}

const char *mtk_lcm_get_dbi_name(unsigned int id)
{
    if (id >= mtk_lcm_get_dbi_count()) {
        LCM_MSG("%s, invalid lcm index:%u\n", __func__, id);
        return NULL;
    }
    return mtk_dbi_lcm_list + id * MTK_LCM_NAME_LENGTH;
}

int parse_lcm_params_dbi(const void *fdt, int nodeoffset,
        LCM_DBI_PARAMS *params, struct mtk_panel_misc_data *misc)
{
    if (fdt == NULL || params == NULL || misc == NULL)
        return -ENOMEM;

    memset(params, 0, sizeof(LCM_DBI_PARAMS));
    return 0;
}

int parse_lcm_ops_dbi(const void *fdt, int nodeoffset,
        struct mtk_lcm_ops_dbi *ops,
        LCM_DBI_PARAMS *params,
        struct mtk_panel_cust *cust)
{
    if (fdt == NULL || ops == NULL)
        return -ENOMEM;

    memset(ops, 0, sizeof(struct mtk_lcm_ops_dbi));
    return 0;
}

void dump_lcm_params_dbi(LCM_DBI_PARAMS *params,
    struct mtk_panel_cust *cust)
{
}

void dump_lcm_ops_dbi(struct mtk_lcm_ops_dbi *ops,
        LCM_DBI_PARAMS *params,
        struct mtk_panel_cust *cust)
{
}

void free_lcm_params_dbi(LCM_DBI_PARAMS *params)
{
    if (params == NULL) {
        LCM_MSG("%s:%d, ERROR: invalid params/ops\n",
            __FILE__, __LINE__);
        return;
    }
}

void free_lcm_ops_dbi(struct mtk_lcm_ops_dbi *ops)
{
    LCM_FREE(ops, sizeof(struct mtk_lcm_ops_dbi));
}
