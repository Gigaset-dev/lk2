/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#include <debug.h>
#include <mminfra.h>
#include <platform/addressmap.h>
#include <platform/reg.h>

//#define MMINFRA_BRINGUP
//#define MMINFRA_DBG

#ifndef MMINFRA_BRINGUP
#include "api_hre_mminfra.h"
#endif

#define MMINFRA_BASE                    (IO_BASE + 0x0E800000)
#define MMINFRA_HANG_FREE               (MMINFRA_BASE+0x280)
#define MMINFRA_VMID_REMAP              (MMINFRA_BASE+0x800)
#define MMINFRA_DISP_AID_REMAP          (MMINFRA_BASE+0x860)
#define MMINFRA_GCE_AID_REMAP           (MMINFRA_BASE+0x8a0)
#define MMINFRA_MDP_AID_REMAP           (MMINFRA_BASE+0x8d0)
#define MMINFRA_AID_REMAP_DOM2AID_EN    (MMINFRA_BASE+0x8f0)

#define MDP_M0_RFIFO0_THRE              (MMINFRA_BASE+0x250)
#define MDP_M0_RFIFO1_THRE              (MMINFRA_BASE+0x254)
#define MDP_M1_RFIFO0_THRE              (MMINFRA_BASE+0x258)
#define MDP_M1_RFIFO1_THRE              (MMINFRA_BASE+0x25c)
#define MM_M0_RFIFO0_THRE               (MMINFRA_BASE+0x260)
#define MM_M0_RFIFO1_THRE               (MMINFRA_BASE+0x264)
#define MM_M1_RFIFO0_THRE               (MMINFRA_BASE+0x268)
#define MM_M1_RFIFO1_THRE               (MMINFRA_BASE+0x26c)


#define MMINFRA_DAPC_BASE           (IO_BASE + 0x0E820000)
#define MMINFRA_AO_SYS0_D0_APC_0    (MMINFRA_DAPC_BASE + 0x0)

#define MM_HRE_SRAM_CON     (SPM_BASE+0xF18)

#define DISP_AID_MASK   (0xf)
#define GCE_AID_MASK    (0x7)
#define MDP_AID_MASK    (0x7)
#define VMID_MASK       (0x3)

#define DISP_AID_WIDTH  (4)
#define GCE_AID_WIDTH   (3)
#define MDP_AID_WIDTH   (3)
#define VMID_WIDTH      (2)

#define ARRAY_SIZE(arr) (countof(arr))

#define mm_read(addr)        readl(addr)
#define mm_write(addr, val)  writel(val, addr)
#define udelay(usec)         spin(usec)

#ifdef MMINFRA_DBG
#define mm_printf(x...) dprintf(CRITICAL, "[mminfra] " x)
#else
#define mm_printf(x...)
#endif
struct record {
    char uid_start;
    char uid_end;
    char aid;
};

static struct record mdp_rec[] = {
    {0x0, 0x0, 0x0},
    {0x1, 0x1, 0x1},
    {0x2, 0x2, 0x0},
    {0x3, 0x3, 0x1},
    {0x4, 0x4, 0x0},
    {0x5, 0x5, 0x1},
    {0x6, 0x6, 0x0},
    {0x7, 0x7, 0x0},
    {0x8, 0x8, 0x1},
    {0x10, 0x10, 0x2},
    {0x11, 0x11, 0x3},
    {0x12, 0x12, 0x2},
    {0x13, 0x13, 0x3},
    {0x14, 0x14, 0x2},
    {0x15, 0x15, 0x3},
    {0x16, 0x16, 0x2},
    {0x17, 0x17, 0x2},
    {0x18, 0x18, 0x3},
};

static struct record disp_rec[] = {
    {0x0, 0x6, 0x0},
    {0x8, 0xe, 0x1},
    {0x10, 0x16, 0x2},
    {0x1c, 0x1c, 0x5},
    {0x1d, 0x1d, 0x6},
    {0x1e, 0x1e, 0x2},
    {0x1f, 0x1f, 0x4},
    {0x20, 0x20, 0x1},
    {0x21, 0x21, 0x5},
    {0x22, 0x22, 0x6},
    {0x2d, 0x2d, 0x7},
    {0x30, 0x36, 0x3},
    {0x38, 0x3e, 0x4},
    {0x40, 0x46, 0x8},
    {0x48, 0x4e, 0x9},
    {0x50, 0x56, 0xa},
    {0x5c, 0x5c, 0xd},
    {0x5d, 0x5d, 0xe},
    {0x5e, 0x5e, 0xa},
    {0x5f, 0x5f, 0xc},
    {0x60, 0x60, 0x9},
    {0x61, 0x61, 0xd},
    {0x62, 0x62, 0xe},
    {0x6d, 0x6d, 0xf},
    {0x70, 0x76, 0xb},
    {0x78, 0x7e, 0xc},

};

static struct record gce_rec[] = {
    {0x0, 0x1f, 0x0}, /* GCE_D */
    /*{0x9, 0x9, 0x2},*/ /* over write thread 9 for TUI */
    {0x20, 0x3f, 0x1}, /* GCE_M */
    {0x40, 0x5f, 0x4}, /* HFRP */
};

static struct record vmid_rec[] = {
    {0x0, 0xff, 0x1},
    /*{0xb4, 0xb5, 0x0},*/ /* set VMID(with AID of GCE_D thread 9) to 0x0 for TUI */
    {0xb0, 0xb3, 0x0}, /* set VMID of all GCE threads to 0x0 for TUI */
    {0xe8, 0xeb, 0x2}, /* set VMID to 0x2 for JPGENC1 and JPGDEC1 */
};

static void set_uid2aid_remap(struct record rec[], unsigned int array_size,
    unsigned long aid_addr, unsigned int aid_width, unsigned int aid_mask)
{
    unsigned int i, j;
    unsigned int offset, shift, value;

    for (i = 0; i < array_size; i++) {
        for (j = rec[i].uid_start; j <= rec[i].uid_end; j++) {
            offset = (aid_width * j) / 32 * 4;
            shift = (aid_width * j) % 32;
            value = mm_read(aid_addr + offset) & ~(aid_mask << shift);
            value = (rec[i].aid << shift) | value;
            mm_printf("%s 1 write(0x%x)=0x%x shift=%d\n",
                __func__, aid_addr + offset, value, shift);
            mm_write(aid_addr + offset, value);
            if ((aid_width * (j+1) % 32 < aid_width) && (aid_width * (j+1) % 32 != 0)) {
                offset = offset + 4;
                shift = aid_width * (j+1) % 32;
                value = mm_read(aid_addr + offset) & ~(aid_mask >> shift);
                value = (rec[i].aid >> shift) | value;
                mm_write(aid_addr + offset, value);
                mm_printf("%s 2 write(0x%x)=0x%x shift=%d\n",
                     __func__, aid_addr + offset, value, shift);
            }
        }
    }
}

void mminfra_init(void)
{
#ifdef MMINFRA_BRINGUP
    dprintf(CRITICAL, "%s: for bring-up\n", __func__);
    mm_write(MMINFRA_HANG_FREE, 0xFFF);
#else

    // Set to HW default value
    mm_write(MM_HRE_SRAM_CON, 0x7007E);

    // Power on MMInfra HRE SRAM
    mm_write(MM_HRE_SRAM_CON, mm_read(MM_HRE_SRAM_CON) & ~(0x1 << 16));

    // Initialize HRE setting
    mminfra_hre_bkrs(INIT_HRE_BK_CONFIG);
    mminfra_hre_bkrs(SAVE_HRE_BK_CONFIG);
    mminfra_hre_bkrs(SAVE_HRE_RS_CONFIG);

    // MMInfra init
    mm_write(MMINFRA_HANG_FREE, 0xFFF); // MMInfra bus hang free
    mm_write(MMINFRA_AO_SYS0_D0_APC_0,
        mm_read(MMINFRA_AO_SYS0_D0_APC_0) | (0x3 << 14)); //Set uP/GCE to VA mode


    // UID to AID remap
    set_uid2aid_remap(disp_rec, ARRAY_SIZE(disp_rec), MMINFRA_DISP_AID_REMAP,
        DISP_AID_WIDTH, DISP_AID_MASK);
    set_uid2aid_remap(gce_rec, ARRAY_SIZE(gce_rec), MMINFRA_GCE_AID_REMAP,
        GCE_AID_WIDTH, GCE_AID_MASK);
    set_uid2aid_remap(mdp_rec, ARRAY_SIZE(mdp_rec), MMINFRA_MDP_AID_REMAP,
        MDP_AID_WIDTH, MDP_AID_MASK);
    mm_printf("%s mdp0=0x%x mdp1=0x%x\n", __func__, mm_read(MMINFRA_MDP_AID_REMAP),
        mm_read(MMINFRA_MDP_AID_REMAP+4));
    mm_printf("%s gce0=0x%x gce1=0x%x gce2=0x%x gce3=0x%x gce4=0x%x\n", __func__,
        mm_read(MMINFRA_GCE_AID_REMAP), mm_read(MMINFRA_GCE_AID_REMAP+4),
        mm_read(MMINFRA_GCE_AID_REMAP+8), mm_read(MMINFRA_GCE_AID_REMAP+12),
        mm_read(MMINFRA_GCE_AID_REMAP+16));
    mm_printf("%s disp0=0x%x disp1=0x%x\n", __func__,
        mm_read(MMINFRA_DISP_AID_REMAP), mm_read(MMINFRA_DISP_AID_REMAP+4));

    // Use the same function to write 0x1 for aid 0~255
    set_uid2aid_remap(vmid_rec, ARRAY_SIZE(vmid_rec), MMINFRA_VMID_REMAP,
        VMID_WIDTH, VMID_MASK);
    mm_printf("%s vmid0=0x%x vmid1=0x%x\n", __func__,
        mm_read(MMINFRA_VMID_REMAP), mm_read(MMINFRA_VMID_REMAP+0x2C));

    // Enable dom2aid
    mm_write(MMINFRA_AID_REMAP_DOM2AID_EN, 0x1);

    mm_write(MDP_M0_RFIFO0_THRE, 0x10);
    mm_write(MDP_M0_RFIFO1_THRE, 0x10);
    mm_write(MDP_M1_RFIFO0_THRE, 0x10);
    mm_write(MDP_M1_RFIFO1_THRE, 0x10);
    mm_write(MM_M0_RFIFO0_THRE, 0x10);
    mm_write(MM_M0_RFIFO1_THRE, 0x10);
    mm_write(MM_M1_RFIFO0_THRE, 0x10);
    mm_write(MM_M1_RFIFO1_THRE, 0x10);

    dprintf(CRITICAL, "%s bl2_ext version is called\n", __func__);
#endif /* MMINFRA_BRINGUP */
}
