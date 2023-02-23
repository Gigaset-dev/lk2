LOCAL_DIR := $(GET_LOCAL_DIR)

MODULE := $(LOCAL_DIR)

MODULE_SRCS += \
	$(LOCAL_DIR)/atags.c \
	$(LOCAL_DIR)/custom.c \
	$(LOCAL_DIR)/sboot.c \
	$(LOCAL_DIR)/write_protect.c

MODULE_DEPS += \
	external/lib/fdt \
	lib/bio \
	lib/pl_boottags \
	platform/$(PLATFORM)/common/dtb_ops \
	platform/$(PLATFORM)/common/partition_utils \
	platform/$(PLATFORM)/common/ufs \
	platform/$(PLATFORM)/common/wdt

MTK_SECURITY_SW_SUPPORT ?= no
MTK_SECURITY_ANTI_ROLLBACK ?= no

ifeq ($(MTK_SECURITY_SW_SUPPORT),yes)
GLOBAL_DEFINES += \
	MTK_SECURITY_SW_SUPPORT=1
endif

ifeq ($(MTK_SECURITY_ANTI_ROLLBACK),yes)
GLOBAL_DEFINES += \
	MTK_SECURITY_ANTI_ROLLBACK=1
endif

ifeq ($(LOCK_STATE_SECCFG_CUSTOM), yes)
MODULE_DEFINES += \
	LOCK_STATE_SECCFG_CUSTOM=1
else
ifeq ($(LOCK_STATE_CUSTOM), yes)
MODULE_DEFINES += \
	LOCK_STATE_CUSTOM=1
else
ifeq ($(LOCK_STATE_RPMB), yes)
MODULE_DEFINES += \
	LOCK_STATE_RPMB=1
endif
endif
endif

ifeq ($(MTK_SECURITY_ENABLE_SUB_GROUP), yes)
MODULE_DEFINES += MTK_SECURITY_ENABLE_SUB_GROUP=1
endif

ifeq ($(MTK_SECURITY_UPDATE_DA_ARB), yes)
MODULE_DEFINES += MTK_SECURITY_UPDATE_DA_ARB=1
endif

include make/module.mk
