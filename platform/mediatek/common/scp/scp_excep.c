/*
 * copyright (c) 2021 mediatek inc.
 *
 * use of this source code is governed by a mit-style
 * license that can be found in the license file or at
 * https://opensource.org/licenses/mit
 */
#include <debug.h>
#include <err.h>
#include <kernel/vm.h>
#include <lib/zlib.h>
#include <platform/mboot_expdb.h>
#include <platform/reg.h>
#include "scp.h"
#include "scp_plat_excep.h"
#include <stdio.h>
#include <string.h>
#include <trace.h>

#define LOCAL_TRACE (0)
#define SCP_EE_SIZE 0xE0000 //896 KB
#define EXTRA_BUF   0x20000

void scp_dts_probe(void *fdt)
{
    if (fdt != NULL)
       scp_memorydump_size_probe(fdt);
}

static void save_scp_coredump(CALLBACK dev_write)
{
    int memory_dump_size;
    unsigned char *output = malloc(SCP_EE_SIZE + EXTRA_BUF); //extra buffer 128 KB

    if (!output) {
        LTRACEF("[SCP] memory allocat for output failed\n");
        return;
    }
    memory_dump_size = scp_crash_dump(output);
    LTRACEF("[SCP] memory_dump_size = 0x%x\n", memory_dump_size);

    if ((memory_dump_size > SCP_EE_SIZE) || (memory_dump_size <= Z_NEED_DICT)) {
        LTRACEF("[SCP] memory_dump_size ERR %d\n", memory_dump_size);
        memory_dump_size = 0;
    } else
        memory_dump_size = dev_write(vaddr_to_paddr(output), memory_dump_size);
    free(output);
    return;
}
AEE_EXPDB_INIT_HOOK_SUFFIX(SYS_SCP_DUMP, gz, SCP_EE_SIZE, save_scp_coredump);
