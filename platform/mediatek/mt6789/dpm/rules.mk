LOCAL_DIR := $(GET_LOCAL_DIR)

MODULE := $(LOCAL_DIR)

MODULE_SRCS += \
	$(LOCAL_DIR)/dpm.c

MODULE_DEPS += \
	platform/$(PLATFORM)/common/dpm

include make/module.mk
