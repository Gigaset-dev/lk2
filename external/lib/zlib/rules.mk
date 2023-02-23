LOCAL_DIR := $(GET_LOCAL_DIR)

MODULE := $(LOCAL_DIR)

MODULE_SRCS += \
	$(LOCAL_DIR)/cpu_features.c\
	$(LOCAL_DIR)/deflate.c \
	$(LOCAL_DIR)/inffast.c\
	$(LOCAL_DIR)/inflate.c\
	$(LOCAL_DIR)/inftrees.c\
	$(LOCAL_DIR)/trees.c\
	$(LOCAL_DIR)/zutil.c

MODULE_COMPILEFLAGS += -Wno-sign-compare

MODULE_DEPS += \
	external/lib/cksum

include make/module.mk
