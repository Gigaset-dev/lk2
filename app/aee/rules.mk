LOCAL_DIR := $(GET_LOCAL_DIR)

MODULE := $(LOCAL_DIR)

MODULE_SRCS := \
	$(LOCAL_DIR)/aee.c \
	$(LOCAL_DIR)/KEDump.c \
	$(LOCAL_DIR)/mmu_translate.c \
	$(LOCAL_DIR)/mrdump_elf.c \
	$(LOCAL_DIR)/mrdump_log.c \
	$(LOCAL_DIR)/mrdump_run.c \
	$(LOCAL_DIR)/mrdump_sddev.c \
	$(LOCAL_DIR)/reboot_record.c \

build_mrdump ?= no

ifneq ($(DEBUG),0)
build_mrdump := yes
endif

ifeq ($(MTK_MRDUMP_ENABLE),yes)
build_mrdump := yes
else ifeq ($(MTK_MRDUMP_ENABLE),no)
build_mrdump := no
endif

ifeq ($(build_mrdump),yes)
MODULE_SRCS += \
	$(LOCAL_DIR)/mrdump_datafs.c \
	$(LOCAL_DIR)/mrdump_dbgscript.c \
	$(LOCAL_DIR)/mrdump_mpart.c \
	$(LOCAL_DIR)/mrdump_usb.c \
	$(LOCAL_DIR)/mrdump_zip.c
endif

WITH_MRDUMP_PCIE ?= 0

ifeq ($(WITH_MRDUMP_PCIE),1)
MODULE_DEFINES += WITH_MRDUMP_PCIE
MODULE_SRCS += $(LOCAL_DIR)/mrdump_pcie.c
endif

# AEE-LK will set BUF_SIZE to 0 in case hit the malloc
# fail condition. We still can get into fastboot_init()
# when setting BUF_SIZE as 0.
GLOBAL_DEFINES += CUSTOM_FASTBOOT_BUF_SIZE=0

MODULE_DEPS += \
	app/fastboot \
	external/lib/cksum \
	external/lib/zlib \
	lib/bio \
	lib/mblock \
	lib/pl_boottags \
	platform/$(PLATFORM)/common/aee \
	platform/$(PLATFORM)/common/log_store \
	platform/$(PLATFORM)/common/rtc \
	platform/$(PLATFORM)/common/wdt

include make/module.mk
