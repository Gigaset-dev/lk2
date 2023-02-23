LOCAL_DIR := $(GET_LOCAL_DIR)

MODULE := $(LOCAL_DIR)

ARCH    := arm
ARM_CPU := cortex-a7
WITH_SMP ?= 0

ifeq ($(LK_AS), AEE)
MEMBASE ?= 0x43000000
MEMSIZE ?= 0x00200000
MODULE_SRCS += \
	$(LOCAL_DIR)/platform_$(BOOT_APP).c
else
MEMBASE ?= 0x44000000
MEMSIZE ?= 0x02000000
MODULE_SRCS += \
	$(LOCAL_DIR)/platform.c
endif

MODULE_SRCS += \
	platform/$(PLATFORM)/common/init.c \
	platform/$(PLATFORM)/common/pmic/mt6330/mt6330_psc.c \
	platform/$(PLATFORM)/common/pmic/mt6330/mt6330_keys.c \
	platform/$(PLATFORM)/common/pmic/mt6330/mt6330_regu.c \
	platform/$(PLATFORM)/common/rtc/mt6330/mt6330_rtc.c

GLOBAL_DEFINES += \
	SUB_PLATFORM=\"$(SUB_PLATFORM)\"

# Although KERNEL_ASPACE_BASE will be defined in arch/$(ARCH)/rules.mk, we need it
# here to caculate KERNEL_BASE, and its value MUST be the same as the one in
# arch/arm/rules.mk.

ifeq ($(ARCH),arm)
KERNEL_ASPACE_BASE = 0x40000000
endif

KERNEL_BASE ?= $(shell printf 0x%x $$(($(KERNEL_ASPACE_BASE) + $(MEMBASE))))
GLOBAL_DEFINES += MMU_WITH_TRAMPOLINE=1

ifneq (, $(findstring fpga, $(PROJECT)))
GLOBAL_DEFINES += \
	PROJECT_TYPE_FPGA
endif

GLOBAL_INCLUDES += \
	platform/$(PLATFORM)/common/include

MODULE_DEPS += \
	$(LOCAL_DIR)/auxadc \
	$(LOCAL_DIR)/devinfo \
	$(LOCAL_DIR)/dpm \
	$(LOCAL_DIR)/fastboot \
	$(LOCAL_DIR)/gpio \
	$(LOCAL_DIR)/mmc \
	$(LOCAL_DIR)/mtcmos \
	$(LOCAL_DIR)/nand \
	dev/interrupt/arm_gic_v3 \
	dev/timer/arm_generic \
	lib/pl_boottags \
	lib/log_store \
	platform/$(PLATFORM)/common/aee \
	platform/$(PLATFORM)/common/arch/$(ARCH) \
	platform/$(PLATFORM)/common/atf \
	platform/$(PLATFORM)/common/debug \
	platform/$(PLATFORM)/common/devapc \
	platform/$(PLATFORM)/common/dramc \
	platform/$(PLATFORM)/common/emi \
	platform/$(PLATFORM)/common/gic \
	platform/$(PLATFORM)/common/i2c \
	platform/$(PLATFORM)/common/medmcu \
	platform/$(PLATFORM)/common/medmcu/RV33 \
	platform/$(PLATFORM)/common/pmic \
	platform/$(PLATFORM)/common/rtc \
	platform/$(PLATFORM)/common/spm \
	platform/$(PLATFORM)/common/spmi \
	platform/$(PLATFORM)/common/sspm \
	platform/$(PLATFORM)/common/ssusb \
	platform/$(PLATFORM)/common/ssusb/tphy \
	platform/$(PLATFORM)/common/uart \
	platform/$(PLATFORM)/common/vcoredvfs \
	platform/$(PLATFORM)/common/wdt

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

ifeq ($(WITH_SBOOT_ASSERT_ENABLE),1)
	MTK_SECURITY_SW_SUPPORT := yes
endif

ifeq ($(WITH_SBOOT_ANTI_ROLLBACK_ENABLE),1)
	MTK_SECURITY_ANTI_ROLLBACK := yes
endif

# choose one of following value -> 1: disabled/ 2: permissive /3: enforcing
# overwrite SELINUX_STATUS value with PRJ_SELINUX_STATUS, if defined. it's by project variable.
SELINUX_STATUS := 3
ifdef PRJ_SELINUX_STATUS
SELINUX_STATUS := $(PRJ_SELINUX_STATUS)
endif

include make/module.mk
