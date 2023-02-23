/*
 * Copyright (c) 2020 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#include <arch/ops.h>
#include <assert.h>
#include <compiler.h>
#include <ctype.h>
#include <debug.h>
#include <err.h>
#include <kernel/vm.h>
#include <libfdt.h>
#include <load_image.h>
#include <malloc.h>
#include <mblock.h>
#include <partition_utils.h>
#include <stdlib.h>
#include <string.h>
#include <trace.h>

#define LOCAL_TRACE 0

/*The buffer layout in DDR is as follows:
 * The total is 45MBs.
 * [phyaddr offset]
 *  0x00000000  +---------------------------+  0
 *              |    Core#0 reset vector    |
 *  0x00100000  +---------------------------+  DSP_RST_VEC_SZ * 1
 *              |    Core#0 main program    |
 *  0x00400000  +---------------------------+  DSP_RST_VEC_SZ * 1 + DSP_PROG_SZ * 1
 *              |    Core#1 reset vector    |
 *  0x00500000  +---------------------------+  DSP_RST_VEC_SZ * 2 + DSP_PROG_SZ * 1
 *              |    Core#1 main program    |
 *  0x00800000  +---------------------------+  DSP_RST_VEC_SZ * 2 + DSP_PROG_SZ * 2
 *              |    Core#2 reset vector    |
 *  0x00900000  +---------------------------+  DSP_RST_VEC_SZ * 3 + DSP_PROG_SZ * 2
 *              |    Core#2 main program    |
 *  0x00C00000  +---------------------------+  DSP_RST_VEC_SZ * 3 + DSP_PROG_SZ * 3
 *              |         Algo area         |
 *  0x02950000  +---------------------------+  default IRAM_OFFSET
 *              |  main program#0 IRAM data |
 *  0x02980000  +---------------------------+
 *              |  main program#1 IRAM data |
 *  0x029B0000  +---------------------------+
 *              |  main program#2 IRAM data |
 *  0x029E0000  +---------------------------+
 *              |    Merged image header    |
 *  0x02A10000  +---------------------------+
 *              |          PRELOAD          |
 *              +---------------------------+
 *
 * the layout of main program IRAM data
 *
 *              +--------------------------+
 *              |  total iram segment num  |
 *              +--------------------------+
 *              |     offset to iram#0     |
 *              +--------------------------+
 *              |      dest of iram#0      |
 *              +--------------------------+
 *              |      size of iram#0      |
 *              +--------------------------+
 *              |     offset to iram#1     |
 *              +--------------------------+
 *              |      dest of iram#1      |
 *              +--------------------------+
 *              |      size of iram#1      |
 *              +--------------------------+
 *              |     offset to iram#2     |
 *              +--------------------------+
 *              |      dest of iram#2      |
 *              +--------------------------+
 *              |      size of iram#2      |
 *              +--------------------------+
 *              |        iram#0 data       |
 *              +--------------------------+
 *              |        iram#1 data       |
 *              +--------------------------+
 *              |        iram#2 data       |
 *              +--------------------------+
 *
 * num: number of segment for IRAM program
 * off: bin data offset from start of Main Prog. IRAM
 * dst: the dst address of bin data at IRAM
 * size: size of bin data
 * seg_data: bin data
 *
 *
 * layout of Preload
 *              +-----------------------+
 *              |   preload#0 program   |
 *              +-----------------------+
 *              |  preload#0 IRAM data  |
 *              +-----------------------+
 *              |   preload#1 program   |
 *              +-----------------------+
 *              |  preload#1 IRAM data  |
 *              +-----------------------+
 *              |          ...          |
 *              +-----------------------+
 *
 *
 */


/*****************************************************************************
 *                             Constants/Macros                              *
 *****************************************************************************/
#define MODULE_NAME "[APU_VPU] "
#define VERSION       (0x21007000)

#define ADDR_ALIGN     (0x00010000)
#define ADDR_MAX       (0xC0000000)
#define DEF_RSRV_SIZE  (0x02A10000) // default reserved ddr size for vpu
#define PROG_OFFSET    (0x00100000) // start address of main program (core0)

#define DSP_RST_VEC_SZ (0x00100000) // Reset vector for 1 VPU: 1MB
#define DSP_PROG_SZ    (0x00300000) // Main program for 1 VPU: 3MB
#define DEF_ALG_SZ     (0x01D50000) // default algo bank size for 3 VPUs

#define IRAM_SEG_SZ    (0x00010000) // max 64k for each iram region
#define DSP_BOOT_SZ    (DSP_RST_VEC_SZ + DSP_PROG_SZ)
#define SPARE_SZ       (0x00001000) // spare size for VPU

#define ADDR_MASK      (0xFFF00000)
#define MAX_MVA_SZ     (0x82600000 - 0x70000000) // Size of MVA address space in LSP

// 16 bytes alignment for algo bin and iram prog so that vpu core can use DMA to copy
#define ALIGN_16       (0x00000010)
#define ALIGN_32       (0x00000020)
#define ALIGN_4K       (0x00001000)
#define ALIGN_64K      (0x00010000)
#define ALIGN_2M       (0x00200000)

#define MAIN_SEG_MAX  (50)
#define ALG_NAME_SIZE (32)
#define HDR_STR_SIZE  (32)
#define VP6_MAGIC     (0x60)
#define PROP_LEN      (16)
#define MAX_DSP_NUM   (3)
#define NODE_CONT_LEN (3)
#define MAJOR_VER(x)  (x & 0xFF000000)

#define IRAM_MAIN_HDR_SIZE (4 + MAIN_SEG_MAX * sizeof(struct iram_desc))
#define MAIN_IRAM_DESC_HDR_SIZE   ALIGN(IRAM_MAIN_HDR_SIZE, ALIGN_16)

#define LOG_CRI(format, args...)  dprintf(CRITICAL, MODULE_NAME format, ##args)
#define LOG_INF(format, args...)  dprintf(INFO, MODULE_NAME format, ##args)

#define DT_ID            "id"
#define DT_IRAM_DAT      "iram-data"
#define DT_ALGO          "algo"
#define DT_COMP          "compatible"
#define DT_BIN           "bin-phy-addr"
#define DT_BIN_SZ        "bin-size"
#define DT_HEAD          "img-head"
#define DT_PRE_BIN       "pre-bin"

/*****************************************************************************
 *                             Type declarations                             *
 *****************************************************************************/
enum CORE_NUM {
    CORE0 = 0,
    CORE1,
    CORE2,
    CORE_MAX,
};

enum _VPU_CORE_ENUM_ {
    VPU_CORE_0 = (VP6_MAGIC | (1 << CORE0)), /* bitwise to represent core index */
    VPU_CORE_1 = (VP6_MAGIC | (1 << CORE1)),
    VPU_CORE_2 = (VP6_MAGIC | (1 << CORE2)),
    VPU_CORE_MAX = 0x03,
};

enum auth_cmd {
    AUTH_INIT,
    AUTH_DO,
};

enum dt_prop {
    PROP_INIT,
    PRE_BIN,
};

enum DSP_ADDR {
    RESET_VEC = 0,
    MAIN_PROG = 1,
    IRAM = 2,
};

enum pre_flag {
    EXE_SEG  = (1 << 0),
    ZERO_SEG = (1 << 1),
};

enum {
    TOTAL_DSP,
    TOTAL_ALG_SZ,
    RESERVE_VAR2,
    RESERVE_VAR3,
    RESERVE_TOTAL,
};

struct iram_desc {
    unsigned int offset;
    unsigned int addr;
    unsigned int size;
};

struct seg_info {
    u32 vpu_core; /* core index */
    u32 off;      /* offset */
    u32 pAddr;    /* destination */
    u32 mem_sz;   /* mem. size byte to occupied */
    u32 file_sz;  /* file size byte to copy */
};

struct addr_param {
    int is_preload;
    int core;
    int iram_segment;
    int iram_num;
    vaddr_t iram_hdr;
    vaddr_t mblock;
    u32 iram_off;
    u32 phys_addr;
    u32 mem_sz;
    u32 ihdr_size;
    u32 preload_base;
    u32 preload_sz;
};

struct alg_info {
    u32 vpu_core; /* core index */
    u32 off;      /* offset */
    u32 file_sz;  /* file size byte to copy */
    char name[ALG_NAME_SIZE];
};

struct pre_info {
    u32 vpu_core;
    u32 off;
    u32 pAddr;
    u32 mem_sz;
    u32 file_sz;
    u32 flag;
    u32 info; // already aligned (to 4k or 64k) at packing stage
    u32 reserve;
    char name[ALG_NAME_SIZE];
};

struct img_info {
    int total_num;
    int hdr_size;
    int img_max;
    int pre_cnt;
    u32 dsp_num;
    u32 total_extra_sz;
    u32 total_alg_sz;
    void *head;
    const char *name;
    void *part_va;
};

struct install_params {
    int img_max;
    vaddr_t mblock;
    vaddr_t alg_dst;
    vaddr_t pre_dst;
    vaddr_t iram_off;
    const char *name;
    int  *overlap;
    struct img_header *part_hdr;
};

struct prop_params {
    u32 dsp_num;
    vaddr_t mblock;
    u32 aligned_size;
    vaddr_t head_offset;
    vaddr_t preload_offset;
    vaddr_t iram_off;
    vaddr_t alg_offset;
    u32 total_alg_sz;
};

struct img_header {
    int version;
    int build_date;
    int hdr_str[HDR_STR_SIZE / 4];
    int hdr_size;
    int img_size;
    int mem_size; // total preload segments size

    int segment_num;
    int seg_info; // offset
    int seg_size;

    int pre_num;
    int pre_info; // offset
    int pre_size;

    int alg_num;
    int alg_info; // offset

    int reserved[RESERVE_TOTAL];
};

/*****************************************************************************
 *                             Global variables                              *
 *****************************************************************************/
struct vpu_part_info {
    const char *name;
    off_t size;
    u32 m4u_addr[3];
};

static struct vpu_part_info vpu_part[3] = {
    {
        .name = "cam_vpu1",
        .m4u_addr = {0x7DA00000, 0x7DB00000, 0x7FF00000},
    },
    {
        .name = "cam_vpu2",
        .m4u_addr = {0x7E300000, 0x7E400000, 0x7FF00000},
    },
    {
        .name = "cam_vpu3",
        .m4u_addr = {0x7EC00000, 0x7ED00000, 0x7FF00000},
    },
};

/*****************************************************************************
 *                             Functions/Interfaces                          *
 *****************************************************************************/
static inline u32 m4u_addr(unsigned int core, unsigned int block)
{
    return vpu_part[core].m4u_addr[block];
}

static int is_dsp_addr_ofs(unsigned int addr, unsigned int core, unsigned int block,
    unsigned int *offset)
{
    unsigned int base = m4u_addr(core, block);

    if ((addr & ADDR_MASK) == base) {
        if (offset)
            *offset = addr - base;
        return 1;
    }
    return 0;
}

#define is_dsp_addr(a, c, b)  is_dsp_addr_ofs(a, c, b, NULL)
#define __uintptr(a)  ((void *)(vaddr_t)(a))

static inline int ascen(const int a, const int b)
{
    return a == b ? 0 : a < b ? -1 : 1;
}

static inline int descen(const int a, const int b)
{
    return -ascen(a, b);
}

static inline void swap(int *const a, int *const b)
{
    int tmp;

    tmp = *a;
    *a = *b;
    *b = tmp;
}

int check_overlap(int *addr, int len)
{
    int cnt = 0, i, cmp;
    int (*compare)(const int a, const int b);

    for (i = 0; i < len; ++i) {
        cmp = ascen(addr[i - 1], addr[i]);
        if (cmp == 0)
            return true;

        cnt += cmp;
    }

    compare = cnt <= 0 ? ascen : descen;

    for (i = 1; i < len; i += cmp == -1 || i == 1 ? 1 : -1) {
        cmp = compare(addr[i - 1], addr[i]);
        if (cmp == 0)
            return true;
        if (cmp == 1)
            swap(&addr[i], &addr[i - 1]);
    }
    return false;
}

static void vpu_prop(void *t, int offset, const char *type, vaddr_t prop)
{
    vaddr_t value;
    int ret = 0;

    value = cpu_to_fdt32(prop);
    ret = fdt_setprop(t, offset, type, (char *)&value, sizeof(u32));
    if (ret < 0)
        LOG_CRI("prop fail\n");
}

static void vpu_prop_array(void *t, int offset, const char *type, void *prop)
{
    int ret = fdt_setprop(t, offset, type, (char *)prop,
        sizeof(fdt32_t) * NODE_CONT_LEN);

    if (ret < 0)
        LOG_CRI("prop array fail\n");
}

static int vpu_prop_subnode(void *fdt, int parent, const char *name)
{
    int offset;

    offset = fdt_subnode_offset(fdt, parent, name);
    if (offset < 0)
        offset = fdt_add_subnode(fdt, parent, name);
    if (offset < 0)
        LOG_CRI("fail to add subnode to fdt: %s\n", name);

    return offset;
}

int vpu_prop_off(void *t, int core)
{
    int off;
    char name[16];

    off = fdt_path_offset(t, "/soc");
    if (off < 0) {
        off = fdt_path_offset(t, "/");
        if (off < 0)
            return ERR_NOT_FOUND;
    }

    if (snprintf(name, 16, "vpu_core%d", core) < 0)
        return ERR_NOT_FOUND;

    off = fdt_subnode_offset(t, off, name);

    return off;
}

static int set_property(void *fdt, struct prop_params *prop)
{
    unsigned int i = 0, j, dsp_num = 0;
    int offset;
    fdt32_t iram_prop[3][NODE_CONT_LEN] = { {0}, {0}, {0} };
    fdt32_t alg_prop[NODE_CONT_LEN] = {0};
    u32 vpu_iram_data[3][NODE_CONT_LEN] = {
        {0x00000000, IRAM_SEG_SZ, 0x02950000},
        {0x00000000, IRAM_SEG_SZ, 0x02950000 + IRAM_SEG_SZ},
        {0x00000000, IRAM_SEG_SZ, 0x02950000 + (IRAM_SEG_SZ * 2)}
    };
    u32 vpu_alg[NODE_CONT_LEN] = {
        0x00000000, DEF_ALG_SZ, (DSP_BOOT_SZ * 3)};

    dsp_num = prop->dsp_num;
    offset = vpu_prop_off(fdt, 0);
    if (offset < 0)
        return offset;

    vpu_alg[1] = prop->total_alg_sz;
    vpu_alg[2] = prop->alg_offset;

    for (i = 0; i < NODE_CONT_LEN; i++)
        alg_prop[i] = cpu_to_fdt32(vpu_alg[i]);

    for (i = 0; i < dsp_num; i++) {
        vpu_iram_data[i][2] =
        prop->alg_offset + prop->total_alg_sz + (i * IRAM_SEG_SZ);

        for (j = 0; j < NODE_CONT_LEN; j++)
            iram_prop[i][j] = cpu_to_fdt32(vpu_iram_data[i][j]);
    }

    vpu_prop(fdt, offset, DT_BIN, prop->mblock);
    vpu_prop(fdt, offset, DT_BIN_SZ, prop->aligned_size);
    vpu_prop(fdt, offset, DT_HEAD, prop->head_offset);
    vpu_prop(fdt, offset, DT_PRE_BIN, prop->preload_offset);

    vpu_prop_array(fdt, offset, DT_ALGO, (char *)&alg_prop);

    for (i = 0; i < dsp_num; i++) {
        offset = vpu_prop_off(fdt, i);
        if (offset < 0)
            continue;

        vpu_prop_array(fdt, offset, DT_IRAM_DAT,
            (char *)&iram_prop[i][0]);
    }

    return NO_ERROR;
}

static vaddr_t get_addr(struct addr_param *param)
{
    unsigned int offset = 0;
    u32 addr = param->phys_addr;
    vaddr_t mblock = param->mblock;
    u32 *iram_num = NULL;
    struct iram_desc *desc = NULL;

    static u32 iram1_seg = MAIN_IRAM_DESC_HDR_SIZE;
    static u32 iram2_seg = MAIN_IRAM_DESC_HDR_SIZE;
    static u32 iram3_seg = MAIN_IRAM_DESC_HDR_SIZE;
    static u32 pre_seg;

    if (is_dsp_addr_ofs(addr, CORE0, MAIN_PROG, &offset)) {
        mblock += DSP_RST_VEC_SZ;
    } else if (is_dsp_addr_ofs(addr, CORE1, MAIN_PROG, &offset)) {
        mblock += (DSP_BOOT_SZ + DSP_RST_VEC_SZ);
    } else if (is_dsp_addr_ofs(addr, CORE2, MAIN_PROG, &offset)) {
        mblock += ((DSP_BOOT_SZ << 1) + DSP_RST_VEC_SZ);
    } else if (is_dsp_addr_ofs(addr, CORE0, RESET_VEC, &offset)) {
        /* mblock offset at 0 */
    } else if (is_dsp_addr_ofs(addr, CORE1, RESET_VEC, &offset)) {
        mblock += DSP_BOOT_SZ;
    } else if (is_dsp_addr_ofs(addr, CORE2, RESET_VEC, &offset)) {
        mblock += (DSP_BOOT_SZ << 1);
    } else if (is_dsp_addr(addr, CORE0, IRAM) && !param->is_preload) {
        param->iram_segment = 1;

        switch (param->core) {
        case VPU_CORE_0:
            offset = iram1_seg;
            mblock += param->iram_off;
            iram_num  = __uintptr(mblock);
            iram1_seg += ALIGN(param->mem_sz, ALIGN_16);
            break;
        case VPU_CORE_1:
            offset = iram2_seg;
            mblock += (param->iram_off + IRAM_SEG_SZ);
            iram_num  = __uintptr(mblock);
            iram2_seg += ALIGN(param->mem_sz, ALIGN_16);
            break;
        case VPU_CORE_2:
            offset = iram3_seg;
            mblock += param->iram_off + (IRAM_SEG_SZ << 1);
            iram_num  = __uintptr(mblock);
            iram3_seg += ALIGN(param->mem_sz, ALIGN_16);
            break;
        default:
            LOG_CRI("invalid segment core 0x%x\n", param->core);
            return 0;
        }

        param->iram_num = (*iram_num);

        //update iram descriptor
        desc = __uintptr(mblock + sizeof(int));
        desc[param->iram_num].offset = offset;
        desc[param->iram_num].addr = addr;
        desc[param->iram_num].size = param->mem_sz;
        *iram_num = (param->iram_num + 1);
    } else if (param->is_preload) {
        if (is_dsp_addr(addr, CORE0, IRAM)) {
            param->iram_segment = 1;
            mblock += param->iram_off;
            param->iram_off += param->mem_sz;
        } else {
            param->iram_segment = 0;

            if (addr == 0xFFFFFFFF) {
                pre_seg += param->preload_sz;
                mblock += pre_seg;
                param->iram_off = pre_seg + param->ihdr_size;
                pre_seg += param->mem_sz;
            } else {
                mblock += (pre_seg +
                      (addr - param->preload_base));
            }
        }

        if (param->iram_hdr && param->iram_segment) {
            iram_num  = __uintptr(param->iram_hdr);
            param->iram_num = (*iram_num);
            desc = (void *)(param->iram_hdr + sizeof(int));
        }
    } else {
        LOG_CRI("invalid segment addr 0x%x\n", addr);
        return 0;
    }

    return (mblock + offset);
}

static int install_main(struct install_params *install)
{
    struct img_header *part_hdr = install->part_hdr;
    struct addr_param param;
    int j, ret = 0;
    int offset = 0, read_size = 0, zero = 0, info = part_hdr->seg_info;
    vaddr_t dst = 0;
    struct seg_info *seg = (void *)((vaddr_t)part_hdr + info);

    memset(&param, 0, sizeof(struct addr_param));

    for (j = 0; j < part_hdr->segment_num; j++) {
        offset = seg->off + part_hdr->hdr_size;
        read_size = seg->file_sz;

        param.core = seg->vpu_core;
        param.phys_addr = seg->pAddr;
        param.mem_sz = seg->mem_sz;
        param.mblock = install->mblock;
        param.iram_off = install->iram_off;
        param.iram_segment = 0;

        dst = get_addr(&param);
        if (!dst) {
            ret = ERR_NOT_VALID;
            break;
        }

        if (param.iram_segment) {
            LTRACEF("seg[%2d] [0x%05x:0x%05x] -> [0x%x/0x%lx]: iram[0x%x:%d]\n",
                j, offset, read_size, seg->pAddr, dst,
                seg->vpu_core, param.iram_num);
        } else {
            LTRACEF("seg[%2d] [0x%05x:0x%05x] -> [0x%x/0x%lx]\n",
                j, offset, read_size, seg->pAddr, dst);
        }

        if (seg->file_sz > 0) {
            if (offset + read_size > install->img_max) {
                LOG_CRI("%s partition offset error, offset 0x%x, read_size 0x%x\n",
                    install->name, offset, read_size);
                ret = ERR_NOT_VALID;
                break;
            } else {
                memcpy((void *)dst,
                    (void *)((u8 *)part_hdr + offset),
                    read_size);
            }
        }

        zero = seg->mem_sz - seg->file_sz;

        if (zero) {
            dst += seg->file_sz;
            LTRACEF("%-7s [0x%lx:0x%x]\n", "zero", dst, zero);
            memset((void *)dst, 0, zero);
        }
        seg++;
    }

    return ret;
}

static int install_preload(struct install_params *install)
{
    struct img_header *part_hdr = install->part_hdr;
    struct addr_param param;
    int j, ret = 0, idx = install->overlap[0];
    int offset = 0, read_size = 0, zero = 0, info = part_hdr->pre_info;
    vaddr_t dst = 0;
    struct pre_info *pre = (void *)((vaddr_t)part_hdr + info);

    memset(&param, 0, sizeof(struct addr_param));

    for (j = 0; j < part_hdr->pre_num; j++) {
        offset = pre->off + part_hdr->hdr_size + part_hdr->seg_size;
        read_size = (pre->pAddr == 0xFFFFFFFF) ?
                 pre->mem_sz : pre->file_sz;

        param.is_preload = true;
        param.core   = pre->vpu_core;
        param.phys_addr = pre->pAddr;
        param.mem_sz = pre->file_sz;
        param.mblock = install->pre_dst;
        param.iram_segment = 0;
        param.iram_hdr = 0;
        param.ihdr_size = (pre->pAddr == 0xFFFFFFFF) ?
            pre->mem_sz : 0;

        if ((pre->flag & EXE_SEG) == EXE_SEG) {
            param.phys_addr = pre->pAddr & 0xFFF00000;
            param.preload_base = param.phys_addr;
            param.preload_sz = pre->info;

            install->overlap[idx] = pre->pAddr & 0xFFF00000;
            install->overlap[0] = ++idx;
        }
        dst = get_addr(&param);
        if (!dst) {
            ret = ERR_NOT_VALID;
            break;
        }

        if (param.iram_segment) {
            LTRACEF("pre[%2d] [0x%05x:0x%06x] -> [0x%x/0x%lx]: iram[0x%x:%d]\n",
                 j, offset, read_size, pre->pAddr, dst,
                 pre->vpu_core, param.iram_num);
        } else {
            if (pre->pAddr == 0xFFFFFFFF) {
                param.iram_hdr = dst;
                LTRACEF("pre[%2d] [0x%05x:0x%06x] -> [0x%x/0x%lx]: iram[0x%x] head\n",
                     j, offset, pre->file_sz, pre->pAddr,
                     dst, pre->vpu_core);
            } else {
                param.iram_hdr = 0;
                LTRACEF("pre[%2d] [0x%05x:0x%06x] -> [0x%x/0x%lx]\n",
                    j, offset, read_size, pre->pAddr, dst);
            }
        }

        if (pre->file_sz > 0) {
            if (offset + read_size > install->img_max) {
                LOG_CRI("%s partition offset error, offset 0x%x, read_size 0x%x\n",
                    install->name, offset, read_size);
                ret = ERR_NOT_VALID;
                break;
            } else {
                memcpy((void *)dst,
                    (void *)((u8 *)part_hdr + offset),
                    read_size);
            }
        }

        zero = pre->mem_sz - pre->file_sz;
        pre->off = dst - install->mblock;

        if (zero && pre->pAddr != 0xFFFFFFFF) {
            dst += pre->file_sz;
            LTRACEF("%-7s [0x%lx:0x%x]\n", "zero", dst, zero);
            memset((void *)dst, 0, zero);
        }
        pre++;
    }

    return ret;
}

static int install_algo(struct install_params *install)
{
    struct img_header *part_hdr = install->part_hdr;
    int j, ret = 0;
    int offset = 0, read_size = 0, info = part_hdr->alg_info;
    vaddr_t dst = install->alg_dst;
    struct alg_info *alg = (void *)((vaddr_t)part_hdr + info);

    for (j = 0; j < part_hdr->alg_num; j++) {
        int new_offset = (int)(dst - install->mblock);

        offset = alg->off + part_hdr->hdr_size;
        read_size = alg->file_sz;

        LTRACEF("alg[%2d] [0x%06x:0x%06x] -> [0x%lx] => 0x%x\n",
            j, offset, read_size, dst, new_offset);

        if (offset + read_size > install->img_max) {
            LOG_CRI("%s partition offset error, offset 0x%x, read_size 0x%x\n",
                install->name, offset, read_size);
            ret = ERR_NOT_VALID;
            break;
        } else {
            memcpy((void *)dst,
                (void *)((u8 *)part_hdr + offset),
                read_size);
        }

        // change alg offset in partition header for driver to get real offset
        alg->off = new_offset;
        dst += ALIGN(alg->file_sz, ALIGN_16);
        alg++;
    }

    install->alg_dst = dst;
    return ret;
}

#define for_each_vpu_part(i, p) \
    for (i = 0, p = &vpu_part[0]; i < countof(vpu_part); i++, p++)

static off_t part_size_init(void)
{
    unsigned int i;
    int j;
    struct vpu_part_info *p;
    bdev_t *dev;
    off_t total_sz = 0;
    int name_list_cnt = 0;
    char *name_list[NUM_PARTITIONS] = {0};
    const char *label;

    partition_get_name_list((char **)&name_list, &name_list_cnt);

    for_each_vpu_part(i, p) {
        label = NULL;
        p->size = 0;
        for (j = 0; j < name_list_cnt; j++) {
            if (!strncmp(name_list[j], p->name, strlen(p->name))) {
                label = name_list[j];
                break;
            }
        }
        if (!label) {
            LOG_INF("unable to locate %s\n", p->name);
            continue;
        }
        dev = bio_open_by_label(label);
        if (dev) {
            p->size = dev->total_size;
            bio_close(dev);
        }
        LOG_INF("%s: size: %llx\n", p->name, p->size);
        total_sz += p->size;
    }
    return total_sz;
}

/* read vpu images
 * phase 0: Get size information from info[] to allocate ap_bin
 * phase 1: Parse partition data into ap_bin
 */
static void *read_img(void *part, size_t buffer_len)
{
    unsigned int i;
    int len = 0, pre_cnt = 0, alg_sum = 0, dsp_num = 0;
    int total_num = countof(vpu_part);
    unsigned long bin_size = 0, hdr_total = 0;
    struct img_header *head = NULL;
    struct img_info *info = calloc(total_num, sizeof(struct img_info));
    unsigned int read = 0;
    struct vpu_part_info *p;

    if (!info) {
        LOG_CRI("calloc fail\n");
        return NULL;
    }

    for_each_vpu_part(i, p) {
        info[i].total_num = total_num;

        len = load_partition(p->name, p->name, part, buffer_len);
        if (len < 0) {
            LOG_CRI("load %s fail: %d\n", p->name, len);
            goto exit;
        }

        read += len;
        if (read > p->size) {
            LOG_CRI("load %s: %d, read: %d, exceeds limit: %lld\n",
                p->name, len, read, p->size);
            goto exit;
        }

        head = part;
        head->hdr_str[HDR_STR_SIZE/4 - 1] = 0;

        if (head->img_size > p->size) {
            LOG_CRI("img size(0x%x) exceeds!\n", head->img_size);
            goto exit;
        }

        if (MAJOR_VER(VERSION) != MAJOR_VER(head->version)) {
            LOG_CRI("version mismatch (%x/[%d]%x)\n",
                VERSION, i, head->version);
            goto exit;
        }

        info[i].head = head;
        info[i].name = p->name;
        info[i].img_max = p->size;
        info[i].part_va = NULL;

        bin_size += head->mem_size;
        LTRACEF("total preload mem size: 0x%x\n", head->mem_size);
        hdr_total += head->hdr_size;
        pre_cnt += head->pre_num;
        part += p->size;

        // cam_vpu1.img carries only main program & preload and does not carry normal libraries
        if (i)
            alg_sum += head->reserved[TOTAL_ALG_SZ];
        else
            dsp_num = head->reserved[TOTAL_DSP];
    }
    info[0].hdr_size = ALIGN(hdr_total, ALIGN_32);
    info[0].pre_cnt = pre_cnt;
    info[0].dsp_num = dsp_num ? dsp_num : 3;
    info[0].total_alg_sz = ALIGN(alg_sum, ALIGN_64K);

    bin_size = ALIGN(bin_size, ALIGN_4K);
    info[0].total_extra_sz = bin_size;
    info[0].part_va = part;

    return info;

exit:
    if (info)
        free(info);

    return NULL;
}

static void free_img(struct img_info *info)
{
    if (!info)
        return;

    free(info);
}

static inline void clear_iram_data(vaddr_t addr)
{
    u32 *data = __uintptr(addr);

    *data = 0;
}

status_t apu_load_vpu(void *fdt)
{
    status_t ret = NO_ERROR;
    int i, num;
    int *overlap = NULL;
    vaddr_t head_dst, iram_off = 0, offset = 0, pre_dst = 0;
    vaddr_t alg_offset = 0;
    u32 extra_size = 0, data_size = 0, aligned_size = 0;
    struct img_info *img = NULL;
    struct img_header *head = NULL;
    struct install_params params;
    struct prop_params prop;

    void *mblock_va_ptr = NULL;
    vaddr_t mblock_va = 0;
    vaddr_t mblock_pa = 0;
    off_t mblock_sz;
    off_t total_part_sz;

    if (!fdt)
        return ERR_NOT_VALID;

    if (vpu_prop_off(fdt, 0) < 0) {
        LOG_INF("VPU is not supported on this platform\n");
        return NO_ERROR;
    }

    /* Reserve mblock according to the sum of
     * 1. Total size of cam_vpu partitions - the maximum size of read buffers
     * 2. MVA address range (MAX_MVA_SZ) - the maximum size of VPU firmware
     */
    total_part_sz = part_size_init();
    LOG_INF("vpu part total size: %llx\n", total_part_sz);
    mblock_sz = total_part_sz + MAX_MVA_SZ;

    mblock_pa = mblock_alloc(mblock_sz, ADDR_ALIGN, ADDR_MAX,
        0, 0, "mb-vpu");

    if (!mblock_pa) {
        LOG_CRI("mblock allocation failed\n");
        ret = ERR_NO_MEMORY;
        goto exit;
    }

    ret = vmm_alloc_physical(vmm_get_kernel_aspace(),
                        "vm-vpu",
                        mblock_sz,
                        &mblock_va_ptr,
                        PAGE_SIZE_SHIFT,
                        (paddr_t)mblock_pa,
                        0,
                        ARCH_MMU_FLAG_CACHED);

    if (ret != NO_ERROR) {
        LOG_CRI("vmm_alloc_physical failed\n");
        goto err_vmm_alloc;
    }

    /* Read cam vpu partitions into the bottom of the reserved mblock */
    img = read_img(((char *)mblock_va_ptr) + MAX_MVA_SZ, total_part_sz);
    if (!img) {
        LOG_CRI("Phase 0 read error\n");
        ret = ERR_NOT_VALID;
        goto exit;
    }

    overlap = calloc((img->pre_cnt + 1), sizeof(int));
    if (!overlap) {
        LOG_CRI("vpu memory allocation failed.\n");
        ret = ERR_NO_MEMORY;
        goto exit;
    }

    extra_size = img->total_extra_sz;
    LOG_INF("extra_size: 0x%x\n", extra_size);

    data_size = (DSP_BOOT_SZ + IRAM_SEG_SZ) * img->dsp_num +
        img->total_alg_sz + img->hdr_size + extra_size + SPARE_SZ;

    aligned_size = ALIGN(data_size, ALIGN_64K);

    if (IS_ALIGNED(aligned_size, ALIGN_2M))
        aligned_size += ALIGN_64K;

    memset(mblock_va_ptr, 0, aligned_size);
    mblock_va = (vaddr_t)mblock_va_ptr;

    alg_offset = img->dsp_num * DSP_BOOT_SZ;
    iram_off = alg_offset + img->total_alg_sz;
    offset = data_size - img->hdr_size - extra_size;
    head_dst = mblock_va + offset;
    pre_dst = ALIGN(mblock_va + data_size, ALIGN_64K) - extra_size;

    LOG_INF("lk ver: %x\n", VERSION);
    LOG_INF("mblk:[0x%lx:0x%x] | head:0x%lx | alg: 0x%lx | pre: 0x%lx\n",
        mblock_va, data_size, head_dst, mblock_va + alg_offset,
        pre_dst);

    clear_iram_data(mblock_va + iram_off);
    clear_iram_data(mblock_va + iram_off + IRAM_SEG_SZ);
    clear_iram_data(mblock_va + iram_off + (IRAM_SEG_SZ << 1));

    num = img->total_num;

    memset(&params, 0, sizeof(struct install_params));
    overlap[0] = 1;
    params.mblock = mblock_va;
    params.iram_off = iram_off;
    params.pre_dst = pre_dst;
    params.alg_dst = mblock_va + alg_offset;
    params.overlap = overlap;

    memset(&prop, 0, sizeof(struct prop_params));
    prop.dsp_num = img->dsp_num;
    prop.iram_off = iram_off;
    prop.alg_offset = alg_offset;
    prop.total_alg_sz = img->total_alg_sz;
    prop.mblock = mblock_pa;
    prop.aligned_size = aligned_size;
    prop.head_offset = offset;
    prop.preload_offset = pre_dst - mblock_va;

    ret = set_property(fdt, &prop);
    if (ret < 0)
        goto exit;

    /* Install vpu firmware into the top of the reserved mblock */
    for (i = 0 ; i < num ; i++) {
        params.img_max = img[i].img_max;
        params.name = img[i].name;
        head = params.part_hdr = img[i].head;

        LOG_INF("%s| v:%x str:%s sz:0x%x/0x%x s:%d a:%d v:%d\n",
            params.name, head->version, (char *)head->hdr_str,
            head->hdr_size, head->mem_size, head->segment_num,
            head->alg_num, head->pre_num);

        ret = install_main(&params);
        if (ret)
            goto exit;

        ret = install_preload(&params);
        if (ret)
            goto exit;

        ret = install_algo(&params);
        if (ret)
            goto exit;

        LTRACEF("copy %s header(0x%X) to 0x%lx\n",
            params.name, head->hdr_size, head_dst);
        memcpy(__uintptr(head_dst), head, head->hdr_size);
        head_dst += head->hdr_size;
    }

    if (overlap[0] - 1) {
        if (check_overlap(&overlap[1], overlap[0] - 1))
            LOG_CRI("preload overlap\n");
    }

exit:
    if (mblock_va)
        vmm_free_region(vmm_get_kernel_aspace(), mblock_va);

    /* Shrink allocated mblock to the actual size of VPU firmware */
    if (mblock_pa && aligned_size)
        mblock_resize(mblock_pa, mblock_sz, aligned_size);

    free_img(img);

    if (overlap)
        free(overlap);

    return ret;

err_vmm_alloc:
    if (mblock_pa)
        mblock_free(mblock_pa);

    return ret;
}

