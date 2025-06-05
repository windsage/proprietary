/*
*Copyright (c) 2022 Qualcomm Technologies, Inc.
*All Rights Reserved.
*Confidential and Proprietary - Qualcomm Technologies, Inc.
*
*  Not a contribution.
*
* Copyright (C) 2016 The Android Open Source Project
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*      http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*/

#include <aidl/Gtest.h>
#include <aidl/Vintf.h>
#include <VtsHalHidlTargetCallbackBase.h>
#include <aidl/vendor/qti/hardware/bttpi/BnBtTpi.h>
#include <aidl/vendor/qti/hardware/bttpi/BnBtTpiStatusCb.h>
#include <aidl/vendor/qti/hardware/bttpi/BnBtTpiEventsCb.h>
#include <aidl/vendor/qti/hardware/bttpi/BnBtTpiStatusRspCb.h>
#include <aidl/vendor/qti/hardware/bttpi/BtTpiState.h>
#include <android/binder_auto_utils.h>
#include <android/binder_manager.h>
#include <binder/IServiceManager.h>
#include <binder/ProcessState.h>
#include <utils/Log.h>
#include <chrono>
#include <condition_variable>

#include <gtest/gtest.h>
#include <hidl/GtestPrinter.h>
#include <hidl/ServiceManagement.h>

#include <future>

using ::aidl::vendor::qti::hardware::bttpi::IBtTpi;
using ::aidl::vendor::qti::hardware::bttpi::BtTpiState;
using ::aidl::vendor::qti::hardware::bttpi::IBtTpiStatusCb;
using ::aidl::vendor::qti::hardware::bttpi::IBtTpiEventsCb;
using ::aidl::vendor::qti::hardware::bttpi::BnBtTpiEventsCb;
using ::aidl::vendor::qti::hardware::bttpi::BnBtTpiStatusCb;
using ::aidl::vendor::qti::hardware::bttpi::BnBtTpiStatusRspCb;
using android::ProcessState;
using android::String16;
using ndk::ScopedAStatus;
using ndk::SpAIBinder;
//using ::android::hardware::Return;
//using ::android::hardware::Void;
using ::android::sp;
//using ::android::binder::Status;
constexpr char kCallbackNameStatusReceived[] = "setStatus";
constexpr char  kCallbackNameEventsReceived[] = "sendTpiEvent";

static const std::string kMaxIter = "--maxIter=";
static uint32_t maxIter = 1;

namespace {
constexpr static int kCallbackTimeoutMs = 5000;
}  // namespace
// A simple test implementation of BtTpiEventsCb.
class BtTpiEventsCb : public BnBtTpiEventsCb {
    public:
      BtTpiEventsCb(){};
      ~BtTpiEventsCb(){};
      ::ndk::ScopedAStatus sendTpiEvent(int eventType,
          const std::vector<uint8_t>& event)  {
          ALOGI("sendTpiEvent type= %d\n", eventType );
          return ScopedAStatus::ok();
      }
      int8_t event_status;
  };

 // A simple test implementation of BtTpiStatusCb.
 class BtTpiStatusCb : public BnBtTpiStatusCb {
    public:
      BtTpiStatusCb(){};
      virtual ~BtTpiStatusCb() = default;

     ::ndk::ScopedAStatus setStatus( int req_type, int8_t status) {
        ALOGD("%s (status = %d)", __func__, static_cast<int>(status));
        std::lock_guard<std::mutex> lock(mutex_);
        status_received = static_cast<int>(status) ;
        ALOGI("Notify all");
        cv_notify_.notify_all();
        return ScopedAStatus::ok();
     }
     bool waitForStatusEvent( std::chrono::milliseconds timeout) {
        std::unique_lock<std::mutex> lock(mutex_);
        status_received = 1;
        ALOGI("waitForStatusEvent : wait ");
        return cv_notify_.wait_for(lock, timeout, [&] { return status_received==0; });
     }
    int status_received;
    std::mutex mutex_;
    std::condition_variable cv_notify_;
 };

  class BtTpiStatusRspCb : public BnBtTpiStatusRspCb {
    public:
      BtTpiStatusRspCb(){};
      virtual ~BtTpiStatusRspCb() = default;
     ::ndk::ScopedAStatus setStatusRsp( int req_type, int8_t status,  const std::vector<uint8_t>& data) {
        int len = data.size();
        ALOGD("%s (status = %d) and len %d", __func__, static_cast<int>(status), len);
        for (int i = 0; i < len ; i++) {
          ALOGD("%s: data[%d] = %02x ", __func__, i, data[i]);
        }
        std::lock_guard<std::mutex> lock(mutex_);
        status_received = static_cast<int>(status) ;
        ALOGI("Notify all");
        cv_notify_.notify_all();
        return ScopedAStatus::ok();
     }
     bool waitForStatusRspEvent( std::chrono::milliseconds timeout) {
        std::unique_lock<std::mutex> lock(mutex_);
        status_received = 1;
        ALOGI("waitForStatusEvent : wait ");
        return cv_notify_.wait_for(lock, timeout, [&] { return status_received==0; });
     }
    int status_received;
    std::mutex mutex_;
    std::condition_variable cv_notify_;
 };


class BtTpiAidl : public testing::TestWithParam<std::string> {
  public:
    virtual void SetUp() override {
      iBtTpi_ = IBtTpi::fromBinder(SpAIBinder(AServiceManager_waitForService(GetParam().c_str())));
      ASSERT_NE(iBtTpi_, nullptr);
      tpievents_cb = ndk::SharedRefBase::make<BtTpiEventsCb>();
      ASSERT_NE(tpievents_cb, nullptr);
      tpistatus_cb = ndk::SharedRefBase::make<BtTpiStatusCb>();
      ASSERT_NE(tpistatus_cb, nullptr);
      tpiStatusRsp_cb = ndk::SharedRefBase::make<BtTpiStatusRspCb>();
      ASSERT_NE(tpiStatusRsp_cb, nullptr);
      ALOGI("register Tpi event");
      iBtTpi_-> registerTpiEventsCallback(tpievents_cb, &ret_status);
   }

   virtual void TearDown() override {
     iBtTpi_->UnRegisterTpiEventsCallback(&ret_status);
     ALOGI("unregister Tpi event done");
     tpistatus_cb = nullptr;
     tpievents_cb = nullptr;
   }
   std::shared_ptr<IBtTpi> iBtTpi_;
   std::shared_ptr<BtTpiStatusCb> tpistatus_cb;
   std::shared_ptr<BtTpiEventsCb> tpievents_cb;
   std::shared_ptr<BtTpiStatusRspCb> tpiStatusRsp_cb;;

   bool status_received;
   bool event_received;
   int8_t ret_status;
};

TEST_P(BtTpiAidl, TpiComboTest) {
   int8_t status_value;
   ScopedAStatus status;
   std::chrono::milliseconds timeout{kCallbackTimeoutMs};

   for(int i = 0; i < maxIter; i++) {

     ALOGI("=====Get TPI Version===iter(%d)===", i);
     status = iBtTpi_->getTpiVersion(3, tpiStatusRsp_cb, &status_value);
     EXPECT_TRUE(status.isOk());
     ALOGI("wait for StatusRsp Callback");
     ASSERT_TRUE(tpiStatusRsp_cb->waitForStatusRspEvent(timeout));

     {
       uint8_t values[] = {0x01};
       std::vector<uint8_t> data(values, values + sizeof(values));
       ALOGI("=====setTpiParamsRsp ===iter(%d)===", i);

       status = iBtTpi_->setTpiParamsRsp(0x2b, data, tpiStatusRsp_cb, &status_value);
       EXPECT_TRUE(status.isOk());
       ALOGI("wait for StatusRsp Callback");
       ASSERT_TRUE(tpiStatusRsp_cb->waitForStatusRspEvent(timeout));
     }

     ALOGI("=====Enable TPI===iter(%d)===", i);
     status  = iBtTpi_->setTpiState(BtTpiState::TPI_STATE_ENABLE,
                               tpistatus_cb, &status_value);
     EXPECT_TRUE(status.isOk());
     ALOGI("wait for Status Callback");
     ASSERT_TRUE(tpistatus_cb->waitForStatusEvent(timeout));
     if (status_value == 0)
       ALOGI("Enable Tpi: Test case succeed");
     else
      ALOGI("Enable Tpi: Test case failed");

     ALOGI("=======Enable Airplane Mode===iter(%d)===", i);
     status = iBtTpi_->setAirplaneMode(true, tpistatus_cb, &status_value);
     EXPECT_TRUE(status.isOk());
     ALOGI("wait for Status Callback");
     ASSERT_TRUE(tpistatus_cb->waitForStatusEvent(timeout));
     if (status_value == 0)
      ALOGI("Enable Airplane Mode: Test case succeed");
     else
      ALOGI("Enable Airplane Mode: Test case failed");

     ALOGI("=======Set Active DSI===iter(%d)===", i);
     status = iBtTpi_->setActiveDSI(0, tpistatus_cb, &status_value);
     EXPECT_TRUE(status.isOk());
     ALOGI("wait for Status Callback");
     ASSERT_TRUE(tpistatus_cb->waitForStatusEvent(timeout));
     if (status_value == 0)
       ALOGI("Set Active DSI: Test case succeed");
     else
      ALOGI("Set Active DSI: Test case failed");

     ALOGI("=======Cfg Ind===iter(%d)===", i);
     uint8_t values[] = {0x64, 0x0, 0x64, 0x0, 0xe8, 0x3, 0x5, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x1, 0x0, 0x5, 0x2, 0x2, 0x5, 0x3, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x1, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x80, 0x5, 0x0, 0x0, 0x0, 0x66, 0xe6, 0x0, 0x0, 0x66, 0xe6, 0x0, 0x0, 0x66, 0xe6, 0x0, 0x0, 0x66, 0xe6, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x5, 0x0, 0x0, 0x0, 0xcc, 0x4c, 0x0, 0x0, 0xcc, 0x4c, 0x0, 0x0, 0xcc, 0x4c, 0x0, 0x0, 0xcc, 0x4c, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x5, 0x0, 0x0, 0x0, 0x33, 0x33, 0x0, 0x0, 0x33, 0x33, 0x0, 0x0, 0x33, 0x33, 0x0, 0x0, 0x33, 0x33, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x5, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x5, 0x3, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x5, 0x0, 0x0, 0x0, 0x66, 0xe6, 0x0, 0x0, 0x66, 0xe6, 0x0, 0x0, 0x66, 0xe6, 0x0, 0x0, 0x66, 0xe6, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x5, 0x0, 0x0, 0x0, 0xcc, 0x4c, 0x0, 0x0, 0xcc, 0x4c, 0x0, 0x0, 0xcc, 0x4c, 0x0, 0x0, 0xcc, 0x4c, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x5, 0x0, 0x0, 0x0, 0x33, 0x33, 0x0, 0x0, 0x33, 0x33, 0x0, 0x0, 0x33, 0x33, 0x0, 0x0, 0x33, 0x33, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x5, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0};
     std::vector<uint8_t> data(values, values + sizeof(values));
     status = iBtTpi_->setTpiParams(0x1B, data, tpistatus_cb, &status_value);
     EXPECT_TRUE(status.isOk());
     ALOGI("Cfg Ind Data Size:%d, wait for Status Callback", sizeof(values));
     ASSERT_TRUE(tpistatus_cb->waitForStatusEvent(timeout));
     if (status_value == 0)
       ALOGV("Cfg Ind Test case succeed");
     else
      ALOGI("Cfg Ind Test case failed");

     ALOGI("=======SAR Table===iter(%d)===", i);
     uint8_t values1[] = {0x68, 0x1, 0x4, 0x0, 0x0, 0x44, 0x0, 0xff, 0x0, 0x7f, 0x44, 0x0, 0xff, 0x0, 0x0, 0x48, 0x1, 0xff, 0x0, 0x7f, 0x48, 0x1, 0xff, 0x0};
     std::vector<uint8_t> data1(values1, values1 + sizeof(values1));
     status = iBtTpi_->setTpiParams(0x1C, data1, tpistatus_cb, &status_value);
     EXPECT_TRUE(status.isOk());
     ALOGI("Cfg Ind Data Size:%d, wait for Status Callback", sizeof(values1));
     ASSERT_TRUE(tpistatus_cb->waitForStatusEvent(timeout));
     if (status_value == 0)
       ALOGV("SAR Table Test case succeed");
     else
      ALOGI("SAR Table Test case failed");

     ALOGI("=======Wait for Async Indications===iter(%d)===", i);
     sleep(5);

     ALOGI("=======Disable Airplane Mode===iter(%d)===", i);
     status = iBtTpi_->setAirplaneMode(false, tpistatus_cb, &status_value);
     EXPECT_TRUE(status.isOk());
     ALOGI("wait for Status Callback");
     ASSERT_TRUE(tpistatus_cb->waitForStatusEvent(timeout));
     if (status_value == 0)
      ALOGI("Enable Airplane Mode: Test case succeed");
     else
      ALOGI("Enable Airplane Mode: Test case failed");

     ALOGI("=======Disable TPI===iter(%d)===", i);
     status  = iBtTpi_->setTpiState(BtTpiState::TPI_STATE_DISABLE,
                      tpistatus_cb, &status_value);
     EXPECT_TRUE(status.isOk());
     ALOGI("wait for Status Callback");
     ASSERT_TRUE(tpistatus_cb->waitForStatusEvent(timeout));
     if (status_value == 0)
       ALOGI("Enable Tpi: Test case succeed");
     else
      ALOGI("Enable Tpi: Test case failed");
   }
}

GTEST_ALLOW_UNINSTANTIATED_PARAMETERIZED_TEST(BtTpiAidl);
INSTANTIATE_TEST_SUITE_P(BtTpi, BtTpiAidl,
                        testing::ValuesIn(android::getAidlHalInstanceNames(IBtTpi::descriptor)),
                        android::PrintInstanceNameToString);

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    ProcessState::self()->setThreadPoolMaxThreadCount(1);
    ProcessState::self()->startThreadPool();

    for (int i = 1; i < argc; i++) {
      if (argv[i] == NULL) continue;

      if (string(argv[i]).find(kMaxIter) == 0) {
        string value = string(argv[i]).substr(kMaxIter.length());
        maxIter = std::atoi(value.c_str());
        ALOGI("%s: MaxIterations(%d)", __func__, maxIter);
      }
    }
    return RUN_ALL_TESTS();
}
