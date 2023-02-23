/*
 * Copyright (c) 2020 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#include <assert.h>
#include <err.h>
#include <platform.h>
#include <platform/reg.h>
#include <spmi_common.h>
#include "spmi_sw.h"

/* SPMI Commands */
#define SPMI_CMD_EXT_WRITE              0x00
#define SPMI_CMD_RESET                  0x10
#define SPMI_CMD_SLEEP                  0x11
#define SPMI_CMD_SHUTDOWN               0x12
#define SPMI_CMD_WAKEUP                 0x13
#define SPMI_CMD_AUTHENTICATE           0x14
#define SPMI_CMD_MSTR_READ              0x15
#define SPMI_CMD_MSTR_WRITE             0x16
#define SPMI_CMD_TRANSFER_BUS_OWNERSHIP 0x1A
#define SPMI_CMD_DDB_MASTER_READ        0x1B
#define SPMI_CMD_DDB_SLAVE_READ         0x1C
#define SPMI_CMD_EXT_READ               0x20
#define SPMI_CMD_EXT_WRITEL             0x30
#define SPMI_CMD_EXT_READL              0x38
#define SPMI_CMD_WRITE                  0x40
#define SPMI_CMD_READ                   0x60
#define SPMI_CMD_ZERO_WRITE             0x80

static struct spmi_device *spmi_dev[SPMI_MAX_SLAVE_ID];

int spmi_register_zero_write(struct spmi_device *dev, u8 data)
{
    return dev->pmif_arb->write_cmd(SPMI_CMD_ZERO_WRITE,
                                    dev->slvid, 0, &data, 1);
}

int spmi_register_read(struct spmi_device *dev, u8 addr, u8 *buf)
{
    /* 5-bit register address */
    if (addr > 0x1F)
        return ERR_INVALID_ARGS;

    return dev->pmif_arb->read_cmd(SPMI_CMD_READ,
                                   dev->slvid, addr, buf, 1);
}

int spmi_register_write(struct spmi_device *dev, u8 addr, u8 data)
{
    /* 5-bit register address */
    if (addr > 0x1F)
        return ERR_INVALID_ARGS;

    return dev->pmif_arb->write_cmd(SPMI_CMD_WRITE,
                                    dev->slvid, addr, &data, 1);
}

int spmi_ext_register_read(struct spmi_device *dev, u8 addr, u8 *buf, u8 len)
{
    /* 8-bit register address, up to 16 bytes */
    if (len == 0 || len > 16)
        return ERR_INVALID_ARGS;

    return dev->pmif_arb->read_cmd(SPMI_CMD_EXT_READ,
                                   dev->slvid, addr, buf, len);
}

int spmi_ext_register_write(struct spmi_device *dev, u8 addr, const u8 *buf,
                            u8 len)
{
    /* 8-bit register address, up to 16 bytes */
    if (len == 0 || len > 16)
        return ERR_INVALID_ARGS;

    return dev->pmif_arb->write_cmd(SPMI_CMD_EXT_WRITE,
                                    dev->slvid, addr, buf, len);
}

int spmi_ext_register_readl(struct spmi_device *dev, u16 addr, u8 *buf, u8 len)
{
    /* 8-bit register address, up to 16 bytes */
    if (len == 0 || len > 16)
        return ERR_INVALID_ARGS;

    return dev->pmif_arb->read_cmd(SPMI_CMD_EXT_READL,
                                   dev->slvid, addr, buf, len);
}

int spmi_ext_register_writel(struct spmi_device *dev, u16 addr, const u8 *buf,
                             u8 len)
{
    /* 8-bit register address, up to 16 bytes */
    if (len == 0 || len > 16)
        return ERR_INVALID_ARGS;

    return dev->pmif_arb->write_cmd(SPMI_CMD_EXT_WRITEL,
                                    dev->slvid, addr, buf, len);
}

int spmi_ext_register_readl_field(struct spmi_device *dev, u16 addr, u8 *buf,
                                  u16 mask, u16 shift)
{
    int ret;
    u8 rdata = 0;

    ret = dev->pmif_arb->read_cmd(SPMI_CMD_EXT_READL,
                                  dev->slvid, addr, &rdata, 1);
    if (!ret)
        *buf = (rdata >> shift) & mask;

    return ret;
}

int spmi_ext_register_writel_field(struct spmi_device *dev, u16 addr,
                                   u8 data, u16 mask, u16 shift)
{
    int ret;
    u8 tmp = 0;

    ret = spmi_ext_register_readl(dev, addr, &tmp, 1);
    if (ret)
        return ret;

    tmp &= ~(mask << shift);
    tmp |= (data << shift);
    return dev->pmif_arb->write_cmd(SPMI_CMD_EXT_WRITEL,
                                    dev->slvid, addr, &tmp, 1);
}

struct spmi_device *get_spmi_device(int mstid, u8 slvid)
{
    if (slvid >= SPMI_MAX_SLAVE_ID) {
        SPMI_ERR("failed to get spmi_device with slave id %d\n", slvid);
        return NULL;
    }
    return spmi_dev[slvid];
}

int spmi_device_register(struct spmi_device *platform_spmi_dev, int num_devs)
{
#ifndef PROJECT_TYPE_FPGA
    int i;

    if (!platform_spmi_dev || num_devs < 0)
        return ERR_INVALID_ARGS;

    for (i = 0; i < num_devs; i++, platform_spmi_dev++) {
        if (platform_spmi_dev->slvid >= SPMI_MAX_SLAVE_ID) {
            SPMI_INFO("invalid slave id %d\n", platform_spmi_dev->slvid);
            continue;
        }
        if (!spmi_dev[platform_spmi_dev->slvid])
            spmi_dev[platform_spmi_dev->slvid] = platform_spmi_dev;
        else {
            SPMI_INFO("duplicated slave id %d\n", platform_spmi_dev->slvid);
            return ERR_INVALID_ARGS;
        }
    }
#endif
    return 0;
}
