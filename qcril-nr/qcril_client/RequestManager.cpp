/******************************************************************************
#  Copyright (c) 2019 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#include <unistd.h>
#include <errno.h>
#include <poll.h>
#include <arpa/inet.h>
#include <inttypes.h>
#include <fstream>
#include <Logger.h>
#include <record_stream.h>
#include <RequestManager.hpp>
#ifdef QCRIL_BUILD_AFL_TESTCASE_GEN
#include <request_api_map_auto.h>
#include <client_fs_utils.hpp>
#endif //QCRIL_BUILD_AFL_TESTCASE_GEN
#define TAG "RequestManager"

RequestManager::RequestManager(int socketFd, const ErrorCallback& errorCb)
        : socketFd(socketFd), errorCallback(errorCb) {
    #ifdef QCRIL_BUILD_AFL_TESTCASE_GEN
    INIT_REQUEST_API_MAP(requestApiMap);
    #endif // QCRIL_BUILD_AFL_TESTCASE_GEN
    requestListener = std::thread(
        [this] {
            this->listenForRequests();
        }
    );

    responseListener = std::thread(
        [this] {
            this->listenForResponses();
        }
    );
}

RequestManager::~RequestManager() {
    Status s = issueRequest(
        RIL_REQUEST_DISCONNECT,
        nullptr,
        [] (RIL_Errno e, std::shared_ptr<Marshal> /*p*/) {
            if (e == RIL_E_SUCCESS) {
                QCRIL_HAL_LOG_DEBUG("Disconnection request successful.");
            } else {
                QCRIL_HAL_LOG_DEBUG("Disconnection request failed.");
            }
        }
    );

    if (s != Status::SUCCESS) {
        QCRIL_HAL_LOG_DEBUG("Failed to issue disconnection request.");
    }

    disconnectionRequested.store(true, std::memory_order_relaxed);

    requestListener.join();
}

void RequestManager::recordSocketError() {
    socketErrorDetected.store(true, std::memory_order_relaxed);
}

bool RequestManager::isSocketErrorDetected() {
    return socketErrorDetected.load(std::memory_order_relaxed);
}

bool RequestManager::isDisconnectionRequested() {
    return disconnectionRequested.load(std::memory_order_relaxed);
}

void RequestManager::invokeErrorCallback() {
    std::call_once(
        errorCallbackInvocationFlag,
        [this] () {
            if (this->errorCallback && !this->isDisconnectionRequested() &&
                    this->isSocketErrorDetected()) {
                this->errorCallback(Status::SOCKET_FAILURE);
            }
        }
    );
}

void RequestManager::detectSocketError() {
    struct pollfd fds;
    fds.fd = socketFd;
    fds.events = POLLIN | POLLOUT;

    int ret;

    do {
        fds.revents = 0;

        ret = poll(&fds, 1, 500);

        if (ret == 0) {
            QCRIL_HAL_LOG_DEBUG("Polling socket timed out. Assuming socket is still alive.");
        } else if (ret < 0) {
            if (ret != EINTR) {
               QCRIL_HAL_LOG_DEBUG("Error in polling socket. Assuming socket is dead.");
               recordSocketError();
            }
        } else if (fds.revents & (POLLERR | POLLHUP | POLLNVAL)) {
            QCRIL_HAL_LOG_DEBUG("Socket error detected.");
            recordSocketError();
        } else if (fds.revents & (POLLIN | POLLOUT)) {
            QCRIL_HAL_LOG_DEBUG("Socket is still alive!");
        }
    } while (ret == EINTR);
}

void RequestManager::listenForRequests() {
    QCRIL_HAL_LOG_DEBUG("Starting loop to listen for requests.");

    std::queue<Request> requests;

    while (true) {
        // BEGIN unique_lock block with requestsWaitingToBeIssuedMutex
        {
            std::unique_lock<std::mutex> lock(requestsWaitingToBeIssuedMutex);
            if (requestsWaitingToBeIssued.empty()) {
                requestNotifier.wait(
                    lock,
                    [this] {
                        return !(this->requestsWaitingToBeIssued).empty() || isSocketErrorDetected();
                    }
                );
            }

            if (isSocketErrorDetected()) {
                break;
            }

            requests = std::move(requestsWaitingToBeIssued);
            requestsWaitingToBeIssued = std::queue<Request>();
        }
        // END unique_lock block with requestsWaitingToBeIssuedMutex

        QCRIL_HAL_LOG_DEBUG("Got one or more requests.");

        while (!requests.empty()) {
            Request& req = requests.front();
            QCRIL_HAL_LOG_DEBUG("Processing request with ID %d.", std::get<0>(req));
            uint64_t token{0};

            // BEGIN scope boundary for "cb"
            {
                std::unique_ptr<GenericResponseCallback> cb(std::move(std::get<2>(req)));
                if (cb == nullptr) {
                    requests.pop();
                    continue;
                }

                if (isSocketErrorDetected()) {
                    (*(cb))(RIL_E_SYSTEM_ERR, nullptr);
                    requests.pop();
                    continue;
                }

                token = reinterpret_cast<uint64_t>(cb.get());

                // BEGIN lock_guard block with requestsWaitingForResponseMutex
                {
                    std::lock_guard<std::mutex> lock(requestsWaitingForResponseMutex);
                    requestsWaitingForResponse[token] = std::move(cb);
                }
                // END lock_guard block with requestsWaitingForResponseMutex
            }
            // END scope boundary for "cb"

            QCRIL_HAL_LOG_DEBUG("Sending request to the server.");
            Status s = sendRequestToServer(
                token,
                std::get<0>(req),
                std::move(std::get<1>(req))
            );

            if (s == Status::SOCKET_WRITE_FAILURE) {
                detectSocketError();
            }

            if (s != Status::SUCCESS) {
                std::unique_ptr<GenericResponseCallback> callback = nullptr;

                // BEGIN lock_guard block with requestsWaitingForResponseMutex
                {
                    std::lock_guard<std::mutex> lock(requestsWaitingForResponseMutex);
                    auto search = requestsWaitingForResponse.find(token);
                    if (search != requestsWaitingForResponse.end()) {
                        callback = std::move(search->second);
                        requestsWaitingForResponse.erase(search);
                    }
                }
                // END lock_guard block with requestsWaitingForResponseMutex

                if (callback) {
                    (*(callback))(RIL_E_SYSTEM_ERR, nullptr);
                }
            }

            requests.pop();
        }

        if (isSocketErrorDetected()) {
            break;
        }
    }

    flushRequestsWaitingToBeIssued();
    //Wait for the response thread to finish before calling the error callback
    responseListener.join();
    invokeErrorCallback();
}

Status RequestManager::sendRequestToServer(uint64_t token, int32_t requestId, std::unique_ptr<Marshal> parcel) {
    if (socketFd < 0) {
        QCRIL_HAL_LOG_DEBUG("Socket FD (%d) not valid", socketFd);
        return Status::FAILURE;
    }

    // TODO: assert parcel size is not larger than the maximum 32-bit unsigned integer
    uint32_t bufferSize = htonl(static_cast<uint32_t>(sizeof(token) + sizeof(requestId) + (parcel ? parcel->dataSize() : 0)));
    QCRIL_HAL_LOG_DEBUG("Sending buffer of size %zu",
            sizeof(token) + sizeof(requestId) + (parcel ? parcel->dataSize() : 0));

    {
        std::lock_guard<std::mutex> lock(socketWriteMutex);
        if (writeToSocket(socketFd, &bufferSize, sizeof(bufferSize)) < 0) {
            // TODO: return a more specific error
            QCRIL_HAL_LOG_DEBUG("Failed to transmit the size of the buffer over the socket.");
            return Status::SOCKET_WRITE_FAILURE;
        }

        // write request ID
        if (writeToSocket(socketFd, &requestId, sizeof(requestId)) < 0) {
            // TODO: return a more specific error
            QCRIL_HAL_LOG_DEBUG("Failed to transmit request ID over the socket.");
            return Status::SOCKET_WRITE_FAILURE;
        }

        // write token
        if (writeToSocket(socketFd, &token, sizeof(token)) < 0) {
            // TODO: return a more specific error
            QCRIL_HAL_LOG_DEBUG("Failed to transmit token over the socket.");
            return Status::SOCKET_WRITE_FAILURE;
        }

        if (parcel) {
            // write request parameter parcel
            if (writeToSocket(socketFd, parcel->data(), parcel->dataSize()) < 0) {
                // TODO: return a more specific error
                QCRIL_HAL_LOG_DEBUG("Failed to transmit request parameter parcel over the socket.");
                return Status::SOCKET_WRITE_FAILURE;
            }
        }

    }
    #ifdef QCRIL_BUILD_AFL_TESTCASE_GEN
    if (saveTestcases) {
        if (!testcaseOutDir.empty()) {
            QCRIL_HAL_LOG_DEBUG("Writing request as testcase file to directory: %s",
                                 testcaseOutDir.c_str());
            std::string testcasePath = std::string(testcaseOutDir);
            std::string fname;
            std::map<int, std::string>::iterator it;
            it = requestApiMap.find(requestId);
            if (it != requestApiMap.end()) {
                fname = it->second + "_" + std::to_string(token);
            }
            else {
                QCRIL_HAL_LOG_DEBUG("Request ID %d does not have an entry in the requestApiMap. \
                                     Falling back to request id for file name.", requestId);
                fname = std::to_string(requestId) + "_" + std::to_string(token);
            }
            /* The path might be the root directory, in which case we don't
             * want to add an extra slash.
             */
            if (testcasePath.back() == '/') {
                testcasePath += "afl_test_case_" + fname;
            }
            else {
                #ifndef _WIN32
                testcasePath += "/afl_test_case_" + fname;
                #else
                testcasePath += "\\afl_test_case_" + fname;
                #endif
            }
            const char* testcaseFileName = testcasePath.c_str();
            std::ofstream testcaseFile(testcaseFileName, std::ios::out | std::ios::binary);
            if (!testcaseFile.is_open()) {
                QCRIL_HAL_LOG_DEBUG("Failed to open testcase file %s", testcaseFileName);
            }
            /*  Split request ID into 2 bytes. AFL will ignore values larger than 2 bytes anyway
             *  and there are currently no request IDs that are larger than that.
             *  It's alright if this int gets truncated into a short.
             */
            char requestIdBytes[2];
            requestIdBytes[0] = static_cast<unsigned short>(requestId) >> 8;
            requestIdBytes[1] = static_cast<unsigned short>(requestId) ^ requestIdBytes[1] << 8;
            testcaseFile.write(requestIdBytes, 2);
            if ( (testcaseFile.rdstate() & std::ifstream::failbit ) != 0 ) {
                QCRIL_HAL_LOG_DEBUG("Error writing to %s", testcaseFileName);
            }
            else {
                QCRIL_HAL_LOG_DEBUG("Wrote request ID to testcase file.");
            }
            if (parcel) {
              testcaseFile.write(reinterpret_cast<const char *>(parcel->data()),
                                 parcel->dataSize());
              if ( (testcaseFile.rdstate() & std::ifstream::failbit ) != 0 ) {
                QCRIL_HAL_LOG_DEBUG("Error writing to %s", testcaseFileName);
              }
              else {
                QCRIL_HAL_LOG_DEBUG("Wrote %zu bytes of parcel data to testcase file.",
                                     parcel->dataSize());
              }
            }
            testcaseFile.close();
       }
       else {
           QCRIL_HAL_LOG_DEBUG("Testcase output directory is an empty string");
       }
   }
   #endif // QCRIL_BUILD_AFL_TESTCASE_GEN


    return Status::SUCCESS;
}

void printBuffer(const void* buffer, size_t bufferLength, std::string& s) {
    char hex[4];
    for (int i = 0; i < bufferLength; i++) {
        std::snprintf(hex, sizeof(hex), "%-3.2hhX", *(static_cast<const uint8_t*>(buffer) + i));
        s += hex;
    }
}

ssize_t writeToSocket(int socketFd, const void* buffer, size_t bufferLength) {
    // TODO: Should writes to the socket timeout?
    std::string s;
    printBuffer(buffer, bufferLength, s);
    QCRIL_HAL_LOG_DEBUG("Buffer contents: %s", s.c_str());

    ssize_t totalBytesWritten = 0;
    while (totalBytesWritten < bufferLength) {
        ssize_t bytesWritten = write(socketFd, static_cast<const uint8_t*>(buffer) + totalBytesWritten,
                                     bufferLength - totalBytesWritten);
        if (bytesWritten < 0) {
            QCRIL_HAL_LOG_DEBUG("Failed to write to the socket: %s", strerror(errno));
            return bytesWritten;
        }
        QCRIL_HAL_LOG_DEBUG("Wrote %zd bytes to the socket.", bytesWritten);
        totalBytesWritten += bytesWritten;
    }

    return totalBytesWritten;
}

void RequestManager::handleResponseListenerFailure() {
    recordSocketError();
    requestNotifier.notify_one();
    flushRequestsWaitingForResponse();
}

void RequestManager::listenForResponses() {
    QCRIL_HAL_LOG_DEBUG("Starting loop to listen for responses.");

    // Start polling on sockets and exit socket
    struct pollfd fds;
    fds.fd = socketFd;
    fds.events = POLLIN;

    std::unique_ptr<RecordStream, void (*)(RecordStream *)> rs(
        record_stream_new(socketFd, MAX_COMMAND_BYTES), record_stream_free);

    if (rs == nullptr) {
        QCRIL_HAL_LOG_DEBUG("Failed to create record stream.");
        handleResponseListenerFailure();
        return;
    }

    int ret;
    void *record;
    size_t recordLen;

    while (true) {
        // revents is set by the poll() implementation to signify which events caused the poll to return
        fds.revents = 0;

        ret = poll(&fds, 1, -1);

        if (ret < 0) {
            QCRIL_HAL_LOG_DEBUG("Error in polling socket, exiting %d", ret);
            break;
        }

        if (fds.revents & (POLLERR | POLLHUP | POLLNVAL)) {
            QCRIL_HAL_LOG_DEBUG("Error in poll, exiting listener thread");
            break;
        }

        if (fds.revents != POLLIN) {
            continue;
        }

        int errsv = 0;

        while (true) {
            errno = 0;
            ret = record_stream_get_next(rs.get(), &record, &recordLen);
            errsv = errno;

            QCRIL_HAL_LOG_DEBUG(
                    "record_stream_get_next: recordLen: %zu, ret: %d, errno: %d, errStr: %s, socket fd: %d",
                    recordLen, ret, ret ? errsv : 0, strerror(errsv), socketFd);

            if(ret == 0 && record == nullptr) {
                // EOS - Done reading data from socket, client has disconnected
                break;
            } else if(ret < 0 && errsv != EAGAIN) {
                // error condition
                break;
            } else if(ret < 0 && errsv == EAGAIN) {
                // client has not finished writing to socket, wait for more data before processing
                break;
            } else if(ret == 0) {
                if(recordLen > MAX_COMMAND_BYTES) {
                    ret = -1;
                    QCRIL_HAL_LOG_DEBUG("Received parcel is too big to be valid; not handling");
                    break;
                } else {
                    QCRIL_HAL_LOG_DEBUG("Got a response parcel");
                    std::shared_ptr<Marshal> p = std::make_shared<Marshal>();
                    // TODO: check if we could end up with a dangling pointer inside
                    // parcel if this thread exits before RIL has a chance to process parcel
                    // (because that will destroy the RecordStream instance and thus free "record")
                    if (p) {
                        p->setData(std::string(reinterpret_cast<const char*>(record), recordLen));

                        uint32_t responseType;
                        p->read(responseType);
                        Status s;
                        //TODO: Use macro RESPONSE_SOLICITED instead of 0
                        if (responseType == 0) {
                            // TODO: Handle response in a separate thread?
                            s = handleResponse(p);
                            if (s != Status::SUCCESS) {
                                QCRIL_HAL_LOG_DEBUG("Failed to process response.");
                            }
                        } else if (responseType == 1) {
                            handleIndication(p);
                        }
                    }
                }
            }
        }

        // Fatal error or end-of-stream
        if (ret == 0 || !(errsv == EAGAIN || errsv == EINTR)) {
            if(ret != 0) {
                QCRIL_HAL_LOG_DEBUG(
                        "Error on reading socket recordLen: %zu, ret: %d, errno: %d, errStr: %s, socket fd: %d",
                        recordLen, ret, ret ? errsv : 0, strerror(errsv), socketFd);
            } else {
                QCRIL_HAL_LOG_DEBUG(
                        "EOS on socket recordLen: %zu, ret: %d, errno: %d, errStr: %s, socket fd: %d",
                        recordLen, ret, ret ? errsv : 0, strerror(errsv), socketFd);
            }
            break;
        }
    }

    handleResponseListenerFailure();
}

Status RequestManager::handleResponse(std::shared_ptr<Marshal> p) {
    if (p == nullptr) {
        return Status::FAILURE;
    }

    // TODO: static assert sizeof(uint64_t) == sizeof(GenericResponseCallback*)
    // TODO: check for errors during unmarshalling
    uint64_t token;
    uint32_t errorCode;
    p->read(token);
    p->read(errorCode);

    std::unique_ptr<GenericResponseCallback> cb;

    {
        std::lock_guard<std::mutex> lock(requestsWaitingForResponseMutex);

        auto iter = requestsWaitingForResponse.find(token);
        if (iter == requestsWaitingForResponse.end()) {
            QCRIL_HAL_LOG_DEBUG("Cannot find request with token %" PRIu64, token);
            return Status::FAILURE;
        } else {
            if (iter->second == nullptr) {
                QCRIL_HAL_LOG_DEBUG(
                    "Response callback is null for request with token %" PRIu64, token);
                return Status::FAILURE;
            } else {
                cb = std::move(iter->second);
                //(*(iter->second))(static_cast<RIL_Errno>(errorCode), p);
            }
            requestsWaitingForResponse.erase(iter);
        }
    }

    if (cb != nullptr) {
        (*cb)(static_cast<RIL_Errno>(errorCode), p);
    }

    return Status::SUCCESS;
}

Status RequestManager::handleIndication(std::shared_ptr<Marshal> p) {
    if (p == nullptr) {
        return Status::FAILURE;
    }

    // TODO: check for errors during unmarshalling
    uint32_t indicationId;
    p->read(indicationId);

    GenericIndicationHandler indicationHandler;

    {
        std::lock_guard<std::mutex> lock(indicationHandlersMutex);
        auto iter = indicationHandlers.find(indicationId);
        if (iter == indicationHandlers.end()) {
            return Status::FAILURE;
        } else {
            if (iter->second == nullptr) {
                return Status::FAILURE;
            } else {
                //GenericIndicationHandler& indHandler = *(iter->second);
                //indicationHandler = indHandler;
                indicationHandler = *(iter->second);
            }
        }
    }

    if (indicationHandler) {
        indicationHandler(p);
    }

    return Status::SUCCESS;
}

Status RequestManager::issueRequest(int32_t requestId, std::unique_ptr<Marshal> p, const GenericResponseCallback& cb) {
    if (isSocketErrorDetected() || isDisconnectionRequested()) {
        QCRIL_HAL_LOG_DEBUG("Requests aren't currently allowed.");
        return Status::FAILURE;
    }

    QCRIL_HAL_LOG_DEBUG("Pushing a request to the queue.");

    std::unique_ptr<GenericResponseCallback> cbCopy = std::make_unique<GenericResponseCallback>(cb);
    if (cbCopy == nullptr) {
        return Status::FAILURE;
    }

    {
        std::lock_guard<std::mutex> lock(requestsWaitingToBeIssuedMutex);
        requestsWaitingToBeIssued.push(std::make_tuple(requestId, std::move(p), std::move(cbCopy)));
    }
    requestNotifier.notify_one();
    QCRIL_HAL_LOG_DEBUG("Pushed a request to the queue.");

    return Status::SUCCESS;
}

Status RequestManager::registerIndicationHandler(int32_t indicationId, const GenericIndicationHandler& indicationHandler) {
    QCRIL_HAL_LOG_DEBUG("Registering indication handler for indication %" PRId32 "", indicationId);

    std::unique_ptr<GenericIndicationHandler> indicationHandlerCopy = std::make_unique<GenericIndicationHandler>(indicationHandler);
    if (indicationHandlerCopy == nullptr) {
        return Status::FAILURE;
    }

    {
        std::lock_guard<std::mutex> lock(indicationHandlersMutex);
        indicationHandlers[indicationId] = std::move(indicationHandlerCopy);
    }

    QCRIL_HAL_LOG_DEBUG("Registered indication handler for indication %" PRId32 "", indicationId);
    return Status::SUCCESS;
}

Status RequestManager::hasValidIndicationHandler(int32_t indicationId) {
    std::lock_guard<std::mutex> lock(indicationHandlersMutex);
    auto iter = indicationHandlers.find(indicationId);
    if (iter == indicationHandlers.end()) {
        return Status::FAILURE;
    } else {
        if (iter->second != nullptr) {
                return Status::SUCCESS;
        }
    }

    return Status::FAILURE;
}

void RequestManager::flushRequestsWaitingForResponse() {
    {
        std::lock_guard<std::mutex> lock(requestsWaitingForResponseMutex);
        std::for_each(
            requestsWaitingForResponse.begin(),
            requestsWaitingForResponse.end(),
            [] (const auto& iter) {
                if (iter.second != nullptr) {
                    (*(iter.second))(RIL_E_SYSTEM_ERR, nullptr);
                }
            }
        );
        requestsWaitingForResponse.clear();
    }
}

void RequestManager::flushRequestsWaitingToBeIssued() {
    {
        std::lock_guard<std::mutex> lock(requestsWaitingToBeIssuedMutex);
        std::unique_ptr<GenericResponseCallback> cb;
        while (!requestsWaitingToBeIssued.empty()) {
            cb = std::move(std::get<2>(requestsWaitingToBeIssued.front()));
            if (cb != nullptr) {
                (*cb)(RIL_E_SYSTEM_ERR, nullptr);
            }
            requestsWaitingToBeIssued.pop();
        }
    }
}

#ifdef QCRIL_BUILD_AFL_TESTCASE_GEN
void RequestManager::setSaveTestcases(bool value) {
    saveTestcases = value;
}

bool RequestManager::getSaveTestcases() {
    return saveTestcases;
}

std::string RequestManager::getTestcaseOutDir() {
    return testcaseOutDir;
}

Status RequestManager::setTestcaseOutDir(std::string path) {
    Status s = Status::FAILURE;
    std::string mkdir_err, dir_check_err;
    dir_check_result_t dir_check;
    QCRIL_HAL_LOG_DEBUG("Setting output path to write new testcases: %s.", path.c_str());
    {
        std::scoped_lock lock(testcaseOutDirMutex);

        // Remove trailing '/' unless the path is just the root directory.
        if (path != "/" && path.back() == '/') {
            path.pop_back();
        }

        // Check if path exists.
        dir_check = fs_utils_check_dir(path, dir_check_err);
        if (!dir_check_err.empty()) {
            QCRIL_HAL_LOG_DEBUG("Directory check error: %s", dir_check_err.c_str());
        }
        if (dir_check == IS_FILE) {
            QCRIL_HAL_LOG_DEBUG("Path %s is not a directory."
                                "Please use a directory for the output path.",
                                path.c_str());
            return s;
        }
        else if (dir_check == IS_DIR) {
            QCRIL_HAL_LOG_DEBUG("The directory %s already exists.", path.c_str());
            testcaseOutDir = path;
            s = Status::SUCCESS;
        }
        else {
            QCRIL_HAL_LOG_DEBUG("Directory %s does not currently exist. Creating directory.",
                                path.c_str());
            s = fs_utils_mkdir(path, mkdir_err);
            if (s == Status::SUCCESS) {
                testcaseOutDir = path; // Use the directory we just created.
            }
            else {
                if (!mkdir_err.empty()) {
                    QCRIL_HAL_LOG_DEBUG("Error when creating directory %s. Error: %s",
                                path.c_str(), mkdir_err.c_str());
                }
                else {
                    QCRIL_HAL_LOG_DEBUG("Failed to create directory %s", path.c_str());
                }
            }

        }

    }
    return s;
}
#endif // QCRIL_BUILD_AFL_TESTCASE_GEN
