LOCAL_DIR := $(GET_LOCAL_DIR)

MODULE := $(LOCAL_DIR)

MODULE_SRCS += \
	$(LOCAL_DIR)/uart.c

ifneq (, $(findstring fpga, $(PROJECT)))
UART_SRC_CLK_FRQ ?= 10000000
else
UART_SRC_CLK_FRQ ?= 26000000
endif

MODULE_DEFINES += \
	SRC_CLK_FRQ=$(UART_SRC_CLK_FRQ)

include make/module.mk

