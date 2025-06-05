LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)
LOCAL_MODULE := vendor.qti.hardware.bluetooth_sar@1.1-impl
LOCAL_MODULE_RELATIVE_PATH := hw
LOCAL_PROPRIETARY_MODULE := true

LOCAL_SRC_FILES := \
    BluetoothSar.cpp

BT_HAL_DIR:= vendor/qcom/proprietary/bluetooth/hidl_transport/bt/1.0/default

LOCAL_C_INCLUDES +=  $(BT_HAL_DIR)
LOCAL_C_INCLUDES += vendor/qcom/proprietary/securemsm/mink/inc/uid
LOCAL_C_INCLUDES += vendor/qcom/proprietary/securemsm/mink/inc/interface
LOCAL_C_INCLUDES += vendor/qcom/proprietary/securemsm/peripheralStateUtils/inc

LOCAL_HEADER_LIBRARIES := libdiag_headers vendor_common_inc

LOCAL_SHARED_LIBRARIES := \
    libhidlbase \
    libutils \
    liblog \
    libbase \
    libcutils \
    vendor.qti.hardware.bluetooth_sar@1.0 \
    vendor.qti.hardware.bluetooth_sar@1.1 \
    android.hardware.bluetooth@1.0-impl-qti \

include $(BUILD_SHARED_LIBRARY)
