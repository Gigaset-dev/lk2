LOCAL_DIR := $(GET_LOCAL_DIR)

MODULE := $(LOCAL_DIR)

PLATFORM := mediatek
SUB_PLATFORM := mt6983

MODULE_SRCS += \
	$(LOCAL_DIR)/board_hw_rev.c

MODULE_DEPS += \
	platform/$(PLATFORM)/common/gpio

include make/module.mk
