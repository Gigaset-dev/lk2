LOCAL_DIR := $(GET_LOCAL_DIR)

MODULE := $(LOCAL_DIR)

CFG_CPU_DVFS_LEVEL ?= 0 # Use default project config if 0
MODULE_DEFINES += CFG_CPU_DVFS_LEVEL=$(CFG_CPU_DVFS_LEVEL)

MODULE_SRCS += \
	$(LOCAL_DIR)/plat_mtk_dvfs.c

include make/module.mk
