LOCAL_DIR := $(GET_LOCAL_DIR)
MODULE := $(LOCAL_DIR)

GLOBAL_INCLUDES += \
	platform/$(PLATFORM)/common/pll/include

MODULE_SRCS += \
	$(LOCAL_DIR)/pll.c \
	platform/$(PLATFORM)/common/pll/pll.c

FMETER_EN ?= 0

ifneq ($(FMETER_EN),0)
MODULE_SRCS += $(LOCAL_DIR)/fmeter.c
MODULE_DEFINES += FMETER_EN=$(FMETER_EN)
endif

include make/module.mk
