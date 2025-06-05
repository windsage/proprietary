QMAA_DISABLES_SPU := false
ifeq ($(TARGET_USES_QMAA),true)
ifneq ($(TARGET_USES_QMAA_OVERRIDE_SPU),true)
QMAA_DISABLES_SPU := true
endif #TARGET_USES_QMAA_OVERRIDE_SPU
endif #TARGET_USES_QMAA

ifneq ($(QMAA_DISABLES_SPU),true)
ifeq ($(call is-vendor-board-platform,QCOM),true)
ifeq ($(call is-board-platform-in-list,pineapple niobe), true)

# Build spcomlib
PRODUCT_PACKAGES += libspcom

# Build native daemons
PRODUCT_PACKAGES += spdaemon
PRODUCT_PACKAGES += sec_nvm
PRODUCT_PACKAGES += init.spdaemon.rc

# Build SPCom utility and test apps
PRODUCT_PACKAGES += splogger
PRODUCT_PACKAGES += spcom_utest

# Build SPU HAL v1.0
PRODUCT_PACKAGES += vendor.qti.spu@1.0
PRODUCT_PACKAGES += vendor.qti.spu@1.0.vendor

# Build SPU HAL v1.1
PRODUCT_PACKAGES += vendor.qti.spu@1.1
PRODUCT_PACKAGES += vendor.qti.spu@1.1.vendor
PRODUCT_PACKAGES += vendor.qti.spu@1.1-service
PRODUCT_PACKAGES += init.vendor.qti.spu@1.1-service.rc
PRODUCT_PACKAGES += sputestclient

# Build SPU HAL test app for V1.1
PRODUCT_PACKAGES += sputestclient

# Build SPU HAL v2.0
PRODUCT_PACKAGES += vendor.qti.spu@2.0
PRODUCT_PACKAGES += vendor.qti.spu@2.0.vendor
PRODUCT_PACKAGES += vendor.qti.spu@2.0-service
PRODUCT_PACKAGES += init.spuservice.rc
PRODUCT_PACKAGES += sputestclientv2

# Build SPU AIDL
PRODUCT_PACKAGES += vendor.qti.hardware.spu
PRODUCT_PACKAGES += vendor.qti.hardware.spu.vendor
PRODUCT_PACKAGES += vendor.qti.hardware.spu-service
PRODUCT_PACKAGES += init.vendor.qti.hardware.spu.service.rc
PRODUCT_PACKAGES += sputesthal

# Build SPU HAL test app for v2.0
PRODUCT_PACKAGES += sputestclientv2

endif
endif
endif #QMAA_DISABLES_SPU