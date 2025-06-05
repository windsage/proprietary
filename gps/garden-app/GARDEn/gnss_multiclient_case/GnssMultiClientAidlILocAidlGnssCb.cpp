/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
  Copyright (c) 2019-2021 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
===========================================================================*/
#define LOG_NDEBUG 0
#define LOG_TAG "GARDEN_GMCC_ILocAidlGnssCb"

#include "GnssMultiClientAidlILocAidlGnssCb.h"
#include "GnssMultiClientCaseUtils.h"

namespace garden {

GnssMultiClientAidlILocAidlGnssCb::GnssMultiClientAidlILocAidlGnssCb(
        GnssMultiClientAidlILocAidlGnss* aidlGnss)
{
    mLocAidlGnss = aidlGnss;
}

GnssMultiClientAidlILocAidlGnssCb::~GnssMultiClientAidlILocAidlGnssCb() {}

::ndk::ScopedAStatus GnssMultiClientAidlILocAidlGnssCb::izatConfigCallback(
        const std::string& izatConfigContent)
{
    PRINTLN("izatConfigCallback()");
    PRINTLN("izat.conf content:\n%s", izatConfigContent.c_str());
    return ndk::ScopedAStatus::ok();
}

//ILocAidlGnssConfigServiceCb
::ndk::ScopedAStatus GnssMultiClientAidlILocAidlGnssCb::getGnssSvTypeConfigCb(
        const std::vector<LocAidlGnssConstellationType>& disabledSvTypeList)
{
    PRINTLN("getGnssSvTypeConfigCb >>> ");
    std::string enableMsg, disableMsg;
    for (LocAidlGnssConstellationType type : disabledSvTypeList) {
        if (type == LocAidlGnssConstellationType::IRNSS) {
            disableMsg.append(std::to_string(NAVIC) + " ");
            continue;
        }
        if (NAVIC == ((uint8_t)((~(uint32_t)type)))) {
            enableMsg.append(std::to_string(NAVIC) + " ");
            continue;
        }
        int constellation = GnssMultiClientAidlILocAidlGnssCb::convertConstellationToint(type);
        if (0 != constellation) {
            disableMsg.append(std::to_string(constellation) + " ");
        } else {
            enableMsg.append(std::to_string((uint8_t)(~(uint32_t)type)) + " ");
        }
    }
    PRINTLN("GPS: 1, SBAS: 2,GLONASS: 3, QZSS: 4, BEIDUO:5, GALILEO:6, NAVIC:7");
    PRINTLN("Constellation enable: %s \nConstellation disable: %s", enableMsg.c_str(),
        disableMsg.c_str());
    return ndk::ScopedAStatus::ok();
}
} // namespace garden
