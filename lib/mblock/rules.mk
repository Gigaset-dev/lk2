LOCAL_DIR := $(GET_LOCAL_DIR)

MODULE := $(LOCAL_DIR)

MODULE_DEPS += \
	external/lib/fdt \
	lib/pl_boottags \


MODULE_SRCS := \
	$(LOCAL_DIR)/mblock.c \
	$(LOCAL_DIR)/mblock_debug.c \

include make/module.mk
