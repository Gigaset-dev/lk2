LOCAL_DIR := $(GET_LOCAL_DIR)

MODULE := $(LOCAL_DIR)

MODULE_SRCS += \
	$(LOCAL_DIR)/scp_plat.c \
	$(LOCAL_DIR)/scp_l1c.c

MODULE_DEPS += \
	platform/$(PLATFORM)/common/loader \
	lib/mblock

include make/module.mk
