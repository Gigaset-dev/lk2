LOCAL_DIR := $(GET_LOCAL_DIR)

MODULE := $(LOCAL_DIR)

GLOBAL_INCLUDES += $(LOCAL_DIR)

MODULE_SRCS += $(LOCAL_DIR)/avb_ab_flow.c
GLOBAL_DEFINES += AVB_COMPILATION

MODULE_DEPS += \
	external/lib/libavb

include make/module.mk
