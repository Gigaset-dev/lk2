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
GICV3_SUPPORT_GIC600 := 1
MEMBASE ?= 0x50700000
MEMSIZE ?= 0x00800000

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
	platform/$(PLATFORM)/common/pmic/mt6359/mt6359_auxadc.c \
	platform/$(PLATFORM)/common/pmic/mt6359/mt6359_gauge.c \
	platform/$(PLATFORM)/common/pmic/mt6359/mt6359_keys.c \
	platform/$(PLATFORM)/common/pmic/mt6359/mt6359_psc.c \
	platform/$(PLATFORM)/common/pmic_dlpt/mt6359_dlpt.c \
	platform/$(PLATFORM)/common/rtc/mt6359/mt6359_rtc.c \
	platform/$(PLATFORM)/common/init.c

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
	$(LOCAL_DIR)/dcm \
	$(LOCAL_DIR)/devinfo \
	$(LOCAL_DIR)/dfd_mcu \
	$(LOCAL_DIR)/disp \
	$(LOCAL_DIR)/gpio \
	$(LOCAL_DIR)/mtcmos \
	$(LOCAL_DIR)/musb \
	$(LOCAL_DIR)/pmic_wrap \
	$(LOCAL_DIR)/ufs \
	$(LOCAL_DIR)/mmc \
	dev/interrupt/arm_gic_v3 \
	dev/timer/arm_generic \
	lib/kcmdline \
	lib/pl_boottags \
	platform/$(PLATFORM)/common/aee \
	platform/$(PLATFORM)/common/arch/$(ARCH) \
	platform/$(PLATFORM)/common/battery \
	platform/$(PLATFORM)/common/bootctrl/3 \
	platform/$(PLATFORM)/common/charger \
	platform/$(PLATFORM)/common/dconfig \
	platform/$(PLATFORM)/common/debug \
	platform/$(PLATFORM)/common/devapc \
	platform/$(PLATFORM)/common/gic \
	platform/$(PLATFORM)/common/i2c \
	platform/$(PLATFORM)/common/leds \
	platform/$(PLATFORM)/common/logo \
	platform/$(PLATFORM)/common/md \
	platform/$(PLATFORM)/common/pmic \
	platform/$(PLATFORM)/common/pmic_dlpt \
	platform/$(PLATFORM)/common/rng \
	platform/$(PLATFORM)/common/rtc \
	platform/$(PLATFORM)/common/sboot \
	platform/$(PLATFORM)/common/scp/RV \
	platform/$(PLATFORM)/common/sda \
	platform/$(PLATFORM)/common/spm \
	platform/$(PLATFORM)/common/spmi \
	platform/$(PLATFORM)/common/sspm \
	platform/$(PLATFORM)/common/subpmic/mt6315 \
	platform/$(PLATFORM)/common/sysenv \
	platform/$(PLATFORM)/common/tee \
	platform/$(PLATFORM)/common/tfa \
	platform/$(PLATFORM)/common/trustzone \
	platform/$(PLATFORM)/common/uart \
	platform/$(PLATFORM)/common/ufs \
	platform/$(PLATFORM)/common/video \
	platform/$(PLATFORM)/common/wdt

#To select i2c register map version
#we need setting here otherwise it would build failed due to
#no value in platform/mediatek/common/i2c/rules.mk
I2C_REG_MAP_VER := 3
CHECK_HAS_BATTERY_REMOVED := 1
WITH_PMIC_LEGACY_AUXADC := 1

#SCP Setting for Secure Dump & EMI MPU
SCP_CORE_NUMS := 1
SCP_DRAM_OFFSET := 0x200000
MTK_TINYSYS_SCP_SECURE_DUMP := no
SCP_RESERVED_SHARE_DRAM_SIZE := 0x3A0000  # 3MB + 128K share mem size
SCP_DUMP_SIZE := 0x200000 #L2TCM:768KB, L1C:240KB, REG:15KB, TBUF:1KB, DRAM:1MB
SCP_SECURE_DOMAIN := 1
SCP_ENABLE_EMI_PROTECTION := no  #TODO: change to yes when emi ready
SCP_EMI_REGION := 6       #for SCP-ROM
SCP_SHARE_EMI_REGION := 7 #for SCP-SHARE

include make/module.mk
