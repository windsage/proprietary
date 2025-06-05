/******************************************************************************
#  Copyright (c) 2022 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#include "RilSocketClientController.h"
#include <sys/socket.h>
#include <sys/un.h>
#include <inttypes.h>
#include <log/log.h>
#include <ril_socket_api.h>
#include <framework/Log.h>

#undef TAG
#define TAG "RILQ"

RilSocketClientController::RilSocketClientController() {
  mClientContextsMap.clear();
  mUnsolIndicationsRegisterMap.clear();
};

/*
* Get instance of RilSocketClientController
*/
RilSocketClientController &RilSocketClientController::getInstance() {
  static RilSocketClientController sInstance;
  return sInstance;
}

/*
* Get the client context associated with socket FD
*/
const std::shared_ptr<ClientContext> RilSocketClientController::getSocketClientContext(int fd) {
  std::lock_guard<qtimutex::QtiRecursiveMutex> lock(mMutex);
  std::shared_ptr<ClientContext> context = nullptr;

  auto iter = mClientContextsMap.find(fd);
  if (iter != mClientContextsMap.end()) {
    context = iter->second;
  }
  return context;
}

/*
* Create new client context for socket FD
*/
void RilSocketClientController::createClientContext(int fd, int uid, int pid) {
  std::lock_guard<qtimutex::QtiRecursiveMutex> lock(mMutex);
  QCRIL_LOG_DEBUG("Create new ClientContext for %d ",fd);
  std::shared_ptr<ClientContext> context = std::make_shared<ClientContext>(fd, uid, pid);

  mClientContextsMap.insert({fd, context});
}

/*
* Register client context for the unsol indication
*/
RIL_Errno RilSocketClientController::registerClientForIndications(int clientFd,
  int32_t indicationId) {
  RIL_Errno errNo = RIL_E_GENERIC_FAILURE;
  std::lock_guard<qtimutex::QtiRecursiveMutex> lock(mMutex);

  //retrieve clientContext from fd
  QCRIL_LOG_DEBUG("query ClientContext for fd %d", clientFd);
  std::shared_ptr<ClientContext> context = getSocketClientContext(clientFd);
  if (context) {
    //find and append to existing key
    auto iter = mUnsolIndicationsRegisterMap.find(indicationId);
    if (iter != mUnsolIndicationsRegisterMap.end()) {
      ClientContextList &clients = iter->second;
      std::pair<ClientContextList::iterator,bool> status;
      QCRIL_LOG_DEBUG("Append client: %d to indication:%d ", clientFd, indicationId);
      status = clients.insert(context);
      if (status.second == false) {
         QCRIL_LOG_DEBUG(" client is already registered for indication: %d", indicationId);
      }
    } else {
      //create list if this is the 1st client
      QCRIL_LOG_DEBUG("Registering 1st client for indication %d ", context->mSocketFd);
      mUnsolIndicationsRegisterMap.insert({indicationId, ClientContextList{context}});

      //store the indication in clientContext also .
      context->mIndicationList.push_back(indicationId);
    }
    errNo = RIL_E_SUCCESS;
  }
  return errNo;
}

/*
* Get list of Clientcontexts for an indication
*/
const ClientContextList RilSocketClientController::getRegisteredClients(
  int indicationId) {
  std::lock_guard<qtimutex::QtiRecursiveMutex> lock(mMutex);
  auto iter = mUnsolIndicationsRegisterMap.find(indicationId);
  if (iter != mUnsolIndicationsRegisterMap.end()) {
    return iter->second;
  } else {
    return ClientContextList();//return empty list if not found
  }
}

/*
* Clear a client context .
* Unregister for unsol indications also
*/
void RilSocketClientController::ClearClientContext(int socketFd) {
  std::lock_guard<qtimutex::QtiRecursiveMutex> lock(mMutex);

  //Retrieve client_context
  std::shared_ptr<ClientContext> context = getSocketClientContext(socketFd);

  if (context) {
    //Set state as inactive
    context->mIsActive = false;

    //get the list of indications registered by this client
    std::vector<int> &indList = context->mIndicationList;

    for (auto iter = indList.begin(); iter != indList.end(); iter++) {
      int indId = *iter;
      //remove the client context from indications map
      auto findIter = mUnsolIndicationsRegisterMap.find(indId);
      if (findIter != mUnsolIndicationsRegisterMap.end()) {
        ClientContextList &regList = findIter->second;
        regList.erase(context);
      }
    }

    //Remove client from ClientContexts Map
    auto iter = mClientContextsMap.find(socketFd);
    if (iter != mClientContextsMap.end()) {
      mClientContextsMap.erase(iter);
    }
  }
  QCRIL_LOG_DEBUG("Current No of active clients %d ", mClientContextsMap.size());
}
