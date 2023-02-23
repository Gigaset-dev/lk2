LOCAL_DIR := $(GET_LOCAL_DIR)

MODULE := $(LOCAL_DIR)

MODULE_SRCS += \
	$(LOCAL_DIR)/cpu_early_init.c \
	$(LOCAL_DIR)/mmu_lpae.c \
	$(LOCAL_DIR)/smc.S \

ifeq ($(WITH_SMP),1)
MODULE_SRCS += \
	$(LOCAL_DIR)/mp.c
endif

include make/module.mk
