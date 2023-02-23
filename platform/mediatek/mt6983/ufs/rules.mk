LOCAL_DIR := $(GET_LOCAL_DIR)

MODULE := $(LOCAL_DIR)

ifneq (, $(findstring fpga, $(PROJECT)))
MODULE_DEFINES += \
	PROJECT_TYPE_FPGA
endif

MODULE_SRCS += \
	$(LOCAL_DIR)/ufs_platform.c

MODULE_DEPS += \
	lib/kcmdline \
	platform/$(PLATFORM)/common/ufs

include make/module.mk

