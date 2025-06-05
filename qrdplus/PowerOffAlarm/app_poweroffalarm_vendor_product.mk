ifeq ($(TARGET_FWK_SUPPORTS_FULL_VALUEADDS),true)
PRODUCT_PACKAGES += \
    power_off_alarm \
    vendor-qti-hardware-alarm.xml \
    vendor.qti.hardware.alarm \
    vendor.qti.hardware.alarm-impl \
    vendor.qti.hardware.alarm-service \
    vendor.qti.hardware.alarm-service.rc
endif