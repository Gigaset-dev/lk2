/*
 * Copyright (c) 2020 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#define LOG_TAG "DUMP"

#include "ddp_info.h"
#include "ddp_log.h"
#include "ddp_path.h"
#include "ddp_dump.h"
#include "ddp_rdma.h"
#include "disp_drv_platform.h"
#include "ddp_reg.h"
#include "ddp_dsi.h"
#include "ddp_postmask.h"


static const char *ddp_signal[6][32] = {
    {
        "afbc_wdma0_sel__to__disp_ufbc_wdma0",
        "disp_aal0__to__disp_gamma0",
        "disp_aal0_sel__to__disp_aal0",
        "disp_ccorr0__to__disp_ccorr0_sout",
        "disp_ccorr0_sout_out0__to__disp_mdp_aal4_sel_in0",
        "disp_ccorr0_sout_out1__to__disp_aal0_sel_in1",
        "disp_color0__to__disp_ccorr0",
        "disp_dither0__to__disp_dither0_mout",
        "disp_dither0_mout_out0__to__dsi0_sel_in1",
        "disp_dither0_mout_out1__to__disp_dsc_wrap0",
        "disp_dither0_mout_out2__to__disp_wdma0_sel_in0",
        "disp_dither0_mout_out3__to__afbc_wdma0_sel_in0",
        "disp_dsc_wrap0__to__dsi0_sel_in2",
        "disp_gamma0__to__disp_postmask0",
        "disp_mdp_aal4_mdp_hdr4_sel__to__mdp_rsz4",
        "disp_mdp_aal4_sel__to__mdp_aal4",
        "disp_mdp_aal4_sout_out0__to__disp_aal0_sel_in0",
        "disp_mdp_aal4_sout_out1__to__disp_mdp_aal4_mdp_hdr4_sel_in1",
        "disp_mdp_tdshp4_sout_out0__to__disp_ovl0_2l_ufod_sel_in1",
        "disp_mdp_tdshp4_sout_out1__to__disp_ovl0_ufod_sel_in1",
        "disp_mdp_tdshp4_sout_out2__to__disp_ovl0_2l_in1",
        "disp_mdp_tdshp4_sout_out3__to__disp_ovl0_in1",
        "disp_ovl0_2l_out0__to__disp_tovl0_out0_mout",
        "disp_ovl0_2l_out1__to__disp_ovl0_in0",
        "disp_ovl0_2l_ufod_sel__to__disp_ovl0_2l_in2",
        "disp_ovl0_out0__to__disp_tovl0_out1_mout",
        "disp_ovl0_out1__to__disp_ovl0_2l_in0",
        "disp_ovl0_ovl0_2l_pqout_sel__to__disp_tovl0_pqout_mdp_rdma4_sel_in1",
        "disp_ovl0_ufod_sel__to__disp_ovl0_in2",
        "disp_ovl2_2l__to__disp_ovl2_2l_out0_mout",
        "disp_ovl2_2l_out0_mout_out0__to__disp_rdma4",
        "disp_ovl2_2l_out0_mout_out1__to__disp_wdma0_sel_in4",
    },
    {
        "disp_ovl2_2l_out0_mout_out2__to__afbc_wdma0_sel_in4",
        "disp_postmask0__to__disp_dither0",
        "disp_rdma0__to__disp_rdma0_rsz0_sout",
        "disp_rdma0_rsz0_sout_out0__to__dsi0_sel_in0",
        "disp_rdma0_rsz0_sout_out1__to__disp_color0",
        "disp_rdma0_sel__to__disp_rdma0",
        "disp_rdma2_rsz0_rsz1_sout_out0__to__disp_ovl0_2l_ufod_sel_in0",
        "disp_rdma2_rsz0_rsz1_sout_out1__to__disp_ovl0_ufod_sel_in0",
        "disp_rdma4__to__disp_dpi0",
        "disp_rsz0__to__disp_rsz0_mout",
        "disp_rsz0_mout_out0__to__disp_rdma0_sel_in1",
        "disp_rsz0_mout_out1__to__disp_wdma0_sel_in1",
        "disp_rsz0_mout_out2__to__afbc_wdma0_sel_in1",
        "disp_rsz0_mout_out3__to__disp_rdma2_rsz0_rsz1_sout",
        "disp_rsz0_mout_out4__to__disp_tovl0_pqout_mdp_rdma4_sel_in2",
        "disp_rsz0_sel__to__disp_rsz0",
        "disp_tovl0_out0_mout_out0__to__disp_rdma0_sel_in3",
        "disp_tovl0_out0_mout_out1__to__disp_rsz0_sel_in0",
        "disp_tovl0_out0_mout_out2__to__disp_wdma0_sel_in2",
        "disp_tovl0_out0_mout_out3__to__afbc_wdma0_sel_in2",
        "disp_tovl0_out0_mout_out4__to__disp_ovl0_ovl0_2l_pqout_sel_in0",
        "disp_tovl0_out1_mout_out0__to__disp_rdma0_sel_in0",
        "disp_tovl0_out1_mout_out1__to__disp_rsz0_sel_in1",
        "disp_tovl0_out1_mout_out2__to__disp_wdma0_sel_in3",
        "disp_tovl0_out1_mout_out3__to__afbc_wdma0_sel_in3",
        "disp_tovl0_out1_mout_out4__to__disp__ovl0_ovl0_2l_pqout_sel_in1",
        "disp_tovl0_pqout_mdp_rdma4_sel__to__mdp_hdr4",
        "disp_wdma0_sel__to__disp_wdma0",
        "disp_y2r0__to__disp_y2r0_sout",
        "disp_y2r0_sout_out0__to__disp_mdp_tdshp4_sout",
        "disp_y2r0_sout_out1__to__disp_rdma0_sel_in2",
        "dsi0_sel__to__thp_lmt_dsi0",
    },
    {
        "mdp_aal4__to__disp_mdp_aal4_sout",
        "mdp_aal4_sout_out0__to__disp_mdp_aal4_mdp_hdr4_sel_in0",
        "mdp_aal4_sout_out1__to__disp_mdp_aal4_sel_in1",
        "mdp_color4__to__disp_y2r0",
        "mdp_hdr4__to__mdp_aal4_sout",
        "mdp_rmda4__to__disp_tovl0_pqout_mdp_rdma4_sel_in0",
        "mdp_rsz4__to__mdp_tdshp4",
        "mdp_tdshp4__to__mdp_color4",
        "thp_lmt_dsi0__to__dsi0",
    },
    {
    },
    {
    },
    {
    },
};

static const char *ddp_greq_name_larb0(int bit)
{
    switch (bit) {
    case 0:
        return "DISP_POSTMASK0 ";
    case 1:
        return "DISP_OVL0_HDR ";
    case 2:
        return "DISP_OVL0 ";
    case 3:
        return "DISP_RDMA0 ";
    case 4:
        return "DISP_WDMA0 ";
    case 5:
        return "DISP_FAKE_ENG0 ";
    default:
        return NULL;
    }
}

static const char *ddp_greq_name_larb1(int bit)
{
    switch (bit) {
    case 0:
        return "DISP_OVL0_2L_HDR ";
    case 1:
        return "DISP_OVL2_2L_HDR ";
    case 2:
        return "DISP_OVL0_2L ";
    case 3:
        return "DISP_OVL2_2L ";
    case 4:
        return "MDP_RDMA4 ";
    case 5:
        return "DISP_RDMA4 ";
    case 6:
        return "DISP_UFBC_WDMA0 ";
    case 7:
        return "DISP_FAKE_ENG1 ";
    default:
        return NULL;
    }
}

static const char *ddp_get_mutex_module0_name(unsigned int bit)
{
    switch (bit) {
    case 0:
        return "disp_ovl0";
    case 1:
        return "disp_ovl0_2l";
    case 2:
        return "disp_rdma0";
    case 3:
        return "disp_rsz0";
    case 4:
        return "disp_color0";
    case 5:
        return "disp_ccorr0";
    case 6:
        return "disp_aal0";
    case 7:
        return "disp_gamma0";
    case 8:
        return "disp_postmask0";
    case 9:
        return "disp_dither0";
    case 10:
        return "disp_dsc";
    case 11:
        return "disp_dsc_wrap0_core1";
    case 12:
        return "dsi0";
    case 13:
        return "disp_wdma0";
    case 14:
        return "disp_ufbc_wdma0";
    case 15:
        return "disp_pwm0";
    case 16:
        return "disp_ovl2_2l";
    case 17:
        return "disp_rdma4";
    case 18:
        return "dpi0";
    case 19:
        return "mdp_rdma4";
    case 20:
        return "mdp_hdr4";
    case 21:
        return "mdp_rsz4";
    case 22:
        return "mdp_aal4";
    case 23:
        return "mdp_tdshp4";
    case 24:
        return "mdp_color4";
    case 25:
        return "disp_y2r0";
    default:
        return "mutex-unknown";
    }
}

static const char *ddp_get_mutex_module1_name(unsigned int bit)
{
    switch (bit) {
    case 0:
        return "mdp_aal4";
    default:
        return "mutex-unknown";
    }
}

char const *ddp_get_fmt_name(enum DISP_MODULE_ENUM module, unsigned int fmt)
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
            DDPDUMP("%s, unknown fmt=%d, module=%d\n", __func__, fmt, module);
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
            DDPDUMP("%s unknown fmt=%d, module=%d\n", __func__, fmt, module);
            return "unknown";
        }
    } else if (module == DISP_MODULE_RDMA0) {
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
            DDPDUMP("%s, unknown fmt=%d, module=%d\n", __func__, fmt, module);
            return "unknown";
        }
    } else {
        DDPDUMP("%s, unknown module=%d\n", __func__, module);
    }

    return "unknown";
}

static const char *ddp_clock_0(int bit)
{
    switch (bit) {
    case 0:
        return "disp_mutex0, ";
    case 1:
        return "dispsys_config, ";
    case 2:
        return "disp_ovl0, ";
    case 3:
        return "disp_rdma0, ";
    case 4:
        return "disp_ovl0_2l, ";
    case 5:
        return "disp_wdma0, ";
    case 6:
        return "disp_ufbc_wdma0, ";
    case 7:
        return "disp_rsz0, ";
    case 8:
        return "disp_aal0, ";
    case 9:
        return "disp_ccorr0, ";
    case 10:
        return "disp_dither0, ";
    case 11:
        return "smi_infra, ";
    case 12:
        return "disp_gamma0, ";
    case 13:
        return "disp_postmask0, ";
    case 14:
        return "disp_dsc_wrap0, ";
    case 15:
        return "dsi0, ";
    case 16:
        return "disp_color0, ";
    case 17:
        return "smi_common, ";
    case 18:
        return "disp_fake_eng0, ";
    case 19:
        return "disp_fake_eng1, ";
    case 20:
        return "mdp_tdshp4, ";
    case 21:
        return "mdp_rsz4, ";
    case 22:
        return "mdp_aal4, ";
    case 23:
        return "mdp_hdr4, ";
    case 24:
        return "mdp_rdma4, ";
    case 25:
        return "mdp_color4, ";
    case 26:
        return "disp_y2r0, ";
    case 27:
        return "smi_gals, ";
    case 28:
        return "disp_ovl2_2l, ";
    case 29:
        return "disp_rdma4, ";
    case 30:
        return "disp_dpi0, ";
    default:
        return NULL;
    }
}

/* no need ddp_clock_1 */
static const char *ddp_clock_1(int bit)
{
    switch (bit) {
    case 0:
        return "smi_iommu, ";
    default:
        return NULL;
    }
}

static void mutex_dump_reg(void)
{
    unsigned long module_base = DISPSYS_MUTEX_BASE;

    DDPDUMP("== START: DISP MUTEX registers ==\n");
    DDPDUMP("MUTEX: 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x\n",
        0x0, readl(module_base + 0x0),
        0x4, readl(module_base + 0x4),
        0x8, readl(module_base + 0x8),
        0xC, readl(module_base + 0xC));
    DDPDUMP("MUTEX: 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x\n",
        0x10, readl(module_base + 0x10),
        0x18, readl(module_base + 0x18),
        0x1C, readl(module_base + 0x1C),
        0x020, readl(module_base + 0x020));
    DDPDUMP("MUTEX: 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x\n",
        0x024, readl(module_base + 0x024),
        0x028, readl(module_base + 0x028),
        0x02C, readl(module_base + 0x02C),
        0x030, readl(module_base + 0x030));
    DDPDUMP("MUTEX: 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x\n",
        0x040, readl(module_base + 0x040),
        0x044, readl(module_base + 0x044),
        0x048, readl(module_base + 0x048),
        0x04C, readl(module_base + 0x04C));
    DDPDUMP("MUTEX: 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x\n",
        0x050, readl(module_base + 0x050),
        0x060, readl(module_base + 0x060),
        0x064, readl(module_base + 0x064),
        0x068, readl(module_base + 0x068));
    DDPDUMP("MUTEX: 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x\n",
        0x06C, readl(module_base + 0x06C),
        0x070, readl(module_base + 0x070),
        0x080, readl(module_base + 0x080),
        0x084, readl(module_base + 0x084));
    DDPDUMP("MUTEX: 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x\n",
        0x088, readl(module_base + 0x088),
        0x08C, readl(module_base + 0x08C),
        0x090, readl(module_base + 0x090),
        0x0A0, readl(module_base + 0x0A0));
    DDPDUMP("MUTEX: 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x\n",
        0x0A4, readl(module_base + 0x0A4),
        0x0A8, readl(module_base + 0x0A8),
        0x0AC, readl(module_base + 0x0AC),
        0x0B0, readl(module_base + 0x0B0));
    DDPDUMP("MUTEX: 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x\n",
        0x0C0, readl(module_base + 0x0C0),
        0x0C4, readl(module_base + 0x0C4),
        0x0C8, readl(module_base + 0x0C8),
        0x0CC, readl(module_base + 0x0CC));
    DDPDUMP("MUTEX: 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x\n",
        0x0D0, readl(module_base + 0x0D0),
        0x0E0, readl(module_base + 0x0E0),
        0x0E4, readl(module_base + 0x0E4),
        0x0E8, readl(module_base + 0x0E8));
    DDPDUMP("MUTEX: 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x\n",
        0x0EC, readl(module_base + 0x0EC),
        0x0F0, readl(module_base + 0x0F0),
        0x100, readl(module_base + 0x100),
        0x104, readl(module_base + 0x104));
    DDPDUMP("MUTEX: 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x\n",
        0x108, readl(module_base + 0x108),
        0x10C, readl(module_base + 0x10C),
        0x110, readl(module_base + 0x110),
        0x120, readl(module_base + 0x120));
    DDPDUMP("MUTEX: 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x\n",
        0x124, readl(module_base + 0x124),
        0x128, readl(module_base + 0x128),
        0x12C, readl(module_base + 0x12C),
        0x130, readl(module_base + 0x130));
    DDPDUMP("MUTEX: 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x\n",
        0x140, readl(module_base + 0x140),
        0x144, readl(module_base + 0x144),
        0x148, readl(module_base + 0x148),
        0x14C, readl(module_base + 0x14C));
    DDPDUMP("MUTEX: 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x\n",
        0x150, readl(module_base + 0x150),
        0x160, readl(module_base + 0x160),
        0x164, readl(module_base + 0x164),
        0x168, readl(module_base + 0x168));
    DDPDUMP("MUTEX: 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x\n",
        0x16C, readl(module_base + 0x16C),
        0x170, readl(module_base + 0x170),
        0x180, readl(module_base + 0x180),
        0x184, readl(module_base + 0x184));
    DDPDUMP("MUTEX: 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x\n",
        0x188, readl(module_base + 0x188),
        0x18C, readl(module_base + 0x18C),
        0x190, readl(module_base + 0x190),
        0x300, readl(module_base + 0x300));
    DDPDUMP("MUTEX: 0x%04x=0x%08x, 0x%04x=0x%08x\n",
        0x304, readl(module_base + 0x304),
        0x30C, readl(module_base + 0x30C));
    DDPDUMP("-- END: DISP MUTEX registers --\n");

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

            regval = DISP_REG_GET(DISP_REG_CONFIG_MUTEX_MOD1(i));

            if ((regval & (1 << j))) {
                len = sprintf(p, "%s,", ddp_get_mutex_module1_name(j));
                p += len;
            }
        }
        DDPDUMP("%s)\n", mutex_module);
    }
}


static void mmsys_config_dump_reg(void)
{
    unsigned long module_base = DISPSYS_CONFIG_BASE;

    DDPDUMP("== START: DISP MMSYS_CONFIG registers ==\n");
    DDPDUMP("MMSYS_CONFIG: 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x\n",
        0x000, readl(module_base + 0x000),
        0x004, readl(module_base + 0x004),
        0x00C, readl(module_base + 0x00C),
        0x010, readl(module_base + 0x010));
    DDPDUMP("MMSYS_CONFIG: 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x\n",
        0x014, readl(module_base + 0x014),
        0x018, readl(module_base + 0x018),
        0x020, readl(module_base + 0x020),
        0x024, readl(module_base + 0x024));
    DDPDUMP("MMSYS_CONFIG: 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x\n",
        0x028, readl(module_base + 0x028),
        0x02C, readl(module_base + 0x02C),
        0x030, readl(module_base + 0x030),
        0x034, readl(module_base + 0x034));
    DDPDUMP("MMSYS_CONFIG: 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x\n",
        0x038, readl(module_base + 0x038),
        0x048, readl(module_base + 0x048),
        0x0F0, readl(module_base + 0x0F0),
        0x0F4, readl(module_base + 0x0F4));
    DDPDUMP("MMSYS_CONFIG: 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x\n",
        0x0F8, readl(module_base + 0x0F8),
        0x100, readl(module_base + 0x100),
        0x104, readl(module_base + 0x104),
        0x108, readl(module_base + 0x108));
    DDPDUMP("MMSYS_CONFIG: 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x\n",
        0x110, readl(module_base + 0x110),
        0x114, readl(module_base + 0x114),
        0x118, readl(module_base + 0x118),
        0x120, readl(module_base + 0x120));
    DDPDUMP("MMSYS_CONFIG: 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x\n",
        0x124, readl(module_base + 0x124),
        0x128, readl(module_base + 0x128),
        0x130, readl(module_base + 0x130),
        0x134, readl(module_base + 0x134));
    DDPDUMP("MMSYS_CONFIG: 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x\n",
        0x138, readl(module_base + 0x138),
        0x140, readl(module_base + 0x140),
        0x144, readl(module_base + 0x144),
        0x150, readl(module_base + 0x150));
    DDPDUMP("MMSYS_CONFIG: 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x\n",
        0x180, readl(module_base + 0x180),
        0x184, readl(module_base + 0x184),
        0x190, readl(module_base + 0x190),
        0x200, readl(module_base + 0x200));
    DDPDUMP("MMSYS_CONFIG: 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x\n",
        0x204, readl(module_base + 0x204),
        0x208, readl(module_base + 0x208),
        0x20C, readl(module_base + 0x20C),
        0x210, readl(module_base + 0x210));
    DDPDUMP("MMSYS_CONFIG: 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x\n",
        0x214, readl(module_base + 0x214),
        0x218, readl(module_base + 0x218),
        0x220, readl(module_base + 0x220),
        0x224, readl(module_base + 0x224));
    DDPDUMP("MMSYS_CONFIG: 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x\n",
        0x228, readl(module_base + 0x228),
        0x22C, readl(module_base + 0x22C),
        0x230, readl(module_base + 0x230),
        0x234, readl(module_base + 0x234));
    DDPDUMP("MMSYS_CONFIG: 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x\n",
        0x238, readl(module_base + 0x238),
        0x800, readl(module_base + 0x800),
        0x804, readl(module_base + 0x804),
        0x808, readl(module_base + 0x808));
    DDPDUMP("MMSYS_CONFIG: 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x\n",
        0x80C, readl(module_base + 0x80C),
        0x810, readl(module_base + 0x810),
        0x814, readl(module_base + 0x814),
        0x818, readl(module_base + 0x818));
    DDPDUMP("MMSYS_CONFIG: 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x\n",
        0x81C, readl(module_base + 0x81C),
        0x820, readl(module_base + 0x820),
        0x824, readl(module_base + 0x824),
        0x828, readl(module_base + 0x828));
    DDPDUMP("MMSYS_CONFIG: 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x\n",
        0x82C, readl(module_base + 0x82C),
        0x830, readl(module_base + 0x830),
        0x834, readl(module_base + 0x834),
        0x838, readl(module_base + 0x838));
    DDPDUMP("MMSYS_CONFIG: 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x\n",
        0x83C, readl(module_base + 0x83C),
        0x840, readl(module_base + 0x840),
        0x844, readl(module_base + 0x844),
        0x848, readl(module_base + 0x848));
    DDPDUMP("MMSYS_CONFIG: 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x\n",
        0x84C, readl(module_base + 0x84C),
        0x854, readl(module_base + 0x854),
        0x858, readl(module_base + 0x858),
        0x85C, readl(module_base + 0x85C));
    DDPDUMP("MMSYS_CONFIG: 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x\n",
        0x860, readl(module_base + 0x860),
        0x864, readl(module_base + 0x864),
        0x868, readl(module_base + 0x868),
        0x870, readl(module_base + 0x870));
    DDPDUMP("MMSYS_CONFIG: 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x\n",
        0x874, readl(module_base + 0x874),
        0x878, readl(module_base + 0x878),
        0x88C, readl(module_base + 0x88C),
        0x890, readl(module_base + 0x890));
    DDPDUMP("MMSYS_CONFIG: 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x\n",
        0x894, readl(module_base + 0x894),
        0x898, readl(module_base + 0x898),
        0x89C, readl(module_base + 0x89C),
        0x8A0, readl(module_base + 0x8A0));
    DDPDUMP("MMSYS_CONFIG: 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x\n",
        0x8A4, readl(module_base + 0x8A4),
        0x8A8, readl(module_base + 0x8A8),
        0x8AC, readl(module_base + 0x8AC),
        0x8B0, readl(module_base + 0x8B0));
    DDPDUMP("MMSYS_CONFIG: 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x\n",
        0x8B4, readl(module_base + 0x8B4),
        0x8B8, readl(module_base + 0x8B8),
        0x8C0, readl(module_base + 0x8C0),
        0x8C4, readl(module_base + 0x8C4));
    DDPDUMP("MMSYS_CONFIG: 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x\n",
        0x8CC, readl(module_base + 0x8CC),
        0x8D0, readl(module_base + 0x8D0),
        0x8D4, readl(module_base + 0x8D4),
        0x8D8, readl(module_base + 0x8D8));
    DDPDUMP("MMSYS_CONFIG: 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x\n",
        0x8DC, readl(module_base + 0x8DC),
        0x8E0, readl(module_base + 0x8E0),
        0x8E4, readl(module_base + 0x8E4),
        0x8E8, readl(module_base + 0x8E8));
    DDPDUMP("MMSYS_CONFIG: 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x\n",
        0x8EC, readl(module_base + 0x8EC),
        0x8F0, readl(module_base + 0x8F0),
        0x908, readl(module_base + 0x908),
        0x90C, readl(module_base + 0x90C));
    DDPDUMP("MMSYS_CONFIG: 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x\n",
        0x910, readl(module_base + 0x910),
        0x914, readl(module_base + 0x914),
        0x918, readl(module_base + 0x918),
        0x91C, readl(module_base + 0x91C));
    DDPDUMP("MMSYS_CONFIG: 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x\n",
        0x920, readl(module_base + 0x920),
        0x924, readl(module_base + 0x924),
        0x928, readl(module_base + 0x928),
        0x934, readl(module_base + 0x934));
    DDPDUMP("MMSYS_CONFIG: 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x\n",
        0x938, readl(module_base + 0x938),
        0x93C, readl(module_base + 0x93C),
        0x940, readl(module_base + 0x940),
        0x944, readl(module_base + 0x944));
    DDPDUMP("MMSYS_CONFIG: 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x\n",
        0xE00, readl(module_base + 0xE00),
        0xE04, readl(module_base + 0xE04),
        0xE08, readl(module_base + 0xE08),
        0xE0C, readl(module_base + 0xE0C));
    DDPDUMP("MMSYS_CONFIG: 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x\n",
        0xE10, readl(module_base + 0xE10),
        0xE14, readl(module_base + 0xE14),
        0xE18, readl(module_base + 0xE18),
        0xE1C, readl(module_base + 0xE1C));
    DDPDUMP("MMSYS_CONFIG: 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x\n",
        0xE70, readl(module_base + 0xE70),
        0xE74, readl(module_base + 0xE74),
        0xE78, readl(module_base + 0xE78),
        0xE7C, readl(module_base + 0xE7C));
    DDPDUMP("MMSYS_CONFIG: 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x\n",
        0xE80, readl(module_base + 0xE80),
        0xE84, readl(module_base + 0xE84),
        0xE88, readl(module_base + 0xE88),
        0xE8C, readl(module_base + 0xE8C));
    DDPDUMP("MMSYS_CONFIG: 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x\n",
        0xE90, readl(module_base + 0xE90),
        0xE94, readl(module_base + 0xE94),
        0xE98, readl(module_base + 0xE98),
        0xE9C, readl(module_base + 0xE9C));
    DDPDUMP("MMSYS_CONFIG: 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x\n",
        0xEA0, readl(module_base + 0xEA0),
        0xEA4, readl(module_base + 0xEA4),
        0xEA8, readl(module_base + 0xEA8),
        0xEAC, readl(module_base + 0xEAC));
    DDPDUMP("MMSYS_CONFIG: 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x\n",
        0xEB0, readl(module_base + 0xEB0),
        0xEB4, readl(module_base + 0xEB4),
        0xEB8, readl(module_base + 0xEB8),
        0xEBC, readl(module_base + 0xEBC));
    DDPDUMP("MMSYS_CONFIG: 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x\n",
        0xF00, readl(module_base + 0xF00),
        0xF04, readl(module_base + 0xF04),
        0xF08, readl(module_base + 0xF08),
        0xF0C, readl(module_base + 0xF0C));
    DDPDUMP("MMSYS_CONFIG: 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x\n",
        0xF10, readl(module_base + 0xF10),
        0xF14, readl(module_base + 0xF14),
        0xF18, readl(module_base + 0xF18),
        0xF1C, readl(module_base + 0xF1C));
    DDPDUMP("MMSYS_CONFIG: 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x\n",
        0xF20, readl(module_base + 0xF20),
        0xF24, readl(module_base + 0xF24),
        0xF28, readl(module_base + 0xF28),
        0xF2C, readl(module_base + 0xF2C));
    DDPDUMP("MMSYS_CONFIG: 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x\n",
        0xF30, readl(module_base + 0xF30),
        0xF34, readl(module_base + 0xF34),
        0xF38, readl(module_base + 0xF38),
        0xF3C, readl(module_base + 0xF3C));
    DDPDUMP("MMSYS_CONFIG: 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x\n",
        0xF40, readl(module_base + 0xF40),
        0xF44, readl(module_base + 0xF44),
        0xF48, readl(module_base + 0xF48),
        0xF4C, readl(module_base + 0xF4C));
    DDPDUMP("MMSYS_CONFIG: 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x\n",
        0xF50, readl(module_base + 0xF50),
        0xF54, readl(module_base + 0xF54),
        0xF58, readl(module_base + 0xF58),
        0xF5C, readl(module_base + 0xF5C));
    DDPDUMP("MMSYS_CONFIG: 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x\n",
        0xF60, readl(module_base + 0xF60),
        0xF64, readl(module_base + 0xF64),
        0xF68, readl(module_base + 0xF68),
        0xF6C, readl(module_base + 0xF6C));
    DDPDUMP("-- END: DISP MMSYS_CONFIG registers --\n");

    return;
}


/* ------ clock:
 * Before power on mmsys:
 * CLK_CFG_0_CLR (address is 0x10000048) = 0x80000000 (bit 31).
 * Before using DISP_PWM0 or DISP_PWM1:
 * CLK_CFG_1_CLR(address is 0x10000058)=0x80 (bit 7).
 * Before using DPI pixel clock:
 * CLK_CFG_6_CLR(address is 0x100000A8)=0x80 (bit 7).
 *
 * Only need to enable the corresponding bits of MMSYS_CG_CON0 and MMSYS_CG_CON1
 * for the modules:
 * smi_common, larb0, mdp_crop, fake_eng, mutex_32k, pwm0, pwm1, dsi0, dsi1, dpi.
 * Other bits could keep 1. Suggest to keep smi_common and larb0 always clock on.
 *
 * --------valid & ready
 * example:
 * ovl0 -> ovl0_mout_ready=1 means engines after ovl_mout are ready for receiving data
 *     ovl0_mout_ready=0 means ovl0_mout can not receive data,
 *     maybe ovl0_mout or after engines config error
 * ovl0 -> ovl0_mout_valid=1 means engines before ovl0_mout is OK,
 *     ovl0_mout_valid=0 means ovl can not transfer data to ovl0_mout,
 *     means ovl0 or before engines are not ready.
 */

static void mmsys_config_dump_analysis(void)
{

    unsigned int i = 0, j;
    unsigned int reg = 0;
    char clock_on[512] = { '\0' };
    char *pos = NULL;
    const char *name;
    /* int len = 0; */
    unsigned int valid[6], ready[6];
    unsigned int greq0;
    unsigned int greq1;
    unsigned int array_size = ARRAY_SIZE(valid);

    valid[0] = DISP_REG_GET(DISP_REG_CONFIG_MMSYS_DL_VALID0);
    valid[1] = DISP_REG_GET(DISP_REG_CONFIG_MMSYS_DL_VALID1);
    valid[2] = DISP_REG_GET(DISP_REG_CONFIG_MMSYS_DL_VALID2);
    valid[3] = DISP_REG_GET(DISP_REG_CONFIG_MMSYS_DL_VALID3);
    valid[4] = DISP_REG_GET(DISP_REG_CONFIG_MMSYS_DL_VALID4);
    valid[5] = DISP_REG_GET(DISP_REG_CONFIG_MMSYS_DL_VALID5);
    ready[0] = DISP_REG_GET(DISP_REG_CONFIG_MMSYS_DL_READY0);
    ready[1] = DISP_REG_GET(DISP_REG_CONFIG_MMSYS_DL_READY1);
    ready[2] = DISP_REG_GET(DISP_REG_CONFIG_MMSYS_DL_READY2);
    ready[3] = DISP_REG_GET(DISP_REG_CONFIG_MMSYS_DL_READY3);
    ready[4] = DISP_REG_GET(DISP_REG_CONFIG_MMSYS_DL_READY4);
    ready[5] = DISP_REG_GET(DISP_REG_CONFIG_MMSYS_DL_READY5);
    greq0 = DISP_REG_GET(DISP_REG_CONFIG_SMI_LARB0_GREQ);
    greq1 = DISP_REG_GET(DISP_REG_CONFIG_SMI_LARB1_GREQ);

    DDPDUMP("== DISP MMSYS_CONFIG ANALYSIS ==\n");
#if 0 /* TODO: mmsys clk?? */
    DDPDUMP("mmsys clock=0x%x, CG_CON0=0x%x, CG_CON1=0x%x\n",
        DISP_REG_GET(DISP_REG_CLK_CFG_0_MM_CLK),
        DISP_REG_GET(DISP_REG_CONFIG_MMSYS_CG_CON0),
        DISP_REG_GET(DISP_REG_CONFIG_MMSYS_CG_CON1));
    if ((DISP_REG_GET(DISP_REG_CLK_CFG_0_MM_CLK) >> 31) & 0x1)
        DDPERR("mmsys clock abnormal!!\n");
#endif

    reg = DISP_REG_GET(DISP_REG_CONFIG_MMSYS_CG_CON0);
    for (i = 0; i < 32; i++) {
        if ((reg & (1 << i)) == 0) {
            name = ddp_clock_0(i);
            if (name)
                strncat(clock_on, name, sizeof(clock_on));
        }
    }

    reg = DISP_REG_GET(DISP_REG_CONFIG_MMSYS_CG_CON1);
    for (i = 0; i < 32; i++) {
        if ((reg & (1 << i)) == 0) {
            name = ddp_clock_1(i);
            if (name)
                strncat(clock_on, name,
                        (sizeof(clock_on) -
                         strlen(clock_on) - 1));
        }
    }

    DDPDUMP("clock on modules:%s\n", clock_on);

    DDPDUMP("valid0=0x%x,valid1=0x%x,valid2=0x%x,ready0=0x%x,ready1=0x%x,ready2=0x%x\n",
        valid[0], valid[1], valid[2], ready[0], ready[1], ready[2]);
    DDPDUMP("greq0=0%x,greq1=0%x\n",
        greq0, greq1);
    DDPDUMP("valid3=0x%x,valid4=0x%x,valid5=0x%x,ready3=0x%x,ready4=0x%x,ready5=0x%x\n",
        valid[3], valid[4], valid[5], ready[3], ready[4], ready[5]);

    for (j = 0; j < array_size; j++) {
        for (i = 0; i < 32; i++) {
            name = ddp_signal[j][i];
            if (!name)
                continue;

            pos = clock_on;

            if ((valid[j] & (1 << i)))
                pos += sprintf(pos, "%s,", "v");
            else
                pos += sprintf(pos, "%s,", "n");

            if ((ready[j] & (1 << i)))
                pos += sprintf(pos, "%s", "r");
            else
                pos += sprintf(pos, "%s", "n");

            pos += sprintf(pos, ": %s", name);

            DDPDUMP("%s\n", clock_on);
        }
    }

    /* greq: 1 means SMI dose not grant, maybe SMI hang */
    if (greq0)
        DDPDUMP("smi greq0 not grant module: (greq0: 1 means SMI dose not grant, maybe SMI hang)");
    if (greq1)
        DDPDUMP("smi greq1 not grant module: (greq1: 1 means SMI dose not grant, maybe SMI hang)");

    clock_on[0] = '\0';
    for (i = 0; i < 32; i++) {
        if (greq0 & (1 << i)) {
            name = ddp_greq_name_larb0(i);
            if (!name)
                continue;
            strncat(clock_on, name, sizeof(clock_on));
        }
    }
    for (i = 0; i < 32; i++) {
        if (greq1 & (1 << i)) {
            name = ddp_greq_name_larb1(i);
            if (!name)
                continue;
            strncat(clock_on, name, sizeof(clock_on));
        }
    }

    DDPDUMP("%s\n", clock_on);

}

static void gamma_dump_reg(void)
{
    unsigned long module_base = DISPSYS_GAMMA_BASE;

    DDPDUMP("== START: DISP gamma0 registers ==\n");
    DDPDUMP("gamma0: 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x\n",
        0x0000, readl(module_base + 0x0000),
        0x0004, readl(module_base + 0x0004),
        0x0008, readl(module_base + 0x0008),
        0x000c, readl(module_base + 0x000c));
    DDPDUMP("gamma0: 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x\n",
        0x0010, readl(module_base + 0x0010),
        0x0020, readl(module_base + 0x0020),
        0x0024, readl(module_base + 0x0024),
        0x0028, readl(module_base + 0x0028));
    DDPDUMP("gamma0: 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x\n",
        0x002c, readl(module_base + 0x002c),
        0x0030, readl(module_base + 0x0030),
        0x0034, readl(module_base + 0x0034),
        0x0700, readl(module_base + 0x0700));
    DDPDUMP("-- END: DISP gamma0 registers --\n");

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

static void color_dump_reg(enum DISP_MODULE_ENUM module)
{
    int idx = 0;
    unsigned long module_base = DISPSYS_COLOR0_BASE;

    DDPDUMP("== START: DISP COLOR%d registers ==\n", idx);
    DDPDUMP("COLOR%d: 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x,    0x%04x=0x%08x\n",
        idx,
        0x00000400, readl(module_base + 0x00000400),
        0x00000404, readl(module_base + 0x00000404),
        0x00000408, readl(module_base + 0x00000408),
        0x0000040C, readl(module_base + 0x0000040C));
    DDPDUMP("COLOR%d: 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x,    0x%04x=0x%08x\n",
        idx,
        0x00000410, readl(module_base + 0x00000410),
        0x00000418, readl(module_base + 0x00000418),
        0x0000041C, readl(module_base + 0x0000041C),
        0x00000420, readl(module_base + 0x00000420));
    DDPDUMP("COLOR%d: 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x,    0x%04x=0x%08x\n",
        idx,
        0x00000428, readl(module_base + 0x00000428),
        0x0000042C, readl(module_base + 0x0000042C),
        0x00000430, readl(module_base + 0x00000430),
        0x00000434, readl(module_base + 0x00000434));
    DDPDUMP("COLOR%d: 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x,    0x%04x=0x%08x\n",
        idx,
        0x00000438, readl(module_base + 0x00000438),
        0x00000484, readl(module_base + 0x00000484),
        0x00000488, readl(module_base + 0x00000488),
        0x0000048C, readl(module_base + 0x0000048C));
    DDPDUMP("COLOR%d: 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x,    0x%04x=0x%08x\n",
        idx,
        0x00000490, readl(module_base + 0x00000490),
        0x00000494, readl(module_base + 0x00000494),
        0x00000498, readl(module_base + 0x00000498),
        0x0000049C, readl(module_base + 0x0000049C));
    DDPDUMP("COLOR%d: 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x,    0x%04x=0x%08x\n",
        idx,
        0x000004A0, readl(module_base + 0x000004A0),
        0x000004A4, readl(module_base + 0x000004A4),
        0x000004A8, readl(module_base + 0x000004A8),
        0x000004AC, readl(module_base + 0x000004AC));
    DDPDUMP("COLOR%d: 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x,    0x%04x=0x%08x\n",
        idx,
        0x000004B0, readl(module_base + 0x000004B0),
        0x000004B4, readl(module_base + 0x000004B4),
        0x000004B8, readl(module_base + 0x000004B8),
        0x000004BC, readl(module_base + 0x000004BC));
    DDPDUMP("COLOR%d: 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x,    0x%04x=0x%08x\n",
        idx,
        0x00000620, readl(module_base + 0x00000620),
        0x00000624, readl(module_base + 0x00000624),
        0x00000628, readl(module_base + 0x00000628),
        0x0000062C, readl(module_base + 0x0000062C));
    DDPDUMP("COLOR%d: 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x,    0x%04x=0x%08x\n",
        idx,
        0x00000630, readl(module_base + 0x00000630),
        0x00000740, readl(module_base + 0x00000740),
        0x0000074C, readl(module_base + 0x0000074C),
        0x00000768, readl(module_base + 0x00000768));
    DDPDUMP("COLOR%d: 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x,    0x%04x=0x%08x\n",
        idx,
        0x0000076C, readl(module_base + 0x0000076C),
        0x0000079C, readl(module_base + 0x0000079C),
        0x000007E0, readl(module_base + 0x000007E0),
        0x000007E4, readl(module_base + 0x000007E4));
    DDPDUMP("COLOR%d: 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x,    0x%04x=0x%08x\n",
        idx,
        0x000007E8, readl(module_base + 0x000007E8),
        0x000007EC, readl(module_base + 0x000007EC),
        0x000007F0, readl(module_base + 0x000007F0),
        0x000007FC, readl(module_base + 0x000007FC));
    DDPDUMP("COLOR%d: 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x,    0x%04x=0x%08x\n",
        idx,
        0x00000800, readl(module_base + 0x00000800),
        0x00000804, readl(module_base + 0x00000804),
        0x00000808, readl(module_base + 0x00000808),
        0x0000080C, readl(module_base + 0x0000080C));
    DDPDUMP("COLOR%d: 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x,    0x%04x=0x%08x\n",
        idx,
        0x00000810, readl(module_base + 0x00000810),
        0x00000814, readl(module_base + 0x00000814),
        0x00000818, readl(module_base + 0x00000818),
        0x0000081C, readl(module_base + 0x0000081C));
    DDPDUMP("COLOR%d: 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x,    0x%04x=0x%08x\n",
        idx,
        0x00000820, readl(module_base + 0x00000820),
        0x00000824, readl(module_base + 0x00000824),
        0x00000828, readl(module_base + 0x00000828),
        0x0000082C, readl(module_base + 0x0000082C));
    DDPDUMP("COLOR%d: 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x,    0x%04x=0x%08x\n",
        idx,
        0x00000830, readl(module_base + 0x00000830),
        0x00000834, readl(module_base + 0x00000834),
        0x00000838, readl(module_base + 0x00000838),
        0x0000083C, readl(module_base + 0x0000083C));
    DDPDUMP("COLOR%d: 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x,    0x%04x=0x%08x\n",
        idx,
        0x00000840, readl(module_base + 0x00000840),
        0x00000844, readl(module_base + 0x00000844),
        0x00000848, readl(module_base + 0x00000848),
        0x0000084C, readl(module_base + 0x0000084C));
    DDPDUMP("COLOR%d: 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x,    0x%04x=0x%08x\n",
        idx,
        0x00000850, readl(module_base + 0x00000850),
        0x00000854, readl(module_base + 0x00000854),
        0x00000858, readl(module_base + 0x00000858),
        0x0000085C, readl(module_base + 0x0000085C));
    DDPDUMP("COLOR%d: 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x,    0x%04x=0x%08x\n",
        idx,
        0x00000860, readl(module_base + 0x00000860),
        0x00000864, readl(module_base + 0x00000864),
        0x00000868, readl(module_base + 0x00000868),
        0x0000086C, readl(module_base + 0x0000086C));
    DDPDUMP("COLOR%d: 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x,    0x%04x=0x%08x\n",
        idx,
        0x00000870, readl(module_base + 0x00000870),
        0x00000874, readl(module_base + 0x00000874),
        0x00000878, readl(module_base + 0x00000878),
        0x0000087C, readl(module_base + 0x0000087C));
    DDPDUMP("COLOR%d: 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x,    0x%04x=0x%08x\n",
        idx,
        0x00000880, readl(module_base + 0x00000880),
        0x00000884, readl(module_base + 0x00000884),
        0x00000888, readl(module_base + 0x00000888),
        0x0000088C, readl(module_base + 0x0000088C));
    DDPDUMP("COLOR%d: 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x,    0x%04x=0x%08x\n",
        idx,
        0x00000890, readl(module_base + 0x00000890),
        0x00000894, readl(module_base + 0x00000894),
        0x00000898, readl(module_base + 0x00000898),
        0x0000089C, readl(module_base + 0x0000089C));
    DDPDUMP("COLOR%d: 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x,    0x%04x=0x%08x\n",
        idx,
        0x000008A0, readl(module_base + 0x000008A0),
        0x000008A4, readl(module_base + 0x000008A4),
        0x000008A8, readl(module_base + 0x000008A8),
        0x000008AC, readl(module_base + 0x000008AC));
    DDPDUMP("COLOR%d: 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x,    0x%04x=0x%08x\n",
        idx,
        0x000008B0, readl(module_base + 0x000008B0),
        0x000008B4, readl(module_base + 0x000008B4),
        0x000008B8, readl(module_base + 0x000008B8),
        0x000008BC, readl(module_base + 0x000008BC));
    DDPDUMP("COLOR%d: 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x,    0x%04x=0x%08x\n",
        idx,
        0x000008C0, readl(module_base + 0x000008C0),
        0x000008C4, readl(module_base + 0x000008C4),
        0x000008C8, readl(module_base + 0x000008C8),
        0x000008CC, readl(module_base + 0x000008CC));
    DDPDUMP("COLOR%d: 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x,    0x%04x=0x%08x\n",
        idx,
        0x000008D0, readl(module_base + 0x000008D0),
        0x000008D4, readl(module_base + 0x000008D4),
        0x000008D8, readl(module_base + 0x000008D8),
        0x000008DC, readl(module_base + 0x000008DC));
    DDPDUMP("COLOR%d: 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x,    0x%04x=0x%08x\n",
        idx,
        0x000008E0, readl(module_base + 0x000008E0),
        0x000008E4, readl(module_base + 0x000008E4),
        0x000008E8, readl(module_base + 0x000008E8),
        0x000008EC, readl(module_base + 0x000008EC));
    DDPDUMP("COLOR%d: 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x,    0x%04x=0x%08x\n",
        idx,
        0x000008F0, readl(module_base + 0x000008F0),
        0x000008F4, readl(module_base + 0x000008F4),
        0x000008F8, readl(module_base + 0x000008F8),
        0x000008FC, readl(module_base + 0x000008FC));
    DDPDUMP("COLOR%d: 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x,    0x%04x=0x%08x\n",
        idx,
        0x00000900, readl(module_base + 0x00000900),
        0x00000904, readl(module_base + 0x00000904),
        0x00000908, readl(module_base + 0x00000908),
        0x0000090C, readl(module_base + 0x0000090C));
    DDPDUMP("COLOR%d: 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x,    0x%04x=0x%08x\n",
        idx,
        0x00000910, readl(module_base + 0x00000910),
        0x00000914, readl(module_base + 0x00000914),
        0x00000C00, readl(module_base + 0x00000C00),
        0x00000C04, readl(module_base + 0x00000C04));
    DDPDUMP("COLOR%d: 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x,    0x%04x=0x%08x\n",
        idx,
        0x00000C08, readl(module_base + 0x00000C08),
        0x00000C0C, readl(module_base + 0x00000C0C),
        0x00000C10, readl(module_base + 0x00000C10),
        0x00000C14, readl(module_base + 0x00000C14));
    DDPDUMP("COLOR%d: 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x,    0x%04x=0x%08x\n",
        idx,
        0x00000C18, readl(module_base + 0x00000C18),
        0x00000C28, readl(module_base + 0x00000C28),
        0x00000C50, readl(module_base + 0x00000C50),
        0x00000C54, readl(module_base + 0x00000C54));
    DDPDUMP("COLOR%d: 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x,    0x%04x=0x%08x\n",
        idx,
        0x00000C60, readl(module_base + 0x00000C60),
        0x00000CA0, readl(module_base + 0x00000CA0),
        0x00000CB0, readl(module_base + 0x00000CB0),
        0x00000CF0, readl(module_base + 0x00000CF0));
    DDPDUMP("COLOR%d: 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x,    0x%04x=0x%08x\n",
        idx,
        0x00000CF4, readl(module_base + 0x00000CF4),
        0x00000CF8, readl(module_base + 0x00000CF8),
        0x00000CFC, readl(module_base + 0x00000CFC),
        0x00000D00, readl(module_base + 0x00000D00));
    DDPDUMP("COLOR%d: 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x,    0x%04x=0x%08x\n",
        idx,
        0x00000D04, readl(module_base + 0x00000D04),
        0x00000D08, readl(module_base + 0x00000D08),
        0x00000D0C, readl(module_base + 0x00000D0C),
        0x00000D10, readl(module_base + 0x00000D10));
    DDPDUMP("COLOR%d: 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x,    0x%04x=0x%08x\n",
        idx,
        0x00000D14, readl(module_base + 0x00000D14),
        0x00000D18, readl(module_base + 0x00000D18),
        0x00000D1C, readl(module_base + 0x00000D1C),
        0x00000D20, readl(module_base + 0x00000D20));
    DDPDUMP("COLOR%d: 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x,    0x%04x=0x%08x\n",
        idx,
        0x00000D24, readl(module_base + 0x00000D24),
        0x00000D28, readl(module_base + 0x00000D28),
        0x00000D2C, readl(module_base + 0x00000D2C),
        0x00000D30, readl(module_base + 0x00000D30));
    DDPDUMP("COLOR%d: 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x,    0x%04x=0x%08x\n",
        idx,
        0x00000D34, readl(module_base + 0x00000D34),
        0x00000D38, readl(module_base + 0x00000D38),
        0x00000D3C, readl(module_base + 0x00000D3C),
        0x00000D40, readl(module_base + 0x00000D40));
    DDPDUMP("COLOR%d: 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x,    0x%04x=0x%08x\n",
        idx,
        0x00000D44, readl(module_base + 0x00000D44),
        0x00000D48, readl(module_base + 0x00000D48),
        0x00000D4C, readl(module_base + 0x00000D4C),
        0x00000D50, readl(module_base + 0x00000D50));
    DDPDUMP("COLOR%d: 0x%04x=0x%08x, 0x%04x=0x%08x,  0x%04x=0x%08x\n",
        idx,
        0x00000D54, readl(module_base + 0x00000D54),
        0x00000D58, readl(module_base + 0x00000D58),
        0x00000D5C, readl(module_base + 0x00000D5C));
    DDPDUMP("-- END: DISP COLOR%d registers --\n", idx);

    return;
}


static void color_dump_analysis(enum DISP_MODULE_ENUM module)
{
    int index = 0;

    if (module == DISP_MODULE_COLOR0) {
        index = 0;
    } else {
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
    int i = 0;
    unsigned long module_base = DISPSYS_AAL_BASE;

    DDPDUMP("== START: DISP AAL%d registers ==\n", i);
    DDPDUMP("AAL: 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x\n",
        0x00000000, readl(module_base + 0x00000000),
        0x00000004, readl(module_base + 0x00000004),
        0x00000008, readl(module_base + 0x00000008),
        0x0000000C, readl(module_base + 0x0000000C));
    DDPDUMP("AAL: 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x\n",
        0x00000010, readl(module_base + 0x00000010),
        0x00000020, readl(module_base + 0x00000020),
        0x00000024, readl(module_base + 0x00000024),
        0x00000028, readl(module_base + 0x00000028));
    DDPDUMP("AAL: 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x\n",
        0x0000002C, readl(module_base + 0x0000002C),
        0x00000030, readl(module_base + 0x00000030),
        0x000000B0, readl(module_base + 0x000000B0),
        0x000000C0, readl(module_base + 0x000000C0));
    DDPDUMP("AAL: 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x\n",
        0x000000FC, readl(module_base + 0x000000FC),
        0x00000204, readl(module_base + 0x00000204),
        0x0000020C, readl(module_base + 0x0000020C),
        0x00000214, readl(module_base + 0x00000214));
    DDPDUMP("AAL: 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x\n",
        0x0000021C, readl(module_base + 0x0000021C),
        0x00000224, readl(module_base + 0x00000224),
        0x00000228, readl(module_base + 0x00000228),
        0x0000022C, readl(module_base + 0x0000022C));
    DDPDUMP("AAL: 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x\n",
        0x00000230, readl(module_base + 0x00000230),
        0x00000234, readl(module_base + 0x00000234),
        0x00000238, readl(module_base + 0x00000238),
        0x0000023C, readl(module_base + 0x0000023C));
    DDPDUMP("AAL: 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x\n",
        0x00000240, readl(module_base + 0x00000240),
        0x00000244, readl(module_base + 0x00000244),
        0x00000248, readl(module_base + 0x00000248),
        0x0000024C, readl(module_base + 0x0000024C));
    DDPDUMP("AAL: 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x\n",
        0x00000250, readl(module_base + 0x00000250),
        0x00000254, readl(module_base + 0x00000254),
        0x00000258, readl(module_base + 0x00000258),
        0x0000025C, readl(module_base + 0x0000025C));
    DDPDUMP("AAL: 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x\n",
        0x00000260, readl(module_base + 0x00000260),
        0x00000264, readl(module_base + 0x00000264),
        0x00000268, readl(module_base + 0x00000268),
        0x0000026C, readl(module_base + 0x0000026C));
    DDPDUMP("AAL: 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x\n",
        0x00000270, readl(module_base + 0x00000270),
        0x00000274, readl(module_base + 0x00000274),
        0x00000278, readl(module_base + 0x00000278),
        0x0000027C, readl(module_base + 0x0000027C));
    DDPDUMP("AAL: 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x\n",
        0x00000280, readl(module_base + 0x00000280),
        0x00000284, readl(module_base + 0x00000284),
        0x00000288, readl(module_base + 0x00000288),
        0x0000028C, readl(module_base + 0x0000028C));
    DDPDUMP("AAL: 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x\n",
        0x00000290, readl(module_base + 0x00000290),
        0x00000294, readl(module_base + 0x00000294),
        0x00000298, readl(module_base + 0x00000298),
        0x0000029C, readl(module_base + 0x0000029C));
    DDPDUMP("AAL: 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x\n",
        0x000002A0, readl(module_base + 0x000002A0),
        0x000002A4, readl(module_base + 0x000002A4),
        0x00000358, readl(module_base + 0x00000358),
        0x0000035C, readl(module_base + 0x0000035C));
    DDPDUMP("AAL: 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x\n",
        0x00000360, readl(module_base + 0x00000360),
        0x00000364, readl(module_base + 0x00000364),
        0x00000368, readl(module_base + 0x00000368),
        0x0000036C, readl(module_base + 0x0000036C));
    DDPDUMP("AAL: 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x\n",
        0x00000370, readl(module_base + 0x00000370),
        0x00000374, readl(module_base + 0x00000374),
        0x00000378, readl(module_base + 0x00000378),
        0x0000037C, readl(module_base + 0x0000037C));
    DDPDUMP("AAL: 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x\n",
        0x00000380, readl(module_base + 0x00000380),
        0x000003B0, readl(module_base + 0x000003B0),
        0x0000040C, readl(module_base + 0x0000040C),
        0x00000410, readl(module_base + 0x00000410));
    DDPDUMP("AAL: 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x\n",
        0x00000414, readl(module_base + 0x00000414),
        0x00000418, readl(module_base + 0x00000418),
        0x0000041C, readl(module_base + 0x0000041C),
        0x00000420, readl(module_base + 0x00000420));
    DDPDUMP("AAL: 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x\n",
        0x00000424, readl(module_base + 0x00000424),
        0x00000428, readl(module_base + 0x00000428),
        0x0000042C, readl(module_base + 0x0000042C),
        0x00000430, readl(module_base + 0x00000430));
    DDPDUMP("AAL: 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x\n",
        0x00000434, readl(module_base + 0x00000434),
        0x00000440, readl(module_base + 0x00000440),
        0x00000444, readl(module_base + 0x00000444),
        0x00000448, readl(module_base + 0x00000448));
    DDPDUMP("-- END: DISP AAL%d registers --\n", i);

    return;
}

static void mdp_aal_dump_reg(enum DISP_MODULE_ENUM module)
{
    int i;
    unsigned long module_base = DISPSYS_MDP_AAL4_BASE;

    if (module == DISP_MODULE_MDP_AAL4)
        i = 0;
    else
        i = 1;

    DDPDUMP("== START: DISP MDP AAL%d registers ==\n", i);
    DDPDUMP("AAL: 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x\n",
        0x00000000, readl(module_base + 0x00000000),
        0x00000004, readl(module_base + 0x00000004),
        0x00000008, readl(module_base + 0x00000008),
        0x0000000C, readl(module_base + 0x0000000C));
    DDPDUMP("AAL: 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x\n",
        0x00000010, readl(module_base + 0x00000010),
        0x00000020, readl(module_base + 0x00000020),
        0x00000024, readl(module_base + 0x00000024),
        0x00000028, readl(module_base + 0x00000028));
    DDPDUMP("AAL: 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x\n",
        0x0000002C, readl(module_base + 0x0000002C),
        0x00000030, readl(module_base + 0x00000030),
        0x00000034, readl(module_base + 0x00000034));
    DDPDUMP("AAL: 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x\n",
        0x000000B0, readl(module_base + 0x000000B0),
        0x000000C0, readl(module_base + 0x000000C0),
        0x000000FC, readl(module_base + 0x000000FC));
    DDPDUMP("AAL: 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x\n",
        0x00000204, readl(module_base + 0x00000204),
        0x0000020C, readl(module_base + 0x0000020C),
        0x00000214, readl(module_base + 0x00000214));
    DDPDUMP("AAL: 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x\n",
        0x0000021C, readl(module_base + 0x0000021C),
        0x00000224, readl(module_base + 0x00000224),
        0x00000228, readl(module_base + 0x00000228),
        0x0000022C, readl(module_base + 0x0000022C));
    DDPDUMP("AAL: 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x\n",
        0x00000230, readl(module_base + 0x00000230),
        0x00000234, readl(module_base + 0x00000234),
        0x00000238, readl(module_base + 0x00000238),
        0x0000023C, readl(module_base + 0x0000023C));
    DDPDUMP("AAL: 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x\n",
        0x00000240, readl(module_base + 0x00000240),
        0x00000244, readl(module_base + 0x00000244),
        0x00000248, readl(module_base + 0x00000248),
        0x0000024C, readl(module_base + 0x0000024C));
    DDPDUMP("AAL: 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x\n",
        0x00000250, readl(module_base + 0x00000250),
        0x00000254, readl(module_base + 0x00000254),
        0x00000258, readl(module_base + 0x00000258),
        0x0000025C, readl(module_base + 0x0000025C));
    DDPDUMP("AAL: 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x\n",
        0x00000260, readl(module_base + 0x00000260),
        0x00000264, readl(module_base + 0x00000264),
        0x00000268, readl(module_base + 0x00000268),
        0x0000026C, readl(module_base + 0x0000026C));
    DDPDUMP("AAL: 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x\n",
        0x00000270, readl(module_base + 0x00000270),
        0x00000274, readl(module_base + 0x00000274),
        0x00000278, readl(module_base + 0x00000278),
        0x0000027C, readl(module_base + 0x0000027C));
    DDPDUMP("AAL: 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x\n",
        0x00000280, readl(module_base + 0x00000280),
        0x00000284, readl(module_base + 0x00000284),
        0x00000288, readl(module_base + 0x00000288),
        0x0000028C, readl(module_base + 0x0000028C));
    DDPDUMP("AAL: 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x\n",
        0x00000290, readl(module_base + 0x00000290),
        0x00000294, readl(module_base + 0x00000294),
        0x00000298, readl(module_base + 0x00000298),
        0x0000029C, readl(module_base + 0x0000029C));
    DDPDUMP("AAL: 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x\n",
        0x000002A0, readl(module_base + 0x000002A0),
        0x000002A4, readl(module_base + 0x000002A4),
        0x00000358, readl(module_base + 0x00000358),
        0x0000035C, readl(module_base + 0x0000035C));
    DDPDUMP("AAL: 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x\n",
        0x00000360, readl(module_base + 0x00000360),
        0x00000364, readl(module_base + 0x00000364),
        0x00000368, readl(module_base + 0x00000368),
        0x0000036C, readl(module_base + 0x0000036C));
    DDPDUMP("AAL: 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x\n",
        0x00000370, readl(module_base + 0x00000370),
        0x00000374, readl(module_base + 0x00000374),
        0x00000378, readl(module_base + 0x00000378),
        0x0000037C, readl(module_base + 0x0000037C));
    DDPDUMP("AAL: 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x\n",
        0x00000380, readl(module_base + 0x00000380),
        0x000003B0, readl(module_base + 0x000003B0),
        0x0000040C, readl(module_base + 0x0000040C),
        0x00000410, readl(module_base + 0x00000410));
    DDPDUMP("AAL:0x%04x= 0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x\n",
        0x00000414, readl(module_base + 0x00000414),
        0x00000418, readl(module_base + 0x00000418),
        0x0000041C, readl(module_base + 0x0000041C),
        0x00000420, readl(module_base + 0x00000420));
    DDPDUMP("AAL:0x%04x= 0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x\n",
        0x00000424, readl(module_base + 0x00000424),
        0x00000428, readl(module_base + 0x00000428),
        0x0000042C, readl(module_base + 0x0000042C),
        0x00000430, readl(module_base + 0x00000430));
    DDPDUMP("AAL: 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x\n",
        0x00000434, readl(module_base + 0x00000434),
        0x00000440, readl(module_base + 0x00000440),
        0x00000444, readl(module_base + 0x00000444),
        0x00000448, readl(module_base + 0x00000448));
    DDPDUMP("-- END: DISP AAL%d registers --\n", i);
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

static void pwm_dump_reg(enum DISP_MODULE_ENUM module)
{
    unsigned long module_base = DISPSYS_PWM0_BASE;

    DDPDUMP("== START: DISP PWM0 registers ==\n");
    DDPDUMP("PWM0: 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x\n",
        0x0, readl(module_base + 0x0),
        0x4, readl(module_base + 0x4),
        0x8, readl(module_base + 0x8),
        0xC, readl(module_base + 0xC));
    DDPDUMP("PWM0: 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x\n",
        0x10, readl(module_base + 0x10),
        0x14, readl(module_base + 0x14),
        0x18, readl(module_base + 0x18),
        0x1C, readl(module_base + 0x1C));
    DDPDUMP("PWM0: 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x\n",
        0x80, readl(module_base + 0x80),
        0x28, readl(module_base + 0x28),
        0x2C, readl(module_base + 0x2C),
        0x30, readl(module_base + 0x30));
    DDPDUMP("PWM0: 0x%04x=0x%08x\n",
        0xC0, readl(module_base + 0xC0));
    DDPDUMP("-- END: DISP PWM0 registers --\n");

}


static void pwm_dump_analysis(enum DISP_MODULE_ENUM module)
{
    int index = 0;
    unsigned int reg_base = 0;

    if (module == DISP_MODULE_PWM0) {
        index = 0;
        reg_base = DISPSYS_PWM0_BASE;
    } else {
        index = 1;
        reg_base = DISPSYS_PWM0_BASE;
    }
    DDPDUMP("== DISP PWM%d REG_BASE:0x%08x ANALYSIS ==\n", index, reg_base);
#if 0 /* TODO: clk reg?? */
    DDPDUMP("pwm clock=%d\n", (DISP_REG_GET(DISP_REG_CLK_CFG_1_CLR) >> 7) & 0x1);
#endif

    return;
}



static void ccorr_dump_reg(void)
{
    int i = 0;
    unsigned long module_base = DISPSYS_CCORR_BASE;

    DDPDUMP("== START: DISP CCORR%i registers ==\n", i);
    DDPDUMP("CCORR: 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x\n",
        0x000, readl(module_base + 0x000),
        0x004, readl(module_base + 0x004),
        0x008, readl(module_base + 0x008),
        0x00C, readl(module_base + 0x00C));
    DDPDUMP("CCORR: 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x\n",
        0x010, readl(module_base + 0x010),
        0x020, readl(module_base + 0x020),
        0x024, readl(module_base + 0x024),
        0x028, readl(module_base + 0x028));
    DDPDUMP("CCORR: 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x\n",
        0x02C, readl(module_base + 0x02C),
        0x030, readl(module_base + 0x030),
        0x080, readl(module_base + 0x080),
        0x084, readl(module_base + 0x084));
    DDPDUMP("CCORR: 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x\n",
        0x088, readl(module_base + 0x088),
        0x08C, readl(module_base + 0x08C),
        0x090, readl(module_base + 0x090),
        0x0A0, readl(module_base + 0x0A0));
    DDPDUMP("CCORR: 0x%04x=0x%08x\n",
        0x0C0, readl(module_base + 0x0C0));
    DDPDUMP("-- END: DISP CCORR%d registers --\n", i);


}


static void ccorr_dump_analyze(void)
{
    DDPDUMP("ccorr: en=%d, config=%d, w=%d, h=%d\n",
                DISP_REG_GET(DISP_REG_CCORR_EN),
                DISP_REG_GET(DISP_REG_CCORR_CFG),
                (DISP_REG_GET(DISP_REG_CCORR_SIZE) >> 16) & 0x1fff,
                 DISP_REG_GET(DISP_REG_CCORR_SIZE) & 0x1fff);
    DDPDUMP("ccorr: in_p_cnt=%d, in_l_cnt=%d, out_p_cnt=%d, out_l_cnt=%d\n",
                 DISP_REG_GET(DISP_REG_CCORR_IN_CNT) & 0x1fff,
                 (DISP_REG_GET(DISP_REG_CCORR_IN_CNT) >> 16) & 0x1fff,
                 DISP_REG_GET(DISP_REG_CCORR_IN_CNT) & 0x1fff,
                 (DISP_REG_GET(DISP_REG_CCORR_IN_CNT) >> 16) & 0x1fff);
}

static void dither_dump_reg(void)
{
    int i = 0;
    unsigned long module_base = DISPSYS_DITHER_BASE;

    DDPDUMP("== START: DISP DITHER%d registers ==\n", i);
    DDPDUMP("DITHER: 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x\n",
        0x00000000, readl(module_base + 0x00000000),
        0x00000004, readl(module_base + 0x00000004),
        0x00000008, readl(module_base + 0x00000008),
        0x0000000C, readl(module_base + 0x0000000C));
    DDPDUMP("DITHER: 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x\n",
        0x00000010, readl(module_base + 0x00000010),
        0x00000020, readl(module_base + 0x00000020),
        0x00000024, readl(module_base + 0x00000024),
        0x00000028, readl(module_base + 0x00000028));
    DDPDUMP("DITHER: 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x\n",
        0x0000002C, readl(module_base + 0x0000002C),
        0x00000030, readl(module_base + 0x00000030),
        0x000000C0, readl(module_base + 0x000000C0),
        0x00000100, readl(module_base + 0x00000100));
    DDPDUMP("DITHER: 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x\n",
        0x00000114, readl(module_base + 0x00000114),
        0x00000118, readl(module_base + 0x00000118),
        0x0000011C, readl(module_base + 0x0000011C),
        0x00000120, readl(module_base + 0x00000120));
    DDPDUMP("DITHER: 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x\n",
        0x00000124, readl(module_base + 0x00000124),
        0x00000128, readl(module_base + 0x00000128),
        0x0000012C, readl(module_base + 0x0000012C),
        0x00000130, readl(module_base + 0x00000130));
    DDPDUMP("DITHER: 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x\n",
        0x00000134, readl(module_base + 0x00000134),
        0x00000138, readl(module_base + 0x00000138),
        0x0000013C, readl(module_base + 0x0000013C),
        0x00000140, readl(module_base + 0x00000140));
    DDPDUMP("DITHER: 0x%04x=0x%08x\n",
        0x00000144, readl(module_base + 0x00000144));
    DDPDUMP("-- END: DISP DITHER%d registers --\n", i);

}


static void dither_dump_analyze(void)
{
    DDPDUMP("dither: en=%d, config=%d, w=%d, h=%d\n",
          DISP_REG_GET(DISPSYS_DITHER_BASE + 0x000),
          DISP_REG_GET(DISPSYS_DITHER_BASE + 0x020),
          (DISP_REG_GET(DISP_REG_DITHER_SIZE) >> 16) & 0x1fff,
          DISP_REG_GET(DISP_REG_DITHER_SIZE) & 0x1fff);
    DDPDUMP("dither: in_p_cnt=%d, in_l_cnt=%d, out_p_cnt=%d, out_l_cnt=%d\n",
          DISP_REG_GET(DISP_REG_DITHER_IN_CNT) & 0x1fff,
          (DISP_REG_GET(DISP_REG_DITHER_IN_CNT) >> 16) & 0x1fff,
          DISP_REG_GET(DISP_REG_DITHER_OUT_CNT) & 0x1fff,
          (DISP_REG_GET(DISP_REG_DITHER_OUT_CNT) >> 16) & 0x1fff);
}


static void dsi_dump_reg(enum DISP_MODULE_ENUM module)
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
}

static void dpi_dump_analysis(void)
{
#if 0
    DDPDUMP("== DISP DPI ANALYSIS ==\n");
    /* TODO: mmsys clk?? */
    DDPDUMP("DPI clock=0x%x\n", DISP_REG_GET(DISP_REG_CLK_CFG_6_DPI));
    DDPDUMP("DPI  clock_clear=%d\n", (DISP_REG_GET(DISP_REG_CLK_CFG_6_CLR) >> 7) & 0x1);
    DDPDUMP("(0x000)S_ENABLE=0x%x\n", DISP_REG_GET(DISP_REG_SPLIT_ENABLE));
    DDPDUMP("(0x004)S_SW_RST=0x%x\n", DISP_REG_GET(DISP_REG_SPLIT_SW_RESET));
#endif
}

static int split_dump_regs(void)
{
    unsigned long module_base = DISPSYS_SPLIT0_BASE;
    unsigned int idx = 0;

    DDPDUMP("== START: DISP SPLIT registers ==\n");
    DDPDUMP("SPLIT%d: 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x,    0x%04x=0x%08x\n",
        idx,
        0x000, readl(module_base + 0x000),
        0x004, readl(module_base + 0x004),
        0x008, readl(module_base + 0x008),
        0x00C, readl(module_base + 0x00C));
    DDPDUMP("SPLIT%d: 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x,    0x%04x=0x%08x\n",
        idx,
        0x010, readl(module_base + 0x010),
        0x020, readl(module_base + 0x020),
        0x024, readl(module_base + 0x024),
        0x028, readl(module_base + 0x028));
    DDPDUMP("SPLIT%d: 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x,    0x%04x=0x%08x\n",
        idx,
        0x02C, readl(module_base + 0x02C),
        0x030, readl(module_base + 0x030),
        0x034, readl(module_base + 0x034),
        0x038, readl(module_base + 0x038));
    DDPDUMP("SPLIT%d: 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x,    0x%04x=0x%08x\n",
        idx,
        0x03C, readl(module_base + 0x03C),
        0x040, readl(module_base + 0x040),
        0x044, readl(module_base + 0x044),
        0x48, readl(module_base + 0x48));
    DDPDUMP("SPLIT%d:  0x%04x=0x%08x\n",
        idx,
        0x080, readl(module_base + 0x080));
    DDPDUMP("-- END: DISP SPLIT registers --\n");
    return 0;
}





static int split_dump_analysis(void)
{
#if 0
    unsigned int pixel = DISP_REG_GET_FIELD(DEBUG_FLD_IN_PIXEL_CNT, DISP_REG_SPLIT_DEBUG);
    unsigned int state = DISP_REG_GET_FIELD(DEBUG_FLD_SPLIT_FSM, DISP_REG_SPLIT_DEBUG);

    DDPDUMP("== DISP SPLIT0 ANALYSIS ==\n");
    DDPDUMP("cur_pixel %u, state %s\n", pixel, split_state(state));
    return 0;
#else
    return 0;
#endif
}

extern int dsc_dump(enum DISP_MODULE_ENUM module);

int ddp_dump_reg(enum DISP_MODULE_ENUM module)
{
    switch (module) {
    case DISP_MODULE_WDMA0:
        //wdma_dump_reg(module);
        break;
    case DISP_MODULE_RDMA0:
        RDMADump(module);
        break;
    case DISP_MODULE_OVL0:
    case DISP_MODULE_OVL0_2L:
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
        color_dump_reg(module);
        break;
    case DISP_MODULE_AAL0:
        aal_dump_reg();
        break;
    case DISP_MODULE_MDP_AAL4:
        mdp_aal_dump_reg(module);
        break;
    case DISP_MODULE_PWM0:
        /* pwm_dump_reg(module); */
        break;
    case DISP_MODULE_DSI0:
    case DISP_MODULE_DSI1:
    case DISP_MODULE_DSIDUAL:
        dsi_dump_reg(module);
        break;
    case DISP_MODULE_CCORR0:
        ccorr_dump_reg();
        break;
    case DISP_MODULE_DITHER0:
        dither_dump_reg();
        break;
    case DISP_MODULE_POSTMASK0:
        postmask_dump_reg(module);
        break;
    case DISP_MODULE_DSC:
        dsc_dump(module);
        break;
    default:
        DDPDUMP("no dump_reg for module %s(%d)\n", ddp_get_module_name(module), module);
    }
    return 0;
}

int ddp_dump_analysis(enum DISP_MODULE_ENUM module)
{
    switch (module) {
    case DISP_MODULE_WDMA0:
        //wdma_dump_analysis(module);
        break;
    case DISP_MODULE_RDMA0:
        rdma_dump_analysis(module);
        break;
    case DISP_MODULE_OVL0:
    case DISP_MODULE_OVL0_2L:
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
        color_dump_analysis(module);
        break;
    case DISP_MODULE_AAL0:
        aal_dump_analysis();
        break;
    case DISP_MODULE_PWM0:
        /* pwm_dump_analysis(module); */
        break;
    case DISP_MODULE_DSI0:
    case DISP_MODULE_DSI1:
    case DISP_MODULE_DSIDUAL:
        dsi_analysis(module);
        break;
    case DISP_MODULE_CCORR0:
        ccorr_dump_analyze();
        break;
    case DISP_MODULE_DITHER0:
        dither_dump_analyze();
        break;
    case DISP_MODULE_POSTMASK0:
        postmask_dump_analysis(module);
        break;
    default:
        DDPDUMP("no dump_analysis for module %s(%d)\n", ddp_get_module_name(module),
            module);
    }
    return 0;
}
