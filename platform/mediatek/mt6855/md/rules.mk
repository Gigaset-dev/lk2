LOCAL_DIR := $(GET_LOCAL_DIR)

MODULE := $(LOCAL_DIR)

MODULE_INCLUDES += platform/$(PLATFORM)/common/md

MODULE_SRCS += \
	$(LOCAL_DIR)/ccci_lk_md_power_ctrl.c \
	$(LOCAL_DIR)/ccci_lk_plat.c

#MODULE_SRCS += $(LOCAL_DIR)/ccci_dummy_ap_plat.c

include make/module.mk
