ifeq ($(TARGET_USES_QMAA),true)
ifneq ($(TARGET_USES_QMAA_OVERRIDE_KMGK),true)
ifeq ($(KMGK_USE_QTI_STUBS),)
KMGK_USE_QTI_SERVICE := true
ENABLE_KM_4_0 := true
KMGK_USE_QTI_STUBS := true
ENABLE_STRONGBOX_KM := false
endif #KMGK_USE_QTI_STUBS
else
ifneq ($(TARGET_USES_GY),true)
KMGK_USE_QTI_SERVICE := true
else
KMGK_USE_QTI_SERVICE := false
endif
ENABLE_KEYMINT := true
ENABLE_STRONGBOX_KM := false
ENABLE_KM_SPLIT_MANIFEST := true
GATEKEEPER_IS_SECURITY_LEVEL_SPU := false
endif #TARGET_USES_QMAA_OVERRIDE_KMGK
endif #TARGET_USES_QMAA

# Enable TDES support for KM4 and above
PRODUCT_PROPERTY_OVERRIDES += ro.hardware.keystore_desede=true

# Delete all keys after factory reset to keep RPMB partition memory clear
PRODUCT_PROPERTY_OVERRIDES += ro.crypto.metadata_init_delete_all_keys.enabled = true

#disable strongbox support for auto targets
ifeq ($(TARGET_BOARD_AUTO), true)
PRODUCT_PROPERTY_OVERRIDES += \
    vendor.gatekeeper.disable_spu = true
endif

#gatekeeper split
ifeq ($(GATEKEEPER_IS_SECURITY_LEVEL_SPU), true)
PRODUCT_PROPERTY_OVERRIDES += vendor.gatekeeper.is_security_level_spu =1
else
PRODUCT_PROPERTY_OVERRIDES += vendor.gatekeeper.is_security_level_spu =0
endif

ifeq ($(ADOPTABLE_STORAGE_SUPPORTED), true)
PRODUCT_PROPERTY_OVERRIDES += \
    ro.crypto.volume.filenames_mode = "aes-256-cts"
endif

ifeq ($(METADATA_ENC_FORMAT_VERSION_2), true)
PRODUCT_PROPERTY_OVERRIDES += \
    ro.crypto.dm_default_key.options_format.version = 2 \
    ro.crypto.volume.metadata.method=dm-default-key
endif

#PRODUCT_PACKAGES
ifeq ($(KMGK_USE_QTI_SERVICE),true)
PRODUCT_PACKAGES += libqtigatekeeper
PRODUCT_PACKAGES += android.hardware.gatekeeper-service-qti
PRODUCT_PACKAGES += android.hardware.keymaster@4.0-service-qti
PRODUCT_PACKAGES += android.hardware.keymaster@3.0-service-qti
PRODUCT_PACKAGES += android.hardware.keymaster@3.0-impl-qti

ifeq ($(ENABLE_KM_4_0), true)
PRODUCT_PACKAGES += android.hardware.keymaster@4.0-service-qti.rc
endif

ifeq ($(ENABLE_KM_4_1), true)
PRODUCT_PACKAGES += android.hardware.keymaster@4.1-service-qti
PRODUCT_PACKAGES += android.hardware.keymaster@4.1-service-qti.rc
endif

ifeq ($(ENABLE_KEYMINT), true)
PRODUCT_PACKAGES += libqtikeymint
PRODUCT_PACKAGES += libkeymaster_messages.vendor
PRODUCT_COPY_FILES += frameworks/native/data/etc/android.hardware.keystore.app_attest_key.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.keystore.app_attest_key.xml
PRODUCT_PACKAGES += android.hardware.security.keymint-service-qti
PRODUCT_PACKAGES += android.hardware.security.keymint-service-qti.rc
endif

PRODUCT_COPY_FILES += hardware/interfaces/security/keymint/aidl/default/android.hardware.hardware_keystore.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.hardware_keystore.xml

ifeq ($(ENABLE_STRONGBOX_KM), true)
PRODUCT_PACKAGES += android.hardware.keymaster@4.0-strongbox-service-qti
PRODUCT_PACKAGES += android.hardware.keymaster@4.0-strongbox-service-qti.rc
PRODUCT_COPY_FILES += vendor/qcom/proprietary/securemsm/keymaster_utils/android.hardware.strongbox_keystore.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.strongbox_keystore.xml
endif #ENABLE_STRONGBOX_KM

ifeq ($(ENABLE_AUTH_SECRET), true)
PRODUCT_PACKAGES += android.hardware.authsecret@1.0-service-qti
PRODUCT_PACKAGES += android.hardware.authsecret@1.0-service-qti.rc
endif

PRODUCT_PACKAGES += gatekeeper.$(TARGET_BOARD_PLATFORM)
PRODUCT_PACKAGES += init.qti.keymaster.sh
PRODUCT_PACKAGES += keystore.$(TARGET_BOARD_PLATFORM)
PRODUCT_PACKAGES += KmInstallKeybox
PRODUCT_PACKAGES += libkeymasterdeviceutils
PRODUCT_PACKAGES += libkeymasterprovision
PRODUCT_PACKAGES += libkeymasterutils
PRODUCT_PACKAGES += libqtikeymaster4
PRODUCT_PACKAGES += qti_gatekeeper_tests
PRODUCT_PACKAGES += qti_keymaster_tests
PRODUCT_PACKAGES += keymasterd
PRODUCT_PACKAGES += keymasterd.rc
PRODUCT_PACKAGES += android.hardware.keymaster@3.0.vendor
PRODUCT_PACKAGES += android.hardware.keymaster@4.0.vendor
PRODUCT_PACKAGES += android.hardware.keymaster@4.1.vendor
PRODUCT_PACKAGES += android.hardware.gatekeeper@1.0.vendor
PRODUCT_PACKAGES += android.hardware.authsecret@1.0.vendor

else

# Software Keymint package has its own manifest.
# Please remove Keymint's part from vendor's manifest in order
# to prevent manifest conflict
PRODUCT_PACKAGES += android.hardware.security.keymint-service
PRODUCT_PACKAGES += android.hardware.security.keymint-service.rc
PRODUCT_PACKAGES += libkeymint
PRODUCT_PACKAGES += libkeymaster_messages
PRODUCT_COPY_FILES += frameworks/native/data/etc/android.software.device_id_attestation.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.software.device_id_attestation.xml

# Software Gatekeeper has HIDL implementation. It is mandatory to use AIDL from
# Android U onwards and all HIDL's are deprecated. Therefore, we can enable
# Software Gatekeeper only for targets with SHIPPING_API_LEVEL < 34.
ifneq ($(call math_gt_or_eq, $(SHIPPING_API_LEVEL),34), true)
# Software gatekeeper package has its own manifest.
# Please remove Gatekeeper's part from vendor's manifest in order
# to prevent manifest conflict
PRODUCT_PACKAGES += android.hardware.gatekeeper@1.0-service.software
endif #ifdef SHIPPING_API_LEVEL

endif # KMGK_USE_QTI_SERVICE
