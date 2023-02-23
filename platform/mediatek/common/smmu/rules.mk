LOCAL_DIR := $(GET_LOCAL_DIR)

MODULE := $(LOCAL_DIR)

MODULE_SRCS += \
	$(LOCAL_DIR)/mtk_smmu.c \
	$(LOCAL_DIR)/ssheap_mem.c

MODULE_DEPS += \
	platform/$(PLATFORM)/common/geniezone

include make/module.mk
