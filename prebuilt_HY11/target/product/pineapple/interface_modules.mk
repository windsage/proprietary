PREBUILT_PATH := $(call my-dir)
LOCAL_PATH := $(PREBUILT_PATH)

include $(CLEAR_VARS)
LOCAL_MODULE := com.qualcomm.qti.dpm.api@1.0.vendor
LOCAL_INSTALLED_MODULE_STEM := com.qualcomm.qti.dpm.api@1.0.so
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libhidlbase liblog libutils libcutils libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 64
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/pineapple/vendor/lib64/com.qualcomm.qti.dpm.api@1.0.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib64
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := vendor.qti.data.factoryservice-V1-ndk.vendor
LOCAL_INSTALLED_MODULE_STEM := vendor.qti.data.factoryservice-V1-ndk.so
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libbinder_ndk vendor.qti.hardware.data.dynamicddsaidlservice-V1-ndk.vendor vendor.qti.hardware.data.qmiaidlservice-V1-ndk.vendor vendor.qti.hardware.data.lceaidlservice-V1-ndk.vendor vendor.qti.hardware.data.flowaidlservice-V1-ndk.vendor vendor.qti.hardware.data.cneaidlservice.internal.api-V1-ndk.vendor vendor.qti.hardware.data.cneaidlservice.internal.server-V1-ndk.vendor vendor.qti.latencyaidlservice-V1-ndk.vendor vendor.qti.data.mwqemaidlservice-V1-ndk.vendor libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 64
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/pineapple/vendor/lib64/vendor.qti.data.factoryservice-V1-ndk.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib64
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := vendor.qti.hardware.cacert@1.0
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libhidlbase liblog libutils libcutils libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 64
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/pineapple/vendor/lib64/vendor.qti.hardware.cacert@1.0.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib64
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := vendor.qti.hardware.data.cneaidlservice.internal.api-V1-ndk.vendor
LOCAL_INSTALLED_MODULE_STEM := vendor.qti.hardware.data.cneaidlservice.internal.api-V1-ndk.so
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libbinder_ndk vendor.qti.hardware.data.cneaidlservice.internal.constants-V1-ndk.vendor libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 64
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/pineapple/vendor/lib64/vendor.qti.hardware.data.cneaidlservice.internal.api-V1-ndk.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib64
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := vendor.qti.hardware.data.cneaidlservice.internal.constants-V1-ndk.vendor
LOCAL_INSTALLED_MODULE_STEM := vendor.qti.hardware.data.cneaidlservice.internal.constants-V1-ndk.so
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libbinder_ndk libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 64
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/pineapple/vendor/lib64/vendor.qti.hardware.data.cneaidlservice.internal.constants-V1-ndk.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib64
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := vendor.qti.hardware.data.cneaidlservice.internal.server-V1-ndk.vendor
LOCAL_INSTALLED_MODULE_STEM := vendor.qti.hardware.data.cneaidlservice.internal.server-V1-ndk.so
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libbinder_ndk vendor.qti.hardware.data.cneaidlservice.internal.constants-V1-ndk.vendor libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 64
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/pineapple/vendor/lib64/vendor.qti.hardware.data.cneaidlservice.internal.server-V1-ndk.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib64
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := vendor.qti.hardware.data.connectionfactory-V1-ndk.vendor
LOCAL_INSTALLED_MODULE_STEM := vendor.qti.hardware.data.connectionfactory-V1-ndk.so
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libbinder_ndk vendor.qti.hardware.data.dataactivity-V1-ndk.vendor vendor.qti.hardware.data.ka-V1-ndk.vendor libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 64
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/pineapple/vendor/lib64/vendor.qti.hardware.data.connectionfactory-V1-ndk.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib64
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := vendor.qti.hardware.data.dataactivity-V1-ndk.vendor
LOCAL_INSTALLED_MODULE_STEM := vendor.qti.hardware.data.dataactivity-V1-ndk.so
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libbinder_ndk libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 64
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/pineapple/vendor/lib64/vendor.qti.hardware.data.dataactivity-V1-ndk.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib64
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := vendor.qti.hardware.data.ka-V1-ndk.vendor
LOCAL_INSTALLED_MODULE_STEM := vendor.qti.hardware.data.ka-V1-ndk.so
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libbinder_ndk libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 64
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/pineapple/vendor/lib64/vendor.qti.hardware.data.ka-V1-ndk.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib64
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := vendor.qti.hardware.data.qmiaidlservice-V1-ndk.vendor
LOCAL_INSTALLED_MODULE_STEM := vendor.qti.hardware.data.qmiaidlservice-V1-ndk.so
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libbinder_ndk libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 64
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/pineapple/vendor/lib64/vendor.qti.hardware.data.qmiaidlservice-V1-ndk.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib64
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := vendor.qti.hardware.dpmaidlservice-V1-ndk.vendor
LOCAL_INSTALLED_MODULE_STEM := vendor.qti.hardware.dpmaidlservice-V1-ndk.so
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libbinder_ndk libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 64
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/pineapple/vendor/lib64/vendor.qti.hardware.dpmaidlservice-V1-ndk.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib64
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := vendor.qti.hardware.dpmservice@1.0.vendor
LOCAL_INSTALLED_MODULE_STEM := vendor.qti.hardware.dpmservice@1.0.so
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libhidlbase liblog libutils libcutils libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 64
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/pineapple/vendor/lib64/vendor.qti.hardware.dpmservice@1.0.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib64
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := vendor.qti.hardware.dpmservice@1.1.vendor
LOCAL_INSTALLED_MODULE_STEM := vendor.qti.hardware.dpmservice@1.1.so
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := vendor.qti.hardware.dpmservice@1.0.vendor libhidlbase liblog libutils libcutils libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 64
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/pineapple/vendor/lib64/vendor.qti.hardware.dpmservice@1.1.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib64
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := vendor.qti.hardware.embmssl@1.0.vendor
LOCAL_INSTALLED_MODULE_STEM := vendor.qti.hardware.embmssl@1.0.so
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libhidlbase liblog libutils libcutils libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 64
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/pineapple/vendor/lib64/vendor.qti.hardware.embmssl@1.0.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib64
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := vendor.qti.hardware.embmssl@1.1.vendor
LOCAL_INSTALLED_MODULE_STEM := vendor.qti.hardware.embmssl@1.1.so
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := vendor.qti.hardware.embmssl@1.0.vendor libhidlbase liblog libutils libcutils libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 64
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/pineapple/vendor/lib64/vendor.qti.hardware.embmssl@1.1.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib64
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := vendor.qti.hardware.mwqemadapter@1.0.vendor
LOCAL_INSTALLED_MODULE_STEM := vendor.qti.hardware.mwqemadapter@1.0.so
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libhidlbase liblog libutils libcutils libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 64
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/pineapple/vendor/lib64/vendor.qti.hardware.mwqemadapter@1.0.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib64
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := vendor.qti.hardware.mwqemadapteraidlservice-V1-ndk.vendor
LOCAL_INSTALLED_MODULE_STEM := vendor.qti.hardware.mwqemadapteraidlservice-V1-ndk.so
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libbinder_ndk libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 64
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/pineapple/vendor/lib64/vendor.qti.hardware.mwqemadapteraidlservice-V1-ndk.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib64
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := vendor.qti.hardware.qccsyshal@1.0.vendor
LOCAL_INSTALLED_MODULE_STEM := vendor.qti.hardware.qccsyshal@1.0.so
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libhidlbase liblog libutils libcutils libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 64
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/pineapple/vendor/lib64/vendor.qti.hardware.qccsyshal@1.0.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib64
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := vendor.qti.hardware.qccsyshal@1.1.vendor
LOCAL_INSTALLED_MODULE_STEM := vendor.qti.hardware.qccsyshal@1.1.so
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := vendor.qti.hardware.qccsyshal@1.0.vendor libhidlbase liblog libutils libcutils libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 64
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/pineapple/vendor/lib64/vendor.qti.hardware.qccsyshal@1.1.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib64
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := vendor.qti.hardware.qccsyshal@1.2.vendor
LOCAL_INSTALLED_MODULE_STEM := vendor.qti.hardware.qccsyshal@1.2.so
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := vendor.qti.hardware.qccsyshal@1.0.vendor vendor.qti.hardware.qccsyshal@1.1.vendor libhidlbase liblog libutils libcutils libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 64
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/pineapple/vendor/lib64/vendor.qti.hardware.qccsyshal@1.2.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib64
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := vendor.qti.hardware.slmadapter@1.0.vendor
LOCAL_INSTALLED_MODULE_STEM := vendor.qti.hardware.slmadapter@1.0.so
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libhidlbase liblog libutils libcutils libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 64
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/pineapple/vendor/lib64/vendor.qti.hardware.slmadapter@1.0.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib64
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := vendor.qti.ims.connectionaidlservice-V1-ndk.vendor
LOCAL_INSTALLED_MODULE_STEM := vendor.qti.ims.connectionaidlservice-V1-ndk.so
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libbinder_ndk libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 64
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/pineapple/vendor/lib64/vendor.qti.ims.connectionaidlservice-V1-ndk.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib64
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := vendor.qti.ims.factory@1.0.vendor
LOCAL_INSTALLED_MODULE_STEM := vendor.qti.ims.factory@1.0.so
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := vendor.qti.ims.callcapability@1.0.vendor vendor.qti.ims.rcsconfig@2.0.vendor libhidlbase liblog libutils libcutils libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 64
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/pineapple/vendor/lib64/vendor.qti.ims.factory@1.0.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib64
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := vendor.qti.ims.factory@1.1.vendor
LOCAL_INSTALLED_MODULE_STEM := vendor.qti.ims.factory@1.1.so
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := vendor.qti.ims.callcapability@1.0.vendor vendor.qti.ims.factory@1.0.vendor vendor.qti.ims.rcsconfig@2.0.vendor vendor.qti.ims.rcsconfig@2.1.vendor libhidlbase liblog libutils libcutils libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 64
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/pineapple/vendor/lib64/vendor.qti.ims.factory@1.1.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib64
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := vendor.qti.ims.factoryaidlservice-V1-ndk.vendor
LOCAL_INSTALLED_MODULE_STEM := vendor.qti.ims.factoryaidlservice-V1-ndk.so
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libbinder_ndk vendor.qti.ims.rcsuceaidlservice-V1-ndk.vendor vendor.qti.ims.rcssipaidlservice-V1-ndk.vendor vendor.qti.ims.callcapabilityaidlservice-V1-ndk.vendor vendor.qti.ims.connectionaidlservice-V1-ndk.vendor vendor.qti.ims.configaidlservice-V1-ndk.vendor libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 64
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/pineapple/vendor/lib64/vendor.qti.ims.factoryaidlservice-V1-ndk.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib64
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := vendor.qti.ims.rcssipaidlservice-V1-ndk.vendor
LOCAL_INSTALLED_MODULE_STEM := vendor.qti.ims.rcssipaidlservice-V1-ndk.so
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libbinder_ndk libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 64
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/pineapple/vendor/lib64/vendor.qti.ims.rcssipaidlservice-V1-ndk.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib64
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := vendor.qti.ims.rcsuceaidlservice-V1-ndk.vendor
LOCAL_INSTALLED_MODULE_STEM := vendor.qti.ims.rcsuceaidlservice-V1-ndk.so
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libbinder_ndk libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 64
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/pineapple/vendor/lib64/vendor.qti.ims.rcsuceaidlservice-V1-ndk.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib64
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := vendor.qti.imsrtpservice@3.0.vendor
LOCAL_INSTALLED_MODULE_STEM := vendor.qti.imsrtpservice@3.0.so
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libhidlbase liblog libutils libcutils libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 64
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/pineapple/vendor/lib64/vendor.qti.imsrtpservice@3.0.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib64
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := vendor.qti.imsrtpservice@3.1.vendor
LOCAL_INSTALLED_MODULE_STEM := vendor.qti.imsrtpservice@3.1.so
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := vendor.qti.imsrtpservice@3.0.vendor libhidlbase liblog libutils libcutils libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 64
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/pineapple/vendor/lib64/vendor.qti.imsrtpservice@3.1.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib64
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := vendor.qti.ImsRtpService-V1-ndk.vendor
LOCAL_INSTALLED_MODULE_STEM := vendor.qti.ImsRtpService-V1-ndk.so
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libbinder_ndk android.hardware.common-V2-ndk libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 64
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/pineapple/vendor/lib64/vendor.qti.ImsRtpService-V1-ndk.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib64
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

$(call symlink-file,,/vendor/lib64/libvndfwk_detect_jni.qti_vendor.so,$(PRODUCT_OUT)/vendor/app/CneApp/lib/arm64/libvndfwk_detect_jni.qti_vendor.so)
ALL_DEFAULT_INSTALLED_MODULES += $(PRODUCT_OUT)/vendor/app/CneApp/lib/arm64/libvndfwk_detect_jni.qti_vendor.so
