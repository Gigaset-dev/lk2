LOCAL_DIR := $(GET_LOCAL_DIR)

MODULE := $(LOCAL_DIR)

MODULE_SRCS += \
	$(LOCAL_DIR)/dm_verity.c \
	$(LOCAL_DIR)/dm_verity_status.c \
	$(LOCAL_DIR)/load_vfy_boot.c \
	$(LOCAL_DIR)/vboot_state.c

MODULE_DEPS += \
	app/fastboot \
	external/lib/libavb \
	external/lib/libavb_user \
	platform/$(PLATFORM)/common/avb_hal \
	platform/$(PLATFORM)/common/RoT \
	platform/$(PLATFORM)/common/sboot

include make/module.mk
