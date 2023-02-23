LOCAL_DIR := $(GET_LOCAL_DIR)

MODULE := $(LOCAL_DIR)

MODULE_SRCS += \
	$(LOCAL_DIR)/ddp_dither.c \
	$(LOCAL_DIR)/ddp_dsc.c \
	$(LOCAL_DIR)/ddp_dsi.c \
	$(LOCAL_DIR)/ddp_dump.c \
	$(LOCAL_DIR)/ddp_info.c \
	$(LOCAL_DIR)/ddp_manager.c \
	$(LOCAL_DIR)/ddp_misc.c \
	$(LOCAL_DIR)/ddp_ovl.c \
	$(LOCAL_DIR)/ddp_path.c \
	$(LOCAL_DIR)/ddp_postmask.c \
	$(LOCAL_DIR)/ddp_pwm.c \
	$(LOCAL_DIR)/ddp_rdma.c \
	$(LOCAL_DIR)/disp_lcm.c \
	$(LOCAL_DIR)/mt_disp_drv.c \
	$(LOCAL_DIR)/primary_display.c

MODULE_DEPS += \
	dev/lcm \
	lib/mblock \
	lib/pl_boottags \
	platform/$(PLATFORM)/common/dtb_ops

ifeq ($(SUB_PLATFORM), $(filter $(SUB_PLATFORM), mt6873))
MODULE_DEFINES += MT6873_GPIO_HARD_CODE=y
endif

include make/module.mk

