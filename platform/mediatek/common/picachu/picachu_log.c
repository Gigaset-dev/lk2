/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */
/*****************************************************************************
 *
 *****************************************************************************/
#if LK_AS_BL2_EXT
#include <platform/picachu_def.h>
#else
#include "picachu_def.h"
#endif
#include "picachu_log.h"


/*****************************************************************************
 *
 *****************************************************************************/
#ifdef TARGET_BUILD_VARIANT_SLT
static char *pi_dram_log_buf;
static char *_g_log_ptr;
static char *_g_log_end;


/*****************************************************************************
 *
 *****************************************************************************/
static void _outchar(const char c)
{
    char *log_ptr = _g_log_ptr;
    char *log_end = _g_log_end;

    if (log_ptr < log_end) {
        *log_ptr = c;
        log_ptr++;
    }
    _g_log_ptr = log_ptr;
}

static void _outstr(const unsigned char *s)
{
    while (*s) {
        if (*s == '\n')
            _outchar('\r');
        _outchar(*s++);
    }
}

static void _outdec(unsigned long n)
{
    if (n >= 10) {
        _outdec(n / 10);
        n %= 10;
    }
    _outchar((unsigned char)(n + '0'));
}

static void _outhex(unsigned long n, long depth)
{
    if (depth)
        depth--;

    if ((n & ~0xf) || depth) {
        _outhex(n >> 4, depth);
        n &= 0xf;
    }

    if (n < 10)
        _outchar((unsigned char)(n + '0'));
    else
        _outchar((unsigned char)(n - 10 + 'A'));

}


static void _vprint(const char *fmt, va_list vl)
{
    unsigned char c;
    unsigned int reg = 1; /* argument register number (32-bit) */
    long l;

    while (*fmt) {
        c = *fmt++;
        switch (c) {
        case '%':
            c = *fmt++;
            switch (c) {
            case 'd':
                l = va_arg(vl, long);
                if (l < 0) {
                    _outchar('-');
                    l = -l;
                }
                _outdec((unsigned long)l);
                break;

            case 'u':
                _outdec(va_arg(vl, unsigned long));
                break;

            case 's':
                _outstr((const unsigned char *)va_arg(vl, char *));
                break;

            case 'x':
                _outhex(va_arg(vl, unsigned long), 0);
                break;

            case 'B':
                _outhex(va_arg(vl, unsigned long), 2);
                break;

            case 'H':
                _outhex(va_arg(vl, unsigned long), 4);
                break;

            case 'X':
                _outhex(va_arg(vl, unsigned long), 8);
                break;

            case 'c':
                c = va_arg(vl, int);
                _outchar(c);
                break;

            case 'l':
                if (*fmt == 'l' && *(fmt+1) == 'x') {
                    u32 ltmp;
                    u32 htmp;

                    #ifdef __ARM_EABI__
                    /* odd and unused argument */
                    if (reg & 0x1) {
                        reg++;
                        /* ignore this 32-bit register */
                        ltmp = va_arg(vl, unsigned int);
                    }
                    reg++;
                    #endif
                    ltmp = va_arg(vl, unsigned int);
                    htmp = va_arg(vl, unsigned int);

                    _outhex(htmp, 8);
                    _outhex(ltmp, 8);
                    fmt += 2;
                }
                break;

            case '%':
                _outchar('%');
                break;

            default:
                _outchar(' ');
                break;
            }
            reg++; /* one argument uses 32-bit register */
            break;

        case '\r':
            if (*fmt == '\n')
                fmt++;
            c = '\n';
            // fall through

        case '\n':
            _outchar('\r');
            // fall through

        default:
            _outchar(c);
        }
    }
}
#endif    //##ifdef TARGET_BUILD_VARIANT_SLT


/*****************************************************************************
 *
 *****************************************************************************/
unsigned long long picachu_log_init(void)
{
    unsigned long long pi_dram_log_addr = 0;

#ifdef TARGET_BUILD_VARIANT_SLT
    pi_dram_log_addr = (unsigned long long)KAL_malloc(PICACHU_LOG_SIZE, 16, KAL_USER_MSDC);

    if (pi_dram_log_addr == 0) {
        PRINTF("pi_dram_log_addr == 0\n");
        return 0;
    }
    pi_dram_log_buf = (char *)pi_dram_log_addr;
    PRINTF("pi_dram_log_buf = 0x%p\n", pi_dram_log_buf);
    _g_log_ptr = pi_dram_log_buf;
    _g_log_end = pi_dram_log_buf + PICACHU_LOG_SIZE;
#endif

    return pi_dram_log_addr;
}


void picachu_print(const char *fmt, ...)
{
#ifdef TARGET_BUILD_VARIANT_SLT
    va_list args;

    va_start(args, fmt);
    _vprint(fmt, args);
    va_end(args);
#else
    return;
#endif
}

void picachu_log_dump(void)
{
#ifdef TARGET_BUILD_VARIANT_SLT
    if (pi_dram_log_buf) {
        PRINTF("%s", pi_dram_log_buf);
        KAL_free(pi_dram_log_buf);
        pi_dram_log_buf = NULL;
    }
#else
    return;
#endif
}
