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
#include <lib/bio.h>
#include <malloc.h>
#include <platform/mrdump.h>
#include <stddef.h>
#include <stdint.h>
#include <trace.h>

#include "aee.h"
#include "mrdump_custom.h"
#include "mrdump_private.h"

#define LOCAL_TRACE 0

static char *part_device_init(struct mrdump_dev *dev, const char *part_name)
{
    dev->bdev = bio_open_by_label(part_name);

    if (!dev->bdev)
        return NULL;

    voprintf_info("%s size: %jd Mb\n", part_name, (dev->bdev->total_size) / 0x100000UL);

    return (char *)part_name;
}

static bool part_device_read(struct mrdump_dev *dev, uint64_t offset, uint8_t *buf, int32_t len)
{
    if (dev == NULL) {
        LTRACEF_LEVEL(ALWAYS, "%s dev is NULL!\n", __func__);
        return false;
    } else {
        int32_t out_len;

        out_len = bio_read(dev->bdev, buf, offset, len);
        if (out_len < 0 || out_len != len) {
            LTRACEF_LEVEL(ALWAYS, "%s bio_read fail (out_len:%d,len:%d)\n",
                    __func__, out_len, len);
            return false;
        }
    }
    return true;
}

static bool part_device_write(struct mrdump_dev *dev, uint64_t offset, uint8_t *buf, int32_t len)
{
    if (dev == NULL) {
        LTRACEF_LEVEL(ALWAYS, "%s dev is NULL!\n", __func__);
        return false;
    } else {
        int32_t out_len;

        out_len = bio_write(dev->bdev, buf, offset, len);
        if (out_len < 0 || out_len != len) {
            LTRACEF_LEVEL(ALWAYS, "%s bio_write fail (out_len:%d,len:%d)\n",
                    __func__, out_len, len);
            return false;
        }
    }
    return true;
}

struct mrdump_dev *mrdump_dev_emmc_ext4(void)
{
    char *ext4part;
    struct mrdump_dev *dev = malloc(sizeof(struct mrdump_dev));

    if (!dev) {
        LTRACEF_LEVEL(ALWAYS, "%s: malloc() failed!\n", __func__);
        return NULL;
    }

    ext4part = part_device_init(dev, MRDUMP_OUTPUT_PARTITION);
    if (ext4part == NULL) {
        LTRACEF_LEVEL(ALWAYS, "No EXT4 partition found!\n");
        free(dev);
        return NULL;
    }
    dev->name = "emmc";
    dev->handle = ext4part;
    dev->read = part_device_read;
    dev->write = part_device_write;
    return dev;
}

struct mrdump_dev *mrdump_dev_partition(const char *partname)
{
    char *mpart;
    struct mrdump_dev *dev = malloc(sizeof(struct mrdump_dev));

    if (!dev) {
        LTRACEF_LEVEL(ALWAYS, "%s: malloc() failed!\n", __func__);
        return NULL;
    }

    mpart = part_device_init(dev, partname);
    if (mpart == NULL) {
        LTRACEF_LEVEL(ALWAYS, "No mrdump partition found!\n");
        free(dev);
        return NULL;
    }
    dev->name = "mpart";
    dev->handle = mpart;
    dev->read = part_device_read;
    dev->write = part_device_write;
    return dev;
}

