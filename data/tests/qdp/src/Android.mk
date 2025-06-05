LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_SRC_FILES := qdp_test.c

#LOCAL_CFLAGS := -DFEATURE_DSI_TEST

LOCAL_SHARED_LIBRARIES += libdiag
LOCAL_SHARED_LIBRARIES += libqdp
LOCAL_SHARED_LIBRARIES += libqmiservices


LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../../qdp/inc
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../../qdp/src
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../../dsutils/inc
ifeq ($(call is-board-platform,msm7630_fusion),true)
LOCAL_CFLAGS += -DFEATURE_QDP_FUSION
endif

LOCAL_HEADER_LIBRARIES := libqmi_common_headers \
                          libqmi_cci_headers \
                          libdiag_headers \
                          libqmi_headers \
                          libdataqmiservices_headers \
                          libqmi_legacy_headers \
                          vendor_common_inc

LOCAL_MODULE := qdp_test
LOCAL_SANITIZE:=integer_overflow
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_PATH := $(TARGET_OUT_DATA)/data_test
LOCAL_MODULE_OWNER := qti
LOCAL_PROPRIETARY_MODULE := true

include $(BUILD_EXECUTABLE)

include $(CLEAR_VARS)




LOCAL_SRC_FILES := qdp_test_02.c

#LOCAL_CFLAGS := -DFEATURE_DSI_TEST

LOCAL_SHARED_LIBRARIES += libdiag
LOCAL_SHARED_LIBRARIES += libqdp
LOCAL_HEADER_LIBRARIES :=   libqmi_common_headers \
                          libqmi_cci_headers \
                          libdiag_headers \
                          libqmi_headers \
                          libdataqmiservices_headers


LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../../qdp/inc
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../../qdp/src
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../../dsutils/inc
ifeq ($(call is-board-platform,msm7630_fusion),true)
LOCAL_CFLAGS += -DFEATURE_QDP_FUSION
endif

LOCAL_HEADER_LIBRARIES := libqmi_common_headers \
                          libqmi_cci_headers \
                          libdiag_headers \
                          libqmi_legacy_headers \
                          vendor_common_inc

LOCAL_MODULE := qdp_test_02
LOCAL_SANITIZE:=integer_overflow
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_PATH := $(TARGET_OUT_DATA)/data_test
LOCAL_MODULE_OWNER := qti
LOCAL_PROPRIETARY_MODULE := true

include $(BUILD_EXECUTABLE)

include $(CLEAR_VARS)


LOCAL_SRC_FILES := qdp_test_03.c

#LOCAL_CFLAGS := -DFEATURE_DSI_TEST

LOCAL_SHARED_LIBRARIES += libdiag
LOCAL_SHARED_LIBRARIES += libqdp


LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../../qdp/inc
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../../qdp/src
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../../dsutils/inc
ifeq ($(call is-board-platform,msm7630_fusion),true)
LOCAL_CFLAGS += -DFEATURE_QDP_FUSION
endif


LOCAL_HEADER_LIBRARIES := libqmi_common_headers \
                          libqmi_cci_headers \
                          libdiag_headers \
                          libqmi_headers \
                          libdataqmiservices_headers \
                          libqmi_legacy_headers \
                          vendor_common_inc

LOCAL_MODULE := qdp_test_03
LOCAL_SANITIZE:=integer_overflow
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_PATH := $(TARGET_OUT_DATA)/data_test
LOCAL_MODULE_OWNER := qti
LOCAL_PROPRIETARY_MODULE := true

include $(BUILD_EXECUTABLE)

include $(CLEAR_VARS)


LOCAL_SRC_FILES := qdp_test_04.c

#LOCAL_CFLAGS := -DFEATURE_DSI_TEST

LOCAL_SHARED_LIBRARIES += libdiag
LOCAL_SHARED_LIBRARIES += libqdp


LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../../qdp/inc
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../../qdp/src
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../../dsutils/inc
ifeq ($(call is-board-platform,msm7630_fusion),true)
LOCAL_CFLAGS += -DFEATURE_QDP_FUSION
endif

LOCAL_HEADER_LIBRARIES := libqmi_common_headers \
                          libqmi_cci_headers \
                          libdiag_headers \
                          libqmi_headers \
                          libdataqmiservices_headers \
                          libqmi_legacy_headers \
                          vendor_common_inc


LOCAL_MODULE := qdp_test_04
LOCAL_SANITIZE:=integer_overflow
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_PATH := $(TARGET_OUT_DATA)/data_test
LOCAL_MODULE_OWNER := qti
LOCAL_PROPRIETARY_MODULE := true

include $(BUILD_EXECUTABLE)

include $(CLEAR_VARS)


LOCAL_SRC_FILES := qdp_test_05.c

#LOCAL_CFLAGS := -DFEATURE_DSI_TEST

LOCAL_SHARED_LIBRARIES += libdiag
LOCAL_SHARED_LIBRARIES += libqdp


LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../../qdp/inc
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../../qdp/src
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../../dsutils/inc
ifeq ($(call is-board-platform,msm7630_fusion),true)
LOCAL_CFLAGS += -DFEATURE_QDP_FUSION
endif


LOCAL_HEADER_LIBRARIES := libqmi_common_headers \
                          libqmi_cci_headers \
                          libdiag_headers \
                          libqmi_legacy_headers \
                          libqmi_headers \
                          libdataqmiservices_headers \
                          vendor_common_inc

LOCAL_MODULE := qdp_test_05
LOCAL_SANITIZE:=integer_overflow
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_PATH := $(TARGET_OUT_DATA)/data_test
LOCAL_MODULE_OWNER := qti
LOCAL_PROPRIETARY_MODULE := true

include $(BUILD_EXECUTABLE)

include $(CLEAR_VARS)


LOCAL_SRC_FILES := qdp_test_06.c

#LOCAL_CFLAGS := -DFEATURE_DSI_TEST

LOCAL_SHARED_LIBRARIES += libdiag
LOCAL_SHARED_LIBRARIES += libqdp


LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../../qdp/inc
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../../qdp/src
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../../dsutils/inc


LOCAL_HEADER_LIBRARIES := libqmi_common_headers \
                          libqmi_cci_headers \
                          libdiag_headers \
                          libqmi_headers \
                          libdataqmiservices_headers \
                          libqmi_legacy_headers \
                          vendor_common_inc

LOCAL_MODULE := qdp_test_06
LOCAL_SANITIZE:=integer_overflow
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_PATH := $(TARGET_OUT_DATA)/data_test
ifeq ($(call is-board-platform,msm7630_fusion),true)
LOCAL_CFLAGS += -DFEATURE_QDP_FUSION
endif
LOCAL_MODULE_OWNER := qti
LOCAL_PROPRIETARY_MODULE := true

include $(BUILD_EXECUTABLE)

include $(CLEAR_VARS)


LOCAL_SRC_FILES := qdp_test_07.c

#LOCAL_CFLAGS := -DFEATURE_DSI_TEST

LOCAL_SHARED_LIBRARIES += libdiag
LOCAL_SHARED_LIBRARIES += libqdp

LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../../qdp/inc
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../../dsutils/inc
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../../qdp/src
ifeq ($(call is-board-platform,msm7630_fusion),true)
LOCAL_CFLAGS += -DFEATURE_QDP_FUSION
endif


LOCAL_HEADER_LIBRARIES := libqmi_common_headers \
                          libqmi_cci_headers \
                          libdiag_headers \
                          libqmi_headers \
                          libdataqmiservices_headers \
                          libqmi_legacy_headers \
                          vendor_common_inc

LOCAL_MODULE := qdp_test_07
LOCAL_SANITIZE:=integer_overflow
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_PATH := $(TARGET_OUT_DATA)/data_test
LOCAL_MODULE_OWNER := qti
LOCAL_PROPRIETARY_MODULE := true

# TODO: replace qmi apis with qdp_qmi apis
#include $(BUILD_EXECUTABLE)

include $(CLEAR_VARS)


LOCAL_SRC_FILES := qdp_test_08.c

#LOCAL_CFLAGS := -DFEATURE_DSI_TEST

LOCAL_SHARED_LIBRARIES += libdiag
LOCAL_SHARED_LIBRARIES += libqdp

LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../../qdp/inc
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../../dsutils/inc
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../../qdp/src
ifeq ($(call is-board-platform,msm7630_fusion),true)
LOCAL_CFLAGS += -DFEATURE_QDP_FUSION
endif


LOCAL_HEADER_LIBRARIES := libqmi_common_headers \
                          libqmi_cci_headers \
                          libdiag_headers \
                          libqmi_headers \
                          libdataqmiservices_headers \
                          libqmi_legacy_headers \
                          vendor_common_inc

LOCAL_MODULE := qdp_test_08
LOCAL_SANITIZE:=integer_overflow
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_PATH := $(TARGET_OUT_DATA)/data_test
LOCAL_MODULE_OWNER := qti
LOCAL_PROPRIETARY_MODULE := true

# TODO: replace qmi apis with qdp_qmi apis
#include $(BUILD_EXECUTABLE)

include $(CLEAR_VARS)


LOCAL_SRC_FILES := qdp_test_09.c

#LOCAL_CFLAGS := -DFEATURE_DSI_TEST

LOCAL_SHARED_LIBRARIES += libdiag
LOCAL_SHARED_LIBRARIES += libqdp


LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../../qdp/inc
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../../qdp/src
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../../dsutils/inc



LOCAL_HEADER_LIBRARIES := libqmi_common_headers \
                          libqmi_cci_headers \
                          libdiag_headers \
                          libqmi_headers \
                          libdataqmiservices_headers \
                          libqmi_legacy_headers \
                          vendor_common_inc


LOCAL_MODULE := qdp_test_09
LOCAL_SANITIZE:=integer_overflow
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_PATH := $(TARGET_OUT_DATA)/data_test
ifeq ($(call is-board-platform,msm7630_fusion),true)
LOCAL_CFLAGS += -DFEATURE_QDP_FUSION
endif
LOCAL_MODULE_OWNER := qti
LOCAL_PROPRIETARY_MODULE := true

include $(BUILD_EXECUTABLE)

include $(CLEAR_VARS)


LOCAL_SRC_FILES := qdp_test_10.c

#LOCAL_CFLAGS := -DFEATURE_DSI_TEST

LOCAL_SHARED_LIBRARIES += libdiag
LOCAL_SHARED_LIBRARIES += libqdp

LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../../qdp/inc
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../../dsutils/inc
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../../qdp/src
ifeq ($(call is-board-platform,msm7630_fusion),true)
LOCAL_CFLAGS += -DFEATURE_QDP_FUSION
endif


LOCAL_HEADER_LIBRARIES := libqmi_common_headers \
                          libqmi_cci_headers \
                          libdiag_headers \
                          libqmi_headers \
                          libdataqmiservices_headers \
                          libqmi_legacy_headers \
                          vendor_common_inc

LOCAL_MODULE := qdp_test_10
LOCAL_SANITIZE:=integer_overflow
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_PATH := $(TARGET_OUT_DATA)/data_test
LOCAL_PROPRIETARY_MODULE := true

# TODO: replace qmi apis with qdp_qmi apis
#include $(BUILD_EXECUTABLE)

include $(CLEAR_VARS)

LOCAL_SRC_FILES := qdp_test_create_profile.c

#LOCAL_CFLAGS := -DFEATURE_DSI_TEST

LOCAL_SHARED_LIBRARIES += libdiag
LOCAL_SHARED_LIBRARIES += libqdp

LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../../qdp/inc
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../../dsutils/inc
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../../qdp/src
ifeq ($(call is-board-platform,msm7630_fusion),true)
LOCAL_CFLAGS += -DFEATURE_QDP_FUSION
endif

LOCAL_HEADER_LIBRARIES := libqmi_common_headers \
                          libqmi_cci_headers \
                          libdiag_headers \
                          libqmi_headers \
                          libdataqmiservices_headers \
                          libqmi_legacy_headers \
                          vendor_common_inc


LOCAL_MODULE := qdp_test_create_profile
LOCAL_SANITIZE:=integer_overflow
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_PATH := $(TARGET_OUT_DATA)/data_test
LOCAL_MODULE_OWNER := qti
LOCAL_PROPRIETARY_MODULE := true

# TODO: replace qmi apis with qdp_qmi apis
#include $(BUILD_EXECUTABLE)
