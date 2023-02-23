LOCAL_DIR := $(GET_LOCAL_DIR)

MODULE := $(LOCAL_DIR)

MODULE_SRCS += \
	$(LOCAL_DIR)/pcie.c

MODULE_DEPS += \
	platform/$(PLATFORM)/$(SUB_PLATFORM)/cldma

include make/module.mk
