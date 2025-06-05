ifeq ($(strip $(ENABLE_SECCAM)),true)
#No seccam lib/bin in /vendor yet (except for hal service)
ifeq ($(strip $(ENABLE_SECCAM_QTI_SERVICE)),true)
SECUREMSM_SECCAM := vendor.qti.hardware.seccam@1.0.vendor
SECUREMSM_SECCAM += vendor.qti.hardware.seccam@1.0_vendor
SECUREMSM_SECCAM += vendor.qti.hardware.seccam@1.0-service-qti
SECUREMSM_SECCAM += vendor.qti.hardware.seccam@1.0-service-qti.rc
endif
endif

ifeq ($(strip $(ENABLE_TRUSTED_UI_2_0)),true)
SECUREMSM_SECDISP += vendor.qti.hardware.systemhelper@1.0
SECUREMSM_SECDISP += vendor.qti.hardware.systemhelper@1.0.vendor
SECUREMSM_SECDISP += vendor.qti.hardware.systemhelper@1.0_vendor
endif

ifeq ($(strip $(ENABLE_TRUSTED_UI_AIDL)),true)
SECUREMSM_SECDISP += libsi
SECUREMSM_SECDISP += libloadtrusteduiapp
SECUREMSM_SECDISP += TrustedUISampleTestAIDL
SECUREMSM_SECDISP += vendor.qti.hardware.trustedui-V1-ndk
SECUREMSM_SECDISP += vendor.qti.hardware.trustedui-aidl-service-qti
SECUREMSM_SECDISP += vendor.qti.hardware.trustedui-aidl-service-qti.rc
SECUREMSM_SECDISP += vendor.qti.hardware.trustedui-aidl-service.xml
SECUREMSM_SECDISP += libTrustedInputAIDL
SECUREMSM_SECDISP += libTrustedUIAIDL
endif

ifeq ($(strip $(ENABLE_TRUSTED_UI_VM_3_0)), true)
SECUREMSM_SECDISP += TrustedUISampleTAClient
SECUREMSM_SECDISP += trusteduilistener
SECUREMSM_SECDISP += trusteduilistener.rc
SECUREMSM_SECDISP += libTrustedInputUtils
endif

ifeq ($(strip $(ENABLE_SECCAM_2_0)),true)
SECUREMSM_SECCAM += vendor.qti.hardware.secureprocessor.common@1.0.so
SECUREMSM_SECCAM += vendor.qti.hardware.secureprocessor.common@1.0.so.vendor
SECUREMSM_SECCAM += vendor.qti.hardware.secureprocessor.common@1.0.so_vendor
SECUREMSM_SECCAM += vendor.qti.hardware.secureprocessor.config@1.0.so
SECUREMSM_SECCAM += vendor.qti.hardware.secureprocessor.config@1.0.so.vendor
SECUREMSM_SECCAM += vendor.qti.hardware.secureprocessor.config@1.0.so_vendor
SECUREMSM_SECCAM += vendor.qti.hardware.secureprocessor.device@1.0.so
SECUREMSM_SECCAM += vendor.qti.hardware.secureprocessor.device@1.0.so.vendor
SECUREMSM_SECCAM += vendor.qti.hardware.secureprocessor.device@1.0.so_vendor
SECUREMSM_SECCAM += vendor.qti.hardware.secureprocessor.common@1.0-helper.so
SECUREMSM_SECCAM += vendor.qti.hardware.secureprocessor.common@1.0-helper.so.vendor
SECUREMSM_SECCAM += vendor.qti.hardware.secureprocessor.common@1.0-helper.so_vendor
SECUREMSM_SECCAM += vendor.qti.hardware.secureprocessor@1.0
SECUREMSM_SECCAM += vendor.qti.hardware.secureprocessor@1.0.rc
endif

ifeq ($(strip $(ENABLE_SECCAM_2_0_AIDL)),true)
SECUREMSM_SECCAM += vendor.qti.hardware.secureprocessor.common-V1-ndk.so
SECUREMSM_SECCAM += vendor.qti.hardware.secureprocessor.common-V1-ndk.so.vendor
SECUREMSM_SECCAM += vendor.qti.hardware.secureprocessor.common-V1-ndk.so_vendor
SECUREMSM_SECCAM += vendor.qti.hardware.secureprocessor.config-V1-ndk.so
SECUREMSM_SECCAM += vendor.qti.hardware.secureprocessor.config-V1-ndk.so.vendor
SECUREMSM_SECCAM += vendor.qti.hardware.secureprocessor.config-V1-ndk.so_vendor
SECUREMSM_SECCAM += vendor.qti.hardware.secureprocessor.device-V1-ndk.so
SECUREMSM_SECCAM += vendor.qti.hardware.secureprocessor.device-V1-ndk.so.vendor
SECUREMSM_SECCAM += vendor.qti.hardware.secureprocessor.device-V1-ndk.so_vendor
SECUREMSM_SECCAM += vendor.qti.hardware.secureprocessor.common-helper.so
SECUREMSM_SECCAM += vendor.qti.hardware.secureprocessor.common-helper.so.vendor
SECUREMSM_SECCAM += vendor.qti.hardware.secureprocessor.common-helper.so_vendor
SECUREMSM_SECCAM += vendor.qti.hardware.secureprocessor
SECUREMSM_SECCAM += vendor.qti.hardware.secureprocessor.xml
SECUREMSM_SECCAM += vendor.qti.hardware.secureprocessor.rc
endif

PRODUCT_PACKAGES += $(SECUREMSM_SECCAM)
PRODUCT_PACKAGES += $(SECUREMSM_SECDISP)

ifeq ($(strip $(ENABLE_WIDEVINE_DRM)), true)
SECUREMSM_WIDEVINE_DRM := liboemcrypto
SECUREMSM_WIDEVINE_DRM += libcppf
SECUREMSM_WIDEVINE_DRM += libcpion
SECUREMSM_WIDEVINE_DRM += libtrustedapploader
SECUREMSM_WIDEVINE_DRM += libminkdescriptor
SECUREMSM_WIDEVINE_DRM += libdrmfs
SECUREMSM_WIDEVINE_DRM += libdrmtime
SECUREMSM_WIDEVINE_DRM += libops
SECUREMSM_WIDEVINE_DRM += libhdcpsrm
SECUREMSM_WIDEVINE_DRM += libhdcp1prov
SECUREMSM_WIDEVINE_DRM += libops
SECUREMSM_WIDEVINE_DRM += libhdcp2p2prov
SECUREMSM_WIDEVINE_DRM += StoreKeybox
SECUREMSM_WIDEVINE_DRM += InstallKeybox
SECUREMSM_WIDEVINE_DRM += hdcp2p2prov
SECUREMSM_WIDEVINE_DRM += hdcp_srm
SECUREMSM_WIDEVINE_DRM += hdcp1prov
SECUREMSM_WIDEVINE_DRM += fsp_client

# Don't use dynamic DRM HAL for non-go SPs

# Add static and dynamic android.hardware.drm-service.widevine
# & android.hardware.drm-service.clearkey to PRODUCT_PACKAGES.
# These static and dynamic (lazy) services are needed for complete
# Widevine DRM use case on full Android-S & S-Go devices.
ifneq ($(TARGET_SUPPORTS_WEARABLES),true)
ifneq ($(TARGET_HAS_LOW_RAM),true)
# Integrating Widevine APEX
TARGET_BUILD_WIDEVINE := nonupdatable
TARGET_BUILD_WIDEVINE_USE_PREBUILT := true
-include vendor/widevine/libwvdrmengine/apex/device/device.mk
SECUREMSM_WIDEVINE_DRM += android.hardware.drm-service.clearkey
else
SECUREMSM_WIDEVINE_DRM += android.hardware.drm-service-lazy.widevine
SECUREMSM_WIDEVINE_DRM += android.hardware.drm-service-lazy.clearkey
endif
endif

ifeq ($(strip $(OTA_FLAG_FOR_DRM)),true)
SECUREMSM_WIDEVINE_DRM += move_widevine_data.sh
endif

#
# Drminitialization is invoked during setDataSource and as a part
# initialization, DrmManagerClient is created which tries to get
# DrmManagerService(“drm.drmManager”).
# Earlier, if the service is not yet started & if the property
# “drm.service.enabled” is not set, then getService call returns NULL
# immediately and client creates a No-op-DrmClientManager.
# But now, AOSP change has disabled this service by default and it will
# only be started if the property  drm.service.enabled is set.
# Hence setting this property in vendor shippable config.mk
#
PRODUCT_PROPERTY_OVERRIDES += drm.service.enabled = true

#The flag makes other components to distinguish and use updated and modified field of WideVine.
SECUREMSM_UPDATED_WIDEVINE_USED := true

#ENABLE_WIDEVINE_DRM
endif

ifeq ($(strip $(ENABLE_HLOS_DATA_PATH)), true)
PRODUCT_PROPERTY_OVERRIDES += vendor.wv.oemcrypto.debug.enable_hlos_data_path = true
endif

PRODUCT_PACKAGES += $(SECUREMSM_WIDEVINE_DRM)

ifeq ($(strip $(ENABLE_PLAYREADY_DRM)), true)
SECUREMSM_PLAYREADY_DRM := drmtest
SECUREMSM_PLAYREADY_DRM += libdrmfs
SECUREMSM_PLAYREADY_DRM += libdrmMinimalfs
SECUREMSM_PLAYREADY_DRM += libdrmtime
SECUREMSM_PLAYREADY_DRM += libbase64
SECUREMSM_PLAYREADY_DRM += libprpk4
SECUREMSM_PLAYREADY_DRM += libprdrmengine
ifeq ($(ENABLE_PLAYREADY_AIDL_SERVICE),true)
SECUREMSM_PLAYREADY_DRM += android.hardware.drm-service.playready-qti
SECUREMSM_PLAYREADY_DRM += android.hardware.drm-service.playready-qti.rc
SECUREMSM_PLAYREADY_DRM += libpraidl
else
SECUREMSM_PLAYREADY_DRM += android.hardware.drm@1.3-service.playready-qti
SECUREMSM_PLAYREADY_DRM += android.hardware.drm@1.3-service.playready-qti.rc
SECUREMSM_PLAYREADY_DRM += libprhidl
endif
SECUREMSM_PLAYREADY_DRM += prdrmkeyprov
SECUREMSM_PLAYREADY_DRM_DEBUG += prdrm_gtest
endif

PRODUCT_PACKAGES += $(SECUREMSM_PLAYREADY_DRM)
PRODUCT_PACKAGES_DEBUG += $(SECUREMSM_PLAYREADY_DRM_DEBUG)

ifeq ($(strip $(ENABLE_PERIPHERAL_STATE_UTILS)), true)
SECUREMSM_PERIPHERAL_SECURITY := libPeripheralStateUtils
endif

PRODUCT_PACKAGES += $(SECUREMSM_PERIPHERAL_SECURITY)
