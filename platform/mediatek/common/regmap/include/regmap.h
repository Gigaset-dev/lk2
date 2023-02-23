/*
 * copyright (c) 2021 mediatek inc.
 *
 * use of this source code is governed by a mit-style
 * license that can be found in the license file or at
 * https://opensource.org/licenses/mit
 */

#pragma once

#include <sys/types.h>

struct regmap_bus {
    int (*read)(void *context, u32 reg, u32 *val);
    int (*write)(void *context, u32 reg, u32 val);
    int (*bulk_read)(void *context, u32 reg, u8 *val, size_t len);
    int (*bulk_write)(void *context, u32 reg, const u8 *val, size_t len);
};

struct regmap_i2c {
    u8 bus;
    u8 addr;
};

struct regmap_config {
    const char *name;
};

struct regmap;

struct regmap *regmap_get_by_name(const char *name);
int regmap_init(const struct regmap_bus *bus, void *bus_context,
                const struct regmap_config *confg);
int regmap_init_i2c(struct regmap_i2c *i2c,
                    const struct regmap_config *config);
int regmap_write(struct regmap *regmap, u32 reg, u32 val);
int regmap_read(struct regmap *regmap, u32 reg, u32 *val);
int regmap_bulk_write(struct regmap *regmap, u32 reg, const u8 *val,
                      size_t len);
int regmap_bulk_read(struct regmap *regmap, u32 reg, u8 *val, size_t len);

static inline int regmap_update_bits(struct regmap *regmap, u32 reg,
                                     u32 mask, u32 val)
{
    int ret;
    u32 buf = 0;

    ret = regmap_read(regmap, reg, &buf);
    if (ret < 0)
        return ret;
    buf &= ~mask;
    buf |= (val & mask);
    return regmap_write(regmap, reg, buf);
}

static inline int regmap_set_bits(struct regmap *regmap, u32 reg, u32 mask)
{
    return regmap_update_bits(regmap, reg, mask, mask);
}

static inline int regmap_clr_bits(struct regmap *regmap, u32 reg, u32 mask)
{
    return regmap_update_bits(regmap, reg, mask, 0);
}
