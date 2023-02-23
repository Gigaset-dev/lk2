LOCAL_DIR := $(GET_LOCAL_DIR)

MODULE := $(LOCAL_DIR)

MODULE_SRCS += \
	$(LOCAL_DIR)/mt_gic_v3.c

# Default disable polarity control due to no need to
# configure it on the 5G platforms starting with MT6885.
# Add 'MTK_POL_DEPRECATED := no' on the legacy 4G platforms.
MTK_POL_DEPRECATED ?= yes
ifeq ($(MTK_POL_DEPRECATED),yes)
GLOBAL_DEFINES += MTK_POL_DEPRECATED
endif

include make/module.mk
