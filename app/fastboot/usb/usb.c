/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#include <dev/udc.h>
#include <err.h>
#include <fastboot.h>
#include <fastboot_entry.h>
#include <fastboot_usb.h>
#include <init_mtk.h>
#include <kernel/event.h>
#include <sys/types.h>
#include <trace.h>

/* would be replaced. ex: in cust_usb.h */
#ifndef USB_VENDORID
#define USB_VENDORID            (0x0E8D)
#define USB_PRODUCTID           (0x201C)
#define USB_VERSIONID           (0x0100)
#define USB_MANUFACTURER        "MediaTek"
#define USB_PRODUCT_NAME        "Android"
#endif

/* MAX_USBFS_BULK_SIZE: if use USB3 QMU GPD mode: cannot exceed 63 * 1024 */
#define MAX_USBFS_BULK_SIZE     (16 * 1024)
#define USB_CMD_TIMEOUT         5000

static event_t usb_online;
static event_t txn_done;
static struct udc_endpoint *in, *out;
static struct udc_request *req;
static int txn_status;

#define LOCAL_TRACE 2

static struct udc_endpoint *fastboot_endpoints[2];

static struct udc_device surf_udc_device = {
    .vendor_id    = USB_VENDORID,
    .product_id   = USB_PRODUCTID,
    .version_id   = USB_VERSIONID,
    .manufacturer = USB_MANUFACTURER,
    .product      = USB_PRODUCT_NAME,
};

static void req_complete(struct udc_request *req, unsigned int actual, int status)
{
    txn_status = status;
    req->length = actual;
    event_signal(&txn_done, 0);
}

/*
 * Read data from USB.
 * buf: the buffer for reading data. Star address must be cache line aligned!
 * len: the length for reading data. Must be cache line size aligned!
 */
static int usb_read_with_timeout(void *buf, unsigned int len, lk_time_t timeout)
{
    int r;
    unsigned int xfer;
    int count = 0;
    status_t ret = NO_ERROR;

    if (fastboot_get_state() == STATE_ERROR)
        goto oops;

    while (len > 0) {
        xfer = (len > MAX_USBFS_BULK_SIZE) ? MAX_USBFS_BULK_SIZE : len;
        req->buffer = buf;
        req->length = xfer;
        req->complete = req_complete;
        r = udc_request_queue(out, req);
        if (r < 0) {
            dprintf(INFO, "%s() queue failed\n", __func__);
            goto oops;
        }
        ret = event_wait_timeout(&txn_done, timeout);
        if (ret != NO_ERROR)
            goto oops;

        if (txn_status < 0) {
            dprintf(INFO, "%s() transaction failed\n", __func__);
            goto oops;
        }

        count += req->length;
        buf += req->length;
        len -= req->length;

        /* short transfer? */
        if (req->length != xfer)
            break;
    }

    return count;

oops:
    fastboot_set_state(STATE_ERROR);
    return -1;
}

int usb_read(void *buf, unsigned int len)
{
    return usb_read_with_timeout(buf, len, INFINITE_TIME);
}

/*
 * Write data to USB.
 * buf: the buffer for writing data. Star address must be cache line aligned!
 * len: the length for writing data. Must be cache line size aligned!
 */
static int usb_write_with_timeout(void *buf, unsigned int len, lk_time_t timeout)
{
    int r;
    status_t ret = NO_ERROR;

    if (fastboot_get_state() == STATE_ERROR)
        goto oops;

    req->buffer = buf;
    req->length = len;
    req->complete = req_complete;
    r = udc_request_queue(in, req);
    if (r < 0) {
        dprintf(INFO, "%s() queue failed\n", __func__);
        goto oops;
    }
    ret = event_wait_timeout(&txn_done, timeout);
    if (ret != NO_ERROR)
        goto oops;

    if (txn_status < 0) {
        dprintf(INFO, "%s() transaction failed\n", __func__);
        goto oops;
    }
    return req->length;

oops:
    fastboot_set_state(STATE_ERROR);
    return -1;
}

int usb_write(void *buf, unsigned int len)
{
    return usb_write_with_timeout(buf, len, INFINITE_TIME);
}

static void fastboot_notify(struct udc_gadget *gadget, unsigned int event)
{
    if (event == UDC_EVENT_ONLINE)
        event_signal(&usb_online, 0);
    else if (event == UDC_EVENT_OFFLINE)
        event_unsignal(&usb_online);
}

static struct udc_gadget fastboot_gadget = {
    .notify        = fastboot_notify,
    .ifc_class     = 0xff,
    .ifc_subclass  = 0x42,
    .ifc_protocol  = 0x03,
    .ifc_endpoints = 2,
    .ifc_string    = "fastboot",
    .ept           = fastboot_endpoints,
};

static int usb_init(void)
{
    LTRACEF("call udc_init... USB_VID=%x USB_PID=%x\n", surf_udc_device.vendor_id,
            surf_udc_device.product_id);
    surf_udc_device.serialno = get_serialno();
    udc_init(&surf_udc_device);

    event_init(&usb_online, 0, EVENT_FLAG_AUTOUNSIGNAL);
    event_init(&txn_done, 0, EVENT_FLAG_AUTOUNSIGNAL);

    in = udc_endpoint_alloc(UDC_BULK_IN, 512);
    if (!in)
        goto fail_alloc_in;
    out = udc_endpoint_alloc(UDC_BULK_OUT, 512);
    if (!out)
        goto fail_alloc_out;

    fastboot_endpoints[0] = in;
    fastboot_endpoints[1] = out;

    req = udc_request_alloc();
    if (!req)
        goto fail_alloc_req;

    if (udc_register_gadget(&fastboot_gadget))
        goto fail_udc_register;

    return 0;

fail_udc_register:
    udc_request_free(req);
fail_alloc_req:
    udc_endpoint_free(out);
fail_alloc_out:
    udc_endpoint_free(in);
fail_alloc_in:
    return -1;
}

static void usb_fini(void)
{
    udc_request_free(req);
    udc_endpoint_free(out);
    udc_endpoint_free(in);
}

static status_t usb_event_wait(void)
{
    status_t ret;

    ret = event_wait(&usb_online);
    return ret;
}

static struct fastboot_controller_interface interface = {
    .name               = "usb",
    .init               = usb_init,
    .fini               = usb_fini,
    .start              = udc_start,
    .stop               = udc_stop,
    .read               = usb_read,
    .write_with_timeout = usb_write_with_timeout,
    .event_wait         = usb_event_wait,
};

static void usb_register_fastboot_controller_interface(unsigned int level)
{
    register_fastboot_controller_interface(&interface);
}

MT_LK_INIT_HOOK_BL33(usb_register_fastboot_controller_interface,
                     &usb_register_fastboot_controller_interface,
                     LK_INIT_LEVEL_APPS - 1);

MT_LK_INIT_HOOK_AEE(usb_register_fastboot_controller_interface,
                    &usb_register_fastboot_controller_interface,
                    LK_INIT_LEVEL_APPS - 1);
