THERMAL_HAL := android.hardware.thermal

THERMAL_HAL += android.hardware.thermal-service.qti
THERMAL_HAL += android.hardware.thermal-service.qti.rc
THERMAL_HAL += android.hardware.thermal-service.qti.xml
PRODUCT_PACKAGES += $(THERMAL_HAL)

LIMITS_HAL += vendor.qti.hardware.limits@1.2-service
LIMITS_HAL += vendor.qti.hardware.limits-service.rc
LIMITS_HAL += vendor.qti.hardware.limits-service.xml

PRODUCT_PACKAGES += $(LIMITS_HAL)

PRODUCT_PACKAGES_DEBUG += thermal_hal_test
PRODUCT_PACKAGES_DEBUG += limits_hal_test
PRODUCT_PACKAGES_DEBUG += limits_hal_1_2_test
