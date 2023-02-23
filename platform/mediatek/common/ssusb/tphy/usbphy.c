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

#include <debug.h>
#include <platform/addressmap.h>
#include <platform/reg.h>
#include <trace.h>
#include "usbphy.h"

#define LOCAL_TRACE 0

/* 2712E1 can't set RG_AVALID */
#define U3D_U2PHYDEV_MASK   (E60802_RG_IDDIG | /*E60802_RG_AVALID |*/ \
    E60802_RG_BVALID | E60802_RG_VBUSVALID)

#define U3D_U2PHYFRCDEV_MASK (E60802_FORCE_IDDIG | /*E60802_FORCE_AVALID |*/ \
    E60802_FORCE_BVALID | E60802_FORCE_SESSEND | E60802_FORCE_VBUSVALID)

#define U3D_U2PHY_UTMI_MASK (E60802_RG_DPPULLDOWN | E60802_RG_DMPULLDOWN |  \
    E60802_RG_XCVRSEL | E60802_RG_TERMSEL | E60802_RG_DATAIN)

#define U3D_U2PHY_FRCUTMI_MASK (E60802_FORCE_SUSPENDM | \
    E60802_FORCE_DP_PULLDOWN | E60802_FORCE_DM_PULLDOWN | \
    E60802_FORCE_XCVRSEL | E60802_FORCE_TERMSEL | E60802_FORCE_DATAIN)

void mt_usb_phy_poweron(void)
{
    LTRACEF("%s\n", __func__);

    /* switch to USB function */
    clrbits32(U3D_U2PHYDTM0, E60802_FORCE_UART_EN);
    clrbits32(U3D_U2PHYDTM1, E60802_RG_UART_EN);
    clrbits32(U3D_U2PHYACR4, E60802_RG_USB20_GPIO_CTL);
    clrbits32(U3D_U2PHYACR4, E60802_USB20_GPIO_MODE);
    /* DP/DM BC1.1 path Disable */
    clrbits32(U3D_USBPHYACR6, E60802_RG_USB20_BC11_SW_EN);
    /* Internal R bias enable */
    setbits32(U3D_USBPHYACR0, E60802_RG_USB20_INTR_EN);
    /* 100U from u2 */
    clrbits32(U3D_USBPHYACR5, E60802_RG_USB20_HS_100U_U3_EN);
    /* let suspendm=1, enable usb 480MHz pll */
    setbits32(U3D_U2PHYDTM0, E60802_RG_SUSPENDM);
    /* force_suspendm=1 */
    setbits32(U3D_U2PHYDTM0, E60802_FORCE_SUSPENDM);
    /* wait 2 ms for USBPLL stable */
    spin(2000);
    /* power on device mode */
    clrbits32(U3D_U2PHYDTM1, E60802_RG_SESSEND);
    /* NOTE: mt2712E1 can't set RG_AVALID */
    setbits32(U3D_U2PHYDTM1, U3D_U2PHYDEV_MASK);
    /* enable force into device mode */
    setbits32(U3D_U2PHYDTM1, U3D_U2PHYFRCDEV_MASK);
    /* clear force on UTMI */
    clrbits32(U3D_U2PHYDTM0, U3D_U2PHY_UTMI_MASK);
    clrbits32(U3D_U2PHYDTM0, U3D_U2PHY_FRCUTMI_MASK);

    /* wait mac ready */
    spin(2000);
    /* apply MAC clock related setting after phy init */
}

void mt_usb_phy_poweroff(void)
{
    /* power down device mode */
    clrbits32(U3D_U2PHYDTM1, E60802_RG_VBUSVALID |
                            E60802_RG_BVALID | E60802_RG_AVALID);
    setbits32(U3D_U2PHYDTM1, E60802_RG_IDDIG | E60802_RG_SESSEND);

    /* cleaer device force mode */
    clrbits32(U3D_U2PHYDTM1, U3D_U2PHYFRCDEV_MASK);

    clrbits32(U3D_U2PHYDTM0, E60802_RG_SUSPENDM);
    setbits32(U3D_U2PHYDTM0, E60802_FORCE_SUSPENDM);
    spin(2000);
    LTRACEF("%s\n", __func__);
}
