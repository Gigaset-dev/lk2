LOCAL_DIR := $(GET_LOCAL_DIR)

MODULE := $(LOCAL_DIR)

ADSP_GEN := hifi3
ADSP_HW_VERSION ?= 2
ADSP_LOADER_SUPPORT ?= yes

MODULE_DEPS += \
	external/lib/fdt \
	lib/mblock \
	platform/$(PLATFORM)/common/adsp \
	platform/$(PLATFORM)/common/loader

MODULE_SRCS += \
	$(LOCAL_DIR)/adsp_plat.c \
	$(LOCAL_DIR)/adsp_hw_v$(ADSP_HW_VERSION).c

ifeq ($(ADSP_LOADER_SUPPORT), yes)
MODULE_DEFINES += ADSP_LOADER_SUPPORT
endif

ifeq ($(ADSP_HW_VERSION),1)
GLOBAL_DEFINES += \
	ADSP_DT_NODE_COMMON=\"mediatek,adsp_common\" \
	ADSP_DT_NODE_CORE=\"mediatek,adsp_core\"
else
GLOBAL_DEFINES += \
	ADSP_DT_NODE_COMMON=\"mediatek,$(SUB_PLATFORM)-adspsys\" \
	ADSP_DT_NODE_CORE=\"mediatek,$(SUB_PLATFORM)-adsp_core\"
endif

include make/module.mk
