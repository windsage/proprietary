# LOCAL_PATH and the include path need to be hard-coded because wmsts is inside
# the qcril directory (bug in the Android makefile system).
# LOCAL_PATH reflects parent directory to ensure common objects are in
# separate pools for each RIL variant.


LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

##
## START build_qcril
##

LOCAL_CPP_EXTENSION := .cc

ifneq ($(qcril_sanitize_diag),)
LOCAL_SANITIZE_DIAG := $(qcril_sanitize_diag)
endif

ifeq ($(QCRIL_BUILD_WITH_ASAN),true)
LOCAL_SANITIZE := $(qcril_sanitize)
LOCAL_NOSANITIZE := cfi
endif
LOCAL_CFLAGS += $(qcril_cflags)
LOCAL_CPPFLAGS += -std=c++17 $(qcril_cppflags)
LOCAL_LDFLAGS += $(qcril_ldflags)

LOCAL_SRC_FILES += $(call all-c-files-under, .)

LOCAL_CFLAGS += -DPB_ENABLE_MALLOC # Needed by BT Sap
#LOCAL_SRC_FILES += $(call all-c-files-under,../nanopb)
#LOCAL_SRC_FILES += $(call all-cpp-files-under, .)
LOCAL_SRC_FILES += $(call all-named-files-under,*.cc,.)

LOCAL_HEADER_LIBRARIES += libqcrilNr-headers
LOCAL_EXPORT_HEADER_LIBRARY_HEADERS := libqcrilNr-headers
LOCAL_HEADER_LIBRARIES += libqcrilNrQtiMutex-headers
LOCAL_HEADER_LIBRARIES += libqcrilNrQtiBus-headers
LOCAL_HEADER_LIBRARIES += qtiwakelock-headers
LOCAL_HEADER_LIBRARIES += qtiril-utils-headers
LOCAL_HEADER_LIBRARIES += libril-db-headers
LOCAL_HEADER_LIBRARIES += libril-legacy-headers
LOCAL_HEADER_LIBRARIES += qtiPeripheralMgr-headers
LOCAL_HEADER_LIBRARIES += libril-qc-qmi-services-headers
LOCAL_HEADER_LIBRARIES += libdiag_headers

# for asprinf
LOCAL_CFLAGS += -D_GNU_SOURCE

LOCAL_WHOLE_STATIC_LIBRARIES += qcrilNrAndroidModule
LOCAL_WHOLE_STATIC_LIBRARIES += qcrilNrAndroidTranslators
#LOCAL_WHOLE_STATIC_LIBRARIES += qcrilNrVoiceModule
LOCAL_WHOLE_STATIC_LIBRARIES += qcrilNrNasModule
LOCAL_WHOLE_STATIC_LIBRARIES += qcrilNrNwRegistrationModule
LOCAL_WHOLE_STATIC_LIBRARIES += qcrilNrDmsModule
LOCAL_WHOLE_STATIC_LIBRARIES += qcrilNrUimModule
LOCAL_WHOLE_STATIC_LIBRARIES += qcrilNrGstkModule
#LOCAL_WHOLE_STATIC_LIBRARIES += qcrilNrImsModule
LOCAL_WHOLE_STATIC_LIBRARIES += qcrilNrAndroidImsRadio
LOCAL_WHOLE_STATIC_LIBRARIES += qcrilNrAndroidAudioModule
LOCAL_WHOLE_STATIC_LIBRARIES += qcrilNrOemHookModule
LOCAL_SHARED_LIBRARIES += qcrilInterfaces
LOCAL_WHOLE_STATIC_LIBRARIES += qcrilIntermodulemsgs
LOCAL_STATIC_LIBRARIES += libqcrilnr-protobuf-c-nano-enable_malloc
ifeq ($(TARGET_FWK_SUPPORTS_FULL_VALUEADDS), true)
LOCAL_WHOLE_STATIC_LIBRARIES += qcrilNrQtiRadio
endif

ifneq ($(TARGET_HAS_LOW_RAM), true)
LOCAL_WHOLE_STATIC_LIBRARIES += qcrilNrMbnModule
endif
ifeq ($(TARGET_SUPPORTS_DS),true)
LOCAL_WHOLE_STATIC_LIBRARIES += qcrilNrDeepSleepModule
LOCAL_SHARED_LIBRARIES       += vendor.qti.hardware.powerstateservice@1.0
endif

LOCAL_SHARED_LIBRARIES	   += libhidlbase
#LOCAL_SHARED_LIBRARIES += qcrild_libqcrilnr

LOCAL_SHARED_LIBRARIES += libqcrilNrFramework

LOCAL_SHARED_LIBRARIES += libcutils
LOCAL_SHARED_LIBRARIES += liblog
LOCAL_SHARED_LIBRARIES += libutils
LOCAL_SHARED_LIBRARIES += qcrild_libqcrilnrutils
ifeq ($(FEATURE_QCRIL_RADIO_CONFIG_SOCKET_ENABLED),true)
LOCAL_SHARED_LIBRARIES += libril-qc-radioconfig
LOCAL_CFLAGS += -DFEATURE_QCRIL_RADIO_CONFIG_SOCKET
endif

LOCAL_SHARED_LIBRARIES += libqmi_cci
LOCAL_SHARED_LIBRARIES += libqmi_client_qmux
LOCAL_SHARED_LIBRARIES += libdiag
LOCAL_SHARED_LIBRARIES += libhardware_legacy
LOCAL_SHARED_LIBRARIES += libqmiservices
LOCAL_SHARED_LIBRARIES += librilqmimiscservices
LOCAL_SHARED_LIBRARIES += libqmi_client_helper
LOCAL_SHARED_LIBRARIES += libidl
LOCAL_SHARED_LIBRARIES += libsqlite
LOCAL_SHARED_LIBRARIES += libbinder
LOCAL_SHARED_LIBRARIES += libmdmdetect
LOCAL_SHARED_LIBRARIES += libperipheral_client
LOCAL_SHARED_LIBRARIES += libqcrilNrLogger
LOCAL_SHARED_LIBRARIES += liblog
LOCAL_SHARED_LIBRARIES += libutils
LOCAL_SHARED_LIBRARIES += libcutils
LOCAL_SHARED_LIBRARIES += libhardware_legacy

LOCAL_SHARED_LIBRARIES += libhidlbase
LOCAL_SHARED_LIBRARIES += qcrild_libqcrilnrutils
LOCAL_SHARED_LIBRARIES += libqcrilNrQtiMutex
LOCAL_SHARED_LIBRARIES += libprotobuf-cpp-full
LOCAL_SHARED_LIBRARIES += libvndksupport
LOCAL_SHARED_LIBRARIES += libbase
LOCAL_SHARED_LIBRARIES += libz
LOCAL_SHARED_LIBRARIES += libconfigdb
LOCAL_SHARED_LIBRARIES += libxml
LOCAL_SHARED_LIBRARIES += libqcrilNrQtiBus
LOCAL_SHARED_LIBRARIES += qtiwakelock
LOCAL_SHARED_LIBRARIES += qtiril-utils
LOCAL_SHARED_LIBRARIES += qcrilMarshal
LOCAL_SHARED_LIBRARIES += libril-db
LOCAL_SHARED_LIBRARIES += libril-legacy
LOCAL_SHARED_LIBRARIES += qcrilNrQmiModule
LOCAL_SHARED_LIBRARIES += libQtiRilLoadable
LOCAL_SHARED_LIBRARIES += libbinder_ndk
LOCAL_SHARED_LIBRARIES += libbase
LOCAL_SHARED_LIBRARIES += qtiPeripheralMgr
LOCAL_SHARED_LIBRARIES += vendor.qti.hardware.radio.ims-V16-ndk
LOCAL_SHARED_LIBRARIES += vendor.qti.hardware.radio.qtiradio-V11-ndk
LOCAL_SHARED_LIBRARIES += vendor.qti.hardware.radio.am-V1-ndk
LOCAL_SHARED_LIBRARIES += vendor.qti.hardware.radio.qcrilhook-V1-ndk
LOCAL_SHARED_LIBRARIES += qcrilNr_aidl_common
LOCAL_SHARED_LIBRARIES += qcrilNr_aidl_IRadioConfig
LOCAL_SHARED_LIBRARIES += qcrilNr_aidl_IRadioMessaging
LOCAL_SHARED_LIBRARIES += qcrilNr_aidl_IRadioModem
LOCAL_SHARED_LIBRARIES += qcrilNr_aidl_IRadioNetwork
LOCAL_SHARED_LIBRARIES += qcrilNr_aidl_IRadioSim
LOCAL_SHARED_LIBRARIES += qcrilNr_aidl_IRadioVoice
LOCAL_SHARED_LIBRARIES += qcrilNr_aidl_IQtiRadioConfig
LOCAL_SHARED_LIBRARIES += qcrilNr_aidl_UimRemoteServer
LOCAL_SHARED_LIBRARIES += qcrilNr_aidl_UimRemoteClient
LOCAL_SHARED_LIBRARIES += qcrilNr_aidl_UimLpaService
LOCAL_SHARED_LIBRARIES += qcrilNr_aidl_UimService
LOCAL_SHARED_LIBRARIES += qcrilNr_aidl_IRadioSap
LOCAL_SHARED_LIBRARIES += qcrilNr_aidl_SecureElementService
LOCAL_SHARED_LIBRARIES += qcrilNrSecureMode

LOCAL_VINTF_FRAGMENTS := vendor.qti.hardware.radio.ims.xml
LOCAL_VINTF_FRAGMENTS += vendor.qti.hardware.radio.am.xml
LOCAL_VINTF_FRAGMENTS += vendor.qti.hardware.radio.qtiradioconfig.xml
LOCAL_VINTF_FRAGMENTS += vendor.qti.hardware.radio.uim_remote_server.xml
LOCAL_VINTF_FRAGMENTS += vendor.qti.hardware.radio.uim_remote_client.xml
LOCAL_VINTF_FRAGMENTS += vendor.qti.hardware.radio.uim.xml
LOCAL_VINTF_FRAGMENTS += android.hardware.radio.config.xml
LOCAL_VINTF_FRAGMENTS += vendor.qti.hardware.radio.lpa.xml
LOCAL_VINTF_FRAGMENTS += android.hardware.radio.messaging.xml
LOCAL_VINTF_FRAGMENTS += android.hardware.radio.modem.xml
LOCAL_VINTF_FRAGMENTS += android.hardware.radio.network.xml
LOCAL_VINTF_FRAGMENTS += android.hardware.radio.sim.xml
LOCAL_VINTF_FRAGMENTS += android.hardware.radio.voice.xml
LOCAL_VINTF_FRAGMENTS += android.hardware.radio.sap.xml
LOCAL_VINTF_FRAGMENTS += android.hardware.secure_element.xml


LOCAL_SHARED_LIBRARIES += qcrilNr_aidl_IQtiRadioConfig
LOCAL_VINTF_FRAGMENTS += vendor.qti.hardware.radio.qtiradioconfig.xml

ifneq ($(SYS_HEALTH_MON_STATUS), false)
LOCAL_SHARED_LIBRARIES += libsystem_health_mon
endif

# Assume by default that libxml2 is available
# This prevents touching existing device
# config files that already support #
# libxml2. Device files not supporting it,
# would have to explicitly define it, but it should be clear when it is
# required (compilation will fail)
ifneq (${LIBXML_SUPPORTED},false)
LOCAL_SHARED_LIBRARIES += libxml2
else
LOCAL_CFLAGS += -DLIBXML_UNSUPPORTED
endif

$(info is-board-platform-in-list=$is-board-platform-in-list)
# For API Definitions and enables
LOCAL_CFLAGS   += $(remote_api_defines)
LOCAL_CFLAGS   += $(remote_api_enables)

# defines necessary for QCRIL code
LOCAL_CFLAGS += -DRIL_SHLIB
LOCAL_CFLAGS += -DFEATURE_MMGSDI_GSM
LOCAL_CFLAGS += -DFEATURE_AUTH
LOCAL_CFLAGS += -DPACKED=

ifdef FEATURE_QCRIL_TOOLKIT_SKIP_SETUP_EVT_LIST_FILTER
LOCAL_CFLAGS += -DFEATURE_QCRIL_TOOLKIT_SKIP_SETUP_EVT_LIST_FILTER
endif

ifneq ($(SYS_HEALTH_MON_STATUS), false)
LOCAL_CFLAGS += -DFEATURE_QCRIL_SHM
endif
ifneq ($(TARGET_HAS_LOW_RAM), true)
LOCAL_CFLAGS += -DFEATURE_QCRIL_OEMHOOK_IMS_PRESENCE
LOCAL_CFLAGS += -DFEATURE_QCRIL_OEMHOOK_IMS_VT
LOCAL_CFLAGS += -DFEATURE_QCRIL_MBN
else
LOCAL_CFLAGS += -DRIL_FOR_LOW_RAM
LOCAL_CPPFLAGS += -DRIL_FOR_LOW_RAM
LOCAL_CXXFLAGS += -DRIL_FOR_LOW_RAM
endif
ifneq ($(TARGET_FWK_SUPPORTS_FULL_VALUEADDS), true)
LOCAL_CFLAGS += -DDISABLE_POWERON_OPT
LOCAL_CPPFLAGS += -DDISABLE_POWERON_OPT
LOCAL_CXXFLAGS += -DDISABLE_POWERON_OPT
endif

LOCAL_CFLAGS += -DFEATURE_QCRIL_HDR_RELB
LOCAL_CFLAGS += -DFEATURE_QCRIL_NCELL

# defines common to all shared libraries
LOCAL_CFLAGS += \
  -DLOG_NDDEBUG=0 \
  -DIMAGE_APPS_PROC \
  -DFEATURE_Q_SINGLE_LINK \
  -DFEATURE_Q_NO_SELF_QPTR \
  -DFEATURE_NATIVELINUX \
  -DFEATURE_DSM_DUP_ITEMS \

# compiler options
LOCAL_CFLAGS += -g
LOCAL_CFLAGS += -fno-inline
LOCAL_CFLAGS += -fno-short-enums

LOCAL_PRELINK_MODULE := false

LOCAL_MODULE_OWNER := qcom
LOCAL_PROPRIETARY_MODULE := true

LOCAL_REQUIRED_MODULES     += qcrilNrDb_vendor

LOCAL_HEADER_LIBRARIES += qcrilDataInternal_headers
LOCAL_HEADER_LIBRARIES += qcrilDataInterfaces_headers
LOCAL_WHOLE_STATIC_LIBRARIES += libqcrilNRDataInternal
LOCAL_WHOLE_STATIC_LIBRARIES += libqcrilNRDataInterfaces

LOCAL_CLANG := true
ifndef QCRIL_DSDA_INSTANCE
   LOCAL_MODULE:= libqcrilNr
else
   LOCAL_CFLAGS += -DFEATURE_DSDA_RIL_INSTANCE=$(QCRIL_DSDA_INSTANCE)
   LOCAL_MODULE:= libqcrilnr-$(QCRIL_DSDA_INSTANCE)
endif
#LOCAL_EXPORT_CFLAGS += $(LOCAL_CFLAGS)
include $(BUILD_SHARED_LIBRARY)
##
## END build_qcril
##
