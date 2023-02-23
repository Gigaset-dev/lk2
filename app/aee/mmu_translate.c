/*
 * Copyright (c) 2019 MediaTek Inc.
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files
 * (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge,
 * publish, distribute, sublicense, and/or sell copies of the Software,
 * and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */
#include <platform/mboot_params.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "KEHeader.h"
#include "mrdump_private.h"

static int read_mem(uint64_t paddr, void *buf, unsigned long size)
{
    uint64_t ret;

    ret = kedump_mem_read(paddr, size, buf);
    return (ret == size) ? 0 : -1;
}

#define PNUM_64 (PAGE_SIZE / sizeof(uint64_t))
static uint64_t lxpage[PNUM_64];

/* ARM PGD/PMD first 3 pages for userspace, last 1 pages for kernel space */
#define PNUM_32 (PAGE_SIZE * 4 / sizeof(uint32_t))
static uint32_t l1page32[PNUM_32];

/* ARM L2 PTE 512 entry and each entry 4 bytes */
#define PNUM_L2_32 512
static uint32_t l2page32[PNUM_L2_32];

#define VA_MASK_64 0x00ffff0000000000

static uint64_t mmutable_64_translate_l3(uint64_t vaddr,
        uint64_t ptable)
{
    int ret;
    uint64_t paddr;
    unsigned int lxidx = (vaddr >> 12) & 0x1ff;

    memset(lxpage, 0, PAGE_SIZE);
    ret = read_mem(ptable, lxpage, PAGE_SIZE);
    if (ret != 0) {
        LOG("kedump:read l3 ptable fail\n");
        return 0;
    }

    switch (lxpage[lxidx] & 3) {
    case 3:
        paddr = (lxpage[lxidx] & 0x0000fffffffff000) + (vaddr & 0xfff);
        break;
    default:
        dprintf(INFO, "kedump: l3 invalid (%d)ent value:0x%llx @0x%llx\n",
                lxidx, lxpage[lxidx], vaddr);
        paddr = 0;
    }

    return paddr;
}

static uint64_t mmutable_64_translate_l2(uint64_t vaddr,
        uint64_t ptable)
{
    int ret;
    uint64_t paddr;
    uint64_t next_table_address;
    unsigned int lxidx = (vaddr >> 21) & 0x1ff;

    memset(lxpage, 0, PAGE_SIZE);
    ret = read_mem(ptable, lxpage, PAGE_SIZE);
    if (ret != 0) {
        LOG("kedump: read l2 ptable fail\n");
        return 0;
    }

    switch (lxpage[lxidx] & 3) {
    case 1:
        paddr = (lxpage[lxidx] & 0x0000ffffffe00000) + (vaddr & 0x1fffff);
        break;
    case 3:
        next_table_address = lxpage[lxidx] & 0x0000fffffffff000;
        paddr = mmutable_64_translate_l3(vaddr, next_table_address);
        break;
    default:
        dprintf(INFO, "kedump: l2 invalid (%d)ent value:0x%llx @0x%llx\n",
                lxidx, lxpage[lxidx], vaddr);
        paddr = 0;
    }

    return paddr;
}

static uint64_t mmutable_64_translate_l1(uint64_t vaddr,
        uint64_t ptable)
{
    int ret;
    uint64_t paddr;
    uint64_t next_table_address;
    unsigned int lxidx = (vaddr >> 30) & 0x1ff;

    memset(lxpage, 0, PAGE_SIZE);
    ret = read_mem(ptable, lxpage, PAGE_SIZE);
    if (ret != 0) {
        LOG("kedump: read l1 ptable fail\n");
        return 0;
    }

    switch (lxpage[lxidx] & 3) {
    case 1:
        paddr = (lxpage[lxidx] & 0x0000ffffc0000000) + (vaddr & 0x3fffffff);
        break;
    case 3:
        next_table_address = lxpage[lxidx] & 0x0000fffffffff000;
        paddr = mmutable_64_translate_l2(vaddr, next_table_address);
        break;
    default:
        dprintf(INFO, "kedump: l1 invalid (%d)ent value:0x%llx @0x%llx\n",
                lxidx, lxpage[lxidx], vaddr);
        paddr = 0;
    }

    return paddr;
}

uint64_t v2p_64(uint64_t vaddr)
{
    uint64_t mpt;
    uint64_t paddr = 0;

    mpt = get_mpt();

    if ((vaddr & VA_MASK_64) == VA_MASK_64)
        paddr = mmutable_64_translate_l1(vaddr, mpt);

    return paddr;
}

static uint64_t mmutable_32_translate_l2(uint64_t vaddr, uint64_t ptable)
{
    int ret;
    uint64_t pa = 0;

    unsigned int lxidx = (vaddr >> 12) & 0x1ff;

    memset(l2page32, 0, sizeof(l2page32));
    ret = read_mem(ptable, l2page32, sizeof(l2page32));
    if (ret != 0) {
        LOG("invalid l2 page table\n");
        return 0;
    }

    if (l2page32[lxidx] & 1)
        pa = (l2page32[lxidx] & 0xfffff000) + (vaddr & 0xfff);
    else
        dprintf(INFO, "l2 invalid (%d)ent value:0x%x @0x%llx\n", lxidx, l2page32[lxidx], vaddr);

    return pa;
}

static uint64_t mmutable_32_translate_l1(uint64_t vaddr, uint64_t ptable)
{
    int ret;
    uint64_t pa;
    uint64_t next_table_address;
    unsigned int lxidx = (vaddr >> 20) & 0xfff;

    memset(l1page32, 0, sizeof(l1page32));
    ret = read_mem(ptable, l1page32, sizeof(l1page32));
    if (ret != 0) {
        LOG("invalid l1 page table\n");
        return 0;
    }

    switch (l1page32[lxidx] & 3) {
    case 1:
        /* PMD point address is 1KB align */
        next_table_address = l1page32[lxidx] & 0xfffff000;
        pa = mmutable_32_translate_l2(vaddr, next_table_address);
        break;
    case 2:
        /* [31:20]12bit as PMD index */
        pa = (l1page32[lxidx] & 0xfff00000) + (vaddr & 0xfffff);
        break;
    default:
        dprintf(INFO, "l1 invalid (%d)ent value:0x%x @0x%llx\n", lxidx, l1page32[lxidx], vaddr);
        pa = 0;
    }

    return pa;
}

uint64_t v2p_32(uint64_t vaddr)
{
    uint64_t mpt = get_mpt();

    if (vaddr > 0xbf000000)
        return mmutable_32_translate_l1(vaddr, mpt);

    return 0;
}
