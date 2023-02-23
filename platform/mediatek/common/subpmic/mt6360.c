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

#include <debug.h>
#include <errno.h>
#include <libfdt.h>
#include <mt_i2c.h>
#include <platform.h>
#include <platform/mt6360.h>
#include <platform_halt.h>
#include <stdlib.h>
#include <string.h>
#include <subpmic.h>
#include <trace.h>

#define LOCAL_TRACE 0

#define MT6360_PMU_SLAVEID  (0x34)
#define MT6360_PMIC_SLAVEID (0x1A)
#define MT6360_LDO_SLAVEID  (0x64)
#define MT6360_TCPC_SLAVEID (0x4E)
#define CRC8_TABLE_SIZE     (256)

#define MT6360_REG_PMIC_BUCK1_SEQOFFDLY (0x07)

static u8 crc8_table[CRC8_TABLE_SIZE];
static u8 pwr_off_seq[4] = {0};

static void crc8_populate_msb(u8 table[CRC8_TABLE_SIZE], u8 polynomial)
{
    int i, j;
    const u8 msbit = 0x80;
    u8 t = msbit;

    table[0] = 0;

    for (i = 1; i < CRC8_TABLE_SIZE; i *= 2) {
        t = (t << 1) ^ (t & msbit ? polynomial : 0);
        for (j = 0; j < i && ((i + j) < CRC8_TABLE_SIZE); j++)
            table[i+j] = table[j]^t;
    }
}

static u8 crc8(const u8 table[CRC8_TABLE_SIZE], u8 *pdata, size_t nbytes,
               u8 crc)
{
    while (nbytes-- > 0)
        crc = table[(crc ^ *pdata++) & 0xff];
    return crc;
}

/* ========================= */
/* I2C operations */
/* ========================= */
static int mt6360_write_byte(u8 chan, u8 slave, u8 cmd, u8 data)
{
    int ret = 0;
    u8 write_buf[2] = {cmd, data};

    ret = mtk_i2c_write(chan, slave, write_buf, 2, NULL);
    if (ret < 0)
        LTRACEF("%s: I2CW[0x%02X] = 0x%02X failed, code = %d\n",
                __func__, cmd, data, ret);
    else
        LTRACEF("%s: I2CW[0x%02X] = 0x%02X\n", __func__, cmd, data);
    return ret;
}

static int mt6360_read_byte(u8 chan, u8 slave, u8 cmd, u8 *data)
{
    int ret = 0;
    u8 ret_data = cmd;

    ret = mtk_i2c_write_read(chan, slave, &ret_data, 1, 1, NULL);
    if (ret < 0)
        LTRACEF("%s: I2CR[0x%02X] failed, code = %d\n", __func__, cmd, ret);
    else {
        LTRACEF("%s: I2CR[0x%02X] = 0x%02X\n", __func__, cmd, ret_data);
        *data = ret_data;
    }
    return ret;
}

static int mt6360_block_write(u8 chan, u8 slave, u8 cmd, u8 len, u8 *data)
{
    u8 *write_buf;
    int i, ret = 0;

    write_buf = malloc(len+1);
    if (!write_buf)
        return -ENOMEM;
    *write_buf = cmd;
    memcpy(write_buf + 1, data, len);
    ret = mtk_i2c_write(chan, slave, write_buf, len + 1, NULL);
    if (ret < 0)
        LTRACEF("%s: I2CW[0x%02X] failed, ret = %d\n",
                __func__, cmd, ret);
    else {
        LTRACEF("%s: I2CW[0x%02X] =", __func__, cmd);
        for (i = 0; i < len; i++)
            LTRACEF(" 0x%02X", data[i]);
        LTRACEF("\n");
    }
    free(write_buf);
    return ret;
}

static int mt6360_block_read(u8 chan, u8 slave, u8 cmd, u8 len, u8 *data)
{
    int i, ret = 0;

    data[0] = cmd;
    ret = mtk_i2c_write_read(chan, slave, data, 1, len, NULL);
    if (ret < 0)
        LTRACEF("%s: I2CR[0x%02X] failed, code = %d\n",
                __func__, cmd, ret);
    else {
        LTRACEF("%s: I2CR[0x%02X] =", __func__, cmd);
        for (i = 0; i < len; i++)
            LTRACEF(" 0x%02X", data[i]);
        LTRACEF("\n");
    }
    return ret;
}
static int mt6360_write_byte_with_crc(u8 chan, u8 slave,
                                      u8 addr, u8 len, u8 *data)
{
    int ret = 0;
    u8 chunk[8] = {0};

    if ((addr & 0xc0) != 0 || len > 4 || len <= 0) {
        LTRACEF("%s: not support addr [%x], len [%d]\n", __func__, addr, len);
        return -EINVAL;
    }
    chunk[0] = (slave & 0x7f) << 1;
    chunk[1] = (addr & 0x3f) | ((len - 1) << 6);
    memcpy(chunk + 2, data, len);
    chunk[2+len] = crc8(crc8_table, chunk, 2 + len, 0);
    ret = mtk_i2c_write(chan, slave, chunk+1, len+3, NULL);
    if (ret < 0)
        LTRACEF("%s: I2CW[0x%02X] = 0x%02X failed, ret = %d\n",
                __func__, addr, *data, ret);
    else
        TRACEF("%s: I2CW[0x%02X] = 0x%02X\n", __func__, addr, *data);
    return ret;
}

static int mt6360_read_byte_with_crc(u8 chan, u8 slave,
                                     u8 addr, u8 len, u8 *data)
{
    int ret = 0;
    u8 chunk[8] = {0};
    u8 temp[5] = {0};

    if ((addr & 0xc0) != 0 || len > 4 || len <= 0) {
        LTRACEF("%s: not support addr [%x], len [%d]\n", __func__, addr, len);
        return -EINVAL;
    }
    chunk[0] = ((slave & 0x7f) << 1) + 1;
    chunk[1] = (addr & 0x3f) | ((len - 1) << 6);
    temp[0] = chunk[1];
    ret = mtk_i2c_write_read(chan, slave, temp, 1, len+1, NULL);
    if (ret < 0) {
        LTRACEF("%s: I2CR[0x%02X] failed, ret = %d\n", __func__, addr, ret);
        return ret;
    }
    memcpy(chunk + 2, temp, len+1);
    chunk[7] = crc8(crc8_table, chunk, 2 + len, 0);
    if (chunk[2+len] != chunk[7]) {
        LTRACEF("%s: crc check fail\n", __func__);
        return -EINVAL;
    }
    LTRACEF("%s: I2CR[0x%02X] = 0x%02X\n", __func__, addr, chunk[2]);
    memcpy(data, chunk+2, len);
    return ret;
}

/* ========================= */
/* GLOBAL operations */
/* ========================= */
int subpmic_read_interface(enum subpmic_chan chan,
                           u8 addr, u8 *data, u8 mask, u8 shift)
{
    int ret = 0;
    u8 rdata = 0;

    switch (chan) {
    case SUBPMIC_CHAN_PMU:
        ret = mt6360_read_byte(MT6360_I2C_CHANNEL,
                               MT6360_PMU_SLAVEID, addr, &rdata);
        break;
    case SUBPMIC_CHAN_PMIC:
        ret = mt6360_read_byte_with_crc(MT6360_I2C_CHANNEL,
                                        MT6360_PMIC_SLAVEID, addr, 1, &rdata);
        break;
    case SUBPMIC_CHAN_LDO:
        ret = mt6360_read_byte_with_crc(MT6360_I2C_CHANNEL,
                                        MT6360_LDO_SLAVEID, addr, 1, &rdata);
        break;
    case SUBPMIC_CHAN_TYPEC:
        ret = mt6360_read_byte(MT6360_I2C_CHANNEL,
                               MT6360_TCPC_SLAVEID, addr, &rdata);
        break;
    default:
        return -ENODEV;
    }

    if (ret < 0) {
        LTRACEF("%s: fail, addr = 0x%x, ret = %d\n", __func__, addr, ret);
        return ret;
    }
    rdata &= mask;
    *data = (rdata >> shift);
    return 0;
}

int subpmic_config_interface(enum subpmic_chan chan,
                             u8 addr, u8 data, u8 mask, u8 shift)
{
    int ret = 0;
    u8 org = 0;

    ret = subpmic_read_interface(chan, addr, &org, 0xff, 0);
    if (ret < 0)
        return ret;

    org &= ~mask;
    org |= (data << shift);

    switch (chan) {
    case SUBPMIC_CHAN_PMU:
        return mt6360_write_byte(MT6360_I2C_CHANNEL,
                                 MT6360_PMU_SLAVEID, addr, org);
    case SUBPMIC_CHAN_PMIC:
        return mt6360_write_byte_with_crc(MT6360_I2C_CHANNEL,
                                          MT6360_PMIC_SLAVEID, addr, 1, &org);
    case SUBPMIC_CHAN_LDO:
        return mt6360_write_byte_with_crc(MT6360_I2C_CHANNEL,
                                          MT6360_LDO_SLAVEID, addr, 1, &org);
    case SUBPMIC_CHAN_TYPEC:
        return mt6360_write_byte(MT6360_I2C_CHANNEL,
                                 MT6360_TCPC_SLAVEID, addr, org);
    default:
        return -ENODEV;
    }
}

int subpmic_block_read(enum subpmic_chan chan, u8 addr, u8 len, u8 *dst)
{
    switch (chan) {
    case SUBPMIC_CHAN_PMU:
        return mt6360_block_read(MT6360_I2C_CHANNEL,
                                 MT6360_PMU_SLAVEID, addr, len, dst);
        break;
    case SUBPMIC_CHAN_PMIC:
        return mt6360_read_byte_with_crc(MT6360_I2C_CHANNEL,
                                         MT6360_PMIC_SLAVEID, addr, len, dst);
        break;
    case SUBPMIC_CHAN_LDO:
        return mt6360_read_byte_with_crc(MT6360_I2C_CHANNEL,
                                         MT6360_LDO_SLAVEID, addr, len, dst);
        break;
    case SUBPMIC_CHAN_TYPEC:
        return mt6360_block_read(MT6360_I2C_CHANNEL,
                                 MT6360_TCPC_SLAVEID, addr, len, dst);
        break;
    default:
        return -ENODEV;
    }
}

int subpmic_block_write(enum subpmic_chan chan, u8 addr, u8 len, u8 *src)
{
    switch (chan) {
    case SUBPMIC_CHAN_PMU:
        return mt6360_block_write(MT6360_I2C_CHANNEL,
                                  MT6360_PMU_SLAVEID, addr, len, src);
    case SUBPMIC_CHAN_PMIC:
        return mt6360_write_byte_with_crc(MT6360_I2C_CHANNEL,
                                          MT6360_PMIC_SLAVEID, addr, len, src);
    case SUBPMIC_CHAN_LDO:
        return mt6360_write_byte_with_crc(MT6360_I2C_CHANNEL,
                                          MT6360_LDO_SLAVEID, addr, len, src);
    case SUBPMIC_CHAN_TYPEC:
        return mt6360_block_write(MT6360_I2C_CHANNEL,
                                  MT6360_TCPC_SLAVEID, addr, len, src);
    default:
        return -ENODEV;
    }
}

static int subpmic_parse_dt(void *fdt)
{
    int i, offset = 0, len = 0;
    const void *data = NULL;

    LTRACEF("%s\n", __func__);

    if (fdt == NULL)
        return -EINVAL;

    offset = fdt_node_offset_by_compatible(fdt, -1, "mediatek,mt6360_pmic");
    if (offset < 0) {
        LTRACEF("%s: failed to get compatible\n", __func__);
        return offset;
    }

    data = fdt_getprop(fdt, offset, "pwr_off_seq", &len);
    if (len > 0 && data)
        memcpy(pwr_off_seq, data, len > 4 ? 4 : len);

    for (i = 0; i < 4; i++)
        LTRACEF("%s: pwr_off_seq[%d] = %d\n", __func__, i, pwr_off_seq[i]);

    return 0;
}

static status_t subpmic_enable_poweroff_seq(void *data,
                                            platform_halt_action suggested_action,
                                            platform_halt_reason reason)
{
    if ((suggested_action == HALT_ACTION_SHUTDOWN) ||
        (suggested_action == HALT_ACTION_REBOOT && reason == HALT_REASON_POR))
        return mt6360_write_byte_with_crc(MT6360_I2C_CHANNEL,
                                          MT6360_PMIC_SLAVEID,
                                          MT6360_REG_PMIC_BUCK1_SEQOFFDLY,
                                          4, pwr_off_seq);

    return NO_ERROR;
}

int subpmic_init(void *fdt)
{
    int ret;

    LTRACEF("%s\n", __func__);
    crc8_populate_msb(crc8_table, 0x7);
    subpmic_parse_dt(fdt);
    ret = register_platform_halt_callback("subpmic_enable_poweroff_seq",
                                          &subpmic_enable_poweroff_seq, NULL);
    if (ret < 0)
        LTRACEF("%s: failed to register shutdown callback(%d)\n", __func__, ret);
    return 0;
}
