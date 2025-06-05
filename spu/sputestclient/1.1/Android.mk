ifeq ($(call is-vendor-board-platform,QCOM),true)
ifeq ($(call is-board-platform-in-list,pineapple niobe),true)

LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := sputestclient
LOCAL_PROPRIETARY_MODULE := true
LOCAL_MODULE_OWNER := qti

LOCAL_C_INCLUDES += \
    $(LOCAL_PATH)/inc \
    $(TOP)/vendor/qcom/proprietary/common/qti-utils
LOCAL_HEADER_LIBRARIES := vendor_common_inc


LOCAL_CFLAGS+= -Werror -Wall

LOCAL_SRC_FILES := src/main.cpp \
     src/SpcomClientTest.cpp \
     src/SpcomServerTest.cpp \
     src/SpcomSSREvent.cpp \
     src/SPUTestFw.cpp

LOCAL_SHARED_LIBRARIES := \
    libhidlbase \
    libhidlmemory \
    liblog \
    libutils \
    libqti-utils \
    android.hidl.allocator@1.0 \
    vendor.qti.spu@1.0 \
    vendor.qti.spu@1.1

include $(BUILD_EXECUTABLE)

endif
endif
