LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

LOCAL_MODULE := libkeymasterutils

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
    pure_soft_remote_provisioning_context.cpp \
    spu_keymaster_utils.cpp

QC_PROP_ROOT ?= vendor/qcom/proprietary

LOCAL_C_INCLUDES := \
                    $(TOP)/$(QC_PROP_ROOT)/securemsm/QSEEComAPI \
                    $(TOP)/external/boringssl/include \
                    $(TOP)/$(QC_PROP_ROOT)/securemsm/keymaster_install_toolbox

LOCAL_HEADER_LIBRARIES := jni_headers libhardware_headers

#LOCAL_CFLAGS := $(keymaster-def)
LOCAL_CFLAGS := -Wall -Werror -Wunused
LOCAL_SHARED_LIBRARIES := \
    libcutils \
    libQSEEComAPI \
    liblog \
    libcrypto

ifeq ($(ENABLE_SPU_GK), true)
LOCAL_CFLAGS += -DENABLE_SPU_GK
endif #ENABLE_SPU_GK

LOCAL_CLANG := true
LOCAL_CLANG_CFLAGS += -Wno-error=unused-const-variable -Wno-error=unused-private-field
LOCAL_CLANG_CFLAGS += -fno-sanitize-coverage=edge,indirect-calls,8bit-counters,trace-cmp
LOCAL_ADDITIONAL_DEPENDENCIES := $(LOCAL_PATH)/Android.mk
LOCAL_EXPORT_C_INCLUDE_DIRS := $(LOCAL_PATH)/include
LOCAL_MODULE_TAGS := optional

LOCAL_PROPRIETARY_MODULE := true

ifeq ($(ENABLE_KM_SPLIT_MANIFEST), true)
    ifeq ($(ENABLE_STRONGBOX_KM), true)
        LOCAL_VINTF_FRAGMENTS := android.hardware.keymaster@4.1-service-default-and-strongbox-qti.xml
    else
        LOCAL_VINTF_FRAGMENTS := android.hardware.keymaster@4.1-service-default-qti.xml
    endif
endif

ifeq ($(ENABLE_KEYMINT), true)
LOCAL_VINTF_FRAGMENTS := android.hardware.security.keymint-service-qti.xml
endif

include $(BUILD_SHARED_LIBRARY)

ifeq ($(ENABLE_KM_4_0),true)
include $(CLEAR_VARS)
LOCAL_MODULE       := android.hardware.keymaster@4.0-service-qti.rc
LOCAL_MODULE_TAGS  := optional
LOCAL_MODULE_CLASS := ETC
LOCAL_SRC_FILES    := $(LOCAL_MODULE)
LOCAL_MODULE_PATH  := $(TARGET_OUT_VENDOR_ETC)/init
include $(BUILD_PREBUILT)
endif #ENABLE_KM_4_0

ifeq ($(ENABLE_KM_4_1),true)
include $(CLEAR_VARS)
LOCAL_MODULE       := android.hardware.keymaster@4.1-service-qti.rc
LOCAL_MODULE_TAGS  := optional
LOCAL_MODULE_CLASS := ETC
LOCAL_SRC_FILES    := $(LOCAL_MODULE)
LOCAL_MODULE_PATH  := $(TARGET_OUT_VENDOR_ETC)/init
include $(BUILD_PREBUILT)
endif #ENABLE_KM_4_1

ifeq ($(ENABLE_STRONGBOX_KM),true)
include $(CLEAR_VARS)
LOCAL_MODULE       := android.hardware.keymaster@4.0-strongbox-service-qti.rc
LOCAL_MODULE_TAGS  := optional
LOCAL_MODULE_CLASS := ETC
LOCAL_SRC_FILES    := $(LOCAL_MODULE)
LOCAL_MODULE_PATH  := $(TARGET_OUT_VENDOR_ETC)/init
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE       := init.qti.keymaster.sh
LOCAL_MODULE_TAGS  := optional
LOCAL_MODULE_CLASS := ETC
LOCAL_SRC_FILES    := $(LOCAL_MODULE)
LOCAL_MODULE_PATH  := $(TARGET_OUT_VENDOR_EXECUTABLES)
include $(BUILD_PREBUILT)

endif #ENABLE_STRONGBOX_KM

ifneq ($(ENABLE_KEYMINT),true)
ifneq ($(ENABLE_KM_4_1),true)
ifneq ($(ENABLE_KM_4_0),true)
ifeq ($(KMGK_USE_QTI_SERVICE),true)
include $(CLEAR_VARS)
LOCAL_MODULE       := android.hardware.keymaster@3.0-service-qti.rc
LOCAL_MODULE_TAGS  := optional
LOCAL_MODULE_CLASS := ETC
LOCAL_SRC_FILES    := $(LOCAL_MODULE)
LOCAL_MODULE_PATH  := $(TARGET_OUT_VENDOR_ETC)/init
include $(BUILD_PREBUILT)
endif #KMGK_USE_QTI_SERVICE
endif #ENABLE_KM_4_0
endif #ENABLE_KM_4_1
endif #ENABLE_KEYMINT

ifeq ($(ENABLE_AUTH_SECRET),true)
include $(CLEAR_VARS)
LOCAL_MODULE       := android.hardware.authsecret@1.0-service-qti.rc
LOCAL_MODULE_TAGS  := optional
LOCAL_MODULE_CLASS := ETC
LOCAL_SRC_FILES    := $(LOCAL_MODULE)
LOCAL_MODULE_PATH  := $(TARGET_OUT_VENDOR_ETC)/init
include $(BUILD_PREBUILT)
endif #ENABLE_AUTH_SECRET


ifeq ($(ENABLE_KEYMINT),true)
include $(CLEAR_VARS)
LOCAL_MODULE       := android.hardware.security.keymint-service-qti.rc
LOCAL_MODULE_TAGS  := optional
LOCAL_MODULE_CLASS := ETC
LOCAL_SRC_FILES    := $(LOCAL_MODULE)
LOCAL_MODULE_PATH  := $(TARGET_OUT_VENDOR_ETC)/init
include $(BUILD_PREBUILT)
endif #ENABLE_KEYMINT
