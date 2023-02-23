LOCAL_DIR := $(GET_LOCAL_DIR)
MODULE := $(LOCAL_DIR)

MODULE_INCLUDES += platform/$(PLATFORM)/$(SUB_PLATFORM)/gpio/

MODULE_SRCS += \
	$(LOCAL_DIR)/gpio.c

ifeq ($(USE_MT6373_GPIO), yes)
MODULE_DEFINES += USE_MT6373_GPIO
MODULE_SRCS += $(LOCAL_DIR)/gpio_ext.c
MODULE_DEPS += platform/$(PLATFORM)/common/spmi
endif

include make/module.mk
