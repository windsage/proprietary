LOCAL_PATH := $(call my-dir)
SECUREMSM_PATH := $(TOP)/vendor/qcom/proprietary/securemsm

include $(CLEAR_VARS)
LOCAL_MODULE := pmic_key_reset
LOCAL_CFLAGS := $(COMMON_CFLAGS) -Wall -Werror

LOCAL_C_INCLUDES := $(SECUREMSM_PATH)/mink/inc/interface \
                    $(SECUREMSM_PATH)/smcinvoke/inc/ \
                    $(LOCAL_PATH)/inc

LOCAL_SHARED_LIBRARIES := libminkdescriptor

LOCAL_SRC_FILES := pmic_key_reset.cpp

LOCAL_MODULE_OWNER := qti
LOCAL_MODULE_TAGS := optional
LOCAL_PROPRIETARY_MODULE := true

include $(BUILD_EXECUTABLE)
