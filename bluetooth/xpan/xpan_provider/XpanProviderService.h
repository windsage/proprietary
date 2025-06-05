/*
 * Copyright (c) 2022 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef XPAN_PROVIDER_SERVICE_H
#define XPAN_PROVIDER_SERVICE_H

#include <aidl/vendor/qti/hardware/bluetooth/xpanprovider/BnXpanProvider.h>

namespace aidl {
namespace vendor {
namespace qti {
namespace hardware {
namespace bluetooth {
namespace xpanprovider {

class XpanProviderService : public BnXpanProvider {
  public:
    std::shared_ptr<IXpanProviderCallback> xpanProviderCb;

    XpanProviderService();
    ~XpanProviderService();

    /* XpanProvider AIDL HAL calls */
    ::ndk::ScopedAStatus registerXpanCallbacks(
        const std::shared_ptr<IXpanProviderCallback>& in_cb) override;
    ::ndk::ScopedAStatus sendXpanCommand(const std::vector<uint8_t>& in_cmd) override;

    /* parser API's for commands from Xpan Profile*/
    void updateXpanBondedDevices(const std::vector<uint8_t>& data);
    void updateTransport(const std::vector<uint8_t>& data);
    void enableSapAcs(const std::vector<uint8_t>& data);
    void updateBearerSwitched(const std::vector<uint8_t>& data);
    void updateTwtSessionParams(const std::vector<uint8_t>& data);
    void updateBearerPrepared(const std::vector<uint8_t>& data);
    void updateLowPowerMode(const std::vector<uint8_t>& data);
    void updateHostParams(const std::vector<uint8_t>& data);
    void updateSapState(const std::vector<uint8_t>& data);
    void updateVbcPeriodicity(const std::vector<uint8_t>& data);
    void updateWifiScanStarted(const std::vector<uint8_t>& data);
    void createSapInterface(const std::vector<uint8_t>& data);
    void updateBearerPreferenceReq(const std::vector<uint8_t>& data);
    void updateClearToSendReq(const std::vector<uint8_t>& data);
    void updateAirplaneModeChanged(const std::vector<uint8_t>& data);
    void updateWifiTransportPreference(const std::vector<uint8_t>& data);
    void connectLeLink(const std::vector<uint8_t>& data);
    void updateApDetailsLocal(const std::vector<uint8_t>& data);
    void updateApDetailsRemote(const std::vector<uint8_t>& data);
    void getPortDetails(const std::vector<uint8_t>& data);
    void updateMdnsStatus(const std::vector<uint8_t>& data);
    void updateBondState(const std::vector<uint8_t>& data);
    void updatemDnsRecord(const std::vector<uint8_t>& data);
    void setApAvailableReq(const std::vector<uint8_t>& data);
    void updateConnectedEbDetails(const std::vector<uint8_t>& data);

    /* XpanProfile callback */
    void sendXpanEvent(const std::vector<uint8_t>& data);

    static void clientDeathRecipient(void* cookie);
};

} // xpanprovider
} // bluetooth
} // hardware
} // qti
} // vendor
} // aidl

#endif
