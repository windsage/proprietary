ifeq ($(strip $(TARGET_USES_GPQESE)),true)
#SECURE_ELEMENT Build Rules
SECUREMSM_VENDOR += libGPQeSE
SECUREMSM_VENDOR += android.hardware.secure_element-service.qti
SECUREMSM_VENDOR += android.hardware.secure_element-service.qti.rc
PRODUCT_COPY_FILES += \
    frameworks/native/data/etc/android.hardware.se.omapi.ese.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.se.omapi.ese.xml
ifeq ($(strip $(ENABLE_OMAPI_VINTF)),true)
SECUREMSM_VENDOR += secure_element-service.xml
SECUREMSM_VENDOR += hal_uuid_map_config.xml
SECUREMSM_VENDOR += com.android.se.xml
endif

ifeq ($(strip $(ENABLE_ESE_STRONGBOX_PROVISION_LIB)),true)
#eSE strongbox provisioning API library
SECUREMSM_VENDOR += libesesbprovision
endif

SECUREMSM_VENDOR += libEseUtils
endif

SECUREMSM_VENDOR += libsbprovision
PRODUCT_PACKAGES += $(SECUREMSM_VENDOR)
