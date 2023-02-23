LOCAL_DIR := $(GET_LOCAL_DIR)

MODULE := $(LOCAL_DIR)

MODULE_DEPS += \
	external/lib/fdt

MODULE_SRCS += \
	$(LOCAL_DIR)/kcmdline.c \
	$(LOCAL_DIR)/debug.c \

include make/module.mk
