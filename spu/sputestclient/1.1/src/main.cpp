/*!
 *
 * Copyright (c) 2019-2021 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 */

#include <vendor/qti/spu/1.0/ISpcomSSREvent.h>
#include <utils/StrongPointer.h>
#include <thread>
#include <stdint.h>

#include "SPUTestFw.h"
#include "SPUTestDefinitions.h"
#include "SpcomClientTest.h"
#include "SpcomServerTest.h"
#include "SpcomSSREvent.h"
#include "log.h"

using namespace std;

using vendor::qti::spu::V1_0::ISpcomSSREvent;
using vendor::qti::spu::V1_1::ISPUManager;
using ::android::sp;

int32_t ssrAddRemoveTest() {

    ISpcomSSREvent* cb1 = new PrintMessageOnSSREvent("SSRCb1");
    ALOGD("New SSR CB1 [%p]", cb1);

    ISpcomSSREvent* cb2 = new PrintMessageOnSSREvent("SSRCb2");
    ALOGD("New SSR CB2 [%p]", cb2);

    ISpcomSSREvent* cb3 = new PrintMessageOnSSREvent("SSRCb3");
    ALOGD("New SSR CB3 [%p]", cb3);

    sp<ISpcomSSREvent> printOnSSR1(cb1);
    sp<ISpcomSSREvent> printOnSSR2(cb2);
    sp<ISpcomSSREvent> printOnSSR3(cb3);

    size_t count = 0;

    // Remove before add
    ALOGD("RM SSR CB1");
    int32_t ret = SPUTestFw::rmSSRCb(printOnSSR1);
    if(ret == SUCCESS) {
        ALOGE("Remove #1 before add should fail");
        ++count;
    }

    // Add twice
    ALOGD("ADD SSR CB1");
    ret = SPUTestFw::addSSRCb(printOnSSR1);
    if(ret != SUCCESS) {
         ALOGE("Add #1 failed, err[%d]", ret);
        ++count;
    }

    ALOGD("ADD SSR CB1");
    ret = SPUTestFw::addSSRCb(printOnSSR1);
    if(ret == SUCCESS) {
        ALOGE("Add #1 twice should fail");
        ++count;
    }

    // Add
    ALOGD("ADD SSR CB2");
    ret = SPUTestFw::addSSRCb(printOnSSR2);
    if(ret != SUCCESS) {
        ALOGE("Add #2 failed, err[%d]", ret);
        ++count;
    }

    ALOGD("ADD SSR CB3");
    ret = SPUTestFw::addSSRCb(printOnSSR3);
    if(ret != SUCCESS) {
        ALOGE("Add #3 failed, err[%d]", ret);
        ++count;
    }

    // Remove
    ALOGD("RM SSR CB2");
    ret = SPUTestFw::rmSSRCb(printOnSSR2);
    if(ret != SUCCESS) {
         ALOGE("Remove #2 failed, err[%d]", ret);
        ++count;
    }

    ALOGD("RM SSR CB3");
    ret = SPUTestFw::rmSSRCb(printOnSSR3);
    if(ret != SUCCESS) {
          ALOGE("Remove #3 failed, err[%d]", ret);
        ++count;
    }

    ALOGD("RM SSR CB1");
    ret = SPUTestFw::rmSSRCb(printOnSSR1);
    if(ret != SUCCESS) {
          ALOGE("Remove #1 failed, err[%d]", ret);
        ++count;
    }

    // Remove twice
    ALOGD("RM SSR CB2 AGAIN");
    ret = SPUTestFw::rmSSRCb(printOnSSR2);
    if(ret == SUCCESS) {
        ALOGE("Remove twice should fail");
        ++count;
    }

    // Add after remove
    ALOGD("ADD SSR CB1");
    ret = SPUTestFw::addSSRCb(printOnSSR1);
    if(ret != SUCCESS) {
        ALOGE("Add #1 after remove failed, err[%d]", ret);
        ++count;
    }

    ALOGD("RM SSR CB1");
    ret = SPUTestFw::rmSSRCb(printOnSSR1);
    if(ret != SUCCESS) {
        ALOGE("Failed final remove, [%d]", ret);
        ++count;
    }

    printOnSSR1.clear();
    printOnSSR2.clear();
    printOnSSR3.clear();

    return (count == 0) ? SUCCESS : SSR_ERROR;
}

int32_t spcomClientTestFlow() {

    int32_t ret = SUCCESS;
    int32_t err = 0;

    // Load test SPU server app if not loaded
    err = SPUTestFw::loadApp(sSpcomServerAppFullPath, sSpcomServerAppChannel);
    ALOGI("loadApp() returned [%d]", err);
    if(err != SUCCESS && err != ALREADY_LOADED) {
        ALOGE("Failed to load app [%s] with channel [%s], err [%d]",
            sSpcomServerAppFullPath, sSpcomServerAppChannel, err);
        return GENERAL_ERROR;
    }

    // Create spcom client test object
    SpcomClientTest* spcomClientTest = SPUTestFw::getSpcomClientTest();
    if(!spcomClientTest || spcomClientTest->valid() == false) {
        ALOGE("Invalid spcom client test obj");
        return FRAMEWORK_ERROR;
    }

    // Register / unregister stress test
    err = spcomClientTest->registerUnregisterClientStress();
    if(err != SUCCESS) {
        ALOGE("Register unregister test client stress failed, err [%d]", err);
        return CHANNEL_ERROR;
    }

    // Register spcom channel
    err = spcomClientTest->registerClient();
    if(err != SUCCESS) {
        ALOGE("Failed to register spcom client [%s], err [%d]",
            sSpcomServerAppChannel, err);
        return CHANNEL_ERROR;
    }

    // Register spcom channel again - should fail
    err = spcomClientTest->registerClient();
    if(err == SUCCESS) {
        ALOGE("Succeeded to register to a registered spcom channel [%s]",
            sSpcomServerAppChannel);
        return GENERAL_ERROR;
    }

    // Test send message sync to spu test app
    err = spcomClientTest->sendPingWaitForPong();
    if(err != SUCCESS) {
        ALOGE("PING-PONG test failed, err [%d]", err);
        ret = GENERAL_ERROR;
    }

    // Test shared buffer read and write
    err = spcomClientTest->sendSharedBufferMessageFlow();
    if(err != SUCCESS) {
        ALOGE("Shared buffer flow test failed, err [%d]", err);
        ret = GENERAL_ERROR;
    }

    return ret;
}

int32_t spcomServerTestFlow() {

    int32_t ret = SUCCESS;
    int32_t err = 0;

    // Create server test object
    SpcomServerTest* spcomServerTest = SPUTestFw::getSpcomServerTest();
    if(spcomServerTest->valid() == false) {
        ALOGE("Invalid spcom server test obj");
        return FRAMEWORK_ERROR;
    }

    // Register / unregister stress test
    err = spcomServerTest->registerUnregisterServerStress();
    if(err != SUCCESS) {
        ALOGE("Register unregister test server stress failed, err [%d]", err);
        return CHANNEL_ERROR;
    }

    // Register spcom channel
    err = spcomServerTest->registerServer();
    if(err != SUCCESS) {
        ALOGE("Failed to register spcom server [%s], err [%d]",
            sSpcomClientAppChannel, err);
        return CHANNEL_ERROR;
    }

    // Register spcom channel again - should fail
    err = spcomServerTest->registerServer();
    if(err == SUCCESS) {
        ALOGE("Succeeded to register to a registered spcom channel [%s]",
            sSpcomClientAppChannel);
        return GENERAL_ERROR;
    }

    // Load test SPU client app if not loaded
    err = SPUTestFw::loadApp(sSpcomClientAppFullPath, sSpcomClientAppChannel);
    ALOGI("loadApp() returned [%d]", err);
    if(err != SUCCESS && err != ALREADY_LOADED) {
        ALOGE("Failed to load app [%s] with channel [%s], err [%d]",
            sSpcomClientAppFullPath, sSpcomClientAppChannel, err);
        return GENERAL_ERROR;
    }

    // Test send message sync to spu test app
    err = spcomServerTest->waitForPingSendPong();
    if(err != SUCCESS) {
        ALOGE("PING-PONG test failed, err [%d]", err);
        ret = GENERAL_ERROR;
    }

    err = spcomServerTest->sendSharedBufferResponseFlow();
    if(err != SUCCESS) {
        ALOGE("Shared buffer flow test failed, err [%d]", err);
        ret = GENERAL_ERROR;
    }

    return ret;
}

int32_t ssrFlow() {

    sp<SpcomSSREvent> spcomSSREvent = SPUTestFw::getSpcomSSREvent();
    if(!spcomSSREvent) {
        ALOGE("SpcomSSREvent is not valid");
        return FRAMEWORK_ERROR;
    }

    int32_t ret = SPUTestFw::spuReset();
    if(ret != SUCCESS) {
        ALOGE("Failed to reset SPU, err [%d]", ret);
        return SSR_ERROR;
    }

    spcomSSREvent->waitForSSRHandled();

    SSRHandlingState state = SPUTestFw::getSSRHandlingState();
    if(state == SSR_STATE_ERROR) {
        ALOGE("SSR error [%d] while waiting for SSR to be handled",
            SPUTestFw::getSSRThreadError());
    }

    ALOGD("SSR handling done, continue test");

    return SUCCESS;
}

int32_t testCore(uint32_t testCycles) {

    // Wait for system readiness before using SPU HAL interface
    if(SPUTestFw::waitForSpuReady()) {
        return FRAMEWORK_ERROR;
    }

    if(SPUTestFw::startSSRHandling() != SUCCESS) {
        SSRHandlingState state = SPUTestFw::getSSRHandlingState();
        int32_t err = SPUTestFw::getSSRThreadError();
        ALOGE("Failed to start SSR handling, SSR handling state [%d], SSR \
            error [%d]", state, err);
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

            ALOGD("Add remove SSR callbacks test");
            if(ssrAddRemoveTest() != SUCCESS) {
                ALOGE("SSR add/remove test failed");
                return SSR_ERROR;
            }

            ALOGD("Wait for SSR handling to be in ready state");
            uint32_t retries = sWaitForSSRReadyRetries;
            while(SPUTestFw::getSSRHandlingState() != SSR_STATE_READY
                && --retries) {
                this_thread::sleep_for(chrono::seconds(1 /*second*/));
            }

            if(SPUTestFw::getSSRHandlingState() != SSR_STATE_READY) {
                ALOGE("Failed to start SSR handling test");
                  ret = FRAMEWORK_ERROR;
                  break;
            }

            if(SPUTestFw::readSpuInfo() != SUCCESS) {
                ALOGE("Error in read SPU info");
                ret = SPU_INFO_TEST_ERROR;
                break;
            }

            if(spcomClientTestFlow() != SUCCESS) {
                ALOGE("Error in client test flow");
                ret = CLIENT_TEST_ERROR;
                break;
            }

            if(spcomServerTestFlow() != SUCCESS) {
                ALOGE("Error in server test flow");
                ret = GENERAL_ERROR;
                break;
            }

            if(ssrFlow() != SUCCESS) {
                ALOGE("Error in SSR test flow");
                ret = SSR_ERROR;
                break;
            }
        } while(0);

        ALOGI("\n\n@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@");
        ALOGI("@@@ TEST CYCLE #%.2u done - ret: [%d]  @@@", cycle, ret);
        ALOGI("@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@\n\n");

        if(ret != SUCCESS) {
            break;
        }
    }

    SPUTestFw::stopSSRHandling();

    return ret;
}

int main(int argc, char** argv) {

    // Print app version
    ALOGI("SPU HAL client test app version [%d.%d]",
        sAppVersionMajor, sAppVersionMinor);

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
    if(SPUTestFw::init() != SUCCESS) {
        ALOGE("Failed to init SPU test framework, abort");
        return FRAMEWORK_ERROR;
    }

    // Run tests
    int32_t ret = testCore(testCycles);

    // Release framework resources
    SPUTestFw::release();

    return ret;
}
