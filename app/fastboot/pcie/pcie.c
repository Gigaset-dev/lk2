/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#include <cldma.h>
#include <debug.h>
#include <err.h>
#include <fastboot.h>
#include <fastboot_pcie.h>
#include <init_mtk.h>
#include <sys/types.h>
#include <trace.h>

#define CLDMA_TX_RX_TIMEOUT 5000 /* 5 sec. */
#define MAX_CLDMA_BULK_SIZE (1024)

static status_t pcie_init(void)
{
    cldma_init();

    return NO_ERROR;
}

static void pcie_fini(void)
{
    return;
}

static status_t pcie_start(void)
{
    return NO_ERROR;
}

static status_t pcie_stop(void)
{
    return NO_ERROR;
}

/*
 * Read data from PCIE.
 * _buf: the buffer for reading data.
 * len: the length for reading data, should be less than MAX_CLDMA_BULK_SIZE.
 * return value: received data length from cldma, < 0 means error happened.
 */
int pcie_read_with_timeout(void *_buf, unsigned int len, lk_time_t timeout)
{
    int ret;
    unsigned char *buf = _buf;

    if (fastboot_get_state() == STATE_ERROR)
        goto oops;

    if (len > MAX_CLDMA_BULK_SIZE) {
        dprintf(CRITICAL, "%s read length reach cldma limit\n", __func__);
        return -2;
    }

    ret = cldma_get_data(buf, len, timeout);
    if (ret < 0) {
        dprintf(CRITICAL, "%s() queue failed\n", __func__);
        goto oops;
    }

    return ret;

oops:
    fastboot_set_state(STATE_ERROR);
    return -1;
}

int pcie_read(void *_buf, unsigned int len)
{
    int ret;

    ret = pcie_read_with_timeout(_buf, len, INFINITE_TIME);
    return ret;
}

/*
 * Write data to PCIE.
 * _buf: the buffer for writing data.
 * len: the length for writing data, should be less than MAX_CLDMA_BULK_SIZE.
 * return value: transferred data length from cldma, < 0 means error happened.
 */
int pcie_write_with_timeout(void *_buf, unsigned int len, lk_time_t timeout)
{
    int r;
    unsigned char *buf = _buf;

    if (fastboot_get_state() == STATE_ERROR)
        goto oops;

    if (len > MAX_CLDMA_BULK_SIZE) {
        dprintf(CRITICAL, "%s write length reach cldma limit\n", __func__);
        return -2;
    }

    r = cldma_put_data(buf, len, timeout);
    if (r < 0) {
        dprintf(CRITICAL, "%s() queue failed\n", __func__);
        goto oops;
    }

    return r;

oops:
    fastboot_set_state(STATE_ERROR);
    return -1;
}

int pcie_write(void *_buf, unsigned int len)
{
    int ret;

    ret = pcie_write_with_timeout(_buf, len, CLDMA_TX_RX_TIMEOUT);
    return ret;
}

static struct fastboot_controller_interface interface = {
    .name               = "pcie",
    .init               = pcie_init,
    .fini               = pcie_fini,
    .start              = pcie_start,
    .stop               = pcie_stop,
    .read               = pcie_read,
    .write_with_timeout = pcie_write_with_timeout,
};

static void pcie_register_fastboot_controller_interface(unsigned int level)
{
    register_fastboot_controller_interface(&interface);
}

MT_LK_INIT_HOOK_AEE(pcie_register_fastboot_controller_interface,
                    &pcie_register_fastboot_controller_interface,
                    LK_INIT_LEVEL_APPS - 1);
