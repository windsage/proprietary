/******************************************************************************
#  Copyright (c) 2017,2020 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#define TAG "RILQ-CELLINFO"
#include <framework/Module.h>
#include <qtiril-loadable/QtiRilLoadable.h>
#include <framework/ThreadPoolManager.h>
#include "interfaces/cellinfo/RilRequestGetCellInfoListMessage.h"

void qcril_qmi_nas_get_cell_info_list_ncl
(
    std::shared_ptr<RilRequestGetCellInfoListMessage> msg
);

class cellinfo_module: public Module
{
    private:
    public:
        void handle_get_cell_info_list(std::shared_ptr<RilRequestGetCellInfoListMessage> msg);
        cellinfo_module();
};



cellinfo_module::cellinfo_module()
{
    mName = "cellinfo_module";
    using std::placeholders::_1;

    mMessageHandler = {
        HANDLER(RilRequestGetCellInfoListMessage, cellinfo_module::handle_get_cell_info_list),
    };
}

void qcril_qmi_nas_get_cell_info_list_ncl_wrapper(void* cb_data)
{
    auto schedCbData = static_cast<ScheduleCallbackData*>(cb_data);
    auto msg(std::static_pointer_cast<RilRequestGetCellInfoListMessage>(schedCbData->getData()));
    delete schedCbData;
    qcril_qmi_nas_get_cell_info_list_ncl(msg);
}


void cellinfo_module::handle_get_cell_info_list(std::shared_ptr<RilRequestGetCellInfoListMessage> msg)
{
    QCRIL_LOG_FUNC_ENTRY();
    auto cbData = new ScheduleCallbackData(msg);
    if (cbData) {
        ThreadPoolManager::getInstance().scheduleExecution(
            qcril_qmi_nas_get_cell_info_list_ncl_wrapper, cbData);
    } else {
        msg->sendResponse(msg, Message::Callback::Status::SUCCESS,
            std::make_shared<QcRilRequestMessageCallbackPayload>(RIL_E_NO_MEMORY, nullptr));
    }
    QCRIL_LOG_FUNC_RETURN();
}

#if defined(QTIRIL_STATIC_MODULES) || defined(QMI_RIL_UTF)
static load_module<cellinfo_module> the_module;
#else
static struct LoadableHeader *theHeader = nullptr;
static QtiSharedMutex theHeaderMutex;
extern "C"
struct LoadableHeader *loadable_init() {
    std::unique_lock<QtiSharedMutex> lock(theHeaderMutex);
    if (theHeader == nullptr) {
        std::shared_ptr<cellinfo_module> theModule = std::make_shared<cellinfo_module>();
        if (theModule) {
            theModule->init();
        }
        struct LoadableHeader *header = new LoadableHeader{"cellinfo_module", std::static_pointer_cast<Module>(theModule)};
        theHeader = header;
    }
    return theHeader;
}

extern "C"
int loadable_deinit() {
    std::unique_lock<QtiSharedMutex> lock(theHeaderMutex);
    if (theHeader != nullptr) {
        delete theHeader;
    }
    return 0;
}
#endif
