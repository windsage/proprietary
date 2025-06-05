LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

ifeq ($(call is-board-platform-in-list, kona lahaina taro sdxlemur parrot kalama pineapple),true)


LOCAL_SRC_FILES:= \
   ccid_uim_daemon.cpp \
   ccid_uim_parsing.cpp

LOCAL_SHARED_LIBRARIES := libqmi_cci libqmi_encdec libqmiservices
LOCAL_HEADER_LIBRARIES += libqcrilNr-headers
LOCAL_HEADER_LIBRARIES     += libqcrilNrLogger-headers

ifneq ($(qcril_sanitize_diag),)
LOCAL_SANITIZE_DIAG := $(qcril_sanitize_diag)
endif

ifeq ($(QCRIL_BUILD_WITH_ASAN),true)
LOCAL_SANITIZE             += $(qcril_sanitize)
LOCAL_NOSANITIZE           := cfi
endif

LOCAL_MODULE:= ccid_daemon_nr
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_OWNER := qti
LOCAL_CFLAGS := $(qcril_cflags)
LOCAL_CPPFLAGS:= $(qcril_cppflags)
LOCAL_PROPRIETARY_MODULE := true

include $(BUILD_EXECUTABLE)

endif

