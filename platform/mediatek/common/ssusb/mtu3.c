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

#include <arch/ops.h>
#include <debug.h>
#include <dev/interrupt/arm_gic.h>
#include <errno.h>
#include <platform.h>
#include <platform/interrupts.h>
#include <platform/irq.h>
#include <platform/reg.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <trace.h>

#include "mtu3.h"
#include "mtu3_qmu.h"
#include "mt_gic.h"
#include "mt_usbphy.h"

#define LOCAL_TRACE 0

#define DBG_C(x...) dprintf(CRITICAL, "[USB] " x)

/* bits used in ep interrupts etc */
#define EPT_RX(n) (1 << ((n) + 16))
#define EPT_TX(n) (1 << (n))

#define EP0 0

/* Request types */
#define USB_TYPE_STANDARD   (0x00 << 5)
#define USB_TYPE_CLASS      (0x01 << 5)
#define USB_TYPE_VENDOR     (0x02 << 5)
#define USB_TYPE_RESERVED   (0x03 << 5)

/* values used in GET_STATUS requests */
#define USB_STAT_SELFPOWERED    0x01

/* USB recipients */
#define USB_RECIP_DEVICE    0x00
#define USB_RECIP_INTERFACE 0x01
#define USB_RECIP_ENDPOINT  0x02
#define USB_RECIP_OTHER     0x03

/* Endpoints */
#define USB_EP_NUM_MASK 0x0f        /* in bEndpointAddress */
#define USB_EP_DIR_MASK 0x80

#define USB_TYPE_MASK   0x60
#define USB_RECIP_MASK  0x1f

static struct mu3d g_u3d;


static void dump_setup_packet(const char *str, struct usb_setup *sp)
{
    LTRACEF("\n");
    LTRACEF("%s", str);
    LTRACEF("        bmRequestType = %x\n", sp->request_type);
    LTRACEF("        bRequest = %x\n", sp->request);
    LTRACEF("        wValue = %x\n", sp->value);
    LTRACEF("        wIndex = %x\n", sp->index);
    LTRACEF("        wLength = %x\n", sp->length);
}

static void *udc_zalloc(size_t size)
{
    void *buf;

    buf = malloc(size);
    if (buf)
        memset(buf, 0, size);
    return buf;
}

/* descriptors APIs */

static void change_eps_maxp(struct udc_descriptor *desc, u8 *buf, int len)
{
    struct udc_gadget *g = g_u3d.gadget;
    int n;

    /* only supports HS/FS bulk,  default maxp of all eps is 512, change it to 64 when FS; */
    if ((g_u3d.speed == SSUSB_SPEED_FULL) &&
            (desc->tag == ((CONFIGURATION << 8) | EP0))) {

        buf += (9 + 9);  /* size of (config + interface) */
        for (n = 0; n < g->ifc_endpoints; n++) {
            if ((9 + 9 + 5 + (7 * n)) >= len)
                break;

            buf[4] = 0x40;  /* ept->maxpkt; 64bytes */
            buf[5] = 0x00;  /* ept->maxpkt >> 8; 64bytes */
            buf += 7;
        }
    }
}

static void copy_desc(struct udc_request *req, void *data, int length)
{
    memcpy(req->buffer, data, length);
    req->length = length;
}

static struct udc_descriptor *udc_descriptor_alloc(unsigned int type,
        unsigned int num, unsigned int len)
{
    struct udc_descriptor *desc;

    if ((len > 255) || (len < 2) || (num > 255) || (type > 255)) {
        DBG_C("%s invalid argument\n", __func__);
        return NULL;
    }

    desc = udc_zalloc(sizeof(struct udc_descriptor) + len);
    if (!desc) {
        DBG_C("alloc desc failed (type:%d)\n", type);
        return NULL;
    }

    desc->next = 0;
    desc->tag = (type << 8) | num;
    desc->len = len;
    desc->data[0] = len;
    desc->data[1] = type;

    return desc;
}

static void udc_descriptor_register(struct udc_descriptor *desc, int dtype)
{
    if (dtype == DESC_TYPE_U2) {
        desc->next = g_u3d.desc_list;
        g_u3d.desc_list = desc;
    } else {
        desc->next = g_u3d.desc_list_u3;
        g_u3d.desc_list_u3 = desc;
    }
}

static unsigned int udc_string_desc_alloc(const char *str, int dtype)
{
    struct udc_descriptor *desc;
    unsigned char *data;
    unsigned int len;
    unsigned int *str_id;

    if (dtype == DESC_TYPE_U2)
        str_id = &g_u3d.next_string_id;
    else
        str_id = &g_u3d.next_string_id_u3;

    if (*str_id > 255)
        return 0;

    if (!str)
        return 0;

    len = strlen(str);
    desc = udc_descriptor_alloc(STRING, *str_id, len * 2 + 2);
    if (!desc)
        return 0;

    *str_id += 1;

    /* expand ascii string to utf16 */
    data = desc->data + 2;
    while (len-- > 0) {
        *data++ = *str++;
        *data++ = 0;
    }

    udc_descriptor_register(desc, dtype);
    return desc->tag & 0xff;
}

static void udc_ept_desc_fill(struct udc_endpoint *ept, unsigned char *data, int dtype)
{
    data[0] = 7;
    data[1] = ENDPOINT;
    data[2] = ept->num | (ept->in ? USB_DIR_IN : USB_DIR_OUT);
    data[3] = 0x02;     /* bulk -- the only kind we support */
    data[4] = 0x00;     /* ept->maxpkt; u2: 512bytes, u3: 1024 by default */
    data[5] = (dtype == DESC_TYPE_U2) ? 0x02 : 0x04; /* ept->maxpkt >> 8; */
    data[6] = ept->in ? 0x01 : 0x00;
}

static void udc_companion_desc_fill(uint8_t *data)
{
    data[0] = 6;
    data[1] = SS_EP_COMP;
    data[2] = 0x0f;     /* max burst: 0x0~0xf */
    data[3] = 0x00;
    data[4] = 0x00;
    data[5] = 0x00;
}

static unsigned int udc_ifc_desc_size(struct udc_gadget *g, int dtype)
{
    int tmp = 7; /* ep desc */

    tmp += ((dtype == DESC_TYPE_U2) ? 0 : 6); /* u3: add Companion desc */
    return 9 + g->ifc_endpoints * tmp;
}

static void udc_ifc_desc_fill(struct udc_gadget *g, unsigned char *data, int dtype)
{
    unsigned int n;

    data[0] = 0x09;
    data[1] = INTERFACE;
    data[2] = 0x00;     /* ifc number */
    data[3] = 0x00;     /* alt number */
    data[4] = g->ifc_endpoints; /* 0x02 */
    data[5] = g->ifc_class;     /* 0xff */
    data[6] = g->ifc_subclass;  /* 0x42 */
    data[7] = g->ifc_protocol;  /* 0x03 */
    data[8] = udc_string_desc_alloc(g->ifc_string, dtype);

    data += 9;
    for (n = 0; n < g->ifc_endpoints; n++) {
        udc_ept_desc_fill(g->ept[n], data, dtype);
        data += 7;
        if (dtype == DESC_TYPE_U3) {
            udc_companion_desc_fill(data);
            data += 6;
        }
    }
}

/* create our device descriptor */
static int udc_create_dev_desc(struct udc_device *udev, int dtype)
{
    struct udc_descriptor *desc;
    unsigned char *data;

    desc = udc_descriptor_alloc(DEVICE, EP0, 18);
    if (!desc)
        return -ENOMEM;

    data = desc->data;
    data[2] = 0x00;     /* usb spec minor rev */
    data[3] = (dtype == DESC_TYPE_U2) ? 0x02 : 0x03; /* usb spec major rev */
    data[4] = 0x00;     /* class */
    data[5] = 0x00;     /* subclass */
    data[6] = 0x00;     /* protocol */
    data[7] = (dtype == DESC_TYPE_U2) ? 0x40 : 0x09; /* maxp on ept 0 */
    memcpy(data + 8, &udev->vendor_id, sizeof(short));
    memcpy(data + 10, &udev->product_id, sizeof(short));
    memcpy(data + 12, &udev->version_id, sizeof(short));
    data[14] = udc_string_desc_alloc(udev->manufacturer, dtype);
    data[15] = udc_string_desc_alloc(udev->product, dtype);
    data[16] = udc_string_desc_alloc(udev->serialno, dtype);
    data[17] = 1;       /* number of configurations */
    udc_descriptor_register(desc, dtype);
    return 0;
}

/* create our BOS Binary Device Object descriptor - USB3 (FULL) */
static int udc_create_u3_bos_desc(void)
{
    struct udc_descriptor *desc;
    unsigned char *data;

    desc = udc_descriptor_alloc(BOS, EP0, 22);
    if (!desc)
        return -ENOMEM;

    data = desc->data;
    data[0] = 0x05;     /* bLength of BOS Header */
    data[2] = 0x16;     /* wTotalLength[0] */
    data[3] = 0x00;     /* wTotalLength[1] */
    data[4] = 0x02;     /* bNumDeviceCaps: number of separate device*/
    /* capability descriptors in BOS */

    /* BOS 1 */
    data[5] = 0x07;     /* bLength: 7 */
    data[6] = 0x10;     /* bDescriptorType: DEVICE CAPABILITY */
    data[7] = 0x02;     /* bDevCapabilityType: USB 2.0 Ext Descriptor */
    data[8] = 0x02;     /* bmAttributes[4]: LPM (SuperSpeed) */
    data[9] = 0x00;
    data[10] = 0x00;
    data[11] = 0x00;

    /* BOS 2 */
    data[12] = 0x0A;        /* bLength: 10 */
    data[13] = 0x10;        /* bDescriptorType: DEVICE CAPABILITY */
    data[14] = 0x03;        /* bDevCapabilityType: SuperSpeed */
    data[15] = 0x00;        /* bmAttributes: Don't support LTM */
    data[16] = 0x0E;        /* wSpeedsSupported[0]: b'1110 */
    data[17] = 0x00;        /* wSpeedsSupported[1] */
    data[18] = 0x01;        /* bFunctionalitySupport */
    data[19] = 0x0A;        /* bU1DevExitLat: Less than 10us */
    data[20] = 0x20;        /* wU2DevExitLat[0]: 32us */
    data[21] = 0x00;        /* wU2DevExitLat[1] */

    udc_descriptor_register(desc, DESC_TYPE_U3);
    return 0;
}

static int udc_create_config_desc(struct udc_gadget *gadget, int dtype)
{
    struct udc_descriptor *desc;
    unsigned char *data;
    unsigned int size;

    /* create our configuration descriptor */
    size = 9 + udc_ifc_desc_size(gadget, dtype);
    desc = udc_descriptor_alloc(CONFIGURATION, EP0, size);
    if (!desc)
        return -ENOMEM;

    data = desc->data;
    data[0] = 0x09;
    data[2] = size;
    data[3] = size >> 8;
    data[4] = 0x01;     /* number of interfaces */
    data[5] = 0x01;     /* configuration value */
    data[6] = 0x00;     /* configuration string */
    data[7] = 0x80;     /* attributes */
    /* max power u2: (250ma), u3: 400ma  */
    data[8] = (dtype == DESC_TYPE_U2) ? 0x80 : 0x32;

    udc_ifc_desc_fill(gadget, data + 9, dtype);
    udc_descriptor_register(desc, dtype);
    return 0;
}

static int udc_create_language_table_desc(int dtype)
{
    struct udc_descriptor *desc;

    /* create and register a language table descriptor */
    /* language 0x0409 is US English */
    desc = udc_descriptor_alloc(STRING, EP0, 4);
    if (!desc)
        return -ENOMEM;

    desc->data[2] = 0x09;
    desc->data[3] = 0x04;
    udc_descriptor_register(desc, dtype);
    return 0;
}

static int udc_create_descriptors(struct udc_device *udev, struct udc_gadget *gadget)
{
    udc_create_dev_desc(udev, DESC_TYPE_U2);
    udc_create_config_desc(gadget, DESC_TYPE_U2);
    udc_create_language_table_desc(DESC_TYPE_U2);

    if (g_u3d.is_u3_ip) {
        udc_create_dev_desc(udev, DESC_TYPE_U3);
        udc_create_u3_bos_desc();
        udc_create_config_desc(gadget, DESC_TYPE_U3);
        udc_create_language_table_desc(DESC_TYPE_U3);
    }

#if 0
    {
        struct udc_descriptor *desc;

        LTRACEF("%s: dump u2 desc_list\n", __func__);
        for (desc = g_u3d.desc_list; desc; desc = desc->next) {
            LTRACEF("tag: %04x\n", desc->tag);
            LTRACEF("len: %d\n", desc->len);
            LTRACEF("data:");
            hexdump8(desc->data, desc->len);
        }

        LTRACEF("%s: dump u3 desc_list\n", __func__);
        for (desc = g_u3d.desc_list_u3; desc; desc = desc->next) {
            LTRACEF("tag: %04x\n", desc->tag);
            LTRACEF("len: %d\n", desc->len);
            LTRACEF("data:");
            hexdump8(desc->data, desc->len);
        }
    }
#endif

    return 0;
}

/* hardware access APIs */

int wait_for_value(paddr_t addr, u32 msk, u32 value, int us_intvl, int count)
{
    int i;

    for (i = 0; i < count; i++) {
        if ((readl(addr) & msk) == value)
            return 0;

        spin(us_intvl);
    }

    return -ETIMEDOUT;
}

static inline void writel_rep(void *addr, const void *buffer,
                              unsigned int count)
{
    if (count) {
        const u32 *buf = buffer;

        do {
            writel(*buf++, addr);
        } while (--count);
    }
}

static inline void readl_rep(const void *addr, void *buffer,
                             unsigned int count)
{
    if (count) {
        u32 *buf = buffer;

        do {
            u32 x = readl(addr);
            *buf++ = x;
        } while (--count);
    }
}

static int pio_read_fifo(int ep_num, u8 *dst, u16 len)
{
    void *fifo = (void *)(paddr_t)(USB_FIFO(ep_num));
    u32 index = 0;
    u32 value;

    if (len >= 4) {
        readl_rep(fifo, dst, len >> 2);
        index = len & ~0x03;
    }
    if (len & 0x3) {
        value = readl(fifo);
        memcpy(&dst[index], &value, len & 0x3);
    }

    LTRACEF("%s - ep_num: %d, len: %d, dst: %p\n",
          __func__, ep_num, len, dst);

    return len;
}

static void pio_write_fifo(int ep_num, u8 *src, u16 len)
{
    void *fifo = (void *)(paddr_t)(USB_FIFO(ep_num));
    u32 index = 0;

    LTRACEF("%s - ep_num: %d, len: %d, src: %p\n",
          __func__, ep_num, len, src);

    if (len >= 4) {
        writel_rep(fifo, src, len >> 2);
        index = len & ~0x03;
    }
    if (len & 0x02) {
        writew(*(u16 *)&src[index], fifo);
        index += 2;
    }
    if (len & 0x01)
        writeb(src[index], fifo);
}

static int mu3d_check_clk_sts(void)
{
    u32 check_val;
    int ret = 0;

    check_val = SSUSB_SYS125_RST_B_STS | SSUSB_SYSPLL_STABLE |
                SSUSB_REF_RST_B_STS;
    if (g_u3d.is_u3_ip)
        check_val |= SSUSB_U3_MAC_RST_B_STS;

    ret = wait_for_value(U3D_SSUSB_IP_PW_STS1, check_val, check_val, 100, 100);
    if (ret) {
        DBG_C("SSUSB_SYS125_RST_B_STS NG\n");
        goto err;
    } else {
        LTRACEF("clk sys125:OK\n");
    }

    ret = wait_for_value(U3D_SSUSB_IP_PW_STS2, SSUSB_U2_MAC_SYS_RST_B_STS,
                         SSUSB_U2_MAC_SYS_RST_B_STS, 100, 100);
    if (ret) {
        DBG_C("SSUSB_U2_MAC_SYS_RST_B_STS NG\n");
        goto err;
    } else {
        LTRACEF("clk mac2:OK\n");
    }

    return 0;

err:
    DBG_C("Refer clocks stability check failed!\n");
    return ret;
}

static void mu3d_ssusb_enable(void)
{
    clrbits32(U3D_SSUSB_IP_PW_CTRL0, SSUSB_IP_SW_RST);
    clrbits32(U3D_SSUSB_IP_PW_CTRL2, SSUSB_IP_DEV_PDN);
    clrbits32(U3D_SSUSB_U2_CTRL_0P, (SSUSB_U2_PORT_DIS | SSUSB_U2_PORT_PDN |
                 SSUSB_U2_PORT_HOST_SEL));
    if (g_u3d.is_u3_ip)
        clrbits32(U3D_SSUSB_U3_CTRL_0P, (SSUSB_U3_PORT_DIS | SSUSB_U3_PORT_PDN |
                     SSUSB_U3_PORT_HOST_SEL));

    mu3d_check_clk_sts();
}

/* enable/disable U3D SS function */
static void mu3d_ss_func_set(bool enable)
{
    /* If usb3_en==0, LTSSM will go to SS.Disable state */
    if (enable)
        setbits32(U3D_USB3_CONFIG, USB3_EN);
    else
        clrbits32(U3D_USB3_CONFIG, USB3_EN);

    DBG_C("U3 pullup D%s\n", enable ? "+" : "-");
}

/* set/clear U3D HS device soft connect */
static void mu3d_hs_softconn_set(bool enable)
{
    if (enable)
        setbits32(U3D_POWER_MANAGEMENT, SOFT_CONN | SUSPENDM_ENABLE);
    else
        clrbits32(U3D_POWER_MANAGEMENT, SOFT_CONN | SUSPENDM_ENABLE);

    DBG_C("U2 pullup D%s\n", enable ? "+" : "-");
}

static void mu3d_soft_connect(void)
{
    if (g_u3d.is_u3_ip && g_u3d.speed > SSUSB_SPEED_HIGH)
        mu3d_ss_func_set(true);
    else
        mu3d_hs_softconn_set(true);
}

static void mu3d_soft_disconnect(void)
{
    if (g_u3d.is_u3_ip && g_u3d.speed > SSUSB_SPEED_HIGH)
        mu3d_ss_func_set(false);
    else
        mu3d_hs_softconn_set(false);
}

static void mu3d_dev_reset(void)
{
    setbits32(U3D_SSUSB_DEV_RST_CTRL, SSUSB_DEV_SW_RST);
    clrbits32(U3D_SSUSB_DEV_RST_CTRL, SSUSB_DEV_SW_RST);
}

static void mu3d_intr_enable(void)
{
    u32 value;

    /* enable LV1 ISR */
    value = BMU_INTR | QMU_INTR | MAC3_INTR | MAC2_INTR | EP_CTRL_INTR;
    writel(value, U3D_LV1IESR);
    /* enable U2 common interrupts */
    value = SUSPEND_INTR | RESUME_INTR | RESET_INTR;
    writel(value, U3D_COMMON_USB_INTR_ENABLE);

    /* Enable U3 LTSSM interrupts */
    if (g_u3d.is_u3_ip) {
        value = HOT_RST_INTR | WARM_RST_INTR | VBUS_RISE_INTR |
                VBUS_FALL_INTR | ENTER_U3_INTR | EXIT_U3_INTR;
        writel(value, U3D_LTSSM_INTR_ENABLE);
    }

    /* Enable QMU interrupts. */
    value = TXQ_CSERR_INT | TXQ_LENERR_INT | RXQ_CSERR_INT |
            RXQ_LENERR_INT | RXQ_ZLPERR_INT;
    writel(value, U3D_QIESR1);
    /* Enable speed change interrupt */
    writel(SSUSB_DEV_SPEED_CHG_INTR, U3D_DEV_LINK_INTR_ENABLE);
}

static void mu3d_intr_disable(void)
{
    writel(0xffffffff, U3D_EPISR);
    writel(0xffffffff, U3D_QISAR0);
    writel(0xffffffff, U3D_QISAR1);
    writel(0xffffffff, U3D_TQERRIR0);
    writel(0xffffffff, U3D_RQERRIR0);
    writel(0xffffffff, U3D_RQERRIR1);
    writel(0xffffffff, U3D_LV1IECR);
    writel(0xffffffff, U3D_EPIECR);

    /* clear registers */
    writel(0xffffffff, U3D_QIECR0);
    writel(0xffffffff, U3D_QIECR1);
    writel(0xffffffff, U3D_TQERRIECR0);
    writel(0xffffffff, U3D_RQERRIECR0);
    writel(0xffffffff, U3D_RQERRIECR1);
    writel(0xffffffff, U3D_COMMON_USB_INTR);
}

static void mu3d_reg_init(void)
{
    if (g_u3d.is_u3_ip) {
        /* disable LGO_U1/U2 by default */
        clrbits32(U3D_LINK_POWER_CONTROL,
                    SW_U1_ACCEPT_ENABLE | SW_U2_ACCEPT_ENABLE |
                    SW_U1_REQUEST_ENABLE | SW_U2_REQUEST_ENABLE);
        /* device responses to u3_exit from host automatically */
        clrbits32(U3D_LTSSM_CTRL, SOFT_U3_EXIT_EN);
        /* automatically build U2 link when U3 detect fail */
        setbits32(U3D_USB2_TEST_MODE, U2U3_AUTO_SWITCH);
    }
    /* delay about 0.1us from detecting reset to send chirp-K */
    clrbits32(U3D_LINK_RESET_INFO, WTCHRP_MSK);
    /* U2/U3 detected by HW */
    writel(0, U3D_DEVICE_CONF);
    /* enable QMU 16B checksum */
    setbits32(U3D_QCR0, QMU_CS16B_EN);
    /* force vbus on */
    setbits32(U3D_MISC_CTRL, VBUS_FRC_EN | VBUS_ON);

}

static enum USB_SPEED mu3d_get_speed(void)
{
    static const char * const spd_str[] = {"UNKNOWN", "FS", "HS", "SS", "SSP"};
    enum USB_SPEED spd;

    switch (SSUSB_DEV_SPEED(readl(U3D_DEVICE_CONF))) {
    case 1:
        spd = SSUSB_SPEED_FULL;
        break;
    case 3:
        spd = SSUSB_SPEED_HIGH;
        break;
    case 4:
        spd = SSUSB_SPEED_SUPER;
        break;
    case 5:
        spd = SSUSB_SPEED_SUPER_PLUS;
        break;
    default:
        spd = SSUSB_SPEED_UNKNOWN;
        break;
    }

    DBG_C("%s (%d) is detected\n", spd_str[spd % ARRAY_SIZE(spd_str)], spd);
    return spd;
}

/* SSP is not supported tmp. */
static void mu3d_set_speed(enum USB_SPEED spd)
{
    static const char * const spd_str[] = {"UNKNOWN", "FS", "HS", "SS", "SSP"};

    switch (spd) {
    case SSUSB_SPEED_FULL:
        clrbits32(U3D_USB3_CONFIG, USB3_EN);
        clrbits32(U3D_POWER_MANAGEMENT, HS_ENABLE);
        break;
    case SSUSB_SPEED_HIGH:
        clrbits32(U3D_USB3_CONFIG, USB3_EN);
        setbits32(U3D_POWER_MANAGEMENT, HS_ENABLE);
        break;
    case SSUSB_SPEED_SUPER:
        /* fall through */
    default:
        clrbits32(U3D_SSUSB_U3_CTRL_0P, SSUSB_U3_PORT_SSP_SPEED);
        break;
    }
    LTRACEF("%s %s (%d)\n", __func__, spd_str[spd % ARRAY_SIZE(spd_str)], spd);
}

static inline void mu3d_set_address(int addr)
{
    writel(DEV_ADDR(addr), U3D_DEVICE_CONF);
}

struct udc_endpoint *mtu3_find_ep(int ep_num, u8 dir)
{
    struct udc_endpoint *ep_list = g_u3d.eps;
    int i;
    u8 in;

    /* convert dir to in */
    if (dir == USB_DIR_IN)
        in = 1;
    else
        in = 0;

    for (i = 1; i < MT_EP_NUM; i++) {
        if ((ep_list[i].num == ep_num) && (ep_list[i].in == in))
            return &ep_list[i];
    }
    return NULL;
}

static void mu3d_flush_fifo(u8 ep_num, u8 dir)
{
    if (ep_num == 0) {
        setbits32(U3D_EP_RST, EP0_RST);
        clrbits32(U3D_EP_RST, EP0_RST);
    } else {
        setbits32(U3D_EP_RST, EP_RST((dir == USB_DIR_IN), ep_num));
        clrbits32(U3D_EP_RST, EP_RST((dir == USB_DIR_IN), ep_num));
    }
}

static void ep0_stall_set(bool set, u32 pktrdy)
{
    u32 csr;

    /* EP0_SENTSTALL is W1C */
    csr = readl(U3D_EP0CSR) & EP0_W1C_BITS;
    if (set)
        csr |= EP0_SENDSTALL | pktrdy;
    else
        csr = (csr & ~EP0_SENDSTALL) | EP0_SENTSTALL;
    writel(csr, U3D_EP0CSR);

    g_u3d.ep0_state = EP0_IDLE;
}

/*
 * Return value indicates the TxFIFO size of 2^n bytes, (ex: value 10 means 2^10 =
 * 1024 bytes.) TXFIFOSEGSIZE should be equal or bigger than 4. The TxFIFO size of
 * 2^n bytes also should be equal or bigger than TXMAXPKTSZ. This EndPoint occupy
 * total memory size  (TX_SLOT + 1 )*2^TXFIFOSEGSIZE bytes.
 */
static u8 get_seg_size(u32 maxp)
{
    /* Set fifo size(double buffering is currently not enabled) */
    switch (maxp) {
    case 8:
    case 16:
        return USB_FIFOSZ_SIZE_16;
    case 32:
        return USB_FIFOSZ_SIZE_32;
    case 64:
        return USB_FIFOSZ_SIZE_64;
    case 128:
        return USB_FIFOSZ_SIZE_128;
    case 256:
        return USB_FIFOSZ_SIZE_256;
    case 512:
        return USB_FIFOSZ_SIZE_512;
    case 1023:
    case 1024:
    case 2048:
    case 3072:
    case 4096:
        return USB_FIFOSZ_SIZE_1024;
    default:
        LTRACEF("The maxp %d is not supported\n", maxp);
        return USB_FIFOSZ_SIZE_512;
    }
}

static void mu3d_setup_ep(unsigned int ep_num, struct udc_endpoint *ept)
{
    u32 csr0, csr1, csr2;
    u32 fifo_addr;
    u8 seg_size;

    /* Nothing needs to be done for ep0 */
    if (ep_num == 0)
        return;

    /* In LK (FASTBOOT) will use BULK transfer only */
    if (ept->type != USB_EP_XFER_BULK)
        DBG_C("ep type is wrong, should be bulk,\n");

    /* Set fifo address, fifo size, and fifo max packet size */
    LTRACEF("%s: %s, maxpkt: %d\n", __func__, ept->name, ept->maxpkt);

    /* Set fifo size(only supports single buffering) */
    seg_size = get_seg_size(ept->maxpkt);

    if (ept->in) {  /* TX case */
        mu3d_flush_fifo(ep_num, USB_DIR_IN);

        csr0 = TX_TXMAXPKTSZ(ept->maxpkt);

        /* only support BULK, set 0 for burst, mult, num_pkts  */
        csr1 = TX_TYPE(TYPE_BULK);

#ifdef SUPPORT_QMU
        csr1 |= TX_SLOT(ept->slot);
#endif

#ifdef SUPPORT_QMU
        fifo_addr = g_u3d.tx_fifo_addr + (U3D_FIFO_SIZE_UNIT * (ept->slot + 1) * ep_num);
#else
        fifo_addr = g_u3d.tx_fifo_addr + (U3D_FIFO_SIZE_UNIT * ep_num);
#endif
        csr2 = TX_FIFOADDR(fifo_addr >> 4);
        csr2 |= TX_FIFOSEGSIZE(seg_size);

#ifdef SUPPORT_QMU
        csr0 |= TX_DMAREQEN;
        /* Enable QMU Done interrupt */
        setbits32(U3D_QIESR0, QMU_TX_DONE_INT(ep_num));
#else
        setbits32(U3D_EPIECR, EP_TXISR(ep_num));  /* W1C */
        setbits32(U3D_EPIESR, EP_TXISR(ep_num));  /* W1S */
#endif

        writel(csr0, MU3D_EP_TXCR0(ep_num));
        writel(csr1, MU3D_EP_TXCR1(ep_num));
        writel(csr2, MU3D_EP_TXCR2(ep_num));

    } else {    /* RX case */
        mu3d_flush_fifo(ep_num, USB_DIR_OUT);

        csr0 = RX_RXMAXPKTSZ(ept->maxpkt);

        /* only support BULK, set 0 for burst, slot, mult, num_pkts  */
        csr1 = RX_TYPE(TYPE_BULK);

#ifdef SUPPORT_QMU
        csr1 |= RX_SLOT(ept->slot);
#endif

#ifdef SUPPORT_QMU
        fifo_addr = g_u3d.rx_fifo_addr + (U3D_FIFO_SIZE_UNIT * (ept->slot + 1) * ep_num);
#else
        fifo_addr = g_u3d.rx_fifo_addr + (U3D_FIFO_SIZE_UNIT * ep_num);
#endif
        csr2 = RX_FIFOADDR(fifo_addr >> 4);
        csr2 |= RX_FIFOSEGSIZE(seg_size);

#ifdef SUPPORT_QMU
        csr0 |= RX_DMAREQEN;
        /* Enable QMU Done interrupt */
        setbits32(U3D_QIESR0, QMU_RX_DONE_INT(ep_num));
#else
        setbits32(U3D_EPIECR, EP_RXISR(ep_num));  /* W1C */
        /* enable it when queue RX request */
        /* setbits32(U3D_EPIESR, EP_RXISR(ep_num));*/   /* W1S */
#endif
        writel(csr0, MU3D_EP_RXCR0(ep_num));
        writel(csr1, MU3D_EP_RXCR1(ep_num));
        writel(csr2, MU3D_EP_RXCR2(ep_num));
    }

#ifdef SUPPORT_QMU
    mtu3_qmu_start(ept);
#endif
}

static void mu3d_ep0en(void)
{
    u32 temp = 0;
    struct udc_endpoint *ep0 = g_u3d.ep0;
    int ret = 0;

    ret = sprintf(ep0->name, "ep0");
    if (ret < 0)
        DBG_C("ep0->name: copy failed\n");

    ep0->type = USB_EP_XFER_CTRL;
    ep0->num = EP0;
    if (g_u3d.speed == SSUSB_SPEED_SUPER)
        ep0->maxpkt = EP0_MAX_PACKET_SIZE_U3;
    else
        ep0->maxpkt = EP0_MAX_PACKET_SIZE;

    temp = readl(U3D_EP0CSR);
    temp &= ~(EP0_MAXPKTSZ_MSK | EP0_AUTOCLEAR | EP0_AUTOSET | EP0_DMAREQEN);
    temp |= EP0_MAXPKTSZ(ep0->maxpkt);
    temp &= EP0_W1C_BITS;
    writel(temp, U3D_EP0CSR);

    /* enable EP0 interrupts */
    setbits32(U3D_EPIESR, EP_EP0ISR);
}

static void mu3d_get_ip_vers(void)
{
    u32 val;

    val = readl(U3D_SSUSB_IP_DEV_CAP);
    g_u3d.is_u3_ip = !!SSUSB_IP_DEV_U3_PORT_NUM(val);
    DBG_C("IP version 0x%x(%s IP)\n", readl(U3D_SSUSB_HW_ID),
          g_u3d.is_u3_ip ? "U3" : "U2");
}

static void mu3d_hw_init(void)
{
    mu3d_dev_reset();
    mu3d_ssusb_enable();
    mu3d_intr_disable();
    mu3d_reg_init();
    mu3d_set_speed(g_u3d.speed);
    mu3d_intr_enable();
    mu3d_ep0en();
}

static void mtu3_setup_eps(void)
{
    struct udc_endpoint *ep_list = g_u3d.eps;
    enum USB_SPEED speed = g_u3d.speed;
    int maxp = 512;
    int i;

    if (speed == SSUSB_SPEED_FULL)
        maxp = 64;
    else if (speed == SSUSB_SPEED_HIGH)
        maxp = 512;
    else if (speed == SSUSB_SPEED_SUPER)
        maxp = 1024;

    for (i = 1; i < MT_EP_NUM; i++) {
        if (ep_list[i].num != 0) { /* allocated */
            ep_list[i].maxpkt = maxp;
            mu3d_setup_ep(ep_list[i].num, &ep_list[i]);
        }
    }
}


/* usb generic functions */

void handle_ept_complete(struct udc_endpoint *ept, int status)
{
    struct udc_request *req;
    struct mu3d_req *mreq;
    unsigned int actual;

    req = ept->req;
    mreq = to_mu3d_req(req);
    if (req) {
        ept->req = NULL;

        if (status)
            DBG_C("%s: %s FAIL status: %d\n", __func__, ept->name, status);

        actual = status ? 0 : mreq->actual;

        LTRACEF("%s: %s, req: %p: complete: %d/%d: status: %d\n",
              __func__, ept->name, req, actual, req->length, status);

        /* clean chache because CPU may prefetch request buffer to cache */
        arch_clean_invalidate_cache_range((addr_t) req->buffer, req->length);

        if (req->complete)
            req->complete(req, actual, status);
    }
}

static int mtu3_read_fifo(struct udc_endpoint *ept)
{
    struct udc_request *req = ept->req;
    struct mu3d_req *mreq = to_mu3d_req(req);
    int ep_num = ept->num;
    u32 count = 0;

    if (mreq) {
        if (ep_num == 0)
            count = readl(U3D_RXCOUNT0);
        else
            count = EP_RX_COUNT(readl(MU3D_EP_RXCR3(ep_num)));

        count = MIN(req->length - mreq->actual, count);
        pio_read_fifo(ep_num, req->buffer + mreq->actual, count);
#if 0
        if (ep_num != 0) {
            LTRACEF("%s: &req->buffer: %p\n", __func__, req->buffer);
            LTRACEF("dump data:\n");
            hexdump8(req->buffer, len);
        }
#endif
        mreq->actual += count;

        LTRACEF("%s: ep%dout, mreq: %p, buf: %p, length: %d, actual: %d\n",
              __func__, ep_num, mreq, req->buffer, req->length, mreq->actual);
    }

    return count;
}

static int mtu3_write_fifo(struct udc_endpoint *ept)
{
    struct udc_request *req = ept->req;
    struct mu3d_req *mreq = to_mu3d_req(req);
    unsigned char *buf;
    int ep_num = ept->num;
    int count = 0;

    if (mreq) {
        LTRACEF("%s: ep%din mreq: %p, length: %d, actual: %d, maxp: %d\n",
              __func__, ep_num, mreq, req->length, mreq->actual, ept->maxpkt);

        count = MIN(req->length - mreq->actual, ept->maxpkt);
        buf = req->buffer + mreq->actual;
        pio_write_fifo(ep_num, buf, count);
        mreq->actual += count;
    }

    return count;
}

static void mtu3_ep0_write(void)
{
    struct udc_endpoint *ep0 = g_u3d.ep0;
    struct udc_request *req = ep0->req;
    struct mu3d_req *mreq = to_mu3d_req(req);
    unsigned int count = 0;
    u32 csr0;

    csr0 = readl(U3D_EP0CSR);
    if (csr0 & EP0_TXPKTRDY) {
        LTRACEF("%s: ep0 is not ready to be written\n", __func__);
        return;
    }

    count = mtu3_write_fifo(ep0);

    /* hardware limitiation: can't set (EP0_TXPKTRDY | EP0_DATAEND) at same time */
    csr0 |= (EP0_TXPKTRDY);
    writel(csr0, U3D_EP0CSR);

    LTRACEF("%s: length=%d, actual=%d\n", __func__, req->length, mreq->actual);
    if ((count < ep0->maxpkt) || (req->length == mreq->actual)) {
        /* last packet */
        mreq->actual = 0;
        g_u3d.ep0_state = EP0_TX_END;
    }
}

static void mtu3_ep0_read(void)
{
    struct udc_endpoint *ep0 = g_u3d.ep0;
    struct udc_request *req = ep0->req;
    struct mu3d_req *mreq = to_mu3d_req(req);
    unsigned int count = 0;
    u32 csr0 = 0;

    csr0 = readl(U3D_EP0CSR);

    /* erroneous ep0 interrupt */
    if (!(csr0 & EP0_RXPKTRDY))
        return;

    count = mtu3_read_fifo(ep0);

    /* work around: cannot set  (EP0_RXPKTRDY | EP0_DATAEND) at same time */
    csr0 |= (EP0_RXPKTRDY);
    writel(csr0, U3D_EP0CSR);

    if ((count < ep0->maxpkt) || (mreq->actual == req->length)) {
        /* last packet */
        csr0 |= EP0_DATAEND;
        g_u3d.ep0_state = EP0_IDLE;
    } else {
        /* more packets are waiting to be transferred */
        csr0 |= EP0_RXPKTRDY;
    }

    writel(csr0, U3D_EP0CSR);
}

static int std_get_descs(struct udc_request *req, struct usb_setup *setup)
{
    struct udc_descriptor *desc = NULL;

    if (g_u3d.speed <= SSUSB_SPEED_HIGH)
        desc = g_u3d.desc_list;
    else
        desc = g_u3d.desc_list_u3;

    for (; desc; desc = desc->next) {
        if (desc->tag == setup->value) {
            unsigned int len = desc->len;

            if (len > setup->length)
                len = setup->length;

            copy_desc(req, desc->data, len);
            change_eps_maxp(desc, req->buffer, len);
            return 0;
        }
    }
    /* descriptor lookup failed */
    return -EINVAL;
}

static int ep0_standard_setup(struct usb_setup *setup)
{
    struct udc_gadget *gadget = g_u3d.gadget;
    struct udc_request *req = g_u3d.ep0->req;
    u8 *cp = req->buffer;

    dump_setup_packet("Device Request\n", setup);

    if ((setup->request_type & USB_TYPE_MASK) != 0)
        return -EINVAL; /* Class-specific requests are handled elsewhere */

    /* handle all requests that return data (direction bit set on bm RequestType) */
    if ((setup->request_type & USB_EP_DIR_MASK)) {
        /* send the descriptor */
        g_u3d.ep0_state = EP0_TX;

        switch (setup->request) {
            /* data stage: from device to host */
        case GET_STATUS:
            LTRACEF("GET_STATUS\n");
            req->length = 2;
            cp[0] = cp[1] = 0;

            switch (setup->request_type & USB_RECIP_MASK) {
            case USB_RECIP_DEVICE:
                cp[0] = USB_STAT_SELFPOWERED;
                break;
            case USB_RECIP_OTHER:
                req->length = 0;
                break;
            default:
                break;
            }
            return 0;
        case GET_DESCRIPTOR:
            LTRACEF("GET_DESCRIPTOR\n");
            return std_get_descs(req, setup);
        case GET_CONFIGURATION:
            LTRACEF("GET_CONFIGURATION\n");
            break;
        case GET_INTERFACE:
            LTRACEF("GET_INTERFACE\n");
            break;
        default:
            DBG_C("Unsupported command with TX data stage\n");
            break;
        }
    } else {
        switch (setup->request) {
        case SET_ADDRESS:
            LTRACEF("SET_ADDRESS\n");
            g_u3d.address = (setup->value);
            mu3d_set_address(g_u3d.address);
            return 0;
        case SET_CONFIGURATION:
            LTRACEF("SET_CONFIGURATION\n");
            g_u3d.usb_online = setup->value ? 1 : 0;
            if (setup->value == 1) {
                mtu3_setup_eps();
                spin(50);
                gadget->notify(gadget, UDC_EVENT_ONLINE);
            } else {
                gadget->notify(gadget, UDC_EVENT_OFFLINE);
            }
            DBG_C("usb_online: %d\n", g_u3d.usb_online);
            return 0;
        default:
            LTRACEF("setup->request: %x, setup->value: %x\n",
                  setup->request, setup->value);
            DBG_C("Unsupported command with RX data stage\n");
            break;
        } /* switch request */
    }

    return -EINVAL;
}

static void mtu3_ep0_setup(void)
{
    struct usb_setup setup;
    int stall = -ENOTSUP;
    u32 csr0;
    u32 len;

    csr0 = readl(U3D_EP0CSR);
    if (!(csr0 & EP0_SETUPPKTRDY))
        return;

    len = readl(U3D_RXCOUNT0);
    if (len != 8) {
        DBG_C("SETUP packet len %d != 8?\n", len);
        return;
    }

    /* unload fifo */
    pio_read_fifo(EP0, (u8 *)&setup, len);

    /* decode command */
    if (((setup.request_type) & USB_TYPE_MASK) == USB_TYPE_STANDARD) {
        LTRACEF("Standard Request\n");
        stall = ep0_standard_setup(&setup);
    }

    /* command is not supported, inlcude  USB_TYPE_CLASS & USB_TYPE_VENDOR */
    if (stall) {
        dump_setup_packet("REQUEST NOT SUPPORTED\n", &setup);
        ep0_stall_set(true, EP0_SETUPPKTRDY);
        return;
    }

    /* handle EP0 state */
    switch (g_u3d.ep0_state) {
    case EP0_TX:
        LTRACEF("%s: EP0_TX\n", __func__);
        csr0 = readl(U3D_EP0CSR);
        csr0 |= (EP0_SETUPPKTRDY | EP0_DPHTX);
        writel(csr0, U3D_EP0CSR);

        mtu3_ep0_write();
        break;
    case EP0_RX:
        LTRACEF("%s: EP0_RX\n", __func__);
        csr0 = readl(U3D_EP0CSR);
        csr0 |= (EP0_SETUPPKTRDY);
        writel(csr0, U3D_EP0CSR);
        break;
    case EP0_IDLE:
        /* no data stage */
        LTRACEF("%s: EP0_IDLE\n", __func__);
        csr0 = readl(U3D_EP0CSR);
        csr0 |= (EP0_SETUPPKTRDY | EP0_DATAEND);
        writel(csr0, U3D_EP0CSR);
        break;
    default:
        break;
    }
}

static void mtu3_ep0_isr(void)
{
    u32 csr0;

    csr0 = readl(U3D_EP0CSR);

    if (csr0 & EP0_SENTSTALL) {
        LTRACEF("USB: [EP0] SENTSTALL\n");
        ep0_stall_set(false, 0);
        csr0 = readl(U3D_EP0CSR);
    }

    switch (g_u3d.ep0_state) {
    case EP0_IDLE:
        LTRACEF("%s: EP0_IDLE\n", __func__);
        mtu3_ep0_setup();
        break;
    case EP0_TX:
        LTRACEF("%s: EP0_TX\n", __func__);
        mtu3_ep0_write();
        break;
    case EP0_TX_END:
        LTRACEF("%s: EP0_TX_END\n", __func__);
        csr0 |= EP0_DATAEND;
        writel(csr0, U3D_EP0CSR);
        g_u3d.ep0_state = EP0_IDLE;
        break;
    case EP0_RX:
        LTRACEF("%s: EP0_RX\n", __func__);
        mtu3_ep0_read();
        g_u3d.ep0_state = EP0_IDLE;
        break;
    default:
        LTRACEF("[ERR]: Unrecognized ep0 state %d", g_u3d.ep0_state);
        break;
    }
}

#ifndef SUPPORT_QMU
/* PIO: TX packet */
static int mtu3_epx_write(struct udc_endpoint *ept)
{
    int ep_num = ept->num;
    int count;
    u32 csr;

    /* only for non-ep0 */
    if (ep_num == 0)
        return -EACCES;

    if (!ept->in)
        return -EINVAL;

    csr = readl(MU3D_EP_TXCR0(ep_num));
    if (csr & TX_TXPKTRDY) {
        LTRACEF("%s: ep%d is busy!\n", __func__, ep_num);
        return -EBUSY;
    }
    count = mtu3_write_fifo(ept);

    csr |= TX_TXPKTRDY;
    writel(csr, MU3D_EP_TXCR0(ep_num));

    return count;
}

static void mtu3_epx_isr(u8 ep_num, u8 dir)
{
    struct udc_endpoint *ept;
    struct mu3d_req *mreq;
    struct udc_request *req;
    u32 csr;
    u32 count;

    ept = mtu3_find_ep(ep_num, dir);
    if (!ept || !ept->req)
        return;

    LTRACEF("%s Interrupt\n", ept->name);
    req = ept->req;
    mreq = to_mu3d_req(req);

    if (dir == USB_DIR_IN) {
        csr = readl(MU3D_EP_TXCR0(ep_num));
        if (csr & TX_SENTSTALL) {
            DBG_C("EP%dIN: STALL\n", ep_num);
            handle_ept_complete(ept, -EPIPE);
            /* exception handling: implement this!! */
            return;
        }

        if (csr & TX_TXPKTRDY) {
            DBG_C("%s: EP%dIN is busy\n", __func__, ep_num);
            return;
        }

        if (req->length == mreq->actual) {
            handle_ept_complete(ept, 0);
            return;
        }

        count = mtu3_write_fifo(ept);
        if (count) {
            csr |= TX_TXPKTRDY;
            writel(csr, MU3D_EP_TXCR0(ep_num));
        }

        LTRACEF("EP%dIN, count=%d, %d/%d\n",
              ep_num, count, mreq->actual, req->length);

    } else {
        csr = readl(MU3D_EP_RXCR0(ep_num));
        if (csr & RX_SENTSTALL) {
            DBG_C("EP%dOUT: STALL\n", ep_num);
            /* exception handling: implement this!! */
            return;
        }

        if (!(csr & RX_RXPKTRDY)) {
            LTRACEF("EP%dOUT: ERRONEOUS INTERRUPT\n", ep_num);
            return;
        }

        count = mtu3_read_fifo(ept);

        LTRACEF("EP%dOUT, count = %d\n", ep_num, count);

        /* write 1 to clear RXPKTRDY */
        csr |= RX_RXPKTRDY;
        writel(csr, MU3D_EP_RXCR0(ep_num));

        if (readl(MU3D_EP_RXCR0(ep_num)) & RX_RXPKTRDY)
            LTRACEF("%s: rxpktrdy clear failed\n", __func__);

        if ((req->length == mreq->actual) || (count < ept->maxpkt)) {
            /* disable EP RX intr */
            setbits32(U3D_EPIECR, EP_RXISR(ep_num));  /* W1C */
            handle_ept_complete(ept, 0);
        }
    }
}
#endif

/* handle abnormal DATA transfer if we had any, like USB unplugged */
static void mtu3_suspend(void)
{
    struct udc_gadget *gadget = g_u3d.gadget;
    struct udc_endpoint *ep_list = g_u3d.eps;
    struct udc_endpoint *ept;
    int i;

    g_u3d.usb_online = 0;
    gadget->notify(gadget, UDC_EVENT_OFFLINE);

    /* error out any pending reqs, except ep0 */
    for (i = 1; i < MT_EP_NUM; i++) {
        ept = &ep_list[i];
        /* End operation when encounter uninitialized ept */
        if (ept->num == 0)
            break;

        LTRACEF("%s: %s, req: %p\n", __func__, ept->name, ept->req);

        mtu3_qmu_flush(ept);

        if (ept->req)
            handle_ept_complete(ept, -ESHUTDOWN);
    }
}

static void mtu3_status_reset(void)
{
    g_u3d.ep0_state = EP0_IDLE;
    g_u3d.address = 0;
}

static void mtu3_link_isr(void)
{
    u32 linkint;

    linkint = readl(U3D_DEV_LINK_INTR) & readl(U3D_DEV_LINK_INTR_ENABLE);
    writel(linkint, U3D_DEV_LINK_INTR);

    if (linkint & SSUSB_DEV_SPEED_CHG_INTR) {
        LTRACEF("[INTR] Speed Change\n");
        g_u3d.speed = mu3d_get_speed();
        if (g_u3d.speed == SSUSB_SPEED_UNKNOWN)
            mtu3_suspend();
        else
            mu3d_ep0en();
    }
}

static void mtu3_u2_common_isr(void)
{
    u32 intrusb = 0;

    intrusb = readl(U3D_COMMON_USB_INTR) & readl(U3D_COMMON_USB_INTR_ENABLE);
    writel(intrusb, U3D_COMMON_USB_INTR);

    if (intrusb & RESET_INTR) {
        LTRACEF("[INTR] Reset\n");
        mtu3_status_reset();
    }

    if (intrusb & SUSPEND_INTR) {
        LTRACEF("[INTR] Suspend\n");
        mtu3_suspend();
    }

    if (intrusb & RESUME_INTR)
        LTRACEF("[INTR] Resume\n");

}

static void mtu3_u3_ltssm_isr(void)
{
    u32 ltssm;

    ltssm = readl(U3D_LTSSM_INTR) & readl(U3D_LTSSM_INTR_ENABLE);
    writel(ltssm, U3D_LTSSM_INTR); /* W1C */
    LTRACEF("=== LTSSM[%x] ===\n", ltssm);

    if (ltssm & (HOT_RST_INTR | WARM_RST_INTR))
        mtu3_status_reset();

    if (ltssm & VBUS_FALL_INTR) {
        mu3d_ss_func_set(false);
        mtu3_status_reset();
    }

    if (ltssm & VBUS_RISE_INTR)
        mu3d_ss_func_set(true);

    if (ltssm & ENTER_U3_INTR)
        mtu3_suspend();

    if (ltssm & EXIT_U3_INTR)
        LTRACEF("[INTR] Resume\n");

}

static void mtu3_bmu_isr(void)
{
    u32 intrep;

    intrep = readl(U3D_EPISR) & readl(U3D_EPIER);
    writel(intrep, U3D_EPISR);
    LTRACEF("[INTR] BMU[tx:%x, rx:%x] IER: %x\n",
          intrep & 0xffff, intrep >> 16, readl(U3D_EPIER));

    /* For EP0 */
    if (intrep & 0x1) {
        mtu3_ep0_isr();
        intrep &= ~0x1;
    }

#ifndef SUPPORT_QMU
    if (intrep) {
        u32 ep_num;

        for (ep_num = 1; ep_num <= (MT_EP_NUM / 2); ep_num++) {
            if (intrep & EPT_RX(ep_num))
                mtu3_epx_isr(ep_num, USB_DIR_OUT);

            if (intrep & EPT_TX(ep_num))
                mtu3_epx_isr(ep_num, USB_DIR_IN);
        }
    }
#endif
}

static enum handler_return mtu3_isr(void *arg)
{
    u32 lv1_isr;

    lv1_isr = readl(U3D_LV1ISR);  /* LV1ISR is RU */
    lv1_isr &= readl(U3D_LV1IER);
    LTRACEF("[INTR] lv1_isr:0x%x\n", lv1_isr);

    if (lv1_isr & EP_CTRL_INTR)
        mtu3_link_isr();

    if (lv1_isr & MAC2_INTR)
        mtu3_u2_common_isr();

    if (lv1_isr & MAC3_INTR)
        mtu3_u3_ltssm_isr();

    if (lv1_isr & BMU_INTR)
        mtu3_bmu_isr();

    if (lv1_isr & QMU_INTR)
        mtu3_qmu_isr();

    return INT_RESCHEDULE;
}

static void mu3d_isr_init(void)
{
    mt_irq_set_sens(MT_SSUSB_IRQ_ID, GIC_LEVEL_SENSITIVE);
    mt_irq_set_polarity(MT_SSUSB_IRQ_ID, GIC_LOW_POLARITY);
    register_int_handler(MT_SSUSB_IRQ_ID, mtu3_isr, NULL);
    unmask_interrupt(MT_SSUSB_IRQ_ID);
}

/* gadget common APIs */

static int g_u3d_init(void)
{
    struct mu3d_req *mreq = &g_u3d.ep0_mreq;
    struct udc_request *req = &mreq->req;

    mu3d_get_ip_vers();
    g_u3d.ep0_state = EP0_IDLE;
    g_u3d.address = 0;
    if (g_u3d.is_u3_ip) {
        g_u3d.speed = U3D_U3IP_DFT_SPEED;
        g_u3d.tx_fifo_addr = U3IP_TX_FIFO_START_ADDR;
        g_u3d.rx_fifo_addr = U3IP_RX_FIFO_START_ADDR;
    } else {
        g_u3d.speed = U3D_U2IP_DFT_SPEED;
        g_u3d.tx_fifo_addr = U2IP_TX_FIFO_START_ADDR;
        g_u3d.rx_fifo_addr = U2IP_RX_FIFO_START_ADDR;
    }

    g_u3d.ep0 = &g_u3d.eps[EP0];

    req->buffer = udc_zalloc(512);
    if (!req->buffer)
        return -ENOMEM;

    g_u3d.ep0->req = req;
    g_u3d.ept_alloc_table = EPT_TX(0) | EPT_RX(0);
    g_u3d.desc_list = NULL;
    g_u3d.next_string_id = 1;
    g_u3d.next_string_id_u3 = 1;

    return mtu3_qmu_init();
}

int udc_init(struct udc_device *dev)
{
    g_u3d_init();
    g_u3d.udev = dev;
    return 0;
}

static struct udc_endpoint *_udc_endpoint_alloc(unsigned char num,
        unsigned char in, unsigned short max_pkt)
{
    struct udc_endpoint *ep_list = g_u3d.eps;
    struct udc_endpoint *ept;
    int ret;
    int i;

    /* allocated and enabled by default */
    if (num == EP0)
        return NULL;

    /*
     * find an unused slot in ep_list from EP1 to MAX_EP
     * for example, EP1 will use 2 eps, one for IN and the other for OUT
     */
    for (i = 1; i < MT_EP_NUM; i++) {
        if (ep_list[i].num == 0) /* usable */
            break;
    }
    if (i == MT_EP_NUM) /* ep has been exhausted. */
        return NULL;

    ept = &ep_list[i];
    ret = sprintf(ept->name, "ep%d%s", num, in ? "in" : "out");
    if (ret < 0)
        DBG_C("ept->name: copy failed\n");

    ret = mtu3_gpd_ring_alloc(ept);
    if (ret) {
        DBG_C("%s gpd alloc failed\n", ept->name);
        return NULL;
    }

    /* only supports BULK */
    ept->type = USB_EP_XFER_BULK;
    ept->maxpkt = max_pkt;
    ept->num = num;
    ept->in = in;
    ept->req = NULL;

#ifdef SUPPORT_QMU
    /* USB burst mode: 1: enable, 0: disable */
    ept->slot = 1;
#endif

    /* store EPT_TX/RX info */
    if (ept->in)
        ept->bit = EPT_TX(num);
    else
        ept->bit = EPT_RX(num);

    /* write parameters to this ep (write to hardware) when SET_CONFIG */

    LTRACEF("%s @%p/%p max=%d bit=%x\n", ept->name,
          ept, &ep_list, max_pkt, ept->bit);

    return &ep_list[i];
}

struct udc_endpoint *udc_endpoint_alloc(unsigned int type, unsigned int maxpkt)
{
    struct udc_endpoint *ept;
    unsigned int n;
    unsigned int in;

    LTRACEF("%s\n", __func__);

    if (type == UDC_BULK_IN)
        in = 1;
    else if (type == UDC_BULK_OUT)
        in = 0;
    else
        return NULL;

    /* udc_endpoint_alloc is used for EPx except EP0 */
    for (n = 1; n < MT_EP_NUM; n++) {
        unsigned int bit = in ? EPT_TX(n) : EPT_RX(n);

        if (g_u3d.ept_alloc_table & bit)
            continue;

        ept = _udc_endpoint_alloc(n, in, maxpkt);
        if (ept) {
            g_u3d.ept_alloc_table |= bit;
            return ept;
        }
    }

    return NULL;
}

void udc_endpoint_free(struct udc_endpoint *ept)
{
    if (ept->num)
        mtu3_gpd_ring_free(ept);
}

struct udc_request *udc_request_alloc(void)
{
    struct mu3d_req *mreq;

    mreq = udc_zalloc(sizeof(*mreq));

    return mreq ? &mreq->req : NULL;
}

void udc_request_free(struct udc_request *req)
{
    free(req);
}

int udc_request_queue(struct udc_endpoint *ept, struct udc_request *req)
{
    struct mu3d_req *mreq = to_mu3d_req(req);
    int ret = 0;

    LTRACEF("%s: %s, req=%p, buf: %p, length=%d\n", __func__,
          ept->name, req, req->buffer, req->length);

    if (!g_u3d.usb_online)
        return -ENXIO;

    mask_interrupt(MT_SSUSB_IRQ_ID);
    ept->req = req;
    mreq->ept = ept;
    mreq->actual = 0;

#ifdef SUPPORT_QMU
    if (req->length > GPD_BUF_SIZE) {
        DBG_C("req length > supported MAX:%d requested:%d\n",
              GPD_BUF_SIZE, req->length);
        ret = -EOPNOTSUPP;
        goto out;
    }

    if (mtu3_prepare_transfer(ept)) {
        ret = -EAGAIN;
        goto out;
    }

    arch_clean_invalidate_cache_range((addr_t)req->buffer, req->length);
    mtu3_insert_gpd(ept, mreq);
    mtu3_qmu_resume(ept);
#else

    /*
     * PIO mode:
     * when class driver shares a buffer to TX and RX data,
     * mtu3 sends a data to host, then host sends a data back immediately,
     * cause EP TX and RX interrupts arise at the same time,
     * but the buffer is using by the TX, so no buffer for RX to receive data.
     * To fix the issue:
     * disable EP RX interrupt by default, enable it when queue RX
     * request and disable it again when complete the request.
     */
    if (ept->in)
        mtu3_epx_write(ept);
    else
        setbits32(U3D_EPIESR, EP_RXISR(ept->num));    /* W1S */
#endif

out:
    unmask_interrupt(MT_SSUSB_IRQ_ID);

    return ret;
}

int udc_register_gadget(struct udc_gadget *gadget)
{
    if (g_u3d.gadget) {
        DBG_C("only one gadget supported\n");
        return -EBUSY;
    }
    g_u3d.gadget = gadget;

    return 0;
}

int udc_start(void)
{
    struct udc_device *udev = g_u3d.udev;
    struct udc_gadget *gadget = g_u3d.gadget;

    if (!udev) {
        DBG_C("udc cannot start before init\n");
        return -ENODEV;
    }
    if (!gadget) {
        DBG_C("udc has no gadget registered\n");
        return -ENXIO;
    }
    LTRACEF("%s\n", __func__);

    udc_create_descriptors(udev, gadget);
    mt_usb_phy_poweron();
    mu3d_hw_init();
    mu3d_isr_init();
    mu3d_soft_connect();

    return 0;
}

int udc_stop(void)
{
    mask_interrupt(MT_SSUSB_IRQ_ID);
    mu3d_soft_disconnect();
    mt_usb_phy_poweroff();

    return 0;
}
