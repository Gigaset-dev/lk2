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
#include <debug.h>
#include "decompressor.h"
#include <lib/zlib.h>
#include <lib/zutil.h>
#include <malloc.h>
#include <profiling.h>
#include <string.h>

#define RESERVED            0xe0
#define COMMENT             0x10
#define ORIG_NAME           8
#define EXTRA_FIELD         4
#define HEAD_CRC            2
#define ZALLOC_ALIGNMENT    16

static void *zlib_alloc(void *x, unsigned int items, unsigned int size)
{
    void *p = NULL;

    size *= items;
    size = (size + ZALLOC_ALIGNMENT - 1) & ~(ZALLOC_ALIGNMENT - 1);

    p = malloc(size);
    return p;
}

static void zlib_free(void *x, void *addr)
{
    free(addr);
}


static int zunzip(unsigned char *src, unsigned long *lenp, void *dst, int dstlen, int offset)
{
    z_stream s;
    int r;

    memset(&s, 0, sizeof(z_stream));

    r = inflateInit2(&s, -MAX_WBITS);
    if (r != Z_OK) {
        ZFREE(&s, s.state);
        dprintf(CRITICAL, "inflateInit2() error, returned %d\n", r);
        return -1;
    }
    s.zalloc = zlib_alloc;
    s.zfree = zlib_free;
    s.next_in = src + offset;
    s.next_out = dst;
    s.avail_in = *lenp - offset;
    s.avail_out = dstlen;
    r = inflate(&s, Z_FINISH);
    if (r != Z_STREAM_END) {
        dprintf(CRITICAL, "inflate() error! return %d\n", r);
        inflateEnd(&s);
        return -1;
    }
    *lenp = s.next_out - (unsigned char *) dst;
    inflateEnd(&s);

    return 0;
}

static int gunzip(unsigned char *src, unsigned long *lenp, void *dst, int dstlen)
{
    unsigned long i, flags;
    int ret;

    i = 10;
    flags = src[3];

    /* Skip the header of gzip */
    if (src[2] != Z_DEFLATED || (flags & RESERVED) != 0) {
        dprintf(CRITICAL, "gzip header is not correct. Error!\n");
        return -1;
    }
    if ((flags & EXTRA_FIELD) != 0)
        i = 12 + src[10] + (src[11] << 8);
    if ((flags & ORIG_NAME) != 0)
        while (src[i++] != 0)
            ;
    if ((flags & COMMENT) != 0)
        while (src[i++] != 0)
            ;
    if ((flags & HEAD_CRC) != 0)
        i += 2;

    if (i >= *lenp) {
        dprintf(CRITICAL, "%s header is bigger than expectation! Error!\n", __func__);
        return -1;
    }

    ret = zunzip(src, lenp, dst, dstlen, i);

    return ret;
}


int decompress_kernel(unsigned char *in, void *out, int inlen, int outlen)
{
    unsigned long lenp = inlen;
    int ret;

    PROFILING_START("decompress_kernel");
    ret = gunzip(in, &lenp, out, outlen);
    PROFILING_END();

    return ret;
}

