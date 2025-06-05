ifeq ($(strip $(TARGET_USES_SOTER)),true)

#SOTER Build Rules
PRODUCT_PACKAGES += vendor.qti.hardware.soter-V1-ndk
PRODUCT_PACKAGES += vendor.qti.hardware.soter-client
PRODUCT_PACKAGES += vendor.qti.hardware.soter-impl
PRODUCT_PACKAGES += vendor.qti.hardware.soter-service
PRODUCT_PACKAGES += vendor.qti.hardware.soter-service.rc
PRODUCT_PACKAGES += vendor.qti.hardware.soter-provision

endif #TARGET_USES_SOTER
