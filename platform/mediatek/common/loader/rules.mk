LOCAL_DIR := $(GET_LOCAL_DIR)

MODULE := $(LOCAL_DIR)

MODULE_SRCS += \
	$(LOCAL_DIR)/load_image.c

MODULE_DEPS += \
	external/lib/fdt \
	lib/bio \
	platform/$(PLATFORM)/common/dtb_ops \
	platform/$(PLATFORM)/common/sboot \
	platform/$(PLATFORM)/$(SUB_PLATFORM)/sboot

include make/module.mk
