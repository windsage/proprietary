TARGET_USES_NQ_NFC := true

NQ_VENDOR_NFC_PROP := nqnfcinfo

#SN100,SN220 NFC firmware images + config files
NQ_VENDOR_NFC_PROP += libsn100u_fw
NQ_VENDOR_NFC_PROP += libsn220u_fw
PRODUCT_COPY_FILES += \
    vendor/qcom/proprietary/nqnfc-firmware/libnfc-mtp-SN100.conf:$(TARGET_COPY_OUT_VENDOR)/etc/libnfc-mtp-SN100.conf \
    vendor/qcom/proprietary/nqnfc-firmware/libnfc-qrd-SN100.conf:$(TARGET_COPY_OUT_VENDOR)/etc/libnfc-qrd-SN100.conf \
    vendor/qcom/proprietary/nqnfc-firmware/libnfc-mtp-SN100_38_4MHZ.conf:$(TARGET_COPY_OUT_VENDOR)/etc/libnfc-mtp-SN100_38_4MHZ.conf \
    vendor/qcom/proprietary/nqnfc-firmware/libnfc-qrd-SN100_38_4MHZ.conf:$(TARGET_COPY_OUT_VENDOR)/etc/libnfc-qrd-SN100_38_4MHZ.conf \
    vendor/qcom/proprietary/nqnfc-firmware/libnfc-SN220_19_2MHZ.conf:$(TARGET_COPY_OUT_VENDOR)/etc/libnfc-SN220_19_2MHZ.conf \
    vendor/qcom/proprietary/nqnfc-firmware/libnfc-SN220_38_4MHZ.conf:$(TARGET_COPY_OUT_VENDOR)/etc/libnfc-SN220_38_4MHZ.conf \

ifeq ($(strip $(TARGET_USES_NQ_NFC)),true)
PRODUCT_PACKAGES += $(NQ_VENDOR_NFC_PROP)
endif
