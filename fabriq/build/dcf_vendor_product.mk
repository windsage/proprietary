DCF := qsap_dcfd
DCF += dcfd.rc
DCF += libDcfHalFramework
DCF += libDcfHalLogger
DCF += libDcfHalQtiMutex
DCF += libDcfHalBleProto

PRODUCT_COPY_FILES += \
  vendor/qcom/proprietary/fabriq-noship/dcfd/vendor.qti.dcf.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/vendor.qti.dcf.xml

PRODUCT_PACKAGES += $(DCF)
