LOCAL_DIR := $(GET_LOCAL_DIR)

WITH_DISABLE_AVB_VERIFICATION := 1

TARGET := k6893v1_64_swrgo_lk2

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
MTK_LCM_LIST_SUPPORT="ft8756_fhdp_dsi_vdo_auo_rt4801 td4330_fhdp_dsi_vdo_auo_rt5081"
MTK_LCM_PHYSICAL_ROTATION = 0
MTK_MT22_MODE := H1
