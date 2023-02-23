/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#include <compiler.h>
#include <debug.h>
#include <kernel/vm.h>
#include <platform/mboot_expdb.h>
#include <reg.h>
#include <stdio.h>
#include <string.h>

#include "srclken_rc_aee.h"

#define SRCLKEN_RC_DEBUG        0
/* #define SRCLKEN_RC_BRINGUP */

static unsigned long rc_dump_regs[] = {
    RC_WDT_LATCH_0,
    RC_WDT_LATCH_1,
    RC_WDT_LATCH_2,
    RC_WDT_LATCH_3,
    RC_WDT_LATCH_4,
    RC_WDT_LATCH_5,
    RC_WDT_LATCH_6,
    RC_WDT_LATCH_7,
    RC_WDT_LATCH_8,
    RC_WDT_LATCH_9,
    RC_WDT_LATCH_10,
    RC_WDT_LATCH_11,
    RC_WDT_LATCH_12,
    RC_WDT_LATCH_13,
    RC_WDT_LATCH_14,
    RC_WDT_LATCH_15,
    RC_WDT_LATCH_16,
    RC_WDT_LATCH_17,
    RC_WDT_LATCH_18,
    RC_WDT_LATCH_19,
    RC_WDT_LATCH_20,
};

static char *srclken_rc_dump_data(char *buf, int buf_size)
{
    char *buff_end = buf + buf_size;
    unsigned long i;

    for (i = 0; i < countof(rc_dump_regs); i++) {
        buf += snprintf(buf, buff_end - buf,
        "RC regs(0x%lx) = 0x%08x\n",
        rc_dump_regs[i],
        readl(rc_dump_regs[i]));
    }

    return buf;
}

static void save_last_srclken_rc_data(CALLBACK dev_write)
{
    unsigned long len;
    void *buf, *output;

    buf = malloc(SRCLKEN_RC_DUMP_SIZE);
    if (buf == NULL) {
        dprintf(CRITICAL, "[SRCLKEN_RC] can not alloc buffer\n");
        return;
    }

    output = srclken_rc_dump_data(buf, SRCLKEN_RC_DUMP_SIZE);
    len = output - buf;

    if (!dev_write(vaddr_to_paddr(buf), len))
        dprintf(CRITICAL, "[SRCLKEN_RC] AEE EXPDB dump fail\n");

    free(buf);
}

#if !defined(SRCLKEN_RC_BRINGUP)
AEE_EXPDB_INIT_HOOK(SYS_LAST_SRCLKEN_RC_DATA,
    SRCLKEN_RC_DUMP_SIZE,
    save_last_srclken_rc_data);
#endif /* !defined(SRCLKEN_RC_BRINGUP) */
