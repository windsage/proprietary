/*!
 * Copyright (c) 2021 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <vendor/qti/spu/2.0/ISPUManager.h>
#include <utils/StrongPointer.h>
#include <thread>
#include <stdint.h>

#include "SPComClientTest.h"
#include "SPComServerTest.h"
#include "SPUNotifier.h"
#include "SPUTestFw.h"
#include "SPUTestDefinitions.h"
#include "log.h"

using namespace std;

using vendor::qti::spu::V2_0::ISPUNotifier;
using vendor::qti::spu::V2_0::ISPUManager;
using ::android::sp;

int32_t spcomClientTestFlow()
{
    int32_t ret = SUCCESS;
    int32_t err = 0;

    // Load test SPU server app if not loaded
    err = SPUTestFw::loadApp(spuSrvAppFullPath, spuSrvAppName);
    ALOGI("loadApp() returned [%d]", err);
    if (err && err != ALREADY_LOADED) {
        ALOGE("Failed to load app [%s] with channel [%s], err [%d]", spuSrvAppFullPath,
            spuSrvAppName, err);
        return GENERAL_ERROR;
    }

    // Create spcom client test object
    SPComClientTest *spcomClientTest = SPUTestFw::getSPComClientTest();
    if(!spcomClientTest || !spcomClientTest->valid()) {
        ALOGE("Invalid spcom client test obj");
        return FRAMEWORK_ERROR;
    }

    // Register / unregister stress test
    err = spcomClientTest->openCloseChStress();
    if (err) {
        ALOGE("Register unregister test client stress failed, err [%d]", err);
        return CHANNEL_ERROR;
    }

    // Register spcom channel
    err = spcomClientTest->registerClient();
    if (err) {
        ALOGE("Failed to register spcom client [%s], err [%d]", spuSrvAppName, err);
        return CHANNEL_ERROR;
    }

    // Register spcom channel again - should fail
    err = spcomClientTest->registerClient();
    if (!err) {
        ALOGE("Double register to client channel succeeded [%s]", spuSrvAppName);
        return GENERAL_ERROR;
    }

    // Test send message sync to spu test app
    err = spcomClientTest->sendPingWaitForPong();
    if (err) {
        ALOGE("PING-PONG test failed, err [%d]", err);
        ret = GENERAL_ERROR;
    }

    // Test shared buffer read and write
    err = spcomClientTest->sendModifiedMessageFlow();
    if (err) {
        ALOGE("Shared buffer flow test failed, err [%d]", err);
        ret = GENERAL_ERROR;
    }

    return ret;
}

int32_t spcomServerTestFlow()
{
    int32_t ret = SUCCESS;
    int32_t err = 0;

    // Create server test object
    SPComServerTest* spcomServerTest = SPUTestFw::getSPComServerTest();
    if (!spcomServerTest->valid()) {
        ALOGE("Invalid spcom server test obj");
        return FRAMEWORK_ERROR;
    }

    // Register / unregister stress test
    err = spcomServerTest->openCloseChStress();
    if (err) {
        ALOGE("Register unregister test server stress failed, err [%d]", err);
        return CHANNEL_ERROR;
    }

    // Register spcom channel
    err = spcomServerTest->registerServer();
    if (err) {
        ALOGE("Failed to register spcom server [%s], err [%d]", spuCliAppName, err);
        return CHANNEL_ERROR;
    }

    // Register spcom channel again - should fail
    err = spcomServerTest->registerServer();
    if (!err) {
        ALOGE("Double register to server channel succeeded [%s]", spuCliAppName);
        return GENERAL_ERROR;
    }

    // Load test SPU client app if not loaded
    err = SPUTestFw::loadApp(spuCliAppFullPath, spuCliAppName);
    ALOGI("loadApp() returned [%d]", err);
    if (err && err != ALREADY_LOADED) {
        ALOGE("Failed to load app [%s] with channel [%s], err [%d]", spuCliAppFullPath,
            spuCliAppName, err);
        return GENERAL_ERROR;
    }

    // Test send message sync to spu test app
    err = spcomServerTest->waitForPingSendPong();
    if (err) {
        ALOGE("PING-PONG test failed, err [%d]", err);
        ret = GENERAL_ERROR;
    }

    err = spcomServerTest->sendModifiedResponseFlow();
    if (err) {
        ALOGE("Shared buffer flow test failed, err [%d]", err);
        ret = GENERAL_ERROR;
    }

    return ret;
}

int32_t ssrFlow()
{
    sp<SPUNotifier> notifier = SPUTestFw::getNotifier();
    if (!notifier)
        return FRAMEWORK_ERROR;

    int32_t ret = SPUTestFw::spuReset();
    if (ret) {
        ALOGE("Failed to reset SPU, err [%d]", ret);
        return SSR_ERROR;
    }

    notifier->waitForSSRHandled();

    notifier->resetSSRHandled();

    SSRHandlingState state = SPUTestFw::getSSRHandlingState();
    if(state == SSR_STATE_ERROR)
        ALOGE("SSR error[%d] while waiting for SSR to be handled", SPUTestFw::getSSRThreadError());

    ALOGD("SSR handling done, continue test");

    return SUCCESS;
}

int32_t testCore(uint32_t testCycles)
{
    // Wait for system readiness before using SPU HAL interface
    if(SPUTestFw::waitForSpuReady())
        return FRAMEWORK_ERROR;

    if(SPUTestFw::startSSRHandling()) {
        SSRHandlingState state = SPUTestFw::getSSRHandlingState();
        int32_t err = SPUTestFw::getSSRThreadError();
        ALOGE("Failed to start SSR handling, SSR handling state [%d], SSR error [%d]", state, err);
        return SSR_ERROR;
    }

    // Run test core in loop
    int32_t ret = SUCCESS;
    for(uint32_t cycle = 1; cycle <= testCycles; ++cycle) {

        ALOGI("@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@");
        ALOGI("@@@         TEST CYCLE #%.2u          @@@", cycle);
        ALOGI("@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@\n\n");

        do {
            // Wait for system readiness before using SPU HAL interface
            if(SPUTestFw::waitForSpuReady()) {
                return FRAMEWORK_ERROR;
            }

            ALOGD("Wait for SSR handling to be in ready state");
            uint32_t retries = waitForSSRReadyRetries;
            while(SPUTestFw::getSSRHandlingState() != SSR_STATE_READY && --retries) {
                this_thread::sleep_for(chrono::seconds(1 /*second*/));
            }

            if (SPUTestFw::getSSRHandlingState() != SSR_STATE_READY) {
                ALOGE("Failed to start SSR handling test");
                  ret = FRAMEWORK_ERROR;
                  break;
            }

            if (SPUTestFw::readSpuInfo()) {
                ALOGE("Error in read SPU info");
                ret = SPU_INFO_TEST_ERROR;
                break;
            }

            if (spcomClientTestFlow()) {
                ALOGE("Error in client test flow");
                ret = CLIENT_TEST_ERROR;
                break;
            }

            if (spcomServerTestFlow()) {
                ALOGE("Error in server test flow");
                ret = GENERAL_ERROR;
                break;
            }

            if (ssrFlow()) {
                ALOGE("Error in SSR test flow");
                ret = SSR_ERROR;
                break;
            }
        } while(0);

        ALOGI("\n\n@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@");
        ALOGI("@@@ TEST CYCLE #%.2u done - ret: [%d]  @@@", cycle, ret);
        ALOGI("@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@\n\n");

        if (ret)
            break;
    }

    SPUTestFw::stopSSRHandling();

    return ret;
}

int main(int argc, char** argv)
{
    // Check input
    if(argc > 2) {
        ALOGE("Can get only 1 command line parameter");
        return INPUT_ERROR;
    }

    // Default cycles number is 1
    uint32_t testCycles = 1;

    // Number of cycles input could only be valid positive integer
    if(argc == 2) {
        int testCyclesInput = atoi(argv[1]);
        if(testCyclesInput > 0) {
            testCycles = testCyclesInput;
        } else {
            ALOGE("Input should be a number of cycles");
            return INPUT_ERROR;
        }
    }

    // Print number of cycles
    ALOGD("Number of test cycles: [%d]", testCycles);

    // Initialize the framework
    if(SPUTestFw::init()) {
        ALOGE("Failed to init SPU test framework, abort");
        return FRAMEWORK_ERROR;
    }

    // Run tests
    int32_t ret = testCore(testCycles);

    // Release framework resources
    SPUTestFw::release();

    return ret;
}
