LOCAL_DIR := $(GET_LOCAL_DIR)

MODULE := $(LOCAL_DIR)

MODULE_SRCS += $(LOCAL_DIR)/plat_auxadc.c

MODULE_DEPS += \
	platform/$(PLATFORM)/common/auxadc

include make/module.mk

