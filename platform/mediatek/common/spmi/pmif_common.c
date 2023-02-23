/*
 * Copyright (c) 2020 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#include <err.h>
#include <kernel/mutex.h>
#include <platform.h>
#include <platform/reg.h>
#include <platform/wait.h>
#include <pmif_common.h>
#include <spmi_common.h>
#include <string.h>
#include "spmi_sw.h"

#define PMIF_CMD_REG_0          0
#define PMIF_CMD_REG            1
#define PMIF_CMD_EXT_REG        2
#define PMIF_CMD_EXT_REG_LONG   3
/* macro for SWINF_FSM */
#define SWINF_FSM_IDLE          (0x00)
#define SWINF_FSM_REQ           (0x02)
#define SWINF_FSM_WFDLE         (0x04)
#define SWINF_FSM_WFVLDCLR      (0x06)

#define GET_SWINF_FSM(x)        ((x >> 1) & 0x7)
#define GET_PMIF_INIT_DONE(x)   ((x >> 15) & 0x1)
#define TIMEOUT_WAIT_IDLE_US    (1000) /* AP latency concern 1ms */

static mutex_t pmif_mutex = MUTEX_INITIAL_VALUE(pmif_mutex);

void pmif_spmi_enable_swinf(struct pmif *arb)
{
    write32(&mtk_pmif->inf_en, 0x1 << (arb->swinf_ch_start + arb->swinf_no));
    write32(&mtk_pmif->arb_en, 0x1 << (arb->swinf_ch_start + arb->swinf_no));
}

void pmif_spmi_enable_cmdIssue(void)
{
    /* Enable cmdIssue */
    write32(&mtk_pmif->cmdissue_en, 1);
}

static int pmif_check_state(int fsm_state, u32 timeout_us)
{
    lk_bigtime_t ret;

    ret = wait_us(GET_SWINF_FSM(read32(&mtk_pmif->swinf_sta)) == fsm_state,
                  timeout_us);
    if (!ret) {
        SPMI_ERR("check state %d timeout over %d\n", fsm_state, timeout_us);
        return ERR_TIMED_OUT;
    }
    return 0;
}

int is_pmif_init_done(void)
{
    if (read32(&mtk_pmif->init_done) & 0x1)
        return 0;

    return ERR_INVALID_ARGS;
}

int pmif_spmi_read_cmd(u8 opc, u8 sid, u16 addr, u8 *buf, u8 len)
{
    int ret;
    u32 data = 0;
    u8 bc = len - 1;

    if (sid > SPMI_MAX_SLAVE_ID || len > PMIF_BYTECNT_MAX)
        return ERR_INVALID_ARGS;

    /* Check the opcode */
    if (opc >= 0x60 && opc <= 0x7F)
        opc = PMIF_CMD_REG;
    else if (opc >= 0x20 && opc <= 0x2F)
        opc = PMIF_CMD_EXT_REG;
    else if (opc >= 0x38 && opc <= 0x3F)
        opc = PMIF_CMD_EXT_REG_LONG;
    else
        return ERR_INVALID_ARGS;

    mutex_acquire(&pmif_mutex);

    /* Wait for Software Interface FSM state to be IDLE. */
    ret = pmif_check_state(SWINF_FSM_IDLE, TIMEOUT_WAIT_IDLE_US);
    if (ret)
        goto done;

    /* Send the command. */
    write32(&mtk_pmif->swinf_acc,
        (opc << 30) | (sid << 24) | (bc << 16) | addr);
    /*
     * Wait for Software Interface FSM state to be WFVLDCLR,
     * read the data and clear the valid flag.
     */
    ret = pmif_check_state(SWINF_FSM_WFVLDCLR, TIMEOUT_WAIT_IDLE_US);
    if (ret)
        goto done;

    data = read32(&mtk_pmif->swinf_rdata);
    memcpy(buf, &data, (bc & 3) + 1);
    write32(&mtk_pmif->swinf_vld_clr, 0x1);

done:
    mutex_release(&pmif_mutex);
    return ret;
}

int pmif_spmi_write_cmd(u8 opc, u8 sid, u16 addr, const u8 *buf, u8 len)
{
    int ret;
    u32 data = 0;
    u8 bc = len - 1;

    if (sid > SPMI_MAX_SLAVE_ID || len > PMIF_BYTECNT_MAX)
        return ERR_INVALID_ARGS;

    /* Check the opcode */
    if (opc >= 0x40 && opc <= 0x5F)
        opc = PMIF_CMD_REG;
    else if (opc <= 0x0F)
        opc = PMIF_CMD_EXT_REG;
    else if (opc >= 0x30 && opc <= 0x37)
        opc = PMIF_CMD_EXT_REG_LONG;
    else if (opc >= 0x80)
        opc = PMIF_CMD_REG_0;
    else
        return ERR_INVALID_ARGS;

    mutex_acquire(&pmif_mutex);

    /* Wait for Software Interface FSM state to be IDLE. */
    ret = pmif_check_state(SWINF_FSM_IDLE, TIMEOUT_WAIT_IDLE_US);
    if (ret)
        goto done;

    /* Set the write data. */
    memcpy(&data, buf, (bc & 3) + 1);
    write32(&mtk_pmif->swinf_wdata, data);
    /* Send the command. */
    write32(&mtk_pmif->swinf_acc,
        (opc << 30) | (0x1 << 29) | (sid << 24) | (bc << 16) | addr);

done:
    mutex_release(&pmif_mutex);
    return ret;
}
