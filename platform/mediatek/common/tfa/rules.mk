LOCAL_DIR := $(GET_LOCAL_DIR)

MODULE := $(LOCAL_DIR)

MODULE_SRCS += \
	$(LOCAL_DIR)/tfa_debug_buf.c

MODULE_DEPS += \
	lib/mblock \
	platform/$(PLATFORM)/common/aee

include make/module.mk
