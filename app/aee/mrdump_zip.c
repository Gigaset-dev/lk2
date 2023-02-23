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

#include <assert.h>
#include <debug.h>
#include <lib/cksum.h>
#include <lib/zlib.h>
#include <lib/zutil.h>
#include <malloc.h>
#include <platform/aee_common.h>
#if !IS_64BIT
#include <platform/mmu_lpae.h>
#endif
#include <platform/mrdump_expdb.h>
#include <platform/wdt.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <trace.h>

#include "aee.h"
#include "mrdump_private.h"

#define LOCAL_TRACE 0

#define KZIP_DEBUG(x...)

#define KDUMP_SIZE_REPORT 0x1000000
#define KDUMP_TICK_WDT 0x10000

#ifdef MRDUMP_ZIP_NO_COMPRESSION
#define KDUMP_ZLIB_LEVEL Z_NO_COMPRESSION
#else
#define KDUMP_ZLIB_LEVEL Z_BEST_SPEED
#endif

#define ZIPVERSION_NEEDED 45UL

static char zero_page[PAGE_SIZE] __ALIGNED(PAGE_SIZE) = {0};

static void putvalue(void *dest, uint64_t x, int nbByte)
{
    uint8_t *buf = (uint8_t *)dest;
    int n;

    for (n = 0; n < nbByte; n++) {
        buf[n] = (unsigned char)(x & 0xff);
        x >>= 8;
    }
#if 0
    if (x != 0) {
        /* data overflow - hack for ZIP64 */
        for (n = 0; n < nbByte; n++)
            buf[n] = 0xff;
    }
#endif
}

static int put_zip64_eoc_directory_record(uint8_t *buf, int record_num,
    uint64_t zip_centralheader_offset, int zip_centralheader_size)
{
    uint8_t *oldbuf = buf;

    putvalue(buf, ZIP64ENDOFCENTRALDIRMAGIC, 4);
    buf += 4;
    /* zip64 eoc size */
    putvalue(buf, 44, 8);
    buf += 8;
    /* version made by */
    putvalue(buf, ZIPVERSION_NEEDED, 2);
    buf += 2;
    /* version needed to extract */
    putvalue(buf, ZIPVERSION_NEEDED, 2);
    buf += 2;
    /* number of this disk */
    putvalue(buf, 0, 4);
    buf += 4;
    /* number of the disk with the start of the central directory */
    putvalue(buf, 0, 4);
    buf += 4;
    /* total number of entries in the central directory on this disk */
    putvalue(buf, record_num, 8);
    buf += 8;
    /* total number of entries in the central directory */
    putvalue(buf, record_num, 8);
    buf += 8;
    /* size of the central directory */
    putvalue(buf, zip_centralheader_size, 8);
    buf += 8;
    /* offset of start of central directory with respect to the starting disk number */
    putvalue(buf, zip_centralheader_offset, 8);
    buf += 8;
    return buf - oldbuf;
}

static int put_zip64_eoc_directory_locator(uint8_t *buf, uint64_t zip64_eoc_offset)
{
    uint8_t *oldbuf = buf;

    putvalue(buf, ZIP64ENDOFCENTRALDIRLOCATORMAGIC, 4);
    buf += 4;
    /* number of the disk with the start of the zip64 end of central directory */
    putvalue(buf, 0, 4);
    buf += 4;
    /* relative offset of the zip64 end of central directory record */
    putvalue(buf, zip64_eoc_offset, 8);
    buf += 8;
    /* total number of disks */
    putvalue(buf, 1, 4);
    buf += 4;
    return buf - oldbuf;
}

static int put_eoc_directory_record(uint8_t *buf, int record_num,
                                    uint64_t zip_centralheader_offset,
                                    int zip_centralheader_size)
{
    uint8_t *oldbuf = buf;

    putvalue(buf, ENDOFCENTRALDIRMAGIC, 4);
    buf += 4;
    /* Number of this disk */
    putvalue(buf, 0, 2);
    buf += 2;
    /* Disk where central directory starts */
    putvalue(buf, 0, 2);
    buf += 2;
    /* Number of central directory records on this disk */
    putvalue(buf, record_num, 2);
    buf += 2;
    /* Total number of central directory records */
    putvalue(buf, record_num, 2);
    buf += 2;
    putvalue(buf, zip_centralheader_size, 4);
    buf += 4;
    putvalue(buf, 0xffffffff, 4);
    buf += 4;
    /* Comment length (n) */
    putvalue(buf, 0, 2);
    buf += 2;
    return buf - oldbuf;
}

static int put_localheader(uint8_t *buf, const char *filename, int level)
{
    uint8_t *oldbuf = buf;

    putvalue(buf, LOCALHEADERMAGIC, 4);
    buf += 4;
    putvalue(buf, ZIPVERSION_NEEDED, 2);
    buf += 2;

    uint16_t flag = 0;

    if ((level == 8) || (level == 9))
        flag |= 2;
    if (level == 2)
        flag |= 4;
    if (level == 1)
        flag |= 6;
#if 0
    if (password != NULL)
        flag |= 1;
#endif

    putvalue(buf, flag | 0x8, 2);
    buf += 2;

    putvalue(buf, Z_DEFLATED, 2);
    buf += 2;

    putvalue(buf, 0UL, 4);
    buf += 4;

    // CRC / Compressed size / Uncompressed size will be filled in later and rewritten later
    putvalue(buf, 0UL, 4); /* crc 32, unknown */
    buf += 4;
    putvalue(buf, 0xFFFFFFFFUL, 4); /* compressed size, unknown */
    buf += 4;
    putvalue(buf, 0xFFFFFFFFUL, 4); /* uncompressed size, unknown */
    buf += 4;
    putvalue(buf, strlen(filename), 2); /* size of filename */
    buf += 2;
    putvalue(buf, 4 + 8 + 8, 2); /* size of extra field */
    buf += 2;

    memcpy(buf, filename, strlen(filename));
    buf += strlen(filename);

    /* ZIP64 extension */
    putvalue(buf, 0x0001, 2); /* ZIP64_EXTENSION */
    buf += 2;
    putvalue(buf, 8 + 8, 2);
    buf += 2;
    putvalue(buf, 0UL, 8);
    buf += 8;
    putvalue(buf, 0UL, 8);
    buf += 8;

    return buf - oldbuf;
}

static int put_centralheader(uint8_t *buf, const char *filename, int level,
    uint64_t zip_localheader_offset, uint64_t size, uint64_t uncomp_size, uint32_t crc32_value)
{
    uint8_t *oldbuf = buf;

    putvalue(buf, CENTRALHEADERMAGIC, 4);
    buf += 4;
    putvalue(buf, ZIPVERSION_NEEDED, 2);
    buf += 2;
    putvalue(buf, ZIPVERSION_NEEDED, 2);
    buf += 2;

    uint16_t flag = 0;

    if ((level == 8) || (level == 9))
        flag |= 2;
    if (level == 2)
        flag |= 4;
    if (level == 1)
        flag |= 6;
#if 0
    if (password != NULL)
        flag |= 1;
#endif

    putvalue(buf, flag | 0x8, 2);
    buf += 2;

    putvalue(buf, Z_DEFLATED, 2);
    buf += 2;

    putvalue(buf, 0UL, 4);
    buf += 4;

    // CRC / Compressed size / Uncompressed size will be filled in later and rewritten later
    putvalue(buf, crc32_value, 4); /* crc 32 */
    buf += 4;
    putvalue(buf, 0xffffffffUL, 4); /* compressed size */
    buf += 4;
    putvalue(buf, 0xffffffffUL, 4); /* uncompressed size */
    buf += 4;
    putvalue(buf, strlen(filename), 2);
    buf += 2;
    putvalue(buf, 4 + 8 + 8 + 8, 2); /* size of extra field */
    buf += 2;
    putvalue(buf, 0UL, 2); /* size of comment field */
    buf += 2;
    putvalue(buf, 0UL, 2); /* disk number */
    buf += 2;
    putvalue(buf, 0UL, 2); /* internal attributes */
    buf += 2;
    putvalue(buf, 0UL, 4); /* external file attributes */
    buf += 4;
    putvalue(buf, 0xffffffffUL, 4); /* Relative offset */
    buf += 4;
    memcpy(buf, filename, strlen(filename));
    buf += strlen(filename);

    /* ZIP64 extension */
    putvalue(buf, 0x0001, 2); /* ZIP64_EXTENSION */
    buf += 2;
    putvalue(buf, 8 + 8 + 8, 2);
    buf += 2;
    putvalue(buf, uncomp_size, 8);
    buf += 8;
    putvalue(buf, size, 8);
    buf += 8;
    putvalue(buf, zip_localheader_offset, 8);
    buf += 8;

    return buf - oldbuf;
}

static int kzip_write_current(struct kzip_file *zfile, void *buf, int len)
{
    KZIP_DEBUG("%s: write_cb %p len %d\n", __func__, zfile->write_cb, len);
    int retval = zfile->write_cb(zfile->handle, buf, len);

    if (retval > 0)
        zfile->current_size += retval;

    if ((zfile->current_size - zfile->reported_size) >= KDUMP_SIZE_REPORT) {
        vo_show_progress(zfile->current_size / 0x100000);
        zfile->reported_size = zfile->current_size;
    }
    if ((zfile->current_size - zfile->wdk_kick_size) >= KDUMP_TICK_WDT) {
        mtk_wdt_restart_timer();
        zfile->wdk_kick_size = zfile->current_size;
    }
    return retval;
}

struct kzip_file *kzip_open(void *handle, int (*write_cb)(void *handle, void *p, int size))
{
    struct kzip_file *zf = malloc(sizeof(struct kzip_file));

    if (zf == NULL) {
        KZIP_DEBUG("%s: No enough memory\n", __func__);
        return NULL;
    }
    memset(zf, 0, sizeof(struct kzip_file));
    zf->handle = handle;
    zf->write_cb = write_cb;
    KZIP_DEBUG("%s: handle zf %p %p write_cb %p\n", __func__, zf, zf->handle, zf->write_cb);

    return zf;
}

bool kzip_close(struct kzip_file *zf)
{
    uint64_t central_header_offset = zf->current_size, zip64_eoc_offset;
    struct kzip_entry *zentries = zf->zentries;
    int num = zf->entries_num;
    int i, hsize = 0, local_hsize;
    uint8_t databuf[128];

    for (i = 0; i < num; i++) {
        local_hsize = put_centralheader(databuf, zentries[i].filename, zentries[i].level,
                                            zentries[i].localheader_offset,
                                            zentries[i].comp_size,
                                            zentries[i].uncomp_size,
                                            zentries[i].crc32);
        if (kzip_write_current(zf, databuf, local_hsize) != local_hsize)
            goto error;

        hsize += local_hsize;
    }
    voprintf_info("%s: current_size %lld hoffset %lld\n",
        __func__, zf->current_size, central_header_offset);

    zip64_eoc_offset = zf->current_size;
    local_hsize = put_zip64_eoc_directory_record(databuf, num, central_header_offset, hsize);
    if ((local_hsize > 0) && (kzip_write_current(zf, databuf, local_hsize) != local_hsize))
        return false;

    local_hsize = put_zip64_eoc_directory_locator(databuf, zip64_eoc_offset);
    if ((local_hsize > 0) && (kzip_write_current(zf, databuf, local_hsize) != local_hsize))
        return false;

    local_hsize = put_eoc_directory_record(databuf, num, central_header_offset, hsize);
    if (kzip_write_current(zf, databuf, local_hsize) != local_hsize)
        return false;

    free(zf);
    return true;
error:
    free(zf);
    return false;
}

#define CHUNK 32768

/* Compress from file source to file dest until EOF on source.
 * def() returns Z_OK on success, Z_MEM_ERROR if memory could not be
 * allocated for processing, Z_STREAM_ERROR if an invalid compression
 * level is supplied, Z_VERSION_ERROR if the version of zlib.h and the
 * version of the library linked do not match, or Z_ERRNO if there is
 * an error reading or writing the files.
 */

static bool kzip_add_file_no_mapped(struct kzip_file *zfile, const struct kzip_addlist *memlist,
                                    int *flush, int *ret, uint8_t *in, uint8_t *out,
                                    struct kzip_entry *zentry, z_stream *strm,
                                    struct aee_timer *zip_time)
{
    uint64_t mem_size = memlist->size;
    uint8_t *memsrc = (uint8_t *)(uintptr_t)memlist->address;
    *flush = (memlist->size == 0) ? Z_FINISH : Z_NO_FLUSH;
    LTRACEF_LEVEL(1, "-- Compress memory %llx, size %llu\n",
                   memlist->address, memlist->size);

    do {
        int in_size = mem_size > CHUNK ? CHUNK : mem_size;

        memcpy(in, memsrc, in_size);
        zentry->crc32 = (crc32(zentry->crc32 ^ 0xffffffffL, in, in_size) ^ 0xffffffffL);
        memsrc += in_size;
        mem_size -= in_size;
        strm->avail_in = in_size;
        strm->next_in = in;
        /* run deflate() on input until output buffer not full, finish
         * compression if all of source has been read in
         */
        do {
            strm->avail_out = CHUNK;
            strm->next_out = out;
            *ret = deflate(strm, *flush);    /* no bad return value */
            assert(*ret != Z_STREAM_ERROR);  /* state not clobbered */
            int have = CHUNK - strm->avail_out;

            if (have > 0) {
                if (kzip_write_current(zfile, out, have) != have) {
                    voprintf_error("-- Compress memory %llx, error. surplus size: %llu\n",
                                   memlist->address, mem_size);
                    return false;
                }
            }
        } while (strm->avail_out == 0);
        assert(strm->avail_in == 0);     /* all input will be used */
    } while (mem_size > 0);
    voprintf_info("-- Compress memory %llx, done. surplus size: %llu\n",
                   memlist->address, mem_size);
    return true;
}

extern uint64_t v2p_64(uint64_t vptr);
long mrdump_mem_map(const struct kzip_addlist *memlist,
    uint64_t paddr, vaddr_t vaddr, unsigned long sz, uint64_t zero_pa)
{
    uint64_t memmap = memlist->memmap;
    uint64_t pageflags = memlist->pageflags;
    uint32_t struct_page_size = memlist->struct_page_size;

    long i, memmap_sz = sz / PAGE_SIZE * struct_page_size, invalid_cnt = 0, offset;
    vaddr_t page_start;
    uint64_t pa;
    status_t err;
    void *dump_va = NULL;

    /* map memmap */
    memmap += paddr / PAGE_SIZE * struct_page_size;
    offset = (long)memmap&(PAGE_SIZE - 1);
    pa = v2p_64(memmap - offset);
    err = vmm_alloc_physical(vmm_get_kernel_aspace(),
                "memmap", memmap_sz,
                &dump_va, 0,
                pa, 0, ARCH_MMU_FLAG_CACHED);
    if (err) {
        voprintf_error("vmm_alloc_physical for memmap fail %d\n", err);
        goto fail;
    }

    /* map DDR according memmap flag */
    for (i = 0; i < memmap_sz; i += struct_page_size, vaddr += PAGE_SIZE, paddr += PAGE_SIZE) {
        int mapped, unmapped;
        vmm_aspace_t *aspace = vaddr_to_aspace(vaddr);

        if (aspace) {
            unmapped = arch_mmu_unmap(&aspace->arch_aspace, (vaddr_t)vaddr, 1);
            if (*(uint64_t *)(dump_va + i)&pageflags) {
                mapped = arch_mmu_map(&aspace->arch_aspace, (vaddr_t)vaddr, zero_pa,
                             1, ARCH_MMU_FLAG_CACHED);
                if (!mapped) {
                        invalid_cnt++;
                        continue;
                    }
            } else {
                mapped = arch_mmu_map(&aspace->arch_aspace, (vaddr_t)vaddr, paddr,
                             1, ARCH_MMU_FLAG_CACHED);
                if (mapped) {
                    voprintf_error("mmu map fail\n");
                    goto fail;
                }
            }
        } else {
            voprintf_error("get NULL aspace\n");
            goto fail;
        }
    }
     vmm_free_region(vmm_get_kernel_aspace(), (vaddr_t)dump_va);
    return invalid_cnt;
fail:
    voprintf_error("map error: paddr=0x%016llx, vaddr=0x%08lx, zero_pa=0x%016llx\n",
                                                                    paddr, vaddr, zero_pa);
    return -1;
}

static bool is_linux_kernel_memory_dump(const struct kzip_addlist *addlist)
{
    uint64_t memmap = addlist->memmap;
    uint64_t pageflags = addlist->pageflags;
    uint32_t struct_page_size = addlist->struct_page_size;
    char dump_type[32] = {0};

    if (mrdump_get_env(MRDUMP_DUMP_TYPE, dump_type, sizeof(dump_type)))
        return false;

    if (strcmp(dump_type, MRDUMP_LINUX_KERNEL_MEMORY) == 0) {
        if ((memmap > 0) && (pageflags > 0) && (struct_page_size > 0)) {
            voprintf_info("dump_linux_kernel_memory = 1\n");
            voprintf_info("memmap(0x%llx), pageflags(0x%llx), struct_page_size(0x%x)\n",
                                                            memmap, pageflags, struct_page_size);
            return true;
        }
    }
    voprintf_info("dump_linux_kernel_memory = 0\n");
    voprintf_info("memmap(0x%llx), pageflags(0x%llx), struct_page_size(0x%x)\n",
                                                    memmap, pageflags, struct_page_size);
    return false;
}

#define MAX_MAP_CHUNK_CNT_KB (CHUNK / CHUNK)
#define MAX_MAP_PAGE_CNT_KB (CHUNK / PAGE_SIZE)
#define MAX_MAP_CHUNK_CNT_MB (AEE_MAP_SIZE / CHUNK)
#define MAX_MAP_PAGE_CNT_MB (AEE_MAP_SIZE / PAGE_SIZE)
static bool kzip_add_file_do_mapped(struct kzip_file *zfile, const struct kzip_addlist *memlist,
                                    int *flush, int *ret, uint8_t *in, uint8_t *out,
                                    struct kzip_entry *zentry, z_stream *strm,
                                    struct aee_timer *zip_time)
{
    /* multiple physical address mapped onto
     * static 2MB address space for aarch64
     * static 16MB address space for aarch32 LPAE
     */
    int cnt = 0, max_map_cnt, max_map_page_cnt;
    uint64_t paddr = memlist->address;
    long invalid_cnt = -1, tot_invalid_cnt = 0;
    bool dump_linux_kernel_memory;
    void *dump_va = NULL;
    uint64_t mem_size = memlist->size;
    uint8_t *memsrc;
    size_t map_size;
    uint8_t align_log2;
    status_t err;

    *flush = (memlist->size == 0) ? Z_FINISH : Z_NO_FLUSH;
    if (mem_size > SZ_2M) {
        map_size = AEE_MAP_SIZE;
        align_log2 = AEE_MAP_ALIGN;
        max_map_cnt = MAX_MAP_CHUNK_CNT_MB;
        max_map_page_cnt = MAX_MAP_PAGE_CNT_MB;
    } else {
        map_size = CHUNK;
        align_log2 = 0;
        max_map_cnt = MAX_MAP_CHUNK_CNT_KB;
        max_map_page_cnt = MAX_MAP_PAGE_CNT_KB;
    }

    voprintf_info("-- Compress memory %llx, size %llu\n", memlist->address, memlist->size);

    dump_linux_kernel_memory = is_linux_kernel_memory_dump(memlist);

    do {
        /* arch_mmu_map by each AEE_MAP_SIZE */
        if (cnt == 0) {
#if !IS_64BIT
            int mapped;

            if (paddr >= SZ_4G) {
                err = vmm_alloc_physical(vmm_get_kernel_aspace(),
                    "coredump_addr", map_size,
                    &dump_va, align_log2,
                    0, VMM_FLAG_NO_MAP_PA, ARCH_MMU_FLAG_CACHED);
                if (err) {
                    voprintf_error("vmm_alloc_physical fail\n");
                    return false;
                }
                vmm_aspace_t *aspace = vaddr_to_aspace(dump_va);

                mapped = arch_mmu_map_large(&aspace->arch_aspace, (vaddr_t)dump_va, paddr,
                                 max_map_page_cnt, ARCH_MMU_FLAG_CACHED);
                if (mapped == 0) {
                    voprintf_error("arch_mmu_map_large fail\n");
                    vmm_free_region(vmm_get_kernel_aspace(), (vaddr_t)dump_va);
                    return false;
                }
            } else
#endif
            {
                err = vmm_alloc_physical(vmm_get_kernel_aspace(),
                        "coredump_addr", map_size,
                        &dump_va, align_log2,
                        (paddr_t)paddr, 0, ARCH_MMU_FLAG_CACHED);
                if (err) {
                    voprintf_error("vmm_alloc_physical fail\n");
                    return false;
                }
            }
            if (dump_linux_kernel_memory)
                invalid_cnt = mrdump_mem_map(memlist, paddr, (vaddr_t)dump_va, map_size,
                                                vaddr_to_paddr((uint64_t)(uintptr_t)zero_page));

            if (invalid_cnt >= 0)
                tot_invalid_cnt += invalid_cnt;

            memsrc = (uint8_t *)dump_va;
        }
        int in_size = mem_size > CHUNK ? CHUNK : mem_size;

        memcpy(in, memsrc, in_size);
        zentry->crc32 = (crc32(zentry->crc32 ^ 0xffffffffL, in, in_size) ^ 0xffffffffL);
        memsrc += in_size;
        mem_size -= in_size;
        strm->avail_in = in_size;
        strm->next_in = in;

        /* run deflate() on input until output buffer not full, finish
         * compression if all of source has been read in
         */
        do {
            strm->avail_out = CHUNK;
            strm->next_out = out;
            *ret = deflate(strm, *flush);    /* no bad return value */
            assert(*ret != Z_STREAM_ERROR);  /* state not clobbered */
            int have = CHUNK - strm->avail_out;

            if (have > 0) {
                if (kzip_write_current(zfile, out, have) != have) {
                    LTRACEF_LEVEL(ALWAYS, "-- Compress memory %llx, error. surplus size: %llu\n",
                                   memlist->address, mem_size);
                    return false;
                }
            }
        } while (strm->avail_out == 0);
        assert(strm->avail_in == 0);     /* all input will be used */

        /* mod by AEE_MAP_SIZE */
        cnt++;
        cnt %= max_map_cnt;
        if (cnt == 0) {
            paddr += map_size;
            vmm_free_region(vmm_get_kernel_aspace(), (vaddr_t)dump_va);
        }

    } while (mem_size > 0);

    voprintf_info("-- Compress memory %llx, done. surplus size: %llu, ignore: %luMB\n",
        memlist->address, mem_size, (tot_invalid_cnt * 4 / 1024));
    return true;
}

static bool kzip_add_file_from_expdb(struct kzip_file *zfile, const struct kzip_addlist *addlist,
                                    int *flush, int *ret, uint8_t *in, uint8_t *out,
                                    struct kzip_entry *zentry, z_stream *strm,
                                    struct aee_timer *zip_time)
{
    uint64_t offset_src = addlist->address;
    int64_t mem_size = addlist->size;

    *flush = (addlist->size == 0) ? Z_FINISH : Z_NO_FLUSH;
    voprintf_info("-- Compress expdb offset %llx, size %llu\n", offset_src, mem_size);

    do {
        /* read from expdb to memsrc */
        if (mem_size > CHUNK)
            mrdump_read_expdb(in, CHUNK, offset_src);
        else
            mrdump_read_expdb(in, (int)mem_size, offset_src);

        int in_size = mem_size > CHUNK ? CHUNK : mem_size;

        zentry->crc32 = (crc32(zentry->crc32 ^ 0xffffffffL, in, in_size) ^ 0xffffffffL);
        mem_size -= in_size;
        strm->avail_in = in_size;
        strm->next_in = in;

        /* run deflate() on input until output buffer not full, finish
         * compression if all of source has been read in
         */
        do {
            strm->avail_out = CHUNK;
            strm->next_out = out;
            *ret = deflate(strm, *flush);    /* no bad return value */
            assert(*ret != Z_STREAM_ERROR);  /* state not clobbered */
            int have = CHUNK - strm->avail_out;

            if (have > 0) {
                if (kzip_write_current(zfile, out, have) != have) {
                    voprintf_info("-- Compress expdb offset %llx, error.",
                                   offset_src);
                    voprintf_info(" surplus size: %llu\n",
                                   mem_size);
                    return false;
                }
            }
        } while (strm->avail_out == 0);
        assert(strm->avail_in == 0);     /* all input will be used */

        offset_src += in_size;

    } while (mem_size > 0);

    voprintf_info("-- Compress expdb offset %llx, done. surplus size: %llu\n",
        offset_src, mem_size);
    return true;
}

bool kzip_add_file(struct kzip_file *zfile, const struct kzip_addlist *addlist,
                   const char *zfilename)
{
    int ret, flush;
    z_stream strm;
    struct aee_timer zip_time;

    if (zfile->entries_num >= KZIP_ENTRY_MAX) {
        voprintf_error("Too many zip entry %d\n", zfile->entries_num);
        return false;
    }

    voprintf_info("%s: zf %p(%p) %s\n", __func__, zfile, zfile->write_cb, zfilename);
    struct kzip_entry *zentry = &zfile->zentries[zfile->entries_num++];

    zentry->filename = strdup(zfilename);
    zentry->localheader_offset = zfile->current_size;
    zentry->level = KDUMP_ZLIB_LEVEL;
    zentry->crc32 = 0xffffffffUL;

    KZIP_DEBUG("%s: write local header\n", __func__);
    uint8_t zip_localheader[128];
    int hsize = put_localheader(zip_localheader, zfilename, zentry->level);

    if (kzip_write_current(zfile, zip_localheader, hsize) != hsize)
        return false;

    KZIP_DEBUG("%s: init compressor\n", __func__);
    /* allocate deflate state */
    memset(&strm, 0, sizeof(z_stream));
    ret = deflateInit2(&strm, zentry->level, Z_DEFLATED, -MAX_WBITS, DEF_MEM_LEVEL,
               Z_DEFAULT_STRATEGY);
    if (ret != Z_OK) {
        voprintf_error("zlib compress init failed\n");
        return false;
    }

    uint64_t start = zfile->current_size, uncomp_size = 0;

    uint8_t *out = malloc(CHUNK);
    uint8_t *in = malloc(CHUNK);

    if ((out == NULL) || (in == NULL)) {
        voprintf_error("%s: malloc failed.\n", __func__);
        goto error;
    }

    do {
        switch (addlist->type) {
        case MEM_NO_MAP:
            if (kzip_add_file_no_mapped(zfile, addlist, &flush, &ret, in, out, zentry, &strm,
                                        &zip_time) == false)
                goto error;
            break;
        case MEM_DO_MAP:
            if (kzip_add_file_do_mapped(zfile, addlist, &flush, &ret, in, out, zentry, &strm,
                                        &zip_time) == false)
                goto error;
            break;
        case EXPDB_FILE:
            if (kzip_add_file_from_expdb(zfile, addlist, &flush, &ret, in, out, zentry, &strm,
                                        &zip_time) == false)
                goto error;
            break;
        default:
            goto error;
            break;
        }
        uncomp_size += addlist->size;
        addlist++;
        /* done when last data in file processed */
    } while (flush != Z_FINISH);
    assert(ret == Z_STREAM_END);        /* stream will be complete */

    zentry->comp_size = zfile->current_size - start;
    zentry->uncomp_size = uncomp_size;
    zentry->crc32 = zentry->crc32 ^ 0xffffffffUL;

    deflateEnd(&strm);
    free(out);
    free(in);
    return true;

error:
    deflateEnd(&strm);
    free(out);
    free(in);
    return false;
}
