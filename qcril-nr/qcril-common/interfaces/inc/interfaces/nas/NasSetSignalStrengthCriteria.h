/******************************************************************************
#  Copyright (c) 2018 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#pragma once
#include <framework/GenericCallback.h>
#include <framework/SolicitedMessage.h>
#include <framework/Message.h>
#include <framework/add_message_id.h>
#include <framework/legacy.h>
#include <telephony/ril.h>
#include "nas_types.h"
/******************************************************************************
#   Message Class: NasSetSignalStrengthCriteria
#   Sender: ConfigurationClients
#   Receiver: NasModule
#   Data transferred: A list of SignalStrengthCriteriaEntry
#   Usage: Client sends this message when wanting to set the reporting criteria
           for the signal strength reporting.
#******************************************************************************/

class NasSetSignalStrengthCriteria : public SolicitedMessage<RIL_Errno>,
    public add_message_id<NasSetSignalStrengthCriteria>
{
private:
    std::vector<qcril::interfaces::SignalStrengthCriteriaEntry> mCriteria;
    bool disableAllTypes{false};

public:
    static constexpr const char *MESSAGE_NAME = "com.qualcomm.qti.qcril.nas.set_signal_strength_criteria";
    static constexpr int RSSI_DELTA = 50;
    static constexpr int RSCP_DELTA = 40;
    static constexpr int RSRP_DELTA = 60;
    static constexpr int RSRQ_DELTA = 20;
    static constexpr int RSSNR_DELTA = 40;

    NasSetSignalStrengthCriteria() = delete;
    inline ~NasSetSignalStrengthCriteria() {};

    inline NasSetSignalStrengthCriteria(std::weak_ptr<MessageContext> ctx, std::vector<qcril::interfaces::SignalStrengthCriteriaEntry> criteria):
        SolicitedMessage<RIL_Errno>(MESSAGE_NAME, ctx, get_class_message_id()),
        mCriteria(criteria)
    {
    }

    std::vector<qcril::interfaces::SignalStrengthCriteriaEntry> getCriteria() { return mCriteria;}
    bool getDisableAllTypes() { return disableAllTypes;}
    void setDisableAllTypes() { disableAllTypes = true;}

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

