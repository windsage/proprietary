LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)
LOCAL_MODULE := cnss-daemon
ifeq ($(PRODUCT_VENDOR_MOVE_ENABLED),true)
LOCAL_PROPRIETARY_MODULE := true
endif
LOCAL_MODULE_TAGS := optional

LOCAL_CLANG := true
LOCAL_SANITIZE := integer_overflow

LOCAL_HEADER_LIBRARIES := libcld80211_headers

LOCAL_HEADER_LIBRARIES += libril-qc-qmi-services-headers

LOCAL_HEADER_LIBRARIES += libqmi_common_headers
LOCAL_HEADER_LIBRARIES += libmdmdetect_headers
LOCAL_HEADER_LIBRARIES += libperipheralclient_headers


ifneq ($(strip $(TARGET_USES_NO_SUBNET_DETECTION)),true)
LOCAL_CFLAGS += -DENABLE_SUBNET_DETECTION
endif

ifneq ($(strip $(TARGET_USES_NO_CNSS_DP)),true)
LOCAL_CFLAGS += -DCONFIG_CNSS_DP
endif

ifeq ($(strip $(CONFIG_MAC_PRIVACY_LOGGING)),true)
LOCAL_CFLAGS += -DCONFIG_MAC_PRIVACY_LOGGING
endif

LOCAL_LD_FLAGS := -Wl,--hash-style=sysv

LOCAL_SRC_FILES := \
	main.c \
	nl_loop.c \
	debug.c \
	cnss_qmi_client.c \
	wireless_lan_proxy_service_v01.c \
	cnss_genl.c

ifneq ($(strip $(TARGET_NO_USE_CNSS_CLI)),true)
LOCAL_SRC_FILES += cnss_user.c
endif

ifneq ($(strip $(TARGET_USES_NO_SUBNET_DETECTION)),true)
LOCAL_SRC_FILES += cnss_gw_update.c
endif

ifneq ($(strip $(TARGET_USES_NO_CNSS_DP)),true)
LOCAL_SRC_FILES += cnss_dp.c
endif

ifneq ($(strip $(TARGET_USES_NO_NETLINK_COMMON)),true)
LOCAL_CFLAGS += -DENABLE_NETLINK_COMMON
LOCAL_SRC_FILES += cnss_netlink_common.c

ifneq ($(strip $(TARGET_USES_NO_INTEROP_ISSUES_AP_DETECTION)),true)
LOCAL_CFLAGS += -DENABLE_INTEROP_ISSUES_AP_DETECTION
LOCAL_SRC_FILES += cnss_interop_issues_ap.c
endif
endif

LOCAL_SHARED_LIBRARIES := \
	libcutils \
	liblog \
	libqmi_cci \
	libqmi_common_so \
	libnl \
	libcld80211

LOCAL_SHARED_LIBRARIES += \
	libperipheral_client \
	libmdmdetect \

LOCAL_CFLAGS += \
	-DCONFIG_DEBUG \
	-DCONFIG_DEBUG_LOGCAT \
	-DCONFIG_CNSS_GW_UPDATE \
	-DCONFIG_CNSS_LPASS_SCAN \
	-DCONFIG_WLAN_MSG_SVC \
	-Werror

ifneq ($(strip $(TARGET_USES_NO_DMS_QMI_CLIENT)),true)
LOCAL_CFLAGS += -DCONFIG_READ_NV_MAC_ADDR
endif

ifeq ($(strip $(TARGET_USES_WIGIG_NPT_IN_CNSS_DAEMON)),true)
LOCAL_CFLAGS += -DCONFIG_WIGIG_NPT_SERV
endif

ifneq ($(strip $(TARGET_NO_USE_CNSS_CLI)),true)
LOCAL_CFLAGS += -DCONFIG_CNSS_USER
endif

ifeq ($(strip $(TARGET_CAL_DATA_CLEAR)),true)
LOCAL_CFLAGS += -DCAL_DATA_REMOVE
endif

LOCAL_SRC_FILES += cnss_plat.c
ifeq ($(strip $(TARGET_USES_NO_FW_QMI_CLIENT)),true)
LOCAL_CFLAGS += -DCNSS_PLAT_IPC_QMI
LOCAL_SRC_FILES += cnss_plat_ipc_qmi.c \
		   cnss_plat_ipc_service_v01.c
else
LOCAL_CFLAGS += -DICNSS_QMI
LOCAL_SRC_FILES += wlfw_qmi_client.c \
		wlan_firmware_service_v01.c \
		device_management_service_v01.c
endif

include $(BUILD_EXECUTABLE)

##################################################

ifneq ($(strip $(TARGET_NO_USE_CNSS_CLI)),true)
include $(CLEAR_VARS)
LOCAL_MODULE := cnss_cli
LOCAL_CFLAGS := -Werror
LOCAL_PROPRIETARY_MODULE := true
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := cnss_cli.c
include $(BUILD_EXECUTABLE)
endif
