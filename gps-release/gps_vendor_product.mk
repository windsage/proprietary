ifneq ($(BOARD_VENDOR_QCOM_GPS_LOC_API_HARDWARE),)

PRODUCT_PACKAGES += vendor.qti.gnss
PRODUCT_PACKAGES += vendor.qti.gnss-service

PRODUCT_PACKAGES += loc_api_app
PRODUCT_PACKAGES += test_loc_api_client
PRODUCT_PACKAGES += test_agps_server
PRODUCT_PACKAGES += izat.conf
PRODUCT_PACKAGES += sap.conf
PRODUCT_PACKAGES += com.qualcomm.location.vzw_library.xml
PRODUCT_PACKAGES += slim_daemon
PRODUCT_PACKAGES += libslimclient
PRODUCT_PACKAGES += loc_parameter.ini
PRODUCT_PACKAGES += pf_test_app
PRODUCT_PACKAGES += libalarmservice_jni
PRODUCT_PACKAGES += liblocationservice_glue
PRODUCT_PACKAGES += liblocationservice
PRODUCT_PACKAGES += libdataitems
PRODUCT_PACKAGES += libizat_core
PRODUCT_PACKAGES += liblbs_core
PRODUCT_PACKAGES += libloc_base_util
PRODUCT_PACKAGES += libloc_mq_client
PRODUCT_PACKAGES += liblowi_common
PRODUCT_PACKAGES += liblowi_client
PRODUCT_PACKAGES += liblowi_wifihal
PRODUCT_PACKAGES += liblowi_wifihal_nl
PRODUCT_PACKAGES += mlid
PRODUCT_PACKAGES += loc_launcher
PRODUCT_PACKAGES += libgdtap
PRODUCT_PACKAGES += libizat_client_api
PRODUCT_PACKAGES += libloc_pla
PRODUCT_PACKAGES += libloc_stub
PRODUCT_PACKAGES += xtra-daemon
PRODUCT_PACKAGES += libpolygon
PRODUCT_PACKAGES += libxtadapter
PRODUCT_PACKAGES += libcdfw
PRODUCT_PACKAGES += libcdfw_remote_api
PRODUCT_PACKAGES += libpal_net_if
PRODUCT_PACKAGES += libsys_info_cache
PRODUCT_PACKAGES += edgnss-daemon
PRODUCT_PACKAGES += libloc_qwes_iface

ifneq ($(TARGET_SUPPORTS_WEARABLES),true)
## do not install for wearable targets
PRODUCT_PACKAGES += libengineplugin
PRODUCT_PACKAGES += libloc_eng_hub
PRODUCT_PACKAGES += engine-service
endif

PRODUCT_PACKAGES += libloc_util
PRODUCT_PACKAGES += libloc_api_wds
PRODUCT_PACKAGES += libmapdata_api

ifneq ($(TARGET_NO_TELEPHONY), true)
## Build loc_ril_client only if telephony enabled
PRODUCT_PACKAGES += libloc_ril_client
endif

ifeq ($(PRODUCT_ENABLE_QESDK),true)
## install for targets supporting QESDK
PRODUCT_PACKAGES += liblocation_qesdk
PRODUCT_PACKAGES += qsap_location
endif

#Location Collector
PRODUCT_PACKAGES += libwifiobserver

#GPS-XTWiFi
PRODUCT_PACKAGES += xtwifi-client
PRODUCT_PACKAGES += xtwifi.conf
PRODUCT_PACKAGES += lowi-server
PRODUCT_PACKAGES += lowi.conf
PRODUCT_PACKAGES += liblowi_client

ifeq ($(TARGET_SUPPORTS_WEARABLES),true)
## install only for wearable targets
PRODUCT_PACKAGES += libloc_glink
endif

#GPS-Automotive
PRODUCT_PACKAGES += libDRPlugin
PRODUCT_PACKAGES += libdrplugin_client
PRODUCT_PACKAGES += lib_drplugin_server
PRODUCT_PACKAGES += DR_AP_Service
PRODUCT_PACKAGES += libloc_externalDr
PRODUCT_PACKAGES += libloc_externalDrcore
PRODUCT_PACKAGES += apdr.conf

#PRODUCT_PACKAGES += libloc_epDr
#PRODUCT_PACKAGES += libloc_externalDrcoreqdr3

ifeq ($(strip $(TARGET_BOARD_AUTO)),true)

SSV_SLL_SW_PLATFORM_LIST += msmnile
SSV_SLL_SW_PLATFORM_LIST += $(MSMSTEPPE)

ifneq (,$(filter $(SSV_SLL_SW_PLATFORM_LIST),$(TARGET_BOARD_PLATFORM)))
PRODUCT_PACKAGES += libloc_sll_impl
PRODUCT_PACKAGES += libloc_sll_sim
PRODUCT_PACKAGES += libsll
endif # FEATURE_SSV_SLL_SW := true

PRODUCT_PACKAGES += sll.conf
PRODUCT_PACKAGES += prgflash_uart
PRODUCT_PACKAGES += GSD5XP_FLASH_5.7.12-P3.bin
PRODUCT_PACKAGES += gnss_ssv_factorytest

endif # ifeq ($(strip $(TARGET_BOARD_AUTO)),true)

# GPS_DBG
PRODUCT_PACKAGES_DEBUG += garden_app
PRODUCT_PACKAGES_DEBUG += lowi_test
PRODUCT_PACKAGES_DEBUG += test-fdal
PRODUCT_PACKAGES_DEBUG += xtwifi-upload-test
PRODUCT_PACKAGES_DEBUG += test-fake-ap
PRODUCT_PACKAGES_DEBUG += test-pos-tx
PRODUCT_PACKAGES_DEBUG += test-version
PRODUCT_PACKAGES_DEBUG += libloc_rtt_pos_eng_simulator
PRODUCT_PACKAGES_DEBUG += gps-test.sh
PRODUCT_PACKAGES_DEBUG += libposlog
PRODUCT_PACKAGES_DEBUG += IZatSample
PRODUCT_PACKAGES_DEBUG += QCLocSvcTests
PRODUCT_PACKAGES_DEBUG += sftc
PRODUCT_PACKAGES_DEBUG += libLocModemEmulator
PRODUCT_PACKAGES_DEBUG += LocModemEmulatorCli
PRODUCT_PACKAGES_DEBUG += OptInAppOverlay
PRODUCT_PACKAGES_DEBUG += libizat_api_pds
PRODUCT_PACKAGES_DEBUG += liblocdiagiface
PRODUCT_PACKAGES_DEBUG += lcs_tester

# Targets to disable SLIM AP packages
SLIM_AP_DISABLE_PLATFORM_LIST += bengal_515
SLIM_AP_DISABLE_PLATFORM_LIST += blair
ifneq (,$(filter $(SLIM_AP_DISABLE_PLATFORM_LIST),$(TARGET_BOARD_PLATFORM)))
FEATURE_SLIM_AP := false
endif

ifneq ($(FEATURE_SLIM_AP),false)
PRODUCT_PACKAGES_DEBUG += slim_client
PRODUCT_PACKAGES_DEBUG += qmislim_client
PRODUCT_PACKAGES_DEBUG += qmislim_service
endif

# Targets to disable QSH packages
LOC_QSH_DISABLE_PLATFORM_LIST += bengal_515
LOC_QSH_DISABLE_PLATFORM_LIST += blair
ifneq (,$(filter $(LOC_QSH_DISABLE_PLATFORM_LIST),$(TARGET_BOARD_PLATFORM)))
FEATURE_GPS_LOC_QSH := false
endif

ifneq ($(FEATURE_GPS_LOC_QSH),false)
PRODUCT_PACKAGES_DEBUG += qsh_location_test
PRODUCT_PACKAGES_DEBUG += qsh_geofence_test
PRODUCT_PACKAGES_DEBUG += qsh_slpi_loc_test
endif

ifeq ($(TARGET_SUPPORTS_WEAR_OS),true)
LW_FEATURE_SET := true
FEATURE_PDMAPPER := false
FEATURE_GTP_AP := false
BUILD_XTWIFI_CLIENT := false
endif

ifeq ($(TARGET_SUPPORTS_WEAR_AON),true)
WEAR_POWER_SERVICE_PLATFORM_LIST += monaco
ifneq (,$(filter $(WEAR_POWER_SERVICE_PLATFORM_LIST),$(TARGET_BOARD_PLATFORM)))
PRODUCT_PACKAGES += libgnsswear_power
WEAR_POWER_SERVICE := true
endif # filter WEAR_POWER_SERVICE_PLATFORM_LIST
endif # ifeq ($(TARGET_SUPPORTS_WEAR_AON),true)

endif # BOARD_VENDOR_QCOM_GPS_LOC_API_HARDWARE

ifeq ($(GPS_LOWI_ONLY_BUILD),true)
PRODUCT_PACKAGES += lowi.conf
PRODUCT_PACKAGES += liblowi_client
PRODUCT_PACKAGES += liblowi_common
PRODUCT_PACKAGES += liblowi_wifihal
PRODUCT_PACKAGES += liblowi_wifihal_nl
PRODUCT_PACKAGES += lowi-server
PRODUCT_PACKAGES_DEBUG += lowi_test
endif
