/******************************************************************************
  @file    OptsHandlerExtn.cpp
  @brief   Implementation of performance server module extn

  DESCRIPTION

  ---------------------------------------------------------------------------
  Copyright (c) 2011-2015,2017,2020-2022 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ---------------------------------------------------------------------------
******************************************************************************/

#include "HintExtHandler.h"
#include "OptsHandlerExtn.h"

#undef FAILED
#undef SUCCESS

#include "ioctl.h"
#include <cutils/android_filesystem_config.h>
#include <cutils/trace.h>

#include <aidl/vendor/qti/hardware/display/config/IDisplayConfig.h>
#include <aidl/vendor/qti/hardware/perf2/BnPerf.h>
#include <android/binder_manager.h>
#include <android/binder_process.h>

#define SLEEP_TIME_PER_ITR 100000   //100ms
#define TOTAL_ITR 400

#define SLEEP_TIME_PER_ITR_SL 60000000  //60sec
#define TOTAL_ITR_SL 300 //5 hr
#define BASE_FPS 60




using ::ndk::SpAIBinder;
using aidl::vendor::qti::hardware::display::config::IDisplayConfig;
using aidl::vendor::qti::hardware::display::config::DisplayType;
using aidl::vendor::qti::hardware::display::config::Attributes;

enum returnValue {
    FAILED = -1,
    SUCCESS = 0
};




class DisplayExtn {
    private:
        pthread_t mNoroot_tid;
        bool mIsNorootThreadAlive;
        EventQueue mNRevqueue;

        // callbacks for EventQueue
        static void *Alloccb() {
            void *mem = (void *) new(std::nothrow) noroot_args;
            return mem;
        }

        static void Dealloccb(void *mem) {
            if (mem) {
                delete (noroot_args *)mem;
            }
        }
        DisplayExtn() {
            mIsNorootThreadAlive = false;
        }
        DisplayExtn(DisplayExtn const& oh);
        DisplayExtn& operator=(DisplayExtn const& oh);

        static void *noroot_thread(void *) {
            DisplayExtn &de = getDisplayExtn();
            EventQueue *NRevqueue = de.getNRQueue();
            //TODO Need to Revisit
            int32_t retval = SUCCESS;
            int32_t cmd;
            noroot_args *msg = NULL;
            bool trace_flag = Target::getCurTarget().isTraceEnabled();
            char trace_buf[TRACE_BUF_SZ] = {0};

            /* We must drop the root privileges before opening the DRM node */
            if (setuid(AID_SYSTEM)) {
                QLOGE(LOG_TAG, "Failed to drop root UID: %s. Cannot open the DRM node", strerror(errno));
                de.setNorootThreadAlive(false);
                pthread_exit(NULL);
            }

            de.setNorootThreadAlive(true);

            /* Main loop */
            for (;;) {
                EventData *evData = NRevqueue->Wait();

                if (!evData || !evData->mEvData) {
                    continue;
                }

                cmd = evData->mEvType;
                msg = (noroot_args *)evData->mEvData;
                if (trace_flag) {
                    snprintf(trace_buf, TRACE_BUF_SZ,"disp_early_wakeup: %d ", cmd);
                    ATRACE_BEGIN(trace_buf);
                }

                switch (cmd) {
                    case DISPLAY_EARLY_WAKEUP_HINT:
                        retval = early_wakeup_ioctl(msg->val);
                        msg->retval = retval;
                        break;
                    default:
                        QLOGE(LOG_TAG, "Unsupported cmd: %d", cmd);
                        break;
                }
                if (trace_flag) {
                    ATRACE_END();
                }

                NRevqueue->GetDataPool().Return(evData);
            }
            return NULL;
        }
    public:

        int32_t initNRThread() {
            int32_t retval = SUCCESS;
            mNRevqueue.GetDataPool().SetCBs(Alloccb, Dealloccb);
            retval = pthread_create(&mNoroot_tid, NULL, noroot_thread, NULL);

            if (retval != SUCCESS) {
                QLOGE(LOG_TAG, "Could not create noroot_thread");
            } else {
                retval = pthread_setname_np(mNoroot_tid, MPCTL_NO_ROOT_NAME);
                if (retval != SUCCESS) {
                    QLOGE(LOG_TAG, "Failed to name no_root Thread: %s", MPCTL_NO_ROOT_NAME);
                }
            }
            return retval;
        }

        void setNorootThreadAlive(bool value) {
            mIsNorootThreadAlive = value;
        }
        bool getIsNorootThreadAlive() {
            return mIsNorootThreadAlive;
        }

        float getDisplayFPS() {
            float fps = BASE_FPS;
            int32_t ret = 0;
            int32_t dpy_index = -1;
            Attributes dpy_attr;

            ndk::SpAIBinder binder(AServiceManager_checkService(
                        "vendor.qti.hardware.display.config.IDisplayConfig/default"));
            if (binder.get() == nullptr) {
                QLOGE(LOG_TAG, "DisplayConfig AIDL is not present");
            } else {
                auto aidlDisplayIntf = IDisplayConfig::fromBinder(binder);
                if (aidlDisplayIntf == nullptr) {
                      QLOGE(LOG_TAG, "displayConfig is null");
                } else {
                    aidlDisplayIntf->getActiveConfig(DisplayType::PRIMARY, &dpy_index);
                    if (dpy_index >= 0) {
                        aidlDisplayIntf->getDisplayAttributes(dpy_index, DisplayType::PRIMARY, &dpy_attr);
                        if (dpy_attr.vsyncPeriod) {
                            fps = NSEC_TO_SEC/dpy_attr.vsyncPeriod;
                            QLOGL(LOG_TAG, QLOG_L2, "Panel Attributes vsyncPeriod: %" PRId32 "Fps: %f", dpy_attr.vsyncPeriod, fps);
                        }
                    }
                }
            }
            updateFPSFile(fps);
            return fps;
        }

        bool updateFPSFile(float &fps) {
            FILE *fpsVal = NULL;
            char buf[NODE_MAX] = {0};
            memset(buf,0,sizeof(buf));
            fpsVal = fopen(CURRENT_FPS_FILE, "w");
            if (fpsVal == NULL) {
                QLOGE(LOG_TAG, "Cannot open/create fps value file");
                return false;
            }
            snprintf(buf, NODE_MAX, "%f", fps);
            fwrite(buf, sizeof(char), strlen(buf), fpsVal);
            fclose(fpsVal);
            return true;
        }

        float readFPSFile() {
            FILE *fpsVal = NULL;
            float fps = BASE_FPS;
            fpsVal = fopen(CURRENT_FPS_FILE, "r");
            if (fpsVal != NULL) {
                fscanf(fpsVal,"%f", &fps);
                fclose(fpsVal);
                QLOGL(LOG_TAG, QLOG_L2, "Read Value from file %f", fps);
            }
            return fps;
        }

        EventQueue *getNRQueue() {
            return &mNRevqueue;
        }

        static DisplayExtn &getDisplayExtn() {
            static DisplayExtn mDisplayExtn;
            return mDisplayExtn;
        }

};



int32_t init_nr_thread() {
    return DisplayExtn::getDisplayExtn().initNRThread();
}

float get_display_fps() {
    return DisplayExtn::getDisplayExtn().getDisplayFPS();
}

bool set_fps_file(float fps) {
    return DisplayExtn::getDisplayExtn().updateFPSFile(fps);
}

float get_fps_file() {
    return DisplayExtn::getDisplayExtn().readFPSFile();
}

bool is_no_root_alive() {
    return DisplayExtn::getDisplayExtn().getIsNorootThreadAlive();
}

EventQueue *get_nr_queue() {
    return DisplayExtn::getDisplayExtn().getNRQueue();
}
int32_t post_boot_init_completed() {

    char boot_completed[PROPERTY_VALUE_MAX];
    uint8_t value = 0, boot_count = 0;
    //wait 40sec and then exit
    QLOGV(LOG_TAG, "Checking boot complete");
    do {
        if (property_get(BOOT_CMPLT_PROP, boot_completed, "0")) {
            value = atoi(boot_completed);
        }
        if (!value){
            QLOGL(LOG_TAG, QLOG_L1, "Waiting for 1st Boot 0.1 sec...");
            usleep(SLEEP_TIME_PER_ITR);
        }
        if (boot_count > TOTAL_ITR) {
            QLOGE(LOG_TAG, "First Boot check over.Going to 2nd Boot check.");
            break;
        }
        boot_count++;
    } while(!value);

    /* Adding one more boot check to handle case like OTA, this is less frequent
     *  check to incur less overhead on system while checking
     */
    //wait longer and then exit
    boot_count=0;
    while(!value) {
        if (property_get(BOOT_CMPLT_PROP, boot_completed, "0")) {
            value = atoi(boot_completed);
        }
        if (!value){
            QLOGL(LOG_TAG, QLOG_L1, "Waiting for 2nd Boot 60 sec...");
            usleep(SLEEP_TIME_PER_ITR_SL);
        }
        if (boot_count > TOTAL_ITR_SL) {
            QLOGE(LOG_TAG, "Second Boot check attempt reached. Exiting.....");
            return FAILED;
        }
        boot_count++;
    }
    return SUCCESS;
}
