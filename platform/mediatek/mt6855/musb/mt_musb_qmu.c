/*
 * Copyright (c) 2021 MediaTek Inc.
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

#include <arch/ops.h>
#include <debug.h>
#include <errno.h>
#include <kernel/vm.h>
#include <platform/reg.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <trace.h>

#include "mt_musb_qmu.h"
#include "mt_musb.h"

u32 gpd_buf_size;
u32 g_dma_buffer_size;
struct tgpd *rx_gpd_head[15];
struct tgpd *tx_gpd_head[15];
struct tgpd *rx_gpd_end[15];
struct tgpd *tx_gpd_end[15];
struct tgpd *rx_gpd_last[15];
struct tgpd *tx_gpd_last[15];
struct gpd_range rx_gpd_list[15] __ALIGNED(64);
struct gpd_range tx_gpd_list[15] __ALIGNED(64);
u64 rx_gpd_offset[15];
u64 tx_gpd_offset[15];

/* USB DEBUG */
#ifdef DBG_USB_QMU
#define DBG_C(x...) dprintf(CRITICAL, "[USB][QMU] " x)
#define DBG_I(x...) dprintf(INFO, "[USB][QMU] " x)
#define DBG_S(x...) dprintf(SPEW, "[USB][QMU] " x)
#else
#define DBG_C(x...) do {} while (0)
#define DBG_I(x...) do {} while (0)
#define DBG_S(x...) do {} while (0)
#endif

/*
 * get_bd - get a null gpd
 * @args - arg1: dir, arg2: ep number
 */
struct tgpd *get_gpd(u8 dir, u32 num)
{
    struct tgpd *ptr;

    DBG_I("%s\n", __func__);

    if (dir == USB_DIR_OUT) {
        ptr = (struct tgpd *)rx_gpd_list[num].pnext;
        DBG_I("(rx_gpd_list[%d].pnext: %p)\n", num, (void *)(rx_gpd_list[num].pnext));
        if ((rx_gpd_list[num].pnext + 1) < rx_gpd_list[num].pend)
            rx_gpd_list[num].pnext++;
        else
            rx_gpd_list[num].pnext = rx_gpd_list[num].pstart;
    } else {
        ptr = (struct tgpd *)tx_gpd_list[num].pnext;
        DBG_I("(tx_gpd_list[%d].pnext: %p)\n", num, (void *)(tx_gpd_list[num].pnext));
        tx_gpd_list[num].pnext++;
        if (tx_gpd_list[num].pnext >= tx_gpd_list[num].pend)
            tx_gpd_list[num].pnext = tx_gpd_list[num].pstart;
    }
    return ptr;
}

/*
 * get_bd - align gpd ptr to target ptr
 * @args - arg1: dir, arg2: ep number, arg3: target ptr
 */
void gpd_ptr_align(u8 dir, u32 num, struct tgpd *ptr)
{
    u32 run_next;

    run_next = true;
    while (run_next) {
        if (ptr == get_gpd(dir, num))
            run_next = false;
    }
}

paddr_t gpd_virt_to_phys(void *vaddr, u8 dir, u32 num)
{
    paddr_t paddr;

    if (dir)
        paddr = (paddr_t)((u64)(uintptr_t)vaddr - rx_gpd_offset[num]);
    else
        paddr = (paddr_t)((u64)(uintptr_t)vaddr - tx_gpd_offset[num]);

    return paddr;
}

void *gpd_phys_to_virt(paddr_t paddr, u8 dir, u32 num)
{
    void *vaddr;

    if (dir)
        vaddr = (void *)(uintptr_t)((u64)paddr + rx_gpd_offset[num]);
    else
        vaddr = (void *)(uintptr_t)((u64)paddr + tx_gpd_offset[num]);

    return vaddr;
}

/*
 * init_gpd_list - initialize gpd management list
 * @args - arg1: dir, arg2: ep number, arg3: gpd virtual addr,
 * arg4: gpd ioremap addr, arg5: gpd number
 */
void init_gpd_list(u8 dir, u32 num, struct tgpd *ptr, struct tgpd *io_ptr, u32 size)
{
    if (dir == USB_DIR_OUT) {
        rx_gpd_list[num].pstart = (struct tbd *)ptr;
        rx_gpd_list[num].pend = (struct tbd *)(ptr + size);
        rx_gpd_offset[num] = (u64)(uintptr_t)ptr - (u64)(uintptr_t)io_ptr;
        rx_gpd_list[num].pnext = (struct tbd *)(ptr + 1);
#if 0
        DBG_I("rx_gpd_list[%d].pstart: %p\n", num, rx_gpd_list[num].pstart);
        DBG_I("rx_gpd_list[%d].pnext:  %p\n", num, rx_gpd_list[num].pnext);
        DBG_I("rx_gpd_list[%d].pend:   %p\n", num, rx_gpd_list[num].pend);
        DBG_I("rx_gpd_offset[%d]: %p\n", num, (void *)rx_gpd_offset[num]);
        DBG_I("ptr: %p\n", ptr);
        DBG_I("ptr end: %p\n", ptr + size);
        DBG_I("io_ptr: %p\n", io_ptr);
        DBG_I("io_ptr end: %p\n", io_ptr + size);
#endif
    } else {
        tx_gpd_list[num].pstart = (struct tbd *)ptr;
        tx_gpd_list[num].pend = (struct tbd *)((u8 *)(ptr + size));
        tx_gpd_offset[num] = (u64)(uintptr_t)ptr - (u64)(uintptr_t)io_ptr;
        tx_gpd_list[num].pnext = (struct tbd *)(ptr + 1);
#if 0

        DBG_I("tx_gpd_list[%d].pstart: %p\n", num, tx_gpd_list[num].pstart);
        DBG_I("tx_gpd_list[%d].pnext:  %p\n", num, tx_gpd_list[num].pnext);
        DBG_I("tx_gpd_list[%d].pend:   %p\n", num, tx_gpd_list[num].pend);
        DBG_I("tx_gpd_offset[%d]: %p\n", num, (void *)tx_gpd_offset[num]);
        DBG_I("ptr: %p\n", ptr);
        DBG_I("ptr end: %p\n", ptr + size);
        DBG_I("io_ptr: %p\n", io_ptr);
        DBG_I("io_ptr end: %p\n", io_ptr + size);
#endif
    }
}

/*
 * free_gpd - free gpd management list
 * @args - arg1: dir, arg2: ep number
 */
void free_gpd(u8 dir, u32 num)
{
    if (dir == USB_DIR_OUT)
        memset(rx_gpd_list[num].pstart, 0, MAX_GPD_NUM * sizeof(struct tgpd));
    else
        memset(tx_gpd_list[num].pstart, 0, MAX_GPD_NUM * sizeof(struct tgpd));
}

/*
 * mu3d_hal_alloc_qmu_mem - allocate gpd and bd memory for all ep
 *
 */
void mu3d_hal_alloc_qmu_mem(void)
{
    int err;
    u32 i, size;
    struct tgpd *ptr, *io_ptr;

    for (i = 1; i <= MAX_QMU_EP; i++) {
        /* alloc RX */
        size = MAX_GPD_NUM * sizeof(struct tgpd);
        err = vmm_alloc_contiguous(vmm_get_kernel_aspace(), "USB uncached memory",
                                   size, &ptr, PAGE_SIZE_SHIFT, 0, ARCH_MMU_FLAG_UNCACHED);
        if (err)
            panic("%s: cannot allocate RX QMU memory\n", __func__);
        memset(ptr, 0, size);
        io_ptr = (struct tgpd *)vaddr_to_paddr(ptr);

        init_gpd_list(USB_DIR_OUT, i, ptr, io_ptr, MAX_GPD_NUM);
        rx_gpd_end[i] = ptr;
        memset(rx_gpd_end[i], 0, sizeof(struct tgpd));
        TGPD_CLR_FLAGS_HWO(rx_gpd_end[i]);
        rx_gpd_head[i] = rx_gpd_last[i] = rx_gpd_end[i];
        DBG_I("RQSAR[%d]: %p\n", i, rx_gpd_end[i]);

        /* alloc TX */
        size = MAX_GPD_NUM * sizeof(struct tgpd);
        err = vmm_alloc_contiguous(vmm_get_kernel_aspace(), "USB uncached memory",
                                   size, &ptr, PAGE_SIZE_SHIFT, 0, ARCH_MMU_FLAG_UNCACHED);
        if (err)
            panic("%s: cannot allocate TX QMU memory\n", __func__);
        memset(ptr, 0, size);
        io_ptr = (struct tgpd *)vaddr_to_paddr(ptr);

        init_gpd_list(USB_DIR_IN, i, ptr, io_ptr, MAX_GPD_NUM);
        tx_gpd_end[i] = ptr;
        memset(tx_gpd_end[i], 0, sizeof(struct tgpd));
        TGPD_CLR_FLAGS_HWO(tx_gpd_end[i]);
        tx_gpd_head[i] = tx_gpd_last[i] = tx_gpd_end[i];
        DBG_I("TQSAR[%d]: %p\n", i, tx_gpd_end[i]);
    }
}

/*
 * mu3d_hal_init_qmu - initialize qmu
 *
 */
void mu3d_hal_init_qmu(void)
{
    DBG_I("%s\n", __func__);
    u32 i;

    /* Initialize QMU Tx/Rx start address. */
    for (i = 1; i <= MAX_QMU_EP; i++) {
        MGC_WriteQMU32(MGC_O_QMU_RQSAR(i),
                       (uintptr_t)gpd_virt_to_phys(rx_gpd_head[i], USB_DIR_OUT, i));
        MGC_WriteQMU32(MGC_O_QMU_TQSAR(i),
                       (uintptr_t)gpd_virt_to_phys(tx_gpd_head[i], USB_DIR_IN, i));
        DBG_I("U3D_RXQSAR%d: %p, val: %x\n", i,
              (void *)MGC_O_QMU_RQSAR(i), MGC_ReadQMU32(MGC_O_QMU_RQSAR(i)));
        DBG_I("U3D_TXQSAR%d: %p, val: %x\n", i,
              (void *)MGC_O_QMU_TQSAR(i), MGC_ReadQMU32(MGC_O_QMU_TQSAR(i)));
        arch_clean_invalidate_cache_range((addr_t) tx_gpd_head[i],
                                          MAX_GPD_NUM * sizeof(struct tgpd));
        arch_clean_invalidate_cache_range((addr_t) rx_gpd_head[i],
                                          MAX_GPD_NUM * sizeof(struct tgpd));

        tx_gpd_end[i] = tx_gpd_last[i] = tx_gpd_head[i];
        rx_gpd_end[i] = rx_gpd_last[i] = rx_gpd_head[i];

        gpd_ptr_align(USB_DIR_OUT, i, rx_gpd_end[i]);
        gpd_ptr_align(USB_DIR_IN, i, tx_gpd_end[i]);

        /* Enable QMU Tx/Rx. */
        MGC_WriteQUCS32(MGC_O_QUCS_USBGCSR,  MGC_ReadQUCS32(MGC_O_QUCS_USBGCSR) | USB_QMU_Rx_EN(i));
        MGC_WriteQUCS32(MGC_O_QUCS_USBGCSR,  MGC_ReadQUCS32(MGC_O_QUCS_USBGCSR) | USB_QMU_Tx_EN(i));
    }

    DBG_I("MGC_O_QUCS_USBGCSR %x\n", MGC_ReadQUCS32(MGC_O_QUCS_USBGCSR));
}

/*
 * mu3d_hal_cal_checksum - calculate check sum
 * @args - arg1: data buffer, arg2: data length
 */
u8 mu3d_hal_cal_checksum(u8 *data, int len)
{
    u8 *pdata, cksum;
    int i;

    *(data + 1) = 0x0;
    pdata = data;
    cksum = 0;
    for (i = 0; i < len; i++)
        cksum += *(pdata + i);

    return 0xFF - cksum;
}

/*
 * mu3d_hal_resume_qmu - resume qmu function
 * @args - arg1: ep number, arg2: dir
 */
void mu3d_hal_resume_qmu(int q_ep_num, u8 dir)
{
    DBG_I("%s\n", __func__);
    if (dir == USB_DIR_OUT) {
        //DBG_I("USB_QMU_Resume USB_RX %x\n", (USB_HW_QMU_OFF + MGC_O_QMU_RQCSR(ep_num)));
        MGC_WriteQMU32(MGC_O_QMU_RQCSR(q_ep_num), DQMU_QUE_RESUME);
        if (!MGC_ReadQMU32(MGC_O_QMU_RQCSR(q_ep_num))) {
            DBG_I("%s: RXQCSR1 val: %x\n", __func__, MGC_ReadQMU32(MGC_O_QMU_RQCSR(q_ep_num)));
            MGC_WriteQMU32(MGC_O_QMU_RQCSR(q_ep_num), DQMU_QUE_RESUME);
        }
    } else {
        //DBG_I("USB_QMU_Resume USB_TX %x\n", (USB_HW_QMU_OFF + MGC_O_QMU_TQCSR(ep_num)));
        MGC_WriteQMU32(MGC_O_QMU_TQCSR(q_ep_num), DQMU_QUE_RESUME);
        if (!MGC_ReadQMU32(MGC_O_QMU_TQCSR(q_ep_num))) {  //judge if Queue is still inactive
            DBG_I("%s: TXQCSR1 val: %x\n", __func__, MGC_ReadQMU32(MGC_O_QMU_TQCSR(q_ep_num)));
            MGC_WriteQMU32(MGC_O_QMU_TQCSR(q_ep_num), DQMU_QUE_RESUME);
        }
    }
}

/*
 * mu3d_hal_prepare_tx_gpd - prepare tx gpd/bd
 * @args - arg1: gpd address, arg2: data buffer address,
 * arg3: data length, arg4: ep number, arg5: with bd or not,
 * arg6: write hwo bit or not,  arg7: write ioc bit or not
 */
struct tgpd *mu3d_hal_prepare_tx_gpd(struct tgpd *gpd, paddr_t pbuf,
                            u32 data_len, u8 ep_num, u8 _is_bdp,
                            u8 ishwo, u8 ioc, u8 bps, u8 zlp)
{
#if defined(SUPPORT_VA)
    u8      *vbuffer;
#endif

    DBG_I("%s: ep_num: %d, pbuf: %x, data_len: %x, zlp: %x\n",
             __func__, (int)ep_num, (u32)pbuf, data_len, (u32)zlp);

    arch_clean_invalidate_cache_range((addr_t) gpd, sizeof(struct tgpd));

    if (!_is_bdp) {
        TGPD_SET_DATA(gpd, pbuf);
#ifdef SUPPORT_36BIT
        TGPD_SET_DATA_TXHI(gpd, (u8)(pbuf >> 32));
#endif
        TGPD_CLR_FORMAT_BDP(gpd);
    } else {
        DBG_I("_is_bdp error\n");
    }

    TGPD_SET_BUF_LEN(gpd, data_len);
    TGPD_SET_EXT_LEN(gpd, 0);

    if (zlp)
        TGPD_SET_FORMAT_ZLP(gpd);
    else
        TGPD_CLR_FORMAT_ZLP(gpd);

    if (bps)
        TGPD_SET_FORMAT_BPS(gpd);
    else
        TGPD_CLR_FORMAT_BPS(gpd);

    if (ioc)
        TGPD_SET_FORMAT_IOC(gpd);
    else
        TGPD_CLR_FORMAT_IOC(gpd);

    /* Create next GPD */
    tx_gpd_end[ep_num] = get_gpd(USB_DIR_IN, ep_num);
    DBG_I("Malloc Tx 01 (GPD+EXT) (tx_gpd_end): 0x%x\n", tx_gpd_end[ep_num]);

    arch_clean_invalidate_cache_range((addr_t) tx_gpd_end[ep_num], sizeof(struct tgpd));
    memset(tx_gpd_end[ep_num], 0, sizeof(struct tgpd));
    TGPD_CLR_FLAGS_HWO(tx_gpd_end[ep_num]);
    TGPD_SET_NEXT(gpd, (uintptr_t)gpd_virt_to_phys(tx_gpd_end[ep_num], USB_DIR_IN, ep_num));
#ifdef SUPPORT_36BIT
    TGPD_SET_NEXT_TXHI(gpd,
                       (u8)(gpd_virt_to_phys(tx_gpd_end[ep_num], USB_DIR_IN, ep_num) >> 32));
#endif
    if (ishwo) {
        TGPD_SET_CHKSUM(gpd, CHECKSUM_LENGTH);
        TGPD_SET_FLAGS_HWO(gpd);
    } else {
        TGPD_CLR_FLAGS_HWO(gpd);
        TGPD_SET_CHKSUM_HWO(gpd, CHECKSUM_LENGTH);
    }

    /* gpd end */
    arch_clean_invalidate_cache_range((addr_t) gpd, sizeof(struct tgpd));

    return gpd;
}

/*
 * mu3d_hal_prepare_rx_gpd - prepare rx gpd/bd
 * @args - arg1: gpd address, arg2: data buffer address,
 * arg3: data length, arg4: ep number, arg5: with bd or not,
 * arg6: write hwo bit or not,  arg7: write ioc bit or not
 */
struct tgpd *mu3d_hal_prepare_rx_gpd(struct tgpd *gpd, paddr_t pbuf,
                                     u32 data_len, u8 ep_num, u8 _is_bdp,
                                     u8 ishwo, u8 ioc, u8 bps, u32 max_pkt_size)
{
    DBG_I("%s: GPD: %p, ep_num: %d, pbuf: %x, data_len: %x, _is_bdp: %d,
            ishwo: %d, ioc: %d, bps: %d\n",
            __func__, gpd, (int)ep_num,
            (u32)pbuf, data_len, _is_bdp, ishwo, ioc, bps);

    arch_clean_invalidate_cache_range((addr_t) gpd, sizeof(struct tgpd));

    if (!_is_bdp) {
        TGPD_SET_DATA(gpd, pbuf);
#ifdef SUPPORT_36BIT
        TGPD_SET_DATA_RXHI(gpd, (u8)(pbuf >> 32));
#endif
        TGPD_CLR_FORMAT_BDP(gpd);
    } else {
        DBG_I("_is_bdp error\n");
    }

    if (data_len < gpd_buf_size)
        TGPD_SET_DATABUF_LEN(gpd, data_len); /* or length?? */
    else
        TGPD_SET_DATABUF_LEN(gpd, gpd_buf_size);

    TGPD_SET_BUF_LEN(gpd, 0);

    if (bps)
        TGPD_SET_FORMAT_BPS(gpd);
    else
        TGPD_CLR_FORMAT_BPS(gpd);

    if (ioc)
        TGPD_SET_FORMAT_IOC(gpd);
    else
        TGPD_CLR_FORMAT_IOC(gpd);

    rx_gpd_end[ep_num] = get_gpd(USB_DIR_OUT, ep_num);
    memset(rx_gpd_end[ep_num], 0, sizeof(struct tgpd));
    DBG_I("Rx Next GPD 0x%x\n", rx_gpd_end[ep_num]);
    TGPD_CLR_FLAGS_HWO(rx_gpd_end[ep_num]);

    /* block memory for qmu */
    mb();
    TGPD_SET_NEXT(gpd, (uintptr_t)gpd_virt_to_phys(rx_gpd_end[ep_num], USB_DIR_OUT, ep_num));

#ifdef SUPPORT_36BIT
    TGPD_SET_NEXT_RXHI(gpd, (u8)(gpd_virt_to_phys(rx_gpd_end[ep_num], USB_DIR_OUT, ep_num) >> 32));
#endif

    /* block memory for qmu */
    mb();
    if (ishwo) {
        TGPD_SET_CHKSUM(gpd, CHECKSUM_LENGTH);
        TGPD_SET_FLAGS_HWO(gpd);
    } else {
        TGPD_CLR_FLAGS_HWO(gpd);
        TGPD_SET_CHKSUM_HWO(gpd, CHECKSUM_LENGTH);
    }

    DBG_I("Rx gpd info { HWO %d, Next_GPD %x ,databuf_length %d, DataBuffer %x,
            Recived Len %d, Endpoint %d, TGL %d, ZLP %d}\n",
            (u32)TGPD_GET_FLAG(gpd), (u32)TGPD_GET_NEXT(gpd),
            (u32)TGPD_GET_DATABUF_LEN(gpd), (u32)TGPD_GET_DATA(gpd),
            (u32)TGPD_GET_BUF_LEN(gpd), (u32)TGPD_GET_EPaddr(gpd),
            (u32)TGPD_GET_TGL(gpd), (u32)TGPD_GET_ZLP(gpd));

    arch_clean_invalidate_cache_range((addr_t) gpd, sizeof(struct tgpd));
    return gpd;
}

/*
 * mu3d_hal_insert_transfer_gpd - insert new gpd/bd
 * @args - arg1: ep number, arg2: dir, arg3: data buffer,
 * arg4: data length,  arg5: write hwo bit or not,
 * arg6: write ioc bit or not
 */
void mu3d_hal_insert_transfer_gpd(u32 ep_num, u8 dir, paddr_t buf, u32 count,
                                  u8 ishwo, u8 ioc, u8 bps, u8 zlp, u32 max_pkt_size)
{
    struct tgpd *gpd;

    DBG_I("%s: ep_num: %d, dir: %d, buf: %x, count: %x, ishwo: %d, ioc: :%d,
            bps: %d, zlp: %x, maxp: %x\n",
            __func__, (int)ep_num, dir, (u32)buf, count, ishwo, ioc, bps,
            (u32)zlp, (u32)max_pkt_size);

    if (dir == USB_DIR_IN) {
        gpd = tx_gpd_end[ep_num];
        DBG_I("TX gpd: %x\n", gpd);
        /*
         *is_bdp = 0; We need to use BD because for mass storage,
         *the largest data > GPD limitation
         */
        mu3d_hal_prepare_tx_gpd(gpd, buf, count, ep_num, 0, ishwo, ioc, bps, zlp);
    } else if (dir == USB_DIR_OUT) {
        gpd = rx_gpd_end[ep_num];
        DBG_I("RX gpd: %x\n", gpd);
        /*
         * is_bdp = 0; We need to use BD because for mass storage,
         * the largest data > GPD limitation
         */
        mu3d_hal_prepare_rx_gpd(gpd, buf, count, ep_num, 0, ishwo, ioc, bps, max_pkt_size);
    }
}

/*
 * mu3d_hal_start_qmu - start qmu function
 * (QMU flow :
 * mu3d_hal_init_qmu ->
 * mu3d_hal_start_qmu ->
 * mu3d_hal_insert_transfer_gpd ->
 * mu3d_hal_resume_qmu)
 * @args - arg1: ep number, arg2: dir
 */
void usb_start_qmu(int q_ep_num, u8 dir)
{
    u32 qcr;
    u32 txZLP, rxZLP;
    u32 isEmptyCheck = 0;

    DBG_I("%s: dir: %x\n", __func__, dir);

#ifdef CFG_RX_ZLP_EN
    rxZLP = 1;
#else
    rxZLP = 0;
#endif

#if (TXZLP == HW_MODE)
    txZLP = 1;
    //qcr = readl(U3D_QCR1);
    //writel(qcr &~ QMU_TX_ZLP(q_ep_num), U3D_QCR1);
    //qcr = readl(U3D_QCR2);
    //writel(qcr | QMU_TX_ZLP(q_ep_num), U3D_QCR2);
#elif (TXZLP == GPD_MODE)
    txZLP = 0;
    //qcr = readl(U3D_QCR1);
    //writel(qcr | QMU_TX_ZLP(q_ep_num), U3D_QCR1);
#endif

    if (dir == USB_DIR_IN) {
        qcr = MGC_ReadQMU32(MGC_O_QMU_QCR0);
        MGC_WriteQMU32(MGC_O_QMU_QCR0, qcr | DQMU_TQCS_EN(q_ep_num));

        if (txZLP) {
            qcr = MGC_ReadQMU32(MGC_O_QMU_QCR2);
            MGC_WriteQMU32(MGC_O_QMU_QCR2, qcr | DQMU_TX_ZLP(q_ep_num));
        }

        MGC_WriteQIRQ32(MGC_O_QIRQ_QIMCR, DQMU_M_TX_DONE(q_ep_num)
                       | DQMU_M_TQ_EMPTY | DQMU_M_TXQ_ERR | DQMU_M_TXEP_ERR);

        if (isEmptyCheck)
            MGC_WriteQIRQ32(MGC_O_QIRQ_TEPEMPMCR, DQMU_M_TX_EMPTY(q_ep_num));
        else
            MGC_WriteQIRQ32(MGC_O_QIRQ_QIMSR, DQMU_M_TQ_EMPTY);

        qcr = DQMU_M_TX_LEN_ERR(q_ep_num);
        qcr |= DQMU_M_TX_GPDCS_ERR(q_ep_num) | DQMU_M_TX_BDCS_ERR(q_ep_num);
        MGC_WriteQIRQ32(MGC_O_QIRQ_TQEIMCR, qcr);

        MGC_WriteQIRQ32(MGC_O_QIRQ_TEPEIMCR, DQMU_M_TX_EP_ERR(q_ep_num));

        if (MGC_ReadQMU16(MGC_O_QMU_TQCSR(q_ep_num)) & DQMU_QUE_ACTIVE) {
            DBG_I("Tx %d Active Now!\n", q_ep_num);
            return;
        }
        MGC_WriteQMU32(MGC_O_QMU_TQCSR(q_ep_num), DQMU_QUE_START);

    } else if (dir == USB_DIR_OUT) {
        qcr = MGC_ReadQMU32(MGC_O_QMU_QCR0);
        MGC_WriteQMU32(MGC_O_QMU_QCR0, qcr | DQMU_RQCS_EN(q_ep_num));
#if 0
        if (rxZLP) {
            qcr = MGC_ReadQMU32(MGC_O_QMU_QCR3);
            MGC_WriteQMU32(MGC_O_QMU_QCR3, qcr | DQMU_RX_ZLP(q_ep_num));
        } else {
            qcr = MGC_ReadQMU32(MGC_O_QMU_QCR3);
            MGC_WriteQMU32(MGC_O_QMU_QCR3, qcr & ~DQMU_RX_ZLP(q_ep_num));
        }
#endif
        MGC_WriteQIRQ32(MGC_O_QIRQ_QIMCR, DQMU_M_RX_DONE(q_ep_num)
                        | DQMU_M_RQ_EMPTY | DQMU_M_RXQ_ERR | DQMU_M_RXEP_ERR);

        if (isEmptyCheck)
            MGC_WriteQIRQ32(MGC_O_QIRQ_REPEMPMCR, DQMU_M_RX_EMPTY(q_ep_num));
        else
            MGC_WriteQIRQ32(MGC_O_QIRQ_QIMSR, DQMU_M_RQ_EMPTY);

        qcr = DQMU_M_RX_LEN_ERR(q_ep_num);
        qcr |= DQMU_M_RX_GPDCS_ERR(q_ep_num);
        qcr |= rxZLP ? DQMU_M_RX_ZLP_ERR(q_ep_num) : 0;
        MGC_WriteQIRQ32(MGC_O_QIRQ_RQEIMCR, qcr);

        MGC_WriteQIRQ32(MGC_O_QIRQ_REPEIMCR, DQMU_M_RX_EP_ERR(q_ep_num));

        if (MGC_ReadQMU16(MGC_O_QMU_RQCSR(q_ep_num)) & DQMU_QUE_ACTIVE) {
            DBG_I("Rx %d Active Now!\n", q_ep_num);
            return;
        }

        MGC_WriteQMU32(MGC_O_QMU_RQCSR(q_ep_num), DQMU_QUE_START);
    }
}

/*
 * mu3d_hal_stop_qmu - stop qmu function (after qmu stop, fifo should be flushed)
 * @args - arg1: ep number, arg2: dir
 */
void mu3d_hal_stop_qmu(int q_ep_num, u8 dir)
{
    DBG_I("%s\n", __func__);
    if (dir == USB_DIR_IN) {
        if (MGC_ReadQMU16(MGC_O_QMU_TQCSR(q_ep_num))&DQMU_QUE_ACTIVE) {
            MGC_WriteQMU32(MGC_O_QMU_TQCSR(q_ep_num), DQMU_QUE_STOP);
            while (MGC_ReadQMU16(MGC_O_QMU_TQCSR(q_ep_num))&DQMU_QUE_ACTIVE)
            ;
            DBG_I("Stop Tx Queue %d!\n", q_ep_num);
        } else {
            DBG_I("Tx Queue %d InActive Now, Don't need to stop!\n", q_ep_num);
        }
    } else {
        if (MGC_ReadQMU16(MGC_O_QMU_RQCSR(q_ep_num))&DQMU_QUE_ACTIVE) {
            MGC_WriteQMU32(MGC_O_QMU_RQCSR(q_ep_num), DQMU_QUE_STOP);
            while (MGC_ReadQMU16(MGC_O_QMU_RQCSR(q_ep_num))&DQMU_QUE_ACTIVE)
            ;
            DBG_I("Stop Rx Queue %d!\n", q_ep_num);
        } else {
            DBG_I("Rx Queue %d InActive Now, Don't need to stop!\n", q_ep_num);
        }
    }
}

/*
 * flush_qmu - stop qmu and align qmu start ptr t0 current ptr
 * @args - arg1: ep number, arg2: dir
 */
void mu3d_hal_flush_qmu(u32 q_ep_num,  u8 dir)
{
    struct tgpd *gpd;
    struct tgpd *gpd_current;
    struct udc_endpoint *ept;
    struct urb *urb;
    //struct USB_REQ *req = mu3d_hal_get_req(q_ep_num, dir);

    ept = mt_find_ep(q_ep_num, dir);

    if (ept == NULL) {
        DBG_I("ept is NULL!\n");
        return;
    }

    DBG_I("%s\n", __func__);

    if (dir == USB_DIR_IN) {
        DBG_I("flush_qmu USB_DIR_IN\n");
        urb = ept->tx_urb;
        mu3d_hal_stop_qmu(q_ep_num, USB_DIR_IN);
        gpd_current = (struct tgpd *)(uintptr_t)(MGC_ReadQMU32(MGC_O_QMU_TQCPR(q_ep_num)));
        gpd = (struct tgpd *)gpd_phys_to_virt((paddr_t)(uintptr_t)gpd_current,
                                               USB_DIR_IN, q_ep_num);


        DBG_I("(MGC_O_QMU_TQCPR gpd_current: %p)\n", gpd_current);

        if (!gpd_current) {
            gpd_current = (struct tgpd *)(uintptr_t)(MGC_ReadQMU32(MGC_O_QMU_TQSAR(q_ep_num)));
            gpd = (struct tgpd *)gpd_phys_to_virt((paddr_t)(uintptr_t)gpd_current,
                                                  USB_DIR_IN, q_ep_num);
        }

        DBG_I("(MGC_O_QMU_TQSAR gpd_current: %p)\n", gpd_current);

        tx_gpd_end[q_ep_num] = tx_gpd_last[q_ep_num] = gpd;

        gpd_ptr_align(dir, q_ep_num, tx_gpd_end[q_ep_num]);
        free_gpd(dir, q_ep_num);

        arch_clean_invalidate_cache_range((addr_t) tx_gpd_list[q_ep_num].pstart,
                                           MAX_GPD_NUM * sizeof(struct tgpd));
        MGC_WriteQMU32(MGC_O_QMU_TQSAR(q_ep_num), gpd_current);

        urb->qmu_complete = true;
        DBG_I("TxQ %d Flush Now!\n", q_ep_num);
    } else if (dir == USB_DIR_OUT) {
        urb = ept->rcv_urb;
        mu3d_hal_stop_qmu(q_ep_num, USB_DIR_OUT);
        gpd_current = (struct tgpd *)(uintptr_t)(MGC_ReadQMU32(MGC_O_QMU_RQCPR(q_ep_num)));
        gpd = (struct tgpd *)gpd_phys_to_virt((paddr_t)(uintptr_t)gpd_current,
                                               USB_DIR_OUT, q_ep_num);

        if (!gpd_current) {
            gpd_current = (struct tgpd *)(uintptr_t)(MGC_ReadQMU32(MGC_O_QMU_RQSAR(q_ep_num)));
            gpd = (struct tgpd *)gpd_phys_to_virt((paddr_t)(uintptr_t)gpd_current,
                                                   USB_DIR_OUT, q_ep_num);
        }

        rx_gpd_end[q_ep_num] = rx_gpd_last[q_ep_num] = gpd;

        gpd_ptr_align(dir, q_ep_num, rx_gpd_end[q_ep_num]);
        free_gpd(dir, q_ep_num);

        arch_clean_invalidate_cache_range((addr_t) rx_gpd_list[q_ep_num].pstart,
                                           MAX_GPD_NUM * sizeof(struct tgpd));
        MGC_WriteQMU32(MGC_O_QMU_RQSAR(q_ep_num), gpd_current);

        urb->qmu_complete = true;
        DBG_I("RxQ %d Flush Now!\n", q_ep_num);
    }

}
