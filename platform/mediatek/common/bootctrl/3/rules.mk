LOCAL_DIR := $(GET_LOCAL_DIR)

MODULE := $(LOCAL_DIR)

MODULE_DEPS += \
	external/lib/cksum

MODULE_SRCS += \
	$(LOCAL_DIR)/bootctrl_api.c

GLOBAL_DEFINES += MTK_WITH_COMMON_BOOTCTRL=3

include make/module.mk
