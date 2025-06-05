LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

LOCAL_MODULE := libspukeymintutils

LOCAL_SRC_FILES := \
    authorization_set.cpp \
    serializable.cpp \
    attestation_record.cpp \
    openssl_utils.cpp \
    openssl_err.cpp \
    cppbor.cpp \
    cppbor_parse.cpp \
    cppcose.cpp \
    remote_provisioning_utils.cpp \
    android_keymaster_messages.cpp \
    pure_soft_remote_provisioning_context.cpp

QC_PROP_ROOT ?= vendor/qcom/proprietary

LOCAL_C_INCLUDES := \
                    $(TOP)/$(QC_PROP_ROOT)/securemsm/QSEEComAPI \
                    $(TOP)/external/boringssl/include \
                    $(TOP)/$(QC_PROP_ROOT)/spu/android_security/keymint_utils \
                    $(TOP)/$(QC_PROP_ROOT)/spu/sputils/inc \
                    $(TOP)/$(QC_PROP_ROOT)/spu/spcomlib/inc/spcomlib.h \
                    $(TOP)/vendor/qcom/proprietary/spu/keymaster_install_toolbox

LOCAL_HEADER_LIBRARIES := jni_headers libhardware_headers

LOCAL_CFLAGS := -Wall -Werror -Wunused
LOCAL_SHARED_LIBRARIES := \
    libcutils \
    libQSEEComAPI \
    liblog \
    libcrypto \
    libspcom

LOCAL_CLANG := true
LOCAL_CLANG_CFLAGS += -Wno-error=unused-const-variable -Wno-error=unused-private-field
LOCAL_CLANG_CFLAGS += -fno-sanitize-coverage=edge,indirect-calls,8bit-counters,trace-cmp
LOCAL_ADDITIONAL_DEPENDENCIES := $(LOCAL_PATH)/Android.mk
LOCAL_EXPORT_C_INCLUDE_DIRS := $(LOCAL_PATH)/include
LOCAL_MODULE_TAGS := optional

LOCAL_PROPRIETARY_MODULE := true

#ifeq ($(ENABLE_SPU_KEYMINT),true)
#LOCAL_VINTF_FRAGMENTS := android.hardware.security.keymint-service-spu-qti.xml
#LOCAL_VINTF_FRAGMENTS := android.hardware.security.rkp-service-spu-qti.xml
#endif

include $(BUILD_SHARED_LIBRARY)

ifeq ($(ENABLE_SPU_AUTHSECRET),true)
include $(CLEAR_VARS)
LOCAL_MODULE       := android.hardware.authsecret.qti.spu-service.rc
LOCAL_MODULE_TAGS  := optional
LOCAL_MODULE_CLASS := ETC
LOCAL_SRC_FILES    := $(LOCAL_MODULE)
LOCAL_MODULE_PATH  := $(TARGET_OUT_VENDOR_ETC)/init
include $(BUILD_PREBUILT)
endif #ENABLE_SPU_AUTHSECRET

ifeq ($(ENABLE_SPU_KEYMINT),true)
include $(CLEAR_VARS)
LOCAL_MODULE       := android.hardware.security.keymint-service-spu-qti.rc
LOCAL_MODULE_TAGS  := optional
LOCAL_MODULE_CLASS := ETC
LOCAL_SRC_FILES    := $(LOCAL_MODULE)
LOCAL_MODULE_PATH  := $(TARGET_OUT_VENDOR_ETC)/init
include $(BUILD_PREBUILT)
endif #ENABLE_SPU_KEYMINT

ifeq ($(ENABLE_SPU_WEAVER),true)

include $(CLEAR_VARS)
LOCAL_MODULE       := android.hardware.weaver-service-spu-qti.rc
LOCAL_MODULE_TAGS  := optional
LOCAL_MODULE_CLASS := ETC
LOCAL_SRC_FILES    := $(LOCAL_MODULE)
LOCAL_MODULE_PATH  := $(TARGET_OUT_VENDOR_ETC)/init
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE       := android.hardware.weaver-service-spu-qti.xml
LOCAL_MODULE_TAGS  := optional
LOCAL_MODULE_CLASS := ETC
LOCAL_SRC_FILES    := $(LOCAL_MODULE)
LOCAL_MODULE_PATH  := $(TARGET_OUT_VENDOR_ETC)/vintf/manifest
include $(BUILD_PREBUILT)

endif #ENABLE_SPU_WEAVER

