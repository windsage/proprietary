LOCAL_PATH:= $(call my-dir)
QC_PROP_ROOT ?= vendor/qcom/proprietary
SECUREMSM_PATH := $(TOP)/$(QC_PROP_ROOT)/securemsm-noship
SECUREMSM_SHIP_PATH := $(TOP)/$(QC_PROP_ROOT)/securemsm
COMPOSER_PATH := $(SECUREMSM_PATH)/TrustedUI/TrustedVM
TUIAPP_PATH := $(SECUREMSM_SHIP_PATH)/TrustedUI/TrustedVM
include $(CLEAR_VARS)
include $(LIBION_HEADER_PATH_WRAPPER)

commonIncludes := $(TARGET_OUT_INTERMEDIATES)/KERNEL_OBJ/usr/include

commonIncludes +=   $(LOCAL_PATH)/inc \
                    $(SECUREMSM_PATH)/ssgtzd \
                    $(TUIAPP_PATH)/libTrustedUI/inc \
                    $(SECUREMSM_SHIP_PATH)/mink/inc/interface \
                    $(SECUREMSM_SHIP_PATH)/smcinvoke/TZCom/inc \
                    $(SECUREMSM_PATH)/smcinvoke/inc \
                    $(SECUREMSM_SHIP_PATH)/ssg/mink/include \
                    $(TUIAPP_PATH)/Composer/TUIComposer/inc \
                    $(SECUREMSM_PATH)/CommonLib/inc \
                    $(SECUREMSM_NOSHIP_PATH)/ssgtzd/include \
                    $(SECUREMSM_NOSHIP_PATH)/ssgtzd

LOCAL_C_INCLUDES := $(LIBION_HEADER_PATHS)
LOCAL_C_INCLUDES += $(commonIncludes)

LOCAL_SHARED_LIBRARIES := \
        libcutils \
        libutils \
        liblog \

LOCAL_CFLAGS := -O3 -DTEST_ON_ANDROID -DENABLE_DEBUG_LOG

LOCAL_HEADER_LIBRARIES := vendor_common_inc

LOCAL_SHARED_LIBRARIES += libTrustedUILib libminksocket_vendor libcommonlib libtuirenderer

LOCAL_MODULE := tuiapp

LOCAL_PROPRIETARY_MODULE := true

LOCAL_SRC_FILES := src/main.cpp \
                   src/CTUISampleApp.cpp

LOCAL_MODULE_TAGS := optional

LOCAL_MODULE_OWNER := qti
include $(BUILD_NATIVE_TEST)
