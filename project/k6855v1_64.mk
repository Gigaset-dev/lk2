LOCAL_DIR := $(GET_LOCAL_DIR)

TARGET := k6855v1_64

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
GLOBAL_DEFINES += MTK_ROUND_CORNER_SUPPORT

MTK_LCM_COMMON_DRIVER_SUPPORT := no
MTK_LCM_LIST_SUPPORT="nt36672e_fhdp_dsi_vdo_120hz_jdi_dphy nt36672e_fhdp_dsi_vdo_60hz_jdi_dphy td4330_fhdp_dsi_cmd_auo_rt5081"
MTK_LCM_PHYSICAL_RPTATION = 0
MTK_RT4831A_SUPPORT = 1
MTK_RUNTIME_SWITCH_FPS_SUPPORT := yes
MTK_SECURITY_SW_SUPPORT := yes
MTK_SECURITY_ANTI_ROLLBACK := no
