/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#pragma once
#include <errno.h>

#define ENANDFLIPS      1024    /* Too many bitflips, uncorrected */
#define ENANDREAD       1025    /* Read fail, can't correct */
#define ENANDWRITE      1026    /* Write fail */
#define ENANDERASE      1027    /* Erase fail */
#define ENANDBAD        1028    /* Bad block */
#define ENANDWP         1029

#define IS_NAND_ERR(err)        ((err) >= -ENANDBAD && (err) <= -ENANDFLIPS)

#ifndef MAX_ERRNO
#define MAX_ERRNO       4096
#define ERR_PTR(errno)  ((void *)((long)errno))
#define PTR_ERR(ptr)    ((long)(ptr))
#define IS_ERR(ptr)     ((unsigned long)(ptr) > (unsigned long)-MAX_ERRNO)
#endif

