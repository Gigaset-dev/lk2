LOCAL_DIR := $(GET_LOCAL_DIR)

MODULE := $(LOCAL_DIR)

MODULE_SRCS += \
	$(LOCAL_DIR)/ccu.c

MODULE_DEPS += \
	external/lib/fdt \
	lib/mblock \
	platform/$(PLATFORM)/common/loader

include make/module.mk
