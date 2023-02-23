/*
 * Copyright (c) 2021 MediaTek Inc. All Rights Reserved.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#include <assert.h>
#include <err.h>
#include <libfdt.h>
#include <trace.h>
#include "bootconfig.h"

#define LOCAL_TRACE                0
#define CMDLINE_LEN                2048
#define BOOTCONFIG_PREFIX          "androidboot."
#define BOOTCONFIG_PREFIX_LEN      12
#define BOOTCONFIG_STR             "bootconfig"
#define BOOTCONFIG_STR_LEN         10
#define BOOTCONFIG_MAGIC           "#BOOTCONFIG\n"
#define BOOTCONFIG_MAGIC_LEN       12
#define BOOTCONFIG_ALIGN_SHIFT     2
#define BOOTCONFIG_ALIGN           (1 << BOOTCONFIG_ALIGN_SHIFT)
#define BOOTCONFIG_ALIGN_MASK      (BOOTCONFIG_ALIGN - 1)

static char *androidboot_cmdline_tail;
static char *androidboot_cmdline_end;
static char *non_androidboot_cmdline_tail;
static char *non_androidboot_cmdline_end;

static inline void validate_cmdline_boundary(const char *tail, const char *end)
{
    if (tail >= end)
        panic("[ERROR] Bootconfig: cmdline buffer is overflow!!!");
}

static int bootconfig_separate(const void *fdt_bootargs)
{
    char *cmdline;
    char *sub_cmdline;
    uint32_t sub_cmdline_len = 0;

    cmdline = (char *)malloc(CMDLINE_LEN);
    ASSERT(cmdline);

    memset(cmdline, 0, CMDLINE_LEN);
    strncpy(cmdline, (const char *)fdt_bootargs, CMDLINE_LEN-1);
    sub_cmdline = strtok(cmdline, " \t");

    while (sub_cmdline != NULL) {
        sub_cmdline_len = strlen(sub_cmdline);
        if (strncmp(sub_cmdline, BOOTCONFIG_PREFIX, BOOTCONFIG_PREFIX_LEN) == 0) {
            validate_cmdline_boundary((androidboot_cmdline_tail + sub_cmdline_len + 1),
                                        androidboot_cmdline_end);
            androidboot_cmdline_tail += snprintf(androidboot_cmdline_tail,
                                                androidboot_cmdline_end
                                                    - androidboot_cmdline_tail,
                                                "\n%s", sub_cmdline);
        } else {
            validate_cmdline_boundary((non_androidboot_cmdline_tail + sub_cmdline_len + 1),
                                        non_androidboot_cmdline_end);
            non_androidboot_cmdline_tail += snprintf(non_androidboot_cmdline_tail,
                                                    non_androidboot_cmdline_end
                                                        - non_androidboot_cmdline_tail,
                                                    " %s", sub_cmdline);
        }
        sub_cmdline = strtok(NULL, " \t");
    }

    free(cmdline);
    return NO_ERROR;
}

static uint32_t bootconfig_calc_checksum(void *data, uint32_t size)
{
    unsigned char *p = data;
    uint32_t ret = 0;

    while (size--)
        ret += *p++;

    return ret;
}

static uint32_t bootconfig_apply(void *ramdisk_end_addr, char *bootconfig_para)
{
    char *bootconfig_data, *bootconfig_ptr;
    uint32_t para_checksum, para_size, bootconfig_total_size;
    int pad;

    // Calculate parameter size and checksum
    para_size = strlen(bootconfig_para) + 1;
    para_checksum = bootconfig_calc_checksum(bootconfig_para, para_size);

    /* Backup the bootconfig data */
    bootconfig_data = calloc(para_size + BOOTCONFIG_ALIGN +
                sizeof(uint32_t) + sizeof(uint32_t) + BOOTCONFIG_MAGIC_LEN, 1);
    ASSERT(bootconfig_data);

    memcpy(bootconfig_data, bootconfig_para, para_size);

    /* To align up the total size to BOOTCONFIG_ALIGN, get padding size */
    bootconfig_total_size = para_size
                            + sizeof(uint32_t)
                            + sizeof(uint32_t)
                            + BOOTCONFIG_MAGIC_LEN;
    pad = ((bootconfig_total_size + BOOTCONFIG_ALIGN - 1) & (~BOOTCONFIG_ALIGN_MASK))
            - bootconfig_total_size;
    para_size += pad;

    // Add the bootconfig trailer to end of the parameters
    bootconfig_ptr = bootconfig_data + para_size;
    *(uint32_t *)bootconfig_ptr = para_size;
    bootconfig_ptr += sizeof(uint32_t);

    *(uint32_t *)bootconfig_ptr = para_checksum;
    bootconfig_ptr += sizeof(uint32_t);

    memcpy(bootconfig_ptr, BOOTCONFIG_MAGIC, BOOTCONFIG_MAGIC_LEN);
    bootconfig_ptr += BOOTCONFIG_MAGIC_LEN;
    bootconfig_total_size = bootconfig_ptr - bootconfig_data;
    memcpy((void *)ramdisk_end_addr, (void *)bootconfig_data, bootconfig_total_size);

    free(bootconfig_data);

    return bootconfig_total_size;
}

uint32_t bootconfig_finalized(void *fdt, void *ramdisk_end_addr)
{
    char *androidboot_cmdline_buf;
    char *non_androidboot_cmdline_buf;
    int offset;
    const void *fdt_bootargs = NULL;
    int fdt_bootargs_len;
    status_t err = NO_ERROR;
    uint32_t bootconfig_sz;

    androidboot_cmdline_buf = (char *)malloc(BOOTCONFIG_LEN);
    ASSERT(androidboot_cmdline_buf);

    memset(androidboot_cmdline_buf, 0, BOOTCONFIG_LEN);
    androidboot_cmdline_tail = androidboot_cmdline_buf;
    androidboot_cmdline_end = androidboot_cmdline_buf + BOOTCONFIG_LEN;

    non_androidboot_cmdline_buf = (char *)malloc(CMDLINE_LEN);
    ASSERT(non_androidboot_cmdline_buf);

    memset(non_androidboot_cmdline_buf, 0, CMDLINE_LEN);
    non_androidboot_cmdline_tail = non_androidboot_cmdline_buf;
    non_androidboot_cmdline_end = non_androidboot_cmdline_buf + CMDLINE_LEN;

    offset = fdt_path_offset(fdt, "/chosen");
    ASSERT(offset >= 0);

    fdt_bootargs = fdt_getprop(fdt, offset, "bootargs", &fdt_bootargs_len);
    if (!fdt_bootargs)
        panic("get all cmdline fail in fdt!!!\n");

    err = bootconfig_separate(fdt_bootargs);
    if (err != NO_ERROR)
        panic("separate cmdline fail!!!\n");

    /* Add "bootconfig" to kernel cmdline.
     * setup_boot_config() in kernel-5.10/init/main.c
     * would check whether “bootconfig” is in cmdline or not.
     * If no “bootconfig” in cmdline, bootconfig data from initrd cannot be used.
     * It will lead to boot up failed.
     */
    validate_cmdline_boundary((non_androidboot_cmdline_tail + BOOTCONFIG_STR_LEN + 1),
                                non_androidboot_cmdline_end);
    non_androidboot_cmdline_tail += snprintf(non_androidboot_cmdline_tail,
                                            non_androidboot_cmdline_end
                                                - non_androidboot_cmdline_tail,
                                            " %s", BOOTCONFIG_STR);

    LTRACEF("kernel cmdline len = %zd, str = \"%s\"\n",
            strlen(non_androidboot_cmdline_buf), non_androidboot_cmdline_buf);

    err = fdt_setprop(fdt, offset, "bootargs", non_androidboot_cmdline_buf,
                        strlen(non_androidboot_cmdline_buf) + 1);
    if (err != NO_ERROR)
        panic("set cmdline fail from non_androidboot_cmdline_buf!!!\n");

    LTRACEF("bootconfig len = %zd, str = \"%s\"\n",
            strlen(androidboot_cmdline_buf), androidboot_cmdline_buf);

    bootconfig_sz = bootconfig_apply(ramdisk_end_addr, androidboot_cmdline_buf);

    free(androidboot_cmdline_buf);
    androidboot_cmdline_buf = NULL;

    free(non_androidboot_cmdline_buf);
    non_androidboot_cmdline_buf = NULL;

    return bootconfig_sz;
}
