LOCAL_DIR := $(GET_LOCAL_DIR)
MODULE := $(LOCAL_DIR)

MODULE_DEPS += \
	lib/bio \
	lib/log_store \
	platform/$(PLATFORM)/common/aee \
	platform/$(PLATFORM)/common/debug \
	platform/$(PLATFORM)/common/pmic

MODULE_SRCS := \
	$(LOCAL_DIR)/log_store_lk.c

include make/module.mk
