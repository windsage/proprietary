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

-include $(LOCAL_PATH)/configs/Android.mk
