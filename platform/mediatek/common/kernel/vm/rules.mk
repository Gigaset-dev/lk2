LOCAL_DIR := $(GET_LOCAL_DIR)
MODULE := $(LOCAL_DIR)

MODULE_SRCS := \
	$(LOCAL_DIR)/bootalloc.c

include make/module.mk
