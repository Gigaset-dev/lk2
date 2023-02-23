LOCAL_DIR := $(GET_LOCAL_DIR)

MODULE := $(LOCAL_DIR)

MODULE_SRCS += \
	$(LOCAL_DIR)/mtk_key.c

MODULE_DEPS += \
	platform/$(PLATFORM)/common/pmic

include make/module.mk

