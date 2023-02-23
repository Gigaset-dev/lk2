LOCAL_DIR := $(GET_LOCAL_DIR)
MODULE := $(LOCAL_DIR)

ifeq ($(LK_AS), BL2)
	RGU_STAGE ?= RGU_STAGE_PRELOADER
else ifeq ($(LK_AS), BL2_EXT)
	RGU_STAGE ?= RGU_STAGE_BL2_EXT
else ifeq ($(LK_AS), AEE)
	RGU_STAGE ?= RGU_STAGE_AEE
else
	RGU_STAGE ?= RGU_STAGE_LK
endif
MODULE_DEFINES += RGU_STAGE=$(RGU_STAGE)

MODULE_DEPS += \
	lib/watchdog \
	platform/$(PLATFORM)/common/dtb_ops \
	platform/$(PLATFORM)/common/log_store \

MODULE_SRCS := \
	$(LOCAL_DIR)/wdt.c \
	$(LOCAL_DIR)/watchdog.c

include make/module.mk
