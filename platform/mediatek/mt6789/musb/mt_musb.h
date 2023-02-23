/*
 * Copyright (c) 2022 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#include <dev/udc.h>
#include <platform/addressmap.h>
#include <platform/reg.h>
#include <platform/usb.h>

/* hardware spec */
#define MT_EP_NUM               4
#define MT_CHAN_NUM             4
#define MT_EP0_FIFOSIZE         64
#define FIFO_ADDR_START         512
#define MT_BULK_MAXP            512
#define MT_INT_MAXP             1024

/* USB PHY registers */
#define USB20_PHY_BASE          (USB_SIF_BASE + 0x0300)

/* USB common registers */
#define FADDR                   (USB_BASE + 0x0000)
#define POWER                   (USB_BASE + 0x0001)
#define INTRTX                  (USB_BASE + 0x0002)
#define INTRRX                  (USB_BASE + 0x0004)
#define INTRTXE                 (USB_BASE + 0x0006)
#define INTRRXE                 (USB_BASE + 0x0008)
#define INTRUSB                 (USB_BASE + 0x000a)
#define INTRUSBE                (USB_BASE + 0x000b)
#define FRAME                   (USB_BASE + 0x000c)
#define INDEX                   (USB_BASE + 0x000e)
#define TESTMODE                (USB_BASE + 0x000f)

/* Toggle registers */
#define RXTOG                   (USB_BASE + 0x0080)
#define RXTOGEN                 (USB_BASE + 0x0082)
#define TXTOG                   (USB_BASE + 0x0084)
#define TXTOGEN                 (USB_BASE + 0x0086)

/* POWER fields */
#define PWR_ISO_UPDATE          (1<<7)
#define PWR_SOFT_CONN           (1<<6)
#define PWR_HS_ENAB             (1<<5)
#define PWR_HS_MODE             (1<<4)
#define PWR_RESET               (1<<3)
#define PWR_RESUME              (1<<2)
#define PWR_SUSPEND_MODE        (1<<1)
#define PWR_ENABLE_SUSPENDM     (1<<0)

/* INTRUSB fields */
#define INTRUSB_VBUS_ERROR      (1<<7)
#define INTRUSB_SESS_REQ        (1<<6)
#define INTRUSB_DISCON          (1<<5)
#define INTRUSB_CONN            (1<<4)
#define INTRUSB_SOF             (1<<3)
#define INTRUSB_RESET           (1<<2)
#define INTRUSB_RESUME          (1<<1)
#define INTRUSB_SUSPEND         (1<<0)

/* DMA control registers */
#define USB_DMA_INTR_UNMASK_SET_OFFSET (24)
#define USB_DMA_INTR                   (USB_BASE + 0x0200)
#define USB_DMA_CNTL(chan)             (USB_BASE + 0x0204 + 0x10*(chan-1))
#define USB_DMA_ADDR(chan)             (USB_BASE + 0x0208 + 0x10*(chan-1))
#define USB_DMA_COUNT(chan)            (USB_BASE + 0x020c + 0x10*(chan-1))

/* Endpoint Control/Status Registers */
#define IECSR                    (USB_BASE + 0x0010)
#define CSR0                     0x2 /* host mode */
#define COUNT0                   0x8
#define NAKLIMIT0                0xB
#define CONFIGDATA               0xF

/* for other endpoints */
#define TXMAP                    0x0
#define TXCSR                    0x2
#define RXMAP                    0x4
#define RXCSR                    0x6
#define RXCOUNT                  0x8
#define TXTYPE                   0xa
#define TXINTERVAL               0xb
#define RXTYPE                   0xc
#define RXINTERVAL               0xd
#define FIFOSIZE                 0xf

/* control status register fields */
/* CSR0_DEV */
#define EP0_FLUSH_FIFO           (1<<8)
#define EP0_SERVICE_SETUP_END    (1<<7)
#define EP0_SERVICED_RXPKTRDY    (1<<6)
#define EP0_SENDSTALL            (1<<5)
#define EP0_SETUPEND             (1<<4)
#define EP0_DATAEND              (1<<3)
#define EP0_SENTSTALL            (1<<2)
#define EP0_TXPKTRDY             (1<<1)
#define EP0_RXPKTRDY             (1<<0)

/* TXCSR_DEV */
#define EPX_TX_AUTOSET           (1<<15)
#define EPX_TX_ISO               (1<<14)
#define EPX_TX_MODE              (1<<13)
#define EPX_TX_DMAREQEN          (1<<12)
#define EPX_TX_FRCDATATOG        (1<<11)
#define EPX_TX_DMAREQMODE        (1<<10)
#define EPX_TX_AUTOSETEN_SPKT    (1<<9)
#define EPX_TX_INCOMPTX          (1<<7)
#define EPX_TX_CLRDATATOG        (1<<6)
#define EPX_TX_SENTSTALL         (1<<5)
#define EPX_TX_SENDSTALL         (1<<4)
#define EPX_TX_FLUSHFIFO         (1<<3)
#define EPX_TX_UNDERRUN          (1<<2)
#define EPX_TX_FIFONOTEMPTY      (1<<1)
#define EPX_TX_TXPKTRDY          (1<<0)

/* RXCSR_DEV */
#define EPX_RX_AUTOCLEAR         (1<<15)
#define EPX_RX_ISO               (1<<14)
#define EPX_RX_DMAREQEN          (1<<13)
#define EPX_RX_DISNYET           (1<<12)
#define EPX_RX_PIDERR            (1<<12)
#define EPX_RX_DMAREQMODE        (1<<11)
#define EPX_RX_AUTOCLRENSPKT     (1<<10)
#define EPX_RX_INCOMPRXINTREN    (1<<9)
#define EPX_RX_INCOMPRX          (1<<8)
#define EPX_RX_CLRDATATOG        (1<<7)
#define EPX_RX_SENTSTALL         (1<<6)
#define EPX_RX_SENDSTALL         (1<<5)
#define EPX_RX_FLUSHFIFO         (1<<4)
#define EPX_RX_DATAERR           (1<<3)
#define EPX_RX_OVERRUN           (1<<2)
#define EPX_RX_FIFOFULL          (1<<1)
#define EPX_RX_RXPKTRDY          (1<<0)

/* CONFIGDATA fields */
#define MP_RXE                   (1<<7)
#define MP_TXE                   (1<<6)
#define BIGENDIAN                (1<<5)
#define HBRXE                    (1<<4)
#define HBTXE                    (1<<3)
#define DYNFIFOSIZING            (1<<2)
#define SOFTCONE                 (1<<1)
#define UTMIDATAWIDTH            (1<<0)

/* FIFO register */
#define FIFO(ep_num)             (USB_BASE + 0x0020 + ep_num*0x0004)

/* additional control registers */
#define DEVCTL                   (USB_BASE + 0x0060)
#define PWRUPCNT                 (USB_BASE + 0x0061)
#define TXFIFOSZ                 (USB_BASE + 0x0062)
#define RXFIFOSZ                 (USB_BASE + 0x0063)
#define TXFIFOADD                (USB_BASE + 0x0064)
#define RXFIFOADD                (USB_BASE + 0x0066)
#define HWVERS                   (USB_BASE + 0x006c)
#define SWRST                    (USB_BASE + 0x0074)
#define EPINFO                   (USB_BASE + 0x0078)
#define RAM_DMAINFO              (USB_BASE + 0x0079)
#define LINKINFO                 (USB_BASE + 0x007a)
#define VPLEN                    (USB_BASE + 0x007b)
#define HSEOF1                   (USB_BASE + 0x007c)
#define FSEOF1                   (USB_BASE + 0x007d)
#define LSEOF1                   (USB_BASE + 0x007e)
#define RSTINFO                  (USB_BASE + 0x007f)

/* FIFO size register fields and available packet size values */
#define DOUBLE_BUF               1
#define FIFOSZ_DPB               (1 << 4)
#define PKTSZ                    0x0f
#define PKTSZ_8                  (1<<3)
#define PKTSZ_16                 (1<<4)
#define PKTSZ_32                 (1<<5)
#define PKTSZ_64                 (1<<6)
#define PKTSZ_128                (1<<7)
#define PKTSZ_256                (1<<8)
#define PKTSZ_512                (1<<9)
#define PKTSZ_1024               (1<<10)

#define FIFOSZ_8                 (0x0)
#define FIFOSZ_16                (0x1)
#define FIFOSZ_32                (0x2)
#define FIFOSZ_64                (0x3)
#define FIFOSZ_128               (0x4)
#define FIFOSZ_256               (0x5)
#define FIFOSZ_512               (0x6)
#define FIFOSZ_1024              (0x7)
#define FIFOSZ_2048              (0x8)
#define FIFOSZ_4096              (0x9)
#define FIFOSZ_3072              (0xF)

/* SWRST fields */
#define SWRST_PHY_RST            (1<<7)
#define SWRST_PHYSIG_GATE_HS     (1<<6)
#define SWRST_PHYSIG_GATE_EN     (1<<5)
#define SWRST_REDUCE_DLY         (1<<4)
#define SWRST_UNDO_SRPFIX        (1<<3)
#define SWRST_FRC_VBUSVALID      (1<<2)
#define SWRST_SWRST              (1<<1)
#define SWRST_DISUSBRESET        (1<<0)

/* DMA_CNTL */
#define USB_DMA_CNTL_ENDMAMODE2  (1 << 13)
#define USB_DMA_CNTL_PP_RST      (1 << 12)
#define USB_DMA_CNTL_PP_EN       (1 << 11)
#define USB_DMA_BURST_MODE_MASK  (3 << 9)
#define USB_DMA_BURST_MODE_0     (0 << 9)
#define USB_DMA_BURST_MODE_1     (0x1 << 9)
#define USB_DMA_BURST_MODE_2     (0x2 << 9)
#define USB_DMA_BURST_MODE_3     (0x3 << 9)
#define USB_DMA_BUS_ERROR        (0x1 << 8)
#define USB_DMA_ENDPNT_MASK      (0xf << 4)
#define USB_DMA_ENDPNT_OFFSET    (4)
#define USB_DMA_INTEN            (1 << 3)
#define USB_DMA_DMAMODE          (1 << 2)
#define USB_DMA_DIR              (1 << 1)
#define USB_DMA_EN               (1 << 0)

/* interrupt register*/
#define USB_L1INTS               (USB_BASE + 0xa0)
#define USB_L1INTM               (USB_BASE + 0xa4)
#define USB_L1INTP               (USB_BASE + 0xa8)

#define TX_INT_STATUS            (1 << 0)
#define RX_INT_STATUS            (1 << 1)
#define USBCOM_INT_STATUS        (1 << 2)
#define DMA_INT_STATUS           (1 << 3)
#define PSR_INT_STATUS           (1 << 4)
#define QINT_STATUS              (1 << 5)
#define QHIF_INT_STATUS          (1 << 6)
#define DPDM_INT_STATUS          (1 << 7)
#define VBUSVALID_INT_STATUS     (1 << 8)
#define IDDIG_INT_STATUS         (1 << 9)
#define DRVVBUS_INT_STATUS       (1 << 10)
#define POWERDWN_INT_STATUS      (1 << 11)

#define VBUSVALID_INT_POL        (1 << 8)
#define IDDIG_INT_POL            (1 << 9)
#define DRVVBUS_INT_POL          (1 << 10)

/* some macros */
#define EPMASK(X)                (1 << X)
#define CHANMASK(X)              (1 << X)
#define CHECKSUM_LENGTH          16

/* USB transfer type */
#define USB_EP_XFER_CTRL         0
#define USB_EP_XFER_ISO          1
#define USB_EP_XFER_BULK         2
#define USB_EP_XFER_INT          3

/* USB transfer directions */
#define USB_DIR_IN               0x80
#define USB_DIR_OUT              0x00

/* mt_usb defines */
enum EP0_STATE {
    EP0_IDLE = 0,
    EP0_RX,
    EP0_TX,
};

void mt_usb_phy_recover(void);
void mt_usb_phy_poweron(void);
void mt_usb_phy_savecurrent(void);

/* mt_usb.c */
struct udc_endpoint *mt_find_ep(int ep_num, u8 dir);
void mt_setup_ep(unsigned int ep_num, struct udc_endpoint *endpoint);
void mt_usb_connect_internal(void);
void mt_usb_disconnect_internal(void);
void usb_qmu_init_and_start(void);

struct urb {
    struct udc_endpoint *endpoint;
    struct udc_device *device;
    struct usb_setup device_request;

    u8 *buf;
    unsigned int actual_length;

    u32 qmu_complete;
};

/* endpoint data - mt_ep */
struct udc_endpoint {
    /* rx side, active urb */
    struct urb *rcv_urb;
    /* tx side, active urb */
    struct urb *tx_urb;
    /* info from hsusb */
    struct udc_request *req;
    /* EPT_TX/EPT_RX */
    unsigned int bit;
    unsigned char num;
    unsigned char in;
    unsigned short maxpkt;
    /* status for error handling */
    int status;
    /* data already sent */
    unsigned int sent;
    /* data sent in last packet XXX do we need this */
    unsigned int last;
    /* double buffer */
    unsigned char mode;
    /* Transfer type */
    unsigned char type;
};

/* from mt_usbtty.h */
#define NUM_ENDPOINTS   3

/* origin endpoint_array */
extern struct udc_endpoint ep_list[NUM_ENDPOINTS + 1];  /* one extra for control endpoint */
