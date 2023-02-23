/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#pragma once

#include <debug.h>

struct adsp_imgs_t {
    char name[20];
    void *base;
    unsigned int size;
};

enum mtk_adsp_bl_ops {
    MTK_ADSP_BL_OP_CFG_MPU = 0, /*can be replaced after MTK_ADSP_BL_OP_SHAREDRAM implemented */
    MTK_ADSP_BL_OP_SHAREDRAM,
    MTK_ADSP_BL_OP_SECURE_INIT,
    MTK_ADSP_BL_OP_LOADER_STATUS,
    MTK_ADSP_BL_OP_LOAD_IMG,
    MTK_ADSP_BL_OP_CFG_BOOTUP,
    MTK_ADSP_BL_OP_NUM
};

/******************************************************************************
 * DTS definition
 ******************************************************************************/
#define ADSP_DT_NODE_MEM_RSV     "mediatek,reserve-memory-adsp"
#define ADSP_DT_NODE_MEM_SHARED  "mediatek,reserve-memory-adsp_share"
#define SND_ADSP_DT_NODE         "mediatek,snd_audio_dsp"

#define ADSP_RSV_PROPERTY_PREFIX "adsp-rsv"
#define ADSP_RSV_AUDIO_PROPERTY  "adsp-rsv-audio"

#define ADSP_IMG_RSV_NAME         "adsp_image_reserved"
#define ADSP_SHARED_RSV_NAME      "adsp_shared_reserved"
#define CCCI_SHAREDC_RSV_NAME     "ap_md_c_smem"

/**********************************************************************
 * Common definition
 **********************************************************************/
#define udelay(x)                spin(x)
#define mdelay(x)                udelay((x) * 1000)
#define ADDR_PHY_TO_VIR(addr)    (IO_BASE + (addr - (IO_BASE_PHY)))
#define LOG_TAG                  "ADSP"

/**********************************************************************
 * Common application interfaces
 **********************************************************************/
int adsp_get_status(void);
void adsp_set_status(int status);
int adsp_core_exist(unsigned int cid);
unsigned int adsp_get_core_num(void);
struct adsp_imgs_t *adsp_get_img_info(unsigned int cid, unsigned int idx);
int adsp_set_img_info(unsigned int cid, unsigned int idx, struct adsp_imgs_t *img);
void print_adsp_img_info(void);

/**********************************************************************
 * Platform interfaces form common code,
 * which should be implemented by version.
 **********************************************************************/
void *adsp_dram_alloc(void **phy, unsigned int *size);
void adsp_update_sram_imgs_base(void *base);
void adsp_emi_protect_enable(const unsigned int id, void *addr, unsigned int size);
int verify_load_adsp_img(const char *part_name, void *addr_phy, void *addr);
void adsp_set_devapc_domain(void);
void adsp_power_on(void);
void adsp_power_off(void);
