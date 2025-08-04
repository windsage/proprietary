# Copyright (c) 2025 Transsion Holdings Limited. All rights reserved.
#
# Multi-platform perf-core encryption build script
# Author: chao.xu5
# Date: August 2025
#
# This script replaces the original platform-specific Android.mk files
# to provide unified multi-platform compilation and encryption support
# for perfboostsconfig.xml and perfconfigstore.xml files.
#
LOCAL_PATH := $(call my-dir)

ifeq ($(TARGET_PERF_DIR),)
    PERF_CONFIG_DIR := $(TARGET_BOARD_PLATFORM)
else
    PERF_CONFIG_DIR := $(TARGET_PERF_DIR)
endif

ifndef TRAN_PERFLOCK_ENCODE_SUPPORT
TRAN_PERFLOCK_ENCODE_SUPPORT := yes
endif

PERFBOOSTS_CONFIG_FILE := $(LOCAL_PATH)/$(PERF_CONFIG_DIR)/perfboostsconfig.xml
PERFCONFIG_STORE_FILE := $(LOCAL_PATH)/$(PERF_CONFIG_DIR)/perfconfigstore.xml

ifneq ($(wildcard $(PERFBOOSTS_CONFIG_FILE)),)

include $(CLEAR_VARS)

LOCAL_MODULE := perfboostsconfig.xml
LOCAL_PROPRIETARY_MODULE := true
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := ETC
LOCAL_MODULE_PATH := $(TARGET_OUT_VENDOR)/etc/perf

ifeq ($(strip $(TRAN_PERFLOCK_ENCODE_SUPPORT)),yes)
ENCRYPT_SCRIPT := $(LOCAL_PATH)/../perfcore_encrypt.py
PERFBOOSTS_INFILE := $(PERFBOOSTS_CONFIG_FILE)
PERFBOOSTS_OUTFILE_ABS := $(PRODUCT_OUT)/obj/perf-encrypted/perfboostsconfig.xml.enc
PERFBOOSTS_OUTFILE_REL := ../../../../../$(PRODUCT_OUT)/obj/perf-encrypted/perfboostsconfig.xml.enc

$(PERFBOOSTS_OUTFILE_ABS): $(PERFBOOSTS_INFILE) $(ENCRYPT_SCRIPT)
	@echo "Encrypting perfboostsconfig.xml for platform $(PERF_CONFIG_DIR)"
	@mkdir -p $(dir $@)
	python3 $(ENCRYPT_SCRIPT) -e $< $@

LOCAL_SRC_FILES := $(PERFBOOSTS_OUTFILE_REL)
LOCAL_ADDITIONAL_DEPENDENCIES := $(PERFBOOSTS_OUTFILE_ABS)
else
LOCAL_SRC_FILES := $(PERF_CONFIG_DIR)/perfboostsconfig.xml
endif

include $(BUILD_PREBUILT)

endif

ifneq ($(wildcard $(PERFCONFIG_STORE_FILE)),)

include $(CLEAR_VARS)

LOCAL_MODULE := perfconfigstore.xml
LOCAL_PROPRIETARY_MODULE := true
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := ETC
LOCAL_MODULE_PATH := $(TARGET_OUT_VENDOR)/etc/perf

ifeq ($(strip $(TRAN_PERFLOCK_ENCODE_SUPPORT)),yes)
ENCRYPT_SCRIPT := $(LOCAL_PATH)/../perfcore_encrypt.py
PERFCONFIG_INFILE := $(PERFCONFIG_STORE_FILE)
PERFCONFIG_OUTFILE_ABS := $(PRODUCT_OUT)/obj/perf-encrypted/perfconfigstore.xml.enc
PERFCONFIG_OUTFILE_REL := ../../../../../$(PRODUCT_OUT)/obj/perf-encrypted/perfconfigstore.xml.enc

$(PERFCONFIG_OUTFILE_ABS): $(PERFCONFIG_INFILE) $(ENCRYPT_SCRIPT)
	@echo "Encrypting perfconfigstore.xml for platform $(PERF_CONFIG_DIR)"
	@mkdir -p $(dir $@)
	python3 $(ENCRYPT_SCRIPT) -e $< $@

LOCAL_SRC_FILES := $(PERFCONFIG_OUTFILE_REL)
LOCAL_ADDITIONAL_DEPENDENCIES := $(PERFCONFIG_OUTFILE_ABS)
else
LOCAL_SRC_FILES := $(PERF_CONFIG_DIR)/perfconfigstore.xml
endif

include $(BUILD_PREBUILT)

endif
