#DPM

ifneq ($(TARGET_HAS_LOW_RAM),true)
ifeq ($(TARGET_FWK_SUPPORTS_FULL_VALUEADDS),true)
ifeq ($(BOARD_USES_DPM),true)
ifneq ($(BUILD_TINY_ANDROID),true)
ifeq ($(call is-vendor-board-platform,QCOM),true)

DPM += dpmQmiMgr
DPM += libdpmqmihal
DPM += dpmQmiMgr.rc
DPM += vendor.libdpmframework
DPM += vendor.dpm.conf
DPM += vendor.dpmd.rc
DPM += vendor.dpmd
DPM += vendor.libdpmfdmgr
DPM += vendor.libdpmctmgr
DPM += vendor.libdpmtcm
DPM += vendor.libmwqemiptablemgr
DPM += vendor.qti.hardware.dpmservice@1.0
DPM += vendor.qti.hardware.dpmservice@1.1


PRODUCT_PACKAGES += $(DPM)

endif
endif
endif
endif

ifeq ($(call is-board-platform-in-list, lahaina holi taro kalama bengal pineapple blair volcano), true)
  PRODUCT_PROPERTY_OVERRIDES += persist.vendor.dpm.vndr.idletimer.mode=default
endif

 PRODUCT_PROPERTY_OVERRIDES += persist.vendor.dpm.vndr.halservice.enable=1
 PRODUCT_PROPERTY_OVERRIDES += persist.vendor.dpm.vndr.feature=11

else
  PRODUCT_PROPERTY_OVERRIDES += persist.vendor.dpm.vndr.halservice.enable=0
  PRODUCT_PROPERTY_OVERRIDES += persist.vendor.dpm.vndr.feature=0
endif

