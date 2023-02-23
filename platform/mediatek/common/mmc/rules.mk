LOCAL_DIR := $(GET_LOCAL_DIR)

MODULE := $(LOCAL_DIR)

ifneq (, $(findstring fpga, $(PROJECT)))
MODULE_DEFINES += \
	PROJECT_TYPE_FPGA
endif

MODULE_INCLUDES += \
	platform/$(PLATFORM)/$(SUB_PLATFORM)/mmc

MODULE_SRCS += \
	$(LOCAL_DIR)/msdc.c \
	$(LOCAL_DIR)/mmc_core.c \
	$(LOCAL_DIR)/mmc_rpmb.c

MODULE_DEPS += \
	lib/bio \
	lib/partition

include make/module.mk
