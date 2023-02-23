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

#define LOG_TAG "INFO"

#include "ddp_log.h"
#include "ddp_info.h"
#include "disp_drv_platform.h"

const char *ddp_get_module_name(DISP_MODULE_ENUM module)
{
    switch (module) {
    case DISP_MODULE_OVL0:
        return "ovl0 ";
    case DISP_MODULE_OVL1:
        return "ovl1 ";
    case DISP_MODULE_OVL0_2L:
        return "ovl0_2l ";
    case DISP_MODULE_OVL1_2L:
        return "ovl1_2l ";
    case DISP_MODULE_OVL0_VIRTUAL:
        return "ovl0_virt ";
    case DISP_MODULE_RDMA0:
        return "rdma0 ";
    case DISP_MODULE_RDMA1:
        return "rdma1 ";
    case DISP_MODULE_RDMA2:
        return "rdma2 ";
    case DISP_MODULE_WDMA0:
        return "wdma0 ";
    case DISP_MODULE_WDMA1:
        return "wdma1 ";
    case DISP_MODULE_COLOR0:
        return "color0 ";
    case DISP_MODULE_COLOR1:
        return "color1 ";
    case DISP_MODULE_CCORR0:
        return "ccorr0 ";
    case DISP_MODULE_CCORR1:
        return "ccorr1 ";
    case DISP_MODULE_AAL0:
        return "aal0 ";
    case DISP_MODULE_AAL1:
        return "aal1 ";
    case DISP_MODULE_GAMMA0:
        return "gamma0 ";
    case DISP_MODULE_GAMMA1:
        return "gamma1 ";
    case DISP_MODULE_OD:
        return "od ";
    case DISP_MODULE_DITHER0:
        return "dither0 ";
    case DISP_MODULE_DITHER1:
        return "dither1 ";
    case DISP_MODULE_PATH0:
        return "path0 ";
    case DISP_MODULE_PATH1:
        return "path1 ";
    case DISP_MODULE_UFOE:
        return "ufoe ";
    case DISP_MODULE_DSC:
        return "dsc ";
    case DISP_MODULE_SPLIT0:
        return "split0 ";
    case DISP_MODULE_DPI:
        return "dpi ";
    case DISP_MODULE_DSI0:
        return "dsi0 ";
    case DISP_MODULE_DSI1:
        return "dsi1 ";
    case DISP_MODULE_DSIDUAL:
        return "dsidual ";
    case DISP_MODULE_PWM0:
        return "pwm0 ";
    case DISP_MODULE_CONFIG:
        return "config ";
    case DISP_MODULE_MUTEX:
        return "mutex ";
    case DISP_MODULE_SMI_COMMON:
        return "smi_common";
    case DISP_MODULE_SMI_LARB0:
        return "smi_larb0";
    case DISP_MODULE_SMI_LARB1:
        return "smi_larb1";
    case DISP_MODULE_MIPI0:
        return "mipi0";
    case DISP_MODULE_MIPI1:
        return "mipi1";
    case DISP_MODULE_RSZ0:
        return "rsz0";
    case DISP_MODULE_RSZ1:
        return "rsz1";
    case DISP_MODULE_POSTMASK:
        return "postmask ";
    default:
        DDPMSG("invalid module id=%d\n", module);
        return "unknown";
    }
}
/*
int ddp_get_module_max_irq_bit(DISP_MODULE_ENUM module)
{
    switch(module)
    {
        case DISP_MODULE_UFOE   :    return 0;
        case DISP_MODULE_AAL    :    return 1;
        case DISP_MODULE_COLOR0 :    return 2;
        case DISP_MODULE_COLOR1 :    return 2;
        case DISP_MODULE_RDMA0  :    return 5;
        case DISP_MODULE_RDMA1  :    return 5;
        case DISP_MODULE_RDMA2  :    return 5;
        case DISP_MODULE_WDMA0  :    return 1;
        case DISP_MODULE_WDMA1  :    return 1;
        case DISP_MODULE_OVL0   :    return 3;
        case DISP_MODULE_OVL1   :    return 3;
        case DISP_MODULE_GAMMA  :    return 0;
        case DISP_MODULE_PWM0   :    return 0;
        case DISP_MODULE_PWM1   :    return 0;
        case DISP_MODULE_OD     :    return 0;
        case DISP_MODULE_MERGE  :    return 0;
        case DISP_MODULE_SPLIT0 :    return 0;
        case DISP_MODULE_SPLIT1 :    return 0;
        case DISP_MODULE_DSI0   :    return 6;
        case DISP_MODULE_DSI1   :    return 6;
        case DISP_MODULE_DSIDUAL:    return 6;
        case DISP_MODULE_DPI    :    return 2;
        case DISP_MODULE_SMI    :    return 0;
        case DISP_MODULE_CONFIG :    return 0;
        case DISP_MODULE_CMDQ   :    return 0;
        case DISP_MODULE_MUTEX  :    return 14;
        case DISP_MODULE_CCORR  :    return 0;
        case DISP_MODULE_DITHER :    return 0;
        case DISP_MODULE_POSTMASK :    return 13;
        default:
             DDPERR("invalid module id=%d", module);
    }
    return 0;
}
*/
unsigned int ddp_module_to_idx(int module)
{
    unsigned int id = 0;
    switch (module) {
    case DISP_MODULE_AAL0:
    case DISP_MODULE_COLOR0:
    case DISP_MODULE_RDMA0:
    case DISP_MODULE_WDMA0:
    case DISP_MODULE_OVL0:
    case DISP_MODULE_GAMMA0:
    case DISP_MODULE_PWM0:
    case DISP_MODULE_OD:
    case DISP_MODULE_SPLIT0:
    case DISP_MODULE_DSI0:
    case DISP_MODULE_DPI:
    case DISP_MODULE_DITHER0:
    case DISP_MODULE_CCORR0:
    case DISP_MODULE_POSTMASK:
        id = 0;
        break;

    case DISP_MODULE_COLOR1:
    case DISP_MODULE_RDMA1:
    case DISP_MODULE_DSI1:
    case DISP_MODULE_OVL1:
    case DISP_MODULE_WDMA1:
        id = 1;
        break;
    case DISP_MODULE_RDMA2:
    case DISP_MODULE_DSIDUAL:
        id = 2;
        break;
    default:
        DDPERR("ddp_module_to_idx, module=0x%x\n", module);
    }

    return id;
}


int ddp_enable_module_clock(DISP_MODULE_ENUM module)
{
    DDPMSG("ddp_enable_module_clock: %s\n", ddp_get_module_name(module));
    switch (module) {
    case DISP_MODULE_SMI_COMMON:
        DISP_REG_SET_FIELD(NULL, FLD_CG0_SMI_COMMON, DISP_REG_CONFIG_MMSYS_CG_CLR0, 1);
        DISP_REG_SET_FIELD(NULL, FLD_CG0_GALS_COMMON0, DISP_REG_CONFIG_MMSYS_CG_CLR0, 1);
        DISP_REG_SET_FIELD(NULL, FLD_CG0_GALS_COMMON1, DISP_REG_CONFIG_MMSYS_CG_CLR0, 1);
        DISP_REG_SET_FIELD(NULL, FLD_CG1_26M, DISP_REG_CONFIG_MMSYS_CG_CLR1, 1);
        DISP_REG_SET_FIELD(NULL, FLD_CG0_SMI_COMMON, DISP_REG_CONFIG_MMSYS_DUMMY0, 0);
        break;
    case DISP_MODULE_SMI_LARB0:
        DISP_REG_SET_FIELD(NULL, FLD_CG0_SMI_LARB0, DISP_REG_CONFIG_MMSYS_CG_CLR0, 1);
        DISP_REG_SET_FIELD(NULL, FLD_CG0_SMI_LARB0, DISP_REG_CONFIG_MMSYS_DUMMY0, 0);
        break;
    case DISP_MODULE_SMI_LARB1:
        DISP_REG_SET_FIELD(NULL, FLD_CG0_SMI_LARB1, DISP_REG_CONFIG_MMSYS_CG_CLR0, 1);
        DISP_REG_SET_FIELD(NULL, FLD_CG0_SMI_LARB1, DISP_REG_CONFIG_MMSYS_DUMMY0, 0);
        break;
    case DISP_MODULE_OVL0:
        DISP_REG_SET_FIELD(NULL, FLD_CG0_OVL0, DISP_REG_CONFIG_MMSYS_CG_CLR0, 1);
        DISP_REG_SET_FIELD(NULL, FLD_CG0_OVL0, DISP_REG_CONFIG_MMSYS_DUMMY0, 0);
        break;
    case DISP_MODULE_OVL1:
        //DISP_REG_SET_FIELD(NULL, FLD_CG0_OVL1, DISP_REG_CONFIG_MMSYS_CG_CLR0, 1);
        //DISP_REG_SET_FIELD(NULL, FLD_CG0_OVL1, DISP_REG_CONFIG_MMSYS_DUMMY0, 0);
        break;
    case DISP_MODULE_OVL0_2L:
        DISP_REG_SET_FIELD(NULL, FLD_CG0_OVL0_2L, DISP_REG_CONFIG_MMSYS_CG_CLR0, 1);
        DISP_REG_SET_FIELD(NULL, FLD_CG0_OVL0_2L, DISP_REG_CONFIG_MMSYS_DUMMY0, 0);
        break;
    case DISP_MODULE_OVL1_2L:
        DISP_REG_SET_FIELD(NULL, FLD_CG0_OVL1_2L, DISP_REG_CONFIG_MMSYS_CG_CLR0, 1);
        DISP_REG_SET_FIELD(NULL, FLD_CG0_OVL1_2L, DISP_REG_CONFIG_MMSYS_DUMMY0, 0);
        break;
    case DISP_MODULE_COLOR0:
        DISP_REG_SET_FIELD(NULL, FLD_CG0_COLOR0, DISP_REG_CONFIG_MMSYS_CG_CLR0, 1);
        DISP_REG_SET_FIELD(NULL, FLD_CG0_COLOR0, DISP_REG_CONFIG_MMSYS_DUMMY0, 0);
        break;
    case DISP_MODULE_CCORR0:
        DISP_REG_SET_FIELD(NULL, FLD_CG0_CCORR0, DISP_REG_CONFIG_MMSYS_CG_CLR0, 1);
        DISP_REG_SET_FIELD(NULL, FLD_CG0_CCORR0, DISP_REG_CONFIG_MMSYS_DUMMY0, 0);
        break;
    case DISP_MODULE_AAL0:
        DISP_REG_SET_FIELD(NULL, FLD_CG0_AAL0, DISP_REG_CONFIG_MMSYS_CG_CLR0, 1);
        DISP_REG_SET_FIELD(NULL, FLD_CG0_AAL0, DISP_REG_CONFIG_MMSYS_DUMMY0, 0);
        break;
    case DISP_MODULE_UFOE:
        //DISP_REG_SET_FIELD(NULL, FLD_CG0_UFOE, DISP_REG_CONFIG_MMSYS_CG_CLR0, 1);
        //DISP_REG_SET_FIELD(NULL, FLD_CG0_UFOE, DISP_REG_CONFIG_MMSYS_DUMMY0, 0);
        break;
    case DISP_MODULE_SPLIT0:
        DISP_REG_SET_FIELD(NULL, FLD_CG0_SPLIT, DISP_REG_CONFIG_MMSYS_CG_CLR0, 1);
        DISP_REG_SET_FIELD(NULL, FLD_CG0_SPLIT, DISP_REG_CONFIG_MMSYS_DUMMY0, 0);
        break;
    case DISP_MODULE_RDMA0:
        DISP_REG_SET_FIELD(NULL, FLD_CG0_RDMA0, DISP_REG_CONFIG_MMSYS_CG_CLR0, 1);
        DISP_REG_SET_FIELD(NULL, FLD_CG0_RDMA0, DISP_REG_CONFIG_MMSYS_DUMMY0, 0);
        break;
    case DISP_MODULE_RDMA1:
        DISP_REG_SET_FIELD(NULL, FLD_CG0_RDMA1, DISP_REG_CONFIG_MMSYS_CG_CLR0, 1);
        DISP_REG_SET_FIELD(NULL, FLD_CG0_RDMA1, DISP_REG_CONFIG_MMSYS_DUMMY0, 0);
        break;
    case DISP_MODULE_GAMMA0:
        DISP_REG_SET_FIELD(NULL, FLD_CG0_GAMMA0, DISP_REG_CONFIG_MMSYS_CG_CLR0, 1);
        DISP_REG_SET_FIELD(NULL, FLD_CG0_GAMMA0, DISP_REG_CONFIG_MMSYS_DUMMY0, 0);
        break;
    case DISP_MODULE_DSI0:
        DISP_REG_SET_FIELD(NULL, FLD_PLL_MIPI_DSI0_26M_CK_EN, DISP_REG_APMIXED_PLL_CON0, 3);
        DISP_REG_SET_FIELD(NULL, FLD_CG1_DSI0_MM, DISP_REG_CONFIG_MMSYS_CG_CLR1, 1);
        DISP_REG_SET_FIELD(NULL, FLD_CG1_DSI0_IF, DISP_REG_CONFIG_MMSYS_CG_CLR1, 1);
        break;
    case DISP_MODULE_DSIDUAL:
        DISP_REG_SET_FIELD(NULL, FLD_PLL_MIPI_DSI0_26M_CK_EN, DISP_REG_APMIXED_PLL_CON0, 3);
        DISP_REG_SET_FIELD(NULL, FLD_CG1_DSI0_MM, DISP_REG_CONFIG_MMSYS_CG_CLR1, 1);
        DISP_REG_SET_FIELD(NULL, FLD_CG1_DSI0_IF, DISP_REG_CONFIG_MMSYS_CG_CLR1, 1);
        //DISP_REG_SET_FIELD(NULL, FLD_CG1_DSI1_MM, DISP_REG_CONFIG_MMSYS_CG_CLR1, 1);
        //DISP_REG_SET_FIELD(NULL, FLD_CG1_DSI1_IF, DISP_REG_CONFIG_MMSYS_CG_CLR1, 1);

        break;
    case DISP_MODULE_WDMA0:
        DISP_REG_SET_FIELD(NULL, FLD_CG0_WDMA0, DISP_REG_CONFIG_MMSYS_CG_CLR0, 1);
        DISP_REG_SET_FIELD(NULL, FLD_CG0_WDMA0, DISP_REG_CONFIG_MMSYS_DUMMY0, 0);
        break;
    case DISP_MODULE_WDMA1:
        //DISP_REG_SET_FIELD(NULL, FLD_CG0_WDMA1, DISP_REG_CONFIG_MMSYS_CG_CLR0, 1);
        //DISP_REG_SET_FIELD(NULL, FLD_CG0_WDMA1, DISP_REG_CONFIG_MMSYS_DUMMY0, 0);
        break;
    case DISP_MODULE_DITHER0:
        DISP_REG_SET_FIELD(NULL, FLD_CG0_DITHER0, DISP_REG_CONFIG_MMSYS_CG_CLR0, 1);
        DISP_REG_SET_FIELD(NULL, FLD_CG0_DITHER0, DISP_REG_CONFIG_MMSYS_DUMMY0, 0);
        break;
    case DISP_MODULE_DPI:
        DISP_REG_SET_FIELD(NULL, FLD_CG1_DPI_MM, DISP_REG_CONFIG_MMSYS_CG_CLR1, 1);
        DISP_REG_SET_FIELD(NULL, FLD_CG1_DPI_IF, DISP_REG_CONFIG_MMSYS_CG_CLR1, 1);
        //DISP_REG_SET_FIELD(NULL, FLD_CG1_DPI_ANALOG, DISP_REG_CONFIG_MMSYS_CG_CLR1,3);
        break;
    case DISP_MODULE_MUTEX:
        // no CG
        break;
    case DISP_MODULE_POSTMASK:
        DISP_REG_SET_FIELD(NULL, FLD_CG1_POSTMASK, DISP_REG_CONFIG_MMSYS_CG_CLR1, 1);
        break;
    case DISP_MODULE_RSZ0:
        DISP_REG_SET_FIELD(NULL, FLD_CG1_RSZ0, DISP_REG_CONFIG_MMSYS_CG_CLR1, 1);
        break;
    default:
        DDPERR("enable module clock unknow module %d \n", module);
    }
    return 0;
}

int ddp_disable_module_clock(DISP_MODULE_ENUM module)
{
    DDPMSG("ddp_disable_module_clock: %s\n", ddp_get_module_name(module));
    switch (module) {
    case DISP_MODULE_SMI_COMMON:
        DISP_REG_SET_FIELD(NULL, FLD_CG1_26M, DISP_REG_CONFIG_MMSYS_CG_SET1, 1);
        DISP_REG_SET_FIELD(NULL, FLD_CG0_GALS_COMMON1, DISP_REG_CONFIG_MMSYS_CG_SET0, 1);
        DISP_REG_SET_FIELD(NULL, FLD_CG0_GALS_COMMON0, DISP_REG_CONFIG_MMSYS_CG_SET0, 1);
        DISP_REG_SET_FIELD(NULL, FLD_CG0_SMI_COMMON, DISP_REG_CONFIG_MMSYS_CG_SET0, 1);
        DISP_REG_SET_FIELD(NULL, FLD_CG0_SMI_COMMON, DISP_REG_CONFIG_MMSYS_DUMMY0, 0);
        break;
    case DISP_MODULE_SMI_LARB0:
        DISP_REG_SET_FIELD(NULL, FLD_CG0_SMI_LARB0, DISP_REG_CONFIG_MMSYS_CG_SET0, 1);
        DISP_REG_SET_FIELD(NULL, FLD_CG0_SMI_LARB0, DISP_REG_CONFIG_MMSYS_DUMMY0, 0);
        break;
    case DISP_MODULE_SMI_LARB1:
        DISP_REG_SET_FIELD(NULL, FLD_CG0_SMI_LARB1, DISP_REG_CONFIG_MMSYS_CG_SET0, 1);
        DISP_REG_SET_FIELD(NULL, FLD_CG0_SMI_LARB1, DISP_REG_CONFIG_MMSYS_DUMMY0, 0);
        break;
    case DISP_MODULE_OVL0:
        DISP_REG_SET_FIELD(NULL, FLD_CG0_OVL0, DISP_REG_CONFIG_MMSYS_CG_SET0, 1);
        DISP_REG_SET_FIELD(NULL, FLD_CG0_OVL0, DISP_REG_CONFIG_MMSYS_DUMMY0, 0);
        break;
    case DISP_MODULE_OVL1:
        //DISP_REG_SET_FIELD(NULL, FLD_CG0_OVL1, DISP_REG_CONFIG_MMSYS_CG_SET0, 1);
        //DISP_REG_SET_FIELD(NULL, FLD_CG0_OVL1, DISP_REG_CONFIG_MMSYS_DUMMY0, 0);
        break;
    case DISP_MODULE_OVL0_2L:
        DISP_REG_SET_FIELD(NULL, FLD_CG0_OVL0_2L, DISP_REG_CONFIG_MMSYS_CG_SET0, 1);
        DISP_REG_SET_FIELD(NULL, FLD_CG0_OVL0_2L, DISP_REG_CONFIG_MMSYS_DUMMY0, 0);
        break;
    case DISP_MODULE_OVL1_2L:
        DISP_REG_SET_FIELD(NULL, FLD_CG0_OVL1_2L, DISP_REG_CONFIG_MMSYS_CG_SET0, 1);
        DISP_REG_SET_FIELD(NULL, FLD_CG0_OVL1_2L, DISP_REG_CONFIG_MMSYS_DUMMY0, 0);
        break;
    case DISP_MODULE_COLOR0:
        DISP_REG_SET_FIELD(NULL, FLD_CG0_COLOR0, DISP_REG_CONFIG_MMSYS_CG_SET0, 1);
        DISP_REG_SET_FIELD(NULL, FLD_CG0_COLOR0, DISP_REG_CONFIG_MMSYS_DUMMY0, 0);
        break;
    case DISP_MODULE_CCORR0:
        DISP_REG_SET_FIELD(NULL, FLD_CG0_CCORR0, DISP_REG_CONFIG_MMSYS_CG_SET0, 1);
        DISP_REG_SET_FIELD(NULL, FLD_CG0_CCORR0, DISP_REG_CONFIG_MMSYS_DUMMY0, 0);
        break;
    case DISP_MODULE_AAL0:
        DISP_REG_SET_FIELD(NULL, FLD_CG0_AAL0, DISP_REG_CONFIG_MMSYS_CG_SET0, 1);
        DISP_REG_SET_FIELD(NULL, FLD_CG0_AAL0, DISP_REG_CONFIG_MMSYS_DUMMY0, 0);
        break;
    case DISP_MODULE_UFOE:
        //DISP_REG_SET_FIELD(NULL, FLD_CG0_UFOE, DISP_REG_CONFIG_MMSYS_CG_SET0, 1);
        //DISP_REG_SET_FIELD(NULL, FLD_CG0_UFOE, DISP_REG_CONFIG_MMSYS_DUMMY0, 0);
        break;
    case DISP_MODULE_SPLIT0:
        DISP_REG_SET_FIELD(NULL, FLD_CG0_SPLIT, DISP_REG_CONFIG_MMSYS_CG_SET0, 1);
        DISP_REG_SET_FIELD(NULL, FLD_CG0_SPLIT, DISP_REG_CONFIG_MMSYS_DUMMY0, 0);
        break;
    case DISP_MODULE_RDMA0:
        DISP_REG_SET_FIELD(NULL, FLD_CG0_RDMA0, DISP_REG_CONFIG_MMSYS_CG_SET0, 1);
        DISP_REG_SET_FIELD(NULL, FLD_CG0_RDMA0, DISP_REG_CONFIG_MMSYS_DUMMY0, 0);
        break;
    case DISP_MODULE_RDMA1:
        DISP_REG_SET_FIELD(NULL, FLD_CG0_RDMA1, DISP_REG_CONFIG_MMSYS_CG_SET0, 1);
        DISP_REG_SET_FIELD(NULL, FLD_CG0_RDMA1, DISP_REG_CONFIG_MMSYS_DUMMY0, 0);
        break;
    case DISP_MODULE_GAMMA0:
        DISP_REG_SET_FIELD(NULL, FLD_CG0_GAMMA0, DISP_REG_CONFIG_MMSYS_CG_SET0, 1);
        DISP_REG_SET_FIELD(NULL, FLD_CG0_GAMMA0, DISP_REG_CONFIG_MMSYS_DUMMY0, 0);
        break;
    case DISP_MODULE_DSI0:
        DISP_REG_SET_FIELD(NULL, FLD_CG1_DSI0_MM, DISP_REG_CONFIG_MMSYS_CG_SET1, 1);
        DISP_REG_SET_FIELD(NULL, FLD_CG1_DSI0_IF, DISP_REG_CONFIG_MMSYS_CG_SET1, 1);
        DISP_REG_SET_FIELD(NULL, FLD_PLL_MIPI_DSI0_26M_CK_EN, DISP_REG_APMIXED_PLL_CON0, 0);
        //DISP_REG_SET_FIELD(NULL, FLD_CG1_DSI0_ANALOG, DISP_REG_CONFIG_MMSYS_CG_SET1, 1);
        break;
    case DISP_MODULE_DSIDUAL:
        DISP_REG_SET_FIELD(NULL, FLD_CG1_DSI0_MM, DISP_REG_CONFIG_MMSYS_CG_SET1, 1);
        DISP_REG_SET_FIELD(NULL, FLD_CG1_DSI0_IF, DISP_REG_CONFIG_MMSYS_CG_SET1, 1);
        DISP_REG_SET_FIELD(NULL, FLD_PLL_MIPI_DSI0_26M_CK_EN, DISP_REG_APMIXED_PLL_CON0, 0);
        //DISP_REG_SET_FIELD(NULL, FLD_CG1_DSI0_ANALOG, DISP_REG_CONFIG_MMSYS_CG_SET1,3);
        //DISP_REG_SET_FIELD(NULL, FLD_CG1_DSI1_MM, DISP_REG_CONFIG_MMSYS_CG_SET1, 1);
        //DISP_REG_SET_FIELD(NULL, FLD_CG1_DSI1_IF, DISP_REG_CONFIG_MMSYS_CG_SET1, 1);
        //DISP_REG_SET_FIELD(NULL, FLD_CG1_DSI1_ANALOG, DISP_REG_CONFIG_MMSYS_CG_SET1,3);
        break;
    case DISP_MODULE_WDMA0:
        DISP_REG_SET_FIELD(NULL, FLD_CG0_WDMA0, DISP_REG_CONFIG_MMSYS_CG_SET0, 1);
        DISP_REG_SET_FIELD(NULL, FLD_CG0_WDMA0, DISP_REG_CONFIG_MMSYS_DUMMY0, 0);
        break;
    case DISP_MODULE_WDMA1:
        //DISP_REG_SET_FIELD(NULL,MMSYS_CG_FLD_CG0_WDMA1, DISP_REG_CONFIG_MMSYS_CG_SET0, 1);
        //DISP_REG_SET_FIELD(NULL,MMSYS_CG_FLD_CG0_WDMA1, DISP_REG_CONFIG_MMSYS_DUMMY0, 0);
        break;
    case DISP_MODULE_DITHER0:
        DISP_REG_SET_FIELD(NULL, FLD_CG0_DITHER0, DISP_REG_CONFIG_MMSYS_CG_SET0, 1);
        DISP_REG_SET_FIELD(NULL, FLD_CG0_DITHER0, DISP_REG_CONFIG_MMSYS_DUMMY0, 0);
        break;
    case DISP_MODULE_DPI:
        DISP_REG_SET_FIELD(NULL, FLD_CG1_DPI_MM, DISP_REG_CONFIG_MMSYS_CG_SET1, 1);
        DISP_REG_SET_FIELD(NULL, FLD_CG1_DPI_IF, DISP_REG_CONFIG_MMSYS_CG_SET1, 1);
        //DISP_REG_SET_FIELD(NULL, FLD_CG1_DPI_ANALOG, DISP_REG_CONFIG_MMSYS_CG_SET1,3);
        break;
    case DISP_MODULE_MUTEX:
        // no CG
        break;
    case DISP_MODULE_POSTMASK:
        DISP_REG_SET_FIELD(NULL, FLD_CG1_POSTMASK, DISP_REG_CONFIG_MMSYS_CG_SET1, 1);
        break;
    case DISP_MODULE_RSZ0:
        DISP_REG_SET_FIELD(NULL, FLD_CG1_RSZ0, DISP_REG_CONFIG_MMSYS_CG_SET1, 1);
        break;
    default:
        DDPERR("disable module clock unknow module %d \n", module);
    }
    DDPMSG("disable %s clk, CG0 0x%x, CG1 0x%x,dummy CON = 0x%x\n",
           ddp_get_module_name(module), DISP_REG_GET(DISP_REG_CONFIG_MMSYS_CG_CON0), DISP_REG_GET(DISP_REG_CONFIG_MMSYS_CG_CON1), DISP_REG_GET(DISP_REG_CONFIG_MMSYS_DUMMY0));
    return 0;
}



// dsi
extern DDP_MODULE_DRIVER ddp_driver_dsi0;
/*extern DDP_MODULE_DRIVER ddp_driver_dsi1;*/
/*extern DDP_MODULE_DRIVER ddp_driver_dsidual;*/
// dpi
//extern DDP_MODULE_DRIVER ddp_driver_dpi;

// ovl
extern DDP_MODULE_DRIVER ddp_driver_ovl;
extern DDP_MODULE_DRIVER ddp_driver_rsz;
// rdma
extern DDP_MODULE_DRIVER ddp_driver_rdma;

// color
extern DDP_MODULE_DRIVER ddp_driver_color;
// ccorr
extern DDP_MODULE_DRIVER ddp_driver_ccorr;
// aal
extern DDP_MODULE_DRIVER ddp_driver_aal;
// ufoe
//extern DDP_MODULE_DRIVER ddp_driver_ufoe;
// gamma
extern DDP_MODULE_DRIVER ddp_driver_gamma;
// dither
extern DDP_MODULE_DRIVER ddp_driver_dither;
// ccorr
//extern DDP_MODULE_DRIVER ddp_driver_ccorr;
// split
//extern DDP_MODULE_DRIVER ddp_driver_split;

// pwm
//extern DDP_MODULE_DRIVER ddp_driver_pwm;
// postmask
extern DDP_MODULE_DRIVER ddp_driver_postmask;

DDP_MODULE_DRIVER  *ddp_modules_driver[DISP_MODULE_NUM] = {
    &ddp_driver_ovl,    /* DISP_MODULE_OVL0 = 0 */
    0,          /* DISP_MODULE_OVL1     */
    &ddp_driver_ovl,    /* DISP_MODULE_OVL0_2L  */
    &ddp_driver_ovl,    /* DISP_MODULE_OVL1_2L  */
    0,          /* DISP_MODULE_OVL0_VIRTUAL */
    0,          /* DISP_MODULE_OVL0_2L_VIRTUAL */
    0,          /* DISP_MODULE_OVL1_2L_VIRTUAL */

    &ddp_driver_rdma,   /* DISP_MODULE_RDMA0    */
    &ddp_driver_rdma,   /* DISP_MODULE_RDMA1    */
    0,          /* DISP_MODULE_RDMA2    */
    0,          /* DISP_MODULE_RDMA_VIRTUAL0 */

    0,          /* DISP_MODULE_WDMA0    */
    0,          /* DISP_MODULE_WDMA1    */
    0,          /* DISP_MODULE_WDMA_VIRTUAL0 */
    0,          /* DISP_MODULE_WDMA_VIRTUAL1 */

    &ddp_driver_color,  /* DISP_MODULE_COLOR0   */
    0,          /* DISP_MODULE_COLOR1   */
    &ddp_driver_ccorr,  /* DISP_MODULE_CCORR0   */
    0,          /* DISP_MODULE_CCORR1   */
    &ddp_driver_aal,    /* DISP_MODULE_AAL0     */
    0,          /* DISP_MODULE_AAL1     */
    &ddp_driver_gamma,  /* DISP_MODULE_GAMMA0   */
    0,          /* DISP_MODULE_GAMMA1   */
    0,          /* DISP_MODULE_OD       */
    &ddp_driver_dither, /* DISP_MODULE_DITHER0  */
    0,          /* DISP_MODULE_DITHER1  */

    0,          /* DISP_MODULE_PATH0    */
    0,          /* DISP_MODULE_PATH1    */

    0,          /* DISP_MODULE_UFOE     */
    0,          /* DISP_MODULE_DSC      */
    0,          /* DISP_MODULE_DSC_2ND  */
    0,          /* DISP_MODULE_SPLIT0   */
    &ddp_driver_dsi0,   /* DISP_MODULE_DSI0     */
    0,          /* DISP_MODULE_DSI1     */
    0,          /* DISP_MODULE_DSIDUAL  */
    0,          /* DISP_MODULE_PWM0     */
    0,          /* DISP_MODULE_PWM1     */

    0,          /* DISP_MODULE_CONFIG   */
    0,          /* DISP_MODULE_MUTEX    */
    0,          /* DISP_MODULE_SMI_COMMON */
    0,          /* DISP_MODULE_SMI_LARB0  */
    0,          /* DISP_MODULE_SMI_LARB1  */

    0,          /* DISP_MODULE_MIPI0    */
    0,          /* DISP_MODULE_MIPI1    */

    &ddp_driver_rsz,    /* DISP_MODULE_RSZ0     */
    0,          /* DISP_MODULE_RSZ1     */
    0,          /* DISP_MODULE_DPI      */
    0,          /* DISP_MODULE_DBI      */
    0,          /* DISP_MODULE_DPI_VIRTUAL */
    &ddp_driver_postmask,   /* DISP_MODULE_POSTMASK */

    0,          /* DISP_MODULE_UNKNOWN  */
};

