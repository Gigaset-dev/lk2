LOCAL_DIR := $(GET_LOCAL_DIR)

MODULE := $(LOCAL_DIR)

MODULE_SRCS += \
	$(LOCAL_DIR)/plat_efuse.c

ifeq ($(BUILD_DEVINFO_LIB),yes)
MODULE_DEPS += \
	$(LOCAL_DIR)/plat_devinfo
else
MODULE_EXTRA_OBJS += \
	$(LOCAL_DIR)/lib/plat_devinfo.mod.o
endif

MODULE_DEPS += \
	platform/$(PLATFORM)/common/devinfo

include make/module.mk

