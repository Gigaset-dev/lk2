LOCAL_DIR := $(GET_LOCAL_DIR)

TARGET := k6880v1_mdot2_datacard

ifeq ($(LK_AS), AEE)
	BOOT_APP := aee
endif

BOOT_APP ?= gem_boot

GLOBAL_DEFINES += \
	CUSTOM_FASTBOOT_BUF_SIZE=0

WITH_MRDUMP_PCIE := 1

WITH_SBOOT_ASSERT_ENABLE := 1
WITH_SBOOT_ANTI_ROLLBACK_ENABLE := 1

MODULE_DEPS += \
	app/$(BOOT_APP) \
	app/fastboot \
	app/fastboot/pcie \
	app/fastboot/usb
