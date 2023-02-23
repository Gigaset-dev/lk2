ifneq ($(strip $(TEE)),)

LOCAL_DIR := $(GET_LOCAL_DIR)

MODULE := $(LOCAL_DIR)

$(info Current BL2 ext. TEE is $(TEE))

MODULE_SRCS += \
	$(LOCAL_DIR)/tee_loader.c \
	$(LOCAL_DIR)/tee_$(TEE).c

MODULE_DEPS += \
	lib/mblock \
	platform/$(PLATFORM)/common/devinfo \
	platform/$(PLATFORM)/common/rtc \
	platform/$(PLATFORM)/common/sboot \
	platform/$(PLATFORM)/$(SUB_PLATFORM)/devinfo \
	platform/$(PLATFORM)/$(SUB_PLATFORM)/sboot

MTK_TEE_DRAM_SIZE ?= 0

# MTK_TEE_DRAM_SIZE is from ${PROJECT}.mk
MODULE_DEFINES += TEE_DRAM_SIZE=$(MTK_TEE_DRAM_SIZE)

#prize add by lipengpeng 20220704 start 

ifeq ($(MICROTRUST_TEE_SUPPORT),yes)
ifeq ($(MICROTRUST_TEE_LOG_ENC_DISABLE),yes)
MODULE_DEFINES += CFG_MICROTRUST_TEE_LOG_ENC_DISABLE=1
else
MODULE_DEFINES += CFG_MICROTRUST_TEE_LOG_ENC_DISABLE=0
endif
MODULE_DEFINES += CFG_RPMB_SET_KEY=0
MODULE_DEFINES += CFG_RPMB_SET_KEY_DELAY=0
endif

#prize add by lipengpeng 20220704 end 
include make/module.mk

endif
