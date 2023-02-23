LOCAL_DIR := $(GET_LOCAL_DIR)

MODULE := $(LOCAL_DIR)

MODULE_SRCS += \
	$(LOCAL_DIR)/atf_ramdump.c

MODULE_DEPS += \
	lib/mblock \
	platform/$(PLATFORM)/common/aee

include make/module.mk
