ifeq ($(call is-vendor-board-platform,QCOM),true)
ifeq ($(call is-board-platform-in-list,pineapple niobe),true)

LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := sputestclientv2
LOCAL_PROPRIETARY_MODULE := true
LOCAL_MODULE_OWNER := qti

LOCAL_C_INCLUDES += \
    $(LOCAL_PATH)/inc \
    $(TOP)/vendor/qcom/proprietary/common/qti-utils \
    $(TOP)/vendor/qcom/proprietary/spu/sputils/inc

LOCAL_HEADER_LIBRARIES := vendor_common_inc


LOCAL_CFLAGS+= -Werror -Wall

LOCAL_SRC_FILES := src/main.cpp \
     src/SPComClientTest.cpp \
     src/SPComServerTest.cpp \
     src/SPUNotifier.cpp \
     src/SPUTestFw.cpp

LOCAL_SHARED_LIBRARIES := \
    libhidlbase \
    libhidlmemory \
    liblog \
    libutils \
    libqti-utils \
    android.hidl.allocator@1.0 \
    vendor.qti.spu@2.0

include $(BUILD_EXECUTABLE)

endif
endif
