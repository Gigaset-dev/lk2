LOCAL_DIR := $(GET_LOCAL_DIR)

MODULE := $(LOCAL_DIR)

MPU_INFO_VER ?= 2
USE_LASTSLC ?= no

MODULE_DEFINES += MPU_INFO_VER=$(MPU_INFO_VER)

MODULE_SRCS += \
	$(LOCAL_DIR)/emi_info_v$(EMI_INFO_VER).c \
	$(LOCAL_DIR)/emi_mpu_v$(MPU_INFO_VER).c \
	$(LOCAL_DIR)/emi_lastemi_v1.c \
	$(LOCAL_DIR)/emi_addr2dram.c \
	$(LOCAL_DIR)/emi_addr2dram_v1.c \
	$(LOCAL_DIR)/emi_addr2dram_v2.c

ifeq ($(USE_LASTSLC), yes)
MODULE_SRCS += $(LOCAL_DIR)/emi_lastslc.c
endif

MODULE_DEPS += \
	external/lib/fdt \
	lib/pl_boottags \
	platform/$(PLATFORM)/common/aee \
	platform/$(PLATFORM)/common/dtb_ops \

include make/module.mk
