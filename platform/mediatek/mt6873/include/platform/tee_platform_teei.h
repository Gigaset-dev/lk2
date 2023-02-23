/*
 * Copyright (c) 2021 MediaTek Inc.
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
#define MT_UART0_IRQ  (141)
#define MT_UART1_IRQ  (142)
#define MT_UART2_IRQ  (143)
#define MT_UART3_IRQ  (0)
#define MT_UART0_DAPC (0)
#define MT_UART1_DAPC (0)
#define MT_UART2_DAPC (0)
#define MT_UART3_DAPC (0)

/* Secure Timer setting (SEJ) */
#define MT_SEC_GPT_BASE (0x1000A000)
#define MT_SEC_GPT_IRQ  (262)
#define MT_SEC_GPT_DAPC (0)

/* Secure watch Dog setting (SEJ) */
#define MT_SEC_WDT_BASE (0x1000A000)
#define MT_SEC_WDT_IRQ  (263)
#define MT_SEC_WDT_DAPC (0)

/* Secure IRQ used in TEE */
#define TEE_FCH_IRQ_NO    (120)
#define TEE_DRV_IRQ_NO    (121)
#define TEE_REV_IRQ_NO    (119)
#define TEE_DUMPER_IRQ_NO (122)

/* Crypto module info */
#define MT_SEC_CRYPTO_BASE 0x10210000
#define MT_SEC_CRYPTO_IRQ  (0)
#define MT_SEC_CRYPTO_DAPC (0)

