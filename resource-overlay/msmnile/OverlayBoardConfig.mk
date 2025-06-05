#Add resource overlays per target
ifneq ($(strip $(TARGET_BOARD_AUTO)),true)
PRODUCT_PACKAGES += \
    FrameworksResTarget_Vendor \
    WifiResTarget_Vendor
else
PRODUCT_PACKAGES += \
    FrameworksResAutoTarget_Vendor \
    ServiceWifiResAutoTarget_Vendor \
    CarServiceResAutoTarget_Vendor
endif
