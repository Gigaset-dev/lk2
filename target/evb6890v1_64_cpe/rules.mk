LOCAL_DIR := $(GET_LOCAL_DIR)

MODULE := $(LOCAL_DIR)

PLATFORM := mediatek
SUB_PLATFORM := mt6890

MODULE_SRCS += \
	$(LOCAL_DIR)/init.c

include make/module.mk
