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

#pragma once

#include <dev/udc.h>
#include <platform/reg.h>
#include <platform/usb.h>

struct udc_endpoint;
struct mu3d_req;

#include "mtu3_hw_regs.h"
#include "mtu3_qmu.h"

/* if want to use PIO mode, comment out the following macro */
#define SUPPORT_QMU

/* two bulk and ep0 */
#define MT_EP_NUM 3
#define MAX_EP_NUM 8

#define DESC_TYPE_U2 0
#define DESC_TYPE_U3 1

/* U3 IP: EP0, TX, RX has separate SRAMs */
#define U3IP_TX_FIFO_START_ADDR   0
#define U3IP_RX_FIFO_START_ADDR   0

/* U2 IP: EP0, TX, RX share one SRAM. 0-63 bytes are reserved for EP0 */
#define U2IP_TX_FIFO_START_ADDR   (64)
#define U2IP_RX_FIFO_START_ADDR   (64 + 512 * (MAX_EP_NUM))

#define U3D_U3IP_DFT_SPEED SSUSB_SPEED_SUPER
#define U3D_U2IP_DFT_SPEED SSUSB_SPEED_HIGH

/*
 * fastboot only supports BULK, alloc 1024B for each ep and offset are
 * also fixed, such as, offset-1024 for ep1, offset-2048 for ep2;
 * so MT_EP_NUM should not greater than 9(ep0 + 4 bulk in + 4 bulk out)
 */
#define U3D_FIFO_SIZE_UNIT 1024

#define EP0_MAX_PACKET_SIZE 64
#define EP0_MAX_PACKET_SIZE_U3 512

#define USB_FIFOSZ_SIZE_8       (0x03)
#define USB_FIFOSZ_SIZE_16      (0x04)
#define USB_FIFOSZ_SIZE_32      (0x05)
#define USB_FIFOSZ_SIZE_64      (0x06)
#define USB_FIFOSZ_SIZE_128     (0x07)
#define USB_FIFOSZ_SIZE_256     (0x08)
#define USB_FIFOSZ_SIZE_512     (0x09)
#define USB_FIFOSZ_SIZE_1024    (0x0A)

#define ARRAY_SIZE(arr) (countof(arr))

enum EP0_STATE {
    EP0_IDLE = 0,
    EP0_RX,
    EP0_TX,
    EP0_TX_END,
};

/* MTK USB3 ssusb defines */
enum USB_SPEED {
    SSUSB_SPEED_UNKNOWN = 0,
    SSUSB_SPEED_FULL = 1,
    SSUSB_SPEED_HIGH = 2,
    SSUSB_SPEED_SUPER = 3,
    SSUSB_SPEED_SUPER_PLUS = 4,
};

#define USB_EP_XFER_CTRL    0
#define USB_EP_XFER_ISO     1
#define USB_EP_XFER_BULK    2
#define USB_EP_XFER_INT     3

/* USB transfer directions */
#define USB_DIR_IN  0x80
#define USB_DIR_OUT 0x00

struct udc_descriptor {
    struct udc_descriptor *next;
    unsigned short tag; /* ((TYPE << 8) | NUM) */
    unsigned short len; /* total length */
    unsigned char data[0];
};

struct mu3d_req {
    struct udc_request req; /* should be first */
    struct udc_endpoint *ept;
    struct qmu_gpd *gpd;
    unsigned int actual;    /* data already sent/rcv */
};

/* endpoint data */
struct udc_endpoint {
    struct udc_request *req;
    struct mtu3_gpd_ring gpd_ring;
    char name[12];
    unsigned int maxpkt;
    unsigned char num;
    unsigned char in;
    unsigned char type; /* Transfer type */
    unsigned char slot; /* Transfer type */
    unsigned int bit;   /* EPT_TX/EPT_RX */
};

struct mu3d {
    struct udc_device *udev;
    struct udc_gadget *gadget;
    enum EP0_STATE ep0_state;
    enum USB_SPEED speed;
    u32 tx_fifo_addr;
    u32 rx_fifo_addr;

    struct udc_endpoint eps[MT_EP_NUM]; /* index 0 is fixed as EP0 */
    struct udc_endpoint *ep0;
    struct mu3d_req ep0_mreq;
    u32 ept_alloc_table;

    struct udc_descriptor *desc_list;
    unsigned int next_string_id;
    struct udc_descriptor *desc_list_u3;
    unsigned int next_string_id_u3;

    u8 address;
    unsigned usb_online:1;
    unsigned is_u3_ip:1;
};

int wait_for_value(paddr_t addr, u32 msk, u32 value, int us_intvl, int count);
struct udc_endpoint *mtu3_find_ep(int ep_num, u8 dir);
void handle_ept_complete(struct udc_endpoint *ept, int status);
static int pio_read_fifo(int ep_num, u8 *dst, u16 len) __attribute__((noinline));

static inline struct mu3d_req *to_mu3d_req(struct udc_request *req)
{
    return (struct mu3d_req *)req;
}
