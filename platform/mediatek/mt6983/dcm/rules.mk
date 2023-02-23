LOCAL_DIR := $(GET_LOCAL_DIR)

MODULE := $(LOCAL_DIR)

MODULE_SRCS += \
	$(LOCAL_DIR)/mtk_dcm.c \
	$(LOCAL_DIR)/mtk_dcm_autogen.c

include make/module.mk
