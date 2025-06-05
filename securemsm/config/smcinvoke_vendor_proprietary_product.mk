
PRODUCT_PACKAGES += libsmcinvoketest_utils
PRODUCT_PACKAGES += smcinvoke_vendor_client
PRODUCT_PACKAGES += libqcbor
PRODUCT_PACKAGES += libminkdescriptor
PRODUCT_PACKAGES += tz_whitelist.json
PRODUCT_PACKAGES += ta_config.json
PRODUCT_PACKAGES += TrustZoneAccessService
PRODUCT_PACKAGES += libnative-api
PRODUCT_PACKAGES += libminksocket_vendor
ifeq ($(strip $(SSGTZD_DAEMON)),true)
PRODUCT_PACKAGES += ssgtzd
PRODUCT_PACKAGES += ssgtzd.rc
PRODUCT_PACKAGES += libtaautoload
PRODUCT_PACKAGES += libSaveQTEEDiag
endif
PRODUCT_PACKAGES += qwes_cli
PRODUCT_PACKAGES += qls_hlos
PRODUCT_PACKAGES += qls_uefi
PRODUCT_PACKAGES += qwesd
