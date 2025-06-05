ifneq ($(TARGET_ENABLE_MINK_COMPONENT),true)
LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

QC_PROP_ROOT ?= vendor/qcom/proprietary
MINK_PATH := $(TOP)/$(QC_PROP_ROOT)/securemsm/mink
QTVM_SDK_IDL_SRC_DIR := $(MINK_PATH)/idl
QTVM_SDK_INC_DIR := $(MINK_PATH)/inc/qtvm
TARGET_OUTPUT_DIR := $(TARGET_OUT_INTERMEDIATES)/qtvm_sdk/include
# get idl filename list in mink/idl
QTVM_SDK_IDL_SRC := $(wildcard $(QTVM_SDK_IDL_SRC_DIR)/*.idl)
QTVM_SDK_IDL_INVOKE_SRC := $(wildcard $(QTVM_SDK_IDL_SRC_DIR)/I*.idl)

$(warning  $(QTVM_SDK_IDL_SRC))
$(warning  $(QTVM_SDK_IDL_INVOKE_SRC))

# Define INTERNAL targets
QTVM_SDK_INT_IDL          := $(QTVM_SDK_IDL_SRC)
QTVM_SDK_INT_IDL_H        := $(patsubst $(QTVM_SDK_IDL_SRC_DIR)/%.idl, $(TARGET_OUTPUT_DIR)/%.h, $(QTVM_SDK_INT_IDL))
QTVM_SDK_INT_IDL_HPP      := $(patsubst $(QTVM_SDK_IDL_SRC_DIR)/%.idl, $(TARGET_OUTPUT_DIR)/%.hpp, $(QTVM_SDK_INT_IDL))
QTVM_SDK_INT_INVOKE_IDL   := $(QTVM_SDK_IDL_INVOKE_SRC)
QTVM_SDK_INT_IDL_INVK_H   := $(patsubst $(QTVM_SDK_IDL_SRC_DIR)/%.idl, $(TARGET_OUTPUT_DIR)/%_invoke.h, $(QTVM_SDK_INT_INVOKE_IDL))
QTVM_SDK_INT_IDL_INVK_HPP := $(patsubst $(QTVM_SDK_IDL_SRC_DIR)/%.idl, $(TARGET_OUTPUT_DIR)/%_invoke.hpp, $(QTVM_SDK_INT_INVOKE_IDL))

# Define INTERNAL recipes
$(TARGET_OUTPUT_DIR):
	@mkdir -p $@
$(QTVM_SDK_INT_IDL_H): $(TARGET_OUTPUT_DIR)/%.h : $(QTVM_SDK_IDL_SRC_DIR)/%.idl
	$(MINK_PATH)/idl/minkidl -o $@ $<
$(QTVM_SDK_INT_IDL_HPP): $(TARGET_OUTPUT_DIR)/%.hpp : $(QTVM_SDK_IDL_SRC_DIR)/%.idl
	$(MINK_PATH)/idl/minkidl -o $@ $< --cpp
$(QTVM_SDK_INT_IDL_INVK_H): $(TARGET_OUTPUT_DIR)/%_invoke.h : $(QTVM_SDK_IDL_SRC_DIR)/%.idl
	$(MINK_PATH)/idl/minkidl -o $@ $< --skel
$(QTVM_SDK_INT_IDL_INVK_HPP): $(TARGET_OUTPUT_DIR)/%_invoke.hpp : $(QTVM_SDK_IDL_SRC_DIR)/%.idl
	$(MINK_PATH)/idl/minkidl -o $@ $< --skel --cpp

LOCAL_MODULE := qtvm_sdk_headers
LOCAL_MODULE_OWNER := qti
LOCAL_PROPRIETARY_MODULE := true
LOCAL_MODULE_TAGS := optional

LOCAL_EXPORT_C_INCLUDE_DIRS := $(TARGET_OUT_INTERMEDIATES)/qtvm_sdk/include
LOCAL_EXPORT_C_INCLUDE_DIRS += $(QTVM_SDK_INC_DIR)
LOCAL_GENERATED_SOURCES := $(TARGET_OUTPUT_DIR) \
	$(QTVM_SDK_INT_IDL_H) \
	$(QTVM_SDK_INT_IDL_HPP) \
	$(QTVM_SDK_INT_IDL_INVK_H) \
	$(QTVM_SDK_INT_IDL_INVK_HPP)

include $(BUILD_HEADER_LIBRARY)
endif
