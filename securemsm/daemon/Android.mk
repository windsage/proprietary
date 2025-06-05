LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)

SECUREMSM_PATH := $(TOP)/vendor/qcom/proprietary/securemsm
SECUREMSM_NOSHIP_PATH := $(TOP)/vendor/qcom/proprietary/securemsm-noship

include $(LIBION_HEADER_PATH_WRAPPER)
LOCAL_C_INCLUDES := $(LIBION_HEADER_PATHS)

LOCAL_C_INCLUDES += $(TARGET_OUT_INTERMEDIATES)/KERNEL_OBJ/usr/include \
                    $(LOCAL_PATH)/../QSEEComAPI \
                    $(LOCAL_PATH)/../tzcommon/inc \
                    $(SECUREMSM_PATH)/daemon/include \
                    $(SECUREMSM_PATH)/smcinvoke/inc/ \
                    $(SECUREMSM_PATH)/smcinvoke/TZCom/inc/ \
                    $(SECUREMSM_PATH)/smcinvoke/minksocket/include \
                    $(SECUREMSM_PATH)/smcinvoke/minksocket/sock \
                    $(SECUREMSM_PATH)/mink/inc \
                    $(SECUREMSM_PATH)/mink/inc/uid \
                    $(SECUREMSM_PATH)/mink/inc/interface \
                    $(SECUREMSM_PATH)/smcinvoke/QSEECom/inc \
                    $(SECUREMSM_NOSHIP_PATH)/smcinvoke/QSEECom/inc

LOCAL_HEADER_LIBRARIES := vendor_common_inc

LOCAL_ADDITIONAL_DEPENDENCIES := $(TARGET_OUT_INTERMEDIATES)/KERNEL_OBJ/usr

LOCAL_SHARED_LIBRARIES := \
        libc \
        libcutils \
        libutils \
        liblog \
        libdl \
        libQSEEComAPI \
        libdrmfs \

LOCAL_MODULE := qseecomd
ifeq ($(PRODUCT_VENDOR_MOVE_ENABLED),true)
LOCAL_PROPRIETARY_MODULE := true
endif

ifeq ($(TARGET_ENABLE_SMCI_SYSLISTENER),true)
QSEECOM_CFLAGS := -DTARGET_ENABLE_SMCI_SYSLISTENER
LOCAL_SRC_FILES := CListenersmci.cpp listenerMngr.cpp
LOCAL_SHARED_LIBRARIES += libminkdescriptor libdmabufheap

#If we call a function from C++ code to C code indirectly,
#We have to add a CFI blacklist.txt to avoid CFI check failure.
LOCAL_SANITIZE_BLOCKLIST := cfi_blacklist.txt
endif

ifeq ($(TARGET_VB_NOT_ENABLED),true)
QSEECOM_CFLAGS += -DHLOS_LOAD_KEYMASTER
endif

#Disable virtualized listeners (rpmb & ssd) for guest OS
ifeq ($(ENABLE_HYP),true)
QSEECOM_CFLAGS += -DLISTENERS_VIRTUALIZED
endif

ifeq ($(TARGET_USES_QMAA_OVERRIDE_GPT), true)
QSEECOM_CFLAGS += -DCOMPILE_GPT_LISTENER
endif

ifeq ($(TARGET_USES_QMAA_OVERRIDE_RPMB), true)
QSEECOM_CFLAGS += -DCOMPILE_RPMB_LISTENER
endif

ifeq ($(TARGET_USES_QMAA_OVERRIDE_DRM),true)
QSEECOM_CFLAGS += -DCOMPILE_OPS_LISTENER
endif

#Enable qseecom if TARGET_ENABLE_QSEECOM or TARGET_BOARD_AUTO is true
ifneq (, $(filter true, $(TARGET_ENABLE_QSEECOM) $(TARGET_BOARD_AUTO)))
QSEECOM_CFLAGS += -DTARGET_ENABLE_QSEECOM
endif
LOCAL_SRC_FILES += qseecomd.c
LOCAL_MODULE_TAGS := optional
LOCAL_CFLAGS := $(QSEECOM_CFLAGS)
LOCAL_PRELINK_MODULE := false
LOCAL_MODULE_OWNER := qti
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_EXECUTABLE)

include $(CLEAR_VARS)
LOCAL_MODULE       := qseecomd.rc
LOCAL_MODULE_TAGS  := optional
LOCAL_MODULE_CLASS := ETC
ifeq ($(call is-board-platform-in-list,sdm845 sdm710),true)
LOCAL_SRC_FILES    := postfs/$(LOCAL_MODULE)
else
LOCAL_SRC_FILES    := $(LOCAL_MODULE)
endif
LOCAL_MODULE_PATH  := $(TARGET_OUT_VENDOR_ETC)/init
include $(BUILD_PREBUILT)

ifeq ($(call is-board-platform-in-list,),true)

###############Compile QSEECOMFSD library #################################

include $(CLEAR_VARS)
LOCAL_C_INCLUDES := $(TARGET_OUT_INTERMEDIATES)/KERNEL_OBJ/usr/include \
                    $(LOCAL_PATH)/../QSEEComAPI \
                    $(LOCAL_PATH)/../tzcommon/inc \

LOCAL_ADDITIONAL_DEPENDENCIES := $(TARGET_OUT_INTERMEDIATES)/KERNEL_OBJ/usr

LOCAL_LDLIBS := -llog
LOCAL_LDLIB +=- landroid
LOCAL_MODULE := qseecomfsd
ifeq ($(TARGET_VB_NOT_ENABLED),true)
QSEECOM_CFLAGS += -DHLOS_LOAD_KEYMASTER
endif
LOCAL_SRC_FILES := qseecomfsd.c
LOCAL_HEADER_LIBRARIES := vendor_common_inc
LOCAL_STATIC_LIBRARIES := libdrmMinimalfs \
                          libQSEEComAPIStatic \
                          libc \
                          liblog \
                          libcutils \

LOCAL_MODULE_PATH := $(TARGET_RECOVERY_OUT)
LOCAL_MODULE_TAGS := optional
LOCAL_FORCE_STATIC_EXECUTABLE := true
LOCAL_CFLAGS := $(QSEECOM_CFLAGS)
LOCAL_PRELINK_MODULE := false
LOCAL_MODULE_OWNER := qti
include $(BUILD_EXECUTABLE)

##############Compile QSEECOMD_STATIC ####################################


include $(CLEAR_VARS)
LOCAL_C_INCLUDES := $(TARGET_OUT_INTERMEDIATES)/KERNEL_OBJ/usr/include \
                    $(LOCAL_PATH)/../QSEEComAPI \
                    $(LOCAL_PATH)/../tzcommon/inc \

LOCAL_ADDITIONAL_DEPENDENCIES := $(TARGET_OUT_INTERMEDIATES)/KERNEL_OBJ/usr

LOCAL_HEADER_LIBRARIES := vendor_common_inc

LOCAL_STATIC_LIBRARIES := \
        libc \
        libcutils \
        liblog \
        libssdStatic \
        librpmbStatic \
        libgptStatic \
        libQSEEComAPIStatic

LOCAL_FORCE_STATIC_EXECUTABLE := true
LOCAL_MODULE := qseecomd_static
ifeq ($(TARGET_VB_NOT_ENABLED),true)
QSEECOM_CFLAGS += -DHLOS_LOAD_KEYMASTER
endif
LOCAL_SRC_FILES := qseecomd_static.c
LOCAL_MODULE_TAGS := optional
LOCAL_CFLAGS := $(QSEECOM_CFLAGS) -static
LOCAL_PRELINK_MODULE := false
LOCAL_MODULE_OWNER := qti
include $(BUILD_EXECUTABLE)

endif


##############GPFS path config file ######################################

include $(CLEAR_VARS)
LOCAL_MODULE := gpfspath_oem_config.xml
LOCAL_MODULE_CLASS := ETC
LOCAL_SRC_FILES := gpfspath_oem_config.xml
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_PATH := $(TARGET_OUT_VENDOR_ETC)
LOCAL_MODULE_OWNER := qti
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)
