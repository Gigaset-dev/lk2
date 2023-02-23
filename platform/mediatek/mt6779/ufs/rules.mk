LOCAL_DIR := $(GET_LOCAL_DIR)

MODULE := $(LOCAL_DIR)

MODULE_INCLUDES += platform/$(PLATFORM)/common/ufs

MODULE_SRCS += \
	$(LOCAL_DIR)/ufs_platform.c

MODULE_DEPS += \
	platform/$(PLATFORM)/common/ufs

include make/module.mk

