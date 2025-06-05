LOCAL_PATH := $(call my-dir)

ifeq ($(BOARD_HAS_QTI_BT_XPAN), true)
include $(CLEAR_VARS)
LOCAL_MODULE := libbtxpan_qhci
LOCAL_PROPRIETARY_MODULE := true
LOCAL_MODULE_RELATIVE_PATH := hw

LOCAL_CFLAGS += -DXPAN_SUPPORTED

LOCAL_SRC_FILES := \
      src/qhci_main.cpp \
      src/qhci_xm_if.cpp \
      src/qhci_packetizer.cpp \
      src/qhci_timer.cpp \
      src/qhci_hci_handler.cpp \
      src/qhci_ac_if.cpp \
      src/qhci_tx.cpp \
      src/qhci_rx.cpp \
      src/qhci_hci_handler.cpp \
      src/qhci_utils.cpp \
      src/qhci_hm.cpp

BT_HAL_DIR:= vendor/qcom/proprietary/bluetooth/hidl_transport/bt/1.0/default
LOCAL_C_INCLUDES += $(BT_HAL_DIR)
LOCAL_C_INCLUDES += vendor/qcom/proprietary/common/inc
LOCAL_C_INCLUDES += vendor/qcom/proprietary/securemsm/mink/inc/uid
LOCAL_C_INCLUDES += vendor/qcom/proprietary/securemsm/mink/inc/interface
LOCAL_C_INCLUDES += vendor/qcom/proprietary/bluetooth/xpan/qhci/include
LOCAL_C_INCLUDES += vendor/qcom/proprietary/securemsm/mink/inc/interface
LOCAL_C_INCLUDES += vendor/qcom/proprietary/securemsm/peripheralStateUtils/inc
LOCAL_C_INCLUDES += vendor/qcom/proprietary/bluetooth/xpan/common/include
LOCAL_C_INCLUDES += vendor/qcom/proprietary/bluetooth/xpan/xpan_manager/include
LOCAL_C_INCLUDES += vendor/qcom/proprietary/bluetooth/xpan/xpan_application_controller/include


LOCAL_HEADER_LIBRARIES := libril-qc-qmi-services-headers libdiag_headers vendor_common_inc

LOCAL_STATIC_LIBRARIES += vendor.qti.hardware.bluetooth.xpan_application_controller

LOCAL_SHARED_LIBRARIES := \
      libcutils \
      libhidlbase \
      liblog \
      libutils \
      libbase \
      libdiag \
      libnl \
      libhardware_legacy \
      libhardware \
      libPeripheralStateUtils \
      android.hardware.bluetooth@1.0-impl-qti

include $(BUILD_STATIC_LIBRARY)
endif #BOARD_HAS_QTI_BT_XPAN
