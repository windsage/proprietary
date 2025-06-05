THERMAL_ENGINE := libthermalclient
THERMAL_ENGINE += libthermalioctl
ifeq ($(TARGET_KERNEL_VERSION),$(filter $(TARGET_KERNEL_VERSION),5.4))
	THERMAL_ENGINE += thermal-engine
	THERMAL_ENGINE += init_thermal-engine.rc
else
	THERMAL_ENGINE += thermal-engine-v2
	THERMAL_ENGINE += init_thermal-engine-v2.rc
endif
THERMAL_ENGINE += thermal-engine.conf

PRODUCT_PACKAGES += $(THERMAL_ENGINE)
