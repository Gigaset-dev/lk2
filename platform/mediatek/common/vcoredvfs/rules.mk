LOCAL_DIR := $(GET_LOCAL_DIR)

MODULE := $(LOCAL_DIR)

MODULE_SRCS += \
	$(LOCAL_DIR)/vcoredvfs.c

MODULE_DEPS += \
	platform/$(PLATFORM)/common/aee

include make/module.mk
