LOCAL_DIR := $(GET_LOCAL_DIR)

TARGET := evb6890v1_64_cpe

ifeq ($(LK_AS), AEE)
	BOOT_APP := aee
endif

BOOT_APP ?= gem_boot

GLOBAL_DEFINES += \
	CUSTOM_FASTBOOT_BUF_SIZE=0

WITH_SBOOT_ASSERT_ENABLE := 1
WITH_SBOOT_ANTI_ROLLBACK_ENABLE := 1

MODULE_DEPS += \
	app/$(BOOT_APP)
