ifeq ($(strip $(TARGET_USES_FACE)),true)
#Face Build Rules
PRODUCT_PACKAGES += android.hardware.biometrics.face@1.0-service.face
PRODUCT_PACKAGES += android.hardware.biometrics.face@1.0-service.face.rc
endif
