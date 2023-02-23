LOCAL_DIR := $(GET_LOCAL_DIR)
MODULE := $(LOCAL_DIR)

ARCH := arm64
ARM_CPU := cortex-a55
WITH_SMP ?= 0

WITH_LINKER_GC := 1

KERNEL_ASPACE_BASE ?= 0xfffffff000000000
KERNEL_ASPACE_SIZE ?= 0x0000001000000000
KERNEL_BASE ?= $(shell printf 0x%x $$(($(KERNEL_ASPACE_BASE) + $(MEMBASE))))

ifneq (, $(findstring fpga, $(PROJECT)))
GLOBAL_DEFINES += \
	PROJECT_TYPE_FPGA
endif

GLOBAL_DEFINES += \
	MMU_IDENT_SIZE_SHIFT=32

GLOBAL_INCLUDES += \
	platform/$(PLATFORM)/common/include \

GICV3_SUPPORT_GIC600 := 1

MODULE_DEPS += \
	$(LOCAL_DIR)/keypad \
	$(LOCAL_DIR)/pll \
	$(LOCAL_DIR)/pmic \
	$(LOCAL_DIR)/pmic_wrap \
	dev/interrupt/arm_gic_v3 \
	dev/timer/arm_generic \
	lib/watchdog \
	platform/$(PLATFORM)/common/arch/$(ARCH) \
	platform/$(PLATFORM)/common/debug \
	platform/$(PLATFORM)/common/gic \
	platform/$(PLATFORM)/common/i2c \
	platform/$(PLATFORM)/common/kernel/vm \
	platform/$(PLATFORM)/common/pmic \
	platform/$(PLATFORM)/common/spmi \
	platform/$(PLATFORM)/common/ssusb \
	platform/$(PLATFORM)/common/ssusb/tphy \
	platform/$(PLATFORM)/common/subpmic \
	platform/$(PLATFORM)/common/subpmic/mt6315 \
	platform/$(PLATFORM)/common/uart \
	platform/$(PLATFORM)/common/wdt \

ifeq ($(LK_AS),BL2)
# If DRAM_TYPE and DRAM_CHANNEL are NULL, DRAM uses auxadc to detect
# DRAM information during runtime.
DRAM_TYPE ?=
DRAM_CHANNEL ?=
GLOBAL_DEFINES += \
	DRAM_TYPE=$(DRAM_TYPE) \
	DRAM_CHANNEL=$(DRAM_CHANNEL) \

MODULE_DEPS += \
	$(LOCAL_DIR)/../../../../dramk_8195/dram
endif

MODULE_SRCS += \
	$(LOCAL_DIR)/platform.c \
	$(LOCAL_DIR)/platform_$(call LC,$(LK_AS)).c \
	platform/$(PLATFORM)/common/init.c \
	platform/$(PLATFORM)/common/pmic/mt6359/mt6359_psc.c \
	platform/$(PLATFORM)/common/subpmic/mt6360.c \

#To select i2c register map version
#we need setting here otherwise it would build failed due to
#no value in platform/mediatek/common/i2c/rules.mk
I2C_REG_MAP_VER := 2

include make/module.mk
