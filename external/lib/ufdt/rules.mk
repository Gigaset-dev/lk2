LOCAL_DIR := $(GET_LOCAL_DIR)

MODULE := $(LOCAL_DIR)

GLOBAL_INCLUDES += $(LOCAL_DIR)/sysdeps/include

MODULE_SRCS += \
	$(LOCAL_DIR)/sysdeps/libufdt_sysdeps_vendor.c \
	$(LOCAL_DIR)/ufdt_convert.c \
	$(LOCAL_DIR)/ufdt_node.c \
	$(LOCAL_DIR)/ufdt_node_pool.c \
	$(LOCAL_DIR)/ufdt_overlay.c \
	$(LOCAL_DIR)/ufdt_prop_dict.c

include make/module.mk
