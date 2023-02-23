LOCAL_DIR := $(GET_LOCAL_DIR)

MODULE := $(LOCAL_DIR)

MODULE_SRCS += \
	$(LOCAL_DIR)/mt6359.c \

MODULE_DEPS += \
	platform/$(PLATFORM)/$(SUB_PLATFORM)/pmic_wrap \

include make/module.mk
