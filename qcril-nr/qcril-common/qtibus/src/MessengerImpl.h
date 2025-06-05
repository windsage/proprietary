/*
 * Copyright (c) 2019 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#pragma once
#include <qtibus/Messenger.h>
#include <QtiMutex.h>
#include <unordered_map>
#include <functional>

enum class MessengerCommands {
    UNKNOWN,
    REGISTER_MESSAGE,
    DELIVER_MESSAGE,
    PING,
};

template <class S>
S &operator<<(S &o, const MessengerCommands cmd) {
    switch(cmd) {
        case MessengerCommands::REGISTER_MESSAGE:
            o << "MessengerCommands::REGISTER_MESSAGE";
            break;
        case MessengerCommands::DELIVER_MESSAGE:
            o << "MessengerCommands::DELIVER_MESSAGE";
            break;
        case MessengerCommands::PING:
            o << "MessengerCommands::PING";
            break;
        case MessengerCommands::UNKNOWN:
        default:
            o << "MessengerCommands::UNKNOWN";
    }
    return o;
}

using RegisteredMessages = std::unordered_map<std::reference_wrapper<message_id_info>, Messenger::MsgConstructor_t>;

struct MessengerImpl {
    private:
        QtiBusTransport *xport;
        RegisteredMessages registeredMessages;
        qtimutex::QtiSharedMutex mutex;

        volatile bool started = false;

        void localMessageDelivery(std::string msgIdString, std::string payload);
        void localRegisterMessage(pid_t sender, std::string msgIdString);
        Messenger &owner;
    public:
        MessengerImpl(Messenger &owner);
        bool start();

        bool stop();

        void registerForMessage(message_id_ref msgid, Messenger::MsgConstructor_t cnstrct,
                bool reRegister = false);

        void deliverMessage(message_id_ref msgid, std::string payload);

        template <class R, class T>
            R debugHook(T &);
};
