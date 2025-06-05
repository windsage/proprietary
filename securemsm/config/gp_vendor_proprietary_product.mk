# Packages associated with Global Platform support
PRODUCT_PACKAGES += GPTEE_Sample_client
PRODUCT_PACKAGES += libGPTEE_vendor
PRODUCT_PACKAGES += libGPMTEEC_vendor

$(call add_soong_config_namespace,qmaa_override_gp_config)
ifeq ($(TARGET_USES_QMAA_OVERRIDE_GP),true)
$(call add_soong_config_var_value,qmaa_override_gp_config,qmaa_override_gp_vendor,true)
else
$(call add_soong_config_var_value,qmaa_override_gp_config,qmaa_override_gp_vendor,false)
endif

$(call add_soong_config_namespace,mteec_QTEE_config)
ifeq ($(TARGET_ENABLE_QTEECONNECTOR),true)
PRODUCT_PACKAGES += libGPQTEEC_vendor
PRODUCT_PACKAGES += libQTEEConnector_vendor
PRODUCT_PACKAGES += libQTEEConnector_listener
PRODUCT_PACKAGES += vendor.qti.hardware.qteeconnector@1.0
PRODUCT_PACKAGES += vendor.qti.hardware.qteeconnector@1.0-impl
PRODUCT_PACKAGES += vendor.qti.hardware.qteeconnector@1.0-service
PRODUCT_PACKAGES += vendor.qti.hardware.qteeconnector@1.0-service.rc
PRODUCT_PACKAGES += vendor.qti.hardware.qteeconnector@1.0_vendor
$(call add_soong_config_var_value,mteec_QTEE_config,mteec_QTEE_vendor,true)
else
$(call add_soong_config_var_value,mteec_QTEE_config,mteec_QTEE_vendor,false)
endif
