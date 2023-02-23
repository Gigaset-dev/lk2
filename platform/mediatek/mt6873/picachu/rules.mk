LOCAL_DIR := $(GET_LOCAL_DIR)

MODULE := $(LOCAL_DIR)

MODULE_SRCS += \
	$(LOCAL_DIR)/picachu_misc.c

MODULE_EXTRA_OBJS += \
	$(LOCAL_DIR)/lib/$(ARCH)/picachu_v2.mod.o

include make/module.mk

