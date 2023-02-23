LOCAL_DIR := $(GET_LOCAL_DIR)

MODULE := $(LOCAL_DIR)

MODULE_SRCS += \
	$(LOCAL_DIR)/bl2_ext.c

MODULE_DEPS += \
	lib/mblock

include make/module.mk
