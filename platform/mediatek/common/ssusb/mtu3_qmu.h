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

#include "mtu3.h"

struct qmu_gpd;

#define MAX_GPD_NUM         4
#define QMU_GPD_SIZE        (sizeof(struct qmu_gpd))
#define QMU_GPD_RING_SIZE   ((MAX_GPD_NUM) * (QMU_GPD_SIZE))

#define GPD_BUF_SIZE        65532

#define MU3D_EP_TXCR0(epnum)    (U3D_TX1CSR0 + (((epnum) - 1) * 0x10))
#define MU3D_EP_TXCR1(epnum)    (U3D_TX1CSR1 + (((epnum) - 1) * 0x10))
#define MU3D_EP_TXCR2(epnum)    (U3D_TX1CSR2 + (((epnum) - 1) * 0x10))

#define MU3D_EP_RXCR0(epnum)    (U3D_RX1CSR0 + (((epnum) - 1) * 0x10))
#define MU3D_EP_RXCR1(epnum)    (U3D_RX1CSR1 + (((epnum) - 1) * 0x10))
#define MU3D_EP_RXCR2(epnum)    (U3D_RX1CSR2 + (((epnum) - 1) * 0x10))
#define MU3D_EP_RXCR3(epnum)    (U3D_RX1CSR3 + (((epnum) - 1) * 0x10))

#define USB_QMU_RQCSR(epnum)    (U3D_RXQCSR1 + (((epnum) - 1) * 0x10))
#define USB_QMU_RQSAR(epnum)    (U3D_RXQSAR1 + (((epnum) - 1) * 0x10))
#define USB_QMU_RQCPR(epnum)    (U3D_RXQCPR1 + (((epnum) - 1) * 0x10))

#define USB_QMU_TQCSR(epnum)    (U3D_TXQCSR1 + (((epnum) - 1) * 0x10))
#define USB_QMU_TQSAR(epnum)    (U3D_TXQSAR1 + (((epnum) - 1) * 0x10))
#define USB_QMU_TQCPR(epnum)    (U3D_TXQCPR1 + (((epnum) - 1) * 0x10))


/* U3D_QCR0 */
#define QMU_RX_CS_EN(x)     (BIT(16) << (x))
#define QMU_TX_CS_EN(x)     (BIT(0) << (x))
#define QMU_CS16B_EN        BIT(0)

/* U3D_QCR1 */
#define QMU_TX_ZLP(x)       (BIT(0) << (x))

/* U3D_QCR3 */
#define QMU_RX_COZ(x)       (BIT(16) << (x))
#define QMU_RX_ZLP(x)       (BIT(0) << (x))

/* U3D_TXQCSR1 */
/* U3D_RXQCSR1 */
#define QMU_Q_ACTIVE    BIT(15)
#define QMU_Q_STOP      BIT(2)
#define QMU_Q_RESUME    BIT(1)
#define QMU_Q_START     BIT(0)

/* U3D_QISAR0, U3D_QIER0, U3D_QIESR0, U3D_QIECR0 */
#define QMU_RX_DONE_INT(x)  (BIT(16) << (x))
#define QMU_TX_DONE_INT(x)  (BIT(0) << (x))


struct qmu_gpd {
    u8 flag;
    u8 chksum;
    u16 data_buf_len;
    u32 next_gpd;
    u32 buffer;
    u16 buf_len;
    u8 ext_len;
    u8 ext_flag;
} __PACKED;

struct mtu3_gpd_ring {
    paddr_t dma;
    struct qmu_gpd *start;
    struct qmu_gpd *end;
    struct qmu_gpd *enqueue;
    struct qmu_gpd *dequeue;
};

int mtu3_qmu_start(struct udc_endpoint *mep);
void mtu3_qmu_resume(struct udc_endpoint *mep);
void mtu3_qmu_flush(struct udc_endpoint *mep);

void mtu3_insert_gpd(struct udc_endpoint *mep, struct mu3d_req *mreq);
int mtu3_prepare_transfer(struct udc_endpoint *mep);

int mtu3_gpd_ring_alloc(struct udc_endpoint *mep);
void mtu3_gpd_ring_free(struct udc_endpoint *mep);

enum handler_return mtu3_qmu_isr(void);
int mtu3_qmu_init(void);
