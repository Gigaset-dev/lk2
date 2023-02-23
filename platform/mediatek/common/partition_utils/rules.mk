LOCAL_DIR := $(GET_LOCAL_DIR)

MODULE := $(LOCAL_DIR)

MTK_USERIMAGES_FORMAT ?= f2fs
PART_BOOTDEVICE_NAME ?= bootdevice

MODULE_DEFINES += \
	MTK_USERIMAGES_FORMAT=\"$(MTK_USERIMAGES_FORMAT)\" \

GLOBAL_DEFINES += \
	PART_BOOTDEVICE_NAME=\"$(PART_BOOTDEVICE_NAME)\" \

MODULE_DEPS += \
	external/lib/cksum \
	lib/bio \
	lib/partition \
	platform/$(PLATFORM)/common/bootctrl/3 \
	platform/$(PLATFORM)/common/ufs

MODULE_SRCS += \
	$(LOCAL_DIR)/debug.c \
	$(LOCAL_DIR)/efi.c \
	$(LOCAL_DIR)/partition_utils.c \
	$(LOCAL_DIR)/partition_wp.c \
	$(LOCAL_DIR)/table.c

include make/module.mk
