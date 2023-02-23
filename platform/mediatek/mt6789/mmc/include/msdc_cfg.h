/*
 *
 * Copyright (c) 2022 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */
#pragma once

/*--------------------------------------------------------------------------*/
/* Common Definition                                                        */
/*--------------------------------------------------------------------------*/
#ifdef PROJECT_TYPE_FPGA
#define FPGA_PLATFORM
#endif

#ifndef MSDC1_EMMC
#define SDCARD_SUPPORT
#endif

#define MSDC_MAX_NUM 2

/* HW deal with the 2K DMA boundary limitation, SW do nothing with it */
/* Most of eMMC request in lk are sequential access, so it's no need to
 * use descript DMA mode, I just remove relevant codes. JieWu@20160607
 */
#define MSDC_USE_DMA_MODE

#define FEATURE_MMC_WR_TUNING
#define FEATURE_MMC_RD_TUNING
#define FEATURE_MMC_CM_TUNING
//#define FEATURE_MMC_TUNING_EDGE_ONLY_WHEN_HIGH_SPEED

/* Maybe we discard these macro definition */
//#define MSDC_USE_PATCH_BIT2_TURNING_WITH_ASYNC

/*--------------------------------------------------------------------------*/
/* Debug Definition                                                         */
/*--------------------------------------------------------------------------*/
//#define KEEP_SLIENT_BUILD
//#define ___MSDC_DEBUG___
#define MSDC_TUNE_LOG (1)
