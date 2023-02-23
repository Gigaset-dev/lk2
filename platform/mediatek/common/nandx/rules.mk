LOCAL_DIR := $(GET_LOCAL_DIR)

MODULE := $(LOCAL_DIR)

MODULE_SRCS += \
	$(LOCAL_DIR)/driver/bbt/bbt.c

MODULE_DEPS += \
	$(LOCAL_DIR)/core \

include make/module.mk
