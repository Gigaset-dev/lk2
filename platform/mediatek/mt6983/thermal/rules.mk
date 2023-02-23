LOCAL_DIR := $(GET_LOCAL_DIR)

MODULE := $(LOCAL_DIR)

MODULE_SRCS += \
	$(LOCAL_DIR)/mt_thermal.c \
	$(LOCAL_DIR)/mt_thermal_sram_init.c \
	$(LOCAL_DIR)/mt_thermal_stat.c

MODULE_DEPS += \
	platform/$(PLATFORM)/common/aee

include make/module.mk

