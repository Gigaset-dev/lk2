LOCAL_DIR := $(GET_LOCAL_DIR)

MODULE := $(LOCAL_DIR)

ifneq (, $(findstring fpga, $(PROJECT)))
MODULE_DEFINES += \
	PROJECT_TYPE_FPGA
endif

CHECK_HAS_BATTERY_REMOVED ?= 0
ifeq ($(CHECK_HAS_BATTERY_REMOVED),1)
GLOBAL_DEFINES += CHECK_HAS_BATTERY_REMOVED
endif

# It depends on new pmic auxadc framework because it registers
# new pmic auxadc so that user can use new pmic auxadc API
# to access old drivers which were developed based on legacy
# pmic auxadc framework.
MODULE_DEPS += \
	platform/$(PLATFORM)/common/pmic_auxadc \

WITH_PMIC_LEGACY_AUXADC ?= 0

ifeq ($(WITH_PMIC_LEGACY_AUXADC),1)
MODULE_SRCS += \
	$(LOCAL_DIR)/pmic_auxadc_legacy.c
endif

MODULE_SRCS += \
	$(LOCAL_DIR)/pmic_keys.c \
	$(LOCAL_DIR)/pmic_psc.c \

include make/module.mk

