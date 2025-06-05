TS_FW_LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

ifeq ($(call is-board-platform,taro),true)
    include $(TS_FW_LOCAL_PATH)/taro/Android.mk
endif
