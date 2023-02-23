LOCAL_DIR := $(GET_LOCAL_DIR)

MODULE := $(LOCAL_DIR)

ARCH    := arm
ARM_CPU := armv8-a
WITH_SMP ?= 0

ifeq ($(LK_AS), AEE)
MEMBASE ?= 0x4DD00000
MEMSIZE ?= 0x00100000
MODULE_SRCS += \
	$(LOCAL_DIR)/platform_$(BOOT_APP).c
else
MODULE_SRCS += \
	$(LOCAL_DIR)/platform.c
endif

MODULE_SRCS += \
	platform/$(PLATFORM)/common/charger/mt6360_pmu_charger.c \
	platform/$(PLATFORM)/common/debug/dynamic_log.c \
	platform/$(PLATFORM)/common/init.c \
	platform/$(PLATFORM)/common/pmic/mt6359/mt6359_auxadc.c \
	platform/$(PLATFORM)/common/pmic/mt6359/mt6359_gauge.c \
	platform/$(PLATFORM)/common/pmic/mt6359/mt6359_keys.c \
	platform/$(PLATFORM)/common/pmic/mt6359/mt6359_psc.c \
	platform/$(PLATFORM)/common/pmic_dlpt/mt6359_dlpt.c \
	platform/$(PLATFORM)/common/rtc/mt6359/mt6359_rtc.c \
	platform/$(PLATFORM)/common/subpmic/mt6360.c

MEMBASE ?= 0x50000000
MEMSIZE ?= 0x0A000000

GLOBAL_DEFINES += \
	SUB_PLATFORM=\"$(SUB_PLATFORM)\"

# Although KERNEL_ASPACE_BASE will be defined in arch/arm/rules.mk, we need it
# here to caculate KERNEL_BASE, and its value MUST be the same as the one in
# arch/arm/rules.mk.
KERNEL_ASPACE_BASE = 0x40000000
KERNEL_BASE ?= $(shell printf 0x%x $$(($(KERNEL_ASPACE_BASE) + $(MEMBASE))))
GLOBAL_DEFINES += MMU_WITH_TRAMPOLINE=1

GLOBAL_INCLUDES += \
	platform/$(PLATFORM)/common/include

ifeq ($(CLANG_BINDIR),)
# Although ARCH_DEFAULT_STACK_SIZE will be defined in arch/arm/rules.mk, we
# need it here to specify the stack-usage value, and its value MUST be the same
# as the one in arch/arm/rules.mk.
ARCH_DEFAULT_STACK_SIZE = 4096
GLOBAL_COMPILEFLAGS += -Werror=stack-usage=1
GLOBAL_COMPILEFLAGS += -Wstack-usage=$(ARCH_DEFAULT_STACK_SIZE)
endif

ifeq ($(WITH_SMP),1)
SMP_MAX_CPUS := 8
SMP_CPU_CLUSTER_SHIFT := 0
SMP_CPU_ON_MASK ?= 0x81 # mask for which cpu to be on

MODULE_SRCS += \
	$(LOCAL_DIR)/plat_mp.c \
	platform/$(PLATFORM)/common/psci/psci.c

GLOBAL_DEFINES += SMP_CPU_ON_MASK=$(SMP_CPU_ON_MASK)
endif

WITH_PMIC_LEGACY_AUXADC :=1

MODULE_DEPS += \
	$(LOCAL_DIR)/devinfo \
	$(LOCAL_DIR)/disp \
	$(LOCAL_DIR)/dvfs \
	$(LOCAL_DIR)/gpio \
	$(LOCAL_DIR)/mcdi \
	$(LOCAL_DIR)/mtcmos \
	$(LOCAL_DIR)/pmic_wrap \
	$(LOCAL_DIR)/sboot \
	$(LOCAL_DIR)/ufs \
	dev/interrupt/arm_gic_v3 \
	dev/timer/arm_generic \
	dev/lcm \
	lib/mblock \
	lib/pl_boottags \
	platform/$(PLATFORM)/common/aee \
	platform/$(PLATFORM)/common/arch/$(ARCH) \
	platform/$(PLATFORM)/common/battery \
	platform/$(PLATFORM)/common/charger \
	platform/$(PLATFORM)/common/debug \
	platform/$(PLATFORM)/common/gic \
	platform/$(PLATFORM)/common/i2c \
	platform/$(PLATFORM)/common/keypad \
	platform/$(PLATFORM)/common/leds \
	platform/$(PLATFORM)/common/logo \
	platform/$(PLATFORM)/common/pmic \
	platform/$(PLATFORM)/common/pmic_dlpt \
	platform/$(PLATFORM)/common/rtc \
	platform/$(PLATFORM)/common/spm \
	platform/$(PLATFORM)/common/sspm \
	platform/$(PLATFORM)/common/ssusb \
	platform/$(PLATFORM)/common/ssusb/tphy \
	platform/$(PLATFORM)/common/subpmic \
	platform/$(PLATFORM)/common/sysenv \
	platform/$(PLATFORM)/common/uart \
	platform/$(PLATFORM)/common/vcoredvfs \
	platform/$(PLATFORM)/common/video \
	platform/$(PLATFORM)/common/wdt

#To select i2c register map version
#we need setting here otherwise it would build failed due to
#no value in platform/mediatek/common/i2c/rules.mk
I2C_REG_MAP_VER := 2

#Set spmfw legacy mode
SPM_FW_LOAD_LEGACY := true

include make/module.mk
