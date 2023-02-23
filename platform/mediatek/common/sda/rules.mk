LOCAL_DIR := $(GET_LOCAL_DIR)

MODULE := $(LOCAL_DIR)

MODULE_SRCS += \
	$(LOCAL_DIR)/dbgtop_drm.c \
	$(LOCAL_DIR)/dfd_mcu.c \
	$(LOCAL_DIR)/etb.c \
	$(LOCAL_DIR)/lastbus.c \
	$(LOCAL_DIR)/lastpc.c \
	$(LOCAL_DIR)/plat_dbg_info.c \
	$(LOCAL_DIR)/plat_debug.c \
	$(LOCAL_DIR)/return_stack.c \
	$(LOCAL_DIR)/tracker.c

MODULE_DEPS += \
	external/lib/fdt \
	platform/$(PLATFORM)/common/aee \
	platform/$(PLATFORM)/common/dtb_ops \
	platform/$(PLATFORM)/common/wdt

include make/module.mk
