LOCAL_DIR := $(GET_LOCAL_DIR)

MODULE := $(LOCAL_DIR)

MODULE_SRCS += \
	$(LOCAL_DIR)/consys.c

MODULE_DEPS += \
	external/lib/fdt \
	platform/$(PLATFORM)/common/dtb_ops

include make/module.mk
