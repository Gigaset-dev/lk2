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

/*
 * Queue Management Unit (QMU) is designed to unload SW effort
 * to serve DMA interrupts.
 * By preparing General Purpose Descriptor (GPD) and Buffer Descriptor (BD),
 * SW links data buffers and triggers QMU to send / receive data to
 * host / from device at a time.
 * And now only GPD is supported.
 *
 * For more detailed information, please refer to QMU Programming Guide
 */

#include <arch/mmu.h>
#include <arch/ops.h>
#include <debug.h>
#include <errno.h>
#include <kernel/vm.h>
#include <platform/reg.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <trace.h>

#include "mtu3.h"
#include "mtu3_hw_regs.h"
#include "mtu3_qmu.h"

#define LOCAL_TRACE 0

#define QMU_CHECKSUM_LEN    16

#define GPD_FLAGS_HWO   BIT(0)
#define GPD_FLAGS_BDP   BIT(1)
#define GPD_FLAGS_BPS   BIT(2)
#define GPD_FLAGS_IOC   BIT(7)

#define GPD_EXT_FLAG_ZLP    BIT(5)

#define DBG_C(x...) dprintf(CRITICAL, "[USB][QMU] " x)

#ifdef SUPPORT_QMU

static struct qmu_gpd *gpd_dma_to_virt(struct mtu3_gpd_ring *ring,
                                       paddr_t dma_addr)
{
    paddr_t dma_base = ring->dma;
    struct qmu_gpd *gpd_head = ring->start;
    u32 offset = (dma_addr - dma_base) / sizeof(*gpd_head);

    if (offset >= MAX_GPD_NUM)
        return NULL;

    return gpd_head + offset;
}

static paddr_t gpd_virt_to_dma(struct mtu3_gpd_ring *ring,
                               struct qmu_gpd *gpd)
{
    paddr_t dma_base = ring->dma;
    struct qmu_gpd *gpd_head = ring->start;
    u32 offset;

    offset = gpd - gpd_head;
    if (offset >= MAX_GPD_NUM)
        return 0;

    return dma_base + (offset * sizeof(*gpd));
}

static void gpd_ring_init(struct mtu3_gpd_ring *ring, struct qmu_gpd *gpd)
{
    ring->start = gpd;
    ring->enqueue = gpd;
    ring->dequeue = gpd;
    ring->end = gpd + MAX_GPD_NUM - 1;
}

static void reset_gpd_list(struct udc_endpoint *mep)
{
    struct mtu3_gpd_ring *ring = &mep->gpd_ring;
    struct qmu_gpd *gpd = ring->start;

    if (gpd) {
        gpd->flag &= ~GPD_FLAGS_HWO;
        gpd_ring_init(ring, gpd);
    }
}

int mtu3_gpd_ring_alloc(struct udc_endpoint *mep)
{
    int err;
    void *ptr = NULL;
    struct qmu_gpd *gpd;
    struct mtu3_gpd_ring *ring = &mep->gpd_ring;
    u32 size;

    /* software own all gpds as default */
    size = sizeof(struct qmu_gpd) * MAX_GPD_NUM;
    err = vmm_alloc_contiguous(vmm_get_kernel_aspace(), "USB uncached memory",
                               size, &ptr, PAGE_SIZE_SHIFT, 0,
                               ARCH_MMU_FLAG_UNCACHED);
    if (err)
        return -ENOMEM;

    gpd = (struct qmu_gpd *)ptr;
    memset(gpd, 0, size);
    ring->dma = vaddr_to_paddr(gpd);
    gpd_ring_init(ring, gpd);
    return 0;
}

void mtu3_gpd_ring_free(struct udc_endpoint *mep)
{
    struct mtu3_gpd_ring *ring = &mep->gpd_ring;

    vmm_free_region(vmm_get_kernel_aspace(), (vaddr_t)ring->start);
}

/*
 * calculate check sum of a gpd or bd
 * add "noinline" and "mb" to prevent wrong calculation
 */
static u8 qmu_calc_checksum(u8 *data)
{
    u8 chksum = 0;
    int i;

    data[1] = 0x0;  /* set checksum to 0 */

    mb();   /* ensure the gpd/bd is really up-to-date */
    for (i = 0; i < QMU_CHECKSUM_LEN; i++)
        chksum += data[i];

    /* Default: HWO=1, @flag[bit0] */
    chksum += 1;

    return 0xFF - chksum;
}

void mtu3_qmu_resume(struct udc_endpoint *mep)
{
    int epnum = mep->num;
    paddr_t qcsr;

    qcsr = mep->in ? USB_QMU_TQCSR(epnum) : USB_QMU_RQCSR(epnum);

    writel(QMU_Q_RESUME, qcsr);
    if (!(readl(qcsr) & QMU_Q_ACTIVE))
        writel(QMU_Q_RESUME, qcsr);
}

static struct qmu_gpd *advance_enq_gpd(struct mtu3_gpd_ring *ring)
{
    if (ring->enqueue < ring->end)
        ring->enqueue++;
    else
        ring->enqueue = ring->start;

    return ring->enqueue;
}

static struct qmu_gpd *advance_deq_gpd(struct mtu3_gpd_ring *ring)
{
    if (ring->dequeue < ring->end)
        ring->dequeue++;
    else
        ring->dequeue = ring->start;

    return ring->dequeue;
}

/* check if a ring is emtpy */
static int gpd_ring_empty(struct mtu3_gpd_ring *ring)
{
    struct qmu_gpd *enq = ring->enqueue;
    struct qmu_gpd *next;

    if (ring->enqueue < ring->end)
        next = enq + 1;
    else
        next = ring->start;

    /* one gpd is reserved to simplify gpd preparation */
    return next == ring->dequeue;
}

int mtu3_prepare_transfer(struct udc_endpoint *mep)
{
    return gpd_ring_empty(&mep->gpd_ring);
}

static int mtu3_prepare_tx_gpd(struct udc_endpoint *mep, struct mu3d_req *mreq)
{
    struct qmu_gpd *enq;
    struct mtu3_gpd_ring *ring = &mep->gpd_ring;
    struct qmu_gpd *gpd = ring->enqueue;
    struct udc_request *req = &mreq->req;

    /* set all fields to zero as default value */
    memset(gpd, 0, sizeof(*gpd));

    gpd->buffer = (u32)vaddr_to_paddr(req->buffer);
    gpd->buf_len = (req->length);
    gpd->flag |= GPD_FLAGS_IOC;

    /* get the next GPD */
    enq = advance_enq_gpd(ring);
    LTRACEF("TX %s queue gpd=%p, enq=%p\n", mep->name, gpd, enq);

    enq->flag &= ~GPD_FLAGS_HWO;
    gpd->next_gpd = (u32)gpd_virt_to_dma(ring, enq);

    if (mep->type != USB_EP_XFER_ISO)
        gpd->ext_flag |= GPD_EXT_FLAG_ZLP;

    gpd->chksum = qmu_calc_checksum((u8 *)gpd);
    gpd->flag |= GPD_FLAGS_HWO;

    arch_clean_invalidate_cache_range((addr_t) gpd, sizeof(struct qmu_gpd));
    mreq->gpd = gpd;

    return 0;
}

static int mtu3_prepare_rx_gpd(struct udc_endpoint *mep, struct mu3d_req *mreq)
{
    struct qmu_gpd *enq;
    struct mtu3_gpd_ring *ring = &mep->gpd_ring;
    struct qmu_gpd *gpd = ring->enqueue;
    struct udc_request *req = &mreq->req;

    /* set all fields to zero as default value */
    memset(gpd, 0, sizeof(*gpd));

    gpd->buffer = (u32)vaddr_to_paddr(req->buffer);
    gpd->data_buf_len = req->length;
    gpd->flag |= GPD_FLAGS_IOC;

    /* get the next GPD */
    enq = advance_enq_gpd(ring);
    LTRACEF("RX %s queue gpd=%p, enq=%p\n", mep->name, gpd, enq);

    enq->flag &= ~GPD_FLAGS_HWO;
    gpd->next_gpd = (u32)gpd_virt_to_dma(ring, enq);
    gpd->chksum = qmu_calc_checksum((u8 *)gpd);
    gpd->flag |= GPD_FLAGS_HWO;

    arch_clean_invalidate_cache_range((addr_t) gpd, sizeof(struct qmu_gpd));
    mreq->gpd = gpd;

    return 0;
}

void mtu3_insert_gpd(struct udc_endpoint *mep, struct mu3d_req *mreq)
{
    if (mep->in)
        mtu3_prepare_tx_gpd(mep, mreq);
    else
        mtu3_prepare_rx_gpd(mep, mreq);
}

int mtu3_qmu_start(struct udc_endpoint *mep)
{
    struct mtu3_gpd_ring *ring = &mep->gpd_ring;
    u8 epnum = mep->num;

    if (mep->in) {
        /* set QMU start address */
        arch_clean_invalidate_cache_range((addr_t) paddr_to_kvaddr(ring->dma),
            sizeof(struct qmu_gpd) * MAX_GPD_NUM);
        writel(ring->dma, USB_QMU_TQSAR(mep->num));
        setbits32(MU3D_EP_TXCR0(mep->num), TX_DMAREQEN);
        setbits32(U3D_QCR0, QMU_TX_CS_EN(epnum));
        /* send zero length packet according to ZLP flag in GPD */
        setbits32(U3D_QCR1, QMU_TX_ZLP(epnum));
        writel(QMU_TX_LEN_ERR(epnum) | QMU_TX_CS_ERR(epnum), U3D_TQERRIESR0);

        if (readl(USB_QMU_TQCSR(epnum)) & QMU_Q_ACTIVE) {
            DBG_C("%s Active Now!\n", mep->name);
            return 0;
        }
        writel(QMU_Q_START, USB_QMU_TQCSR(epnum));

    } else {
        arch_clean_invalidate_cache_range((addr_t) paddr_to_kvaddr(ring->dma),
            sizeof(struct qmu_gpd) * MAX_GPD_NUM);
        writel(ring->dma, USB_QMU_RQSAR(mep->num));
        setbits32(MU3D_EP_RXCR0(mep->num), RX_DMAREQEN);
        setbits32(U3D_QCR0, QMU_RX_CS_EN(epnum));
        /* don't expect ZLP */
        clrbits32(U3D_QCR3, QMU_RX_ZLP(epnum));
        /* move to next GPD when receive ZLP */
        setbits32(U3D_QCR3, QMU_RX_COZ(epnum));
        writel(QMU_RX_LEN_ERR(epnum) | QMU_RX_CS_ERR(epnum), U3D_RQERRIESR0);
        writel(QMU_RX_ZLP_ERR(epnum), U3D_RQERRIESR1);

        if (readl(USB_QMU_RQCSR(epnum)) & QMU_Q_ACTIVE) {
            DBG_C("%s Active Now!\n", mep->name);
            return 0;
        }
        writel(QMU_Q_START, USB_QMU_RQCSR(epnum));
    }
    LTRACEF("%s's qmu start now!\n", mep->name);

    return 0;
}

/* may called in atomic context */
static void mtu3_qmu_stop(struct udc_endpoint *mep)
{
    int epnum = mep->num;
    paddr_t qcsr;
    int ret;

    qcsr = mep->in ? USB_QMU_TQCSR(epnum) : USB_QMU_RQCSR(epnum);

    if (!(readl(qcsr) & QMU_Q_ACTIVE)) {
        DBG_C("%s's qmu is inactive now!\n", mep->name);
        return;
    }
    writel(QMU_Q_STOP, qcsr);

    ret = wait_for_value(qcsr, QMU_Q_ACTIVE, 0, 10, 100);
    if (ret) {
        DBG_C("stop %s's qmu failed\n", mep->name);
        return;
    }

    LTRACEF("%s's qmu stop now!\n", mep->name);
}

void mtu3_qmu_flush(struct udc_endpoint *mep)
{
    LTRACEF("%s flush QMU %s\n", __func__, mep->name);

    /*Stop QMU */
    mtu3_qmu_stop(mep);
    reset_gpd_list(mep);
}

static void qmu_done_tx(u8 epnum)
{
    struct udc_endpoint *mep = NULL;
    struct mtu3_gpd_ring *ring = NULL;
    struct qmu_gpd *gpd = NULL;
    struct qmu_gpd *gpd_current = NULL;
    struct udc_request *request = NULL;
    struct mu3d_req *mreq = NULL;
    paddr_t gpd_dma;

    mep = mtu3_find_ep(epnum, USB_DIR_IN);
    if (mep == NULL) {
        DBG_C("no udc endpoint is found\n");
        return;
    }

    ring = &mep->gpd_ring;
    gpd = ring->dequeue;

    gpd_dma = readl(USB_QMU_TQCPR(epnum));
    /*transfer phy address got from QMU register to virtual address */
    gpd_current = gpd_dma_to_virt(ring, gpd_dma);

    LTRACEF("%s %s, last=%p, current=%p, enq=%p\n",
          __func__, mep->name, gpd, gpd_current, ring->enqueue);

    arch_clean_invalidate_cache_range((addr_t) gpd, sizeof(struct qmu_gpd));
    while (gpd != gpd_current && !(gpd->flag & GPD_FLAGS_HWO)) {
        arch_clean_invalidate_cache_range((addr_t) gpd, sizeof(struct qmu_gpd));

        request = mep->req;
        mreq = to_mu3d_req(request);
        if (mreq == NULL || mreq->gpd != gpd) {
            DBG_C("no correct TX req is found\n");
            break;
        }

        mreq->actual = gpd->buf_len;
        handle_ept_complete(mep, 0);
        gpd = advance_deq_gpd(ring);
    }

    LTRACEF("%s EP%dIN, deq=%p, enq=%p, complete\n",
          __func__, epnum, ring->dequeue, ring->enqueue);
}

static void qmu_done_rx(u8 epnum)
{
    struct udc_endpoint *mep = NULL;
    struct mtu3_gpd_ring *ring = NULL;
    struct qmu_gpd *gpd = NULL;
    struct qmu_gpd *gpd_current = NULL;
    struct udc_request *request = NULL;
    struct mu3d_req *mreq = NULL;
    paddr_t gpd_dma;

    mep = mtu3_find_ep(epnum, USB_DIR_OUT);
    if (mep == NULL) {
        DBG_C("no udc_endpoint is found\n");
        return;
    }

    ring = &mep->gpd_ring;
    gpd = ring->dequeue;

    gpd_dma = readl(USB_QMU_RQCPR(epnum));
    gpd_current = gpd_dma_to_virt(ring, gpd_dma);

    LTRACEF("%s %s, last=%p, current=%p, enq=%p\n",
          __func__, mep->name, gpd, gpd_current, ring->enqueue);

    arch_clean_invalidate_cache_range((addr_t) gpd, sizeof(struct qmu_gpd));
    while (gpd != gpd_current && !(gpd->flag & GPD_FLAGS_HWO)) {
        arch_clean_invalidate_cache_range((addr_t) gpd, sizeof(struct qmu_gpd));

        request = mep->req;
        mreq = to_mu3d_req(request);
        if (mreq == NULL || mreq->gpd != gpd) {
            DBG_C("no correct RX req is found\n");
            break;
        }
        mreq->actual = gpd->buf_len;
        handle_ept_complete(mep, 0);
        gpd = advance_deq_gpd(ring);
    }

    LTRACEF("%s EP%dOUT, deq=%p, enq=%p, complete\n",
          __func__, epnum, ring->dequeue, ring->enqueue);
}

static void qmu_done_isr(u32 done_status)
{
    int i;

    for (i = 1; i <= (MT_EP_NUM / 2); i++) {
        if (done_status & QMU_RX_DONE_INT(i))
            qmu_done_rx(i);
        if (done_status & QMU_TX_DONE_INT(i))
            qmu_done_tx(i);
    }
}

static void qmu_exception_isr(u32 qmu_status)
{
    u32 errval;
    int i;

    if ((qmu_status & RXQ_CSERR_INT) || (qmu_status & RXQ_LENERR_INT)) {
        errval = readl(U3D_RQERRIR0);
        for (i = 1; i <= (MT_EP_NUM / 2); i++) {
            if (errval & QMU_RX_CS_ERR(i))
                DBG_C("Rx EP%d CS error!\n", i);

            if (errval & QMU_RX_LEN_ERR(i))
                DBG_C("RX EP%d Length error\n", i);
        }
        writel(errval, U3D_RQERRIR0);
    }

    if (qmu_status & RXQ_ZLPERR_INT) {
        errval = readl(U3D_RQERRIR1);
        for (i = 1; i <= (MT_EP_NUM / 2); i++) {
            if (errval & QMU_RX_ZLP_ERR(i))
                LTRACEF("RX EP%d Recv ZLP\n", i);
        }
        writel(errval, U3D_RQERRIR1);
    }

    if ((qmu_status & TXQ_CSERR_INT) || (qmu_status & TXQ_LENERR_INT)) {
        errval = readl(U3D_TQERRIR0);
        for (i = 1; i <= (MT_EP_NUM / 2); i++) {
            if (errval & QMU_TX_CS_ERR(i))
                DBG_C("Tx EP%d checksum error!\n", i);

            if (errval & QMU_TX_LEN_ERR(i))
                LTRACEF("Tx EP%d send ZLP failed\n", i);
        }
        writel(errval, U3D_TQERRIR0);
    }
}

enum handler_return mtu3_qmu_isr(void)
{
    u32 qmu_status;
    u32 qmu_done_status;

    /* U3D_QISAR1 is read update */
    qmu_status = readl(U3D_QISAR1);
    qmu_status &= readl(U3D_QIER1);

    qmu_done_status = readl(U3D_QISAR0);
    qmu_done_status &= readl(U3D_QIER0);
    writel(qmu_done_status, U3D_QISAR0); /* W1C */
    LTRACEF("[INTR] QMUdone[TX=%x, RX=%x] QMUexp[%x]\n",
          (qmu_done_status & 0xFFFF), qmu_done_status >> 16,
          qmu_status);

    if (qmu_done_status)
        qmu_done_isr(qmu_done_status);

    if (qmu_status)
        qmu_exception_isr(qmu_status);

    return INT_RESCHEDULE;
}

int mtu3_qmu_init(void)
{
    if (QMU_GPD_SIZE != 16) {
        DBG_C("QMU_GPD size SHOULD be 16 Bytes");
        return -EFAULT;
    }
    return 0;
}

#else   /* PIO mode */

void mtu3_qmu_flush(struct udc_endpoint *mep)
{}

int mtu3_gpd_ring_alloc(struct udc_endpoint *mep)
{
    return 0;
}

void mtu3_gpd_ring_free(struct udc_endpoint *mep)
{}

enum handler_return mtu3_qmu_isr(void)
{
    return INT_NO_RESCHEDULE;
}

int mtu3_qmu_init(void)
{
    return 0;
}

#endif  /* SUPPORT_QMU */
