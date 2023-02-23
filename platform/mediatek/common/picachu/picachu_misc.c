/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */
/*****************************************************************************
 *
 *****************************************************************************/
#ifdef LK_AS_BL2_EXT
#include <arch/ops.h>
#include <platform/picachu_def.h>
#include <platform/picachu_reg.h>
#else
#include "picachu_def.h"
#include "picachu_reg.h"
#endif
#include "picachu_log.h"
#include "picachu_lut.h"
#include "picachu_para.h"


/*****************************************************************************
 *
 *****************************************************************************/
const char *picachu_partition_name = "pi_img";


/*****************************************************************************
 *
 *****************************************************************************/
struct dvfs_vf_tbl {
    unsigned short pi_freq_tbl[NR_FREQ];
    unsigned char pi_volt_tbl[NR_FREQ];
    unsigned char pi_vf_num;
};


/*****************************************************************************
 *
 *****************************************************************************/
// Pass voltage/frequency tables of all points to EEM.
static struct dvfs_vf_tbl *g_vf_tbl_det;


/*****************************************************************************
 *
 *****************************************************************************/
static unsigned int get_deb_id_via_cluster_id(unsigned int cluster_id)
{
    switch (cluster_id) {
#if HAS_CLUSTER_L
    case kNPTP_CPU_L:
        return MT_CPU_DVFS_L;
#endif

#if HAS_CLUSTER_CCI_DSU
    case kNPTP_CPU_D:
        return MT_CPU_DVFS_CCI;
#endif

#if HAS_CLUSTER_B
    case kNPTP_CPU_B:
        return MT_CPU_DVFS_B;
#endif


#if HAS_CLUSTER_M
    case kNPTP_CPU_M:
        return MT_CPU_DVFS_M;
#endif

    default:
        return 0xDEADDEAD;
    }

}


static void dump_vf_table(void)
{
    unsigned int deb_id = 0;
    unsigned int point_id = 0;

    if (!g_vf_tbl_det)
        return;

    for (deb_id = 0; deb_id < NR_EEMSN_DET; deb_id++) {
        for (point_id = 0; point_id < NR_FREQ    ; point_id++) {
            if (point_id < g_vf_tbl_det[deb_id].pi_vf_num) {
                PICACHU_INFO("vf_tbl_det[%d][%d] freq = %d\n",
                deb_id, point_id,
                g_vf_tbl_det[deb_id].pi_freq_tbl[point_id]);
                PICACHU_INFO("vf_tbl_det[%d][%d] volt = %d\n",
                deb_id, point_id,
                g_vf_tbl_det[deb_id].pi_volt_tbl[point_id]);
            }
        }
    }
}


/*****************************************************************************
 *
 *****************************************************************************/
int picachu_check_bat_low_voltage(void)
{
    return 0;
}


int picachu_voltage_calibration_via_doe(void)
{
    return 0;
}


int picachu_check_doe(char *doe, const char *str)
{
    return 0;
}


#ifdef MTK_AGING_FLAVOR_LOAD
int is_aging_flavor_load(void)
{
    return 1;
}
#else
int is_aging_flavor_load(void)
{
    return 0;
}
#endif


#ifdef MTK_SLT_FLAVOR_LOAD
int is_slt_load(void)
{
    return 1;
}
#else
int is_slt_load(void)
{
    return 0;
}
#endif


void lut_update_vf_table(
    const unsigned int cluster_id,
    const unsigned int point_id,
    const unsigned int freq,
    const unsigned int volt)
{
    unsigned int deb_id = 0;

    if (!g_vf_tbl_det)
        return;

    deb_id = get_deb_id_via_cluster_id(cluster_id);
    if (deb_id == 0xDEADDEAD)
        return;

    if (deb_id >= NR_EEMSN_DET || point_id >= NR_FREQ)
        return;

    if (g_vf_tbl_det[deb_id].pi_vf_num < point_id + 1)
        g_vf_tbl_det[deb_id].pi_vf_num = point_id + 1;

    g_vf_tbl_det[deb_id].pi_freq_tbl[point_id] = freq;
    g_vf_tbl_det[deb_id].pi_volt_tbl[point_id] = volt;

    return;
}

#ifdef SUPPORT_DVCS_LUT
static void dump_dvcs_table(void)
{
    unsigned int *mincode_tbl = get_dvcs_mincode_tbl();
    int i;

    for (i = 0; i < MAX_MINCODE_TBL_SIZE; i++)
        PICACHU_INFO("mincode_tbl[%d]= 0x%X\n", i, mincode_tbl[i]);

}
#endif


/*****************************************************************************
 *            TARGET_BUILD_VARIANT_PRELOADER
 *****************************************************************************/
#ifdef TARGET_BUILD_VARIANT_PRELOADER
int pi_img_suffix(void)
{
    //    0: no suffix; 1: "_a" suffix; 2: "_b" suffix; -1: get_suffix() error
#if defined(MTK_AB_OTA_UPDATER)
    const char *ab_suffix = get_suffix();

    if (ab_suffix == NULL)
        return -1;

    if (!memcmp(ab_suffix, BOOTCTRL_SUFFIX_A, strlen(BOOTCTRL_SUFFIX_A)))
        return 1;
    else
        return 2;
#else
    return 0;
#endif
}
#endif


/*****************************************************************************
 *            LK_AS_BL2_EXT
 *****************************************************************************/
#ifdef LK_AS_BL2_EXT
#if KRAKEN_SUPPOT
#include "kraken_ld.h"
#include "utstack_static.h"

//#define KRAKEN_DEBUG_ENABLE

STACK_INIT();

struct kraken_header {
    unsigned int magic;
    unsigned int size;
    unsigned int is_safe_efuse : 1,
                 is_aging_load : 1,
                 is_slt_load   : 1,
                 is_mc50_load  : 1,
                 is_lut_load   : 1,
                 unused        : 27;
};

struct __attribute__((__packed__)) kraken_section_table_entry_t {
    uint32_t cond_expr_pos;
    uint32_t sec_ptr;
    uint32_t sec_size;
};


static unsigned char *g_pi_part_buff; // The buffer to load pi_img
static struct kraken_header g_header;

static unsigned int __creat_mask(int a, int b)
{
    unsigned int r = 0;
    char i;

    for (i = a; i <= b; i++)
        r |= 1 << i;

    return r;
}

static int __eval_op(
    enum OPCODE op,
    const uint32_t *var,
    struct kraken_session_t *kd,
    int pos)
{
    unsigned int first = 0;
    unsigned int second = 0;
    unsigned int result = 0;

    PICACHU_DEBUG("[interpreter] op:%x, pos:%d\n", op, pos);
    if (op == K_INT || op == K_UINT) {
        STACK_PUSH(stack, *var);
        PICACHU_DEBUG("[interpreter] op:%s, value:%d(0x%x)\n", "INT", *var, *var);
        return 1;
    }

    if (op == K_FUNC_ISAGING) {
#ifdef MTK_AGING_FLAVOR_LOAD
        STACK_PUSH(stack, 1);
#else
        STACK_PUSH(stack, 0);
#endif
        return 1;
    }

    if (op == K_FUNC_ISSLT) {
#ifdef MTK_SLT_FLAVOR_LOAD
        STACK_PUSH(stack, 1);
#else
        STACK_PUSH(stack, 0);
#endif
        return 1;
    }

    if (op == K_FUNC_ISMC50) {
#ifdef MTK_MCL50_FLAVOR_LOAD
        STACK_PUSH(stack, 1);
#else
        STACK_PUSH(stack, 0);
#endif
        return 1;
    }

    // TODO: rewrite it using a more code size efficient way
    if (op != K_FUNC_GET &&
        op != K_FUNC_KRAKEN_OP_A &&
        op != K_FUNC_KRAKEN_OP_B &&
        op != K_FUNC_KRAKEN_OP_C &&
        op != K_FUNC_KRAKEN_OP_D &&
        op != K_FUNC_KRAKEN_OP_E &&
        op != K_FUNC_KRAKEN_OP_F) {
        STACK_POP(stack, second);
        STACK_POP(stack, first);
        PICACHU_DEBUG("[interpreter] pop#1:%d(0x%x), pop#2:%d(0x%x)\n",
            second, second, first, first);
    } else {
        STACK_POP(stack, first);
        PICACHU_DEBUG("[interpreter] pop#1:%d(0x%x)\n", first, first);
    }

    switch (op) {
    case K_BOOL_EQ:
        result = (first == second);
        STACK_PUSH(stack, result);
        PICACHU_DEBUG("[interpreter] op:%s, push result:%d(0x%x)\n", "BOOL_EQ", result, result);
        break;

    case K_FUNC_EFUSE: {
        /* efuse(       ADDR,       LOW_BIT,        HIGH_BIT,       ARG_NUMBER) */
        /*              args[1]     args[0]             first       second      */
        unsigned int args_num = second - 1;
        unsigned int mask = 0;
        unsigned int word = 0;
        unsigned int i;
        /* FIXME: remove magic number */
        uint32_t args[16];

        PICACHU_DEBUG("\n[interpreter] op:%s\n", "FUNC_EFUSE");
        for (i = 0; i < args_num; i++) {
            STACK_POP(stack, args[i]);
            PICACHU_DEBUG("\n[interpreter] args[%d]=0x%x", i, args[i]);
        }
        PICACHU_DEBUG("\n");

        mask = __creat_mask(args[0], first);
        word = picachu_read32(IO_BASE + args[1] - 0x10000000);
        PICACHU_DEBUG("\n[interpreter] mask:0x%x\n", mask);
        PICACHU_DEBUG("[interpreter] read addr:0x%x\n", args[1]);
        PICACHU_DEBUG("[interpreter] target word:0x%x\n", word);
        PICACHU_DEBUG("[interpreter] word & mask:0x%x\n", word & mask);
        result = (word & mask) >> args[0];
        STACK_PUSH(stack, result);
        PICACHU_DEBUG("[interpreter] push result:%d(0x%x)\n", result, result);
        break;
    }
    default:
        /* Wrong Opcode */
        PICACHU_WARN("[interpreter] error! wrong op code:0x%x!\n", op);
        return 0;
    }

    return 1;
}


static int __eval_expr(struct kraken_session_t *kd, int pos, int *committable)
{
    int opcode, start_pos, dummy;
    uint32_t ret;

    if (!committable)
        committable = &dummy;

    if (pos >= (int)kd->img_len) {
        PICACHU_ERR("Error: parameter image: illegal access at address: 0x%04x, exiting ...\n",
            pos);
        *committable = 0;
        return -1;
    }

    *committable = 1;

    start_pos = pos;
    while (1) {
        opcode = kd->img_symbtree_block[pos++];

        if (opcode == K_INT || opcode == K_UINT) {
            uint32_t var;

            memcpy(&var, &kd->img_symbtree_block[pos], sizeof(uint32_t));
            pos += sizeof(var);
            __eval_op(opcode, &var, kd, pos);
        } else if (opcode == K_HEXCHUNK) {
            uint8_t *ptr, len;

            /* [ len, 0x00, 0x00, 0x00, ... ] */
            memcpy(&len, &kd->img_symbtree_block[pos], sizeof(len));
            pos += sizeof(len);
            ptr = kd->img_symbtree_block + pos;

            /* never overwrite a hexchunk */
            *committable = 0;

            /* we don't expect hexchunk to be a part of a formula */
            return (uintptr_t) ptr;
        } else if (opcode == K_EOE) {
            STACK_POP(stack, ret);
            pos++;
            /* TODO: make this loop safer */
            return ret;

            /* we need at least this length of space to commit a outcome */
            /* [ K_INT, BYTE_3, BYTE_2, BYTE_1, BYTE_0, K_EOE ]          */
            if (pos - start_pos < 5)
                *committable = 1;
        } else {
            __eval_op(opcode, NULL, kd, pos);
        }
    }

    *committable = 0;

    return 0;
}

static void __parse_section_header(uint8_t **sec_start, uint32_t *sec_size,
                   uint8_t *src, int size)
{
    struct kraken_session_t kd;
    const struct kraken_section_table_entry_t *psec_ent;
    uint16_t section_table_size;
    uint32_t cond, ii;

    memcpy(&section_table_size, src, sizeof(uint16_t));

    kd.image = src;
    kd.img_symbtree_block = kd.image;

    psec_ent = (struct kraken_section_table_entry_t *) (kd.image + 2);
    /* TODO: add error handling of empty section list */
    /* section header's size equals to the start address of first section */
    kd.img_len = psec_ent->sec_ptr;

    kd.img_shadow_table_block = NULL;

    for (ii = 0, psec_ent = (struct kraken_section_table_entry_t *) (kd.image + 2);
         ii < section_table_size;
         ii++, psec_ent++) {
        cond = __eval_expr(&kd, psec_ent->cond_expr_pos, NULL);

        if (cond) {
            *sec_start = src + psec_ent->sec_ptr;
            *sec_size = psec_ent->sec_size;
            break;
        }
    }
}

static void __load_section(uint8_t *dest, uint32_t *len, uint8_t *sec_start, int sec_size)
{
    struct kraken_session_t kd;
    struct kraken_shadow_rule_t *psd_ent, *psd_wpos;
    struct kraken_shadow_rule_t sd_ent;
    uint32_t shadow_table_ent_num, symbtree_start,
        shadow_table_size, shadow_table_size_diff,
        symtree_wpos, symtree_orig_size, symtree_size,
        shadow_table_ld_num, expr_wpos, ld_symtree_start;
    uint32_t cond, ii;

    memcpy(&shadow_table_ent_num, &sec_start[0], sizeof(shadow_table_ent_num));
    memcpy(&symbtree_start, &sec_start[4], sizeof(symbtree_start));
    /* we expect [32:48] and [48:64] are identical */
    memcpy(&symtree_size, &sec_start[8], sizeof(symtree_size));
    memcpy(&symtree_size, &sec_start[12], sizeof(symtree_size));

    kd.image = sec_start;
    kd.img_len = sec_size;

    /* init shadowing table */
    kd.img_shadow_table_block = sec_start + 16;
    kd.img_symbtree_block = sec_start;

    shadow_table_ld_num = 0;
    shadow_table_size = 0;

    symtree_orig_size = symtree_size;
    shadow_table_size_diff = 0;
    for (ii = 0, psd_ent = (struct kraken_shadow_rule_t *)kd.img_shadow_table_block;
         ii < shadow_table_ent_num;
         ii++, psd_ent++) {
        cond = __eval_expr(&kd, psd_ent->expr_pos, NULL);

        if (cond) {
            shadow_table_ld_num++;
            shadow_table_size += sizeof(*psd_ent) + psd_ent->expr_len;
            symtree_size += psd_ent->val_len;
        } else {
            shadow_table_size_diff += psd_ent->expr_len;
        }
    }

    PICACHU_INFO("[KRAKEN]Linked shadow table entries: %d\n", shadow_table_ld_num);
    psd_wpos = (struct kraken_shadow_rule_t *) (dest + 16);
    expr_wpos = (shadow_table_ld_num * sizeof(*psd_ent)) + 16;
    symtree_wpos = shadow_table_size + symtree_orig_size + 16;
    ld_symtree_start = shadow_table_size + 16;
    for (ii = 0, psd_ent = (struct kraken_shadow_rule_t *)kd.img_shadow_table_block;
         ii < shadow_table_ent_num;
         ii++, psd_ent++) {
        cond = __eval_expr(&kd, psd_ent->expr_pos, NULL);

        if (cond) {
            memcpy(&sd_ent, psd_ent, sizeof(sd_ent));

            sd_ent.expr_pos = expr_wpos;
            expr_wpos += psd_ent->expr_len;
            sd_ent.val_pos = symtree_wpos - ld_symtree_start - 4;

            memcpy(dest + sd_ent.expr_pos,
                   sec_start + psd_ent->expr_pos, psd_ent->expr_len);
            memcpy(dest + symtree_wpos,
                   sec_start + psd_ent->val_pos, psd_ent->val_len);
            symtree_wpos += psd_ent->val_len;

            memcpy(psd_wpos, &sd_ent, sizeof(*psd_ent));
            psd_wpos++;
        }
    }

    memcpy(&dest[0], &shadow_table_ld_num, sizeof(shadow_table_ld_num));
    memcpy(&dest[4], &ld_symtree_start, sizeof(ld_symtree_start));
    memcpy(&dest[8], &symtree_size, sizeof(symtree_size));
    memcpy(&dest[12], &symtree_orig_size, sizeof(symtree_orig_size));

    PICACHU_INFO("[KRAKEN]Loaded main block size: %d (0x%x) bytes at offset 0x%x\n",
       symtree_orig_size, symtree_orig_size, ld_symtree_start);
    memcpy(dest + ld_symtree_start, sec_start + symbtree_start, symtree_orig_size);
    *len = ld_symtree_start + symtree_size;
    PICACHU_INFO("[KRAKEN]Total loaded section size: %d (0x%x) bytes\n", *len, *len);
}

int check_kraken_image_magic_cookie(uint8_t **dest, uint32_t *len,
                    uint8_t *img, int img_size)
{
    uint32_t *header_cookie, *footer_cookie;

    header_cookie = (uint32_t *) (img);
    footer_cookie = (uint32_t *) (img + img_size - 4);

    PICACHU_INFO("[KRAKEN]header_cookie=0x%x footer_cookie=0x%x img_size=%d\n",
        *header_cookie, *footer_cookie, img_size);

    if (!(img_size >= 0x14 &&
         *header_cookie == 0x17C3A6B4 &&
         *footer_cookie == 0x17C3A6B4)) {
        PICACHU_INFO("[KRAKEN]Magic cookie check failed, skip image loading\n");
        return 1;
    }

    *dest = img + 0x10;
    *len = img_size - 0x14;

    return 0;
}

void load_kraken_image(uint8_t *dest, uint32_t *len, uint8_t *img, int img_size)
{
    uint8_t *sec_start = NULL;
    uint32_t sec_size = 0;

    __parse_section_header(&sec_start, &sec_size, img, img_size);
    __load_section(dest, &sec_size, sec_start, sec_size);

    *len = sec_size;
}

static int kraken_load_para_img(const char *partition_name)
{
    unsigned int size = 0;
    status_t ret = NO_ERROR;

    //allocate mblock
    g_pi_part_buff = malloc(PART_MALLOC_SIZE);
    if (!g_pi_part_buff) {
        PICACHU_ERR("KRAKEN para malloc allocation failed\n");
        return ERR_NO_MEMORY;
    }

    //load image from flash, A/B suffix handle by load_partition
    ret = load_partition(partition_name, "pi_img", (void *)g_pi_part_buff, PART_MALLOC_SIZE);
    if (ret < 0) {
        PICACHU_ERR("KRAKEN load from partition failed, ret: %d\n", ret);
        return -1;
    }
    size = ret;

#if 0
    int i;

    PICACHU_INFO("KRAKEN image load\n");
    for (i = 0; i < size; ) {
        PICACHU_INFO("0x%X = 0x%8X 0x%8X 0x%8X 0x%8X\n",
            (char *)g_pi_part_buff + i,
            picachu_read32((char *)g_pi_part_buff + i),
            picachu_read32((char *)g_pi_part_buff + i + 4),
            picachu_read32((char *)g_pi_part_buff + i + 8),
            picachu_read32((char *)g_pi_part_buff + i + 12));
        i = i + 16;
    }
#endif

    /* save pi_img size to csam */
    if (size < PART_MALLOC_SIZE) {
        g_header.magic = 0xA5A5A5A5;
        g_header.size = size;
    }

    return size;
}

static int kraken_free_para_img(void)
{
    if (g_pi_part_buff) {
        free(g_pi_part_buff);
        g_pi_part_buff = 0;
    }

    return 0;
}

static void kraken_pi_img_to_csram(void)
{
    unsigned char *image_buf = NULL;
    unsigned char *image_body = NULL;
    unsigned int img_size = 0;
    unsigned int image_body_sz = 0;

    image_buf = malloc(g_header.size);
    if (image_buf && g_header.magic == 0xA5A5A5A5) {
        g_header.magic = 0xDEADBEEF;

#ifdef MTK_AGING_FLAVOR_LOAD
        g_header.is_aging_load = 1;
#endif

#ifdef MTK_SLT_FLAVOR_LOAD
        g_header.is_slt_load = 1;
#endif

#ifdef MTK_MCL50_FLAVOR_LOAD
        g_header.is_mc50_load = 1;
#endif

#ifdef MTK_LUT_FLAVOR_LOAD
        g_header.is_lut_load = 1;
        //TODO
        //update f/v from table
#endif

        check_kraken_image_magic_cookie(&image_body, &image_body_sz,
                        g_pi_part_buff, g_header.size);
        load_kraken_image(image_buf, &img_size, image_body, image_body_sz);
        memcpy((void *)CSRAM_START + sizeof(struct kraken_header), image_buf, img_size);
        g_header.size = img_size;

        memcpy((void *)CSRAM_START, &g_header, sizeof(struct kraken_header));
        arch_clean_invalidate_cache_range((addr_t)CSRAM_START, (CSRAM_END - CSRAM_START));

        PICACHU_INFO("[KRAKEN]is_aging_load=%d\n", g_header.is_aging_load);
        PICACHU_INFO("[KRAKEN]is_slt_load=%d\n", g_header.is_slt_load);
        PICACHU_INFO("[KRAKEN]is_mc50_load=%d\n", g_header.is_mc50_load);
        PICACHU_INFO("[KRAKEN]is_lut_load=%d\n", g_header.is_lut_load);

#ifdef KRAKEN_DEBUG_ENABLE
        int i;

        PICACHU_INFO("[KRAKEN]CSRAM_START=0x%8X\n", picachu_read32((void *)CSRAM_START));
        PICACHU_INFO("[KRAKEN]CSRAM_START+4=0x%8X\n", picachu_read32((void *)(CSRAM_START+4)));
        PICACHU_INFO("[KRAKEN]CSRAM_START+8=0x%8X\n", picachu_read32((void *)(CSRAM_START+8)));

        for (i = 0; i < g_header.size; ) {
            PICACHU_INFO("0x%X = 0x%8X 0x%8X 0x%8X 0x%8X\n",
                (char *)CSRAM_START + sizeof(struct kraken_header) + i,
                picachu_read32((char *)CSRAM_START + sizeof(struct kraken_header) + i),
                picachu_read32((char *)CSRAM_START + sizeof(struct kraken_header) + i + 4),
                picachu_read32((char *)CSRAM_START + sizeof(struct kraken_header) + i + 8),
                picachu_read32((char *)CSRAM_START + sizeof(struct kraken_header) + i + 12));
            i = i + 16;
        }
#endif
    }
    free(image_buf);
}


static status_t platform_load_kraken(void)
{
    PICACHU_INFO("[KRAKEN][CPU]: %s\n", MTK_CHIP);
    PICACHU_INFO("[KRAKEN][CPU][HRID][0]=0x%X\n", picachu_read32(PICACHU_EFUSE_HRID0));
    PICACHU_INFO("[KRAKEN][CPU][HRID][1]=0x%X\n", picachu_read32(PICACHU_EFUSE_HRID1));
    PICACHU_INFO("[KRAKEN][CPU][HRID][2]=0x%X\n", picachu_read32(PICACHU_EFUSE_HRID2));
    PICACHU_INFO("[KRAKEN][CPU][HRID][3]=0x%X\n", picachu_read32(PICACHU_EFUSE_HRID3));

    memset(&g_header, 0x0, sizeof(struct kraken_header));
    // CSRAM data layout
    // 0x0 -> pi_image magic
    // 0x4 -> pi_image size
    // 0x8 -> pi_image header
    // 0xC -> pi_image data
    picachu_write32(CSRAM_START, 0x0);
    // /* step 1 get setting from picachu parameter partition */
    if (kraken_load_para_img(picachu_partition_name) < 0) {
        PICACHU_ERR("line-%d: kraken_load_para_img failed\n", __LINE__);
        goto out;
    } else {
        kraken_pi_img_to_csram();
    }

out:
    kraken_free_para_img();
    return NO_ERROR;
}
#endif

status_t platform_load_picachu(void)
{
#if KRAKEN_SUPPOT
    return platform_load_kraken();
#else
    unsigned int offset = 0;

    picachu_write32(CSRAM_START, 0x0);
    PICACHU_INFO("[PICAHCU] CPU: %s\n", MTK_CHIP);

    // /* step 1 get setting from picachu parameter partition */
    if (picachu_load_para_img(picachu_partition_name) != 0) {
        PICACHU_INFO("line-%d: picachu_load_para_img failed\n", __LINE__);
        goto out;
    }
    PICACHU_INFO("Image Date: %u\n", picachu_get_img_date());
    PICACHU_INFO("[PICACHU][CPU][Picachu][HRID][0]: =0x%X\n", picachu_read32(PICACHU_EFUSE_HRID0));
    PICACHU_INFO("[PICACHU][CPU][Picachu][HRID][1]: =0x%X\n", picachu_read32(PICACHU_EFUSE_HRID1));
    PICACHU_INFO("[PICACHU][CPU][Picachu][HRID][2]: =0x%X\n", picachu_read32(PICACHU_EFUSE_HRID2));
    PICACHU_INFO("[PICACHU][CPU][Picachu][HRID][3]: =0x%X\n", picachu_read32(PICACHU_EFUSE_HRID3));

    /* step 2 update load_type */
    if (is_aging_flavor_load() == 1)
        picachu_set_is_aging_load(1);

    if (is_slt_load() == 1)
        picachu_set_is_slt_load(1);

    /* step 3 update pi_img to CSRAM */
    if (picachu_read32(CSRAM_START) == 0xA5A5A5A5) {
        //
        // CSRAM data layout
        // 0x0 -> pi_image magic
        // 0x4 -> pi_image size
        // 0x8 -> pi_image data
        // ...
        // 0x8+pi_image_size -> lut magic
        // 0x8+pi_image_size+4 -> lut cluster number
        // 0x8+pi_image_size+8 -> lut binning number
        // 0x8+pi_image_size+12 -> lut data
        // ...
        // 0x8+pi_image_size+12+lut_size -> picachu data
        //
        offset = picachu_read32(CSRAM_START + 4);
        offset = offset + 0x8;
        picachu_write32(CSRAM_START + offset, 0xDEADBEEF);
        picachu_write32(CSRAM_START + offset + 4, NR_EEMSN_DET);
        picachu_write32(CSRAM_START + offset + 8, NR_FREQ);
        g_vf_tbl_det = (struct dvfs_vf_tbl *)(CSRAM_START + offset + 12);
        memset(g_vf_tbl_det, 0x0, NR_EEMSN_DET * sizeof(struct dvfs_vf_tbl));
    }

    if (picachu_has_lut() == 1) {
        /* step 4 check lut */
        if (check_lut() == 1) {
            picachu_write32(CSRAM_START + offset, 0xBEEFDEAD);
            dump_vf_table();
            offset = offset + 12 + NR_EEMSN_DET * sizeof(struct dvfs_vf_tbl);
        }

#ifdef SUPPORT_DVCS_LUT
        /* step 5 check dvcs lut */
        if (check_dvcs_lut() == 1) {
            picachu_write32(CSRAM_START + offset, 0xBEEFDEAD);
            picachu_write32(CSRAM_START + offset + 4, MAX_MINCODE_TBL_SIZE);
            memcpy((void *)(CSRAM_START + offset + 8),
                (void *)get_dvcs_mincode_tbl(),
                MAX_MINCODE_TBL_SIZE * sizeof(int));
            dump_dvcs_table();
            offset = offset + 8 + (MAX_MINCODE_TBL_SIZE * sizeof(int));
        }
#endif

    }

    picachu_pi_img_to_csram();

out:
    picachu_free_para_img();

#ifdef PICACHU_FULL_FUNCTION
    extern void start_picachu(void);
    start_picachu();
#endif
#endif

    return NO_ERROR;
}
#endif

