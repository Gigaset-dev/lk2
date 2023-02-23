LOCAL_DIR := $(GET_LOCAL_DIR)

MODULE := $(LOCAL_DIR)

MODULE_SRCS += \
	$(LOCAL_DIR)/cust_msdc.c \
	$(LOCAL_DIR)/msdc_io.c

MODULE_DEPS += \
	platform/$(PLATFORM)/common/gpio \
	platform/$(PLATFORM)/common/mmc

include make/module.mk
