LOCAL_DIR := $(GET_LOCAL_DIR)

MODULE := $(LOCAL_DIR)

MODULE_SRCS += \
	$(LOCAL_DIR)/gpueb_expdb.c \
	$(LOCAL_DIR)/gpueb_loader.c \

ifneq (, $(findstring fpga, $(PROJECT)))
MODULE_DEFINES += \
	GPUEB_FPGA_PLATFORM=1
endif

MODULE_DEPS += \
	platform/$(PLATFORM)/common/aee \
	platform/$(PLATFORM)/common/dtb_ops

include make/module.mk
