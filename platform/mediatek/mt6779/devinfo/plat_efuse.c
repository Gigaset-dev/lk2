/*
 * Copyright (c) 2019 MediaTek Inc.
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
#include <errno.h>
#include <platform/sec_devinfo.h>
#include <stdlib.h>
#include <string.h>
#include <trace.h>

#define LOCAL_TRACE 0


/**************************************************************
 * Module name porting
 **************************************************************/
int get_devinfo_model_name(char *ptr, size_t ptr_size)
{
    unsigned int segment = get_devinfo_with_index(7) & 0xFF;
    int model_index = 0;
    const char * const model_name[] = {
        "MT6779V/WU",
        "MT6779V/CU",
        "MT6779V/WY",
        "MT6779V/CY",
        "MT6779V/WB",
        "MT6779V/CB",
        "MT6779V/WE",
        "MT6779V/CE",
        "MT6779V/WV",
        "MT6779V/CV",
        "MT6779V/WZ",
        "MT6779V/CZ",
        "MT6779V/WC",
        "MT6779V/CC",
        "MT6779V/WF",
        "MT6779V/CF",
        "MT6779V/CD",
        "MT6779V/WD"
    };

    switch (segment) {
    case 0x80:
        model_index = 0;
        break;
    case 0x01:
        model_index = 1;
        break;
    case 0x40:
        model_index = 2;
        break;
    case 0x02:
        model_index = 3;
        break;
    case 0xC0:
        model_index = 4;
        break;
    case 0x03:
        model_index = 5;
        break;
    case 0xA0:
        model_index = 6;
        break;
    case 0x05:
        model_index = 7;
        break;
    case 0x20:
        model_index = 8;
        break;
    case 0x04:
        model_index = 9;
        break;
    case 0x60:
        model_index = 10;
        break;
    case 0x06:
        model_index = 11;
        break;
    case 0x10:
        model_index = 12;
        break;
    case 0x08:
        model_index = 13;
        break;
    case 0x90:
        model_index = 14;
        break;
    case 0x09:
        model_index = 15;
        break;
    case 0xe0:
        model_index = 16;
        break;
    case 0x07:
        model_index = 17;
        break;
    default:
        dprintf(CRITICAL, "segment not support\n");
        return 1;
    }

    unsigned int proj_code = get_devinfo_with_index(29) & 0x3FFF;

    dprintf(CRITICAL, "proj_code: 0x%x\n", proj_code);
    dprintf(CRITICAL, "[LK] segment = 0x%x\n", segment);

    snprintf(ptr, ptr_size, "%s", model_name[model_index]);

    return 0;
}

/****************************************************
 * Fsource
 * return 0 : success
 ****************************************************/
__WEAK unsigned int efuse_fsource_set(void)
{
    return 0;
}

__WEAK unsigned int efuse_fsource_close(void)
{
    return 0;
}

__WEAK unsigned int efuse_fsource_is_enabled(void)
{
    return 0;
}
