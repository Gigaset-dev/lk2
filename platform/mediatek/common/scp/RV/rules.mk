LOCAL_DIR := $(GET_LOCAL_DIR)

MODULE := $(LOCAL_DIR)

MODULE_SRCS += \
        $(LOCAL_DIR)/scp_plat.c \
        $(LOCAL_DIR)/scp_plat_excep.c

MODULE_DEPS += \
        platform/$(PLATFORM)/common/loader \
        platform/$(PLATFORM)/common/wdt \
        platform/$(PLATFORM)/common/scp \
        lib/mblock

SCP_RV_VERSION ?= 55
SCP_CORE_NUMS ?= 2
SCP_DRAM_OFFSET ?= 0x300000

# The default size of share dram (total = SCP_RESERVED_SHARE_DRAM_SIZE + SCP_DUMP_SIZE)
# User may define new values on platform's rules.mk
# Notice: The size of reserved-memory needs to be 4K-aligned
# L2TCM:2MB, DRAM:1MB, L1C:256KB, REG:15KB, TBUF:1KB
SCP_DUMP_SIZE ?= 0x380000
# +0x20000 for ultrasound
SCP_RESERVED_SHARE_DRAM_SIZE ?= 0x00330000

SCP_SECURE_DOMAIN ?= 1
SCP_EMI_REGION ?= 6
SCP_ENABLE_EMI_PROTECTION ?= no
MTK_TINYSYS_SCP_SECURE_DUMP ?= no

MODULE_DEFINES += \
        IMG_NAME_SCP_A=\"tinysys-scp-RV$(SCP_RV_VERSION)_A\"\
        DRM_NAME_SCP_A=\"tinysys-scp-RV$(SCP_RV_VERSION)_A_dram\"\
        SCP_CORE_NUMS=$(SCP_CORE_NUMS)\
        SCP_DRAM_OFFSET=$(SCP_DRAM_OFFSET)\
        SCP_RV_VERSION=$(SCP_RV_VERSION)\
        SCP_DUMP_SIZE=$(SCP_DUMP_SIZE)\
        SCP_RESERVED_SHARE_DRAM_SIZE=$(SCP_RESERVED_SHARE_DRAM_SIZE)\
        SCP_SECURE_DOMAIN=$(SCP_SECURE_DOMAIN) \
        SCP_EMI_REGION=$(SCP_EMI_REGION)

ifdef SCP_SHARE_EMI_REGION
MODULE_DEFINES += \
        SCP_SHARE_EMI_REGION=$(SCP_SHARE_EMI_REGION)
endif

ifeq ($(SCP_ENABLE_EMI_PROTECTION), yes)
MODULE_DEFINES += SCP_ENABLE_EMI_PROTECTION
endif

ifeq ($(MTK_TINYSYS_SCP_SECURE_DUMP), yes)
MODULE_DEFINES += MTK_TINYSYS_SCP_SECURE_DUMP
endif

include make/module.mk
