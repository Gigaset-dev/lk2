LOCAL_DIR := $(GET_LOCAL_DIR)

MODULE := $(LOCAL_DIR)

MODULE_DEPS += \
	platform/$(PLATFORM)/common/i2c \

MODULE_SRCS += \
	$(LOCAL_DIR)/regmap_i2c.c \
	$(LOCAL_DIR)/regmap.c \

include make/module.mk
