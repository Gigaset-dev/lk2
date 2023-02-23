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
#include <lib/zlib.h>
#include <stdio.h>
#include <string.h>
#include <trace.h>

#include "decompress_common.h"

#define LOCAL_TRACE 0

/* Decompress zlib compressed file */
int decompress_logo(void *in, void *out, int inlen, int outlen)
{
    int ret;
    int have = 0;
    z_stream strm;

    LTRACEF("in=0x%08x, out=0x%08x, inlen=%d, logolen=%d\n",
            (unsigned int)in, (unsigned int)out, inlen, outlen);

    memset(&strm, 0, sizeof(z_stream));
    /* allocate inflate state */
    strm.zalloc = Z_NULL;
    strm.zfree = Z_NULL;
    strm.opaque = Z_NULL;
    strm.avail_in = 0;
    strm.next_in = Z_NULL;
    ret = inflateInit(&strm);
    if (ret != Z_OK)
        return ret;

    /* decompress until deflate stream ends or end of file */
    do {
        strm.avail_in = inlen;

        if (strm.avail_in <= 0) {
            LTRACEF("strm.avail_in <= 0\n");
            break;
        }
        strm.next_in = (Bytef *)in;

        /* run inflate() on input until output buffer not full */
        do {
            strm.avail_out = outlen;
            strm.next_out = (Bytef *)out;
            ret = inflate(&strm, Z_NO_FLUSH);
            switch (ret) {
            case Z_NEED_DICT:
                ret = Z_DATA_ERROR;     /* and fall through */
            case Z_DATA_ERROR:
            case Z_MEM_ERROR:
                dprintf(CRITICAL, "Z_DATA_ERROR or Z_MEM_ERROR\n");
                (void)inflateEnd(&strm);
                return ret;
            }
            have += outlen - strm.avail_out;
        } while (strm.avail_out == 0);

        /* done when inflate() says it's done */
    } while (ret != Z_STREAM_END);

    if (ret == Z_STREAM_END)
        (void)inflateEnd(&strm); /* clean up and return */

    LTRACEF("have=%d\n", have);

    //return ret == Z_STREAM_END ? Z_OK : Z_DATA_ERROR;
    return have;
}
