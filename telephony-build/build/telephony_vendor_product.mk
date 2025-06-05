
ifneq ($(TARGET_NO_TELEPHONY), true)
PRODUCT_PROPERTY_OVERRIDES += \
    persist.vendor.radio.apm_sim_not_pwdn=1 \
    persist.vendor.radio.sib16_support=1 \
    persist.vendor.radio.custom_ecc=1 \
    persist.vendor.radio.procedure_bytes=SKIP

ifeq ($(TARGET_SUPPORTS_WEARABLES),true)
    PRODUCT_PROPERTY_OVERRIDES += persist.radio.multisim.config=ssss
else
ifeq ($(TARGET_FWK_SUPPORTS_FULL_VALUEADDS),true)
    PRODUCT_PROPERTY_OVERRIDES += persist.radio.multisim.config=dsds
else
    # For pdk/vanila aosp.
    PRODUCT_PROPERTY_OVERRIDES += persist.radio.multisim.config=ssss
endif #TARGET_FWK_SUPPORTS_FULL_VALUEADDS
endif #TARGET_SUPPORTS_WEARABLES

PRODUCT_COPY_FILES += \
frameworks/native/data/etc/android.hardware.telephony.gsm.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.telephony.gsm.xml \
frameworks/native/data/etc/android.hardware.telephony.cdma.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.telephony.cdma.xml \
frameworks/native/data/etc/android.hardware.telephony.ims.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.telephony.ims.xml \
frameworks/native/data/etc/android.hardware.se.omapi.uicc.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.se.omapi.uicc.xml
endif #TARGET_NO_TELEPHONY


# =================================================================================
# Copy files for runtime differentiation between modem-based and modem-less targets
# =================================================================================

TELEPHONY_XML_PATH := $(QCPATH)/telephony-apps/etc
NO_RIL_DIR := noRil

ifneq ($(wildcard $(TELEPHONY_XML_PATH)),)
  # Copy list of telephony packages. This will be used by frameworks to disable telephony modules
  # at runtime for APQ variants
  PRODUCT_COPY_FILES += $(TELEPHONY_XML_PATH)/telephony_packages.xml:$(TARGET_COPY_OUT_VENDOR)/etc/telephony_packages.xml

  # Copy file to exclude telephony feature on APQ variants
  PRODUCT_COPY_FILES += \
    $(TELEPHONY_XML_PATH)/apq_excluded_telephony_features.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/$(NO_RIL_DIR)/apq_excluded_telephony_features.xml
endif

# =================================================================================

ifneq ($(TARGET_SUPPORTS_WEARABLES),true)
PRODUCT_PACKAGES += ATFWD-daemon
endif

ifneq ($(TARGET_NO_TELEPHONY), true)
# QCRIL-NR
QCRIL := qcrilNr.db
QCRIL += libqcrilNr
QCRIL += qcrild_libqcrilnr
QCRIL += qcrild_libqcrilnrutils
QCRIL += libqcrilNrLogger
QCRIL += qcrilNrd
QCRIL += qcrilNrd.rc
QCRIL += libqcrilNrQtiMutex
QCRIL += qtiwakelock
QCRIL += libqcrilNrFramework
QCRIL += deviceInfoServiceModuleNr
QCRIL += ccid_daemon_nr
QCRIL += libqcrilDataModule
QCRIL += liblqe
QCRIL += libril-qc-radioconfig
QCRIL += libril-qc-ltedirectdisc
QCRIL += librilqmiservices
QCRIL += libqmiextservices

# qtisetprop, qtigetprop required for all targets
QCRIL += qtisetprop
QCRIL += qtigetprop
QCRIL += libril-qc-radioconfig
QCRIL += libril-qc-ltedirectdisc
QCRIL += libqmiextservices
QCRIL += qcril_client

QCRIL_DEBUG += qcril_cli
QCRIL_DEBUG += libqcrilNrModuleConfigModule
endif #TARGET_NO_TELEPHONY

PRODUCT_PACKAGES += $(QCRIL)
PRODUCT_PACKAGES += $(TELEPHONY_APPS)

PRODUCT_PACKAGES_DEBUG += $(QCRIL_DEBUG)
ENABLE_VENDOR_RIL_SERVICE := true

