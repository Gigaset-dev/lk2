LOCAL_DIR := $(GET_LOCAL_DIR)
MODULE := $(LOCAL_DIR)

MODULE_DEPS += \
	platform/$(PLATFORM)/common/pll

MODULE_SRCS += \
	$(LOCAL_DIR)/pll.c \
	$(LOCAL_DIR)/fmeter.c

FMETER_EN ?= 0

ifneq ($(FMETER_EN),0)
MODULE_DEFINES += FMETER_EN=$(FMETER_EN)
endif

include make/module.mk
