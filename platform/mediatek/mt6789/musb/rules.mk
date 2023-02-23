LOCAL_DIR := $(GET_LOCAL_DIR)

MODULE := $(LOCAL_DIR)

ifneq (, $(findstring fpga, $(PROJECT)))
MODULE_DEFINES += \
	PROJECT_TYPE_FPGA
endif

MODULE_SRCS += \
	$(LOCAL_DIR)/mt_musb.c \
	$(LOCAL_DIR)/mt_musb_qmu.c \
	$(LOCAL_DIR)/mt_musbphy.c \

MODULE_DEPS += \
	dev/interrupt/arm_gic_v3 \

MODULE_DEFINES += \
	SUPPORT_QMU \
	SUPPORT_36BIT \

include make/module.mk
