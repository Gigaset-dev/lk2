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
#define MTK_CHIP                        "MT6983"

#define NR_CPUS                         (8)
#define NR_BINS                         (2)
#define NR_PMIC_VPROC                   (3)
#define NR_BINNING_VERSIONS             (1)

#define L_CORE_NUM                      (4)
#define M_CORE_NUM                      (3)
#define B_CORE_NUM                      (1)
#define NR_CPU_CORE                     (L_CORE_NUM + M_CORE_NUM + B_CORE_NUM)

#define HAS_CLUSTER_LL                  (0)
#define HAS_CLUSTER_L                   (1)
#define HAS_CLUSTER_M                   (1)
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
#define HAS_PWR_ISO_REG                 (0)
#define HAS_AACH64_ONLY                 (1)
#define SUPPORT_VOLT_CALIBRATION        (0)
#define SUPPORT_BIG_PAGE_TABLE          (0)
#define ENABLE_FREQ_METER               (0)
#define DUMP_ALL_EFUSE_VMIN             (0)
#define SUPPORT_OUTPUT_VMIN_H           (0)
#define DUMP_SENSOR_NETWORK_REG         (0)
#define SUPPORT_SENSOR_NETWORK          (1)

#define DEFAULT_MARGIN_VERSION          (1)


/*******************************************************************************
 *                        DRAM LOG Define
 ******************************************************************************/
#define PI_DRAM_SIZE                    (0x00400000UL)
#define PI_ALIGNMENT                    (0x00100000UL)
#define PI_ADDR_MAX                     (0x80000000UL)

/*****************************************************************************
 *                      LUT Define
 *****************************************************************************/
#define NR_EEMSN_DET   4
#define NR_FREQ        10

/*****************************************************************************
 *                      SD Define
 *****************************************************************************/
#define MAX_SD_GLOBAL_SIZE   10

/*****************************************************************************
 *         pattern select
 *****************************************************************************/
extern void aarch64_start_slave(void);


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
