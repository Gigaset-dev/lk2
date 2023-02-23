/*
 * Copyright (c) 2020 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#pragma once

#include <pmif_common.h>
#include <sys/types.h>

/* enum marco for cmd/channel */
enum spmi_master {
    SPMI_MASTER_0 = 0,
    SPMI_MASTER_1,
    SPMI_MASTER_P_1,
    SPMI_MASTER_MAX
};

enum spmi_slave {
    SPMI_SLAVE_0 = 0,
    SPMI_SLAVE_1,
    SPMI_SLAVE_2,
    SPMI_SLAVE_3,
    SPMI_SLAVE_4,
    SPMI_SLAVE_5,
    SPMI_SLAVE_6,
    SPMI_SLAVE_7,
    SPMI_SLAVE_8,
    SPMI_SLAVE_9,
    SPMI_SLAVE_10,
    SPMI_SLAVE_11,
    SPMI_SLAVE_12,
    SPMI_SLAVE_13,
    SPMI_SLAVE_14,
    SPMI_SLAVE_15,
    SPMI_MAX_SLAVE_ID
};

struct spmi_device {
    u8 slvid;
    int grpiden;
    int mstid;
    u16 hwcid_addr;
    u8 hwcid_val;
    u16 swcid_addr;
    u8 swcid_val;
    u16 wpk_key_addr;
    u8 wpk_key_val;
    u8 wpk_key_h_val;
    u16 tma_key_addr;
    u8 tma_key_val;
    u8 tma_key_h_val;
    struct pmif *pmif_arb;
    int (*read_check)(const struct spmi_device *dev);
};

/* external API */
int spmi_register_zero_write(struct spmi_device *dev, u8 data);
int spmi_register_read(struct spmi_device *dev, u8 addr, u8 *buf);
int spmi_register_write(struct spmi_device *dev, u8 addr, u8 data);
int spmi_ext_register_read(struct spmi_device *dev, u8 addr, u8 *buf, u8 len);
int spmi_ext_register_write(struct spmi_device *dev, u8 addr, const u8 *buf,
                            u8 len);
int spmi_ext_register_readl(struct spmi_device *dev, u16 addr, u8 *buf, u8 len);
int spmi_ext_register_writel(struct spmi_device *dev, u16 addr, const u8 *buf,
                             u8 len);
int spmi_ext_register_readl_field(struct spmi_device *dev, u16 addr, u8 *buf,
                                  u16 mask, u16 shift);
int spmi_ext_register_writel_field(struct spmi_device *dev, u16 addr,
                                   u8 data, u16 mask, u16 shift);
struct spmi_device *get_spmi_device(int mstid, u8 slvid);
int spmi_device_register(struct spmi_device *platform_spmi_dev, int num_devs);
