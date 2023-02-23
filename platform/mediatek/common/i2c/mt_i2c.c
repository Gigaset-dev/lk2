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
#include <gpio_api.h>
#include <libfdt.h>
#include <mt_i2c.h>
#include <platform_i2c.h>
#include <reg.h>
#include <sys/types.h>
#include <trace.h>
#include "i2c.h"

#define LOCAL_TRACE ERR

void i2c_writel(struct mt_i2c *i2c, u8 offset, u16 value)
{
    writel(value, (i2c->base) + (mt_i2c_regs[offset]));
}

u32 i2c_readl(struct mt_i2c *i2c, u16 offset)
{
    return readl((i2c->base) + (mt_i2c_regs[offset]));
}

void i2c_writeb(struct mt_i2c *i2c, u8 offset, u8 value)
{
    writeb(value, (i2c->base) + (mt_i2c_regs[offset]));
}

u8 i2c_readb(struct mt_i2c *i2c, u16 offset)
{
    return readb((i2c->base) + (mt_i2c_regs[offset]));
}

/* calculate i2c port speed */
static s32 mtk_i2c_calculate_speed(struct mt_i2c *i2c,
        u32 clk_src_in_khz,
        u32 speed_hkz,
        u32 *timing_step_cnt,
        u32 *timing_sample_cnt)
{
    u32 khz;
    u32 step_cnt;
    u32 sample_cnt;
    u32 sclk;
    u32 hclk;
    u32 max_step_cnt;
    u32 sample_div = MAX_SAMPLE_CNT_DIV;
    u32 step_div;
    u32 min_div;
    u32 best_mul;
    u32 cnt_mul;

    if (speed_hkz > MAX_HS_MODE_SPEED)
        return ERR_NOT_VALID;
    else if (speed_hkz > MAX_FS_MODE_SPEED)
        max_step_cnt = MAX_HS_STEP_CNT_DIV;
    else
        max_step_cnt = MAX_STEP_CNT_DIV;

    step_div = max_step_cnt;

    /* Find the best combination */
    khz = speed_hkz;
    hclk = clk_src_in_khz;
    min_div = ((hclk >> 1) + khz - 1) / khz;
    best_mul = MAX_SAMPLE_CNT_DIV * max_step_cnt;
    for (sample_cnt = 1; sample_cnt <= MAX_SAMPLE_CNT_DIV; sample_cnt++) {
        step_cnt = (min_div + sample_cnt - 1) / sample_cnt;
        cnt_mul = step_cnt * sample_cnt;
        if (step_cnt > max_step_cnt)
            continue;
        if (cnt_mul < best_mul) {
            best_mul = cnt_mul;
            sample_div = sample_cnt;
            step_div = step_cnt;
            if (best_mul == min_div)
                break;
        }
    }
    sample_cnt = sample_div;
    step_cnt = step_div;
    sclk = hclk / (2 * sample_cnt * step_cnt);
    if (sclk > khz) {
        LTRACEF_LEVEL(ERR, "i2c%d %s mode: unsupported speed (%dkhz)\n",
            i2c->id, (speed_hkz > MAX_FS_MODE_SPEED) ?  "HS" : "ST/FT", khz);
        return ERR_NOT_VALID;
    }

    /* which the step_cnt needn't minus 1 when sample_cnt==1 */
    if (--sample_cnt)
        step_cnt--;

    *timing_step_cnt = step_cnt;
    *timing_sample_cnt = sample_cnt;

    return 0;
}

/*Set i2c port speed*/
static s32 i2c_set_speed(struct mt_i2c *i2c)
{
    int ret = 0;
    u32 step_cnt = 0;
    u32 sample_cnt = 0;
    u32 l_step_cnt = 0;
    u32 l_sample_cnt = 0;
    u32 speedInHz;
    u32 clk_src_in_khz = i2c->clk;
    u32 duty = 50;
    u32 high_speed_reg;
    u32 timing_reg;
    u32 ltiming_reg;

    speedInHz = i2c->speed;

    if (speedInHz > MAX_FS_MODE_SPEED && !i2c->pushpull) {
        /* Set the high speed mode register */
        ret = mtk_i2c_calculate_speed(i2c, clk_src_in_khz,
            MAX_FS_MODE_SPEED, &l_step_cnt, &l_sample_cnt);
        if (ret < 0)
            return ret;

        ret = mtk_i2c_calculate_speed(i2c, clk_src_in_khz,
            speedInHz, &step_cnt, &sample_cnt);
        if (ret < 0)
            return ret;

        high_speed_reg = 0x3 |
            (sample_cnt & 0x7) << 12 |
            (step_cnt & 0x7) << 8;

        timing_reg = (l_sample_cnt & 0x7) << 8 | (l_step_cnt & 0x3f) << 0;
        ltiming_reg = (l_sample_cnt << 6) | (l_step_cnt << 0) |
            (sample_cnt & 0x7) << 12 |
            (step_cnt & 0x7) << 9;
    } else {
        if (speedInHz > MAX_ST_MODE_SPEED && speedInHz <= MAX_FS_MODE_SPEED)
            duty = DUTY_CYCLE;
        ret = mtk_i2c_calculate_speed(i2c, clk_src_in_khz,
            (speedInHz * 50 / duty), &step_cnt, &sample_cnt);
        if (ret < 0)
            return ret;

        ret = mtk_i2c_calculate_speed(i2c, clk_src_in_khz,
            (speedInHz * 50 / (100 - duty)), &l_step_cnt, &l_sample_cnt);
        if (ret < 0)
            return ret;

        timing_reg = (sample_cnt & 0x7) << 8 | (step_cnt & 0x3f) << 0;
        ltiming_reg = (l_sample_cnt & 0x7) << 6 |
            (l_step_cnt & 0x3f) << 0;
        /* Disable the high speed transaction */
        high_speed_reg = 0x0;
    }

    i2c->timing_reg = timing_reg;
    i2c->ltiming_reg = ltiming_reg;
    i2c->high_speed_reg = high_speed_reg;

    i2c_set_timing_regs(i2c);
    return ret;
}

static void _i2c_dump_info(struct mt_i2c *i2c)
{
    LTRACEF_LEVEL(ERR, "I2C structure:\n"
        "Clk=%d,Id=%d,St_rs=%x,Dma_en=%x,Op=%x,Poll_en=%x,Irq_stat=%x\n"
        "Trans_len=%x,Trans_num=%x,Trans_auxlen=%x,Data_size=%x,speed=%d\n",
        i2c->clk, i2c->id, i2c->st_rs, i2c->dma_en, i2c->op, i2c->poll_en,
        i2c->irq_stat, i2c->trans_data.trans_len, i2c->trans_data.trans_num,
        i2c->trans_data.trans_auxlen, i2c->trans_data.data_size, i2c->speed);

    LTRACEF_LEVEL(ERR, "base address 0x%lx\n", i2c->base);
    LTRACEF_LEVEL(ERR, "I2C register:\n"
        "SLAVE_ADDR=%x,INTR_MASK=%x,INTR_STAT=%x,CONTROL=%x,TRANSFER_LEN=%x\n"
        "TRANSAC_LEN=%x,DELAY_LEN=%x,TIMING=%x,START=%x,FIFO_STAT=%x\n"
        "IO_CONFIG=%x,HS=%x,DCM_EN=%x,DEBUGSTAT=%x\n"
        "EXT_CONF=%x,TRANSFER_LEN_AUX=%x\n",
        (i2c_readl(i2c, OFFSET_SLAVE_ADDR)),
        (i2c_readl(i2c, OFFSET_INTR_MASK)),
        (i2c_readl(i2c, OFFSET_INTR_STAT)),
        (i2c_readl(i2c, OFFSET_CONTROL)),
        (i2c_readl(i2c, OFFSET_TRANSFER_LEN)),
        (i2c_readl(i2c, OFFSET_TRANSAC_LEN)),
        (i2c_readl(i2c, OFFSET_DELAY_LEN)),
        (i2c_readl(i2c, OFFSET_TIMING)),
        (i2c_readl(i2c, OFFSET_START)),
        (i2c_readl(i2c, OFFSET_FIFO_STAT)),
        (i2c_readl(i2c, OFFSET_IO_CONFIG)),
        (i2c_readl(i2c, OFFSET_HS)),
        (i2c_readl(i2c, OFFSET_DCM_EN)),
        (i2c_readl(i2c, OFFSET_DEBUGSTAT)),
        (i2c_readl(i2c, OFFSET_EXT_CONF)),
        (i2c_readl(i2c, OFFSET_TRANSFER_LEN_AUX)));

    return;
}
static s32 _i2c_deal_result(struct mt_i2c *i2c)
{
    long tmo = 1;
    u16 data_size = 0;
    u8 *ptr = i2c->msg_buf;
    bool TRANSFER_ERROR = false;
    s32 ret = 0;
    long tmo_poll = 0xffff;

    if (i2c->poll_en) {
        /*master read && poll mode*/
        for (;;) {
            /*check the interrupt status register*/
            i2c->irq_stat = i2c_readl(i2c, OFFSET_INTR_STAT);
            if (i2c->irq_stat & (I2C_HS_NACKERR | I2C_ACKERR)) {
                TRANSFER_ERROR = true;
                tmo = 1;
                break;
            } else if (i2c->irq_stat &  I2C_TRANSAC_COMP) {
                //transfer complete
                tmo = 1;
                break;
            }
            tmo_poll--;
            if (tmo_poll == 0) {
                tmo = 0;
                break;
            }
        }
    }

    /*Check the transfer status*/
    if (!(tmo == 0) && TRANSFER_ERROR == false) {
        /*Transfer success ,we need to get data from fifo*/
        if ((!i2c->dma_en) && (i2c->op == I2C_MASTER_RD ||
            i2c->op == I2C_MASTER_WRRD)) {
            /*only read mode or write_read mode and fifo mode need to get data*/
            if (i2c->op != I2C_MASTER_WRRD)
                data_size = i2c->trans_data.trans_len;
            else
                data_size = i2c->trans_data.trans_auxlen;

            while (data_size--) {
                *ptr = i2c_readb(i2c, OFFSET_DATA_PORT);
                ptr++;
            }
        }
    } else {
        /*Timeout or ACKERR*/
        if (tmo == 0) {
            LTRACEF_LEVEL(ERR, "id=%d,addr: %x, transfer timeout\n",
                          i2c->id, i2c->addr);
            ret = ERR_TIMED_OUT;
        } else {
            LTRACEF_LEVEL(ERR, "id=%d,addr: %x, transfer error\n",
                          i2c->id, i2c->addr);
            ret = ERR_I2C_NACK;
        }
        if (i2c->irq_stat & I2C_HS_NACKERR)
            LTRACEF_LEVEL(ERR, "I2C_HS_NACKERR\n");
        if (i2c->irq_stat & I2C_ACKERR)
            LTRACEF_LEVEL(ERR, "I2C_ACKERR\n");
        if (i2c->filter_msg == false) { //TEST
            _i2c_dump_info(i2c);
        }

        /*Reset i2c port*/
        i2c_writel(i2c, OFFSET_SOFTRESET, I2C_SOFT_RST);
        /*Set slave address*/
        i2c_writel(i2c, OFFSET_SLAVE_ADDR, I2C_SLAVE_ADDR_CLR);
        /*Clear interrupt status*/
        i2c_writel(i2c, OFFSET_INTR_STAT,
                   (I2C_HS_NACKERR|I2C_ACKERR|I2C_TRANSAC_COMP));
        /*Clear fifo address*/
        i2c_writel(i2c, OFFSET_FIFO_ADDR_CLR, I2C_FIFO_ADDR_CLR);
    }

    return ret;
}


static void _i2c_write_reg(struct mt_i2c *i2c)
{
    u8 *ptr = i2c->msg_buf;
    u32 data_size = i2c->trans_data.data_size;
    u32 addr_reg = 0;

    /*set start condition */
    if (i2c->speed <= MAX_ST_MODE_SPEED)
        i2c_writel(i2c, OFFSET_EXT_CONF, 0x8001);
    else
        i2c_writel(i2c, OFFSET_EXT_CONF, 0x1801);

    i2c_writel(i2c, OFFSET_DELAY_LEN, I2C_DELAY_LEN);

    /*Set ioconfig*/
    if (i2c->pushpull) {
        i2c->control_reg &= (~I2C_CONTROL_CLK_EXT_EN);
        i2c_writel(i2c, OFFSET_IO_CONFIG, I2C_IO_CONFIG_PUSH_PULL);
    } else {
        i2c_writel(i2c, OFFSET_IO_CONFIG, I2C_IO_CONFIG_OPEN_DRAIN);
    }
    i2c_writel(i2c, OFFSET_CONTROL, i2c->control_reg);

    /*Set slave address*/
    addr_reg = i2c->read_flag ? ((i2c->addr << 1) | 0x1) :
                                ((i2c->addr << 1) & ~0x1);
    i2c_writel(i2c, OFFSET_SLAVE_ADDR, addr_reg);
    /*Clear interrupt status*/
    i2c_writel(i2c, OFFSET_INTR_STAT,
               (I2C_HS_NACKERR | I2C_ACKERR | I2C_TRANSAC_COMP));
    /*Clear fifo address*/
    i2c_writel(i2c, OFFSET_FIFO_ADDR_CLR, I2C_FIFO_ADDR_CLR);
    /*Setup the interrupt mask flag*/
    if (i2c->poll_en)
        /*Disable interrupt*/
        i2c_writel(i2c, OFFSET_INTR_MASK, i2c_readl(i2c, OFFSET_INTR_MASK) &
                   ~(I2C_HS_NACKERR | I2C_ACKERR | I2C_TRANSAC_COMP));
    else
         /*Enable interrupt*/
        i2c_writel(i2c, OFFSET_INTR_MASK, i2c_readl(i2c, OFFSET_INTR_MASK) |
                   (I2C_HS_NACKERR | I2C_ACKERR | I2C_TRANSAC_COMP));
    /*Set transfer len */
    i2c_writel(i2c, OFFSET_TRANSFER_LEN, i2c->trans_data.trans_len & 0xFFFF);
    i2c_writel(i2c, OFFSET_TRANSFER_LEN_AUX,
               i2c->trans_data.trans_auxlen & 0xFFFF);
    /*Set transaction len*/
    i2c_writel(i2c, OFFSET_TRANSAC_LEN, i2c->trans_data.trans_num & 0xFF);

    /*Prepare buffer data to start transfer*/

    if (i2c->dma_en) {
        /* Reset I2C DMA status */
        writel(0x0001, i2c->pdmabase + OFFSET_RST);
        if (i2c->op == I2C_MASTER_RD) {
            writel(0x0000, i2c->pdmabase + OFFSET_INT_FLAG);
            writel(0x0001, i2c->pdmabase + OFFSET_CON);
            writel((u32)(addr_t)i2c->msg_buf,
                   i2c->pdmabase + OFFSET_RX_MEM_ADDR);
            writel(i2c->trans_data.data_size, i2c->pdmabase + OFFSET_RX_LEN);
        } else if (i2c->op == I2C_MASTER_WR) {
            writel(0x0000, i2c->pdmabase + OFFSET_INT_FLAG);
            writel(0x0000, i2c->pdmabase + OFFSET_CON);
            writel((u32)(addr_t)i2c->msg_buf,
                   i2c->pdmabase + OFFSET_TX_MEM_ADDR);
            writel(i2c->trans_data.data_size, i2c->pdmabase + OFFSET_TX_LEN);
        } else {
            writel(0x0000, i2c->pdmabase + OFFSET_INT_FLAG);
            writel(0x0000, i2c->pdmabase + OFFSET_CON);
            writel((u32)(addr_t)i2c->msg_buf,
                   i2c->pdmabase + OFFSET_TX_MEM_ADDR);
            writel((u32)(addr_t)i2c->msg_buf,
                   i2c->pdmabase + OFFSET_RX_MEM_ADDR);
            writel(i2c->trans_data.trans_len, i2c->pdmabase + OFFSET_TX_LEN);
            writel(i2c->trans_data.trans_auxlen, i2c->pdmabase + OFFSET_RX_LEN);
        }
        I2C_MB();
        writel(0x0001, i2c->pdmabase + OFFSET_EN);

        LTRACEF("addr %.2x dma %.2X byte\n",
                i2c->addr, i2c->trans_data.data_size);
        LTRACEF("DMA Register:INT_FLAG:0x%x,CON:0x%x,TX_MEM_ADDR:0x%x,\n"
            "RX_MEM_ADDR:0x%x,TX_LEN:0x%x,RX_LEN:0x%x,EN:0x%x\n",
            readl(i2c->pdmabase + OFFSET_INT_FLAG),
            readl(i2c->pdmabase + OFFSET_CON),
            readl(i2c->pdmabase + OFFSET_TX_MEM_ADDR),
            readl(i2c->pdmabase + OFFSET_RX_MEM_ADDR),
            readl(i2c->pdmabase + OFFSET_TX_LEN),
            readl(i2c->pdmabase + OFFSET_RX_LEN),
            readl(i2c->pdmabase + OFFSET_EN));

    } else {
        /*Set fifo mode data*/
        if (i2c->op == I2C_MASTER_RD) {
            /*do not need set fifo data*/
        } else {
            /*both write && write_read mode*/
            while (data_size--) {
                i2c_writeb(i2c, OFFSET_DATA_PORT, *ptr);
                ptr++;
            }
        }
    }
    /*Set trans_data*/
    i2c->trans_data.data_size = data_size;

}
static s32 _i2c_get_transfer_len(struct mt_i2c *i2c)
{
    s32 ret = NO_ERROR;
    u16 trans_num = 0;
    u16 data_size = 0;
    u16 trans_len = 0;
    u16 trans_auxlen = 0;
    /*Get Transfer len and transaux len*/
    if (i2c->dma_en == false) {
        /*non-DMA mode*/
        if (i2c->op != I2C_MASTER_WRRD) {
            trans_len = (i2c->msg_len) & 0xFFFF;
            trans_num = (i2c->msg_len >> 16) & 0xFF;
            if (trans_num == 0)
                trans_num = 1;
            trans_auxlen = 0;
            data_size = trans_len*trans_num;

            if (!trans_len || !trans_num ||
                trans_len*trans_num > I2C_FIFO_SIZE) {
                LTRACEF_LEVEL(ERR, " non-WRRD transfer length is not right.\n"
                             "trans_len=%x, tans_num=%x, trans_auxlen=%x\n",
                    trans_len, trans_num, trans_auxlen);
                ret = ERR_INVALID_ARGS;
                goto err;
            }
        } else {
            trans_len = (i2c->msg_len) & 0xFFFF;
            trans_auxlen = (i2c->msg_len >> 16) & 0xFFFF;
            trans_num = 2;
            data_size = trans_len;
            if (!trans_len || !trans_auxlen || trans_len > I2C_FIFO_SIZE ||
                trans_auxlen > I2C_FIFO_SIZE) {
                LTRACEF_LEVEL(ERR, " WRRD transfer length is not right.\n"
                             "trans_len=%x, tans_num=%x, trans_auxlen=%x\n",
                    trans_len, trans_num, trans_auxlen);
                ret = ERR_INVALID_ARGS;
                goto err;
            }
        }
    } else {
        /*DMA mode*/
        if (i2c->op != I2C_MASTER_WRRD) {
            trans_len = (i2c->msg_len) & 0xFFFF;
            trans_num = (i2c->msg_len >> 16) & 0xFF;
            if (trans_num == 0)
                trans_num = 1;
            trans_auxlen = 0;
            data_size = trans_len*trans_num;

            if (!trans_len || !trans_num || trans_len > MAX_DMA_TRANS_SIZE ||
                trans_num > MAX_DMA_TRANS_NUM) {
                LTRACEF_LEVEL(ERR, "DMA non-WRRD transfer length is bad.\n"
                             "trans_len=%x, tans_num=%x, trans_auxlen=%x\n",
                    trans_len, trans_num, trans_auxlen);
                ret = ERR_INVALID_ARGS;
                goto err;
            }
            LTRACEF("DMA non-WRRD mode!\n"
                    "trans_len=%x, tans_num=%x, trans_auxlen=%x\n",
                trans_len, trans_num, trans_auxlen);
        } else {
            trans_len = (i2c->msg_len) & 0xFFFF;
            trans_auxlen = (i2c->msg_len >> 16) & 0xFFFF;
            trans_num = 2;
            data_size = trans_len;
            if (!trans_len || !trans_auxlen || trans_len > MAX_DMA_TRANS_SIZE ||
                trans_auxlen > MAX_DMA_TRANS_NUM) {
                LTRACEF_LEVEL(ERR, " DMA WRRD transfer length is not right.\n"
                             "trans_len=%x, tans_num=%x, trans_auxlen=%x\n",
                    trans_len, trans_num, trans_auxlen);
                ret = ERR_INVALID_ARGS;
                goto err;
            }
            LTRACEF("DMA WRRD mode!\n"
                   "trans_len=%x, tans_num=%x, trans_auxlen=%x\n",
                trans_len, trans_num, trans_auxlen);
        }
    }

    i2c->trans_data.trans_num = trans_num;
    i2c->trans_data.trans_len = trans_len;
    i2c->trans_data.data_size = data_size;
    i2c->trans_data.trans_auxlen = trans_auxlen;

err:

    return ret;
}

static void _i2c_hw_reset(struct mt_i2c *i2c)
{
    writel(0x0001, i2c->pdmabase + OFFSET_RST);
    writel(0x0000, i2c->pdmabase + OFFSET_RST);
    writel(0x0006, i2c->pdmabase + OFFSET_RST);
    writel(0x0021, i2c->base + OFFSET_SOFTRESET);
    writel(0x0000, i2c->pdmabase + OFFSET_RST);
    writel(0x0000, i2c->base + OFFSET_SOFTRESET);
}

static s32 _i2c_transfer_interface(struct mt_i2c *i2c)
{
    s32 ret = 0;
    u8 *ptr = i2c->msg_buf;

    if (i2c->dma_en) {
        LTRACEF("DMA Transfer mode!\n");
        if (i2c->pdmabase == 0) {
            LTRACEF_LEVEL(ERR, " I2C%d doesnot support DMA mode!\n", i2c->id);
            ret = ERR_INVALID_ARGS;
            goto err;
        }
        if ((addr_t)ptr > DMA_ADDRESS_HIGH) {
            LTRACEF_LEVEL(ERR,
                         " DMA mode should use physical buffer address!\n");
            ret = ERR_INVALID_ARGS;
            goto err;
        }
        _i2c_hw_reset(i2c);
    }

    i2c->irq_stat = 0;

    ret = _i2c_get_transfer_len(i2c);
    if (ret)
        goto err;

    /*Set Control Register*/
    i2c->control_reg = I2C_CONTROL_ACKERR_DET_EN | I2C_CONTROL_CLK_EXT_EN;

    if (i2c->dma_en)
        i2c->control_reg |= I2C_CONTROL_DMA_EN;

    if (i2c->op == I2C_MASTER_WRRD)
        i2c->control_reg |= I2C_CONTROL_DIR_CHANGE;

    if (i2c->speed > MAX_FS_MODE_SPEED || (i2c->trans_data.trans_num > 1 &&
        i2c->st_rs == I2C_TRANS_REPEATED_START)) {
        i2c->control_reg |= I2C_CONTROL_RS;
    }

    _i2c_write_reg(i2c);

    /*All register must be prepared before setting the start bit [SMP]*/
    I2C_MB();
    LTRACEF("Before start .....\n");
    /*Start the transfer*/
    i2c_writel(i2c, OFFSET_START, I2C_TRANSAC_START);
    ret = _i2c_deal_result(i2c);
    LTRACEF("After i2c transfer .....\n");
err:

    return ret;
}

/*-----------------------------------------------------------------------
 * new read interface: Read bytes
 *   bus_id:  i2c bus number.
 *   device_addr: i2c slave device adderss.
 *   buffer:  Where to read/write the data.
 *   len:     How many bytes to read/write
 *   Returns: ERROR_CODE
 */
s32 mtk_i2c_read(u8 bus_id, u8 device_addr, u8 *buffer,
    u32 len, void *reserve)
{
    s32 ret = NO_ERROR;
    struct mt_i2c *i2c;

    if (bus_id >= I2C_NR) {
        LTRACEF_LEVEL(ERR, "bus_id is invalid.\n");
        return ERR_INVALID_ARGS;
    }

    if (device_addr == 0) {
        LTRACEF_LEVEL(ERR, "addr is invalid.\n");
        return ERR_INVALID_ARGS;
    }

    if (buffer == NULL) {
        LTRACEF_LEVEL(ERR, "data buffer is NULL.\n");
        return ERR_INVALID_ARGS;
    }

    i2c = &mtk_i2c[bus_id];

    ret = mutex_acquire_timeout(&i2c->bus_lock, I2C_LOCK_TIMEOUT);
    if (ret)
        return ERR_BUSY;

    //read
    i2c->addr = device_addr;
    i2c->id = bus_id;
    i2c->read_flag |= I2C_M_RD;
    i2c->op = I2C_MASTER_RD;
    i2c->msg_buf = buffer;
    i2c->msg_len = len;

    ret = _i2c_transfer_interface(i2c);

    mutex_release(&i2c->bus_lock);

    if (ret < 0)
        LTRACEF_LEVEL(ERR, "read %d bytes fails,ret=%d.\n", i2c->msg_len, ret);

    return ret;
}

/*-----------------------------------------------------------------------
 * new read interface: Write bytes
 *   bus_id:  i2c bus number.
 *   device_addr: i2c slave device adderss.
 *   buffer:  Where to read/write the data.
 *   len:     How many bytes to read/write
 *   Returns: ERROR_CODE
 */

s32 mtk_i2c_write(u8 bus_id, u8 device_addr, u8 *buffer,
    u32 len, void *reserve)
{
    s32 ret = NO_ERROR;
    struct mt_i2c *i2c;

    if (bus_id >= I2C_NR) {
        LTRACEF_LEVEL(ERR, "bus_id is invalid.\n");
        return ERR_INVALID_ARGS;
    }

    if (device_addr == 0) {
        LTRACEF_LEVEL(ERR, "addr is invalid.\n");
        return ERR_INVALID_ARGS;
    }

    if (buffer == NULL) {
        LTRACEF_LEVEL(ERR, "data buffer is NULL.\n");
        return ERR_INVALID_ARGS;
    }

    i2c = &mtk_i2c[bus_id];

    ret = mutex_acquire_timeout(&i2c->bus_lock, I2C_LOCK_TIMEOUT);
    if (ret)
        return ERR_BUSY;

    //write
    i2c->id = bus_id;
    i2c->addr = device_addr;
    i2c->read_flag = !I2C_M_RD;
    i2c->op = I2C_MASTER_WR;
    i2c->msg_buf = buffer;
    i2c->msg_len = len;

    ret = _i2c_transfer_interface(i2c);

    mutex_release(&i2c->bus_lock);

    if (ret < 0)
        LTRACEF_LEVEL(ERR, "Write %d bytes fails,ret=%d.\n", i2c->msg_len, ret);

    return ret;
}

/*-----------------------------------------------------------------------
 * New write then read back interface: Write bytes then read bytes
 *   bus_id:  i2c bus number.
 *   device_addr: i2c slave device adderss.
 *   buffer:  Where to read/write the data.
 *   write_len:     How many bytes to write
 *   read_len:     How many bytes to read
 *   Returns: ERROR_CODE
 */
s32 mtk_i2c_write_read(u8 bus_id, u8 device_addr, u8 *buffer, u32 write_len,
    u32 read_len, void *reserve)
{
    s32 ret = NO_ERROR;
    struct mt_i2c *i2c;

    if (bus_id >= I2C_NR) {
        LTRACEF_LEVEL(ERR, "bus_id is invalid.\n");
        return ERR_INVALID_ARGS;
    }

    if (device_addr == 0) {
        LTRACEF_LEVEL(ERR, "addr is invalid.\n");
        return ERR_INVALID_ARGS;
    }

    if (buffer == NULL) {
        LTRACEF_LEVEL(ERR, "data buffer is NULL.\n");
        return ERR_INVALID_ARGS;
    }

    i2c = &mtk_i2c[bus_id];

    ret = mutex_acquire_timeout(&i2c->bus_lock, I2C_LOCK_TIMEOUT);
    if (ret)
        return ERR_BUSY;

    //write and read
    i2c->id = bus_id;
    i2c->addr = device_addr;
    i2c->op = I2C_MASTER_WRRD;
    i2c->read_flag = !I2C_M_RD;
    i2c->msg_buf = buffer;
    i2c->msg_len = ((read_len & 0xFFFF) << 16) | (write_len & 0xFFFF);

    ret = _i2c_transfer_interface(i2c);

    mutex_release(&i2c->bus_lock);

    if (ret < 0)
        LTRACEF_LEVEL(ERR, "write_read 0x%x bytes fails,ret=%d.\n",
                      i2c->msg_len, ret);

    return ret;
}

status_t i2c_transmit(int bus, uint8_t address, const void *buf, size_t count)
{
    s32 ret = NO_ERROR;

    if (bus < 0) {
        LTRACEF_LEVEL(ERR, "bus is invalid(negative).\n");
        return ERR_INVALID_ARGS;
    }

    if (count == 0) {
        LTRACEF_LEVEL(ERR, "count is invalid(zero).\n");
        return ERR_INVALID_ARGS;
    }

    ret = mtk_i2c_write(bus, address, (u8 *)buf, count, NULL);

    return ret;
}

status_t i2c_receive(int bus, uint8_t address, void *buf, size_t count)
{
    s32 ret = NO_ERROR;

    if (bus < 0) {
        LTRACEF_LEVEL(ERR, "bus is invalid(negative).\n");
        return ERR_INVALID_ARGS;
    }

    if (count == 0) {
        LTRACEF_LEVEL(ERR, "count is invalid(zero).\n");
        return ERR_INVALID_ARGS;
    }

    ret = mtk_i2c_read(bus, address, (u8 *)buf, count, NULL);

    return ret;
}

status_t i2c_write_reg_bytes(int bus, uint8_t address, uint8_t reg,
                             const uint8_t *buf, size_t count)
{
    s32 ret = NO_ERROR;
    uint8_t *w_buf;

    if (bus < 0) {
        LTRACEF_LEVEL(ERR, "bus is invalid(negative).\n");
        return ERR_INVALID_ARGS;
    }

    if (buf == NULL) {
        LTRACEF_LEVEL(ERR, "buffer is NULL.\n");
        return ERR_INVALID_ARGS;
    }

    if (count == 0) {
        LTRACEF_LEVEL(ERR, "count is invalid(zero).\n");
        return ERR_INVALID_ARGS;
    }

    w_buf = (uint8_t *)malloc(count + 1);
    if (w_buf == NULL) {
        LTRACEF_LEVEL(ERR, "Fail to alloc memory.\n");
        return ERR_NO_MEMORY;
    }

    w_buf[0] = reg;
    memcpy(w_buf + 1, buf, count);

    ret = mtk_i2c_write(bus, address, w_buf, count, NULL);

    free(w_buf);

    return ret;
}

status_t i2c_read_reg_bytes(int bus, uint8_t address, uint8_t reg, uint8_t *buf,
                            size_t count)
{
    s32 ret = NO_ERROR;
    uint8_t *r_buf;

    if (bus < 0) {
        LTRACEF_LEVEL(ERR, "bus is invalid(negative).\n");
        return ERR_INVALID_ARGS;
    }

    if (buf == NULL) {
        LTRACEF_LEVEL(ERR, "buffer is NULL.\n");
        return ERR_INVALID_ARGS;
    }

    if (count == 0) {
        LTRACEF_LEVEL(ERR, "count is invalid(zero).\n");
        return ERR_INVALID_ARGS;
    }

    r_buf = (uint8_t *)malloc(count + 1);
    if (r_buf == NULL) {
        LTRACEF_LEVEL(ERR, "Fail to alloc memory.\n");
        return ERR_NO_MEMORY;
    }

    r_buf[0] = reg;
    memcpy(r_buf + 1, buf, count);

    ret = mtk_i2c_write_read(bus, address, r_buf, 1, count, NULL);

    if (ret >= 0)
        memcpy(buf, r_buf, count);

    free(r_buf);

    return ret;
}

static unsigned int i2c_fdt_getprop_u32(const void *fdt, int nodeoffset,
                                        const char *name)
{
    const void *data = NULL;
    int len = 0;

    data = fdt_getprop(fdt, nodeoffset, name, &len);
    if (len > 0 && data)
        return fdt32_to_cpu(*(unsigned int *)data);
    else
        return 0;
}

static void i2c_fdt_getprop_u32_array(const void *fdt, int nodeoffset,
                                      const char *name, char *out_value,
                                      int maxlen)
{
    const void *data = NULL;
    int len = 0;

    data = fdt_getprop(fdt, nodeoffset, name, &len);
    if (len > 0 && data)
        memcpy(out_value, data, len > maxlen ? maxlen : len);
    else
        memset(out_value, 0, len > maxlen ? maxlen : len);
}

static int get_i2c_info_from_dts(void *fdt, struct mt_i2c_dts_info *mt_i2c_info)
{
    int offset;
    int ret;
    u32 i;
    char buffer[32];

    offset = fdt_path_offset(fdt, "/i2c_common");
    if (offset >= 0) {
        mt_i2c_info->clock_source = i2c_fdt_getprop_u32(fdt, offset,
                                                        "clock-source");
        mt_i2c_info->i2c_nr = i2c_fdt_getprop_u32(fdt, offset, "i2c-nr");
        i2c_fdt_getprop_u32_array(fdt, offset, "scl-gpio",
            (char *)mt_i2c_info->scl_gpio_num, I2C_NR);
        i2c_fdt_getprop_u32_array(fdt, offset, "sda-gpio",
            (char *)mt_i2c_info->sda_gpio_num, I2C_NR);
    } else
        return offset;

    if (mt_i2c_info->i2c_nr >= I2C_NR) {
        LTRACEF("i2c_nr(%u) from dts error.\n", mt_i2c_info->i2c_nr);
        return -1;
    }

    for (i = 0; i < mt_i2c_info->i2c_nr; i++) {
        memset(buffer, 0, sizeof(buffer));
        ret = snprintf(buffer, sizeof(buffer), "%s%d%s", "/i2c", i, "@");
        if ((ret < 0) || (ret >= sizeof(buffer))) {
            LTRACEF("string truncated error or output error, ret=%d.\n", ret);
            return -1;
        }
        offset = fdt_path_offset(fdt, buffer);
        if (offset >= 0) {
            mt_i2c_info->i2c_mode[i] = i2c_fdt_getprop_u32(fdt, offset, "i2c-mode");
            mt_i2c_info->internal_pull_en[i] = i2c_fdt_getprop_u32(fdt, offset, "internal-pull-up");
            mtk_i2c[i].speed = i2c_fdt_getprop_u32(fdt, offset, "speed");
            mtk_i2c[i].clk = mt_i2c_info->clock_source;
        }
    }

    return 0;
}

void set_gpio_i2c_mode(int gpio_num, int gpio_mode, int internal_pull_en)
{
    mt_set_gpio_mode(gpio_num, gpio_mode);
    mt_set_gpio_pull_enable(gpio_num, internal_pull_en);
    if (internal_pull_en)
        mt_set_gpio_pull_select(gpio_num, PULL_UP);
    mt_set_gpio_dir(gpio_num, DIR_IN);
    mt_set_gpio_smt(gpio_num, SMT_EN);
}

static void set_gpio_from_dts(struct mt_i2c_dts_info *mt_i2c_info)
{
    u32 i = 0;

    for (i = 0; i < mt_i2c_info->i2c_nr; i++) {
        set_gpio_i2c_mode(mt_i2c_info->scl_gpio_num[i],
            mt_i2c_info->i2c_mode[i], mt_i2c_info->internal_pull_en[i]);
        set_gpio_i2c_mode(mt_i2c_info->sda_gpio_num[i],
            mt_i2c_info->i2c_mode[i], mt_i2c_info->internal_pull_en[i]);
    }
}

static void i2c_set_bus_speed(int bus_num)
{
    int ret = 0;

    ret = i2c_set_speed(&mtk_i2c[bus_num]);
    if (ret < 0)
        LTRACEF_LEVEL(ERR, "bus(%d)i2c_set_speed fail,ret=%d\n", bus_num, ret);
}

static void i2c_bus_init(void)
{
    int i;

    i2c_config_init();

    for (i = 0; i < I2C_NR; i++) {
        i2c_set_bus_speed(i);
        mutex_init(&mtk_i2c[i].bus_lock);
    }
}

int i2c_hw_init(void *fdt)
{
    struct mt_i2c_dts_info mt_i2c_info;

    if (fdt) {
        if (fdt_check_header(fdt) != 0)
            panic("Bad DTB header.\n");
    } else {
        LTRACEF_LEVEL(MSG, "[i2c] fdt null, fall back to default\n");
        goto apply_default;
    }

    if (get_i2c_info_from_dts(fdt, &mt_i2c_info) >= 0) {
        set_gpio_from_dts(&mt_i2c_info);
        LTRACEF_LEVEL(MSG, "set i2c gpio for dts\n");
        goto bus_init;
    }

apply_default:
    i2c_gpio_default_init();
bus_init:
    i2c_bus_init();

    return 0;
}
