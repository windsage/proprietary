QMAA_DISABLES_SPUKM := false
ifeq ($(TARGET_USES_QMAA),true)
ifneq ($(TARGET_USES_QMAA_OVERRIDE_SPU),true)
QMAA_DISABLES_SPUKM := true
endif #TARGET_USES_QMAA_OVERRIDE_SPU
endif #TARGET_USES_QMAA

ifneq ($(QMAA_DISABLES_SPUKM),true)
ifeq ($(call is-vendor-board-platform,QCOM),true)
ifeq ($(call is-board-platform-in-list,pineapple niobe),true)
ENABLE_SPU_KEYMINT := true
ENABLE_SPU_RKP := true
ENABLE_SPU_GK := true
ENABLE_SPU_AUTHSECRET := false
ENABLE_SPU_KEYMINT_TESTS := false
ENABLE_SPU_WEAVER := false
endif # Pineapple,niobe
endif # QCOM Platform

# Keymint
ifeq ($(ENABLE_SPU_KEYMINT),true)

PRODUCT_PACKAGES += android.hardware.security.keymint-service-spu-qti.rc
PRODUCT_COPY_FILES += frameworks/native/data/etc/android.hardware.keystore.app_attest_key.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.keystore.app_attest_key.xml

# RKP
ifeq ($(ENABLE_SPU_RKP),true)
PRODUCT_PROPERTY_OVERRIDES += vendor.spu.rkp_enabled=true
else
PRODUCT_PROPERTY_OVERRIDES += vendor.spu.rkp_enabled=false
endif # ENABLE_SPU_RKP

ifeq ($(call is-board-platform-in-list,pineapple),true)
PRODUCT_COPY_FILES += vendor/qcom/proprietary/spu/android_security/keymint_utils/android.hardware.strongbox_keystore.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/sku_pineapple/android.hardware.strongbox_keystore.xml
endif # Pineapple
ifeq ($(call is-board-platform-in-list,niobe),true)
PRODUCT_COPY_FILES += vendor/qcom/proprietary/spu/android_security/keymint_utils/android.hardware.strongbox_keystore.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/sku_niobe/android.hardware.strongbox_keystore.xml
endif # niobe

endif # ENABLE_SPU_KEYMINT

# Gatekeeper
ifeq ($(ENABLE_SPU_GK),true)
ENABLE_SPU_GK_TESTS := true
PRODUCT_PACKAGES += android.hardware.gatekeeper-service-spu-qti.rc
endif # ENABLE_SPU_GK

# Authsecret
ifeq ($(ENABLE_SPU_AUTHSECRET),true)
PRODUCT_PACKAGES += android.hardware.authsecret.qti.spu-service.rc
endif # ENABLE_SPU_AUTHSECRET

# Tests
ifeq ($(ENABLE_SPU_KEYMINT_TESTS),true)
PRODUCT_PACKAGES += qti_spu_keymint_tests
endif # ENABLE_SPU_KEYMINT_TESTS

# Gatekeeper Tests
ifeq ($(ENABLE_SPU_GK_TESTS),true)
PRODUCT_PACKAGES += spu_gatekeeper_tests
endif # ENABLE_SPU_GK_TESTS

# Weaver
ifeq ($(ENABLE_SPU_WEAVER),true)
PRODUCT_PACKAGES += android.hardware.weaver-service-spu-qti.rc
PRODUCT_PACKAGES += android.hardware.weaver-service-spu-qti.xml
endif # ENABLE_SPU_WEAVER

PRODUCT_PROPERTY_OVERRIDES += ro.hardware.keystore_desede=true # Enable TDES support
PRODUCT_PACKAGES += gatekeeper.$(TARGET_BOARD_PLATFORM)
PRODUCT_PACKAGES += keystore.$(TARGET_BOARD_PLATFORM)
PRODUCT_PACKAGES += libkeymaster_messages.vendor
PRODUCT_PACKAGES += android.hardware.authsecret@1.0.vendor
PRODUCT_PACKAGES += libspuqtigatekeeper
PRODUCT_PACKAGES += android.hardware.gatekeeper-service-spu-qti
PRODUCT_PACKAGES += android.hardware.security.keymint-service-spu-qti
PRODUCT_PACKAGES += android.hardware.authsecret.qti.spu-service
PRODUCT_PACKAGES += android.hardware.weaver-service-spu-qti
PRODUCT_PACKAGES += libspukeymint
PRODUCT_PACKAGES += libspucborutils
PRODUCT_PACKAGES += libspukeymintutils
PRODUCT_PACKAGES += libspukeymintdeviceutils
PRODUCT_PACKAGES += libspukeymintprovision
PRODUCT_PACKAGES += spu_install_keybox

endif #QMAA_DISABLES_SPUKM
