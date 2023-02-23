LOCAL_DIR := $(GET_LOCAL_DIR)

MODULE := $(LOCAL_DIR)

MODULE_SRCS += \
	$(LOCAL_DIR)/mt_logo.c

MODULE_DEPS += \
	lib/libshowlogo \
	lib/mblock \
	platform/$(PLATFORM)/common/loader \
	platform/$(PLATFORM)/$(SUB_PLATFORM)/disp

include make/module.mk
