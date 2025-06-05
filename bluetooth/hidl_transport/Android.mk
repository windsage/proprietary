LOCAL_DIR_PATH:= $(call my-dir)
ifeq ($(BOARD_HAVE_BLUETOOTH_QCOM),true)

LOCAL_PATH := $(LOCAL_DIR_PATH)
TMP_LOCAL_PATH := $(LOCAL_DIR_PATH)
ifeq ($(BOARD_HAVE_QTI_BT_SERVICE_VER_1_1), true)
include $(LOCAL_PATH)/bt/1.1/default/Android.mk
endif
include $(TMP_LOCAL_PATH)/bt/1.0/default/Android.mk

ifeq ($(TARGET_USE_QTI_BT_SAR),true)
LOCAL_PATH := $(LOCAL_DIR_PATH)
include $(LOCAL_PATH)/bluetooth_sar/1.1/default/Android.mk
endif # TARGET_USE_QTI_BT_SAR

LOCAL_PATH := $(LOCAL_DIR_PATH)
include $(LOCAL_PATH)/bttpi/default/Android.mk

ifeq ($(TARGET_USE_QTI_BT_CONFIGSTORE),true)
LOCAL_PATH := $(LOCAL_DIR_PATH)
include $(LOCAL_PATH)/btconfigstore/1.0/default/Android.mk

TMP_LOCAL_PATH := $(LOCAL_DIR_PATH)
include $(TMP_LOCAL_PATH)/btconfigstore/2.0/default/Android.mk
endif # TARGET_USE_QTI_BT_CONFIGSTORE

endif # BOARD_HAVE_BLUETOOTH_QCOM

ifeq ($(BOARD_HAVE_QCOM_FM),true)
   LOCAL_PATH := $(LOCAL_DIR_PATH)
   include $(LOCAL_PATH)/fm/1.0/default/Android.mk
endif # BOARD_HAVE_QCOM_FM

ifneq ($(BOARD_ANT_WIRELESS_DEVICE),)
   LOCAL_PATH := $(LOCAL_DIR_PATH)
   include $(LOCAL_PATH)/ant/1.0/default/Android.mk
endif # BOARD_ANT_WIRELESS_DEVICE
