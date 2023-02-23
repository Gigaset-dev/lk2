LOCAL_DIR := $(GET_LOCAL_DIR)

LINKER_SCRIPT += $(BUILDDIR)/system-onesegment.ld

SHELL := /bin/bash

LK_AS ?= BL33

GLOBAL_DEFINES += \
	LK_AS_$(LK_AS)=1

include $(LOCAL_DIR)/$(SUB_PLATFORM)/rules.mk

