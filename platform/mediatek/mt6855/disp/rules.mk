LOCAL_DIR := $(GET_LOCAL_DIR)

MODULE := $(LOCAL_DIR)

ifneq (, $(findstring fpga, $(PROJECT)))
MODULE_DEFINES += \
	PROJECT_TYPE_FPGA
endif


MTK_LCM_GATEIC_VERSION ?= "rt4801h"
LCM_GATEIC_DEFINES := $(shell echo $(MTK_LCM_GATEIC_VERSION) | tr a-z A-Z)
GLOBAL_DEFINES += \
	$(foreach GATEIC,$(LCM_GATEIC_DEFINES), MTK_LCM_GATEIC_$(GATEIC)_SUPPORT) \
	MTK_RUNTIME_SWITCH_SPR_SUPPORT=\"$(MTK_RUNTIME_SWITCH_SPR_SUPPORT)\"
GLOBAL_DEFINES += LCM_SET_DISPLAY_ON_DELAY

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

include make/module.mk

