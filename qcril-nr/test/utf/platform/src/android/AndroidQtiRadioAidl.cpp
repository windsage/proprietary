/*===========================================================================
 *
 *    Copyright (c) 2022 Qualcomm Technologies, Inc.
 *    All Rights Reserved.
 *    Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 *===========================================================================*/

#include "ril_utf_ril_api.h"
#include "platform/common/CommonPlatform.h"

#include "ibinder_internal.h"
// #include <binder/IBinder.h>
#include <android/binder_status.h>
#include <android/binder_ibinder.h>
#include <android/binder_manager.h>
#include <aidl/vendor/qti/hardware/radio/qtiradio/IQtiRadio.h>
#include <aidl/vendor/qti/hardware/radio/qtiradio/IQtiRadioResponse.h>
#include <aidl/vendor/qti/hardware/radio/qtiradio/IQtiRadioIndication.h>

#include "platform/android/IQtiRadioAidlResponseClientImpl.h"
#include "platform/android/IQtiRadioAidlIndicationClientImpl.h"
#include "platform/android/NasAidlUtil.h"
#include "platform/android/ril_qti_radio.h"

#include "platform/android/NasAidlUtil.h"

class AndroidQtiRadioAidl : public CommonPlatform {
public:
    AndroidQtiRadioAidl() {
        utfQtiRadioAidl = nullptr;
        bFirstCall = true;
    }
    void Register(RIL_RadioFunctions *callbacks) override;
    int OnRequest(int request, void *data, size_t datalen,
        RIL_Token t) override;
private:
    std::shared_ptr<qtiradioaidl::IQtiRadio> utfQtiRadioAidl = nullptr;
    std::shared_ptr<IQtiRadioAidlResponseClientImpl> mRespClient;
    std::shared_ptr<IQtiRadioAidlIndicationClientImpl> mIndClient;
    bool bFirstCall;
};

void AndroidQtiRadioAidl::Register(RIL_RadioFunctions *callbacks) {
    if (!bFirstCall) return;
    while (utfQtiRadioAidl == nullptr) {
        auto qtiradioBinder = ::ndk::SpAIBinder(AServiceManager_getService(
                "vendor.qti.hardware.radio.qtiradio.IQtiRadioStable/slot1"));
        utfQtiRadioAidl = qtiradioaidl::IQtiRadio::fromBinder(qtiradioBinder);
        if (utfQtiRadioAidl != nullptr) {
            RIL_UTF_DEBUG("\n QMI_RIL_UTL: calling setCallbacks");
            mRespClient = ndk::SharedRefBase::make<IQtiRadioAidlResponseClientImpl>();
            mIndClient = ndk::SharedRefBase::make<IQtiRadioAidlIndicationClientImpl>();
            utfQtiRadioAidl->setCallbacks(mRespClient, mIndClient);
            break;
        }
        RIL_UTF_DEBUG("\n QMI_RIL_UTL: get(IQtiRadioStable) returned null");
        usleep(1);
     }
}

int AndroidQtiRadioAidl::OnRequest(int request, void *data, size_t datalen,
        RIL_Token t) {
    switch (request) {
        case RIL_REQUEST_GET_DATA_NR_ICON_TYPE:
            if (utfQtiRadioAidl != nullptr) {
                utfQtiRadioAidl->queryNrIconType(*static_cast<int32_t*>(t));
                return 0;
            }
            break;
        case RIL_REQUEST_ENABLE_ENDC:
          RIL_UTF_DEBUG("Calling RIL_REQUEST_ENABLE_ENDC");
          if (utfQtiRadioAidl != nullptr) {
            RIL_UTF_DEBUG("Calling enableEndc");
            utfQtiRadioAidl->enableEndc(*static_cast<int32_t*>(t), *static_cast<bool*>(data));
            return 0;
          } else {
            return 1;
          }
        case RIL_REQUEST_QUERY_ENDC_STATUS:
          RIL_UTF_DEBUG("Calling RIL_REQUEST_QUERY_ENDC_STATUS");
          if (utfQtiRadioAidl != nullptr) {
            RIL_UTF_DEBUG("Calling queryEndcStatus");
            utfQtiRadioAidl->queryEndcStatus(*static_cast<int32_t*>(t));
            return 0;
          } else {
            return 1;
          }
        case RIL_REQUEST_QUERY_NR_DISABLE_MODE:
          RIL_UTF_DEBUG("Calling RIL_REQUEST_QUERY_NR_DISABLE_MODE");
          if (utfQtiRadioAidl != nullptr) {
            RIL_UTF_DEBUG("Calling queryNrConfig");
            utfQtiRadioAidl->queryNrConfig(*static_cast<int32_t*>(t));
            return 0;
          } else {
            return 1;
          }
        case RIL_REQUEST_SET_NR_DISABLE_MODE:
          RIL_UTF_DEBUG("Calling RIL_REQUEST_SET_NR_DISABLE_MODE");
          if (utfQtiRadioAidl != nullptr) {
            RIL_UTF_DEBUG("Calling setNrConfig");
            qtiradioaidl::NrConfig mode =
                convert_nr_config_to_aidl(*static_cast<RIL_NR_DISABLE_MODE*>(data));
            utfQtiRadioAidl->setNrConfig(*static_cast<int32_t*>(t), mode);
            return 0;
          } else {
            return 1;
          }
        case RIL_REQUEST_GET_ENHANCED_RADIO_CAPABILITY:
          RIL_UTF_DEBUG("Calling RIL_REQUEST_GET_ENHANCED_RADIO_CAPABILITY");
          if (utfQtiRadioAidl != nullptr) {
            RIL_UTF_DEBUG("Calling getQtiRadioCapability");
            utfQtiRadioAidl->getQtiRadioCapability(*static_cast<int32_t*>(t));
            return 0;
          } else {
            return 1;
          }
        case RIL_QTI_RADIO_REQUEST_START_NETWORK_SCAN:
            if (utfQtiRadioAidl != nullptr) {
                RIL_UTF_DEBUG("Calling startNetworkScan");
                qtiradioaidl::QtiNetworkScanRequest nsRequest = {};
                RIL_NetworkScanRequest* rilReq = static_cast<RIL_NetworkScanRequest*>(data);
                convertNetworkScanRequestToAidl(*rilReq, nsRequest);
                utfQtiRadioAidl->startNetworkScan(*static_cast<int32_t*>(t), nsRequest);
                return 0;
            }
            break;
        case RIL_QTI_RADIO_REQUEST_QUERY_FACILITY_LOCK:
            if (utfQtiRadioAidl != nullptr) {
                RIL_UTF_DEBUG("Calling getFacilityLockForApp for QtiRadio");
                char **req = (char**)data;
                qtiradioaidl::FacilityLockInfo fli;
                fli.facility = req[0] ? req[0] : "";
                fli.password = req[1] ? req[1] : "";
                fli.serviceClass = req[2] ? atoi(req[2]) : 0;
                fli.appId = req[3] ? req[3] : "";
                fli.expectMore = atoi(req[4]) ? 1 : 0;
                utfQtiRadioAidl->getFacilityLockForApp(*static_cast<int32_t*>(t), fli);
                return 0;
            }
            break;
        default:
            return -1;
    }
    return 1;
}

static void __attribute__((constructor)) registerPlatform() {
    static AndroidQtiRadioAidl aidlQtiRadio;
    setPlatform(&aidlQtiRadio);
}
