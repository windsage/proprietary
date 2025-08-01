LOCAL_PATH := $(call my-dir)

ifeq ($(TARGET_PERF_DIR),)
    PERF_CONFIG_DIR := $(TARGET_BOARD_PLATFORM)
else
    PERF_CONFIG_DIR := $(TARGET_PERF_DIR)
endif

-include $(LOCAL_PATH)/configs/$(PERF_CONFIG_DIR)/Android.mk
