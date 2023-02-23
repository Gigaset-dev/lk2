/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#include <compiler.h>
#include <libfdt.h>
#include <platform_mtk.h>
#include <sys/types.h>
#include <string.h>

#include "ccci_lk_plat.h"
#include "ccci_lk_plat_md_pwr.h"
#include "ccci_ld_md_core.h"
#include "ccci_ld_md_log.h"

#define LOCAL_TRACE 0

#define TAG_MEM_ALIGNMENT     (0x1000L)
#define TAG_MEM_LIMIT         (0xC0000000LL)
#define SMEM_LOW_BOUND        (0x50000000LL)

struct mem_attr_setting {
    const char *name;
    unsigned int mapping;
    unsigned long long low_bound;
    unsigned long long limit;
    unsigned long align;
};

/* cfg_default table use for no need_relocate device */
static struct mem_attr_setting md_mem_attr_cfg_default[] = {
    {"ap_md_c_smem",  0, SMEM_LOW_BOUND, SMEM_CACHE_LIMIT,    SMEM_CACHE_ALIGNMENT},
    {"ap_md_nc_smem", 0, SMEM_LOW_BOUND, SMEM_NONCACHE_LIMIT, SMEM_NONCACHE_ALIGNMENT},
    {"md_mem_usage",  0, 0,              RO_RW_MEM_LIMIT,     RO_RW_MEM_ALIGNMENT},
    {"ccci_tag_mem",  0, 0,              TAG_MEM_LIMIT,       TAG_MEM_ALIGNMENT},
    {"md1_sib_mem",   0, 0,              SIB_SMEM_LIMIT,      SIB_SMEM_ALIGNMENT},
};

/*
 * If DRAM > 16G, AP-MD share memory need to be in [0x250000000, 0x2a0000000], because
 * md has dependency with scp. Others can locate in 9G to 13G.
 */

/* cfg table use for need_relocate device */
static struct mem_attr_setting md_mem_attr_cfg[] = {
    {"ap_md_c_smem",  0, 0x250000000ULL, 0x2a0000000ULL, SMEM_CACHE_ALIGNMENT},
    {"ap_md_nc_smem", 0, 0x250000000ULL, 0x2a0000000ULL, SMEM_NONCACHE_ALIGNMENT},
    {"md_mem_usage",  0, 0x240000000ULL, 0x340000000ULL, RO_RW_MEM_ALIGNMENT},
    {"ccci_tag_mem",  0, 0x240000000ULL, 0x340000000ULL, TAG_MEM_ALIGNMENT},
    {"md1_sib_mem",   0, 0x240000000ULL, 0x340000000ULL, SIB_SMEM_ALIGNMENT},
};

struct arguments {
    const char *key;
    int         val;
};

static struct arguments load_args[] = {
    {"md_disable", 0},
};

static struct mem_attr_setting *get_md_mem_attr_table(unsigned int *table_size)
{
    if (!table_size)
        return NULL;

    if (need_relocate()) {
        *table_size = countof(md_mem_attr_cfg);
        return md_mem_attr_cfg;
    } else {
        *table_size = countof(md_mem_attr_cfg_default);
        return md_mem_attr_cfg_default;
    }
}

static int apply_args_setting(const char *arg)
{
    unsigned int i;

    for (i = 0; i < countof(load_args); i++) {
        if (strcmp(arg, load_args[i].key) == 0) {
            load_args[i].val = 1;
            return 1;
        }
    }
    return 0;
}

static int get_args_setting(const char *arg)
{
    unsigned int i;

    for (i = 0; i < countof(load_args); i++) {
        if (strcmp(arg, load_args[i].key) == 0)
            return load_args[i].val;
    }
    ALWAYS_LOG("args:%s not support\n", arg);
    return -1;
}

static void dump_args_setting(void)
{
    unsigned int i;

    ALWAYS_LOG("%s +++++++++++++++++++++\n", __func__);
    for (i = 0; i < countof(load_args); i++)
        ALWAYS_LOG("[%s]:[%d]\n", load_args[i].key, load_args[i].val);
    ALWAYS_LOG("%s ---------------------\n", __func__);
}


static int update_args_by_dt(const char *list, int len)
{
    const char *end;
    int str_num = 0;

    if ((!list) || (!len))
        return str_num;

    end = list + len;

    while (list < end) {
        len = strnlen(list, end - list) + 1;
        if (list + len > end)
            return str_num;

        /* Get one and process */
        apply_args_setting(list);
        list += len;
        str_num++;
    }

    return str_num;
}

static int update_limt_and_align_by_dt(const char *name, const char *str, int len)
{
    unsigned long long limit, align;
    fdt64_t *ptr = (fdt64_t *)str;
    unsigned int i, mem_attr_num;
    struct mem_attr_setting *md_mem_table = NULL;

    md_mem_table = get_md_mem_attr_table(&mem_attr_num);
    if (!md_mem_table) {
        ALWAYS_LOG("[ccci] %s line:%d: get_md_mem_attr_table fail\n", __func__, __LINE__);
        return -1;
    }

    if (len != 16)
        return -1;

    limit = fdt64_to_cpu(*ptr);
    ptr++;
    align = fdt64_to_cpu(*ptr);

    for (i = 0; i < mem_attr_num; i++) {
        if (strcmp(md_mem_table[i].name, name) == 0) {
            md_mem_table[i].limit = limit;
            md_mem_table[i].align = (unsigned long)align;
            return 0;
        }
    }

    return 0;
}

static void dump_limit_align_map_setting(void)
{
    unsigned int i, mem_attr_num;
    struct mem_attr_setting *md_mem_table = NULL;

    md_mem_table = get_md_mem_attr_table(&mem_attr_num);
    if (!md_mem_table) {
        ALWAYS_LOG("[ccci] %s line:%d: get_md_mem_attr_table fail\n", __func__, __LINE__);
        return;
    }

    ALWAYS_LOG("%s +++++++++++++++++++++\n", __func__);
    for (i = 0; i < mem_attr_num; i++)
        ALWAYS_LOG("[%s]:%llx-%llx:%lx:%d\n", md_mem_table[i].name,
            md_mem_table[i].low_bound, md_mem_table[i].limit,
            md_mem_table[i].align, md_mem_table[i].mapping);
    ALWAYS_LOG("%s ---------------------\n", __func__);
}

//===========================================================================
// Export API
//===========================================================================
void ccci_get_boot_args_from_dt(void *fdt)
{
    int node_offset = 0, prop_offset = 0;
    int len;
    const char *name = NULL;
    int cnt = 0;
    const void *val;
    if (!fdt) {
      ALWAYS_LOG("[ccci] fdt = NULL\n");
      return;
    }
    node_offset = fdt_path_offset(fdt, "/mddriver");
    if (node_offset < 0) {
        ALWAYS_LOG("[ccci]/mddriver not found, try /soc/mddriver\n");
        node_offset = fdt_path_offset(fdt, "/soc/mddriver");
        if (node_offset < 0) {
            ALWAYS_LOG("[ccci] /soc/mddriver not found!\n");
            return;
        }
    }

    for (prop_offset = fdt_first_property_offset(fdt, node_offset);
        prop_offset >= 0; prop_offset = fdt_next_property_offset(fdt, prop_offset)) {

        len = 0;
        val = fdt_getprop_by_offset(fdt, prop_offset, &name, &len);

        cnt++;
        if (val) {
            if (strncmp(name, "ref_args", strlen("ref_args")) == 0)
                update_args_by_dt(val, len);
            else
                update_limt_and_align_by_dt(name, val, len);
        } else
            ALWAYS_LOG("[ccci] %s line:%d: return NULL loop:%d\n", __func__, __LINE__, cnt);

    }
    dump_args_setting();
    dump_limit_align_map_setting();
}

int ccci_get_mem_limit_align_map(const char name[], unsigned long long *low_bound,
    unsigned long long *limit, unsigned long *align, unsigned int *mapping)
{
    unsigned int i = 0;
    unsigned int mem_attr_num = 0;
    struct mem_attr_setting *md_mem_table = NULL;

    md_mem_table = get_md_mem_attr_table(&mem_attr_num);
    if (!md_mem_table) {
        ALWAYS_LOG("[ccci] %s line:%d: get_md_mem_attr_table fail\n", __func__, __LINE__);
        return -1;
    }

    while (i < mem_attr_num) {
        if (!md_mem_table[i].name)
            break;

        if (strcmp(name, md_mem_table[i].name) == 0) {
            if (limit)
                *limit = md_mem_table[i].limit;
            if (align)
                *align = md_mem_table[i].align;
            if (mapping)
                *mapping = md_mem_table[i].mapping;
            if (low_bound)
                *low_bound = md_mem_table[i].low_bound;
            return 0;
        }
        i++;
    }

    return -1;
}

int ccci_get_md_enabled(void)
{
    // check from efuse
    if (ccci_get_md_dev_en() == 0)
        return 0;

    // check from device tree
    if (get_args_setting("md_disable") > 0)
        return 0;

    return 1;
}
