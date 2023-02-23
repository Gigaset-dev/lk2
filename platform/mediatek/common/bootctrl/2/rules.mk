LOCAL_DIR := $(GET_LOCAL_DIR)

MODULE := $(LOCAL_DIR)

MODULE_SRCS += \
	$(LOCAL_DIR)/bootctrl_api.c

MODULE_DEPS += \
	lib/bio \
	lib/kcmdline

GLOBAL_DEFINES += MTK_WITH_COMMON_BOOTCTRL=2

include make/module.mk
