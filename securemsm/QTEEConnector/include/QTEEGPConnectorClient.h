/********************************************************************
 * Copyright (c) 2017, 2021, 2023 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *********************************************************************/

#ifndef CLIENT_INCLUDE_QTEEGPCONNECTORCLIENT_H_
#define CLIENT_INCLUDE_QTEEGPCONNECTORCLIENT_H_

#include <utils/Log.h>
#include <aidl/vendor/qti/hardware/qteeconnector/BnGPApp.h>
#include <aidl/vendor/qti/hardware/qteeconnector/BnGPAppConnector.h>
#include <mutex>
#include <string>
#include <vector>
#include <atomic>
#include "QSEEComAPI.h"

namespace QTEE {

using IGPAppAidl = ::aidl::vendor::qti::hardware::qteeconnector::IGPApp;
using IGPAppConnectorAidl = ::aidl::vendor::qti::hardware::qteeconnector::IGPAppConnector;
using ::ndk::SpAIBinder;

/**
 * @brief Helper class for connection via IGPApp/IGPAppConnector
 *
 * This class wraps the IGPAppConnector/IGPApp hwbinder interface used to load and to communicate
 * with
 * GP applications. It should be transparent to the GPTEE environment.
 */
class QTEEGPConnectorClient {

 public:
  QTEEGPConnectorClient() = delete;
  /**
   * @brief create a QTEEGPConnectorClient
   *
   * The constructor tries to establish a connection to the service.
   *
   * @note The size of the shared memory can be overridden by the daemon, if the
   * QSEE Application is configured to require a larger memory than here requested.
   *
   * @note The requested size MUST take into account alignment and padding of all the
   * command and response buffers which are going to be used in subsequent calls to
   * openSession, invokeCommand or closeSession.
   *
   * @param[in] path path in the HLOS file system where the QSEE App is located
   * @param[in] name name of the QSEE application to be loaded
   * @param[in] requestedSize size of the shared memory associated with the QSEE Application
   */
  QTEEGPConnectorClient(std::string const& path, std::string const& name, uint32_t requestedSize);

  /**
   * @brief Destructor
   *
   * If the app is still loaded, the client attempts to unload the application.
   */
  virtual ~QTEEGPConnectorClient();

  /**
   * @brief load the GP application
   *
   * @return true on success, false otherwise
   */
  bool load();

  /**
   * @brief load the GP application
   *
   * @return true on success, false otherwise
   */
  bool load(android::status_t &err);

  /**
   * @brief load the GP application
   *
   * @return true on success, false otherwise
   */
  void unload();

  /**
   * @brief check whether the loaded app is a 64bit application
   *
   * @return true if the loaded app is 64bit, false otherwise
   */
  bool isApp64() const { return mIsApp64; }

  /**
   * @brief check whether the remote is alive
   *
   * @return true if the remote is alive, false otherwise
   */
  bool isAlive();

  /**
   * @brief invoke a command in the GP application

   * @param[in] req the request buffer
   * @param[in] reqLen length of the request buffer
   * @param[out] rsp the response buffer
   * @param[in] rspLen length of the response buffer
   * @param[in] info a description of the memory references shared with the trusted application
   *
   * @return android::OK on success, errorcode otherwise
   */
  android::status_t invokeCommand(void const* req, uint32_t reqLen, void* rsp, uint32_t rspLen,
                                  struct QSEECom_ion_fd_info const* info);

  /**
   * @brief open a session to a GP application

   * @param[in] req the request buffer
   * @param[in] reqLen length of the request buffer
   * @param[out] rsp the response buffer
   * @param[in] rspLen length of the response buffer
   * @param[in] info a description of the memory references shared with the trusted application
   *
   * @return android::OK on success, errorcode otherwise
   */
  android::status_t openSession(void const* req, uint32_t reqLen, void* rsp, uint32_t rspLen,
                                struct QSEECom_ion_fd_info const* info);

  /**
   * @brief close a session with a GP application

   * @param[in] req the request buffer
   * @param[in] reqLen length of the request buffer
   * @param[out] rsp the response buffer
   * @param[in] rspLen length of the response buffer
   *
   * @return android::OK on success, errorcode otherwise
   */
  android::status_t closeSession(void const* req, uint32_t reqLen, void* rsp, uint32_t rspLen);

  /**
   * @brief request a cancellation
   * @param[in] sessionId the session id to be cancelled
   *
   * @return android::OK on success, errorcode otherwise
   */
  android::status_t requestCancellation(uint32_t sessionId);

  /**
   * @brief clear up the local proxy objects (mGPApp and mGPAppConnector)
   *
   * @return void
   */
  void clearUp(bool clearFlag);

  std::mutex mutable mQTEEmutex;           ///< a mutex for all calls to the service
 private:

  /**
   * @brief clear up the local proxy objects (mGPApp and mGPAppConnector)
   *
   * @return void
   */

  /**
   * @brief load the applications dependencies
   *
   * @return true on success, false otherwise
   */
  bool loadDependencies();

  /**
   * @brief load the application
   *
   * @return true on success, false otherwise
   */
  bool doLoad();

  /**
   * @brief load the application
   *
   * @return true on success, false otherwise
   */
  bool doLoad(android::status_t &err);

  /**
   * @brief Return the aidl service instance
   *
   * @return service object on success, nullptr otherwise
   */
  std::shared_ptr<IGPAppConnectorAidl> getServiceAidl();

  ::ndk::ScopedAIBinder_DeathRecipient mDeathRecipientAidl; ///< the death notifier
  std::shared_ptr<IGPAppConnectorAidl> mGPAppConnector;  ///< a strongpointer to the IGPAppConnector interface
  ::ndk::SpAIBinder mGPAppConnectorBinder;
  std::shared_ptr<IGPAppAidl> mGPApp;  ///< a strongpointer representing the application at the service side

  std::string const mPath;        ///< the path of the GP application
  std::string const mName;        ///< the name of the GP application
  int32_t mRequestedSize;  ///< the originally requested buffer size
  bool mIsApp64;                  ///< whether the application is 64bit
  bool mLoaded;                   ///< whether the application is loaded
  std::atomic_flag mRemoteAlive = ATOMIC_FLAG_INIT; ///< used to check if the remote is died
};
};  // namespace QTEE

#endif  // CLIENT_INCLUDE_QTEEGPCONNECTORCLIENT_H_
