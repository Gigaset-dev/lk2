LOCAL_DIR := $(GET_LOCAL_DIR)

MODULE := $(LOCAL_DIR)

ifneq (, $(findstring fpga, $(PROJECT)))
MODULE_DEFINES += \
	PROJECT_TYPE_FPGA
endif

MODULE_SRCS += \
	$(LOCAL_DIR)/platform_pmif_spmi.c

include make/module.mk
