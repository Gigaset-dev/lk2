LOCAL_DIR := $(GET_LOCAL_DIR)

MODULE := $(LOCAL_DIR)

ARCH    := arm64
ARM_CPU := armv8-a
WITH_SMP ?= 0

GLOBAL_INCLUDES += \
	platform/$(PLATFORM)/common/include

ifeq ($(LK_AS), BL2_EXT)
GICV3_SUPPORT_GIC600 := 1

MEMBASE ?= 0x62F00000
MEMSIZE ?= 0x02000000

MODULE_SRCS += $(LOCAL_DIR)/platform_$(BOOT_APP).c
else ifeq ($(LK_AS), AEE)
GICV3_SUPPORT_GIC600 := 1
MTK_MRDUMP_PARTITION_ENABLE := yes
MEMBASE ?= 0x50700000
MEMSIZE ?= 0x00800000

MODULE_SRCS += \
	$(LOCAL_DIR)/platform_$(BOOT_APP).c
else
MTK_MRDUMP_PARTITION_ENABLE := yes
MEMBASE ?= 0x50F00000
MEMSIZE ?= 0x12000000

MODULE_SRCS += $(LOCAL_DIR)/platform.c
endif

MODULE_SRCS += \
	platform/$(PLATFORM)/common/charger/mt6375_charger.c \
	platform/$(PLATFORM)/common/debug/dynamic_log.c \
	platform/$(PLATFORM)/common/init.c \
	platform/$(PLATFORM)/common/pmic/mt6363/mt6363_keys.c \
	platform/$(PLATFORM)/common/pmic/mt6363/mt6363_psc.c \
	platform/$(PLATFORM)/common/pmic_auxadc/mt6363_auxadc.c \
	platform/$(PLATFORM)/common/pmic_auxadc/mt6375_auxadc.c \
	platform/$(PLATFORM)/common/pmic_auxadc/mt6363_sdmadc.c \
	platform/$(PLATFORM)/common/pmic_auxadc/mt6368_sdmadc.c \
	platform/$(PLATFORM)/common/pmic_dlpt/mt6375_dlpt.c \
	platform/$(PLATFORM)/common/rtc/mt6685/mt6685_rtc.c \
	platform/$(PLATFORM)/common/subpmic/mt6375/mt6375_gauge.c \

# Although KERNEL_ASPACE_BASE will be defined in arch/$(ARCH)/rules.mk, we need it
# here to caculate KERNEL_BASE, and its value MUST be the same as the one in
# arch/arm/rules.mk.
ifeq ($(ARCH),arm)
KERNEL_ASPACE_BASE = 0x40000000
MODULE_SRCS += \
	platform/$(PLATFORM)/common/arch/$(ARCH)/cpu_early_init.c \
	platform/$(PLATFORM)/common/arch/$(ARCH)/mmu_lpae.c
endif

KERNEL_BASE ?= $(shell printf 0x%x $$(($(KERNEL_ASPACE_BASE) + $(MEMBASE))))
GLOBAL_DEFINES += MMU_WITH_TRAMPOLINE=1

GLOBAL_DEFINES += \
        SUB_PLATFORM=\"$(SUB_PLATFORM)\" \
        LK_AS=\"$(LK_AS)\"

ifneq (, $(findstring fpga, $(PROJECT)))
MODULE_DEFINES += \
	PROJECT_TYPE_FPGA
endif

ifeq ($(MTK_RUNTIME_SWITCH_FPS_SUPPORT), yes)
GLOBAL_DEFINES += MTK_RUNTIME_SWITCH_FPS_SUPPORT
endif

MODULE_DEPS += \
	$(LOCAL_DIR)/clkbuf \
	$(LOCAL_DIR)/consys \
	$(LOCAL_DIR)/dcm \
	$(LOCAL_DIR)/devinfo \
	$(LOCAL_DIR)/dfd_mcu \
	$(LOCAL_DIR)/disp \
	$(LOCAL_DIR)/gpio \
	$(LOCAL_DIR)/gpu_dfd \
	$(LOCAL_DIR)/hre \
	$(LOCAL_DIR)/mcdi \
	$(LOCAL_DIR)/mmc \
	$(LOCAL_DIR)/mminfra \
	$(LOCAL_DIR)/mtcmos \
	$(LOCAL_DIR)/musb \
	$(LOCAL_DIR)/thermal \
	$(LOCAL_DIR)/ufs \
	dev/interrupt/arm_gic_v3 \
	dev/timer/arm_generic \
	lib/kcmdline \
	lib/mblock \
	lib/pl_boottags \
	platform/$(PLATFORM)/common/aee \
	platform/$(PLATFORM)/common/arch/$(ARCH) \
	platform/$(PLATFORM)/common/battery \
	platform/$(PLATFORM)/common/bootctrl/3 \
	platform/$(PLATFORM)/common/charger \
	platform/$(PLATFORM)/common/cmdq \
	platform/$(PLATFORM)/common/cpu_sys_pi \
	platform/$(PLATFORM)/common/dconfig \
	platform/$(PLATFORM)/common/debug \
	platform/$(PLATFORM)/common/devapc \
	platform/$(PLATFORM)/common/dpm \
	platform/$(PLATFORM)/common/dramc \
	platform/$(PLATFORM)/common/dvfs \
	platform/$(PLATFORM)/common/emi \
	platform/$(PLATFORM)/common/geniezone \
	platform/$(PLATFORM)/common/gic \
	platform/$(PLATFORM)/common/gpueb \
	platform/$(PLATFORM)/common/i2c \
	platform/$(PLATFORM)/common/keypad \
	platform/$(PLATFORM)/common/leds \
	platform/$(PLATFORM)/common/logo \
	platform/$(PLATFORM)/common/mcupm \
	platform/$(PLATFORM)/common/md \
	platform/$(PLATFORM)/common/picachu \
	platform/$(PLATFORM)/common/pmic \
	platform/$(PLATFORM)/common/pmic_auxadc \
	platform/$(PLATFORM)/common/pmic_dlpt \
	platform/$(PLATFORM)/common/regmap \
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
	platform/$(PLATFORM)/common/subpmic \
	platform/$(PLATFORM)/common/subpmic/mt6368 \
	platform/$(PLATFORM)/common/subpmic/mt6375 \
	platform/$(PLATFORM)/common/sysenv \
	platform/$(PLATFORM)/common/tee \
	platform/$(PLATFORM)/common/tfa \
	platform/$(PLATFORM)/common/trustzone \
	platform/$(PLATFORM)/common/sramrc \
	platform/$(PLATFORM)/common/uart \
	platform/$(PLATFORM)/common/vcoredvfs \
	platform/$(PLATFORM)/common/ufs \
	platform/$(PLATFORM)/common/vcp \
	platform/$(PLATFORM)/common/vcp/RV \
	platform/$(PLATFORM)/common/video \
	platform/$(PLATFORM)/common/wdt

PMIC_DUMMY_LOAD := MT6363

#To select i2c register map version
#we need setting here otherwise it would build failed due to
#no value in platform/mediatek/common/i2c/rules.mk
I2C_REG_MAP_VER := 3

#EMI
#To select emi info version
#we need setting here otherwise it would build failed due to
#no value in platform/mediatek/common/dramc/rules.mk and
#no value in platform/mediatek/common/emi/rules.mk
EMI_INFO_VER := 2

# choose one of following value -> 1: disabled/ 2: permissive /3: enforcing
# overwrite SELINUX_STATUS value with PRJ_SELINUX_STATUS, if defined. it's by project variable.
SELINUX_STATUS := 3
ifdef PRJ_SELINUX_STATUS
SELINUX_STATUS := $(PRJ_SELINUX_STATUS)
endif

#SCP
SCP_CORE_NUMS := 1
SCP_RESERVED_SHARE_DRAM_SIZE := 0x005A0000
MTK_TINYSYS_SCP_SECURE_DUMP := yes
SCP_DUMP_SIZE := 0x300000 #L2TCM:1.5MB, DRAM:1MB, L1C:256KB, REG:15KB, TBUF:1KB
SCP_SECURE_DOMAIN := 3    #set to SCP0

#enable EMI_MPU
SCP_ENABLE_EMI_PROTECTION := yes
#for SCP-ROM
SCP_EMI_REGION := 26
#for SCP-SHARE
SCP_SHARE_EMI_REGION := 27
include make/module.mk

#RSC support(optional)
-include $(LOCAL_DIR)/../../../../../../../internal/bootloader/lk2/rsc/rules.mk
