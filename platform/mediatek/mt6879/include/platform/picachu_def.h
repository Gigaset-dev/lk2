/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */
#pragma once


/*******************************************************************************
 *                CPU CORE Define
 ******************************************************************************/
#define MTK_CHIP                        "MT6879"

#define NR_CPUS                         (8)
#define NR_BINS                         (2)
#define NR_PMIC_VPROC                   (2)
#define NR_BINNING_VERSIONS             (1)

#define L_CORE_NUM                      (6)
#define M_CORE_NUM                      (0)
#define B_CORE_NUM                      (2)
#define NR_CPU_CORE                     (L_CORE_NUM + M_CORE_NUM + B_CORE_NUM)

#define HAS_CLUSTER_LL                  (0)
#define HAS_CLUSTER_L                   (1)
#define HAS_CLUSTER_M                   (0)
#define HAS_CLUSTER_B                   (1)
#define HAS_CLUSTER_CCI_DSU             (1)

#define MAX_MP0_CPU_NR                  (8)
#define MAX_MP1_CPU_NR                  (0)

#if (MAX_MP1_CPU_NR == 0)
#define MAX_CLUSTER_NR                  (1)
#else
#define MAX_CLUSTER_NR                  (2)
#endif


/*******************************************************************************
 *                        Hardware Feature Define
 ******************************************************************************/
#define HAS_AACH64_ONLY                 (1)
#define SUPPORT_SENSOR_NETWORK          (1)
#define DEFAULT_MARGIN_VERSION          (1)


/*****************************************************************************
 *                      LUT Define
 *****************************************************************************/
#define NR_EEMSN_DET   3
#define NR_FREQ        5

/*****************************************************************************
 *                      SD Define
 *****************************************************************************/
#define MAX_SD_GLOBAL_SIZE   4


/*******************************************************************************
 *                        Cluster & PMIC ID Define
 ******************************************************************************/
enum mt_cpu_dvfs_id {
#if HAS_CLUSTER_LL
   MT_CPU_DVFS_LL,
#endif

#if HAS_CLUSTER_L
   MT_CPU_DVFS_L,
#endif

#if HAS_CLUSTER_M
   MT_CPU_DVFS_M,
#endif

#if HAS_CLUSTER_B
   MT_CPU_DVFS_B,
#endif

#if HAS_CLUSTER_CCI_DSU
   MT_CPU_DVFS_CCI,
#endif

   NR_MT_CPU_DVFS
};

enum mt_vproc_id {
   MT_VPROC1,

#if (NR_PMIC_VPROC < 3)
   MT_VPROC2,
#endif

#if (NR_PMIC_VPROC == 3)
   MT_VPROC3,
#endif

   NR_VPROC
};
