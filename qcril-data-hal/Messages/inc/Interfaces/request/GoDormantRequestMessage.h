/*===========================================================================

  Copyright (c) 2020 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

===========================================================================*/

#ifndef GODORMANTREQUESTMESSAGE
#define GODORMANTREQUESTMESSAGE

namespace rildata {

class GoDormantRequestMessage : public SolicitedMessage<RIL_Errno>,
                          public add_message_id<GoDormantRequestMessage> {
    private:
        std::string devName;
    public:
        static constexpr const char *MESSAGE_NAME = "GoDormantRequestMessage";
        GoDormantRequestMessage(std::string setdevName)
        :SolicitedMessage<RIL_Errno>(get_class_message_id()) {
            mName = MESSAGE_NAME;
            devName = setdevName;
        }
        ~GoDormantRequestMessage(){}
        std::string getParams() { return devName; }
        std::string dump() { return mName; }
};

}

#endif