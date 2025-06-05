/******************************************************************************
#  Copyright (c) 2022 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#pragma once

#include <list>
#include <map>
#include <algorithm>
#include <vector>
#include <set>
#include "QtiMutex.h"
#include <telephony/ril.h>

/*
 * Structure to hold a Client context
*/
class ClientContext {
public:
  int mSocketFd;
  bool mIsActive;
  int mUid;
  int mPid;
  std::vector<int> mIndicationList;
  ClientContext(int fd, int uid, int pid) : mSocketFd(fd), mIsActive(true),
      mUid(uid), mPid(pid) { mIndicationList.clear(); };
  ~ClientContext() {};
};
using ClientContextList = std::set<std::shared_ptr<ClientContext>>;

/*
 * Singleton class to create and manage client contexts.
 * Manages the indication registration by clients
*/
class RilSocketClientController {
  public:
    void createClientContext(int fd, int uid, int pid);
    static RilSocketClientController &getInstance();
    const std::shared_ptr<ClientContext> getSocketClientContext(int fd);
    RIL_Errno registerClientForIndications(int clientFd, int32_t indicationId);
    const ClientContextList getRegisteredClients(int indicationId);
    void ClearClientContext(int socketFd);

  private:
    qtimutex::QtiRecursiveMutex mMutex;
    std::map<int32_t , std::shared_ptr<ClientContext>> mClientContextsMap;
    std::map<int32_t , ClientContextList> mUnsolIndicationsRegisterMap;
    RilSocketClientController();
};
