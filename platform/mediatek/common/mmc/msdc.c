/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#include <assert.h>
#include <cust_msdc.h>
#include <kernel/event.h>
#include <kernel/vm.h>
#include <mmc_core.h>
#include <msdc.h>
#include <msdc_io_layout.h>
#include <mt_gic.h>
#include <platform/interrupts.h>
#include <platform/irq.h>
#include <string.h>
#include <trace.h>

#include "mmc_cmd.h"
#include "mmc_rpmb.h"
#include "msdc_reg.h"

#define LOCAL_TRACE 0

#define CMD_RETRIES        (5)
#define CMD_TIMEOUT        (100) /* 100ms */

#define PERI_MSDC_SRCSEL   (0xc100000c)

/* Tuning Parameter */
#define DEFAULT_DEBOUNCE   (8)  /* 8 cycles */
#define DEFAULT_DTOC       (40) /* data timeout counter. 65536x40 sclk. */
#define DEFAULT_WDOD       (0)  /* write data output delay. no delay. */
#define DEFAULT_BSYDLY     (8)  /* card busy delay. 8 extend sclk */

/* Declarations */
static int msdc_send_cmd(struct mmc_host *host, struct mmc_command *cmd);
static int msdc_wait_cmd_done(struct mmc_host *host, struct mmc_command *cmd);
static int msdc_tune_cmdrsp(struct mmc_host *host, struct mmc_command *cmd);

struct msdc_priv_t {
    int    autocmd;
    int    rdsmpl;
    int    wdsmpl;
    int    rsmpl;
    int    start_bit;
    int    cmd23_flags;
};

static int msdc_rsp[] = {
    0,  /* RESP_NONE */
    1,  /* RESP_R1 */
    2,  /* RESP_R2 */
    3,  /* RESP_R3 */
    4,  /* RESP_R4 */
    1,  /* RESP_R5 */
    1,  /* RESP_R6 */
    1,  /* RESP_R7 */
    7,  /* RESP_R1b */
};

static event_t msdc_int_event;
static u32 g_int_status;
static struct msdc_priv_t msdc_priv;

static void msdc_set_startbit(struct mmc_host *host, u8 start_bit)
{
    addr_t base = host->base;
    struct msdc_priv_t *priv = (struct msdc_priv_t *)host->priv;

    /* set start bit */
    MSDC_SET_FIELD(MSDC_CFG, MSDC_CFG_START_BIT, start_bit);
    priv->start_bit = start_bit;
    dprintf(INFO, "start bit = %d, MSDC_CFG[0x%x]\n", start_bit, MSDC_READ32(MSDC_CFG));
}

#define TYPE_CMD_RESP_EDGE      (0)
#define TYPE_WRITE_CRC_EDGE     (1)
#define TYPE_READ_DATA_EDGE     (2)
#define TYPE_WRITE_DATA_EDGE    (3)

static void msdc_set_smpl(struct mmc_host *host, u8 HS400, u8 mode, u8 type)
{
    addr_t base = host->base;
    int i = 0;
    struct msdc_priv_t *priv = (struct msdc_priv_t *)host->priv;
    static u8 read_data_edge[8] = {MSDC_SMPL_RISING, MSDC_SMPL_RISING, MSDC_SMPL_RISING,
                                      MSDC_SMPL_RISING, MSDC_SMPL_RISING, MSDC_SMPL_RISING,
                                      MSDC_SMPL_RISING, MSDC_SMPL_RISING};
    static u8 write_data_edge[4] = {MSDC_SMPL_RISING, MSDC_SMPL_RISING,
        MSDC_SMPL_RISING, MSDC_SMPL_RISING};

    switch (type) {
    case TYPE_CMD_RESP_EDGE:
        if (HS400) {
            // eMMC5.0 only output resp at CLK pin, so no need to select DS pin
            /* latch cmd resp at CLK pin */
            MSDC_SET_FIELD(EMMC50_CFG0, MSDC_EMMC50_CFG_PADCMD_LATCHCK, 0);
            /* latch cmd resp at CLK pin */
            MSDC_SET_FIELD(EMMC50_CFG0, MSDC_EMMC50_CFG_CMD_RESP_SEL, 0);
        }

        if (mode == MSDC_SMPL_RISING || mode == MSDC_SMPL_FALLING) {
            MSDC_SET_FIELD(MSDC_IOCON, MSDC_IOCON_RSPL, mode);
            priv->rsmpl = mode;
        } else {
            dprintf(CRITICAL, "[%s]: invalid resp parameter: HS400=%d, type=%d, mode=%d\n",
                __func__, HS400, type, mode);
        }
        break;

    case TYPE_WRITE_CRC_EDGE:
        if (HS400) {
            /* latch write crc status at DS pin */
            MSDC_SET_FIELD(EMMC50_CFG0, MSDC_EMMC50_CFG_CRC_STS_SEL, 1);
        } else {
            /* latch write crc status at CLK pin */
            MSDC_SET_FIELD(EMMC50_CFG0, MSDC_EMMC50_CFG_CRC_STS_SEL, 0);
        }

        if (mode == MSDC_SMPL_RISING || mode == MSDC_SMPL_FALLING) {
            if (HS400) {
                MSDC_SET_FIELD(EMMC50_CFG0, MSDC_EMMC50_CFG_CRC_STS_EDGE, mode);
            } else {
                MSDC_SET_FIELD(MSDC_IOCON, MSDC_IOCON_W_D_SMPL_SEL, 0);
                MSDC_SET_FIELD(MSDC_IOCON, MSDC_IOCON_W_D_SMPL, mode);
            }
            priv->wdsmpl = mode;
        } else if (mode == MSDC_SMPL_SEPARATE && !HS400) {
            /* only dat0 is for write crc status. */
            MSDC_SET_FIELD(MSDC_IOCON, MSDC_IOCON_W_D0SPL, write_data_edge[0]);
            priv->wdsmpl = mode;
        } else {
            dprintf(CRITICAL, "[%s]: invalid crc parameter: HS400=%d, type=%d, mode=%d\n",
                __func__, HS400, type, mode);
        }
        break;

    case TYPE_READ_DATA_EDGE:
        if (HS400) {
            /* for HS400, start bit is output both on rising and falling edge */
            msdc_set_startbit(host, START_AT_RISING_AND_FALLING);
            priv->start_bit = START_AT_RISING_AND_FALLING;
        } else {
           /*
            * for the other mode, start bit is only output on rising edge.
            * but DDR50 can try falling edge if error casued by pad delay
            */
            msdc_set_startbit(host, START_AT_RISING);
            priv->start_bit = START_AT_RISING;
        }
        if (mode == MSDC_SMPL_RISING || mode == MSDC_SMPL_FALLING) {
            MSDC_SET_FIELD(MSDC_IOCON, MSDC_IOCON_R_D_SMPL_SEL, 0);
            MSDC_SET_FIELD(MSDC_IOCON, MSDC_IOCON_R_D_SMPL, mode);
            priv->rdsmpl = mode;
        } else if (mode == MSDC_SMPL_SEPARATE) {
            MSDC_SET_FIELD(MSDC_IOCON, MSDC_IOCON_R_D_SMPL_SEL, 1);
            for (i = 0; i < 8; i++)
                MSDC_SET_FIELD(MSDC_IOCON, (MSDC_IOCON_R_D0SPL << i), read_data_edge[i]);
            priv->rdsmpl = mode;
        } else {
            dprintf(CRITICAL, "[%s]: invalid read parameter: HS400=%d, type=%d, mode=%d\n",
                __func__, HS400, type, mode);
        }
        break;

    case TYPE_WRITE_DATA_EDGE:
        /* latch write crc status at CLK pin */
        MSDC_SET_FIELD(EMMC50_CFG0, MSDC_EMMC50_CFG_CRC_STS_SEL, 0);

        if (mode == MSDC_SMPL_RISING || mode == MSDC_SMPL_FALLING) {
            MSDC_SET_FIELD(MSDC_IOCON, MSDC_IOCON_W_D_SMPL_SEL, 0);
            MSDC_SET_FIELD(MSDC_IOCON, MSDC_IOCON_W_D_SMPL, mode);
            priv->wdsmpl = mode;
        } else if (mode == MSDC_SMPL_SEPARATE) {
            MSDC_SET_FIELD(MSDC_IOCON, MSDC_IOCON_W_D_SMPL_SEL, 1);
            for (i = 0; i < 4; i++) {
                /* dat0~4 is for SDIO card. */
                MSDC_SET_FIELD(MSDC_IOCON, (MSDC_IOCON_W_D0SPL << i), write_data_edge[i]);
            }
            priv->wdsmpl = mode;
        } else {
            dprintf(CRITICAL, "[%s]: invalid write parameter: HS400=%d, type=%d, mode=%d\n",
                __func__, HS400, type, mode);
        }
        break;

    default:
        dprintf(CRITICAL, "[%s]: invalid parameter: HS400=%d, type=%d, mode=%d\n",
            __func__, HS400, type, mode);
        break;
    }
}

void msdc_set_timeout(struct mmc_host *host, u32 ns, u32 clks)
{
    addr_t base = host->base;
    u32 timeout, clk_ns;
    u32 mode = 0;

    if (host->cur_bus_clk == 0) {
        timeout = 0;
    } else {
        clk_ns  = 1000000000UL / host->cur_bus_clk;
        timeout = (ns + clk_ns - 1) / clk_ns + clks;
        timeout = (timeout + (1 << 20) - 1) >> 20; /* in 1048576 sclk cycle unit */
        MSDC_GET_FIELD(MSDC_CFG, MSDC_CFG_CKMOD, mode);
        /* DDR mode will double the clk cycles for data timeout */
        timeout = mode >= 2 ? timeout * 2 : timeout;
        timeout = timeout > 1 ? timeout - 1 : 0;
        timeout = timeout > 255 ? 255 : timeout;
    }
    MSDC_SET_FIELD(SDC_CFG, SDC_CFG_DTOC, timeout);
    LTRACEF("[MSDC] Set read data timeout: %dns %dclks -> %d (65536 sclk cycles)\n",
            ns, clks, timeout + 1);
}

void msdc_set_autocmd(struct mmc_host *host, int cmd)
{
    struct msdc_priv_t *priv = (struct msdc_priv_t *)host->priv;

    priv->autocmd = cmd;
}

int msdc_get_autocmd(struct mmc_host *host)
{
    struct msdc_priv_t *priv = (struct msdc_priv_t *)host->priv;

    return priv->autocmd;
}

void msdc_set_reliable_write(struct mmc_host *host, int on)
{
    struct msdc_priv_t *priv = (struct msdc_priv_t *)host->priv;

    if (on)
        priv->cmd23_flags |= MSDC_RELIABLE_WRITE;
    else
        priv->cmd23_flags &= ~MSDC_RELIABLE_WRITE;
}

void msdc_set_max_phys_segs(struct mmc_host *host)
{
    if (msdc_get_autocmd(host) == MSDC_AUTOCMD23)
        /* The maximal transferring size is size of *[15:0] number of blocks */
        host->max_phys_segs = 0xffff;
    else
        /* The maximal transferring size is size of DMA_LENGTH */
        host->max_phys_segs = (UINT_MAX & ~511) >> MMC_BLOCK_BITS_SHFT;
}

static void msdc_abort(struct mmc_host *host)
{
    addr_t base = host->base;

    dprintf(CRITICAL, "[MSDC] Abort: MSDC_FIFOCS=%xh MSDC_PS=%xh SDC_STS=%xh\n",
            MSDC_READ32(MSDC_FIFOCS), MSDC_READ32(MSDC_PS), MSDC_READ32(SDC_STS));
    msdc_dump_padctl_by_id(host->id);
    msdc_dump_clock_sts(host);

    /* reset controller */
    MSDC_RESET();

    /* clear fifo */
    MSDC_CLR_FIFO();

    /* make sure txfifo and rxfifo are empty */
    if (MSDC_TXFIFOCNT() != 0 || MSDC_RXFIFOCNT() != 0) {
        dprintf(CRITICAL, "[MSDC] Abort: TXFIFO(%d), RXFIFO(%d) != 0\n",
                MSDC_TXFIFOCNT(), MSDC_RXFIFOCNT());
    }

    /* clear all interrupts */
    MSDC_CLR_INT();
}

static int msdc_get_card_status(struct mmc_host *host, u32 *status)
{
    int err;
    struct mmc_command cmd;

    cmd.opcode  = MMC_CMD_SEND_STATUS;
    cmd.arg     = host->card->rca << 16;
    cmd.rsptyp  = RESP_R1;
    cmd.retries = CMD_RETRIES;
    cmd.timeout = CMD_TIMEOUT;

    err = msdc_send_cmd(host, &cmd);
    if (!err)
        err = msdc_wait_cmd_done(host, &cmd);

    if (err == MMC_ERR_NONE)
        *status = cmd.resp[0];

    return err;
}

int msdc_abort_handler(struct mmc_host *host, int abort_card)
{
    struct mmc_command stop;
    u32 status = 0;
    u32 state = 0;

    while (state != 4) { // until status to "tran"
        msdc_abort(host);
        if (msdc_get_card_status(host, &status)) {
            dprintf(CRITICAL, "Get card status failed\n");
            return 1;
        }
        state = R1_CURRENT_STATE(status);
        dprintf(INFO, "check card state<%d>\n", state);
        if (state == 5 || state == 6) {
            dprintf(INFO, "state<%d> need cmd12 to stop\n", state);
            if (abort_card) {
                stop.opcode  = MMC_CMD_STOP_TRANSMISSION;
                stop.rsptyp  = RESP_R1B;
                stop.arg     = 0;
                stop.retries = CMD_RETRIES;
                stop.timeout = CMD_TIMEOUT;
                msdc_send_cmd(host, &stop);
                msdc_wait_cmd_done(host, &stop); // don't tuning
            } else if (state == 7) {  // busy in programing
                dprintf(INFO, "state<%d> card is busy\n", state);
                mdelay(100);
            } else if (state != 4) {
                dprintf(INFO, "state<%d> ???\n", state);
                return 1;
            }
        }
    }
    msdc_abort(host);
    return 0;
}

static u32 msdc_intr_wait(struct mmc_host *host, u32 intrs)
{
    u32 sts = 0;
    u32 tmo = UINT_MAX;
    int ret = 0;

    /* warning that interrupts are not enabled */
    ret = event_wait_timeout(&msdc_int_event, tmo);
    if (ret != 0) {
        addr_t base = host->base;

        dprintf(CRITICAL, "[%s]: failed to get event INT=0x%x\n",
                __func__, MSDC_READ32(MSDC_INT));
        g_int_status = 0;
        return 0;
    }

    sts = g_int_status;
    g_int_status = 0;

    if (~intrs & sts)
        dprintf(CRITICAL, "[MSDC]<CHECKME> Unexpected INT(0x%x)\n", ~intrs & sts);

    return sts;
}

static enum handler_return msdc_interrupt_handler(void *arg)
{
    struct mmc_host *host = arg;
    addr_t base = host->base;

    /* Save & Clear the interrupt */
    MSDC_WRITE32(MSDC_INTEN, 0);
    g_int_status = MSDC_READ32(MSDC_INT);
    MSDC_WRITE32(MSDC_INT, g_int_status & host->intr_mask);
    host->intr_mask = 0;

    /* MUST BE *false*! otherwise, schedule in interrupt */
    event_signal(&msdc_int_event, false);

    return INT_RESCHEDULE;
}

static int msdc_send_cmd(struct mmc_host *host, struct mmc_command *cmd)
{
    struct msdc_priv_t *priv = (struct msdc_priv_t *)host->priv;
    addr_t base   = host->base;
    u32 opcode = cmd->opcode;
    u32 rsptyp = cmd->rsptyp;
    u32 rawcmd;
    u32 error = MMC_ERR_NONE;
    u32 blknum;

    /* rawcmd :
     * vol_swt << 30 | auto_cmd << 28 | blklen << 16 | go_irq << 15 |
     * stop << 14 | rw << 13 | dtype << 11 | rsptyp << 7 | brk << 6 | opcode
     */
    rawcmd = (opcode & ~(SD_CMD_BIT | SD_CMD_APP_BIT)) |
             msdc_rsp[rsptyp] << 7 | host->blklen << 16;

    if (opcode == MMC_CMD_WRITE_MULTIPLE_BLOCK) {
        rawcmd |= ((2 << 11) | (1 << 13));
        if (priv->autocmd & MSDC_AUTOCMD12)
            rawcmd |= (1 << 28);
        else if (priv->autocmd & MSDC_AUTOCMD23)
            rawcmd |= (2 << 28);
    } else if (opcode == MMC_CMD_WRITE_BLOCK || opcode == MMC_CMD50) {
        rawcmd |= ((1 << 11) | (1 << 13));
    } else if (opcode == MMC_CMD_READ_MULTIPLE_BLOCK) {
        rawcmd |= (2 << 11);
        if (priv->autocmd & MSDC_AUTOCMD12)
            rawcmd |= (1 << 28);
        else if (priv->autocmd & MSDC_AUTOCMD23)
            rawcmd |= (2 << 28);
    } else if (opcode == MMC_CMD_READ_SINGLE_BLOCK ||
               opcode == SD_ACMD_SEND_SCR ||
               opcode == SD_CMD_SWITCH ||
               opcode == MMC_CMD_SEND_EXT_CSD ||
               opcode == MMC_CMD_SEND_WRITE_PROT ||
               opcode == MMC_CMD_SEND_WRITE_PROT_TYPE ||
               opcode == MMC_CMD21) {
        rawcmd |= (1 << 11);
    } else if (opcode == MMC_CMD_STOP_TRANSMISSION) {
        rawcmd |= (1 << 14);
        rawcmd &= ~(0x0FFF << 16);
    } else if (opcode == SD_IO_RW_EXTENDED) {
        if (cmd->arg & 0x80000000)  /* R/W flag */
            rawcmd |= (1 << 13);
        if ((cmd->arg & 0x08000000) && ((cmd->arg & 0x1FF) > 1))
            rawcmd |= (2 << 11); /* multiple block mode */
        else
            rawcmd |= (1 << 11);
    } else if (opcode == SD_IO_RW_DIRECT) {
        if ((cmd->arg & 0x80000000) && ((cmd->arg >> 9) & 0x1FFFF))/* I/O abt */
            rawcmd |= (1 << 14);
    } else if (opcode == SD_CMD_VOL_SWITCH) {
        rawcmd |= (1 << 30);
    } else if (opcode == SD_CMD_SEND_TUNING_BLOCK) {
        rawcmd |= (1 << 11); /* CHECKME */
        if (priv->autocmd & MSDC_AUTOCMD19)
            rawcmd |= (3 << 28);
    } else if (opcode == MMC_CMD_GO_IRQ_STATE) {
        rawcmd |= (1 << 15);
    } else if (opcode == MMC_CMD_WRITE_DAT_UNTIL_STOP) {
        rawcmd |= ((1 << 13) | (3 << 11));
    } else if (opcode == MMC_CMD_READ_DAT_UNTIL_STOP) {
        rawcmd |= (3 << 11);
    }

    if (host->card && mmc_card_mmc(host->card)) {
        blknum = MSDC_READ32(SDC_BLK_NUM);
        if (priv->cmd23_flags & MSDC_RELIABLE_WRITE) {
            blknum |= (1U << 31);
            blknum &= ~(1 << 30);
        }
        MSDC_WRITE32(SDC_BLK_NUM, blknum);
    }

    LTRACEF("+[MSDC%d] CMD(%d): ARG(0x%x), RAW(0x%x), BLK_NUM(0x%x) RSP(%d)\n", host->id,
            (opcode & ~(SD_CMD_BIT | SD_CMD_APP_BIT)), cmd->arg, rawcmd,
            MSDC_READ32(SDC_BLK_NUM), rsptyp);

    while (SDC_IS_CMD_BUSY())
        ;
    if ((rsptyp == RESP_R1B) || (opcode == MMC_CMD_WRITE_MULTIPLE_BLOCK) ||
            opcode == MMC_CMD_WRITE_BLOCK || opcode == MMC_CMD_READ_MULTIPLE_BLOCK ||
            opcode == MMC_CMD_READ_SINGLE_BLOCK)
        while (SDC_IS_BUSY())
            ;

    SDC_SEND_CMD(rawcmd, cmd->arg);

end:
    cmd->error = error;

    return error;
}

static int msdc_wait_cmd_done(struct mmc_host *host, struct mmc_command *cmd)
{
    addr_t base   = host->base;
    u32 rsptyp = cmd->rsptyp;
    u32 status;
    u32 opcode = (cmd->opcode & ~(SD_CMD_BIT | SD_CMD_APP_BIT));
    u32 error = MMC_ERR_NONE;
    u32 wints = MSDC_INT_CMDTMO | MSDC_INT_CMDRDY | MSDC_INT_RSPCRCERR |
                MSDC_INT_ACMDRDY | MSDC_INT_ACMDCRCERR | MSDC_INT_ACMDTMO;
    u32 *resp = &cmd->resp[0];
    struct msdc_priv_t *priv = (struct msdc_priv_t *)host->priv;

    while (1) {
        /* Wait for interrupt coming */
        while (((status = MSDC_READ32(MSDC_INT)) & wints) == 0)
            ;
        MSDC_WRITE32(MSDC_INT, (status & wints));
        if (~wints & status)
            dprintf(CRITICAL, "[MSDC]<CHECKME> Unexpected INT(0x%x)\n",
                    ~wints & status);

        if (status & MSDC_INT_CMDRDY)
            break;
        else if (status & MSDC_INT_RSPCRCERR) {
            if (opcode != MMC_CMD21)
                dprintf(CRITICAL, "[MSDC] cmd%d CRCERR! (0x%x)\n", opcode, status);
            error = MMC_ERR_BADCRC;
            goto err;
        } else if (status & MSDC_INT_CMDTMO) {
            dprintf(CRITICAL, "[MSDC] cmd%d TMO! (0x%x)\n", opcode, status);
            error = MMC_ERR_TIMEOUT;
            goto err;
        } else if (priv->autocmd & MSDC_AUTOCMD23) {
            if (status & MSDC_INT_ACMDRDY)
                /* Autocmd rdy is set prior to cmd rdy */
                continue;
            else if (status & MSDC_INT_ACMDCRCERR) {
                dprintf(CRITICAL, "[MSDC] autocmd23 CRCERR! (0x%x)\n", status);
                error = MMC_ERR_ACMD_RSPCRC;
                goto err;
            } else if (status & MSDC_INT_ACMDTMO) {
                dprintf(CRITICAL, "[MSDC] autocmd23 TMO! (0x%x)\n", status);
                error = MMC_ERR_ACMD_TIMEOUT;
                goto err;
            }
        } else {
            dprintf(CRITICAL, "[MSDC] cmd%d UNEXPECT status! (0x%x)\n",
                    opcode, status);
            error = MMC_ERR_UNEXPECT;
            goto err;
        }
    }

    switch (rsptyp) {
    case RESP_NONE:
        LTRACEF("-[MSDC] CMD(%d): RSP(%d)\n",
                opcode, rsptyp);
        break;
    case RESP_R2:
        *resp++ = MSDC_READ32(SDC_RESP3);
        *resp++ = MSDC_READ32(SDC_RESP2);
        *resp++ = MSDC_READ32(SDC_RESP1);
        *resp++ = MSDC_READ32(SDC_RESP0);
        LTRACEF("-[MSDC] CMD(%d): RSP(%d) = 0x%x 0x%x 0x%x 0x%x\n",
                opcode, cmd->rsptyp, cmd->resp[0], cmd->resp[1],
                cmd->resp[2], cmd->resp[3]);
        break;
    default: /* Response types 1, 3, 4, 5, 6, 7(1b) */
        cmd->resp[0] = MSDC_READ32(SDC_RESP0);
        LTRACEF("-[MSDC] CMD(%d): RSP(%d) = 0x%x\n",
                opcode, cmd->rsptyp, cmd->resp[0]);
        break;
    }

err:
    if (rsptyp == RESP_R1B)
        while ((MSDC_READ32(MSDC_PS) & MSDC_PS_DAT0) != MSDC_PS_DAT0)
            ;

    cmd->error = error;

    return error;
}

int msdc_cmd(struct mmc_host *host, struct mmc_command *cmd)
{
    int err;

    err = msdc_send_cmd(host, cmd);
    if (err != MMC_ERR_NONE)
        return err;

    err = msdc_wait_cmd_done(host, cmd);

    /*
     * For CMD21 resp CRC error, sitll need receive data, so MUST not
     * clear fifo or do host reset
     */
    if (err && cmd->opcode != MMC_CMD21) {
        addr_t base = host->base;
        u32 tmp = MSDC_READ32(SDC_CMD);

        /* check if data is used by the command or not */
        if (tmp & SDC_CMD_DTYP) {
            if (msdc_abort_handler(host, 1))
                dprintf(CRITICAL, "[MSDC] abort failed\n");
        }

        if (cmd->opcode == MMC_CMD_APP_CMD ||
                cmd->opcode == SD_CMD_SEND_IF_COND) {
            if (err ==  MMC_ERR_TIMEOUT)
                return err;
        }

        err = msdc_tune_cmdrsp(host, cmd);
    }

    return err;
}

#ifdef MSDC_USE_DMA_MODE
static void msdc_flush_membuf(void *buf, u32 len)
{
    arch_clean_invalidate_cache_range((addr_t)buf, len);
}

static int msdc_dma_wait_done(struct mmc_host *host, struct mmc_command *cmd)
{
    addr_t base = host->base;
    struct msdc_priv_t *priv = (struct msdc_priv_t *)host->priv;
    u32 status;
    u32 error = MMC_ERR_NONE;
    u32 wints = MSDC_INT_XFER_COMPL | MSDC_INT_DATTMO | MSDC_INT_DATCRCERR |
                MSDC_INT_DXFER_DONE | MSDC_INT_DMAQ_EMPTY |
                MSDC_INT_ACMDRDY | MSDC_INT_ACMDTMO | MSDC_INT_ACMDCRCERR |
                MSDC_INT_CMDRDY | MSDC_INT_CMDTMO | MSDC_INT_RSPCRCERR;

    /* Deliver it to irq handler */
    host->intr_mask = wints;

    do {
        status = msdc_intr_wait(host, wints);

        if (status & MSDC_INT_XFER_COMPL) {
            if (mmc_op_multi(cmd->opcode) && (priv->autocmd & MSDC_AUTOCMD12)) {
                /* acmd rdy should be checked after xfer_done been held */
                if (status & MSDC_INT_ACMDRDY) {
                    break;
                } else if (status & MSDC_INT_ACMDTMO) {
                    dprintf(CRITICAL, "[MSDC] ACMD12 timeout(%xh)\n", status);
                    error = MMC_ERR_ACMD_TIMEOUT;
                    goto end;
                } else if (status & MSDC_INT_ACMDCRCERR) {
                    dprintf(CRITICAL, "[MSDC] ACMD12 CRC error(%xh)\n", status);
                    error = MMC_ERR_ACMD_RSPCRC;
                    goto end;
                }
            } else
                break;
        }

        if (status == 0 || status & MSDC_INT_DATTMO) {
            dprintf(CRITICAL, "[MSDC] DMA DAT timeout(%xh)\n", status);
            error = MMC_ERR_TIMEOUT;
            goto end;
        } else if (status & MSDC_INT_DATCRCERR) {
            if (cmd->opcode != MMC_CMD21)
                dprintf(CRITICAL, "[MSDC] DMA DAT CRC error(%xh)\n", status);
            error = MMC_ERR_BADCRC;
            goto end;
        } else {
            dprintf(CRITICAL, "[MSDC] Unexpect status(0x%x)\n", status);
            error = MMC_ERR_UNEXPECT;
            goto end;
        }
    } while (1);

end:
    if (error)
        MSDC_RESET();

    return error;
}

#ifndef upper_32_bits
#define upper_32_bits(n) ((u32)(((n) >> 16) >> 16))
#endif

#ifndef lower_32_bits
#define lower_32_bits(n) ((u32)((n) & 0xffffffff))
#endif

int msdc_dma_transfer(struct mmc_host *host, struct mmc_data *data)
{
    addr_t base = host->base;
    int err;
    paddr_t pa;

    /* Set dma timeout */
    msdc_set_timeout(host, data->timeout * 1000000, 0);
    /* DRAM address */
#if WITH_KERNEL_VM
    pa = vaddr_to_paddr(data->buf);
#else
    pa = (paddr_t)(data->buf);
#endif

    MSDC_WRITE32(MSDC_DMA_SA, (u32)(lower_32_bits(pa)));
    MSDC_WRITE32(MSDC_DMA_SA_HIGH, (u32)(upper_32_bits(pa)));

    MSDC_SET_FIELD(MSDC_DMA_CTRL, MSDC_DMA_CTRL_BURSTSZ, MSDC_DMA_BURST_64B);
    /* BASIC_DMA mode */
    MSDC_SET_FIELD(MSDC_DMA_CTRL, MSDC_DMA_CTRL_MODE, 0);
    /* This is the last buffer */
    MSDC_SET_FIELD(MSDC_DMA_CTRL, MSDC_DMA_CTRL_LASTBUF, 1);
    /* Total transfer size */
    MSDC_WRITE32(MSDC_DMA_LEN, data->blks * host->blklen);
    /* Set interrupts bit */
    MSDC_SET_BIT32(MSDC_INTEN,
                   MSDC_INT_XFER_COMPL | MSDC_INT_DATTMO | MSDC_INT_DATCRCERR);
    /* Clean & Invalidate cache */
    msdc_flush_membuf(data->buf, data->blks * host->blklen);
    /* Trigger DMA start */
    MSDC_SET_FIELD(MSDC_DMA_CTRL, MSDC_DMA_CTRL_START, 1);
    /* wait DMA transferring done */
    err = msdc_dma_wait_done(host, data->cmd);
    msdc_flush_membuf(data->buf, data->blks * host->blklen);
    /* Check DMA status and stop DMA transfer */
    MSDC_SET_FIELD(MSDC_DMA_CTRL, MSDC_DMA_CTRL_STOP, 1);
    while (MSDC_READ32(MSDC_DMA_CFG) & MSDC_DMA_CFG_STS)
        ;
    if (err) {
        /*
         * We do not want print out error logs of CMD21, As it may
         * let user confused.
         */
        if (data->cmd->opcode == MMC_CMD21) {
            /* reset controller */
            MSDC_RESET();

            /* clear fifo */
            MSDC_CLR_FIFO();

            /* make sure txfifo and rxfifo are empty */
            if (MSDC_TXFIFOCNT() != 0 || MSDC_RXFIFOCNT() != 0) {
                dprintf(CRITICAL, "[MSDC] Abort: TXFIFO(%d), RXFIFO(%d) != 0\n",
                        MSDC_TXFIFOCNT(), MSDC_RXFIFOCNT());
            }

            /* clear all interrupts */
            MSDC_CLR_INT();

        } else {
            dprintf(CRITICAL, "[MSDC] DMA failed! err(%d)\n", err);
            if (msdc_abort_handler(host, 1)) {
                dprintf(CRITICAL, "[MSDC] eMMC cannot back to TRANS mode!\n");
                return MMC_ERR_FAILED;
            }
        }
    }

    return err;
}

static int msdc_dma_rw(struct mmc_host *host, u8 *buf, u32 blkaddr, u32 nblks, bool rd)
{
    int multi, err;
    struct mmc_command cmd;
    struct mmc_data data;
    addr_t base = host->base;

    ASSERT(nblks <= host->max_phys_segs);

    LTRACEF("[MSDC] %s data %d blks %s 0x%x\n",
            rd ? "Read" : "Write", nblks, rd ? "from" : "to", blkaddr);

    multi = nblks > 1 ? 1 : 0;
    /* DMA and block number _MUST_BE_ set prior to issuing command */
    MSDC_DMA_ON;
    MSDC_WRITE32(SDC_BLK_NUM, nblks);

    /* send read command */
    if (rd) {
        cmd.opcode =
            multi ? MMC_CMD_READ_MULTIPLE_BLOCK : MMC_CMD_READ_SINGLE_BLOCK;
        if (msdc_get_autocmd(host) == MSDC_AUTOCMD23)
            cmd.opcode = MMC_CMD_READ_MULTIPLE_BLOCK;
    } else {
        cmd.opcode = multi ? MMC_CMD_WRITE_MULTIPLE_BLOCK : MMC_CMD_WRITE_BLOCK;
        if (msdc_get_autocmd(host) == MSDC_AUTOCMD23)
            cmd.opcode = MMC_CMD_WRITE_MULTIPLE_BLOCK;
    }
    cmd.arg = blkaddr;
    cmd.rsptyp  = RESP_R1;
    cmd.retries = 0;
    cmd.timeout = CMD_TIMEOUT;

    err = msdc_cmd(host, &cmd);
    if (err != MMC_ERR_NONE)
        return err;

    data.cmd = &cmd;
    data.blks = nblks;
    data.buf = buf;
    if (rd)
        data.timeout = 100;
    else
        data.timeout = 250;

    err = msdc_dma_transfer(host, &data);
    MSDC_DMA_OFF;

    return err;
}

static int msdc_dma_bread(struct mmc_host *host, u8 *dst, u32 src, u32 nblks)
{
    return msdc_dma_rw(host, dst, src, nblks, true);
}

static int msdc_dma_bwrite(struct mmc_host *host, u32 dst, u8 *src, u32 nblks)
{
    return msdc_dma_rw(host, src, dst, nblks, false);
}
#else
static int msdc_pio_write_word(struct mmc_host *host, u32 *ptr, u32 size)
{
    int err = MMC_ERR_NONE;
    addr_t base = host->base;
    u32 ints = MSDC_INT_DATCRCERR | MSDC_INT_DATTMO | MSDC_INT_XFER_COMPL | MSDC_INT_ACMDRDY;
    u32 status;
    u8 *u8ptr;

    while (1) {
        status = MSDC_READ32(MSDC_INT);
        MSDC_WRITE32(MSDC_INT, status);
        if (status & ~ints)
            dprintf(CRITICAL, "[MSDC]<CHECKME> Unexpected INT(0x%x)\n", status);
        if (status & MSDC_INT_DATCRCERR) {
            dprintf(CRITICAL, "[MSDC] DAT CRC error (0x%x), Left DAT: %d bytes\n",
                    status, size);
            err = MMC_ERR_BADCRC;
            break;
        } else if (status & MSDC_INT_DATTMO) {
            dprintf(CRITICAL, "[MSDC] DAT TMO error (0x%x), Left DAT: %d bytes, MSDC_FIFOCS=%xh\n",
                    status, size, MSDC_READ32(MSDC_FIFOCS));
            err = MMC_ERR_TIMEOUT;
            break;
        } else if (status & MSDC_INT_ACMDCRCERR) {
            dprintf(CRITICAL, "[MSDC] AUTO CMD CRC error (0x%x), Left DAT: %d bytes\n",
                    status, size);
            err = MMC_ERR_ACMD_RSPCRC;
            break;
        } else if (status & MSDC_INT_XFER_COMPL) {
            if (size == 0) {
                LTRACEF("[MSDC] all data flushed to card\n");
                break;
            } else
                LTRACEF("[MSDC]<CHECKME> XFER_COMPL before all data written\n");
        }

        if (size == 0)
            continue;

        if (size >= MSDC_FIFO_SZ) {
            if (MSDC_TXFIFOCNT() == 0) {
                int left = MSDC_FIFO_SZ >> 2;

                do {
                    MSDC_FIFO_WRITE32(*ptr);
                    ptr++;
                } while (--left);
                size -= MSDC_FIFO_SZ;
            }
        } else if (size < MSDC_FIFO_SZ && MSDC_TXFIFOCNT() == 0) {
            while (size) {
                if (size > 3) {
                    MSDC_FIFO_WRITE32(*ptr);
                    ptr++;
                    size -= 4;
                } else {
                    u8ptr = (u8 *)ptr;
                    while (size--) {
                        MSDC_FIFO_WRITE8(*u8ptr);
                        u8ptr++;
                    }
                }
            }
        }
    }

    return err;
}

static int msdc_pio_write(struct mmc_host *host, u32 *ptr, u32 size)
{
    int err = msdc_pio_write_word(host, (u32 *)ptr, size);

    if (err != MMC_ERR_NONE) {
        msdc_abort(host); /* reset internal fifo and state machine */
        dprintf(CRITICAL, "[MSDC] PIO Write Error (%d)\n", err);
    }

    return err;
}

static int msdc_pio_bread(struct mmc_host *host, u8 *dst, u32 src, u32 nblks)
{
    addr_t base = host->base;
    u32 blksz = host->blklen;
    int err = MMC_ERR_NONE, derr = MMC_ERR_NONE;
    int multi;
    struct mmc_command cmd;
    u32 *ptr = (u32 *)dst;

    LTRACEF("[MSDC] Read data %d bytes from 0x%x\n", nblks * blksz, src);

    multi = nblks > 1 ? 1 : 0;

    MSDC_CLR_FIFO();
    MSDC_WRITE32(SDC_BLK_NUM, nblks);
    msdc_set_timeout(host, 100000000, 0);

    /* send read command */
    cmd.opcode  = multi ? MMC_CMD_READ_MULTIPLE_BLOCK : MMC_CMD_READ_SINGLE_BLOCK;
    /* CMD23 with length only 1 */
    if (msdc_get_autocmd(host) == MSDC_AUTOCMD23)
        cmd.opcode = MMC_CMD_READ_MULTIPLE_BLOCK;
    cmd.rsptyp  = RESP_R1;
    cmd.arg     = src;
    cmd.retries = 0;
    cmd.timeout = CMD_TIMEOUT;
    err = msdc_cmd(host, &cmd);

    if (err != MMC_ERR_NONE)
        goto done;

    err = derr = msdc_pio_read(host, (u32 *)ptr, nblks * blksz);

done:
    if (err != MMC_ERR_NONE) {
        if (derr != MMC_ERR_NONE) {
            dprintf(CRITICAL, "[MSDC] Read data error (%d)\n", derr);
            if (msdc_abort_handler(host, 1))
                dprintf(CRITICAL, "[MSDC] abort failed\n");
        } else {
            dprintf(CRITICAL, "[MSDC] Read error (%d)\n", err);
        }
    }
    return (derr == MMC_ERR_NONE) ? err : derr;
}

static int msdc_pio_bwrite(struct mmc_host *host, u32 dst, u8 *src, u32 nblks)
{
    addr_t base = host->base;
    int err = MMC_ERR_NONE, derr = MMC_ERR_NONE;
    int multi;
    u32 blksz = host->blklen;
    struct mmc_command cmd;
    u32 *ptr = (u32 *)src;

    LTRACEF("[MSDC] Write data %d bytes to 0x%x\n", nblks * blksz, dst);

    multi = nblks > 1 ? 1 : 0;

    MSDC_CLR_FIFO();
    MSDC_WRITE32(SDC_BLK_NUM, nblks);

    /* No need since MSDC always waits 8 cycles for write data timeout */

    /* send write command */
    cmd.opcode  = multi ? MMC_CMD_WRITE_MULTIPLE_BLOCK : MMC_CMD_WRITE_BLOCK;
    /* CMD23 with length only 1 */
    if (msdc_get_autocmd(host) == MSDC_AUTOCMD23)
        cmd.opcode = MMC_CMD_WRITE_MULTIPLE_BLOCK;

    cmd.rsptyp  = RESP_R1;
    cmd.arg     = dst;
    cmd.retries = 0;
    cmd.timeout = CMD_TIMEOUT;
    err = msdc_cmd(host, &cmd);

    if (err != MMC_ERR_NONE)
        goto done;

    err = derr = msdc_pio_write(host, (u32 *)ptr, nblks * blksz);

done:
    if (err != MMC_ERR_NONE) {
        if (derr != MMC_ERR_NONE) {
            dprintf(CRITICAL, "[MSDC] Write data error (%d)\n", derr);
            if (msdc_abort_handler(host, 1))
                dprintf(CRITICAL, "[MSDC] abort failed\n");
        } else {
            dprintf(CRITICAL, "[MSDC] Write error (%d)\n", err);
        }
    }
    return (derr == MMC_ERR_NONE) ? err : derr;
}
#endif

static int msdc_pio_read_word(struct mmc_host *host, u32 *ptr, u32 size)
{
    int err = MMC_ERR_NONE;
    addr_t base = host->base;
    u32 ints = MSDC_INT_DATCRCERR | MSDC_INT_DATTMO | MSDC_INT_XFER_COMPL | MSDC_INT_ACMDRDY;
    //u32 timeout = 100000;
    u32 status;
    u32 totalsz = size;
    u8  done = 0;
    u8 *u8ptr;
    u32 dcrc = 0;

    while (1) {
        status = MSDC_READ32(MSDC_INT);
        MSDC_WRITE32(MSDC_INT, status);
        if (status & ~ints)
            dprintf(CRITICAL, "[SD%d]<CHECKME> Unexpected INT(0x%x)\n",
                host->id, status);
        if (status & MSDC_INT_DATCRCERR) {
            MSDC_GET_FIELD(SDC_DCRC_STS, SDC_DCRC_STS_POS|SDC_DCRC_STS_NEG, dcrc);
            dprintf(CRITICAL,
                "[SD%d] DAT CRC error (0x%x), Left:%d/%d bytes, RXFIFO:%d,dcrc:0x%x\n",
                host->id, status, size, totalsz, MSDC_RXFIFOCNT(), dcrc);
            err = MMC_ERR_BADCRC;
            break;
        } else if (status & MSDC_INT_DATTMO) {
            dprintf(CRITICAL, "[SD%d] DAT TMO error (0x%x), Left: %d/%d bytes, RXFIFO:%d\n",
                    host->id, status, size, totalsz, MSDC_RXFIFOCNT());
            err = MMC_ERR_TIMEOUT;
            break;
        } else if (status & MSDC_INT_ACMDCRCERR) {
            MSDC_GET_FIELD(SDC_DCRC_STS, SDC_DCRC_STS_POS|SDC_DCRC_STS_NEG, dcrc);
            dprintf(CRITICAL,
                "[SD%d] AUTOCMD CRC error (0x%x), Left:%d/%d bytes, RXFIFO:%d, dcrc:0x%x\n",
                host->id, status, size, totalsz, MSDC_RXFIFOCNT(), dcrc);
            err = MMC_ERR_ACMD_RSPCRC;
            break;
        } else if (status & MSDC_INT_XFER_COMPL) {
            done = 1;
        }

        if (size == 0 && done)
            break;

        /* Note. RXFIFO count would be aligned to 4-bytes alignment size */
        if ((size >= MSDC_FIFO_THD) && (MSDC_RXFIFOCNT() >= MSDC_FIFO_THD)) {
            int left = MSDC_FIFO_THD >> 2;

            do {
                *ptr++ = MSDC_FIFO_READ32();
            } while (--left);
            size -= MSDC_FIFO_THD;
            LTRACEF("[SD%d] Read %d bytes, RXFIFOCNT: %d,  Left: %d/%d\n",
                    host->id, MSDC_FIFO_THD, MSDC_RXFIFOCNT(), size, totalsz);
        } else if ((size < MSDC_FIFO_THD) && MSDC_RXFIFOCNT() >= size) {
            while (size) {
                if (size > 3) {
                    *ptr++ = MSDC_FIFO_READ32();
                    size -= 4;
                } else {
                    u8ptr = (u8 *)ptr;
                    while (size--)
                        *u8ptr++ = MSDC_FIFO_READ8();
                }
            }
            LTRACEF("[SD%d] Read left bytes, RXFIFOCNT: %d, Left: %d/%d\n",
                    host->id, MSDC_RXFIFOCNT(), size, totalsz);
        }
    }

    return err;
}

int msdc_pio_read(struct mmc_host *host, u32 *ptr, u32 size)
{
    int err = msdc_pio_read_word(host, (u32 *)ptr, size);

    if (err != MMC_ERR_NONE) {
        msdc_abort(host); /* reset internal fifo and state machine */
        dprintf(CRITICAL, "[MSDC] %d-bit PIO Read Error (%d)\n", 32, err);
    }

    return err;
}

void msdc_reset_timing_register(struct mmc_host *host)
{
    addr_t base = host->base;

    MSDC_WRITE32(MSDC_IOCON, 0x00000000);
    MSDC_WRITE32(MSDC_DAT_RDDLY0, 0x00000000);
    MSDC_WRITE32(MSDC_DAT_RDDLY1, 0x00000000);
    MSDC_WRITE32(MSDC_DAT_RDDLY2, 0x00000000);
    MSDC_WRITE32(MSDC_DAT_RDDLY3, 0x00000000);
    MSDC_WRITE32(MSDC_PATCH_BIT0, MSDC_PB0_DEFAULT);
    MSDC_WRITE32(MSDC_PATCH_BIT1, MSDC_PB1_DEFAULT);
    MSDC_WRITE32(MSDC_PATCH_BIT2, MSDC_PB2_DEFAULT);
#ifndef FEATURE_MMC_TUNING_EDGE_ONLY_WHEN_HIGH_SPEED
    /* DE comment: clear SDC_FIFO_CFG[24:25] can make
     * tune(modify EMMC_TOP_CONTROL & EMMC_TOP_CMD) better
     */
    MSDC_SET_FIELD(SDC_FIFO_CFG, SDC_FIFO_CFG_WR_VALID_SEL, 0);
    MSDC_SET_FIELD(SDC_FIFO_CFG, SDC_FIFO_CFG_RD_VALID_SEL, 0);
#endif
    MSDC_WRITE32(MSDC_PAD_TUNE0, 0);
    MSDC_WRITE32(MSDC_PAD_TUNE1, 0);
}

void msdc_init_tune_path(struct mmc_host *host, int hs400)
{
    addr_t base = host->base;

    MSDC_SET_FIELD(MSDC_PATCH_BIT2, MSDC_PB2_RESPWAITCNT, 3);

    MSDC_CLR_BIT32(MSDC_PAD_TUNE0, MSDC_PAD_TUNE0_RXDLYSEL);

    MSDC_CLR_BIT32(MSDC_IOCON, MSDC_IOCON_DDLSEL);
    MSDC_CLR_BIT32(MSDC_IOCON, MSDC_IOCON_R_D_SMPL_SEL);

#if !defined(FPGA_PLATFORM)
    MSDC_CLR_BIT32(MSDC_IOCON, MSDC_IOCON_R_D_SMPL);
    MSDC_CLR_BIT32(MSDC_IOCON, MSDC_IOCON_R_D_SMPL_SEL);
#else
    MSDC_SET_BIT32(MSDC_IOCON, MSDC_IOCON_R_D_SMPL);
    MSDC_CLR_BIT32(MSDC_PATCH_BIT0, MSDC_PB0_RD_DAT_SEL);
#endif

    if (hs400) {
        MSDC_CLR_BIT32(MSDC_PAD_TUNE0, MSDC_PAD_TUNE0_DATRRDLYSEL);
        MSDC_CLR_BIT32(MSDC_PAD_TUNE1, MSDC_PAD_TUNE1_DATRRDLY2SEL);
    } else {
        MSDC_SET_BIT32(MSDC_PAD_TUNE0, MSDC_PAD_TUNE0_DATRRDLYSEL);
        MSDC_CLR_BIT32(MSDC_PAD_TUNE1, MSDC_PAD_TUNE1_DATRRDLY2SEL);
    }

    if (hs400)
        MSDC_CLR_BIT32(MSDC_PATCH_BIT2, MSDC_PB2_CFGCRCSTS);
    else
        MSDC_SET_BIT32(MSDC_PATCH_BIT2, MSDC_PB2_CFGCRCSTS);

    MSDC_CLR_BIT32(MSDC_IOCON, MSDC_IOCON_W_D_SMPL_SEL);

    MSDC_CLR_BIT32(MSDC_PATCH_BIT2, MSDC_PB2_CFGRESP);
    MSDC_SET_BIT32(MSDC_PAD_TUNE0, MSDC_PAD_TUNE0_CMDRRDLYSEL);
    MSDC_CLR_BIT32(MSDC_PAD_TUNE1, MSDC_PAD_TUNE1_CMDRRDLY2SEL);

    if (host->id != 1)
        MSDC_CLR_BIT32(EMMC50_CFG0, MSDC_EMMC50_CFG_CMD_RESP_SEL);

    MSDC_SET_FIELD(MSDC_PATCH_BIT1, MSDC_PB0_CKGEN_MSDC_DLY_SEL, 0);
    MSDC_SET_FIELD(MSDC_PATCH_BIT1, MSDC_PB1_CMD_RSP_TA_CNTR,
                   CMD_RSP_TA_CNTR_DEFAULT);
    MSDC_SET_FIELD(MSDC_PATCH_BIT1, MSDC_PB1_WRDAT_CRCS_TA_CNTR,
                   WRDAT_CRCS_TA_CNTR_DEFAULT);
    MSDC_SET_FIELD(MSDC_PATCH_BIT1, MSDC_PB1_GET_BUSY_MA,
                   BUSY_MA_DEFAULT);

#if !defined(FPGA_PLATFORM)
    if (hs400) {
        MSDC_SET_FIELD(MSDC_PATCH_BIT2, MSDC_PB2_CRCSTSENSEL,
                       CRCSTSENSEL_HS400_DEFAULT);
        MSDC_SET_FIELD(MSDC_PATCH_BIT2, MSDC_PB2_RESPSTENSEL,
                       RESPSTENSEL_HS400_DEFAULT);
    } else {
        MSDC_SET_FIELD(MSDC_PATCH_BIT2, MSDC_PB2_CRCSTSENSEL,
                       CRCSTSENSEL_HS_DEFAULT);
        MSDC_SET_FIELD(MSDC_PATCH_BIT2, MSDC_PB2_RESPSTENSEL,
                       RESPSTENSEL_HS_DEFAULT);
    }
#else
    if (!hs400) {
        MSDC_SET_FIELD(MSDC_PATCH_BIT2, MSDC_PB2_CRCSTSENSEL,
                       CRCSTSENSEL_FPGA_DEFAULT);
    }
#endif

    MSDC_SET_BIT32(MSDC_PATCH_BIT1, MSDC_PB1_DDR_CMD_FIX_SEL);

    /* DDR50 mode */
    MSDC_SET_BIT32(MSDC_PATCH_BIT2, MSDC_PB2_DDR50_SEL);
    /* set SDC_RX_ENHANCE_EN for async-fifo RX tune */
    if (!host->base_top)
        MSDC_SET_FIELD(SDC_ADV_CFG0, SDC_ADV_CFG0_SDC_RX_ENH_EN, 1);
    else
        MSDC_SET_FIELD(EMMC_TOP_CONTROL, SDC_RX_ENH_EN, 1);
}

void msdc_config_clock(struct mmc_host *host, int state, u32 hz)
{
    struct msdc_priv_t *priv = host->priv;
    addr_t base = host->base;
    u32 mode = 0;
    u32 div;
    u32 sclk;
    u32 u4buswidth = 0;
    struct msdc_cust msdc_cap = get_msdc_capability(host->id);

    if (hz >= host->f_max)
        hz = host->f_max;
    else if (hz < host->f_min)
        hz = host->f_min;

    msdc_config_clksrc(host, host->src_clk);
    MSDC_CLR_BIT32(MSDC_CFG, MSDC_CFG_CKMOD_HS400);
    MSDC_SET_BIT32(MSDC_PATCH_BIT2, MSDC_PB2_CFGCRCSTS);

    if (state & MMC_STATE_HS400) {
        mode = 0x3;
        div = 0; /* we let hs400 mode fixed at 200Mhz */
        sclk = host->src_clk >> 1;
        MSDC_SET_BIT32(MSDC_CFG, MSDC_CFG_CKMOD_HS400);
        MSDC_CLR_BIT32(MSDC_PATCH_BIT2, MSDC_PB2_CFGCRCSTS);
    } else if (state&MMC_STATE_DDR) {
        mode = 0x2; /* ddr mode and use divisor */
        if (hz >= (host->src_clk >> 2)) {
            div  = 0;              /* mean div = 1/2 */
            sclk = host->src_clk >> 2; /* sclk = clk/div/2. 2: internal divisor */
        } else {
            div  = (host->src_clk + ((hz << 2) - 1)) / (hz << 2);
            sclk = (host->src_clk >> 2) / div;
            div  = (div >> 1);     /* since there is 1/2 internal divisor */
        }
    } else if (hz >= host->src_clk) {
        mode = 0x1; /* no divisor and divisor is ignored */
        div  = 0;
        sclk = host->src_clk;
    } else {
        mode = 0x0; /* use divisor */
        if (hz >= (host->src_clk >> 1)) {
            div  = 0;              /* mean div = 1/2 */
            sclk = host->src_clk >> 1; /* sclk = clk / 2 */
        } else {
            div  = (host->src_clk + ((hz << 2) - 1)) / (hz << 2);
            sclk = (host->src_clk >> 2) / div;
        }
    }
    video_printf("MSDC going to set bus clock to: %d\n", sclk);
    host->cur_bus_clk = sclk;

    /* set clock mode and divisor */
    MSDC_SET_FIELD(MSDC_CFG, (MSDC_CFG_CKMOD | MSDC_CFG_CKDIV),
                   (mode << 12) | div);
    /* wait clock stable */
    while (!(MSDC_READ32(MSDC_CFG) & MSDC_CFG_CKSTB))
        ;

    if (state & MMC_STATE_HS400) {
        msdc_set_smpl(host, 1, priv->rsmpl, TYPE_CMD_RESP_EDGE);
        msdc_set_smpl(host, 1, priv->rdsmpl, TYPE_READ_DATA_EDGE);
        msdc_set_smpl(host, 1, priv->wdsmpl, TYPE_WRITE_CRC_EDGE);
    } else {
#if !defined(FPGA_PLATFORM)
        msdc_set_smpl(host, 0, priv->rsmpl, TYPE_CMD_RESP_EDGE);
#else
        if (hz < 1000000)
            msdc_set_smpl(host, 0, MSDC_SMPL_RISING, TYPE_CMD_RESP_EDGE);
        else
            msdc_set_smpl(host, 0, priv->rsmpl, TYPE_CMD_RESP_EDGE);
#endif

#if !defined(FPGA_PLATFORM)
        msdc_set_smpl(host, 0, priv->rdsmpl, TYPE_READ_DATA_EDGE);
#else
        if (hz < 1000000)
            msdc_set_smpl(host, 0, MSDC_SMPL_RISING, TYPE_READ_DATA_EDGE);
        else
            msdc_set_smpl(host, 0, priv->rdsmpl, TYPE_READ_DATA_EDGE);
#endif

        msdc_set_smpl(host, 0, priv->wdsmpl, TYPE_WRITE_CRC_EDGE);
    }

    if (mode == 2 || mode == 3)
        MSDC_CLR_BIT32(MSDC_PATCH_BIT0, MSDC_PB0_RD_DAT_SEL);
    else
        MSDC_SET_BIT32(MSDC_PATCH_BIT0, MSDC_PB0_RD_DAT_SEL);
    if (mode == 2)
        MSDC_SET_BIT32(MSDC_IOCON, MSDC_IOCON_DDR50CKD);
    else
        MSDC_CLR_BIT32(MSDC_IOCON, MSDC_IOCON_DDR50CKD);

    msdc_init_tune_path(host, (mode == 3) ? 1 : 0);

    MSDC_GET_FIELD(SDC_CFG, SDC_CFG_BUSWIDTH, u4buswidth);

    dprintf(ALWAYS,
            "[MSDC] SET_CLK(%dkHz): SCLK(%dkHz) MODE(%d) DIV(%d) DS(%d) RS(%d) buswidth(%s)\n",
            hz/1000, sclk/1000, mode, div, msdc_cap.data_edge,
            msdc_cap.cmd_edge,
            (u4buswidth == 0) ?
            "1-bit" : (u4buswidth == 1) ?
            "4-bits" : (u4buswidth == 2) ?
            "8-bits" : "undefined");
}

void msdc_config_bus(struct mmc_host *host, u32 width)
{
    u32 val, mode, div;
    addr_t base = host->base;

    val = (width == HOST_BUS_WIDTH_8) ? 2 :
          (width == HOST_BUS_WIDTH_4) ? 1 : 0;

    MSDC_SET_FIELD(SDC_CFG, SDC_CFG_BUSWIDTH, val);
    MSDC_GET_FIELD(MSDC_CFG, MSDC_CFG_CKMOD, mode);
    MSDC_GET_FIELD(MSDC_CFG, MSDC_CFG_CKDIV, div);

    dprintf(ALWAYS, "CLK (%dMHz), SCLK(%dkHz) MODE(%d) DIV(%d) buswidth(%u-bits)\n",
            host->src_clk/1000000, host->cur_bus_clk/1000, mode, div, width);
}

void msdc_reset_tune_counter(struct mmc_host *host)
{
    host->time_read = 0;
}

#ifdef FEATURE_MMC_CM_TUNING
static int msdc_tune_cmdrsp(struct mmc_host *host, struct mmc_command *cmd)
{
    addr_t base = host->base;
    int result = MMC_ERR_CMDTUNEFAIL;
    unsigned int orig_rsmpl, cur_rsmpl, rsmpl, orig_clkmode;
#ifndef FEATURE_MMC_TUNING_EDGE_ONLY_WHEN_HIGH_SPEED
    unsigned int orig_dly1 = 0, orig_dly1_sel, cur_dly1;
    unsigned int orig_dly2 = 0, orig_dly2_sel, cur_dly2, cur_dly1_sel, cur_dly2_sel;
    unsigned int orig_dly = 0, cur_dly;
    unsigned int dly;
#endif
    u8 hs400 = 0;
#if MSDC_TUNE_LOG
    u32 times = 0;
#endif

    MSDC_GET_FIELD(MSDC_CFG, MSDC_CFG_CKMOD, orig_clkmode);
    hs400 = (orig_clkmode == 3) ? 1 : 0;
    MSDC_GET_FIELD(MSDC_IOCON, MSDC_IOCON_RSPL, orig_rsmpl);

#ifndef FEATURE_MMC_TUNING_EDGE_ONLY_WHEN_HIGH_SPEED
    if (!host->base_top) {
        MSDC_GET_FIELD(MSDC_PAD_TUNE0, MSDC_PAD_TUNE0_CMDRDLY, orig_dly1);
        MSDC_GET_FIELD(MSDC_PAD_TUNE0, MSDC_PAD_TUNE0_CMDRRDLYSEL, orig_dly1_sel);
        MSDC_GET_FIELD(MSDC_PAD_TUNE0, MSDC_PAD_TUNE1_CMDRDLY2, orig_dly2);
        MSDC_GET_FIELD(MSDC_PAD_TUNE0, MSDC_PAD_TUNE1_CMDRRDLY2SEL, orig_dly2_sel);
    } else {
        MSDC_GET_FIELD(EMMC_TOP_CMD, PAD_CMD_RXDLY, orig_dly1);
        MSDC_GET_FIELD(EMMC_TOP_CMD, PAD_CMD_RD_RXDLY_SEL, orig_dly1_sel);
        MSDC_GET_FIELD(EMMC_TOP_CMD, PAD_CMD_RXDLY2, orig_dly2);
        MSDC_GET_FIELD(EMMC_TOP_CMD, PAD_CMD_RD_RXDLY2_SEL, orig_dly2_sel);
    }
    orig_dly = orig_dly1 * orig_dly1_sel + orig_dly2 * orig_dly2_sel;
    cur_dly = orig_dly;
    cur_dly1 = orig_dly1;
    cur_dly2 = orig_dly2;
    cur_dly1_sel = orig_dly1_sel;
    cur_dly2_sel = orig_dly2_sel;

    dly = 0;

    do {
#endif

        for (rsmpl = 0; rsmpl < 2; rsmpl++) {
            cur_rsmpl = (orig_rsmpl + rsmpl) % 2;
            msdc_set_smpl(host, hs400, cur_rsmpl, TYPE_CMD_RESP_EDGE);
            if (host->cur_bus_clk <= 400000)
                msdc_set_smpl(host, hs400, 0, TYPE_CMD_RESP_EDGE);
            if (cmd->opcode != MMC_CMD_STOP_TRANSMISSION) {
                result = msdc_send_cmd(host, cmd);
                if (result == MMC_ERR_TIMEOUT)
                    rsmpl--;
                if (result != MMC_ERR_NONE && cmd->opcode != MMC_CMD_STOP_TRANSMISSION) {
                    if (cmd->opcode == MMC_CMD_READ_MULTIPLE_BLOCK
                            || cmd->opcode == MMC_CMD_WRITE_MULTIPLE_BLOCK
                            || cmd->opcode == MMC_CMD_READ_SINGLE_BLOCK
                            || cmd->opcode == MMC_CMD_WRITE_BLOCK
                            || cmd->opcode == MMC_CMD_SEND_WRITE_PROT_TYPE)
                        msdc_abort_handler(host, 1);
                    continue;
                }
                result = msdc_wait_cmd_done(host, cmd);
            } else if (cmd->opcode == MMC_CMD_STOP_TRANSMISSION) {
                result = MMC_ERR_NONE;
                goto Pass;
            } else
                result = MMC_ERR_BADCRC;

#if MSDC_TUNE_LOG

            times++;

#ifndef FEATURE_MMC_TUNING_EDGE_ONLY_WHEN_HIGH_SPEED
            dprintf(INFO, "[SD%d] <TUNE_CMD%d><%d><%s> CMDRRDLY=%d, RSPL=%dh\n",
                    host->id, (cmd->opcode & (~(SD_CMD_BIT | SD_CMD_APP_BIT))),
                    times, (result == MMC_ERR_NONE) ? "PASS" : "FAIL",
                    orig_dly + dly, cur_rsmpl);
            dprintf(INFO, "[SD%d] <TUNE_CMD><%d><%s> CMDRRDLY1=%xh, CMDRRDLY1SEL=%x,",
                    host->id, times, (result == MMC_ERR_NONE) ? "PASS" : "FAIL",
                    cur_dly1, cur_dly1_sel);
            dprintf(INFO, " CMDRRDLY2=%xh, CMDRRDLY2SEL=%xh\n",
                    cur_dly2, cur_dly2_sel);

#else
            dprintf(INFO, "[SD%d] <TUNE_CMD%d><%d><%s> RSPL=%dh\n",
                    host->id, (cmd->opcode & (~(SD_CMD_BIT | SD_CMD_APP_BIT))),
                    times, (result == MMC_ERR_NONE) ? "PASS" : "FAIL",
                    cur_rsmpl);
#endif
#endif

            if (result == MMC_ERR_NONE)
                goto Pass;

            if (cmd->opcode == MMC_CMD_READ_MULTIPLE_BLOCK
                    || cmd->opcode == MMC_CMD_WRITE_MULTIPLE_BLOCK
                    || cmd->opcode == MMC_CMD_READ_SINGLE_BLOCK
                    || cmd->opcode == MMC_CMD_WRITE_BLOCK)
                msdc_abort_handler(host, 1);
        }

#ifndef FEATURE_MMC_TUNING_EDGE_ONLY_WHEN_HIGH_SPEED
        cur_dly = (orig_dly + dly + 1) % 63;
        cur_dly1_sel = 1;
        if (cur_dly < 32) {
            cur_dly1 = cur_dly;
            cur_dly2 = 0;
            cur_dly2_sel = 0;
        } else {
            cur_dly1 = 31;
            cur_dly2 = cur_dly - 31;
            cur_dly2_sel = 1;
        }

        if (!host->base_top) {
            MSDC_SET_BIT32(MSDC_PAD_TUNE0, MSDC_PAD_TUNE0_CMDRRDLYSEL);
            MSDC_SET_FIELD(MSDC_PAD_TUNE0, MSDC_PAD_TUNE0_CMDRDLY, cur_dly1);
            MSDC_SET_FIELD(MSDC_PAD_TUNE1, MSDC_PAD_TUNE1_CMDRDLY2, cur_dly2);
            MSDC_SET_FIELD(MSDC_PAD_TUNE1, MSDC_PAD_TUNE1_CMDRRDLY2SEL, cur_dly2_sel);
        } else {
            MSDC_SET_BIT32(EMMC_TOP_CMD, PAD_CMD_RD_RXDLY_SEL);
            MSDC_SET_FIELD(EMMC_TOP_CMD, PAD_CMD_RXDLY, cur_dly1);
            MSDC_SET_FIELD(EMMC_TOP_CMD, PAD_CMD_RXDLY2, cur_dly2);
            MSDC_SET_FIELD(EMMC_TOP_CMD, PAD_CMD_RD_RXDLY2_SEL, cur_dly2_sel);
        }
    } while (++dly <= 62);
#endif

    dprintf(ALWAYS, "msdc tune CMD failed\n");
    return MMC_ERR_CMDTUNEFAIL;
Pass:
    dprintf(ALWAYS, "msdc tune CMD pass\n");
    return result;
}
#endif

#ifdef FEATURE_MMC_RD_TUNING
int msdc_tune_bread(struct mmc_host *host, u8 *dst, u32 src, u32 nblks)
{
    addr_t base = host->base;
    int result = MMC_ERR_CMDTUNEFAIL;
    u32 orig_rdsmpl, cur_rdsmpl, rdsmpl, orig_clkmode;
    unsigned int rdsmpl_end;
#ifndef FEATURE_MMC_TUNING_EDGE_ONLY_WHEN_HIGH_SPEED
    u32 orig_dly1 = 0, orig_dly1_sel, cur_dly1;
    u32 orig_dly2 = 0, orig_dly2_sel, cur_dly2, cur_dly1_sel, cur_dly2_sel;
    u32 orig_dly = 0, cur_dly;
    u32 dly;
#endif
    u32 dcrc = 0;
    u32 hs400;
#if MSDC_TUNE_LOG
    u32 times = 0;
#endif

    MSDC_GET_FIELD(MSDC_CFG, MSDC_CFG_CKMOD, orig_clkmode);
    hs400 = (orig_clkmode == 3) ? 1 : 0;
    if (orig_clkmode == 2 || orig_clkmode == 3)
        rdsmpl_end = 0;
    else
        rdsmpl_end = 1;

    MSDC_GET_FIELD(MSDC_PATCH_BIT0, MSDC_PB0_RD_DAT_SEL, orig_rdsmpl);

#ifndef FEATURE_MMC_TUNING_EDGE_ONLY_WHEN_HIGH_SPEED
    if (!host->base_top) {
        MSDC_GET_FIELD(MSDC_PAD_TUNE0, MSDC_PAD_TUNE0_DATRRDLY, orig_dly1);
        MSDC_GET_FIELD(MSDC_PAD_TUNE0, MSDC_PAD_TUNE0_DATRRDLYSEL, orig_dly1_sel);
        MSDC_GET_FIELD(MSDC_PAD_TUNE1, MSDC_PAD_TUNE1_DATRRDLY2, orig_dly2);
        MSDC_GET_FIELD(MSDC_PAD_TUNE1, MSDC_PAD_TUNE1_DATRRDLY2SEL, orig_dly2_sel);
    } else {
        MSDC_GET_FIELD(EMMC_TOP_CONTROL, PAD_DAT_RD_RXDLY, orig_dly1);
        MSDC_GET_FIELD(EMMC_TOP_CONTROL, PAD_DAT_RD_RXDLY_SEL, orig_dly1_sel);
        MSDC_GET_FIELD(EMMC_TOP_CONTROL, PAD_DAT_RD_RXDLY2, orig_dly2);
        MSDC_GET_FIELD(EMMC_TOP_CONTROL, PAD_DAT_RD_RXDLY2_SEL, orig_dly2_sel);
    }
    orig_dly = orig_dly1 * orig_dly1_sel + orig_dly2 * orig_dly2_sel;
    cur_dly = orig_dly;
    cur_dly1 = orig_dly1;
    cur_dly2 = orig_dly2;
    cur_dly1_sel = orig_dly1_sel;
    cur_dly2_sel = orig_dly2_sel;

    dly = 0;

    do {
#endif
        for (rdsmpl = 0; rdsmpl <= rdsmpl_end; rdsmpl++) {

            cur_rdsmpl = (orig_rdsmpl + rdsmpl) % 2;
            msdc_set_smpl(host, hs400, cur_rdsmpl, TYPE_READ_DATA_EDGE);

            result = host->blk_read(host, dst, src, nblks);
            if (result == MMC_ERR_CMDTUNEFAIL || result == MMC_ERR_CMD_RSPCRC ||
                result == MMC_ERR_ACMD_RSPCRC)
                goto Pass;

            MSDC_GET_FIELD(SDC_DCRC_STS, SDC_DCRC_STS_POS|SDC_DCRC_STS_NEG, dcrc);

#if MSDC_TUNE_LOG

            times++;

#ifndef FEATURE_MMC_TUNING_EDGE_ONLY_WHEN_HIGH_SPEED
            dprintf(INFO, "[SD%d] <TUNE_READ><%d><%s> DATRRDLY=%d, RSPL=%dh\n",
                    host->id, times, (result == MMC_ERR_NONE) ? "PASS" : "FAIL",
                    orig_dly + dly, cur_rdsmpl);
            dprintf(INFO, "[SD%d] <TUNE_READ><%d><%s> DATRRDLY1=%xh, DATRRDLY1SEL=%x,",
                    host->id, times, (result == MMC_ERR_NONE) ? "PASS" : "FAIL",
                    cur_dly1, cur_dly1_sel);
            dprintf(INFO, " DATRRDLY2=%xh, DATRRDLY2SEL=%xh\n",
                    cur_dly2, cur_dly2_sel);
#else
            dprintf(INFO, "[SD%d] <TUNE_READ><%d><%s>RSPL=%dh\n",
                    host->id, times, (result == MMC_ERR_NONE) ? "PASS" : "FAIL",
                    cur_rdsmpl);
#endif
#endif

            if (result == MMC_ERR_NONE && dcrc == 0)
                goto Pass;
            else
                result = MMC_ERR_BADCRC;

        }

#ifndef FEATURE_MMC_TUNING_EDGE_ONLY_WHEN_HIGH_SPEED
        cur_dly = (orig_dly + dly + 1) % 63;
        cur_dly1_sel = 1;
        if (cur_dly < 32) {
            cur_dly1 = cur_dly;
            cur_dly2 = 0;
            cur_dly2_sel = 0;
        } else {
            cur_dly1 = 31;
            cur_dly2 = cur_dly - 31;
            cur_dly2_sel = 1;
        }

        if (!host->base_top) {
            MSDC_SET_BIT32(MSDC_PAD_TUNE0, MSDC_PAD_TUNE0_DATRRDLYSEL);
            MSDC_SET_FIELD(MSDC_PAD_TUNE0, MSDC_PAD_TUNE0_DATRRDLY, cur_dly1);
            MSDC_SET_FIELD(MSDC_PAD_TUNE1, MSDC_PAD_TUNE1_DATRRDLY2, cur_dly2);
            MSDC_SET_FIELD(MSDC_PAD_TUNE1, MSDC_PAD_TUNE1_DATRRDLY2SEL, cur_dly2_sel);
        } else {
            MSDC_SET_BIT32(EMMC_TOP_CONTROL, PAD_DAT_RD_RXDLY_SEL);
            MSDC_SET_FIELD(EMMC_TOP_CONTROL, PAD_DAT_RD_RXDLY, cur_dly1);
            MSDC_SET_FIELD(EMMC_TOP_CONTROL, PAD_DAT_RD_RXDLY2, cur_dly2);
            MSDC_SET_FIELD(EMMC_TOP_CONTROL, PAD_DAT_RD_RXDLY2_SEL, cur_dly2_sel);
        }
    } while (dly++ <= 62);
#endif

    dprintf(ALWAYS, "msdc tune bread failed\n");
    return MMC_ERR_CMDTUNEFAIL;
Pass:
    dprintf(ALWAYS, "msdc tune bread Pass\n");
    return result;
}
#define READ_TUNING_MAX_HS (2 * 32)
#define READ_TUNING_MAX_UHS (2 * 32 * 32)
#define READ_TUNING_MAX_UHS_CLKMOD1 (2 * 32 * 32 * 8)

int msdc_tune_read(struct mmc_host *host)
{
    addr_t base = host->base;
    u32 cur_dsmpl = 0, orig_dsmpl;

    MSDC_GET_FIELD(MSDC_IOCON, MSDC_IOCON_R_D_SMPL, orig_dsmpl);
    cur_dsmpl = (orig_dsmpl + 1);
    MSDC_SET_FIELD(MSDC_IOCON, MSDC_IOCON_R_D_SMPL, cur_dsmpl % 2);
    return 0;
}
#endif

#ifdef FEATURE_MMC_WR_TUNING
int msdc_tune_bwrite(struct mmc_host *host, u32 dst, u8 *src, u32 nblks)
{
    addr_t base = host->base;
    int result = MMC_ERR_CMDTUNEFAIL;
    unsigned int orig_dsmpl, cur_dsmpl, dsmpl, dsmpl_end, orig_clkmode;
#ifndef FEATURE_MMC_TUNING_EDGE_ONLY_WHEN_HIGH_SPEED
    unsigned int orig_dly1 = 0, orig_dly1_sel, cur_dly1;
    unsigned int orig_dly2 = 0, orig_dly2_sel, cur_dly2, cur_dly1_sel, cur_dly2_sel;
    unsigned int orig_dly = 0, cur_dly;
    unsigned int dly;
#endif
    u8 hs400 = 0;
#if MSDC_TUNE_LOG
    u32 times = 0;
#endif

    MSDC_GET_FIELD(MSDC_CFG, MSDC_CFG_CKMOD, orig_clkmode);
    hs400 = (orig_clkmode == 3) ? 1 : 0;
    if (orig_clkmode == 2 || orig_clkmode == 3)
        dsmpl_end = 0;
    else
        dsmpl_end = 1;

    if (host->id == 0) {
        if (hs400)
            MSDC_GET_FIELD(EMMC50_CFG0, MSDC_EMMC50_CFG_CRC_STS_EDGE, orig_dsmpl);
        else
            MSDC_GET_FIELD(MSDC_PATCH_BIT2, MSDC_PB2_CFGCRCSTSEDGE, orig_dsmpl);
    } else {
        MSDC_GET_FIELD(MSDC_PATCH_BIT2, MSDC_PB2_CFGCRCSTSEDGE, orig_dsmpl);
    }

#ifndef FEATURE_MMC_TUNING_EDGE_ONLY_WHEN_HIGH_SPEED
    if (!host->base_top) {
        MSDC_GET_FIELD(MSDC_PAD_TUNE0, MSDC_PAD_TUNE0_DATRRDLY, orig_dly1);
        MSDC_GET_FIELD(MSDC_PAD_TUNE0, MSDC_PAD_TUNE0_DATRRDLYSEL, orig_dly1_sel);
        MSDC_GET_FIELD(MSDC_PAD_TUNE1, MSDC_PAD_TUNE1_DATRRDLY2, orig_dly2);
        MSDC_GET_FIELD(MSDC_PAD_TUNE1, MSDC_PAD_TUNE1_DATRRDLY2SEL, orig_dly2_sel);
    } else {
        MSDC_GET_FIELD(EMMC_TOP_CONTROL, PAD_DAT_RD_RXDLY, orig_dly1);
        MSDC_GET_FIELD(EMMC_TOP_CONTROL, PAD_DAT_RD_RXDLY_SEL, orig_dly1_sel);
        MSDC_GET_FIELD(EMMC_TOP_CONTROL, PAD_DAT_RD_RXDLY2, orig_dly2);
        MSDC_GET_FIELD(EMMC_TOP_CONTROL, PAD_DAT_RD_RXDLY2_SEL, orig_dly2_sel);
    }
    orig_dly = orig_dly1 * orig_dly1_sel + orig_dly2 * orig_dly2_sel;
    cur_dly = orig_dly;
    cur_dly1 = orig_dly1;
    cur_dly2 = orig_dly2;
    cur_dly1_sel = orig_dly1_sel;
    cur_dly2_sel = orig_dly2_sel;

    dly = 0;

    do {
#endif

        for (dsmpl = 0; dsmpl <= dsmpl_end; dsmpl++) {

            cur_dsmpl = (orig_dsmpl + dsmpl) % 2;
            msdc_set_smpl(host, hs400, cur_dsmpl, TYPE_WRITE_CRC_EDGE);

            result = host->blk_write(host, dst, src, nblks);
            if (result == MMC_ERR_CMDTUNEFAIL || result == MMC_ERR_CMD_RSPCRC ||
                result == MMC_ERR_ACMD_RSPCRC)
                goto Pass;

#if MSDC_TUNE_LOG

            times++;

#ifndef FEATURE_MMC_TUNING_EDGE_ONLY_WHEN_HIGH_SPEED
            dprintf(INFO, "[SD%d] <TUNE_WRITE><%d><%s> DATRRDLY=%d, RDSPL=%dh\n",
                    host->id, times, (result == MMC_ERR_NONE) ? "PASS" : "FAIL",
                    orig_dly + dly, cur_dsmpl);
            dprintf(INFO, "[SD%d] <TUNE_WRITE><%d><%s> DATRRDLY1=%xh, DATRRDLY1SEL=%x,",
                    host->id, times, (result == MMC_ERR_NONE) ? "PASS" : "FAIL",
                    cur_dly1, cur_dly1_sel);
            dprintf(INFO, " DATRRDLY2=%xh, DATRRDLY2SEL=%xh\n",
                    cur_dly2, cur_dly2_sel);
#else
            dprintf(INFO, "[SD%d] <TUNE_WRITE><%d><%s> RDSPL=%dh\n",
                    host->id, times, (result == MMC_ERR_NONE) ? "PASS" : "FAIL",
                    cur_dsmpl);
#endif
#endif

            if (result == MMC_ERR_NONE)
                goto Pass;
            else
                result = MMC_ERR_BADCRC;
        }

#ifndef FEATURE_MMC_TUNING_EDGE_ONLY_WHEN_HIGH_SPEED
        cur_dly = (orig_dly + dly + 1) % 63;
        cur_dly1_sel = 1;
        if (cur_dly < 32) {
            cur_dly1 = cur_dly;
            cur_dly2 = 0;
            cur_dly2_sel = 0;
        } else {
            cur_dly1 = 31;
            cur_dly2 = cur_dly - 31;
            cur_dly2_sel = 1;
        }

        if (!host->base_top) {
            MSDC_SET_BIT32(MSDC_PAD_TUNE0, MSDC_PAD_TUNE0_DATRRDLYSEL);
            MSDC_SET_FIELD(MSDC_PAD_TUNE0, MSDC_PAD_TUNE0_DATRRDLY, cur_dly1);
            MSDC_SET_FIELD(MSDC_PAD_TUNE1, MSDC_PAD_TUNE1_DATRRDLY2, cur_dly2);
            MSDC_SET_FIELD(MSDC_PAD_TUNE1, MSDC_PAD_TUNE1_DATRRDLY2SEL, cur_dly2_sel);
        } else {
            MSDC_SET_BIT32(EMMC_TOP_CONTROL, PAD_DAT_RD_RXDLY_SEL);
            MSDC_SET_FIELD(EMMC_TOP_CONTROL, PAD_DAT_RD_RXDLY, cur_dly1);
            MSDC_SET_FIELD(EMMC_TOP_CONTROL, PAD_DAT_RD_RXDLY2, cur_dly2);
            MSDC_SET_FIELD(EMMC_TOP_CONTROL, PAD_DAT_RD_RXDLY2_SEL, cur_dly2_sel);
        }
    } while (dly++ <= 62);
#endif

    dprintf(ALWAYS, "msdc tune bwrite failed\n");
    return MMC_ERR_CMDTUNEFAIL;
Pass:
    dprintf(ALWAYS, "msdc tune bwrite Pass\n");
    return result;
}
#endif

void msdc_emmc_boot_stop(struct mmc_host *host)
{
    addr_t base = host->base;
    u32 count = 0;

    /* Step5. stop the boot mode */
    MSDC_WRITE32(SDC_ARG, 0x00000000);
    MSDC_WRITE32(SDC_CMD, 0x00001000);

    MSDC_SET_FIELD(EMMC_CFG0, EMMC_CFG0_BOOTWDLY, 2);
    MSDC_SET_BIT32(EMMC_CFG0, EMMC_CFG0_BOOTSTOP);
    while (MSDC_READ32(EMMC_STS) & EMMC_STS_BOOTUPSTATE) {
        mdelay(1);
        count++;
        if (count >= 1000) {
            dprintf(ALWAYS, "Timeout to wait EMMC to leave boot state!\n");
            break;
        }
    }

    /* Step6. */
    MSDC_CLR_BIT32(EMMC_CFG0, EMMC_CFG0_BOOTSUPP);

    /* Step7. clear EMMC_STS bits */
    MSDC_WRITE32(EMMC_STS, MSDC_READ32(EMMC_STS));
}

int msdc_init(struct mmc_host *host)
{
    addr_t base = host->id ? MSDC1_BASE : MSDC0_BASE; /* only support MSDC0, MSDC1 */
    addr_t top_base = host->id ? MSDC1_TOP_BASE : MSDC0_TOP_BASE; /* only support MSDC0, MSDC1 */
    struct msdc_priv_t *priv;
    struct msdc_cust msdc_cap = get_msdc_capability(host->id);
#ifdef MSDC_USE_DMA_MODE
    unsigned int irq_num = 0;
#endif

    dprintf(ALWAYS, "[%s]: Host controller initialization start\n", __func__);

    priv = &msdc_priv;
    memset(priv, 0, sizeof(struct msdc_priv_t));

    host->base   = base;
    host->base_top = top_base;

    msdc_config_clksrc(host, msdc_cap.clk_src);

    host->f_max  = MSDC_MAX_SCLK;
    host->f_min  = MSDC_MIN_SCLK;
    host->blklen = 0;
    host->priv   = (void *)priv;
    host->caps   = MMC_CAP_MULTIWRITE;

    if (msdc_cap.flags & MSDC_HIGHSPEED)
        host->caps |= (MMC_CAP_MMC_HIGHSPEED | MMC_CAP_SD_HIGHSPEED);
    if (msdc_cap.flags & MSDC_DDR)
        host->caps |= MMC_CAP_DDR;
    if (msdc_cap.flags & MSDC_UHS1)
        host->caps |= MMC_CAP_SD_UHS1;
    if (msdc_cap.data_pins == 4)
        host->caps |= MMC_CAP_4_BIT_DATA;
    if (msdc_cap.data_pins == 8)
        host->caps |= MMC_CAP_8_BIT_DATA | MMC_CAP_4_BIT_DATA;
    if (msdc_cap.flags & MSDC_HS200)
        host->caps |= MMC_CAP_EMMC_HS200;
    if (msdc_cap.flags & MSDC_HS400)
        host->caps |= MMC_CAP_EMMC_HS400;

    host->ocr_avail = MMC_VDD_27_36;  /* TODO: To be customized */

    /* Configure BASIC_DMA + AUTOCMD12 for better R/W performance
     * NOTE: ACMD23 only support transferring size of up to 32M
     */
    priv->autocmd = MSDC_AUTOCMD12;
    msdc_set_max_phys_segs(host);

    priv->rdsmpl       = msdc_cap.data_edge;
    priv->wdsmpl       = msdc_cap.data_edge;
    priv->rsmpl       = msdc_cap.cmd_edge;

#ifdef MSDC_USE_DMA_MODE
    host->blk_read  = msdc_dma_bread;
    host->blk_write = msdc_dma_bwrite;
    dprintf(ALWAYS, "Transfer method: DMA\n");
#else
    host->blk_read  = msdc_pio_bread;
    host->blk_write = msdc_pio_bwrite;
    dprintf(ALWAYS, "Transfer method: PIO\n");
#endif

    priv->rdsmpl      = msdc_cap.data_edge;
    priv->rsmpl       = msdc_cap.cmd_edge;

    /* disable EMMC boot mode */
    msdc_emmc_boot_stop(host);

    msdc_power(host, MMC_POWER_OFF);
    msdc_power(host, MMC_POWER_ON);

    if (host->id == 1)
        sd_card_vccq_on();

    /* set to SD/MMC mode */
    MSDC_SET_FIELD(MSDC_CFG, MSDC_CFG_MODE, MSDC_SDMMC);
    MSDC_SET_BIT32(MSDC_CFG, MSDC_CFG_PIO);
    MSDC_SET_BIT32(MSDC_CFG, MSDC_CFG_CKPDN);

    MSDC_RESET();
    MSDC_CLR_FIFO();
    MSDC_CLR_INT();

    /* enable SDIO mode. it's must otherwise sdio command failed */
    MSDC_SET_BIT32(SDC_CFG, SDC_CFG_SDIO);

    /* disable detect SDIO device interrupt function */
    MSDC_CLR_BIT32(SDC_CFG, SDC_CFG_SDIOIDE);

    /* reset tuning parameter */
    msdc_reset_timing_register(host);

    msdc_init_tune_path(host, 0);

    /* Enable support 64G */
    MSDC_SET_BIT32(MSDC_PATCH_BIT2, MSDC_PB2_SUPPORT64G);

    /* clear boot mode setting */
    MSDC_WRITE32(EMMC_CFG0, 0);
    MSDC_WRITE32(EMMC_STS, 0);

#if !defined(FPGA_PLATFORM)
    msdc_gpio_and_pad_init(host);
#endif

    /* set sampling edge */
    MSDC_SET_FIELD(MSDC_IOCON, MSDC_IOCON_RSPL, msdc_cap.cmd_edge);
    MSDC_SET_FIELD(MSDC_IOCON, MSDC_IOCON_R_D_SMPL, msdc_cap.data_edge);

    /* write crc timeout detection */
    MSDC_SET_FIELD(MSDC_PATCH_BIT0, 1 << 30, 1);

    msdc_set_startbit(host, START_AT_RISING);

    msdc_config_bus(host, HOST_BUS_WIDTH_1);
    msdc_config_clock(host, 0, MSDC_MIN_SCLK);

    msdc_set_timeout(host, 100000000, 0);

    /* disable SDIO func */
    MSDC_SET_FIELD(SDC_CFG, SDC_CFG_SDIO, 0);
    MSDC_SET_FIELD(SDC_CFG, SDC_CFG_SDIOIDE, 0);
    MSDC_SET_FIELD(SDC_CFG, SDC_CFG_INSWKUP, 0);

    /* Clear all interrupts first */
    MSDC_CLR_INT();
    MSDC_WRITE32(MSDC_INTEN, 0);

#ifdef MSDC_USE_DMA_MODE
    /* Register msdc irq */
#if defined(SDCARD_SUPPORT) || defined(MSDC1_EMMC)
    if (host->id)
        irq_num = MSDC1_IRQ_BIT_ID;
    else
#endif
        irq_num = MSDC0_IRQ_BIT_ID;
    mt_irq_set_sens(irq_num, GIC_LEVEL_SENSITIVE);
    mt_irq_set_polarity(irq_num, GIC_LOW_POLARITY);
    event_init(&msdc_int_event, false, EVENT_FLAG_AUTOUNSIGNAL);
    register_int_handler(irq_num, msdc_interrupt_handler, host);
    unmask_interrupt(irq_num);
#endif

    dprintf(ALWAYS, "[%s]: Host controller initialization done\n", __func__);

    return 0;
}

int msdc_deinit(struct mmc_host *host)
{
    int ret = 0;

    deinit_rpmb_sharemem();

    return ret;
}
