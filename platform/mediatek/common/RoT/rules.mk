LOCAL_DIR := $(GET_LOCAL_DIR)

MODULE := $(LOCAL_DIR)

MODULE_SRCS += \
	$(LOCAL_DIR)/RoT.c

MODULE_DEPS += \
	external/lib/libavb

include make/module.mk
