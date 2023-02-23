/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#pragma once

enum MTK_TINYSYS_SCP_BL_OP {
    MTK_TINYSYS_SCP_BL_OP_SHAREDRAM = 0,
    MTK_TINYSYS_SCP_BL_OP_DRAM,
    MTK_TINYSYS_SCP_BL_OP_RESVDRAM,
    MTK_TINYSYS_SCP_BL_OP_REGION_INFO,
    MTK_TINYSYS_SCP_BL_OP_DEVAPC,
    MTK_TINYSYS_SCP_BL_OP_DEVAPC_L2TCM,
    MTK_TINYSYS_SCP_BL_OP_NUM,
};

/**********************************************************************
 * Platform interfaces form common code,
 * which should be implemented by each platform.
 **********************************************************************/
void *scp_dram_alloc(void **phy, unsigned int *size);
int verify_load_scp_image(const char *part_name, void *addr_phy, void *addr);
void scp_emi_protect_enable(void *addr);
void scp_set_devapc_domain(void);
void scp_set_devapc_regbank(void);
void scp_set_devapc_l2tcm(void);
void disable_scp_hw(void);
void scp_sharedram_alloc(void);

