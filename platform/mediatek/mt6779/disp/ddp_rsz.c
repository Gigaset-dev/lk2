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

#define LOG_TAG "RSZ"

#include "ddp_hal.h"
#include "ddp_reg.h"
#include "ddp_info.h"
#include "ddp_log.h"

int RSZClockOn(DISP_MODULE_ENUM module, void *handle)
{
    ddp_enable_module_clock(module);
    DDPMSG("%s CG 0x%x\n", __func__,
           DISP_REG_GET(DISP_REG_CONFIG_MMSYS_CG_CON1));

    return 0;
}

int RSZClockOff(DISP_MODULE_ENUM module, void *handle)
{
    DDPMSG("%s\n", __func__);
    ddp_disable_module_clock(module);

    return 0;
}

static int RSZInit(DISP_MODULE_ENUM module, void *handle)
{
    ddp_enable_module_clock(module);
    DDPMSG("%s CG 0x%x\n", __func__,
           DISP_REG_GET(DISP_REG_CONFIG_MMSYS_CG_CON1));
    return 0;
}

static int RSZDeInit(DISP_MODULE_ENUM module, void *handle)
{
    DDPMSG("%s\n", __func__);
    ddp_disable_module_clock(module);
    return 0;
}

DDP_MODULE_DRIVER ddp_driver_rsz = {
    .module          = DISP_MODULE_RSZ0,
    .init            = RSZInit,
    .deinit          = RSZDeInit,
    .power_on        = RSZClockOn,
    .power_off       = RSZClockOff,
};
