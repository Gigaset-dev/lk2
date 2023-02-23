LOCAL_DIR := $(GET_LOCAL_DIR)

MODULE := $(LOCAL_DIR)

CS_VERSION ?= v1
MODULE_SRCS += \
	$(LOCAL_DIR)/arch.c \
	$(LOCAL_DIR)/asm.S \
	$(LOCAL_DIR)/cache-ops.S \
	$(LOCAL_DIR)/cs_$(CS_VERSION).c \
	$(LOCAL_DIR)/exceptions_el2_el3.S \
	$(LOCAL_DIR)/hvc.S \
	$(LOCAL_DIR)/smc.S \

ifeq ($(WITH_SMP),1)
MODULE_SRCS += \
	$(LOCAL_DIR)/mp.c
endif

include make/module.mk
