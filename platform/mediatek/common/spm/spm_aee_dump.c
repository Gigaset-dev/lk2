/*
 * Copyright (c) 2021 MediaTek Inc.
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
#include <kernel/vm.h>
#include <platform/mboot_expdb.h>
#include <platform/spm.h>
#include <platform/addressmap.h>
#include <reg.h>

#include "spm_common.h"

#define PCM_FIRMWARE_VERSION_SIZE 128

static char *spm_dump_data(char *buf, int buf_size)
{
    char *buff_end = buf + buf_size;
    unsigned long i;
    int ret;
    char spmfw_version[PCM_FIRMWARE_VERSION_SIZE];

    for (i = SPM_DUMP_START; i <= SPM_DUMP_END; i += 4) {
        buf += snprintf(buf, buff_end - buf,
            "SPM regs(0x%x) = 0x%08x\n", (unsigned int)i, readl(i));
    }

    ret = aee_get_spmfw_version(spmfw_version, PCM_FIRMWARE_VERSION_SIZE);
    if (ret < 0)
        return buf;

    buf += snprintf(buf, buff_end - buf,
            "SPM firmware version = %s\n", spmfw_version);

    return buf;
}

static void save_spm_data(CALLBACK dev_write)
{
    unsigned long len;
    void *buf, *output;

    buf = malloc(SPM_BUF_LENGTH);
    if (buf == NULL) {
        dprintf(CRITICAL, "[spm] can not alloc buffer\n");
        return;
    }

    output = spm_dump_data(buf, SPM_BUF_LENGTH);
    len = output - buf;

    if (!dev_write(vaddr_to_paddr(buf), len))
        dprintf(CRITICAL, "[spm] AEE EXPDB dump fail\n");

    free(buf);
}
AEE_EXPDB_INIT_HOOK(SYS_LAST_SPM_DATA, SPM_BUF_LENGTH, save_spm_data);

static void save_spm_sram_data(CALLBACK dev_write)
{
    if (!dev_write(vaddr_to_paddr((void *)SPM_SRAM_BASE), (unsigned long)SPM_SRAM_SIZE))
        dprintf(CRITICAL, "[SSPM] AEE EXPDB SPM SRAM DUMP fail\n");
    return;
}
AEE_EXPDB_INIT_HOOK(SYS_LAST_SPM_SRAM_DATA, SPM_SRAM_SIZE, save_spm_sram_data);
