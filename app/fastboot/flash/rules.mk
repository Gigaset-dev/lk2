LOCAL_DIR := $(GET_LOCAL_DIR)

MODULE := $(LOCAL_DIR)

MODULE_SRCS += \
	$(LOCAL_DIR)/blockheader.c \
	$(LOCAL_DIR)/bulk_process.c \
	$(LOCAL_DIR)/flash_commands.c \
	$(LOCAL_DIR)/sparse_state_machine.c \
	$(LOCAL_DIR)/transfer_parallel.c

MODULE_DEPS += \
	app/fastboot \
	lib/bio \
	lib/partition \
	platform/$(PLATFORM)/common/partition_utils \
	platform/$(PLATFORM)/common/sboot \

include make/module.mk
