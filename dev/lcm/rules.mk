LOCAL_DIR := $(GET_LOCAL_DIR)

MODULE := $(LOCAL_DIR)

CUSTOM_ALPHA := k6983v1_64_alpha

GLOBAL_DEFINES += \
	MTK_LCM_PHYSICAL_ROTATION=\"$(MTK_LCM_PHYSICAL_ROTATION)\"

ifeq ($(MTK_ROUND_CORNER_SUPPORT),yes)
GLOBAL_DEFINES += MTK_ROUND_CORNER_SUPPORT=\"$(MTK_ROUND_CORNER_SUPPORT)\"
endif

MODULE_SRCS += $(LOCAL_DIR)/lcm_util.c

MTK_LCM_COMMON_DRIVER_SUPPORT ?= yes
ifeq ($(MTK_LCM_COMMON_DRIVER_SUPPORT),yes)
GLOBAL_DEFINES += \
	MTK_LCM_COMMON_DRIVER_SUPPORT=\"$(MTK_LCM_COMMON_DRIVER_SUPPORT)\"

ifeq ($(MTK_ROUND_CORNER_SUPPORT),yes)
RC_LIST_DEFINES := $(shell echo $(MTK_RC_LIST_SUPPORT) | tr a-z A-Z)
GLOBAL_DEFINES += $(foreach RC,$(RC_LIST_DEFINES),$(RC))
endif

MODULE_DEPS += dev/lcm/drv \
	platform/$(PLATFORM)/common/i2c \
	platform/$(PLATFORM)/common/dtb_ops

else
LCM_LIST_DEFINES := $(shell echo $(MTK_LCM_LIST_SUPPORT) | tr a-z A-Z)
GLOBAL_DEFINES += \
	$(foreach LCM,$(LCM_LIST_DEFINES),$(LCM))

LCM_LISTS := $(subst ",,$(MTK_LCM_LIST_SUPPORT))

ifneq (,$(findstring $(CUSTOM_ALPHA),$(CUSTOM_LK_LCM)))
MODULE_SRCS += \
	$(foreach LCM,$(LCM_LISTS),$(LOCAL_DIR)/$(CUSTOM_ALPHA)/$(addsuffix .c, $(LCM))) \
	$(LOCAL_DIR)/mt65xx_lcm_list.c
else
MODULE_SRCS += \
	$(foreach LCM,$(LCM_LISTS),$(LOCAL_DIR)/$(LCM)/$(addsuffix .c, $(LCM))) \
	$(LOCAL_DIR)/mt65xx_lcm_list.c
endif

ifeq (, $(findstring fpga, $(PROJECT)))
MODULE_DEPS += dev/gate_ic/rt4831a
endif

endif

include make/module.mk
