/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#define NAME_LEN 32

/* define scenario type */
#define SVP_FLAGS               0x01u
#define FACE_REGISTRATION_FLAGS 0x02u
#define FACE_PAYMENT_FLAGS      0x04u
#define FACE_UNLOCK_FLAGS       0x08u
#define TUI_FLAGS               0x10u

enum ssmr_scheme_state {
    SSMR_SVP,
    SSMR_FACE_REGISTRATION,
    SSMR_FACE_PAYMENT,
    SSMR_FACE_UNLOCK,
    SSMR_TUI_SCHEME,
    __MAX_NR_SCHEME,
};

struct SSMR_Scheme {
    char name[NAME_LEN];
    u64  usable_size;
    unsigned int flags;
};

static struct SSMR_Scheme _ssmrscheme[__MAX_NR_SCHEME] = {
    [SSMR_SVP] = {
        .name = "svp_scheme",
        .flags = SVP_FLAGS
    },
    [SSMR_FACE_REGISTRATION] = {
        .name = "face_registration_scheme",
        .flags = FACE_REGISTRATION_FLAGS
    },
    [SSMR_FACE_PAYMENT] = {
        .name = "face_payment_scheme",
        .flags = FACE_PAYMENT_FLAGS
    },
    [SSMR_FACE_UNLOCK] = {
        .name = "face_unlock_scheme",
        .flags = FACE_UNLOCK_FLAGS
    },
    [SSMR_TUI_SCHEME] = {
        .name = "tui_scheme",
        .flags = TUI_FLAGS
    }
};

struct SSMR_Feature {
    char dt_prop_name[NAME_LEN];
    char feat_name[NAME_LEN];
    unsigned int scheme_flag;
    u64 req_size;
};

enum ssmr_feature_type {
    SSMR_FEAT_SVP_REGION,
    SSMR_FEAT_PROT_REGION,
    SSMR_FEAT_WFD_REGION,
    SSMR_FEAT_TA_ELF,
    SSMR_FEAT_TA_STACK_HEAP,
    SSMR_FEAT_SDSP_FIRMWARE,
    SSMR_FEAT_SDSP_TEE_SHAREDMEM,
    SSMR_FEAT_2D_FR,
    SSMR_FEAT_TUI,
    SSMR_FEAT_SAPU_DATA_SHM,
    SSMR_FEAT_SAPU_ENGINE_SHM,
    __MAX_NR_SSMR_FEATURES,
};

static struct SSMR_Feature _ssmr_feats[__MAX_NR_SSMR_FEATURES] = {
    [SSMR_FEAT_SVP_REGION] = {
        .dt_prop_name = "svp-region-based-size",
        .feat_name = "svp_region_based",
        .scheme_flag = SVP_FLAGS
    },
    [SSMR_FEAT_PROT_REGION] = {
        .dt_prop_name = "prot-region-based-size",
        .feat_name = "prot_region_based",
        .scheme_flag = FACE_REGISTRATION_FLAGS | FACE_PAYMENT_FLAGS |
                FACE_UNLOCK_FLAGS | SVP_FLAGS
    },
    [SSMR_FEAT_WFD_REGION] = {
        .dt_prop_name = "wfd-region-based-size",
        .feat_name = "wfd_region_based",
        .scheme_flag = SVP_FLAGS
    },
    [SSMR_FEAT_TA_ELF] = {
        .dt_prop_name = "ta-elf-size",
        .feat_name = "ta-elf",
        .scheme_flag = FACE_REGISTRATION_FLAGS | FACE_PAYMENT_FLAGS |
                FACE_UNLOCK_FLAGS
    },
    [SSMR_FEAT_TA_STACK_HEAP] = {
        .dt_prop_name = "ta-stack-heap-size",
        .feat_name = "ta-stack-heap",
        .scheme_flag = FACE_REGISTRATION_FLAGS | FACE_PAYMENT_FLAGS |
                FACE_UNLOCK_FLAGS
    },
    [SSMR_FEAT_SDSP_TEE_SHAREDMEM] = {
        .dt_prop_name = "sdsp-tee-sharedmem-size",
        .feat_name = "sdsp-tee-sharedmem",
        .scheme_flag = FACE_REGISTRATION_FLAGS | FACE_PAYMENT_FLAGS |
                FACE_UNLOCK_FLAGS
    },
    [SSMR_FEAT_SDSP_FIRMWARE] = {
        .dt_prop_name = "sdsp-firmware-size",
        .feat_name = "sdsp-firmware",
        .scheme_flag = FACE_UNLOCK_FLAGS
    },
    [SSMR_FEAT_2D_FR] = {
        .dt_prop_name = "2d_fr-size",
        .feat_name = "2d_fr",
        .scheme_flag = FACE_REGISTRATION_FLAGS | FACE_PAYMENT_FLAGS |
                FACE_UNLOCK_FLAGS
    },
    [SSMR_FEAT_TUI] = {
        .dt_prop_name = "tui-size",
        .feat_name = "tui",
        .scheme_flag = TUI_FLAGS
    },
    [SSMR_FEAT_SAPU_DATA_SHM] = {
        .dt_prop_name = "sapu-data-shm-size",
        .feat_name = "sapu_data_shm",
        .scheme_flag = FACE_UNLOCK_FLAGS
    },
    [SSMR_FEAT_SAPU_ENGINE_SHM] = {
        .dt_prop_name = "sapu-engine-shm-size",
        .feat_name = "sapu_engine_shm",
        .scheme_flag = FACE_UNLOCK_FLAGS
    }
};

