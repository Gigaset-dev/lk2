LOCAL_DIR := $(GET_LOCAL_DIR)

MODULE := $(LOCAL_DIR)

MODULE_SRCS += \
	$(LOCAL_DIR)/fastboot.c \
	$(LOCAL_DIR)/fastboot_entry.c \
	$(LOCAL_DIR)/sys_commands.c \
	$(LOCAL_DIR)/variables.c

MODULE_DEPS += \
	lib/log_store \
	lib/version \
	platform/$(PLATFORM)/common/aee \
	platform/$(PLATFORM)/common/log_store \
	platform/$(PLATFORM)/common/partition_utils \
	platform/$(PLATFORM)/common/sboot \
	platform/$(PLATFORM)/common/usb2jtag


EXTRA_LINKER_SCRIPTS += $(LOCAL_DIR)/fastboot_oem_cmd.ld

include make/module.mk

