LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)

LOCAL_SRC_FILES := \
                   InstallKeybox.cpp \
                   ProvisionKeybox.cpp \
                   ProvisionDeviceIds.cpp \
                   VerifyAttestation.cpp \
                   device_management_service_v01.c

QC_PROP_ROOT ?= vendor/qcom/proprietary
SECUREMSM_PATH := $(TOP)/$(QC_PROP_ROOT)/securemsm


LOCAL_C_INCLUDES += \
                    $(LOCAL_PATH)/../inc \
                    $(SECUREMSM_PATH)/QSEEComAPI \
                    $(TOP)/$(QC_PROP_ROOT)/spu/android_security/keymint_utils \
                    $(TOP)/external/boringssl/include \
                    $(TOP)/hardware/libhardware/include/hardware \
                    $(TOP)/$(QC_PROP_ROOT)/securemsm/mink/inc/uid/ \
                    $(TOP)/$(QC_PROP_ROOT)/qmi-framework/inc/ \
                    $(TOP)/$(QC_PROP_ROOT)/securemsm/mink/inc/interface

LOCAL_HEADER_LIBRARIES := jni_headers libhardware_headers vendor_common_inc \
                          libqmi_common_headers libdiag_headers \
                          libril-qc-qmi-services-headers libutils_headers

LOCAL_SHARED_LIBRARIES := \
                          liblog \
                          libcutils \
                          libcrypto \
                          libspukeymintprovision \
                          libqmi_cci \
                          libqmi_common_so \
                          libspukeymintutils\
                          libspukeymintdeviceutils \
                          libspukeymint \
                          libminkdescriptor \
                          libminksocket_vendor \
                          libdmabufheap

LOCAL_MODULE := spu_install_keybox
LOCAL_MODULE_TAGS := optional

LOCAL_CFLAGS := -Wall -Werror -Wunused -DKEYMASTER_NAME_TAGS -Wno-missing-field-initializers
LOCAL_MODULE_OWNER := qti
LOCAL_PROPRIETARY_MODULE := true

include $(BUILD_EXECUTABLE)

