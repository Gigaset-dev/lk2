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

#define LOG_TAG "DUMP"

#include "ddp_info.h"
#include "ddp_log.h"
#include "ddp_path.h"
#include "ddp_dump.h"
#include "disp_drv_platform.h"
#include "ddp_reg.h"
#include "ddp_dsi.h"
#include "ddp_rdma.h"
#include "ddp_ovl.h"
#include "ddp_postmask.h"

#include <string.h>
#include <reg.h>


static const char *ddp_signal[5][32] = {
    {
        "aal0__to__gamma0",
        "ccorr0__to__aal0",
        "color0__to__disp_color_out_sel_in0",
        "disp_color_out_sel__to__ccorr0",
        "dither0__to__dither0_mout",
        "dither0_mout0__to__dsi0_sel_in0",
        "dither0_mout1__to__dpi_sel_in2",
        "dither0_mout2__to__disp_wdma0_pre_sel_in1",
        "disp_gamma0__to__disp_postmask",
        "disp_ovl0_2l_mout_out0__to__disp_path0_sel_in1",
        "disp_ovl0_2l_mout_out1__to__ovl_to_wrot_sel_in1",
        "disp_ovl0_2l_mout_out2__to__ovl_to_wdma_sel_in1",
        "disp_ovl0_2l_mout_out3__to__ovl_to_rsz_sel_in1",
        "disp_ovl0_2l_mout_out4__to__disp_rsz_sel_in5",
        "disp_ovl0_2l_out0__to__disp_ovl0_2l_mout",
        "disp_ovl0_2l_out1__to__disp_ovl0_2l_wcg_mout",
        "disp_ovl0_2l_out2__to__disp_rsz_sel_in1",
        "disp_ovl0_2l_sel__to__disp_ovl0_2l_in1",
        "disp_ovl0_2l_wcg_mout_out0__to__disp_ovl0_wcg_sel_in0",
        "disp_ovl0_2l_wcg_mout_out1__to__disp_ovl1_2l_wcg_sel_in1",
        "disp_ovl0_2l_wcg_sel__to__disp_ovl0_2l_in0",
        "disp_ovl0_mout_out0__to__disp_path0_sel_in0",
        "disp_ovl0_mout_out1__to__ovl_to_wrot_sel_in0",
        "disp_ovl0_mout_out2__to__ovl_to_wdma_sel_in0",
        "disp_ovl0_mout_out3__to__ovl_to_rsz_sel_in0",
        "disp_ovl0_mout_out4__to__disp_rsz_sel_in4",
        "disp_ovl0_out0__to__disp_ovl0_mout",
        "disp_ovl0_out1__to__disp_ovl0_wcg_mout",
        "disp_ovl0_out2__to__disp_rsz_sel_in0",
        "disp_ovl0_sel__to__disp_ovl0_in1",
        "disp_ovl0_wcg_mout_out0__to__disp_ovl0_2l_wcg_sel_in0",
        "disp_ovl0_wcg_mout_out1__to__disp_ovl1_2l_wcg_sel_in0",
    },
    {
        "disp_ovl0_wcg_sel__to__disp_ovl0_in0",
        "disp_ovl1_2l_mout_out0__to__disp_path0_sel_in2",
        "disp_ovl1_2l_mout_out1__to__ovl_to_wrot_sel_in2",
        "disp_ovl1_2l_mout_out2__to__ovl_to_wdma_sel_in2",
        "disp_ovl1_2l_mout_out3__to__ovl_to_rsz_sel_in2",
        "disp_ovl1_2l_mout_out4__to__disp_rdma1",
        "disp_ovl1_2l_mout_out5__to__disp_rsz_sel_in6",
        "disp_ovl1_2l_out0__to__disp_ovl1_2l_mout",
        "disp_ovl1_2l_out1__to__disp_ovl1_2l_wcg_mout",
        "disp_ovl1_2l_out2__to__disp_rsz_sel_in2",
        "disp_ovl1_2l_wcg_mout_out0__to__disp_ovl0_2l_wcg_sel_in0",
        "disp_ovl1_2l_wcg_mout_out1__to__disp_ovl0_wcg_sel_in1",
        "disp_ovl1_2l_wcg_sel__to__disp_ovl1_2l_in0",
        "disp_path0_sel__to__disp_rdma0",
        "disp_postmask0__to__disp_dither0",
        "disp_rdma0__to__disp_rdma0_rsz_in_sout",
        "disp_rdma0_rsz_in_sout_out0__to__disp_rdma0_rsz_out_sel_in0",
        "disp_rdma0_rsz_in_sout_out1__to__disp_rsz_sel_in3",
        "disp_rdma0_esz_out_sel__to__disp_rdma0_sout",
        "disp_rdma0_sout_out0__to__dsi0_sel_in1",
        "disp_rdma0_sout_out1__to__disp_color0",
        "disp_rdma0_sout_out2__to__disp_color_out_sel_in1",
        "disp_rdma0_sout_out3__to__dpi0_sel_in0",
        "disp_rdma1__to__disp_rdma1_sout",
        "disp_rdma1_sout_out0__to__dpi0_sel_in1",
        "disp_rdma1_sout_out1__to__dsi0_sel_in2",
        "disp_rsz0__to__disp_rsz_mout",
        "disp_rsz_mout_out0__to__disp_ovl0_in2",
        "disp_rsz_mout_out1__to__disp_ovl0_2l_in2",
        "disp_rsz_mout_out2__to__disp_ovl1_2l_in2",
        "disp_rsz_mout_out3__to__disp_path0_sel_in3",
        "disp_rsz_mout_out4__to__ovl_to_wdma_sel_in3",
    },
    {
        "disp_rsz_mout_out5__to__disp_rdma0_rsz_out_sel_in3",
        "disp_rsz_mout_out6__to__ovl_to_wrot_sel_in3",
        "disp_rsz_sel__to__disp_rsz0",
        "disp_to_wrot_sout_out0__to__mdp_wrot0_sel_in1",
        "disp_to_wrot_sout_out1__to__mdp_wrot1_sel_in1",
        "disp_wdma0_pre_sel__to__disp_wdma0_sel_in3",
        "disp_wdma0_sel__to__disp_wdma0",
        "dpi0_sel__to__dpi0_thp_lmt",
        "dpi0_thp_lmt__to__dpi0",
        "dsi0_sel__to__dsi0_thp_lmt",
        "dsi0_thp_lmt__to__dsi0",
        "ipu_mout_out0__to__mdp_hdr_sel_in3",
        "ipu_mout_out1__to__mdp_rsz1_sel_in4",
        "ipu_mout_out2__to__mdp_aal_sel_in2",
        "isp_mout_out0__to__mdp_hdr_sel_in0",
        "isp_mout_out1__to__mdp_rsz1_sel_in0",
        "isp_mout_out2__to__mdp_aal_sel_in0",
        "mdp_aal0__to__mdp_aal_mout",
        "mdp_aal_mout_out0__to__mdp_hdr_sel_in4",
        "mdp_aal_mout_out1__to__mdp_rsz1_sel_in5",
        "mdp_aal_mout_out2__to__mdp_rsz0_sel_in0",
        "mdp_aal_sel__to__mdp_aal0",
        "mdp_hdr_sel__to__mdp_hdr0",
        "mdp_hdr_sout_out0__to__mdp_aal_sel_in3",
        "mdp_hdr_sout_out1__to__mdp_rsz0_sel_in1",
        "mdp_color0__to__mdp_color_out_sel_in1",
        "mdp_color_in_sout_out0__to__mdp_color_out_sel_in0",
        "mdp_color_in_sout_out1__to__mdp_color0",
        "mdp_color_mout_out0__to__mdp_path0_sel_in2",
        "mdp_color_mout_out1__to__mdp_path1_sel_in1",
        "mdp_color_mout_out2__to__disp_wdma0_sel_in0",
        "mdp_color_out_sel__to__mdp_color_mout",
    },
    {
        "mdp_color_sel__to__mdp_color_in_sout",
        "mdp_dl_async__to__isp_mout",
        "mdp_dl_async_ipu__to__ipu_mout",
        "mdp_dl_relay__to__mdp_dl_async",
        "mdp_dl_relay_ipu__to__mdp_dl_async_ipu",
        "mdp_hdr0__to__mdp_hdr_sout",
        "mdp_path0_sel__to__mdp_path0_sout",
        "mdp_path0_sout_out0__to__mdp_mdp_wrot0_sel_in0",
        "mdp_path0_sout_out1__to__mdp_to_disp0_sout",
        "mdp_path1_sel__to__mdp_path1_sout",
        "mdp_path1_sout_out0__to__mdp_mdp_wrot1_sel_in0",
        "mdp_path1_sout_out1__to__mdp_to_disp1_sout",
        "mdp_rdma0__to__mdp_rdma0_mout",
        "mdp_rdma0_mout_out0__to__mdp_hdr_sel_in1",
        "mdp_rdma0_mout_out1__to__mdp_rsz1_sel_in1",
        "mdp_rdma0_mout_out2__to__mdp_path0_sel_in3",
        "mdp_rdma0_mout_out3__to__mdp_aal_sel_in1",
        "mdp_rdma1__to__mdp_rdma1_mout",
        "mdp_rdma1_mout_out0__to__mdp_hdr_sel_in2",
        "mdp_rdma1_mout_out1__to__mdp_rsz1_sel_in2",
        "mdp_rdma1_mout_out2__to__mdp_path1_sel_in2",
        "mdp_rdma1_mout_out3__to__disp_wdma0_sel_in2",
        "mdp_rsz0__to__mdp_rsz0_mout",
        "mdp_rsz0_mout_out0__to__mdp_path0_sel_in0",
        "mdp_rsz0_mout_out1__to__mdp_tdshlp_sel_in0",
        "mdp_rsz0_sel__to__mdp_rsz0",
        "mdp_rsz1__to__mdp_rsz1_mout",
        "mdp_rsz1_mout_out0__to__mdp_path0_sel_in1",
        "mdp_rsz1_mout_out1__to__mdp_tdshp_sel_in1",
        "mdp_rsz1_mout_out2__to__mdp_path1_sel_in0",
        "mdp_rsz1_mout_out3__to__disp_wdma0_sel_in1",
        "mdp_rsz1_mout_out4__to__mdp_color_sel_in1",
    },
    {
        "mdp_rsz1_sel__to__mdp_rsz1",
        "mdp_tdshp0__to__mdp_color_sel_in0",
        "mdp_tdshp_sel__to__ndp_tdshp0",
        "mdp_to_disp0_sout_out0__to__disp_ovl0_sel_in0",
        "mdp_to_disp0_sout_out1__to__disp_ovl0_2l_sel_in0",
        "mdp_to_disp1_sout_out0__to__disp_ovl0_2l_sel_in1",
        "mdp_to_disp1_sout_out1__to__disp_ovl1_2l_sel_in1",
        "mdp_to_disp1_sout_out2__to__disp_ovl0_sel_in1",
        "mdp_wrot0_sel__to__mdp_wrot0",
        "mdp_wrot1_sel__to__mdp_wrot1",
        "mmsys_r2y__to__mdp_rsz1_sel_in3",
        "ovl_to_r2y_sel__to__disp_wdma0_pre_sel_in0",
        "ovl_to_wdma_sel__to__disp_wdma0_pre_sel_in0",
        "ovl_to_wrot_sel__to__disp_to_wrot_sout",
    }
};

static const char *ddp_greq_name(int bit)
{
    switch (bit) {
    case 0:
        return "OVL0";
    case 1:
        return "OVL0_2L";
    case 2:
        return "RDMA0";
    case 3:
        return "WDMA0";
    case 4:
        return "MDP_RDMA0";
    case 5:
        return "MDP_WDMA";
    case 6:
        return "MDP_WROT0";
    case 16:
        return "OVL1";
    case 17:
        return "OVL1_2L";
    case 18:
        return "RDMA1";
    case 19:
        return "RDMA2";
    case 20:
        return "WDMA1";
    case 21:
        return "OD_R";
    case 22:
        return "OD_W";
    case 23:
        return "OVL0_2L_LARB4";
    case 26:
        return "WMDA0_LARB5";
    default:
        return NULL;
    }
}

static const char *ddp_get_mutex_module0_name(unsigned int bit)
{
    switch (bit) {
    case 0:
        return "rdma0";
    case 1:
        return "rdma1";
    case 2:
        return "mdp_rdma0";
    case 4:
        return "mdp_rsz0";
    case 5:
        return "mdp_rsz1";
    case 6:
        return "mdp_tdshp";
    case 7:
        return "mdp_wrot0";
    case 8:
        return "mdp_wrot1";
    case 9:
        return "ovl0";
    case 10:
        return "ovl0_2L";
    case 11:
        return "ovl1_2L";
    case 12:
        return "wdma0";
    case 13:
        return "color0";
    case 14:
        return "ccorr0";
    case 15:
        return "aal0";
    case 16:
        return "gamma0";
    case 17:
        return "dither0";
    case 18:
        return "PWM0";
    case 19:
        return "DSI";
    case 20:
        return "DPI";
    case 21:
        return "postmask";
    case 22:
        return "rsz";
    default:
        return "mutex-unknown";
    }
}

static const char *ddp_get_mutex_module1_name(unsigned int bit)
{
    switch (bit) {
    case 0:
        return "pwm";
    case 1:
        return "dsi0";
    case 2:
        return "dsi1";
    default:
        return "mutex-unknown";
    }
}

const char *ddp_get_fmt_name(DISP_MODULE_ENUM module, unsigned int fmt)
{
    if (module == DISP_MODULE_WDMA0) {
        switch (fmt) {
        case 0:
            return "rgb565";
        case 1:
            return "rgb888";
        case 2:
            return "rgba8888";
        case 3:
            return "argb8888";
        case 4:
            return "uyvy";
        case 5:
            return "yuy2";
        case 7:
            return "y-only";
        case 8:
            return "iyuv";
        case 12:
            return "nv12";
        default:
            DDPDUMP("ddp_get_fmt_name, unknown fmt=%d, module=%d\n", fmt, module);
            return "unknown";
        }
    } else if (module == DISP_MODULE_OVL0) {
        switch (fmt) {
        case 0:
            return "rgb565";
        case 1:
            return "rgb888";
        case 2:
            return "rgba8888";
        case 3:
            return "argb8888";
        case 4:
            return "uyvy";
        case 5:
            return "yuyv";
        default:
            DDPDUMP("ddp_get_fmt_name, unknown fmt=%d, module=%d\n", fmt, module);
            return "unknown";
        }
    } else if (module == DISP_MODULE_RDMA0 || module == DISP_MODULE_RDMA1 || module == DISP_MODULE_RDMA2) {
        switch (fmt) {
        case 0:
            return "rgb565";
        case 1:
            return "rgb888";
        case 2:
            return "rgba8888";
        case 3:
            return "argb8888";
        case 4:
            return "uyvy";
        case 5:
            return "yuyv";
        default:
            DDPDUMP("ddp_get_fmt_name, unknown fmt=%d, module=%d\n", fmt, module);
            return "unknown";
        }
    } else {
        DDPDUMP("ddp_get_fmt_name, unknown module=%d\n", module);
    }

    return "unknown";
}

static const char *ddp_clock_0(int bit)
{
    switch (bit) {
    case 0:
        return "smi_common(cg), ";
    case 1:
        return "smi_larb0(cg), ";
    case 2:
        return "smi_larb1(cg), ";

    case 19:
        return "fake_engine, ";
    case 20:
        return "ovl0, ";
    case 21:
        return "ovl0_2L, ";
    case 22:
        return "ovl1_2L, ";
    case 23:
        return "rdma0, ";
    case 24:
        return "rdma1, ";
    case 25:
        return "wdma0, ";
    case 26:
        return "color, ";
    case 27:
        return "ccorr, ";
    case 28:
        return "aal, ";
    case 29:
        return "gamma, ";
    case 30:
        return "dither, ";
    case 31:
        return "split, ";
    default:
        return NULL;
    }
}

static const char *ddp_clock_1(int bit)
{
    switch (bit) {
    case 0:
        return "dsi0_mm(cg), ";
    case 1:
        return "dsi0_interface(cg), ";
    case 2:
        return "dsi1_mm(cg), ";
    case 3:
        return "dsi1_interface(cg), ";
    case 4:
        return "fake_engine2, ";

    case 7:
        return "26m, ";
    case 8:
        return "mmsys_r2y, ";
    case 9:
        return "disp_rsz, ";

    default:
        return NULL;
    }
}

static void mutex_dump_reg(void)
{
    DDPDUMP("==DISP MUTEX REGS==\n");
    DDPDUMP("(0x000)M_INTEN   =0x%x\n", DISP_REG_GET(DISP_REG_CONFIG_MUTEX_INTEN));
    DDPDUMP("(0x004)M_INTSTA  =0x%x\n", DISP_REG_GET(DISP_REG_CONFIG_MUTEX_INTSTA));
    DDPDUMP("(0x020)M0_EN     =0x%x\n", DISP_REG_GET(DISP_REG_CONFIG_MUTEX0_EN));
    /*DDPDUMP("(0x024)M0_GET     =0x%x\n", DISP_REG_GET(DISP_REG_CONFIG_MUTEX0_GET));*/
    DDPDUMP("(0x028)M0_RST    =0x%x\n", DISP_REG_GET(DISP_REG_CONFIG_MUTEX0_RST));
    DDPDUMP("(0x02c)M0_SOF    =0x%x\n", DISP_REG_GET(DISP_REG_CONFIG_MUTEX0_SOF));
    DDPDUMP("(0x030)M0_MOD0    =0x%x\n", DISP_REG_GET(DISP_REG_CONFIG_MUTEX0_MOD0));
    /*DDPDUMP("(0x034)M0_MOD1    =0x%x\n", DISP_REG_GET(DISP_REG_CONFIG_MUTEX0_MOD1));*/
    DDPDUMP("(0x040)M1_EN     =0x%x\n", DISP_REG_GET(DISP_REG_CONFIG_MUTEX1_EN));
    /*DDPDUMP("(0x044)M1_GET     =0x%x\n", DISP_REG_GET(DISP_REG_CONFIG_MUTEX1_GET));*/
    DDPDUMP("(0x048)M1_RST    =0x%x\n", DISP_REG_GET(DISP_REG_CONFIG_MUTEX1_RST));
    DDPDUMP("(0x04c)M1_SOF    =0x%x\n", DISP_REG_GET(DISP_REG_CONFIG_MUTEX1_SOF));
    DDPDUMP("(0x050)M1_MOD0    =0x%x\n", DISP_REG_GET(DISP_REG_CONFIG_MUTEX1_MOD0));
    /*DDPDUMP("(0x054)M1_MOD1    =0x%x\n", DISP_REG_GET(DISP_REG_CONFIG_MUTEX1_MOD1));*/
    DDPDUMP("(0x060)M2_EN     =0x%x\n", DISP_REG_GET(DISP_REG_CONFIG_MUTEX2_EN));
    /*DDPDUMP("(0x064)M2_GET     =0x%x\n", DISP_REG_GET(DISP_REG_CONFIG_MUTEX2_GET));*/
    DDPDUMP("(0x068)M2_RST    =0x%x\n", DISP_REG_GET(DISP_REG_CONFIG_MUTEX2_RST));
    DDPDUMP("(0x06c)M2_SOF    =0x%x\n", DISP_REG_GET(DISP_REG_CONFIG_MUTEX2_SOF));
    DDPDUMP("(0x070)M2_MOD0    =0x%x\n", DISP_REG_GET(DISP_REG_CONFIG_MUTEX2_MOD0));
    /*DDPDUMP("(0x074)M2_MOD1    =0x%x\n", DISP_REG_GET(DISP_REG_CONFIG_MUTEX2_MOD1));*/
    DDPDUMP("(0x080)M3_EN     =0x%x\n", DISP_REG_GET(DISP_REG_CONFIG_MUTEX3_EN));
    /*DDPDUMP("(0x084)M3_GET     =0x%x\n", DISP_REG_GET(DISP_REG_CONFIG_MUTEX3_GET));*/
    DDPDUMP("(0x088)M3_RST    =0x%x\n", DISP_REG_GET(DISP_REG_CONFIG_MUTEX3_RST));
    DDPDUMP("(0x08C)M3_SOF    =0x%x\n", DISP_REG_GET(DISP_REG_CONFIG_MUTEX3_SOF));
    DDPDUMP("(0x090)M3_MOD0    =0x%x\n", DISP_REG_GET(DISP_REG_CONFIG_MUTEX3_MOD0));
    /*DDPDUMP("(0x090)M3_MOD1    =0x%x\n", DISP_REG_GET(DISP_REG_CONFIG_MUTEX3_MOD1));*/
    DDPDUMP("(0x0a0)M4_EN     =0x%x\n", DISP_REG_GET(DISP_REG_CONFIG_MUTEX4_EN));
    /*DDPDUMP("(0x0a4)M4_GET     =0x%x\n", DISP_REG_GET(DISP_REG_CONFIG_MUTEX4_GET));*/
    DDPDUMP("(0x0a8)M4_RST    =0x%x\n", DISP_REG_GET(DISP_REG_CONFIG_MUTEX4_RST));
    DDPDUMP("(0x0ac)M4_SOF    =0x%x\n", DISP_REG_GET(DISP_REG_CONFIG_MUTEX4_SOF));
    DDPDUMP("(0x0b0)M4_MOD0    =0x%x\n", DISP_REG_GET(DISP_REG_CONFIG_MUTEX4_MOD0));
    /*DDPDUMP("(0x0b4)M4_MOD1    =0x%x\n", DISP_REG_GET(DISP_REG_CONFIG_MUTEX4_MOD1));*/
    DDPDUMP("(0x0c0)M5_EN     =0x%x\n", DISP_REG_GET(DISP_REG_CONFIG_MUTEX5_EN));
    /*DDPDUMP("(0x0c4)M5_GET     =0x%x\n", DISP_REG_GET(DISP_REG_CONFIG_MUTEX5_GET));*/
    DDPDUMP("(0x0c8)M5_RST    =0x%x\n", DISP_REG_GET(DISP_REG_CONFIG_MUTEX5_RST));
    DDPDUMP("(0x0cc)M5_SOF    =0x%x\n", DISP_REG_GET(DISP_REG_CONFIG_MUTEX5_SOF));
    DDPDUMP("(0x0d0)M5_MOD0    =0x%x\n", DISP_REG_GET(DISP_REG_CONFIG_MUTEX5_MOD0));
    /*  DDPDUMP("(0x0d4)M5_MOD1    =0x%x\n", DISP_REG_GET(DISP_REG_CONFIG_MUTEX5_MOD1));
        DDPDUMP("(0x300)DISP_REG_CONFIG_MUTEX_DUMMY0 =0x%x\n", DISP_REG_GET(DISP_REG_CONFIG_MUTEX_DUMMY0));
        DDPDUMP("(0x304)DISP_REG_CONFIG_MUTEX_DUMMY1 =0x%x\n", DISP_REG_GET(DISP_REG_CONFIG_MUTEX_DUMMY1));
    */
    DDPDUMP("(0x30c)DEBUG_OUT_SEL =0x%x\n", DISP_REG_GET(DISP_REG_CONFIG_DEBUG_OUT_SEL));
    return;
}


static void mutex_dump_analysis(void)
{
    int i = 0;
    int j = 0;
    char mutex_module[512] = { '\0' };
    char *p = NULL;
    int len = 0;
    unsigned int val;

    DDPDUMP("== DISP Mutex Analysis ==\n");
    for (i = 0; i < 5; i++) {
        p = mutex_module;
        len = 0;
        if (DISP_REG_GET(DISP_REG_CONFIG_MUTEX_MOD0(i)) == 0)
            continue;

        val = DISP_REG_GET(DISP_REG_CONFIG_MUTEX_SOF(i));
        len = sprintf(p, "MUTEX%d :SOF=%s,EOF=%s,WAIT=%d,module=(",
                      i,
                      ddp_get_mutex_sof_name(REG_FLD_VAL_GET(SOF_FLD_MUTEX0_SOF, val)),
                      ddp_get_mutex_sof_name(REG_FLD_VAL_GET(SOF_FLD_MUTEX0_EOF, val)),
                      REG_FLD_VAL_GET(SOF_FLD_MUTEX0_SOF_WAIT, val));

        p += len;
        for (j = 0; j < 32; j++) {
            unsigned int regval = DISP_REG_GET(DISP_REG_CONFIG_MUTEX_MOD0(i));

            if ((regval & (1 << j))) {
                len = sprintf(p, "%s,", ddp_get_mutex_module0_name(j));
                p += len;
            }
        }
        /*      for (j = 0; j < 32; j++) {
                    unsigned int regval = DISP_REG_GET(DISP_REG_CONFIG_MUTEX_MOD1(i));

                    if ((regval & (1 << j))) {
                        len = sprintf(p, "%s,", ddp_get_mutex_module1_name(j));
                        p += len;
                    }
                }
        */
        DDPDUMP("%s)\n", mutex_module);
    }
}

static void mmsys_config_dump_reg(void)
{
    DDPDUMP("== DISP MMSYS_Config REGS ==\n");
    DDPDUMP("MMSYS_INTEN=0x%x\n", DISP_REG_GET(DISP_REG_CONFIG_MMSYS_INTEN));
    DDPDUMP("MMSYS_INTSTA=0x%x\n", DISP_REG_GET(DISP_REG_CONFIG_MMSYS_INTSTA));
    DDPDUMP("OVL0_MOUT_EN=0x%x\n", DISP_REG_GET(DISP_REG_CONFIG_DISP_OVL0_MOUT_EN));
    DDPDUMP("OVL0_2L_MOUT_EN=0x%x\n", DISP_REG_GET(DISP_REG_CONFIG_DISP_OVL0_2L_MOUT_EN));
    DDPDUMP("OVL1_2L_MOUT_EN=0x%x\n", DISP_REG_GET(DISP_REG_CONFIG_DISP_OVL1_2L_MOUT_EN));
    DDPDUMP("DITHER_MOUT_EN=0x%x\n", DISP_REG_GET(DISP_REG_CONFIG_DISP_DITHER0_MOUT_EN));
    DDPDUMP("RSZ_MOUT_EN=0x%x\n", DISP_REG_GET(DISP_REG_CONFIG_DISP_RSZ_MOUT_EN));
    DDPDUMP("MMSYS_MOUT_RST=0x%x\n", DISP_REG_GET(DISP_REG_CONFIG_MMSYS_MOUT_RST));
    DDPDUMP("COLOR0_SIN=0x%x\n", DISP_REG_GET(DISP_REG_CONFIG_DISP_COLOR_OUT_SEL_IN));
    DDPDUMP("WDMA0_SIN=0x%x\n", DISP_REG_GET(DISP_REG_CONFIG_DISP_WDMA0_SEL_IN));
    DDPDUMP("DSI0_SIN=0x%x\n", DISP_REG_GET(DISP_REG_CONFIG_DSI0_SEL_IN));
    DDPDUMP("DPI0_SIN=0x%x\n", DISP_REG_GET(DISP_REG_CONFIG_DPI0_SEL_IN));
    DDPDUMP("DISP_PATH_SOUT_SIN=0x%x\n", DISP_REG_GET(DISP_REG_CONFIG_DISP_PATH0_SEL_IN));
    DDPDUMP("RDMA0_SOUT_SIN=0x%x\n", DISP_REG_GET(DISP_REG_CONFIG_DISP_RDMA0_SOUT_SEL_IN));
    DDPDUMP("RDMA1_SOUT_SIN=0x%x\n", DISP_REG_GET(DISP_REG_CONFIG_DISP_RDMA1_SOUT_SEL_IN));
    DDPDUMP("WDMA0_SOUT_SIN=0x%x\n", DISP_REG_GET(DISP_REG_CONFIG_OVL_TO_WDMA_SEL_IN));
    DDPDUMP("OVL1_INT_SOUT_SIN=0x%x\n", DISP_REG_GET(DISP_REG_CONFIG_OVL_TO_WDMA_SEL_IN));

    DDPDUMP("MM_MISC=0x%x\n", DISP_REG_GET(DISP_REG_CONFIG_MMSYS_MISC));
    DDPDUMP("MM_CG_CON0=0x%x\n", DISP_REG_GET(DISP_REG_CONFIG_MMSYS_CG_CON0));
    DDPDUMP("MM_CG_CON1=0x%x\n", DISP_REG_GET(DISP_REG_CONFIG_MMSYS_CG_CON1));
    DDPDUMP("MM_HW_DCM_DIS0=0x%x\n", DISP_REG_GET(DISP_REG_CONFIG_MMSYS_HW_DCM_DIS0));
    DDPDUMP("MM_HW_DCM_DIS1=0x%x\n", DISP_REG_GET(DISP_REG_CONFIG_MMSYS_HW_DCM_DIS1));
    DDPDUMP("MM_SW0_RST_B=0x%x\n", DISP_REG_GET(DISP_REG_CONFIG_MMSYS_SW0_RST_B));
    DDPDUMP("MM_SW1_RST_B=0x%x\n", DISP_REG_GET(DISP_REG_CONFIG_MMSYS_SW1_RST_B));
    DDPDUMP("MM_LCM_RST_B=0x%x\n", DISP_REG_GET(DISP_REG_CONFIG_MMSYS_LCM_RST_B));
    DDPDUMP("MM_DBG_OUT_SEL=0x%x\n", DISP_REG_GET(DISP_REG_CONFIG_MMSYS_DEBUG_OUT_SEL));
    DDPDUMP("MM_DUMMY0=0x%x\n", DISP_REG_GET(DISP_REG_CONFIG_MMSYS_DUMMY0));
    DDPDUMP("MM_DUMMY1=0x%x\n", DISP_REG_GET(DISP_REG_CONFIG_MMSYS_DUMMY1));
    DDPDUMP("MM_DUMMY2=0x%x\n", DISP_REG_GET(DISP_REG_CONFIG_MMSYS_DUMMY2));
    DDPDUMP("MM_DUMMY3=0x%x\n", DISP_REG_GET(DISP_REG_CONFIG_MMSYS_DUMMY3));
    DDPDUMP("DISP_VALID_0=0x%x\n", DISP_REG_GET(DISP_REG_CONFIG_DISP_DL_VALID_0));
    DDPDUMP("DISP_VALID_1=0x%x\n", DISP_REG_GET(DISP_REG_CONFIG_DISP_DL_VALID_1));
    DDPDUMP("DISP_READY_0=0x%x\n", DISP_REG_GET(DISP_REG_CONFIG_DISP_DL_READY_0));
    DDPDUMP("DISP_READY_1=0x%x\n", DISP_REG_GET(DISP_REG_CONFIG_DISP_DL_READY_1));

}


/* ------ clock:
Before power on mmsys:
CLK_CFG_0_CLR (address is 0x10000048) = 0x80000000 (bit 31).
Before using DISP_PWM0 or DISP_PWM1:
CLK_CFG_1_CLR(address is 0x10000058)=0x80 (bit 7).
Before using DPI pixel clock:
CLK_CFG_6_CLR(address is 0x100000A8)=0x80 (bit 7).

Only need to enable the corresponding bits of MMSYS_CG_CON0 and MMSYS_CG_CON1 for the modules:
smi_common, larb0, mdp_crop, fake_eng, mutex_32k, pwm0, pwm1, dsi0, dsi1, dpi.
Other bits could keep 1. Suggest to keep smi_common and larb0 always clock on.

--------valid & ready
example:
ovl0 -> ovl0_mout_ready=1 means engines after ovl_mout are ready for receiving data
    ovl0_mout_ready=0 means ovl0_mout can not receive data, maybe ovl0_mout or after engines config error
ovl0 -> ovl0_mout_valid=1 means engines before ovl0_mout is OK,
    ovl0_mout_valid=0 means ovl can not transfer data to ovl0_mout, means ovl0 or before engines are not ready.
*/

static void mmsys_config_dump_analysis(void)
{
    unsigned int i = 0, j;
    unsigned int reg = 0;
    char clock_on[512] = { '\0' };
    char *pos = NULL;
    const char *name;
    /* int len = 0; */
    unsigned int valid[5], ready[5];
    unsigned int greq;
    unsigned int array_size = (sizeof(valid) / sizeof(valid[0]));

    valid[0] = DISP_REG_GET(DISP_REG_CONFIG_DISP_DL_VALID_0);
    valid[1] = DISP_REG_GET(DISP_REG_CONFIG_DISP_DL_VALID_1);
    valid[2] = DISP_REG_GET(DISP_REG_CONFIG_DISP_DL_VALID_2);
    valid[3] = DISP_REG_GET(DISP_REG_CONFIG_DISP_DL_VALID_3);
    valid[4] = DISP_REG_GET(DISP_REG_CONFIG_DISP_DL_VALID_4);
    ready[0] = DISP_REG_GET(DISP_REG_CONFIG_DISP_DL_READY_0);
    ready[1] = DISP_REG_GET(DISP_REG_CONFIG_DISP_DL_READY_1);
    ready[2] = DISP_REG_GET(DISP_REG_CONFIG_DISP_DL_READY_2);
    ready[3] = DISP_REG_GET(DISP_REG_CONFIG_DISP_DL_READY_3);
    ready[4] = DISP_REG_GET(DISP_REG_CONFIG_DISP_DL_READY_4);
    greq = DISP_REG_GET(DISP_REG_CONFIG_SMI_LARB_GREQ);

    DDPDUMP("== DISP MMSYS_CONFIG ANALYSIS ==\n");
#if 0 /* TODO: mmsys clk?? */
    DDPDUMP("mmsys clock=0x%x, CG_CON0=0x%x, CG_CON1=0x%x\n",
            DISP_REG_GET(DISP_REG_CLK_CFG_0_MM_CLK),
            DISP_REG_GET(DISP_REG_CONFIG_MMSYS_CG_CON0),
            DISP_REG_GET(DISP_REG_CONFIG_MMSYS_CG_CON1));
    if ((DISP_REG_GET(DISP_REG_CLK_CFG_0_MM_CLK) >> 31) & 0x1)
        DDPERR("mmsys clock abnormal!!\n");
#endif
    DDPDUMP("MMSYS_CG_CON0: 0x%08x\n", readl(DISP_REG_CONFIG_MMSYS_CG_CON0));
    DDPDUMP("MMSYS_CG_CON1: 0x%08x\n", readl(DISP_REG_CONFIG_MMSYS_CG_CON1));

    reg = DISP_REG_GET(DISP_REG_CONFIG_MMSYS_CG_CON0);
    for (i = 0; i < 32; i++) {
        if ((reg & (1 << i)) == 0) {
            name = ddp_clock_0(i);
            if (name)
                strncat(clock_on, name, (sizeof(clock_on) - strlen(clock_on) - 1));
        }
    }

    reg = DISP_REG_GET(DISP_REG_CONFIG_MMSYS_CG_CON1);
    for (i = 0; i < 32; i++) {
        if ((reg & (1 << i)) == 0) {
            name = ddp_clock_1(i);
            if (name)
                strncat(clock_on, name, (sizeof(clock_on) - strlen(clock_on) - 1));
        }
    }
    DDPDUMP("clock on modules:%s\n", clock_on);

    DDPDUMP("valid0=0x%x,valid1=0x%x,valid2=0x%x,ready0=0x%x,ready1=0x%x,ready2=0x%x,greq=0%x\n",
            valid[0], valid[1], valid[2], ready[0], ready[1], ready[2], greq);

    for (j = 0; j < array_size; j++) {
        for (i = 0; i < 32; i++) {
            name = ddp_signal[j][i];
            if (!name)
                continue;

            pos = clock_on;

            if ((valid[j] & (1 << i)))
                pos += snprintf(pos, (sizeof(clock_on) - strlen(clock_on) - 1), "%s,", "v");
            else
                pos += snprintf(pos, (sizeof(clock_on) - strlen(clock_on) - 1), "%s,", "n");

            if ((ready[j] & (1 << i)))
                pos += snprintf(pos, (sizeof(clock_on) - strlen(clock_on) - 1), "%s", "r");
            else
                pos += snprintf(pos, (sizeof(clock_on) - strlen(clock_on) - 1), "%s", "n");

            pos += snprintf(pos, (sizeof(clock_on) - strlen(clock_on) - 1), ": %s", name);

            DDPDUMP("%s\n", clock_on);
        }
    }

    /* greq: 1 means SMI dose not grant, maybe SMI hang */
    if (greq)
        DDPDUMP("smi greq not grant module: (greq: 1 means SMI dose not grant, maybe SMI hang)");

    clock_on[0] = '\0';
    for (i = 0; i < 32; i++) {
        if (greq & (1 << i)) {
            name = ddp_greq_name(i);
            if (!name)
                continue;
            strncat(clock_on, name, (sizeof(clock_on) - strlen(clock_on) - 1));
        }
    }
    DDPDUMP("%s\n", clock_on);
}

static void gamma_dump_reg(void)
{
    DDPDUMP("== DISP GAMMA REGS ==\n");
    DDPDUMP("(0x000)GA_EN=0x%x\n", DISP_REG_GET(DISP_REG_GAMMA_EN));
    DDPDUMP("(0x004)GA_RESET=0x%x\n", DISP_REG_GET(DISP_REG_GAMMA_RESET));
    DDPDUMP("(0x008)GA_INTEN=0x%x\n", DISP_REG_GET(DISP_REG_GAMMA_INTEN));
    DDPDUMP("(0x00c)GA_INTSTA=0x%x\n", DISP_REG_GET(DISP_REG_GAMMA_INTSTA));
    DDPDUMP("(0x010)GA_STATUS=0x%x\n", DISP_REG_GET(DISP_REG_GAMMA_STATUS));
    DDPDUMP("(0x020)GA_CFG=0x%x\n", DISP_REG_GET(DISP_REG_GAMMA_CFG));
    DDPDUMP("(0x024)GA_IN_COUNT=0x%x\n", DISP_REG_GET(DISP_REG_GAMMA_INPUT_COUNT));
    DDPDUMP("(0x028)GA_OUT_COUNT=0x%x\n", DISP_REG_GET(DISP_REG_GAMMA_OUTPUT_COUNT));
    DDPDUMP("(0x02c)GA_CHKSUM=0x%x\n", DISP_REG_GET(DISP_REG_GAMMA_CHKSUM));
    DDPDUMP("(0x030)GA_SIZE=0x%x\n", DISP_REG_GET(DISP_REG_GAMMA_SIZE));
    DDPDUMP("(0x0c0)GA_DUMMY_REG=0x%x\n", DISP_REG_GET(DISP_REG_GAMMA_DUMMY_REG));
    DDPDUMP("(0x800)GA_LUT=0x%x\n", DISP_REG_GET(DISP_REG_GAMMA_LUT));
}

static void gamma_dump_analysis(void)
{
    DDPDUMP("== DISP GAMMA ANALYSIS ==\n");
    DDPDUMP("gamma: en=%d, w=%d, h=%d, in_p_cnt=%d, in_l_cnt=%d, out_p_cnt=%d, out_l_cnt=%d\n",
            DISP_REG_GET(DISP_REG_GAMMA_EN),
            (DISP_REG_GET(DISP_REG_GAMMA_SIZE) >> 16) & 0x1fff,
            DISP_REG_GET(DISP_REG_GAMMA_SIZE) & 0x1fff,
            DISP_REG_GET(DISP_REG_GAMMA_INPUT_COUNT) & 0x1fff,
            (DISP_REG_GET(DISP_REG_GAMMA_INPUT_COUNT) >> 16) & 0x1fff,
            DISP_REG_GET(DISP_REG_GAMMA_OUTPUT_COUNT) & 0x1fff,
            (DISP_REG_GET(DISP_REG_GAMMA_OUTPUT_COUNT) >> 16) & 0x1fff);
}

static void color_dump_reg(DISP_MODULE_ENUM module)
{
    int index = 0;

    if (DISP_MODULE_COLOR0 == module) {
        index = 0;
    } else if (DISP_MODULE_COLOR1 == module) {
        DDPDUMP("error: DISP COLOR%d dose not exist!\n", index);
        return;
    }
    DDPDUMP("== DISP COLOR%d REGS ==\n", index);
    DDPDUMP("(0x400)COLOR_CFG_MAIN=0x%x\n", DISP_REG_GET(DISP_COLOR_CFG_MAIN));
    DDPDUMP("(0x404)COLOR_PXL_CNT_MAIN=0x%x\n", DISP_REG_GET(DISP_COLOR_PXL_CNT_MAIN));
    DDPDUMP("(0x408)COLOR_LINE_CNT_MAIN=0x%x\n", DISP_REG_GET(DISP_COLOR_LINE_CNT_MAIN));
    DDPDUMP("(0xc00)COLOR_START=0x%x\n", DISP_REG_GET(DISP_COLOR_START));
    DDPDUMP("(0xc28)DISP_COLOR_CK_ON=0x%x\n", DISP_REG_GET(DISP_COLOR_CK_ON));
    DDPDUMP("(0xc50)COLOR_INTER_IP_W=0x%x\n", DISP_REG_GET(DISP_COLOR_INTERNAL_IP_WIDTH));
    DDPDUMP("(0xc54)COLOR_INTER_IP_H=0x%x\n", DISP_REG_GET(DISP_COLOR_INTERNAL_IP_HEIGHT));
    return;
}

static void color_dump_analysis(DISP_MODULE_ENUM module)
{
    int index = 0;

    if (DISP_MODULE_COLOR0 == module) {
        index = 0;
    } else if (DISP_MODULE_COLOR1 == module) {
        DDPDUMP("error: DISP COLOR%d dose not exist!\n", index);
        return;
    }
    DDPDUMP("== DISP COLOR%d ANALYSIS ==\n", index);
    DDPDUMP("color%d: bypass=%d, w=%d, h=%d, pixel_cnt=%d, line_cnt=%d,\n",
            index,
            (DISP_REG_GET(DISP_COLOR_CFG_MAIN) >> 7) & 0x1,
            DISP_REG_GET(DISP_COLOR_INTERNAL_IP_WIDTH),
            DISP_REG_GET(DISP_COLOR_INTERNAL_IP_HEIGHT),
            DISP_REG_GET(DISP_COLOR_PXL_CNT_MAIN) & 0xffff,
            (DISP_REG_GET(DISP_COLOR_LINE_CNT_MAIN) >> 16) & 0x1fff);

    return;
}

static void aal_dump_reg(void)
{
    DDPDUMP("== DISP AAL REGS ==\n");
    DDPDUMP("(0x000)AAL_EN=0x%x\n", DISP_REG_GET(DISP_AAL_EN));
    DDPDUMP("(0x008)AAL_INTEN=0x%x\n", DISP_REG_GET(DISP_AAL_INTEN));
    DDPDUMP("(0x00c)AAL_INTSTA=0x%x\n", DISP_REG_GET(DISP_AAL_INTSTA));
    DDPDUMP("(0x020)AAL_CFG=0x%x\n", DISP_REG_GET(DISP_AAL_CFG));
    DDPDUMP("(0x024)AAL_IN_CNT=0x%x\n", DISP_REG_GET(DISP_AAL_IN_CNT));
    DDPDUMP("(0x028)AAL_OUT_CNT=0x%x\n", DISP_REG_GET(DISP_AAL_OUT_CNT));
    DDPDUMP("(0x030)AAL_SIZE=0x%x\n", DISP_REG_GET(DISP_AAL_SIZE));
    DDPDUMP("(0x20c)AAL_CABC_00=0x%x\n", DISP_REG_GET(DISP_AAL_CABC_00));
    DDPDUMP("(0x214)AAL_CABC_02=0x%x\n", DISP_REG_GET(DISP_AAL_CABC_02));
    DDPDUMP("(0x20c)AAL_STATUS_00=0x%x\n", DISP_REG_GET(DISP_AAL_STATUS_00));
    DDPDUMP("(0x210)AAL_STATUS_01=0x%x\n", DISP_REG_GET(DISP_AAL_STATUS_00 + 0x4));
    DDPDUMP("(0x2a0)AAL_STATUS_31=0x%x\n", DISP_REG_GET(DISP_AAL_STATUS_32 - 0x4));
    DDPDUMP("(0x2a4)AAL_STATUS_32=0x%x\n", DISP_REG_GET(DISP_AAL_STATUS_32));
    DDPDUMP("(0x3b0)AAL_DRE_MAPPING_00=0x%x\n", DISP_REG_GET(DISP_AAL_DRE_MAPPING_00));
    return;
}

static void aal_dump_analysis(void)
{
    DDPDUMP("== DISP AAL ANALYSIS ==\n");
    DDPDUMP("aal: bypass=%d, relay=%d, en=%d, w=%d, h=%d, in(%d,%d),out(%d,%d)\n",
            DISP_REG_GET(DISP_AAL_EN) == 0x0,
            DISP_REG_GET(DISP_AAL_CFG) & 0x01,
            DISP_REG_GET(DISP_AAL_EN),
            (DISP_REG_GET(DISP_AAL_SIZE) >> 16) & 0x1fff,
            DISP_REG_GET(DISP_AAL_SIZE) & 0x1fff,
            DISP_REG_GET(DISP_AAL_IN_CNT) & 0x1fff,
            (DISP_REG_GET(DISP_AAL_IN_CNT) >> 16) & 0x1fff,
            DISP_REG_GET(DISP_AAL_OUT_CNT) & 0x1fff,
            (DISP_REG_GET(DISP_AAL_OUT_CNT) >> 16) & 0x1fff);
}

static void pwm_dump_reg(DISP_MODULE_ENUM module)
{
    int index = 0;
    unsigned long reg_base = 0;

    if (module == DISP_MODULE_PWM0) {
        index = 0;
        reg_base = DISPSYS_PWM0_BASE;
    } else {
        index = 1;
        reg_base = DISPSYS_PWM0_BASE;
    }
    DDPDUMP("== DISP PWM%d REGS ==\n", index);
    DDPDUMP("(0x000)PWM_EN=0x%x\n", DISP_REG_GET(reg_base + DISP_PWM_EN_OFF));
    DDPDUMP("(0x008)PWM_CON_0=0x%x\n", DISP_REG_GET(reg_base + DISP_PWM_CON_0_OFF));
    DDPDUMP("(0x010)PWM_CON_1=0x%x\n", DISP_REG_GET(reg_base + DISP_PWM_CON_1_OFF));
    DDPDUMP("(0x028)PWM_DEBUG=0x%x\n", DISP_REG_GET(reg_base + 0x28));
    return;
}

static void pwm_dump_analysis(DISP_MODULE_ENUM module)
{
    int index = 0;
    //unsigned int reg_base = 0;

    if (module == DISP_MODULE_PWM0) {
        index = 0;
        //reg_base = DISPSYS_PWM0_BASE;
    } else {
        index = 1;
        //reg_base = DISPSYS_PWM0_BASE;
    }
    DDPDUMP("== DISP PWM%d ANALYSIS ==\n", index);
#if 0 /* TODO: clk reg?? */
    DDPDUMP("pwm clock=%d\n", (DISP_REG_GET(DISP_REG_CLK_CFG_1_CLR) >> 7) & 0x1);
#endif

    return;
}

static void ccorr_dump_reg(void)
{
    DDPDUMP("== DISP CCORR REGS ==\n");
    DDPDUMP("(00)EN=0x%x\n", DISP_REG_GET(DISP_REG_CCORR_EN));
    DDPDUMP("(20)CFG=0x%x\n", DISP_REG_GET(DISP_REG_CCORR_CFG));
    DDPDUMP("(24)IN_CNT=0x%x\n", DISP_REG_GET(DISP_REG_CCORR_IN_CNT));
    DDPDUMP("(28)OUT_CNT=0x%x\n", DISP_REG_GET(DISP_REG_CCORR_OUT_CNT));
    DDPDUMP("(30)SIZE=0x%x\n", DISP_REG_GET(DISP_REG_CCORR_SIZE));

}

static void ccorr_dump_analyze(void)
{
    DDPDUMP("ccorr: en=%d, config=%d, w=%d, h=%d, in_p_cnt=%d, in_l_cnt=%d, out_p_cnt=%d, out_l_cnt=%d\n",
            DISP_REG_GET(DISP_REG_CCORR_EN), DISP_REG_GET(DISP_REG_CCORR_CFG),
            (DISP_REG_GET(DISP_REG_CCORR_SIZE) >> 16) & 0x1fff,
            DISP_REG_GET(DISP_REG_CCORR_SIZE) & 0x1fff,
            DISP_REG_GET(DISP_REG_CCORR_IN_CNT) & 0x1fff,
            (DISP_REG_GET(DISP_REG_CCORR_IN_CNT) >> 16) & 0x1fff,
            DISP_REG_GET(DISP_REG_CCORR_IN_CNT) & 0x1fff,
            (DISP_REG_GET(DISP_REG_CCORR_IN_CNT) >> 16) & 0x1fff);
}

static void dither_dump_reg(void)
{
    DDPDUMP("== DISP DITHER REGS ==\n");
    DDPDUMP("(00)EN=0x%x\n", DISP_REG_GET(DISP_REG_DITHER_EN));
    DDPDUMP("(20)CFG=0x%x\n", DISP_REG_GET(DISP_REG_DITHER_CFG));
    DDPDUMP("(24)IN_CNT=0x%x\n", DISP_REG_GET(DISP_REG_DITHER_IN_CNT));
    DDPDUMP("(28)OUT_CNT=0x%x\n", DISP_REG_GET(DISP_REG_DITHER_OUT_CNT));
    DDPDUMP("(30)SIZE=0x%x\n", DISP_REG_GET(DISP_REG_DITHER_SIZE));
}

static void dither_dump_analyze(void)
{
    DDPDUMP("dither: en=%d, config=%d, w=%d, h=%d, in_p_cnt=%d, in_l_cnt=%d, out_p_cnt=%d, out_l_cnt=%d\n",
            DISP_REG_GET(DISPSYS_DITHER0_BASE + 0x000), DISP_REG_GET(DISPSYS_DITHER0_BASE + 0x020),
            (DISP_REG_GET(DISP_REG_DITHER_SIZE) >> 16) & 0x1fff,
            DISP_REG_GET(DISP_REG_DITHER_SIZE) & 0x1fff,
            DISP_REG_GET(DISP_REG_DITHER_IN_CNT) & 0x1fff,
            (DISP_REG_GET(DISP_REG_DITHER_IN_CNT) >> 16) & 0x1fff,
            DISP_REG_GET(DISP_REG_DITHER_OUT_CNT) & 0x1fff,
            (DISP_REG_GET(DISP_REG_DITHER_OUT_CNT) >> 16) & 0x1fff);

}

static void dsi_dump_reg(DISP_MODULE_ENUM module)
{
    DSI_DumpRegisters(module, 1);
#if 0
    if (DISP_MODULE_DSI0) {
        int i = 0;

        DDPDUMP("== DISP DSI0 REGS ==\n");
        for (i = 0; i < 25 * 16; i += 16) {
            DDPDUMP("DSI0+%04x: 0x%08x 0x%08x 0x%08x 0x%08x\n", i,
                    readl(DISPSYS_DSI0_BASE + i), readl(DISPSYS_DSI0_BASE + i + 0x4),
                    readl(DISPSYS_DSI0_BASE + i + 0x8),
                    readl(DISPSYS_DSI0_BASE + i + 0xc));
        }
        DDPDUMP("DSI0 CMDQ+0x200: 0x%08x 0x%08x 0x%08x 0x%08x\n",
                readl(DISPSYS_DSI0_BASE + 0x200), readl(DISPSYS_DSI0_BASE + 0x200 + 0x4),
                readl(DISPSYS_DSI0_BASE + 0x200 + 0x8),
                readl(DISPSYS_DSI0_BASE + 0x200 + 0xc));
    }
#endif
}

static void dpi_dump_reg(void)
{
    int i;

    DDPDUMP("-- Start dump DPI registers --\n");

    for (i = 0; i <= 0x40; i += 4)
        DDPDUMP("DPI+%04x: 0x%08x\n", i, readl(DISPSYS_DPI_BASE + i));

    for (i = 0x68; i <= 0x7C; i += 4)
        DDPDUMP("DPI+%04x: 0x%08x\n", i, readl(DISPSYS_DPI_BASE + i));

    DDPDUMP("DPI+Color Bar    : 0x%04x : 0x%08x\n", 0xF00, readl(DISPSYS_DPI_BASE + 0xF00));
    DDPDUMP("DPI MMSYS_CG_CON0: 0x%08x\n", readl(DISP_REG_CONFIG_MMSYS_CG_CON0));
    DDPDUMP("DPI MMSYS_CG_CON1: 0x%08x\n", readl(DISP_REG_CONFIG_MMSYS_CG_CON1));
}

static void dpi_dump_analysis(void)
{
#if 0
    DDPDUMP("== DISP DPI ANALYSIS ==\n");
    /* TODO: mmsys clk?? */
    DDPDUMP("DPI clock=0x%x\n", DISP_REG_GET(DISP_REG_CLK_CFG_6_DPI));
    DDPDUMP("DPI  clock_clear=%d\n", (DISP_REG_GET(DISP_REG_CLK_CFG_6_CLR) >> 7) & 0x1);
#endif
}

int ddp_dump_reg(DISP_MODULE_ENUM module)
{
    switch (module) {
    case DISP_MODULE_WDMA0:
        //wdma_dump_reg(module);
        break;
    case DISP_MODULE_RDMA0:
    case DISP_MODULE_RDMA1:
    case DISP_MODULE_RDMA2:
        RDMADump(module);
        break;
    case DISP_MODULE_OVL0:
    case DISP_MODULE_OVL1:
    case DISP_MODULE_OVL0_2L:
    case DISP_MODULE_OVL1_2L:
        OVLDump(module);
        break;
    case DISP_MODULE_GAMMA0:
        gamma_dump_reg();
        break;
    case DISP_MODULE_CONFIG:
        mmsys_config_dump_reg();
        break;
    case DISP_MODULE_MUTEX:
        mutex_dump_reg();
        break;
    case DISP_MODULE_COLOR0:
    case DISP_MODULE_COLOR1:
        color_dump_reg(module);
        break;
    case DISP_MODULE_AAL0:
        aal_dump_reg();
        break;
    case DISP_MODULE_PWM0:
        pwm_dump_reg(module);
        break;
    case DISP_MODULE_DSI0:
    case DISP_MODULE_DSI1:
    case DISP_MODULE_DSIDUAL:
        dsi_dump_reg(module);
        break;
    case DISP_MODULE_DPI:
        dpi_dump_reg();
        break;
    case DISP_MODULE_CCORR0:
        ccorr_dump_reg();
        break;
    case DISP_MODULE_DITHER0:
        dither_dump_reg();
        break;
    case DISP_MODULE_POSTMASK:
        postmask_dump_reg(module);
        break;
    default:
        DDPDUMP("no dump_reg for module %s(%d)\n", ddp_get_module_name(module), module);
    }
    return 0;
}

int ddp_dump_analysis(DISP_MODULE_ENUM module)
{
    switch (module) {
    case DISP_MODULE_WDMA0:
        //wdma_dump_analysis(module);
        break;
    case DISP_MODULE_RDMA0:
    case DISP_MODULE_RDMA1:
    case DISP_MODULE_RDMA2:
        rdma_dump_analysis(module);
        break;
    case DISP_MODULE_OVL0:
    case DISP_MODULE_OVL1:
    case DISP_MODULE_OVL0_2L:
    case DISP_MODULE_OVL1_2L:
        ovl_dump_analysis(module);
        break;
    case DISP_MODULE_GAMMA0:
        gamma_dump_analysis();
        break;
    case DISP_MODULE_CONFIG:
        mmsys_config_dump_analysis();
        break;
    case DISP_MODULE_MUTEX:
        mutex_dump_analysis();
        break;
    case DISP_MODULE_COLOR0:
    case DISP_MODULE_COLOR1:
        color_dump_analysis(module);
        break;
    case DISP_MODULE_AAL0:
        aal_dump_analysis();
        break;
    case DISP_MODULE_PWM0:
        pwm_dump_analysis(module);
        break;
    case DISP_MODULE_DSI0:
    case DISP_MODULE_DSI1:
    case DISP_MODULE_DSIDUAL:
        dsi_analysis(module);
        break;
    case DISP_MODULE_DPI:
        dpi_dump_analysis();
        break;
    case DISP_MODULE_CCORR0:
        ccorr_dump_analyze();
        break;
    case DISP_MODULE_DITHER0:
        dither_dump_analyze();
        break;
    case DISP_MODULE_POSTMASK:
        postmask_dump_analysis(module);
        break;
    default:
        DDPDUMP("no dump_analysis for module %s(%d)\n", ddp_get_module_name(module),
                module);
    }
    return 0;
}
