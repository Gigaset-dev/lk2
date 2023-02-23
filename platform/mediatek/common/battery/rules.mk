LOCAL_DIR := $(GET_LOCAL_DIR)

MODULE := $(LOCAL_DIR)

MODULE_DEPS := \
	lib/pl_boottags \
	platform/$(PLATFORM)/common/dtb_ops \
	platform/$(PLATFORM)/common/pmic \
	platform/$(PLATFORM)/common/pmic_auxadc \
	platform/$(PLATFORM)/common/pmic_dlpt

MODULE_SRCS += \
	$(LOCAL_DIR)/mtk_battery.c

include make/module.mk

