/******************************************************************************
#  Copyright (c) 2021 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#define TAG "QCRILSocketModConfig"
#include <SocketRequestContext.h>
#include <telephony/ril.h>
#include <interfaces/module_config/ModuleConfig.h>
#include <marshal/ModuleConfig.hpp>
#include <ril_socket_api.h>
#include <memory>

namespace ril {
namespace socket {
namespace api {

void dispatchGetModuleList(std::shared_ptr<SocketRequestContext> context, Marshal& p) {
    std::optional<std::string> config;
    if (p.read(config) == Marshal::Result::SUCCESS) {
        auto msg = std::make_shared<ModuleConfig::GetModuleListMessage>(config);
        if (!msg) {
            QCRIL_LOG_ERROR("Unable to allocate message");
            sendResponse(context, RIL_E_NO_MEMORY, nullptr);
            return;
        }
        msg->dispatch(
                [context](std::shared_ptr<Message> msg, Message::Callback::Status status,
                    std::shared_ptr<ModuleConfig::ModuleList> response) {
                    RIL_Errno errorCode = dispatchStatusToRilErrno(status);
                    std::shared_ptr<Marshal> p = nullptr;
                    if (status == Message::Callback::Status::SUCCESS) {
                        p = std::make_shared<Marshal>();
                        if (p) {
                            if (response) {
                                Marshal::Result e;
                                e = p->write(response);
                                if (e != Marshal::Result::SUCCESS) {
                                    QCRIL_LOG_ERROR("Error writing to marshal: %d", (int)e);
                                    p = nullptr;
                                    errorCode = RIL_E_INTERNAL_ERR;
                                } else {
                                    QCRIL_LOG_DEBUG("Marshal size: %zu available: %zu", p->dataSize(),
                                                p->dataAvail());
                                }
                            }
                        }
                    }
                    sendResponse(context, errorCode, p);
                }
        );
    }
}

void dispatchGetConfigList(std::shared_ptr<SocketRequestContext> context, Marshal &p) {
    auto msg = std::make_shared<ModuleConfig::GetConfigListMessage>();
    if (!msg) {
        QCRIL_LOG_ERROR("Unable to allocate message");
        sendResponse(context, RIL_E_NO_MEMORY, nullptr);
        return;
    }
    msg->dispatch(
            [context](std::shared_ptr<Message> msg, Message::Callback::Status status,
                std::shared_ptr<std::vector<std::string>> response) {
                RIL_Errno errorCode = dispatchStatusToRilErrno(status);
                std::shared_ptr<Marshal> p = nullptr;
                if (status == Message::Callback::Status::SUCCESS) {
                    p = std::make_shared<Marshal>();
                    if (p) {
                        if (response) {
                            Marshal::Result e;
                            e = p->write(*response);
                            if  (e != Marshal::Result::SUCCESS) {
                                QCRIL_LOG_ERROR("Error writing to marshal: %d", (int)e);
                                p = nullptr;
                                errorCode = RIL_E_INTERNAL_ERR;
                            }
                        }
                    }
                }
                sendResponse(context, errorCode, p);
            }
    );
}

void dispatchGetCurrentConfig(std::shared_ptr<SocketRequestContext> context, Marshal& p) {
    auto msg = std::make_shared<ModuleConfig::GetCurrentConfigMessage>();
    if (!msg) {
        QCRIL_LOG_ERROR("Unable to allocate message");
        sendResponse(context, RIL_E_NO_MEMORY, nullptr);
        return;
    }
    msg->dispatch(
            [context](std::shared_ptr<Message> msg, Message::Callback::Status status,
                std::shared_ptr<std::string> response) {
                RIL_Errno errorCode = dispatchStatusToRilErrno(status);
                std::shared_ptr<Marshal> p = nullptr;
                if (status == Message::Callback::Status::SUCCESS) {
                    p = std::make_shared<Marshal>();
                    if (p) {
                        if (response) {
                            QCRIL_LOG_DEBUG("Got response. Value: %s", response->c_str());
                            Marshal::Result e;
                            e = p->write(response);
                            if (e != Marshal::Result::SUCCESS) {
                                QCRIL_LOG_ERROR("Error writing to marshal: %d", (int)e);
                                p = nullptr;
                                errorCode = RIL_E_INTERNAL_ERR;
                            } else {
                                QCRIL_LOG_ERROR("Successfully wrote to marshal");
                                QCRIL_LOG_DEBUG("Data Size: %zu. Data Available: %zu. Data Position: %zu",
                                        p->dataSize(),
                                        p->dataAvail(),
                                        p->dataPosition());
                            }
                        }
                    }
                }
                QCRIL_LOG_DEBUG("Sending response: errorCode: %d. pointer: %pK", (int) errorCode, p.get());
                sendResponse(context, errorCode, p);
            }
    );
}

void dispatchSetCurrentConfig(std::shared_ptr<SocketRequestContext> context, Marshal& p) {
    std::string config;
    if (p.read(config) != Marshal::Result::SUCCESS) {
        sendResponse(context, RIL_E_INTERNAL_ERR, nullptr);
        QCRIL_LOG_ERROR("Unable to read from Marshal");
        return;
    }

    auto msg = std::make_shared<ModuleConfig::SetCurrentConfigMessage>(config);
    if (!msg) {
        QCRIL_LOG_ERROR("Unable to allocate message");
        sendResponse(context, RIL_E_NO_MEMORY, nullptr);
        return;
    }

    msg->dispatch(
            [context](std::shared_ptr<Message> msg, Message::Callback::Status status,
                std::shared_ptr<void> response) {
                RIL_Errno errorCode = dispatchStatusToRilErrno(status);
                sendResponse(context, errorCode, nullptr);
            }
    );
}

void dispatchSetCustomConfigModules(std::shared_ptr<SocketRequestContext> context, Marshal& p) {
    std::vector<std::string> modules;
    if (p.read(modules) != Marshal::Result::SUCCESS) {
        sendResponse(context, RIL_E_INTERNAL_ERR, nullptr);
        QCRIL_LOG_ERROR("Unable to read module list from Marshal");
        return;
    }

    auto msg = std::make_shared<ModuleConfig::SetCustomConfigModules>(modules);
    if (!msg) {
        QCRIL_LOG_ERROR("Unable to allocate message");
        sendResponse(context, RIL_E_NO_MEMORY, nullptr);
        return;
    }

    msg->dispatch(
            [context](std::shared_ptr<Message> msg, Message::Callback::Status status,
                std::shared_ptr<void> response) {
                RIL_Errno errorCode = dispatchStatusToRilErrno(status);
                sendResponse(context, errorCode, nullptr);
            }
    );

}

}}}
