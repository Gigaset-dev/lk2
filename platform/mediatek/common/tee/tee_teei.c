/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#include <arch/mmu.h>
#include <assert.h>
#include <chip_id.h>
#include <err.h>
#include <kernel/vm.h>
#include <key_derive.h>
//prize add by lipengpeng 20220704 start 
#include <lib/pl_boottags.h>
//prize add by lipengpeng 20220704 end 
#include <mblock.h>
#include <platform.h>
#include <platform/sec_devinfo.h>
#include <platform/tee_platform_teei.h>
#include <platform_mtk.h>
#include <reg.h>
#include <rtc.h>
#include <sboot.h>
#include <string.h>
#include "tee_loader.h"
#include "tee_teei.h"
//prize add by lipengpeng 20220704 start 
#include <ufs_interface.h>
//prize add by lipengpeng 20220704 end 
#include <verified_boot_common.h>

/* #define TEEI_DEBUG 1 */

#if (CFG_RPMB_SET_KEY && CFG_RPMB_SET_KEY_DELAY)
extern unsigned char rpmb_key_flag;
#endif

static void CONFIG_ISEE_DEVS(struct tee_dev *devs, u32 type, u64 addr, u32 irq,
    u32 dapc_num, u32 param0)
{
    ((struct tee_dev *)devs)->dev_type = type;
    ((struct tee_dev *)devs)->base_addr = addr;
    ((struct tee_dev *)devs)->intr_num = irq;
    ((struct tee_dev *)devs)->apc_num = dapc_num;
    ((struct tee_dev *)devs)->param[0] = param0;

}

static void CONFIG_ISEE_FLAGS(u64 *flags, u32 set_value, u64 mask, u32 offset)
{
    u64 teei_flags = *flags;

    teei_flags &= ~mask;
#ifdef TEEI_DEBUG
    dprintf(INFO, "[START] FLAGS[0x%x] VALUE[0x%x] OFFSET[0x%x]\n", (unsigned int)teei_flags,
        (unsigned int)set_value, offset);
#endif
    teei_flags = teei_flags | (set_value << offset);
#ifdef TEEI_DEBUG
    dprintf(INFO, "[END] FLAGS[0x%x]\n", (unsigned int)teei_flags);
#endif
    *flags = teei_flags;
}

static void CONFIG_ISEE_FLAGS_MULTI_SELECTION(u64 *flags, u32 set_value, u64 mask,
    u32 offset)
{
    u64 teei_flags = *flags;

    if (!set_value) {
#ifdef TEEI_DEBUG
        dprintf(INFO, "[START] FLAGS[0x%x] VALUE[0x%x] OFFSET[0x%x]\n",
            (unsigned int)teei_flags,
            (unsigned int)set_value, offset);
#endif
        teei_flags &= ~mask;

    }
    teei_flags |= (set_value << offset);
#ifdef TEEI_DEBUG
    dprintf(INFO, "[END] FLAGS[0x%x]\n", (unsigned int)teei_flags);
#endif
    *flags = teei_flags;
}

#ifdef TEEI_DEBUG
static void teei_boot_param_dump(struct tee_arg *teearg)
{
    dprintf(CRITICAL, "=== DUMP TEEI START @ BL2 ===\n");
    dprintf(CRITICAL, "teearg (%x)\n", teearg);

    dprintf(CRITICAL, "magic (%x) : 0x%x\n", &(teearg->magic), teearg->magic);
    dprintf(CRITICAL, "length (%x) : 0x%x\n", &(teearg->length), teearg->length);
    dprintf(CRITICAL, "tee_log_base (%x) : 0x%x\n",
        &(teearg->tee_log_base), teearg->tee_log_base);
    dprintf(CRITICAL, "secDRamBase (%x) : 0x%x\n", &(teearg->secDRamBase), teearg->secDRamBase);
    dprintf(CRITICAL, "secDRamSize (%x) : 0x%x\n", &(teearg->secDRamSize), teearg->secDRamSize);

    dprintf(CRITICAL, "gic_distributor_base (%x) : 0x%x\n", &(teearg->gic_distributor_base),
        teearg->gic_distributor_base);
    dprintf(CRITICAL, "gic_redistributor_base (%x) : 0x%x\n", &(teearg->gic_redistributor_base),
        teearg->gic_redistributor_base);
    dprintf(CRITICAL, "gic_version (%x) : 0x%x\n", &(teearg->gic_version), teearg->gic_version);

    dprintf(CRITICAL, "total_number_spi (%x) : 0x%x\n", &(teearg->total_number_spi),
        teearg->total_number_spi);
    dprintf(CRITICAL, "ssiq_number[0] (%x) : 0x%x\n", &(teearg->ssiq_number[0]),
        teearg->ssiq_number[0]);
    dprintf(CRITICAL, "ssiq_number[1] (%x) : 0x%x\n", &(teearg->ssiq_number[1]),
        teearg->ssiq_number[1]);
    dprintf(CRITICAL, "ssiq_number[2] (%x) : 0x%x\n", &(teearg->ssiq_number[2]),
        teearg->ssiq_number[2]);

    dprintf(CRITICAL, "tee_dev[0].dev_type (%x) : 0x%x\n", &((teearg->tee_dev[0]).dev_type),
        (teearg->tee_dev[0]).dev_type);
    dprintf(CRITICAL, "tee_dev[0].base_addr (%x) : 0x%x\n", &((teearg->tee_dev[0]).base_addr),
        (teearg->tee_dev[0]).base_addr);
    dprintf(CRITICAL, "tee_dev[0].intr_num (%x) : 0x%x\n", &((teearg->tee_dev[0]).intr_num),
        (teearg->tee_dev[0]).intr_num);
    dprintf(CRITICAL, "tee_dev[0].apc_num (%x) : 0x%x\n", &((teearg->tee_dev[0]).apc_num),
        (teearg->tee_dev[0]).apc_num);
    dprintf(CRITICAL, "tee_dev[1].dev_type (%x) : 0x%x\n", &(teearg->tee_dev[1].dev_type),
        teearg->tee_dev[1].dev_type);
    dprintf(CRITICAL, "tee_dev[1].base_addr (%x) : 0x%x\n", &(teearg->tee_dev[1].base_addr),
        teearg->tee_dev[1].base_addr);
    dprintf(CRITICAL, "tee_dev[1].intr_num (%x) : 0x%x\n", &(teearg->tee_dev[1].intr_num),
        teearg->tee_dev[1].intr_num);
    dprintf(CRITICAL, "tee_dev[1].apc_num (%x) : 0x%x\n", &(teearg->tee_dev[1].apc_num),
        teearg->tee_dev[1].apc_num);
    dprintf(CRITICAL, "tee_dev[2].dev_type (%x) : 0x%x\n", &(teearg->tee_dev[2].dev_type),
        teearg->tee_dev[2].dev_type);
    dprintf(CRITICAL, "tee_dev[2].base_addr (%x) : 0x%x\n", &(teearg->tee_dev[2].base_addr),
        teearg->tee_dev[2].base_addr);
    dprintf(CRITICAL, "tee_dev[2].intr_num (%x) : 0x%x\n", &(teearg->tee_dev[2].intr_num),
        teearg->tee_dev[2].intr_num);
    dprintf(CRITICAL, "tee_dev[2].apc_num (%x) : 0x%x\n", &(teearg->tee_dev[2].apc_num),
        teearg->tee_dev[2].apc_num);
    dprintf(CRITICAL, "tee_dev[3].dev_type (%x) : 0x%x\n", &(teearg->tee_dev[3].dev_type),
        teearg->tee_dev[3].dev_type);
    dprintf(CRITICAL, "tee_dev[3].base_addr (%x) : 0x%x\n", &(teearg->tee_dev[3].base_addr),
        teearg->tee_dev[3].base_addr);
    dprintf(CRITICAL, "tee_dev[3].intr_num (%x) : 0x%x\n", &(teearg->tee_dev[3].intr_num),
        teearg->tee_dev[3].intr_num);
    dprintf(CRITICAL, "tee_dev[3].apc_num (%x) : 0x%x\n", &(teearg->tee_dev[3].apc_num),
        teearg->tee_dev[3].apc_num);
    dprintf(CRITICAL, "tee_dev[4].dev_type (%x) : 0x%x\n", &(teearg->tee_dev[4].dev_type),
        teearg->tee_dev[4].dev_type);
    dprintf(CRITICAL, "tee_dev[4].base_addr (%x) : 0x%x\n", &(teearg->tee_dev[4].base_addr),
        teearg->tee_dev[4].base_addr);
    dprintf(CRITICAL, "tee_dev[4].intr_num (%x) : 0x%x\n", &(teearg->tee_dev[4].intr_num),
        teearg->tee_dev[4].intr_num);
    dprintf(CRITICAL, "tee_dev[4].apc_num (%x) : 0x%x\n", &(teearg->tee_dev[4].apc_num),
        teearg->tee_dev[4].apc_num);
    dprintf(CRITICAL, "tee_dev[4].param[0] (%x): 0x%x\n", &(teearg->tee_dev[4].param[0]),
        teearg->tee_dev[4].param[0]);

    dprintf(CRITICAL, "flags (%x) : 0x%x\n", &(teearg->flags), teearg->flags);
    dprintf(CRITICAL, "chip_hw_code (%x) : 0x%x\n",
        &(teearg->chip_hw_code), teearg->chip_hw_code);
}
#endif

static int is_tee_64_bit_supported(u32 tee_entry)
{
    u32 tee_first_instr;

    tee_first_instr = *(u32 *)tee_entry;
#define INSTR_32BIT_NOP 0xE320F000
    if (tee_first_instr == (u32)INSTR_32BIT_NOP) {
        dprintf(INFO, "tee is 32-bit supported (0x%x)\n", tee_first_instr);
        return 0;
    } else {
        dprintf(INFO, "tee is 64-bit supported (0x%x)\n", tee_first_instr);
        return 1;
    }
}

static unsigned long rtc_mktime(int yea, int mth, int dom, int hou, int min, int sec)
{
    unsigned long d1, d2, d3;

    mth -= 2;
    if (mth <= 0) {
            mth += 12;
            yea -= 1;
    }

    d1 = (yea - 1) * 365 + (yea / 4 - yea / 100 + yea / 400);
    d2 = (367 * mth / 12 - 30) + 59;
    d3 = d1 + d2 + (dom - 1) - 719162;

    return ((d3 * 24 + hou) * 60 + min) * 60 + sec;
}

static unsigned long rtcgettime_teei(void)
{
    struct rtc_time tm;
    unsigned long now_time;

    rtc_get_time(&tm);

    now_time = rtc_mktime(tm.tm_year, tm.tm_mon, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
    dprintf(INFO, "%s now_time = %d\n", __func__, now_time);

    return now_time;
}

static void teei_rtctime_param_prepare(u64 param_addr)
{
    struct tee_arg *teearg = (struct tee_arg *)param_addr;
    struct tee_dev *teedev1 = &(teearg->tee_dev[1]);
    u32 rtctime = 0;

    rtctime = rtcgettime_teei();
    teedev1->param[0] = rtctime;
}

/**************************************************************************
 * TEE FUNCTIONS
 **************************************************************************/
static void teei_key_param_prepare(u64 key_addr)
{
    u32 i;
    u32 ret = 0;
    u32 HRID[8];

    struct tee_keys *keyarg = (struct tee_keys *)key_addr;

    memset(keyarg->hr_id, 0, KEY_LEN);
    memset(keyarg->rpmb_key, 0, KEY_LEN);
    memset(keyarg->hw_id, 0, HWUID_SIZE);
    keyarg->magic = TEEI_BOOTCFG_MAGIC;
    keyarg->version = 0xFCBEFABE;

    /* hr_id[]: 32 bytes Length */
    for (i = 0; i < HRID_SIZE; i++)
        HRID[i] = get_devinfo_with_index(12 + i);
    for (i = HRID_SIZE; i < (sizeof(HRID) / sizeof(u32)); i++)
        HRID[i] = 0;

    keyarg->hrid_size = sizeof(u32) * HRID_SIZE;
//prize add by lipengpeng 20220704 start 	
    if (keyarg->hrid_size)
//prize add by lipengpeng 20220704 start 	
    memcpy(keyarg->hr_id, HRID, keyarg->hrid_size);

    /* hw_id[]: 16 bytes Length */
    keyarg->hwid_size = HWUID_SIZE;
    ret = sec_get_meid_from_pl_boottags(keyarg->hw_id, keyarg->hwid_size);
    if (ret) {
        dprintf(CRITICAL, "Get MEID from pl_boottags failed, ret = 0x%lx.\n", ret);
        goto end;
    }

    /* rpmb_key[]: 32 bytes Length */
    ret = sec_key_derive_adapter(KEY_TYPE_RPMB, keyarg->rpmb_key, RPMB_KEY_SIZE);
    if (ret) {
        dprintf(CRITICAL, "RPMB Key derive from seclib failed, ret = 0x%lx.\n", ret);
        goto end;
    }

end:
    if (ret) {
        dprintf(CRITICAL, "ERROR: Prepare TEEI key parameters failed!!!\n");
        dprintf(CRITICAL, "Please check the keys for teei.\n");
        dprintf(CRITICAL, "Or there will be unexpected fail case in teei os\n");
    }
}

static u64 get_tee_log_buf(void)
{
    u64 addr;

    addr = mblock_alloc((u64)TEE_LOG_BUF_SIZE, 0x200000,
                    MBLOCK_NO_LIMIT, 0, 0, "security_tee_log");

    return addr;
}

static u64 alloc_tee_ree_share_mem_buf(void)
{
    u64 addr;

    addr = mblock_alloc((u64)TEEI_RESERVED_REE_SHAREMEM_SIZE, 0x200000,
                    MBLOCK_NO_LIMIT, 0, 0, "security_tee_ree");

    return addr;
}

void teei_boot_param_prepare(u64 param_addr, u64 tee_entry, u64 teei_sec_dram_size,
    u64 dram_base, u64 dram_size)
{
    struct tee_arg *teearg = (struct tee_arg *)param_addr;
    u32 teei_log_port = 0;
    u64 tee_log_buf_start;
    u64 tee_ree_share_buf_start;
    u64 tee_key_param_addr;

    //dprintf(INFO, "==================================================================\n");
    //dprintf(INFO, "uTos PL VERSION [%s]\n", UTOS_VERSION);
    //dprintf(INFO, "==================================================================\n");

    if (teearg == NULL) {
        dprintf(CRITICAL, "boot param argument is NULL!\n");
        return;
    }

    memset(teearg, 0, sizeof(struct tee_arg));

    tee_log_buf_start = get_tee_log_buf();
    if (tee_log_buf_start == NULL) {
        dprintf(CRITICAL, "Failed to alloc memory to dump TEE log.\n");
        return;
    }

    tee_ree_share_buf_start = alloc_tee_ree_share_mem_buf();
    if (tee_ree_share_buf_start == NULL) {
        dprintf(CRITICAL, "Failed to alloc the shared memory for communication.\n");
        return;
    }

    /* Prepare TEE boot parameters */
    teearg->magic = TEEI_BOOTCFG_MAGIC;          /* TEEI magic number */
    teearg->length = sizeof(struct tee_arg);     /* TEEI argument block size */
    teearg->tee_log_base = tee_log_buf_start;    /* TEEI dump log buffer */
    teearg->secDRamBase = tee_entry;             /* Secure DRAM base address */
    teearg->secDRamSize = teei_sec_dram_size;    /* Secure DRAM size */
    dprintf(INFO, "%s secDRamSize: 0x%x\n", __func__, teearg->secDRamSize);
    teearg->share_mem_paddr = tee_ree_share_buf_start;
    teearg->share_mem_size  = TEEI_RESERVED_REE_SHAREMEM_SIZE;

    /* SSI Reserve */
    teearg->total_number_spi = 256;              /* Platform Support total 256 SPIs and 32 PPIs */
    //teearg->ssiq_number[0] = (32 + 248);
    teearg->ssiq_number[0] = 0;
    teearg->ssiq_number[1] = 0;
    teearg->ssiq_number[2] = TEE_REV_IRQ_NO;

    /* GIC parameters */
    teearg->gic_distributor_base = GIC_BASE;
    teearg->gic_redistributor_base = GIC_RBASE;
    teearg->gic_version = GIC_VERSION;
    teearg->chip_hw_code = get_chip_hw_code();
    dprintf(INFO, "%s teearg->chip_hw_code: 0x%x\n", __func__, teearg->chip_hw_code);

    /* Configure for ISEE flags */
    teearg->flags = ISEE_FLAGS_DEFAULT_VALUE;
    CONFIG_ISEE_FLAGS(&teearg->flags, ISEE_FLAGS_LOG_PATH_BUFFER, ISEE_FLAGS_LOG_PATH_MASK,
        ISEE_FLAGS_LOG_PATH_OFFSET);
    CONFIG_ISEE_FLAGS(&teearg->flags, ISEE_FLAGS_LOG_LEVEL_INFO, ISEE_FLAGS_LOG_LEVEL_MASK,
        ISEE_FLAGS_LOG_LEVEL_OFFSET);
    CONFIG_ISEE_FLAGS(&teearg->flags, ISEE_FLAGS_SOTER_TYPE_MINIMAL, ISEE_FLAGS_SOTER_TYPE_MASK,
        ISEE_FLAGS_SOTER_TYPE_OFFSET);
    CONFIG_ISEE_FLAGS(&teearg->flags, ISEE_FLAGS_VERIFY_MODE_DISABLE_COUNTER,
        ISEE_FLAGS_VERIFY_MODE_MASK, ISEE_FLAGS_VERIFY_MODE_OFFSET);
#if CFG_RPMB_SET_KEY
    CONFIG_ISEE_FLAGS(&teearg->flags, ISEE_FLAGS_SECURE_STORAGE_RPMB,
        ISEE_FLAGS_SECURE_STORAGE_MASK, ISEE_FLAGS_SECURE_STORAGE_OFFSET);
#else
    CONFIG_ISEE_FLAGS(&teearg->flags, ISEE_FLAGS_SECURE_STORAGE_FS,
        ISEE_FLAGS_SECURE_STORAGE_MASK, ISEE_FLAGS_SECURE_STORAGE_OFFSET);
#endif

    CONFIG_ISEE_FLAGS_MULTI_SELECTION(&teearg->flags, ISEE_FLAGS_SOTER_FEATURE_RESET,
        ISEE_FLAGS_SOTER_FEATURE_MASK, ISEE_FLAGS_SOTER_FEATURE_OFFSET);
#if CFG_MICROTRUST_TUI_SUPPORT
    CONFIG_ISEE_FLAGS_MULTI_SELECTION(&teearg->flags, ISEE_FLAGS_SOTER_FEATURE_TUI,
        ISEE_FLAGS_SOTER_FEATURE_MASK, ISEE_FLAGS_SOTER_FEATURE_OFFSET);
#endif
#if CFG_MICROTRUST_GP_SUPPORT
    CONFIG_ISEE_FLAGS_MULTI_SELECTION(&teearg->flags, ISEE_FLAGS_SOTER_FEATURE_GP,
        ISEE_FLAGS_SOTER_FEATURE_MASK, ISEE_FLAGS_SOTER_FEATURE_OFFSET);
#endif
    CONFIG_ISEE_FLAGS_MULTI_SELECTION(&teearg->flags, ISEE_FLAGS_SOTER_FEATURE_DRM,
        ISEE_FLAGS_SOTER_FEATURE_MASK, ISEE_FLAGS_SOTER_FEATURE_OFFSET);
    //if (is_tee_64_bit_supported(tee_entry)) { /* TODO */
    CONFIG_ISEE_FLAGS_MULTI_SELECTION(&teearg->flags, ISEE_FLAGS_SOTER_FEATURE_AARCH64,
        ISEE_FLAGS_SOTER_FEATURE_MASK, ISEE_FLAGS_SOTER_FEATURE_OFFSET);
    //}

    /* TODO */
    //if (!seclib_sbc_enabled())
    //    CONFIG_ISEE_FLAGS_MULTI_SELECTION(&teearg->flags, ISEE_FLAGS_SOTER_FEATURE_EFUSEDISABLE,
    //        ISEE_FLAGS_SOTER_FEATURE_MASK, ISEE_FLAGS_SOTER_FEATURE_OFFSET);

    /* Default disabled Multi-Ta */
    // CONFIG_ISEE_FLAGS(&teearg->flags, ISEE_FLAGS_SOTER_FEATURE_MULTI_TA,
    //    ISEE_FLAGS_SOTER_FEATURE_MASK, ISEE_FLAGS_SOTER_FEATURE_OFFSET);
    CONFIG_ISEE_FLAGS(&teearg->flags, ISEE_FLAGS_SOTER_SYSTEM_DEBUG_OPEN,
        ISEE_FLAGS_SOTER_SYSTEM_DEBUG_MASK, ISEE_FLAGS_SOTER_SYSTEM_DEBUG_OFFSET);
    CONFIG_ISEE_FLAGS(&teearg->flags, ISEE_FLAGS_BOOT_RELEASE_MODE, ISEE_FLAGS_BOOT_MODE_MASK,
        ISEE_FLAGS_BOOT_MODE_OFFSET);
    CONFIG_ISEE_FLAGS(&teearg->flags, ISEE_FLAGS_SYSTRACE_DISABLE_MODE,
        ISEE_FLAGS_SYSTRACE_MODE_MASK, ISEE_FLAGS_SYSTRACE_MODE_OFFSET);
#if (CFG_RPMB_SET_KEY && CFG_RPMB_SET_KEY_DELAY)
//prize add by lipengpeng 20220704 start 
    if (ufs_rpmb_get_setkey_flag() == 1)
//prize add by lipengpeng 20220704 start 
        CONFIG_ISEE_FLAGS(&teearg->flags, ISEE_FLAGS_RPMB_KEY_NOT_PROGRAMMED,
            ISEE_FLAGS_RPMB_KEY_PROGRAM_MASK, ISEE_FLAGS_RPMB_KEY_PROGRAM_OFFSET);
    else
        CONFIG_ISEE_FLAGS(&teearg->flags, ISEE_FLAGS_RPMB_KEY_IS_PROGRAMMED,
            ISEE_FLAGS_RPMB_KEY_PROGRAM_MASK, ISEE_FLAGS_RPMB_KEY_PROGRAM_OFFSET);
#endif
//prize add by lipengpeng 20220704 start 

    /* Add this for enc ta log start */
#if CFG_MICROTRUST_TEE_LOG_ENC_DISABLE
    CONFIG_ISEE_FLAGS(&teearg->flags, ISEE_FLAGS_LOG_CRYPTO_DISABLE_MODE, ISEE_FLAGS_LOG_CRYPTO_MASK, ISEE_FLAGS_LOG_CRYPTO_OFFSET);
#else
    CONFIG_ISEE_FLAGS(&teearg->flags, ISEE_FLAGS_LOG_CRYPTO_ENABLE_MODE, ISEE_FLAGS_LOG_CRYPTO_MASK, ISEE_FLAGS_LOG_CRYPTO_OFFSET);
#endif
    /* Add this for enc ta log end */
	
//prize add by lipengpeng 20220704 end 
    dprintf(INFO, "%s teearg->flags :  0x%x\n", __func__, (unsigned int)teearg->flags);

    teei_log_port = DEBUG_UART;
    dprintf(INFO, "%s teearg : CFG_BOOT_ARGUMENT_BY_ATAG 0x%x\n", __func__, DEBUG_UART);

    /* UART parameters for Log Debug from Uart */
    switch (teei_log_port) {
    case 0:
        CONFIG_ISEE_DEVS(&(teearg->tee_dev[0]), MT_UART16550, (UART0_BASE - KERNEL_ASPACE_BASE),
            MT_UART0_IRQ, MT_UART0_DAPC, DEBUG_BAUDRATE);
        break;
    case 1:
        CONFIG_ISEE_DEVS(&(teearg->tee_dev[0]), MT_UART16550, (UART1_BASE - KERNEL_ASPACE_BASE),
            MT_UART1_IRQ, MT_UART1_DAPC, DEBUG_BAUDRATE);
        break;
    default:
        break;
    }

    teei_rtctime_param_prepare(param_addr);

    /* when  spi  is used, you should set a right SPI_INDEX*/
#ifdef CFG_MICROTRUST_FP_SUPPORT
    CONFIG_ISEE_DEVS(&(teearg->tee_dev[4]),
            MT_SEC_SPI | DEBUG | LOOPBACK, 0, 0, 0, SPI_INDEX);
#endif

    tee_key_param_addr = param_addr + 0x200;
    teei_key_param_prepare(tee_key_param_addr);

#ifdef TEEI_DEBUG
    teei_boot_param_dump(teearg);
#endif
}

DECLARE_TEE_MODULE("Microtrust", NULL, teei_boot_param_prepare);

