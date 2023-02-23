LOCAL_DIR := $(GET_LOCAL_DIR)
MODULE := $(LOCAL_DIR)

PLATFORM := mediatek
SUB_PLATFORM := mt8195

MODULE_SRCS += \
	$(LOCAL_DIR)/target.c \

MODULE_DEPS += \
	platform/$(PLATFORM)/$(SUB_PLATFORM)/mmc \
	platform/$(PLATFORM)/common/mmc \

include make/module.mk
