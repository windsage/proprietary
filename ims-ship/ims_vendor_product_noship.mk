ifneq ($(ENABLE_HYP), true)

ifeq ($(call is-board-platform-in-list,sdm660 sdm845 sdm710 msm8909 msm8998 msm8996 msm8937 msm8953 atoll kona lito msmnile sm6150 trinket), true)
IMS_NEWARCH := imsdatadaemon
IMS_NEWARCH += imsdatadaemon.rc
IMS_NEWARCH += imsqmidaemon
IMS_NEWARCH += imsqmidaemon.rc
IMS_NEWARCH += lib-siputility
IMS_RCS := lib-uceservice
IMS_RCS += lib-imscmservice
IMS_RCS += imsrcsd
IMS_RCS += imsrcsd.rc
IMS_RCS += lib-imsrcs-v2
IMS_RCS += lib-imsrcsbaseimpl
IMS_RCS += lib-rcsconfig
else
IMS_NEWARCH := imsdaemon
IMS_NEWARCH += imsdaemon.rc
IMS_NEWARCH += lib-imscommon

ifneq ($(TARGET_SUPPORTS_WEARABLES),true)
IMS_NEWARCH += lib-imsservice
IMS_NEWARCH += ims-dataservice-daemon
IMS_NEWARCH += ims-dataservice-daemon.rc
IMS_NEWARCH += lib-imsdcservice

IMS_RCS := lib-imsrcsservice
endif
endif

IMS_NEWARCH += ims_rtp_daemon

MSTAT_NEWARCH += vendor.qti.mstatservice@1.0


ifneq ($(call is-board-platform-in-list,msm8909 msm8953 msm8937 msm8952 msm8976 msm8998 sdm660),true)
IMS_NEWARCH += ims_rtp_daemon.rc
else
IMS_NEWARCH += ims_rtp_daemon_legacy.rc
endif

IMS_NEWARCH += lib-imsdpl
IMS_NEWARCH += lib-imsqimf
IMS_NEWARCH += lib-imsSDP
IMS_NEWARCH += lib-rtpcommon
IMS_NEWARCH += lib-rtpcore
IMS_NEWARCH += lib-rtpsl
IMS_NEWARCH += lib-imsvtcore
IMS_NEWARCH += librcc
IMS_NEWARCH += vendor.qti.ImsRtpService-aidl-Impl

#IMS_RCS
IMS_RCS += lib-imsxml



PRODUCT_PACKAGES += $(IMS_NEWARCH)
PRODUCT_PACKAGES += $(IMS_RCS)
PRODUCT_PACKAGES += $(MSTAT_NEWARCH)

ifneq ($(call is-board-platform-in-list,bengal),true)
PRODUCT_PROPERTY_OVERRIDES += persist.vendor.rcs.singlereg.feature=1
endif

endif
