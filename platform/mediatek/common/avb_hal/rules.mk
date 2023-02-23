LOCAL_DIR := $(GET_LOCAL_DIR)

MODULE := $(LOCAL_DIR)

MODULE_SRCS += \
	$(LOCAL_DIR)/avb_hal.c \
	$(LOCAL_DIR)/avb_hal_cmdline.c \
	$(LOCAL_DIR)/avb_persist.c \
	$(LOCAL_DIR)/avb_sha256_crypto_hw.c

MODULE_INCLUDES += \
	target/$(TARGET)/include

MODULE_DEPS += \
	external/lib/libavb \
	lib/bio \
	platform/$(PLATFORM)/$(SUB_PLATFORM)/sboot

GLOBAL_DEFINES += AVB_COMPILATION

include make/module.mk
