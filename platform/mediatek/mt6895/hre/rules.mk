LOCAL_DIR := $(GET_LOCAL_DIR)

MODULE := $(LOCAL_DIR)

MODULE_SRCS += \
	$(LOCAL_DIR)/infra_bus_hre_aee_dump.c \
	$(LOCAL_DIR)/mminfra_bus_hre_aee_dump.c

include make/module.mk