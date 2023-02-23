LOCAL_DIR := $(GET_LOCAL_DIR)

MODULE := $(LOCAL_DIR)

MODULE_SRCS += \
	$(LOCAL_DIR)/dramc_v$(EMI_INFO_VER).c \
	$(LOCAL_DIR)/dramc_dump.c

MODULE_DEPS += \
	external/lib/fdt \
	lib/pl_boottags \
	platform/$(PLATFORM)/common/dtb_ops

include make/module.mk
