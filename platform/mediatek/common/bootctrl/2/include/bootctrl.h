/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

/* THE HAL BOOTCTRL HEADER MUST BE IN SYNC WITH THE UBOOT BOOTCTRL HEADER */

#pragma once

#include <stdint.h>
#include <stdbool.h>

#define BOOTCTRL_SUFFIX_A       "_a"
#define BOOTCTRL_SUFFIX_B       "_b"

struct slot_metadata {
    // Slot priority with 15 meaning highest priority, 1 lowest
    // priority and 0 the slot is unbootable.
    uint8_t priority;
    // Number of times left attempting to boot this slot.
    uint8_t retry_count;
    // 1 if this slot has booted successfully, 0 otherwise.
    uint8_t successful_boot;
    // Reserved for further use.
    uint8_t up_type[1];
};

struct bootloader_control {
    // Bootloader Control AB magic number (see BOOT_CTRL_MAGIC).
    uint32_t magic;
    /* Version of on-disk struct - see AVB_AB_{MAJOR,MINOR}_VERSION. */
    uint8_t version_major;
    uint8_t version_minor;

    /* Padding to ensure |slots| field start eight bytes in. */
    uint8_t reserved1[2];

    // Per-slot information.  Up to 4 slots.
    struct slot_metadata slot_info[2];

    /* Reserved for future use. */
    uint8_t reserved2[12];

    // CRC32 of all 28 bytes preceding this field (little endian
    // format).
    uint32_t crc32_le;
};

/* bootctrl API */
const char *get_suffix(void);
bool is_ab_enable(void);
