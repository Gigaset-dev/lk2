LOCAL_DIR := $(GET_LOCAL_DIR)

MODULE := $(LOCAL_DIR)

MODULE_SRCS += $(LOCAL_DIR)/dvfs.c

MODULE_DEPS += \
	platform/$(PLATFORM)/common/aee \
	platform/$(PLATFORM)/$(SUB_PLATFORM)/devinfo

include make/module.mk

