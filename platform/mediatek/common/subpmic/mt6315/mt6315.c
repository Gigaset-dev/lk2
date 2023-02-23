/*
 * Copyright (c) 2020 MediaTek Inc.
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

#define MT6315_PMIC_RG_TOP2_RSV2            0x139
#define MT6315_PMIC_TMA_KEY                 0x39F
#define MT6315_PMIC_DIG_WPK_KEY             0x3A8

#define MT6315_PMIC_VBUCK1_OP_EN            0x148D
#define MT6315_PMIC_VBUCK1_OP_CFG           0x1493
#define MT6315_VBUCK_RG_OFFSET              0x80

#define MT6315_PMIC_RG_LDO_VDIG18_HW_OP_EN          0x196
#define MT6315_PMIC_RG_LDO_VDIG18_HW_OP_EN_SHIFT    0x2

static struct spmi_device *mt6315_devs[SPMI_MAX_SLAVE_ID];

enum MT6315_KEY {
    TMA_KEY,
    DIG_WPK_KEY,
};

static u16 mt6315_key_addr[] = {
    [TMA_KEY] = MT6315_PMIC_TMA_KEY,
    [DIG_WPK_KEY] = MT6315_PMIC_DIG_WPK_KEY,
};

static void mt6315_set_lock_reg(struct spmi_device *mt6315_dev,
                                enum MT6315_KEY key_type, u16 val)
{
    u8 wdata;

    wdata = val & 0xFF;
    spmi_ext_register_writel(mt6315_dev,
                             mt6315_key_addr[key_type], &wdata, 1);
    wdata = (val & 0xFF00) >> 8;
    spmi_ext_register_writel(mt6315_dev,
                             mt6315_key_addr[key_type] + 1, &wdata, 1);
}

static void mt6315_set_seq_off(struct spmi_device *mt6315_dev, u8 en_seq_off)
{
    mt6315_set_lock_reg(mt6315_dev, TMA_KEY, 0x9CEA);
    spmi_ext_register_writel(mt6315_dev,
                             MT6315_PMIC_RG_TOP2_RSV2, &en_seq_off, 1);
    mt6315_set_lock_reg(mt6315_dev, TMA_KEY, 0);
}

static status_t mt6315_shutdown_callback(void *data,
                                         platform_halt_action suggested_action,
                                         platform_halt_reason reason)
{
    struct spmi_device *mt6315_dev = (struct spmi_device *)data;

    if ((suggested_action == HALT_ACTION_SHUTDOWN) ||
        (suggested_action == HALT_ACTION_REBOOT && reason == HALT_REASON_POR))
        mt6315_set_seq_off(mt6315_dev, 1);

    return NO_ERROR;
}

static void mt6315_vdig18_hw_op_set(struct spmi_device *mt6315_dev, bool enable)
{
    mt6315_set_lock_reg(mt6315_dev, DIG_WPK_KEY, 0x6315);
    spmi_ext_register_writel_field(mt6315_dev,
                                   MT6315_PMIC_RG_LDO_VDIG18_HW_OP_EN, enable,
                                   0x1, MT6315_PMIC_RG_LDO_VDIG18_HW_OP_EN_SHIFT);
    mt6315_set_lock_reg(mt6315_dev, DIG_WPK_KEY, 0);
}

int mt6315_vbuck_lp_setting(int slvid, u8 buck_id, bool en, u8 cfg)
{
    u16 rg_offset;

    if (slvid < SPMI_SLAVE_0 || slvid >= SPMI_MAX_SLAVE_ID) {
        dprintf(CRITICAL, "invalid slave id %d\n", slvid);
        return ERR_INVALID_ARGS;
    }
    if (buck_id < 1 || buck_id > 4) {
        dprintf(CRITICAL, "invalid MT6315 buck id %d\n", buck_id);
        return ERR_INVALID_ARGS;
    }
    rg_offset = MT6315_VBUCK_RG_OFFSET * (buck_id - 1);
    spmi_ext_register_writel(mt6315_devs[slvid],
                             MT6315_PMIC_VBUCK1_OP_EN + rg_offset, (u8 *)&en, 1);
    spmi_ext_register_writel(mt6315_devs[slvid],
                             MT6315_PMIC_VBUCK1_OP_CFG + rg_offset, &cfg, 1);
    return NO_ERROR;
}

#ifdef PROJECT_TYPE_FPGA
__WEAK int mt6315_device_register(int mstid, int slvid)
{
    return ERR_NOT_SUPPORTED;
}
#else
int mt6315_device_register(int mstid, int slvid)
{
    int ret;

    if (slvid < SPMI_SLAVE_0 || slvid >= SPMI_MAX_SLAVE_ID) {
        dprintf(CRITICAL, "invalid slave id %d\n", slvid);
        return ERR_INVALID_ARGS;
    }
    if (mt6315_devs[slvid]) {
        dprintf(CRITICAL, "duplicated slave id %d\n", slvid);
        return ERR_INVALID_ARGS;
    }
    mt6315_devs[slvid] = get_spmi_device(mstid, slvid);
    if (!mt6315_devs[slvid])
        return ERR_INVALID_ARGS;

    mt6315_vdig18_hw_op_set(mt6315_devs[slvid], true);

    ret = register_platform_halt_callback("mt6315_shutdown_callback",
                                          &mt6315_shutdown_callback, mt6315_devs[slvid]);
    if (ret) {
        dprintf(CRITICAL, "slave id %d register shutdown callback failed, ret=%d\n",
                slvid, ret);
        return ret;
    }
    return NO_ERROR;
}
#endif
