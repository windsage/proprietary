ifeq ($(TARGET_USES_QTI_BTFTM), true)
ifeq ($(call is-vendor-board-platform,QCOM), true)

# Build only if board has BT/FM/WLAN
ifeq ($(findstring true, $(BOARD_HAVE_QCOM_FM) $(BOARD_HAVE_BLUETOOTH)),true)

LOCAL_PATH:= $(call my-dir)

BDROID_DIR:= system/bt

# path changed in Android-T
BDROIDT_DIR:= packages/modules/Bluetooth/system

ifeq ($(TARGET_SUPPORTS_WEARABLES),true)
QTI_DIR  := $(BOARD_BT_DIR)/libbt-vendor
else
QTI_DIR  := hardware/qcom/bt/libbt-vendor
endif


include $(CLEAR_VARS)

LOCAL_HEADER_LIBRARIES :=  libdiag_headers vendor_common_inc
LOCAL_C_INCLUDES += vendor/qcom/proprietary/diag/src \


ifneq ($(TARGET_SUPPORTS_ANDROID_WEAR),true)
LOCAL_C_INCLUDES += vendor/qcom/proprietary/bt/hci_qcomm_init
endif

LOCAL_C_INCLUDES += vendor/qcom/opensource/fm/helium \
LOCAL_C_INCLUDES += vendor/qcom/proprietary/fm/fmvendorlib/helium \
LOCAL_C_INCLUDES += $(BDROID_DIR)/hci/include \
LOCAL_C_INCLUDES += $(BDROIDT_DIR)/hci/include
LOCAL_C_INCLUDES += $(QTI_DIR)/include
ifeq ($(TARGET_SUPPORTS_WEARABLES),true)
ifeq ($(TARGET_BOARD_PLATFORM),sdm429w_law)
LOCAL_C_INCLUDES += vendor/qcom/proprietary/bluetooth/hidl_client/inc
else
ifeq ($(call is-platform-sdk-version-at-least,29),true)
LOCAL_C_INCLUDES += vendor/qcom/proprietary/bluetooth/hidl_client/inc
else
LOCAL_C_INCLUDES += $(BOARD_OPENSOURCE_DIR)/bluetooth/tools/hidl_client/inc
endif
endif
else
LOCAL_C_INCLUDES += vendor/qcom/proprietary/bluetooth/hidl_client/inc
LOCAL_C_INCLUDES += vendor/qcom/proprietary/bluetooth/hidl_transport/bt/1.0/default
endif

LOCAL_CFLAGS:= \
              -DANDROID \
              -DDEBUG \
              -DSUPPORT_USB_INTERFACE

ifneq ($(DISABLE_BT_FTM),true)
LOCAL_CFLAGS +=  -DCONFIG_FTM_BT
endif

ifeq ($(BOARD_HAVE_QCOM_FM),true)
LOCAL_CFLAGS +=  -DCONFIG_FTM_FM
endif

ifeq ($(BOARD_HAS_QCA_FM_SOC), "cherokee")
LOCAL_CFLAGS += -DFM_SOC_TYPE_CHEROKEE
endif

ifeq ($(BOARD_HAVE_BLUETOOTH_BLUEZ), true)
    LOCAL_CFLAGS += -DHAS_BLUEZ_BUILDCFG
endif # BOARD_HAVE_BLUETOOTH_BLUEZ

LOCAL_SRC_FILES:= \
    ftm_main.c

ifneq ($(DISABLE_BT_FTM),true)
LOCAL_SRC_FILES += \
    ftm_bt.c \
    ftm_bt_power_pfal_linux.c \
    ftm_bt_hci_pfal_linux.c \
    ftm_bt_persist.cpp \
    diag_pkt_handler.c  \
    Socket.c
endif

ifeq ($(call is-platform-sdk-version-at-least,23),true)
LOCAL_CFLAGS += -DANDROID_M
endif

ifeq ($(BOARD_HAVE_QCOM_FM),true)
LOCAL_SRC_FILES += ftm_fm.c ftm_fm_pfal_linux_3990.c
endif

LOCAL_SHARED_LIBRARIES += libdl

ifneq ($(DISABLE_BT_FTM),true)
LOCAL_SHARED_LIBRARIES += libbt-hidlclient
endif

LOCAL_MODULE_PATH := $(TARGET_OUT_VENDOR_EXECUTABLES)
LOCAL_MODULE:= btftmdaemon
LOCAL_CLANG := true
ifeq ($(PRODUCT_VENDOR_MOVE_ENABLED),true)
LOCAL_PROPRIETARY_MODULE := true
endif
LOCAL_MODULE_TAGS := optional
LOCAL_SHARED_LIBRARIES  += libdiag
LOCAL_SHARED_LIBRARIES  += libcutils liblog libhardware
LOCAL_HEADER_LIBRARIES += libdiag_headers

ifneq ($(DISABLE_BT_FTM),true)
ifneq ($(TARGET_SUPPORTS_ANDROID_WEAR),true)
LOCAL_SHARED_LIBRARIES  += libbtnv
else
ifeq ($(call is-platform-sdk-version-at-least,29),true)
LOCAL_SHARED_LIBRARIES  += libbtnv
else
LOCAL_SHARED_LIBRARIES  += libbtnv-wear
endif
endif
endif

# By default NV persist gets used
LOCAL_CFLAGS += -DBT_NV_SUPPORT

LDFLAGS += -ldl

include $(BUILD_EXECUTABLE)
include $(call all-makefiles-under,$(LOCAL_PATH))

endif # filter
else // NON QCOM MSM Based configurations
LOCAL_PATH:= $(call my-dir)

BDROID_DIR:= system/bt
ifeq ($(TARGET_SUPPORTS_WEARABLES),true)
QTI_DIR  := $(BOARD_BT_DIR)/libbt-vendor
else
QTI_DIR  := hardware/qcom/bt/libbt-vendor
endif


include $(CLEAR_VARS)

LOCAL_HEADER_LIBRARIES :=  vendor_common_inc


ifneq ($(TARGET_SUPPORTS_ANDROID_WEAR),true)
LOCAL_C_INCLUDES += vendor/qcom/proprietary/bt/hci_qcomm_init
endif

LOCAL_C_INCLUDES += vendor/qcom/opensource/fm/helium \
LOCAL_C_INCLUDES += vendor/qcom/proprietary/fm/fmvendorlib/helium \
LOCAL_C_INCLUDES += $(BDROID_DIR)/hci/include \
LOCAL_C_INCLUDES += $(QTI_DIR)/include
ifeq ($(TARGET_SUPPORTS_WEARABLES),true)
ifeq ($(TARGET_BOARD_PLATFORM),sdm429w_law)
LOCAL_C_INCLUDES += vendor/qcom/proprietary/bluetooth/hidl_client/inc
else
ifeq ($(call is-platform-sdk-version-at-least,29),true)
LOCAL_C_INCLUDES += vendor/qcom/proprietary/bluetooth/hidl_client/inc
else
LOCAL_C_INCLUDES += $(BOARD_OPENSOURCE_DIR)/bluetooth/tools/hidl_client/inc
endif
endif
else
LOCAL_C_INCLUDES += vendor/qcom/proprietary/bluetooth/hidl_client/inc
LOCAL_C_INCLUDES += vendor/qcom/proprietary/bluetooth/hidl_transport/bt/1.0/default
endif

LOCAL_CFLAGS:= \
              -DANDROID \
              -DDEBUG \

ifneq ($(DISABLE_BT_FTM),true)
LOCAL_CFLAGS +=  -DCONFIG_FTM_BT
endif

#ifeq ($(BOARD_HAVE_QCOM_FM),true)
#LOCAL_CFLAGS +=  -DCONFIG_FTM_FM
#endif

ifeq ($(BOARD_HAS_QCA_FM_SOC), "cherokee")
LOCAL_CFLAGS += -DFM_SOC_TYPE_CHEROKEE
endif

#ifneq ($(BOARD_ANT_WIRELESS_DEVICE), )
#LOCAL_CFLAGS +=  -DCONFIG_FTM_ANT
#endif

ifeq ($(BOARD_HAVE_BLUETOOTH_BLUEZ), true)
    LOCAL_CFLAGS += -DHAS_BLUEZ_BUILDCFG
endif # BOARD_HAVE_BLUETOOTH_BLUEZ

LOCAL_SRC_FILES:= \
    ftm_main.c

ifneq ($(DISABLE_BT_FTM),true)
LOCAL_SRC_FILES += \
    ftm_bt.c \
    ftm_bt_power_pfal_linux.c \
    ftm_bt_hci_pfal_linux.c \
    ftm_bt_persist.cpp \
    diag_pkt_handler.c  \
    Socket.c
endif

ifeq ($(call is-platform-sdk-version-at-least,23),true)
LOCAL_CFLAGS += -DANDROID_M
endif

#ifeq ($(BOARD_HAVE_QCOM_FM),true)
#LOCAL_SRC_FILES += ftm_fm.c ftm_fm_pfal_linux_3990.c
#endif

#ifneq ($(BOARD_ANT_WIRELESS_DEVICE), )
#LOCAL_SRC_FILES += ftm_ant.c
#endif


LOCAL_SHARED_LIBRARIES += libdl

ifneq ($(DISABLE_BT_FTM),true)
LOCAL_SHARED_LIBRARIES += libbt-hidlclient
endif

LOCAL_MODULE_PATH := $(TARGET_OUT_VENDOR_EXECUTABLES)
LOCAL_MODULE:= btftmdaemon
LOCAL_CLANG := true
ifeq ($(PRODUCT_VENDOR_MOVE_ENABLED),true)
LOCAL_PROPRIETARY_MODULE := true
endif
LOCAL_MODULE_TAGS := optional
LOCAL_SHARED_LIBRARIES  += libcutils liblog libhardware

ifneq ($(DISABLE_BT_FTM),true)
ifneq ($(TARGET_SUPPORTS_ANDROID_WEAR),true)
LOCAL_SHARED_LIBRARIES  += libbtnv
else
ifeq ($(call is-platform-sdk-version-at-least,29),true)
LOCAL_SHARED_LIBRARIES  += libbtnv
else
LOCAL_SHARED_LIBRARIES  += libbtnv-wear
endif
endif
endif

# By default NV persist gets used
LOCAL_CFLAGS += -DBT_NV_SUPPORT

LDFLAGS += -ldl

include $(BUILD_EXECUTABLE)
include $(call all-makefiles-under,$(LOCAL_PATH))

endif
endif
