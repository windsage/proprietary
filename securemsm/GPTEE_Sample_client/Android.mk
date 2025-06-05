LOCAL_PATH:= $(call my-dir)
SECUREMSM_SHIP_PATH := $(TOP)/vendor/qcom/proprietary/securemsm


include $(CLEAR_VARS)

commonIncludes := $(LOCAL_PATH) \
                  $(SECUREMSM_SHIP_PATH)/GPTEE/inc \

LOCAL_C_INCLUDES = $(commonIncludes)

LOCAL_STATIC_LIBRARIES :=

LOCAL_SHARED_LIBRARIES := \
        libGPTEE_vendor


LOCAL_CFLAGS := -O3

LOCAL_MODULE := GPTEE_Sample_client

LOCAL_PROPRIETARY_MODULE := true

LOCAL_SRC_FILES := GPTEE_Sample_client.cpp

LOCAL_MODULE_TAGS := optional

LOCAL_MODULE_OWNER := qti
include $(BUILD_EXECUTABLE)

