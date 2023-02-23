/*
 * Copyright (c) 2022 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#pragma once

#include <platform/gic.h>

/* GIC setting */
#define GIC_VERSION  (0x03)

#define GIC_BASE  GIC_BASE_PHY
#define GIC_RBASE GIC_REDIS_BASE_PHY

/* UART setting */
#define MT_UART0_IRQ  (173)
#define MT_UART1_IRQ  (174)
#define MT_UART0_DAPC (0)
#define MT_UART1_DAPC (0)

/* Secure IRQ used in TEE */
#define TEE_REV_IRQ_NO    (149)

