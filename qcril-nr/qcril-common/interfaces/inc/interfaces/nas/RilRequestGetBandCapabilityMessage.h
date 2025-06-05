/******************************************************************************
#  Copyright (c) 2022 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#pragma once
#include "framework/GenericCallback.h"
#include "framework/SolicitedMessage.h"
#include "framework/Message.h"
#include "framework/add_message_id.h"
#include "framework/message_translator.h"
#include "framework/legacy.h"
#include "interfaces/QcRilRequestMessage.h"

#define QMI_RIL_MAX_SUPPORTED_LTE_BANDS    (256)
#define QMI_RIL_MAX_SUPPORTED_NR5G_BANDS   (512)

struct DefaultBandCapabilities_t : public qcril::interfaces::BasePayload {
    uint8_t dms_band_capability_valid;
    uint64_t dms_band_capability;

    uint8_t supported_lte_bands_valid;
    uint32_t supported_lte_bands_len;
    std::array<uint16_t, QMI_RIL_MAX_SUPPORTED_LTE_BANDS> supported_lte_bands{};

    uint8_t supported_nr5g_bands_valid;
    uint32_t supported_nr5g_bands_len;
    std::array<uint16_t, QMI_RIL_MAX_SUPPORTED_NR5G_BANDS> supported_nr5g_bands{};
};


class RilRequestGetBandCapabilityMessage : public SolicitedMessage<QcRilRequestMessageCallbackPayload>,
    public add_message_id<RilRequestGetBandCapabilityMessage> {

    public:
      static constexpr const char *MESSAGE_NAME = "com.qualcomm.qti.qcril.nas.GetBandCapability";
      ~RilRequestGetBandCapabilityMessage() {}

      RilRequestGetBandCapabilityMessage():
                          SolicitedMessage<QcRilRequestMessageCallbackPayload>(get_class_message_id())
      {
        mName = MESSAGE_NAME;
      }

      inline string dump() {
        return mName;
      }
};
