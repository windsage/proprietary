LOCAL_PATH := $(call my-dir)

ifeq ($(BOARD_HAS_QTI_BT_CP), true)
include $(CLEAR_VARS)
LOCAL_MODULE := libbtxpanmanager
LOCAL_PROPRIETARY_MODULE := true
LOCAL_MODULE_RELATIVE_PATH := hw

LOCAL_SRC_FILES := \
    src/xm_glink_transport.cpp \
    src/xm_main.cpp \
    src/xm_kp_transport.cpp \
    src/xm_async_fd_watcher.cpp \
    src/xm_packetizer.cpp 

BT_HAL_DIR:= vendor/qcom/proprietary/bluetooth/hidl_transport/bt/1.0/default
LOCAL_C_INCLUDES += $(BT_HAL_DIR)
LOCAL_C_INCLUDES += vendor/qcom/proprietary/common/inc
LOCAL_C_INCLUDES += vendor/qcom/proprietary/securemsm/mink/inc/uid
LOCAL_C_INCLUDES += vendor/qcom/proprietary/securemsm/mink/inc/interface
LOCAL_C_INCLUDES += vendor/qcom/proprietary/securemsm/peripheralStateUtils/inc
LOCAL_C_INCLUDES += vendor/qcom/proprietary/bluetooth/xpan/xpan_manager/include
LOCAL_C_INCLUDES += vendor/qcom/proprietary/bluetooth/xpan/common/include

LOCAL_HEADER_LIBRARIES := libdiag_headers

LOCAL_SHARED_LIBRARIES := \
    libcutils \
    libhidlbase \
    liblog \
    libutils \
    libnl \
    libbase \
    libnl \
    libhardware_legacy \
    libhardware \
    android.hardware.bluetooth@1.0-impl-qti

ifeq ($(BOARD_HAS_QTI_BT_XPAN), true)
LOCAL_CFLAGS += -DXPAN_ENABLED
LOCAL_SRC_FILES += \
    src/xm_qhci_if.cpp \
    src/xm_xprofile_if.cpp \
    src/xm_xac_if.cpp \
    src/xm_wifi_if.cpp \
    src/xm_state_machine.cpp \
    src/timer.cpp
LOCAL_C_INCLUDES += vendor/qcom/proprietary/bluetooth/xpan/xpan_wifi_lib/inc
LOCAL_C_INCLUDES += vendor/qcom/proprietary/bluetooth/xpan/xpan_provider
LOCAL_C_INCLUDES += vendor/qcom/proprietary/bluetooth/xpan/qhci/include
LOCAL_C_INCLUDES += vendor/qcom/proprietary/bluetooth/xpan/xpan_application_controller/include
LOCAL_SHARED_LIBRARIES += libxpan_wifi_hal \
                          libbinder_ndk \
                          vendor.qti.hardware.bluetooth.xpanprovider-V1-ndk
LOCAL_SHARED_LIBRARIES += vendor.qti.hardware.bluetooth.xpanprovider-impl-qti
LOCAL_STATIC_LIBRARIES += libbtxpan_qhci
LOCAL_STATIC_LIBRARIES += vendor.qti.hardware.bluetooth.xpan_application_controller
endif

include $(BUILD_STATIC_LIBRARY)
endif
