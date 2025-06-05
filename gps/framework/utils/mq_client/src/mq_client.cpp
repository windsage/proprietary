/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

 mq-client

 GENERAL DESCRIPTION
 This component implements the client connection functionality

 Copyright (c) 2012-2014 Qualcomm Atheros, Inc.
 All Rights Reserved.
 Qualcomm Atheros Confidential and Proprietary.

 Copyright (c) 2016-2017, 2021, 2022 Qualcomm Technologies, Inc.
 All Rights Reserved.
 Confidential and Proprietary - Qualcomm Technologies, Inc.
 =============================================================================*/
#define LOG_NDEBUG 0
#define LOG_TAG "MessageQ_Client"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <new>

// define FD_SETSIZE to any number you like to handle more clients. default is 64 or 256
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <linux/sockios.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/un.h>
#include <sys/ioctl.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include <pthread.h>

#include <mq_client/mq_client.h>
#include <base_util/postcard.h>
#include <base_util/sync.h>
#include <memory>
#include <LocIpc.h>
#include <unistd.h>
#include <map>
#include <log_util.h>
#include <dirent.h>

#define BREAK_IF_ZERO(ERR,X) if(0==(X)) {result = (ERR); break;}
#define BREAK_IF_NON_ZERO(ERR,X) if(0!=(X)) {result = (ERR); break;}

#define SOCK_PATH "/dev/socket/location/mq/"
#define SOCK_PATH_MAX_SIZE 100

#ifndef IZAT_OFFLINE
const char* const mq_server_name="/dev/socket/location/mq/location-mq-s";
#endif

using namespace std;
using namespace loc_util;

namespace qc_loc_fw
{

MessageQueueServiceCallback::~MessageQueueServiceCallback()
{
}

MessageQueueClient::~MessageQueueClient()
{
}

class MqClientIpcListener: public ILocIpcListener {
private:
    MessageQueueServiceCallback * mCallback;
    MessageQueueClient * mMsgQueueClient;
public:
    MqClientIpcListener(MessageQueueClient* client) : mMsgQueueClient(client) {}
    inline void setCallback(MessageQueueServiceCallback* callback) {mCallback = callback;}
    virtual void onReceive(const char* data, uint32_t len, const LocIpcRecver* recver) override;
};

class MessageQueueClientImpl: public MessageQueueClient
{
private:
  int processRegisterMessage(InPostcard* card);
  int processAddAliasMessage(InPostcard* card);
  int sendMessage(const char * to, const MemoryStreamBase * const buffer);
  int BroadcastClientRegistration(const char * const name);
public:
  MessageQueueClientImpl();
  ~MessageQueueClientImpl() = default;

  virtual inline int setServerNameDup(const char * const name) override { return 0;}
  virtual inline int connect(const bool name_in_file_system) override { return 0;}
  virtual int send(const MemoryStreamBase * const buffer) override;
  virtual int run_block(MessageQueueServiceCallback * const callback) override;
  virtual inline int shutdown() override {
      if (mIpcRecver != nullptr) {
          mIpcRecver->abort();
      }
      return 0;
  }

  void cleanupIpcSenders(InPostcard* card);
protected:

  shared_ptr<MqClientIpcListener> mListenerPtr;
  char mLocalSockName[SOCK_PATH_MAX_SIZE];
  unique_ptr<LocIpcRecver> mIpcRecver;
  map<string, shared_ptr<LocIpcSender>> mIpcSendersMap;
  mutex mLock;
};

MessageQueueClientImpl::MessageQueueClientImpl() :
    mIpcRecver(nullptr), mListenerPtr(nullptr){}

int MessageQueueClientImpl::send(const MemoryStreamBase * const buffer)
{
    int result = 1;
    InPostcard* card = nullptr;
    do {
        BREAK_IF_ZERO(2, buffer);

        card = InPostcard::createInstance();
        BREAK_IF_ZERO(3, card);

        BREAK_IF_NON_ZERO(4, card->init(buffer->getBuffer(), buffer->getSize()));

        // you must have a "TO" field!
        const char * to = 0;
        BREAK_IF_NON_ZERO(5, card->getString("TO", &to));
        BREAK_IF_ZERO(6, to);

        if (0 == strcmp(to, "SERVER")) {
            // this is the control message for registeration or adding alias
            const char * req = 0;
            BREAK_IF_NON_ZERO(7, card->getString("REQ", &req));
            BREAK_IF_ZERO(8, req);

            if (0 == strcmp(req, "REGISTER")) {
                BREAK_IF_NON_ZERO(9, processRegisterMessage(card));
            } else if (0 == strcmp(req, "ADD-ALIAS")) {
                BREAK_IF_NON_ZERO(10, processAddAliasMessage(card));
            }
            //broadcast the registration message to all clients
            const char* from = 0;
            BREAK_IF_NON_ZERO(11, card->getString("FROM", &from));
            BREAK_IF_ZERO(12, from);
            BREAK_IF_NON_ZERO(13, BroadcastClientRegistration(from));
        } else {
            // this is the message for other mq clients
            BREAK_IF_NON_ZERO(14, sendMessage(to, buffer));
        }

        result = 0;
    } while (0);

    if (0 != result) {
        LOC_LOGe("send failed %d", result);
    }
    if (card != nullptr) {
        delete card;
    }
    return result;
}

int MessageQueueClientImpl::sendMessage(const char * to,
        const MemoryStreamBase * const buffer)
{
    int result = 1;
    do {
        string clientName = string(to);
        LOC_LOGd("Send normal message to %s", clientName.c_str());
        char clientPath[SOCK_PATH_MAX_SIZE];
        snprintf(clientPath, SOCK_PATH_MAX_SIZE, SOCK_PATH"%s", to);

        {
            std::lock_guard<mutex> lck(mLock);
            auto search = mIpcSendersMap.find(clientName);
            if (access(clientPath, F_OK ) != -1 ) {
                if (mIpcSendersMap.end() == search) {
                    //this is the first time to send message to that client,
                    //cache that client in mIpcSendersMap
                    shared_ptr<LocIpcSender> sender = LocIpc::getLocIpcLocalSender(clientPath);
                    mIpcSendersMap[clientName] = sender;
                }
                LocIpc::send(*(mIpcSendersMap[clientName]), buffer->getBuffer(), buffer->getSize());
            } else if (mIpcSendersMap.end() != search) {
                mIpcSendersMap.erase(search);
            }
        }

        result = 0;
    } while (0);

    if (0 != result) {
        LOC_LOGe("send failed %d", result);
    }
    return result;
}

void MessageQueueClientImpl::cleanupIpcSenders(InPostcard* card) {
    //Check to see if there is outdated sender in mIpcSendersMap
    std::lock_guard<mutex> lck(mLock);
    for (auto iter = mIpcSendersMap.begin(); iter != mIpcSendersMap.end();) {
        char clientPath[SOCK_PATH_MAX_SIZE];
        snprintf(clientPath, SOCK_PATH_MAX_SIZE, SOCK_PATH"%s", (iter->first).c_str());
        if (access(clientPath, F_OK) == -1) {
            LOC_LOGw("Socket file doesn't exit, erese it from mIpcSendersMap");
            iter = mIpcSendersMap.erase(iter);
        } else {
            ++iter;
        }
    }
}

int MessageQueueClientImpl::processRegisterMessage(InPostcard* card) {
    int result = 1;

    do {
        const char* from = 0;
        BREAK_IF_NON_ZERO(2, card->getString("FROM", &from));
        BREAK_IF_ZERO(3, from);

        if (mIpcRecver == nullptr) {
            snprintf(mLocalSockName, SOCK_PATH_MAX_SIZE, SOCK_PATH"%s", from);
            LOC_LOGd("processRegisterMessage: %s", mLocalSockName);
            mListenerPtr = make_shared<MqClientIpcListener>(this);
            mIpcRecver = LocIpc::getLocIpcLocalRecver(mListenerPtr, (const char*)mLocalSockName);
            if(0 != chmod(mLocalSockName, 0770)) {
                LOC_LOGe("error setting permission for socket file: %d, [%s]",
                        errno, strerror(errno));
            }
            result = 0;
        }
    } while(0);

    if (0 != result) {
        LOC_LOGe("register client failed %d", result);
    }
    return result;
}

int MessageQueueClientImpl::processAddAliasMessage(InPostcard* card) {
    int result = 1;

    do {
        const char* alias = 0;
        BREAK_IF_NON_ZERO(2, card->getString("ALIAS", &alias));
        BREAK_IF_ZERO(3, alias);
        char symbolicPath[SOCK_PATH_MAX_SIZE];
        snprintf(symbolicPath, SOCK_PATH_MAX_SIZE, SOCK_PATH"%s", alias);
        LOC_LOGd("processRegisterMessage: %s", symbolicPath);

        if (mIpcRecver != nullptr) {
            if (access(symbolicPath, F_OK) != -1) {
                BREAK_IF_NON_ZERO(4, remove(symbolicPath));
            }
            int ret = symlink(mLocalSockName, symbolicPath);
            if (ret == -1) {
                LOC_LOGe("symlink %d", errno);
            }
            BREAK_IF_NON_ZERO(5, ret);
        } else {
            break;
        }
        result = 0;
    } while(0);

    if (0 != result) {
        LOC_LOGe("add alias failed %d", result);
    }
    return result;
}

int MessageQueueClientImpl::BroadcastClientRegistration(const char * const name) {
    int result = 1;
    OutPostcard* card = 0;
    struct dirent *entry;
    DIR *dir = opendir(SOCK_PATH);
    if (dir == NULL) {
        return result;
    }
    while ((entry = readdir(dir)) != NULL) {
        const char * to = entry->d_name;
        if (strcmp(to, ".") != 0 &&
                strcmp(to, "..") !=0) {
            card = OutPostcard::createInstance();
            BREAK_IF_ZERO(1, card);
            BREAK_IF_NON_ZERO(2, card->init());
            BREAK_IF_NON_ZERO(3, card->addString("TO", to));
            BREAK_IF_NON_ZERO(4, card->addString("FROM", "LOCATION-MQ"));
            BREAK_IF_NON_ZERO(5, card->addString("INFO", "REGISTER-EVENT"));
            BREAK_IF_NON_ZERO(6, card->addString("CLIENT", name));
            BREAK_IF_NON_ZERO(7, card->finalize ());
            BREAK_IF_NON_ZERO(8, sendMessage(to, card->getEncodedBuffer()));
            delete card;
            card = nullptr;
        }
        result = 0;
    }
    if (card != nullptr) {
        delete card;
    }
    closedir(dir);

    if (0 != result) {
        LOC_LOGe("BroadcastClientRegistration %d", result);
    }
    return result;
}

void MqClientIpcListener::onReceive(const char* data, uint32_t len, const LocIpcRecver* recver) {
    int result = 1;
    InMemoryStream * new_buffer = 0;
    char* copied_buffer = 0;
    do {
        new_buffer = InMemoryStream::createInstance();
        BREAK_IF_ZERO(1, new_buffer);

        copied_buffer = new char[len];
        memcpy(copied_buffer, data, len);
        BREAK_IF_NON_ZERO(2, new_buffer->setBufferOwnership((const void **)&copied_buffer, len));

        //Check if msg is register event message, if it is, go to cleanupIpcSenders
        //and then go to the real message handling function
        InPostcard * cardHolder = InPostcard::createInstance();
        BREAK_IF_ZERO(3, cardHolder);
        cardHolder->init(new_buffer->getBuffer(), new_buffer->getCapacity());
        const char * info = 0;
        cardHolder->getString("INFO", &info);
        if (info) {
            string infoStr(info);
            if (0 == infoStr.compare ("REGISTER-EVENT") && mMsgQueueClient != nullptr) {
                LOC_LOGd("enter cleanupIpcSenders");
                (reinterpret_cast<MessageQueueClientImpl*>(mMsgQueueClient))
                        ->cleanupIpcSenders(cardHolder);
            }
        }
        if (cardHolder != nullptr) {
            delete cardHolder;
            cardHolder = 0;
        }

        if (mCallback != NULL) {
            BREAK_IF_NON_ZERO(4, mCallback->newMsg(new_buffer));
        }
        new_buffer = 0;

        result = 0;
    } while (0);

    if (copied_buffer) {
        LOC_LOGd("the ownership of copied buffer isn't transfered..");
        delete[] copied_buffer;
    }
    if (new_buffer) {
        delete new_buffer;
    }
    if (0 != result) {
        LOC_LOGe("onReceive failed %d", result);
    }
}

int MessageQueueClientImpl::run_block(MessageQueueServiceCallback * const callback)
{
    if (mIpcRecver != nullptr && mListenerPtr != nullptr) {
        mListenerPtr->setCallback(callback);
        LocIpc::startBlockingListening(*mIpcRecver);
    }
    return 0;
}

MessageQueueClient * MessageQueueClient::createInstance()
{
  return new (std::nothrow) MessageQueueClientImpl();
}

} // namespace qc_loc_fw
