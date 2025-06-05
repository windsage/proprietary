/********************************************************************
 * Copyright (c) 2017, 2023 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *********************************************************************/

#ifndef CLIENT_INCLUDE_QTEECONNECTORCLIENT_H_
#define CLIENT_INCLUDE_QTEECONNECTORCLIENT_H_

#include <utils/Log.h>
#include <aidl/vendor/qti/hardware/qteeconnector/BnApp.h>
#include <aidl/vendor/qti/hardware/qteeconnector/BnAppConnector.h>
#include <mutex>
#include <string>
#include "QSEEComAPI.h"

namespace QTEE {

using IAppAidl = ::aidl::vendor::qti::hardware::qteeconnector::IApp;
using IAppConnectorAidl = ::aidl::vendor::qti::hardware::qteeconnector::IAppConnector;
using ::ndk::SpAIBinder;

/**
 * @brief Helper class for connection via IApp/IAppConnector
 *
 * This class wraps the IAppConnector/IApp hwbinder interface used for connections
 * via the QSEEComAPI to match the earlier used QSEEConnectorClient.h
 */
class QTEEConnectorClient {

public:
  QTEEConnectorClient() = delete;
  /**
   * @brief create a QTEEConnectorClient
   *
   * The constructor tries to establish a connection to the service.
   *
   * @note The size of the shared memory can be overridden by the daemon, if the
   * QSEE Application is configured to require a larger memory than here requested.
   *
   * @note The requested size MUST take into account alignment and padding of all the
   * command and response buffers which are going to be used in subsequent calls to
   * sendCommand and sendModifiedCommand.
   *
   * @param[in] path path in the HLOS file system where the QSEE App is located
   * @param[in] name name of the QSEE application to be loaded
   * @param[in] requestedSize size of the shared memory associated with the QSEE Application
   */
  QTEEConnectorClient(std::string const& path, std::string const& name, uint32_t requestedSize);

  /**
   * @brief Destructor
   *
   * If the app is still loaded, the client attempts to unload the application.
   */
  virtual ~QTEEConnectorClient();

  /**
   * @brief load the trusted application
   *
   * @return true on success, false otherwise
   */
  bool load();
  /**
   * @brief unload the trusted application
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
   * @brief send a command to the trusted application
   *
   * @param[in] req the request buffer
   * @param[in] reqLen length of the request buffer
   * @param[out] rsp the response buffer
   * @param[in] rspLen length of the response buffer
   *
   * @return android::OK on success, errorcode otherwise
   */
  android::status_t sendCommand(void const* req, uint32_t reqLen, void* rsp, uint32_t rspLen);
  /**
   * @brief send a modified command to the trusted application
   *
   * @param[in] req the request buffer
   * @param[in] reqLen length of the request buffer
   * @param[out] rsp the response buffer
   * @param[in] rspLen length of the response buffer
   * @param[in] info a description of the memory references shared with the trusted application
   *
   * @return android::OK on success, errorcode otherwise
   */
  android::status_t sendModifiedCommand(void const* req, uint32_t reqLen, void* rsp,
                                        uint32_t rspLen, struct QSEECom_ion_fd_info const* info);


  /**
   * @brief attempt to recover the connection to the service
   *
   * @return true if recovery succeeded, false otherwise
   */
  bool recover();
  std::mutex mutable mQTEEmutex;  ///< a mutex for all calls to the service
 private:
  /**
   * @brief load the application
   *
   * @return true on success, false otherwise
   */
  bool doLoad();

  /**
   * @brief Return the aidl service instance
   *
   * @return service object on success, nullptr otherwise
   */

  std::shared_ptr<IAppConnectorAidl> getServiceAidl();

  ::ndk::ScopedAIBinder_DeathRecipient mDeathRecipientAidl;  ///< the death notifier
  std::shared_ptr<IAppConnectorAidl> mAppConnector;  ///< a strongpointer to the IAppConnector interface
  ::ndk::SpAIBinder mAppConnectorBinder; 
  std::shared_ptr<IAppAidl> mApp;  ///< a strongpointer representing the application at the service side

  std::string const mPath;        ///< the path of the trusted application
  std::string const mName;        ///< the name of the trusted application
  int32_t mRequestedSize;         ///< the originally requested buffer size
  bool mIsApp64;                  ///< whether the application is 64bit
  bool mLoaded;                   ///< whether the application is loaded
};
};  // namespace QTEE

#endif  // CLIENT_INCLUDE_QTEECONNECTORCLIENT_H_
