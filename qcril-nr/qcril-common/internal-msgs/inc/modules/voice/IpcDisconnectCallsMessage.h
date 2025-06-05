/*
 * Copyright (c) 2023 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#pragma once

class IpcDisconnectCallsMessage : public IPCMessage,
                                  public add_message_id<IpcDisconnectCallsMessage>
{
    public:
        static constexpr const char* MESSAGE_NAME = "IpcDisconnectCallsMessage";
    private:
        uint8_t mInstanceId;
    public:
        IpcDisconnectCallsMessage()
            : IPCMessage(get_class_message_id()),
              mInstanceId(QCRIL_DEFAULT_INSTANCE_ID)
        {
                mName = MESSAGE_NAME;
        }

        IpcDisconnectCallsMessage(uint8_t instanceId)
            : IPCMessage(get_class_message_id()), mInstanceId(instanceId) {
              mName = MESSAGE_NAME;
        }

      inline ~IpcDisconnectCallsMessage() {}

      std::shared_ptr<UnSolicitedMessage> clone() {
        auto ret = std::make_shared<IpcDisconnectCallsMessage>(mInstanceId);
        if (ret) {
            ret->setIsRemote(getIsRemote());
        }
        return ret;
      }

      void serialize(IPCOStream& os) {
        os << mInstanceId;
      }

      void deserialize(IPCIStream& is) {
        is >> mInstanceId;
      }

      string dump() {
        std::string os = mName;
        os += "{";
        os += " .isRemote=";
        os += (getIsRemote() ? "true" : "false");
        os += " .mInstanceId=" + std::to_string(mInstanceId);
        os += "}";
        return os;
      }
};
