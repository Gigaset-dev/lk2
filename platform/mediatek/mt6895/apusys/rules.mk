LOCAL_DIR := $(GET_LOCAL_DIR)

MODULE := $(LOCAL_DIR)

MODULE_INCLUDES += \
	platform/$(PLATFORM)/common/apusys_rv

MODULE_SRCS += \
	$(LOCAL_DIR)/apupw.c \
	$(LOCAL_DIR)/apusys.c \

include make/module.mk
