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
#include <kernel/vm.h>
#include <platform/mboot_expdb.h>
#include <platform/vcoredvfs.h>
#include <reg.h>

static char *spm_dump_data(char *buf, int buf_size)
{
    char *buff_end = buf + buf_size;
    unsigned int i;

    buf += snprintf(buf, buff_end - buf,
            "DVFSRC regs(0x%x) = 0x%08x\n",
            (unsigned int)DVFSRC_LAST_L, readl(DVFSRC_LAST_L));

    for (i = 0; i < DVFSRC_DUMP_SIZE; i += 4) {
        buf += snprintf(buf, buff_end - buf,
                "DVFSRC regs(0x%x) = 0x%08x\n", (unsigned int)(DVFSRC_DUMP + i),
                readl(DVFSRC_DUMP + i));
    }

    return buf;
}

static void save_dvfsrc_data(CALLBACK dev_write)
{
    unsigned long len;
    void *buf, *output;

    buf = malloc(DVFSRC_BUF_LENGTH);
    if (buf == NULL) {
        dprintf(CRITICAL, "[dvfsrc] can not alloc buffer\n");
        return;
    }

    output = spm_dump_data(buf, DVFSRC_BUF_LENGTH);
    len = output - buf;

    if (!dev_write(vaddr_to_paddr(buf), len))
        dprintf(CRITICAL, "[dvfsrc] AEE EXPDB dump fail\n");

    free(buf);
}
AEE_EXPDB_INIT_HOOK(SYS_LAST_DVFSRC_DATA, DVFSRC_BUF_LENGTH, save_dvfsrc_data);

