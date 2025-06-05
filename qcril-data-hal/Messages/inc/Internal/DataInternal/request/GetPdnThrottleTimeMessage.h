/**
 * Copyright (c) 2021 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 **/
#include <variant>

#include "framework/SolicitedMessage.h"
#include "framework/add_message_id.h"

namespace rildata {

class GetPdnThrottleTimeMessage : public SolicitedMessage<int64_t>,
                                  public add_message_id<GetPdnThrottleTimeMessage> {
  public:
    static constexpr const char *MESSAGE_NAME = "GetPdnThrottleTimeMessage";
    GetPdnThrottleTimeMessage() = delete;
    GetPdnThrottleTimeMessage(std::string apn, DataProfileInfoType_t techType, std::string ipType):
      SolicitedMessage<int64_t>(get_class_message_id()), apn(apn), techType(techType), ipType(ipType) {
      mName = MESSAGE_NAME;
    }
    ~GetPdnThrottleTimeMessage() {}

    std::string dump() {
        std::stringstream ss;
        ss << mName << " {";
        ss << "apn: " << apn;
        ss << "tech: " << techType;
        ss << "ipType: " << ipType;
        ss << "}";
        return ss.str();
    }
    std::string getApn() {
      return apn;
    }
    DataProfileInfoType_t getTechType() {
      return techType;
    }
    std::string getIpType() {
      return ipType;
    }
  private:
    std::string apn;
    DataProfileInfoType_t techType;
    std::string ipType;
};

}
