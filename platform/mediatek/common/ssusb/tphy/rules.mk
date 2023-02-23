LOCAL_DIR := $(GET_LOCAL_DIR)

MODULE := $(LOCAL_DIR)

MODULE_SRCS += \
	platform/$(PLATFORM)/common/ssusb/tphy/usbphy.c \

include make/module.mk
