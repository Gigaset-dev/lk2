LOCAL_DIR := $(GET_LOCAL_DIR)

MODULE := $(LOCAL_DIR)

MODULE_SRCS += \
	$(LOCAL_DIR)/plat_dfd_mcu.c

GLOBAL_DEFINES += PLAT_DFD_MCU_READY

include make/module.mk
