LOCAL_DIR := $(GET_LOCAL_DIR)

MODULE := $(LOCAL_DIR)

MODULE_SRCS += \
	$(LOCAL_DIR)/anti_rollback_cust.c \
	$(LOCAL_DIR)/sec_policy.c \
	$(LOCAL_DIR)/vb_init.c

MODULE_INCLUDES += \
	target/$(TARGET)/include

ifeq ($(BUILD_SEC_LIB),yes)
MODULE_DEPS += \
	$(LOCAL_DIR)/crypto \
	$(LOCAL_DIR)/platsec \
	$(LOCAL_DIR)/sec
MODULE_EXTRA_OBJS += \
	$(LOCAL_DIR)/platsec/tzcc/tzcc.mod.o
else
MODULE_EXTRA_OBJS += \
	$(LOCAL_DIR)/lib/arm64/crypto.mod.o \
	$(LOCAL_DIR)/lib/arm64/platsec.mod.o \
	$(LOCAL_DIR)/lib/arm64/sec.mod.o \
	$(LOCAL_DIR)/lib/arm64/tzcc.mod.o
endif

AVB_SHA256_CRYPTO_HW_SUPPORT ?= yes
ifeq ($(AVB_SHA256_CRYPTO_HW_SUPPORT),yes)
GLOBAL_DEFINES += \
	AVB_SHA256_CRYPTO_HW_SUPPORT=1
endif

include make/module.mk
