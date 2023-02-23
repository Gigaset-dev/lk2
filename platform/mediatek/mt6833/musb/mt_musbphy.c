/*
 * Copyright (c) 2022 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#include <arch/ops.h>
#include <debug.h>
#include <errno.h>
#include <platform.h>
#include <platform/reg.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <trace.h>

/* common */
#include "mt_musb.h"

#ifdef PROJECT_TYPE_FPGA
#include <platform/mt_i2c.h>
#define CFG_FPGA_PLATFORM               (1)
#else
#define DBG_PHY_CALIBRATION             1
#endif

/* usb phy bring up */
#if CFG_FPGA_PLATFORM
#define USB_I2C_ID                      I2C3    /* 0 - 6 */
#define PATH_NORMAL                     0
#define PATH_PMIC                       1

#define USBPHY_I2C_READ8(addr, buffer)  usb_i2c_read8(addr, buffer)
#define USBPHY_I2C_WRITE8(addr, value)  usb_i2c_write8(addr, value)

U32 usb_i2c_read8(U8 addr, U8 *dataBuffer)
{
    U32 ret_code = I2C_OK;
    U16 len;

    static struct mt_i2c_t i2c;

    i2c.id = USB_I2C_ID;
    i2c.addr = 0x60;    /* i2c API will shift-left this value to 0xc0 */
    i2c.mode = ST_MODE;
    i2c.speed = 100;
    i2c.dma_en = 0;
    len = 1;

    ret_code = i2c_write_read(&i2c, dataBuffer, len, len);

    return ret_code;
}

U32 usb_i2c_write8(U8 addr, U8 value)
{
    U32 ret_code = I2C_OK;
    U8 write_data[2];
    U16 len;

    static struct mt_i2c_t i2c;

    write_data[0] = addr;
    write_data[1] = value;

    i2c.id = USB_I2C_ID;
    i2c.addr = 0x60;    /* i2c API will shift-left this value to 0xc0 */
    i2c.mode = ST_MODE;
    i2c.speed = 100;
    i2c.dma_en = 0;
    len = 2;

    ret_code = i2c_write(&i2c, write_data, len);

    return ret_code;
}
#endif

/* use mt_typedefs.h */
#define USBPHY_READ8(offset)            readb(USB20_PHY_BASE+offset)
#define USBPHY_WRITE8(offset, value)    writeb(value, USB20_PHY_BASE+offset)

#define USBPHY_SET8(offset, mask)       USBPHY_WRITE8(offset, \
                                                      (USBPHY_READ8(offset)) | (mask))
#define USBPHY_CLR8(offset, mask)       USBPHY_WRITE8(offset, \
                                                      (USBPHY_READ8(offset)) & (~mask))

#define DRV_USB_Reg32(addr)             (*(u32 *)(addr))
#define DRV_USB_WriteReg32(addr, data)  ((*(u32 *)(addr)) = (u32)data)

#define USBPHYREAD32(offset)            DRV_USB_Reg32(USB20_PHY_BASE+offset)
#define USBPHYWRITE32(offset, value)    DRV_USB_WriteReg32(USB20_PHY_BASE+offset, value)

#define USB11PHY_READ8(offset)          readb(USB11_PHY_BASE+offset)
#define USB11PHY_WRITE8(offset, value)  writeb(value, USB11_PHY_BASE+offset)

#define USB11PHY_SET8(offset, mask)     USB11PHY_WRITE8(offset, \
                                                        (USB11PHY_READ8(offset)) | (mask))
#define USB11PHY_CLR8(offset, mask)     USB11PHY_WRITE8(offset, \
                                                        (USB11PHY_READ8(offset)) & (~mask))

#define USBPHY_SET32(offset, mask)      USBPHYWRITE32(offset, \
                                                        USBPHYREAD32(offset) | mask)
#define USBPHY_CLR32(offset, mask)      USBPHYWRITE32(offset, \
                                                        USBPHYREAD32(offset) & ~mask)


#if CFG_FPGA_PLATFORM
void mt_usb_phy_poweron(void)
{
#define PHY_DRIVING   0x3

#if defined(USB_PHY_DRIVING_TUNING)
    UINT8 usbreg8;
    unsigned int i;
#endif

    /* force_suspendm = 0 */
    USBPHY_CLR8(0x6a, 0x04);

    USBPHY_I2C_WRITE8(0xff, 0x00);
    USBPHY_I2C_WRITE8(0x61, 0x04);
    USBPHY_I2C_WRITE8(0x68, 0x00);
    USBPHY_I2C_WRITE8(0x6a, 0x00);
    USBPHY_I2C_WRITE8(0x00, 0x6e);
    USBPHY_I2C_WRITE8(0x1b, 0x0c);
    USBPHY_I2C_WRITE8(0x08, 0x44);
    USBPHY_I2C_WRITE8(0x11, 0x55);
    USBPHY_I2C_WRITE8(0x1a, 0x68);

#if defined(USB_PHY_DRIVING_TUNING)
    /* driving tuning */
    USBPHY_I2C_READ8(0xab, &usbreg8);
    usbreg8 &= ~0x3;
    usbreg8 |= PHY_DRIVING;
    USBPHY_I2C_WRITE8(0xab, usbreg8);

    for (i = 0; i < 16; i++) {
        USBPHY_I2C_READ8((0x92 + i), &usbreg8);
        usbreg8 &= ~0x3;
        usbreg8 |= PHY_DRIVING;
        USBPHY_I2C_WRITE8((0x92 + i), usbreg8);
    }

    USBPHY_I2C_READ8(0xbc, &usbreg8);
    usbreg8 &= ~0x3;
    usbreg8 |= PHY_DRIVING;
    USBPHY_I2C_WRITE8(0xbc, usbreg8);

    USBPHY_I2C_READ8(0xbe, &usbreg8);
    usbreg8 &= ~0x3;
    usbreg8 |= PHY_DRIVING;
    USBPHY_I2C_WRITE8(0xbe, usbreg8);

    USBPHY_I2C_READ8(0xbf, &usbreg8);
    usbreg8 &= ~0x3;
    usbreg8 |= PHY_DRIVING;
    USBPHY_I2C_WRITE8(0xbf, usbreg8);

    USBPHY_I2C_READ8(0xcd, &usbreg8);
    usbreg8 &= ~0x3;
    usbreg8 |= PHY_DRIVING;
    USBPHY_I2C_WRITE8(0xcd, usbreg8);

    USBPHY_I2C_READ8(0xf1, &usbreg8);
    usbreg8 &= ~0x3;
    usbreg8 |= PHY_DRIVING;
    USBPHY_I2C_WRITE8(0xf1, usbreg8);

    USBPHY_I2C_READ8(0xa7, &usbreg8);
    usbreg8 &= ~0x3;
    usbreg8 |= PHY_DRIVING;
    USBPHY_I2C_WRITE8(0xa7, usbreg8);

    USBPHY_I2C_READ8(0xa8, &usbreg8);
    usbreg8 &= ~0x3;
    usbreg8 |= PHY_DRIVING;
    USBPHY_I2C_WRITE8(0xa8, usbreg8);
#endif

    spin(800);
}

void mt_usb_phy_savecurrent(void)
{
    /* no need */
}

void mt_usb_phy_recover(void)
{
    /* no need */
}

void mt_usb11_phy_savecurrent(void)
{
    /* no need */
}

void Charger_Detect_Init(void)
{
    /* no need */
}

void Charger_Detect_Release(void)
{
    /* no need */
}

#else
void mt_usb_phy_poweron(void)
{
    dprintf(INFO, "[USB] %s:\n", __func__);
    /*
     * force_uart_en            1'b0    0x68 26
     * RG_UART_EN               1'b0    0x6c 16
     * rg_usb20_gpio_ctl        1'b0    0x20 09
     * usb20_gpio_mode          1'b0    0x20 08
     * RG_USB20_BC11_SW_EN      1'b0    0x18 23
     * rg_usb20_dp_100k_mode    1'b1    0x20 18
     * USB20_DP_100K_EN         1'b0    0x20 16
     * RG_USB20_DM_100K_EN      1'b0    0x20 17
     * RG_USB20_OTG_VBUSCMP_EN  1'b1    0x18 20
     * force_suspendm           1'b0    0x68 18
     */

    USBPHY_CLR32(0x68, (0x1 << 26));
    USBPHY_CLR32(0x6c, (0x1 << 16));
    USBPHY_CLR32(0x20, ((0x1 << 9) | (0x1 << 8)));
    USBPHY_CLR32(0x18, (0x1 << 23));
    USBPHY_SET32(0x20, (0x1 << 18));
    USBPHY_CLR32(0x20, ((0x1 << 16) | (0x1 << 17)));
    USBPHY_SET32(0x18, (0x1 << 20));
    USBPHY_CLR32(0x68, (0x1 << 18));
    spin(800);
}

void mt_usb_phy_savecurrent(void)
{
    dprintf(INFO, "[USB] %s:\n", __func__);
    /*
     * force_uart_en            1'b0        0x68 26
     * RG_UART_EN               1'b0        0x6c 16
     * rg_usb20_gpio_ctl        1'b0        0x20 09
     * usb20_gpio_mode          1'b0        0x20 08
     * RG_USB20_BC11_SW_EN      1'b0        0x18 23
     * RG_USB20_OTG_VBUSCMP_EN  1'b0        0x18 20
     * RG_SUSPENDM              1'b1        0x68 03
     * force_suspendm           1'b1        0x68 18
     * RG_DPPULLDOWN            1'b1        0x68 06
     * RG_DMPULLDOWN            1'b1        0x68 07
     * RG_XCVRSEL[1:0]          2'b01       0x68 [04-05]
     * RG_TERMSEL               1'b1        0x68 02
     * RG_DATAIN[3:0]           4'b0000     0x68 [10-13]
     * force_dp_pulldown        1'b1        0x68 20
     * force_dm_pulldown        1'b1        0x68 21
     * force_xcversel           1'b1        0x68 19
     * force_termsel            1'b1        0x68 17
     * force_datain             1'b1        0x68 23
     * RG_SUSPENDM              1'b0        0x68 03
     */

    USBPHY_CLR32(0x68, (0x1 << 26));
    USBPHY_CLR32(0x6c, (0x1 << 16));
    USBPHY_CLR32(0x20, (0x1 << 9));
    USBPHY_CLR32(0x20, (0x1 << 8));
    USBPHY_CLR32(0x18, (0x1 << 23));
    USBPHY_CLR32(0x18, (0x1 << 20));
    USBPHY_SET32(0x68, (0x1 << 3));
    USBPHY_SET32(0x68, (0x1 << 18));
    USBPHY_SET32(0x68, ((0x1 << 6) | (0x1 << 7)));
    USBPHY_CLR32(0x68, (0x3 << 4));
    USBPHY_SET32(0x68, (0x1 << 4));
    USBPHY_SET32(0x68, (0x1 << 2));
    USBPHY_CLR32(0x68, (0xF << 10));

    USBPHY_SET32(0x68, ((0x1 << 20) |
                        (0x1 << 21) |
                        (0x1 << 19) |
                        (0x1 << 17) |
                        (0x1 << 23)));

    spin(800);
    USBPHY_CLR32(0x68, (0x1 << 3));
    spin(1);
    return;
}

void mt_usb_phy_recover(void)
{

    dprintf(INFO, "[USB] %s:\n", __func__);
    /*
     * 04.force_uart_en 1'b0 0x68 26
     * 04.RG_UART_EN                1'b0    0x6C 16
     * 04.rg_usb20_gpio_ctl         1'b0    0x20 09
     * 04.usb20_gpio_mode           1'b0    0x20 08
     * 05.force_suspendm            1'b0    0x68 18
     * 06.RG_DPPULLDOWN             1'b0    0x68 06
     * 07.RG_DMPULLDOWN             1'b0    0x68 07
     * 08.RG_XCVRSEL[1:0]           2'b00   0x68 [04:05]
     * 09.RG_TERMSEL                1'b0    0x68 02
     * 10.RG_DATAIN[3:0]            4'b0000 0x68 [10:13]
     * 11.force_dp_pulldown         1'b0    0x68 20
     * 12.force_dm_pulldown         1'b0    0x68 21
     * 13.force_xcversel            1'b0    0x68 19
     * 14.force_termsel             1'b0    0x68 17
     * 15.force_datain              1'b0    0x68 23
     * 16.RG_USB20_BC11_SW_EN       1'b0    0x18 23
     * 17.RG_USB20_OTG_VBUSCMP_EN   1'b1    0x18 20
     */

    USBPHY_CLR32(0x1c, (0x1 << 12));
    USBPHY_CLR32(0x68, (0x1 << 26));
    USBPHY_CLR32(0x6C, (0x1 << 16));
    USBPHY_CLR32(0x20, (0x1 << 9));
    USBPHY_CLR32(0x20, (0x1 << 8));
    USBPHY_CLR32(0x68, (0x1 << 18));
    USBPHY_CLR32(0x68, ((0x1 << 6) | (0x1 << 7)));
    USBPHY_CLR32(0x68, (0x3 << 4));
    USBPHY_CLR32(0x68, (0x1 << 2));
    USBPHY_CLR32(0x68, (0xF << 10));

    USBPHY_CLR32(0x68, ((0x1 << 20) |
                        (0x1 << 21) |
                        (0x1 << 19) |
                        (0x1 << 17) |
                        (0x1 << 23)));

    USBPHY_CLR32(0x18, (0x1 << 23));
    USBPHY_SET32(0x18, (0x1 << 20));

    spin(800);

    USBPHY_CLR32(0x6C, (0x10 << 0));
    USBPHY_SET32(0x6C, (0x2F << 0));
    USBPHY_SET32(0x6C, (0x3F << 8));
}

void mt_usb11_phy_savecurrent(void)
{
    /* no need */
}

void Charger_Detect_Init(void)
{
    /* RG_USB20_BC11_SW_EN = 1'b1 */
    USBPHY_SET8(0x1a, 0x80);
}

void Charger_Detect_Release(void)
{
    /* RG_USB20_BC11_SW_EN = 1'b0 */
    USBPHY_CLR8(0x1a, 0x80);
}
#endif
/* usb phy bring up end */

#if !CFG_FPGA_PLATFORM
void mt_usb_phy_calibraion(int case_set, int input_reg)
{
    /* no need */
}
#endif
