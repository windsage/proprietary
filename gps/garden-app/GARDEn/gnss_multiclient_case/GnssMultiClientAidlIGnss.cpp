/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
  Copyright (c) 2021-2022, 2024 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
===========================================================================*/

#define LOG_NDEBUG 0
#define LOG_TAG "GARDEN_GMCC_AidlIGnss"

#include "GnssMultiClientAidlIGnss.h"
#include "GnssMultiClientCaseUtils.h"

using aidl::android::hardware::gnss::GnssConstellationType;
using aidl::android::hardware::gnss::IAGnssCallback;
namespace garden {

#define getUserInputEnterToContinue GmccUtils::get().getUserInputEnterToContinue
#define getUserInputInt GmccUtils::get().getUserInputInt
#define getUserInputDouble GmccUtils::get().getUserInputDouble
#define getUserInputString GmccUtils::get().getUserInputString
#define getUserInputYesNo GmccUtils::get().getUserInputYesNo
#define getUserInputMask64Bit GmccUtils::get().getUserInputMask64Bit

/* GnssMultiClientAidlIGnss static elements */
GnssMultiClientAidlIGnss* GnssMultiClientAidlIGnss::sInstance = nullptr;

/* GnssMultiClientAidlIGnss Public APIs */
GARDEN_RESULT GnssMultiClientAidlIGnss::menuTest() {
    bool exit_loop = false;
    GARDEN_RESULT gardenResult = GARDEN_RESULT_INVALID;

    while (!exit_loop) {
        PRINTLN("\n\n"
                "1: Create AIDL client for IGnss \n"
                "1001: IGnss            -> start \n"
                "1002: IGnss            -> stop \n"
                "1003: IGnss            -> setPositionMode \n"
                "1004: IGnss            -> setCallback \n"
                "1005: IGnss            -> injectBestLocation \n"
                "1006: IGnss            -> close \n"
                "1007: IGnss            -> startSvStatus \n"
                "1008: IGnss            -> stopSvStatus \n"
                "1009: IGnss            -> startNmea \n"
                "1010: IGnss            -> stopNmea \n"
                "2001: IGnssConfiguration   -> setSuplVersion \n"
                "2002: IGnssConfiguration   -> setSuplMode \n"
                "2003: IGnssConfiguration  -> setLppProfile \n"
                "2004: IGnssConfiguration  -> setGlonassPositioningProtocol \n"
                "2005: IGnssConfiguration  -> setEmergencySuplPdn \n"
                "2006: IGnssConfiguration  -> setEsExtensionSec \n"
                "2007: IGnssConfiguration  -> setBlocklist \n"
                "3001: IGnssDebug  -> getDebugData \n"
                "4001: IGnssVisibilityControl -> setCallback \n"
                "4002: IGnssVisibilityControl -> enable/disable Nfw \n"
                "5001: IGnssBatching  -> init \n"
                "6001: IAGnss -> setCallback \n"
                "6002: IAGnss -> dataConnOpen \n"
                "6003: IAGnss -> setServer \n"
                "6004: IAGnss -> dataConnClosed \n"
                "7001: IGnssMeasurement  -> setCallback \n"
                "7002: IGnssMeasurement  -> close \n"
                "8001: IPowerIndication -> setCallback \n"
                "8002: IPowerIndication -> requestGnssPowerStats \n"
                "9001: IGnssAntennaInfo  -> setCallback \n"
                "9002: IGnssAntennaInfo  -> close \n"
                "1101: IGnss  -> Open Measurement Corrections Interface \n"
                "1102: IGnss  -> start with measurement corrections \n"

                "99: Display this menu again \n"
                "0: <-- back\n");
        int choice = getUserInputInt("Enter choice: ");

        switch (choice) {
        case 1:
            gardenResult = createAidlClient();
            break;
        case 7001:
            gardenResult = IGnssMeasurement_setCallback();
            break;
        case 7002:
            gardenResult = IGnssMeasurement_close();
            break;
        case 8001:
            gardenResult = IPowerIndication_setCallback();
            break;
        case 8002:
            gardenResult = IPowerIndication_requestGnssPowerStats();
            break;
        case 1001:
            mMeasurementCorrectionsAllowed = false;
            gardenResult = IGnss_start();
            break;
        case 1002:
            gardenResult = IGnss_stop();
            break;
        case 1003:
            gardenResult = IGnss_setPositionMode();
            break;
        case 1004:
            gardenResult = IGnss_setCallback();
            break;
        case 1005:
            gardenResult = IGnss_injectBestLocation();
            break;
        case 1006:
            gardenResult = IGnss_close();
            break;
        case 1007:
            gardenResult = IGnss_startSvStatus();
            break;
        case 1008:
            gardenResult = IGnss_stopSvStatus();
            break;
        case 1009:
            gardenResult = IGnss_startNmea();
            break;
        case 1010:
            gardenResult = IGnss_stopNmea();
            break;
        case 2001:
            gardenResult = IGnssConfiguration_setSuplVersion();
            break;
        case 2002:
            gardenResult = IGnssConfiguration_setSuplMode();
            break;
        case 2003:
            gardenResult = IGnssConfiguration_setLppProfile();
            break;
        case 2004:
            gardenResult = IGnssConfiguration_setGlonassPositioningProtocol();
            break;
        case 2005:
            gardenResult = IGnssConfiguration_setEmergencySuplPdn();
            break;
        case 2006:
            gardenResult = IGnssConfiguration_setEsExtensionSec();
            break;
        case 2007:
            gardenResult = IGnssConfiguration_setBlocklist();
            break;
        case 3001:
            gardenResult = IGnssDebug_getDebugData();
            break;
        case 4001:
            gardenResult = IGnssVisibilityControl_setCallback();
            break;
        case 4002:
            gardenResult = IGnssVisibilityControl_controlNfwLocationAccess();
            break;
        case 5001:
            gardenResult = IGnssBatching_init();
            break;
        case 6001:
            gardenResult = IAGnss_setCallback();
            break;
        case 6002:
            gardenResult = IAGnss_dataConnOpen();
            break;
        case 6003:
            gardenResult = IAGnss_setServer();
            break;
        case 6004:
            gardenResult = IAGnss_dataConnClosed();
            break;
        case 9001:
            gardenResult = IGnss_setAntennaInfoCallback();
            break;
        case 9002:
            gardenResult = IGnss_setAntennaInfoClose();
            break;
        case 1101:
            gardenResult = IGnss_setMeasurementCorrectionsCallback();
            break;
        case 1102:
            mMeasurementCorrectionsAllowed = true;
            gardenResult = IGnss_start();
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

/* GnssMultiClientAidlIGnss TEST CASES */
GARDEN_RESULT GnssMultiClientAidlIGnss::createAidlClient() {
    GARDEN_RESULT gardenResult = GARDEN_RESULT_FAILED;
    ndk::SpAIBinder binder(AServiceManager_getService("android.hardware.gnss.IGnss/default"));
    mGnssHalAidl = IGnssAidl::fromBinder(binder);
    if (mGnssHalAidl != nullptr) {
        mAidlIGnssCb = SharedRefBase::make<GnssMultiClientAidlIGnssCb>();
        auto status = mGnssHalAidl->setCallback(mAidlIGnssCb);
        if (!status.isOk()) {
            PRINTERROR("mGnssHalAidl->setCallback() failed.");
        }
        shared_ptr<IGnssMeasurementInterfaceAidl> iGnssMeasurement;
        status = mGnssHalAidl->getExtensionGnssMeasurement(&iGnssMeasurement);
        if (status.isOk()) {
            mGnssMeasurementIface = iGnssMeasurement;
            gardenResult = GARDEN_RESULT_PASSED;
        } else {
            PRINTERROR("mGnssHalAidl->getExtensionGnssMeasurement() failed.");
        }

        shared_ptr<IGnssPowerIndicationAidl> iPowerIndication;
        status = mGnssHalAidl->getExtensionGnssPowerIndication(&iPowerIndication);
        if (status.isOk()) {
            mGnssPowerIndication = iPowerIndication;
            gardenResult = GARDEN_RESULT_PASSED;
        } else {
            PRINTERROR("mGnssHalAidl->getExtensionGnssPowerIndication() failed.");
        }
        shared_ptr<IGnssConfigurationAidl> iGnssConfig;
        status = mGnssHalAidl->getExtensionGnssConfiguration(&iGnssConfig);
        if (status.isOk()) {
            mGnssConfigIface = iGnssConfig;
            gardenResult = GARDEN_RESULT_PASSED;
        } else {
            PRINTERROR("mGnssHalAidl->getExtensionGnssConfiguration() failed.");
        }
        shared_ptr<IGnssDebug> iGnssDebug;
        status = mGnssHalAidl->getExtensionGnssDebug(&iGnssDebug);
        if (status.isOk()) {
            mGnssDebug = iGnssDebug;
            gardenResult = GARDEN_RESULT_PASSED;
        } else {
            PRINTERROR("mGnssHalAidl->getExtensionGnssDebug() failed.");
        }
        shared_ptr<IAGnss> iAGnss;
        status = mGnssHalAidl->getExtensionAGnss(&iAGnss);
        if (status.isOk()) {
            mAGnss = iAGnss;
            gardenResult = GARDEN_RESULT_PASSED;
        } else {
            PRINTERROR("mGnssHalAidl->getExtensionAGnss() failed.");
        }
        shared_ptr<IGnssBatching> iGnssBatching;
        status = mGnssHalAidl->getExtensionGnssBatching(&iGnssBatching);
        if (status.isOk()) {
            mGnssBatching = iGnssBatching;
            gardenResult = GARDEN_RESULT_PASSED;
        } else {
            PRINTERROR("mGnssHalAidl->getExtensionGnssBatching() failed.");
        }
        shared_ptr<IGnssVisibilityControl> iGnssVisibilityControl;
        status = mGnssHalAidl->getExtensionGnssVisibilityControl(&iGnssVisibilityControl);
        if (status.isOk()) {
            mGnssVisibilityControl = iGnssVisibilityControl;
            gardenResult = GARDEN_RESULT_PASSED;
        } else {
            PRINTERROR("mGnssHalAidl->getExtensionGnssVisibilityControl() failed.");
        }
        shared_ptr<IMeasurementCorrectionsInterface> iMeasurementCorrections;
        status = mGnssHalAidl->getExtensionMeasurementCorrections(&iMeasurementCorrections);
        if (status.isOk()) {
            mMeasurementCorrections = iMeasurementCorrections;
            gardenResult = GARDEN_RESULT_PASSED;
        } else {
            PRINTERROR("mGnssHalAidl->getExtensionMeasurementCorrections() failed.");
        }
        shared_ptr<IGnssAntennaInfo> iGnssAntennaInfo;
        status = mGnssHalAidl->getExtensionGnssAntennaInfo(&iGnssAntennaInfo);
        if (status.isOk()) {
            mGnssAntennaInfo = iGnssAntennaInfo;
            gardenResult = GARDEN_RESULT_PASSED;
        } else {
            PRINTERROR("mGnssHalAidl->getExtensionGnssAntennaInfo() failed.");
        }
    } else {
        PRINTERROR("AIDL client not created.");
    }
    return gardenResult;
}

GARDEN_RESULT GnssMultiClientAidlIGnss::IGnssMeasurement_setCallback() {
    GARDEN_RESULT gardenResult = GARDEN_RESULT_FAILED;
    if (mGnssHalAidl != nullptr) {
        if (mGnssMeasurementIface != nullptr) {
            IGnssMeasurementInterfaceAidl::Options options;
            options.enableFullTracking = getUserInputYesNo("Enable Full Tracking (y/n): ");
            options.enableCorrVecOutputs = false;
            options.intervalMs =
                    getUserInputInt("Enter time between measurements in msec: ");
            mAidlIGnssMeasCb = SharedRefBase::make<GnssMultiClientAidlIGnssMeasCb>();

            auto result = mGnssMeasurementIface->setCallbackWithOptions(
                                mAidlIGnssMeasCb, options);
            if (result.isOk()) {
                gardenResult = GARDEN_RESULT_PASSED;
            } else {
                PRINTERROR("mGnssMeasurementIface->setCallback failed.");
            }
        } else {
            PRINTERROR("mGnssMeasurementIface null");
        }
    } else {
        PRINTERROR("AIDL client not created.");
    }
    return gardenResult;
}

GARDEN_RESULT GnssMultiClientAidlIGnss::IGnssMeasurement_close() {
    GARDEN_RESULT gardenResult = GARDEN_RESULT_FAILED;
    if (mGnssHalAidl != nullptr) {
        if (mGnssMeasurementIface != nullptr) {
            auto result = mGnssMeasurementIface->close();
            if (result.isOk()) {
                gardenResult = GARDEN_RESULT_PASSED;
            } else {
                PRINTERROR("mGnssMeasurementIface->close failed.");
            }
        } else {
            PRINTERROR("mGnssMeasurementIface null");
        }
    } else {
        PRINTERROR("AIDL client not created.");
    }
    return gardenResult;
}

GARDEN_RESULT GnssMultiClientAidlIGnss::IPowerIndication_setCallback() {
    GARDEN_RESULT gardenResult = GARDEN_RESULT_FAILED;
    if (mGnssHalAidl != nullptr) {
        if (mGnssPowerIndication != nullptr) {
            mAidlIGnssPowerCb = SharedRefBase::make<GnssMultiClientAidlIGnssPowerIndCb>();
            auto result = mGnssPowerIndication->setCallback(mAidlIGnssPowerCb);
            if (result.isOk()) {
                gardenResult = GARDEN_RESULT_PASSED;
            } else {
                PRINTERROR("mGnssPowerIndication->setCallback failed.");
            }
        } else {
            PRINTERROR("mGnssPowerIndication null");
        }
    } else {
        PRINTERROR("AIDL client not created.");
    }
    return gardenResult;
}

GARDEN_RESULT GnssMultiClientAidlIGnss::IPowerIndication_requestGnssPowerStats() {
    GARDEN_RESULT gardenResult = GARDEN_RESULT_FAILED;
    if (mGnssHalAidl != nullptr) {
        if (mGnssPowerIndication != nullptr) {
            auto result = mGnssPowerIndication->requestGnssPowerStats();
            if (result.isOk()) {
                gardenResult = GARDEN_RESULT_PASSED;
            } else {
                PRINTERROR("mGnssPowerIndication->requestGnssPowerStats failed.");
            }
        } else {
            PRINTERROR("mGnssPowerIndication null");
        }
    } else {
        PRINTERROR("AIDL client not created.");
    }
    return gardenResult;
}
GARDEN_RESULT GnssMultiClientAidlIGnss::IGnss_setCallback() {
    GARDEN_RESULT gardenResult = GARDEN_RESULT_FAILED;
    if (mGnssHalAidl != nullptr) {
        mAidlIGnssCb = SharedRefBase::make<GnssMultiClientAidlIGnssCb>();
        auto status = mGnssHalAidl->setCallback(mAidlIGnssCb);

        if (!status.isOk()) {
            PRINTERROR("mGnssHalAidl->setCallback() failed.");
        } else {
            gardenResult = GARDEN_RESULT_PASSED;
        }
    } else {
        PRINTERROR("AIDL client not created.");
    }
    return gardenResult;
}
GARDEN_RESULT GnssMultiClientAidlIGnss::IGnss_start() {
    GARDEN_RESULT gardenResult = GARDEN_RESULT_FAILED;
    if (mGnssHalAidl != nullptr) {
        auto result = mGnssHalAidl->start();
        if (result.isOk()) {
            PRINTLN("mGnssHalAidl->start success.");
            gardenResult = GARDEN_RESULT_PASSED;
        } else {
            PRINTERROR("mGnssHalAidl->start failed.");
        }
    } else {
        PRINTERROR("AIDL client not created.");
    }
    return gardenResult;
}

GARDEN_RESULT GnssMultiClientAidlIGnss::IGnss_stop() {
    GARDEN_RESULT gardenResult = GARDEN_RESULT_FAILED;
    if (mGnssHalAidl != nullptr) {
        auto result = mGnssHalAidl->stop();
        if (result.isOk()) {
            PRINTLN("mGnssHalAidl->stop success.");
            gardenResult = GARDEN_RESULT_PASSED;

        } else {
            PRINTERROR("mGnssHalAidl->stop failed.");
        }
    } else {
        PRINTERROR("AIDL client not created.");
    }
    return gardenResult;
}

GARDEN_RESULT GnssMultiClientAidlIGnss::IGnss_setPositionMode() {
    GARDEN_RESULT gardenResult = GARDEN_RESULT_FAILED;
    if (mGnssHalAidl != nullptr) {
        IGnss::PositionModeOptions option;
        PRINTLN("Please specify position mode options:");
        option.mode = (IGnss::GnssPositionMode)getUserInputInt(
                "Position Mode ( Standalone 0 / MSB 1 / MSA 2 ) : ");
        option.recurrence = (IGnss::GnssPositionRecurrence)getUserInputInt(
                "Recurrence ( Periodic 0 / Single 1 ) : ");
        option.minIntervalMs = getUserInputInt("Min Interval (millis) : ");
        option.preferredAccuracyMeters = getUserInputInt("Preferred Accuracy (metres) : ");
        option.preferredTimeMs = getUserInputInt("Preferred Time (for first fix) (millis) : ");
        option.lowPowerMode = getUserInputYesNo("Use Low Power Mode (y/n) : ");
        auto result = mGnssHalAidl->setPositionMode(option);
        if (result.isOk()) {
            PRINTLN("mGnssHalAidl->setPositionMode success.");
            gardenResult = GARDEN_RESULT_PASSED;
        } else {
            PRINTERROR("mGnssHalAidl->setPositionMode failed.");
        }
    } else {
        PRINTERROR("AIDL client not created.");
    }
    return gardenResult;
}
GARDEN_RESULT GnssMultiClientAidlIGnss::IGnss_close() {
    GARDEN_RESULT gardenResult = GARDEN_RESULT_FAILED;
    if (mGnssHalAidl != nullptr) {
        auto result = mGnssHalAidl->close();
        if (result.isOk()) {
            gardenResult = GARDEN_RESULT_PASSED;
        } else {
            PRINTERROR("mGnssHalAidl->close failed.");
        }
    } else {
        PRINTERROR("AIDL client not created.");
    }
    return gardenResult;
}
GARDEN_RESULT GnssMultiClientAidlIGnss::IGnss_startSvStatus() {
    GARDEN_RESULT gardenResult = GARDEN_RESULT_FAILED;
    if (mGnssHalAidl != nullptr) {
        auto result = mGnssHalAidl->startSvStatus();
        if (result.isOk()) {
            gardenResult = GARDEN_RESULT_PASSED;
        } else {
            PRINTERROR("mGnssHalAidl->startSvStatus failed.");
        }
    } else {
        PRINTERROR("AIDL client not created.");
    }
    return gardenResult;
}
GARDEN_RESULT GnssMultiClientAidlIGnss::IGnss_stopSvStatus() {
    GARDEN_RESULT gardenResult = GARDEN_RESULT_FAILED;
    if (mGnssHalAidl != nullptr) {
        auto result = mGnssHalAidl->stopSvStatus();
        if (result.isOk()) {
            gardenResult = GARDEN_RESULT_PASSED;
        } else {
            PRINTERROR("mGnssHalAidl->stopSvStatus failed.");
        }
    } else {
        PRINTERROR("AIDL client not created.");
    }
    return gardenResult;
}
GARDEN_RESULT GnssMultiClientAidlIGnss::IGnss_startNmea() {
    GARDEN_RESULT gardenResult = GARDEN_RESULT_FAILED;
    if (mGnssHalAidl != nullptr) {
        auto result = mGnssHalAidl->startNmea();
        if (result.isOk()) {
            gardenResult = GARDEN_RESULT_PASSED;
        } else {
            PRINTERROR("mGnssHalAidl->startNmea failed.");
        }
    } else {
        PRINTERROR("AIDL client not created.");
    }
    return gardenResult;
}
GARDEN_RESULT GnssMultiClientAidlIGnss::IGnss_stopNmea() {
    GARDEN_RESULT gardenResult = GARDEN_RESULT_FAILED;
    if (mGnssHalAidl != nullptr) {
        auto result = mGnssHalAidl->stopNmea();
        if (result.isOk()) {
            gardenResult = GARDEN_RESULT_PASSED;
        } else {
            PRINTERROR("mGnssHalAidl->stopNmea failed.");
        }
    } else {
        PRINTERROR("AIDL client not created.");
    }
    return gardenResult;
}
GARDEN_RESULT GnssMultiClientAidlIGnss::IGnss_injectBestLocation() {
    GARDEN_RESULT gardenResult = GARDEN_RESULT_FAILED;
    GnssLocation location;
    PRINTLN("Please specify values to inject:");
    location.gnssLocationFlags = static_cast<uint16_t>(0x11);
    location.latitudeDegrees = getUserInputDouble("Latitude (degrees): ");
    location.longitudeDegrees = getUserInputDouble("Longitude (degrees): ");
    location.horizontalAccuracyMeters = static_cast<float>(
            getUserInputInt("Horizontal Accuracy (metres): "));
    if (mGnssHalAidl != nullptr) {
        auto result = mGnssHalAidl->injectBestLocation(location);
        if (result.isOk()) {
            PRINTLN("mGnssHalAidl->injectBestLocation success.");
            gardenResult = GARDEN_RESULT_PASSED;
        }
    } else {
        PRINTERROR("AIDL client not created.");
    }
    return gardenResult;
}
GARDEN_RESULT GnssMultiClientAidlIGnss::IGnssConfiguration_setSuplVersion() {
    GARDEN_RESULT gardenResult = GARDEN_RESULT_FAILED;
    int version = 0;
    if (mGnssConfigIface != nullptr) {
        PRINTLN("1. GNSS_CONFIG_SUPL_VERSION_0\n"
                "2. GNSS_CONFIG_SUPL_VERSION_0\n"
                "3. GNSS_CONFIG_SUPL_VERSION_2\n"
                "4. GNSS_CONFIG_SUPL_VERSION_4");
        int choice = getUserInputInt("Enter choice: ");
        switch (choice) {
            case 1:
                version = 0x00010000;
                break;
            case 2:
                version = 0x00020000;
                break;
            case 3:
                version = 0x00020002;
                break;
            case 4:
                version = 0x00020004;
                break;
        }
        auto result = mGnssConfigIface->setSuplVersion(version);
        if (result.isOk()) {
            gardenResult = GARDEN_RESULT_PASSED;
        } else {
            PRINTERROR("mGnssConfigIface->setSuplVersion failed.");
        }
    } else {
        PRINTERROR("mGnssConfigIface null");
    }
    return gardenResult;
}
GARDEN_RESULT GnssMultiClientAidlIGnss::IGnssConfiguration_setSuplMode() {
    GARDEN_RESULT gardenResult = GARDEN_RESULT_FAILED;
    if (mGnssConfigIface != nullptr) {
        PRINTLN("0. GNSS_CONFIG_SUPL_MODE_STANDALONE_ONLY\n"
                "1. GNSS_CONFIG_SUPL_MODE_MSB\n"
                "2. GNSS_CONFIG_SUPL_MODE_MSA\n"
                "3. GNSS_CONFIG_SUPL_MODE_MSB_MSA");
        int mode = getUserInputInt("Enter choice: ");
        auto result = mGnssConfigIface->setSuplMode(mode);
        if (result.isOk()) {
            gardenResult = GARDEN_RESULT_PASSED;
        } else {
            PRINTERROR("mGnssConfigIface->setSuplMode failed.");
        }
    } else {
        PRINTERROR("mGnssConfigIface null");
    }
    return gardenResult;
}
GARDEN_RESULT GnssMultiClientAidlIGnss::IGnssConfiguration_setLppProfile() {
    GARDEN_RESULT gardenResult = GARDEN_RESULT_FAILED;
    int lppProfileMask = GNSS_CONFIG_LPP_PROFILE_RRLP_ON_LTE;
    if (mGnssConfigIface != nullptr) {
        PRINTLN("1. GNSS_CONFIG_LPP_PROFILE_USER_PLANE_BIT\n"
                "2. GNSS_CONFIG_LPP_PROFILE_CONTROL_PLANE_BIT\n"
                "4. GNSS_CONFIG_LPP_PROFILE_USER_PLANE_OVER_NR5G_SA_BIT\n"
                "8. GNSS_CONFIG_LPP_PROFILE_CONTROL_PLANE_OVER_NR5G_SA_BIT");
        lppProfileMask = getUserInputInt("Enter bitmask of Lpp Profile: ");
        auto result = mGnssConfigIface->setLppProfile(lppProfileMask);
        if (result.isOk()) {
            gardenResult = GARDEN_RESULT_PASSED;
        } else {
            PRINTERROR("mGnssConfigIface->setLppProfile failed.");
        }
    } else {
        PRINTERROR("mGnssConfigIface null");
    }
    return gardenResult;
}
GARDEN_RESULT GnssMultiClientAidlIGnss::IGnssConfiguration_setGlonassPositioningProtocol() {
    GARDEN_RESULT gardenResult = GARDEN_RESULT_FAILED;
    int aGlonassPositionProtocolMask = GNSS_CONFIG_LPP_PROFILE_RRLP_ON_LTE;
    if (mGnssConfigIface != nullptr) {
        PRINTLN("1. GNSS_CONFIG_RRC_CONTROL_PLANE_BIT\n"
                "2. GNSS_CONFIG_RRLP_USER_PLANE_BIT\n"
                "4. GNSS_CONFIG_LLP_USER_PLANE_BIT\n"
                "8. GNSS_CONFIG_LLP_CONTROL_PLANE_BIT");
        aGlonassPositionProtocolMask = getUserInputInt(
                "Enter bitmask of Positioning Protocol on A-GLONASS system: ");
        auto result = mGnssConfigIface->setGlonassPositioningProtocol(aGlonassPositionProtocolMask);
        if (result.isOk()) {
            gardenResult = GARDEN_RESULT_PASSED;
        } else {
            PRINTERROR("mGnssConfigIface->setGlonassPositioningProtocol failed.");
        }
    } else {
        PRINTERROR("mGnssConfigIface null");
    }
    return gardenResult;
}
GARDEN_RESULT GnssMultiClientAidlIGnss::IGnssConfiguration_setEmergencySuplPdn() {
    GARDEN_RESULT gardenResult = GARDEN_RESULT_FAILED;
    if (mGnssConfigIface != nullptr) {
        bool enableEmergencyPdnForEmergencySupl = getUserInputYesNo(
                "Enable Emergency Supl Pdn (y/n): ");
        auto result = mGnssConfigIface->setEmergencySuplPdn(enableEmergencyPdnForEmergencySupl);
        if (result.isOk()) {
            gardenResult = GARDEN_RESULT_PASSED;
        } else {
            PRINTERROR("mGnssConfigIface->setEmergencySuplPdn failed.");
        }
    } else {
        PRINTERROR("mGnssConfigIface null");
    }
    return gardenResult;
}
GARDEN_RESULT GnssMultiClientAidlIGnss::IGnssConfiguration_setEsExtensionSec() {
    GARDEN_RESULT gardenResult = GARDEN_RESULT_FAILED;
    if (mGnssConfigIface != nullptr) {
        int emergencyExtensionSeconds = getUserInputInt("Enter Emergency Extension Seconds: ");
        auto result = mGnssConfigIface->setEsExtensionSec(emergencyExtensionSeconds);
        if (result.isOk()) {
            gardenResult = GARDEN_RESULT_PASSED;
        } else {
            PRINTERROR("mGnssConfigIface->setEsExtensionSec failed.");
        }
    } else {
        PRINTERROR("mGnssConfigIface null");
    }
    return gardenResult;
}
GARDEN_RESULT GnssMultiClientAidlIGnss::IGnssConfiguration_setBlocklist() {
    GARDEN_RESULT gardenResult = GARDEN_RESULT_FAILED;
    std::vector<BlocklistedSource> tempVector;
    // Populate the blacklist vector
    while (true) {
        if (!getUserInputYesNo("Add a blacklist source (y/n)? ")) {
            break;
        }
        PRINTLN("GnssConstellationType: GPS 1 / GLONASS 3 / "
                "QZSS 4 / BEIDOU 5 / GALILEO 6/ NAVIC 7");
        PRINTLN("SV ID: GPS (1-32) / GLONASS (1-32) / QZSS (1-5) / "
                "BEIDOU (1-37) / GALILEO (1-36)/ NAVIC (1-14)\n");
        BlocklistedSource source;
        source.constellation = (GnssConstellationType)
            getUserInputInt("Constellation Type: ");
        source.svid = (uint16_t)getUserInputInt("SV ID: ");
        tempVector.push_back(source);
    }
    PRINTLN("blacklist vector size %d", (int)tempVector.size());
    if (mGnssConfigIface != nullptr) {
        auto result = mGnssConfigIface->setBlocklist(tempVector);
        if (result.isOk()) {
            gardenResult = GARDEN_RESULT_PASSED;
        } else {
            PRINTERROR("mGnssConfigIface->setBlocklist failed.");
        }
    } else {
        PRINTERROR("mGnssConfigIface null");
    }
    return gardenResult;
}

GARDEN_RESULT GnssMultiClientAidlIGnss::IGnssDebug_getDebugData() {
    GARDEN_RESULT gardenResult = GARDEN_RESULT_FAILED;
    if (mGnssHalAidl != nullptr) {
        if (mGnssDebug != nullptr) {
            IGnssDebug::DebugData data;
            auto result = mGnssDebug->getDebugData(&data);
            if (!result.isOk()) {
                PRINTERROR("mGnssDebug->getDebugData() failed.");
            } else {
                PRINTLN("mGnssDebug->getDebugData() success.");
                // Postion debug report data
                PRINTLN("Postion debug Data--> Postionvalid: %d, latitude:  %lf, longitude:  %lf,"
                        "altitude: %f, speed: %f, bearing: %f, horizontalAccuracy: %lf,"
                        "verticalAccuracy: %lf, speedAccuracyMeters: %lf, bearingAccuracy: %lf,"
                        "ageS= %f \n", data.position.valid, data.position.latitudeDegrees,
                        data.position.longitudeDegrees, data.position.altitudeMeters,
                        data.position.speedMetersPerSec, data.position.bearingDegrees,
                        data.position.horizontalAccuracyMeters,
                        data.position.verticalAccuracyMeters,
                        data.position.speedAccuracyMetersPerSecond,
                        data.position.bearingAccuracyDegrees, data.position.ageSeconds);

                // time debug report data
                PRINTLN("Time debug data--> timeEstimate: %lld, timeUncertainty: %f,"
                        "frequencyUncertainty: %f\n", data.time.timeEstimateMs,
                        data.time.timeUncertaintyNs, data.time.frequencyUncertaintyNsPerSec);
                // satellite debug report data
                string constellations[8] = {"UNKNOWN", "GPS", "SBAS", "GLONASS",
                    "QZSS", "BEIDOU", "GALILEO", "NAVIC"};
                string ephType[3] = {"EPHEMERIS", "ALMANAC_ONLY", "NOT_AVAILABLE"};
                string ephSrc[4] = {"DEMODULATED", "SUPL_PROVIDED", "OTHER_SERVER_PROVIDED",
                    "OTHER"};
                string ephHealth[3] = {"GOOD", "BAD", "UNKNOWN"};
                PRINTLN("satellite debug data total : %d\n", data.satelliteDataArray.size());
                for (auto satelliteData : data.satelliteDataArray) {
                    PRINTLN("satelliteData--> Svid: %d, constellation: %s, ephemerisType: %s,"
                            "ephemerisSource: %s, ephemerisHealth: %s, ephemeris: %f, server"
                            "PredictionIsAvailable: %d, serverPredictionAge: %f",
                            satelliteData.svid,
                            constellations[(int)satelliteData.constellation].c_str(),
                            ephType[(int)satelliteData.ephemerisType].c_str(),
                            ephSrc[(int)satelliteData.ephemerisSource].c_str(),
                            ephHealth[(int)satelliteData.ephemerisHealth].c_str(),
                            satelliteData.ephemerisAgeSeconds,
                            satelliteData.serverPredictionIsAvailable,
                            satelliteData.serverPredictionAgeSeconds);
                }
                gardenResult = GARDEN_RESULT_PASSED;
            }
        } else {
            PRINTERROR("mGnssDebug null");
        }
    } else {
        PRINTERROR("AIDL client not created.");
    }
    return gardenResult;
}
GARDEN_RESULT GnssMultiClientAidlIGnss::IGnssVisibilityControl_setCallback() {
    GARDEN_RESULT gardenResult = GARDEN_RESULT_FAILED;
    if (mGnssHalAidl != nullptr) {
        mGnssVisibCtrlCb = SharedRefBase::make<GnssMultiClientAidlIGnssVisibCtrlCb>();
        if (mGnssVisibilityControl != nullptr) {
            auto result = mGnssVisibilityControl->setCallback(mGnssVisibCtrlCb);
            if (result.isOk()) {
                PRINTLN("mGnssVisibilityControl->setCallback success.");
                gardenResult = GARDEN_RESULT_PASSED;
            } else {
                PRINTERROR("mGnssVisibilityControl->setCallback failed.");
            }
        } else {
            PRINTERROR("mGnssVisibilityControl null");
        }
    } else {
        PRINTERROR("AIDL client not created.");
    }
    return gardenResult;
}

GARDEN_RESULT GnssMultiClientAidlIGnss::IGnssVisibilityControl_controlNfwLocationAccess() {
    GARDEN_RESULT gardenResult = GARDEN_RESULT_FAILED;
    std::vector<string> hVec;
    string hString;
    // get input from the user here
    std::string packages = getUserInputString("Input Packages (e.g. package1 package2 package9):");
    if (mGnssHalAidl != nullptr) {
        if (mGnssVisibilityControl != nullptr) {
            char* tmp = NULL;
            char* substr;
            char* str;
            str = &packages[0];
            hVec.resize(0);
            int i = 0;
            for (substr = strtok_r(str, " ", &tmp);
                substr != NULL;
                substr = strtok_r(NULL, " ", &tmp)) {
                hVec.resize(++i);
                hVec[i-1] = substr;
            }
            auto result = mGnssVisibilityControl->enableNfwLocationAccess(hVec);
            if (result.isOk()) {
                PRINTLN("mGnssVisibilityControl->enableNfwLocationAccess success.");
                gardenResult = GARDEN_RESULT_PASSED;
            } else {
                PRINTERROR("mGnssVisibilityControl->enableNfwLocationAccess failed.");
            }
        } else {
            PRINTERROR("mGnssVisibilityControl null");
        }
    } else {
        PRINTERROR("AIDL client not created.");
    }
    return gardenResult;
}
GARDEN_RESULT GnssMultiClientAidlIGnss::IGnssBatching_init() {
    GARDEN_RESULT gardenResult = GARDEN_RESULT_FAILED;
        mGnssBatchingCb = SharedRefBase::make<GnssMultiClientAidlIGnssBatchingCb>();
    if (mGnssHalAidl != nullptr) {
        if (mGnssBatching != nullptr) {
            auto result = mGnssBatching->init(mGnssBatchingCb);
            if (!result.isOk()) {
                PRINTERROR("mGnssBatching->init() failed.");
            } else {
                PRINTLN("mGnssBatching->init() success.");
                gardenResult = GARDEN_RESULT_PASSED;
            }
        } else {
            PRINTERROR("mGnssBatching null");
        }
    } else {
        PRINTERROR("AIDL client not created.");
    }
    return gardenResult;
}

GARDEN_RESULT GnssMultiClientAidlIGnss::IAGnss_setCallback() {
    GARDEN_RESULT gardenResult = GARDEN_RESULT_FAILED;
        mAGnssCb = SharedRefBase::make<GnssMultiClientAidlIAGnssCb>();
    if (mGnssHalAidl != nullptr) {
        if (mAGnss != nullptr) {
            auto result = mAGnss->setCallback(mAGnssCb);
            if (!result.isOk()) {
                PRINTERROR("mAGnss->setCallback failed.");
            } else {
                PRINTLN("mAGnss->setCallback success.");
                gardenResult = GARDEN_RESULT_PASSED;
            }
        } else {
            PRINTERROR("mAGnss null");
        }
    } else {
        PRINTERROR("AIDL client not created.");
    }
    return gardenResult;
}

GARDEN_RESULT GnssMultiClientAidlIGnss::IAGnss_dataConnOpen() {
    GARDEN_RESULT gardenResult = GARDEN_RESULT_FAILED;
    if (mGnssHalAidl != nullptr) {
        if (mAGnss != nullptr) {
            uint64_t networkHandle = getUserInputMask64Bit(
                    "Input handle represents the network for use with the NDK API");
            std::string apn = getUserInputString("Input AGnss APN:");
            int apnType = getUserInputInt("Input AGnss APN IP Type:");
            auto result = mAGnss->dataConnOpen(networkHandle, apn,
                    (IAGnss::ApnIpType)apnType);
            if (result.isOk()) {
                PRINTLN("mAGnss->dataConnOpen() success.");
                gardenResult = GARDEN_RESULT_PASSED;
            } else {
                PRINTERROR("mAGnss->dataConnOpen() failed.");
            }
        } else {
            PRINTERROR("mAGnss null");
        }
    } else {
        PRINTERROR("AIDL client not created.");
    }
    return gardenResult;
}
GARDEN_RESULT GnssMultiClientAidlIGnss::IAGnss_setServer() {
    GARDEN_RESULT gardenResult = GARDEN_RESULT_FAILED;
    if (mGnssHalAidl != nullptr) {
        if (mAGnss != nullptr) {
            int agnssType = getUserInputInt("Input AGnss type Specifies if SUPL or C2K");
            std::string host = getUserInputString("Input Hostname of the AGNSS server");
            int port = getUserInputInt("Input Port number associated with the server");
            auto result = mAGnss->setServer((IAGnssCallback::AGnssType)agnssType, host,
                    port);
            if (result.isOk()) {
                PRINTLN("mAGnss->setServer() success.");
                gardenResult = GARDEN_RESULT_PASSED;
            } else {
                PRINTERROR("mAGnss->setServer() failed.");
            }
        } else {
            PRINTERROR("mAGnss null");
        }
    } else {
        PRINTERROR("AIDL client not created.");
    }
    return gardenResult;
}

GARDEN_RESULT GnssMultiClientAidlIGnss::IAGnss_dataConnClosed() {
    GARDEN_RESULT gardenResult = GARDEN_RESULT_FAILED;
    if (mGnssHalAidl != nullptr) {
        if (mAGnss != nullptr) {
            auto result = mAGnss->dataConnClosed();
            if (result.isOk()) {
                PRINTLN("mAGnss->dataConnClosed() success.");
                gardenResult = GARDEN_RESULT_PASSED;
            } else {
                PRINTERROR("mAGnss->dataConnClosed() failed.");
            }
        } else {
            PRINTERROR("mAGnss null");
        }
    } else {
        PRINTERROR("AIDL client not created.");
    }
    return gardenResult;
}
GARDEN_RESULT GnssMultiClientAidlIGnss::IGnss_setAntennaInfoCallback() {
    GARDEN_RESULT gardenResult = GARDEN_RESULT_FAILED;
        mGnssAntennaInfoCb = SharedRefBase::make<GnssMultiClientAidlIGnssAntennaInfoCb>();
    if (mGnssAntennaInfo != nullptr) {
            auto result = mGnssAntennaInfo->setCallback(mGnssAntennaInfoCb);
            if (result.isOk()) {
                PRINTLN("mGnssAntennaInfo->setCallback success.");
                gardenResult = GARDEN_RESULT_PASSED;
            } else {
                PRINTERROR("mGnssAntennaInfo->setCallback failed.");
            }
        } else {
            PRINTERROR("mGnssAntennaInfo null");
        }
    return gardenResult;
}
GARDEN_RESULT GnssMultiClientAidlIGnss::IGnss_setAntennaInfoClose() {
    GARDEN_RESULT gardenResult = GARDEN_RESULT_FAILED;
    if (mGnssAntennaInfo != nullptr) {
            auto result = mGnssAntennaInfo->close();
            if (result.isOk()) {
                PRINTLN("mGnssAntennaInfo->close success.");
                gardenResult = GARDEN_RESULT_PASSED;
            } else {
                PRINTERROR("mGnssAntennaInfo->close failed.");
            }
        } else {
            PRINTERROR("mGnssAntennaInfo null");
        }
    return gardenResult;
}
GARDEN_RESULT GnssMultiClientAidlIGnss::IGnss_setMeasurementCorrectionsCallback() {
    GARDEN_RESULT gardenResult = GARDEN_RESULT_FAILED;
        mGnssMeasCorrCb = SharedRefBase::make<GnssMultiClientAidlIGnssMeasCorrCb>();
        if (mMeasurementCorrections != nullptr) {
            auto result = mMeasurementCorrections->setCallback(mGnssMeasCorrCb);
            if (result.isOk()) {
                PRINTLN("mMeasurementCorrections->setCallback success.");
                gardenResult = GARDEN_RESULT_PASSED;
            } else {
                PRINTERROR("mMeasurementCorrections->setCallback failed.");
            }
        } else {
            PRINTERROR("mMeasurementCorrections null");
        }
    return gardenResult;
}
} // namespace garden
