LOCAL_DIR := $(GET_LOCAL_DIR)

MODULE := $(LOCAL_DIR)

PLATFORM := mediatek
SUB_PLATFORM := mt8133

MODULE_SRCS += \
	$(LOCAL_DIR)/target.c

include make/module.mk
