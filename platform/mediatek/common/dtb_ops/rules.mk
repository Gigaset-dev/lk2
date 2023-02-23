LOCAL_DIR := $(GET_LOCAL_DIR)

MODULE := $(LOCAL_DIR)

MODULE_SRCS += \
	$(LOCAL_DIR)/dtb.c \
	$(LOCAL_DIR)/dtbo.c \
	$(LOCAL_DIR)/set_fdt.c \
	$(LOCAL_DIR)/set_fdt_test.c

MODULE_DEPS += \
	external/lib/fdt \
	external/lib/ufdt \
	platform/$(PLATFORM)/common/loader

EXTRA_LINKER_SCRIPTS += $(LOCAL_DIR)/set_fdt.ld

include make/module.mk

