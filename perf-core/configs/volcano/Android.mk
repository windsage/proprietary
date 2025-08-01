LOCAL_PATH := $(call my-dir)

TRAN_PERFLOCK_ENCODE_SUPPORT := yes

include $(CLEAR_VARS)

LOCAL_MODULE := perfboostsconfig.xml
LOCAL_PROPRIETARY_MODULE := true
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := ETC
LOCAL_MODULE_PATH := $(TARGET_OUT_VENDOR)/etc/perf

ifeq ($(strip $(TRAN_PERFLOCK_ENCODE_SUPPORT)),yes)
ENCRYPT_SCRIPT := $(LOCAL_PATH)/../../perfcore_encrypt.py
PERFBOOSTS_INFILE := $(LOCAL_PATH)/perfboostsconfig.xml
PERFBOOSTS_OUTFILE_ABS := $(PRODUCT_OUT)/obj/perf-encrypted/perfboostsconfig.xml.enc
PERFBOOSTS_OUTFILE_REL := ../../../../../../out/target/product/volcano/obj/perf-encrypted/perfboostsconfig.xml.enc

$(PERFBOOSTS_OUTFILE_ABS): $(PERFBOOSTS_INFILE) $(ENCRYPT_SCRIPT)
	@echo "Encrypting perfboostsconfig.xml"
	@mkdir -p $(dir $@)
	python3 $(ENCRYPT_SCRIPT) -e $< $@

LOCAL_SRC_FILES := $(PERFBOOSTS_OUTFILE_REL)
LOCAL_ADDITIONAL_DEPENDENCIES := $(PERFBOOSTS_OUTFILE_ABS)
else
LOCAL_SRC_FILES := perfboostsconfig.xml
endif

include $(BUILD_PREBUILT)

include $(CLEAR_VARS)

LOCAL_MODULE := perfconfigstore.xml
LOCAL_PROPRIETARY_MODULE := true
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := ETC
LOCAL_MODULE_PATH := $(TARGET_OUT_VENDOR)/etc/perf

ifeq ($(strip $(TRAN_PERFLOCK_ENCODE_SUPPORT)),yes)
ENCRYPT_SCRIPT := $(LOCAL_PATH)/../../perfcore_encrypt.py
PERFCONFIG_INFILE := $(LOCAL_PATH)/perfconfigstore.xml
PERFCONFIG_OUTFILE_ABS := $(PRODUCT_OUT)/obj/perf-encrypted/perfconfigstore.xml.enc
PERFCONFIG_OUTFILE_REL := ../../../../../../out/target/product/volcano/obj/perf-encrypted/perfconfigstore.xml.enc

$(PERFCONFIG_OUTFILE_ABS): $(PERFCONFIG_INFILE) $(ENCRYPT_SCRIPT)
	@echo "Encrypting perfconfigstore.xml"
	@mkdir -p $(dir $@)
	python3 $(ENCRYPT_SCRIPT) -e $< $@

LOCAL_SRC_FILES := $(PERFCONFIG_OUTFILE_REL)
LOCAL_ADDITIONAL_DEPENDENCIES := $(PERFCONFIG_OUTFILE_ABS)
else
LOCAL_SRC_FILES := perfconfigstore.xml
endif

include $(BUILD_PREBUILT)
