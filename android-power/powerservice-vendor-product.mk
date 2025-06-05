ifeq ($(TARGET_USES_POWEROPT),true)
PRODUCT_PACKAGES += \
    libpowercore \
    libpowercallback \
    poweropt-service \
    poweropt-service.rc

ifeq ($(GENERIC_ODM_IMAGE), true)
PRODUCT_PACKAGES += \
    init.pixel.poweropt-service.rc
endif
endif
