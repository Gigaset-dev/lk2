LOCAL_DIR := $(GET_LOCAL_DIR)

MODULE := $(LOCAL_DIR)

MODULE_SRCS += \
	$(LOCAL_DIR)/gpio_init.c

MODULE_DEPS += \
	platform/$(PLATFORM)/common/gpio \
	external/lib/fdt

include make/module.mk
