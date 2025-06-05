
#include "HintExtHandler.h"
#include "OptsHandlerExtn.h"

#undef FAILED
#undef SUCCESS

#include "ioctl.h"
#include <cutils/android_filesystem_config.h>
#include <cutils/trace.h>

#include <aidl/vendor/qti/hardware/perf2/BnPerf.h>
#include <aidl/vendor/qti/memory/pasrmanager/IPasrManager.h>
#include <android/binder_manager.h>
#include <android/binder_process.h>

using aidl::vendor::qti::memory::pasrmanager::IPasrManager;
using aidl::vendor::qti::memory::pasrmanager::PasrStatus;
using aidl::vendor::qti::memory::pasrmanager::PasrPriority;
using aidl::vendor::qti::memory::pasrmanager::PasrSrc;
using aidl::vendor::qti::memory::pasrmanager::PasrState;

enum returnValue {
    FAILED = -1,
    SUCCESS = 0
};


class PASRComm {
    std::shared_ptr<IPasrManager> mPasrManagerAidl = nullptr;
    bool mPASREnabled = false;
    const std::string instance = std::string() + IPasrManager::descriptor + "/default";
    PASRComm() {
        ndk::SpAIBinder binder(AServiceManager_checkService(instance.c_str()));
        if (binder.get() == nullptr) {
            QLOGE(LOG_TAG, "AIDL pasrmanager HAL does not exist");
        } else {
            mPasrManagerAidl = IPasrManager::fromBinder(binder);
        }

        if (mPasrManagerAidl && !access(MEM_OFFLINE_NODE, F_OK)) {
            mPASREnabled = true;
        }
    }
    PASRComm(PASRComm const& oh);
    PASRComm& operator=(PASRComm const& oh);
    public:
    bool isPASREnabled() {
        return mPASREnabled;
    }

    int32_t PASREntry() {
        PasrSrc src = PasrSrc::PASR_SRC_PERF;
        PasrPriority pri = PasrPriority::PASR_PRI_CRITICAL;
        PasrState state = PasrState::MEMORY_ONLINE;
        PasrStatus status = PasrStatus::ERROR;

        if (!mPASREnabled)
            return SUCCESS;    //nothing to do for pasr

        /* Enter into critical state for Online */
        mPasrManagerAidl->stateEnter(src, pri, state, &status);

        if (status != PasrStatus::SUCCESS) {
            QLOGE(LOG_TAG, "Entering critical online state for pasrmanager failed!");
            return FAILED;
        }

        /* Attempt to Online all blocks */
        mPasrManagerAidl->attemptOnlineAll(src, pri, &status);
        if(status != PasrStatus::ONLINE) {
            QLOGE(LOG_TAG, "Attempting to online all blocks failed!");
            return FAILED;
        }

        QLOGL(LOG_TAG, QLOG_L2, "Onlined all blocks for perf mode entry");
        return SUCCESS;
    }

    int32_t PASRExit() {
        PasrSrc src = PasrSrc::PASR_SRC_PERF;
        PasrStatus status = PasrStatus::ERROR;

        if (!mPASREnabled)
            return SUCCESS;    //nothing to do for pasr

        /* Exit from critical state for Online */
        mPasrManagerAidl->stateExit(src, &status);

        if(status != PasrStatus::SUCCESS) {
            QLOGE(LOG_TAG, "Exiting from critical online state for pasrmanager failed!");
            return FAILED;
        }

        QLOGV(LOG_TAG, "Exit from perf mode successful for pasrmanager");
        return SUCCESS;
    }

    static PASRComm &getPASRService() {
        static PASRComm mPASR;
        return mPASR;
    }
};

int32_t init_pasr() {
    PASRComm &PASR = PASRComm::getPASRService();
    if (PASR.isPASREnabled()) {
        return SUCCESS;
    }
    return FAILED;
}

int pasr_entry_func(Resource &, OptsData &) {
    PASRComm &PASR = PASRComm::getPASRService();
    return PASR.PASREntry();
}

int pasr_exit_func(Resource &, OptsData &) {
    PASRComm &PASR = PASRComm::getPASRService();
    return PASR.PASRExit();
}
