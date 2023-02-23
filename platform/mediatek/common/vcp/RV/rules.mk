LOCAL_DIR := $(GET_LOCAL_DIR)

MODULE := $(LOCAL_DIR)

MODULE_SRCS += \
	$(LOCAL_DIR)/vcp_plat.c

MODULE_DEPS += \
	platform/$(PLATFORM)/common/loader \
	platform/$(PLATFORM)/common/vcp \
	lib/mblock

VCP_RV_VERSION ?= 55
VCP_CORE_NUMS ?= 1
VCP_DRAM_OFFSET ?= 0x200000

GLOBAL_DEFINES += \
	IMG_NAME_VCP_A=\"tinysys-vcp-RV$(VCP_RV_VERSION)_A\"\
	DRM_NAME_VCP_A=\"tinysys-vcp-RV$(VCP_RV_VERSION)_A_dram\"\
	CORE_NUMS=$(VCP_CORE_NUMS)\
	VCP_DRAM_OFFSET=$(VCP_DRAM_OFFSET)

include make/module.mk
