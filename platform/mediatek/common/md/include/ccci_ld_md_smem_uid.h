/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#pragma once

/* Please make sure sync with kernel */
enum SMEM_USER_ID {
    SMEM_PADDING = -1, /* Keep this no change */
    /* this should remain to be 0 for backward compatibility */
    SMEM_USER_RAW_DBM = 0,

    /* sequence in CCB users matters, must align with ccb_configs[]  */
    SMEM_USER_CCB_START,
    SMEM_USER_CCB_DHL = SMEM_USER_CCB_START,
    SMEM_USER_CCB_MD_MONITOR,
    SMEM_USER_CCB_META,
    SMEM_USER_CCB_END = SMEM_USER_CCB_META,

    /* squence of other users does not matter */
    SMEM_USER_RAW_CCB_CTRL,
    SMEM_USER_RAW_DHL,
    SMEM_USER_RAW_MDM,
    SMEM_USER_RAW_NETD,
    SMEM_USER_RAW_USB,
    SMEM_USER_RAW_AUDIO,
    SMEM_USER_RAW_DFD, /* 10 */
    SMEM_USER_RAW_LWA,
    SMEM_USER_RAW_MDCCCI_DBG,
    SMEM_USER_RAW_MDSS_DBG,
    SMEM_USER_RAW_RUNTIME_DATA,
    SMEM_USER_RAW_FORCE_ASSERT,
    SMEM_USER_CCISM_SCP,
    SMEM_USER_RAW_MD2MD,
    SMEM_USER_RAW_RESERVED,
    SMEM_USER_CCISM_MCU,
    SMEM_USER_CCISM_MCU_EXP, /* 20 */
    SMEM_USER_SMART_LOGGING,
    SMEM_USER_RAW_MD_CONSYS,
    SMEM_USER_RAW_PHY_CAP,
    SMEM_USER_RAW_USIP,
    SMEM_USER_RESV_0,// Sync to MT6779 SMEM_USER_MAX_K,
    SMEM_USER_ALIGN_PADDING, // Sync to MT6779 SMEM_USER_NON_PADDING
    SMEM_USER_RAW_UDC_DATA, /* 27 */
    SMEM_USER_RAW_UDC_DESCTAB,
    SMEM_USER_RAW_AMMS_POS,
    SMEM_USER_RAW_AMMS_ALIGN_PADDING,
    SMEM_USER_MD_WIFI_PROXY, /* 31 */
    SMEM_USER_MD_NVRAM_CACHE, /* 32 */
    SMEM_USER_LOW_POWER,
    SMEM_USER_SECURITY_SMEM,
    SMEM_USER_SAP_EX_DBG,
    SMEM_USER_SAP_DFD_DBG, // 36
    SMEM_USER_32K_LOW_POWER,
    SMEM_USER_USB_DATA,
    SMEM_USER_MD_CDMR, /* CDMR:Crash Dump Memory Region/MIDR:Modem Internals Dump Region */
    SMEM_USER_RESERVED,  //40
    SMEM_USER_MD_DRDI, //41
    SMEM_USER_MD_DATA, //42
    SMEM_USER_LAST, /* Make sure USER ID sync with Kernel before SMEM_USER_LAST */
    /* Note, if you add more user, please update the followig table: smem_align_ref[] */
};
