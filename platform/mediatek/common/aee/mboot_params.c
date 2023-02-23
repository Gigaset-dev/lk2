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
#include <arch/mmu.h>
#include <debug.h>
#include <err.h>
#include <kernel/vm.h>
#include <libfdt.h>
#include <lib/bio.h>
#include <lib/cksum.h>
#include <lib/pl_boottags.h>
#include <lib/kcmdline.h>
#include <lib/zlib.h>
#include <lk/init.h>
#include <mblock.h>
#include <platform_mtk.h>
#include <platform/aee_common.h>
#include <platform/mboot_expdb.h>
#include <platform/mboot_params.h>
#include <set_fdt.h>
#include <stdio.h>
#include <stdlib.h>        // for ALIGN()
#include <string.h>

#define AEETAG "[MBOOT_PARAMS]"

#define MBOOT_PARAMS_SIG (0x43474244) /* DBGC */
#define BOOT_TAG_MBOOT_PARAMS_INFO   0x8861001C
/* ddr reserve state */
#define BOOT_TAG_DDR_RESERVE 0x8861000D

#define MBOOT_PARAMS_LK_SIZE 16
struct reboot_reason_lk {
    u32 last_func[MBOOT_PARAMS_LK_SIZE];
};

#define MBOOT_PARAMS_PL_SIZE 3
struct reboot_reason_pl {
    u32 wdt_status;
    u32 last_func[MBOOT_PARAMS_PL_SIZE];
};

/*
 * Must be sync-ed with struct last_reboot_reason defined at
 * [kernel-x/drivers/misc/mediatek/aee/mrdump/mboot_params.c]
 */
struct reboot_reason_kernel {
    uint32_t fiq_step;
    /* 0xaeedeadX: X=1 (HWT), X=2 (KE), X=3 (nested panic) */
    uint32_t exp_type;
    uint32_t padding[2];
    uint64_t kaslr_offset;
    uint64_t mboot_params_buffer_addr;
};
static struct reboot_reason_kernel *krbr;

/*
 * mboot params sram layout description:
 * |mboot params|plt_dbg_info|reserved|memory_info|log_store|mrdump(i.e. fulldump)|
 * mboot params dram layout description:
 * |mboot params dram memory|pstore|mrdump mini header|
 */
struct mboot_params_memory_info {
    u32 magic1;
    u32 sram_plat_dbg_info_addr;
    u32 sram_plat_dbg_info_size;
    u32 sram_log_store_addr;
    u32 sram_log_store_size;
    u32 mrdump_addr;
    u32 mrdump_size;
    u32 dram_addr;
    u32 dram_size;
    u32 pstore_addr;
    u32 pstore_size;
    u32 pstore_console_size;
    u32 pstore_pmsg_size;
    u32 mrdump_mini_header_addr;
    u32 mrdump_mini_header_size;
    u32 magic2;
};

struct boot_tag_mboot_params_info {
    u32 sram_addr;
    u32 sram_size;
    u32 def_type;
    u32 memory_info_offset;
};

struct boot_tag_ddr_reserve {
    u32 ddr_reserve_enable;
    u32 ddr_reserve_success;
    u32 ddr_reserve_ready;
};

#define SZLOG 40960
static char logbuf[SZLOG];

int zaee_log(const char *fmt, ...)
{
    va_list args;
    static int pos;

    va_start(args, fmt);
    if (pos < SZLOG - 1) /* vsnprintf return the length of original string */
        pos += vsnprintf(logbuf + pos, SZLOG - pos - 1, fmt, args);
    va_end(args);
    return 0;
};

void zaee_logbuf_addr_size(addr_t *addr, unsigned int *size)
{
    if (addr == NULL || size == NULL) {
        LOG("%s. get zaee logbuf info args invalid\n", AEETAG);
        return;
    }

    *addr = (addr_t)logbuf;
    *size = SZLOG;
}

static struct mboot_params_buffer *mboot_params;
static struct mboot_params_memory_info *memory_info;
static uint32_t mboot_params_size;
static struct boot_tag_mboot_params_info mboot_params_info;
static struct boot_tag_ddr_reserve ddr_reserve_state;

static u32 mboot_params_get_dram_addr(void)
{
    const struct reserved_t *reserved = NULL;

    reserved = mblock_query_reserved_by_name("aee_debug_kinfo", 0);
    if (!reserved) {
        panic("%s: fail to get dram addr of mboot params\n", AEETAG);
        return 0;
    }

    return reserved->start + RAM_CONSOLE_DRAM_OFF;
}

static void mboot_params_ptr_init(void)
{
    u32 sram_addr, dram_addr;
    u32 sram_size;
    u32 def_type;
    u32 memory_info_offset;
    void *mp_va;
    status_t err;

    sram_addr = mboot_params_info.sram_addr;
    sram_size = mboot_params_info.sram_size;
    def_type = mboot_params_info.def_type;
    memory_info_offset = mboot_params_info.memory_info_offset;
    LOG("%s. atag(PL2LK): 0x%x, 0x%x, 0x%x, 0x%x\n",
            AEETAG, sram_addr, sram_size, def_type, memory_info_offset);

    mp_va = paddr_to_kvaddr(sram_addr);

    if (memory_info->magic1 != MEM_MAGIC1 || memory_info->magic2 != MEM_MAGIC2)
        panic("%s: illegal magic number:[0x%x, 0x%x]",
                AEETAG, memory_info->magic1, memory_info->magic2);

    LOG("%s. boot_arg(PL2LK): sram addr va(%p)\n", AEETAG, mp_va);

    if (def_type == MBOOT_PARAMS_DEF_SRAM) {
        mboot_params = (struct mboot_params_buffer *)mp_va;
        mboot_params_size = sram_size;
    } else if (def_type == MBOOT_PARAMS_DEF_DRAM) {
        dram_addr = mboot_params_get_dram_addr();
        mboot_params_size = RAM_CONSOLE_SIZE;
        err = vmm_alloc_physical(vmm_get_kernel_aspace(),
                "dmboot_params", mboot_params_size,
                &mp_va, 0,
                dram_addr, 0, ARCH_MMU_FLAG_CACHED);
        if (err != NO_ERROR)
            panic("mboot_params vmm_alloc_physical dram failed\n");
        /* Don't unmap it, mboot_params depend on it */
        mboot_params = (struct mboot_params_buffer *)mp_va;
        LOG("%s. dram addr va(%p) pa(0x%x)\n", AEETAG, mp_va, dram_addr);
    } else {
        panic("unknown def type");
    }

    if (mboot_params->sig != MBOOT_PARAMS_SIG)
        panic("%s. %s(%p) sig %x mismatch\n",
                AEETAG, (def_type == MBOOT_PARAMS_DEF_SRAM) ? "sram" : "dram",
                (void *)mboot_params, mboot_params->sig);
}

#define RE_BOOT_BY_WDT_SW 2
#define RE_BOOT_NORMAL_BOOT 0

/*we can find the definition from preloader ,
 *this value should  sync with preloader incase issue happened
 */
#define RE_BOOT_BY_EINT 256
/*we can find the definition from preloader ,
 *this value should  sync with preloader incase issue happened
 */
#define RE_BOOT_BY_SYSRST 512

#define RE_BOOT_FULL_PMIC 0x800

#define U_VAR(type) rc_##type
static unsigned int U_VAR(wdt_status), U_VAR(fiq_step), U_VAR(exp_type);
static bool U_VAR(set_flag);

bool mboot_params_reboot_by_mrdump_key(void)
{
    unsigned int wdt_status;

    wdt_status = ((struct reboot_reason_pl *)
            ((void *)mboot_params + mboot_params->off_pl))->wdt_status;
    return ((wdt_status & RE_BOOT_BY_EINT) || (wdt_status & RE_BOOT_BY_SYSRST))?true:false;
}

static bool mboot_params_reboot_by_cold_reset(void)
{
    unsigned int wdt_status;

    wdt_status = ((struct reboot_reason_pl *)
            ((void *)mboot_params + mboot_params->off_pl))->wdt_status;
    return (wdt_status & RE_BOOT_FULL_PMIC) ? true : false;
}

bool mboot_params_get_wdt_status(unsigned int *wdt_status)
{
    if (wdt_status && U_VAR(set_flag)) {
        *wdt_status = U_VAR(wdt_status);
        return true;
    }
    return false;
}

bool mboot_params_get_fiq_step(unsigned int *fiq_step)
{
    if (fiq_step && U_VAR(set_flag)) {
        *fiq_step = U_VAR(fiq_step);
        return true;
    }
    return false;
}

bool mboot_params_get_exp_type(unsigned int *exp_type)
{
    if (exp_type && U_VAR(set_flag)) {
        *exp_type = U_VAR(exp_type);
        return true;
    }
    return false;
}

bool mboot_params_set_exp_type(unsigned int exp_type)
{
    bool ret = false;
    struct reboot_reason_kernel *rrk;

    if (mboot_params && mboot_params->off_linux &&
        (mboot_params->off_linux == (mboot_params->off_llk + ALIGN(mboot_params->sz_lk, 64))) &&
        (mboot_params->off_pl == sizeof(struct mboot_params_buffer))) {
        rrk = (struct reboot_reason_kernel *)((void *)mboot_params + mboot_params->off_linux);
        if ((rrk->exp_type ^ MBOOT_PARAMS_EXP_TYPE_MAGIC) > 0 &&
                (rrk->exp_type ^ MBOOT_PARAMS_EXP_TYPE_MAGIC) < 16) {
            LOG("%s. set exp type failed: it had been set as %d before\n",
                AEETAG, rrk->exp_type ^ MBOOT_PARAMS_EXP_TYPE_MAGIC);
            return ret;
        }
        if (exp_type < 16) {
            LOG("%s. set exp type %d\n", AEETAG, exp_type);
            exp_type = exp_type ^ MBOOT_PARAMS_EXP_TYPE_MAGIC;
            rrk->exp_type = exp_type;
            ret = true;
        } else {
            LOG("%s. set exp type failed: exp type:%d\n", AEETAG, exp_type);
        }
    } else {
        LOG("%s. mboot params not ready, set exp type failed: exp type:%d\n", AEETAG, exp_type);
    }
    return ret;
}

#define AEE_KASLR_DIS 0xd15ab1e
#define AEE_KASLR_EN  0xecab1e
bool mboot_params_get_kaslr_offset(uint64_t *kaslr_offset)
{
    if (!kaslr_offset)
        return false;

    if (krbr && U_VAR(set_flag)) {
        if (krbr->kaslr_offset == AEE_KASLR_EN) {
            *kaslr_offset = 0;
            return false;
        }

        if (krbr->kaslr_offset == AEE_KASLR_DIS)
            *kaslr_offset = 0;
        else
            *kaslr_offset = krbr->kaslr_offset;

        return true;
    }
    return false;
}

static bool mboot_params_reason_update(void)
{
    unsigned int fiq_step, wdt_status, exp_type;

    if (mboot_params && mboot_params->off_linux &&
        (mboot_params->off_linux == (mboot_params->off_llk + ALIGN(mboot_params->sz_lk, 64))) &&
        (mboot_params->off_pl == sizeof(struct mboot_params_buffer))) {
        wdt_status = ((struct reboot_reason_pl *)
                ((void *)mboot_params + mboot_params->off_pl))->wdt_status;
        krbr = (struct reboot_reason_kernel *)((void *)mboot_params + mboot_params->off_linux);
        fiq_step = krbr->fiq_step;
        exp_type = krbr->exp_type;
        LOG("%s. wdt_status 0x%x, fiq_step 0x%x, exp_type 0x%x\n",
                AEETAG, wdt_status, fiq_step, MBOOT_PARAMS_EXP_TYPE_DEC(exp_type));
        if (fiq_step != 0 && (exp_type ^ MBOOT_PARAMS_EXP_TYPE_MAGIC) >= 16) {
            fiq_step = 0;
            krbr->fiq_step = fiq_step;
        }
        U_VAR(wdt_status) = wdt_status;
        U_VAR(fiq_step) = fiq_step;
        U_VAR(exp_type) = MBOOT_PARAMS_EXP_TYPE_DEC(exp_type);
        U_VAR(set_flag) = true;
        LOG("%s. set reboot reason info done\n", AEETAG);
        if ((fiq_step == 0 && (wdt_status == RE_BOOT_BY_WDT_SW || wdt_status == RE_BOOT_FULL_PMIC))
                || (wdt_status == RE_BOOT_NORMAL_BOOT))
            return false;
        else
            return true;
    } else {
        if (mboot_params) {
            LOG("%s. FAILED: off_linux:0x%x, off_llk:0x%x, off_pl:0x%x\n",
                    AEETAG, mboot_params->off_linux, mboot_params->off_llk,
                    mboot_params->off_pl);
        } else {
            LOG("%s. mboot params buffer NULL\n", AEETAG);
        }
    }
    return false;
}

/*
 * To know if current boot is abnormal boot.
 */
bool mboot_params_current_abnormal_boot(void)
{
    if (get_boot_stage() == BOOT_STAGE_AEE)
        return true;

    return false;
}

static bool mboot_params_should_restore(struct mboot_params_buffer *tmp_mboot_params)
{
    unsigned int fiq_step, wdt_status, exp_type;

    mboot_params_reason_update(); /* init U_VAR(xxx) */

    if (U_VAR(set_flag) && tmp_mboot_params && tmp_mboot_params->off_linux &&
            (tmp_mboot_params->off_linux ==
                    (tmp_mboot_params->off_llk + ALIGN(tmp_mboot_params->sz_lk, 64))) &&
            (tmp_mboot_params->off_pl == sizeof(struct mboot_params_buffer))) {
        wdt_status = ((struct reboot_reason_pl *)
                ((void *)tmp_mboot_params + tmp_mboot_params->off_pl))->wdt_status;
        fiq_step = ((struct reboot_reason_kernel *)
                ((void *)tmp_mboot_params + tmp_mboot_params->off_linux))->fiq_step;
        exp_type = MBOOT_PARAMS_EXP_TYPE_DEC(((struct reboot_reason_kernel *)
                ((void *)tmp_mboot_params + tmp_mboot_params->off_linux))->exp_type);

        if ((wdt_status != 0 && wdt_status != U_VAR(wdt_status)) ||
            (fiq_step != 0 && fiq_step != U_VAR(fiq_step)) ||
            (exp_type != 0 && exp_type != U_VAR(exp_type))) {
            return true;
        }
    }
    return false;
}

void mboot_params_lk_save(unsigned int val, int index)
{
    struct reboot_reason_lk *rr_lk;

    if (mboot_params && mboot_params->off_lk < mboot_params_size) {
        rr_lk = (void *)mboot_params + mboot_params->off_lk;
        if (index < MBOOT_PARAMS_LK_SIZE)
            rr_lk->last_func[index] = val;
    }
}

/* default can be either sram or dram */
void mboot_params_addr_size(addr_t *addr, unsigned int *size)
{
    if (addr == NULL || size == NULL) {
        LOG("%s. get mboot params default info args invalid\n", AEETAG);
        return;
    }

    *addr = (addr_t)mboot_params;
    *size = mboot_params_size;
}

static void mboot_params_sram_addr_size(addr_t *addr, unsigned int *size)
{
    if (addr == NULL || size == NULL) {
        LOG("%s. get mboot params sram info args invalid\n", AEETAG);
        return;
    }

    *addr = mboot_params_info.sram_addr;
    *size = mboot_params_info.sram_size;
}

static u32 mboot_params_def_memory(void)
{
    return mboot_params_info.def_type;
}

static u32 mboot_params_sram_offset(void)
{
    return mboot_params_info.memory_info_offset;
}

void sram_plat_dbg_info_addr_size(addr_t *addr, unsigned int *size)
{
    if (addr == NULL || size == NULL) {
        LOG("%s. get plat dbg info args invalid\n", AEETAG);
        return;
    }

    *addr = memory_info->sram_plat_dbg_info_addr;
    *size = memory_info->sram_plat_dbg_info_size;
}

static void sram_log_store_addr_size(addr_t *addr, unsigned int *size)
{
    if (addr == NULL || size == NULL) {
        LOG("%s. get log_store info args invalid\n", AEETAG);
        return;
    }

    *addr = memory_info->sram_log_store_addr;
    *size = memory_info->sram_log_store_size;
}

void pstore_addr_size(addr_t *addr, unsigned int *size)
{
    const struct reserved_t *reserved = NULL;

    if (addr == NULL || size == NULL) {
        LOG("%s. get pstore info args invalid\n", AEETAG);
        return;
    }

    reserved = mblock_query_reserved_by_name("pstore", 0);
    if (!reserved) {
        LOG("%s: fail to get dram addr of pstore\n", AEETAG);
        *addr = 0;
        *size = 0;
        return;
    }

    *addr = reserved->start;
    *size = reserved->size;
}

void mrdump_mini_header_addr_size(addr_t *addr, unsigned int *size)
{
    const struct reserved_t *reserved = NULL;

    if (addr == NULL || size == NULL) {
        LOG("%s. get pstore info args invalid\n", AEETAG);
        return;
    }

    reserved = mblock_query_reserved_by_name("minirdump", 0);
    if (!reserved) {
        LOG("%s: fail to get dram addr of pstore\n", AEETAG);
        *addr = 0;
        *size = 0;
        return;
    }

    *addr = reserved->start;
    *size = reserved->size;
}

void mboot_params_dts_write(void *fdt)
{
    addr_t addr = 0;
    u32 size = 0;
    u32 buf[4];
    int offset;
    int ret;

    offset = fdt_path_offset(fdt, "/chosen");
    if (offset < 0)
        panic("get /chosen offset failed(%d)", offset);

    mboot_params_sram_addr_size(&addr, &size);
    buf[0] = addr;
    buf[1] = size;
    buf[2] = mboot_params_def_memory();
    buf[3] = mboot_params_sram_offset();
    dprintf(INFO, "%s. [0x%x,0x%x,0x%x,0x%x]\n", AEETAG, buf[0], buf[1],
            buf[2], buf[3]);
    ret = fdt_setprop(fdt, offset, "ram_console", buf, (4 * sizeof(u32)));
    if (ret)
        panic("dts write failed");

    sram_log_store_addr_size(&addr, &size);
    buf[0] = addr;
    buf[1] = size;
    dprintf(INFO, "%s. log_store [0x%x,0x%x]\n", AEETAG, buf[0], buf[1]);
    ret = fdt_setprop(fdt, offset, "log_store", buf, (2 * sizeof(u32)));
    if (ret)
        panic("log_store dts write failed");

}
SET_FDT_INIT_HOOK(mboot_params_dts_write, mboot_params_dts_write);

void mboot_params_set_dump_step(unsigned int step)
{
    if (mboot_params)
        mboot_params->dump_step = step;
}

int mboot_params_get_dump_step(void)
{
    if (mboot_params)
        return mboot_params->dump_step;
    else {
        LOG("%s. mboot_params not ready\n", AEETAG);
        return 0;
    }
}

unsigned int calculate_crc32(void *data, unsigned int len)
{
    unsigned int mycrc;
    unsigned int ret;

    mycrc = crc32(0L, Z_NULL, 0);
    ret = crc32(mycrc, data, len);
    LOG("kedump: crc = 0x%x\n", ret);
    return ret;
}

static int kedump_restore_mem(void)
{
    int i;
    struct ipanic_header *iheader;
    struct kedump_crc saved_crc = {0, 0};
    unsigned int crc;
    unsigned int sz_misc = 0;
    paddr_t addr_misc = 0, addr_misc_pstore = 0;
    unsigned char *temp_mboot_params = NULL;
    bdev_t *dev_expdb;
    unsigned int sz_misc_pstore = 0;
    unsigned char *temp_pstore = NULL;
    void *vaddr_pst = NULL;

    iheader = (struct ipanic_header *)malloc(sizeof(struct ipanic_header));
    if (iheader == NULL)
        return ERR_NO_MEMORY;

    dev_expdb = bio_open_by_label(AEE_IPANIC_LABEL);
    if (!dev_expdb) {
        LOG("kedump: Partition [%s] is not exist.\n", AEE_IPANIC_LABEL);
        free(iheader);
        return ERR_NOT_FOUND;
    }

    /* start: get info from expdb */
    bio_read(dev_expdb, (void *)iheader, 0, sizeof(struct ipanic_header));
    if (iheader->magic == AEE_IPANIC_MAGIC && iheader->version >= AEE_IPANIC_PHDR_VERSION) {
        LOG("kedump: found content in expdb\n");
        for (i = IPANIC_NR_SECTIONS - 1; i >= 0; i--) {
            if (strncmp((const char *)iheader->data_hdr[i].name,
                        "KEDUMP_CRC", sizeof("KEDUMP_CRC") - 1) == 0) {
                LOG("kedump: read %s from offset 0x%x size 0x%x\n",
                        iheader->data_hdr[i].name, iheader->data_hdr[i].offset,
                        iheader->data_hdr[i].used);
                bio_read(dev_expdb, (uchar *)(&saved_crc), iheader->data_hdr[i].offset,
                        iheader->data_hdr[i].used);
            } else if (strncmp((const char *)iheader->data_hdr[i].name,
                        "SYS_RAMCONSOLE_RAW", sizeof("SYS_RAMCONSOLE_RAW") - 1) == 0) {
                mboot_params_addr_size(&addr_misc, &sz_misc);
                if (addr_misc && sz_misc) {
                    temp_mboot_params = malloc(sz_misc);
                    if (!temp_mboot_params) {
                        LOG("kedump: temp mboot_params alloc fail\n");
                    } else {
                        LOG("kedump: read %s from offset 0x%x size 0x%x\n",
                                iheader->data_hdr[i].name, iheader->data_hdr[i].offset,
                                iheader->data_hdr[i].used);
                        memset(temp_mboot_params, 0x0, sz_misc);
                        bio_read(dev_expdb, temp_mboot_params, iheader->data_hdr[i].offset,
                                iheader->data_hdr[i].used);
                        crc = calculate_crc32(temp_mboot_params, sz_misc);
                        if (crc != saved_crc.mboot_params_crc) {
                            LOG("kedump: temp mboot_params crc fail\n");
                            free(temp_mboot_params);
                            temp_mboot_params = NULL;
                        }
                    }
                } else {
                    LOG("kedump: mboot_params not init\n");
                }
            } else if (strncmp((const char *)iheader->data_hdr[i].name,
                        "SYS_PSTORE_RAW", sizeof("SYS_PSTORE_RAW") - 1) == 0) {
                pstore_addr_size(&addr_misc_pstore, &sz_misc_pstore);
                if (!addr_misc_pstore || !sz_misc_pstore)
                    continue;
                temp_pstore = malloc(sz_misc_pstore);
                if (!temp_pstore) {
                    LOG("kedump: temp pstore alloc fail\n");
                } else {
                    LOG("kedump: read %s from offset 0x%x size 0x%x\n",
                                iheader->data_hdr[i].name, iheader->data_hdr[i].offset,
                                iheader->data_hdr[i].used);
                    memset(temp_pstore, 0x0, sz_misc_pstore);
                    bio_read(dev_expdb, temp_pstore, iheader->data_hdr[i].offset,
                            iheader->data_hdr[i].used);
                    crc = calculate_crc32(temp_pstore, sz_misc_pstore);
                    if (crc != saved_crc.pstore_crc) {
                        LOG("kedump: temp pstore crc fail\n");
                        free(temp_pstore);
                        temp_pstore = NULL;
                    }
                }
            }
        }
        /* end: get info from expdb */

        /* check if we should store the info from expdb to dram/sram */
        if (mboot_params_should_restore((struct mboot_params_buffer *)temp_mboot_params)) {
            LOG("kedump: mboot_params_should_restore\n");
            memcpy((uchar *)addr_misc, temp_mboot_params, sz_misc); /* addr_misc is VA */
            if (temp_pstore) {
                /* access memory in DRAM, map it first */
                int map_ok = vmm_alloc_physical(vmm_get_kernel_aspace(),
                        "pstore_raw", sz_misc_pstore,
                        &vaddr_pst, PAGE_SIZE_SHIFT,
                        (paddr_t)addr_misc_pstore, 0, ARCH_MMU_FLAG_CACHED);
                if (map_ok == NO_ERROR) {
                    memcpy((uchar *)vaddr_pst, temp_pstore, sz_misc_pstore);
                    vmm_free_region(vmm_get_kernel_aspace(), (vaddr_t)vaddr_pst);
                } else {
                    LOG("kedump: map error for pstore\n");
                }
            }
            mboot_params_set_dump_step(AEE_LKDUMP_CLEAR);
            mboot_params_reason_update(); /* update reboot reason info */
        }

        if (temp_mboot_params)
            free(temp_mboot_params);
        if (temp_pstore)
            free(temp_pstore);
    }
    free(iheader);
    bio_close(dev_expdb);
    return 0;
}

static void mboot_params_init(uint level)
{
    mboot_params_ptr_init();

    if (mboot_params) {
        dprintf(INFO, "%s. start: %p, size: 0x%x\n",
                AEETAG, mboot_params, mboot_params_size);
    } else {
        LOG("%s. mboot params init failed\n", AEETAG);
        return;
    }

    mboot_params->off_lk = mboot_params->off_lpl + ALIGN(mboot_params->sz_pl, 64);
    mboot_params->off_llk = mboot_params->off_lk + ALIGN(sizeof(struct reboot_reason_lk), 64);
    mboot_params->sz_lk = sizeof(struct reboot_reason_lk);
    if (mboot_params->sz_lk == sizeof(struct reboot_reason_lk) &&
            (mboot_params->off_lk + mboot_params->sz_lk == mboot_params->off_llk)) {
        memcpy((void *)mboot_params + mboot_params->off_llk,
                (void *)mboot_params + mboot_params->off_lk, mboot_params->sz_lk);
    } else {
        LOG("%s. lk size mismatch %x + %x != %x\n",
                AEETAG, mboot_params->sz_lk, mboot_params->off_lk, mboot_params->off_llk);
        mboot_params->sz_lk = sizeof(struct reboot_reason_lk);
    }
    mboot_params->off_linux = mboot_params->off_llk + ALIGN(mboot_params->sz_lk, 64);
}

static void mboot_params_late_init(uint level)
{
    char cmd_buf[256];
    int err = 0;
    u32 bootstage = get_boot_stage();

    if (mboot_params_reboot_by_cold_reset())
        LOG("kedump: last is full pmic reset!\n");
    else
        LOG("kedump: last is not full pmic reset!\n");

    if (bootstage  == BOOT_STAGE_AEE) {
        /* will do cold reset after lk dump finished */
        LOG("kedump: lk dump flow\n");
        return;
    }

    if (bootstage != BOOT_STAGE_BL33) {
        /* skip the restore and cmdline works if not in bl33 stage */
        LOG("kedump: normal flow at stage %d\n", bootstage);
        return;
    }

    LOG("kedump: normal flow\n");
    // for power lost, full pmic reset, or reboot before KE DB collected scenario
    kedump_restore_mem();

    memset(cmd_buf, 0x0, sizeof(cmd_buf));
    err = snprintf(cmd_buf, sizeof(cmd_buf),
        "ramoops.mem_address=0x%x ramoops.mem_size=0x%x ramoops.pmsg_size=0x%x ramoops.console_size=0x%x",
        memory_info->pstore_addr, memory_info->pstore_size,
        memory_info->pstore_pmsg_size, memory_info->pstore_console_size);
    if (err > 0)
        kcmdline_append(cmd_buf);
    else
        LOG("kedump: failed to set pstore\n");
}

static void mboot_params_pl_boottag_hook(struct boot_tag *tag)
{
    memcpy(&mboot_params_info, &tag->data, sizeof(mboot_params_info));

    if (mboot_params_info.def_type != MBOOT_PARAMS_DEF_SRAM &&
            mboot_params_info.def_type != MBOOT_PARAMS_DEF_DRAM)
        panic("unknown def type");

    if (mboot_params_info.sram_addr == 0 || mboot_params_info.sram_size == 0)
        panic("sram addr size zero");

    if (mboot_params_info.memory_info_offset <= mboot_params_info.sram_size)
        panic("memory offset invalid value");

    memory_info = (struct mboot_params_memory_info *)(
            paddr_to_kvaddr(mboot_params_info.sram_addr) +
            mboot_params_info.memory_info_offset);
    LOG("mboot_params_info init hook: 0x%x, 0x%x, 0x%x, 0x%x\n",
            mboot_params_info.sram_addr, mboot_params_info.sram_size,
            mboot_params_info.def_type, mboot_params_info.memory_info_offset);
}

PL_BOOTTAGS_INIT_HOOK(mboot_params_tag, BOOT_TAG_MBOOT_PARAMS_INFO,
                                    mboot_params_pl_boottag_hook);


u32 aee_ddr_reserve_ready(void)
{
    return ddr_reserve_state.ddr_reserve_ready;
}

u32 aee_ddr_reserve_enable(void)
{
    return ddr_reserve_state.ddr_reserve_enable;
}

u32 aee_ddr_reserve_success(void)
{
    return ddr_reserve_state.ddr_reserve_success;
}

static void ddr_state_pl_boottag_hook(struct boot_tag *tag)
{
    memcpy(&ddr_reserve_state, &tag->data, sizeof(ddr_reserve_state));
    dprintf(ALWAYS, "%s get ddr_reserve_state done\n", AEETAG);
}
PL_BOOTTAGS_INIT_HOOK(ddr_state, BOOT_TAG_DDR_RESERVE, ddr_state_pl_boottag_hook);

LK_INIT_HOOK(init_mboot_params, mboot_params_init, LK_INIT_LEVEL_KERNEL);
LK_INIT_HOOK(init_mboot_params_late, mboot_params_late_init, LK_INIT_LEVEL_TARGET);
