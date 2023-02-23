LOCAL_DIR := $(GET_LOCAL_DIR)

MODULE := $(LOCAL_DIR)

MODULE_SRCS += \
	$(LOCAL_DIR)/device_lock.c

MODULE_DEPS += \
	app/fastboot \
	lib/bio \
	platform/$(PLATFORM)/common/keypad \
	platform/$(PLATFORM)/$(SUB_PLATFORM)/sboot

include make/module.mk
