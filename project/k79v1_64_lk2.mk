LOCAL_DIR := $(GET_LOCAL_DIR)

WITH_SMP := 0

TARGET := k79v1_64_lk2

ifeq ($(LK_AS), AEE)
BOOT_APP := aee
endif

BOOT_APP ?= mt_boot

MODULE_DEPS += \
	app/$(BOOT_APP) \
	app/fastboot \
	app/fastboot/device_lock \
	app/fastboot/flash \
	app/fastboot/usb

MTK_LCM_COMMON_DRIVER_SUPPORT := no
MTK_LCM_LIST_SUPPORT="hx83112b_fhdp_dsi_cmd_auo_rt4801"
MTK_LCM_PHYSICAL_ROTATION = 0
