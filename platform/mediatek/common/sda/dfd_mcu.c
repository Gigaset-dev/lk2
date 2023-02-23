/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */
#include <debug.h>
#include <err.h>
#include <errno.h>
#include <kernel/vm.h>
#include <libfdt.h>
#include <malloc.h>
#include <mblock.h>
#include "plat_sram_flag.h"
#include <platform/dbgtop.h>
#include <platform/dfd_mcu.h>
#include <platform/mboot_expdb.h>
#include <platform/plat_dbg_info.h>
#include <platform/sec_devinfo.h>
#include <reg.h>
#include "return_stack.h"
#include <smc.h>
#include <smc_id_table.h>
#include "utils.h"

#define dfd_mcu_smc_setup(buffer_addr, cachedump_en, tap_en, res) \
    __smc_conduit(MTK_SIP_BL_DFD_MCU_CONTROL, DFD_MCU_SMC_MAGIC_SETUP,\
                  buffer_addr, cachedump_en, tap_en, 0, 0, 0, &res)

#define dfd_mcu_smc_get_status(res) \
    __smc_conduit(MTK_SIP_BL_DFD_MCU_CONTROL, DFD_MCU_SMC_MAGIC_GET_STATUS, 0, 0, 0, 0, 0, 0, &res)

static unsigned int dfd_internal_dump_triggered;
uint32_t dfd_status = ~(DFD_START | DFD_FINISH);

struct dfd_drv drv;
struct decoded_lastpc *lastpc;
struct decoded_return_stack *return_stack_little, *return_stack_big;

void __WEAK plat_dfd_mcu_init(void)
{
    dprintf(CRITICAL, "%s is not implemented\n", __func__);
}

void __WEAK save_dfd_status(void)
{
    /* DO NOT print any message here, because this function will be called in EL3 */
}

const char *dfd_compatible[] = {
    "/soc/dfd_mcu",
    "/dfd_mcu",
};

static int get_dfd_mcu_setting_from_fdt(void *fdt)
{
    int i, len, off;
    unsigned int dfd_mcu_en;
    u32 *data;

    if (!fdt)
        return -ENODATA;

    len = countof(dfd_compatible);

    for (i = 0; i < len; i++) {
        off = fdt_path_offset(fdt, dfd_compatible[i]);
        if (off >= 0)
            break;
    }

    if (i >= len) {
        dprintf(CRITICAL, "%s: couldn't find the dfd_mcu node\n", __func__);
        return -ENODATA;
    }

    /* dfd enable */
    data = (u32 *)fdt_getprop(fdt, off, "enabled", &len);
    if (!data || !len) {
        dprintf(CRITICAL, "%s: couldn't find enabled property\n", __func__);
        return -ENODATA;
    } else {
        dfd_mcu_en = fdt32_to_cpu(data[0]);
        dprintf(INFO, "%s: enabled = %d\n", __func__, dfd_mcu_en);
    }

    if (!dfd_mcu_en) {
        dprintf(CRITICAL, "%s: DFD_mcu is not enabled\n", __func__);
        return 0;
    }

    /* HW version */
    data = (u32 *)fdt_getprop(fdt, off, "hw_version", &len);
    if (!data || !len) {
        dprintf(CRITICAL, "%s: couldn't find hw_version property\n", __func__);
        return -ENODATA;
    } else {
        drv.hw_version = fdt32_to_cpu(data[0]);
        dprintf(INFO, "%s: hw_version = %d\n", __func__, drv.hw_version);
    }

    /* SW version */
    data = (u32 *)fdt_getprop(fdt, off, "sw_version", &len);
    if (!data || !len) {
        dprintf(CRITICAL, "%s: couldn't find sw_version property\n", __func__);
        return -ENODATA;
    } else {
        drv.sw_version = fdt32_to_cpu(data[0]);
        dprintf(INFO, "%s: sw_version = %d\n", __func__, drv.sw_version);
    }

    /* dfd_timeout */
    data = (u32 *)fdt_getprop(fdt, off, "dfd_timeout", &len);
    if (!data || !len) {
        dprintf(CRITICAL, "%s: couldn't find dfd_timeout property\n", __func__);
        return -ENODATA;
    } else {
        drv.dfd_timeout = fdt32_to_cpu(data[0]);
        dprintf(INFO, "%s: dfd_timeout = 0x%lx\n", __func__, drv.dfd_timeout);
    }

    /* buf_addr_align */
    data = (u32 *)fdt_getprop(fdt, off, "buf_addr_align", &len);
    if (!data || !len) {
        dprintf(CRITICAL, "%s: couldn't find buf_addr_align property\n", __func__);
        drv.buf_addr_align = DFD_ALIGN;
        dprintf(CRITICAL, "%s: set buf_addr_align as default value (0x%lx)\n",
            __func__, drv.buf_addr_align);
    } else {
        drv.buf_addr_align = fdt32_to_cpu(data[0]);
        dprintf(INFO, "%s: buf_addr_align = 0x%lx\n", __func__, drv.buf_addr_align);
    }

    /* buf_addr_max */
    data = (u32 *)fdt_getprop(fdt, off, "buf_addr_max", &len);
    if (!data || !len) {
        dprintf(CRITICAL, "%s: couldn't find buf_addr_max property\n", __func__);
        drv.buf_addr_max = DFD_ADDR_MAX;
        dprintf(CRITICAL, "%s: set buf_addr_max as default value (0x%lx)\n",
            __func__, drv.buf_addr_max);
    } else {
        drv.buf_addr_max = fdt32_to_cpu(data[0]);
        dprintf(INFO, "%s: buf_addr_max = 0x%lx\n", __func__, drv.buf_addr_max);
    }

    /* dfd buffer_length */
    data = (u32 *)fdt_getprop(fdt, off, "buf_length", &len);
    if (!data || !len) {
        dprintf(CRITICAL, "%s: couldn't find buf_length property\n", __func__);
        return -ENODATA;
    } else {
        drv.buf_length = fdt32_to_cpu(data[0]);
        dprintf(INFO, "%s: buf_length = 0x%lx\n", __func__, drv.buf_length);
    }

    /* nr_max_core */
    data = (u32 *)fdt_getprop(fdt, off, "nr_max_core", &len);
    if (!data || !len) {
        dprintf(CRITICAL, "%s: couldn't find nr_max_core property\n", __func__);
        return -ENODATA;
    } else {
        drv.nr_max_core = fdt32_to_cpu(data[0]);
        dprintf(INFO, "%s: nr_max_core = %d\n", __func__, drv.nr_max_core);
    }

    /* nr_big_core */
    data = (u32 *)fdt_getprop(fdt, off, "nr_big_core", &len);
    if (!data || !len) {
        dprintf(CRITICAL, "%s: couldn't find nr_big_core property\n", __func__);
        return -ENODATA;
    } else {
        drv.nr_big_core = fdt32_to_cpu(data[0]);
        dprintf(INFO, "%s: nr_big_core = %d\n", __func__, drv.nr_big_core);
    }

    /* nr_rs_entry_little */
    data = (u32 *)fdt_getprop(fdt, off, "nr_rs_entry_little", &len);
    if (!data || !len) {
        dprintf(CRITICAL, "%s: couldn't find nr_rs_entry_little property\n", __func__);
        drv.nr_rs_entry_little = NUM_RETRUN_STACK_ENTRY_LITTLE_CORE;
        dprintf(CRITICAL, "%s: set nr_rs_entry_little as default value (0x%x)\n",
            __func__, drv.nr_rs_entry_little);
    } else {
        drv.nr_rs_entry_little = fdt32_to_cpu(data[0]);
        dprintf(INFO, "%s: nr_rs_entry_little = %d\n", __func__, drv.nr_rs_entry_little);
    }

    /* nr_rs_entry_big */
    data = (u32 *)fdt_getprop(fdt, off, "nr_rs_entry_big", &len);
    if (!data || !len) {
        dprintf(CRITICAL, "%s: couldn't find nr_rs_entry_big property\n", __func__);
        drv.nr_rs_entry_big = NUM_RETRUN_STACK_ENTRY_BIG_CORE;
        dprintf(CRITICAL, "%s: set nr_rs_entry_big as default value (0x%x)\n",
            __func__, drv.nr_rs_entry_big);
    } else {
        drv.nr_rs_entry_big = fdt32_to_cpu(data[0]);
        dprintf(INFO, "%s: nr_rs_entry_big = %d\n", __func__, drv.nr_rs_entry_big);
    }

    /* nr_header_row */
    data = (u32 *)fdt_getprop(fdt, off, "nr_header_row", &len);
    if (!data || !len) {
        dprintf(CRITICAL, "%s: couldn't find nr_header_row property\n", __func__);
        return -ENODATA;
    } else {
        drv.nr_header_row = fdt32_to_cpu(data[0]);
        dprintf(INFO, "%s: nr_header_row = %d\n", __func__, drv.nr_header_row);
    }

    /* check_pattern_offset */
    data = (u32 *)fdt_getprop(fdt, off, "check_pattern_offset", &len);
    if (!data || !len) {
        dprintf(CRITICAL, "%s: couldn't find check_pattern_offset property\n", __func__);
        drv.check_pattern_offset = 0;
        dprintf(CRITICAL, "%s: set check_pattern_offset as default value 0\n", __func__);
    } else {
        drv.check_pattern_offset = fdt32_to_cpu(data[0]);
        dprintf(INFO, "%s: check_pattern_offset = %d\n", __func__, drv.check_pattern_offset);
    }

    /* chip_id_offset */
    data = (u32 *)fdt_getprop(fdt, off, "chip_id_offset", &len);
    if (!data || !len) {
        dprintf(CRITICAL, "%s: couldn't find chip_id_offset property\n", __func__);
        return -ENODATA;
    } else {
        drv.chip_id_offset = fdt32_to_cpu(data[0]);
        dprintf(INFO, "%s: chip_id_offset = %d\n", __func__, drv.chip_id_offset);
    }

    /* dfd_disable_devinfo_index */
    data = (u32 *)fdt_getprop(fdt, off, "dfd_disable_efuse", &len);
    if (!data || !len) {
        dprintf(CRITICAL, "%s: couldn't find dfd_disable_efuse property\n", __func__);
        return -ENODATA;
    } else {
        drv.dfd_disable_devinfo_index = fdt32_to_cpu(data[0]);
        drv.dfd_disable_bit = fdt32_to_cpu(data[1]);
        dprintf(INFO, "%s: dfd_disable_efuse = <%d,%d>\n", __func__,
                drv.dfd_disable_devinfo_index, drv.dfd_disable_bit);
    }

    /* get dfd_cache settings */
    off = fdt_subnode_offset(fdt, off, "dfd_cache");
    if (!off) {
        dprintf(CRITICAL, "%s: couldn't find the dfd_cache subnode.\n", __func__);
        return -ENODATA;
    }

    /* dfd_cache enable */
    data = (u32 *)fdt_getprop(fdt, off, "enabled", &len);
    if (!data || !len) {
        dprintf(CRITICAL, "%s: couldn't find property cache enabled\n", __func__);
        return -ENODATA;
    } else {
        drv.cachedump_en = fdt32_to_cpu(data[0]);
        dprintf(INFO, "%s: cachedump_en = %d\n", __func__, drv.cachedump_en);
    }

    if (drv.cachedump_en) {
        /* dfd_timeout */
        data = (u32 *)fdt_getprop(fdt, off, "dfd_timeout", &len);
        if (!data || !len) {
            dprintf(CRITICAL, "%s: couldn't find property cache enabled\n", __func__);
            return -ENODATA;
        } else {
            drv.dfd_timeout = fdt32_to_cpu(data[0]);
            dprintf(INFO, "%s: dfd_timeout = 0x%lx\n", __func__, drv.dfd_timeout);
        }

        /* dfd_cache buffer_length */
        data = (u32 *)fdt_getprop(fdt, off, "buf_length", &len);
        if (!data || !len) {
            dprintf(CRITICAL, "%s: couldn't find buf_length property\n", __func__);
            return -ENODATA;
        } else {
            drv.buf_length = fdt32_to_cpu(data[0]);
            dprintf(INFO, "%s: buf_length = 0x%lx\n", __func__, drv.buf_length);
        }

        data = (u32 *)fdt_getprop(fdt, off, "tap_en", &len);
        if (!data || !len) {
            dprintf(CRITICAL, "%s: couldn't find tap_en property\n", __func__);
            return -ENODATA;
        } else {
            drv.tap_en = fdt32_to_cpu(data[0]);
            dprintf(INFO, "%s: tap_en = 0x%lx\n", __func__, drv.tap_en);
        }
    }
    drv.initialized = 1;

    if (drv.buf_length > DFD_MCU_DUMP_SIZE) {
        dprintf(CRITICAL, "%s: buf_length (0x%lx) is greater than DFD_DUMP_DUMP_SIZE (0x%x)\n",
            __func__, drv.buf_length, DFD_MCU_DUMP_SIZE);
        return -EINVAL;
    }

    return 1;
}

void dfd_mcu_init(void *fdt)
{
    int ret;
    struct PLAT_SRAM_FLAG *plat = NULL;
    struct __smccc_res res;

    dprintf(CRITICAL, "%s: In dfd init\n", __func__);

    plat = (struct PLAT_SRAM_FLAG *) get_dbg_info_base(PLAT_SRAM_FLAG_KEY);
    if (!plat) {
        dprintf(CRITICAL, "%s: [Error] plat == NULL\n", __func__);
        return;
    } else {
        plat->plat_magic = SRAM_FLAG_MAGIC;
        plat->plat_sram_flag1 = 0;
        plat->plat_sram_flag2 = 0;
    }

    ret = get_dfd_mcu_setting_from_fdt(fdt);
    if (ret <= 0)
        return;

    drv.buf_addr = mblock_alloc(drv.buf_length, drv.buf_addr_align, drv.buf_addr_max,
            0, 0, "platform-debug_dfdmcu_dump");

    dprintf(CRITICAL, "%s: buf_addr = 0x%llx\n", __func__, drv.buf_addr);

    if (drv.buf_addr > 0) {
        dprintf(CRITICAL, "%s: In dfd setup\n", __func__);

        ret = mtk_dbgtop_dfd_therm1_dis(1);
        if (ret < 0) {
            dprintf(CRITICAL, "%s: disable therm1 reboot fail: %d\n", __func__, ret);
            return;
        }

        ret = mtk_dbgtop_dfd_therm2_dis(0);
        if (ret < 0) {
            dprintf(CRITICAL, "%s: disable therm2 reboot fail: %d\n", __func__, ret);
            return;
        }

        ret = mtk_dbgtop_dfd_timeout(drv.dfd_timeout, 0);
        if (ret < 0) {
            dprintf(CRITICAL, "%s: Setup dfd_mcu timeout fail: %d\n", __func__, ret);
            return;
        }

        dfd_mcu_smc_setup(drv.buf_addr, drv.cachedump_en, drv.tap_en, res);

        if (res.a0) {
            dprintf(CRITICAL, "%s: call __smc_conuit fail:  0x%x, 0x%x\n", __func__,
            MTK_SIP_BL_DFD_MCU_CONTROL, DFD_MCU_SMC_MAGIC_SETUP);
            return;
        }

        ret = mtk_dbgtop_dfd_count_en(1);
        if (ret < 0) {
            dprintf(CRITICAL, "%s: dfd_mcu start count fail: %d\n", __func__, ret);
            return;
        }

        plat->plat_sram_flag2 = drv.buf_addr;
        plat->plat_sram_flag1 = DFD_VALID;
        dprintf(CRITICAL, "%s: plat_sram_flag2 = 0x%x, plat_sram_flag = 0x%x\n", __func__,
            plat->plat_sram_flag2, plat->plat_sram_flag1);

        dprintf(CRITICAL, "%s: Complete dfd mcu setup!\n", __func__);
    } else {
        dprintf(CRITICAL, "%s: Reserve DFD buffer failed\n", __func__);
    }
}

void dfd_mcu_init_aee(void *fdt)
{
    struct PLAT_SRAM_FLAG *plat = NULL;

    dprintf(CRITICAL, "%s: In dfd init\n", __func__);

    plat = (struct PLAT_SRAM_FLAG *) get_dbg_info_base(PLAT_SRAM_FLAG_KEY);
    if (!plat) {
        dprintf(CRITICAL, "%s: [Error] plat == NULL\n", __func__);
        return;
    } else {
        if (plat->plat_magic == SRAM_FLAG_MAGIC) {
            dprintf(CRITICAL, "%s: plat_sram_flag2 = 0x%x, plat_sram_flag = 0x%x\n",
                __func__, plat->plat_sram_flag2, plat->plat_sram_flag1);
        } else {
            dprintf(CRITICAL, "%s: SRAM flag magic number check fail! (0x%x)\n",
                __func__, plat->plat_magic);
            return;
        }
    }

    memcpy(drv.chip_id, &SUB_PLATFORM[2], 4); // SUB_PLATFORM = mtXXXX
    dprintf(INFO, "%s: Chip:%s\n", __func__, drv.chip_id);

    plat_dfd_mcu_init();
    get_dfd_mcu_setting_from_fdt(fdt);
}

static void dfd_decode_return_stack(const u64 *dfd_raw_data)
{
    unsigned int i, j;
    unsigned int nr_little_core, nr_big_core;
    unsigned int nr_little_entry, nr_big_entry;

    nr_little_core = drv.nr_max_core - drv.nr_big_core;
    nr_big_core = drv.nr_big_core;
    nr_little_entry = drv.nr_rs_entry_little;
    nr_big_entry = drv.nr_rs_entry_big;

    if (nr_little_core != 0) {
        return_stack_little = malloc(nr_little_core * sizeof(struct decoded_return_stack));
        if (!return_stack_little) {
            dprintf(CRITICAL, "%s, malloc failed for little core's return stack\n", __func__);
            return;
        }
    }

    if (nr_big_core != 0) {
        return_stack_big = malloc(nr_big_core * sizeof(struct decoded_return_stack));
        if (!return_stack_big) {
            dprintf(CRITICAL, "%s, malloc failed for big core's return stack\n", __func__);
            return;
        }
    }

    for (i = 0; i < nr_little_core; ++i) {
        return_stack_little[i].entry = malloc(nr_little_entry * sizeof(unsigned long long));
        if (!return_stack_little[i].entry) {
            dprintf(CRITICAL, "%s, malloc failed for return stack entry (little)\n", __func__);
            return;
        }

        return_stack_little[i].ptr = 0;
        for (j = 0; j < nr_little_entry; j++)
            return_stack_little[i].entry[j] = 0;
    }

    for (i = 0; i < nr_big_core; ++i) {
        return_stack_big[i].entry = malloc(nr_big_entry * sizeof(unsigned long long));
        if (!return_stack_big[i].entry) {
            dprintf(CRITICAL, "%s, malloc failed for return stack entry (big)\n", __func__);
            return;
        }

        return_stack_big[i].ptr = 0;
        for (j = 0; j < nr_big_entry; j++)
            return_stack_big[i].entry[j] = 0;
    }

    if (drv.plt_dfd_op.plt_dfd_return_stack_decode)
        drv.plt_dfd_op.plt_dfd_return_stack_decode(dfd_raw_data);
}

/*
 * fills return stack into the buffer, and returns the total length we wrote
 */
unsigned int dfd_get_decoded_return_stack(char *buf, unsigned int max_buf_size)
{
    unsigned int len = 0, i, j;
    unsigned int nr_little_core, nr_big_core;
    unsigned int nr_little_entry, nr_big_entry;

    if (!return_stack_little && !return_stack_big)
        return len;

    if (!drv.plt_dfd_op.plt_dfd_return_stack_decode) {
        if (return_stack_little != NULL)
            free(return_stack_little);
        if (return_stack_big != NULL)
            free(return_stack_big);
        return len;
    }

    len += snprintf(buf, max_buf_size, "DFD triggered\n\n");

    nr_little_core = drv.nr_max_core - drv.nr_big_core;
    nr_big_core = drv.nr_big_core;
    nr_little_entry = drv.nr_rs_entry_little;
    nr_big_entry = drv.nr_rs_entry_big;

    if (return_stack_little) {
        for (i = 0; i < nr_little_core; i++) {
            len += snprintf(buf + len, max_buf_size - len, "[CORE_%d]\n", i);
            len += snprintf(buf + len, max_buf_size - len, "Top pointer : 0x%llx\n",
                       return_stack_little[i].ptr);
            for (j = 0; j < nr_little_entry; j++)
                len += snprintf(buf + len, max_buf_size - len, "return stack%d = [<0x%016llx>]\n",
                       j, return_stack_little[i].entry[j]);
            len += snprintf(buf + len, max_buf_size - len, "\n");
        }
        free(return_stack_little);
    }

    if (return_stack_big) {
        for (i = 0; i < nr_big_core; i++) {
            len += snprintf(buf + len, max_buf_size - len, "[CORE_%d]\n", i + nr_little_core);
            len += snprintf(buf + len, max_buf_size - len, "Top pointer : 0x%llx\n",
                       return_stack_big[i].ptr);
            for (j = 0; j < nr_big_entry; j++)
                len += snprintf(buf + len, max_buf_size - len, "return stack%d = [<0x%016llx>]\n",
                       j, return_stack_big[i].entry[j]);
            len += snprintf(buf + len, max_buf_size - len, "\n");
        }
        free(return_stack_big);
    }

    return len;
}

static void dfd_internal_dump_decode_lastpc(const u64 *dfd_raw_data)
{
    unsigned int i;
    unsigned int nr_max_core, nr_little_core, nr_big_core;

    nr_max_core = drv.nr_max_core;
    nr_big_core = drv.nr_big_core;
    nr_little_core = nr_max_core - nr_big_core;

    lastpc = malloc(nr_max_core * sizeof(struct decoded_lastpc));
    if (!lastpc) {
        dprintf(CRITICAL, "%s, malloc failed for lastpc\n", __func__);
        return;
    }

    for (i = 0; i < nr_max_core; i++) {
        lastpc[i].power_state = 0x0;
        lastpc[i].pc = 0x0;
        lastpc[i].sp_64 = 0x0;
        lastpc[i].fp_64 = 0x0;
        lastpc[i].sp_32 = 0x0;
        lastpc[i].fp_32 = 0x0;
    }

    if (drv.plt_dfd_op.plt_dfd_lastpc_decode)
        drv.plt_dfd_op.plt_dfd_lastpc_decode(dfd_raw_data);
}

/*
 * fills lastpc into the buffer, and returns the total length we wrote
 */
unsigned int dfd_internal_dump_get_decoded_lastpc(char *buf, unsigned int max_buf_size)
{
    struct PLAT_SRAM_FLAG *plat = NULL;
    unsigned int plat_sram_flag1, plat_sram_flag2;
    unsigned int len = 0, i;
    int is_64bit_kernel = 1;

    plat = (struct PLAT_SRAM_FLAG *) get_dbg_info_base(PLAT_SRAM_FLAG_KEY);
    plat_sram_flag1 = (plat == NULL) ? 0 : plat->plat_sram_flag1;
    plat_sram_flag2 = (plat == NULL) ? 0 : plat->plat_sram_flag2;

    len += snprintf(buf, max_buf_size,
                    "DFD triggered\nPlease refer to dfd post-processing result for lastpc\n\n");
    len += snprintf(buf + len, max_buf_size - len,
                    "plat_sram_flag1 = 0x%x ([0]:dfd_valid=%x)\n",
                    plat_sram_flag1,
                    extract_n2mbits(plat_sram_flag1, 0, 0));
    len += snprintf(buf + len, max_buf_size - len,
                    "plat_sram_flag2 = 0x%x (base address)\n\n",
                    plat_sram_flag2);

    if (!lastpc)
        return len;

    if (!drv.plt_dfd_op.plt_dfd_lastpc_decode) {
        free(lastpc);
        return len;
    }

    if (is_64bit_kernel) {
        for (i = 0; i < drv.nr_max_core; ++i)
            len += snprintf(buf + len, max_buf_size - len,
                            "[LAST PC] CORE_%d PC = 0x%016llx, FP = 0x%016llx, SP = 0x%016llx\n",
                            i, lastpc[i].pc, lastpc[i].fp_64, lastpc[i].sp_64);
    } else {
        for (i = 0; i < drv.nr_max_core; ++i)
            len += snprintf(buf + len, max_buf_size - len,
                            "[LAST PC] CORE_%d PC = 0x%016llx, FP = 0x%08lx, SP = 0x%08lx\n",
                            i, lastpc[i].pc, lastpc[i].fp_32, lastpc[i].sp_32);
    }

    free(lastpc);

    return len;
}

unsigned int get_efuse_dfd_disabled(void)
{
    unsigned int reg_val;

    if (drv.dfd_disable_bit == -1 || drv.dfd_disable_devinfo_index == -1)
        return 0;

    reg_val = get_devinfo_with_index(drv.dfd_disable_devinfo_index);
    dprintf(CRITICAL, "%s: get_devinfo_with_index() = 0x%x\n", __func__, reg_val);

    return ((reg_val & (0x1 << drv.dfd_disable_bit)) >> drv.dfd_disable_bit);
}

static bool dfd_valid;
static bool check_dfd_valid(u32 *data)
{
    if (data != NULL) {
        /* skip first 32 bytes */
        data = ((unsigned long long)data + drv.check_pattern_offset);

        /*
         * 0x0 = 16'hAA55
         * 0x4 = 16'h0F0F
         * 0x8 = 32'h1683-0000
         */

        if ((((data[0] & 0xffff0000) >> 16) == 0xAA55)
               && (((data[1] & 0xffff0000) >> 16) == 0x0F0F)
               && (((data[2] & 0xffff0000) >> 16) == 0x1683))
                   dfd_valid = true;
    }

    return dfd_valid;
}

static unsigned int dfd_internal_dump_check_triggered_or_not(void)
{
    struct PLAT_SRAM_FLAG *plat = NULL;

    if (drv.hw_version >= DFD_V3_0) {

        plat = (struct PLAT_SRAM_FLAG *) get_dbg_info_base(PLAT_SRAM_FLAG_KEY);
        if (!plat) {
            dprintf(CRITICAL, "%s: get_dbg_info_base(PLAT_SRAM_FLAG_KEY) is NULL\n", __func__);
            return 0;
        }

        if (plat->plat_magic != SRAM_FLAG_MAGIC) {
            dprintf(CRITICAL, "%s: SRAM flag magic number check fail! (0x%x)\n",
                __func__, plat->plat_magic);
            return 0;
        }

        /* DFD triggers only if dfd_valid == 1 and efuse_dfd_disabled == 0 */
        if ((plat->plat_sram_flag1 & DFD_VALID) && (!get_efuse_dfd_disabled())) {
            dfd_internal_dump_triggered = 1;
        } else {
            dprintf(CRITICAL, "%s: plat_sram_flag1 = 0x%x\n", __func__, plat->plat_sram_flag1);
            dprintf(CRITICAL, "%s: get_efuse_dfd_disabled() = 0x%x\n", __func__,
                    get_efuse_dfd_disabled());
            dfd_internal_dump_triggered = 0;
        }
    } else
        dfd_internal_dump_triggered = 0;

    /* print dfd trigger status */
    dprintf(CRITICAL, "%s: dfd_status = 0x%x\n", __func__, dfd_status);
    if (dfd_status == (uint32_t)(~(DFD_START | DFD_FINISH))) {
        dprintf(CRITICAL, "%s: dfd_status is as initialized value,", __func__);
        dprintf(CRITICAL, "it should be save_dfd_status() is not implemented\n");
    } else if ((dfd_status & DFD_START) == 0)
        dfd_internal_dump_triggered = 0;

    dprintf(CRITICAL, "%s: dfd_internal_dump_triggered = 0x%x\n",
                       __func__, dfd_internal_dump_triggered);

    return dfd_internal_dump_triggered;
}

/*
 * DFD internal dump before reboot implies that mcusys registers are all corrupted
 */
unsigned int dfd_internal_dump_before_reboot(void)
{
    /* for platform that is not support DFD internal dump */
    if (drv.hw_version < DFD_V3_0)
        return 0;

    /*
     * check the magic pattern again,
     * pass NULL as the parameter to get the result directly
     */
    if (dfd_internal_dump_triggered)
        return check_dfd_valid(NULL);
    else
        return 0;
}

int dfd_mcu_get(void **data, int *len)
{
    unsigned int i;
    uint64_t paddr;
    vaddr_t vaddr;
    int status;
    struct PLAT_SRAM_FLAG *plat = NULL;

    if (len == NULL || data == NULL)
        return 0;

    *len = 0;
    *data = NULL;

    if (!drv.initialized || drv.hw_version < DFD_V3_0)
        return 0;

    if (dfd_internal_dump_check_triggered_or_not()) {

        plat = (struct PLAT_SRAM_FLAG *) get_dbg_info_base(PLAT_SRAM_FLAG_KEY);
        if (!plat) {
            dprintf(CRITICAL, "%s: [Error] Can't get plat_sram_flag2\n", __func__);
            return 0;
        }

        if (plat->plat_magic != SRAM_FLAG_MAGIC) {
            dprintf(CRITICAL, "%s: SRAM flag magic number check fail! (0x%x)\n",
                __func__, plat->plat_magic);
            return 0;
        }

        paddr = (uint64_t)plat->plat_sram_flag2;
        if ((paddr & (drv.buf_addr_align - 1)) != 0) {
            dprintf(CRITICAL, "%s: [Error] paddr (0x%llx)is abnormal\n", __func__, paddr);
            return 0;
        } else {
            dprintf(CRITICAL, "%s: paddr = (0x%llx)\n", __func__, paddr);
        }

        status = vmm_alloc_physical(vmm_get_kernel_aspace(),
                "dfd_temp_buffer", drv.buf_length, (void **)&vaddr, PAGE_SIZE_SHIFT,
                paddr, 0, ARCH_MMU_FLAG_UNCACHED);

        if (status != NO_ERROR) {
            dprintf(CRITICAL, "%s: vmm_alloc_physical fail! (status = 0x%x)\n", __func__, status);
            return 0;
        }

        /* check pa and va */
        if (paddr == 0 || vaddr == 0) {
            dprintf(CRITICAL, "%s: pa or va is invalid -> skip\npa = 0x%llx, va = 0x%lx",
                __func__, paddr, vaddr);
            return 0;
        }

        *data = (void *)vaddr;

        /* decode lastpc & return stack*/
        if (check_dfd_valid((u32 *)*data)) {
            if (drv.plt_dfd_op.plt_dfd_misc_decode)
                drv.plt_dfd_op.plt_dfd_misc_decode((u64 *)*data);
            else
                dprintf(CRITICAL, "%s, no plat_dfd_decode function\n", __func__);

            dfd_internal_dump_decode_lastpc((u64 *)*data);
            dfd_decode_return_stack((u64 *)*data);

            /* insert chip id */
            for (i = 0; i <= 7; ++i)
                *((char *)(*data) + drv.chip_id_offset + i) = drv.chip_id[i];

            *len = drv.buf_length;
        } else {
            dprintf(CRITICAL, "%s: check_dfd_valid = 0\n", __func__);
            /* still dump raw data of dfd */
            *len = drv.buf_length;
        }
    } else {
        /* not triggered */
        *len = 0;
        return 0;
    }

    return 1;
}

void dfd_mcu_put(void **data)
{
    vmm_free_region(vmm_get_kernel_aspace(), (vaddr_t)*data);
}

static void save_dfd_mcu_data(CALLBACK dev_write)
{
    char *buf = NULL;
    int ret;
    int len = 0;
    unsigned int datasize = 0;
    /* Save latch buffer */

    ret = dfd_mcu_get((void **)&buf, &len);
    if (ret && (buf != NULL)) {
        dprintf(CRITICAL, "%s: In dfd_mcu dump\n", __func__);
        if (len > 0)
            datasize = dev_write(vaddr_to_paddr(buf), len);
        dfd_mcu_put((void **)&buf);
    }
    dprintf(CRITICAL, "%s: dfd_mcu done (len = 0x%x)\n", __func__, len);

    return;
}

AEE_EXPDB_INIT_HOOK_SUFFIX(DFD, dfd, DFD_MCU_DUMP_SIZE, save_dfd_mcu_data);
