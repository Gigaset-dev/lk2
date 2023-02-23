LOCAL_DIR := $(GET_LOCAL_DIR)

MODULE := $(LOCAL_DIR)

# Config setting for allocating kernel image
# KERNEL_ALIGN align with kernel 64-bit text_offset setting
# DYNAMIC_KERNEL_MAX_SIZE default is 100MB
# DYNAMIC_LIMIT defaut is MBLOCK_NO_LIMIT(0xffffffffffffffff)
KERNEL_ALIGN ?= 0x80000
DYNAMIC_KERNEL_MAX_SIZE ?= 0x06400000
DYNAMIC_LIMIT ?= 0xffffffffffffffff

MODULE_DEFINES += \
	DYNAMIC_KERNEL_MAX_SIZE=$(DYNAMIC_KERNEL_MAX_SIZE) \
	DYNAMIC_LIMIT=$(DYNAMIC_LIMIT) \
	KERNEL_ALIGN=$(KERNEL_ALIGN)

MODULE_SRCS += \
	$(LOCAL_DIR)/boot_info.c \
	$(LOCAL_DIR)/boot_mode.c \
	$(LOCAL_DIR)/decompressor.c \
	$(LOCAL_DIR)/fdt_para.c \
	$(LOCAL_DIR)/gem_boot.c \
	$(LOCAL_DIR)/io_task.c \
	$(LOCAL_DIR)/meta.c

MODULE_DEPS += \
	external/lib/fdt \
	external/lib/ufdt \
	external/lib/zlib \
	lib/dpc \
	lib/kcmdline \
	lib/mblock \
	$(LOCAL_DIR)/vboot \
	platform/$(PLATFORM)/common/dtb_ops \
	platform/$(PLATFORM)/common/loader \
	platform/$(PLATFORM)/common/sboot

include make/module.mk
