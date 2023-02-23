/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */
#include <debug.h>
#include <err.h>
#include <lib/kcmdline.h>
#include <lk/init.h>
#include <platform.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sysenv.h>
#include <trace.h>

#define LOCAL_TRACE 0

unsigned int get_usb2jtag(void)
{
    unsigned int status;

    status = (get_env("usb2jtag") == NULL) ? 0 : atoi(get_env("usb2jtag"));
    LTRACEF_LEVEL(ALWAYS, "[USB2JTAG] current setting is %d.\n", status);

    return status;

}

unsigned int set_usb2jtag(unsigned int en)
{
    const char *USB2JTAG[2] = {"0", "1"};
    unsigned int ret = 0;

    LTRACEF_LEVEL(ALWAYS, "[USB2JTAG] current setting is %d.\n",
                    (get_env("usb2jtag") == NULL) ? 0 : atoi(get_env("usb2jtag")));
    if (set_env("usb2jtag", USB2JTAG[en]) == 0)
        LTRACEF_LEVEL(ALWAYS, "[USB2JTAG]set USB2JTAG %s success.\n", USB2JTAG[en]);
    else {
        LTRACEF_LEVEL(ALWAYS, "[USB2JTAG]set USB2JTAG %s fail.\n", USB2JTAG[en]);
        ret = 1;
    }

    return ret;
}

static void usb2jtag_init(uint level)
{
    if (get_usb2jtag() == 1)
        kcmdline_append("usb2jtag_mode=1");
    else
        kcmdline_append("usb2jtag_mode=0");

    return;
}
LK_INIT_HOOK(init_usb2jtag, usb2jtag_init, LK_INIT_LEVEL_APPS - 1);
