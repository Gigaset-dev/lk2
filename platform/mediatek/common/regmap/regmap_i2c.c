/*
 * copyright (c) 2021 mediatek inc.
 *
 * use of this source code is governed by a mit-style
 * license that can be found in the license file or at
 * https://opensource.org/licenses/mit
 */

#include <debug.h>
#include <errno.h>
#include <mt_i2c.h>
#include <regmap.h>
#include <string.h>

static int regmap_i2c_read(void *context, u32 reg, u32 *val)
{
    int ret;
    u8 buf = reg;
    struct regmap_i2c *i2c = context;

    ret = mtk_i2c_write_read(i2c->bus, i2c->addr, &buf, 1, 1, NULL);
    if (ret < 0)
        return ret;
    *val = buf;
    return 0;
}

static int regmap_i2c_write(void *context, u32 reg, u32 val)
{
    u8 buf[2] = { reg, val };
    struct regmap_i2c *i2c = context;

    return mtk_i2c_write(i2c->bus, i2c->addr, buf, 2, NULL);
}

static int regmap_i2c_bulk_write(void *context, u32 reg, const u8 *val,
                                 size_t len)
{
    u8 buf[len + 1];
    struct regmap_i2c *i2c = context;

    buf[0] = reg;
    memcpy(&buf[1], val, len);
    return mtk_i2c_write(i2c->bus, i2c->addr, buf, len + 1, NULL);
}

static int regmap_i2c_bulk_read(void *context, u32 reg, u8 *val, size_t len)
{
    struct regmap_i2c *i2c = context;

    val[0] = reg;
    return mtk_i2c_write_read(i2c->bus, i2c->addr, val, 1, len, NULL);
}

static const struct regmap_bus regmap_i2c_bus = {
    .read = regmap_i2c_read,
    .write = regmap_i2c_write,
    .bulk_read = regmap_i2c_bulk_read,
    .bulk_write = regmap_i2c_bulk_write,
};

int regmap_init_i2c(struct regmap_i2c *i2c,
                    const struct regmap_config *config)
{
    return regmap_init(&regmap_i2c_bus, (void *)i2c, config);
}
