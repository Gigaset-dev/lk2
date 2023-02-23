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

#include <assert.h>
#include <platform.h>
#include <platform/reg.h>
#include <pmic_wrap_common.h>

static u32 wait_for_state_idle(loop_condition_fp fp, u32 timeout_us,
             void *wacs_register, void *wacs_vldclr_register, u32 *read_reg)
{
    u32 reg_rdata;
    lk_bigtime_t start = current_time_hires();

    do {
        reg_rdata = read32((wacs_register));
        /* if last read command timeout,clear vldclr bit
         * read command state machine:FSM_REQ-->wfdle-->WFVLDCLR;
         * write:FSM_REQ-->idle
         */
        switch (((reg_rdata >> RDATA_WACS_FSM_SHIFT) &
            RDATA_WACS_FSM_MASK)) {
            case WACS_FSM_WFVLDCLR:
                write32(wacs_vldclr_register, 1);
                pwrap_err("WACS_FSM = PMIC_WRAP_WACS_VLDCLR\n");
                break;
            case WACS_FSM_WFDLE:
                pwrap_err("WACS_FSM = WACS_FSM_WFDLE\n");
                break;
            case WACS_FSM_REQ:
                pwrap_err("WACS_FSM = WACS_FSM_REQ\n");
                break;
            default:
                break;
        }

        if ((current_time_hires() - start) > timeout_us)
            return E_PWR_WAIT_IDLE_TIMEOUT;

    } while (fp(reg_rdata));  /* IDLE State */
    if (read_reg)
        *read_reg = reg_rdata;
    return 0;
}

static u32 wait_for_state_ready(loop_condition_fp fp, u32 timeout_us,
             void *wacs_register, u32 *read_reg)
{
    u32 reg_rdata;
    lk_bigtime_t start = current_time_hires();

    do {
        reg_rdata = read32((wacs_register));
        if ((current_time_hires() - start) > timeout_us)
            return E_PWR_WAIT_IDLE_TIMEOUT;
    } while (fp(reg_rdata));  /* IDLE State */
    if (read_reg)
        *read_reg = reg_rdata;
    return 0;
}

s32 pwrap_wacs2(u32 write, u16 addr, u16 wdata, u16 *rdata)
{
    u32 reg_rdata = 0;
    u32 wacs_write = 0;
    u32 wacs_addr = 0;
    u32 wacs_cmd = 0;
    u32 wait_result = 0;

    if (write == 0) {
        if (rdata == NULL) {
            pwrap_err("rdata is a NULL pointer\n");
            return E_PWR_INVALID_ARG;
        }
    }

    /* Check IDLE in advance */
    wait_result = wait_for_state_idle(wait_for_fsm_idle,
                                      TIMEOUT_WAIT_IDLE_US,
                                      &mtk_pwrap->wacs2_rdata,
                                      &mtk_pwrap->wacs2_vldclr,
                                      0);
    if (wait_result != 0) {
        pwrap_err("wait_for_fsm_idle fail,wait_result=%d\n",
              wait_result);
        return E_PWR_WAIT_IDLE_TIMEOUT;
    }
    wacs_write = write << 31;
    wacs_addr = (addr >> 1) << 16;
    wacs_cmd = wacs_write | wacs_addr | wdata;

    write32(&mtk_pwrap->wacs2_cmd, wacs_cmd);

    if (write == 0) {
        wait_result = wait_for_state_ready(wait_for_fsm_vldclr,
                                           TIMEOUT_READ_US,
                                           &mtk_pwrap->wacs2_rdata,
                                           &reg_rdata);
        if (wait_result != 0) {
            pwrap_err("wait_for_fsm_vldclr fail,wait_result=%d\n",
                  wait_result);
            return E_PWR_WAIT_IDLE_TIMEOUT_READ;
        }
        *rdata = ((reg_rdata >> RDATA_WACS_RDATA_SHIFT)
                  & RDATA_WACS_RDATA_MASK);
        write32(&mtk_pwrap->wacs2_vldclr, 1);
    }

    return 0;
}

