LOCAL_DIR := $(GET_LOCAL_DIR)

MODULE := $(LOCAL_DIR)

ifneq (, $(findstring fpga, $(PROJECT)))
MODULE_DEFINES += \
	PROJECT_TYPE_FPGA
endif

MODULE_SRCS += \
	$(LOCAL_DIR)/cust_msdc.c \
	$(LOCAL_DIR)/msdc_io.c

MODULE_DEPS += \
	platform/$(PLATFORM)/common/mmc \
	platform/$(PLATFORM)/common/subpmic

include make/module.mk

