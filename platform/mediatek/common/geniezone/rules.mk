LOCAL_DIR := $(GET_LOCAL_DIR)

MODULE := $(LOCAL_DIR)

MODULE_SRCS += \
	$(LOCAL_DIR)/gz_boot_tag.c \
	$(LOCAL_DIR)/gz_unmap2.c \

MODULE_DEPS += \
	lib/mblock\
	lib/pl_boottags \

include make/module.mk
