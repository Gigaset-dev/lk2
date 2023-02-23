LOCAL_DIR := $(GET_LOCAL_DIR)

MODULE := $(LOCAL_DIR)

MODULE_SRCS += \
	$(LOCAL_DIR)/medmcu_plat.c

MODULE_DEPS += \
	platform/$(PLATFORM)/common/loader \
	platform/$(PLATFORM)/common/emi \
	lib/mblock

include make/module.mk
