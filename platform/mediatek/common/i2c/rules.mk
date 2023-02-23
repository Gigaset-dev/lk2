LOCAL_DIR := $(GET_LOCAL_DIR)

MODULE := $(LOCAL_DIR)

MODULE_DEPS += \
	external/lib/fdt \
	platform/$(PLATFORM)/$(SUB_PLATFORM)/i2c \
	platform/$(PLATFORM)/common/gpio

MODULE_SRCS += \
	$(LOCAL_DIR)/mt_i2c.c \
	$(LOCAL_DIR)/i2c_set_timing_reg_v$(I2C_REG_MAP_VER).c

include make/module.mk
