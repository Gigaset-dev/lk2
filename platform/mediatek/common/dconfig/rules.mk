LOCAL_DIR := $(GET_LOCAL_DIR)

MODULE := $(LOCAL_DIR)

MODULE_SRCS += \
	$(LOCAL_DIR)/dconfig.c \
	$(LOCAL_DIR)/load_dconfig.c

MODULE_DEPS += \
	external/lib/fdt \
	lib/bio \
	platform/$(PLATFORM)/$(SUB_PLATFORM)/sboot \
	platform/$(PLATFORM)/common/dtb_ops \
	platform/$(PLATFORM)/common/loader

include make/module.mk
