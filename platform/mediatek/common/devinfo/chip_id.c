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
#include <assert.h>
#include <atags.h>
#include <chip_id.h>
#include <libfdt.h>
#include <reg.h>
#include <platform/hw_ver.h>
#include <set_fdt.h>
#include <sys/types.h>
#include <trace.h>

#define LOCAL_TRACE 0

/*chip id information*/
struct tag_chipid {
    u32 hw_code;
    u32 hw_subcode;
    u32 hw_ver;
    u32 sw_ver;
};

__WEAK u32 get_chip_sw_ver(void)
{
    return readl(APSW_VER);
}

__WEAK u32 get_chip_hw_ver(void)
{
    return readl(APHW_VER);
}

__WEAK u32 get_chip_hw_code(void)
{
    return readl(APHW_CODE);
}

__WEAK u32 get_chip_hw_subcode(void)
{
    return readl(APHW_SUBCODE);
}

__WEAK void platform_fdt_set_chipid(void *fdt)
{
    #define CID_LEN 5
    u32 ptr[CID_LEN] = {0};
    int offset, ret;

    ptr[0] = tag_size(tag_chipid);
    ptr[1] = get_chip_hw_code();
    ptr[2] = get_chip_hw_subcode();
    ptr[3] = get_chip_hw_ver();
    ptr[4] = get_chip_sw_ver();
    LTRACEF("hw_code = 0x%x\n", get_chip_hw_code());
    LTRACEF("hw_subcode = 0x%x\n", get_chip_hw_subcode());
    LTRACEF("hw_ver = 0x%x\n", get_chip_hw_ver());
    LTRACEF("sw_ver = 0x%x\n", get_chip_sw_ver());

    offset = fdt_path_offset(fdt, "/chosen");
    ASSERT(offset >= 0);

    ret = fdt_setprop(fdt, offset, "atag,chipid", ptr, sizeof(ptr));
    ASSERT(ret == 0);
}
SET_FDT_INIT_HOOK(platform_fdt_set_chipid, platform_fdt_set_chipid);
