LOCAL_DIR := $(GET_LOCAL_DIR)

WITH_DISABLE_AVB_VERIFICATION := 1

TARGET := fpga6879_64

ifeq ($(LK_AS), BL2_EXT)
BOOT_APP ?= bl2_ext
else ifeq ($(LK_AS), AEE)
BOOT_APP ?= aee

MODULE_DEPS += app/fastboot \
	app/fastboot/device_lock \
	app/fastboot/flash \
	app/fastboot/usb
else
BOOT_APP ?= mt_boot

MODULE_DEPS += app/fastboot \
	app/fastboot/device_lock \
	app/fastboot/flash \
	app/fastboot/usb
endif

MODULE_DEPS += app/$(BOOT_APP)

MTK_LCM_COMMON_DRIVER_SUPPORT := no
MTK_SECURITY_SW_SUPPORT := yes
MTK_SECURITY_ANTI_ROLLBACK := no
