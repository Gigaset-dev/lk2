/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

//#include <app/boot_mode.h>
#include <arch/ops.h>
#include <err.h>
#include <load_image.h>

#include "ccci_ld_md_args.h"
#include "ccci_ld_md_ass.h"
#include "ccci_ld_md_core.h"
#include "ccci_ld_md_log.h"
#include "ccci_ld_md_mem.h"
#include "ccci_ld_md_mpu.h"
#include "ccci_ld_md_sec.h"
#include "ccci_ld_md_tag_dt.h"
#include "ccci_lk_plat.h"
#include "ccci_lk_plat_md_pwr.h"

#define LOCAL_TRACE 0

/* ============================================================ */
/* Compatible codes for legacy                                  */
/* ============================================================ */
struct load_modem_info {
    unsigned long long base_addr;
    unsigned int resv_mem_size;
    char md_id;
    char errno;
    char md_type;
    char ver;
    int load_size;/*ROM + Check header*/
    int ro_rw_size;
};

static struct load_modem_info s_g_md_ld_status[4];

static void compatible_add_load_inf_tbl_base(unsigned long long pa)
{
    s_g_md_ld_status[0].base_addr = pa;
}

static void load_modem_image_compatible(void)
{
    int ld_num = 1;

    ALWAYS_LOG("+++++++\n");
    ccci_update_md_load_bitmap(0);

    if (ccci_insert_tag_inf("hdr_count", (char *)&ld_num, sizeof(unsigned int)) < 0)
        ALWAYS_LOG("insert hdr_count fail\n");

    /* update hdr tbl info */
    if (ccci_insert_tag_inf("hdr_tbl_inf", (char *)&s_g_md_ld_status[0],
                            sizeof(struct load_modem_info)) < 0)
        ALWAYS_LOG("insert hdr_tbl_inf fail\n");
    ALWAYS_LOG("--------\n");
}

void ccci_update_load_inf_tbl_compatible(int md_cap, int ld_size, int mem_size)
{
    s_g_md_ld_status[0].resv_mem_size = mem_size;
    s_g_md_ld_status[0].errno = 0;
    s_g_md_ld_status[0].md_id = 0;
    s_g_md_ld_status[0].ver = 0;
    s_g_md_ld_status[0].md_type = md_cap;
    s_g_md_ld_status[0].load_size = ld_size;
    s_g_md_ld_status[0].ro_rw_size = mem_size;
}

/* ============================================================ */
/* Load modem image core main codes                             */
/* ============================================================ */

static const struct download_info s_md_download_list_def[] = {
    /* img type | partition | image name | ref_flag */
    {main_img,   "md1img",   "md1rom",     0},
    {dsp_img,    "md1img",   "md1dsp",     0},
    {drdi_img,   "md1img",   "md1drdi",    0},
    {0,      NULL,       NULL,     0},
};

static const struct download_info *get_dl_tbl(void)
{
    const struct download_info *dl_tbl = NULL;

    dl_tbl = ccci_get_tbl_for_test();
    if (dl_tbl)
        return dl_tbl;

    dl_tbl = ccci_get_tbl_cust();
    if (dl_tbl)
        return dl_tbl;

    dl_tbl = NULL;
    dl_tbl = ccci_get_tbl_from_platform();
    if (dl_tbl)
        return dl_tbl;

    return s_md_download_list_def;
}

static int load_image_by_name(const char *partition, const char *name,
                  unsigned char *mem_addr, unsigned int max_size,
          unsigned int flags)
{
    int ret;

    ALWAYS_LOG("Load image: [%s] from partition: [%s], with:%p 0x%x\n", name, partition,
                mem_addr, max_size);

    /* Using this API to make sure AP CPU get data from DRAM, not cache */
    arch_invalidate_cache_range((addr_t)mem_addr, max_size);

    /* Load modem relate image by DMA or CPU */
    ret = load_partition(partition, name, mem_addr, max_size);
    if (ret < 0) {
        ALWAYS_LOG("Load image: [%s] fail from partition: [%s](mem_addr:%p, size:0x%x) ret:%d\n",
                    name, partition, mem_addr, max_size, ret);
        return ret;
    }

    /* There are three cases:
     * 1. If data load by DMA, that means DRAM data is valid, cache is invalid.
     *    We has called invalid cache earlier, so CPU will get data from DRAM.
     *    Then, cache and DRAM are sync.
     *
     * 2. If data load by CPU(Dirty bit set), that means DRAM data maybe invalid,
     *    and the valid data is in the cache. Using clean cache will flush the cache line
     *    with dirty bit to DRAM and clear dirty bit. Then cache and DRAM are sync.
     *
     * 3. If part of data are loaded by DMA(withoud dirty bit) and other part of data are loaded
     *    by CPU(Dirty bit set). Using clean cache will flush the cache line
     *    with dirty bit to DRAM and clear dirty bit. The memory region loaded by DMA does not
     *    be affected.
     */
    arch_clean_cache_range((addr_t)mem_addr, ret);

    return ret;
}

#define MAX_REGION_NUM_DEF       32
#define MAX_MD_RESV_DRAM_SIZE    (512 * 1024 * 1024)

static int get_max_region_num(void)
{
    int ret;

    ret = ccci_get_max_region_num_from_platform();
    if (ret < 0)
        return MAX_REGION_NUM_DEF;

    return ret;
}

static unsigned int get_max_reserved_size(void)
{
    unsigned int ret;

    ret = ccci_get_max_md_resv_size_from_platform();
    if (!ret)
        return MAX_MD_RESV_DRAM_SIZE;

    return ret;
}

static int add_md_memory_layout_to_tag_inf(struct md_mem_blk *mem_blk_table, int blk_num)
{
    int ret;

    if (!blk_num) {
        CCCI_TRACE_LOG("blk_num is 0\n");
        return -1;
    }

    ret = ccci_insert_tag_inf("md_mem_layout", (char *)mem_blk_table,
        sizeof(struct md_mem_blk) * blk_num);
    if (ret < 0) {
        ALWAYS_LOG("Add memory layout to tag fail\n");
        return -1;
    }

    ret = ccci_insert_tag_inf("md_bank0_base", (char *)&mem_blk_table[0].ap_phy,
        sizeof(unsigned long long));
    if (ret < 0)
        ALWAYS_LOG("Add md bank0 base tag fail\n");

    return 0;
}

struct freeable_memoy_t {
    unsigned long long base;
    unsigned int size;
};

static struct freeable_memoy_t s_free_tbl[MAX_REGION_NUM_DEF];
static unsigned int s_free_tbl_num;

static void update_freeable_memory_table(struct md_mem_blk *mem_blk_table, int blk_num)
{
    int i;

    for (i = 0; i < blk_num; i++) {
        if (s_free_tbl_num >= MAX_REGION_NUM_DEF) {
            ALWAYS_LOG("Freeable memory table full\n");
            return;
        }
        if (mem_blk_table[i].attr_flag & ATTR_PADDING_RETRIEVE) {
            s_free_tbl[s_free_tbl_num].base = mem_blk_table[i].ap_phy;
            s_free_tbl[s_free_tbl_num].size = mem_blk_table[i].size;
            s_free_tbl_num++;
            continue;
        }
        if ((!mem_blk_table[i].attr_flag) && (!mem_blk_table[i].inf_flag)) {
            s_free_tbl[s_free_tbl_num].base = mem_blk_table[i].ap_phy;
            s_free_tbl[s_free_tbl_num].size = mem_blk_table[i].size;
            s_free_tbl_num++;
        }
    }
}

static void free_not_used_memory(void)
{
    int i;

    for (i = 0; i < s_free_tbl_num; i++) {
        ALWAYS_LOG("Free 0x%llx with size:0x%x\n", s_free_tbl[i].base, s_free_tbl[i].size);
        ccci_retrieve_mem(s_free_tbl[i].base, s_free_tbl[i].size);
    }
}

static int load_main_image(const struct download_info *img, unsigned char *base, unsigned int size)
{
    int load_size;
    char *hdr_ptr = NULL;

    load_size = load_image_by_name(img->partition_name, img->image_name, base, size, img->ref_flag);
    if (load_size < 0)
        return -1;

    if (ccci_main_img_parsing(base, load_size, &hdr_ptr) < 0)
        return -1;

    ccci_md_chk_hdr_notify_platform((void *)hdr_ptr);

    return 0;
}

static int get_sub_img_blk_inf(unsigned int flag, struct md_mem_blk **inf, unsigned int size)
{
    struct md_mem_blk *blk;

    blk = ccci_get_1st_sub_mem_blk(0, flag);
    if (!blk) {
        ALWAYS_LOG("Not found relate memory blk for attr_flag:0x%x\n", flag);
        return 0;
    }
    if (!blk->offset) {
        ALWAYS_LOG("attr_flag:0x%x,Offset should not be zero\n", flag);
        return -1;
    }
    if (blk->offset >= size) {
        ALWAYS_LOG("attr_flag:0x%x,Get invalid offset:0x%x(0x%x)\n", flag, blk->offset, size);
        return -1;
    }

    if ((blk->offset + blk->size) > size) {
        ALWAYS_LOG("attr_flag:0x%x,Memory size not enough. Request:0x%x Left:0x%x\n",
                    flag, blk->size, size - blk->offset);
        return -1;
    }

    *inf = blk;

    return 1;
}

static int load_dsp_image(const struct download_info *img, unsigned char *base, unsigned int size)
{
    struct md_mem_blk *blk;
    unsigned char *load_addr = NULL;
    unsigned int mem_size;
    int ret, load_size;

    ret = get_sub_img_blk_inf(ATTR_DSP_RESERVED, &blk, size);
    if (ret < 0)
        return -1;
    if (ret == 0)
        return 0;

    mem_size = blk->size;
    load_addr = base + blk->offset;

    load_size = load_image_by_name(img->partition_name, img->image_name, load_addr, mem_size,
                                   img->ref_flag);
    if (load_size < 0) {
        if (img->ref_flag & LK_IMG_OPTINAL) {
            ALWAYS_LOG("skip optional sub image: %s\n", img->image_name);
            blk->attr_flag |= ATTR_OPTIONAL;
            return 0;
        }
        ccci_mem_blk_tmp_list_dump();
        return -1;
    }

    if (ccci_dsp_img_parsing(load_addr, load_size, blk->offset) < 0)
        return -1;

    return 0;
}

static int load_drdi_image(const struct download_info *img, unsigned char *base, unsigned int size)
{
    struct md_mem_blk *blk;
    unsigned char *load_addr = NULL;
    int ret;
    unsigned int mem_size;
    int drdi_version = 0;

    if (ccci_find_tag_inf("drdi_version", (char *)&drdi_version, sizeof(int)) != sizeof(int))
        CCCI_TRACE_LOG("[%s] error: 'drdi_version' not found!\n", __func__);
    else {
        ALWAYS_LOG("[%s]: drdi_version = %d\n", __func__, drdi_version);
        if (drdi_version == AMMS_V3)
            return 0;
    }

    ret = get_sub_img_blk_inf(ATTR_DRDI_RT_DES, &blk, size);
    if (ret < 0)
        return -1;
    if (ret == 0)
        return 0;

    if ((blk->offset == 0) || (blk->size == 0)) {
        ALWAYS_LOG("DRDI v1 not enable\n");
        return 0;
    }

    ret = get_sub_img_blk_inf(ATTR_DRDI_SRC, &blk, size);
    if (ret < 0)
        return -1;
    if (ret == 0)
        return 0;

    mem_size = blk->size;
    load_addr = base + blk->offset;

    if (load_image_by_name(img->partition_name, img->image_name, load_addr, mem_size,
                           img->ref_flag) < 0) {
        if (img->ref_flag & LK_IMG_OPTINAL) {
            ALWAYS_LOG("skip optional sub image: %s\n", img->image_name);
            return 0;
        }
        ccci_mem_blk_tmp_list_dump();
        return -1;
    }

    return 0;
}

static int load_ramdisk_image(const struct download_info *img, unsigned char *base,
                              unsigned int size)
{
    struct md_mem_blk *blk;
    unsigned char *load_addr = NULL;
    int ret;
    unsigned int mem_size;

    ret = get_sub_img_blk_inf(ATTR_RAMDISK, &blk, size);
    if (ret < 0)
        return -1;
    if (ret == 0)
        return 0;

    mem_size = blk->size;
    load_addr = base + blk->offset;

    if (load_image_by_name(img->partition_name, img->image_name, load_addr, mem_size,
                           img->ref_flag) < 0) {
        if (img->ref_flag & LK_IMG_OPTINAL) {
            ALWAYS_LOG("skip optional sub image: %s\n", img->image_name);
            return 0;
        }
        ccci_mem_blk_tmp_list_dump();
        return -1;
    }

    return 0;
}

static const struct download_info *find_main_img(const struct download_info *img)
{
    while (img->img_type != 0) {
        if (img->img_type == main_img)
            break;
        img++;
    }
    if (img->img_type != main_img) {
        ALWAYS_LOG("Main image info not found\n");
        return NULL;
    }

    return img;
}

static int load_sub_img(const struct download_info *img, unsigned char *base, unsigned int size)
{
    while (img->img_type != 0) {
        if (img->img_type == main_img) {
            img++;
            continue;
        }

        switch (img->img_type) {
        case dsp_img:
            if (load_dsp_image(img,  base, size) < 0)
                return -1;
            break;
        case drdi_img:
            if (load_drdi_image(img,  base, size) < 0)
                return -1;
            break;
        case ramdisk_img:
            if (load_ramdisk_image(img,  base, size) < 0)
                return -1;
            break;
        default:
            ALWAYS_LOG("Not support image type:%d\n", img->img_type);
            break;
        }
        img++;
    }

    return 0;
}

static int load_image_at_list(const struct download_info *dl_tbl, unsigned char *base,
                              unsigned int size)
{
    const struct download_info *main_img = NULL;

    /* Find main image and load */
    main_img = find_main_img(dl_tbl);
    if (!main_img)
        return -1;

    if (load_main_image(main_img, base, size) < 0)
        return -1;

    /* Load sub image */
    if (load_sub_img(dl_tbl, base, size) < 0)
        return -1;

    return 0;
}

static int load_soc_modem_image_only(void)
{
    const struct download_info *dl_tbl = NULL;
    unsigned char *md_mem_base = NULL;
    unsigned long long base_pa;
    unsigned int md_mem_size = 0;
    struct md_mem_blk *mem_blk_table = NULL;
    int max_blk_num;
    int blk_num;

    dl_tbl = get_dl_tbl();
    max_blk_num = get_max_region_num();

    md_mem_size = get_max_reserved_size();
    if (md_mem_size <= 0)
        return -1;

    if (ccci_mem_layout_init(max_blk_num) < 0)
        return -1;

    base_pa = ccci_resv_named_memory("md_mem_usage", md_mem_size, &md_mem_base);
    if (!base_pa)
        goto _Exit;

    ALWAYS_LOG("Memory base: %p(va)|0x%llx(pa) size:0x%x\n", md_mem_base,
        base_pa, md_mem_size);

    if (ccci_create_1st_mem_node(md_mem_size) < 0)
        goto _Exit;

    if (load_image_at_list(dl_tbl, md_mem_base, md_mem_size) < 0)
        goto _Exit;

    compatible_add_load_inf_tbl_base(base_pa);

    /* Make sure this function is called before MPU relate function!!! */
    blk_num = ccci_update_base_to_mem_tbl(base_pa);
    if (blk_num <= 0)
        return -1;

    mem_blk_table = ccci_mem_layout_tbl_get();
    if (!mem_blk_table)
        goto _Exit;

    if (ccci_update_md_mpu_cfg_tbl((void *)mem_blk_table, blk_num) < 0)
        goto _Exit;

    if (ccci_apply_md_bank0_1_hw_remap(base_pa) < 0)
        goto _Exit;

    add_md_memory_layout_to_tag_inf(mem_blk_table, blk_num);

    update_freeable_memory_table(mem_blk_table, blk_num);

    ccci_mem_layout_de_init();

    ccci_md_rorw_base_notify_platform(base_pa);

    return 0;

_Exit:
    /* Free allocated memory for MD */
    ccci_retrieve_mem(base_pa, md_mem_size);
    ccci_mem_layout_de_init();

    return -1;
}

static int load_soc_modem_image_full_function(void *fdt, unsigned char b_mode)
{
    if (load_soc_modem_image_only() < 0)
        return -1;

    if (cal_ap_md_smem_layout(fdt, b_mode) < 0)
        return -1;

    ccci_plat_apply_mpu_setting();
    free_not_used_memory();
    ccci_set_hw_locked();
    return 0;
}

/* ============================================================ */
/* Export to external functions                                 */
/* ============================================================ */
status_t platform_load_modem(void *fdt, unsigned char b_mode)
{
    int step = 0;

    ccci_get_boot_args_from_dt(fdt);

    step++;
    if (ccci_get_md_enabled() == 0) {
        ALWAYS_LOG("MD not EN, power off directly\n");
        goto __MD_OFF;
    }

    step++;
    if (ccci_lk_trace_init() < 0)
        goto __MD_OFF;

    step++;
    if (ccci_lk_tag_info_init() < 0)
        goto __MD_OFF;

    step++;
    if (ccci_is_load_md_img_only(b_mode)) {// load image only
        load_soc_modem_image_only();
        goto __MD_OFF;
    }

    step++;
    if (load_soc_modem_image_full_function(fdt, b_mode) < 0) {
        ccci_update_common_err_info(step);
        ccci_lk_add_trace_to_kernel();
        goto __MD_OFF;
    }

    load_modem_image_compatible();
    ccci_lk_add_trace_to_kernel();
    ccci_update_md_load_bitmap(0);

    ccci_post_process(fdt);
    ccci_platform_post_process();
    ccci_md_power_on();
    ccci_let_md_go();

    return NO_ERROR;

__MD_OFF:
    ccci_post_process(NULL);
    ccci_md_power_off();
    return NO_ERROR;
}
