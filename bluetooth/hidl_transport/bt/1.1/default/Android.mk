LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE := android.hardware.bluetooth@1.1-impl-qti
LOCAL_MODULE_RELATIVE_PATH := hw
LOCAL_PROPRIETARY_MODULE := true

ifeq ($(UART_BAUDRATE_3_0_MBPS),true)
LOCAL_CPPFLAGS := -DUART_BAUDRATE_3_0_MBPS
endif

LOCAL_SRC_FILES := \
        bluetooth_hci.cpp

LOCAL_CFLAGS += -DDIAG_ENABLED
LOCAL_CFLAGS += -Werror=unused-variable
# disable below flag to disable IBS
LOCAL_CFLAGS += -DWCNSS_IBS_ENABLED
# Disable this flag for disabling wakelocks
LOCAL_CFLAGS += -DWAKE_LOCK_ENABLED

ifeq ($(BOARD_HAVE_QTI_BT_LAZY_SERVICE),true)
LOCAL_CFLAGS += -DLAZY_SERVICE
endif

ifneq (,$(filter userdebug eng,$(TARGET_BUILD_VARIANT)))
LOCAL_CFLAGS += -DDUMP_IPC_LOG -DDUMP_RINGBUF_LOG -DDETECT_SPURIOUS_WAKE -DENABLE_HEALTH_TIMER
#LOCAL_CFLAGS += -DDUMP_HEALTH_INFO_TO_FILE
#LOCAL_C_INCLUDES += vendor/qcom/proprietary/bt/hci_qcomm_init
LOCAL_CFLAGS += -DENABLE_FW_CRASH_DUMP -DUSER_DEBUG
endif
ifeq ($(TARGET_HAS_BT_QCV_FOR_SPF), true)
LOCAL_CPPFLAGS += -DQTI_BT_QCV_SUPPORTED
endif #TARGET_HAS_BT_QCV_FOR_SPF

ifeq ($(TARGET_BOARD_AUTO),true)
LOCAL_CPPFLAGS += -DTARGET_BOARD_AUTO
endif # TARGET_BOARD_AUTO

LOCAL_C_INCLUDES += vendor/qcom/proprietary/securemsm/mink/inc/uid
LOCAL_C_INCLUDES += vendor/qcom/proprietary/securemsm/mink/inc/interface
LOCAL_C_INCLUDES += vendor/qcom/proprietary/securemsm/peripheralStateUtils/inc
LOCAL_C_INCLUDES += vendor/qcom/proprietary/bluetooth/hidl_transport/bttpi/default/

LOCAL_C_INCLUDES += vendor/qcom/proprietary/qmi/inc
LOCAL_C_INCLUDES += vendor/qcom/proprietary/qmi/platform
LOCAL_C_INCLUDES += vendor/qcom/proprietary/qcril-qmi-services/
LOCAL_C_INCLUDES += vendor/qcom/proprietary/bluetooth/hidl_transport/bt/1.0/default/

LOCAL_SHARED_LIBRARIES := \
    libbase \
    libcutils \
    libhidlbase \
    liblog \
    libutils \
    libdiag \
    libqmi_cci \
    libbtnv \
    android.hardware.bluetooth@1.0 \
    android.hardware.bluetooth@1.1 \
    android.hardware.bluetooth@1.0-impl-qti \
    libsoc_helper

ifeq ($(TARGET_LEGACY_QMI_NOT_SUPPORTED), true)
LOCAL_CPPFLAGS+= -DFEATURE_LEGACY_QMI_NOT_SUPPORTED
else
LOCAL_SHARED_LIBRARIES += libqmiservices
endif

LOCAL_HEADER_LIBRARIES := libril-qc-qmi-services-headers libdiag_headers vendor_common_inc

include $(BUILD_SHARED_LIBRARY)

#service

include $(CLEAR_VARS)
LOCAL_MODULE_RELATIVE_PATH := hw
LOCAL_PROPRIETARY_MODULE := true

ifeq ($(BOARD_HAVE_QTI_BT_LAZY_SERVICE),true)
LOCAL_MODULE := android.hardware.bluetooth@1.1-service-qti-lazy
LOCAL_CPPFLAGS += -DLAZY_SERVICE
ifeq ($(TARGET_BOARD_PLATFORM), msm8937)
LOCAL_INIT_RC := lazy-hal-rc/msm8937_32go/android.hardware.bluetooth@1.0-service-qti-lazy.rc
else
LOCAL_INIT_RC := lazy-hal-rc/common/android.hardware.bluetooth@1.0-service-qti-lazy.rc
endif
else
LOCAL_MODULE := android.hardware.bluetooth@1.1-service-qti
ifneq (,$(filter userdebug eng, $(TARGET_BUILD_VARIANT)))
LOCAL_INIT_RC := android.hardware.bluetooth@1.1-service-qti-debug.rc
else
LOCAL_INIT_RC := android.hardware.bluetooth@1.1-service-qti.rc
endif
endif #BOARD_HAVE_QTI_BT_LAZY_SERVICE

LOCAL_SRC_FILES := \
  service.cpp

ifeq ($(BOARD_HAS_QTI_BT_CP), true)
LOCAL_CFLAGS += -DBT_CP_CONNECTED
endif

LOCAL_SHARED_LIBRARIES := \
  liblog \
  libcutils \
  libutils \
  libhidlbase \
  libbinder_ndk \
  android.hardware.bluetooth@1.0 \
  android.hardware.bluetooth@1.1 \
  android.hardware.bluetooth@1.0-impl-qti \
  android.hardware.bluetooth@1.1-impl-qti \
  vendor.qti.hardware.bttpi-V3-ndk \

LOCAL_C_INCLUDES += vendor/qcom/proprietary/securemsm/mink/inc/uid
LOCAL_C_INCLUDES += vendor/qcom/proprietary/securemsm/mink/inc/interface
LOCAL_C_INCLUDES += vendor/qcom/proprietary/securemsm/peripheralStateUtils/inc
LOCAL_C_INCLUDES += vendor/qcom/proprietary/bluetooth/hidl_transport/bt/1.0/default/
LOCAL_C_INCLUDES += vendor/qcom/proprietary/bluetooth/hidl_transport/bttpi/default/

LOCAL_HEADER_LIBRARIES := libdiag_headers vendor_common_inc

ifeq ($(TARGET_HAS_BT_QCV_FOR_SPF), true)
LOCAL_CPPFLAGS += -DQTI_BT_QCV_SUPPORTED
LOCAL_C_INCLUDES += vendor/qcom/proprietary/qcv-utils/libsoc-helper/native/inc

LOCAL_SHARED_LIBRARIES += libsoc_helper
LOCAL_SRC_FILES +=../../1.0/default/soc_properties.cpp
endif #TARGET_HAS_BT_QCV_FOR_SPF

ifeq ($(BOARD_HAVE_QCOM_FM),true)
LOCAL_SHARED_LIBRARIES += vendor.qti.hardware.fm@1.0
LOCAL_CPPFLAGS += -DQCOM_FM_SUPPORTED
endif #BOARD_HAVE_QCOM_FM

ifneq ($(filter msm8909 msm8937 msm8953,$(TARGET_BOARD_PLATFORM)),)
else
ifeq ($(BOARD_ANT_WIRELESS_DEVICE),"qualcomm-hidl")
LOCAL_SHARED_LIBRARIES += com.dsi.ant@1.0
LOCAL_CPPFLAGS += -DQCOM_ANT_SUPPORTED
endif
endif

ifeq ($(BOARD_HAS_QTI_BT_XPAN),true)
LOCAL_C_INCLUDES += vendor/qcom/proprietary/bluetooth/xpan/xpan_provider
LOCAL_SHARED_LIBRARIES += vendor.qti.hardware.bluetooth.xpanprovider-V1-ndk
LOCAL_SHARED_LIBRARIES += vendor.qti.hardware.bluetooth.xpanprovider-impl-qti
LOCAL_CPPFLAGS += -DQTI_BT_XPANPROVIDER_SUPPORTED
endif # BOARD_HAS_QTI_BT_XPAN

ifeq ($(TARGET_USE_QTI_BT_SAR),true)
LOCAL_SHARED_LIBRARIES += vendor.qti.hardware.bluetooth_sar@1.0
LOCAL_SHARED_LIBRARIES += vendor.qti.hardware.bluetooth_sar@1.1
LOCAL_CPPFLAGS += -DQTI_BT_SAR_SUPPORTED
endif # TARGET_USE_QTI_BT_SAR

LOCAL_SHARED_LIBRARIES += vendor.qti.hardware.bttpi-impl

ifeq ($(TARGET_USE_QTI_BT_CONFIGSTORE),true)
LOCAL_SHARED_LIBRARIES += vendor.qti.hardware.btconfigstore@1.0
LOCAL_SHARED_LIBRARIES += vendor.qti.hardware.btconfigstore@2.0
LOCAL_CPPFLAGS += -DQTI_BT_CONFIGSTORE_SUPPORTED
endif # TARGET_USE_QTI_BT_CONFIGSTORE

ifeq ($(TARGET_USE_QTI_VND_FWK_DETECT),true)
LOCAL_SHARED_LIBRARIES += libqti_vndfwk_detect_vendor
LOCAL_CPPFLAGS += -DQTI_VND_FWK_DETECT_SUPPORTED
endif # TARGET_USE_QTI_VND_FWK_DETECT

ifeq ($(TARGET_USE_HCI_MANIFEST_FRAGMENT),true)
LOCAL_VINTF_FRAGMENTS := bluetooth_hci.xml
endif

include $(BUILD_EXECUTABLE)
