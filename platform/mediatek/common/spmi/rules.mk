LOCAL_DIR := $(GET_LOCAL_DIR)

MODULE := $(LOCAL_DIR)

ifneq (, $(findstring fpga, $(PROJECT)))
MODULE_DEFINES += \
	PROJECT_TYPE_FPGA
endif

MODULE_DEPS += \
	platform/$(PLATFORM)/$(SUB_PLATFORM)/spmi \

MODULE_SRCS += \
	$(LOCAL_DIR)/pmif_common.c \
	$(LOCAL_DIR)/spmi_common.c \

include make/module.mk
