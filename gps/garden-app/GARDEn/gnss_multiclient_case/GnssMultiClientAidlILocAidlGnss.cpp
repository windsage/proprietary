/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
  Copyright (c) 2019-2022, 2024 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
===========================================================================*/

#define LOG_NDEBUG 0
#define LOG_TAG "GARDEN_GMCC_ILocAidlGnss"

#include "GnssMultiClientAidlILocAidlGnss.h"
#include "GnssMultiClientCaseUtils.h"

#include <android/binder_auto_utils.h>
#include <android/binder_ibinder_platform.h>
#include <android/binder_libbinder.h>
#include <android/binder_manager.h>
#include <android/binder_process.h>

using ::aidl::vendor::qti::gnss::ILocAidlGnss;
using ::aidl::vendor::qti::gnss::LocAidlBoolDataItem;

namespace garden {

#define getUserInputEnterToContinue GmccUtils::get().getUserInputEnterToContinue
#define getUserInputInt GmccUtils::get().getUserInputInt
#define getUserInputDouble GmccUtils::get().getUserInputDouble
#define getUserInputMask64Bit GmccUtils::get().getUserInputMask64Bit
#define getUserInputString GmccUtils::get().getUserInputString
#define getUserInputYesNo GmccUtils::get().getUserInputYesNo
#define getUserInputSessionMode GmccUtils::get().getUserInputSessionMode
#define getUserInputTrackingOptions GmccUtils::get().getUserInputTrackingOptions
#define getUserInputLocClientIndex GmccUtils::get().getUserInputLocClientIndex
#define getUserInputGnssConfig GmccUtils::get().getUserInputGnssConfig
#define getUserInputGnssConfigFlagsMask GmccUtils::get().getUserInputGnssConfigFlagsMask
#define getUserInputGnssConfigBlacklistSvId GmccUtils::get().getUserInputGnssConfigBlacklistSvId
#define getUserInputGnssSvIdSource GmccUtils::get().getUserInputGnssSvIdSource
#define getUserInputGnssSvType GmccUtils::get().getUserInputGnssSvType
#define getUserInputGnssSvTypesMask GmccUtils::get().getUserInputGnssSvTypesMask
#define printGnssConfigBlacklistSvId GmccUtils::get().printGnssConfigBlacklistSvId
#define convertGnssSvIdMaskToList GmccUtils::get().convertGnssSvIdMaskToList
#define strUtilTokenize GmccUtils::get().strUtilTokenize


/* GnssMultiClientAidlILocAidlGnss static elements */
GnssMultiClientAidlILocAidlGnss* GnssMultiClientAidlILocAidlGnss::sInstance = nullptr;

/* GnssMultiClientAidlILocAidlGnss Public APIs */
GARDEN_RESULT GnssMultiClientAidlILocAidlGnss::menuTest()
{
    bool exit_loop = false;
    GARDEN_RESULT gardenResult = GARDEN_RESULT_INVALID;

    while (!exit_loop) {
        PRINTLN("\n\n"
                "1: Create AIDL client for ILocAidlGnss \n"
                "1001: ILocAidlIzatConfig -> init \n"
                "1002: ILocAidlIzatConfig -> readConfig \n"
                "2001: ILocAidlIzatSubscription -> init \n"
                "2002: ILocAidlIzatSubscription -> boolDataItemUpdate (Opt-in)\n"
                "3001: ILocAidlIzatConfig_init\n"
                "3002: ILocAidlIzatConfig_reset\n"
                "3003: ILocAidlGnssConfigService_setGnssSvTypeConfig\n"
                "3004: ILocAidlGnssConfigService_getGnssSvTypeConfig\n"
                "99: Display this menu again \n"
                "0: <-- back\n");
        int choice = getUserInputInt("Enter choice: ");

        switch (choice) {
        case 1:
            gardenResult = createAidlClient();
            break;
        case 1001:
            gardenResult = ILocAidlIzatConfig_init();
            break;
        case 1002:
            gardenResult = ILocAidlIzatConfig_readConfig();
            break;
        case 2001:
            gardenResult = ILocAidlIzatSubscription_init();
            break;
        case 2002:
            {
                int result = getUserInputInt("Enable: 1, disable: 0");
                gardenResult = ILocAidlIzatSubscription_boolDataItemUpdate(result);
                break;
            }
        case 3001:
                gardenResult = ILocAidlGnssConfigService_init();
                break;
        case 3002:
                gardenResult = ILocAidlGnssConfigService_reset();
                break;
        case 3003:
            {
                PRINTLN("Constellation allowed to disable: glo / gal / bds / qzss/ navic");
                GnssSvTypesMask enableMask = getUserInputGnssSvTypesMask
                    ("*** Enable constellation Input ***");
                GnssSvTypesMask disableMask = getUserInputGnssSvTypesMask
                    ("*** disable constellation Input ***");
                if (!(enableMask & disableMask)) {
                    gardenResult = ILocAidlGnssConfigService_setGnssSvTypeConfig(enableMask,
                        disableMask);
                } else {
                    PRINTERROR("Same Constallation cannot be in enable and disable mask!!!");
                }
                break;
            }
        case 3004:
                gardenResult = ILocAidlGnssConfigService_getGnssSvTypeConfig();
                break;
        case 99:
            continue;
        case 0:
            gardenResult = GARDEN_RESULT_ABORT;
            exit_loop = true;
            break;
        default:
            PRINTERROR("Invalid command");
        }

        if (0 != choice) {
            PRINTLN("\nExecution Result: %d", gardenResult);
            getUserInputEnterToContinue();
        }
    }

    return gardenResult;
}


template <class T>
GARDEN_RESULT GnssMultiClientAidlILocAidlGnss::checkResultOk(T& Result,
    const std::string printMsg) {
    if (Result.isOk()) {
        PRINTLN("%s success.", printMsg.c_str());
        return GARDEN_RESULT_PASSED;
    } else {
        PRINTERROR("%s failed.", printMsg.c_str());
        return GARDEN_RESULT_FAILED;
    }
}

void GnssMultiClientAidlILocAidlGnss::initVendorHal() {
    if (mLocAidlGnssIface == nullptr) {
        LOC_LOGd("Getting LocAidl service");
        ndk::SpAIBinder binder(AServiceManager_getService("vendor.qti.gnss.ILocAidlGnss/default"));
        mLocAidlGnssIface = ILocAidlGnss::fromBinder(binder);
        if (mLocAidlGnssIface != nullptr) {
            mLocAidlGnssIface = mLocAidlGnssIface;
        } else {
            PRINTERROR("Failed to get LocAidl service.");
        }
    }
}

/* GnssMultiClientAidlILocAidlGnss TEST CASES */
GARDEN_RESULT GnssMultiClientAidlILocAidlGnss::createAidlClient()
{
    GARDEN_RESULT gardenResult = GARDEN_RESULT_FAILED;
    if (mLocAidlGnssIface) {
        PRINTLN("ILocAidlGnss instance already initalized!");
        return GARDEN_RESULT_PASSED;
    }
    // Get IGNSS service
    initVendorHal();
    if (mLocAidlGnssIface != nullptr) {
        PRINTLN("ILocAidlGnss::getService() success.");

        // Get Extension : IGnssConfiguration
        auto status = mLocAidlGnssIface->getExtensionLocAidlIzatConfig(
                                            &mLocAidlGnssExtensionIzatConfig);
        gardenResult = checkResultOk(status,
                           "mLocAidlGnssIface->getExtensionLocAidlIzatConfig");

        // Get Extension : IzatSubscribtion
        status = mLocAidlGnssIface->getExtensionLocAidlIzatSubscription(
                                            &mLocAidlIzatSubscription);
        gardenResult = checkResultOk(status,
                           "mLocAidlGnssIface->getExtensionLocAidlIzatSubscription");

        // Get Extension : ILocAidlGnssConfigService
        status = mLocAidlGnssIface->getExtensionLocAidlGnssConfigService(
                                        &mLocAidlGnssConfigService);
        gardenResult = checkResultOk(status,
            "mLocAidlGnssIface->getExtensionLocAidlGnssConfigService()");
    } else {
        PRINTERROR("ILocAidlGnss::getService() call failed.");
    }

    return gardenResult;
}

template <class T>
::ndk::ScopedAStatus GnssMultiClientAidlILocAidlGnss::ILocAidlExtinit(T& ext) {
    bool retVal = false;
    return ext->init(mLocAidlGnssCb, &retVal);
}

GARDEN_RESULT GnssMultiClientAidlILocAidlGnss::ILocAidlIzatConfig_init()
{
    GARDEN_RESULT gardenResult = GARDEN_RESULT_FAILED;

    if (mLocAidlGnssIface != nullptr) {
        if (mLocAidlGnssExtensionIzatConfig != nullptr) {
            auto result = ILocAidlExtinit(mLocAidlGnssExtensionIzatConfig);
            gardenResult = checkResultOk(result, "mLocAidlGnssExtensionIzatConfig->init");
        } else {
            PRINTERROR("mLocAidlGnssExtensionIzatConfig null");
        }
    } else {
        PRINTERROR("LOC AIDL client not created.");
    }

    return gardenResult;
}

GARDEN_RESULT GnssMultiClientAidlILocAidlGnss::ILocAidlIzatConfig_readConfig()
{
    GARDEN_RESULT gardenResult = GARDEN_RESULT_FAILED;

    if (mLocAidlGnssIface != nullptr) {
        if (mLocAidlGnssExtensionIzatConfig != nullptr) {
            bool readConfigSuccess = false;
            auto result = mLocAidlGnssExtensionIzatConfig->readConfig(&readConfigSuccess);
            gardenResult = checkResultOk(result, "mLocAidlGnssExtensionIzatConfig->readConfig");
        } else {
            PRINTERROR("mLocAidlGnssExtensionIzatConfig null");
        }
    } else {
        PRINTERROR("LOC AIDL client not created.");
    }
    return gardenResult;
}

GARDEN_RESULT GnssMultiClientAidlILocAidlGnss::ILocAidlIzatSubscription_init() {
    GARDEN_RESULT gardenResult = GARDEN_RESULT_FAILED;
    if (mLocAidlGnssIface != nullptr) {
        if (mLocAidlIzatSubscription != nullptr) {
            auto result = ILocAidlExtinit(mLocAidlIzatSubscription);
            gardenResult = checkResultOk(result, "mLocAidlIzatSubscription->init");
        } else {
            PRINTERROR("mLocAidlIzatSubscription null");
        }
    } else {
        PRINTERROR("LOC AIDL client not created.");
    }
    return gardenResult;
}

GARDEN_RESULT GnssMultiClientAidlILocAidlGnss::ILocAidlIzatSubscription_boolDataItemUpdate
    (int result) {
    std::vector<LocAidlBoolDataItem> dataItemArray;
    LocAidlBoolDataItem dataItem;
    dataItem.id = ENH_DATA_ITEM_ID;
    dataItem.enabled = (result > 0) ? true: false;
    dataItemArray.push_back(dataItem);
    auto status = mLocAidlIzatSubscription->boolDataItemUpdate(dataItemArray);
    return checkResultOk(status, "mLocAidlIzatSubscription->boolDataItemUpdate");
}

GARDEN_RESULT GnssMultiClientAidlILocAidlGnss::ILocAidlGnssConfigService_init() {
    GARDEN_RESULT gardenResult = GARDEN_RESULT_FAILED;
    if (mLocAidlGnssIface != nullptr) {
        if (mLocAidlGnssConfigService != nullptr) {
            auto result = ILocAidlExtinit(mLocAidlGnssConfigService);
            gardenResult = checkResultOk(result, "mLocAidlGnssConfigService->init");
        } else {
            PRINTERROR("mLocAidlGnssConfigService null");
        }
    } else {
        PRINTERROR("LOC AIDL client not created.");
    }
    return gardenResult;
}

GARDEN_RESULT GnssMultiClientAidlILocAidlGnss::ILocAidlGnssConfigService_reset() {
    GARDEN_RESULT gardenResult = GARDEN_RESULT_FAILED;
    if (mLocAidlGnssConfigService != nullptr) {
       auto r = mLocAidlGnssConfigService->resetGnssSvTypeConfig();
       gardenResult = checkResultOk(r, "mLocAidlGnssConfigService->resetGnssSvTypeConfig()");
    }
    return gardenResult;
}

template <class T>
void GnssMultiClientAidlILocAidlGnss::convertGnssTypeMaskToConstellationVec(GnssSvTypesMask
    enableMask, GnssSvTypesMask disableMask,
    std::vector<T>& disableVec) {
    //push disable constellation
    if (GNSS_SV_TYPES_MASK_GPS_BIT & disableMask) {
        disableVec.push_back(T::GPS);
    }
    if (GNSS_SV_TYPES_MASK_GLO_BIT & disableMask) {
        disableVec.push_back(T::GLONASS);
    }
    if (GNSS_SV_TYPES_MASK_BDS_BIT & disableMask) {
        disableVec.push_back(T::BEIDOU);
    }
    if (GNSS_SV_TYPES_MASK_QZSS_BIT & disableMask) {
        disableVec.push_back(T::QZSS);
    }
    if (GNSS_SV_TYPES_MASK_GAL_BIT & disableMask) {
        disableVec.push_back(T::GALILEO);
    }

   //enable constellation as inverted
    if (GNSS_SV_TYPES_MASK_GPS_BIT & enableMask) {
        disableVec.push_back((T)(~GnssMultiClientAidlILocAidlGnssCb::
            convertConstellationToint(T::GPS)));
    }
    if (GNSS_SV_TYPES_MASK_GLO_BIT & enableMask) {
        disableVec.push_back((T)(~GnssMultiClientAidlILocAidlGnssCb::
            convertConstellationToint(T::GLONASS)));
    }
    if (GNSS_SV_TYPES_MASK_BDS_BIT & enableMask) {
        disableVec.push_back((T)(~GnssMultiClientAidlILocAidlGnssCb::
            convertConstellationToint(T::BEIDOU)));
    }
    if (GNSS_SV_TYPES_MASK_QZSS_BIT & enableMask) {
        disableVec.push_back((T)(~GnssMultiClientAidlILocAidlGnssCb::
            convertConstellationToint(T::QZSS)));
    }
    if (GNSS_SV_TYPES_MASK_GAL_BIT & enableMask) {
        disableVec.push_back((T)(~GnssMultiClientAidlILocAidlGnssCb::
            convertConstellationToint(T::GALILEO)));
    }
}


GARDEN_RESULT GnssMultiClientAidlILocAidlGnss::ILocAidlGnssConfigService_setGnssSvTypeConfig(
    GnssSvTypesMask enableMask, GnssSvTypesMask disableMask) {
    GARDEN_RESULT gardenResult = GARDEN_RESULT_FAILED;
    PRINTLN("Enabled mask: 0x%" PRIx64 ", Disable Mask: %" PRIx64, enableMask, disableMask);
    if (mLocAidlGnssIface != nullptr && mLocAidlGnssConfigService != nullptr) {
        std::vector<LocAidlGnssConstellationType> disableVec = {};
        if (disableMask & GNSS_SV_TYPES_MASK_NAVIC_BIT) {
            disableVec.push_back(LocAidlGnssConstellationType::IRNSS);
        }
        convertGnssTypeMaskToConstellationVec(enableMask, disableMask, disableVec);
        if (enableMask & GNSS_SV_TYPES_MASK_NAVIC_BIT) {
            disableVec.push_back((LocAidlGnssConstellationType)(~(NAVIC)));
        }
        auto r = mLocAidlGnssConfigService->setGnssSvTypeConfig(disableVec);
        gardenResult = checkResultOk(r, "mLocAidlGnssConfigService->setGnssSvTypeConfig");
    } else {
        PRINTERROR("Unable to execute ILocAidlGnssConfigService_setGnssSvTypeConfig");
    }
    return gardenResult;
}

GARDEN_RESULT GnssMultiClientAidlILocAidlGnss::ILocAidlGnssConfigService_getGnssSvTypeConfig() {
    GARDEN_RESULT gardenResult = GARDEN_RESULT_FAILED;
    if (mLocAidlGnssIface != nullptr && mLocAidlGnssConfigService != nullptr) {
        auto r = mLocAidlGnssConfigService->getGnssSvTypeConfig();
        gardenResult = checkResultOk(r, "mLocAidlGnssConfigService->getGnssSvTypeConfig");
    }
    return gardenResult;
}
} // namespace garden
