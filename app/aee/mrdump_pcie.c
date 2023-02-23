/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#include <arch/arm/mmu.h>
#include <debug.h>
#include <err.h>
#include <fastboot.h>
#include <fastboot_oem_cmd.h>
#include <malloc.h>
#include <platform.h>
#include <platform/mboot_params.h>
#include <platform/mrdump_expdb.h>
#include <platform/wdt.h>
#include <platform/mtk_pcie.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <trace.h>
#include "aee.h"
#include "KEHeader.h"
#include "mrdump_private.h"

#define LOCAL_TRACE 0
/*
 * Note: for pcie cldma transmission
 *   Set EXSPACE to 1024 since the limitation of cldma
 */
#define EXSPACE      1024
#define MAX_RSP_SIZE 64
#define CON_TIMEOUT  5000

/* Flow control */
#define PCIE_RTS      "_RTS"
#define PCIE_CTS      "_CTS"
#define PCIE_FIN      "_FIN"
#define USBDONE      "DONE"

#ifndef mdelay
#define mdelay(x)       spin((x) * 1000)
#endif

/**
 * For MT6880, pcie_read/write will be supported in fastboot_dual_ipc,
 * However, for MT6890, they will be undefined.
 */
__WEAK int pcie_read(void *_buf, unsigned int len)
{
    return ERR_NOT_IMPLEMENTED;
}

__WEAK int pcie_write(void *_buf, unsigned int len)
{
    return ERR_NOT_IMPLEMENTED;
}

/*
 * Need to align 64 bytes to meet cache line length
 * 1. cmd[MAX_RSP_SIZE]
 * 2. data[EXSPACE]
 */
struct mrdump_pcie_handle {
    char cmd[MAX_RSP_SIZE];
    uint8_t data[EXSPACE];
    unsigned int zipsize;
    unsigned int idx;
};

/* flow control of pcie connection */
static bool pcie_data_transfer(struct mrdump_pcie_handle *handle, int length)
{
    int len;

    /* send RTS */
    memset(handle->cmd, 0, MAX_RSP_SIZE);
    snprintf(handle->cmd, MAX_RSP_SIZE, "%s", PCIE_RTS);
    len = pcie_write(handle->cmd, strlen(handle->cmd));
    if (len > 0) {

        /* receive Clear-to-Send */
        memset(handle->cmd, 0, MAX_RSP_SIZE);
        len = pcie_read(handle->cmd, MAX_RSP_SIZE);
        if ((len == (int)strlen(PCIE_CTS)) &&
            (!strncmp(handle->cmd, PCIE_CTS, strlen(PCIE_CTS)))) {
            /* send DATA */
            len = pcie_write(handle->data, length);
            if (len > 0) {
                /* get FIN */
                memset(handle->cmd, 0, sizeof(handle->cmd));
                len = pcie_read(handle->cmd, sizeof(handle->cmd));
                if ((len == (int)strlen(PCIE_FIN)) &&
                    (!strncmp(handle->cmd, PCIE_FIN, strlen(PCIE_FIN)))) {
                    return true;
                } else {
                    voprintf_error("%s: failed to get FIN.cmd<%p,%d><%x,%x,%x,%x>\n", __func__,
                        handle->cmd, len, handle->cmd[0], handle->cmd[1],
                        handle->cmd[2], handle->cmd[3]);
                }
            } else {
                voprintf_error("%s: send DATA error.\n", __func__);
            }
        } else {
            voprintf_error("%s: Not Clear-to-Send after RTS.cmd<%p,%d><%x,%x,%x,%x>\n", __func__,
                handle->cmd, len, handle->cmd[0], handle->cmd[1], handle->cmd[2], handle->cmd[3]);
        }
    } else {
        voprintf_error("%s: send RTS error.\n", __func__);
    }
    return false;
}

/* store data in pool (EXSPACE) and write when pool is full */
static int do_store_or_write(struct mrdump_pcie_handle *handle, uint8_t *buf, uint32_t length)
{
    int total;
    unsigned int leftspace, mylen, reval;

    /* count for leftspace */
    total = EXSPACE;
    leftspace = total - handle->idx;

    /* check length */
    if (length > leftspace) {
        mylen = leftspace;
        reval = length - leftspace;
    } else {
        mylen = length;
        reval = 0;
    }

    /* store */
    while (mylen > 0) {
        handle->data[handle->idx] = *buf;
        handle->idx++;
        buf++;
        mylen--;
    }

    /* write */
    if (handle->idx == total) {
        if (!pcie_data_transfer(handle, handle->idx)) {
            voprintf_error("%s: connection failed.(error idx: %d)\n",
            __func__, handle->idx);
            return -1;
        }
        handle->idx = 0;
    }

    return reval;
}

static int pcie_write_cb(void *opaque_handle, void *buf, int size)
{
    unsigned int    len, moves;
    int             ret = 0;
    uint8_t         *ptr;

    struct mrdump_pcie_handle *handle = opaque_handle;

    handle->zipsize += size;

    /* EOF, write the left Data in handle data buffer... */
    if ((buf == NULL) && (size == 0)) {

        /* MUST: a delay for the last transmission */
        mdelay(10);

        if (!pcie_data_transfer(handle, handle->idx)) {
            voprintf_error("%s: connection failed.(error idx: %d)\n",
            __func__, handle->idx);
            return -1;
        }

        /* send "MRDUMP END Packet: pattern: MRDUMP08_DONE" */
        memset((void *)handle->data, 0, sizeof(handle->data));
        size = snprintf((char *)handle->data, sizeof(handle->data), "MRDUMP08_DONE");
        if (size < 0) {
            voprintf_error(" USB Dump: Make MRDUMP END Packet failed.\n");
            return -1;
        }
        if (pcie_write(handle->data, strlen((char *)handle->data)) < 0) {
            voprintf_error(" USB Dump: Write MRDUMP ZLP failed.\n");
            return -1;
        }

        return 0;
    }

    /* buf should not be NULL if not EOF */
    if (buf == NULL)
        return -1;

    /* process of Store and write */
    len = size;
    ptr = (uint8_t *)buf;
    while (1) {
        ret = do_store_or_write(handle, ptr, len);
        if (ret < 0) {
            voprintf_error(" USB Dump: store and write failed.\n");
            return -1;
        } else if (ret == 0) {
            break;
        } else {
            moves = len - ret;
            ptr += moves;
            len = ret;
        }
    }

    return size;
}

int mrdump_pcie_output(const struct mrdump_control_block *mrdump_cb,
              const struct kzip_addlist *memlist,
              const struct kzip_addlist *memlist_cmm)
{
    int i;
    struct kzip_addlist expdb_file[2];
    char *expdb_filename;
    unsigned int expdb_offset, expdb_filesize;
    bool ok = true;
    struct kzip_file *zf;
    struct mrdump_pcie_handle *handle;

    voprintf_info("Output by PCIE\n");

    handle = memalign(64, sizeof(struct mrdump_pcie_handle));
    if (handle == NULL) {
        LTRACEF_LEVEL(ALWAYS, "No enough memory.");
        return -1;
    }
    memset(handle, 0, sizeof(struct mrdump_pcie_handle));

    mdelay(100);
    mtk_wdt_restart_timer();
    zf = kzip_open(handle, pcie_write_cb);
    if (zf != NULL) {
        /* add SYS_COREDUMP */
        if (!kzip_add_file(zf, memlist, "SYS_COREDUMP"))
            ok = false;

        if (ok && !kzip_add_file(zf, memlist_cmm, "kernel.cmm")) {
            ok = false;
            mrdump_memlist_free_cmm(memlist_cmm);
        }
        if (!ok) {
            LTRACEF_LEVEL(ALWAYS, "failed to generate cmm\n");
            kzip_close(zf);
            return -1;
        }

        /* add file on expdb */
        for (i = 0; i < IPANIC_NR_SECTIONS; i++) {
            if (kedump_get_data_info(i, &expdb_filename, &expdb_offset, &expdb_filesize) == 0) {
                expdb_file[0].address = expdb_offset;
                expdb_file[0].size = expdb_filesize;
                expdb_file[0].type = EXPDB_FILE;
                expdb_file[0].memmap = 0;
                expdb_file[0].pageflags = 0;
                expdb_file[0].struct_page_size = 0;
                expdb_file[1].address = 0;
                expdb_file[1].size = 0;
                expdb_file[1].type = MEM_NO_MAP;
                expdb_file[1].memmap = 0;
                expdb_file[1].pageflags = 0;
                expdb_file[1].struct_page_size = 0;
                if (!kzip_add_file(zf, expdb_file, expdb_filename))
                    ok = false;
            }
        }

        /* close zipfile */
        kzip_close(zf);
        pcie_write_cb(handle, NULL, 0); /* really write the last part */
    } else {
        ok = false;
    }
    free(handle);

    mtk_wdt_restart_timer();

    return ok ? 0 : -1;
}
