LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_SRC_FILES += dsi_netctrl.c
LOCAL_SRC_FILES += dsi_netctrl_init.c
LOCAL_SRC_FILES += dsi_netctrli.c
LOCAL_SRC_FILES += dsi_netctrl_mni_cb.c
LOCAL_SRC_FILES += dsi_netctrl_mni.c
LOCAL_SRC_FILES += dsi_netctrl_multimodem.c
LOCAL_SRC_FILES += dsi_netctrl_platform.c
LOCAL_SRC_FILES += dsi_netctrl_cb_thrd.c
LOCAL_SRC_FILES += dsi_netctrl_qos.c
LOCAL_SRC_FILES += dsi_netctrl_embms.c
LOCAL_SRC_FILES += dsi_netctrl_qmi.c
LOCAL_SRC_FILES += dsi_netctrl_qmi_wds.c
LOCAL_SRC_FILES += dsi_netctrl_qmi_qos.c
LOCAL_SRC_FILES += dsi_config.c
LOCAL_SRC_FILES += dsi_netctrl_nicm_helper.c

#LOCAL_CFLAGS := -DFEATURE_DSI_TEST

LOCAL_CFLAGS := -DFEATURE_DS_LINUX_ANDROID

ifeq ($(BOARD_USES_QCNE),true)
LOCAL_CFLAGS += -DFEATURE_DATA_IWLAN
endif

LOCAL_SHARED_LIBRARIES := libqdi
LOCAL_SHARED_LIBRARIES += libdiag
LOCAL_SHARED_LIBRARIES += libcutils
LOCAL_SHARED_LIBRARIES += liblog
LOCAL_SHARED_LIBRARIES += libdiag
LOCAL_SHARED_LIBRARIES += libdsutils
LOCAL_SHARED_LIBRARIES += libconfigdb
LOCAL_SHARED_LIBRARIES += libqmiservices
LOCAL_SHARED_LIBRARIES += libqmi_cci
LOCAL_SHARED_LIBRARIES += libqmi_common_so
LOCAL_SHARED_LIBRARIES += libqmi_client_qmux
LOCAL_SHARED_LIBRARIES += libqmi_client_helper
LOCAL_SHARED_LIBRARIES += libqmi

LOCAL_SHARED_LIBRARIES += libnicm
LOCAL_SHARED_LIBRARIES += libnicm_dsi

LOCAL_C_INCLUDES := $(LOCAL_PATH)/../../dsutils/inc
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../configdb/inc
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../qdi/inc

ifeq ($(call is-board-platform,msm7630_fusion),true)
LOCAL_CFLAGS += -DFEATURE_DSI_FUSION
endif
LOCAL_CFLAGS += -DFEATURE_QCRIL_USE_QDP
LOCAL_CFLAGS += -DFEATURE_DSI_MULTIMODEM_ROUTELOOKUP
LOCAL_CFLAGS += -DFEATURE_DATA_LOG_QXDM
LOCAL_CFLAGS += -DFEATURE_DATA_LOG_ADB

LOCAL_HEADER_LIBRARIES := libdsi_netctrl_headers \
                          libqmi_common_headers \
                          libqmi_cci_headers \
                          libdiag_headers \
                          libqmi_headers \
                          libdataqmiservices_headers \
                          vendor_common_inc \
                          libutils_headers \
                          libdsi_netctrl_internal_headers

LOCAL_HEADER_LIBRARIES += libnicm_headers

LOCAL_EXPORT_HEADER_LIBRARY_HEADERS := libdsi_netctrl_headers \
                                       libdsi_netctrl_internal_headers

LOCAL_MODULE := libdsi_netctrl
LOCAL_SANITIZE:=integer_overflow

LOCAL_MODULE_TAGS := optional

LOCAL_CLANG := true

LOCAL_MODULE_OWNER := qti
LOCAL_PROPRIETARY_MODULE := true


include $(BUILD_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := dsi_config.xml
LOCAL_SANITIZE:=integer_overflow
LOCAL_MODULE_CLASS := ETC
LOCAL_MODULE_PATH := $(TARGET_OUT_VENDOR_ETC)/data
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := $(LOCAL_MODULE)
LOCAL_MODULE_OWNER := qti
LOCAL_HEADER_LIBRARIES += libutils_headers
include $(BUILD_PREBUILT)

