LOCAL_DIR := $(GET_LOCAL_DIR)

MODULE := $(LOCAL_DIR)

GLOBAL_INCLUDES += $(LOCAL_DIR)

MODULE_SRCS += $(LOCAL_DIR)/avb_chain_partition_descriptor.c
MODULE_SRCS += $(LOCAL_DIR)/avb_crc32.c
MODULE_SRCS += $(LOCAL_DIR)/avb_crypto.c
MODULE_SRCS += $(LOCAL_DIR)/avb_cmdline.c
MODULE_SRCS += $(LOCAL_DIR)/avb_descriptor.c
MODULE_SRCS += $(LOCAL_DIR)/avb_footer.c
MODULE_SRCS += $(LOCAL_DIR)/avb_hash_descriptor.c
MODULE_SRCS += $(LOCAL_DIR)/avb_hashtree_descriptor.c
MODULE_SRCS += $(LOCAL_DIR)/avb_kernel_cmdline_descriptor.c
MODULE_SRCS += $(LOCAL_DIR)/avb_property_descriptor.c
MODULE_SRCS += $(LOCAL_DIR)/avb_rsa.c
MODULE_SRCS += $(LOCAL_DIR)/avb_sha256.c
MODULE_SRCS += $(LOCAL_DIR)/avb_sha512.c
MODULE_SRCS += $(LOCAL_DIR)/avb_slot_verify.c
MODULE_SRCS += $(LOCAL_DIR)/avb_sysdeps_posix.c
MODULE_SRCS += $(LOCAL_DIR)/avb_util.c
MODULE_SRCS += $(LOCAL_DIR)/avb_vbmeta_image.c
MODULE_SRCS += $(LOCAL_DIR)/avb_version.c
GLOBAL_DEFINES += AVB_COMPILATION

include make/module.mk
