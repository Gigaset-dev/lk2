LOCAL_DIR := $(GET_LOCAL_DIR)

WITH_DISABLE_AVB_VERIFICATION := 1

TARGET := k6873v1_64_bl2_ext

ifeq ($(LK_AS), BL2_EXT)
BOOT_APP ?= bl2_ext
GLOBAL_DEFINES += LEGACY_BL31
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
MTK_LCM_LIST_SUPPORT="td4330_fhdp_dsi_vdo_auo_rt5081"
MTK_LCM_PHYSICAL_ROTATION = 0
MTK_MT22_MODE := H2
