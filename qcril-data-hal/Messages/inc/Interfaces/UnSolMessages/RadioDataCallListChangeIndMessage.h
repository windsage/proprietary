/*===========================================================================

  Copyright (c) 2018, 2020 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

===========================================================================*/
#ifndef RADIODATACALLLISTCHANGEINDMESSAGE
#define RADIODATACALLLISTCHANGEINDMESSAGE

#include "MessageCommon.h"

namespace rildata {

class RadioDataCallListChangeIndMessage : public UnSolicitedMessage,
                       public add_message_id<RadioDataCallListChangeIndMessage> {
  protected:
    std::vector<DataCallResult_t> mDCList;

  public:
    static constexpr const char *MESSAGE_NAME = "RadioDataCallListChangeIndMessage";
    inline RadioDataCallListChangeIndMessage(const std::vector<DataCallResult_t>& dcList):
     UnSolicitedMessage(get_class_message_id())
    {
        mName = MESSAGE_NAME;
        mDCList = dcList;
    }
    ~RadioDataCallListChangeIndMessage() {};

    string dump() {
      std::stringstream ss;
      ss << mName << " {";
      for (const DataCallResult_t& call : mDCList) {
        call.dump("", ss);
        ss << ",";
      }
      ss << "}";
      return ss.str();
    };

    std::vector<DataCallResult_t> getDCList() {return mDCList;};
    std::shared_ptr<UnSolicitedMessage> clone() {
      return std::make_shared<RadioDataCallListChangeIndMessage>(mDCList);
    }
};
}

#endif
