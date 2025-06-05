LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE := vendor.qti.hardware.fm@1.0-impl
LOCAL_PROPRIETARY_MODULE := true
LOCAL_MODULE_RELATIVE_PATH := hw

LOCAL_SRC_FILES := \
    FmHci.cpp \
    FmIoctlsInterface.cpp \
    FmIoctlHci.cpp \

ifeq ($(TARGET_HAS_BT_QCV_FOR_SPF), true)
LOCAL_CPPFLAGS += -DQTI_BT_QCV_SUPPORTED
endif

BT_HAL_DIR:= vendor/qcom/proprietary/bluetooth/hidl_transport/bt/1.0/default
LOCAL_C_INCLUDES += $(BT_HAL_DIR)
LOCAL_C_INCLUDES += vendor/qcom/proprietary/securemsm/mink/inc/uid
LOCAL_C_INCLUDES += vendor/qcom/proprietary/securemsm/mink/inc/interface
LOCAL_C_INCLUDES += vendor/qcom/proprietary/securemsm/peripheralStateUtils/inc

LOCAL_HEADER_LIBRARIES := libdiag_headers vendor_common_inc
LOCAL_SHARED_LIBRARIES := \
    libbase \
    libcutils \
    libhidlbase \
    liblog \
    libutils \
    vendor.qti.hardware.fm@1.0 \
    android.hardware.bluetooth@1.0-impl-qti \
    libqmi_cci \

ifeq ($(TARGET_LEGACY_QMI_NOT_SUPPORTED), true)
LOCAL_CPPFLAGS+= -DFEATURE_LEGACY_QMI_NOT_SUPPORTED
else
LOCAL_SHARED_LIBRARIES += libqmiservices libqmi
endif

LOCAL_HEADER_LIBRARIES += libqmi_common_headers libqmi_cci_headers libqmi_csi_headers libril-qc-qmi-services-headers

include $(BUILD_SHARED_LIBRARY)
