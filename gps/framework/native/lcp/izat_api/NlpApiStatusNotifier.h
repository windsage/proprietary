/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
  Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/

#ifndef NLP_NOTIFIER_H
#define NLP_NOTIFIER_H

#include <vector>
#include "IzatRemoteApi.h"
#include "gps_extended.h"

#define AP_SSID_PREFIX_LENGTH            8

namespace izat_remote_api {

typedef struct {
    uint8_t networkType;
    uint64_t networkHandle;
} Network;

class NlpApiStatusNotifier : public IzatNotifier {
protected:
    NlpApiStatusNotifier();
    virtual inline ~NlpApiStatusNotifier() { }
public:
    static std::string sName;
    virtual void handleMsg(qc_loc_fw::InPostcard *const in_card) final;

    virtual void onOptInStatusUpdate(bool isOptIn) = 0;
    virtual void onLocationRequestUpdate(bool isEmergency) = 0;
    virtual void onNetworkStatusUpdate(bool isConected, Network* networksAvailable,
            uint8_t networksAvailableCount) = 0;
    virtual void onLocationChange(UlpLocation& ulpLocation, GpsLocationExtended& locExt) = 0;

protected:
    bool requestOrReleaseDataConnection(bool connectIfTrueElseRelease);
    inline BackhaulContext& getBackhaulContext() {
        return mBackhaulContext;
    }
    int registerOSAgentUpdate();
    int registerLocationUpdate();

private:
    void handleEmergencyStatusChange(qc_loc_fw::InPostcard * const in_msg);
    void handleOptInStatusChange(qc_loc_fw::InPostcard * const in_msg);
    void handleConnectivityStatusChange(qc_loc_fw::InPostcard * const in_msg);
    void handleLocationChange(qc_loc_fw::InPostcard * const in_msg);

    BackhaulContext mBackhaulContext;
};

} //izat_remote_api

#endif // #ifndef NLP_NOTIFIER_H
