LOCAL_DIR := $(GET_LOCAL_DIR)

TARGET := k6789v1_64
DROIDBOOT_PLATFORM := GX4
ifeq ($(LK_AS), BL2_EXT)
BOOT_APP ?= bl2_ext
else ifeq ($(LK_AS), AEE)
BOOT_APP ?= aee

MODULE_DEPS += app/fastboot \
	app/fastboot/usb
else
BOOT_APP ?= mt_boot

MODULE_DEPS += app/fastboot \
	app/fastboot/device_lock \
	app/fastboot/flash \
	app/fastboot/usb \
	app/droidboot_gui
endif

MODULE_DEPS += app/$(BOOT_APP)

MTK_LCM_COMMON_DRIVER_SUPPORT := no
MTK_LCM_LIST_SUPPORT="td4330_fhdp_dsi_vdo_auo_rt5081"
MTK_LCM_PHYSICAL_ROTATION = 0
#prize add by lipengpeng 20220704 start 
MTK_TEE_DRAM_SIZE = 0x3200000
#prize add by lipengpeng 20220704 end 
MTK_RT4831A_SUPPORT = 1
#prize add by lipengpeng 20220704 start 
MICROTRUST_TEE_SUPPORT = yes
MICROTRUST_TEE_LOG_ENC_DISABLE = yes
#prize add by lipengpeng 20220704 end 
MTK_SECURITY_SW_SUPPORT := yes
MTK_SECURITY_ANTI_ROLLBACK := no
