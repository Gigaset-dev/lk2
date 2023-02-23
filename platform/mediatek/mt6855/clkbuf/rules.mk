LOCAL_DIR := $(GET_LOCAL_DIR)

MODULE := $(LOCAL_DIR)

MODULE_SRCS += \
	$(LOCAL_DIR)/clkbuf.c \
	$(LOCAL_DIR)/srclken_rc_aee.c

MODULE_DEPS += \
	external/lib/fdt \
	platform/$(PLATFORM)/common/clkbuf \
	platform/$(PLATFORM)/common/spmi

include make/module.mk
