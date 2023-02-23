/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */
#if LK_AS_BL2_EXT
#include <arch/ops.h> //arch_clean_invalidate_cache_range
#include <platform/picachu_def.h>
#include <platform/picachu_reg.h>
#else
#include "picachu_def.h"
#include "picachu_reg.h"
#endif
#include "picachu_log.h"
#include "picachu_para.h"


/*******************************************************************************
 *
 ******************************************************************************/
#define PARTITION_FILENAME_SIZE  16
#define PART_MALLOC_SIZE         0x100000


/*******************************************************************************
 *
 ******************************************************************************/
#ifdef TARGET_BUILD_VARIANT_PRELOADER
extern int pi_img_suffix(void);
#endif


/*******************************************************************************
 *
 ******************************************************************************/
int DEBUG_MODE;


/*******************************************************************************
 *
 ******************************************************************************/
static struct picachu_img_header *g_header;
static struct pichchu_misc *g_misc;
static struct picachu_lut_header *g_lut_header;
static struct pichchu_cluster *g_cluster;
static struct pichchu_sd_global_header *g_sd_global_header_list[MAX_SD_GLOBAL_SIZE];
static char *g_picachu_lut_db_start;

#ifdef SUPPORT_DVCS_LUT
struct picachu_dvcs_lut_header *g_dvcs_lut_header;
static char *g_picachu_dvcs_lut_db_start;
#endif

#ifdef TARGET_BUILD_VARIANT_SLT
static char g_pi_part_buff[1024*1024]; // The buffer to load pi_img
#else
static char *g_pi_part_buff; // The buffer to load pi_img
#endif


/*******************************************************************************
 *                header
 ******************************************************************************/
unsigned int picachu_get_img_date(void)
{
    return g_header->date;
}


/*******************************************************************************
 *                misc
 ******************************************************************************/
unsigned int picachu_get_pmic_resolution(void)
{
    return g_misc->pmic_resolution;
}

unsigned int picachu_get_disable_picachu(void)
{
    return g_misc->disable_picachu;
}

unsigned int picachu_get_half_year_aging_percent(void)
{
    return g_misc->half_year_aging_percent;
}

unsigned int picachu_get_upper_bound(void)
{
    return g_misc->upper_bound;
}

unsigned int picachu_get_lower_bound(void)
{
    return g_misc->lower_bound;
}

unsigned int picachu_has_sensor_network(void)
{
    return g_misc->has_sensor_network;
}

unsigned int picachu_has_dvcs(void)
{
    return g_misc->has_dvcs;
}

unsigned int picachu_has_lut(void)
{
    return g_misc->has_lut;
}

unsigned int picachu_is_aging_load(void)
{
    return g_misc->is_aging_load;
}

unsigned int picachu_is_slt_load(void)
{
    return g_misc->is_slt_load;
}

void picachu_set_is_aging_load(int mode)
{
    g_misc->is_aging_load = mode;
}

void picachu_set_is_slt_load(int mode)
{
    g_misc->is_slt_load = mode;
}


/*******************************************************************************
 *                cluster
 ******************************************************************************/
const struct pichchu_cluster *picachu_get_cluster(
    unsigned int cluster_id,
    unsigned int frequency)
{
    unsigned int i;
    struct pichchu_cluster *cluster = NULL;

    for (i = 0; i < g_misc->cluster_size; i++) {
        cluster = &g_cluster[i];
        if (cluster && cluster->cluster_id == cluster_id
            && cluster->frequency == frequency)
            return cluster;
    }

    return NULL;
}


/*******************************************************************************
 *
 ******************************************************************************/
struct pichchu_sd_global_header *picachu_get_sd_global_header_list(int idx)
{
    if (idx < 0)
        return NULL;

    return g_sd_global_header_list[idx];
}


/*******************************************************************************
 *
 ******************************************************************************/
int picachu_load_para_img(const char *partition_name)
{
    unsigned int i;
    unsigned int offset = 0;
    unsigned int size = 0;
    struct pichchu_cluster *cluster_ptr = NULL;
    struct pichchu_sd_global_header sd_header;

#if LK_AS_BL2_EXT
    status_t ret = NO_ERROR;

    //allocate mblock
    g_pi_part_buff = malloc(PART_MALLOC_SIZE);
        PICACHU_DEBUG("[PICACHU] %s enter\n", __func__);
    if (!g_pi_part_buff) {
        PICACHU_ERR("PICACHU para malloc allocation failed\n");
        return ERR_NO_MEMORY;
    }

    //load image from flash, A/B suffix handle by load_partition
    ret = load_partition(partition_name, "pi_img", (void *)g_pi_part_buff, PART_MALLOC_SIZE);
    if (ret < 0) {
        PICACHU_ERR("PICACHU load from partition failed, ret: %d\n", ret);
        return -1;
    }
    size = ret;
#endif /* #ifdef LK_AS_BL2_EXT */

#ifdef TARGET_BUILD_VARIANT_SLT
    size = &__pi_img_end - &__pi_img_start;
    memcpy(g_pi_part_buff, &__pi_img_start, size);
#endif /* #ifdef TARGET_BUILD_VARIANT_SLT */

#ifdef TARGET_BUILD_VARIANT_MCUPM
    if (picachu_read32(CSRAM_START) != 0xDEADBEEF) {
        PICACHU_ERR("picachu_pi_img_load fail\n");
        return -1;
    }
    size = picachu_read32(CSRAM_START + 4);
    if ((CSRAM_START + size) >= CSRAM_END) {
        PICACHU_ERR("size\n", size);
        return -1;
    }
    g_pi_part_buff = (char *)(CSRAM_START + 8);
#endif /* #ifdef TARGET_BUILD_VARIANT_MCUPM */

    /*  picachu_img_header */
    PICACHU_DEBUG("\nHeader:\n");
    g_header = (struct picachu_img_header *)g_pi_part_buff;
    offset += sizeof(struct picachu_img_header);
    PICACHU_DEBUG("header.magic=0x%X\n", g_header->magic);
    PICACHU_DEBUG("header.version=%u\n", g_header->version);
    PICACHU_DEBUG("header.chip_id=%u\n", g_header->chip_id);
    PICACHU_DEBUG("header.date=%u\n\n", g_header->date);
    if (g_header->magic != 0xA5A5BEEF) {
        PICACHU_DEBUG("header.magic is not match\n");
        return -1;
    }

    /* pichchu_misc */
    PICACHU_DEBUG("\nMisc:\n");
    g_misc = (struct pichchu_misc *)(g_pi_part_buff + offset);
    offset += sizeof(struct pichchu_misc);
    DEBUG_MODE = (*g_misc).debug_mode;
    PICACHU_DEBUG("misc.debug_mode=%u, DEBUG_MODE=%u\n", g_misc->debug_mode, DEBUG_MODE);
    PICACHU_DEBUG("misc.voltage_calibration=%u\n", g_misc->voltage_calibration);
    PICACHU_DEBUG("misc.pmic_resolution=%u\n", g_misc->pmic_resolution);
    PICACHU_DEBUG("misc.disable_picachu=%u\n", g_misc->disable_picachu);
    PICACHU_DEBUG("misc.half_year_aging_percent=%u\n", g_misc->half_year_aging_percent);
    PICACHU_DEBUG("misc.upper_bound=%u\n", g_misc->upper_bound);
    PICACHU_DEBUG("misc.lower_bound=%u\n", g_misc->lower_bound);
    PICACHU_DEBUG("misc.has_sensor_network=%u\n", g_misc->has_sensor_network);
    PICACHU_DEBUG("misc.has_dvcs=%u\n", g_misc->has_dvcs);
    PICACHU_DEBUG("misc.has_lut=%u\n", g_misc->has_lut);
    PICACHU_DEBUG("misc.is_aging_load=%u\n", g_misc->is_aging_load);
    PICACHU_DEBUG("misc.is_slt_load=%u\n", g_misc->is_slt_load);
    PICACHU_DEBUG("misc.reserve=%u\n", g_misc->reserve);
    PICACHU_DEBUG("misc.cluster_size=%u\n", g_misc->cluster_size);
    PICACHU_DEBUG("misc.sd_global_size=%u\n", g_misc->sd_global_size);

    /*  picachu cluster data */
    g_cluster = (struct pichchu_cluster *)(g_pi_part_buff + offset);
    cluster_ptr = g_cluster;
    offset += g_misc->cluster_size * sizeof(struct pichchu_cluster);
    for (i = 0; i < g_misc->cluster_size; i++) {
        PICACHU_DEBUG("cluster.cluster_id=%u\n", cluster_ptr->cluster_id);
        PICACHU_DEBUG("cluster.frequency=%u\n", cluster_ptr->frequency);
        PICACHU_DEBUG("cluster.version=%u\n", cluster_ptr->version);
        PICACHU_DEBUG("cluster.adj_volt=%d\n\n", cluster_ptr->adj_volt);
        cluster_ptr++;
    }

    /* picachu sd data */
    if (g_misc->sd_global_size > MAX_SD_GLOBAL_SIZE) {
        PICACHU_DEBUG("sd_global_size=%d > %d\n",
            g_misc->sd_global_size, MAX_SD_GLOBAL_SIZE);
        return -1;
    }

    for (i = 0; i < g_misc->sd_global_size; i++) {
        g_sd_global_header_list[i] =
            (struct pichchu_sd_global_header *)(g_pi_part_buff + offset);
        memcpy(&sd_header, g_pi_part_buff + offset,
            sizeof(struct pichchu_sd_global_header));
        offset += sizeof(struct pichchu_sd_global_header);

        PICACHU_DEBUG("SD: magic=0x%X, type=%d, ver=%d, size=%d\n",
            sd_header.magic, sd_header.type, sd_header.cpu_volt_version, sd_header.size);

        if (sd_header.magic == 0x76AA248E && sd_header.size > 0) {
            switch (sd_header.type) {
            case kSENSOR_NETWORK_EFUSE_DATA:
                offset += sd_header.size * sizeof(unsigned int);
                break;

            case kDVCS_EFUSE_DATA:
                offset += sd_header.size * sizeof(unsigned int);
                break;

            case kPTP_EFUSE_DATA:
                offset += sd_header.size * sizeof(unsigned int);
                break;

            case kDVCS_CC_TBL_DATA:
                offset += sd_header.size * sizeof(unsigned int);
                break;

            case kPTP_V05_TBL_DATA:
                offset += sd_header.size * sizeof(unsigned int);
                break;

            case kVSN_V05_TBL_DATA:
                offset += sd_header.size * sizeof(unsigned int);
                break;

            case kSYNAGE_TBL_DATA:
                offset += sd_header.size * sizeof(unsigned int);
                break;

            case kPTP_PP_OLD2NEW_AGE_TBL_DATA:
                offset += sd_header.size * sizeof(unsigned int);
                break;

            default:
                break;
            }
        }
    }

    /* save pi_img size to csam */
    if (offset < 0x0000FFFFUL) {
        picachu_write32(CSRAM_START, 0xA5A5A5A5);
        picachu_write32(CSRAM_START + 4, offset);
    }

    /* picachu look up table header */
    if (g_misc->has_lut == 1) {
        unsigned int magic = 0;

        // case 1: ptp only LUT
        // case 2: dvcs only LUT
        // case 3: ptp + dvcs LUT

        while (offset < size) {
            memcpy(&magic, g_pi_part_buff + offset, sizeof(int));
            if (magic == 0xA5BEEFA5)
                goto lut_header;

#ifdef SUPPORT_DVCS_LUT
            if (magic == 0x68866783)
                goto dvcs_lut_header;
#endif
            else
                offset += sizeof(int);
        }

        if (magic != 0xA5BEEFA5) {
            PICACHU_DEBUG("offset=0x%X\n", offset);
            return 0;
        }

lut_header:
        g_lut_header = (struct picachu_lut_header *)(g_pi_part_buff + offset);

        PICACHU_DEBUG("LUT magic=0x%X\n", g_lut_header->magic);
        PICACHU_DEBUG("LUT date=%u\n", g_lut_header->date);
        PICACHU_DEBUG("LUT db_size=%u\n", g_lut_header->db_size);
        PICACHU_DEBUG("LUT hrid_num=%u\n", g_lut_header->hrid_num);
        PICACHU_DEBUG("LUT record_number=%u\n", g_lut_header->record_number);
        PICACHU_DEBUG("LUT version=%u\n", g_lut_header->version);
        PICACHU_DEBUG("LUT cluster_id_bit=%u\n", g_lut_header->cluster_id_bit);
        PICACHU_DEBUG("LUT point_id_bit=%u\n", g_lut_header->point_id_bit);
        PICACHU_DEBUG("LUT frequency_bit=%u\n", g_lut_header->frequency_bit);
        PICACHU_DEBUG("LUT voltage_bit=%u\n\n", g_lut_header->voltage_bit);
        offset += sizeof(struct picachu_lut_header);
        g_picachu_lut_db_start = (char *)g_pi_part_buff + offset;
        offset += g_lut_header->db_size;
        memcpy(&magic, g_pi_part_buff + offset, sizeof(int));
        if (magic != 0x68866783) {
            // case 1: ptp only LUT
            return 0;
        }

#ifdef SUPPORT_DVCS_LUT
dvcs_lut_header:
        /* picachu dvcs look up table header */
        // case 2: dvcs only LUT
        // case 3: ptp + dvcs LUT

        g_dvcs_lut_header = (struct picachu_dvcs_lut_header *)(g_pi_part_buff + offset);
        PICACHU_DEBUG("DVCS LUT magic=0x%X\n", g_dvcs_lut_header->magic);
        PICACHU_DEBUG("DVCS LUT date=%u\n", g_dvcs_lut_header->date);
        PICACHU_DEBUG("DVCS LUT db_size=%u\n", g_dvcs_lut_header->db_size);
        PICACHU_DEBUG("DVCS LUT hrid_num=%u\n", g_dvcs_lut_header->hrid_num);
        PICACHU_DEBUG("DVCS LUT record_number=%u\n", g_dvcs_lut_header->record_number);
        PICACHU_DEBUG("DVCS LUT version=%u\n", g_dvcs_lut_header->version);
        PICACHU_DEBUG("DVCS LUT nptp_id_bits=%u\n", g_dvcs_lut_header->nptp_id_bits);
        PICACHU_DEBUG("DVCS LUT cc_id_bits=%u\n", g_dvcs_lut_header->cc_id_bits);
        PICACHU_DEBUG("DVCS LUT fc_id_bits=%u\n", g_dvcs_lut_header->fc_id_bits);
        PICACHU_DEBUG("DVCS LUT voltage_bits=%u\n", g_dvcs_lut_header->voltage_bits);

        offset += sizeof(struct picachu_dvcs_lut_header);
        g_picachu_dvcs_lut_db_start = g_pi_part_buff + offset;
#endif
    }

    return 0;
}


int picachu_free_para_img(void)
{
    if (g_pi_part_buff) {
        if (g_lut_header && g_picachu_lut_db_start)
            g_picachu_lut_db_start = 0;

#if LK_AS_BL2_EXT
        free(g_pi_part_buff);
        g_pi_part_buff = 0;
#endif

#ifdef TARGET_BUILD_VARIANT_SLT
        // data in CSRAM
#endif

#ifdef TARGET_BUILD_VARIANT_MCUPM
        // data in CSRAM
#endif
    }

    return 0;
}


/*******************************************************************************
 *
 ******************************************************************************/
struct picachu_lut_header *get_picachu_lut_header(void)
{
    return g_lut_header;
}


char *get_picachu_lut_db_start(void)
{
    return g_picachu_lut_db_start;
}

#ifdef SUPPORT_DVCS_LUT
struct picachu_dvcs_lut_header *get_picachu_dvcs_lut_header(void)
{
    return g_dvcs_lut_header;
}

char *get_picachu_dvcs_lut_db_start(void)
{
    return g_picachu_dvcs_lut_db_start;
}
#endif


/*******************************************************************************
 *
 ******************************************************************************/
#if LK_AS_BL2_EXT
unsigned int picachu_pi_img_to_csram(void)
{
    unsigned int size = 0;
    unsigned int value = 0;

    value = picachu_read32(CSRAM_START);
    if (value == 0xA5A5A5A5) {
        size = picachu_read32(CSRAM_START + 4);
        memcpy((void *)CSRAM_START + 8, g_pi_part_buff, size);
        picachu_write32(CSRAM_START, 0xDEADBEEF);
        arch_clean_invalidate_cache_range((addr_t)CSRAM_START, (CSRAM_END - CSRAM_START));
    }

    return size;
}
#endif
