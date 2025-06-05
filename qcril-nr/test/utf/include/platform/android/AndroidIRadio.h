/*===========================================================================
 *
 * Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
 * All rights reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 *===========================================================================*/

#include <aidl/android/hardware/radio/config/IRadioConfig.h>
#include <aidl/android/hardware/radio/messaging/IRadioMessaging.h>
#include <aidl/android/hardware/radio/modem/IRadioModem.h>
#include <aidl/android/hardware/radio/network/IRadioNetwork.h>
#include <aidl/android/hardware/radio/sim/IRadioSim.h>
#include <aidl/android/hardware/radio/voice/IRadioVoice.h>
#include <aidl/vendor/qti/hardware/radio/qtiradioconfig/IQtiRadioConfig.h>

#include "ril_service.h"
#include "ril_utf_hidl_services.h"
#include "ril_utf_ril_api.h"
#include "platform/common/CommonPlatform.h"
#include "platform/android/IRadioResponseClientImpl.h"
#include "platform/android/IRadioIndicationClientImpl.h"

#include <android/hardware/radio/1.6/IRadio.h>

namespace aidlradio {
  using namespace aidl::android::hardware::radio;
}
namespace aidlconfig {
    using namespace aidl::android::hardware::radio::config;
}
namespace aidlmessaging {
    using namespace aidl::android::hardware::radio::messaging;
}
namespace aidlmodem {
    using namespace aidl::android::hardware::radio::modem;
}
namespace aidlnetwork {
    using namespace aidl::android::hardware::radio::network;
}
namespace aidlsim {
    using namespace aidl::android::hardware::radio::sim;
}
namespace aidlvoice {
    using namespace aidl::android::hardware::radio::voice;
}
namespace aidlqtiradioconfig {
    using namespace aidl::vendor::qti::hardware::radio::qtiradioconfig;
}

#ifdef OFF
#undef OFF
#endif

class AndroidIRadio : public CommonPlatform {
public:
    AndroidIRadio() {
        utfIRadio = nullptr;
        utfIRadioConfig = nullptr;
        utfIRadioMessaging = nullptr;
        utfIRadioModem = nullptr;
        utfIRadioNetwork = nullptr;
        utfIRadioSim = nullptr;
        utfIRadioVoice = nullptr;
        utfQtiRadioConfig = nullptr;

        bFirstCall = true;
        dataOnRequest = [](::android::sp<::android::hardware::radio::V1_6::IRadio>, int, void *, size_t, RIL_Token) {
            return 1;
        };
    }

    void Register(RIL_RadioFunctions *callbacks) override;
    int OnRequest(int request, void *data, size_t datalen,
        RIL_Token t) override;
    void setDataAPIs(::android::sp<IRadioResponseClientImpl>, ::android::sp<IRadioIndicationClientImpl>,
        std::function<int(::android::sp<::android::hardware::radio::V1_6::IRadio> utfIRadioData, int, void *, size_t, RIL_Token)> onRequest);
    void handle_sync_response(void *payload, int payload_len, unsigned long msg_id, int token_id,
        RIL_Errno e, enum ril_utf_q_element_t type);

private:
    ::android::sp<::android::hardware::radio::V1_6::IRadio> utfIRadio;
    ::android::sp<IRadioResponseClientImpl> dataRespClient;
    ::android::sp<IRadioIndicationClientImpl> dataIndClient;
    std::function<int(::android::sp<::android::hardware::radio::V1_6::IRadio> utfIRadioData, int, void *, size_t, RIL_Token)> dataOnRequest;
    bool bFirstCall;

    std::shared_ptr<::aidlconfig::IRadioConfig> utfIRadioConfig;
    std::shared_ptr<::aidlmessaging::IRadioMessaging> utfIRadioMessaging;
    std::shared_ptr<::aidlmodem::IRadioModem> utfIRadioModem;
    std::shared_ptr<::aidlnetwork::IRadioNetwork> utfIRadioNetwork;
    std::shared_ptr<::aidlsim::IRadioSim> utfIRadioSim;
    std::shared_ptr<::aidlvoice::IRadioVoice> utfIRadioVoice;
    std::shared_ptr<::aidlqtiradioconfig::IQtiRadioConfig> utfQtiRadioConfig;
};

AndroidIRadio& getAndroidIRadio();
