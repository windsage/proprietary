/*
 * Copyright (c) 2019 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#pragma once

#include <memory>
#include <framework/Module.h>
#include <qtibus/IPCMessage.h>
#include <istream>

class QtiBusTransport;

struct MessengerImpl;
class Messenger {
    public:
        using OnClientConnectedCb = std::function<void(pid_t)>;
        void setOnClientConnectedCb(OnClientConnectedCb cb) {
            onClientConnected = cb;
        }
        using OnClientDeadCb = std::function<void(pid_t)>;
        void setOnClientDeadCb(OnClientDeadCb cb) {
            onClientDead = cb;
        }

        using OnRemoteRegistrationCb = std::function<void(pid_t, message_id_p)>;
        void setOnRemoteRegistrationCb(OnRemoteRegistrationCb cb) {
            onRemoteRegistration = cb;
        }

        friend struct MessengerImpl;

    private:
        Messenger();
        struct MessengerImpl *impl;
        OnClientConnectedCb onClientConnected;
        OnClientDeadCb onClientDead;
        OnRemoteRegistrationCb onRemoteRegistration;
    public:
        using MsgConstructor_t = std::function<std::shared_ptr<IPCMessage>(IPCIStream &)>;
        static Messenger &get();
        void registerForMessage(message_id_ref msgid, MsgConstructor_t cnstrct);
        void deliverMessage(message_id_ref msgid, std::string buf);
        void start();
        void stop();

        template <class R, class T>
            R debugHook(T &);
};
