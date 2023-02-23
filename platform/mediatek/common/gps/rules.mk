LOCAL_DIR := $(GET_LOCAL_DIR)

MODULE := $(LOCAL_DIR)

MODULE_SRCS += \
	$(LOCAL_DIR)/gps_init.c

MODULE_DEPS += \
	external/lib/fdt \
	platform/$(PLATFORM)/common/emi \
	platform/$(PLATFORM)/common/dtb_ops \
	lib/mblock

include make/module.mk
