/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#include <debug.h>
#include <errno.h>
#include "lastpc.h"
#include <kernel/vm.h>
#include <platform/etb.h>
#include <platform/lastbus.h>
#include <platform/mboot_expdb.h>
#include <platform/plat_debug.h>
#include <platform/tracker.h>
#include "return_stack.h"
#include <trace.h>

#define LOCAL_TRACE 0

int __WEAK plat_dfd_mcu_get(void **data, int *len)
{
    dprintf(CRITICAL, "%s is not implemented\n", __func__);
    return 0;
}

void __WEAK plat_dfd_mcu_put(void **data)
{
    dprintf(CRITICAL, "%s is not implemented\n", __func__);
}

static void save_cpu_bus_data(CALLBACK dev_write)
{
    char *buf = NULL;
    int ret;
    int len = 0;
    unsigned int datasize = 0;
    /* Save latch buffer */

    ret = lastpc_get((void **)&buf, &len);
    if (ret && (buf != NULL)) {
        LTRACEF("In lastpc dump\n");
        if (len > 0)
            datasize = dev_write(vaddr_to_paddr(buf), len);
        lastpc_put((void **)&buf);
    }
    dprintf(CRITICAL, "lastpc done\n");

    ret = plat_dfd_mcu_get((void **)&buf, &len);
    if (ret && (buf != NULL)) {
        LTRACEF("In plat_dfd_mcu dump\n");
        if (len > 0)
            datasize = dev_write(vaddr_to_paddr(buf), len);
        plat_dfd_mcu_put((void **)&buf);
    }
    dprintf(CRITICAL, "plat_dfd_mcu done\n");

    ret = return_stack_get((void **)&buf, &len);
    if (ret && (buf != NULL)) {
        LTRACEF("In return_stack dump\n");
        if (len > 0)
            datasize = dev_write(vaddr_to_paddr(buf), len);
        return_stack_put((void **)&buf);
    }
    dprintf(CRITICAL, "return_stack done\n");

    ret = tracker_get((void **)&buf, &len, SYS_TRACK_ENTRY);
    if (ret && (buf != NULL)) {
        LTRACEF("In tracker dump\n");
        if (len > 0)
            datasize = dev_write(vaddr_to_paddr(buf), len);
        tracker_put((void **)&buf);
    }
    dprintf(CRITICAL, "tracker done\n");

    ret = lastbus_get((void **)&buf, &len);
    if (ret && (buf != NULL)) {
        LTRACEF("In lastbus dump\n");
        if (len > 0)
            datasize = dev_write(vaddr_to_paddr(buf), len);
        lastbus_put((void **)&buf);
    }
    dprintf(CRITICAL, "lastbus done\n");

    ret = etb_get((void **)&buf, &len);
    if (ret && (buf != NULL)) {
        LTRACEF("In etb dump\n");
        if (len > 0)
            datasize = dev_write(vaddr_to_paddr(buf), len);
        etb_put((void **)&buf);
    }
    dprintf(CRITICAL, "etb done\n");

    return;
}

AEE_EXPDB_INIT_HOOK(SYS_LAST_CPU_BUS, LAST_DUMP_SIZE, save_cpu_bus_data);
