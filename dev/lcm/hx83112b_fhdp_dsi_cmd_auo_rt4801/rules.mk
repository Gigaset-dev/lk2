LOCAL_DIR := $(GET_LOCAL_DIR)

MODULE := $(LOCAL_DIR)

PLATFORM := mediatek
SUB_PLATFORM := mt6779

MODULE_SRCS += $(LOCAL_DIR)/hx83112b_fhdp_dsi_cmd_auo_rt4801.c

include make/module.mk
