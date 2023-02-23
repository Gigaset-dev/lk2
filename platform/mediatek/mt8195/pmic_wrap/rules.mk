LOCAL_DIR := $(GET_LOCAL_DIR)

MODULE := $(LOCAL_DIR)

GLOBAL_INCLUDES += platform/$(PLATFORM)/common/pmic_wrap/include

MODULE_DEPS += \
	platform/$(PLATFORM)/$(SUB_PLATFORM)/spmi

MODULE_SRCS += \
	$(LOCAL_DIR)/platform_pmif_spi.c \
	platform/$(PLATFORM)/common/pmic_wrap/pmic_wrap_common_v2.c \

include make/module.mk
