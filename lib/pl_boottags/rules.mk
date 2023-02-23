LOCAL_DIR := $(GET_LOCAL_DIR)

MODULE := $(LOCAL_DIR)

MODULE_SRCS := $(LOCAL_DIR)/pl_boottags.c

EXTRA_LINKER_SCRIPTS += $(LOCAL_DIR)/pl_boottags.ld

include make/module.mk
