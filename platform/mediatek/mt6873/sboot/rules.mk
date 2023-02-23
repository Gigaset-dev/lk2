LOCAL_DIR := $(GET_LOCAL_DIR)

MODULE := $(LOCAL_DIR)

MODULE_SRCS += \
	$(LOCAL_DIR)/vb_init.c

MODULE_INCLUDES += \
	target/$(PROJECT)/include

include make/module.mk
