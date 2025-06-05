ADSPRPC_VENDOR += libadsp_default_listener
ADSPRPC_VENDOR += libcdsp_default_listener
ADSPRPC_VENDOR += libssc_default_listener
ADSPRPC_VENDOR += libadsprpc
ADSPRPC_VENDOR += libmdsprpc
ADSPRPC_VENDOR += libsdsprpc
ADSPRPC_VENDOR += libcdsprpc
ADSPRPC_VENDOR += vendor.qti.hardware.dsp@1.0
ADSPRPC_VENDOR += vendor.qti.hardware.dsp@1.0.vendor
ADSPRPC_VENDOR += vendor.qti.hardware.dsp@1.0_vendor
ADSPRPC_VENDOR += com.qualcomm.qti.FASTRPCPRIVILEGE_vendor.xml
$(call add_soong_config_namespace,fastrpc_QMAA_config)
ifeq ($(TARGET_USES_QMAA),true)
$(call add_soong_config_var_value,fastrpc_QMAA_config,fastrpc_QMAA_vendor,enabled)
else
$(call add_soong_config_var_value,fastrpc_QMAA_config,fastrpc_QMAA_vendor,disabled)
endif
TARGET_AUDIOPD_DISABLED := sdm845
TARGET_AUDIOPD_DISABLED += lahaina
TARGET_AUDIOPD_DISABLED += holi
TARGET_AUDIOPD_DISABLED += msm8953
TARGET_AUDIOPD_DISABLED += msm8937
TARGET_AUDIOPD_DISABLED += msm8998
TARGET_AUDIOPD_DISABLED += taro
TARGET_AUDIOPD_DISABLED += kalama
TARGET_AUDIOPD_DISABLED += neo
TARGET_AUDIOPD_DISABLED += bengal
TARGET_AUDIOPD_DISABLED += pineapple
TARGET_AUDIOPD_DISABLED += cliffs
TARGET_AUDIOPD_DISABLED += pitti
TARGET_AUDIOPD_DISABLED += volcano
TARGET_AUDIOPD_DISABLED += anorak61
TARGET_AUDIOPD_DISABLED += neo61

TARGET_FASTRPC_SENSORSPD_ENABLED := sm6150
TARGET_FASTRPC_SENSORSPD_ENABLED += sdm710
TARGET_FASTRPC_SENSORSPD_ENABLED += trinket

TARGET_CDSPRPCD_DISABLED := msm8937
TARGET_CDSPRPCD_DISABLED += msm8953
TARGET_CDSPRPCD_DISABLED += msm8998
TARGET_CDSPRPCD_DISABLED += monaco
TARGET_CDSPRPCD_DISABLED += pitti

ifneq ($(TARGET_USES_QMAA), true)
ENABLE_FASTRPC_DAEMONS := true
endif

ifeq ($(TARGET_USES_QMAA_OVERRIDE_FASTRPC), true)
ENABLE_FASTRPC_DAEMONS := true
endif

ifeq ($(ENABLE_FASTRPC_DAEMONS),true)
ifneq ($(ENABLE_HYP), true)
ifeq ($(call is-board-platform-in-list, $(TARGET_FASTRPC_SENSORSPD_ENABLED)),true)
ADSPRPC_VENDOR += vendor.qti.adsprpc-sscrpc-service.rc
else ifeq ($(call is-board-platform-in-list, $(TARGET_AUDIOPD_DISABLED)),true)
ADSPRPC_VENDOR += vendor.qti.adsprpc-guestos-service.rc
else
#targets where sscrpcd is enabled
ADSPRPC_VENDOR += vendor.qti.adsprpc-service.rc
endif
ADSPRPC_VENDOR += adsprpcd
ifneq ($(call is-board-platform-in-list, $(TARGET_CDSPRPCD_DISABLED)),true)
ADSPRPC_VENDOR += cdsprpcd
ADSPRPC_VENDOR += libloadalgo_stub
ADSPRPC_VENDOR += loadalgo
ADSPRPC_VENDOR += libsysmon_cdsp_skel
ADSPRPC_VENDOR += vendor.qti.cdsprpc-service.rc
ifeq ($(call is-board-platform,volcano),true)
ADSPRPC_VENDOR += nsp_disabled.rc
endif
endif  #TARGET_CDSPRPCD_DISABLED

endif  #ENABLE_HYP

ifneq ($(TARGET_BOARD_AUTO), true)
ADSPRPC_VENDOR += dspservice
endif  #TARGET_BOARD_AUTO
endif  #ENABLE_FASTRPC_DAEMONS

ADSPRPC_VENDOR += vendor.qti.hardware.dsp.policy
ADSPRPC_VENDOR += vendor.qti.hardware.dsp@1.0-service.rc
ifeq ($(call is-vendor-board-platform,QCOM),true)
PRODUCT_PACKAGES += $(ADSPRPC_VENDOR)
endif

$(call add_soong_config_namespace,fastrpc_HYP_config)
ifeq ($(ENABLE_HYP),true)
$(call add_soong_config_var_value,fastrpc_HYP_config,fastrpc_HYP_vendor,enabled)
endif

$(call add_soong_config_namespace, target_auto_config)
ifeq ($(TARGET_BOARD_AUTO), true)
$(call add_soong_config_var_value,target_auto_config,target_auto_vendor,enabled)
endif

#ADSPRPC_DBG
ADSPRPC_DBG := fastrpc_tests_apps
ADSPRPC_DBG += fastrpc_tests_apps32
ADSPRPC_DBG += libreverse_rpc_tests_skel
ADSPRPC_DBG += libfastrpc_tests_skel
ADSPRPC_DBG += libhap_power_test_skel
ADSPRPC_DBG += libtest_dl_skel
ADSPRPC_DBG += libtest_dl_perf_skel
ADSPRPC_DBG += librpcperf_skel
ADSPRPC_DBG += libfastrpc_tests1_skel
ADSPRPC_DBG += libtest_dl1_skel
ADSPRPC_DBG += libtest_dl_perf1_skel
ADSPRPC_DBG += librpcperf1_skel
ADSPRPC_DBG += libunsigned_pd_test_skel
ADSPRPC_DBG += libbenchmark_skel
ADSPRPC_DBG += test_elf_size_16k
ADSPRPC_DBG += test_elf_size_32k
ADSPRPC_DBG += test_elf_size_64k
ADSPRPC_DBG += test_elf_size_128k
ADSPRPC_DBG += test_elf_size_256k
ADSPRPC_DBG += test_elf_size_512k
ADSPRPC_DBG += test_elf_size_768k
ADSPRPC_DBG += test_elf_size_1MB
ADSPRPC_DBG += test_elf_nop
ADSPRPC_DBG += test_elf_dep1
ADSPRPC_DBG += test_elf_dep2
ifeq ($(call is-vendor-board-platform,QCOM),true)
ifneq ($(TARGET_BOARD_AUTO), true)
PRODUCT_PACKAGES_DEBUG += $(ADSPRPC_DBG)
endif
endif
