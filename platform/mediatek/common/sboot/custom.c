/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#include <custom.h>
#include <debug.h>
#include <err.h>
#include <lock_state.h>
#include <verified_boot_error.h>

/* Replace here with custom struct declare */
static struct mtk_param_t *g_param;

void set_param(void *param)
{
    /* Replace here with custom struct declare */
    g_param = (struct mtk_param_t *)param;
}

void *get_param(void)
{
    return g_param;
}

/* Redefine this function */
__WEAK int custom_get_lock_state(uint32_t *lock_state)
{
    dprintf(CRITICAL, "Custom get lock state...\n");

    /* Replace with custom implementation */
    return sec_cfg_get_lock_state(lock_state);
}

/* Redefine this function */
__WEAK int custom_error_process(struct res_state_t *res)
{
    char *err_str = NULL;

    switch (res->err) {
    case ERR_VB_LKS_SECCFG_FAIL:
        err_str = "[seccfg error]";
        break;
    case ERR_VB_LKS_CUSTOM_FAIL:
        err_str = "[custom error]";
        break;
    case ERR_VB_LKS_NOT_EQUAL:
        err_str = "[state not equal]";
        break;
    default:
        err_str = "[unknown error]";
    }

    dprintf(CRITICAL, "Custom_error_process%s: err: %d, cust_state: %d, seccfg_state: %d\n"
            , err_str, res->err, res->cust_state, res->seccfg_state);

    return NO_ERROR;
}

/* Redefine this function */
__WEAK int custom_set_lock_state(uint32_t lock_state)
{
    dprintf(CRITICAL, "Custom set lock state...\n");

    /* Replace with custom param struct here */
    struct mtk_param_t *param;

    param = (struct mtk_param_t *)get_param();
    if (param != NULL)
        dprintf(CRITICAL, "param->sz: %s, param->size: %d\n", param->sz, param->size);

    /* Replace with custom implementation */
    return sec_cfg_set_lock_state(lock_state);
}
