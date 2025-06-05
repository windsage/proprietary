#ANT
ANT_PACKAGE = com.dsi.ant@1.0.vendor
ANT_PACKAGE += com.dsi.ant@1.0-impl

#BT
ifeq ($(BOARD_HAVE_QTI_BT_LAZY_SERVICE), true)
ifeq ($(BOARD_HAVE_QTI_BT_SERVICE_VER_1_1), true)
BT_HAL_VER = android.hardware.bluetooth@1.1-service-qti-lazy
else
BT_HAL_VER = android.hardware.bluetooth@1.0-service-qti-lazy
endif
else
ifeq ($(BOARD_HAVE_QTI_BT_SERVICE_VER_1_1), true)
BT_HAL_VER = android.hardware.bluetooth@1.1-service-qti
else
BT_HAL_VER = android.hardware.bluetooth@1.0-service-qti
endif
endif

ifeq ($(BOARD_HAVE_QTI_BT_SERVICE_VER_1_1), true)
BT_HAL = android.hardware.bluetooth@1.1-impl-qti
endif

BT_PACKAGES += android.hardware.bluetooth@1.0-impl-qti
BT_PACKAGES += vendor.qti.hardware.bluetooth_audio@2.0-impl
BT_PACKAGES += vendor.qti.hardware.bluetooth_audio@2.1-impl
BT_PACKAGES += android.hardware.bluetooth.audio-impl-qti
#PRODUCT_PACKAGES += android.hardware.bluetooth.a2dp@2.0-impl
#PRODUCT_PACKAGES += com.qualcomm.qti.bluetooth_audio@1.0.vendor
PRODUCT_PACKAGES += vendor.qti.hardware.bluetooth_audio@2.0.vendor
PRODUCT_PACKAGES += vendor.qti.hardware.bluetooth_audio@2.1.vendor
BT_PACKAGES += btaudio_offload_if
BT_PACKAGES += audio.bluetooth.default
BT_PACKAGES += audio.bluetooth_qti.default
BT_PACKAGES += libbluetooth_audio_session_qti
BT_PACKAGES += libbluetooth_audio_session_qti_2_1
BT_PACKAGES += libbluetooth_audio_session
BT_PACKAGES += libbt-hidlclient
BT_PACKAGES += vendor.qti.hardware.btconfigstore@1.0.vendor
BT_PACKAGES += vendor.qti.hardware.btconfigstore@1.0-impl
BT_PACKAGES += vendor.qti.hardware.btconfigstore@2.0.vendor
BT_PACKAGES += vendor.qti.hardware.btconfigstore@2.0-impl
BT_PACKAGES += libbtnv
BT_PACKAGES_DEBUG += btconfig
ifeq ($(TARGET_USES_QTI_BTFTM), true)
BT_PACKAGES_DEBUG += btftmdaemon
endif
BT_PACKAGES += $(BT_HAL)
BT_PACKAGES += $(BT_HAL_VER)
#BTSAR
PRODUCT_PACKAGES += vendor.qti.hardware.bluetooth_sar@1.0.vendor
PRODUCT_PACKAGES += vendor.qti.hardware.bluetooth_sar@1.0-impl
SAR_PACKAGES += vendor.qti.hardware.bluetooth_sar@1.1.vendor
SAR_PACKAGES += vendor.qti.hardware.bluetooth_sar@1.1-impl

#BTTPI
BTTPI_PACKAGES += vendor.qti.hardware.bttpi-V1-ndk
BTTPI_PACKAGES += vendor.qti.hardware.bttpi-impl
PRODUCT_PACKAGES += bttpi-saidl

#BTDUN
ifeq ($(TARGET_USE_BT_DUN), true)
ifeq ($(TARGET_HAS_DUN_HIDL_FEATURE), true)
PRODUCT_PACKAGES += vendor.qti.hardware.bluetooth_dun@1.0.vendor
PRODUCT_PACKAGES += vendor.qti.hardware.bluetooth_dun@1.0-impl
PRODUCT_PACKAGES += vendor.qti.hardware.bluetooth_dun@1.0-service
PRODUCT_PACKAGES += vendor.qti.hardware.bluetooth_dun@1.0-service.rc
PRODUCT_PACKAGES += vendor.qti.hardware.bluetooth_dun@1.0-service.disable.rc
endif #TARGET_HAS_DUN_HIDL_FEATURE
endif #TARGET_USE_BT_DUN

PRODUCT_COPY_FILES += \
    frameworks/native/data/etc/android.hardware.bluetooth.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.bluetooth.xml \
    frameworks/native/data/etc/android.hardware.bluetooth_le.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.bluetooth_le.xml

ifeq ($(TARGET_HAS_QSH_BLE_FEATURE), true)
#QSH BLE
QSH_BLE_DIR := vendor/qcom/proprietary/bluetooth/qsh_ble
qsh_ble_config_files := $(wildcard $(QSH_BLE_DIR)/config/common/*.json)
qsh_ble_config_files += $(wildcard $(QSH_BLE_DIR)/config/$(TARGET_BOARD_PLATFORM)/*.json)
qsh_ble_copied_config_files := \
    $(foreach _conf_file,$(qsh_ble_config_files),$(_conf_file):$(TARGET_COPY_OUT_VENDOR)/etc/sensors/config/$(notdir $(_conf_file)))
PRODUCT_COPY_FILES += $(qsh_ble_copied_config_files)
#$(warning qsh_ble_copied_config_files=$(qsh_ble_copied_config_files))
# temporarily disable libqsh_ble_pb
#PRODUCT_PACKAGES += libqsh_ble_pb
#QSH_BLE = libqsh_ble_pb
endif

#FM
ifeq ($(BOARD_HAVE_QCOM_FM), true)
FM_PACKAGES += fmconfig
FM_PACKAGES += fmfactorytest
FM_PACKAGES += fmfactorytestserver
FM_PACKAGES += fm_qsoc_patches
FM_PACKAGES += vendor.qti.hardware.fm@1.0.vendor
FM_PACKAGES += vendor.qti.hardware.fm@1.0-impl
FM_PACKAGES_DEBUG += hal_ss_test_manual
FM_PACKAGES += init.qti.fm.sh
endif

#FM default packages
FM_PACKAGES += ftm_fm_lib

BTFM_PACKAGES = $(ANT_PACKAGE)
BTFM_PACKAGES += $(BT_PACKAGES)
BTFM_PACKAGES += $(SAR_PACKAGES)
BTFM_PACKAGES += $(BTTPI_PACKAGES)
#BTFM_PACKAGES += $(QSH_BLE)
BTFM_PACKAGES += $(FM_PACKAGES)
BTFM_PACKAGES_DEBUG = $(BT_PACKAGES_DEBUG)
BTFM_PACKAGES_DEBUG += $(FM_PACKAGES_DEBUG)

PRODUCT_PACKAGES += $(BTFM_PACKAGES)
PRODUCT_PACKAGES_DEBUG += $(BTFM_PACKAGES_DEBUG)

#WIPOWER
ifeq ($(BOARD_USES_WIPOWER), true)
PRODUCT_PACKAGES += vendor.qti.hardware.wipower@1.0_vendor
PRODUCT_PACKAGES += vendor.qti.hardware.wipower@1.0-impl
endif #BOARD_USES_WIPOWER


#ANT/BT/FM/WIPOWER PROPERTIES

ifeq ($(TARGET_BOARD_PLATFORM), msmnile) # msmnile specific defines
ifeq ($(TARGET_BOARD_TYPE), auto)
PRODUCT_PROPERTY_OVERRIDES += vendor.qcom.bluetooth.soc=rome
PRODUCT_PROPERTY_OVERRIDES += persist.vendor.qcom.bluetooth.soc=rome
else
#Bluetooth SOC type
PRODUCT_PROPERTY_OVERRIDES += persist.vendor.qcom.bluetooth.soc=cherokee
#split a2dp support
PRODUCT_PROPERTY_OVERRIDES += persist.vendor.qcom.bluetooth.enable.splita2dp=true
#a2dp offload capability
PRODUCT_PROPERTY_OVERRIDES += persist.vendor.qcom.bluetooth.a2dp_offload_cap=sbc-aptx-aptxtws-aptxhd-aac-ldac-aptxadaptive
#Embedded wipower mode
PRODUCT_PROPERTY_OVERRIDES += ro.vendor.bluetooth.wipower=false
#aac frame control support
PRODUCT_PROPERTY_OVERRIDES += persist.vendor.qcom.bluetooth.aac_frm_ctl.enabled=true
#TWS+ state support
PRODUCT_PROPERTY_OVERRIDES += persist.vendor.qcom.bluetooth.twsp_state.enabled=false
#Scrambling support
PRODUCT_PROPERTY_OVERRIDES += persist.vendor.qcom.bluetooth.scram.enabled=true
#AAC VBR support
PRODUCT_PROPERTY_OVERRIDES += persist.vendor.qcom.bluetooth.aac_vbr_ctl.enabled=false
endif
endif

ifeq ($(TARGET_BOARD_PLATFORM), sdm845) # SDM845 specific defines
#Bluetooth SOC type
PRODUCT_PROPERTY_OVERRIDES += persist.vendor.qcom.bluetooth.soc=cherokee
#split a2dp support
PRODUCT_PROPERTY_OVERRIDES += persist.vendor.qcom.bluetooth.enable.splita2dp=true
#a2dp offload capability
PRODUCT_PROPERTY_OVERRIDES += persist.vendor.qcom.bluetooth.a2dp_offload_cap=sbc-aptx-aptxtws-aptxhd-aac-ldac
#Embedded wipower mode
PRODUCT_PROPERTY_OVERRIDES += ro.vendor.bluetooth.wipower=false
#aac frame control support
PRODUCT_PROPERTY_OVERRIDES += persist.vendor.qcom.bluetooth.aac_frm_ctl.enabled=true
#TWS+ state support
PRODUCT_PROPERTY_OVERRIDES += persist.vendor.qcom.bluetooth.twsp_state.enabled=false
#Scrambling support
PRODUCT_PROPERTY_OVERRIDES += persist.vendor.qcom.bluetooth.scram.enabled=true
#AAC VBR support
PRODUCT_PROPERTY_OVERRIDES += persist.vendor.qcom.bluetooth.aac_vbr_ctl.enabled=false
endif


ifeq ($(TARGET_BOARD_PLATFORM), sdmshrike)  # Poipu/8195 specific defines
ifeq ($(TARGET_BOARD_TYPE), auto)
PRODUCT_PROPERTY_OVERRIDES += vendor.qcom.bluetooth.soc=rome
PRODUCT_PROPERTY_OVERRIDES += persist.vendor.qcom.bluetooth.soc=rome
endif
endif

ifeq ($(TARGET_BOARD_PLATFORM), sm6150)  # Talos/sm6150 specific defines
ifeq ($(TARGET_BOARD_TYPE), auto)
PRODUCT_PROPERTY_OVERRIDES += vendor.qcom.bluetooth.soc=rome
PRODUCT_PROPERTY_OVERRIDES += persist.vendor.qcom.bluetooth.soc=rome
else
#Bluetooth SOC type
PRODUCT_PROPERTY_OVERRIDES += persist.vendor.qcom.bluetooth.soc=cherokee
#split a2dp support
PRODUCT_PROPERTY_OVERRIDES += persist.vendor.qcom.bluetooth.enable.splita2dp=true
#a2dp offload capability
PRODUCT_PROPERTY_OVERRIDES += persist.vendor.qcom.bluetooth.a2dp_offload_cap=sbc-aptx-aptxtws-aptxhd-aac-ldac
#Embedded wipower mode
PRODUCT_PROPERTY_OVERRIDES += ro.vendor.bluetooth.wipower=false
#aac frame control support
PRODUCT_PROPERTY_OVERRIDES += persist.vendor.qcom.bluetooth.aac_frm_ctl.enabled=true
#TWS+ state support
PRODUCT_PROPERTY_OVERRIDES += persist.vendor.qcom.bluetooth.twsp_state.enabled=false
#Scrambling support
PRODUCT_PROPERTY_OVERRIDES += persist.vendor.qcom.bluetooth.scram.enabled=true
#AAC VBR support
PRODUCT_PROPERTY_OVERRIDES += persist.vendor.qcom.bluetooth.aac_vbr_ctl.enabled=false
endif
endif

ifeq ($(TARGET_BOARD_PLATFORM), sdm710)  # sdm710/Warlock specific defines
#Bluetooth SOC type
PRODUCT_PROPERTY_OVERRIDES += persist.vendor.qcom.bluetooth.soc=cherokee
#split a2dp support
PRODUCT_PROPERTY_OVERRIDES += persist.vendor.qcom.bluetooth.enable.splita2dp=true
#a2dp offload capability
PRODUCT_PROPERTY_OVERRIDES += persist.vendor.qcom.bluetooth.a2dp_offload_cap=sbc-aptx-aptxtws-aptxhd-aac-ldac
#Embedded wipower mode
PRODUCT_PROPERTY_OVERRIDES += ro.vendor.bluetooth.wipower=false
#aac frame control support
PRODUCT_PROPERTY_OVERRIDES += persist.vendor.qcom.bluetooth.aac_frm_ctl.enabled=true
#TWS+ state support
PRODUCT_PROPERTY_OVERRIDES += persist.vendor.qcom.bluetooth.twsp_state.enabled=false
#Scrambling support
PRODUCT_PROPERTY_OVERRIDES += persist.vendor.qcom.bluetooth.scram.enabled=true
#AAC VBR support
PRODUCT_PROPERTY_OVERRIDES += persist.vendor.qcom.bluetooth.aac_vbr_ctl.enabled=false
endif

ifeq ($(TARGET_BOARD_PLATFORM), kona)  # kona/sm8250 specific defines
#Bluetooth SOC type
PRODUCT_PROPERTY_OVERRIDES += persist.vendor.qcom.bluetooth.soc=hastings
#split a2dp support
PRODUCT_PROPERTY_OVERRIDES += persist.vendor.qcom.bluetooth.enable.splita2dp=true
#a2dp offload capability
PRODUCT_PROPERTY_OVERRIDES += persist.vendor.qcom.bluetooth.a2dp_offload_cap=sbc-aptx-aptxtws-aptxhd-aac-ldac-aptxadaptiver2
#Embedded wipower mode
PRODUCT_PROPERTY_OVERRIDES += ro.vendor.bluetooth.wipower=false
#aac frame control support
PRODUCT_PROPERTY_OVERRIDES += persist.vendor.qcom.bluetooth.aac_frm_ctl.enabled=true
#A2dp Multicast support
PRODUCT_PROPERTY_OVERRIDES += persist.vendor.qcom.bluetooth.a2dp_mcast_test.enabled=false
#TWS+ state support
PRODUCT_PROPERTY_OVERRIDES += persist.vendor.qcom.bluetooth.twsp_state.enabled=false
#Scrambling support
PRODUCT_PROPERTY_OVERRIDES += persist.vendor.qcom.bluetooth.scram.enabled=false
#AAC VBR support
PRODUCT_PROPERTY_OVERRIDES += persist.vendor.qcom.bluetooth.aac_vbr_ctl.enabled=true
#AptX Adaptive R2.1 support
PRODUCT_PROPERTY_OVERRIDES += persist.vendor.qcom.bluetooth.aptxadaptiver2_1_support=false
#HearingAid support
PRODUCT_PROPERTY_OVERRIDES += persist.sys.fflag.override.settings_bluetooth_hearing_aid=true
endif

ifeq ($(TARGET_BOARD_PLATFORM), lahaina) # lahaina specific defines
#a2dp offload capability
PRODUCT_PROPERTY_OVERRIDES += persist.vendor.qcom.bluetooth.a2dp_offload_cap=sbc-aptx-aptxtws-aptxhd-aac-ldac-aptxadaptiver2
#aac frame control support
PRODUCT_PROPERTY_OVERRIDES += persist.vendor.qcom.bluetooth.aac_frm_ctl.enabled=true
#TWS+ state support
PRODUCT_PROPERTY_OVERRIDES += persist.vendor.qcom.bluetooth.twsp_state.enabled=false
#A2dp Multicast support
PRODUCT_PROPERTY_OVERRIDES += persist.vendor.qcom.bluetooth.a2dp_mcast_test.enabled=false
#Scrambling support
PRODUCT_PROPERTY_OVERRIDES += persist.vendor.qcom.bluetooth.scram.enabled=false
#AAC VBR support
PRODUCT_PROPERTY_OVERRIDES += persist.vendor.qcom.bluetooth.aac_vbr_ctl.enabled=true
#AptX Adaptive R2.1 support
PRODUCT_PROPERTY_OVERRIDES += persist.vendor.qcom.bluetooth.aptxadaptiver2_1_support=true
#HearingAid support
PRODUCT_PROPERTY_OVERRIDES += persist.sys.fflag.override.settings_bluetooth_hearing_aid=true
endif

ifeq ($(TARGET_BOARD_PLATFORM), taro) # waipio specific defines
#a2dp offload capability
PRODUCT_PROPERTY_OVERRIDES += persist.vendor.qcom.bluetooth.a2dp_offload_cap=sbc-aptx-aptxtws-aptxhd-aac-ldac-aptxadaptiver2
#aac frame control support
PRODUCT_PROPERTY_OVERRIDES += persist.vendor.qcom.bluetooth.aac_frm_ctl.enabled=true
#TWS+ state support
PRODUCT_PROPERTY_OVERRIDES += persist.vendor.qcom.bluetooth.twsp_state.enabled=false
#A2dp Multicast support
PRODUCT_PROPERTY_OVERRIDES += persist.vendor.qcom.bluetooth.a2dp_mcast_test.enabled=false
#Scrambling support
PRODUCT_PROPERTY_OVERRIDES += persist.vendor.qcom.bluetooth.scram.enabled=false
#AAC VBR support
PRODUCT_PROPERTY_OVERRIDES += persist.vendor.qcom.bluetooth.aac_vbr_ctl.enabled=true
#AptX Adaptive R2.1 support
PRODUCT_PROPERTY_OVERRIDES += persist.vendor.qcom.bluetooth.aptxadaptiver2_1_support=true
#HearingAid support
PRODUCT_PROPERTY_OVERRIDES += persist.sys.fflag.override.settings_bluetooth_hearing_aid=true
endif

ifeq ($(TARGET_BOARD_PLATFORM), kalama) # kailua specific defines
#a2dp offload capability
PRODUCT_PROPERTY_OVERRIDES += persist.vendor.qcom.bluetooth.a2dp_offload_cap=sbc-aptx-aptxtws-aptxhd-aac-ldac-aptxadaptiver2
#aac frame control support
PRODUCT_PROPERTY_OVERRIDES += persist.vendor.qcom.bluetooth.aac_frm_ctl.enabled=true
#TWS+ state support
PRODUCT_PROPERTY_OVERRIDES += persist.vendor.qcom.bluetooth.twsp_state.enabled=false
#A2dp Multicast support
PRODUCT_PROPERTY_OVERRIDES += persist.vendor.qcom.bluetooth.a2dp_mcast_test.enabled=false
#Scrambling support
PRODUCT_PROPERTY_OVERRIDES += persist.vendor.qcom.bluetooth.scram.enabled=false
#AAC VBR support
PRODUCT_PROPERTY_OVERRIDES += persist.vendor.qcom.bluetooth.aac_vbr_ctl.enabled=true
#AptX Adaptive R2.1 support
PRODUCT_PROPERTY_OVERRIDES += persist.vendor.qcom.bluetooth.aptxadaptiver2_1_support=true
#HearingAid support
PRODUCT_PROPERTY_OVERRIDES += persist.sys.fflag.override.settings_bluetooth_hearing_aid=true
#Lossless over Aptx Adaptive LE support
PRODUCT_PROPERTY_OVERRIDES += persist.vendor.qcom.bluetooth.lossless_aptx_adaptive_le.enabled=false
#Basic Audio Profile (BAP) broadcast assist role is enabled
PRODUCT_PROPERTY_OVERRIDES += bluetooth.profile.bap.broadcast.assist.enabled=true
#Basic Audio Profile (BAP) broadcast source role is enabled
PRODUCT_PROPERTY_OVERRIDES += bluetooth.profile.bap.broadcast.source.enabled=true
#Basic Audio Profile (BAP) unicast client role is enabled
PRODUCT_PROPERTY_OVERRIDES += bluetooth.profile.bap.unicast.client.enabled=true
#Volume Control Profile (VCP) controller role is enabled
PRODUCT_PROPERTY_OVERRIDES += bluetooth.profile.vcp.controller.enabled=true
#Coordinated Set Identification Profile (CSIP) is enabled
PRODUCT_PROPERTY_OVERRIDES += bluetooth.profile.csip.set_coordinator.enabled=true
#Media Control Profile (MCP) is enabled
PRODUCT_PROPERTY_OVERRIDES += bluetooth.profile.mcp.server.enabled=true
#Media Control Profile (CCP) is enabled
PRODUCT_PROPERTY_OVERRIDES += bluetooth.profile.ccp.server.enabled=true
#Hearing Aid Profile (HAP) is enabled
PRODUCT_PROPERTY_OVERRIDES += bluetooth.profile.hap.client.enabled=true
endif

ifeq ($(TARGET_BOARD_PLATFORM), pineapple) # lanai specific defines
#a2dp offload capability
PRODUCT_PROPERTY_OVERRIDES += persist.vendor.qcom.bluetooth.a2dp_offload_cap=sbc-aptx-aptxtws-aptxhd-aac-ldac-aptxadaptiver2
#aac frame control support
PRODUCT_PROPERTY_OVERRIDES += persist.vendor.qcom.bluetooth.aac_frm_ctl.enabled=true
#TWS+ state support
PRODUCT_PROPERTY_OVERRIDES += persist.vendor.qcom.bluetooth.twsp_state.enabled=false
#A2dp Multicast support
PRODUCT_PROPERTY_OVERRIDES += persist.vendor.qcom.bluetooth.a2dp_mcast_test.enabled=false
#Scrambling support
PRODUCT_PROPERTY_OVERRIDES += persist.vendor.qcom.bluetooth.scram.enabled=false
#AAC VBR support
PRODUCT_PROPERTY_OVERRIDES += persist.vendor.qcom.bluetooth.aac_vbr_ctl.enabled=true
#AptX Adaptive R2.1 support
PRODUCT_PROPERTY_OVERRIDES += persist.vendor.qcom.bluetooth.aptxadaptiver2_1_support=true
#HearingAid support
PRODUCT_PROPERTY_OVERRIDES += persist.sys.fflag.override.settings_bluetooth_hearing_aid=true
#Lossless over Aptx Adaptive LE support
PRODUCT_PROPERTY_OVERRIDES += persist.vendor.qcom.bluetooth.lossless_aptx_adaptive_le.enabled=true
#Basic Audio Profile (BAP) broadcast assist role is enabled
PRODUCT_PROPERTY_OVERRIDES += bluetooth.profile.bap.broadcast.assist.enabled=true
#Basic Audio Profile (BAP) broadcast source role is enabled
PRODUCT_PROPERTY_OVERRIDES += bluetooth.profile.bap.broadcast.source.enabled=true
#Basic Audio Profile (BAP) unicast client role is enabled
PRODUCT_PROPERTY_OVERRIDES += bluetooth.profile.bap.unicast.client.enabled=true
#Volume Control Profile (VCP) controller role is enabled
PRODUCT_PROPERTY_OVERRIDES += bluetooth.profile.vcp.controller.enabled=true
#Coordinated Set Identification Profile (CSIP) is enabled
PRODUCT_PROPERTY_OVERRIDES += bluetooth.profile.csip.set_coordinator.enabled=true
#Media Control Profile (MCP) is enabled
PRODUCT_PROPERTY_OVERRIDES += bluetooth.profile.mcp.server.enabled=true
#Media Control Profile (CCP) is enabled
PRODUCT_PROPERTY_OVERRIDES += bluetooth.profile.ccp.server.enabled=true
#Hearing Aid Profile (HAP) is enabled
PRODUCT_PROPERTY_OVERRIDES += bluetooth.profile.hap.client.enabled=true
#DUAL Mode Transport support
PRODUCT_PROPERTY_OVERRIDES += persist.vendor.qcom.bluetooth.dualmode_transport_support=true
endif

ifeq ($(TARGET_BOARD_PLATFORM), neo61) # neo specific defines
PRODUCT_PACKAGES += libbthost_if_sink
PRODUCT_PROPERTY_OVERRIDES += persist.vendor.bluetooth.hfp_client=true
PRODUCT_PROPERTY_OVERRIDES += persist.vendor.bluetooth.split_a2dp_sink=true
PRODUCT_PROPERTY_OVERRIDES += persist.vendor.bluetooth.a2dp_sink_cap=sbc-aac
PRODUCT_PROPERTY_OVERRIDES += persist.vendor.qcom.bluetooth.aac_frm_ctl.enabled=true
PRODUCT_PROPERTY_OVERRIDES += persist.vendor.qcom.bluetooth.aac_vbr_ctl.enabled=true
PRODUCT_PROPERTY_OVERRIDES += persist.vendor.qcom.bluetooth.scram.enabled=false
#PBAP Client Support
PRODUCT_PROPERTY_OVERRIDES += persist.vendor.bluetooth.pbap_client=true
PRODUCT_PROPERTY_OVERRIDES += bluetooth.profile.pbap.client.enabled=true
endif

ifeq ($(TARGET_BOARD_PLATFORM), anorak) # halliday specific defines
#a2dp offload capability
PRODUCT_PROPERTY_OVERRIDES += persist.vendor.qcom.bluetooth.a2dp_offload_cap=sbc-aptx-aptxtws-aptxhd-aac-ldac-aptxadaptiver2
#aac frame control support
PRODUCT_PROPERTY_OVERRIDES += persist.vendor.qcom.bluetooth.aac_frm_ctl.enabled=true
#TWS+ state support
PRODUCT_PROPERTY_OVERRIDES += persist.vendor.qcom.bluetooth.twsp_state.enabled=false
#A2dp Multicast support
PRODUCT_PROPERTY_OVERRIDES += persist.vendor.qcom.bluetooth.a2dp_mcast_test.enabled=false
#Scrambling support
PRODUCT_PROPERTY_OVERRIDES += persist.vendor.qcom.bluetooth.scram.enabled=false
#AAC VBR support
PRODUCT_PROPERTY_OVERRIDES += persist.vendor.qcom.bluetooth.aac_vbr_ctl.enabled=true
#AptX Adaptive R2.1 support
PRODUCT_PROPERTY_OVERRIDES += persist.vendor.qcom.bluetooth.aptxadaptiver2_1_support=true
#HearingAid support
PRODUCT_PROPERTY_OVERRIDES += persist.sys.fflag.override.settings_bluetooth_hearing_aid=true
#Basic Audio Profile (BAP) broadcast assist role is enabled
PRODUCT_PROPERTY_OVERRIDES += bluetooth.profile.bap.broadcast.assist.enabled=true
#Basic Audio Profile (BAP) broadcast source role is enabled
PRODUCT_PROPERTY_OVERRIDES += bluetooth.profile.bap.broadcast.source.enabled=true
#Basic Audio Profile (BAP) unicast client role is enabled
PRODUCT_PROPERTY_OVERRIDES += bluetooth.profile.bap.unicast.client.enabled=true
#Volume Control Profile (VCP) controller role is enabled
PRODUCT_PROPERTY_OVERRIDES += bluetooth.profile.vcp.controller.enabled=true
#Coordinated Set Identification Profile (CSIP) is enabled
PRODUCT_PROPERTY_OVERRIDES += bluetooth.profile.csip.set_coordinator.enabled=true
#Media Control Profile (MCP) is enabled
PRODUCT_PROPERTY_OVERRIDES += bluetooth.profile.mcp.server.enabled=true
#Call Control Profile (CCP) is enabled
PRODUCT_PROPERTY_OVERRIDES += bluetooth.profile.ccp.server.enabled=true
#Hearing Aid Profile (HAP) is enabled
PRODUCT_PROPERTY_OVERRIDES += bluetooth.profile.hap.client.enabled=true
endif

ifeq ($(TARGET_BOARD_PLATFORM), niobe) # Matrix specific defines
#a2dp offload capability
PRODUCT_PROPERTY_OVERRIDES += persist.vendor.qcom.bluetooth.a2dp_offload_cap=sbc-aptx-aptxtws-aptxhd-aac-ldac-aptxadaptiver2
#aac frame control support
PRODUCT_PROPERTY_OVERRIDES += persist.vendor.qcom.bluetooth.aac_frm_ctl.enabled=true
#TWS+ state support
PRODUCT_PROPERTY_OVERRIDES += persist.vendor.qcom.bluetooth.twsp_state.enabled=false
#A2dp Multicast support
PRODUCT_PROPERTY_OVERRIDES += persist.vendor.qcom.bluetooth.a2dp_mcast_test.enabled=false
#Scrambling support
PRODUCT_PROPERTY_OVERRIDES += persist.vendor.qcom.bluetooth.scram.enabled=false
#AAC VBR support
PRODUCT_PROPERTY_OVERRIDES += persist.vendor.qcom.bluetooth.aac_vbr_ctl.enabled=true
#AptX Adaptive R2.1 support
PRODUCT_PROPERTY_OVERRIDES += persist.vendor.qcom.bluetooth.aptxadaptiver2_1_support=true
#HearingAid support
PRODUCT_PROPERTY_OVERRIDES += persist.sys.fflag.override.settings_bluetooth_hearing_aid=true
#Basic Audio Profile (BAP) broadcast assist role is enabled
PRODUCT_PROPERTY_OVERRIDES += bluetooth.profile.bap.broadcast.assist.enabled=true
#Basic Audio Profile (BAP) broadcast source role is enabled
PRODUCT_PROPERTY_OVERRIDES += bluetooth.profile.bap.broadcast.source.enabled=true
#Basic Audio Profile (BAP) unicast client role is enabled
PRODUCT_PROPERTY_OVERRIDES += bluetooth.profile.bap.unicast.client.enabled=true
#Volume Control Profile (VCP) controller role is enabled
PRODUCT_PROPERTY_OVERRIDES += bluetooth.profile.vcp.controller.enabled=true
#Coordinated Set Identification Profile (CSIP) is enabled
PRODUCT_PROPERTY_OVERRIDES += bluetooth.profile.csip.set_coordinator.enabled=true
#Media Control Profile (MCP) is enabled
PRODUCT_PROPERTY_OVERRIDES += bluetooth.profile.mcp.server.enabled=true
#Call Control Profile (CCP) is enabled
PRODUCT_PROPERTY_OVERRIDES += bluetooth.profile.ccp.server.enabled=true
endif

ifeq ($(TARGET_BOARD_PLATFORM), seraph) # Balsam specific defines
#a2dp offload capability
PRODUCT_PROPERTY_OVERRIDES += persist.vendor.qcom.bluetooth.a2dp_offload_cap=sbc-aptx-aptxtws-aptxhd-aac-ldac-aptxadaptiver2
#aac frame control support
PRODUCT_PROPERTY_OVERRIDES += persist.vendor.qcom.bluetooth.aac_frm_ctl.enabled=true
#TWS+ state support
PRODUCT_PROPERTY_OVERRIDES += persist.vendor.qcom.bluetooth.twsp_state.enabled=false
#A2dp Multicast support
PRODUCT_PROPERTY_OVERRIDES += persist.vendor.qcom.bluetooth.a2dp_mcast_test.enabled=false
#Scrambling support
PRODUCT_PROPERTY_OVERRIDES += persist.vendor.qcom.bluetooth.scram.enabled=false
#AAC VBR support
PRODUCT_PROPERTY_OVERRIDES += persist.vendor.qcom.bluetooth.aac_vbr_ctl.enabled=true
#AptX Adaptive R2.1 support
PRODUCT_PROPERTY_OVERRIDES += persist.vendor.qcom.bluetooth.aptxadaptiver2_1_support=true
#HearingAid support
PRODUCT_PROPERTY_OVERRIDES += persist.sys.fflag.override.settings_bluetooth_hearing_aid=true
#Basic Audio Profile (BAP) broadcast assist role is enabled
PRODUCT_PROPERTY_OVERRIDES += bluetooth.profile.bap.broadcast.assist.enabled=true
#Basic Audio Profile (BAP) broadcast source role is enabled
PRODUCT_PROPERTY_OVERRIDES += bluetooth.profile.bap.broadcast.source.enabled=true
#Basic Audio Profile (BAP) unicast client role is enabled
PRODUCT_PROPERTY_OVERRIDES += bluetooth.profile.bap.unicast.client.enabled=true
#Volume Control Profile (VCP) controller role is enabled
PRODUCT_PROPERTY_OVERRIDES += bluetooth.profile.vcp.controller.enabled=true
#Coordinated Set Identification Profile (CSIP) is enabled
PRODUCT_PROPERTY_OVERRIDES += bluetooth.profile.csip.set_coordinator.enabled=true
#Media Control Profile (MCP) is enabled
PRODUCT_PROPERTY_OVERRIDES += bluetooth.profile.mcp.server.enabled=true
#Call Control Profile (CCP) is enabled
PRODUCT_PROPERTY_OVERRIDES += bluetooth.profile.ccp.server.enabled=true
endif

ifeq ($(TARGET_BOARD_PLATFORM), parrot) # Netrani specific defines
#a2dp offload capability
PRODUCT_PROPERTY_OVERRIDES += persist.vendor.qcom.bluetooth.a2dp_offload_cap=sbc-aptx-aptxtws-aptxhd-aac-ldac-aptxadaptiver2
#aac frame control support
PRODUCT_PROPERTY_OVERRIDES += persist.vendor.qcom.bluetooth.aac_frm_ctl.enabled=true
#TWS+ state support
PRODUCT_PROPERTY_OVERRIDES += persist.vendor.qcom.bluetooth.twsp_state.enabled=false
#A2dp Multicast support
PRODUCT_PROPERTY_OVERRIDES += persist.vendor.qcom.bluetooth.a2dp_mcast_test.enabled=false
#Scrambling support
PRODUCT_PROPERTY_OVERRIDES += persist.vendor.qcom.bluetooth.scram.enabled=false
#AAC VBR support
PRODUCT_PROPERTY_OVERRIDES += persist.vendor.qcom.bluetooth.aac_vbr_ctl.enabled=true
#AptX Adaptive R2.1 support
PRODUCT_PROPERTY_OVERRIDES += persist.vendor.qcom.bluetooth.aptxadaptiver2_1_support=true
#HearingAid support
PRODUCT_PROPERTY_OVERRIDES += persist.sys.fflag.override.settings_bluetooth_hearing_aid=true
#QC CSIP enable
PRODUCT_PROPERTY_OVERRIDES += ro.vendor.bluetooth.csip_qti=true
#ASHA central support
PRODUCT_PROPERTY_OVERRIDES += bluetooth.profile.asha.central.enabled=false
endif

ifeq ($(TARGET_BOARD_PLATFORM), trinket) # trinket specific defines
#Bluetooth SOC type
PRODUCT_PROPERTY_OVERRIDES += persist.vendor.qcom.bluetooth.soc=cherokee
#split a2dp support
PRODUCT_PROPERTY_OVERRIDES += persist.vendor.qcom.bluetooth.enable.splita2dp=true
#a2dp offload capability
PRODUCT_PROPERTY_OVERRIDES += persist.vendor.qcom.bluetooth.a2dp_offload_cap=sbc-aptx-aptxtws-aptxhd-aac-ldac
#Embedded wipower mode
PRODUCT_PROPERTY_OVERRIDES += ro.vendor.bluetooth.wipower=false
#TWS+ state support
PRODUCT_PROPERTY_OVERRIDES += persist.vendor.qcom.bluetooth.twsp_state.enabled=false
#Scrambling support
PRODUCT_PROPERTY_OVERRIDES += persist.vendor.qcom.bluetooth.scram.enabled=true
#AAC VBR support
PRODUCT_PROPERTY_OVERRIDES += persist.vendor.qcom.bluetooth.aac_vbr_ctl.enabled=false
endif

ifeq ($(TARGET_BOARD_PLATFORM), lito) # lito specific defines
#Bluetooth SOC type
PRODUCT_PROPERTY_OVERRIDES += persist.vendor.qcom.bluetooth.soc=cherokee
#split a2dp support
PRODUCT_PROPERTY_OVERRIDES += persist.vendor.qcom.bluetooth.enable.splita2dp=true
#a2dp offload capability
PRODUCT_PROPERTY_OVERRIDES += persist.vendor.qcom.bluetooth.a2dp_offload_cap=sbc-aptx-aptxtws-aptxhd-aac-ldac-aptxadaptiver2
#Embedded wipower mode
PRODUCT_PROPERTY_OVERRIDES += ro.vendor.bluetooth.wipower=false
#TWS+ state support
PRODUCT_PROPERTY_OVERRIDES += persist.vendor.qcom.bluetooth.twsp_state.enabled=false
#Scrambling support
PRODUCT_PROPERTY_OVERRIDES += persist.vendor.qcom.bluetooth.scram.enabled=false
#AAC VBR support
PRODUCT_PROPERTY_OVERRIDES += persist.vendor.qcom.bluetooth.aac_vbr_ctl.enabled=true
#AptX Adaptive R2.1 support
PRODUCT_PROPERTY_OVERRIDES += persist.vendor.qcom.bluetooth.aptxadaptiver2_1_support=false
#HearingAid support
PRODUCT_PROPERTY_OVERRIDES += persist.sys.fflag.override.settings_bluetooth_hearing_aid=true
endif

ifeq ($(TARGET_BOARD_PLATFORM), bengal) # bengal specific defines
#Bluetooth SOC type
PRODUCT_PROPERTY_OVERRIDES += persist.vendor.qcom.bluetooth.soc=cherokee
#split a2dp support
PRODUCT_PROPERTY_OVERRIDES += persist.vendor.qcom.bluetooth.enable.splita2dp=true
#a2dp offload capability
PRODUCT_PROPERTY_OVERRIDES += persist.vendor.qcom.bluetooth.a2dp_offload_cap=sbc-aptx-aptxtws-aptxhd-aac-ldac
#Embedded wipower mode
PRODUCT_PROPERTY_OVERRIDES += ro.vendor.bluetooth.wipower=false
#TWS+ state support
PRODUCT_PROPERTY_OVERRIDES += persist.vendor.qcom.bluetooth.twsp_state.enabled=false
#Scrambling support
PRODUCT_PROPERTY_OVERRIDES += persist.vendor.qcom.bluetooth.scram.enabled=false
#AAC VBR support
PRODUCT_PROPERTY_OVERRIDES += persist.vendor.qcom.bluetooth.aac_vbr_ctl.enabled=false
#ASHA central support
PRODUCT_PROPERTY_OVERRIDES += bluetooth.profile.asha.central.enabled=false
endif

ifeq ($(TARGET_BOARD_PLATFORM), holi) # holi specific defines
#split a2dp support
PRODUCT_PROPERTY_OVERRIDES += persist.vendor.qcom.bluetooth.enable.splita2dp=true
#a2dp offload capability
PRODUCT_PROPERTY_OVERRIDES += persist.vendor.qcom.bluetooth.a2dp_offload_cap=sbc-aptx-aptxtws-aptxhd-aac-ldac
#Embedded wipower mode
PRODUCT_PROPERTY_OVERRIDES += ro.vendor.bluetooth.wipower=false
#TWS+ state support
PRODUCT_PROPERTY_OVERRIDES += persist.vendor.qcom.bluetooth.twsp_state.enabled=false
#Scrambling support
PRODUCT_PROPERTY_OVERRIDES += persist.vendor.qcom.bluetooth.scram.enabled=false
#AAC VBR support
PRODUCT_PROPERTY_OVERRIDES += persist.vendor.qcom.bluetooth.aac_vbr_ctl.enabled=false
#ASHA central support
PRODUCT_PROPERTY_OVERRIDES += bluetooth.profile.asha.central.enabled=false
endif

ifeq ($(TARGET_BOARD_PLATFORM), blair) # blair specific defines
#split a2dp support
PRODUCT_PROPERTY_OVERRIDES += persist.vendor.qcom.bluetooth.enable.splita2dp=true
#a2dp offload capability
PRODUCT_PROPERTY_OVERRIDES += persist.vendor.qcom.bluetooth.a2dp_offload_cap=sbc-aptx-aptxtws-aptxhd-aac-ldac
#Embedded wipower mode
PRODUCT_PROPERTY_OVERRIDES += ro.vendor.bluetooth.wipower=false
#TWS+ state support
PRODUCT_PROPERTY_OVERRIDES += persist.vendor.qcom.bluetooth.twsp_state.enabled=false
#Scrambling support
PRODUCT_PROPERTY_OVERRIDES += persist.vendor.qcom.bluetooth.scram.enabled=false
#AAC VBR support
PRODUCT_PROPERTY_OVERRIDES += persist.vendor.qcom.bluetooth.aac_vbr_ctl.enabled=false
#ASHA central support
PRODUCT_PROPERTY_OVERRIDES += bluetooth.profile.asha.central.enabled=false
endif

ifeq ($(TARGET_BOARD_PLATFORM), atoll) # atoll specific defines
#Bluetooth SOC type
PRODUCT_PROPERTY_OVERRIDES += persist.vendor.qcom.bluetooth.soc=cherokee
#split a2dp support
PRODUCT_PROPERTY_OVERRIDES += persist.vendor.qcom.bluetooth.enable.splita2dp=true
#a2dp offload capability
PRODUCT_PROPERTY_OVERRIDES += persist.vendor.qcom.bluetooth.a2dp_offload_cap=sbc-aptx-aptxtws-aptxhd-aac-ldac-aptxadaptive
#Embedded wipower mode
PRODUCT_PROPERTY_OVERRIDES += ro.vendor.bluetooth.wipower=false
#aac frame control support
PRODUCT_PROPERTY_OVERRIDES += persist.vendor.qcom.bluetooth.aac_frm_ctl.enabled=true
#TWS+ state support
PRODUCT_PROPERTY_OVERRIDES += persist.vendor.qcom.bluetooth.twsp_state.enabled=false
#Scrambling support
PRODUCT_PROPERTY_OVERRIDES += persist.vendor.qcom.bluetooth.scram.enabled=false
#AAC VBR support
PRODUCT_PROPERTY_OVERRIDES += persist.vendor.qcom.bluetooth.aac_vbr_ctl.enabled=false
endif

ifeq ($(TARGET_BOARD_PLATFORM), sdm660)  # sdm660
#Bluetooth SOC type
PRODUCT_PROPERTY_OVERRIDES += persist.vendor.qcom.bluetooth.soc=cherokee
#split a2dp support
PRODUCT_PROPERTY_OVERRIDES += persist.vendor.qcom.bluetooth.enable.splita2dp=true
#a2dp offload capability
PRODUCT_PROPERTY_OVERRIDES += persist.vendor.qcom.bluetooth.a2dp_offload_cap=sbc-aptx-aptxhd-aac-ldac
#Embedded wipower mode
PRODUCT_PROPERTY_OVERRIDES += ro.vendor.bluetooth.wipower=false
#Scrambling support
PRODUCT_PROPERTY_OVERRIDES += persist.vendor.qcom.bluetooth.scram.enabled=false
endif

ifeq ($(TARGET_BOARD_PLATFORM), msm8998) # MSM8998 specific defines
#Bluetooth SOC type
PRODUCT_PROPERTY_OVERRIDES += persist.vendor.qcom.bluetooth.soc=cherokee
#split a2dp support
PRODUCT_PROPERTY_OVERRIDES += persist.vendor.qcom.bluetooth.enable.splita2dp=true
#a2dp offload capability
PRODUCT_PROPERTY_OVERRIDES += persist.vendor.qcom.bluetooth.a2dp_offload_cap=sbc-aptx-aptxhd-aac-ldac
#Embedded wipower mode
PRODUCT_PROPERTY_OVERRIDES += ro.vendor.bluetooth.wipower=false
#Scrambling support
PRODUCT_PROPERTY_OVERRIDES += persist.vendor.qcom.bluetooth.scram.enabled=true
endif

ifeq ($(TARGET_BOARD_PLATFORM), msm8996) # MSM8996 specific defines
#Bluetooth SOC type
PRODUCT_PROPERTY_OVERRIDES += persist.vendor.qcom.bluetooth.soc=rome
#split a2dp support
PRODUCT_PROPERTY_OVERRIDES += persist.vendor.qcom.bluetooth.enable.splita2dp=false
# Set this true as ROME which is programmed
# as embedded wipower mode by default
PRODUCT_PROPERTY_OVERRIDES += ro.vendor.bluetooth.wipower=false
endif

ifeq ($(TARGET_BOARD_PLATFORM), msm8937) # msm8937 specific defines
#Bluetooth SOC type
PRODUCT_PROPERTY_OVERRIDES += persist.vendor.qcom.bluetooth.soc=pronto
#split a2dp support
PRODUCT_PROPERTY_OVERRIDES += persist.vendor.qcom.bluetooth.enable.splita2dp=false
#Embedded wipower mode
PRODUCT_PROPERTY_OVERRIDES += ro.vendor.bluetooth.wipower=false
endif

ifeq ($(TARGET_BOARD_PLATFORM), msm8953) # MSM8953 specific defines
#Bluetooth SOC type
PRODUCT_PROPERTY_OVERRIDES += persist.vendor.qcom.bluetooth.soc=pronto
#split a2dp support
PRODUCT_PROPERTY_OVERRIDES += persist.vendor.qcom.bluetooth.enable.splita2dp=false
#Embedded wipower mode
PRODUCT_PROPERTY_OVERRIDES += ro.vendor.bluetooth.wipower=false
endif

ifeq ($(TARGET_BOARD_PLATFORM), msm8909) # MSM8909 specific defines
#Bluetooth SOC type
PRODUCT_PROPERTY_OVERRIDES += persist.vendor.qcom.bluetooth.soc=pronto
#split a2dp support
PRODUCT_PROPERTY_OVERRIDES += persist.vendor.qcom.bluetooth.enable.splita2dp=false
#Embedded wipower mode
PRODUCT_PROPERTY_OVERRIDES += ro.vendor.bluetooth.wipower=false
endif

ifeq ($(TARGET_BOARD_PLATFORM), msm8952) # MSM8952 specific defines
#Bluetooth SOC type
PRODUCT_PROPERTY_OVERRIDES += persist.vendor.qcom.bluetooth.soc=pronto
#split a2dp support
PRODUCT_PROPERTY_OVERRIDES += persist.vendor.qcom.bluetooth.enable.splita2dp=false
#Embedded wipower mode
PRODUCT_PROPERTY_OVERRIDES += ro.vendor.bluetooth.wipower=false
endif

ifeq ($(TARGET_BOARD_PLATFORM), crow) # camano specific defines
#a2dp offload capability
PRODUCT_PROPERTY_OVERRIDES += persist.vendor.qcom.bluetooth.a2dp_offload_cap=sbc-aptx-aptxtws-aptxhd-aac-ldac-aptxadaptiver2
#aac frame control support
PRODUCT_PROPERTY_OVERRIDES += persist.vendor.qcom.bluetooth.aac_frm_ctl.enabled=true
#TWS+ state support
PRODUCT_PROPERTY_OVERRIDES += persist.vendor.qcom.bluetooth.twsp_state.enabled=false
#A2dp Multicast support
PRODUCT_PROPERTY_OVERRIDES += persist.vendor.qcom.bluetooth.a2dp_mcast_test.enabled=false
#Scrambling support
PRODUCT_PROPERTY_OVERRIDES += persist.vendor.qcom.bluetooth.scram.enabled=false
#AAC VBR support
PRODUCT_PROPERTY_OVERRIDES += persist.vendor.qcom.bluetooth.aac_vbr_ctl.enabled=true
#AptX Adaptive R2.1 support
PRODUCT_PROPERTY_OVERRIDES += persist.vendor.qcom.bluetooth.aptxadaptiver2_1_support=true
#HearingAid support
PRODUCT_PROPERTY_OVERRIDES += persist.sys.fflag.override.settings_bluetooth_hearing_aid=true
#Basic Audio Profile (BAP) broadcast assist role is enabled
PRODUCT_PROPERTY_OVERRIDES += bluetooth.profile.bap.broadcast.assist.enabled=true
#Basic Audio Profile (BAP) broadcast source role is enabled
PRODUCT_PROPERTY_OVERRIDES += bluetooth.profile.bap.broadcast.source.enabled=true
#Basic Audio Profile (BAP) unicast client role is enabled
PRODUCT_PROPERTY_OVERRIDES += bluetooth.profile.bap.unicast.client.enabled=true
#Volume Control Profile (VCP) controller role is enabled
PRODUCT_PROPERTY_OVERRIDES += bluetooth.profile.vcp.controller.enabled=true
#Coordinated Set Identification Profile (CSIP) is enabled
PRODUCT_PROPERTY_OVERRIDES += bluetooth.profile.csip.set_coordinator.enabled=true
#Media Control Profile (MCP) is enabled
PRODUCT_PROPERTY_OVERRIDES += bluetooth.profile.mcp.server.enabled=true
#Call Control Profile (CCP) is enabled
PRODUCT_PROPERTY_OVERRIDES += bluetooth.profile.ccp.server.enabled=true
endif

ifeq ($(TARGET_BOARD_PLATFORM), pitti) # Kalpeni specific defines
#Bluetooth SOC type
PRODUCT_PROPERTY_OVERRIDES += persist.vendor.qcom.bluetooth.soc=cherokee
#split a2dp support
PRODUCT_PROPERTY_OVERRIDES += persist.vendor.qcom.bluetooth.enable.splita2dp=true
#a2dp offload capability
PRODUCT_PROPERTY_OVERRIDES += persist.vendor.qcom.bluetooth.a2dp_offload_cap=sbc-aptx-aptxtws-aptxhd-aac-ldac
#Embedded wipower mode
PRODUCT_PROPERTY_OVERRIDES += ro.vendor.bluetooth.wipower=false
#TWS+ state support
PRODUCT_PROPERTY_OVERRIDES += persist.vendor.qcom.bluetooth.twsp_state.enabled=false
#Scrambling support
PRODUCT_PROPERTY_OVERRIDES += persist.vendor.qcom.bluetooth.scram.enabled=false
#AAC VBR support
PRODUCT_PROPERTY_OVERRIDES += persist.vendor.qcom.bluetooth.aac_vbr_ctl.enabled=true
#ASHA central support
PRODUCT_PROPERTY_OVERRIDES += bluetooth.profile.asha.central.enabled=false
endif

ifeq ($(TARGET_BOARD_PLATFORM), volcano) # milos specific defines
#a2dp offload capability
PRODUCT_PROPERTY_OVERRIDES += persist.vendor.qcom.bluetooth.a2dp_offload_cap=sbc-aptx-aptxtws-aptxhd-aac-ldac-aptxadaptiver2
#aac frame control support
PRODUCT_PROPERTY_OVERRIDES += persist.vendor.qcom.bluetooth.aac_frm_ctl.enabled=true
#TWS+ state support
PRODUCT_PROPERTY_OVERRIDES += persist.vendor.qcom.bluetooth.twsp_state.enabled=false
#A2dp Multicast support
PRODUCT_PROPERTY_OVERRIDES += persist.vendor.qcom.bluetooth.a2dp_mcast_test.enabled=false
#Scrambling support
PRODUCT_PROPERTY_OVERRIDES += persist.vendor.qcom.bluetooth.scram.enabled=false
#AAC VBR support
PRODUCT_PROPERTY_OVERRIDES += persist.vendor.qcom.bluetooth.aac_vbr_ctl.enabled=true
#AptX Adaptive R2.1 support
PRODUCT_PROPERTY_OVERRIDES += persist.vendor.qcom.bluetooth.aptxadaptiver2_1_support=true
#HearingAid support
PRODUCT_PROPERTY_OVERRIDES += persist.sys.fflag.override.settings_bluetooth_hearing_aid=true
#Lossless over Aptx Adaptive LE support
PRODUCT_PROPERTY_OVERRIDES += persist.vendor.qcom.bluetooth.lossless_aptx_adaptive_le.enabled=true
#Basic Audio Profile (BAP) broadcast assist role is enabled
PRODUCT_PROPERTY_OVERRIDES += bluetooth.profile.bap.broadcast.assist.enabled=true
#Basic Audio Profile (BAP) broadcast source role is enabled
PRODUCT_PROPERTY_OVERRIDES += bluetooth.profile.bap.broadcast.source.enabled=true
#Basic Audio Profile (BAP) unicast client role is enabled
PRODUCT_PROPERTY_OVERRIDES += bluetooth.profile.bap.unicast.client.enabled=true
#Volume Control Profile (VCP) controller role is enabled
PRODUCT_PROPERTY_OVERRIDES += bluetooth.profile.vcp.controller.enabled=true
#Coordinated Set Identification Profile (CSIP) is enabled
PRODUCT_PROPERTY_OVERRIDES += bluetooth.profile.csip.set_coordinator.enabled=true
#Media Control Profile (MCP) is enabled
PRODUCT_PROPERTY_OVERRIDES += bluetooth.profile.mcp.server.enabled=true
#Media Control Profile (CCP) is enabled
PRODUCT_PROPERTY_OVERRIDES += bluetooth.profile.ccp.server.enabled=true
#Hearing Aid Profile (HAP) is enabled
PRODUCT_PROPERTY_OVERRIDES += bluetooth.profile.hap.client.enabled=true
#DUAL Mode Transport support
PRODUCT_PROPERTY_OVERRIDES += persist.vendor.qcom.bluetooth.dualmode_transport_support=true
endif

ifeq ($(TARGET_BOARD_PLATFORM), neo61) # NEO specific defines
PRODUCT_PACKAGES += libbthost_if_sink
PRODUCT_PROPERTY_OVERRIDES += persist.vendor.bluetooth.hfp_client=true
PRODUCT_PROPERTY_OVERRIDES += persist.vendor.bluetooth.split_a2dp_sink=true
endif