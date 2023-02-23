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
#include <libfdt.h>
#include <kernel/mutex.h>
#include <mblock.h>
#include <lib/pl_boottags.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <trace.h>

#define LOCAL_TRACE 0

#define MBLOCK_NUM_MAX 128
#define BOOT_TAG_MEM             0x88610005
#define MBLOCK_MAGIC 0x99999999
#define MBLOCK_VERSION 3
#define MBLOCK_BESTFIT 0

/*please acquire mblock_lock before traverse the list*/
#define for_each_mblock_reserved(pp_reserve) \
    for (pp_reserve = &internal_mblock_info->reserved[0];\
        (*minfo_reserved_num > 0) && \
        pp_reserve <= &internal_mblock_info->reserved[*minfo_reserved_num - 1]; pp_reserve++)

/*please acquire mblock_lock before traverse the list*/
#define for_each_mblock(pp_mblock) \
    for (pp_mblock = &internal_mblock_info->mblock[0];\
        pp_mblock <= &internal_mblock_info->mblock[*minfo_mblock_num - 1]; pp_mblock++)

struct mem_desc_t {
    u64 start;
    u64 size;
};

/* rank is deprecated but existint for compatibility*/
struct mblock_t {
    u64 start;
    u64 size;
    u32 rank;
};

struct dram_info_t {
    u32 rank_num;
    struct mem_desc_t rank_info[4];
};

struct mblock_info_t {
    u32 mblock_num;
    struct mblock_t mblock[MBLOCK_NUM_MAX];
    u32 mblock_magic;
    u32 mblock_version;
    u32 reserved_num;
    struct reserved_t reserved[MBLOCK_RESERVED_NUM_MAX];
};

struct dt_dram_info {      /* RAM configuration */
    unsigned int start_hi;
    unsigned int start_lo;
    unsigned int size_hi;
    unsigned int size_lo;
};
struct dt_size_info {
    unsigned int size_hi;
    unsigned int size_lo;
};

/* for phandle */
struct device_dts_info {
    char device_name[MBLOCK_RESERVED_NAME_SIZE];
    char mblock_name[MBLOCK_RESERVED_NAME_SIZE];
};
/*in V3 version we only use mblock_info */
/*Other members are for backward compatibility*/

struct boot_tag_mem {
    u32 dram_rank_num;
    u64 dram_rank_size[4];
    struct mblock_info_t mblock_info;
    struct dram_info_t orig_dram_info;
    struct mem_desc_t lca_reserved_mem;
    struct mem_desc_t tee_reserved_mem;
};

static struct mblock_info_t *internal_mblock_info;
static mutex_t mblock_lock = MUTEX_INITIAL_VALUE(mblock_lock);
static int mblock_init_done;

static u32 *minfo_mblock_num;
static u32 *minfo_reserved_num;
static struct dram_info_t *internal_orig_dram_info;
struct boot_tag_mem mblock_boot_tag;
static unsigned int device_dts_num;
static struct device_dts_info device_dts[MBLOCK_RESERVED_NUM_MAX];

static void mblock_show_internal(int need_lock)
{
    unsigned int i;
    u64 start, sz, total;
    struct mblock_t *p_mblock;
    struct reserved_t *p_reserved;

    LTRACEF_LEVEL(CRITICAL, "mblock_magic:0x%x mblock_version:0x%x\n",
            internal_mblock_info->mblock_magic, internal_mblock_info->mblock_version);
    if (need_lock)
        mutex_acquire(&mblock_lock);
    i = 0;
    total = 0;
    for_each_mblock(p_mblock) {
        start = p_mblock->start;
        sz = p_mblock->size;
        LTRACEF_LEVEL(CRITICAL,
                      "mblock[%d]->start: 0x%llx, size: 0x%llx\n", i,
                      start, sz);
        total += sz;
        i++;
    }

    i = 0;
    for_each_mblock_reserved(p_reserved) {
        start = p_reserved->start;
        sz = p_reserved->size;
        LTRACEF_LEVEL(CRITICAL,
                      "mblock-R[%d].start: 0x%llx, size: 0x%llx map:%d name:%s\n", i,
                      start, sz, p_reserved->mapping,
                      p_reserved->name);
        i++;
        total += sz;
    }
    LTRACEF_LEVEL(CRITICAL, "Total DRAM = 0x%llx\n", total);
    if (need_lock)
        mutex_release(&mblock_lock);
}

void mblock_show(void)
{
    mblock_show_internal(1);
}
static int find_first_set_bit(u64 addr)
{
    int idx = 0;

    for (idx = 63; idx >= 0; idx--) {
        if (addr >> idx)
            return idx;
    }
    return 0;
}

static bool mblock_cross_bound(u64 start, u64 size)
{
    // below 4G is safe
    if ((!size) || (start+size <= 0x100000000ULL))
        return false;

    // above 4G need to check
    if (find_first_set_bit(start) < find_first_set_bit(start+size-1))
        return true;

    return false;
}

static void mblock_merge(void)
{

    unsigned int i;
    u64 start, sz;
    u64 next_start, next_size;

    for (i = 0; (i < *minfo_mblock_num-1) && ((*minfo_mblock_num) >= 2); ) {
        start = internal_mblock_info->mblock[i].start;
        sz = internal_mblock_info->mblock[i].size;
        next_start = internal_mblock_info->mblock[i+1].start;
        next_size = internal_mblock_info->mblock[i+1].size;
        if ((next_start == start + sz) && (i+2 < *minfo_mblock_num)) {
            internal_mblock_info->mblock[i].size = sz + next_size;
            internal_mblock_info->mblock[i+1].size = 0;
            LTRACEF("merge mblock[%d]->start: 0x%llx, end: 0x%llx\n", i,
                      start, start+sz);
            LTRACEF("with mblock[%d]->start: 0x%llx, end: 0x%llx\n", i+1,
                      next_start, next_start+next_size);
            // merge the mblock and move mblock forward
            memmove(&internal_mblock_info->mblock[i+1], &internal_mblock_info->mblock[i+2],
                    sizeof(struct mblock_t)*(*minfo_mblock_num-i-2));
            *minfo_mblock_num -= 1;
            i = 0;
            continue;
        } else if ((next_start == start + sz) && (i+2 == *minfo_mblock_num)) {
            //the last mblock
            internal_mblock_info->mblock[i].size = sz + next_size;
            internal_mblock_info->mblock[i+1].size = 0;
            LTRACEF("merge mblock[%d]->start: 0x%llx, end: 0x%llx\n", i,
                      start, start+sz);
            LTRACEF("with mblock[%d]->start: 0x%llx, end: 0x%llx\n", i+1,
                      next_start, next_start+next_size);
            *minfo_mblock_num -= 1;
            i = 0;
            continue;
        } else if ((next_start != start+sz) && (next_size == 0) && (i+2 < *minfo_mblock_num)) {
            LTRACEF("Delete mblock[%d]->start: 0x%llx, size: 0x%llx\n", i+1,
                      next_start, next_size);
            // merge the mblock and move mblock forward
            memmove(&internal_mblock_info->mblock[i+1], &internal_mblock_info->mblock[i+2],
                    sizeof(struct mblock_t)*(*minfo_mblock_num-i-2));
            *minfo_mblock_num -= 1;
            i = 0;
            continue;
        } else if ((next_start != start+sz) && (next_size == 0) && (i+2 == *minfo_mblock_num)) {
            // the last mblock
            LTRACEF("Delete mblock[%d]->start: 0x%llx, size: 0x%llx\n", i+1,
                      next_start, next_size);
            *minfo_mblock_num -= 1;
            i = 0;
            continue;
        }
        i++;
    }
}

static void shared_dma_pool_sanity_check(u64 reserved_size, u64 align, u64 limit,
    u64 expected_address, u32 mapping, const char *name)
{
    /* shared-dma-pool check */
    // e.g. name="shared-dma-pool_wifi-hotspot_dma"
    if (strstr(name, "shared-dma-pool") != NULL) {
        if (strncmp("_dma", name+strlen(name)-4, 4) == 0 && mapping != MBLOCK_NO_MAP) {
            LTRACEF_LEVEL(CRITICAL,
            "requester info, sz: 0x%llx map:%d name:%s align:0x%llx limit:0x%llx expect_addr:0x%llx\n"
                    , reserved_size, mapping, name, align, limit, expected_address);
            LTRACEF_LEVEL(CRITICAL, "node:%s, dma should be set MBLOCK_NO_MAP\n", name);
            ASSERT(0);
        } else if (strncmp("_cma", name+strlen(name)-4, 4) == 0) {
            if (mapping != MBLOCK_REUSABLE) {
                LTRACEF_LEVEL(CRITICAL,
                "requester info, sz: 0x%llx map:%d name:%s align:0x%llx limit:0x%llx expect_addr:0x%llx\n"
                        , reserved_size, mapping, name, align, limit, expected_address);
                // name="shared-dma-pool_wifi-hotspot_cma"
                LTRACEF_LEVEL(CRITICAL, "node:%s, cma should be set MBLOCK_REUSABLE\n", name);
                ASSERT(0);
            }
            if ((align & (0x400000-1)) || (reserved_size&(0x400000-1))) {
                // should align PAGE_SIZE<<10
                LTRACEF_LEVEL(CRITICAL,
                "requester info, sz: 0x%llx map:%d name:%s align:0x%llx limit:0x%llx expect_addr:0x%llx\n"
                        , reserved_size, mapping, name, align, limit, expected_address);
                // name="shared-dma-pool_wifi-hotspot_cma"
                LTRACEF_LEVEL(CRITICAL,
                    "node:%s, cma size, align should be above 4MB align\n", name);
                ASSERT(0);
            }
        } else if ((strncmp("_dma", name+strlen(name)-4, 4) != 0 &&
            strncmp("_cma", name+strlen(name)-4, 4) != 0)) {
            // fail if name is not end with _cma, _dma
            LTRACEF_LEVEL(CRITICAL,
                "node:%s, shared-dma-pool should be end with \"_cma\", \"_dma\"\n", name);
            ASSERT(0);
        }

    }
}

static u64 mblock_alloc_range_no_lock(u64 reserved_size, u64 align, u64 lower_bound,
    u64 limit, u64 expected_address, u32 mapping, const char *name)
{
    unsigned int i;
    int target = -1;
    u64 start, end, sz;
    u64 reserved_addr = 0;
    struct mblock_t mblock_tmp = {0};
    struct mblock_t *p_mblock;
    u64 smallest_size = UINT64_MAX;

    if (!name) {
        LTRACEF_LEVEL(CRITICAL, "name is NULL\n");
        ASSERT(0);
    }

    LTRACEF("reserved_size=0x%llx expected_address=0x%llx\n"
            , reserved_size, expected_address);
    LTRACEF("align=%llx limit=%llx map=%d name=%s\n",
            align, limit, mapping, name);
    ASSERT(mblock_init_done);

    shared_dma_pool_sanity_check(reserved_size, align, limit, expected_address, mapping, name);

    if (reserved_size & (PAGE_SIZE - 1) || align & (PAGE_SIZE - 1))
        panic("size or align not align PAGE_SIZE\n");
    if (expected_address && expected_address & (PAGE_SIZE - 1))
        panic("expected_address not valid\n");
    if (((align - 1) & align) != 0)
        panic("align need to be power of 2\n");
    mblock_merge();

    if (*minfo_mblock_num >= (MBLOCK_NUM_MAX - 1) ||
            *minfo_reserved_num >= (MBLOCK_RESERVED_NUM_MAX - 1) ||
            (mapping > 2) || (name)?(strlen(name) >=
                                      (MBLOCK_RESERVED_NAME_SIZE - 1)):true) {
        /* the mblock[] is full */
        LTRACEF_LEVEL(CRITICAL,
                      "mblock_num=%d,reserved_num=%d mapping=%d name=%s\n",
                      *minfo_mblock_num,
                      *minfo_reserved_num, mapping, name);
        mblock_show_internal(0);
        return 0;
    }

    if (!align)
        align = 0x1000;
    /* must be at least 4k aligned */
    align = ALIGN(align, 0x1000);
    i = 0;
    for_each_mblock(p_mblock) {
        start = p_mblock->start;
        sz = p_mblock->size;
        end = limit < (start + sz) ? limit : (start + sz);
        if (expected_address && (expected_address < start ||
                                 expected_address >= (start + sz))) {
            i++;
            continue;
        }

        if (reserved_size > sz || limit <= start || end <= start || end < lower_bound) {
            LTRACEF("skip this mblock start=%llx sz=%llx limit=%llx end=%llx\n",
                start, sz, limit, end);
            i++;
            continue;
        }

        // Check [lower_bound, end) has enough space, if [lower_bound, end] is in mblock
        if (start <= lower_bound && end >= lower_bound) {
            if (end < lower_bound + reserved_size) {
                i++;
                continue;
            }
        }

        if (expected_address) {
            reserved_addr = expected_address;
        } else {
            reserved_addr = (end - reserved_size);
            reserved_addr &= ~(align - 1);
        }
        LTRACEF("mblock[%d].start: 0x%llx, sz: 0x%llx\n", i, start, sz);
        LTRACEF("limit: 0x%llx\n", limit);
        LTRACEF("reserved_addr: 0x%llx, reserved_size: 0x%llx\n",
                reserved_addr, reserved_size);
        LTRACEF("%s: dbg[%d]: %d, %d, %d, %d %d\n", __func__,
                i, !mblock_cross_bound(reserved_addr, reserved_size),
                ((reserved_addr + reserved_size) <= (start + sz)),
                (reserved_addr >= start),
                ((reserved_addr + reserved_size) <= limit),
                (reserved_addr >= lower_bound));
        if ((!mblock_cross_bound(reserved_addr, reserved_size)) &&
                ((reserved_addr + reserved_size) <= (start + sz)) &&
                (reserved_addr >= start) &&
                ((reserved_addr + reserved_size) <= limit) &&
                (reserved_addr >= lower_bound)) {
            if (internal_mblock_info->mblock[i].size <= smallest_size) {
                target = i; // default: find as high address as possible
#if MBLOCK_BESTFIT
                // find the smallest mblock
                smallest_size = internal_mblock_info->mblock[target].size;
#endif
                LTRACEF("target = %d\n", i);
            }
        }
        i++;
    }
    if (target < 0) {
        LTRACEF_LEVEL(CRITICAL, "error cannot find a target\n");
        LTRACEF_LEVEL(CRITICAL,
        "requester info, sz: 0x%llx map:%d name:%s align:0x%llx lower_bound: 0x%llx, limit:0x%llx,"
                , reserved_size, mapping, name, align, lower_bound, limit);
        LTRACEF_LEVEL(CRITICAL, "expect_addr:0x%llx\n", expected_address);
        mblock_show_internal(0);
        return 0;
    }


    /* update variable reference to correct target info*/
    start = internal_mblock_info->mblock[target].start;
    sz = internal_mblock_info->mblock[target].size;
    LTRACEF("start: 0x%llx, sz: 0x%llx\n", start, sz);
    end = limit < (start + sz) ? limit : (start + sz);
    if (!expected_address) {
        reserved_addr = (end - reserved_size);
        reserved_addr &= ~(align - 1);
    }


    /*sanity check , reserved_num of array must be empty */
    if (internal_mblock_info->reserved[*minfo_reserved_num].start) {
        LTRACEF_LEVEL(CRITICAL,
                      "error ,exist start=0x%llx size=0x%llx\n",
                      internal_mblock_info->reserved[*minfo_reserved_num].start
                      , internal_mblock_info->reserved[*minfo_reserved_num].size);
        mblock_show_internal(0);
        return 0;
    }

    internal_mblock_info->reserved[*minfo_reserved_num].start = reserved_addr;
    internal_mblock_info->reserved[*minfo_reserved_num].size = reserved_size;
    internal_mblock_info->reserved[*minfo_reserved_num].mapping = mapping;
    if (name && strlen(name) < MBLOCK_RESERVED_NAME_SIZE) {
        strncpy(internal_mblock_info->reserved[*minfo_reserved_num].name, name, strlen(name));
        internal_mblock_info->reserved[*minfo_reserved_num].name[strlen(name)] = '\0';
    } else
        panic("%s: name length exceed max length\n", __func__);

    (*minfo_reserved_num)++;

    /* split mblock if necessary */
    if (reserved_addr == start) {
        /*
         * only needs to fixup target mblock
         * [reserved_addr, reserved_size](reserved) +
         * [reserved_addr + reserved_size, sz - reserved_size]
         */
        internal_mblock_info->mblock[target].start = reserved_addr + reserved_size;
        internal_mblock_info->mblock[target].size -= reserved_size;
    } else {
        /*
         * fixup target mblock and create a new mblock
         * [start, reserved_addr - start] +
         * [reserved_addr, reserved_size](reserved) +
         * [reserved_addr + reserved_size, start + sz - reserved_addr - reserved_size]
         */

        /* new mblock */
        mblock_tmp.start = reserved_addr + reserved_size;
        mblock_tmp.size = (start + sz) - (reserved_addr + reserved_size);
        LTRACEF("start = %llx sz=%llx reserved_addr=%llx reserved_size=%llx\n"
                , start, sz, reserved_addr, reserved_size);
        LTRACEF("mblock.start = %llx mblock.size=%llx", mblock_tmp.start
                , mblock_tmp.size);
        /* setup rank */
        for (i = 0; i < internal_orig_dram_info->rank_num; i++) {
            u64 rank_start, rank_sz;

            rank_start = internal_orig_dram_info->rank_info[i].start;
            rank_sz = internal_orig_dram_info->rank_info[i].size;
            if ((mblock_tmp.start >= rank_start) &&
                ((mblock_tmp.start + mblock_tmp.size) <= (rank_start + rank_sz))) {
                mblock_tmp.rank = i;
                break;
            }
        }
        if (i >= internal_orig_dram_info->rank_num) {
            LTRACEF("mblock not in orig_dram_info, or cross rank,addr:%llx, size:%llx\n"
                , mblock_tmp.start, mblock_tmp.size);
        }

        /* fixup original mblock */
        internal_mblock_info->mblock[target].size = reserved_addr - start;

        if ((target + 1) < 0 || (target + 2) < 0) {
            LTRACEF_LEVEL(CRITICAL, "error: (target +1) < 0 or (target + 2) < 0\n");
            return 0;
        }

        /* insert the new node, keep the list sorted */
        memmove(&internal_mblock_info->mblock[target + 2],
                &internal_mblock_info->mblock[target + 1],
                sizeof(struct mblock_t) *
                (*minfo_mblock_num - target - 1));

        internal_mblock_info->mblock[target + 1] = mblock_tmp;
        *minfo_mblock_num += 1;

        LTRACEF("mblock[%d]: %llx, %llx from mblock\n"
                "mblock[%d]: %llx, %llx from mblock\n",
                target,
                internal_mblock_info->mblock[target].start,
                internal_mblock_info->mblock[target].size,
                target + 1,
                internal_mblock_info->mblock[target + 1].start,
                internal_mblock_info->mblock[target + 1].size);
    }

    LTRACEF_LEVEL(CRITICAL, "start: 0x%llx, sz: 0x%llx lower_bound: 0x%llx, limit: 0x%llx,"
                , reserved_addr, reserved_size, lower_bound, limit);
    LTRACEF_LEVEL(CRITICAL, "map:%d name:%s\n"
                , mapping, name);

    return reserved_addr;
}

/*
 * reserve a memory from mblock
 * @reserved_size: size of memory
 * @align: alignment
 * @limit: address limit. Must higher than return address + reserved_size
 * @expected_address: 0:system find a suitable mblock for user,
 *                    otherwise user can decide expected addr
 * @mapping: describe kernel mapping mechanism ,
 *          MBLOCK_MAP:map, MBLOCK_NO_MAP:no-map or MBLOCK_REUSABLE:reusable
 * @name: assign a dedicated name for this memory area
 * It returns address as high as possible.
 * If fail, reserved_addr will be zero and panic.
 */
u64 mblock_alloc(u64 reserved_size, u64 align, u64 limit,
                 u64 expected_address, u32 mapping, const char *name)
{
    u64 reserved_addr;

    mutex_acquire(&mblock_lock);
    reserved_addr = mblock_alloc_range_no_lock(reserved_size, align,
        0, limit, expected_address, mapping, name);
    mutex_release(&mblock_lock);

    if (!reserved_addr)
        panic("%s failed\n", __func__);

    return reserved_addr;
}

/*
 * reserve a memory from mblock
 * @reserved_size: size of memory
 * @align: alignment
 * @lower_bound: Must lower than return address
 * @limit: address limit. Must higher than return address + reserved_size
 * @expected_address: 0:system find a suitable mblock for user,
 *                    otherwise user can decide expected addr
 * @mapping: describe kernel mapping mechanism ,
 *          MBLOCK_MAP:map, MBLOCK_NO_MAP:no-map or MBLOCK_REUSABLE:reusable
 * @name: assign a dedicated name for this memory area
 * It returns address as high as possible between [lower_bound, limit).
 * If fail, reserved_addr will be zero and panic.
 */
u64 mblock_alloc_range(u64 reserved_size, u64 align, u64 lower_bound, u64 limit,
                 u64 expected_address, u32 mapping, const char *name)
{
    u64 reserved_addr;

    mutex_acquire(&mblock_lock);
    reserved_addr = mblock_alloc_range_no_lock(reserved_size, align, lower_bound, limit,
        expected_address, mapping, name);
    mutex_release(&mblock_lock);

    if (!reserved_addr)
        panic("%s failed\n", __func__);

    return reserved_addr;
}

/*
 * pre_alloc phandle, will generate phandle and set memory-region for device node
 * when append reserved memory, only support in LK, Not support in BL2_EXT
 * @device_name: device node name, NOTICE: if it is hierarchy, please add slash.
 * @mblock_name: reserved-memory name
 * return 0 on success, otherwise -1
 */
int mblock_pre_alloc_phandle(const char *device_name, const char *mblock_name)
{
    const struct reserved_t *p_reserved = NULL;

    ASSERT(mblock_init_done);

    p_reserved = mblock_query_reserved_by_name(mblock_name, 0);

    mutex_acquire(&mblock_lock);
    if (p_reserved == NULL) {
        LTRACEF_LEVEL(CRITICAL, "Error: cannot find %s node\n", mblock_name);
        mutex_release(&mblock_lock);
        return -1;
    }

    if (device_dts_num >= MBLOCK_RESERVED_NUM_MAX) {
        LTRACEF_LEVEL(CRITICAL, "Error: device_dts reach the max:%d\n", MBLOCK_RESERVED_NUM_MAX);
        mutex_release(&mblock_lock);
        return -1;
    }

    if (strlen(device_name) >= MBLOCK_RESERVED_NAME_SIZE) {
        LTRACEF_LEVEL(CRITICAL, "Warning device_name reach max size\n");
        memcpy(device_dts[device_dts_num].device_name, device_name, MBLOCK_RESERVED_NAME_SIZE);
        device_dts[device_dts_num].device_name[MBLOCK_RESERVED_NAME_SIZE-1] = '\0';
    } else {
        memcpy(device_dts[device_dts_num].device_name, device_name, strlen(device_name));
        device_dts[device_dts_num].device_name[strlen(device_name)] = '\0';
    }

    if (strlen(mblock_name) >= MBLOCK_RESERVED_NAME_SIZE) {
        LTRACEF_LEVEL(CRITICAL, "Warning mblock_name reach max size\n");
        memcpy(device_dts[device_dts_num].mblock_name, mblock_name, MBLOCK_RESERVED_NAME_SIZE);
        device_dts[device_dts_num].mblock_name[MBLOCK_RESERVED_NAME_SIZE-1] = '\0';
    } else {
        memcpy(device_dts[device_dts_num].mblock_name, mblock_name, strlen(mblock_name));
        device_dts[device_dts_num].mblock_name[strlen(mblock_name)] = '\0';
    }
    LTRACEF_LEVEL(CRITICAL, "device_name: %s, mblock_name: %s success\n",
        device_dts[device_dts_num].device_name, device_dts[device_dts_num].mblock_name);
    device_dts_num++;
    mutex_release(&mblock_lock);
    return 0;
}

static int phandle_index(char *mblock_name)
{
    unsigned int i = 0;

    for (i = 0; i < device_dts_num; i++) {
        if (strcmp(mblock_name, device_dts[i].mblock_name) == 0)
            return i;
    }
    return -1;
}

/*
 * mblock_resize - resize mblock started at addr from oldsize to newsize,
 * current implementation only consider oldsize >= newsize.
 *
 * @addr: start address of a mblock
 * @oldsize: origianl size of the mblock
 * @newsize: new size of the given block
 * return 0 on success, otherwise panic
 */
int mblock_resize(u64 addr, u64 oldsize, u64 newsize)
{
    u64 start, sz;
    u32 i;
    struct mblock_t mblock_tmp = {0};
    struct mblock_t *p_mblock;
    struct reserved_t *p_reserved;
    unsigned int target = 0;

    ASSERT(mblock_init_done);
    /* check size, oldsize must larger than newsize */
    if (oldsize <= newsize) {
        LTRACEF_LEVEL(CRITICAL,
                      "error: mblock %llx oldsize(%llx) <= newsize(%llx)",
                      addr, oldsize, newsize);
        return -1;
    }

    /* check alignment, at least 4k aligned */
    if ((oldsize & (0x1000 - 1)) || (newsize & (0x1000 - 1))) {
        LTRACEF_LEVEL(CRITICAL,
                      "alignment error: oldsize(%llx) or newsize(%llx)\n",
                      oldsize, newsize);
        return -1;
    }

    /* check mblock for overlap
     * resized memory must not exist in mblock[i]
     * it must be in reserved[i]
     */
    mutex_acquire(&mblock_lock);
    for_each_mblock(p_mblock) {
        start = p_mblock->start;
        sz = p_mblock->size;
        /* invalid mblock */
        if (sz && (((start >= addr) && (start < (addr + oldsize)))
                   || ((start < addr) && (start + sz > addr)))) {
            LTRACEF_LEVEL(CRITICAL,
                          "addr %llx, oldsize: %llx  start: 0x%llx sz: 0x%llx is free\n",
                          addr, oldsize, start, sz);
            mblock_show_internal(0);
            mutex_release(&mblock_lock);
            panic("%s: invalid resize mblock\n", __func__);
            return -1;
        }
    }

    /* check reserved */
    /* this record must exist exactly */
    i = 0;
    for_each_mblock_reserved(p_reserved) {
        LTRACEF("start: %llx, size: %llx addr = %llx oldsize = %llx\n",
                p_reserved->start,
                p_reserved->size, addr, oldsize);
        if ((addr == p_reserved->start) && (oldsize
                                            == p_reserved->size)) {
            target = i;
            break;
        }
        i++;
    }

    if (i == *minfo_reserved_num) {
        LTRACEF_LEVEL(CRITICAL,
                      "error: mblock addr: %llx, size: %llx is not exist\n",
                      addr, oldsize);
        mblock_show_internal(0);
        mutex_release(&mblock_lock);
        panic("%s: failed\n", __func__);
        return -1;
    }

    /*
     * ok, the mblock is valid and oldsize > newsize, let's
     * shrink this mblock
     */
    /* setup a new mblock */
    mblock_tmp.start = addr + newsize;
    mblock_tmp.size = oldsize - newsize;
    LTRACEF("putback mblock %llx size: %llx\n",
            mblock_tmp.start, mblock_tmp.size);
    /* setup rank */
    for (i = 0; i < internal_orig_dram_info->rank_num; i++) {
        u64 rank_start, rank_sz;

        rank_start = internal_orig_dram_info->rank_info[i].start;
        rank_sz = internal_orig_dram_info->rank_info[i].size;
        if ((mblock_tmp.start >= rank_start) &&
            ((mblock_tmp.start + mblock_tmp.size) <= (rank_start + rank_sz))) {
            mblock_tmp.rank = i;
            break;
        }
    }
    if (i >= internal_orig_dram_info->rank_num) {
        LTRACEF("mblock not in orig_dram_info, or cross rank,addr:%llx, size:%llx\n"
            , mblock_tmp.start, mblock_tmp.size);
    }

    /* here , we decide to shrink or delete this reserved record*/
    if (newsize == 0) {
        /* destroy current record first */
        memset(&internal_mblock_info->reserved[target], 0, sizeof(struct reserved_t));
        /* more than one record exist , we need to shift record */
        /* we no need to shif record once it's the last one */
        if (*minfo_reserved_num > 1 && target !=
                *minfo_reserved_num - 1) {
            memmove(&internal_mblock_info->reserved[target],
                    &internal_mblock_info->reserved[target + 1], sizeof(struct reserved_t) *
                    ((*minfo_reserved_num - 1) - target));
        }

        (*minfo_reserved_num)--;
    }
    /* shirnk size directly */
    else
        internal_mblock_info->reserved[target].size = newsize;

    /* put the mblock back to mblock_info */
    for_each_mblock(p_mblock) {
        start = p_mblock->start;
        sz = p_mblock->size;
        if (mblock_tmp.start == start + sz) {
            /*
             * the new mblock can be merged to this mblock
             * [start, start + sz] +
             * [mblock_tmp.start, mblock_tmp.start + mblock_tmp.size](new)
             */
            p_mblock->size += mblock_tmp.size;
            /* destroy block */
            mblock_tmp.size = 0;
            break;
        } else if (start == mblock_tmp.start + mblock_tmp.size) {
            /*
             * the new mblock can be merged to this mblock
             * [mblock_tmp.start, mblock_tmp.start + * mblock_tmp.size](new) +
             * [start, start + sz]
             */
            p_mblock->start = mblock_tmp.start;
            p_mblock->size += mblock_tmp.size;
            /* destroy block */
            mblock_tmp.size = 0;
            break;
        }
    }

    /*
     * mblock cannot be merge info mblock_info, insert it into mblock_info
     */
    if (mblock_tmp.size) {
        i = 0;
        for_each_mblock(p_mblock) {
            if (mblock_tmp.start < p_mblock->start)
                break;
            i++;
        }
        memmove(&internal_mblock_info->mblock[i + 1],
                &internal_mblock_info->mblock[i],
                sizeof(struct mblock_t) *
                (*minfo_mblock_num - i));
        internal_mblock_info->mblock[i] = mblock_tmp;
        *minfo_mblock_num += 1;
    }

    mutex_release(&mblock_lock);

    return 0;
}

/*
 * mblock_free_with_size - free mblock started at addr
 *
 * @addr: start address of a mblock, must be 4k align
 * @size: size of the given block, must be 4K align
 * return 0 on success, otherwise -1
 */
static int mblock_free_with_size(u64 addr, u64 size)
{
    unsigned int i, target;
    u64 start, sz;
    struct mblock_t mblock_tmp = {0};
    struct reserved_t reserved_tmp;
    struct mblock_t *p_mblock;
    struct reserved_t *p_reserved;
    struct mblock_t *mblock_candidate_left = NULL, *mblock_candidate_right = NULL;
    char name[MBLOCK_RESERVED_NAME_SIZE] = {0};

    ASSERT(mblock_init_done);
    LTRACEF_LEVEL(INFO, "addr=0x%llx size=0x%llx\n"
                  , addr, size);
    /* check size, addr valid and align with 4K*/
    if (!size || size & (PAGE_SIZE - 1) || addr & (PAGE_SIZE - 1))
        panic("%s:size invalid size=%llx\n", __func__, size);

    /* check every mblock the addr and size should not be within any existing mblock */
    for_each_mblock(p_mblock) {
        start = p_mblock->start;
        sz = p_mblock->size;
        /*addr should start from reserved memory
         * space and addr + size should not overlap with mblock
         * when addr is smaller than start
         */
        if (((addr >= start) && (addr < start + sz)) || (addr < start && addr + size > start)) {
            LTRACEF_LEVEL(CRITICAL,
                          "error addr %llx overlap with mblock %llx, size: %llx\n",
                          addr, start, sz);
            return -1;
        }
    }

    /* check if reserved record contain this one , it must exist */
    i = 0;
    target = 0;
    for_each_mblock_reserved(p_reserved) {
        start = p_reserved->start;
        sz = p_reserved->size;
        if (addr >= start && ((addr + size) <= (start + sz))) {
            target = i;
            break;
        }
        i++;
    }
    if (strlen(internal_mblock_info->reserved[target].name) < MBLOCK_RESERVED_NAME_SIZE) {
        memcpy(name, internal_mblock_info->reserved[target].name,
               strlen(internal_mblock_info->reserved[target].name));
        name[strlen(internal_mblock_info->reserved[target].name)] = '\0';
    } else {
        memcpy(&name, &internal_mblock_info->reserved[target].name, MBLOCK_RESERVED_NAME_SIZE - 1);
        name[MBLOCK_RESERVED_NAME_SIZE - 1] = '\0';
    }

    if (i == *minfo_reserved_num) {
        LTRACEF_LEVEL(CRITICAL,
                      "error: not exist\n addr=0x%llx size=0x%llx\n",
                      addr, size);
        return -1;
    }

    /* dealing with 4 case */
    /* 1. create whole reserved record means destroy it and shift rest record*/
    /* 2. create from the left most side to the middle of record*/
    /* 3. create from the right most side to the middle of record*/
    /* 4. create from the middle of record, and then divide it*/
    start =  internal_mblock_info->reserved[target].start;
    sz = internal_mblock_info->reserved[target].size;
    if (addr == start && size == sz) {
        /* destroy current record first */
        memset(&internal_mblock_info->reserved[i], 0, sizeof(struct reserved_t));
        /* more than one record exist , we need to shift record */
        /* we no need to shift record once it's the last one */
        if (*minfo_reserved_num > 1 &&
                target != *minfo_reserved_num - 1) {
            memmove(&internal_mblock_info->reserved[target],
                    &internal_mblock_info->reserved[target + 1]
                    , sizeof(struct reserved_t) *
                    ((*minfo_reserved_num - 1) - target));
            /* after memmove, we must clean the last one */
            memset(&internal_mblock_info->reserved[*minfo_reserved_num - 1],
                   0, sizeof(struct reserved_t));
        }
        (*minfo_reserved_num)--;
    } else if (addr == start || (addr + size == start + sz)) {
        /*Now we deal with left and right most case*/
        /* we just shrink the record */
        if (addr == start)
            internal_mblock_info->reserved[target].start = start + size;

        internal_mblock_info->reserved[target].size = sz - size;
    } else {/* this is middle case we need to divide it*/
        /* shrink original one and create new one after */
        if (*minfo_reserved_num >= MBLOCK_RESERVED_NUM_MAX) {
            LTRACEF_LEVEL(CRITICAL,
                          "error reserved_num reach the max\n");
            return -1;
        }

        reserved_tmp.start = addr + size;
        reserved_tmp.size = (start + sz) - (addr + size);
        /* clone from original one*/
        reserved_tmp.mapping  = internal_mblock_info->reserved[target].mapping;
        memcpy(&reserved_tmp.name, &internal_mblock_info->reserved[target].name,
               MBLOCK_RESERVED_NAME_SIZE);
        /* check if this target is last one or not */
        /* target start from 0 , reserved_num start from 1 */
        if (target != *minfo_reserved_num - 1) {
            for (i = 0; i < (*minfo_reserved_num - target - 1); i++) {
                internal_mblock_info->reserved[*minfo_reserved_num - i] =
                    internal_mblock_info->reserved[*minfo_reserved_num - i - 1];
            }
            internal_mblock_info->reserved[target+1] = reserved_tmp;
        } else {/*target is the last one */
            internal_mblock_info->reserved[*minfo_reserved_num] = reserved_tmp;
        }
        /* shrink original target size at last step */
        internal_mblock_info->reserved[target].size = addr - start;

        (*minfo_reserved_num)++;

    }

    /*
     * ok, the mblock is valid let's create the mblock
     * and try to merge it with the same bank and choose the bigger size one
     */
    /* setup a new mblock */
    mblock_tmp.start = addr;
    mblock_tmp.size = size;
    // print the caller's name
    LTRACEF_LEVEL(CRITICAL, "start 0x%llx size: 0x%llx, name: %s\n",
            mblock_tmp.start, mblock_tmp.size, name);
    /* setup rank */
    for (i = 0; i < internal_orig_dram_info->rank_num; i++) {
        u64 rank_start, rank_sz;

        rank_start = internal_orig_dram_info->rank_info[i].start;
        rank_sz = internal_orig_dram_info->rank_info[i].size;
        if ((mblock_tmp.start >= rank_start) &&
            ((mblock_tmp.start + mblock_tmp.size) <= (rank_start + rank_sz))) {
            mblock_tmp.rank = i;
            break;
        }
    }
    if (i >= internal_orig_dram_info->rank_num) {
        LTRACEF("mblock not in orig_dram_info, or cross rank,addr:%llx, size:%llx\n"
            , mblock_tmp.start, mblock_tmp.size);
    }

    /* put the mblock back to mblock_info */
    for_each_mblock(p_mblock) {
        start = p_mblock->start;
        sz = p_mblock->size;
        if (mblock_tmp.start + mblock_tmp.size == start) {
            /*
             * the new mblock could be merged to this mblock
             */
            mblock_candidate_right = p_mblock;
        } else if (start + sz == mblock_tmp.start) {
            /*
             * the new mblock can be merged to this mblock
             */
            mblock_candidate_left =  p_mblock;
        }
    }
    /*we can merge either left or right , choose the bigger one */
    if (mblock_candidate_right && mblock_candidate_left) {
        if (mblock_candidate_right->size >= mblock_candidate_left->size) {
            LTRACEF("right->size = %llx left->size = %llx\n",
                    mblock_candidate_right->size, mblock_candidate_left->size);
            mblock_candidate_right->start = mblock_tmp.start;
            mblock_candidate_right->size += mblock_tmp.size;
        } else { /*left bigger*/
            LTRACEF("right->size = %llx left->size = %llx\n",
                    mblock_candidate_right->size, mblock_candidate_left->size);
            mblock_candidate_left->size += mblock_tmp.size;
        }
        /* destroy block */
        mblock_tmp.size = 0;
    } else {
        if (mblock_candidate_right) {
            mblock_candidate_right->start = mblock_tmp.start;
            mblock_candidate_right->size += mblock_tmp.size;
            /* destroy block */
            mblock_tmp.size = 0;
        }

        if (mblock_candidate_left) {
            mblock_candidate_left->size += mblock_tmp.size;
            /* destroy block */
            mblock_tmp.size = 0;
        }
    }

    /*
     * mblock cannot be merge into mblock_info, insert it into mblock_info
     */
    if (mblock_tmp.size) {
        i = 0;
        for_each_mblock(p_mblock) {
            if (mblock_tmp.start < p_mblock->start)
                break;
            i++;
        }
        /* insert the new node, keep the list sorted */
        if (i != *minfo_mblock_num) {
            memmove(&internal_mblock_info->mblock[i + 1],
                    &internal_mblock_info->mblock[i],
                    sizeof(struct mblock_t) *
                    (*minfo_mblock_num - i));
        }
        internal_mblock_info->mblock[i] = mblock_tmp;
        (*minfo_mblock_num)++;
        LTRACEF("create mblock[%d]: %llx, %llx\n",
                i,
                internal_mblock_info->mblock[i].start,
                internal_mblock_info->mblock[i].size);
    }
    mblock_merge();
    return 0;
}

int mblock_free(u64 addr)
{
    struct reserved_t *p_reserved;
    int ret;

    ASSERT(mblock_init_done);
    mutex_acquire(&mblock_lock);
    for_each_mblock_reserved(p_reserved) {
        if (p_reserved->start == addr) {
            ret = mblock_free_with_size(addr, p_reserved->size);
            if (ret != 0)
                break;
            mutex_release(&mblock_lock);
            return ret;
        }
    }

    LTRACEF_LEVEL(CRITICAL, "Error: addr: %llx\n", addr);
    mblock_show_internal(0);

    mutex_release(&mblock_lock);
    panic("%s failed panic\n", __func__);
    return -1;
}

int mblock_free_partial(u64 addr, u64 size)
{
    struct reserved_t *p_reserved;
    int ret;

    ASSERT(mblock_init_done);
    mutex_acquire(&mblock_lock);
    for_each_mblock_reserved(p_reserved) {
        if (p_reserved->start >= addr &&
                addr < p_reserved->start + p_reserved->size) {
            ret = mblock_free_with_size(addr, size);
            if (ret != 0)
                break;
            mutex_release(&mblock_lock);
            return ret;
        }
    }

    LTRACEF_LEVEL(CRITICAL, "Error: addr: %llx, size: %llx\n", addr, size);
    mblock_show_internal(0);

    mutex_release(&mblock_lock);
    panic("%s failed panic\n", __func__);
    return -1;
}

const struct reserved_t *mblock_query_reserved_by_idx(int index)
{
    struct reserved_t *p_reserved;

    mutex_acquire(&mblock_lock);
    if (((unsigned int)index < *minfo_reserved_num) && (*minfo_reserved_num > 0)) {
        p_reserved = &internal_mblock_info->reserved[index];
        mutex_release(&mblock_lock);
        return p_reserved;
    } else {
        mutex_release(&mblock_lock);
        return NULL;
    }
}

const struct reserved_t *mblock_query_reserved_by_name
(const char *name, struct reserved_t *index)
{
    struct reserved_t *p_reserved;

    mutex_acquire(&mblock_lock);
    for_each_mblock_reserved(p_reserved) {
        if (index && (p_reserved <= index))
            continue;
        if ((strlen(p_reserved->name) && strlen(name)) &&
                (strlen(p_reserved->name) == strlen(name)) &&
                (strncmp(p_reserved->name, name, strlen(name)) == 0)) {
            mutex_release(&mblock_lock);
            return p_reserved;
        }
    }
    mutex_release(&mblock_lock);
    return 0;
}

int mblock_query_reserved_count(const char *name)
{
    struct reserved_t *p_reserved;
    int i = 0;

    mutex_acquire(&mblock_lock);
    for_each_mblock_reserved(p_reserved) {
        if ((strlen(p_reserved->name) && strlen(name)) &&
                (strlen(p_reserved->name) == strlen(name)) &&
                (strncmp(p_reserved->name, name, strlen(name)) == 0))
            i++;
    }
    mutex_release(&mblock_lock);
    return i;
}

u64 mblock_get_memory_size(void)
{
    u64 total_size = 0;
    struct mblock_t *p_mblock;
    struct reserved_t *p_reserved;

    mutex_acquire(&mblock_lock);

    for_each_mblock(p_mblock) {
        if (p_mblock->start)
            total_size += p_mblock->size;
    }

    for_each_mblock_reserved(p_reserved) {
        if (p_reserved->start)
            total_size += p_reserved->size;
    }

    mutex_release(&mblock_lock);

    return total_size;
}

u64 mblock_get_memory_start(void)
{
    struct mblock_t *p_mblock;
    struct reserved_t *p_reserved;
    u64 dram_start = MBLOCK_NO_LIMIT;

    mutex_acquire(&mblock_lock);

    for_each_mblock(p_mblock) {
        if (p_mblock->start <= dram_start)
            dram_start = p_mblock->start;
    }

    for_each_mblock_reserved(p_reserved) {
        if (p_reserved->start <= dram_start)
            dram_start = p_reserved->start;
    }

    mutex_release(&mblock_lock);

    return dram_start;
}

/*
 * Basic check if dts reserved-memory node which has alloc-range property cannot
 * be allocated in mblock.
 * Important!! It still has probability failing alloc reserved-memory alloc-range node
 * in the kernel. Because we cannot check whether two alloc-range node can fit
 * in one mblock or an alloc-range node cross rank.
 */
static int reserved_memory_alloc_ranges_basic_check(void *fdt)
{

    int reserved_memory_offset, reserved_node, last_node;
    int len, alloc_fail = 1;
    const struct fdt_property *prop;
    struct dt_dram_info *data;
    struct dt_size_info *size_data;
    u64 mblock_start, mblock_sz;
    u64 alloc_ranges_size, alloc_ranges_start;
    u64 alloc_size;
    char *status;
    struct mblock_t *pmblock;

    mblock_merge();
    // check if the alloc-ranges is enough
    reserved_memory_offset = fdt_path_offset(fdt, "/reserved-memory");
    if (reserved_memory_offset < 0) {
        LTRACEF_LEVEL(CRITICAL, "couldn't find /reserved-memory\n");
        panic("couldn't find /reserved-memory\n");
    }

    for (reserved_node = fdt_first_subnode(fdt, reserved_memory_offset); reserved_node >= 0;
        reserved_node = fdt_next_subnode(fdt, last_node)) {
        prop = fdt_get_property(fdt, reserved_node, "alloc-ranges", &len);
        if (!prop) {
            last_node = reserved_node;
            continue;
        }

        data = (struct dt_dram_info *)prop->data;
        alloc_ranges_start = (((u64)fdt32_to_cpu(data->start_hi))<<32)|
                    (fdt32_to_cpu(data->start_lo));
        alloc_ranges_size = (((u64)fdt32_to_cpu(data->size_hi))<<32)|
                    (fdt32_to_cpu(data->size_lo));

        prop = fdt_get_property(fdt, reserved_node, "status", &len);

        // If there is status, status need to be "ok" or "okay"
        // If there is no status, default is ok
        if (prop) {
            status = (char *)prop->data;
            LTRACEF("DTS node:%s status: %s\n", fdt_get_name(fdt, reserved_node, NULL), status);

            if (strncmp(status, "ok", 2) != 0 && strncmp(status, "okay", 4) != 0) {
                last_node = reserved_node;
                continue;
            }
        }

        prop = fdt_get_property(fdt, reserved_node, "size", &len);
        if (!prop) {
            last_node = reserved_node;
            continue;
        }
        size_data = (struct dt_size_info *)prop->data;
        alloc_size = (((u64)fdt32_to_cpu(size_data->size_hi))<<32)|
            (fdt32_to_cpu(size_data->size_lo));

        LTRACEF("DTS node:%s alloc-ranges: 0x%llx ~ 0x%llx\n",
            fdt_get_name(fdt, reserved_node, NULL), alloc_ranges_start,
                alloc_ranges_start+alloc_ranges_size);
        LTRACEF("DTS node:%s alloc-ranges size: 0x%llx\n",
            fdt_get_name(fdt, reserved_node, NULL), alloc_size);

        // if alloc-range cross rank, we will bypass this node
        if (mblock_cross_bound(alloc_ranges_start, alloc_ranges_size)) {
            LTRACEF("DTS node:%s cross rank, bypass\n", fdt_get_name(fdt, reserved_node, NULL));
            last_node = reserved_node;
            continue;
        }

        // check overlap with dts reserved-memory. If overlap, check the range size
        alloc_fail = 1;
        for_each_mblock(pmblock) {
            mblock_start = pmblock->start;
            mblock_sz = pmblock->size;
            if (((alloc_ranges_start >= mblock_start) &&
                (alloc_ranges_start < (mblock_start+mblock_sz)))
            || ((mblock_start >= alloc_ranges_start) &&
                (mblock_start < (alloc_ranges_start + alloc_ranges_size)))) {
                u64 high, low;

                high = (alloc_ranges_start+alloc_ranges_size > mblock_start+mblock_sz)
                    ? (mblock_start+mblock_sz) : (alloc_ranges_start+alloc_ranges_size);
                low = (alloc_ranges_start < mblock_start) ? mblock_start : alloc_ranges_start;
                if (high - low >= alloc_size) {
                    LTRACEF("DTS node:%s done, still has probability of allocate fail in kernel\n"
                        , fdt_get_name(fdt, reserved_node, NULL));
                    alloc_fail = 0;
                    break;
                }
            }
        }
        if (alloc_fail) {
            /* print debug info */
            mblock_show_internal(0);
            LTRACEF_LEVEL(CRITICAL,
            "fatal error,dts reserved-memory node conflict with mblock reserved-memory node\n");
            LTRACEF_LEVEL(CRITICAL, "DTS node:%s alloc-ranges: 0x%llx ~ 0x%llx\n",
                fdt_get_name(fdt, reserved_node, NULL), alloc_ranges_start,
                    alloc_ranges_start+alloc_ranges_size);
            LTRACEF_LEVEL(CRITICAL, "DTS node:%s alloc-ranges size: 0x%llx\n",
                fdt_get_name(fdt, reserved_node, NULL), alloc_size);

            ASSERT(0);
        }

        last_node = reserved_node;
    }

    /*
     * pass basic check, dts rsv memory node still has probability allocate fail in kernel
     * This function is just a basic check.
     */
    LTRACEF("%s, Only basic check done.\n", __func__);
    LTRACEF("Note that, dts rsv memory node still has probability alloc fail in kernel\n");
    return 0;
}

static int kernel_dts_mblock_conflict_check(void *fdt)
{
    int reserved_memory_offset, reserved_node, last_node;
    int len, sanity_fail = 0;
    unsigned int i;
    const struct fdt_property *prop;
    struct reserved_t *preserved;
    u64 start, sz, reserved_start, reserved_sz;
    struct dt_dram_info *data;

    if (!fdt) {
        LTRACEF_LEVEL(CRITICAL, "fdt is NULL\n");
        ASSERT(0);
    }

    reserved_memory_offset = fdt_path_offset(fdt, "/reserved-memory");
    if (reserved_memory_offset < 0)
        panic("couldn't find /reserved-memory\n");

    for (reserved_node = fdt_first_subnode(fdt, reserved_memory_offset); reserved_node >= 0;
            reserved_node = fdt_next_subnode(fdt, last_node)) {
        prop = fdt_get_property(fdt, reserved_node, "reg", &len);
        if (!prop) {
            last_node = reserved_node;
            continue;
        }

        data = (struct dt_dram_info *)prop->data;
        start = (((u64)fdt32_to_cpu(data->start_hi))<<32)|(fdt32_to_cpu(data->start_lo));
        sz = (((u64)fdt32_to_cpu(data->size_hi))<<32)|(fdt32_to_cpu(data->size_lo));

        LTRACEF_LEVEL(INFO, "DTS node:%s reserved start: 0x%llx size: 0x%llx\n",
                      fdt_get_name(fdt, reserved_node, NULL), start, sz);
        i = 0;
        for_each_mblock_reserved(preserved) {
            reserved_start = preserved->start;
            reserved_sz = preserved->size;
            if (((start < reserved_start) &&
                    ((start + sz) > (reserved_start))) ||
                    ((start >= reserved_start) && ((start < (reserved_start + reserved_sz))))) {
                sanity_fail = 1;
                LTRACEF_LEVEL(CRITICAL, "failed i:%d %d:%d:%d:%d\n", i
                   , (start < reserved_start), ((start + sz) > (reserved_start))
                   , (start >= reserved_start), ((start < (reserved_start + reserved_sz))));
            }
            i++;
        }

        if (sanity_fail) {
            LTRACEF_LEVEL(CRITICAL, "error\n");
            mblock_show_internal(0);
            ASSERT(0);
        }

        last_node = reserved_node;
    }
    LTRACEF_LEVEL(INFO, "%s:PASS\n", __func__);
    return 0;
}
/*
 * mblock_sanity_check
 * after executing mblock related api , we perform
 * sanity check in the last step to confirm there
 * is nothing wrong, we will check overlap and
 * conflict between DTS and mblock API
 *
 * input:
 * @fdt: pointer to fdt
 *
 * output:
 * return 0 on success, otherwise 1
 */
static int mblock_sanity_check(void *fdt)
{
    u64 mblock_start, reserved_start, mblock_sz, reserved_sz;
    unsigned int i, j, sanity_fail = 0, ret = 0;
    struct mblock_t *pmblock;
    struct reserved_t *preserved;

    mutex_acquire(&mblock_lock);
    /* check if mblock and reserved overlap*/
    i = 0;
    j = 0;
    for_each_mblock(pmblock) {
        mblock_start = pmblock->start;
        mblock_sz = pmblock->size;
        for_each_mblock_reserved(preserved) {
            reserved_start = preserved->start;
            reserved_sz = preserved->size;
            if ((mblock_sz != 0) && (((reserved_start >= mblock_start) &&
                    (reserved_start < (mblock_start + mblock_sz))) ||
                    ((reserved_start < mblock_start) && ((reserved_start + reserved_sz)
                            > mblock_start)))) {
                sanity_fail = 1;
                LTRACEF_LEVEL(CRITICAL, "failed i:%d j:%d %d:%d:%d:%d\n",
                              i, j, (reserved_start >= mblock_start)
                              , (reserved_start <= (mblock_start + mblock_sz))
                              , (reserved_start < mblock_start),
                              ((reserved_start + reserved_sz) > mblock_start));
            }
            j++;
        }
        i++;
    }

    if (sanity_fail) {
        LTRACEF_LEVEL(CRITICAL, "error\n");
        mblock_show_internal(0);
        ASSERT(0);
    }

    ret = kernel_dts_mblock_conflict_check(fdt);
    if (ret)
        panic("%s fatal error ASSERT\n", __func__);

    ret = reserved_memory_alloc_ranges_basic_check(fdt);
    if (ret)
        panic("%s fatal error ASSERT\n", __func__);

    mutex_release(&mblock_lock);
    LTRACEF_LEVEL(INFO, "%s:PASS\n", __func__);

    return 0;

}

static void mblock_fdt_reserved_append(void *fdt)
{
    int offset, ret = 0;
    int nodeoffset = 0;
    unsigned int i;
    char node_name[128] = {0};
    char compatible[MBLOCK_RESERVED_NAME_SIZE + 64] = {0};
    struct dt_dram_info reg_info;
    struct reserved_t *preserved;
    int device_nodeoffset = 0;
    uint32_t phandle = 0;
    int phandle_idx = 0;
    uint32_t fdt_phandle = 0;

    mutex_acquire(&mblock_lock);

    if (!fdt) {
        LTRACEF_LEVEL(CRITICAL, "fdt is NULL\n");
        ASSERT(0);
    }

    i = 1;
    for_each_mblock_reserved(preserved) {
        offset = fdt_path_offset(fdt, "/reserved-memory");
        if (offset < 0) {
            LTRACEF_LEVEL(CRITICAL, "Error: fdt_path_offset failed offset=0x%x\n", offset);
            ASSERT(0);
        }
        snprintf(node_name, sizeof(node_name), "mblock-%d-%s", i, preserved->name);
        nodeoffset = fdt_add_subnode(fdt, offset, node_name);
        if (nodeoffset < 0) {
            LTRACEF_LEVEL(CRITICAL, "Error: fdt_add_subnode failed offset=0x%x\n", nodeoffset);
            ASSERT(0);
        }

        if (strstr(preserved->name, "shared-dma-pool") != NULL)
            snprintf(compatible, sizeof(compatible), "shared-dma-pool");
        else
            snprintf(compatible, sizeof(compatible), "mediatek,%s", preserved->name);

        ret = fdt_setprop_string(fdt, nodeoffset, "compatible", compatible);
        if (ret) {
            LTRACEF_LEVEL(CRITICAL, "Error: fdt_setprop_string failed ret=0x%x\n", ret);
            ASSERT(0);
        }

        if (preserved->mapping == MBLOCK_NO_MAP) {
            ret = fdt_setprop(fdt, nodeoffset, "no-map", NULL, 0);
            if (ret) {
                LTRACEF_LEVEL(CRITICAL, "Error: fdt_setprop failed ret=0x%x\n", ret);
                ASSERT(0);
            }
        }

        if (preserved->mapping == MBLOCK_REUSABLE) {
            ret = fdt_setprop(fdt, nodeoffset, "reusable", NULL, 0);
            if (ret) {
                LTRACEF_LEVEL(CRITICAL, "Error: fdt_setprop failed ret=0x%x\n", ret);
                ASSERT(0);
            }
        }

        reg_info.start_hi = cpu_to_fdt32(preserved->start>>32);
        reg_info.start_lo = cpu_to_fdt32(preserved->start);
        reg_info.size_hi = cpu_to_fdt32((preserved->size)>>32);
        reg_info.size_lo = cpu_to_fdt32(preserved->size);

        ret = fdt_setprop(fdt, nodeoffset, "reg", &reg_info, sizeof(reg_info));
        if (ret) {
            LTRACEF_LEVEL(CRITICAL, "set reg prop failed ret=0x%x\n", ret);
            ASSERT(0);
        }

        // add phandle to reserved-memory and device node
        phandle_idx = phandle_index(preserved->name);
        if (phandle_idx != -1) {
            phandle = 0;
            ret = fdt_generate_phandle(fdt, &phandle); // generate a valid phandle
            if (ret != 0) {
                LTRACEF_LEVEL(CRITICAL, "Error: fdt_generate_phandle failed ret=%d\n", ret);
                panic("fdt generate phandle fail\n");
            }

            // set phandle for the reserved memory
            fdt_phandle = cpu_to_fdt32(phandle);
            ret = fdt_setprop(fdt, nodeoffset, "phandle", &fdt_phandle, sizeof(fdt_phandle));
            if (ret) {
                LTRACEF_LEVEL(CRITICAL, "Error: set phandle for reserved memory:%s failed\n",
                    preserved->name);
                mblock_show_internal(0);
                panic("set reserved memory failed\n");
            }
            LTRACEF_LEVEL(CRITICAL, "Set phandle reserved-memory:%s, phandle=%u\n",
                preserved->name, phandle);

            device_nodeoffset = fdt_path_offset(fdt, device_dts[phandle_idx].device_name);
            if (device_nodeoffset < 0) {
                LTRACEF_LEVEL(CRITICAL, "Error: fdt do NOT has device_node name:%s\n",
                    device_dts[phandle_idx].device_name);
                panic("You don't has device node\n");
            }

            // set memory-region with valid phandle to device node
            ret = fdt_setprop(fdt, device_nodeoffset, "memory-region", &fdt_phandle,
                sizeof(fdt_phandle));
            if (ret) {
                LTRACEF_LEVEL(CRITICAL, "Error: set device node memory-region: %s failed\n",
                    device_dts[phandle_idx].device_name);
                panic("set device memory-region failed\n");
            }
            LTRACEF_LEVEL(CRITICAL, "Set device: %s, memory-region=%u\n",
                device_dts[phandle_idx].device_name, phandle);

        }

        LTRACEF("mblock-reserved-memory is appended (0x%llx, 0x%llx)\n",
                      preserved->start, preserved->size);
        i++;
    }
    mblock_show_internal(0);
    LTRACEF_LEVEL(CRITICAL, "mblock-reserved-memory is appended successfully\n");
    mutex_release(&mblock_lock);
}

static int setup_mem_property_use_mblock_info(void *fdt)
{
    struct dt_dram_info *p;
    struct reserved_t *preserved;
    struct dt_dram_info mem_reg_property;
    int offset, ret;

    u64 mblock_low_start = 0, mblock_high_end = 0, reserved_low_start = 0
                           , reserved_high_end = 0, mem_start, mem_sz;

    mutex_acquire(&mblock_lock);
    mblock_low_start = internal_mblock_info->mblock[0].start;
    mblock_high_end = internal_mblock_info->mblock[*minfo_mblock_num - 1].start +
                      internal_mblock_info->mblock[*minfo_mblock_num - 1].size;
    /* reserved info is not sorted, we need to traverse to find out min*/
    /* and max value */
    if (*minfo_reserved_num) {
        reserved_low_start = internal_mblock_info->reserved[0].start;
        reserved_high_end = internal_mblock_info->reserved[*minfo_reserved_num].start
                            + internal_mblock_info->reserved[*minfo_reserved_num].size;

        for_each_mblock_reserved(preserved) {

            if (preserved->start < reserved_low_start)
                reserved_low_start = preserved->start;

            if ((preserved->start + preserved->size) > reserved_high_end)
                reserved_high_end = preserved->start + preserved->size;
        }
    }


    if (internal_mblock_info->reserved_num)
        mem_start = (reserved_low_start < mblock_low_start)?reserved_low_start:mblock_low_start;
    else
        mem_start = mblock_low_start;

    mem_sz = (reserved_high_end > mblock_high_end)
             ? (reserved_high_end - mem_start):(mblock_high_end - mem_start);
    p = &mem_reg_property;

    p->start_hi = cpu_to_fdt32(mem_start>>32);
    p->start_lo = cpu_to_fdt32(mem_start);
    p->size_hi = cpu_to_fdt32((mem_sz>>32));
    p->size_lo = cpu_to_fdt32(mem_sz);

    if (!fdt) {
        LTRACEF_LEVEL(CRITICAL, "fdt is NULL\n");
        ASSERT(0);
    }

    offset = fdt_path_offset(fdt, "/memory");
    if (offset < 0) {
        LTRACEF_LEVEL(CRITICAL, " path /memory failed\n");
        ASSERT(0);
    }

    ret = fdt_setprop(fdt, offset, "reg", p, sizeof(struct dt_dram_info));
    if (ret) {
        LTRACEF_LEVEL(CRITICAL, "setprop for reg failed ret=0x%x\n", ret);
        ASSERT(0);
    }

    mutex_release(&mblock_lock);
    LTRACEF_LEVEL(ALWAYS, " mem_start=0x%llx mem_sz=0x%llx\n", mem_start, mem_sz);
    LTRACEF_LEVEL(ALWAYS, " mem_reg_property.start_hi = 0x%08X\n", p->start_hi);
    LTRACEF_LEVEL(ALWAYS, " mem_reg_property.start_lo = 0x%08X\n", p->start_lo);
    LTRACEF_LEVEL(ALWAYS, " mem_reg_property.size_hi  = 0x%08X\n", p->size_hi);
    LTRACEF_LEVEL(ALWAYS, " mem_reg_property.size_lo  = 0x%08X\n", p->size_lo);

    return 0;
}

void mblock_fdt_append(void *fdt)
{
    char *ptr;
    int offset;
    int ret = 0;

    /*we set this flag to prevent any mblock operation after us*/
    mblock_init_done = 0;

    if (!fdt) {
        LTRACEF_LEVEL(CRITICAL, "fdt is NULL\n");
        ASSERT(0);
    }

    mblock_sanity_check(fdt);
    offset = fdt_path_offset(fdt, "/memory");
    if (offset < 0) {
        LTRACEF_LEVEL(CRITICAL, "%s:[%d] get path /memory failed\n", __func__, __LINE__);
        ret = offset;
        ASSERT(0);
    }

    ptr = (char *)internal_mblock_info;
    ret = fdt_setprop(fdt, offset, "mblock_info", ptr, sizeof(struct mblock_info_t));
    if (ret) {
        LTRACEF_LEVEL(CRITICAL, "%s:[%d] fdt_setprop failed, ret=%d\n", __func__, __LINE__, ret);
        ASSERT(0);
    }

    setup_mem_property_use_mblock_info(fdt);
    mblock_fdt_reserved_append(fdt);
}

u64 mblock_change_map_type(u64 addr, u64 size, u32 mapping, const char *name)
{
    u64 ret;
    u64 align = 0x1000;

    mutex_acquire(&mblock_lock);
    LTRACEF_LEVEL(CRITICAL, "Change the memory type: addr: 0x%llx, size: 0x%llx, map:%d\n",
                addr, size, mapping);

    ret = mblock_free_with_size(addr, size);
    if (ret != 0) {
        mblock_show_internal(0);
        mutex_release(&mblock_lock);
        panic("Error: %s: free failed\n", __func__);
    }

    ret = mblock_alloc_range_no_lock(size, align, 0, MBLOCK_NO_LIMIT,
        addr, mapping, name);
    if (ret != addr) {
        mblock_show_internal(0);
        mutex_release(&mblock_lock);
        panic("Error: %s: realloc failed\n", __func__);
    }

    mutex_release(&mblock_lock);

    return ret;
}

PL_BOOTTAGS_TO_BE_UPDATED(mblock, BOOT_TAG_MEM, &mblock_boot_tag);

static void mblock_pl_boottag_hook(struct boot_tag *tag)
{
    struct boot_tag_mem *mem = (struct boot_tag_mem *)(&tag->data);

    ASSERT((!(mem->mblock_info.mblock_magic != MBLOCK_MAGIC)
            /*current version is V3 but we backward compatible with V2*/
            && (mem->mblock_info.mblock_version <= (MBLOCK_VERSION))
            && !(tag->hdr.size != ALIGN((sizeof(struct boot_tag_header)
                                         + sizeof(struct boot_tag_mem)), 8))));
    memcpy(&mblock_boot_tag, mem, sizeof(struct boot_tag_mem));

    internal_mblock_info = &mblock_boot_tag.mblock_info;
    minfo_mblock_num = &internal_mblock_info->mblock_num;
    minfo_reserved_num = &internal_mblock_info->reserved_num;
    internal_orig_dram_info = &mblock_boot_tag.orig_dram_info;

    mblock_init_done = 1;
    device_dts_num = 0;
    LTRACEF_LEVEL(ALWAYS, "%s init done\n", __func__);
    mblock_merge();
    mblock_show();

}
PL_BOOTTAGS_INIT_HOOK(mblock_init, BOOT_TAG_MEM, mblock_pl_boottag_hook);
