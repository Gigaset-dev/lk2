/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#pragma once

#include <arch/ops.h>
#include <debug.h>
#include <kernel/event.h>
#include <kernel/vm.h>
#include <malloc.h>
#include <platform.h>
#include <platform/interrupts.h>
#include <platform/irq.h>
#include <platform/reg.h>
#include <platform/timer.h>
#include <reg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

enum nand_irq_return {
    NAND_IRQ_NONE,
    NAND_IRQ_HANDLED,
};

enum nand_dma_operation {
    NDMA_FROM_DEV,
    NDMA_TO_DEV,
};

#define pr_err(fmt, ...) \
    dprintf(CRITICAL, "[ERR]" fmt, ##__VA_ARGS__)
#define pr_warn(fmt, ...) \
    dprintf(ALWAYS, "[WARN]" fmt, ##__VA_ARGS__)
#define pr_info(fmt, ...) \
    dprintf(INFO, fmt, ##__VA_ARGS__)
#define pr_debug(fmt, ...) \
    do {} while (0)

#define NANDX_LOG(fmt, ...) dprintf(CRITICAL, fmt, ##__VA_ARGS__)

#define NANDX_ASSERT    assert

#define NANDX_BULK_IO_USE_DRAM  1
#define NANDX_NFI_BUF_ADDR      NAND_BUF_ADDR
#define NANDX_NFI_BUF_ADDR_LEN  (4096 + 256)
#define NANDX_CORE_BUF_ADDR     (NANDX_NFI_BUF_ADDR + NANDX_NFI_BUF_ADDR_LEN)
#define NANDX_CORE_BUF_LEN      (2 * (4096 + 256))
#define NANDX_BBT_BUF_ADDR      (NANDX_CORE_BUF_ADDR + NANDX_CORE_BUF_LEN)
#define NANDX_BBT_BUF_LEN       (8192)
#define NANDX_BBT_MAN_BUF_ADDR  (NANDX_BBT_BUF_ADDR + NANDX_BBT_BUF_LEN)
#define NANDX_BBT_MAN_BUF_LEN   (8192)
#define NANDX_TEST_R_ADDR       (NANDX_BBT_MAN_BUF_ADDR + NANDX_BBT_MAN_BUF_LEN)
#define NANDX_TEST_R_LEN        (4096 + 256)
#define NANDX_TEST_W_ADDR       (NANDX_TEST_R_ADDR + NANDX_TEST_R_LEN)
#define NANDX_TEST_W_LEN        (4096 + 256)

static inline void *mem_alloc(u32 count, u32 size)
{
    return calloc(count, size);
}

static inline void mem_free(void *mem)
{
    if (mem)
        free(mem);
}

#define nandx_irq_register(dev, irq, irq_handler, name, data) \
    ({ \
        int __ret; \
        register_int_handler(irq, irq_handler, data); \
        __ret = unmask_interrupt(irq); \
        __ret; \
    })

static inline u64 get_current_time_us(void)
{
    return current_time();
}

static inline u32 nandx_dma_map(void *dev, void *buf, u64 len,
                enum nand_dma_operation op)
{
    u32 addr;

#if WITH_KERNEL_VM
    addr = (u32)vaddr_to_paddr(buf);
#else
    addr = (u32)buf;
#endif

    arch_clean_invalidate_cache_range((addr_t)buf, (size_t)len);

    return addr;
}

static inline void nandx_dma_unmap(void *dev, void *buf, void *addr,
                   u64 len,
                   enum nand_dma_operation op)
{
    arch_clean_invalidate_cache_range((addr_t)buf, len);
}

#define container_of    containerof

