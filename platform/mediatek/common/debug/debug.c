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
#include <dev/uart.h>
#include <platform/debug.h>

__WEAK unsigned int log_dynamic_switch(void)
{
    /*
     * for those projects:
     * which don't use dynamic_log.c
     * and always print uart log,
     * even if the DEBUG=0.
     */
    return 1;
}

__WEAK unsigned int log_port(void)
{
    return DEBUG_UART;
}

__WEAK unsigned int log_baudrate(void)
{
    return DEBUG_BAUDRATE;
}

void platform_dputc(char c)
{
    if (!log_dynamic_switch())
        return;

    if (c == '\n')
        uart_putc(log_port(), '\r');

    uart_putc(log_port(), c);
}

int platform_dgetc(char *c, bool wait)
{
    int ret = uart_getc((int)log_port(), wait);

    if (ret < 0)
        return ret;

    *c = (char)ret;

    return 0;
}
