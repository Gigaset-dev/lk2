LOCAL_DIR := $(GET_LOCAL_DIR)

MODULE := $(LOCAL_DIR)

ARCH    := arm64
ARM_CPU := armv8-a
WITH_SMP ?= 0

GLOBAL_INCLUDES += \
        platform/$(PLATFORM)/common/include

ifeq ($(LK_AS), BL2_EXT)
GICV3_SUPPORT_GIC600 := 1

MEMBASE ?= 0x68200000
MEMSIZE ?= 0x01600000

MODULE_SRCS += $(LOCAL_DIR)/platform_$(BOOT_APP).c
else ifeq ($(LK_AS), AEE)
MEMBASE ?= 0x56000000
MEMSIZE ?= 0x00200000

MODULE_SRCS += \
	$(LOCAL_DIR)/platform_$(BOOT_APP).c
else
MEMBASE ?= 0x56200000
MEMSIZE ?= 0x12000000

MODULE_SRCS += $(LOCAL_DIR)/platform.c
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

# Although KERNEL_ASPACE_BASE will be defined in arch/$(ARCH)/rules.mk, we need it
# here to caculate KERNEL_BASE, and its value MUST be the same as the one in
# arch/arm/rules.mk.
ifeq ($(ARCH),arm)
KERNEL_ASPACE_BASE = 0x40000000
endif

KERNEL_BASE ?= $(shell printf 0x%x $$(($(KERNEL_ASPACE_BASE) + $(MEMBASE))))
GLOBAL_DEFINES += MMU_WITH_TRAMPOLINE=1

GLOBAL_DEFINES += \
	SUB_PLATFORM=\"$(SUB_PLATFORM)\" \
	LK_AS=\"$(LK_AS)\"

WITH_PMIC_LEGACY_AUXADC :=1

MODULE_DEPS += \
	$(LOCAL_DIR)/devapc \
	$(LOCAL_DIR)/devinfo \
	$(LOCAL_DIR)/disp \
	$(LOCAL_DIR)/gpio \
	$(LOCAL_DIR)/mcdi \
	$(LOCAL_DIR)/mtcmos \
	$(LOCAL_DIR)/picachu \
	$(LOCAL_DIR)/pmic_wrap \
	$(LOCAL_DIR)/sec_cfg \
	$(LOCAL_DIR)/ufs \
	dev/interrupt/arm_gic_v3 \
	dev/timer/arm_generic \
	lib/log_store \
	lib/pl_boottags \
	platform/$(PLATFORM)/common/aee \
	platform/$(PLATFORM)/common/arch/$(ARCH) \
	platform/$(PLATFORM)/common/apu \
	platform/$(PLATFORM)/common/bootctrl/3 \
	platform/$(PLATFORM)/common/charger \
	platform/$(PLATFORM)/common/dconfig \
	platform/$(PLATFORM)/common/debug \
	platform/$(PLATFORM)/common/dpm \
	platform/$(PLATFORM)/common/dramc \
	platform/$(PLATFORM)/common/gic \
	platform/$(PLATFORM)/common/i2c \
	platform/$(PLATFORM)/common/keypad \
	platform/$(PLATFORM)/common/leds \
	platform/$(PLATFORM)/common/logo \
	platform/$(PLATFORM)/common/mcupm \
	platform/$(PLATFORM)/common/pmic \
	platform/$(PLATFORM)/common/pmic_dlpt \
	platform/$(PLATFORM)/common/rng \
	platform/$(PLATFORM)/common/rtc \
	platform/$(PLATFORM)/common/sda \
	platform/$(PLATFORM)/common/spm \
	platform/$(PLATFORM)/common/spmi \
	platform/$(PLATFORM)/common/scp/RV \
	platform/$(PLATFORM)/common/sspm \
	platform/$(PLATFORM)/common/ssusb \
	platform/$(PLATFORM)/common/ssusb/tphy \
	platform/$(PLATFORM)/common/subpmic \
	platform/$(PLATFORM)/common/subpmic/mt6315 \
	platform/$(PLATFORM)/common/tee \
	platform/$(PLATFORM)/common/trustzone \
	platform/$(PLATFORM)/common/uart \
	platform/$(PLATFORM)/common/vcoredvfs \
	platform/$(PLATFORM)/common/video

#To select i2c register map version
#we need setting here otherwise it would build failed due to
#no value in platform/mediatek/common/i2c/rules.mk
I2C_REG_MAP_VER := 2

#To select emi info version
#we need setting here otherwise it would build failed due to
#no value in platform/mediatek/common/dramc/rules.mk and
#no value in platform/mediatek/common/emi/rules.mk
EMI_INFO_VER := 2

#To select emimpu info version
#we need setting here otherwise it would build failed due to
#no value in platform/mediatek/common/emi/rules.mk
MPU_INFO_VER := 1

# choose one of following value -> 1: disabled/ 2: permissive /3: enforcing
# overwrite SELINUX_STATUS value with PRJ_SELINUX_STATUS, if defined. it's by project variable.
SELINUX_STATUS := 3
ifdef PRJ_SELINUX_STATUS
SELINUX_STATUS := $(PRJ_SELINUX_STATUS)
endif

include make/module.mk
