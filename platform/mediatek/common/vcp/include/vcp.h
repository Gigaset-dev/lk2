/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#pragma once

#define VCP_DRAM_BOUND_LOWER    0x240000000ULL
#define VCP_DRAM_BOUND_UPPER    0x340000000ULL
#define VCP_DRAM_RESV_NAME      "me_vcp_reserved"

/**********************************************************************
 * Common application interfaces
 **********************************************************************/
int vcp_get_exist(void);
int vcp_get_vcpctl(void);
int vcp_get_sram_size(void);
int vcp_get_resv_dram_size(void);
int vcp_get_core_num(void);
void *vcp_dram_alloc(void **phy, unsigned int size);
void vcp_dram_free(void *phy);
int verify_load_vcp_image(const char *part_name, void *addr_phy, void *addr);
void vcp_emi_protect_enable(unsigned long long addr, unsigned int size);
void vcp_set_devapc_domain(void);
void vcp_power_on(void);
void vcp_power_off(void);

enum MTK_TINYSYS_VCP_BL_OP {
    MTK_TINYSYS_VCP_BL_OP_SHAREDRAM = 0,
    MTK_TINYSYS_VCP_BL_OP_DRAM,
    MTK_TINYSYS_VCP_BL_OP_SECURE_INIT_VCP,
    MTK_TINYSYS_VCP_BL_OP_NUM,
};
