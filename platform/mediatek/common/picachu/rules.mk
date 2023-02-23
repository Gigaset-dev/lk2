LOCAL_DIR := $(GET_LOCAL_DIR)

MODULE := $(LOCAL_DIR)

ifeq ($(LK_AS), BL2_EXT)
MODULE_SRCS += \
	$(LOCAL_DIR)/picachu_log.c \
	$(LOCAL_DIR)/picachu_lut.c \
	$(LOCAL_DIR)/picachu_misc.c \
	$(LOCAL_DIR)/picachu_para.c


ifeq ($(MTK_AGING_FLAVOR_LOAD),yes)
	MODULE_DEFINES += MTK_AGING_FLAVOR_LOAD
endif

ifeq ($(MTK_SLT_FLAVOR_LOAD),yes)
	MODULE_DEFINES += MTK_SLT_FLAVOR_LOAD
endif

ifeq ($(MTK_MCL50_FLAVOR_LOAD),yes)
	MODULE_DEFINES += MTK_MCL50_FLAVOR_LOAD
endif

include make/module.mk
endif

