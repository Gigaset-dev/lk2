LOCAL_DIR := $(GET_LOCAL_DIR)

MODULE := $(LOCAL_DIR)

MODULE_SRCS += \
	$(LOCAL_DIR)/dpm.c \
	$(LOCAL_DIR)/dpm_dump.c

MODULE_DEPS += \
	lib/mblock \
	platform/$(PLATFORM)/common/loader

include make/module.mk
