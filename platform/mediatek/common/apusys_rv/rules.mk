LOCAL_DIR := $(GET_LOCAL_DIR)

MODULE := $(LOCAL_DIR)

MODULE_DEPS += \
	lib/mblock \
	platform/mediatek/common/partition_utils \
	platform/$(PLATFORM)/common/loader \

MODULE_SRCS += \
	$(LOCAL_DIR)/apusys_rv.c \

include make/module.mk
