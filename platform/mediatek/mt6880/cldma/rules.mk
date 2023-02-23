LOCAL_DIR := $(GET_LOCAL_DIR)

MODULE := $(LOCAL_DIR)

MODULE_SRCS += \
	$(LOCAL_DIR)/cldma.c \
	$(LOCAL_DIR)/cldma_reg_ops.c

include make/module.mk