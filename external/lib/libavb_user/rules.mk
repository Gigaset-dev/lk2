LOCAL_DIR := $(GET_LOCAL_DIR)

MODULE := $(LOCAL_DIR)

GLOBAL_INCLUDES += $(LOCAL_DIR)

MODULE_SRCS += $(LOCAL_DIR)/avb_user_verity.c
MODULE_SRCS += $(LOCAL_DIR)/avb_user_verification.c

MODULE_DEPS += \
	external/lib/libavb

include make/module.mk
