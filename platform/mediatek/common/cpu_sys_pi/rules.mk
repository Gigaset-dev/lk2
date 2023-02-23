LOCAL_DIR := $(GET_LOCAL_DIR)

MODULE := $(LOCAL_DIR)

MODULE_SRCS += \
	$(LOCAL_DIR)/cpu_sys_pi_expdb.c

MODULE_DEPS += \
	platform/$(PLATFORM)/common/aee

include make/module.mk
