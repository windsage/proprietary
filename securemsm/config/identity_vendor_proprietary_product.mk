#PRODUCT_PACKAGES
ifeq ($(IDENTITY_USE_QTI_SERVICE),true)
PRODUCT_PACKAGES += android.hardware.identity-service-qti
PRODUCT_PACKAGES += android.hardware.identity-service-qti.rc
PRODUCT_PACKAGES += libqtiidentitycredential
else
PRODUCT_PACKAGES += android.hardware.identity-service.example
PRODUCT_PACKAGES += identity-default.rc
PRODUCT_PACKAGES += android.hardware.identity-libeic-hal-common
endif # IDENTITY_USE_QTI_SERVICE
