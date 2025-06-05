LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

ifneq ($(qcril_sanitize_diag),)
LOCAL_SANITIZE_DIAG := $(qcril_sanitize_diag)
endif

ifeq ($(QCRIL_BUILD_WITH_ASAN),true)
LOCAL_SANITIZE                          := $(qcril_sanitize)
endif
LOCAL_MODULE                            := libQtiDatabaseTests
LOCAL_VENDOR_MODULE                     := true
LOCAL_SHARED_LIBRARIES                  := qcrilMarshal
LOCAL_SHARED_LIBRARIES                  += liblog
LOCAL_SHARED_LIBRARIES                  += libqcrilNrQtiMutex
LOCAL_SHARED_LIBRARIES                  += qcrilInterfaces
LOCAL_SHARED_LIBRARIES                  += qcrilIntermodulemsgs
LOCAL_SHARED_LIBRARIES                  += libqcrilNrFramework
LOCAL_SHARED_LIBRARIES                  += libqcrilNrLogger
LOCAL_SHARED_LIBRARIES                  += libril-db
LOCAL_SHARED_LIBRARIES                  += libQtiTestMain
LOCAL_STATIC_LIBRARIES                  := libgtest
LOCAL_CFLAGS                            += $(qcril_cflags)
LOCAL_CPPFLAGS                          += $(qcril_cppflags)
LOCAL_LDFLAGS                           += $(qcril_ldflags)
LOCAL_SRC_FILES                         += $(filter Test%.cpp,$(call all-cpp-files-under,src))
include $(BUILD_HOST_SHARED_LIBRARY)

include $(CLEAR_VARS)
ifneq ($(qcril_sanitize_diag),)
LOCAL_SANITIZE_DIAG := $(qcril_sanitize_diag)
endif

ifeq ($(QCRIL_BUILD_WITH_ASAN),true)
LOCAL_SANITIZEi                         := $(qcril_sanitize)
endif
LOCAL_MODULE                            := QtiDatabaseTest
LOCAL_VENDOR_MODULE                     := true
LOCAL_SHARED_LIBRARIES                  := qcrilMarshal
LOCAL_SHARED_LIBRARIES                  += liblog
LOCAL_SHARED_LIBRARIES                  += libqcrilNrQtiMutex
LOCAL_SHARED_LIBRARIES                  += libqcrilNrQtiMutex
LOCAL_SHARED_LIBRARIES                  += libQtiMarshallingTests
LOCAL_SHARED_LIBRARIES                  += qcrilInterfaces
LOCAL_SHARED_LIBRARIES                  += qcrilIntermodulemsgs
LOCAL_SHARED_LIBRARIES                  += libqcrilNrFramework
LOCAL_SHARED_LIBRARIES                  += libqcrilNrLogger
LOCAL_SHARED_LIBRARIES                  += libril-db
LOCAL_SHARED_LIBRARIES                  += libQtiTestMain
LOCAL_STATIC_LIBRARIES                  := libgtest
LOCAL_CFLAGS                            += $(qcril_cflags)
LOCAL_CPPFLAGS                          += $(qcril_cppflags)
LOCAL_LDFLAGS                           += $(qcril_cppflags)
LOCAL_SRC_FILES                         += $(filter-out Test%.cpp,$(call all-cpp-files-under,src))
include $(BUILD_HOST_EXECUTABLE)
