/*
 * copyright (c) 2021 mediatek inc.
 *
 * use of this source code is governed by a mit-style
 * license that can be found in the license file or at
 * https://opensource.org/licenses/mit
 */

#include <errno.h>
#include <lk/init.h>
#include <mt_i2c.h>
#include <regmap.h>
#include <string.h>
#include <subpmic.h>
#include <trace.h>

#define LOCAL_TRACE         0

#define MT6375_I2C_BUS      0x05
#define MT6375_SLAVEID_TCPC 0x4E
#define MT6375_SLAVEID_PMU  0x34
#define MT6375_SLAVEID_BM   0x1A
#define MT6375_SLAVEID_HK1  0x4A
#define MT6375_SLAVEID_HK2  0x64

enum {
    MT6375_SLAVE_TCPC,
    MT6375_SLAVE_PMU,
    MT6375_SLAVE_BM,
    MT6375_SLAVE_HK1,
    MT6375_SLAVE_HK2,
    MT6375_SLAVE_MAX,
};

static const u8 mt6375_slave_addr[MT6375_SLAVE_MAX] = {
    MT6375_SLAVEID_TCPC,
    MT6375_SLAVEID_PMU,
    MT6375_SLAVEID_BM,
    MT6375_SLAVEID_HK1,
    MT6375_SLAVEID_HK2,
};

static struct regmap_i2c *bank_to_i2c(u8 bank)
{
    static struct regmap_i2c i2c = { .bus = MT6375_I2C_BUS, };

    if (bank >= MT6375_SLAVE_MAX)
        return NULL;
    i2c.addr = mt6375_slave_addr[bank];
    return &i2c;
}

static int mt6375_reg_write(void *context, u32 reg, u32 val)
{
    int ret;
    u8 buf[2] = { reg, val };
    struct regmap_i2c *i2c;

    i2c = bank_to_i2c(reg >> 8);
    if (!i2c)
        return -EINVAL;
    ret = mtk_i2c_write(i2c->bus, i2c->addr, buf, 2, NULL);
    if (ret < 0)
        LTRACEF("%s I2CW[0x%02X]=0x%02X failed, ret=%d\n", __func__, reg, val,
                ret);
    else
        LTRACEF("%s I2CW[0x%02X]=0x%02X\n", __func__, reg, val);
    return ret;
}

static int mt6375_reg_read(void *context, u32 reg, u32 *val)
{
    int ret;
    u8 buf = reg;
    struct regmap_i2c *i2c;

    i2c = bank_to_i2c(reg >> 8);
    if (!i2c)
        return -EINVAL;
    ret = mtk_i2c_write_read(i2c->bus, i2c->addr, &buf, 1, 1, NULL);
    if (ret < 0)
        LTRACEF("%s I2CR[0x%02X] failed, ret=%d\n", __func__, reg, ret);
    else {
        LTRACEF("%s I2CR[0x%02X]=0x%02X\n", __func__, reg, buf);
        *val = buf;
    }
    return ret;
}

static int mt6375_reg_bulk_write(void *context, u32 reg, const u8 *val,
                size_t len)
{
    u8 buf[len + 1];
    struct regmap_i2c *i2c;

    i2c = bank_to_i2c(reg >> 8);
    if (!i2c)
        return -EINVAL;
    buf[0] = reg;
    memcpy(&buf[1], val, len);
    return mtk_i2c_write(i2c->bus, i2c->addr, buf, len + 1, NULL);
}

static int mt6375_reg_bulk_read(void *context, u32 reg, u8 *val, size_t len)
{
    struct regmap_i2c *i2c;

    i2c = bank_to_i2c(reg >> 8);
    if (!i2c)
        return -EINVAL;
    val[0] = reg;
    return mtk_i2c_write_read(i2c->bus, i2c->addr, val, 1, len, NULL);
}

static const struct regmap_bus mt6375_regmap_bus = {
    .read = mt6375_reg_read,
    .write = mt6375_reg_write,
    .bulk_read = mt6375_reg_bulk_read,
    .bulk_write = mt6375_reg_bulk_write,
};

static const struct regmap_config mt6375_regmap_config = {
    .name = "mt6375",
};

static void mt6375_init(uint level)
{
    int ret;

    LTRACEF("%s\n", __func__);
    ret = regmap_init(&mt6375_regmap_bus, NULL, &mt6375_regmap_config);
    if (ret < 0)
        LTRACEF("failed to init mt6375 regmap, ret = %d\n", ret);
}
LK_INIT_HOOK(mt6375, mt6375_init, LK_INIT_LEVEL_PLATFORM_EARLY - 1);
