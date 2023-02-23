LOCAL_DIR := $(GET_LOCAL_DIR)

MODULE := $(LOCAL_DIR)

MODULE_SRCS += \
	platform/$(PLATFORM)/common/ssusb/mtu3.c \
	platform/$(PLATFORM)/common/ssusb/mtu3_qmu.c \

MODULE_DEPS += \
	dev/interrupt/arm_gic_v3 \

include make/module.mk
