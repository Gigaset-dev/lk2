LOCAL_DIR := $(GET_LOCAL_DIR)

MODULE := $(LOCAL_DIR)

MODULE_SRCS += \
	$(LOCAL_DIR)/scp.c \
	$(LOCAL_DIR)/scp_excep.c

MODULE_DEPS += \
	external/lib/fdt \
	platform/$(PLATFORM)/common/aee

include make/module.mk
