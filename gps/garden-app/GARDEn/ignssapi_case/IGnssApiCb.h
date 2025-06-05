/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
  Copyright (c) 2021-2022 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/
#ifndef IGNSSAPI_CB_H
#define IGNSSAPI_CB_H
#include "IGnssApiCase.h"

namespace garden {

class IGnssApiCase;
class IGnssApiCb : public GnssCbBase {
public:
    IGnssApiCb(IGnssApiCase* gCase);
    ~IGnssApiCb();
    void gnssLocationCb(const Location& location) override;
    void gnssStatusCb(uint32_t status) override;
    void gnssSvStatusCb(const GnssSvNotification& svNotify) override;
    void gnssNmeaCb(uint64_t timestamp, const char* nmea, int length) override;
    void gnssMeasurementsCb(
            const GnssMeasurementsNotification& gnssMeasurementsNotification) override;
    void gnssDataCb(const GnssDataNotification& dataNotify) override;
    void gnssNiNotifyCb(uint32_t id, const GnssNiNotification& notification) override;
    void gnssLocInfoNotifCb(GnssLocationInfoNotification gnssLocInfoNotif) override;

    void setNiParam(int niCount, int niResPatCount, int* networkInitiatedResponse);

private:
    IGnssApiCase* mGnssCase = nullptr;
    std::vector<int> mNetworkInitiatedResponse;
};

} //namespace garden

#endif //IGNSSAPI_CB_H
