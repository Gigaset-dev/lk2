LOCAL_DIR := $(GET_LOCAL_DIR)

MODULE := $(LOCAL_DIR)

ARCH    := arm64
ARM_CPU := armv8-a
WITH_SMP ?= 0

GLOBAL_INCLUDES += \
	platform/$(PLATFORM)/common/include

ifeq ($(LK_AS), BL2_EXT)
GICV3_SUPPORT_GIC600 := 1

MEMBASE ?= 0x55900000
MEMSIZE ?= 0x01600000

MODULE_SRCS += $(LOCAL_DIR)/platform_$(BOOT_APP).c
else ifeq ($(LK_AS), AEE)
MEMBASE ?= 0x55100000
MEMSIZE ?= 0x00500000

MODULE_SRCS += \
	$(LOCAL_DIR)/platform_$(BOOT_APP).c
else
MEMBASE ?= 0x90000000
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
	platform/$(PLATFORM)/common/rtc/mt6359/mt6359_rtc.c

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

MODULE_DEPS += \
	$(LOCAL_DIR)/devinfo \
	$(LOCAL_DIR)/disp \
	$(LOCAL_DIR)/gpio \
	$(LOCAL_DIR)/gpu_dfd \
        $(LOCAL_DIR)/mcdi \
	$(LOCAL_DIR)/mtcmos \
	$(LOCAL_DIR)/pmic_wrap \
	$(LOCAL_DIR)/ufs \
	$(LOCAL_DIR)/mmc \
	$(LOCAL_DIR)/thermal \
	dev/interrupt/arm_gic_v3 \
	dev/timer/arm_generic \
	lib/kcmdline \
	lib/log_store \
	lib/pl_boottags \
	platform/$(PLATFORM)/common/adsp/hifi3 \
	platform/$(PLATFORM)/common/aee \
	platform/$(PLATFORM)/common/apu \
	platform/$(PLATFORM)/common/arch/$(ARCH) \
	platform/$(PLATFORM)/common/battery \
	platform/$(PLATFORM)/common/bootctrl/3 \
	platform/$(PLATFORM)/common/charger \
	platform/$(PLATFORM)/common/dconfig \
	platform/$(PLATFORM)/common/debug \
	platform/$(PLATFORM)/common/devapc \
	platform/$(PLATFORM)/common/dramc \
	platform/$(PLATFORM)/common/dpm \
	platform/$(PLATFORM)/common/gic \
	platform/$(PLATFORM)/common/i2c \
	platform/$(PLATFORM)/common/keypad \
	platform/$(PLATFORM)/common/leds \
	platform/$(PLATFORM)/common/logo \
	platform/$(PLATFORM)/common/log_store \
	platform/$(PLATFORM)/common/mcupm_md32 \
	platform/$(PLATFORM)/common/md \
	platform/$(PLATFORM)/common/pmic \
	platform/$(PLATFORM)/common/rng \
	platform/$(PLATFORM)/common/RoT \
	platform/$(PLATFORM)/common/rtc \
	platform/$(PLATFORM)/common/sboot \
	platform/$(PLATFORM)/common/scp/RV \
	platform/$(PLATFORM)/common/sda \
	platform/$(PLATFORM)/common/smmu \
	platform/$(PLATFORM)/common/spm \
	platform/$(PLATFORM)/common/spmi \
	platform/$(PLATFORM)/common/sspm \
	platform/$(PLATFORM)/common/ssusb \
	platform/$(PLATFORM)/common/ssusb/tphy \
	platform/$(PLATFORM)/common/sysenv \
	platform/$(PLATFORM)/common/tee \
	platform/$(PLATFORM)/common/tfa \
	platform/$(PLATFORM)/common/trustzone \
	platform/$(PLATFORM)/common/uart \
	platform/$(PLATFORM)/common/vcoredvfs \
	platform/$(PLATFORM)/common/video \
	platform/$(PLATFORM)/common/wdt

#To select emi info version
#we need setting here otherwise it would build failed due to
#no value in platform/mediatek/common/dramc/rules.mk and
#no value in platform/mediatek/common/emi/rules.mk
EMI_INFO_VER := 2

#To select emimpu info version
#we need setting here otherwise it would build failed due to
#no value in platform/mediatek/common/emi/rules.mk
MPU_INFO_VER := 1

CHECK_HAS_BATTERY_REMOVED := 1
WITH_PMIC_LEGACY_AUXADC := 1

#To select i2c register map version
#we need setting here otherwise it would build failed due to
#no value in platform/mediatek/common/i2c/rules.mk
I2C_REG_MAP_VER := 2

#To select scp version & dram offset
SCP_RV_VERSION := 33
SCP_DRAM_OFFSET := 0x200000

#To select adsp hw version
#define here to overwrite the default version defined
#in platform/mediatek/common/adsp/hifi3/rules.mk
ADSP_HW_VERSION := 1

# choose one of following value -> 1: disabled/ 2: permissive /3: enforcing
# overwrite SELINUX_STATUS value with PRJ_SELINUX_STATUS, if defined. it's by project variable.
SELINUX_STATUS := 3
ifdef PRJ_SELINUX_STATUS
SELINUX_STATUS := $(PRJ_SELINUX_STATUS)
endif

include make/module.mk
