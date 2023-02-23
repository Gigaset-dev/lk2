LOCAL_DIR := $(GET_LOCAL_DIR)

MODULE := $(LOCAL_DIR)

ifneq (, $(findstring fpga, $(PROJECT)))
	MODULE_DEFINES += PROJECT_TYPE_FPGA
endif

ifeq ($(MTK_LCM_COMMON_DRIVER_SUPPORT),yes)
MODULE_DEFINES += \
	MTK_LCM_LIST_SUPPORT=\"$(shell echo $(MTK_LCM_LIST_SUPPORT) | tr '[A-Z]' '[a-z]')\"

MODULE_SRCS += \
	$(LOCAL_DIR)/lcm_dconfig.c \
	$(LOCAL_DIR)/lcm_drv.c \
	$(LOCAL_DIR)/lcm_drv_dbi.c \
	$(LOCAL_DIR)/lcm_drv_dpi.c \
	$(LOCAL_DIR)/lcm_drv_dsi.c \
	$(LOCAL_DIR)/lcm_gateic_rt4801h.c \
	$(LOCAL_DIR)/lcm_gateic_rt4831a.c \
	$(LOCAL_DIR)/lcm_gateic.c \
	$(LOCAL_DIR)/lcm_helper_dbi.c \
	$(LOCAL_DIR)/lcm_helper_dpi.c \
	$(LOCAL_DIR)/lcm_helper_dsi.c \
	$(LOCAL_DIR)/lcm_helper.c \
	$(LOCAL_DIR)/lcm_i2c.c \
	$(LOCAL_DIR)/round_corner/lcm_rc.c

MODULE_DEPS += \
	dev/timer/arm_generic \
	platform/$(PLATFORM)/common/sysenv
endif

include make/module.mk
