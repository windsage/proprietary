LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE := android.hardware.bluetooth@1.0-impl-qti
LOCAL_MODULE_RELATIVE_PATH := hw
LOCAL_PROPRIETARY_MODULE := true

ifeq ($(UART_BAUDRATE_3_0_MBPS),true)
LOCAL_CPPFLAGS := -DUART_BAUDRATE_3_0_MBPS
endif


LOCAL_SRC_FILES := \
    bluetooth_hci.cpp \
    bluetooth_address.cpp \
    async_fd_watcher.cpp \
    hci_packetizer.cpp \
    soc_properties.cpp \
    data_handler.cpp \
    uart_controller.cpp \
    patch_dl_manager.cpp \
    hci_uart_transport.cpp \
    power_manager.cpp \
    ibs_handler.cpp \
    logger.cpp \
    uart_ipc.cpp \
    uart_logs.cpp \
    ring_buffer.cpp \
    wake_lock.cpp \
    diag_interface.cpp \
    mct_controller.cpp \
    hci_mct_transport.cpp \
    nvm_tags_manager.cpp \
    health_info_log.cpp \
    state_info.cpp

ifeq ($(BOARD_HAS_QTI_BT_GANGES),true)
LOCAL_CPPFLAGS := -DBT_GANGES_SUPPORTED
LOCAL_SRC_FILES += \
    peri_patch_dl_manager.cpp \
    notify_signal.cpp
endif

ifeq ($(BOARD_HAVE_QTI_BT_SERVICE_VER_1_1), true)
LOCAL_CFLAGS += -DBT_VER_1_1
endif

ifeq ($(TARGET_DEFINE_LOCAL_MSM_IDS), true)
LOCAL_CFLAGS += -DMSM_CPU_KALAMA=519 -DMSM_CPU_PINEAPPLE=557
endif

LOCAL_CFLAGS += -DDIAG_ENABLED
LOCAL_CFLAGS += -Werror=unused-variable

# disable below flag to disable IBS/OBS
ifeq ($(TARGET_USE_QTI_BT_OBS), true)
LOCAL_CFLAGS += -DWCNSS_OBS_ENABLED
LOCAL_SRC_FILES += obs_handler.cpp
else
LOCAL_CFLAGS += -DWCNSS_IBS_ENABLED
endif

# Disable this flag for disabling wakelocks
LOCAL_CFLAGS += -DWAKE_LOCK_ENABLED

ifeq ($(BOARD_HAVE_QTI_BT_LAZY_SERVICE),true)
LOCAL_CFLAGS += -DLAZY_SERVICE
endif

#Enables the UART SYSFS support
ifeq ($(TARGET_HAVE_BT_UART_SYSFS_SUPPORT), true)
LOCAL_CPPFLAGS += -DQTI_BT_UART_SYSFS_SUPPORTED
endif #TARGET_HAVE_BT_UART_SYSFS_SUPPORT

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

LOCAL_C_INCLUDES += vendor/qcom/proprietary/bluetooth/hidl_transport/bttpi/default/

LOCAL_C_INCLUDES += vendor/qcom/proprietary/qmi/inc
LOCAL_C_INCLUDES += vendor/qcom/proprietary/qmi/platform
LOCAL_C_INCLUDES += vendor/qcom/proprietary/qcril-qmi-services/

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
    libsoc_helper

ifeq ($(strip $(ENABLE_PERIPHERAL_STATE_UTILS)), true)
LOCAL_CFLAGS += -DBT_SECURE_PERIPHERAL_ENABLED
LOCAL_C_INCLUDES += vendor/qcom/proprietary/securemsm/peripheralStateUtils/inc
LOCAL_SHARED_LIBRARIES +=  \
    libminksocket_vendor \
    libPeripheralStateUtils
LOCAL_HEADER_LIBRARIES += peripheralstate_headers
endif


ifeq ($(TARGET_LEGACY_QMI_NOT_SUPPORTED), true)
LOCAL_CPPFLAGS+= -DFEATURE_LEGACY_QMI_NOT_SUPPORTED
else
LOCAL_SHARED_LIBRARIES += libqmiservices
endif

ifeq ($(BOARD_HAS_QTI_BT_XPAN), true)
LOCAL_CFLAGS += -DBT_XPAN_ENABLED
LOCAL_SHARED_LIBRARIES += libxpan_wifi_hal \
                          libbinder_ndk \
                          vendor.qti.hardware.bluetooth.xpanprovider-V1-ndk
LOCAL_SHARED_LIBRARIES += vendor.qti.hardware.bluetooth.xpanprovider-impl-qti
LOCAL_STATIC_LIBRARIES += libbtxpan_qhci
LOCAL_STATIC_LIBRARIES += vendor.qti.hardware.bluetooth.xpan_application_controller
endif

ifeq ($(BOARD_HAS_QTI_BT_XPAN), true)
LOCAL_CFLAGS += -DXPAN_SUPPORTED
LOCAL_STATIC_LIBRARIES += libbtxpan_qhci
LOCAL_C_INCLUDES += vendor/qcom/proprietary/bluetooth/xpan/xpan_manager/include
LOCAL_C_INCLUDES += $(TOP)/vendor/qcom/proprietary/bluetooth/xpan/qhci/include
LOCAL_C_INCLUDES += vendor/qcom/proprietary/bluetooth/xpan/xpan_application_controller/include
endif

ifeq ($(BOARD_HAS_QTI_BT_CP), true)
LOCAL_CFLAGS += -DBT_CP_CONNECTED
LOCAL_STATIC_LIBRARIES += libbtxpanmanager
LOCAL_C_INCLUDES += vendor/qcom/proprietary/bluetooth/xpan/xpan_manager/include
LOCAL_C_INCLUDES += vendor/qcom/proprietary/bluetooth/xpan/common/include
endif

LOCAL_HEADER_LIBRARIES := libril-qc-qmi-services-headers libdiag_headers vendor_common_inc

include $(BUILD_SHARED_LIBRARY)

# compile service 1.0 if flag is set
ifneq ($(BOARD_HAVE_QTI_BT_SERVICE_VER_1_1), true)
include $(CLEAR_VARS)
LOCAL_MODULE_RELATIVE_PATH := hw
LOCAL_PROPRIETARY_MODULE := true

ifeq ($(BOARD_HAVE_QTI_BT_LAZY_SERVICE),true)
LOCAL_MODULE := android.hardware.bluetooth@1.0-service-qti-lazy
LOCAL_CPPFLAGS += -DLAZY_SERVICE
ifeq ($(TARGET_BOARD_PLATFORM), msm8937)
LOCAL_INIT_RC := lazy-hal-rc/msm8937_32go/android.hardware.bluetooth@1.0-service-qti-lazy.rc
else
LOCAL_INIT_RC := lazy-hal-rc/common/android.hardware.bluetooth@1.0-service-qti-lazy.rc
endif
else
LOCAL_MODULE := android.hardware.bluetooth@1.0-service-qti
ifneq (,$(filter userdebug eng, $(TARGET_BUILD_VARIANT)))
LOCAL_INIT_RC := android.hardware.bluetooth@1.0-service-qti-debug.rc
else
LOCAL_INIT_RC := android.hardware.bluetooth@1.0-service-qti.rc
endif
endif #BOARD_HAVE_QTI_BT_LAZY_SERVICE

LOCAL_SRC_FILES := \
  service.cpp

LOCAL_SHARED_LIBRARIES := \
  liblog \
  libcutils \
  libutils \
  libhidlbase \
  libbinder_ndk \
  android.hardware.bluetooth@1.0 \
  vendor.qti.hardware.bttpi-V3-ndk \

LOCAL_C_INCLUDES += vendor/qcom/proprietary/securemsm/mink/inc/uid
LOCAL_C_INCLUDES += vendor/qcom/proprietary/securemsm/mink/inc/interface
LOCAL_C_INCLUDES += vendor/qcom/proprietary/securemsm/peripheralStateUtils/inc
LOCAL_C_INCLUDES += vendor/qcom/proprietary/bluetooth/hidl_transport/bttpi/default/

LOCAL_HEADER_LIBRARIES := libdiag_headers vendor_common_inc

ifeq ($(TARGET_HAS_BT_QCV_FOR_SPF), true)
LOCAL_CPPFLAGS += -DQTI_BT_QCV_SUPPORTED
LOCAL_C_INCLUDES += vendor/qcom/proprietary/qcv-utils/libsoc-helper/native/inc

LOCAL_SHARED_LIBRARIES += libsoc_helper
LOCAL_SRC_FILES += soc_properties.cpp
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

include $(BUILD_EXECUTABLE)
endif
