LOCAL_DIR := $(GET_LOCAL_DIR)

MODULE := $(LOCAL_DIR)

MODULE_SRCS += \
	$(LOCAL_DIR)/nftl_bdev.c \
	$(LOCAL_DIR)/nftl_core.c \
	$(LOCAL_DIR)/nftl_part.c \

MODULE_DEPS += \
	lib/bio

include make/module.mk
