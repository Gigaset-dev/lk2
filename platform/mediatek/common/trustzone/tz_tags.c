/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */
#include <assert.h>
#include <compiler.h>
#include <err.h>
#if CFG_GZ_REMAP_OFFSET_ENABLE
#include <gz_remap.h>
#endif
#include <init_mtk.h>
#include <kernel/vm.h>
#include <lib/kcmdline.h>
#include <libfdt.h>
#include <mblock.h>
#ifdef WITH_PLATFORM_MEDIATEK_COMMON_MMC
#include <mmc_rpmb.h>
#endif
#include <platform_mtk.h>
#include <platform/gpio.h>
#include <platform_mtk.h>
#include <reg.h>
#include <set_fdt.h>
#include <string.h>
#ifdef WITH_PLATFORM_MEDIATEK_COMMON_UFS
#include <partition_utils.h>
#include <ufs_interface.h>
#include <ufs_platform.h>
#endif

#include "ssmr.h"
#include "tz_mem.h"
#include "tz_tags.h"

#define MOD "TZ_TAGS"
#define SVP_FEATURES_DT_UNAME "mediatek,mtk_sec_video_path_support"

#define CMDLINE_BUF_SIZE 64

#define IS_ZERO(val) (val == 0)
#define ASSERT_NONNULL(val) \
    do { \
        if (val == NULL) { \
            dprintf(CRITICAL, "[%s] param is null!\n", MOD); \
            ASSERT(0); \
        } \
    } while (0)

#define ASSERT_LT(val1, val2) \
    do { \
        if (val1 > val2) { \
            dprintf(CRITICAL, "[%s] 0x%x exceeds 0x%x\n", MOD, \
                (u32)val1, (u32)val2); \
            ASSERT(0); \
        } \
    } while (0)

#define CMA_REMAIN_SIZE              0x2000000
#define MPU_ALLOC_START              0x240000000
#define MPU_ALLOC_SIZE               0x100000000

static u8 *tz_update_sec_mem_cfg(struct tz_tag *tag,
    u32 tz_sec_mem_start)
{
    tag = (struct tz_tag *) NEXT_TAG(tag);
    tag->hdr.size = sizeof(struct tz_sec_mem_cfg);
    tag->hdr.tag = TZ_TAG_SEC_MEM_CFG;
#if CFG_TEE_SECURE_MEM_PROTECTED
    tag->u.sec_mem_cfg.secmem_obfuscation = 1;
#else
    tag->u.sec_mem_cfg.secmem_obfuscation = 0;
#endif
    tag->u.sec_mem_cfg.shared_secmem = 1;
    tag->u.sec_mem_cfg.svp_mem_start = tz_sec_mem_start;

    return (u8 *)tag;
}

static void tz_clean_flash_info(struct tz_sec_flash_dev_cfg *cfg)
{
    cfg->rpmb_size = 0;
}

#ifdef WITH_PLATFORM_MEDIATEK_COMMON_MMC
static void tz_update_sdmmc_info(struct tz_sec_flash_dev_cfg *cfg)
{
    cfg->rpmb_size = mmc_rpmb_get_size();
    cfg->emmc_rel_wr_sec_c = mmc_rpmb_get_rel_wr_sec_c();
}
#endif

#ifdef WITH_PLATFORM_MEDIATEK_COMMON_UFS
static void tz_update_ufs_info(struct tz_sec_flash_dev_cfg *cfg)
{
    cfg->rpmb_size = (int)ufs_rpmb_get_lu_size();
    /*
     * We still use legacy eMMC name "rel_wr_sec" here.
     *
     * Since eMMC's REL_WR_SEC shall be 1 (512 bytes) which means
     * 2 RPMB frams and TEE will multiply this value by 2, we divide
     * it by 2 here for UFS.
     */
    cfg->emmc_rel_wr_sec_c = ufs_rpmb_get_rw_size() / 2;
    cfg->emmc_setkey_flag = ufs_rpmb_get_setkey_flag();
}
#endif

static u8 *tz_update_flash_dev_info(struct tz_tag *tag)
{
    tag = (struct tz_tag *) NEXT_TAG(tag);
    tag->hdr.size = sizeof(struct tz_sec_flash_dev_cfg);
    tag->hdr.tag = TZ_TAG_FLASH_DEV_INFO;

#ifdef WITH_PLATFORM_MEDIATEK_COMMON_UFS
    if (partition_get_bootdev_type() == BOOTDEV_UFS)
        tz_update_ufs_info(&tag->u.flash_dev_cfg);
#ifdef WITH_PLATFORM_MEDIATEK_COMMON_MMC
    else if (partition_get_bootdev_type() == BOOTDEV_SDMMC)
        tz_update_sdmmc_info(&tag->u.flash_dev_cfg);
#endif
    else
#endif
        tz_clean_flash_info(&tag->u.flash_dev_cfg);

    return (u8 *)tag;
}

#define UPDATE_RESERVE_MEM_TAG(mem_size, tag_magic) \
    pre_reserve_size = mem_size; \
    pre_reserve_start -= mem_size; \
    tag = (struct tz_tag *) NEXT_TAG(tag); \
    tag->hdr.size = sizeof(struct tz_sec_reserved_mem); \
    tag->hdr.tag = tag_magic; \
    tag->u.res_mem_cfg.size = pre_reserve_size; \
    tag->u.res_mem_cfg.start = pre_reserve_start

static u8 *tz_update_reserved_mem(struct tz_tag *tag,
    u32 tz_sec_mem_end)
{
    struct tz_tag *tz_tag_sec_mem_cfg = tag;
    u32 pre_reserve_start = tz_sec_mem_end;
    u32 pre_reserve_size = 0;

    UPDATE_RESERVE_MEM_TAG(SEC_MEM_RESERVED_NONE_SIZE,
        TZ_TAG_RESERVED_MEM_NONE_BUF);
    UPDATE_RESERVE_MEM_TAG(SEC_MEM_RESERVED_CMDQ_SIZE,
        TZ_TAG_RESERVED_MEM_CMDQ_BUF);
    UPDATE_RESERVE_MEM_TAG(SEC_MEM_RESERVED_M4U_SIZE,
        TZ_TAG_RESERVED_MEM_M4U_BUF);
    UPDATE_RESERVE_MEM_TAG(SEC_MEM_RESERVED_SPI_SIZE,
        TZ_TAG_RESERVED_MEM_SPI_BUF);
    UPDATE_RESERVE_MEM_TAG(SEC_MEM_RESERVED_I2C_SIZE,
        TZ_TAG_RESERVED_MEM_I2C_BUF);

    /* Sanity check for boundary */
    tz_tag_sec_mem_cfg->u.sec_mem_cfg.svp_mem_end = pre_reserve_start;
    ASSERT_LT(tz_tag_sec_mem_cfg->u.sec_mem_cfg.svp_mem_start,
        tz_tag_sec_mem_cfg->u.sec_mem_cfg.svp_mem_end);

    return (u8 *)tag;
}

static u8 *tz_update_sec_hardware_cfg(struct tz_tag *tag)
{
    tag = (struct tz_tag *) NEXT_TAG(tag);
    tag->hdr.size = sizeof(struct tz_sec_device_cfg);
    tag->hdr.tag = TZ_TAG_SEC_DEVICE_CFG;
#if CFG_GZ_REMAP_OFFSET_ENABLE
    tag->u.sec_hw_cfg.remap_offset = gz_remap_ddr_get_offset(GZ_REMAP_VMID_GZ);
#endif

    return (u8 *)tag;
}

static u8 *tz_update_gz_cfg(struct tz_tag *tag)
{
    const struct reserved_t *mem;

    tag = (struct tz_tag *) NEXT_TAG(tag);
    tag->hdr.size = sizeof(struct tz_sec_gz_cfg);
    tag->hdr.tag = TZ_TAG_SEC_GZ_CFG;

    mem = mblock_query_reserved_by_name("gz-tee-static-shm", 0);
    if (mem) {
        tag->u.gz_cfg.gz_tee_shared_mem_pa = mem->start;
        tag->u.gz_cfg.gz_tee_shared_mem_size = mem->size;
    }

    return (u8 *)tag;
}

#define DUMP_HEX(name) \
    dprintf(INFO, "[%s] %s: 0x%x\n", MOD, #name, tag->u.name)
#define DUMP_HEX_WITH_TAG(name) \
    dprintf(INFO, "[%s] %s(0x%x): 0x%x\n", MOD, #name, tag->hdr.tag, tag->u.name)
#define DUMP_HEX_64(name) \
    dprintf(INFO, "[%s] %s: 0x%llx\n", MOD, #name, tag->u.name)
#define DUMP_INT(name) \
    dprintf(INFO, "[%s] %s: %d\n", MOD, #name, tag->u.name)

static void tz_secmem_tags_dump(unsigned long param_addr)
{
    struct tz_tag *tag = (struct tz_tag *) param_addr;

    ASSERT_NONNULL(tag);
    if (!IS_TAG_FMT(tag)) {
        dprintf(CRITICAL, "[%s] secmem params are not tag format!\n", MOD);
        ASSERT(0);
    }

    do {
        switch (tag->hdr.tag) {
        case SEC_MEM_TAG_FMT_MAGIC:
            dprintf(CRITICAL, "[%s] secmem is tag format\n", MOD);
            break;
        case TZ_TAG_SEC_MEM_CFG:
            DUMP_HEX_64(sec_mem_cfg.svp_mem_start);
            DUMP_HEX_64(sec_mem_cfg.svp_mem_end);
            DUMP_INT(sec_mem_cfg.secmem_obfuscation);
            DUMP_INT(sec_mem_cfg.shared_secmem);
            break;
        case TZ_TAG_RESERVED_MEM_TPLAY_TABLE:
        case TZ_TAG_RESERVED_MEM_TPLAY_BUF:
        case TZ_TAG_RESERVED_MEM_M4U_BUF:
        case TZ_TAG_RESERVED_MEM_CMDQ_BUF:
        case TZ_TAG_RESERVED_MEM_SPI_BUF:
        case TZ_TAG_RESERVED_MEM_I2C_BUF:
        case TZ_TAG_RESERVED_MEM_NONE_BUF:
            DUMP_HEX_WITH_TAG(res_mem_cfg.start);
            DUMP_HEX_WITH_TAG(res_mem_cfg.size);
            break;
        case TZ_TAG_FLASH_DEV_INFO:
            DUMP_HEX(flash_dev_cfg.rpmb_size);
            DUMP_HEX(flash_dev_cfg.emmc_rel_wr_sec_c);
            DUMP_HEX(flash_dev_cfg.emmc_setkey_flag);
            break;
        case TZ_TAG_SEC_DEVICE_CFG:
            DUMP_HEX_64(sec_hw_cfg.remap_offset);
            break;
        case TZ_TAG_SEC_GZ_CFG:
            DUMP_HEX(gz_cfg.gz_tee_shared_mem_pa);
            DUMP_HEX(gz_cfg.gz_tee_shared_mem_size);
            break;
        default:
            dprintf(CRITICAL, "[%s] unknown tags: 0x%x\n", MOD, tag->hdr.tag);
            break;
        }
        tag = (struct tz_tag *)NEXT_TAG(tag);

        if (OUT_OF_TAG_SEARCH_RANGE(tag, param_addr))
            break;
    } while (!IS_TAG_END(tag));

    dprintf(CRITICAL, "[%s] remained tag len: 0x%lx\n", MOD,
        PTR_DIFF_BYTES(param_addr + TAG_STRUCT_MAX_SIZE, NEXT_TAG(tag)));
}

#if LK_DEBUGLEVEL > 1
static void tz_secmem_tags_auto_verify(unsigned long param_addr)
{
    ASSERT_NONNULL(find_tag(param_addr, SEC_MEM_TAG_FMT_MAGIC));
    ASSERT_NONNULL(find_tag(param_addr, TZ_TAG_SEC_MEM_CFG));
    ASSERT_NONNULL(find_tag(param_addr, TZ_TAG_RESERVED_MEM_M4U_BUF));
    ASSERT_NONNULL(find_tag(param_addr, TZ_TAG_RESERVED_MEM_CMDQ_BUF));
    ASSERT_NONNULL(find_tag(param_addr, TZ_TAG_RESERVED_MEM_SPI_BUF));
    ASSERT_NONNULL(find_tag(param_addr, TZ_TAG_RESERVED_MEM_I2C_BUF));
    ASSERT_NONNULL(find_tag(param_addr, TZ_TAG_FLASH_DEV_INFO));
    ASSERT_NONNULL(find_tag(param_addr, TZ_TAG_SEC_DEVICE_CFG));
    ASSERT_NONNULL(find_tag(param_addr, TZ_TAG_SEC_GZ_CFG));
    ASSERT_NONNULL(find_tag(param_addr, SEC_MEM_TAG_ENG_MAGIC));
}
#endif

#define INIT_SEC_MEM_TAG_STRUCT(start_addr) \
    memset((u8 *)start_addr, 0x0, TAG_STRUCT_MAX_SIZE); \
    tag = (struct tz_tag *) start_addr; \
    tag->hdr.tag = SEC_MEM_TAG_FMT_MAGIC; \
    tag->hdr.size = 0

#define UPDATE_SEC_MEM_END_MAGIC(tag) \
    tag = (struct tz_tag *) NEXT_TAG(tag); \
    tag->hdr.tag = SEC_MEM_TAG_ENG_MAGIC; \
    tag->hdr.size = 0

#define SECMEM_ALIGNMENT            (0x200000) /* 2MB for DEVMPU Protect */
#define SECMEM_LIMITATION           (0x80000000)
#define SECMEM_RESERVED_NAME        "tee-secmem"
#define TEE_PARAMETER_SIZE          (0x100000)
#define TEE_SECMEM_PARAMETER_OFFSET (0x200)

static u64 tee_static_reserved_secmem_addr;
static u32 tee_static_reserved_secmem_size;
#define SECMEM_RESERVED_SIZE        SEC_MEM_RESERVED_MAX_SIZE
static u32 secmem_static_reserved_secmem_addr;
static u32 secmem_static_reserved_secmem_size;
#define WA_MIRROR_OFFSET 0x400000000ULL

static void tz_secmem_static_reserve(void)
{
    u64 addr, mirror_addr_pa;
    u32 total_reserved_size = SECMEM_RESERVED_SIZE + TEE_PARAMETER_SIZE;

    addr = mblock_alloc(total_reserved_size, (u64)SECMEM_ALIGNMENT,
        (u64)SECMEM_LIMITATION, 0, 0, SECMEM_RESERVED_NAME);

    if (need_relocate()) {
        mirror_addr_pa = mblock_alloc(total_reserved_size, (u64)SECMEM_ALIGNMENT,
            MBLOCK_NO_LIMIT, (paddr_t)addr + WA_MIRROR_OFFSET,
            0, SECMEM_RESERVED_NAME);
        dprintf(INFO, "tee-secmem wa mirror address:0x%llx\n", mirror_addr_pa);
    }

    if (!addr) {
        dprintf(CRITICAL, "[%s] mblock reserve failed!\n", MOD);
        ASSERT(0);
    }

    /* secmem patarmer store in front of tee-secmem reserved memory */
    tee_static_reserved_secmem_addr = addr;
    tee_static_reserved_secmem_size = total_reserved_size;

    /* secmem reserved memory for m4u, cmdq, spi, and i2c */
    secmem_static_reserved_secmem_addr = addr + TEE_PARAMETER_SIZE;
    secmem_static_reserved_secmem_size = SECMEM_RESERVED_SIZE;
}

static void tz_secmem_tags_prepare(unsigned long param_addr,
    u32 tz_sec_mem_start, u32 tz_sec_mem_end)
{
    u8 *tag_start_ptr = (u8 *) param_addr;
    u8 *tag_end_ptr;
    struct tz_tag *tag = NULL;

    if (IS_ZERO(tz_sec_mem_start)) {
        dprintf(CRITICAL, "[%s] secmem tags are not prepared!\n", MOD);
        return;
    }
    if (IS_ZERO(tz_sec_mem_end - tz_sec_mem_start)) {
        dprintf(CRITICAL, "[%s] no space for secmem tags!\n", MOD);
        return;
    }

    dprintf(CRITICAL, "[%s] 0x%llx-0x%llx (res)\n", MOD, tee_static_reserved_secmem_addr,
                      tee_static_reserved_secmem_addr + tee_static_reserved_secmem_size);

    ASSERT_NONNULL(tag_start_ptr);

    dprintf(CRITICAL, "[%s] tag_start_addr: 0x%llx\n", MOD,
                      tee_static_reserved_secmem_addr + TEE_SECMEM_PARAMETER_OFFSET);
    INIT_SEC_MEM_TAG_STRUCT(tag_start_ptr);
    tag = (struct tz_tag *)tz_update_sec_mem_cfg(tag, tz_sec_mem_start);
    tag = (struct tz_tag *)tz_update_reserved_mem(tag, tz_sec_mem_end);
    tag = (struct tz_tag *)tz_update_flash_dev_info(tag);
    tag = (struct tz_tag *)tz_update_sec_hardware_cfg(tag);
    tag = (struct tz_tag *)tz_update_gz_cfg(tag);
    UPDATE_SEC_MEM_END_MAGIC(tag);

    /* Sanity check for boundary */
    tag_end_ptr = (u8 *) NEXT_TAG(tag);
    ASSERT_LT(tag_end_ptr, (tag_start_ptr + TAG_STRUCT_MAX_SIZE));

#if 0
/*
 * To Be Continued.
 * EMMC and UFS are not ready, so we mark below source code.
 */
#if LK_DEBUGLEVEL > 1
    /* Verify/dump tag contents */
    tz_secmem_tags_auto_verify(param_addr);
#endif
#endif
    tz_secmem_tags_dump(param_addr);
}

status_t platform_set_tz_tags(void)
{
    void *secmem_param_va  = NULL;
    status_t ret;

    tz_secmem_static_reserve();

    /* mmap the allocated memory */
    ret = vmm_alloc_physical(vmm_get_kernel_aspace(),
                             "vm-tztags-reserved",
                             tee_static_reserved_secmem_size,
                             &secmem_param_va,
                             PAGE_SIZE_SHIFT,
                             (paddr_t)tee_static_reserved_secmem_addr,
                             0,
                             ARCH_MMU_FLAG_CACHED);

    if (ret != NO_ERROR) {
        dprintf(CRITICAL, "vmm_alloc_physical failed, err: %d\n", ret);
        goto err_vmm_alloc;
    }

    tz_secmem_tags_prepare((unsigned long)secmem_param_va + TEE_SECMEM_PARAMETER_OFFSET,
                           secmem_static_reserved_secmem_addr,
                           secmem_static_reserved_secmem_addr + secmem_static_reserved_secmem_size);

    vmm_free_region(vmm_get_kernel_aspace(), (vaddr_t)secmem_param_va);

    return NO_ERROR;

err_vmm_alloc:

    return ret;
}

#ifndef MTK_PAC_BTI_ENABLE
#if LK_DEBUGLEVEL > 0
/*
 * Setting kernel command line of boot device
 */
void disable_pac(uint level)
{
    int ret = 0;
    char cmdline_buf[CMDLINE_BUF_SIZE];

    ret = snprintf(cmdline_buf, CMDLINE_BUF_SIZE,
        "arm64.nopauth");

    ret = kcmdline_append(cmdline_buf);
    if (ret != NO_ERROR) {
        dprintf(ALWAYS, "kcmdline append %s failed, rc=%d\n", cmdline_buf, ret);
        return;
    }
    dprintf(ALWAYS, "kcmdline append %s\n", cmdline_buf);
    return;
}
MT_LK_INIT_HOOK_BL33(init_pac, disable_pac, LK_INIT_LEVEL_PLATFORM);
#endif
#endif

static bool is_svp_enabled(void *fdt)
{
    int offset;

    offset = fdt_node_offset_by_compatible(fdt, -1, SVP_FEATURES_DT_UNAME);
    if (offset < 0)
        return false;

    return true;
}

int memory_ssmr_fixup(void *fdt)
{
    int offset, nodeoffset, len, ret, i, j, n;
    int prop_offset;
    const char *name;
    const void *val;
    u32 node_size[2];
    u32 node_alloc_ranges[4];
    u64 size = 0;
    u64 alloc_start, alloc_size;
    int nr_ssmr_features, nr_scheme;

    nr_ssmr_features = __MAX_NR_SSMR_FEATURES;
    nr_scheme = __MAX_NR_SCHEME;

    offset = fdt_path_offset(fdt, "/memory-ssmr-features");
    if (offset < 0) {
        dprintf(CRITICAL, "can't find memory-ssmr-features\n");
        goto exit;
    }
    for (prop_offset = fdt_first_property_offset(fdt, offset);
            prop_offset >= 0; prop_offset = fdt_next_property_offset(fdt, prop_offset)) {

        val = fdt_getprop_by_offset(fdt, prop_offset, &name, &len);
        if (val) {
            u32 *out;

            for (i = 0; i < nr_ssmr_features; i++) {
                if ((!strncmp(name, _ssmr_feats[i].dt_prop_name,
                              strlen(_ssmr_feats[i].dt_prop_name)))) {
                    for (j = 0; j < nr_scheme; j++) {
                        if ((_ssmr_feats[i].scheme_flag & _ssmrscheme[j].flags)) {
                            out = (u32 *)fdt_getprop(fdt, offset, name, &len);
                            if (out != NULL) {
                                if (!is_svp_enabled(fdt) &&
                                    (!strncmp(name,
                                              _ssmr_feats[SSMR_FEAT_SVP_REGION].dt_prop_name,
                                              strlen(name)) ||
                                     !strncmp(name,
                                              _ssmr_feats[SSMR_FEAT_WFD_REGION].dt_prop_name,
                                              strlen(name))))
                                    continue;

                                _ssmrscheme[j].usable_size += ((u64)fdt32_to_cpu(*out) << 32) +
                                                              fdt32_to_cpu(*(out + 1));
                            }
                        }
                    }
                    break;
                }
            }
        }
    }

    for (n = 0; n < nr_scheme; n++) {
        dprintf(CRITICAL, "%s, scenario: %s, scenario_size 0x%llx\n", __func__,
                _ssmrscheme[n].name, _ssmrscheme[n].usable_size);
        if (size < _ssmrscheme[n].usable_size)
            size = _ssmrscheme[n].usable_size;
    }

    offset = fdt_path_offset(fdt, "/reserved-memory");
    if (offset < 0) {
        dprintf(CRITICAL, "%s, get reserved-memory failed\n", __func__);
        goto exit;
    }
    nodeoffset = fdt_subnode_offset(fdt, offset, "ssmr-reserved-cma_memory");
    if (nodeoffset < 0) {
        dprintf(CRITICAL, "%s, get ssmr-reserved-cma_memory failed\n", __func__);
        goto exit;
    }
    size = size + CMA_REMAIN_SIZE;
    node_size[0] = (u32)cpu_to_fdt32(size >> 32);
    node_size[1] = (u32)cpu_to_fdt32(size);
    ret = fdt_setprop(fdt, nodeoffset, "size",  node_size,
                sizeof(u32) * 2);
    if (ret) {
        dprintf(CRITICAL, "%s, set ssmr-reserved-cma_memory size failed\n", __func__);
        goto exit;
    }

    if (need_relocate()) {
        alloc_start = MPU_ALLOC_START;
        alloc_size =  MPU_ALLOC_SIZE;
        node_alloc_ranges[0] = (u32)cpu_to_fdt32(alloc_start >> 32);
        node_alloc_ranges[1] = (u32)cpu_to_fdt32(alloc_start);
        node_alloc_ranges[2] = (u32)cpu_to_fdt32(alloc_size >> 32);
        node_alloc_ranges[3] = (u32)cpu_to_fdt32(alloc_size);
        ret = fdt_setprop(fdt, nodeoffset, "alloc-ranges",  node_alloc_ranges,
                    sizeof(u32) * 4);
        if (ret) {
            dprintf(CRITICAL, "%s, set ssmr-reserved-cma_memory alloc-ranges failed\n", __func__);
            goto exit;
        }
    } else {
        /* Delete alloc-ranges */
        fdt_delprop(fdt, nodeoffset, "alloc-ranges");
    }

exit:
    return 0;
}
SET_FDT_INIT_HOOK(memory_ssmr_fixup, memory_ssmr_fixup)

