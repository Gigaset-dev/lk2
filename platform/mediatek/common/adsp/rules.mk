LOCAL_DIR := $(GET_LOCAL_DIR)

MODULE := $(LOCAL_DIR)

MODULE_SRCS += \
	$(LOCAL_DIR)/adsp.c

MODULE_DEPS += \
	external/lib/fdt

MODULE_INCLUDES += \
	$(LOCAL_DIR)/$(ADSP_GEN)

include make/module.mk
