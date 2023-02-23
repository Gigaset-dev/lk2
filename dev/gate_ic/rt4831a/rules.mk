LOCAL_DIR := $(GET_LOCAL_DIR)

MODULE := $(LOCAL_DIR)

GLOBAL_DEFINES += \
	MTK_RT4831A_SUPPORT=\"$(MTK_RT4831A_SUPPORT)\"

MODULE_DEPS += dev/lcm

MODULE_SRCS += \
	$(LOCAL_DIR)/rt4831a_drv.c

include make/module.mk
