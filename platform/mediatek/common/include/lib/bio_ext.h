/*
 * Copyright (c) 2020 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#pragma once

#include <lib/bio.h>

#define MAX_LUN  2

enum bio_ioctl_num_ext {
    BIO_IOCTL_CUSTOM_START = BIO_IOCTL_NULL + 0x100, /* for custom to add new enum */
    BIO_IOCTL_SET_BOOT_REGION = BIO_IOCTL_CUSTOM_START,
    BIO_IOCTL_SET_WRITE_PROTECT,
    BIO_IOCTL_GET_BOOTDEV_TYPE,
    BIO_IOCTL_GET_ACTIVE_BOOT,
};
