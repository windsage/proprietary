ifneq ($(TARGET_HAS_LOW_RAM),true)
    ifeq ($(TARGET_FWK_SUPPORTS_FULL_VALUEADDS),true)
        ifneq (,$(filter userdebug eng, $(TARGET_BUILD_VARIANT)))
            PRODUCT_PACKAGES += tracing_config.sh
            PRODUCT_PACKAGES += smart_trace.rc
        endif
    endif
endif

ifeq ($(filter $(TARGET_BOARD_PLATFORM), pineapple),$(TARGET_BOARD_PLATFORM))
PRODUCT_COPY_FILES += \
  vendor/qcom/proprietary/qrdplus/Extension/QVA/vendor.qti.qva.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/vendor.qti.qva.xml
PRODUCT_COPY_FILES += \
  vendor/qcom/proprietary/qrdplus/Extension/Whisper/libSnpeHtpV75Skel.so:$(TARGET_COPY_OUT_VENDOR)/etc/ASR_binary/Whisper/libSnpeHtpV75Skel.so
PRODUCT_COPY_FILES += \
  vendor/qcom/proprietary/qrdplus/Extension/Whisper/speech_float.eai:$(TARGET_COPY_OUT_VENDOR)/etc/ASR_binary/Whisper/speech_float.eai
endif

ifeq ($(filter $(TARGET_BOARD_PLATFORM), sun),$(TARGET_BOARD_PLATFORM))
PRODUCT_COPY_FILES += \
  vendor/qcom/proprietary/qrdplus/Extension/QVA/vendor.qti.qva.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/vendor.qti.qva.xml
PRODUCT_COPY_FILES += \
  vendor/qcom/proprietary/qrdplus/Extension/Whisper/libQnnHtpV79Skel.so:$(TARGET_COPY_OUT_VENDOR)/etc/ASR_binary/Whisper/libQnnHtpV79Skel.so
PRODUCT_COPY_FILES += \
  vendor/qcom/proprietary/qrdplus/Extension/Whisper/speech_float.eai:$(TARGET_COPY_OUT_VENDOR)/etc/ASR_binary/Whisper/speech_float.eai
endif