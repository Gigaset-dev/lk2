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
#include <debug.h>
#include <errno.h>
#include <lib/bio.h>
#include <libfdt.h>
#include <platform/sec_devinfo.h>
#ifdef WITH_PLATFORM_MEDIATEK_COMMON_VIDEO
#include <platform/video.h>
#endif
#include <set_fdt.h>
#include <stdlib.h>
#include <trace.h>

#define LOCAL_TRACE 0

#define EFUSE_PART_NAME "efuse"
#define CHK_SIZE 20
#define CHK_THRESHOLD 19

#define MOD_NAME_LEN 32

#define EFUSE_NONE   0x00
#define EFUSE_FINE   0xAA
#define EFUSE_BROKEN 0x5A
#define EFUSE_REBLOW 0x55

/****************************************************
 * EFUSE SELF-BLOW status
 ****************************************************/
static int efuse_load_misc(unsigned char *misc_addr, unsigned int size)
{
    bdev_t *bdev = NULL;
    ssize_t len;

    LTRACEF("[%s]: size is %u\n", __func__, size);
    LTRACEF("[%s]: misc_addr is 0x%p\n", __func__, misc_addr);

    bdev = bio_open_by_label(EFUSE_PART_NAME);
    if (bdev == NULL) {
        LTRACEF("%s does not exist\n", EFUSE_PART_NAME);
        return -ENODEV;
    }

    len = bio_read(bdev, misc_addr, 0, (size_t)size);

    bio_close(bdev);
    if (len < (ssize_t)size)
        return -EIO;
    return 0;
}

#ifdef MTK_EFUSE_WRITER_SUPPORT
static unsigned int get_efuse_writer_status(void)
{
    unsigned char data[CHK_SIZE] = {0};
    unsigned char efuse = EFUSE_NONE;
    char *str = "";
    int ret, i, cnt;

    ret = efuse_load_misc(data, CHK_SIZE);
    if (ret < 0) {
        dprintf(CRITICAL, "%s: read partition failed\n", __func__);
        goto fail;
    }

    switch (data[0]) {
    case EFUSE_FINE:
        str = "success";
        efuse = data[0];
        break;
    case EFUSE_BROKEN:
        str = "broken";
        efuse = data[0];
        break;
    case EFUSE_REBLOW:
        str = "reblow";
        efuse = data[0];
        break;
    }

    if (efuse != EFUSE_NONE) {
        for (i = cnt = 1; i < CHK_SIZE; i++) {
            if (data[i] == efuse)
                cnt++;
        }

        if (cnt >= CHK_THRESHOLD)
            video_printf("efuse blow: %s\n", str);
    }

fail:
    return (unsigned int)efuse;
}
#endif

/**************************************************************
 * ATAG info
 **************************************************************/
static void platform_atag_devinfo_data(void *fdt)
{
    unsigned int ptr[ATAG_DEVINFO_DATA_SIZE+1];
    int offset, ret;
    int i = 0;

#ifdef MTK_EFUSE_WRITER_SUPPORT
    set_devinfo_data(EFUSE_SELF_BLOW_RESULT_IDX, get_efuse_writer_status());
#endif

    if (ATAG_DEVINFO_DATA_SIZE == 0) {
        dprintf(CRITICAL, "devindo not supported on ATAG!\n");
        return;
    }

    ptr[0] = ATAG_DEVINFO_DATA_SIZE;

    for (i = 0; i < ATAG_DEVINFO_DATA_SIZE; i++)
        ptr[1+i] = get_devinfo_with_index(i);

    dprintf(CRITICAL, "SSSS:0x%x\n", get_devinfo_with_index(0));
    dprintf(CRITICAL, "SSSS:0x%x\n", get_devinfo_with_index(1));
    dprintf(CRITICAL, "SSSS:0x%x\n", get_devinfo_with_index(2));
    dprintf(CRITICAL, "SSSS:0x%x\n", get_devinfo_with_index(3));
    dprintf(CRITICAL, "SSSS:0x%x\n", get_devinfo_with_index(4));
    dprintf(CRITICAL, "SSSS:0x%x\n", get_devinfo_with_index(20));
    dprintf(CRITICAL, "SSSS:0x%x\n", get_devinfo_with_index(21));
    dprintf(CRITICAL, "SSSS:0x%x\n", get_devinfo_with_index(30));

    offset = fdt_path_offset(fdt, "/chosen");
    if (offset < 0)
        panic("get /chosen offset failed(%d)", offset);

    ret = fdt_setprop(fdt, offset, "atag,devinfo", ptr, sizeof(ptr));
    if (ret)
        panic("devinfo dts write failed");

}
SET_FDT_INIT_HOOK(platform_atag_devinfo_data, platform_atag_devinfo_data);

static void platform_fdt_set_model(void *fdt)
{
    char dev_model_name[MOD_NAME_LEN];

    if (get_devinfo_model_name(dev_model_name, sizeof(dev_model_name)))
        return;

    /**********************************************************************
     * Use nodeoffset=0, because model name is at the begin of device tree.
     **********************************************************************/
    int len = 0, nodeoffset = 0;
    const char *prop_name = "model";
    const struct fdt_property *prop = fdt_get_property(fdt, nodeoffset, prop_name, &len);

    if (prop) {
        int namestroff, ret;

        ret = fdt_setprop_string(fdt, nodeoffset, prop_name, dev_model_name);
        if (ret)
            ASSERT(0);

        prop = fdt_get_property(fdt, nodeoffset, prop_name, &len);
        if (prop) {
            namestroff = fdt32_to_cpu(prop->nameoff);
            dprintf(CRITICAL, "%s=%s\n", fdt_string(fdt, namestroff), (char *)prop->data);
        }
    }
}
SET_FDT_INIT_HOOK(platform_fdt_set_model, platform_fdt_set_model);

