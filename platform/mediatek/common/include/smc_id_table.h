/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#pragma once

#define MTK_SIP_LK_DAPC_INIT_AARCH32           0x82000101
#define MTK_SIP_LK_DUMP_ATF_LOG_INFO           0x8200010A
#define MTK_SIP_RAM_DUMP_ADDR                  0x82000114
#define MTK_SIP_KERNEL_SPM_ARGS                0x8200022A
#define MTK_SIP_BL_ATF_CONTROL_AARCH32         0x82000400
#define MTK_SIP_BL_LPM_CONTROL                 0x82000410
#define MTK_SIP_EMIMPU_CONTROL                 0x82000415

#if ARCH_ARM
#define MTK_SIP_FILL_MD_PUBK_HASH              0x82000102
#define MTK_SIP_LK_CRYPTO_HW_ENGINE_INIT       0x8200010B
#define MTK_SIP_LK_CRYPTO_HW_ENGINE_DISABLE    0x8200010C
#define MTK_SIP_LK_CRYPTO_SHA256_INIT          0x8200010D
#define MTK_SIP_LK_CRYPTO_SHA256_PROCESS       0x8200010E
#define MTK_SIP_LK_CRYPTO_SHA256_DONE          0x8200010F
#define MTK_SIP_LK_GET_RND                     0x82000110
#define MTK_SIP_LK_CRYPTO_SHA384_INIT          0x82000111
#define MTK_SIP_LK_CRYPTO_SHA384_PROCESS       0x82000112
#define MTK_SIP_LK_CRYPTO_SHA384_DONE          0x82000113
#define MTK_SIP_KERNEL_BOOT                    0x82000115
#define MTK_SIP_LK_ROOT_OF_TRUST               0x82000120
#define MTK_SIP_BL_CCCI_CONTROL                0x8200040B
#define MTK_SIP_BL_APUSYS_CONTROL              0x8200040C
#define MTK_SIP_BL_UFS_CONTROL                 0x8200040D
#define MTK_SIP_BL_TINYSYS_SCP_CONTROL         0x8200040F
#define MTK_SIP_BL_SSC_CONTROL                 0x82000411
#define MTK_SIP_BL_CCU_CONTROL                 0x82000412
#define MTK_SIP_BL_TINYSYS_VCP_CONTROL         0x82000413
#define MTK_SIP_BL_MMC_CONTROL                 0x82000414
#define MTK_SIP_BL_DFD_MCU_CONTROL             0x82000416
#define MTK_SIP_BL_DCM_CONTROL                 0x82000417
#define MTK_SIP_BL_CMDQ_CONTROL                0x82000418
#define MTK_SIP_BL_ADSP_CONTROL                0x82000419
#define MTK_SIP_BL_TINYSYS_GPUEB_CONTROL       0x8200041B
#elif ARCH_ARM64
#define MTK_SIP_FILL_MD_PUBK_HASH              0xC2000102
#define MTK_SIP_LK_CRYPTO_HW_ENGINE_INIT       0xC200010B
#define MTK_SIP_LK_CRYPTO_HW_ENGINE_DISABLE    0xC200010C
#define MTK_SIP_LK_CRYPTO_SHA256_INIT          0xC200010D
#define MTK_SIP_LK_CRYPTO_SHA256_PROCESS       0xC200010E
#define MTK_SIP_LK_CRYPTO_SHA256_DONE          0xC200010F
#define MTK_SIP_LK_GET_RND                     0xC2000110
#define MTK_SIP_LK_CRYPTO_SHA384_INIT          0xC2000111
#define MTK_SIP_LK_CRYPTO_SHA384_PROCESS       0xC2000112
#define MTK_SIP_LK_CRYPTO_SHA384_DONE          0xC2000113
#define MTK_SIP_KERNEL_BOOT                    0xC2000115
#define MTK_SIP_LK_ROOT_OF_TRUST               0xC2000120
#define MTK_SIP_BL_CCCI_CONTROL                0xC200040B
#define MTK_SIP_BL_APUSYS_CONTROL              0xC200040C
#define MTK_SIP_BL_UFS_CONTROL                 0xC200040D
#define MTK_SIP_BL_TINYSYS_SCP_CONTROL         0xC200040F
#define MTK_SIP_BL_SSC_CONTROL                 0xC2000411
#define MTK_SIP_BL_CCU_CONTROL                 0xC2000412
#define MTK_SIP_BL_TINYSYS_VCP_CONTROL         0xC2000413
#define MTK_SIP_BL_MMC_CONTROL                 0xC2000414
#define MTK_SIP_BL_DFD_MCU_CONTROL             0xC2000416
#define MTK_SIP_BL_DCM_CONTROL                 0xC2000417
#define MTK_SIP_BL_CMDQ_CONTROL                0xC2000418
#define MTK_SIP_BL_ADSP_CONTROL                0xC2000419
#define MTK_SIP_BL_TINYSYS_GPUEB_CONTROL       0xC200041B
#endif

#define MTK_SIP_KERNEL_EMIMPU_SET              0x82000262
#define MTK_SIP_KERNEL_EMIMPU_CLEAR            0x82000263

#define MTK_STD_LK_MBLOCK_INFO                 0x84000001
#define MTK_STD_LK_SECURE_REGION_QUERY         0x84000002
#define MTK_STD_LK_EMIMPU_REGION_INFO          0x84000003
