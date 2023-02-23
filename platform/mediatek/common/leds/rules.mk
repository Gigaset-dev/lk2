LOCAL_DIR := $(GET_LOCAL_DIR)

MODULE := $(LOCAL_DIR)

MODULE_SRCS += \
	$(LOCAL_DIR)/leds.c  \
	$(LOCAL_DIR)/leds_disp_pwm.c

MODULE_DEPS += \
	platform/$(PLATFORM)/$(SUB_PLATFORM)/disp

include make/module.mk
