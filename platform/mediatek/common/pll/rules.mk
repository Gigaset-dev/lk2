LOCAL_DIR := $(GET_LOCAL_DIR)
MODULE := $(LOCAL_DIR)

MODULE_INCLUDES += platform/$(PLATFORM)/$(SUB_PLATFORM)/pll/

MODULE_SRCS += \
	$(LOCAL_DIR)/pll.c

include make/module.mk
