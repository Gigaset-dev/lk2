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

#include <ufs_cfg.h>
#include "ufs.h"
#include "ufs_core.h"
#include "ufs_error.h"
#include "ufs_hcd.h"
#include "ufs_platform.h"
#include "ufs_quirks.h"
#include "ufs_types.h"
#include "ufs_unipro.h"
#include "ufs_utils.h"

#include <arch/ops.h>  /* for cache maintanance APIs */
#include <assert.h>
#include <endian.h>
#include <kernel/event.h>
#include <kernel/vm.h>
#include <mt_gic.h>  /* for mt_irq_ APIs */
#include <platform/interrupts.h>  /* for mask_interrupt APIs */
#include <platform_mtk.h>
#include <profiling.h>
#include <smc.h>
#include <smc_id_table.h>
#include <stdlib.h>
#include <string.h>  /* For memcpy, memset */
#include <trace.h>

#define LOCAL_TRACE 0

static event_t ufshcd_intr_event;
static u32 ufshcd_intr_status;

static struct ufs_hba g_ufs_hba = {0};

static u32 ufs_query_desc_max_size[] = {
    QUERY_DESC_DEVICE_MAX_SIZE,
    QUERY_DESC_CONFIGURAION_MAX_SIZE_HPB_20,
    QUERY_DESC_UNIT_MAX_SIZE,
    QUERY_DESC_RFU_MAX_SIZE,
    QUERY_DESC_INTERCONNECT_MAX_SIZE,
    QUERY_DESC_STRING_MAX_SIZE,
    QUERY_DESC_RFU_MAX_SIZE,
    QUERY_DESC_GEOMETRY_MAX_SIZE,
    QUERY_DESC_POWER_MAX_SIZE,
    QUERY_DESC_HEALTH_MAX_SIZE,
    QUERY_DESC_RFU_MAX_SIZE,
};

static void ufshcd_init_desc_sizes(struct ufs_hba *hba);
static void ufs_reset_device(struct ufs_hba *hba);
static void ufs_util_dump_config_table_hex(u8 *table, u32 bytes,
    u32 ud0_base_offset, u32 ud_config_len);
static void ufs_util_dump_table_hex(u8 *table, u32 bytes);

static enum handler_return ufshcd_irq_handler(void *arg)
{
    struct ufs_hba *hba = &g_ufs_hba;
    struct ufshcd_lrb *lrbp;

    lrbp = &hba->lrb[0];

    ufshcd_intr_status = ufs_readl(hba, REG_INTERRUPT_STATUS);
    if (ufshcd_intr_status) {
        ufshcd_writel(hba, ufshcd_intr_status, REG_INTERRUPT_STATUS);
        /* DSB make sure clear initerrupt status */
        DSB;

        if (hba->outstanding_reqs) {
            /* Wakeup ufshcd_wait_command */
            event_signal(&ufshcd_intr_event, 0);
            hba->outstanding_reqs &= ~(1 << lrbp->task_tag);
        }
    }

    /* Let irq handler can enter again, must return INT_RESCHEDULE */
    return INT_RESCHEDULE;
}

__WEAK int ufs_get_irq_id(void)
{
    return 0xFFFFFFFF;
}

static int ufshcd_irq_init(struct ufs_hba *hba)
{
    hba->irq = ufs_get_irq_id();

    if (hba->irq == 0xFFFFFFFF) {
        dprintf(CRITICAL, "[UFS] UFS_IRQ_ID is not define\n");
        return -1;
    }

    /* Set ufs interrupt level, polarity */
    mt_irq_set_sens(hba->irq, GIC_LEVEL_SENSITIVE);
    mt_irq_set_polarity(hba->irq, GIC_LOW_POLARITY);

    /* Init event for thread function ufshcd_wait_command sleep/wakeup */
    event_init(&ufshcd_intr_event, false, EVENT_FLAG_AUTOUNSIGNAL);
    register_int_handler(hba->irq, ufshcd_irq_handler, NULL);

    /*
     * Mask all interrupt first, only unmask before thread sleep and wait.
     * Else there may have un-wanted interrupt come in, which is needed by
     * polling function, like UIC_COMMAND_COMPL
     */
    mask_interrupt(hba->irq);

    return 0;
}

static inline void ufshcd_get_host_capabilities(struct ufs_hba *hba)
{
    /* for AIO driver, only 1 task is enough */
    hba->nutrs = UFS_MAX_NUTRS;
    /* hba->nutmrs = UFS_MAX_NUTMRS; */
}

/**
 * ufshcd_memory_alloc - allocate memory for hba memory space data structures
 * @hba: per adapter instance
 *
 * 1. Allocate DMA memory for Command Descriptor array
 *    Each command descriptor consist of Command UPIU, Response UPIU and PRDT
 * 2. Allocate DMA memory for UTP Transfer Request Descriptor List (UTRDL).
 * 3. Allocate DMA memory for UTP Task Management Request Descriptor List
 *    (UTMRDL)
 * 4. Allocate memory for local reference block(lrb).
 *
 * Returns 0 for success, non-zero in case of failure
 */
static int ufshcd_memory_alloc(struct ufs_hba *hba)
{
    /* Allocate memory for UTP command descriptors */
    hba->ucdl_base_addr = memalign(UFS_ALIGN_UCD,
        sizeof(struct utp_transfer_cmd_desc) * UFS_MAX_NUTRS);
    /* malloc fail */
    ASSERT(hba->ucdl_base_addr);
    hba->ucdl_dma_addr = vaddr_to_paddr(hba->ucdl_base_addr);

    if (hba->ucdl_dma_addr & (UFS_ALIGN_UCD - 1)) {
        LTRACEF("memory alignment failed: ucdl, addr 0x%x, shall align %d\n",
            (unsigned int)hba->ucdl_dma_addr, UFS_ALIGN_UCD);
        free(hba->ucdl_base_addr);
        goto out;
    }

    /*
     * Allocate memory for UTP Transfer descriptors
     * UFSHCI requires 1024 byte alignment of UTRD
     */
    hba->utrdl_base_addr = memalign(UFS_ALIGN_UTRD,
        sizeof(struct utp_transfer_req_desc) * UTRD_NUTRS);
    /* malloc fail */
    ASSERT(hba->utrdl_base_addr);
    hba->utrdl_dma_addr = vaddr_to_paddr(hba->utrdl_base_addr);

    if (hba->utrdl_dma_addr & (UFS_ALIGN_UTRD - 1)) {
        LTRACEF("memory alignment failed: utrdl, addr 0x%x, shall align %d\n",
            (unsigned int)hba->utrdl_dma_addr, UFS_ALIGN_UTRD);
        free(hba->ucdl_base_addr);
        free(hba->utrdl_base_addr);
        goto out;
    }

    /*
     * Note. UFSHCI 2.1 requires 1024-byte alignment for UTRD (UTP Transfer Request Descriptor)
     *
     * SW tricks:
     *  1. Fill-in UFS_HCI_REG_UTRLBA a 1024-byte alignment address to assume the 1st UTRD entry
     *     (task ID = 0) is in that address.
     *  2. Since every UTRD entry is fixed to 32 bytes, now we can use our real UTRD base address
     *     by using "its" task ID.
     */

    hba->active_tr_tag = (u8)(((unsigned long)hba->utrdl_base_addr & UFS_1KB_MASK) /
        sizeof(struct utp_transfer_req_desc));

    /* Allocate memory for local reference block */
    hba->lrb = malloc(sizeof(struct ufshcd_lrb) * UFS_MAX_NUTRS);
    /* malloc fail */
    ASSERT(hba->lrb);

    return 0;
out:
    return -1;
}

static void ufshcd_host_memory_configure(struct ufs_hba *hba)
{
    int i;
    int cmd_desc_size;
    u16 prdt_offset;
    u16 response_offset;
    paddr_t cmd_desc_element_addr;
    paddr_t cmd_desc_dma_addr;
    struct utp_transfer_req_desc *utrdlp;
    struct utp_transfer_cmd_desc *cmd_descp;

    cmd_descp = hba->ucdl_base_addr;
    utrdlp = hba->utrdl_base_addr;

    prdt_offset     = offsetof(struct utp_transfer_cmd_desc, prd_table);
    response_offset = offsetof(struct utp_transfer_cmd_desc, response_upiu);

    cmd_desc_dma_addr = hba->ucdl_dma_addr;
    cmd_desc_size     = sizeof(struct utp_transfer_cmd_desc);

    for (i = 0; i < hba->nutrs; i++) {
        cmd_desc_element_addr =
            (cmd_desc_dma_addr + (cmd_desc_size * i));
        utrdlp[i].command_desc_base_addr_hi = (u32)(upper_32_bits(cmd_desc_element_addr));
        utrdlp[i].command_desc_base_addr_lo = (u32)(lower_32_bits(cmd_desc_element_addr));

        utrdlp[i].response_upiu_length = (u16)(ALIGNED_UPIU_SIZE >> 2);
        utrdlp[i].prd_table_offset     = (u16)((prdt_offset >> 2));
        utrdlp[i].response_upiu_offset = (u16)((response_offset >> 2));

        hba->lrb[i].ucd_prdt_ptr = (struct ufshcd_sg_entry *)cmd_descp[i].prd_table;
        hba->lrb[i].ucd_req_ptr = (struct utp_upiu_req *)(cmd_descp + i);
        hba->lrb[i].ucd_rsp_ptr = (struct utp_upiu_rsp *)cmd_descp[i].response_upiu;
        hba->lrb[i].utr_descriptor_ptr = (utrdlp + i);
    }
}

/**
 * ufshcd_enable_intr - enable interrupts
 * @hba: per adapter instance
 * @intrs: interrupt bits
 */
static void ufshcd_enable_intr(struct ufs_hba *hba, u32 intrs)
{
    u32 set = ufshcd_readl(hba, REG_INTERRUPT_ENABLE);

    set |= intrs;

    ufshcd_writel(hba, set, REG_INTERRUPT_ENABLE);
}

static int ufshcd_hba_enable(struct ufs_hba *hba)
{
    int retry;

    if ((ufshcd_readl(hba, REG_CONTROLLER_ENABLE) & 0x1) == 1) {
        ufshcd_writel(hba, CONTROLLER_DISABLE,  REG_CONTROLLER_ENABLE);
        usleep(5000);
    }

    /*
     * Reset device after hba disabled
     * This is to make sure host TX is idle
     * when resetting device.
     */
    ufs_reset_device(hba);

    ufshcd_writel(hba, CONTROLLER_ENABLE, REG_CONTROLLER_ENABLE);
    usleep(1000);

    retry = 20;

    while ((ufshcd_readl(hba, REG_CONTROLLER_ENABLE) & 0x1) == 0) {
        if (retry-- == 0) {
            dprintf(CRITICAL, "Controller enable failed\n");
            return -1;
        }
        usleep(5000);
    }

    /* enable UIC related interrupts */
    ufshcd_enable_intr(hba, UFSHCD_UIC_MASK);

    return 0;
}

static int
__ufshcd_send_uic_cmd(struct ufs_hba *hba, struct uic_command *uic_cmd)
{
    if (!(ufshcd_readl(hba, REG_CONTROLLER_STATUS) & UIC_COMMAND_READY)) {
        LTRACEF("Controller not ready to accept UIC commands\n");
        return -1;
    }

    LTRACEF("[UFS] info: UCMD:%x,%x,%x,%x\n", uic_cmd->command, uic_cmd->argument1,
        uic_cmd->argument2, uic_cmd->argument3);

    hba->active_uic_cmd = uic_cmd;

    ufshcd_writel(hba, uic_cmd->argument1, REG_UIC_COMMAND_ARG_1);
    ufshcd_writel(hba, uic_cmd->argument2, REG_UIC_COMMAND_ARG_2);
    ufshcd_writel(hba, uic_cmd->argument3, REG_UIC_COMMAND_ARG_3);

    ufshcd_writel(hba, uic_cmd->command & COMMAND_OPCODE_MASK, REG_UIC_COMMAND);

    return 0;
}

static int ufshcd_wait_for_register(struct ufs_hba *hba, u32 reg, u32 mask,
    u32 val, unsigned long interval_us, unsigned long timeout_ms)
{
    unsigned long elapsed_us = 0;
    int err = 0;

    val &= mask;

    while (val != (ufshcd_readl(hba, reg) & mask)) {

        /* wakeup within 50us of expiry */
        usleep(interval_us);
        elapsed_us += interval_us;

        if (elapsed_us > timeout_ms * 1000) {
            if ((ufshcd_readl(hba, reg) & mask) != val)
                err = -1;
            break;
        }
    }

    return err;
}

static void ufshcd_uic_cmd_compl(struct ufs_hba *hba, u32 intr_status)
{
    if ((intr_status & UIC_COMMAND_COMPL) && hba->active_uic_cmd) {
        hba->active_uic_cmd->argument2 |=
            (ufshcd_readl(hba, REG_UIC_COMMAND_ARG_2) & MASK_UIC_COMMAND_RESULT);
        hba->active_uic_cmd->argument3 = ufshcd_readl(hba, REG_UIC_COMMAND_ARG_3);
    }
}

static int ufshcd_wait_for_uic_cmd(struct ufs_hba *hba, struct uic_command *uic_cmd)
{
    int ret;

    ret = ufshcd_wait_for_register(hba, REG_INTERRUPT_STATUS,
                                   UIC_COMMAND_COMPL, UIC_COMMAND_COMPL,
                                   100, UIC_CMD_TIMEOUT);
    if (!ret) {
        ufshcd_writel(hba, UIC_COMMAND_COMPL,
                      REG_INTERRUPT_STATUS);
        ufshcd_uic_cmd_compl(hba, UIC_COMMAND_COMPL);
    } else {
        LTRACEF("%s error! ret: %d\n", __func__, ret);
    }

    hba->active_uic_cmd = NULL;

    return ret;
}

static int ufshcd_send_uic_cmd(struct ufs_hba *hba, struct uic_command *uic_cmd)
{
    int ret;

    ret = __ufshcd_send_uic_cmd(hba, uic_cmd);

    if (!ret)
        ret = ufshcd_wait_for_uic_cmd(hba, uic_cmd);

    return ret;
}

int ufshcd_uic_cmd_run(struct ufs_hba *hba, struct uic_command *cmds, int ncmds)
{
    int i;
    int err = 0;

    for (i = 0; i < ncmds; i++) {

        err = ufshcd_send_uic_cmd(hba, &cmds[i]);

        if (err) {
            LTRACEF("%s fail, cmd: %x, arg1: %x\n", __func__,
                cmds->command, cmds->argument1);

            /* send next commands anyway */
        }
    }

    return err;
}

static int ufshcd_dme_link_startup(struct ufs_hba *hba)
{
    struct uic_command uic_cmd = {0};

    uic_cmd.command = UIC_CMD_DME_LINK_STARTUP;

    return ufshcd_send_uic_cmd(hba, &uic_cmd);
}

/**
 * ufshcd_is_device_present - Check if any device connected to
 *                  the host controller
 * @hba: pointer to adapter instance
 *
 * Returns 1 if device present, 0 if no device detected
 */
static inline int ufshcd_is_device_present(struct ufs_hba *hba)
{
    return (ufshcd_readl(hba, REG_CONTROLLER_STATUS) &
            DEVICE_PRESENT) ? 1 : 0;
}

/**
 * ufshcd_get_lists_status - Check UCRDY, UTRLRDY and UTMRLRDY
 * @reg: Register value of host controller status
 *
 * Returns integer, 0 on Success and positive value if failed
 */
static inline int ufshcd_get_lists_status(u32 reg)
{
    /*
     * The mask 0xFF is for the following HCS register bits
     * Bit      Description
     *  0       Device Present
     *  1       UTRLRDY
     *  2       UTMRLRDY
     *  3       UCRDY
     *  4       HEI
     *  5       DEI
     * 6-7      reserved
     */
    return (((reg) & (0xFF)) >> 1) ^ (0x07);
}

/**
 * ufshcd_enable_run_stop_reg - Enable run-stop registers,
 *          When run-stop registers are set to 1, it indicates the
 *          host controller that it can process the requests
 * @hba: per adapter instance
 */
static void ufshcd_enable_run_stop_reg(struct ufs_hba *hba)
{
    ufshcd_writel(hba, UTP_TASK_REQ_LIST_RUN_STOP_BIT,
                  REG_UTP_TASK_REQ_LIST_RUN_STOP);
    ufshcd_writel(hba, UTP_TRANSFER_REQ_LIST_RUN_STOP_BIT,
                  REG_UTP_TRANSFER_REQ_LIST_RUN_STOP);
}

static int ufshcd_make_hba_operational(struct ufs_hba *hba)
{
    int err = 0;
    u32 reg;

    ufshcd_writel(hba, upper_32_bits(hba->utrdl_dma_addr),
                  REG_UTP_TRANSFER_REQ_LIST_BASE_H);
    ufshcd_writel(hba, lower_32_bits(hba->utrdl_dma_addr),
                  REG_UTP_TRANSFER_REQ_LIST_BASE_L);

    ufshcd_enable_intr(hba, UFSHCD_ENABLE_INTRS);

    reg = ufshcd_readl(hba, REG_CONTROLLER_STATUS);
    if ((ufshcd_get_lists_status(reg)) == 0) {
        ufshcd_enable_run_stop_reg(hba);
    } else {
        LTRACEF("Host controller not ready to process requests");
        err = -1;
    }

    return err;
}

static void ufs_dma_map(addr_t buf, size_t len, enum dma_data_direction dir)
{
    if (dir == DMA_FROM_DEVICE) {
        /* read from device (or write to memory) */
        arch_invalidate_cache_range((addr_t)buf, (size_t)len);
    } else if (dir == DMA_TO_DEVICE) {
        /* write to device (or read from memory) */
        arch_clean_cache_range((addr_t)buf, (size_t)len);
    } else {
        /* bi-directional or read from memory (or write to device) */
        arch_clean_invalidate_cache_range((addr_t)buf, (size_t)len);
    }
}

static void ufs_dma_unmap(addr_t buf, size_t len, enum dma_data_direction dir)
{
    if (!buf || !len)
        return;

    /* bi-directional or read from device (or write to memory) */
    if (dir != DMA_TO_DEVICE)
        arch_invalidate_cache_range((addr_t)buf, (size_t)len);
}

int ufs_enable_unipro_cg(struct ufs_hba *hba, bool enable)
{
    u32 tmp;
    u32 cfg1 = ((1 << RX_SYMBOL_CLK_GATE_EN) | (1 << SYS_CLK_GATE_EN) |
        (1 << TX_CLK_GATE_EN));
    u32 cfg2 = (1 << TX_SYMBOL_CLK_REQ_FORCE);

    hba->dme_get(hba, UIC_ARG_MIB(VENDOR_SAVEPOWERCONTROL), &tmp);
    tmp = (tmp & ~cfg1) | ((enable) ? cfg1 : 0);
    hba->dme_set(hba, UIC_ARG_MIB(VENDOR_SAVEPOWERCONTROL), tmp);

    hba->dme_get(hba, UIC_ARG_MIB(VENDOR_DEBUGCLOCKENABLE), &tmp);
    tmp = (tmp & ~cfg2) | ((enable) ? 0 : cfg2);
    hba->dme_set(hba, UIC_ARG_MIB(VENDOR_DEBUGCLOCKENABLE), tmp);

    return 0;
}

/*
 * In early-porting stage, because of no bootrom, something finished by bootrom
 * shall be finished here instead.
 * Returns:
 *  0: Successful.
 *  Non-zero: Failed.
 */
static int ufs_bootrom_deputy(struct ufs_hba *hba)
{
#ifdef UFS_CFG_FPGA_PLATFORM
    u32 reg;
    u32 mask = 0;

    if (!hba->pericfg_base)
        return 1;

    reg = readl(hba->pericfg_base + REG_UFS_PERICFG);

    if (REG_UFS_PERICFG_LDO_N_BIT != 0xF)
        mask |= (1 << REG_UFS_PERICFG_LDO_N_BIT);

    if (REG_UFS_PERICFG_RST_N_BIT != 0xF)
        mask |= (1 << REG_UFS_PERICFG_RST_N_BIT);

    reg = reg & ~mask;
    writel(reg, hba->pericfg_base + REG_UFS_PERICFG);

    udelay(10);

    if (REG_UFS_PERICFG_LDO_N_BIT != 0xF) {
        reg = readl(hba->pericfg_base + REG_UFS_PERICFG);
        reg = reg | (1 << REG_UFS_PERICFG_LDO_N_BIT);
        writel(reg, hba->pericfg_base + REG_UFS_PERICFG);
        udelay(10);
    }

    if (REG_UFS_PERICFG_RST_N_BIT != 0xF) {
        reg = readl(hba->pericfg_base + REG_UFS_PERICFG);
        reg = reg | (1 << REG_UFS_PERICFG_RST_N_BIT);
        writel(reg, hba->pericfg_base + REG_UFS_PERICFG);
        udelay(10);
    }

    mdelay(1);

    return 0;

#else

    return 0;

#endif
}

static int ufs_pre_link(struct ufs_hba *hba)
{
    int ret = 0;
    u32 tmp;

    struct uic_command dme_setting_before_link[] = {
        UIC_CMD_DME_SET(PA_LOCALTXLCCENABLE, 0, 0x0), /* set LCC_Enable to 0 */
    };

    ufs_bootrom_deputy(hba);

    ufs_init_mphy();

    if (hba->hci_quirks & UFSHCD_QUIRK_MTK_MPHY_TESTCHIP) {

        /* enable UIC related interrupts */
        ufshcd_enable_intr(hba, UIC_COMMAND_COMPL);

        /* apply setting before link startup */
        ret = ufshcd_uic_cmd_run(hba, dme_setting_before_link,
                                 countof(dme_setting_before_link));

        if (ret)
            LTRACEF("dme_setting_before_link fail\n");

        hba->dme_get(hba, UIC_ARG_MIB(VENDOR_AUTOBURSTENDCTRL), &tmp);
        tmp = ((1<<8) | 0x3c);
        hba->dme_set(hba, UIC_ARG_MIB(VENDOR_AUTOBURSTENDCTRL), tmp);
    }

    /* disable deep stall by default */
    hba->dme_get(hba, UIC_ARG_MIB(VENDOR_SAVEPOWERCONTROL), &tmp);
    tmp &= ~(1 << 6);
    hba->dme_set(hba, UIC_ARG_MIB(VENDOR_SAVEPOWERCONTROL), tmp);

    /* disable scrambling by default */
    hba->dme_set(hba, UIC_ARG_MIB(PA_SCRAMBLING), 0);

    /* disable unipro clock gating feature by default */
    ufs_enable_unipro_cg(hba, false);

    return ret;
}

static int ufs_post_link(struct ufs_hba *hba)
{
    int ret = 0;
    u32 tmp;

    struct uic_command dme_setting_after_link[] = {
        UIC_CMD_DME_PEER_SET(PA_LOCALTXLCCENABLE, 0, 0x0), /* device LCC_Enable to 0 */
    };

    if (hba->hci_quirks & UFSHCD_QUIRK_MTK_MPHY_TESTCHIP) {

        /* apply setting after link startup */
        ret = ufshcd_uic_cmd_run(hba, dme_setting_after_link,
                                 countof(dme_setting_after_link));

        if (ret)
            LTRACEF("dme_setting_after_link fail\n");

        hba->dme_get(hba, UIC_ARG_MIB(VENDOR_AUTOBURSTENDCTRL), &tmp);
        tmp &= ~(1<<8);
        hba->dme_set(hba, UIC_ARG_MIB(VENDOR_AUTOBURSTENDCTRL), tmp);
    }

    return ret;

}

/**
 * ufshcd_link_startup - Initialize unipro link startup
 * @hba: per adapter instance
 *
 * Returns 0 for success, non-zero in case of failure
 */
static int ufshcd_link_startup(struct ufs_hba *hba)
{
    int ret;
    int retries = DME_LINKSTARTUP_RETRIES;

    do {
        ufs_pre_link(hba);

        ret = ufshcd_dme_link_startup(hba);

        if (ret) {
            dprintf(CRITICAL, "link: UIC command fail\n");
            goto retry;
        }

        /* check if device is detected by inter-connect layer */
        if (!ufshcd_is_device_present(hba)) {
            dprintf(CRITICAL, "link: Device not present\n");
            ret = -1;
            /* directly go through to retry section */
        }
retry:
        /*
         * DME link lost indication is only received when link is up,
         * but we can't be sure if the link is up until link startup
         * succeeds. So reset the local Uni-Pro and try again.
         */
        if (ret) {
            dprintf(CRITICAL, "link startup fail, retrying the %d times ...\n",
                DME_LINKSTARTUP_RETRIES - retries + 1);

            if (ufshcd_hba_enable(hba)) {
                /* critical error: controller is not working now, keep ret and goto out */
                goto out;
            }
        }
    } while (ret && retries--);

    if (ret)
        /* failed to get the link up... retired */
        goto out;

    /* Include any host controller configuration via UIC commands */
    ret = ufs_post_link(hba);

    if (ret)
        goto out;

    ret = ufshcd_make_hba_operational(hba);
out:
    if (ret)
        dprintf(CRITICAL, "link startup failed %d\n", ret);
    return ret;
}

/**
 * ufshcd_init_pwr_info - setting the POR (power on reset)
 * values in hba power info
 * @hba: per-adapter instance
 */
static void ufshcd_init_pwr_info(struct ufs_hba *hba)
{
    hba->pwr_info.gear_rx = UFS_PWM_G1;
    hba->pwr_info.gear_tx = UFS_PWM_G1;
    hba->pwr_info.lane_rx = 1;
    hba->pwr_info.lane_tx = 1;
    hba->pwr_info.pwr_rx = SLOW_MODE;
    hba->pwr_info.pwr_tx = SLOW_MODE;
    hba->pwr_info.hs_rate = 1;
}

/**
 * ufshcd_get_dev_cmd_tag - Get device management command tag
 * @hba: per-adapter instance
 * @tag: pointer to variable with available slot value
 *
 * Get a free slot and lock it until device management command
 * completes.
 *
 * Returns false if free slot is unavailable for locking, else
 * return true with tag value in @tag.
 */
bool ufshcd_get_free_tag(struct ufs_hba *hba, int *tag_out)
{
    unsigned int flags;
    lk_bigtime_t start_tick;
    /* get timeout tick */
    start_tick = current_time_hires();

    if (!tag_out)
        return false;

retry:
    spin_lock_irqsave(&hba->free_tag_lock, flags);
    if (hba->lrb_in_use & (1 << hba->active_tr_tag)) {
        spin_unlock_irqrestore(&hba->free_tag_lock, flags);
        if ((current_time_hires() - start_tick) > UFS_GETTAG_TIMEOUT_US) {
            dprintf(INFO, "[UFS] %s fail\n", __func__);
            return false;
        }
        goto retry;
    }
    hba->lrb_in_use |= (1 << hba->active_tr_tag);
    *tag_out = hba->active_tr_tag;
    spin_unlock_irqrestore(&hba->free_tag_lock, flags);
    return true;
}

void ufshcd_put_tag(struct ufs_hba *hba, int tag)
{
    /* clear_bit(tag, &hba->lrb_in_use); */
    unsigned int flags;

    spin_lock_irqsave(&hba->free_tag_lock, flags);
    hba->lrb_in_use &= ~(1 << tag);
    spin_unlock_irqrestore(&hba->free_tag_lock, flags);
}

static void ufshcd_prepare_req_desc_hdr(struct ufs_hba *hba,
                                        struct ufshcd_lrb *lrbp,
                                        u32 *upiu_flags, enum dma_data_direction cmd_dir)
{
    struct utp_transfer_req_desc *req_desc = lrbp->utr_descriptor_ptr;
    u32 dword_0;

    if (cmd_dir == DMA_FROM_DEVICE) {
        dword_0 = UTP_DEVICE_TO_HOST;
        *upiu_flags = UPIU_CMD_FLAGS_READ;
    } else if (cmd_dir == DMA_TO_DEVICE) {
        dword_0 = UTP_HOST_TO_DEVICE;
        *upiu_flags = UPIU_CMD_FLAGS_WRITE;
    } else {
        dword_0 = UTP_NO_DATA_TRANSFER;
        *upiu_flags = UPIU_CMD_FLAGS_NONE;
    }

    dword_0 |= 1 << UPIU_COMMAND_TYPE_OFFSET;

    if (lrbp->intr_cmd)
        dword_0 |= UTP_REQ_DESC_INT_CMD;

    req_desc->header.dword_2 = (u32)(OCS_INVALID_COMMAND_STATUS);
    req_desc->header.dword_0 = (u32)(dword_0);
}


static void ufshcd_prepare_utp_scsi_cmd_upiu(struct ufshcd_lrb *lrbp, u32 upiu_flags)
{
    struct utp_upiu_req *ucd_req = lrbp->ucd_req_ptr;

    ucd_req->sc.exp_data_transfer_len = SWAP_32(lrbp->cmd->exp_len);

    ucd_req->header.dword_2 = 0;

    ucd_req->header.dword_1 = UPIU_HEADER_DWORD(
                                  UPIU_COMMAND_SET_TYPE_SCSI, 0, 0, 0);
    ucd_req->header.dword_0 = UPIU_HEADER_DWORD(
                                  UPIU_TRANSACTION_COMMAND, upiu_flags,
                                  lrbp->lun, lrbp->task_tag);

    memcpy(ucd_req->sc.cdb, lrbp->cmd->cmd_data,
        (MIN(lrbp->cmd->cmd_len, MAX_CDB_SIZE)));
}

static void ufshcd_prepare_utp_query_req_upiu(struct ufs_hba *hba,
        struct ufshcd_lrb *lrbp, u32 upiu_flags)
{
    u8 *descp = (u8 *)lrbp->ucd_req_ptr + GENERAL_UPIU_REQUEST_SIZE;
    struct ufs_query *query = &hba->dev_cmd.query;
    u16 len = SWAP_16(query->request.upiu_req.length);
    struct utp_upiu_req *ucd_req = lrbp->ucd_req_ptr;

    ucd_req->header.dword_2 = UPIU_HEADER_DWORD(
                                  0, 0, len >> 8, (u8)len);

    ucd_req->header.dword_1 = UPIU_HEADER_DWORD(
                                  0, query->request.query_func, 0, 0);

    ucd_req->header.dword_0 = UPIU_HEADER_DWORD(
                                  UPIU_TRANSACTION_QUERY_REQ, upiu_flags,
                                  lrbp->lun, lrbp->task_tag);

    /* Copy the Query Request buffer as is */
    memcpy(&ucd_req->qr, &query->request.upiu_req, QUERY_OSF_SIZE);

    /* Copy the Descriptor */
    if (query->request.upiu_req.opcode == UPIU_QUERY_OPCODE_WRITE_DESC) {
        memcpy(descp, query->descriptor, len);
        /*
         * ufshcd_send_command() only flushes length of sizeof(struct utp_upiu_req).
         * For Write Descriptor, we may need to flush more data range.
         */
        ufs_dma_map((unsigned long)descp, len, DMA_TO_DEVICE);
    }

}

static int ufshcd_compose_upiu(struct ufs_hba *hba, struct ufshcd_lrb *lrbp)
{
    u32 upiu_flags;
    int ret = 0;

    switch (lrbp->command_type) {
    case UTP_CMD_TYPE_SCSI:
        if (lrbp->cmd) {
            /* prepare transfer request descriptor */
            ufshcd_prepare_req_desc_hdr(hba, lrbp, &upiu_flags, lrbp->cmd->dir);

            /* prepare COMMAND UPIU */
            ufshcd_prepare_utp_scsi_cmd_upiu(lrbp, upiu_flags);
        } else {
            ret = -1;
        }
        break;
    case UTP_CMD_TYPE_DEV_MANAGE:
        ufshcd_prepare_req_desc_hdr(hba, lrbp, &upiu_flags, DMA_NONE);

        if (hba->dev_cmd.type == DEV_CMD_TYPE_QUERY)
            ufshcd_prepare_utp_query_req_upiu(hba, lrbp, upiu_flags);
        else if (hba->dev_cmd.type == DEV_CMD_TYPE_NOP) {
            memset(lrbp->ucd_req_ptr, 0, sizeof(struct utp_upiu_req));
            lrbp->ucd_req_ptr->header.dword_0 = UPIU_HEADER_DWORD(UPIU_TRANSACTION_NOP_OUT,
                0, 0, lrbp->task_tag);

        } else
            ret = -1;

        break;
    default:
        ret = -1;
        LTRACEF("unknown command type: 0x%x\n", lrbp->command_type);
        break;
    } /* end of switch */

    return ret;
}

static int ufshcd_compose_dev_cmd(struct ufs_hba *hba,
                                  struct ufshcd_lrb *lrbp, enum dev_cmd_type cmd_type, int tag)
{
    lrbp->cmd = NULL;
    lrbp->task_tag = tag;
    lrbp->lun = 0; /* device management cmd is not specific to any LUN */
    lrbp->command_type = UTP_CMD_TYPE_DEV_MANAGE;
    lrbp->intr_cmd = true; /* No interrupt aggregation */
    hba->dev_cmd.type = cmd_type;

    return ufshcd_compose_upiu(hba, lrbp);
}

/**
 * ufshcd_send_command - Send SCSI or device management commands
 * @hba: per adapter instance
 * @task_tag: Task tag of the command
 */
static inline void ufshcd_send_command(struct ufs_hba *hba, unsigned int task_tag)
{
    u32 intr_status;

    /* set_bit(task_tag, &hba->outstanding_reqs); */
    hba->outstanding_reqs |= (1 << task_tag);

    /* flush COMMAND UPIU */
    ufs_dma_map((unsigned long)hba->lrb->ucd_req_ptr, sizeof(struct utp_upiu_req),
        DMA_TO_DEVICE);

    /* invalidate RESPONSE UPIU */
    ufs_dma_map((unsigned long)hba->lrb->ucd_rsp_ptr, sizeof(struct utp_upiu_rsp),
        DMA_FROM_DEVICE);

    /* flush PRDT */
    ufs_dma_map((unsigned long)hba->lrb->ucd_prdt_ptr,
        sizeof(struct ufs_sg_entry) * UFS_MAX_SG_SEGMENTS, DMA_TO_DEVICE);

    /* invalidate transfer request descriptor (UTRD) */
    ufs_dma_map((unsigned long)hba->lrb->utr_descriptor_ptr,
        sizeof(struct utp_transfer_req_desc), DMA_BIDIRECTIONAL);

    /* clear interrupt status */
    intr_status = ufs_readl(hba, REG_INTERRUPT_STATUS);

    if (intr_status)
        ufs_writel(hba, intr_status, REG_INTERRUPT_STATUS);

    ufshcd_writel(hba, 1 << task_tag, REG_UTP_TRANSFER_REQ_DOOR_BELL);
}

/**
 * ufshcd_exec_dev_cmd - API for sending device management requests
 * @hba - UFS hba
 * @cmd_type - specifies the type (NOP, Query...)
 * @timeout - time in seconds
 *
 * NOTE: Since there is only one available tag for device management commands,
 * it is expected you hold the hba->dev_cmd.lock mutex.
 */
static int ufshcd_exec_dev_cmd(struct ufs_hba *hba,
                               enum dev_cmd_type cmd_type, u32 timeout)
{
    struct ufshcd_lrb *lrbp;
    int err;
    int tag;

    if (!ufshcd_get_free_tag(hba, &tag))
        return -1;

    lrbp = &hba->lrb[0];

    err = ufshcd_compose_dev_cmd(hba, lrbp, cmd_type, tag);

    if (err)
        goto out_put_tag;

    ufshcd_send_command(hba, tag);

    err = ufshcd_wait_command(hba, lrbp, timeout);

out_put_tag:

    ufshcd_put_tag(hba, tag);

    return err;
}

static inline void ufshcd_init_query(struct ufs_hba *hba,
    struct ufs_query_req **request, struct ufs_query_res **response,
    enum query_opcode opcode, u8 idn, u8 index, u8 selector)
{
    *request = &hba->dev_cmd.query.request;
    *response = &hba->dev_cmd.query.response;
    memset(*request, 0, sizeof(struct ufs_query_req));
    memset(*response, 0, sizeof(struct ufs_query_res));
    (*request)->upiu_req.opcode = opcode;
    (*request)->upiu_req.idn = idn;
    (*request)->upiu_req.index = index;
    (*request)->upiu_req.selector = selector;
}

/**
 * ufshcd_complete_dev_init() - checks device readiness
 * hba: per-adapter instance
 *
 * Set fDeviceInit flag and poll until device toggles it.
 */
static int ufshcd_complete_dev_init(struct ufs_hba *hba)
{
    int retries, err = 0;
    bool flag_res = 1;
    lk_bigtime_t start_tick;

    for (retries = QUERY_REQ_RETRIES; retries > 0; retries--) {

        /* Set the fDeviceInit flag */
        err = hba->query_flag(hba, UPIU_QUERY_OPCODE_SET_FLAG, QUERY_FLAG_IDN_FDEVICEINIT, NULL);

        if (!err)
            break;

        LTRACEF("error %d retrying\n", err);
    }
    if (err) {
        LTRACEF("setting fDeviceInit flag failed with error %d\n", err);
        goto out;
    }
    // get timeout tick
    start_tick = current_time_hires();

    /* poll for max. UFS_FDEVICEINIT_TIMEOUT_US(5s) for fDeviceInit flag to clear */
    do {
        for (retries = QUERY_REQ_RETRIES; retries > 0; retries--) {
            err = hba->query_flag(hba, UPIU_QUERY_OPCODE_READ_FLAG,
                QUERY_FLAG_IDN_FDEVICEINIT, &flag_res);

            if (!err)
                break;

            LTRACEF("error %d retrying\n", err);
        }
    } while (!err && flag_res &&
        ((current_time_hires() - start_tick) < UFS_FDEVICEINIT_TIMEOUT_US));

    if (err)
        LTRACEF("reading fDeviceInit flag failed with error %d\n", err);
    else if (flag_res) {
        LTRACEF("fDeviceInit was not cleared by the device\n");
        err = UFS_ERR_FDEVICEINIT_NOT_CLR;
    }

out:
    return err;
}

static int dump_unipro_params(struct ufs_hba *hba)
{
    int ret = 0;
    u32 granularity, peer_granularity;
    u32 pa_tactivate, peer_pa_tactivate;
    u32 pa_h8time, peer_pa_h8time = 0;

    ret += hba->dme_get(hba, UIC_ARG_MIB(PA_GRANULARITY), &granularity);
    ret += hba->dme_peer_get(hba, UIC_ARG_MIB(PA_GRANULARITY), &peer_granularity);

    ret += hba->dme_get(hba, UIC_ARG_MIB(PA_TACTIVATE), &pa_tactivate);
    ret += hba->dme_peer_get(hba, UIC_ARG_MIB(PA_TACTIVATE), &peer_pa_tactivate);

    ret += hba->dme_get(hba, UIC_ARG_MIB(PA_HIBERN8TIME), &pa_h8time);
    ret += hba->dme_peer_get(hba, UIC_ARG_MIB(PA_HIBERN8TIME), &peer_pa_h8time);

    dprintf(CRITICAL, "[UFS] unipro_params: granularity=0x%x\n", granularity);
    dprintf(CRITICAL, "[UFS] unipro_params: peer_granularity=0x%x\n", peer_granularity);
    dprintf(CRITICAL, "[UFS] unipro_params: pa_tactivate=0x%x\n", pa_tactivate);
    dprintf(CRITICAL, "[UFS] unipro_params: peer_pa_tactivate=0x%x\n", peer_pa_tactivate);
    dprintf(CRITICAL, "[UFS] unipro_params: pa_h8time=0x%x\n", pa_h8time);
    dprintf(CRITICAL, "[UFS] unipro_params: peer_pa_h8time=0x%x\n", peer_pa_h8time);

    if (ret)
        dprintf(CRITICAL, "unipro_params: %s: dump with error (%d)\n", __func__, ret);
    return ret;
}

static int ufshcd_quirk_tune_host_pa_tactivate(struct ufs_hba *hba)
{
    int ret = 0;
    u32 granularity, peer_granularity;
    u32 pa_tactivate, peer_pa_tactivate;
    u32 pa_tactivate_us, peer_pa_tactivate_us;
    u8 gran_to_us_table[] = {1, 4, 8, 16, 32, 100};

    ret = hba->dme_get(hba, UIC_ARG_MIB(PA_GRANULARITY), &granularity);
    if (ret)
        goto out;

    ret = hba->dme_peer_get(hba, UIC_ARG_MIB(PA_GRANULARITY), &peer_granularity);
    if (ret)
        goto out;

    if ((granularity < PA_GRANULARITY_MIN_VAL) ||
        (granularity > PA_GRANULARITY_MAX_VAL)) {
        dprintf(CRITICAL, "%s: invalid host PA_GRANULARITY %d\n",
            __func__, granularity);
        return -1;
    }

    if ((peer_granularity < PA_GRANULARITY_MIN_VAL) ||
        (peer_granularity > PA_GRANULARITY_MAX_VAL)) {
        dprintf(CRITICAL, "%s: invalid device PA_GRANULARITY %d\n",
            __func__, peer_granularity);
        return -1;
    }

    ret = hba->dme_get(hba, UIC_ARG_MIB(PA_TACTIVATE), &pa_tactivate);
    if (ret)
        goto out;

    ret = hba->dme_peer_get(hba, UIC_ARG_MIB(PA_TACTIVATE), &peer_pa_tactivate);
    if (ret)
        goto out;

    pa_tactivate_us = pa_tactivate * gran_to_us_table[granularity - 1];
    peer_pa_tactivate_us = peer_pa_tactivate *
                 gran_to_us_table[peer_granularity - 1];

    if (pa_tactivate_us > peer_pa_tactivate_us) {
        u32 new_peer_pa_tactivate;

        new_peer_pa_tactivate = pa_tactivate_us /
                      gran_to_us_table[peer_granularity - 1];

        new_peer_pa_tactivate++;
        ret = hba->dme_peer_set(hba, UIC_ARG_MIB(PA_TACTIVATE),
                      new_peer_pa_tactivate);
    }

out:
    return ret;
}

/* tune unipro pa parapmer for hibern8 */
static void ufshcd_tune_unipro_params(struct ufs_hba *hba)
{
    if (hba->dev_info.wmanufacturerid == UFS_VENDOR_SAMSUNG) {
        hba->dme_set(hba, UIC_ARG_MIB(PA_TACTIVATE), 6);
        hba->dme_set(hba, UIC_ARG_MIB(PA_HIBERN8TIME), 10);
    }
    if (hba->dev_quirks & UFS_DEVICE_QUIRK_PA_TACTIVATE)
        /* set 1ms timeout for PA_TACTIVATE */
        hba->dme_set(hba, UIC_ARG_MIB(PA_TACTIVATE), 10);

    if (hba->dev_quirks & UFS_DEVICE_QUIRK_HOST_PA_TACTIVATE)
        ufshcd_quirk_tune_host_pa_tactivate(hba);

    dump_unipro_params(hba);
}

int ufshcd_dme_get_attr(struct ufs_hba *hba, u32 attr_sel, u32 *mib_val, u8 peer)
{
    int ret;
    struct uic_command uic_cmd = {0};

    uic_cmd.argument1 = attr_sel;
    uic_cmd.command = peer ? UIC_CMD_DME_PEER_GET : UIC_CMD_DME_GET;

    ret = ufshcd_send_uic_cmd(hba, &uic_cmd);

    if (mib_val && !ret)
        *mib_val = uic_cmd.argument3;

    if (ret)
        LTRACEF("attr-id 0x%x error code %d\n", UIC_GET_ATTR_ID(attr_sel), ret);

    return ret;
}

/**
 * ufshcd_dme_set_attr - UIC command for DME_SET, DME_PEER_SET
 * @hba: per adapter instance
 * @attr_sel: uic command argument1
 * @attr_set: attribute set type as uic command argument2
 * @mib_val: setting value as uic command argument3
 * @peer: indicate whether peer or local
 *
 * Returns 0 on success, non-zero value on failure
 */
int ufshcd_dme_set_attr(struct ufs_hba *hba, u32 attr_sel, u8 attr_set, u32 mib_val, u8 peer)
{
    struct uic_command uic_cmd = {0};
    int ret;

    uic_cmd.command = peer ? UIC_CMD_DME_PEER_SET : UIC_CMD_DME_SET;
    uic_cmd.argument1 = attr_sel;
    uic_cmd.argument2 = UIC_ARG_ATTR_TYPE(attr_set);
    uic_cmd.argument3 = mib_val;

    ret = ufshcd_send_uic_cmd(hba, &uic_cmd);

    if (ret)
        dprintf(CRITICAL, "[UFS] err: attr-id 0x%x val 0x%x error code %d (peer %d)\n",
            UIC_GET_ATTR_ID(attr_sel), mib_val, ret, peer);

    return ret;
}

/**
 * ufshcd_get_max_pwr_mode - reads the max power mode negotiated with device
 * @hba: per-adapter instance
 */
static int ufshcd_get_max_pwr_mode(struct ufs_hba *hba)
{
    struct ufs_pa_layer_attr *pwr_info = &hba->max_pwr_info.info;

    if (hba->max_pwr_info.is_valid)
        return 0;

    pwr_info->pwr_tx = FAST_MODE;
    pwr_info->pwr_rx = FAST_MODE;
    pwr_info->hs_rate = PA_HS_MODE_B;

    /* Get the connected lane count */
    hba->dme_get(hba, UIC_ARG_MIB(PA_CONNECTEDRXDATALANES),
                 &pwr_info->lane_rx);
    /* mtk debug */
    LTRACEF("[UFS] info: %s, lane_rx: %d\n", __func__, pwr_info->lane_rx);

    hba->dme_get(hba, UIC_ARG_MIB(PA_CONNECTEDTXDATALANES),
                 &pwr_info->lane_tx);
    /* mtk debug */
    LTRACEF("[UFS] info: %s, lane_tx: %d\n", __func__, pwr_info->lane_tx);

    if (!pwr_info->lane_rx || !pwr_info->lane_tx) {
        LTRACEF("invalid connected lanes value. rx=%d, tx=%d\n",
                     pwr_info->lane_rx,
                     pwr_info->lane_tx);
        return -1;
    }

    /*
     * First, get the maximum gears of HS speed.
     * If a zero value, it means there is no HSGEAR capability.
     * Then, get the maximum gears of PWM speed.
     */
    hba->dme_get(hba, UIC_ARG_MIB(PA_MAXRXHSGEAR), &pwr_info->gear_rx);

    if (!pwr_info->gear_rx) {
        hba->dme_get(hba, UIC_ARG_MIB(PA_MAXRXPWMGEAR), &pwr_info->gear_rx);
        if (!pwr_info->gear_rx) {
            LTRACEF("invalid max pwm rx gear read = %d\n", pwr_info->gear_rx);
            return -1;
        }
        pwr_info->pwr_rx = SLOWAUTO_MODE;
    } else
        /* mtk debug */
        LTRACEF("[UFS] info: %s, gear_rx: %d\n", __func__, pwr_info->gear_rx);
    hba->dme_peer_get(hba, UIC_ARG_MIB(PA_MAXRXHSGEAR),
                      &pwr_info->gear_tx);

    if (!pwr_info->gear_tx) {
        hba->dme_peer_get(hba, UIC_ARG_MIB(PA_MAXRXPWMGEAR),
                          &pwr_info->gear_tx);
        if (!pwr_info->gear_tx) {
            LTRACEF("invalid max pwm tx gear read = %d\n", pwr_info->gear_tx);
            return -1;
        }
        pwr_info->pwr_tx = SLOWAUTO_MODE;
    } else
        /* mtk debug */
        LTRACEF("[UFS] info: %s, gear_tx: %d\n", __func__, pwr_info->gear_tx);

    hba->max_pwr_info.is_valid = true;

    return 0;
}

/**
 * ufshcd_get_upmcrs - Get the power mode change request status
 * @hba: Pointer to adapter instance
 *
 * This function gets the UPMCRS field of HCS register
 * Returns value of UPMCRS field
 */
static inline u8 ufshcd_get_upmcrs(struct ufs_hba *hba)
{
    return (ufshcd_readl(hba, REG_CONTROLLER_STATUS) >> 8) & 0x7;
}

static int ufshcd_uic_pwr_ctrl(struct ufs_hba *hba, struct uic_command *cmd)
{
    u8 status;
    int ret;

    ret = __ufshcd_send_uic_cmd(hba, cmd);

    if (ret) {
        LTRACEF("pwr ctrl cmd 0x%x with mode 0x%x uic error %d\n", cmd->command,
            cmd->argument3, ret);
        goto out;
    }

    ret = ufshcd_wait_for_uic_cmd(hba, cmd);

    if (ret) {
        LTRACEF("pwr ctrl cmd 0x%x with mode 0x%x uic error %d\n", cmd->command,
            cmd->argument3, ret);
        goto out;
    }

    ret = ufshcd_wait_for_register(hba, REG_INTERRUPT_STATUS,
                                   UIC_POWER_MODE, UIC_POWER_MODE,
                                   100, UIC_CMD_TIMEOUT);
    if (ret) {
        dprintf(CRITICAL, "[UFS] err: wait UIC_POWER_MODE interrupt timeout\n");
        goto out;
    }

    ufshcd_writel(hba, UIC_POWER_MODE, REG_INTERRUPT_STATUS);

    status = ufshcd_get_upmcrs(hba);

    if (status != PWR_LOCAL) {
        dprintf(CRITICAL, "[UFS] err: pwr ctrl cmd 0x%0x failed, host umpcrs:0x%x\n",
            cmd->command, status);
        ret = (status != PWR_OK) ? status : -1;
    }

out:
    return ret;
}

static int ufshcd_uic_change_pwr_mode(struct ufs_hba *hba, u8 mode)
{
    int ret;
    struct uic_command uic_cmd = {0};

    uic_cmd.argument1 = UIC_ARG_MIB(PA_PWRMODE);
    uic_cmd.command = UIC_CMD_DME_SET;
    uic_cmd.argument3 = mode;
    ret = ufshcd_uic_pwr_ctrl(hba, &uic_cmd);

    return ret;
}

static int ufshcd_dme_copy_attr(struct ufs_hba *hba,
                                u32 attr_sel_dst, u8 peer_dst,
                                u32 attr_sel_src, u8 peer_src)
{
    int ret;
    u32 mib_val;

    ret = ufshcd_dme_get_attr(hba, attr_sel_src, &mib_val, peer_src);
    if (ret)
        return ret;

    return ufshcd_dme_set_attr(hba, attr_sel_dst, ATTR_SET_NOR,
                               mib_val, peer_dst);
}

static int ufshcd_setup_userdata(struct ufs_hba *hba)
{
    /* Set PAPowerModeUserData[0~5] */
    hba->dme_set(hba, UIC_ARG_MIB(PA_PWRMODEUSERDATA0), 0x1fff);
    hba->dme_set(hba, UIC_ARG_MIB(PA_PWRMODEUSERDATA1), 0xffff);
    hba->dme_set(hba, UIC_ARG_MIB(PA_PWRMODEUSERDATA2), 0x7fff);
    hba->dme_set(hba, UIC_ARG_MIB(PA_PWRMODEUSERDATA3), 0x1fff);
    hba->dme_set(hba, UIC_ARG_MIB(PA_PWRMODEUSERDATA4), 0xffff);
    hba->dme_set(hba, UIC_ARG_MIB(PA_PWRMODEUSERDATA5), 0x7fff);

    return 0;
}

static u32 get_termination(u32 pwr_mode)
{
    switch (pwr_mode) {
    case FASTAUTO_MODE:
    case FAST_MODE:
        return 1;
    case SLOWAUTO_MODE:
    case SLOW_MODE:
        return 0;
    }

    return 0;
}
static int pwr_mode_change(struct ufs_hba *hba,
                           u32 tx_pwr_mode, u32 rx_pwr_mode,
                           u32 tx_gear, u32 rx_gear,
                           u32 tx_lanes, u32 rx_lanes, u32 hs_series)
{
    u32 tx_term = get_termination(tx_pwr_mode);
    u32 rx_term = get_termination(rx_pwr_mode);
    struct uic_command cmds[7];
    int ret;

    ret = ufshcd_dme_copy_attr(hba,
                               UIC_ARG_MIB(PA_TXTRAILINGCLOCKS), DME_LOCAL,
                               UIC_ARG_MIB(PA_MINRXTRAILINGCLOCKS), DME_PEER);
    if (ret)
        return ret;

    ret = ufshcd_dme_copy_attr(hba,
                               UIC_ARG_MIB(PA_TXTRAILINGCLOCKS), DME_PEER,
                               UIC_ARG_MIB(PA_MINRXTRAILINGCLOCKS), DME_LOCAL);
    if (ret)
        return ret;

    cmds[0] = UIC_CMD_DME_SET(PA_TXTERMINATION, 0, tx_term);
    cmds[1] = UIC_CMD_DME_SET(PA_RXTERMINATION, 0, rx_term);

    cmds[2] = UIC_CMD_DME_SET(PA_TXGEAR, 0, tx_gear);
    cmds[3] = UIC_CMD_DME_SET(PA_RXGEAR, 0, rx_gear);

    cmds[4] = UIC_CMD_DME_SET(PA_ACTIVETXDATALANES, 0, tx_lanes);
    cmds[5] = UIC_CMD_DME_SET(PA_ACTIVERXDATALANES, 0, rx_lanes);

    cmds[6] = UIC_CMD_DME_SET(PA_HSSERIES, 0, hs_series);

    return ufshcd_uic_cmd_run(hba, cmds, countof(cmds));
}

int ufshcd_legacy_pwr_mode_change(struct ufs_hba *hba, u32 tx_pwr_mode,
                                  u32 rx_pwr_mode, u32 tx_gear, u32 rx_gear,
                                  u32 hs_series, u32 tx_lanes, u32 rx_lanes)
{
    int err;

    err = ufshcd_setup_userdata(hba);
    if (err)
        return err;

    err = pwr_mode_change(hba, tx_pwr_mode, rx_pwr_mode,
                          tx_gear, rx_gear, tx_lanes, rx_lanes, hs_series);
    if (err)
        return err;

    err = ufshcd_uic_change_pwr_mode(hba, rx_pwr_mode << 4 | tx_pwr_mode);

    return err;
}

static int ufs_pre_pwr_change(struct ufs_hba *hba, struct ufs_pa_layer_attr *desired,
    struct ufs_pa_layer_attr *final)
{
    if (hba->dev_info.wmanufacturerid == 0) {
        LTRACEF("bug: manufacturer_id shall not be 0");
        return -1;
    }

    /* fill-in desired gear for different environment */
    /* make sure device capability */
    final->pwr_rx = UFS_DEV_DEFAULT_PWR_RX;
    final->pwr_tx = UFS_DEV_DEFAULT_PWR_TX;
    final->hs_rate = UFS_DEV_DEFAULT_HS_RATE;

#ifndef UFS_CFG_SAFE_BRING_UP
    if (final->pwr_rx == SLOW_MODE || final->pwr_rx == SLOWAUTO_MODE) {
        final->gear_rx = MIN(UFS_DEV_MAX_GEAR_RX, 4);
        final->gear_tx = MIN(UFS_DEV_MAX_GEAR_TX, 4);
    } else {
        final->gear_rx = MIN(UFS_DEV_MAX_GEAR_RX, desired->gear_rx);
        final->gear_tx = MIN(UFS_DEV_MAX_GEAR_TX, desired->gear_tx);

        final->gear_rx = MIN(final->gear_rx, final->gear_tx);
        final->gear_tx = final->gear_rx;
        if ((final->gear_rx == UFS_HS_G4) && (final->gear_tx == UFS_HS_G4)) {
            /* INITIAL ADAPT */
            hba->dme_set(hba, UIC_ARG_MIB(PA_TXHSADAPTTYPE), PA_INITIAL_ADAPT);
        } else {
            /* NO ADAPT */
            hba->dme_set(hba, UIC_ARG_MIB(PA_TXHSADAPTTYPE), PA_NO_ADAPT);
        }
        dprintf(ALWAYS, "[UFS] info: RX HS-G%d, setting is %d\n",
            final->gear_rx, UFS_DEV_MAX_GEAR_RX);
        dprintf(ALWAYS, "[UFS] info: TX HS-G%d, setting is %d\n",
            final->gear_tx, UFS_DEV_MAX_GEAR_TX);
    }

    final->lane_rx = MIN(UFS_DEV_MAX_LANE_RX, desired->lane_rx);
    final->lane_tx = MIN(UFS_DEV_MAX_LANE_TX, desired->lane_tx);
#else
    final->gear_rx = UFS_DEV_MAX_GEAR_RX;
    final->gear_tx = UFS_DEV_MAX_GEAR_TX;
    final->lane_rx = UFS_DEV_MAX_LANE_RX;
    final->lane_tx = UFS_DEV_MAX_LANE_TX;
#endif

    if (final->pwr_rx == SLOW_MODE || final->pwr_rx == SLOWAUTO_MODE)
        dprintf(ALWAYS, "[UFS] info: PWM-G%d, %d lane\n",
                final->gear_rx, final->lane_rx);
    else
        dprintf(ALWAYS, "[UFS] info: HS-G%d-%d, %d lane\n",
                final->gear_rx, final->hs_rate, final->lane_rx);

    return 0;
}

/**
 * ufshcd_config_pwr_mode - configure a new power mode
 * @hba: per-adapter instance
 * @desired_pwr_mode: desired power configuration
 */
int ufshcd_config_pwr_mode(struct ufs_hba *hba,
                           struct ufs_pa_layer_attr *desired_pwr_mode)
{
    struct ufs_pa_layer_attr final_params = { 0 };
    int ret;

    ufs_pre_pwr_change(hba, desired_pwr_mode, &final_params);

    ret = ufshcd_legacy_pwr_mode_change(hba,
                                        final_params.pwr_tx, final_params.pwr_rx,
                                        final_params.gear_tx, final_params.gear_rx,
                                        final_params.hs_rate,
                                        final_params.lane_tx, final_params.lane_rx);

    return ret;
}

int ufshcd_map_sg(struct ufs_hba *hba, struct ufshcd_lrb *lrbp)
{
    struct ufshcd_sg_entry *prd_table;
    struct ufs_scsi_cmd *cmd;
    int sg_size = UFS_MAX_SIZE_PER_SG_SEGMENT;
    int sg_segments;
    int i;
    u32 residual;
    paddr_t buf_dma_addr;

    cmd = lrbp->cmd;

    if (cmd->exp_len == 0)
        sg_segments = 0;
    else
        sg_segments = ((cmd->exp_len + sg_size - 1) / sg_size);

    if (sg_segments) {
        lrbp->utr_descriptor_ptr->prd_table_length = (u16)((u16) (sg_segments));

        prd_table = (struct ufshcd_sg_entry *)lrbp->ucd_prdt_ptr;

        buf_dma_addr = vaddr_to_paddr(cmd->data_buf);

        residual = cmd->exp_len;

        for (i = 0; i < sg_segments; i++) {
            /* size should be 32-bit aligned, this is forced by spec */
            if (residual > (u32)sg_size)
                prd_table[i].size = (u32)((u32)sg_size - 1);
            else
                prd_table[i].size = (u32)((u32)residual - 1);

            /* addr should be 32-bit aligned, this is forced by spec */
            prd_table[i].base_addr = (u32)(lower_32_bits(buf_dma_addr));
            prd_table[i].upper_addr = (u32)(upper_32_bits(buf_dma_addr));

            buf_dma_addr += sg_size;
            residual -= sg_size;
        }

        /* invalidate data buffer if existed */
        ufs_dma_map((addr_t)cmd->data_buf, cmd->exp_len, lrbp->cmd->dir);
    } else {
        lrbp->utr_descriptor_ptr->prd_table_length = 0;
    }

    return 0;
}

/**
 * ufshcd_get_tr_ocs - Get the UTRD Overall Command Status
 * @lrb: pointer to local command reference block
 *
 * This function is used to get the OCS field from UTRD
 * Returns the OCS field in the UTRD
 */
static inline int ufshcd_get_tr_ocs(struct ufshcd_lrb *lrbp)
{
    return (lrbp->utr_descriptor_ptr->header.dword_2) & MASK_OCS;
}

/**
 * ufshcd_get_rsp_upiu_result - Get the result from response UPIU
 * @ucd_rsp_ptr: pointer to response UPIU
 *
 * This function gets the response status and scsi_status from response UPIU
 * Returns the response result code.
 */
static inline int
ufshcd_get_rsp_upiu_result(struct utp_upiu_rsp *ucd_rsp_ptr)
{
    return SWAP_32(ucd_rsp_ptr->header.dword_1) & MASK_RSP_UPIU_RESULT;
}

static int ufshcd_copy_query_response(struct ufs_hba *hba, struct ufshcd_lrb *lrbp)
{
    struct ufs_query_res *query_res = &hba->dev_cmd.query.response;

    memcpy(&query_res->upiu_res, &lrbp->ucd_rsp_ptr->qr, QUERY_OSF_SIZE);

    /* Get the descriptor */
    if (lrbp->ucd_rsp_ptr->qr.opcode == UPIU_QUERY_OPCODE_READ_DESC) {
        u8 *descp = (u8 *)lrbp->ucd_rsp_ptr +
                    GENERAL_UPIU_REQUEST_SIZE;
        u16 resp_len;
        u16 buf_len;

        /* data segment length */
        resp_len = SWAP_32(lrbp->ucd_rsp_ptr->header.dword_2) &
                   MASK_QUERY_DATA_SEG_LEN;
        buf_len = SWAP_16(
                      hba->dev_cmd.query.request.upiu_req.length);
        if (buf_len >= resp_len) {
            /*
             * ufshcd_wait_command() only invalidates length of "strcut utp_upiu_rsp".
             * We need to invalidate more data range for Read Descriptor operation.
             */
            ufs_dma_unmap((unsigned long)descp, resp_len, DMA_FROM_DEVICE);
            memcpy(hba->dev_cmd.query.descriptor, descp, resp_len);
        } else {
            LTRACEF("Response size is bigger than buffer");
            return -1;
        }
    }

    return 0;
}

static inline int ufshcd_get_req_rsp(struct utp_upiu_rsp *ucd_rsp_ptr)
{
    return SWAP_32(ucd_rsp_ptr->header.dword_0) >> 24;
}

static int ufshcd_dev_cmd_completion(struct ufs_hba *hba, struct ufshcd_lrb *lrbp)
{
    int err = 0;
    int resp;

    resp = ufshcd_get_req_rsp(lrbp->ucd_rsp_ptr);

    switch (resp) {
    case UPIU_TRANSACTION_REJECT_UPIU:
        /* TODO: handle Reject UPIU Response */
        err = -1;
        LTRACEF("Reject UPIU not fully implemented\n");
        break;

    case UPIU_TRANSACTION_QUERY_RSP:
        hba->dev_cmd.query.response.response = ufshcd_get_rsp_upiu_result(lrbp->ucd_rsp_ptr)
            >> UPIU_RSP_CODE_OFFSET;
        err = hba->dev_cmd.query.response.response;
        if (!err)
            err = ufshcd_copy_query_response(hba, lrbp);
        break;

    case UPIU_TRANSACTION_NOP_IN:
        if (hba->dev_cmd.type != DEV_CMD_TYPE_NOP) {
            err = -1;
            LTRACEF("unexpected response %x\n", resp);
        }
        break;

    default:
        err = -1;
        LTRACEF("Invalid device management cmd response: %x\n", resp);
        break;
    }

    return err;
}

int ufshcd_wait_command(struct ufs_hba *hba, struct ufshcd_lrb *lrbp, u32 timeout_ms)
{
    u8 ocs;
    u16 resp;
    int ret = UFS_ERR_NONE;
    u32 value;
    u32 tag_mask = 1 << lrbp->task_tag;
    u32 tr_doorbell;
    u32 intr_status;
    unsigned long elapsed_ms = 0;

    if (hba->drv_status & UFS_DRV_STS_DMA_WAIT_INTERRUPT) {
        /* for msdc irq support */
        /* Unmask interrupt now and wait */
        unmask_interrupt(hba->irq);
        ret = event_wait_timeout(&ufshcd_intr_event, timeout_ms);
        if (ret != 0)
            LTRACEF("[UFS]: failed to get event timeout:%d\n", timeout_ms);
        /* Mask interrupt */
        mask_interrupt(hba->irq);
    } else
        do {
            tr_doorbell = ufshcd_readl(hba, REG_UTP_TRANSFER_REQ_DOOR_BELL);

            if (!(tag_mask & tr_doorbell)) {
                /* clear_bit(lrbp->task_tag, &hba->outstanding_reqs); */
                hba->outstanding_reqs &= ~(1 << lrbp->task_tag);
                break;
            }

            usleep(1000);
            elapsed_ms += 1;

            if (elapsed_ms > timeout_ms)
                break;
        } while (1);

    // need to clear lrb_in_use for polling case,
    // otherwise no one can use this tag in the future.
    /* clear_bit(lrbp->task_tag, &hba->lrb_in_use);*/
    hba->lrb_in_use &= ~(1 << lrbp->task_tag);

    tr_doorbell = ufshcd_readl(hba, REG_UTP_TRANSFER_REQ_DOOR_BELL);

    if (tag_mask & tr_doorbell) {
        dprintf(CRITICAL, "[UFS] err: Query Request timeout.timeout_ms:%d\n", timeout_ms);

        ret = UFS_ERR_TIMEOUT_QUERY_REQUEST;
    }

    if (hba->drv_status & UFS_DRV_STS_DMA_WAIT_INTERRUPT)
        intr_status = ufshcd_intr_status;
    else
        intr_status = ufs_readl(hba, REG_INTERRUPT_STATUS);

    /* check interrupt status if any error happens */
    if (intr_status & UFSHCD_ERROR_MASK) {

        dprintf(CRITICAL, "[UFS] ERR! intr_status: 0x%x\n", intr_status);

        if (intr_status & UIC_ERROR) {

            dprintf(CRITICAL, "[UFS] REG_UIC_ERROR_CODE_PHY_ADAPTER_LAYER: 0x%x\n",
                ufs_readl(hba, REG_UIC_ERROR_CODE_PHY_ADAPTER_LAYER));
            dprintf(CRITICAL, "[UFS] REG_UIC_ERROR_CODE_DATA_LINK_LAYER: 0x%x\n",
                ufs_readl(hba, REG_UIC_ERROR_CODE_DATA_LINK_LAYER));
            dprintf(CRITICAL, "[UFS] REG_UIC_ERROR_CODE_NETWORK_LAYER: 0x%x\n",
                ufs_readl(hba, REG_UIC_ERROR_CODE_NETWORK_LAYER));
            dprintf(CRITICAL, "[UFS] REG_UIC_ERROR_CODE_TRANSPORT_LAYER: 0x%x\n",
                ufs_readl(hba, REG_UIC_ERROR_CODE_TRANSPORT_LAYER));
            dprintf(CRITICAL, "[UFS] REG_UIC_ERROR_CODE_DME: 0x%x\n",
                ufs_readl(hba, REG_UIC_ERROR_CODE_DME));
        }
        ufshcd_writel(hba, UFSHCD_ERROR_MASK, REG_INTERRUPT_STATUS);
    }

    if (ret == UFS_ERR_TIMEOUT_QUERY_REQUEST) {
        /* dump host reg */
        dprintf(ALWAYS, "[UFS] dump host reg\n");
        hexdump((void *)UFS_HCI_BASE, 0xA0);
        goto out;
    }

    /* invalid data buffer */
    if (lrbp->cmd)
        ufs_dma_unmap((unsigned long)lrbp->cmd->data_buf, lrbp->cmd->exp_len, lrbp->cmd->dir);
    /* dummy, just for pairing with ufs_dma_map(UTRD) and ufs_dma_map(PRDT) */
    /* ufs_dma_unmap((unsigned long)lrbp->ucd_req_ptr, sizeof(struct utp_upiu_req), */
    /* DMA_TO_DEVICE); */
    /* ufs_dma_unmap((unsigned long)hba->lrb->ucd_prdt_ptr, */
    /* sizeof(struct ufs_sg_entry) * UFS_MAX_SG_SEGMENTS, DMA_TO_DEVICE); */

    /* check OCS */

    /* invalid UTRD cache */
    ufs_dma_unmap((unsigned long)lrbp->utr_descriptor_ptr,
                  sizeof(struct utp_transfer_req_desc), DMA_BIDIRECTIONAL);

    ocs = ufshcd_get_tr_ocs(lrbp);

    if (ocs != OCS_SUCCESS) {
        dprintf(CRITICAL, "[UFS] err: OCS error = %x, T:%d\n", ocs, lrbp->task_tag);

        {
            u8 cnt;

            value = ufshcd_readl(hba, REG_UFS_MTK_OCS_ERR_STATUS);
            LTRACEF("[UFS] err: OCS err status reg = %x\n", value);

            for (cnt = 0; g_ufs_ocs_error_str[cnt].err != 0xFF; cnt++) {
                if (value & (1 << cnt))
                    LTRACEF("[UFS] err: %s\n", g_ufs_ocs_error_str[cnt].name);
            }
        }

        ret = UFS_ERR_OCS_ERROR;
        goto out;
    }

    /* check response code */

    /* invalid RESPONSE UPIU cache
     *
     * NOTE: Here we only invalidate length of "strcut utp_upiu_rsp". For Response UPIU with
     *       longer data size (e.g., Read Descriptor), need to invalidate more data.
     *       Please see ufshcd_copy_query_response().
     */
    ufs_dma_unmap((unsigned long)lrbp->ucd_rsp_ptr, sizeof(struct utp_upiu_rsp), DMA_FROM_DEVICE);

    if (lrbp->command_type == UTP_CMD_TYPE_DEV_MANAGE)  /* for device management commands */
        return ufshcd_dev_cmd_completion(hba, lrbp);
    else {                                              /* for SCSI commands */
        resp = ufshcd_get_rsp_upiu_result(lrbp->ucd_rsp_ptr);

        if (resp & MASK_TASK_RESPONSE) {

            value = ufshcd_get_req_rsp(lrbp->ucd_rsp_ptr);

            if (value == UPIU_TRANSACTION_RESPONSE) {
                ufs_mtk_dump_asc_ascq(hba, lrbp->ucd_rsp_ptr->sr.sense_data[12],
                    lrbp->ucd_rsp_ptr->sr.sense_data[13]);
                    dprintf(CRITICAL, "[UFS] Sense Data(all): ");
                    ufs_util_dump_config_table_hex(lrbp->ucd_rsp_ptr->sr.sense_data,
                        18, 18, 0);
            }

            /* sense key is UNIT_ATTENTION, will retry */
            if ((lrbp->ucd_rsp_ptr->sr.sense_data[2] & 0xF) == UNIT_ATTENTION)
                ret = UFS_ERR_UNIT_ATTENTION_RETRY;
            else {
                ret = UFS_ERR_TASK_RESP_ERROR;
                dprintf(CRITICAL, "[UFS] err: task response error = %x\n",
                    (resp & MASK_TASK_RESPONSE) >> 8);
            }
            goto out;
        }
    }

out:
    if (lrbp->cmd)
        lrbp->cmd = NULL;

    LTRACEF("[UFS] info: CMD done\n");

    return ret;
}

void ufs_crypto_cal_dun(u32 alg_id, u32 lba, u32 *dunl, u32 *dunu)
{
    if (alg_id != UFS_CRYPTO_ALGO_BITLOCKER_AES_CBC) {
        *dunl = lba;
        *dunu = 0;
    } else {                             /* bitlocker dun use byte address */
        *dunl = (lba & 0x7FFFF) << 12;   /* byte address for lower 32 bit */
        *dunu = (lba >> (32-12)) << 12;  /* byte address for higher 32 bit */
    }
}

static void ufshcd_queuecommand_timeout(struct ufs_scsi_cmd *cmd, int *timeout)
{
    if (cmd->cmd_data[0] == UNMAP)
        *timeout = UTP_UNMAP_TIMEOUT_MS;
    else
        *timeout = UTP_TRANSFER_REQ_TIMEOUT_MS;
}

int ufshcd_queuecommand(struct ufs_hba *hba, struct ufs_scsi_cmd *cmd)
{
    struct ufshcd_lrb *lrbp;
    int tag, lun;
    int err = 0;
    u32 blk_cnt;
    u32 lba;
    int timeout = UTP_TRANSFER_REQ_TIMEOUT_MS;

    tag = cmd->tag;
    lun = cmd->lun;

    lba = cmd->cmd_data[5] | (cmd->cmd_data[4] << 8) | (cmd->cmd_data[3] << 16) |
        (cmd->cmd_data[2] << 24);
    blk_cnt = cmd->cmd_data[8] | (cmd->cmd_data[7] << 8);

    LTRACEF("[UFS] info: QCMD,L:%x,T:%d,0x%x,A:0x%x,C:%d\n", lun, tag, cmd->cmd_data[0],
        lba, blk_cnt);

    lrbp = &hba->lrb[0];

    lrbp->cmd = cmd;
    lrbp->task_tag = tag;
    lrbp->lun = lun;
    lrbp->intr_cmd = true;    /* in AIO driver, use interrupt for every commands. */
    lrbp->command_type = UTP_CMD_TYPE_SCSI;

    /* form UPIU before issuing the command */
    ufshcd_compose_upiu(hba, lrbp);
    err = ufshcd_map_sg(hba, lrbp);

    if (err) {
        LTRACEF("Err: ufshcd_map_sg_ut fails\n");
        lrbp->cmd = NULL;
        /* clear_bit(tag, &hba->lrb_in_use); */
        hba->lrb_in_use &= ~(1 << tag);
        goto out;
    }

    /* issue command to the controller */
    ufshcd_send_command(hba, tag);

    /* Setting utp transfer req command timeout according to different cmd */
    ufshcd_queuecommand_timeout(cmd, &timeout);

    return ufshcd_wait_command(hba, lrbp, timeout);

out:
    return err;
}

static void ufs_advertise_hci_quirks(struct ufs_hba *hba)
{
#ifdef UFS_CFG_FPGA_PLATFORM
    hba->hci_quirks |= UFSHCD_QUIRK_MTK_MPHY_TESTCHIP;
#endif
}

static int ufs_test_unit_ready_all_device(struct ufs_hba *hba)
{
    int i, tag;
    struct ufs_scsi_cmd cmd;
    int ret = UFS_ERR_NONE;

    if (hba->dev_info.num_active_lu == 0) {
        dprintf(CRITICAL, "bug: active_num_lu shall not be 0");
        return -1;
    }

    if (!ufshcd_get_free_tag(hba, &tag))
        return -1;


    /* send test unit ready to each LUN to remove possible UNIT ATTENTION */
    for (i = 0; i < hba->dev_info.num_active_lu; i++) {
        do {
            ufs_scsi_cmd_test_unit_ready(&cmd, tag, i);

            /* check error code if retrun with UNIT ATTENTION */
            ret = ufshcd_queuecommand(hba, &cmd);
    } while (ret);
    }

    /* send test unit ready to WRPMB LUN */
    do {
        ufs_scsi_cmd_test_unit_ready(&cmd, tag, WLUN_RPMB);
    /* check error code if retrun with UNIT ATTENTION */
        ret = ufshcd_queuecommand(hba, &cmd);
    } while (ret);

    hba->drv_status &= ~UFS_DRV_STS_TEST_UNIT_READY_ALL_DEVICE;

    ufshcd_put_tag(hba, tag);

    return 0;
}

static int ufshcd_probe_hba(struct ufs_hba *hba)
{
    int ret;

    ret = ufshcd_link_startup(hba);
    if (ret)
        goto out;

    ufshcd_init_pwr_info(hba);

    ret = hba->nopin_nopout(hba);

    if (ret)
        goto out;

    ret = ufshcd_complete_dev_init(hba);

    if (ret)
        goto out;

    /* Init check for device descriptor sizes */
    ufshcd_init_desc_sizes(hba);

    ret = ufs_get_device_info(hba);
    if (ret) {
        dprintf(CRITICAL, "Failed getting device info. err = %d\n", ret);
        goto out;
    }

    /* get quirks according to Device Descritpor contents */
    ufs_advertise_fixup_device(hba);

    ufshcd_tune_unipro_params(hba);

    ufs_check_lu_cfg(hba);

    ufs_test_unit_ready_all_device(hba);

    /* ufshcd_force_reset_auto_bkops(hba); */

    if (ufshcd_get_max_pwr_mode(hba))
        dprintf(CRITICAL, "Failed getting max supported power mode\n");
    else {
        ret = ufshcd_config_pwr_mode(hba, &hba->max_pwr_info.info);
        if (ret) {
            dprintf(CRITICAL, "Failed setting power mode, err = %d\n", ret);

            /* ignore power mode change error to let UFS init go through anyway */
            ret = 0;
        }
    }

    /* enable auto-hibern8 (10x100ms=1s) for pre-charging reduce ufs power */
    ufshcd_writel(hba, (0x5 << 10 | 0x0A), REG_AHIT);

out:
    return ret;
}

/**
 * ufshcd_init - Driver initialization routine
 * Returns 0 on success, non-zero value on failure
 */
int ufshcd_init(void)
{
    int err;
    int retry_init_cnt = 0;
    struct ufs_hba *hba = &g_ufs_hba;

    hba->hci_base = (void *)UFS_HCI_BASE;
    hba->pericfg_base = (void *)UFS_MMIO_PERICFG_BASE;
    hba->mphy_base = (void *)UFS_MPHY_BASE;

    /* get hba capabilities */
    ufshcd_get_host_capabilities(hba);

    /* init power, add when need */
    /* init clock, add when need */

    ufs_advertise_hci_quirks(hba);

    /* allocate memory for hba memory space */
    err = ufshcd_memory_alloc(hba);

    if (err) {
        LTRACEF("Memory allocation failed\n");
        goto out_error;
    }

    /* Configure LRB */
    ufshcd_host_memory_configure(hba);

ufshcd_init_reinit_host:

    /* Host controller enable */
    err = ufshcd_hba_enable(hba);

    if (err) {
        dprintf(CRITICAL, "[UFS] err: Host controller enable failed\n");
        goto out_error;
    }

    spin_lock_init(&hba->free_tag_lock);

    err = ufshcd_probe_hba(hba);

    if (err == UFS_ERR_NEED_REINIT_HOST) {
        retry_init_cnt++;

        if (retry_init_cnt > 3) {
            dprintf(CRITICAL, "[UFS] err: retry number exceeded (reinit host)\n");
            goto out_error;
        }

        goto ufshcd_init_reinit_host;
    }

    if (err) {
        dprintf(CRITICAL, "[UFS] err: ufshcd_probe_hba failed\n");
        goto out_error;
    }

    if (!ufshcd_irq_init(hba))
        hba->drv_status |= UFS_DRV_STS_DMA_WAIT_INTERRUPT;

    ufs_set_bootdevice();

    return 0;

out_error:
    return err;
}

static int ufshcd_query_descriptor(struct ufs_hba *hba,
                                   enum query_opcode opcode, enum desc_idn idn, u8 index,
                                   u8 selector, u8 *desc_buf, u32 *buf_len)
{
    struct ufs_query_req *request = NULL;
    struct ufs_query_res *response = NULL;
    int err;

    ASSERT(hba);

    if (*buf_len < QUERY_DESC_MIN_SIZE || *buf_len > QUERY_DESC_MAX_SIZE) {
        LTRACEF("descriptor buffer size (%d) is out of range\n", *buf_len);
        err = -1;
        goto out;
    }

    if (!desc_buf) {
        LTRACEF("descriptor buffer required for opcode 0x%x\n", opcode);
        err = -1;
        goto out;
    }

    ufshcd_init_query(hba, &request, &response, opcode, idn, index, selector);
    request->upiu_req.length = SWAP_16(*buf_len);
    hba->dev_cmd.query.descriptor = desc_buf;

    switch (opcode) {
    case UPIU_QUERY_OPCODE_READ_DESC:
        request->query_func = UPIU_QUERY_FUNC_STANDARD_READ_REQUEST;
        break;

    case UPIU_QUERY_OPCODE_WRITE_DESC:
        request->query_func = UPIU_QUERY_FUNC_STANDARD_WRITE_REQUEST;
        break;

    default:
        dprintf(CRITICAL, "Expected query descriptor opcode but got = 0x%x\n", opcode);
        err = -1;
        goto out;
    }

    err = ufshcd_exec_dev_cmd(hba, DEV_CMD_TYPE_QUERY, QUERY_REQ_TIMEOUT);

    if (err) {
        dprintf(CRITICAL, "opcode 0x%x for idn %d failed, err = %d\n", opcode, idn, err);
        goto out;
    }

    *buf_len = SWAP_16(response->upiu_res.length);
    hba->dev_cmd.query.descriptor = NULL;

out:
    return err;
}

static int ufshcd_read_desc_param(struct ufs_hba *hba,
                                  enum desc_idn desc_id,
                                  int desc_index,
                                  u8 selector,
                                  u32 param_offset,
                                  u8 *param_read_buf,
                                  u32 param_size)
{
    u32 desc_len;
    u8 *desc_buf;
    int ret;

    if (desc_id >= QUERY_DESC_IDN_MAX)
        return -1;

    desc_len = ufs_query_desc_max_size[desc_id];

    if (desc_len > param_size)
        desc_len = param_size;
    desc_buf = param_read_buf;

    ret = ufshcd_query_descriptor(hba, UPIU_QUERY_OPCODE_READ_DESC,
                                  desc_id, desc_index, selector, desc_buf,
                                  &desc_len);

    if (ret ||
            (desc_len > ufs_query_desc_max_size[desc_id]) ||
            (desc_buf[QUERY_DESC_DESC_TYPE_OFFSET] != desc_id)) {
        dprintf(CRITICAL,
            "[UFS] failed reading %s. desc_id %d param_offset %d desc_len %d ret %d\n",
            __func__, desc_id, param_offset, desc_len, ret);

        if (!ret)
            ret = UFS_ERR_INVALID_DESCRIPTOR;

        goto out;
    }

out:

    return ret;
}

static int ufshcd_read_desc_length(struct ufs_hba *hba,
    enum desc_idn desc_id,
    int desc_index,
    u8 selector,
    u32 *desc_length)
{
    int ret;
    u8 header[QUERY_DESC_HDR_SIZE];
    u32 header_len = QUERY_DESC_HDR_SIZE;

    if (desc_id >= QUERY_DESC_IDN_MAX)
        return -1;

    ret = ufshcd_query_descriptor(hba, UPIU_QUERY_OPCODE_READ_DESC,
                    desc_id, desc_index, selector, header,
                    &header_len);

    if (ret) {
        dprintf(CRITICAL, "%s: Failed to get descriptor header id %d\n",
            __func__, desc_id);
        return ret;
    } else if (desc_id != header[QUERY_DESC_DESC_TYPE_OFFSET]) {
        dprintf(CRITICAL, "%s: descriptor header id %d and desc_id %d mismatch\n",
            __func__, header[QUERY_DESC_DESC_TYPE_OFFSET],
            desc_id);
        ret = -1;
    }

    *desc_length = header[QUERY_DESC_LENGTH_OFFSET];
    return ret;

}

static void ufshcd_init_desc_sizes(struct ufs_hba *hba)
{
    int err;
    static int init_done;

    if (init_done)
        return;

    err = ufshcd_read_desc_length(hba, QUERY_DESC_IDN_DEVICE, 0, 0,
        &ufs_query_desc_max_size[QUERY_DESC_IDN_DEVICE]);
    if (err)
        ufs_query_desc_max_size[QUERY_DESC_IDN_DEVICE] = QUERY_DESC_DEVICE_MAX_SIZE;
    dprintf(ALWAYS, "device descriptor length = 0x%x\n",
        ufs_query_desc_max_size[QUERY_DESC_IDN_DEVICE]);

    err = ufshcd_read_desc_length(hba, QUERY_DESC_IDN_POWER, 0, 0,
        &ufs_query_desc_max_size[QUERY_DESC_IDN_POWER]);
    if (err)
        ufs_query_desc_max_size[QUERY_DESC_IDN_POWER] = QUERY_DESC_POWER_MAX_SIZE;
    dprintf(ALWAYS, "power descriptor length = 0x%x\n",
        ufs_query_desc_max_size[QUERY_DESC_IDN_POWER]);

    err = ufshcd_read_desc_length(hba, QUERY_DESC_IDN_INTERCONNECT, 0, 0,
        &ufs_query_desc_max_size[QUERY_DESC_IDN_INTERCONNECT]);
    if (err)
        ufs_query_desc_max_size[QUERY_DESC_IDN_INTERCONNECT] = QUERY_DESC_INTERCONNECT_MAX_SIZE;
    dprintf(ALWAYS, "interconnect descriptor length = 0x%x\n",
        ufs_query_desc_max_size[QUERY_DESC_IDN_INTERCONNECT]);

    err = ufshcd_read_desc_length(hba, QUERY_DESC_IDN_CONFIGURATION, 0, 0,
        &ufs_query_desc_max_size[QUERY_DESC_IDN_CONFIGURATION]);
    if (err)
        ufs_query_desc_max_size[QUERY_DESC_IDN_CONFIGURATION] = QUERY_DESC_CONFIGURAION_MAX_SIZE;
    dprintf(ALWAYS, "configuration descriptor length = 0x%x\n",
        ufs_query_desc_max_size[QUERY_DESC_IDN_CONFIGURATION]);

    err = ufshcd_read_desc_length(hba, QUERY_DESC_IDN_UNIT, 0, 0,
        &ufs_query_desc_max_size[QUERY_DESC_IDN_UNIT]);
    if (err)
        ufs_query_desc_max_size[QUERY_DESC_IDN_UNIT] = QUERY_DESC_UNIT_MAX_SIZE;
    dprintf(ALWAYS, "unit descriptor length = 0x%x\n",
        ufs_query_desc_max_size[QUERY_DESC_IDN_UNIT]);

    err = ufshcd_read_desc_length(hba, QUERY_DESC_IDN_GEOMETRY, 0, 0,
        &ufs_query_desc_max_size[QUERY_DESC_IDN_GEOMETRY]);
    if (err)
        ufs_query_desc_max_size[QUERY_DESC_IDN_GEOMETRY] = QUERY_DESC_GEOMETRY_MAX_SIZE;
    dprintf(ALWAYS, "geometry descriptor length = 0x%x\n",
        ufs_query_desc_max_size[QUERY_DESC_IDN_GEOMETRY]);

    init_done = 1;
}

static int ufshcd_init_vendor_desc_sizes(struct ufs_hba *hba)
{
    int err = 0;
    static int vendor_init_done;

    if (vendor_init_done)
        return err;

    err = ufshcd_read_desc_length(hba, QUERY_DESC_IDN_DEVICE, 0, 1,
        &ufs_query_desc_max_size[QUERY_DESC_IDN_DEVICE]);
    if (err)
        goto out;
    dprintf(ALWAYS, "vendor device descriptor length = 0x%x\n",
        ufs_query_desc_max_size[QUERY_DESC_IDN_DEVICE]);

    err = ufshcd_read_desc_length(hba, QUERY_DESC_IDN_CONFIGURATION, 0, 1,
        &ufs_query_desc_max_size[QUERY_DESC_IDN_CONFIGURATION]);
    if (err)
        goto out;
    dprintf(ALWAYS, "vendor configuration descriptor length = 0x%x\n",
        ufs_query_desc_max_size[QUERY_DESC_IDN_CONFIGURATION]);

    err = ufshcd_read_desc_length(hba, QUERY_DESC_IDN_GEOMETRY, 0, 1,
        &ufs_query_desc_max_size[QUERY_DESC_IDN_GEOMETRY]);
    if (err)
        goto out;
    dprintf(ALWAYS, "vendor geometry descriptor length = 0x%x\n",
        ufs_query_desc_max_size[QUERY_DESC_IDN_GEOMETRY]);

out:
    vendor_init_done = 1;

    return err;
}

/* Return if current boot stage is in secure world or not */
bool ufs_is_in_secure_world(void)
{
    u32 bstage;

    bstage = get_boot_stage();

    /* secure EL1, TFA smc cannot use */
    if ((bstage == BOOT_STAGE_BL2) || (bstage == BOOT_STAGE_BL2_EXT) ||
        (bstage == BOOT_STAGE_DA) || (bstage == BOOT_STAGE_AEE))
        return true;
    else
        return false;
}

static void ufs_reset_device(struct ufs_hba *hba)
{
    u32 reg;
    struct __smccc_res smccc_res;

    if (!hba->pericfg_base) {
        LTRACEF("[UFS] err: %s: pericfg_base is NULL\n", __func__);
        return;
    }

    /* secure EL1, TFA smc cannot use, direct write reg */
    if (ufs_is_in_secure_world()) {
        reg = readl(hba->pericfg_base + REG_UFS_PERICFG);

        reg = reg & ~(1 << REG_UFS_PERICFG_RST_N_BIT);
        writel(reg, hba->pericfg_base + REG_UFS_PERICFG);

        udelay(10);

        reg = reg | (1 << REG_UFS_PERICFG_RST_N_BIT);
        writel(reg, hba->pericfg_base + REG_UFS_PERICFG);
    } else {
        /* notify tfa to reset ufs device  */
        memset(&smccc_res, 0, sizeof(smccc_res));

        __smc_conduit(MTK_SIP_BL_UFS_CONTROL, UFS_MTK_SIP_DEVICE_RESET,
            0, 0, 0, 0, 0, 0, &smccc_res);

        udelay(10);

        __smc_conduit(MTK_SIP_BL_UFS_CONTROL, UFS_MTK_SIP_DEVICE_RESET,
            1, 0, 0, 0, 0, 0, &smccc_res);
    }

    /* wait awhile after device reset */
    /* We add this as general solution even though only SK-Hynix needs this delay actually. */
    mdelay(10);
}

void ufs_reset_device_low(struct ufs_hba *hba)
{
    u32 reg;
    struct __smccc_res smccc_res;

    if (!hba->pericfg_base) {
        LTRACEF("[UFS] err: %s: pericfg_base is NULL\n", __func__);
        return;
    }

    /* secure EL1, TFA smc cannot use, direct write reg */
    if (ufs_is_in_secure_world()) {
        reg = readl(hba->pericfg_base + REG_UFS_PERICFG);

        reg = reg & ~(1 << REG_UFS_PERICFG_RST_N_BIT);
        writel(reg, hba->pericfg_base + REG_UFS_PERICFG);

    } else {
        /* notify tfa to reset ufs device  */
        memset(&smccc_res, 0, sizeof(smccc_res));

        __smc_conduit(MTK_SIP_BL_UFS_CONTROL, UFS_MTK_SIP_DEVICE_RESET,
            0, 0, 0, 0, 0, 0, &smccc_res);
    }
}

int ufs_set_boot_lu(struct ufs_hba *hba, u32 b_boot_lun_en)
{
    int ret;

    ret = hba->query_attr(hba, UPIU_QUERY_OPCODE_WRITE_ATTR, ATTR_B_BOOT_LUN_EN, 0, 0,
        &b_boot_lun_en);

    if (ret != 0)
        LTRACEF("[UFS] err: %s error: %d\n", __func__, ret);
    else
        LTRACEF("[UFS] info: %s %d done\n", __func__, b_boot_lun_en);

    return ret;
}

u32 ufs_get_lu_cfg_size(struct ufs_hba *hba, struct ufs_geometry_info *info, u32 lun_id)
{
    u32 d_num_alloc_units;

    dprintf(CRITICAL, "[UFS] %s: lun %d\n", __func__, lun_id);

    dprintf(CRITICAL, "[UFS] d_segment_size=0x%x\n", info->d_segment_size);
    dprintf(CRITICAL, "[UFS] b_allocation_units_size=0x%x\n", info->b_allocation_units_size);
    dprintf(CRITICAL, "[UFS] q_total_raw_device_capacity=0x%llx\n",
        info->q_total_raw_device_capacity);

    if (lun_id < 2) {   /* boot lu */
        /*
         * Boot size is 4MB which have enhance feature, real size
         * should be 4MB * (w_adj_factor_enahnced_1 / 256)
         */
        dprintf(CRITICAL, "[UFS] w_adj_factor_enahnced_1=%d\n", info->w_adj_factor_enahnced_1);

        d_num_alloc_units = ((UFS_BOOT_LU_SIZE_BYTE >> 9) * (info->w_adj_factor_enahnced_1 >> 8)) /
                            info->d_segment_size / info->b_allocation_units_size;
    } else {            /* user lu */

        d_num_alloc_units = (u32)(((info->q_total_raw_device_capacity) -
            (u64)(2 * (UFS_BOOT_LU_SIZE_BYTE >> 9) * (info->w_adj_factor_enahnced_1 >> 8))) /
                info->d_segment_size / info->b_allocation_units_size);
    }

    dprintf(CRITICAL, "[UFS] d_num_alloc_units=0x%x\n", d_num_alloc_units);

    return d_num_alloc_units;
}

static void ufs_util_dump_config_table_hex(u8 *table, u32 bytes,
    u32 ud0_base_offset, u32 ud_config_len)
{
    u32 i, ptr;

    ptr = 0;

    for (i = 0; i < ud0_base_offset; i++) {
        dprintf(ALWAYS, "%x ", table[ptr]);
        ptr++;

        if (ptr >= bytes)
            break;
    }
    dprintf(CRITICAL, "\n");

    while (ptr < bytes) {

        for (i = 0; i < ud_config_len; i++) {
            dprintf(ALWAYS, "%x ", table[ptr]);
            ptr++;

            if (ptr >= bytes)
                break;
        }

        dprintf(CRITICAL, "\n");

        if (ptr >= bytes)
            return;
    }
}

static void ufs_util_dump_table_hex(u8 *table, u32 bytes)
{
    u32 i, ptr;

    ptr = 0;

    while (ptr < bytes) {

        for (i = 0; i < 16; i++) {
            dprintf(ALWAYS, "%x ", table[ptr]);
            ptr++;

            if (ptr >= bytes)
                break;
        }

        dprintf(CRITICAL, "\n");

        if (ptr >= bytes)
            return;
    }
}

int ufs_check_lu_cfg(struct ufs_hba *hba)
{
    int ret;
    u32 i;
    u8 *p;
    u32 num_active_lu = 0;
    u32 num_boot_lu = 0;
    u8 *ufs_temp_buf = memalign(CACHE_LINE, UFS_TEMP_BUF_SIZE);
    /* malloc fail */
    ASSERT(ufs_temp_buf);

    /* HPB support, read configuration descriptor with HPB, selector = 1, length = 0xD0 */
    ret = hba->read_descriptor(hba, QUERY_DESC_IDN_CONFIGURATION, 0, hba->quirk_sel,
            (u8 *)ufs_temp_buf, ufs_query_desc_max_size[QUERY_DESC_IDN_CONFIGURATION]);

    if (ret != 0) {
        dprintf(CRITICAL, "[UFS] err: %s: read config descr error: %d\n", __func__, ret);
        free(ufs_temp_buf);
        return ret;
    }

    dprintf(CRITICAL, "[UFS] %s: original Configuration Desc:\n", __func__);

    ufs_util_dump_config_table_hex(ufs_temp_buf,
        ufs_query_desc_max_size[QUERY_DESC_IDN_CONFIGURATION],
        hba->dev_info.ud0_base_offset, hba->dev_info.ud_config_len);

    p = (u8 *)ufs_temp_buf;

    /* check: number of active LU */
    for (i = 0; i < 8; i++) { /* traverse 8 Unit Descriptor configurable parameters */
        if (i == 0)
            p += hba->dev_info.ud0_base_offset;
        else
            p += hba->dev_info.ud_config_len;

        /* 01h: Logical Unit enabled, 02h: HPB Logical Unit enabled */
        if ((p[CONF_DESC_UNIT_B_LU_ENABLE] == 1) ||
                ((hba->dev_info.hpb_support) && (p[CONF_DESC_UNIT_B_LU_ENABLE] == 2)))
            num_active_lu++;

        if ((i < 2) && (p[CONF_DESC_UNIT_B_BOOT_LUN_ID] != 0))
            num_boot_lu++;

        if (i == 2) {
            /* check whether HPB is enabled for LU2 if available */
            if ((hba->dev_info.hpb_support) && (p[CONF_DESC_UNIT_B_LU_ENABLE] != 0x02))
                dprintf(CRITICAL, "[UFS] HPB is supported but not enabled on LU%d\n", i);

        }
    }

    hba->dev_info.num_active_lu = num_active_lu;

    free(ufs_temp_buf);
    return UFS_ERR_NONE;
}

int ufs_poweroff_notify(struct ufs_hba *hba)
{
    int ret = 0;
    int tag;
    int retry = 3;
    struct ufs_scsi_cmd cmd;

    if (!ufshcd_get_free_tag(hba, &tag))
        return -1;

    ufs_scsi_cmd_start_stop_unit_pd(&cmd, tag);
    do {
        ret = ufshcd_queuecommand(hba, &cmd);
        if (ret == 0) {
            dprintf(INFO, "[UFS] notify power-off success\n");
            break;
        }
        retry--;
    } while (ret && retry > 0);


    if (ret)
        dprintf(CRITICAL, "[UFS] %s fail\n", __func__);

    return ret;
}

struct ufs_hba *ufs_get_host(u8 host_id)
{
    return &g_ufs_hba;
}

int ufs_get_device_info(struct ufs_hba *hba)
{
    struct ufs_scsi_cmd cmd;
    struct ufs_device_info *card_data;
    int err;
    int tag;
    u8 *p = NULL, sel = 0;
    int str_len;
    u32 tmp_len = 0;
    int  i;
    bool vpd80 = false;
    u8 logical_blk_size;
    u32 logical_blk_size_bytes;
    u64 logical_blk_cnt;
    u8 *ufs_temp_buf = memalign(CACHE_LINE, UFS_TEMP_BUF_SIZE);

    ASSERT(ufs_temp_buf);
    card_data = &hba->dev_info;

    /* Have init and not reconfing device */
    if ((card_data->wmanufacturerid != 0) && (card_data->bootable == 1)) {
        free(ufs_temp_buf);
        return 0;
    }

    memset(card_data, 0, sizeof(struct ufs_device_info));

    err = hba->read_descriptor(hba, QUERY_DESC_IDN_DEVICE, 0, 0, (u8 *)ufs_temp_buf,
        ufs_query_desc_max_size[QUERY_DESC_IDN_DEVICE]);

    if (err) {
        dprintf(CRITICAL, "[UFS] err: %s: failed reading Device Desc. err = %d\n", __func__, err);
        free(ufs_temp_buf);
        return -1;
    }

    p = (u8 *)ufs_temp_buf;

    card_data->ufs_ver = p[DEVICE_DESC_PARAM_SPEC_VER] << 8 |
                     p[DEVICE_DESC_PARAM_SPEC_VER + 1];
    dprintf(ALWAYS, "[UFS] UFS version %x.%x\n", p[DEVICE_DESC_PARAM_SPEC_VER],
        p[DEVICE_DESC_PARAM_SPEC_VER + 1]);

    dprintf(CRITICAL, "[UFS] %s: Device Desc:\n", __func__);
    ufs_util_dump_table_hex(p, ufs_query_desc_max_size[QUERY_DESC_IDN_DEVICE]);

    if (p[DEVICE_DESC_PARAM_BOOT_ENBL] == 0)
        card_data->bootable = 0;
    else
        card_data->bootable = 1;

    /*
     * getting vendor (manufacturerID) and Bank Index in big endian format
     */
    card_data->wmanufacturerid = p[DEVICE_DESC_PARAM_MANF_ID] << 8 |
                                 p[DEVICE_DESC_PARAM_MANF_ID + 1];

read_feature:
    dprintf(INFO, "Try selector: 0x%x\n", sel);
        /* Check supported features */
    dprintf(INFO, "Dev. Descr. F. Support: 0x%x\n", p[DEVICE_DESC_PARAM_FEAT_SUP]);
    dprintf(INFO, "Dev. Descr. Ext. F. Support: 0x%x\n", p[DEVICE_DESC_PARAM_EXT_FEAT_SUP]);

    /* Check support HPB feature or not */
    if (p[DEVICE_DESC_PARAM_FEAT_SUP] & 0x80)
        card_data->hpb_support = 1;
    else
        card_data->hpb_support = 0;

    if (card_data->hpb_support) {
        card_data->hpb_ver = (p[DEVICE_DESC_PARAM_HPB_VER] << 8) |
                             p[DEVICE_DESC_PARAM_HPB_VER + 1];
        card_data->hpb_ctrl_mode = p[DEVICE_DESC_PARAM_HPB_CTRL_MODE];

        LTRACEF("HPB Version = %x.%x.%x, Control mode = %d\n",
                     (card_data->hpb_ver >> 8) & 0xFF,
            (card_data->hpb_ver >> 4) & 0xF,
            card_data->hpb_ver & 0xF, card_data->hpb_ctrl_mode);
    }

    /* Check support TW feature or not */
    if (p[DEVICE_DESC_PARAM_EXT_FEAT_SUP + 2] & 0x1)
        card_data->tw_support = 1;
    else
        card_data->tw_support = 0;

    if (card_data->tw_support) {
        card_data->tw_red = p[DEVICE_DESC_PARAM_TW_BUF_USER_REDUCED];
        card_data->tw_type = p[DEVICE_DESC_PARAM_TW_BUF_TYPE];
        dprintf(ALWAYS, "TW Reduction = %d, TW Type = %d\n",
            card_data->tw_red, card_data->tw_type);

        /* Only Samsung special ufs version have this field */
        if ((card_data->wmanufacturerid == UFS_VENDOR_SAMSUNG) && (sel == 0x01)) {
            if ((card_data->ufs_ver == 0x210) || (card_data->ufs_ver == 0x300))
                card_data->tw_ver = (p[0x55] << 8) | p[0x56];
            else
                card_data->tw_ver = (p[0x4D] << 8) | p[0x4E];

            dprintf(ALWAYS, "TW Version = %x.%x.%x\n",
                (card_data->tw_ver >> 8) & 0xFF,
                (card_data->tw_ver >> 4) & 0xF,
                card_data->tw_ver & 0xF);
        } else {
            card_data->wb_buf_au = (p[DEVICE_DESC_PARAM_NUM_SHARED_WB_BUF_AU] << 24) |
                           (p[DEVICE_DESC_PARAM_NUM_SHARED_WB_BUF_AU + 1] << 16) |
                           (p[DEVICE_DESC_PARAM_NUM_SHARED_WB_BUF_AU + 2] << 5) |
                           (p[DEVICE_DESC_PARAM_NUM_SHARED_WB_BUF_AU + 3]);
            dprintf(ALWAYS, "Shared WB Buffer AU = 0x%x\n", card_data->wb_buf_au);
        }
    }

    /* Normal device:(0x10, 0x10) / HPB device:(0x10, 0x18) */
    card_data->ud0_base_offset = p[DEVICE_DESC_PARAM_UD_OFFSET];
    card_data->ud_config_len = p[DEVICE_DESC_PARAM_UD_LEN];
    dprintf(ALWAYS, "ud0_base_offset = %d, ud_config_len = %d\n",
        card_data->ud0_base_offset, card_data->ud_config_len);

    if (card_data->wmanufacturerid == UFS_VENDOR_SAMSUNG && (sel == 0x00)) {
        // TODO: remove after EOL of Selen and Santos

        err = ufshcd_read_desc_length(hba, QUERY_DESC_IDN_DEVICE, 0, 1, &tmp_len);
        err += hba->read_descriptor(hba, QUERY_DESC_IDN_DEVICE, 0, 1, (u8 *)ufs_temp_buf, tmp_len);

        if (err) {
            dprintf(CRITICAL, "[UFS] err: %s: failed reading HPB Device Desc. err = %d\n",
                __func__, err);
            free(ufs_temp_buf);
            return -1;
        }

        LTRACEF("[UFS] %s: legacy Device Desc:\n", __func__);
        ufs_util_dump_table_hex(p, tmp_len);

        /*
         * Assume device after Solvit doesn't show HPB capability in DEVICE_DESC_PARAM_FEAT_SUP
         * with SEL=0x00
         */
        if (p[DEVICE_DESC_PARAM_FEAT_SUP] & 0x80) {
            sel++;
            err = ufshcd_init_vendor_desc_sizes(hba);
            if (err) {
                free(ufs_temp_buf);
                return -1;
            }
            goto read_feature;
        }
    }

    // set quirk selector for later use
    hba->quirk_sel = sel;

    /* getting active LU number */
    /* card_data->num_active_lu = p[DEVICE_DESC_PARAM_NUM_LU]; */

    if (!ufshcd_get_free_tag(hba, &tag)) {
        free(ufs_temp_buf);
        return -1;
    }

    memset(ufs_temp_buf, 0, QUERY_DESC_HEALTH_MAX_SIZE);
    ufs_scsi_cmd_inquiry(&cmd, tag, 0, (unsigned long *)ufs_temp_buf, 0, 0);
    err = ufshcd_queuecommand(hba, &cmd);
    if (err) {
        LTRACEF("[UFS] err: ufshcd_queuecommand err\n");
        free(ufs_temp_buf);
        return -1;
    }

    p = (u8 *)ufs_temp_buf;

    /*
     * get product ID and product revision level (fw ver)
     * by INQUIRY command
     */

    /* product ID */
    str_len = ufs_util_sanitize_inquiry_string(&p[16], 16);
    if (str_len) {
        LTRACEF("[UFS] get product id string len 0x%x\n", str_len);
        memcpy(card_data->product_id, &p[16], str_len);
    }

    /* product revision level */
    str_len = ufs_util_sanitize_inquiry_string(&p[32], 4);
    if (str_len) {
        LTRACEF("[UFS] get revision level string len 0x%x\n", str_len);
        memcpy(card_data->product_revision_level, &p[32], str_len);
    }

    /* Serial number in VPD80 */
    memset(ufs_temp_buf, 0, QUERY_DESC_HEALTH_MAX_SIZE);
    ufs_scsi_cmd_inquiry(&cmd, tag, 0, (unsigned long *)ufs_temp_buf, 1, 0);
    err = ufshcd_queuecommand(hba, &cmd);
    if (err) {
        dprintf(CRITICAL, "[UFS] err: ufshcd_queuecommand err\n");
        free(ufs_temp_buf);
        return -1;
    }

    for (i = 4; i < ufs_temp_buf[3]; i++) {
        if (ufs_temp_buf[i] == 0x80) {
            vpd80 = true;
            break;
        }
    }

    if (vpd80) {
        memset(ufs_temp_buf, 0, QUERY_DESC_HEALTH_MAX_SIZE);
        ufs_scsi_cmd_inquiry(&cmd, tag, 0, (unsigned long *)ufs_temp_buf, 1, 0x80);
        err = ufshcd_queuecommand(hba, &cmd);
        if (err) {
            dprintf(CRITICAL, "[UFS] err: ufshcd_queuecommand err\n");
            free(ufs_temp_buf);
            return -1;
        }

        str_len = ufs_util_sanitize_inquiry_string(&p[4], ufs_temp_buf[3]);
        card_data->serial_number_len = str_len;
        if (str_len) {
            dprintf(CRITICAL, "[UFS] get unit serial number string len 0x%x\n", str_len);
            memcpy(card_data->serial_number, &p[4], str_len);
        }
    }

    /* Get rpmb lu size */
    memset(ufs_temp_buf, 0, ufs_query_desc_max_size[QUERY_DESC_IDN_UNIT]);
    err = hba->read_descriptor(hba, QUERY_DESC_IDN_UNIT, UFS_UPIU_RPMB_WLUN, 0,
        (u8 *)ufs_temp_buf, ufs_query_desc_max_size[QUERY_DESC_IDN_UNIT]);
    if (err) {
        dprintf(CRITICAL, "[UFS] err: %s: failed reading RPMB UNIT Desc. err = %d\n",
            __func__, err);
        free(ufs_temp_buf);
        return -1;
    }

    logical_blk_size = ufs_temp_buf[UNIT_DESC_PARAM_LOGICAL_BLK_SIZE];

    for (i = 0, logical_blk_size_bytes = 1; i < logical_blk_size; i++)
        logical_blk_size_bytes = logical_blk_size_bytes << 1;

    dprintf(CRITICAL, "[UFS] rpmb: logical_blk_size: %d bytes\n",
        (u32) logical_blk_size_bytes);

    logical_blk_cnt = ((u64)ufs_temp_buf[UNIT_DESC_PARAM_LOGICAL_BLK_COUNT] << 56) |
                  ((u64)ufs_temp_buf[UNIT_DESC_PARAM_LOGICAL_BLK_COUNT + 1] << 48) |
                  ((u64)ufs_temp_buf[UNIT_DESC_PARAM_LOGICAL_BLK_COUNT + 2] << 40) |
                  ((u64)ufs_temp_buf[UNIT_DESC_PARAM_LOGICAL_BLK_COUNT + 3] << 32) |
                  ((u64)ufs_temp_buf[UNIT_DESC_PARAM_LOGICAL_BLK_COUNT + 4] << 24) |
                  ((u64)ufs_temp_buf[UNIT_DESC_PARAM_LOGICAL_BLK_COUNT + 5] << 16) |
                  ((u64)ufs_temp_buf[UNIT_DESC_PARAM_LOGICAL_BLK_COUNT + 6] << 8) |
                  ((u64)ufs_temp_buf[UNIT_DESC_PARAM_LOGICAL_BLK_COUNT + 7]);

    dprintf(CRITICAL, "[UFS] rpmb: logical_blk_cnt: 0x%llx\n", logical_blk_cnt);
    card_data->rpmb_lu_size = logical_blk_size_bytes * logical_blk_cnt;

    /* Get rpmb rw size */
    memset(ufs_temp_buf, 0, ufs_query_desc_max_size[QUERY_DESC_IDN_GEOMETRY]);
    err = hba->read_descriptor(hba, QUERY_DESC_IDN_GEOMETRY, 0, 0,
        (u8 *)ufs_temp_buf, ufs_query_desc_max_size[QUERY_DESC_IDN_GEOMETRY]);
    if (err) {
        dprintf(CRITICAL, "[UFS] err: %s: failed reading GEOMETRY Desc. err = %d\n",
            __func__, err);
        free(ufs_temp_buf);
        return -1;
    }
    card_data->rpmb_rw_size  = (u32)ufs_temp_buf[GEOMETRY_DESC_PARAM_RPMB_READ_WRITE_SIZE];
    dprintf(CRITICAL, "[UFS] rpmb: read write size: %d\n", card_data->rpmb_rw_size);


    ufshcd_put_tag(hba, tag);

    free(ufs_temp_buf);

    dprintf(INFO, "[UFS] quirk_sel: 0x%x\n", hba->quirk_sel);
    dprintf(INFO, "[UFS] vendor id : 0x%x\n", hba->dev_info.wmanufacturerid);
    dprintf(INFO, "[UFS] ufs id : %s\n", hba->dev_info.product_id);
    dprintf(INFO, "[UFS] ufs fwver: %s\n", hba->dev_info.product_revision_level);
    dprintf(INFO, "[UFS] ufs serial number: %s\n", hba->dev_info.serial_number);
    dprintf(INFO, "[UFS] dev_quirk = 0x%x\n", hba->dev_quirks);

    return 0;
}

int ufs_get_lu_size(struct ufs_hba *hba, u32 lun, u32 *blk_size_in_byte, u32 *blk_cnt)
{
    struct ufs_scsi_cmd cmd;
    int tag;
    int ret;
    u8 *p;
    u32 blk_cnt_int, blk_size_int;
    u8 *ufs_temp_buf = memalign(CACHE_LINE, UFS_TEMP_BUF_SIZE);
    /* malloc fail */
    ASSERT(ufs_temp_buf);

    if (!ufshcd_get_free_tag(hba, &tag)) {
        free(ufs_temp_buf);
        return -1;
    }

    ufs_scsi_cmd_read_capacity(&cmd, (u32)tag, lun, (unsigned long *)ufs_temp_buf);
    ret = ufshcd_queuecommand(hba, &cmd);

    if (ret != UFS_ERR_NONE) {
        dprintf(CRITICAL, "[UFS] err: %s: ufshcd_queuecommand err\n", __func__);
        free(ufs_temp_buf);
        return ret;
    }

    ufshcd_put_tag(hba, tag);

    p = (u8 *)ufs_temp_buf;

    blk_cnt_int = (((u32)(p[0]) << 24) | ((u32)(p[1]) << 16) | ((u32)(p[2]) << 8) |
        (u32)(p[3])) + 1;

    if (blk_cnt)
        *blk_cnt = blk_cnt_int;

    blk_size_int = ((u32)(p[4]) << 24) | ((u32)(p[5]) << 16) | ((u32)(p[6]) << 8) | (u32)(p[7]);

    if (blk_size_in_byte)
        *blk_size_in_byte = blk_size_int;

    dprintf(CRITICAL, "[UFS] get lu size: lun=%d, blk_cnt=%d, blk_size_in_byte=%d\n",
                 lun, blk_cnt_int, blk_size_int);

    free(ufs_temp_buf);
    return ret;
}

void ufs_dbg_show_reg(struct ufs_hba *hba, u32 addr)
{
    u32 data;

    data = ufshcd_readl(hba, addr);

    LTRACEF("\t: 0x%x, 0x%x\n", addr, data);
}

int ufs_dma_dme_get(struct ufs_hba *hba, u32 attr_sel, u32 *mib_val)
{
    return ufshcd_dme_get_attr(hba, attr_sel, mib_val, DME_LOCAL);
}

int ufs_dma_dme_peer_get(struct ufs_hba *hba, u32 attr_sel, u32 *mib_val)
{
    return ufshcd_dme_get_attr(hba, attr_sel, mib_val, DME_PEER);
}

int ufs_dma_dme_set(struct ufs_hba *hba, u32 attr_sel, u32 mib_val)
{
    return ufshcd_dme_set_attr(hba, attr_sel, ATTR_SET_NOR, mib_val, DME_LOCAL);
}

int ufs_dma_dme_peer_set(struct ufs_hba *hba, u32 attr_sel, u32 mib_val)
{
    return ufshcd_dme_set_attr(hba, attr_sel, ATTR_SET_NOR, mib_val, DME_PEER);
}

int ufs_dma_nopin_nopout(struct ufs_hba *hba)
{
    int err = 0;
    int retries;

    for (retries = NOP_OUT_RETRIES; retries > 0; retries--) {
        err = ufshcd_exec_dev_cmd(hba, DEV_CMD_TYPE_NOP,
                                  NOP_OUT_TIMEOUT);

        if (!err)
            break;
    }

    if (err)
        dprintf(CRITICAL, "[UFS] err: NOP OUT failed\n");

    return err;
}

int ufs_dma_query_flag(struct ufs_hba *hba, enum query_opcode opcode,
                           enum flag_idn idn, bool *flag_res)
{
    struct ufs_query_req *request = NULL;
    struct ufs_query_res *response = NULL;
    int err, index = 0, selector = 0;

    ASSERT(hba);

    ufshcd_init_query(hba, &request, &response, opcode, idn, index,
                      selector);

    switch (opcode) {
    case UPIU_QUERY_OPCODE_SET_FLAG:
    case UPIU_QUERY_OPCODE_CLEAR_FLAG:
    case UPIU_QUERY_OPCODE_TOGGLE_FLAG:
        request->query_func = UPIU_QUERY_FUNC_STANDARD_WRITE_REQUEST;
        break;
    case UPIU_QUERY_OPCODE_READ_FLAG:
        request->query_func = UPIU_QUERY_FUNC_STANDARD_READ_REQUEST;
        if (!flag_res) {
            /* No dummy reads */
            LTRACEF("Invalid argument for read request\n");
            err = -1;
            goto out_unlock;
        }
        break;
    default:
        LTRACEF("Expected query flag opcode but got = %d\n", opcode);
        err = -1;
        goto out_unlock;
    }

    err = ufshcd_exec_dev_cmd(hba, DEV_CMD_TYPE_QUERY, QUERY_REQ_TIMEOUT);

    if (err) {
        LTRACEF("Sending flag query for idn %d failed, err = %d\n", idn, err);
        goto out_unlock;
    }

    if (flag_res)
        *flag_res = (SWAP_32(response->upiu_res.value) &
                     MASK_QUERY_UPIU_FLAG_LOC) & 0x1;

out_unlock:

    return err;
}

int ufs_dma_query_attr(struct ufs_hba *hba, enum query_opcode opcode,
                           enum attr_idn idn, u8 index, u8 selector, u32 *attr_val)
{
    struct ufs_query_req *request = NULL;
    struct ufs_query_res *response = NULL;
    int err;

    if (!attr_val) {
        LTRACEF("attribute value required for opcode 0x%x\n", opcode);
        err = -1;
        goto out;
    }

    ufshcd_init_query(hba, &request, &response, opcode, idn, index,    selector);

    switch (opcode) {
    case UPIU_QUERY_OPCODE_WRITE_ATTR:
        request->query_func = UPIU_QUERY_FUNC_STANDARD_WRITE_REQUEST;
        request->upiu_req.value = SWAP_32(*attr_val);
        break;
    case UPIU_QUERY_OPCODE_READ_ATTR:
        request->query_func = UPIU_QUERY_FUNC_STANDARD_READ_REQUEST;
        break;
    default:
        LTRACEF("Expected query attr opcode but got = 0x%.2x\n", opcode);
        err = -1;
        goto out;
    }

    err = ufshcd_exec_dev_cmd(hba, DEV_CMD_TYPE_QUERY, QUERY_REQ_TIMEOUT);

    if (err) {
        LTRACEF("opcode 0x%x for idn %d failed, err = %d\n", opcode, idn, err);
        goto out;
    }

    *attr_val = SWAP_32(response->upiu_res.value);

out:

    return err;
}

int ufs_dma_read_desc(struct ufs_hba *hba, enum desc_idn desc_id, int desc_index,
    u8 selector, u8 *buf, u32 size)
{
    return ufshcd_read_desc_param(hba, desc_id, desc_index, selector, 0, buf, size);
}

int ufs_dma_write_desc(struct ufs_hba *hba, enum desc_idn idn, int index, u8 selector,
    u8 *src_buf, u32 buf_len)
{
    u32 buf_len_local = buf_len;

    return ufshcd_query_descriptor(hba, UPIU_QUERY_OPCODE_WRITE_DESC, idn, index, selector,
        src_buf, &buf_len_local);
}

__WEAK void ufs_set_bootdevice(void)
{
/*
 * Previously, kernel cmdline is set in ufs_init_mphy() and might cause multiple
 * bootdevice definition. The new interface ufs_set_bootdevice() which is only called
 * after success hba probe is exported to replace ufs_commandline_bootdevice().
 *
 * To keep backward compatibility, this function does nothing unless implemented in
 * ufs_platform.c.
 */
    dprintf(ALWAYS, "[UFS] warn: platform %s() not implemeted\n", __func__);
}

