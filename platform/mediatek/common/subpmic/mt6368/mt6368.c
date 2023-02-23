/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#include <debug.h>
#include <err.h>
#include <platform.h>
#include <platform_halt.h>
#include <spmi_common.h>

/* PMIC Registers for PSC */
#define MT6368_TOP_CFG_ELR5                 0x14b

static struct spmi_device *mt6368_devs;

static void mt6368_set_seq_off(struct spmi_device *mt6368_dev, u8 en_seq_off)
{
    spmi_ext_register_writel(mt6368_dev, MT6368_TOP_CFG_ELR5, &en_seq_off, 1);
}

static status_t mt6368_shutdown_callback(void *data,
                                         platform_halt_action suggested_action,
                                         platform_halt_reason reason)
{
    struct spmi_device *mt6368_dev = (struct spmi_device *)data;

    if ((suggested_action == HALT_ACTION_SHUTDOWN) ||
        (suggested_action == HALT_ACTION_REBOOT && reason == HALT_REASON_POR))
        mt6368_set_seq_off(mt6368_dev, 1);

    return NO_ERROR;
}

#ifdef PROJECT_TYPE_FPGA
__WEAK int mt6368_device_register(void)
{
    return ERR_NOT_SUPPORTED;
}
#else
int mt6368_device_register(void)
{
    int ret;

    mt6368_devs = get_spmi_device(SPMI_MASTER_1, SPMI_SLAVE_5);
    if (!mt6368_devs)
        return ERR_INVALID_ARGS;

    ret = register_platform_halt_callback("mt6368_shutdown_callback",
                                          &mt6368_shutdown_callback, mt6368_devs);
    if (ret) {
        dprintf(CRITICAL, "%s register shutdown callback failed, ret=%d\n", __func__, ret);
        return ret;
    }
    return NO_ERROR;
}
#endif
