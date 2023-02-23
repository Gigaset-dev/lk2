LOCAL_DIR := $(GET_LOCAL_DIR)

MODULE := $(LOCAL_DIR)

MODULE_SRCS += \
	$(LOCAL_DIR)/lock_state.c \
	$(LOCAL_DIR)/sec_policy.c \
	$(LOCAL_DIR)/verified_boot.c

include make/module.mk
