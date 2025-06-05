LOCAL_PATH:= $(call my-dir)

#ifeq ($(BOARD_HAS_QTI_BT_XPAN),true)
include $(CLEAR_VARS)
LOCAL_MODULE := vendor.qti.hardware.bluetooth.xpan_application_controller
LOCAL_MODULE_RELATIVE_PATH := hw
LOCAL_PROPRIETARY_MODULE := true

LOCAL_C_INCLUDES += vendor/qcom/proprietary/bluetooth/xpan/xpan_application_controller/include
LOCAL_C_INCLUDES += vendor/qcom/proprietary/bluetooth/xpan/common/include
LOCAL_C_INCLUDES += vendor/qcom/proprietary/bluetooth/xpan/qhci/include
LOCAL_C_INCLUDES += vendor/qcom/proprietary/bluetooth/xpan/xpan_manager/include

LOCAL_SRC_FILES := \
    src/XpanAcQhciIf.cpp \
    src/XpanAcXmIf.cpp \
    src/xpan_ac_main.cpp \
    src/XpanAcStateMachine.cpp \
    src/XpanDevice.cpp \
    src/XpanLmpManager.cpp \
    src/XpanSocketHandler.cpp \
    src/XpanAcTimer.cpp \

LOCAL_SHARED_LIBRARIES := \
    libbase \
    liblog \
    libhardware \
    libutils \
    libcutils \
    libbinder_ndk \

include $(BUILD_STATIC_LIBRARY)

#endif #BOARD_HAS_QTI_BT_XPAN
