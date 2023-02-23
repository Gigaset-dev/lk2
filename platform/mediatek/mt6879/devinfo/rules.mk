LOCAL_DIR := $(GET_LOCAL_DIR)

MODULE := $(LOCAL_DIR)

MODULE_SRCS += \
	$(LOCAL_DIR)/plat_efuse.c

ifneq ($(wildcard $(LOCAL_DIR)/plat_devinfo/*),)
MODULE_DEPS += \
	$(LOCAL_DIR)/plat_devinfo
else
MODULE_EXTRA_OBJS += \
	$(LOCAL_DIR)/lib/$(ARCH)/plat_devinfo.mod.o
endif

MODULE_DEPS += \
	platform/$(PLATFORM)/common/devinfo \
	platform/$(PLATFORM)/common/spmi

GLOBAL_DEFINES += \
	MTK_MT22_MODE=\"$(MTK_MT22_MODE)\"

include make/module.mk

