/******************************************************************************
#  Copyright (c) 2022 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#pragma once
#include <framework/GenericCallback.h>
#include <framework/SolicitedMessage.h>
#include <framework/Message.h>
#include <framework/add_message_id.h>
#include "nas_types.h"
/******************************************************************************
#   Message Class: NasSetSignalStrengthCriteriaAllRans
#   Sender: ConfigurationClients
#   Receiver: SignalStrengthCriteriaModule
#   Data transferred: A list of SignalStrengthCriteriaEntry
#   Usage: Client sends this message when wanting to set the reporting criteria for the signal strength for all RANs and its signal measurement types
#******************************************************************************/

class NasSetSignalStrengthCriteriaAllRans : public SolicitedMessage<RIL_Errno>,
    public add_message_id<NasSetSignalStrengthCriteriaAllRans>
{
private:
    std::vector<qcril::interfaces::SignalStrengthCriteriaEntry> mCriteria;

public:
    static constexpr const char *MESSAGE_NAME = "com.qualcomm.qti.qcril.nas.set_signal_strength_criteria_all_rans";
    static constexpr int RSSI_DELTA = 50;
    static constexpr int RSCP_DELTA = 40;
    static constexpr int RSRP_DELTA = 60;
    static constexpr int RSRQ_DELTA = 20;
    static constexpr int RSSNR_DELTA = 40;

    NasSetSignalStrengthCriteriaAllRans() = delete;
    inline ~NasSetSignalStrengthCriteriaAllRans() {};

    inline NasSetSignalStrengthCriteriaAllRans(std::weak_ptr<MessageContext> ctx, std::vector<qcril::interfaces::SignalStrengthCriteriaEntry> criteria):
        SolicitedMessage<RIL_Errno>(MESSAGE_NAME, ctx, get_class_message_id()),
        mCriteria(criteria)
    {
    }

    std::vector<qcril::interfaces::SignalStrengthCriteriaEntry> getCriteria() { return mCriteria;}

    string dump()
    {
      string dump = mName + "{";
      for (const qcril::interfaces::SignalStrengthCriteriaEntry &entry: mCriteria) {
          dump += "{ ran: " + qcril::interfaces::toString(entry.ran);
          dump += " hysteresis db: " + std::to_string(entry.hysteresisDb);
          dump += " thresholds: { ";
          for (int32_t th: entry.thresholds) {
              dump += std::to_string(th) + ", ";
          }
          dump += "}";
      }
      return dump;
    }
};

