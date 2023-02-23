LOCAL_DIR := $(GET_LOCAL_DIR)

MODULE := $(LOCAL_DIR)

GLOBAL_INCLUDES += platform/$(PLATFORM)/common/pmic_wrap/include

MODULE_SRCS += \
	platform/$(PLATFORM)/common/pmic_wrap/pmic_wrap_common.c

include make/module.mk
