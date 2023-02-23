/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */
#pragma once
/*******************************************************************************
 * enum define
 ******************************************************************************/
enum {
    kNPTP_CPU_L = 1,
    kNPTP_CPU_D = 2,
    kNPTP_CPU_B = 3,
    kNPTP_CPU_M = 4
};

enum {
    kSENSOR_NETWORK_EFUSE_DATA = 1,
    kDVCS_EFUSE_DATA = 2,
    kPTP_EFUSE_DATA = 3,
    kDVCS_CC_TBL_DATA = 129,
    kPTP_V05_TBL_DATA = 130,
    kVSN_V05_TBL_DATA = 131,
    kSYNAGE_TBL_DATA = 132,
    kPTP_PP_OLD2NEW_AGE_TBL_DATA = 133,
};


/*******************************************************************************
 *
 ******************************************************************************/
#ifdef TARGET_BUILD_VARIANT_SLT
extern unsigned char __pi_img_start; //from pi_img.S
extern unsigned char __pi_img_end; // from pi_img.S
#endif /* #ifdef TARGET_BUILD_VARIANT_SLT */


/*******************************************************************************
 * struct define
 ******************************************************************************/
struct picachu_img_header {
    unsigned int magic;
    unsigned int version;
    unsigned int chip_id;
    unsigned int date;
};

struct pichchu_misc {
    unsigned char debug_mode;
    unsigned char voltage_calibration;
    unsigned short pmic_resolution;

    unsigned char disable_picachu;
    unsigned char half_year_aging_percent;
    unsigned char upper_bound;
    unsigned char lower_bound;

    unsigned char has_sensor_network;
    unsigned char has_dvcs;
    unsigned char has_lut;
    unsigned char is_aging_load;

    unsigned char is_slt_load;
    unsigned char reserve;
    unsigned char cluster_size;
    unsigned char sd_global_size;
};

struct pichchu_cluster {
    unsigned short cluster_id : 3,
               frequency  : 13;
    unsigned char version;
    char adj_volt;
};

struct pichchu_sd_global_header {
    unsigned int magic; /* 0x76AA248E */
    unsigned char type;
    unsigned char cpu_volt_version;
    unsigned short size;
};

struct picachu_lut_header {
    unsigned int magic; /* 0xA5BEEFA5 */
    unsigned int date;
    unsigned int db_size;
    unsigned int record_number  : 24,
             hrid_num       : 8;
    unsigned int voltage_bit    : 6,
             frequency_bit  : 6,
             point_id_bit   : 6,
             cluster_id_bit : 6,
             version    : 8;
};

struct picachu_dvcs_lut_header {
    unsigned int magic; /* 0xA5BEEFA5 */
    unsigned int date;
    unsigned int db_size;
    unsigned int record_number   : 24,
             hrid_num            : 8;
    unsigned int voltage_bits    : 5,
             tj_bits             : 5,
             fc_id_bits          : 5,
             cc_id_bits          : 5,
             nptp_id_bits        : 5,
             version             : 7;
};


/*******************************************************************************
 *            header
 ******************************************************************************/
unsigned int picachu_get_img_date(void);


/*******************************************************************************
 *            misc
 ******************************************************************************/
unsigned int picachu_get_pmic_resolution(void);
unsigned int picachu_get_disable_picachu(void);
unsigned int picachu_get_half_year_aging_percent(void);
unsigned int picachu_get_upper_bound(void);
unsigned int picachu_get_lower_bound(void);
unsigned int picachu_has_sensor_network(void);
unsigned int picachu_has_dvcs(void);
unsigned int picachu_has_lut(void);
unsigned int picachu_is_aging_load(void);
unsigned int picachu_is_slt_load(void);

void picachu_set_is_aging_load(int mode);
void picachu_set_is_slt_load(int mode);


/*******************************************************************************
 *                cluster
 ******************************************************************************/
const struct pichchu_cluster *picachu_get_cluster(
    unsigned int cluster_id,
    unsigned int frequency);


/*******************************************************************************
 *                sd_global_header
 ******************************************************************************/
struct pichchu_sd_global_header *picachu_get_sd_global_header_list(int idx);



/*******************************************************************************
 *            LUT
 ******************************************************************************/
struct picachu_lut_header *get_picachu_lut_header(void);
char *get_picachu_lut_db_start(void);

#ifdef SUPPORT_DVCS_LUT
char *get_picachu_dvcs_lut_db_start(void);
struct picachu_dvcs_lut_header *get_picachu_dvcs_lut_header(void);
#endif


/*******************************************************************************
 *
 ******************************************************************************/
int picachu_load_para_img(const char *partition_name);
int picachu_free_para_img(void);


/*******************************************************************************
 *    LK_AS_BL2_EXT only
 ******************************************************************************/
#if LK_AS_BL2_EXT
unsigned int picachu_pi_img_to_csram(void);
#endif
