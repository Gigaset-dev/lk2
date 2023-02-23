LOCAL_DIR := $(GET_LOCAL_DIR)

MODULE := $(LOCAL_DIR)

MODULE_DEPS += \
	lib/pl_boottags \
	platform/$(PLATFORM)/common/atm \
	platform/$(PLATFORM)/common/battery \
	platform/$(PLATFORM)/common/dtb_ops \
	platform/$(PLATFORM)/common/leds \
	platform/$(PLATFORM)/common/logo \
	platform/$(PLATFORM)/common/pmic \
	platform/$(PLATFORM)/common/pmic_auxadc \
	platform/$(PLATFORM)/common/wdt

MODULE_SRCS += \
	$(LOCAL_DIR)/mtk_charger.c \
	$(LOCAL_DIR)/mtk_charger_intf.c \

include make/module.mk
