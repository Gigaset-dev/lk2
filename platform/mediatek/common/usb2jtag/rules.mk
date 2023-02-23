LOCAL_DIR := $(GET_LOCAL_DIR)

MODULE := $(LOCAL_DIR)

MODULE_SRCS += \
	$(LOCAL_DIR)/usb2jtag.c

MODULE_DEPS += \
	platform/$(PLATFORM)/common/sysenv

include make/module.mk
