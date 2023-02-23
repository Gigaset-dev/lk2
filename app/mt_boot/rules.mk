LOCAL_DIR := $(GET_LOCAL_DIR)

MODULE := $(LOCAL_DIR)

GLOBAL_INCLUDES += \
	platform/$(PLATFORM)/$(SUB_PLATFORM)/include

# Choose one of following values
# 1: disabled 2: permissive 3: enforcing
SELINUX_STATUS ?= 3
MODULE_DEFINES += \
	SELINUX_STATUS=$(SELINUX_STATUS) \

MTK_USB_UNIQUE_SERIAL ?= no
#PRIZE-uique serial-tangan-20170920-start
#ifeq ($(MTK_USB_UNIQUE_SERIAL),yes)
MODULE_DEFINES += MTK_USB_UNIQUE_SERIAL
#endif
#PRIZE-uique serial-tangan-20170920-end

MODULE_SRCS += \
	$(LOCAL_DIR)/boot_info.c \
	$(LOCAL_DIR)/boot_mode.c \
	$(LOCAL_DIR)/boot_ui.c \
	$(LOCAL_DIR)/bootconfig.c \
	$(LOCAL_DIR)/decompressor.c \
	$(LOCAL_DIR)/fdt_para.c \
	$(LOCAL_DIR)/io_task.c \
	$(LOCAL_DIR)/met.c \
	$(LOCAL_DIR)/meta.c \
	$(LOCAL_DIR)/mt_boot.c\
	$(LOCAL_DIR)/serialno.c

ifeq ($(MTK_DM_OTA),yes)
MODULE_SRCS += \
	$(LOCAL_DIR)/dm_ota.c
endif

MODULE_DEPS += \
	external/lib/fdt \
	external/lib/ufdt \
	external/lib/zlib \
	lib/dpc \
	lib/kcmdline \
	lib/mblock \
	$(LOCAL_DIR)/avb \
	platform/$(PLATFORM)/common/aee \
	platform/$(PLATFORM)/common/atm \
	platform/$(PLATFORM)/common/bootctrl/3 \
	platform/$(PLATFORM)/common/dconfig \
	platform/$(PLATFORM)/common/dtb_ops \
	platform/$(PLATFORM)/common/loader \
	platform/$(PLATFORM)/common/sboot \
	platform/$(PLATFORM)/common/sysenv

include make/module.mk
