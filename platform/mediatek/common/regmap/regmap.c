/*
 * copyright (c) 2021 mediatek inc.
 *
 * use of this source code is governed by a mit-style
 * license that can be found in the license file or at
 * https://opensource.org/licenses/mit
 */

#include <debug.h>
#include <err.h>
#include <regmap.h>
#include <string.h>

#define REGMAP_SLOT_SIZE   10

struct regmap {
    const struct regmap_config *config;
    const struct regmap_bus *bus;
    void *bus_context;
};

static struct regmap regmap_tbl[REGMAP_SLOT_SIZE];

struct regmap *regmap_get_by_name(const char *name)
{
    int i;

    if (!name)
        return NULL;

    for (i = 0; i < REGMAP_SLOT_SIZE; i++) {
        if (regmap_tbl[i].config && !strcmp(regmap_tbl[i].config->name, name))
            return &regmap_tbl[i];
    }
    return NULL;
}

int regmap_init(const struct regmap_bus *bus, void *bus_context,
                const struct regmap_config *config)
{
    int i;
    struct regmap *regmap = NULL;

    if (!bus || !config)
        return ERR_INVALID_ARGS;
    for (i = 0; i < REGMAP_SLOT_SIZE; i++) {
        if (regmap_tbl[i].config &&
            !strcmp(regmap_tbl[i].config->name, config->name))
            return ERR_INVALID_ARGS;
        else if (!regmap)
            regmap = &regmap_tbl[i];
    }
    regmap->config = config;
    regmap->bus = bus;
    regmap->bus_context = bus_context;

    return 0;
}

static inline void *regmap_get_bus_context(struct regmap *regmap)
{
    return regmap->bus_context ? regmap->bus_context : regmap;
}

int regmap_write(struct regmap *regmap, u32 reg, u32 val)
{
    if (!regmap || !regmap->bus || !regmap->bus->write)
        return ERR_INVALID_ARGS;
    return regmap->bus->write(regmap_get_bus_context(regmap), reg, val);
}

int regmap_read(struct regmap *regmap, u32 reg, u32 *val)
{
    if (!regmap || !regmap->bus || !regmap->bus->read)
        return ERR_INVALID_ARGS;
    return regmap->bus->read(regmap_get_bus_context(regmap), reg, val);
}

int regmap_bulk_write(struct regmap *regmap, u32 reg, const u8 *val, size_t len)
{
    if (!regmap || !regmap->bus || !regmap->bus->bulk_write)
        return ERR_INVALID_ARGS;
    return regmap->bus->bulk_write(regmap_get_bus_context(regmap), reg, val,
                                   len);
}

int regmap_bulk_read(struct regmap *regmap, u32 reg, u8 *val, size_t len)
{
    if (!regmap || !regmap->bus || !regmap->bus->bulk_read)
        return ERR_INVALID_ARGS;
    return regmap->bus->bulk_read(regmap_get_bus_context(regmap), reg, val,
                                  len);
}
