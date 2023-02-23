LOCAL_DIR := $(GET_LOCAL_DIR)

MODULE := $(LOCAL_DIR)

MODULE_SRCS += \
	$(LOCAL_DIR)/sspm_expdb.c \
	$(LOCAL_DIR)/sspm_loader.c

MODULE_DEPS += \
	platform/$(PLATFORM)/common/aee \
	platform/$(PLATFORM)/common/dtb_ops \
	platform/$(PLATFORM)/$(SUB_PLATFORM)/sspm

include make/module.mk
