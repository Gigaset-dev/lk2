LOCAL_DIR := $(GET_LOCAL_DIR)

MODULE := $(LOCAL_DIR)

MODULE_SRCS += \
	$(LOCAL_DIR)/mboot_expdb.c \
	$(LOCAL_DIR)/mboot_params.c \
	$(LOCAL_DIR)/mrdump_key_setup.c \
	$(LOCAL_DIR)/mrdump_params.c \
	$(LOCAL_DIR)/mrdump_expdb.c

ifeq ($(LK_AS), BL33)
MODULE_SRCS += \
	$(LOCAL_DIR)/debug_cmd_init.c \
	$(LOCAL_DIR)/mrdump_setup.c
endif

MODULE_DEPS += \
	external/lib/cksum \
	external/lib/fdt \
	external/lib/zlib \
	lib/bio \
	lib/kcmdline \
	lib/mblock \
	lib/pl_boottags \
	platform/$(PLATFORM)/common/dconfig \
	platform/$(PLATFORM)/common/dtb_ops \
	platform/$(PLATFORM)/common/wdt

EXTRA_LINKER_SCRIPTS += $(LOCAL_DIR)/mboot_expdb.ld

ifeq ($(DEBUG),0)
MODULE_DEFINES += MTK_AEE_LEVEL=1
else
MODULE_DEFINES += MTK_AEE_LEVEL=2
MTK_MRDUMP_ENABLE ?= yes
endif

ifeq ($(MTK_MRDUMP_ENABLE),yes)
GLOBAL_DEFINES += MTK_MRDUMP_ENABLE
endif

ifeq ($(MTK_MRDUMP_PARTITION_ENABLE), yes)
GLOBAL_DEFINES += MRDUMP_PARTITION_ENABLE
endif

ifeq ($(MTK_HWTAG_KASAN_SECURITY_ONLY), yes)
MODULE_DEFINES += MTK_CMD_HWTAG_KASAN_SECURITY_ONLY=1
else
MODULE_DEFINES += MTK_CMD_HWTAG_KASAN_SECURITY_ONLY=0
endif

ifeq ($(MTK_MTE_SUPPRESS), no)
MODULE_DEFINES += MTK_MTE_CMD_SUPPRESS=0
else
MODULE_DEFINES += MTK_MTE_CMD_SUPPRESS=1
endif

include make/module.mk
