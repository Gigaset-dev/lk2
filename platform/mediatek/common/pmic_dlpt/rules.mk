LOCAL_DIR := $(GET_LOCAL_DIR)

MODULE := $(LOCAL_DIR)

ifneq (, $(findstring fpga, $(PROJECT)))
MODULE_DEFINES += \
	PROJECT_TYPE_FPGA
endif

ifeq ($(PMIC_DUMMY_LOAD),MT6359)
GLOBAL_DEFINES += PMIC_MT6359_DUMMY_LOAD
else ifeq ($(PMIC_DUMMY_LOAD),MT6363)
GLOBAL_DEFINES += PMIC_MT6363_DUMMY_LOAD
endif

MODULE_SRCS += \
	$(LOCAL_DIR)/pmic_dlpt.c \

MODULE_DEPS += \
	platform/$(PLATFORM)/common/battery \
	platform/$(PLATFORM)/common/charger \
	platform/$(PLATFORM)/common/dtb_ops \
	platform/$(PLATFORM)/common/pmic \

include make/module.mk

