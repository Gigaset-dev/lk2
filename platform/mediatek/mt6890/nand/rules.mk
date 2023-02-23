LOCAL_DIR := $(GET_LOCAL_DIR)

MODULE := $(LOCAL_DIR)

MODULE_DEPS += \
	lib/bio \
	lib/nftl \
	lib/partition \
	platform/$(PLATFORM)/common/nandx

MODULE_SRCS += \
	$(LOCAL_DIR)/driver.c \
	$(LOCAL_DIR)/pmt.c

include make/module.mk
