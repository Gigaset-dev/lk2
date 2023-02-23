/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */
#include <debug.h>
#include <errno.h>
#include <kernel/vm.h>
#include <load_image.h>
#include <mkimg.h>
#include <platform/dpm.h>
#include <platform/mboot_expdb.h>
#include <platform/mboot_params.h>
#include <reg.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

static unsigned int set_dpm_header(unsigned char *buf)
{
    unsigned int version = DPM_VERSION;
    unsigned int hw_id = DPM_HWID;
    unsigned int nr_dpm = DPM_NUM;
    unsigned int nr_channel = DRAM_CHANNEL;
    unsigned int coredump_sz = DPM_DM_OFFSET;
    unsigned int lpif_sz = DPM_DBG_LEN;
    unsigned int cfg_sz = DPM_CFG1_LEN + DPM_CFG2_LEN;
    unsigned int dram_sz = DDRPHY_LATCH_LEN*DRAM_CHANNEL;
    unsigned int offset = 0;

    memcpy(buf + offset, &version, sizeof(version));
    offset += sizeof(version);
    memcpy(buf + offset, &hw_id, sizeof(hw_id));
    offset += sizeof(hw_id);
    memcpy(buf + offset, &nr_dpm, sizeof(nr_dpm));
    offset += sizeof(nr_dpm);
    memcpy(buf + offset, &nr_channel, sizeof(nr_channel));
    offset += sizeof(nr_channel);
    memcpy(buf + offset, &coredump_sz, sizeof(coredump_sz));
    offset += sizeof(coredump_sz);
    memcpy(buf + offset, &lpif_sz, sizeof(lpif_sz));
    offset += sizeof(lpif_sz);
    memcpy(buf + offset, &cfg_sz, sizeof(cfg_sz));
    offset += sizeof(cfg_sz);
    memcpy(buf + offset, &dram_sz, sizeof(dram_sz));
    offset += sizeof(dram_sz);

    return offset;
}

static void save_dpm_xfile(CALLBACK dev_write)
{
    int ret, xfile_size = 0;
    union mkimg_hdr *p_hdr;
    void *xfile_ptr = malloc(DPMPT_LENGTH);

    if (!xfile_ptr) {
        dprintf(CRITICAL, "xfile_ptr malloc failed\n");
        return;
    }

    ret = load_partition(DPM_IMG, DPM_PT, xfile_ptr, DPMPT_LENGTH);
    if (ret < 0) {
        dprintf(CRITICAL, "[DPM] load_DPM_image fail %s\n", DPM_PT);
        free(xfile_ptr);
        return;
    } else
        xfile_size += ret;

    if (xfile_size + sizeof(union mkimg_hdr) > DPMPT_LENGTH) {
        dprintf(CRITICAL, "DPMPT_LENGTH(0x%x) is not enough(0x%lx)\n",
            DPMPT_LENGTH, (xfile_size + sizeof(union mkimg_hdr)));
        free(xfile_ptr);
        return;
    }

    p_hdr = (union mkimg_hdr *)malloc(sizeof(union mkimg_hdr));
    if (!p_hdr) {
        dprintf(CRITICAL, "p_hdr malloc failed\n");
        free(xfile_ptr);
        return;
    }

    memset(p_hdr, 0, sizeof(union mkimg_hdr));

    p_hdr->info.magic = MKIMG_MAGIC;
    p_hdr->info.ext_magic = MKIMG_EXT_MAGIC;
    p_hdr->info.dsz = xfile_size;
    p_hdr->info.hdr_sz = MKIMG_HDR_SZ;
    p_hdr->info.hdr_ver = 1;
    p_hdr->info.align_sz = 0x10;
    ret = snprintf(p_hdr->info.name, sizeof(p_hdr->info.name), "%s", DPM_PT);
    if (ret < 0) {
        dprintf(CRITICAL, "Fail to save DPMPT name (%d)\n", ret);
        free(xfile_ptr);
        free(p_hdr);
        return;
    }

    if (!dev_write(vaddr_to_paddr((void *)p_hdr), sizeof(union mkimg_hdr)))
            dprintf(CRITICAL, "DPM xfile hdr  dump failed!\n");
    if (!dev_write(vaddr_to_paddr((void *)xfile_ptr), DPMPT_LENGTH))
            dprintf(CRITICAL, "DPM xfile dump failed!\n");

    free(p_hdr);
    free(xfile_ptr);
}
AEE_EXPDB_INIT_HOOK(SYS_DPM_XFILE, DPMPT_LENGTH, save_dpm_xfile);

static void save_dpm_data(CALLBACK dev_write)
{
    unsigned int *buf = NULL;
    unsigned int *cfg_buf = NULL;
    unsigned int headersize = 0;
    unsigned int channel_index = 0;
    unsigned int i = 0, j = 0;
    unsigned char *header = malloc(DPM_HEAD_SIZE);

    if (header == NULL) {
        dprintf(CRITICAL, "Fail to alloc memory.\n");
        return;
    }

    headersize += set_dpm_header(header);

    if (headersize > 0) {
        if (!dev_write(vaddr_to_paddr((void *)header), headersize))
            dprintf(CRITICAL, "DPM Header dump failed!\n");
    }
    free(header);

    for (i = 0; i < DPM_NUM; i++) {
        buf = malloc(DPM_DM_OFFSET);
        if (buf == NULL) {
            dprintf(CRITICAL, "Fail to alloc memory.\n");
            return;
        }
        memcpy(buf, (void const *) (DPM_DM1_SRAM_BASE + DPM_CH_OFFSET * i),
                  (int)DPM_DM_OFFSET);
        if (!dev_write(vaddr_to_paddr((void *)buf), (int)DPM_DM_OFFSET))
            dprintf(CRITICAL, "DPM_DM1_SRAM_BASE dump failed!\n");
        free(buf);

        buf = malloc(DPM_DBG_LEN);
        if (buf == NULL) {
            dprintf(CRITICAL, "Fail to alloc memory.\n");
            return;
        }
        memcpy(buf, (void const *) (DPM_DBG_LATCH_CH0 + DPM_CH_OFFSET * i),
              (int)DPM_DBG_LEN);
        if (!dev_write(vaddr_to_paddr((void *)buf), (int)DPM_DBG_LEN))
            dprintf(CRITICAL, "DPM_DBG_LATCH_CH0 dump failed!\n");
        free(buf);

        cfg_buf = malloc(DPM_CFG1_LEN + DPM_CFG2_LEN);
        if (cfg_buf == NULL) {
            dprintf(CRITICAL, "Fail to alloc memory.\n");
            return;
        }
        memset(cfg_buf, 0, DPM_CFG1_LEN + DPM_CFG2_LEN);

        for (j = 0; j < DPM_CFG1_LEN >> 2; j++)
            *(cfg_buf + j) = readl((DPM_CFG1_CH0 + DPM_CH_OFFSET * i) + j * 4);

        for (j = 0; j < DPM_CFG2_LEN >> 2; j++) {
            unsigned int cfg2_val = readl((DPM_CFG2_CH0 + DPM_CH_OFFSET * i) + j * 4);
            *(cfg_buf + (DPM_CFG1_LEN >> 2) + j) = cfg2_val;
        }
        if (!dev_write(vaddr_to_paddr(cfg_buf), (int)(DPM_CFG1_LEN + DPM_CFG2_LEN)))
            dprintf(CRITICAL, "DPM_CFG1_CH0 dump failed!\n");
        free(cfg_buf);
    }

    buf = malloc(DDRPHY_LATCH_LEN);
    if (buf == NULL) {
        dprintf(CRITICAL, "Fail to alloc memory.\n");
        return;
    }
    for (channel_index = 0; channel_index < DRAM_CHANNEL; channel_index++) {
        //ddrphy RG
        memcpy(buf, (void const *)(DDRPHY_AO_CH0_BASE + channel_index * CHANNEL_OFFSET
                                  + DDRPHY_LATCH_OFFSET),
                                  (int)DDRPHY_LATCH_LEN);
        if (!dev_write(vaddr_to_paddr((void *)buf), (int)DDRPHY_LATCH_LEN)) {
            dprintf(CRITICAL, "channel_index = %d dump failed!\n", channel_index);
            break;
        }
    }
    free(buf);
    dprintf(CRITICAL, "END DPM DUMP.\n");
}
AEE_EXPDB_INIT_HOOK(SYS_DPM_DATA, DPM_BUF_LENGTH, save_dpm_data);
