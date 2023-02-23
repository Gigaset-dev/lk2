LOCAL_DIR := $(GET_LOCAL_DIR)
MODULE := $(LOCAL_DIR)

ARCH := arm64
ARM_CPU := cortex-a53
WITH_SMP ?= 0

WITH_LINKER_GC := 1

KERNEL_ASPACE_BASE ?= 0xfffffff000000000
KERNEL_ASPACE_SIZE ?= 0x0000001000000000
KERNEL_BASE ?= $(shell printf 0x%x $$(($(KERNEL_ASPACE_BASE) + $(MEMBASE))))

GLOBAL_DEFINES += \
	MMU_IDENT_SIZE_SHIFT=32

GLOBAL_INCLUDES += \
	platform/$(PLATFORM)/common/include

WITH_PMIC_LEGACY_AUXADC :=1

MODULE_DEPS += \
	$(LOCAL_DIR)/gpio \
	$(LOCAL_DIR)/pll \
	$(LOCAL_DIR)/pmic_wrap \
	$(LOCAL_DIR)/sboot \
	dev/interrupt/arm_gic_v3 \
	dev/timer/arm_generic \
	platform/$(PLATFORM)/common/arch/$(ARCH) \
	platform/$(PLATFORM)/common/auxadc \
	platform/$(PLATFORM)/common/debug \
	platform/$(PLATFORM)/common/gic \
	platform/$(PLATFORM)/common/i2c \
	platform/$(PLATFORM)/common/kernel/vm \
	platform/$(PLATFORM)/common/keypad \
	platform/$(PLATFORM)/common/pmic \
	platform/$(PLATFORM)/common/ssusb \
	platform/$(PLATFORM)/common/uart \
	platform/$(PLATFORM)/common/wdt

MODULE_SRCS += \
	$(LOCAL_DIR)/platform.c \
	$(LOCAL_DIR)/platform_$(PROJ_EXT).c \
	platform/$(PLATFORM)/common/pmic/mt6357/mt6357_auxadc.c \
	platform/$(PLATFORM)/common/pmic/mt6357/mt6357_keys.c \
	platform/$(PLATFORM)/common/pmic/mt6357/mt6357_psc.c \
	platform/$(PLATFORM)/common/ssusb/tphy/usbphy.c

#To select i2c register map version
#we need setting here otherwise it would build failed due to
#no value in platform/mediatek/common/i2c/rules.mk
I2C_REG_MAP_VER := 1

include make/module.mk
