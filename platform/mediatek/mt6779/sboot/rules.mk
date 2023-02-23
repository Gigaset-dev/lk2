LOCAL_DIR := $(GET_LOCAL_DIR)

MODULE := $(LOCAL_DIR)

MODULE_SRCS += \
	$(LOCAL_DIR)/lock_state.c \
	$(LOCAL_DIR)/sec_policy.c \
	$(LOCAL_DIR)/vb_init.c

MODULE_INCLUDES += \
	target/$(PROJECT)/include

ifeq ($(BUILD_SEC_LIB),yes)
MODULE_DEPS += \
	$(LOCAL_DIR)/crypto \
	$(LOCAL_DIR)/platsec \
	$(LOCAL_DIR)/sec
else
MODULE_EXTRA_OBJS += \
	$(LOCAL_DIR)/lib/crypto.mod.o \
	$(LOCAL_DIR)/lib/platsec.mod.o \
	$(LOCAL_DIR)/lib/sec.mod.o
endif

include make/module.mk
