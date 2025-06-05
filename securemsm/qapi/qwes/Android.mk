LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE := qls_hlos
LOCAL_PROPRIETARY_MODULE    := true
QC_PROP_ROOT ?= vendor/qcom/proprietary

HLOS_LICENSES := $(wildcard $(LOCAL_PATH)/data/*.pfm)
# The next two lines are only needed for internal builds
INTERNAL_PATH := $(LOCAL_PATH)/../../../securemsm-internal
HLOS_LICENSES += $(wildcard $(INTERNAL_PATH)/qwes/licenses/*.pfm)

STORE_DIR := $(PRODUCT_OUT)/persist/data/pfm/licenses
LOCAL_MODULE_PATH := $(STORE_DIR)

$(STORE_DIR)/store_0: $(HLOS_LICENSES)
	mkdir -p $(STORE_DIR)
	@python $(QC_PROP_ROOT)/securemsm/tools/qls/qlstool.py -m hlos -o $(STORE_DIR) $^
	cp $(HLOS_LICENSES) $(STORE_DIR)/

$(STORE_DIR)/store_1 $(STORE_DIR)/store_2 $(STORE_DIR)/store_3 $(STORE_DIR)/store_4: $(STORE_DIR)/store_0

LOCAL_ADDITIONAL_DEPENDENCIES := $(STORE_DIR)/store_0 $(STORE_DIR)/store_1 $(STORE_DIR)/store_2 $(STORE_DIR)/store_3 $(STORE_DIR)/store_4
include $(BUILD_PHONY_PACKAGE)

include $(CLEAR_VARS)
LOCAL_MODULE := qls_uefi
LOCAL_PROPRIETARY_MODULE    := true
LOCAL_MODULE_PATH := $(PRODUCT_OUT)

UEFI_LICENSES := $(wildcard $(LOCAL_PATH)/data/UEFI/*.pfm)
# The next two lines are only needed for internal builds
INTERNAL_PATH := $(LOCAL_PATH)/../../../securemsm-internal
UEFI_LICENSES += $(wildcard $(INTERNAL_PATH)/qwes/licenses/UEFI/*.pfm)

$(PRODUCT_OUT)/qweslicstore.bin: $(UEFI_LICENSES)
	mkdir -p $(PRODUCT_OUT)
	@python $(QC_PROP_ROOT)/securemsm/tools/qls/qlstool.py -o $(PRODUCT_OUT) $^

LOCAL_ADDITIONAL_DEPENDENCIES := $(PRODUCT_OUT)/qweslicstore.bin
include $(BUILD_PHONY_PACKAGE)
