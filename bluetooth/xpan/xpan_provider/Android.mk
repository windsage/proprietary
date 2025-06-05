LOCAL_PATH:= $(call my-dir)

ifeq ($(BOARD_HAS_QTI_BT_XPAN),true)
include $(CLEAR_VARS)
LOCAL_MODULE := vendor.qti.hardware.bluetooth.xpanprovider-impl-qti
LOCAL_MODULE_RELATIVE_PATH := hw
LOCAL_PROPRIETARY_MODULE := true
LOCAL_VENDOR_MODULE := true

LOCAL_C_INCLUDES += vendor/qcom/proprietary/bluetooth/xpan/common/include

LOCAL_SRC_FILES := \
    XpanProviderService.cpp \

LOCAL_SHARED_LIBRARIES := \
    libbase \
    liblog \
    libhardware \
    libutils \
    libcutils \
    libbinder_ndk \
    vendor.qti.hardware.bluetooth.xpanprovider-V1-ndk \

LOCAL_VINTF_FRAGMENTS := vendor.qti.hardware.bluetooth.xpanprovider.xml

include $(BUILD_SHARED_LIBRARY)

endif #BOARD_HAS_QTI_BT_XPAN
