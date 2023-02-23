LOCAL_DIR := $(GET_LOCAL_DIR)

MODULE := $(LOCAL_DIR)

MODULE_SRCS += \
	$(LOCAL_DIR)/medmcu.c

MODULE_DEPS += \
	external/lib/fdt

include make/module.mk
