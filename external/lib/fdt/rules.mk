LOCAL_DIR := $(GET_LOCAL_DIR)

MODULE := $(LOCAL_DIR)

GLOBAL_INCLUDES += $(LOCAL_DIR)

MODULE_SRCS += \
	$(LOCAL_DIR)/fdt.c \
	$(LOCAL_DIR)/fdt_ro.c \
	$(LOCAL_DIR)/fdt_rw.c \
	$(LOCAL_DIR)/fdt_strerror.c \
	$(LOCAL_DIR)/fdt_sw.c \
	$(LOCAL_DIR)/fdt_empty_tree.c \
	$(LOCAL_DIR)/fdt_addresses.c \
	$(LOCAL_DIR)/fdt_wip.c \
	$(LOCAL_DIR)/fdt_overlay.c

include make/module.mk
