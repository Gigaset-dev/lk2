LOCAL_DIR := $(GET_LOCAL_DIR)

MODULE := $(LOCAL_DIR)

BOOT_LOGO ?= fhdplus
GLOBAL_DEFINES += $(shell echo $(BOOT_LOGO) | tr a-z A-Z)

MODULE_SRCS += \
	$(LOCAL_DIR)/decompress_common.c \
	$(LOCAL_DIR)/show_animation_common.c \
	$(LOCAL_DIR)/show_logo_common.c

MODULE_DEPS += \
	external/lib/zlib\

include make/module.mk
