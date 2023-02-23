LOCAL_DIR := $(GET_LOCAL_DIR)

MODULE := $(LOCAL_DIR)

MODULE_INCLUDES += \
	platform/$(PLATFORM)/$(SUB_PLATFORM)/md

MODULE_SRCS += \
	$(LOCAL_DIR)/ccci_ld_ap_md_smem.c \
	$(LOCAL_DIR)/ccci_ld_md_api_wrapper.c \
	$(LOCAL_DIR)/ccci_ld_md_args.c \
	$(LOCAL_DIR)/ccci_ld_md_ass.c \
	$(LOCAL_DIR)/ccci_ld_md_core.c \
	$(LOCAL_DIR)/ccci_ld_md_log.c \
	$(LOCAL_DIR)/ccci_ld_md_mem.c \
	$(LOCAL_DIR)/ccci_ld_md_mpu.c \
	$(LOCAL_DIR)/ccci_ld_md_sec.c \
	$(LOCAL_DIR)/ccci_ld_md_tag_dt.c

#MODULE_SRCS += $(LOCAL_DIR)/ccci_dummy_ap.c

MODULE_DEPS += \
	external/lib/fdt \
	lib/bio \
	lib/mblock \
	lib/pl_boottags \
	platform/$(PLATFORM)/common/dtb_ops \
	platform/$(PLATFORM)/common/sysenv \
	platform/$(PLATFORM)/$(SUB_PLATFORM)/md

include make/module.mk
