/*
 * Copyright (c) 2020 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#include <assert.h>
#include <err.h>
#include <platform.h>
#include <platform/reg.h>
#include <platform/wait.h>
#include <pmic_wrap_common.h>

#define GET_SWINF_FSM(x) ((x>>1)  & 0x00000007)

s32 check_idle(void *addr, u32 expected)
{
    lk_bigtime_t ret;

    ret = wait_us(read32(addr) != expected, TIMEOUT_WAIT_IDLE_US);
    if (!ret)
        return ERR_TIMED_OUT;

    return 0;
}

s32 pwrap_wacs2(u32 write, u16 addr, u16 wdata, u16 *rdata)
{
    u32 reg_rdata = 0x0;

    /* Check argument validation */
    if ((write & ~(0x1)) != 0)
        return E_PWR_INVALID_RW;
    if ((addr & ~(0xffff)) != 0)
        return E_PWR_INVALID_ADDR;
    if ((wdata & ~(0xffff)) != 0)
        return E_PWR_INVALID_WDAT;

    /* Wait for Software Interface FSM state to be IDLE */
    do {
        reg_rdata = read32(&mtk_pwrap->swinf_sta);
    } while (GET_SWINF_FSM(reg_rdata) != 0x0);

    /* Set the write data */
    if (write == 1)
        write32(&mtk_pwrap->swinf_wdata, wdata);

    /* Send the command */
    write32(&mtk_pwrap->swinf_acc, (write << 29) | addr);

    if (write == 0) {
        /* Wait for Software Interface FSM to be WFVLDCLR,
         * read the data and clear the valid flag
         */
        do {
            reg_rdata = read32(&mtk_pwrap->swinf_sta);
        } while (GET_SWINF_FSM(reg_rdata) != 0x6);

        *rdata = read32(&mtk_pwrap->swinf_rdata);

        write32(&mtk_pwrap->swinf_vld_clr, 0x1);
    }

    return 0;
}
