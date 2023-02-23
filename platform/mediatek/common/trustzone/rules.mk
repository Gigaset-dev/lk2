LOCAL_DIR := $(GET_LOCAL_DIR)

MODULE := $(LOCAL_DIR)

MODULE_SRCS += \
	$(LOCAL_DIR)/tz_tags.c

MODULE_DEPS += \
	lib/mblock \
	platform/$(PLATFORM)/common/aee \
	platform/$(PLATFORM)/common/ufs

ifeq ($(MTK_PAC_BTI_ENABLE),yes)
MODULE_DEFINES += MTK_PAC_BTI_ENABLE
endif

include make/module.mk
