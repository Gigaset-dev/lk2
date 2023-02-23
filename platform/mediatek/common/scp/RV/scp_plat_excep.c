/*
 * copyright (c) 2021 mediatek inc.
 *
 * use of this source code is governed by a mit-style
 * license that can be found in the license file or at
 * https://opensource.org/licenses/mit
 */
#include <arch/mmu.h>
#include <debug.h>
#include <dtb.h>
#include <err.h>
#include <kernel/vm.h>
#include <platform/mboot_expdb.h>
#include <platform/memory_layout.h>
#include <platform/reg.h>
#include <libfdt.h>
#include <lib/zlib.h>
#include <platform/wdt.h>
#include <scp.h>
#include <scp_plat_excep.h>
#include "scp_plat_priv.h"
#include <stdio.h>
#include <string.h>
#include <trace.h>

#define LOCAL_TRACE (0)

struct reg_save_st {
    uint32_t addr;
    uint32_t size;
};

struct reg_save_st reg_save_list[] = {
    /* size must 16 byte alignment */
    {0x00021000, 0x120},
    {0x00024000, 0x170},
    {0x00030000, 0x180},
    {0x00032000, 0x260},
    {0x00033000, 0x120},
    {0x00040000, 0x180},
    {0x00042000, 0x260},
    {0x00043000, 0x120},
    {0x00050000, 0x330},
    {0x00051000, 0x10},
    {0x00051400, 0x70},
    {0x00052000, 0x400},
    {0x000A5000, 0x110},
    {0x10001B14, 0x10},
};

struct scp_dump_st {
    uint8_t *ramdump;
    /* use prefix to get size or offset in O(1) to save memory */
    uint32_t prefix[MDUMP_TOTAL];
};

static struct scp_dump_st scp_dump;


static uint32_t get_MDUMP_size(enum MDUMP_t type)
{
    return scp_dump.prefix[type] - scp_dump.prefix[type - 1];
}

static uint32_t get_MDUMP_size_accumulate(enum MDUMP_t type)
{
    return scp_dump.prefix[type];
}

static uint8_t *get_MDUMP_addr(enum MDUMP_t type)
{
    return (uint8_t *)(scp_dump.ramdump + scp_dump.prefix[type - 1]);
}

void scp_memorydump_size_probe(void *fdt)
{

    uint32_t i;
    int len = 0, nodeoffset = 0;
    uint32_t *data = NULL;

    LTRACEF("[SCP] %s:\n", __func__);

    nodeoffset = fdt_node_offset_by_compatible(fdt, -1, "mediatek,scp");
    if (nodeoffset < 0) {
        LTRACEF("[SCP] get node fail, nodeoffset = 0x%x\n", nodeoffset);
        assert(0);
    }
    LTRACEF("[SCP] nodeoffset = 0x%x\n", nodeoffset);

    data = (uint32_t *)fdt_getprop(fdt, nodeoffset, "memorydump", &len);
    if (data == NULL) {
        LTRACEF("[SCP] %s:Cannot get memorydump size\n", __func__);
    } else {
        for (i = MDUMP_L2TCM; i < MDUMP_TOTAL; ++i) {
            scp_dump.prefix[i] = fdt32_to_cpu(data[i - 1]);
            LTRACEF("[SCP] scp_dump.prefix[%u] = 0x%x\n", i, scp_dump.prefix[i]);
            scp_dump.prefix[i] += scp_dump.prefix[i - 1];
        }
    }
}

void scp_do_regdump(uint32_t *out, uint32_t *out_end)
{
    int i = 0;
    void *from;
    uint32_t *buf = out;
    int size_limit = sizeof(reg_save_list) / sizeof(struct reg_save_st);
    uint32_t scp_reg_base_phy = 0x10000000 + (SCP_CFG_BASE - IO_BASE);


    LTRACEF("[SCP] scp reg base phy addr = 0x%x\n", scp_reg_base_phy);
    /* last addr is infra */
    for (i = 0; i < size_limit - 1; i++)
        reg_save_list[i].addr += scp_reg_base_phy;

    for (i = 0; i < size_limit; i++) {
        if (((void *)buf + reg_save_list[i].size
            + sizeof(struct reg_save_st)) > (void *)out_end) {
            LTRACEF("[SCP] %s overflow:%p, %d\n", __func__,
                (void *)buf + reg_save_list[i].size +
                sizeof(struct reg_save_st), i);
            break;
        }

        LTRACEF("[SCP] reg_save_list[%d].addr = 0x%x\n", i, reg_save_list[i].addr);
        *buf = reg_save_list[i].addr;
        buf++;
        *buf = reg_save_list[i].size;
        buf++;

        /* pa to va */
        from = (void *)((reg_save_list[i].addr & 0x0fffffff) + IO_BASE);
        memcpy(buf, from, reg_save_list[i].size);
        buf += (reg_save_list[i].size / sizeof(uint32_t));
    }
}

void scp_do_l1cdump(uint32_t *out, uint32_t *out_end)
{
    uint32_t *buf = out;
    uint32_t tmp;
    uint32_t l1c_size = get_MDUMP_size(MDUMP_L1C);

    LTRACEF("[SCP] l1c_size = 0x%x\n", l1c_size);
    tmp = readl(R_SEC_CTRL);
    /* enable cache debug */
    writel(tmp | B_CORE0_CACHE_DBG_EN | B_CORE1_CACHE_DBG_EN, R_SEC_CTRL);
    if ((void *)buf + l1c_size > (void *)out_end) {
        LTRACEF("[SCP] %s overflow\n", __func__);
        return;
    }
    memcpy(buf, (void *)R_CORE0_CACHE_RAM, l1c_size);
    /* disable cache debug */
    writel(tmp, R_SEC_CTRL);
}

void scp_do_tbufdump_RV33(uint32_t *out, uint32_t *out_end)
{
    LTRACEF("[SCP] %s\n", __func__);
    uint32_t *buf = out;
    uint32_t tmp, index, offset, wbuf_ptr;
    int i;

    wbuf_ptr = readl(R_CORE0_TBUF_WPTR);
    tmp = readl(R_CORE0_DBG_CTRL) & (~M_CORE_TBUF_DBG_SEL);
    for (i = 0; i < 16; i++) {
        index = (wbuf_ptr + i) / 2;
        offset = ((wbuf_ptr + i) % 2) * 0x8;
        writel(tmp | (index << S_CORE_TBUF_DBG_SEL), R_CORE0_DBG_CTRL);
        *(buf) = readl(R_CORE0_TBUF_DATA31_0 + offset);
        *(buf + 1) = readl(R_CORE0_TBUF_DATA63_32 + offset);
        buf += 2;
    }

    wbuf_ptr = readl(R_CORE1_TBUF_WPTR);
    tmp = readl(R_CORE1_DBG_CTRL) & (~M_CORE_TBUF_DBG_SEL);
    for (i = 0; i < 16; i++) {
        index = (wbuf_ptr + i) / 2;
        offset = ((wbuf_ptr + i) % 2) * 0x8;
        writel(tmp | (index << S_CORE_TBUF_DBG_SEL), R_CORE1_DBG_CTRL);
        *(buf) = readl(R_CORE1_TBUF_DATA31_0 + offset);
        *(buf + 1) = readl(R_CORE1_TBUF_DATA63_32 + offset);
        buf += 2;
    }

    for (i = 0; i < 16; i++) {
        LTRACEF("[SCP] C0:%02d:0x%08x::0x%08x\n",
            i, *(out + i * 2), *(out + i * 2 + 1));
    }
    for (i = 0; i < 16; i++) {
        LTRACEF("[SCP] C1:%02d:0x%08x::0x%08x\n",
            i, *(out + i * 2 + 16), *(out + i * 2 + 17));
    }
}

void scp_do_tbufdump_RV55(uint32_t *out, uint32_t *out_end)
{
    LTRACEF("[SCP] %s\n", __func__);
    uint32_t *buf = out;
    uint32_t tmp, tmp1, index, offset, wbuf_ptr, wbuf1_ptr;
    int i;

    wbuf1_ptr = readl(R_CORE0_TBUF_WPTR);
    wbuf_ptr = wbuf1_ptr & 0x1f;
    wbuf1_ptr = wbuf1_ptr >> 8;
    tmp = readl(R_CORE0_DBG_CTRL) & M_CORE_TBUF_DBG_SEL_RV55;
    for (i = 0; i < 32; i++) {
        index = ((wbuf_ptr + i) / 4) & 0x7;
        offset = ((wbuf_ptr + i) % 4) * 0x8;
        tmp1 = (index << S_CORE_TBUF_S) | (index << S_CORE_TBUF1_S);
        writel(tmp | tmp1, R_CORE0_DBG_CTRL);
        *(buf) = readl(R_CORE0_TBUF_DATA31_0 + offset);
        *(buf + 1) = readl(R_CORE0_TBUF_DATA63_32 + offset);
        buf += 2;
    }
    for (i = 0; i < 32; i++) {
        LTRACEF("[SCP] C0:H0:%02d:0x%08x::0x%08x\n",
            i, *(out + i * 2), *(out + i * 2 + 1));
    }
    for (i = 0; i < 32; i++) {
        index = (((wbuf1_ptr + i) / 4) & 0x7) | 0x8;
        offset = ((wbuf1_ptr + i) % 4) * 0x8;
        tmp1 = (index << S_CORE_TBUF_S) | (index << S_CORE_TBUF1_S);
        writel(tmp | tmp1, R_CORE0_DBG_CTRL);
        *(buf) = readl(R_CORE0_TBUF_DATA31_0 + offset);
        *(buf + 1) = readl(R_CORE0_TBUF_DATA63_32 + offset);
        buf += 2;
    }
    for (i = 0; i < 32; i++) {
        LTRACEF("[SCP] C0:H1:%02d:0x%08x::0x%08x\n",
            i, *(out + 64 + i * 2), *(out + 64 + i * 2 + 1));
    }
#if (SCP_CORE_NUMS == 2)
    wbuf1_ptr = readl(R_CORE1_TBUF_WPTR);
    wbuf_ptr = wbuf1_ptr & 0x1f;
    wbuf1_ptr = wbuf1_ptr >> 8;
    tmp = readl(R_CORE1_DBG_CTRL) & M_CORE_TBUF_DBG_SEL_RV55;
    for (i = 0; i < 32; i++) {
        index = ((wbuf_ptr + i) / 4) & 0x7;
        offset = ((wbuf_ptr + i) % 4) * 0x8;
        tmp1 = (index << S_CORE_TBUF_S) | (index << S_CORE_TBUF1_S);
        writel(tmp | tmp1, R_CORE1_DBG_CTRL);
        *(buf) = readl(R_CORE1_TBUF_DATA31_0 + offset);
        *(buf + 1) = readl(R_CORE1_TBUF_DATA63_32 + offset);
        buf += 2;
    }
    for (i = 0; i < 32; i++) {
        LTRACEF("[SCP] C1:H0:%02d:0x%08x::0x%08x\n",
            i, *(out + i * 2), *(out + i * 2 + 1));
    }
    for (i = 0; i < 32; i++) {
        index = (((wbuf1_ptr + i) / 4) & 0x7) | 0x8;
        offset = ((wbuf1_ptr + i) % 4) * 0x8;
        tmp1 = (index << S_CORE_TBUF_S) | (index << S_CORE_TBUF1_S);
        writel(tmp | tmp1, R_CORE1_DBG_CTRL);
        *(buf) = readl(R_CORE1_TBUF_DATA31_0 + offset);
        *(buf + 1) = readl(R_CORE1_TBUF_DATA63_32 + offset);
        buf += 2;
    }
    for (i = 0; i < 32; i++) {
        LTRACEF("[SCP] C1:H1:%02d:0x%08x::0x%08x\n",
            i, *(out + 64 + i * 2), *(out + 64 + i * 2 + 1));
    }
#endif
}

#define BUF_SIZE 65536
int scp_zip_compress(unsigned char *input, unsigned char *output, unsigned int input_size)
{
    LTRACEF("[SCP] %s\n", __func__);
    int ret, flush;
    int have;
    z_stream strm;

    int i = 0;
    int j = 0;
    unsigned int avail_input_size = input_size;
    unsigned char *ibuf;
    unsigned char *obuf;

    ibuf = malloc(BUF_SIZE);
    if (ibuf == NULL) {
        LTRACEF("[SCP] ibuf malloc fail\n");
        return Z_BUF_ERROR;
    }

    obuf = malloc(BUF_SIZE);
    if (obuf == NULL) {
        free(ibuf);
        LTRACEF("[SCP] obuf malloc fail\n");
        return Z_BUF_ERROR;
    }

    strm.zalloc = Z_NULL;
    strm.zfree = Z_NULL;
    strm.opaque = Z_NULL;
    /* allocate deflate state */
    memset(&strm, 0, sizeof(z_stream));
    ret = deflateInit2(&strm, Z_BEST_COMPRESSION, Z_DEFLATED, 31, 8, Z_DEFAULT_STRATEGY);
    if (ret != Z_OK) {
        free(ibuf);
        free(obuf);
        LTRACEF("[SCP] ret = %d\n", ret);
        return ret;
    }

    /* compress until end of file */
    do {
        strm.avail_in = BUF_SIZE;
        strm.next_in = ibuf;
        strm.avail_out = BUF_SIZE;
        strm.next_out = obuf;
        flush = Z_NO_FLUSH;
        if (avail_input_size < BUF_SIZE) {
            strm.avail_in = avail_input_size;
            flush = Z_FINISH;
        }

        memcpy(ibuf, (void *)&input[i], strm.avail_in);
        i += strm.avail_in;
        avail_input_size -= strm.avail_in;
        strm.next_in = ibuf;

        /* run deflate() on input until output buffer not full, finish
         * compression if all of source has been read in
         */
        do {
            strm.avail_out = BUF_SIZE;
            strm.next_out = obuf;
            ret = deflate(&strm, flush);    /* no bad return value */
            assert(ret != Z_STREAM_ERROR);  /* state not clobbered */
            have = BUF_SIZE - strm.avail_out;
            memcpy((void *)&output[j], obuf, have);
            j += have;
            mtk_wdt_restart_timer();
        } while (strm.avail_out == 0);
        assert(strm.avail_in == 0);     /* all input will be used */
    /* done when last data in file processed */
    } while (flush != Z_FINISH);
    assert(ret == Z_STREAM_END);        /* stream will be complete */

    free(ibuf);
    free(obuf);
    /* clean up and return */
    (void)deflateEnd(&strm);
    return j;
}

/*
 * this function need SCP to keeping awaken
 * scp_crash_dump: dump scp tcm info.
 * @param MemoryDump:   scp dump struct
 * @param scp_core_id:  core id
 * @return:             scp dump size
 */

int scp_crash_dump(void *crash_buffer)
{
    unsigned int scp_dump_size;
    int datasize, ret;
    void *tmp_ptr;
    paddr_t tmp_ptr_phy;
    void *dram_start = NULL;
    uint32_t dram_size = 0, dram_size_max = get_MDUMP_size(MDUMP_DRAM);
    struct scp_region_info_st *scp_region_info = (struct scp_region_info_st *)(SCP_SRAM_BASE + 0x4);

#if (SCP_RV_VERSION == 33)
    void (*scp_do_tbufdump)(uint32_t*, uint32_t*) = scp_do_tbufdump_RV33;
#else
    void (*scp_do_tbufdump)(uint32_t*, uint32_t*) = scp_do_tbufdump_RV55;
#endif
    tmp_ptr_phy = scp_region_info->ap_loader_start;

    /* region_info will be null when recovery boot fail, skip scp coredump */
    if (tmp_ptr_phy == 0) {
        LTRACEF("[SCP] scp_region_info = 0, skip coredump\n");
        return 0;
    }
    /* mapping physical addr to ramdump addr(virtual) */
    ret = vmm_alloc_physical(vmm_get_kernel_aspace(),
                        "SCP-reserved",
                        SCP_DRAM_OFFSET + dram_size_max,
                        (void **)&scp_dump.ramdump,
                        PAGE_SIZE_SHIFT,
                        tmp_ptr_phy,
                        0,
                        ARCH_MMU_FLAG_CACHED);
    if (ret != NO_ERROR) {
        LTRACEF("[SCP] %s: tmp_dram error\n", __func__);
        return 0;
    }
    tmp_ptr = scp_dump.ramdump;
    LTRACEF("[SCP] scp_dump.ramdump = %p\n", scp_dump.ramdump);


    memset(tmp_ptr, 0, SCP_DRAM_OFFSET);
    memcpy((void *)get_MDUMP_addr(MDUMP_L2TCM),
        (void *)(SCP_SRAM_BASE), get_MDUMP_size(MDUMP_L2TCM));
    scp_do_l1cdump((void *)get_MDUMP_addr(MDUMP_L1C),
        (void *)get_MDUMP_addr(MDUMP_REGDUMP));
    /* dump sys registers */
    scp_do_regdump((void *)get_MDUMP_addr(MDUMP_REGDUMP),
        (void *)get_MDUMP_addr(MDUMP_TBUF));
    scp_do_tbufdump((void *)get_MDUMP_addr(MDUMP_TBUF),
        (void *)get_MDUMP_addr(MDUMP_DRAM));
    scp_dump_size = get_MDUMP_size_accumulate(MDUMP_TBUF);
    LTRACEF("[SCP] scp_dump_size so far = 0x%x\n", scp_dump_size);
    /* L1C support? */
    if ((int)(scp_region_info->ap_dram_size) <= 0) {
        LTRACEF("[SCP] ap_dram_size <=0\n");
    } else {
        dram_start = tmp_ptr + SCP_DRAM_OFFSET;
        dram_size = scp_region_info->ap_dram_size;

        if (dram_size > dram_size_max) {
            LTRACEF("ap_dram_size %x > %x, reset size\n", dram_size, dram_size_max);
            dram_size = dram_size_max;
        }
        /* beware, dest overwrite source memcpy, scp_dump_size must < SCP_DRAM_OFFSET */
        memcpy((void *)get_MDUMP_addr(MDUMP_DRAM),
            dram_start, dram_size);
        scp_dump_size += ROUNDUP(dram_size, 4);
    }
    LTRACEF("[SCP] scp_dump_size total = 0x%x\n", scp_dump_size);
    datasize = scp_zip_compress((void *)tmp_ptr, (void *)crash_buffer, scp_dump_size);
    vmm_free_region(vmm_get_kernel_aspace(), (vaddr_t)scp_dump.ramdump);
    return datasize;
}
