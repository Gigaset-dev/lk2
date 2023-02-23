LOCAL_DIR := $(GET_LOCAL_DIR)

MODULE := $(LOCAL_DIR)

MODULE_SRCS += \
	$(LOCAL_DIR)/api_hre_mminfra.c \
	$(LOCAL_DIR)/mminfra.c \

include make/module.mk

