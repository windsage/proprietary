/******************************************************************************
 *   @file    LearningModule.cpp
 *   @brief   Implementation of Android Framework Learning Module
 *
 *   DESCRIPTION
 *      Learning Module is a framework that enables "Feature"s to be developed
 *    to "learn" at runtime and apply actions based on such learning.
 *    Each "Feature" contains a triplet - "Meters, Algorithm, Action". Feature
 *    takes "Input" from its Meters based on triggers from the system and
 *    stores it in an Input Database. Algorithm takes the stored Input from
 *    Database, "learns" from input and writes output for the Action to an
 *    Output Database. Action uses the Output Database to apply actions.
 *      Learning Module class provides the core functionality of the framework.
 *    LM creates a thread (lmCoreThread) on initialization and all further
 *    logic is dispatched to this dedicated thread to relieve the caller
 *    (perf-hal). The thread starts by initializing the framework and features.
 *    Once initialization is done, LM waits for triggers from perf-hal.
 *
 *---------------------------------------------------------------------------
 *    Copyright (c) 2017 Qualcomm Technologies, Inc.
 *    All Rights Reserved.
 *    Confidential and Proprietary - Qualcomm Technologies, Inc.
 *---------------------------------------------------------------------------
 *******************************************************************************/

#include <dirent.h>
#include <dlfcn.h>
#include <fstream>
#include <errno.h>
#include <cutils/properties.h>
#define ATRACE_TAG ATRACE_TAG_ALWAYS
#include <cutils/trace.h>
#include "internal/LearningModule.h"
#include "internal/FeatureXML.h"
#include "internal/BatteryInfo.h"
#include "Registry.h"
#include "lmDB.h"
#include "FeedbackAPI.h"
#include "DynamicLoader.h"

#define LM_THREAD_NAME "lmCoreThread"

/* MAX defines are to be used for rate control */
#define MAX_CONCURR_METAMETERS 1
#define MAX_CONCURR_ALGOS 1
#define IDLE_BATTERY_THRESHOLD 50
#define TARGET_VENDOR_LM_DIR "/vendor/etc/lm/"
/* Use TARGET_DATA_LM_DIR defined in LMDB */
#define TARGET_DATA_LM_DIR LMDB_ROOT_PATH
#define LIBMETERS_NAME "libmeters.so"
#define WAIT_RETRIES_FOR_DATA_DIR 20
#define TRIGGERQ_FLUSH_THRESHOLD 50
#define BOOT_CMPLT_PROP "vendor.post_boot.parsed"

using namespace std;

/* Instantiate featureList of FeatureRegistry */
using FeatureRegistryBase = RegistryBase<Feature, FeatureInfo>;
template<> FeatureRegistryBase::NameClassMap* FeatureRegistryBase::classList = nullptr;
template<> mutex FeatureRegistryBase::mMutex {};

/* Instantiate classList of MeterRegistry */
using MeterRegistryBase = RegistryBase<Meter, MeterInfo>;
template<> MeterRegistryBase::NameClassMap* MeterRegistryBase::classList = nullptr;
template<> mutex MeterRegistryBase::mMutex {};

PerfOutputFeedback* PerfOutputFeedback::mInstance = NULL;
FeatureLocator FeatureLocator::mInstance;

pthread_mutex_t PerfOutputFeedback::mMutex = PTHREAD_MUTEX_INITIALIZER;
LearningModule::LearningModule() : mTriggerQueue("LMTriggerQ", TRIGGERQ_FLUSH_THRESHOLD),  mFeatureLocator(FeatureLocator::getInstance()) {
    /*
     * All LM inits are done from LM Thread to relieve caller (Perf HAL) for
     * further initializations.
     */

    /* create and init thread infrastructure */
    lmThreadReady = false;
    DEBUGV(LOG_TAG_LM,"LM Constructor");
    idleAlgoThrBusy = false;
    algoThreadInterrupt = false;
    mEnableLMtracing = (property_get_bool("vendor.debug.trace.perf", 0) == 1);
    try {
        lmThread = thread(&LearningModule::lmMain, this);
    } catch(...) {
        DEBUGE(LOG_TAG_LM,"Thread Constructor Exception");
    }
    mOutputFB = PerfOutputFeedback::getInstance();
    if (NULL != mOutputFB) {
        mOutputFB->setFeatureLocator(&mFeatureLocator);
    }
}

LearningModule::~LearningModule() {
    DEBUGV(LOG_TAG_LM,"LM destructor, deleting objects");

    /* Exit the LM Core Thread */
    if(lmThreadReady) {
        /* Signal the LM Core Thread if it's running */
        TriggerInfo exitThreadTrigger;
        exitThreadTrigger.exitLMThread = true;
        mTriggerQueue.push(exitThreadTrigger);
        /* The thread would exit only after all triggers are processed. */
    }
    if(lmThread.joinable()) {
        lmThread.join();
    }

    /* Clear the featureList */
    for (Feature* currFeature : mFeatureList) {
        delete currFeature;
        currFeature = nullptr;
    }
    mFeatureList.clear();

    /* De-init the Database */
    LMDBGlobal::LMDBGlobalDeConfig();
    /* Close dl handles */
    for(void* dlHandle : dlLibHandles) {
        if(dlHandle) {
            int ret_code = dlclose(dlHandle);
            if(ret_code!= 0) {
                DEBUGE(LOG_TAG_LM,"Unable to close dlopened library");
            }
        }
    }
    dlLibHandles.clear();
    DEBUGV(LOG_TAG_LM,"~LearningModule done");
}

int LearningModule::trigger(AsyncTriggerPayload &payload) {
    /* Don't start delivering triggers until LM Thread is ready */
    if(!lmThreadReady) {
        return -1;
    }

    try {

        if ((payload.hintID >= VENDOR_FEEDBACK_INPUT_HINT_BEGIN) &&
                (payload.hintID <= VENDOR_FEEDBACK_INPUT_HINT_END)) {
            //if lm feedback related, get feedback and return
            if (mOutputFB) {
                if (payload.hintID == VENDOR_FEEDBACK_GPU_HEADROOM ||
                        payload.hintID == VENDOR_FEEDBACK_MAX_PIPELINE_NUM)
                    return mOutputFB->getFeedback(payload.hintID, payload.appName, payload.hintType, payload.duration);
                else if (payload.hintID >= VENDOR_FEEDBACK_PA_FW && payload.hintID <= VENDOR_FEEDBACK_PA_ULMK) {
                    if (payload.feedback == nullptr)
                        return -1;
                    else
                        return mOutputFB->getFeedback(payload.hintID, payload.feedback);
                } else
                    return mOutputFB->getFeedback(payload.hintID, payload.appName, payload.hintType);
            }
        }

        /* Push the trigger to the TriggerQueue */
        TriggerInfo trigger;
        trigger.payload = payload;
        trigger.exitLMThread = false;
        mTriggerQueue.push(trigger);
    } catch (std::exception &e) {
        DEBUGV(LOG_TAG_LM, "Exception caught: %s in %s", e.what(), __func__);
    } catch (...) {
        DEBUGV(LOG_TAG_LM, "Exception caught in %s", __func__);
    }
    return 0;
}

void LearningModule::detectAndRunIdle(bool debugIdleTrigger) {
    idleAlgoThrBusy = true;
    /* If the trigger is not from debug trigger, run idle detection logic */
    DEBUGV(LOG_TAG_LM, "Checking for debug Idle Trigger = %s \n", debugIdleTrigger ? "true" : "false");
    if(!debugIdleTrigger) {
        /*
         * Open sysfs files and read battery status and level
         */
        DEBUGV(LOG_TAG_LM, "Reading Battery Status");
        ifstream batteryStatusFile(SYSFS_BATTERY_STATUS_PATH);
        if(!batteryStatusFile.is_open()) {
            DEBUGE(LOG_TAG_LM, "Not able to open %s, Error: %s",SYSFS_BATTERY_STATUS_PATH, strerror(errno));
            /* Not able to read battery charge status */
            idleAlgoThrBusy = false;
            return;
        }

        string batteryStatus;
        /* Assumes the sysfs file would contain single line */
        getline(batteryStatusFile,batteryStatus);
        BatteryStatusMap statusMap;
        BatteryStatus chargeStatus = statusMap[batteryStatus];
        DEBUGV(LOG_TAG_LM, "getChargeStatus() returned %d \n", chargeStatus);
        if(chargeStatus != BatteryStatus::CHARGING && chargeStatus != BatteryStatus::FULL) {
            DEBUGV(LOG_TAG_LM, "getChargeStatus() is NOT CHARGING \n");
            /* Battery is not charging don't run algo's*/
            idleAlgoThrBusy = false;
            return;
        }

        DEBUGV(LOG_TAG_LM, "getChargeStatus() is CHARGING / FULL \n");
        unsigned int batteryLevel;
        do {
            ifstream batteryLevelFile(SYSFS_BATTERY_LEVEL_PATH);
            if(!batteryLevelFile.is_open()) {
                DEBUGE(LOG_TAG_LM, "Not able to open %s, Error: %s",SYSFS_BATTERY_LEVEL_PATH, strerror(errno));
                /* Not able to read battery charge level */
                idleAlgoThrBusy = false;
                return;
            }
            batteryLevelFile >> batteryLevel;
            DEBUGV(LOG_TAG_LM, "Battery Level is %u \n", batteryLevel);
            DEBUGV(LOG_TAG_LM,"interruptAlgoThread %p = %s", &algoThreadInterrupt, algoThreadInterrupt ? "true" : "false");
            if(batteryLevel < IDLE_BATTERY_THRESHOLD) {
                this_thread::sleep_for(5min);
            }
        } while (batteryLevel < IDLE_BATTERY_THRESHOLD && !algoThreadInterrupt);

        if(algoThreadInterrupt) {
            /* Interrupted from LM Core thread */
            idleAlgoThrBusy = false;
            return;
        }
    }

    for (Feature* currFeature : mFeatureList) {
        if(currFeature) {
            DEBUGV(LOG_TAG_LM,"Calling runAsyncIdle for %s", currFeature->getFeatureName().c_str());
            if(mEnableLMtracing) {
                string traceLog = "LM-runAsyncIdle() " + currFeature->getFeatureName();
                ATRACE_ASYNC_BEGIN(traceLog.c_str(), 0);
            }
            currFeature->runAsyncIdle(algoThreadInterrupt);
            if(mEnableLMtracing) {
                string traceLog = "LM-runAsyncIdle() " + currFeature->getFeatureName();
                ATRACE_ASYNC_END(traceLog.c_str(), 0);
            }
            DEBUGV(LOG_TAG_LM,"runAsyncIdle for %s returned", currFeature->getFeatureName().c_str());
        }
    }
    idleAlgoThrBusy = false;
}

void LearningModule::alterFeaturesRegistered(vector<Feature *> &featureList) {
    try {
        if (featureList.size() == 0) {
            DEBUGI(LOG_TAG_LM, "Registered Feature list is empty");
            return;
        }
        vector <string> listOfFeatureNames;
        vector <string> validFeatures;
        DEBUGI(LOG_TAG_LM, "Before shuffling, No.of LM features registered = %d", featureList.size());
        vector<Feature *>::iterator feature = featureList.begin();
        for (;feature != featureList.end(); ++feature)
            if (*feature != NULL)
                validFeatures.push_back((*feature)->getFeatureName());
        for (feature = featureList.begin(); feature != featureList.end(); ++feature) {
            string dependsOnFeature = "";
            if (*feature != NULL) {
                Feature * store = NULL;
                // For each feature push the name to a vector of strings
                listOfFeatureNames.push_back((*feature)->getFeatureName());
                DEBUGI(LOG_TAG_LM, "Feature = %s", (*feature)->getFeatureName().c_str());
                // If feature depends on certain name, search for the name in vector of strings and if present do nothing
                dependsOnFeature = (*feature)->getDependentFeature();
                if (dependsOnFeature != "") {
                    // check if parsed feature name is valid
                    if (find(validFeatures.begin(), validFeatures.end(), dependsOnFeature) == validFeatures.end()){
                        DEBUGE(LOG_TAG_LM, "Error: Invalid feature dependency %s for %s", dependsOnFeature.c_str(), (*feature)->getFeatureName().c_str());
                        continue;
                    }
                    if (find(listOfFeatureNames.begin(), listOfFeatureNames.end(), dependsOnFeature) == listOfFeatureNames.end()) {
                        // erase the feature
                        store = *feature;
                        featureList.erase(feature);
                        --feature;
                    }
                    if (store != NULL) { // and add back to the end.
                        featureList.push_back(store);
                        // Remove the entry in listOfFeatureNames so that the features are placed according to dependency
                        listOfFeatureNames.erase(std::remove(listOfFeatureNames.begin(), listOfFeatureNames.end(), (store)->getFeatureName()), listOfFeatureNames.end());
                    }
                }
            }
        }
    } catch (std::exception &e) {
        DEBUGE(LOG_TAG_LM, "Exception caught: %s in %s", e.what(), __func__);
    } catch (...) {
        DEBUGE(LOG_TAG_LM, "Exception caught in %s", __func__);
    }
    DEBUGI(LOG_TAG_LM, "After shuffling, No.of LM features registered = %d", featureList.size());
    return;
}

void LearningModule::lmMain() {
    int ret_code = 0;
    char boot_completed[PROPERTY_VALUE_MAX];
    int value = 0;

    DEBUGV(LOG_TAG_LM,"Checking boot complete ");
    do {
        if (property_get(BOOT_CMPLT_PROP, boot_completed, "0")) {
            value = atoi(boot_completed);
        }
        if(!value)
            usleep(100000);
    } while(!value);
    DEBUGV(LOG_TAG_LM,"Boot complete, post boot executed");
    DEBUGV(LOG_TAG_LM, "lmMain starting\n");

    if(mEnableLMtracing) {
        ATRACE_ASYNC_BEGIN("LM-Core Init", 0);
    }

    DEBUGV(LOG_TAG_LM, "calling pthread_setname_np()\n");
    ret_code = pthread_setname_np(pthread_self(),LM_THREAD_NAME);
    if (ret_code != 0) {
        DEBUGE(LOG_TAG_LM, "Failed to setname ThreadName: %s\n",
                LM_THREAD_NAME);
        return;
    }

    //Loading libperfconfig config to utilise it later in AsyncData.
    DynamicLoader::getInstance().DynamicLoading();

    /* Initialize LM */
    /* Initialize the Database */
    DEBUGV(LOG_TAG_LM, "LMDB initial config\n");
    LMDBGlobal::LMDBGlobalConfig();

    /* Load LM non-core shared libraries */
    DEBUGV(LOG_TAG_LM, "Loading libraries \n");
    void* handle = dlopen(LIBMETERS_NAME, RTLD_NOW | RTLD_LOCAL);
    if (!handle) {
        DEBUGE(LOG_TAG_LM,"Unable to load library %s: %s", LIBMETERS_NAME,dlerror());
        return;
    }
    try {
        dlLibHandles.push_back(handle);

        /*
         * Wait for data filesystem to become available.
         * This is to ensure that the db folder is ready before we call Feature constructors
         * failing which the constructors won't be able to initialize the db
         */
        unsigned int numRetries = 0;
        DIR* lmDataDir = NULL;
        while ((lmDataDir = opendir(TARGET_DATA_LM_DIR)) == NULL) {
            DEBUGE(LOG_TAG_LM, "Waiting for LM data dir: %s, Error: %s", TARGET_DATA_LM_DIR,strerror(errno));
            this_thread::sleep_for(1s);
            numRetries++;
            if(numRetries > WAIT_RETRIES_FOR_DATA_DIR) {
                DEBUGE(LOG_TAG_LM, "Timeout waiting for LM data dir: %s, exiting", TARGET_DATA_LM_DIR);
                return;
            }
        }

        /* Read the xml files from TARGET_VENDOR_LM_DIR and construct the Feature objects */
        /* Assumes sane usage of xml files and feature names:
         *      No check for multiple xml files with same feature name, etc.
         */
        DEBUGV(LOG_TAG_LM, "Parsing xml's \n");
        DIR* lmDir;
        if ((lmDir = opendir(TARGET_VENDOR_LM_DIR)) == NULL) {
            DEBUGE(LOG_TAG_LM, "Fatal: can't open feature xml dir: %s, Error: %s", TARGET_VENDOR_LM_DIR,strerror(errno));
            closedir(lmDataDir);
            return;
        }

        struct dirent* lmDirEnt;
        while ((lmDirEnt = readdir(lmDir)) != NULL) {
            string fileName = TARGET_VENDOR_LM_DIR + string(lmDirEnt->d_name);
            /* if file name contains .xml, use it to construct Feature objects */
            if (fileName.find(".xml") != string::npos) {
                DEBUGV(LOG_TAG_LM, "Calling getFeatureLibName for xml: %s",
                        fileName.c_str());
                FeatureXML currFeatureXML(fileName);
                string libName = currFeatureXML.getFeatureLibName();
                FeatureInfo currFeatureInfo = currFeatureXML.getFeatureInfo();
                if (currFeatureInfo.configsList.find("Enable") != currFeatureInfo.configsList.end()) {
                    int enableModule = strtol(currFeatureInfo.configsList["Enable"].c_str(), nullptr, 10);
                    if (!enableModule)
                        continue;
                }

                if(!libName.empty()) {
                    handle = dlopen(libName.c_str(), RTLD_NOW | RTLD_LOCAL);
                    if (!handle) {
                        DEBUGE(LOG_TAG_LM,"Unable to load library %s: %s", libName.c_str(),dlerror());
                    } else {
                        dlLibHandles.push_back(handle);
                        DEBUGV(LOG_TAG_LM, "Calling getFeature for xml: %s",
                                fileName.c_str());
                        if(!currFeatureInfo.name.empty()) {
                            /* RegistryBase::get() calls new operator for corresponding element */
                            Feature* currFeature = FeatureRegistryBase::get(currFeatureInfo);
                            if (currFeature != nullptr) {
                                if (currFeature->isEnabled()) {
                                    mFeatureList.push_back(currFeature);
                                    //Construct the trigger to feature map
                                    for(auto currAsyncId : currFeatureInfo.asyncIdList) {
                                        mAsyncFeatMap[currAsyncId.first].push_back(currFeature);
                                    }
                                } else {
                                    //feature was disabled dynamically by feature itself, delete the feature object as
                                    //we are not going to track, unloading lib will be taken care in lm destructor
                                    DEBUGV(LOG_TAG_LM, "Disabling %s, feature was dynamically disabled", libName.c_str());
                                    FeatureRegistryBase::remove(currFeatureInfo);
                                    delete currFeature;
                                }
                            }
                        } /*if(!currFeatureInfo.name.empty())*/
                    } /*if (!handle)*/
                } /*if(!libName.empty())*/
            } /*if (fileName.find(".xml") != string::npos)*/
        }
        closedir(lmDir);
        closedir(lmDataDir);

        if (mFeatureList.empty()) {
            //no features enabled, lm not needed
            DEBUGE(LOG_TAG_LM, "No features enabled, exiting..");
            return;
        }

        alterFeaturesRegistered(mFeatureList);

        /* Set lm thread readiness flag */
        lmThreadReady = true;

        if(mEnableLMtracing) {
            ATRACE_ASYNC_END("LM-Core Init", 0);
        }

        /* register feature list with feature locator */
        mFeatureLocator.registerFeatureList(mFeatureList);

        /* register feature locator with each feature */
        for (Feature* currFeature : mFeatureList) {
            currFeature->registerServiceLocator(&mFeatureLocator);
        }

        /* Create an algo thread per feature which can call feature algo periodically */
        for (Feature* currFeature : mFeatureList) {
            if (currFeature->enablePeriodic) {
                try {
                    currFeature->periodicAlgoThread = thread(&Feature::runPeriodic, currFeature);
                } catch(...) {
                    DEBUGE(LOG_TAG_LM,"Thread Constructor Exception");
                }
            }
            try {
                    currFeature->asyncDeliveryThread = thread(&Feature::runAsyncDelivery, currFeature);
                } catch(exception &e) {
                    DEBUGE(LOG_TAG_LM,"%s Async Delivery Thread creation failed %s",
                        currFeature->getFeatureName().c_str(), e.what());
                }
        }

        /* loop forever to receive triggers from perf HAL - exit from LM destructor */
        bool shouldExit = false;
        do {
            DEBUGV(LOG_TAG_LM,"Waiting on trigger Queue");
            TriggerInfo trigger = mTriggerQueue.pop();
            shouldExit = trigger.exitLMThread;
            if(!shouldExit) {
                if (trigger.payload.hintID == VENDOR_HINT_ACTIVITY_START ||
                    trigger.payload.hintID == VENDOR_HINT_ACTIVITY_RESUME ||
                    trigger.payload.hintID == VENDOR_HINT_PICARD_RENDER_RATE ||
                    trigger.payload.hintID == VENDOR_HINT_CONTENT_FPS ||
                    trigger.payload.hintID == VENDOR_HINT_ACTIVITY_PAUSE ||
                    trigger.payload.hintID == VENDOR_HINT_ACTIVITY_STOP) {
                    const char *eventType = "UNKNOWN";
                    switch (trigger.payload.hintID) {
                        case VENDOR_HINT_ACTIVITY_START:
                            eventType = "ACTIVITY_START";
                            break;
                        case VENDOR_HINT_ACTIVITY_RESUME:
                            eventType = "ACTIVITY_RESUME";
                            break;
                        case VENDOR_HINT_ACTIVITY_PAUSE:
                            eventType = "ACTIVITY_PAUSE";
                            break;
                        case VENDOR_HINT_ACTIVITY_STOP:
                            eventType = "ACTIVITY_STOP";
                            break;
                        case VENDOR_HINT_PICARD_RENDER_RATE:
                            eventType = "PICARD_RENDER_RATE";
                            break;
                        case VENDOR_HINT_CONTENT_FPS:
                            eventType = "CONTENT_FPS";
                            break;
                    }
                    DEBUGI(LOG_TAG_LM, "=== ACTIVITY EVENT DEBUG ===");
                    DEBUGI(LOG_TAG_LM, "Event Type: %s (0x%x)", eventType, trigger.payload.hintID);
                    DEBUGI(LOG_TAG_LM, "Raw appName: %s", trigger.payload.appName.c_str());
                    DEBUGI(LOG_TAG_LM, "Hint Type: %d", trigger.payload.hintType);
                    DEBUGI(LOG_TAG_LM, "Duration: %d ms", trigger.payload.duration);
                    DEBUGI(LOG_TAG_LM, "App PID: %d", trigger.payload.appPID);
                    DEBUGI(LOG_TAG_LM, "App TID: %d", trigger.payload.appThreadTID);
                    DEBUGI(LOG_TAG_LM, "Handle: %d", trigger.payload.handle);
                    DEBUGI(LOG_TAG_LM, "Workload Type: %d", trigger.payload.app_workload_type);
                    DEBUGI(LOG_TAG_LM, "Timestamp: %lld", trigger.payload.timeStamp);
                }
                auto currAsyncEntry = mAsyncFeatMap.find(trigger.payload.hintID);
                if (currAsyncEntry != mAsyncFeatMap.end()) {
                    for (Feature *currFeature : currAsyncEntry->second) {
                        if (currFeature) {
                            DEBUGV(LOG_TAG_LM,"Calling storeAsync for %s", currFeature->getFeatureName().c_str());
                            if(mEnableLMtracing) {
                                string traceLog = "LM-storeAsync() " + currFeature->getFeatureName();
                                ATRACE_ASYNC_BEGIN(traceLog.c_str(), 0);
                            }
                            FeaturePayload tmpPayload;
                            tmpPayload.payload = trigger.payload;
                            tmpPayload.exitThread = shouldExit;
                            currFeature->storeAsync(tmpPayload);
                            if(mEnableLMtracing) {
                                string traceLog = "LM-storeAsync() " + currFeature->getFeatureName();
                                ATRACE_ASYNC_END(traceLog.c_str(), 0);
                            }
                            DEBUGV(LOG_TAG_LM,"storeAsync for %s returned", currFeature->getFeatureName().c_str());
                        }
                    }
                }

                if(trigger.payload.hintID == VENDOR_HINT_DISPLAY_OFF) {
                    DEBUGV(LOG_TAG_LM,"trigger is VENDOR_HINT_DISPLAY_OFF");
                    algoThreadInterrupt = false;
                    /*
                     * LMTESTS_HINTTYPE is debug addition to trigger Algos
                     * manually from lmtests
                     */
                    if(!idleAlgoThrBusy) {
                        /* Idle Algo thread not spawned yet or thread spawned earlier has completed*/
                        if(idleAlgoThread.joinable()) {
                            /* Idle Algo thread thread spawned earlier has completed, so join */
                            idleAlgoThread.join();
                        }
                        try {
                            idleAlgoThread = thread(&LearningModule::detectAndRunIdle, this, (trigger.payload.hintType == LMTESTS_HINTTYPE));
                        } catch(...) {
                            DEBUGE(LOG_TAG_LM,"Thread Constructor Exception");
                        }
                    }
                }
                if(trigger.payload.hintID == VENDOR_HINT_DISPLAY_ON) {
                    DEBUGV(LOG_TAG_LM,"trigger is VENDOR_HINT_DISPLAY_ON, sending algo interrupt");
                    algoThreadInterrupt = true;
                    /*
                     * Send interrupt to the algo thread and detach so that algo
                     * not exiting won't block LM and the impact if any of algo
                     * continuing execution ignoring interrupt is only on CPU
                     * resources
                     */
                    if(idleAlgoThread.joinable()) {
                        idleAlgoThread.join();
                    }
                }
            }
        } while (!shouldExit);

        /* Interrupt and wait for Algo thread */
        if(idleAlgoThread.joinable()) {
            algoThreadInterrupt = true;
            idleAlgoThread.join();
        }
        for (Feature* currFeature : mFeatureList) {
            FeaturePayload tmpPayload;
            tmpPayload.exitThread = shouldExit;
            currFeature->storeAsync(tmpPayload);
        }

        /* Interrupt and wait for Periodic feature execution thread*/
        for (Feature* currFeature : mFeatureList) {
            if(currFeature->periodicAlgoThread.joinable()) {
               currFeature->runPeriodicThreadInterrupt = true;
               currFeature->periodicAlgoThread.join();
            }
            if(currFeature->asyncDeliveryThread.joinable()) {
               currFeature->asyncDeliveryThread.join();
            }
        }
    } catch (exception &e) {
        for (Feature* currFeature : mFeatureList) {
            currFeature->runPeriodicThreadInterrupt = true;
        }
        algoThreadInterrupt = true;
        DEBUGE(LOG_TAG_LM, "Exception caught: %s in %s", e.what(), __func__);
    } catch (...) {
        for (Feature* currFeature : mFeatureList) {
            currFeature->runPeriodicThreadInterrupt = true;
        }
        algoThreadInterrupt = true;
        DEBUGE(LOG_TAG_LM, "Exception caught in %s", __func__);
    }
    DEBUGV(LOG_TAG_LM,"Exiting LMCore Thread");
    return;
}

