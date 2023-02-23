LOCAL_DIR := $(GET_LOCAL_DIR)

MODULE := $(LOCAL_DIR)

MODULE_SRCS += \
	$(LOCAL_DIR)/platform_pmif_clk.c \
	$(LOCAL_DIR)/platform_pmif_spmi.c \

include make/module.mk
