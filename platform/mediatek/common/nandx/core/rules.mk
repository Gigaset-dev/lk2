LOCAL_DIR := $(GET_LOCAL_DIR)

MODULE := $(LOCAL_DIR)

MODULE_SRCS +=  \
	$(LOCAL_DIR)/core_io.c \
	$(LOCAL_DIR)/nand/device_slc.c \
	$(LOCAL_DIR)/nand/device_spi.c \
	$(LOCAL_DIR)/nand/nand_slc.c \
	$(LOCAL_DIR)/nand/nand_spi.c \
	$(LOCAL_DIR)/nand_base.c \
	$(LOCAL_DIR)/nand_chip.c \
	$(LOCAL_DIR)/nand_device.c \
	$(LOCAL_DIR)/nfi/nfi_base.c \
	$(LOCAL_DIR)/nfi/nfi_spi.c \
	$(LOCAL_DIR)/nfi/nfiecc.c \

include make/module.mk
