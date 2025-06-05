LOCAL_PATH:= $(call my-dir)

JNI_DIR  := .

include $(CLEAR_VARS)

LOCAL_MODULE := libWlanServiceJni

LOCAL_SRC_FILES := $(JNI_DIR)/AidlClient.cpp


LOCAL_CLANG := true
LOCAL_CPPFLAGS := -Wall -Werror
LOCAL_HEADER_LIBRARIES :=jni_headers

LOCAL_SHARED_LIBRARIES := libcutils \
                          liblog \
                          libbinder_ndk \
                          vendor.qti.hardware.data.iwlandata-V1-ndk \


LOCAL_WHOLE_STATIC_LIBRARIES += android.hardware.radio.data-V2-ndk \
                                android.hardware.radio-V2-ndk \

LOCAL_STATIC_JAVA_LIBRARIES += android.hardware.radio.data-V2-java-source \
                               android.hardware.radio-V2-java-source \
                               vendor.qti.hardware.data.iwlandata-V1-java-source \


LOCAL_C_INCLUDES := $(JNI_DIR)

LOCAL_MODULE_OWNER := qti
LOCAL_PROPRIETARY_MODULE := true

ifeq ($(call is-board-platform-in-list,kona lito lahaina),true)
LOCAL_SANITIZE := integer_overflow
endif

include $(BUILD_SHARED_LIBRARY)
