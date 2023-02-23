LOCAL_DIR := $(GET_LOCAL_DIR)

MODULE := $(LOCAL_DIR)

MODULE_SRCS += $(LOCAL_DIR)/mfgsys_history_expdb.c

MODULE_DEPS += \
	platform/$(PLATFORM)/common/aee \
	platform/$(PLATFORM)/common/dtb_ops

include make/module.mk
