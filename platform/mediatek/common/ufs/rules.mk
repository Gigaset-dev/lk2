LOCAL_DIR := $(GET_LOCAL_DIR)

MODULE := $(LOCAL_DIR)

ifneq (, $(findstring fpga, $(PROJECT)))
MODULE_DEFINES += \
	PROJECT_TYPE_FPGA
endif

MODULE_SRCS += \
	$(LOCAL_DIR)/ufs_core.c \
	$(LOCAL_DIR)/ufs_hcd.c \
	$(LOCAL_DIR)/ufs_interface.c \
	$(LOCAL_DIR)/ufs_quirks.c \
	$(LOCAL_DIR)/ufs_rpmb.c \
	$(LOCAL_DIR)/ufs_utils.c

MODULE_DEPS += \
	lib/bio \
	lib/partition \

include make/module.mk
