LOCAL_DIR := $(GET_LOCAL_DIR)

MODULE := $(LOCAL_DIR)

MODULE_INCLUDES += platform/$(PLATFORM)/$(SUB_PLATFORM)/devinfo/

MODULE_SRCS += \
	$(LOCAL_DIR)/chip_id.c \
	$(LOCAL_DIR)/efuse.c

MODULE_DEPS += \
	external/lib/fdt \
	lib/bio \
	platform/$(PLATFORM)/common/dtb_ops

MTK_EFUSE_WRITER_SUPPORT ?= no

ifeq ($(MTK_EFUSE_WRITER_SUPPORT), yes)
MODULE_DEFINES += MTK_EFUSE_WRITER_SUPPORT
MODULE_DEPS += platform/$(PLATFORM)/common/video
endif

include make/module.mk
