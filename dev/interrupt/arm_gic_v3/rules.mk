LOCAL_DIR := $(GET_LOCAL_DIR)

MODULE := $(LOCAL_DIR)

MODULE_SRCS += \
	$(LOCAL_DIR)/arm_gic_v3.c

ifeq ($(GICV3_SUPPORT_GIC600),1)
MODULE_SRCS += \
	$(LOCAL_DIR)/gic600.c
endif


include make/module.mk
