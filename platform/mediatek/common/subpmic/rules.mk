LOCAL_DIR := $(GET_LOCAL_DIR)

MODULE := $(LOCAL_DIR)

ifneq (, $(findstring fpga, $(PROJECT)))
MODULE_DEFINES += \
	PROJECT_TYPE_FPGA
endif

MODULE_INCLUDES += platform/$(PLATFORM)/$(SUB_PLATFORM)/include

MODULE_SRCS += \
	$(LOCAL_DIR)/subpmic_common.c \

include make/module.mk

