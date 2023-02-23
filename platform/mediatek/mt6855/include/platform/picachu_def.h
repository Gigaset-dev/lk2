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
#define MTK_CHIP                        "MT6855"

#define NR_CPUS                         (8)
#define NR_BINS                         (2)
#define NR_BINNING_VERSIONS             (1)

#define HAS_CLUSTER_LL                  (0)
#define HAS_CLUSTER_L                   (1)
#define HAS_CLUSTER_M                   (0)
#define HAS_CLUSTER_B                   (1)
#define HAS_CLUSTER_CCI_DSU             (1)


/*****************************************************************************
 *                      LUT Define
 *****************************************************************************/
#define NR_EEMSN_DET   3
#define NR_FREQ        6

/*****************************************************************************
 *                      SD Define
 *****************************************************************************/
#define MAX_SD_GLOBAL_SIZE   30


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


