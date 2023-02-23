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
#include <dev/uart.h>
#include <kernel/vm.h>
#include <platform/addressmap.h>
#include <platform/round.h>
#include <reg.h>
#include <uart_api.h>

struct mtk_uart {
    union {
        uint32_t thr;
        uint32_t rbr;
        uint32_t dll;
    };
    union{
        uint32_t ier;
        uint32_t dlm;
    };
    uint32_t fcr;
    uint32_t lcr;
    uint32_t mcr;
    uint32_t lsr;
    uint32_t msr;
    uint32_t reserved0[2];
    uint32_t highspeed;
    uint32_t sample_count;
    uint32_t sample_point;
    uint32_t reserved1[9];
    uint32_t fracdiv_l;
    uint32_t fracdiv_m;
};

STATIC_ASSERT(__offsetof(struct mtk_uart, sample_point) == 0x2c);

enum {
    FCR_FIFOE          = 1 << 0,
    FCR_CLRR           = 1 << 1,
    FCR_CLRT           = 1 << 2,
    FCR_TXFIFO_4B_TRI  = 1 << 4,
    FCR_RXFIFO_12B_TRI = 2 << 6,

    FCR_FIFO_INIT      = FCR_FIFOE | FCR_CLRR | FCR_CLRT,
    FCR_RX_CLEAR       = FCR_RXFIFO_12B_TRI | FCR_TXFIFO_4B_TRI | FCR_CLRR |
                         FCR_FIFOE,
};

enum {
    LCR_WLS_8       = 3 << 0,
    LCR_DLAB        = 1 << 7
};

enum {
    LSR_DR   = 1 << 0,
    LSR_THRE = 1 << 5,
    LSR_TEMT = 1 << 6
};

static struct mtk_uart *const uarts[] = {
    (struct mtk_uart *)UART0_BASE,
    (struct mtk_uart *)UART1_BASE
};

static void uart_setbrg(int port, unsigned int baud)
{
    unsigned int byte;
    unsigned int highspeed;
    unsigned int divisor;
    unsigned int sample_count, sample_point, fraction;

    unsigned short fraction_L_mapping[] = {
        0, 1, 0x5, 0x15, 0x55, 0x57, 0x57, 0x77, 0x7F, 0xFF, 0xFF
    };

    unsigned short fraction_M_mapping[] = {
        0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 3
    };

    assert(port >= 0);
    struct mtk_uart *uart = uarts[port];
    const unsigned int uartclk = SRC_CLK_FRQ;

    if (baud <= 115200) {
        highspeed = 0;
        divisor = DIV_ROUND_CLOSEST(uartclk, 16 * baud);
    } else {
        highspeed = 3;
        divisor = DIV_ROUND_UP(uartclk, 256 * baud);
    }

    if (highspeed == 3) {
        sample_count = (uartclk / (divisor * baud)) - 1;
        sample_point = (sample_count >> 1) - 1;

        writel(sample_count, &uart->sample_count);
        writel(sample_point, &uart->sample_point);

        /*count fraction to set fractoin register */
        fraction = ((uartclk  * 100) / baud / divisor) % 100;
        fraction = DIV_ROUND_CLOSEST(fraction, 10);

        writel(fraction_L_mapping[fraction], &uart->fracdiv_l);
        writel(fraction_M_mapping[fraction], &uart->fracdiv_m);
    }

    /* configure register */
    writel(highspeed, &uart->highspeed);

    byte = readl(&uart->lcr);
    writel((byte | LCR_DLAB), &uart->lcr);
    writel((divisor & 0xff), &uart->dll);
    writel(((divisor >> 8) & 0xff), &uart->dlm);
    writel(byte, &uart->lcr);
}

int uart_putc(int port, char c)
{
    assert((port >= 0));
    struct mtk_uart *uart = uarts[port];

    while (!(readl(&uart->lsr) & LSR_THRE))
        ;
    writel((unsigned int)c, &uart->thr);
    return 0;
}

int uart_getc(int port, bool wait)
{
    assert(port >= 0);
    struct mtk_uart *uart = uarts[port];

    do {
        if (readl(&uart->lsr) & LSR_DR)
            return (int)(readl(&uart->rbr) & 0xff);
    } while (wait);

    return -1;
}

void uart_flush_tx(int port)
{
    assert(port >= 0);
    while (!(readl(&uarts[port]->lsr) & LSR_TEMT))
        ;
}

void uart_flush_rx(int port)
{
    assert(port >= 0);
    writel(FCR_RX_CLEAR, &uarts[port]->fcr);
}

void uart_init_port(int port, uint baud)
{
    assert(((unsigned int)port < countof(uarts)) && (port >= 0));
    struct mtk_uart *uart = uarts[port];

    writel(FCR_FIFO_INIT, &uart->fcr);
    writel(LCR_WLS_8, &uart->lcr);
    uart_setbrg(port, baud);
}

unsigned int uart_base_to_id(addr_t base)
{
    unsigned int id = 0;
    vaddr_t base_v = 0;

    for (; id < countof(uarts); id++) {
        /*
         *  base == UART0_BASE when:
         *  a. lk is the first loader.
         *  b. log printing before pl_boottag hook.
         */
        if (base == (addr_t)uarts[id])
            return id;
         /*using pl_boottag with physical addr*/
         else  {
            base_v = (vaddr_t)paddr_to_kvaddr(base);
            if (base_v == (vaddr_t)uarts[id])
                return id;
        }
    }

    return id;
}

