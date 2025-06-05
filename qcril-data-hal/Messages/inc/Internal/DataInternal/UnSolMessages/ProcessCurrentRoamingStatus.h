/******************************************************************************
#  Copyright (c) 2023 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#ifndef PROCESSCUREENTROAMINGSTATUS
#define PROCESSCUREENTROAMINGSTATUS

#include <framework/legacy.h>
#include <framework/UnSolicitedMessage.h>
#include <framework/add_message_id.h>

namespace rildata {

class ProcessCurrentRoamingStatus: public UnSolicitedMessage, public add_message_id<ProcessCurrentRoamingStatus>
{
    private:
      std::shared_ptr<dsd_roaming_status_change_ind_msg_v01> mInd;

    public:
      static constexpr const char *MESSAGE_NAME = "ProcessCurrentRoamingStatus";
      ProcessCurrentRoamingStatus(dsd_roaming_status_change_ind_msg_v01 ind) :
          UnSolicitedMessage(get_class_message_id()) {
            mName = MESSAGE_NAME;
            mInd = std::make_shared<dsd_roaming_status_change_ind_msg_v01>(ind);
      }
      ~ProcessCurrentRoamingStatus() {}

      std::shared_ptr<UnSolicitedMessage> clone() {
        return std::make_shared<ProcessCurrentRoamingStatus>(*(mInd.get()));
      }

      string dump() {
        return MESSAGE_NAME;
      }

      dsd_roaming_status_change_ind_msg_v01* getData() {
        return (mInd.get());
      }
};
}
#endif
