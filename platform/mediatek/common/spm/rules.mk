LOCAL_DIR := $(GET_LOCAL_DIR)

MODULE := $(LOCAL_DIR)

ifeq ($(SPM_FW_LOAD_LEGACY), true)
MODULE_DEFINES += \
	SPM_FW_LOAD_LEGACY=$(SPM_FW_LOAD_LEGACY)
endif

MODULE_SRCS += \
	$(LOCAL_DIR)/spm_aee_dump.c \
	$(LOCAL_DIR)/spm_common.c

MODULE_DEPS += \
	lib/mblock \
	platform/$(PLATFORM)/common/aee \
	platform/$(PLATFORM)/common/loader

include make/module.mk