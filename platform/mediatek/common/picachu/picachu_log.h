/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */
#pragma once


/*****************************************************************************
 *                        common C lib
 *****************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>



/*****************************************************************************
 *                        TARGET_BUILD_VARIANT_SLT
 *****************************************************************************/
#ifdef TARGET_BUILD_VARIANT_SLT
#include "common.h"  /* printf */
#include "kal_type.h" /* KAL_malloc */

#define PRINTF printf
#endif


/*****************************************************************************
 *                        TARGET_BUILD_VARIANT_MCUPM
 *****************************************************************************/
#ifdef TARGET_BUILD_VARIANT_MCUPM
#define PRINTF printf
#endif


/*****************************************************************************
 *                        LK_AS_BL2_EXT
 *****************************************************************************/
#if LK_AS_BL2_EXT
#include <debug.h> /* printf */
#include <err.h>
#include <load_image.h>        /* load_partition */

#define PRINTF(...)        dprintf(INFO, __VA_ARGS__)
#endif


/*****************************************************************************
 *                        PICACHU LOG DEFINE
 *****************************************************************************/
#define PART_MALLOC_SIZE                0x100000

#define SRAM_LOG_SIZE                   4096
#define SECTION_SIZE                    0x10000 /* 64KB */

#define PICACHU_LOG_SIZE                0x80000 /* 512KB */
#define PICACHU_LOG_IDX                 4
#define PICACHU_LOG_BACKUP_IDX          5
#define PICACHU_KERNEL_IDX              6
#define PICACHU_AEE_IDX                 7
#define PICACHU_LOG_PARTITION_SIZE      0x1000 /* 4KB */


/*****************************************************************************
 *                        Debug message macro.
 *****************************************************************************/
extern int DEBUG_MODE;

#ifdef TARGET_BUILD_VARIANT_MCUPM
    #define PICACHU_DEBUG(str, ...) do { \
    } while (0)

    #define PICACHU_INFO(str, ...) do { \
    } while (0)
#else
    #define PICACHU_DEBUG(str, ...) do { \
        if (DEBUG_MODE) \
            PRINTF(str, ##__VA_ARGS__); \
    } while (0)

    #define PICACHU_INFO(str, ...) do { \
        PRINTF(str, ##__VA_ARGS__); \
        picachu_print("INFO: " str, ##__VA_ARGS__); \
    } while (0)
#endif

#define PICACHU_WARN(str, ...) do { \
    PRINTF("WARN: " str, ##__VA_ARGS__); \
    picachu_print("WARN: " str, ##__VA_ARGS__); \
} while (0)

#define PICACHU_ERR(str, ...) do { \
    PRINTF("ERR: " str, ##__VA_ARGS__); \
    picachu_print("ERR: " str, ##__VA_ARGS__); \
} while (0)



/*****************************************************************************
 *                        inline function
 *****************************************************************************/
#define picachu_read32(addr) \
    (*(unsigned int *)((unsigned long)(addr)))
#define picachu_write32(addr, val) \
    (*(unsigned int *)((unsigned long)(addr)) = (unsigned long)(val))


/*****************************************************************************
 *                        External function
 *****************************************************************************/
unsigned long long picachu_log_init(void);
void picachu_print(const char *fmt, ...);
void picachu_log_dump(void);
