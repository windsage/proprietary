/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

GENERAL DESCRIPTION
  DataItemSerializers Implementation

  Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
  All rights reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/

#define LOG_TAG "DataItemSerializers"

#include <inttypes.h>
#include <string>
#include <string.h>
#include <comdef.h>
#include <DataItemId.h>
#include <IDataItemCore.h>
#include <DataItemDefines.h>
#include <DataItemSerializers.h>
#include "DataItemConcreteTypesExt.h"
#include <postcard.h>
#include <log_util.h>
#include <loc_pla.h>

using namespace std;
using namespace izat_manager;
using namespace qc_loc_fw;
using loc_core::IDataItemCore;


// macro for serializer
#define BREAK_AND_DELETE_IF_NON_ZERO(ERR, X, OBJ) \
if (0!=(X)) {result = (ERR); delete (OBJ); break;}

#define ADD_INNER_CARD() \
    if (ic != NULL) { \
        BREAK_AND_DELETE_IF_NON_ZERO(100, ic->finalize(), ic); \
        BREAK_AND_DELETE_IF_NON_ZERO(101, oc->addCard(getCardName().c_str(), ic), ic); \
        delete ic; ic = NULL; \
    } \

#define CREATE_INNER_CARD(T, ID) \
    if (oc==NULL) { result = 1; break; } \
    if (mDataItem.getId() != ID) { result = 2; break; } \
    if (hasCardName()) { \
        ic = OutPostcard::createInstance(); \
        BREAK_IF_ZERO(3, ic); \
        BREAK_AND_DELETE_IF_NON_ZERO(4, ic->init(), ic); \
        card = ic; \
    } \
    T * d = static_cast<T *>(&mDataItem);

// macro for deserializer
/*
    This macro to be used when we start filling the methods
    defined using DATA_ITEM_DESERIALIZE macro
#define CHECK_AND_ADD_NEW_CARD() \
    if (ic == NULL) { result = 1; break; } \
    if (hasCardName() ) { \
        card = OutPostcard::createInstance(); \
        if (card == NULL) { result = 2; break; } \
        BREAK_IF_NON_ZERO(3, oc->addCard(getCardName().c_str(), card)); \
    }
*/


// serialize
int32_t AirplaneModeSerializer::serialize (OutPostcard * oc) {
    int32 result = 0;
    OutPostcard * card = oc;
    OutPostcard * ic = NULL;
    ENTRY_LOG();
    do {
        CREATE_INNER_CARD(AirplaneModeDataItem, AIRPLANEMODE_DATA_ITEM_ID);
        BREAK_AND_DELETE_IF_NON_ZERO(
                5, card->addBool(AIRPLANEMODE_FIELD_MODE, d->mMode), ic);
        ADD_INNER_CARD();
    } while (0);
    EXIT_LOG_WITH_ERROR("%d", result);
    return result;
}
int32_t ENHSerializer::serialize (OutPostcard * oc) {
    int32 result = 0;
    OutPostcard * card = oc;
    OutPostcard * ic = NULL;
    ENTRY_LOG();
    do {
        CREATE_INNER_CARD(ENHDataItem, ENH_DATA_ITEM_ID);
        BREAK_AND_DELETE_IF_NON_ZERO(
                5, card->addBool(ENH_FIELD_ENABLED, d->isEnabled()), ic);
        ADD_INNER_CARD();
    } while (0);
    EXIT_LOG_WITH_ERROR("%d", result);
    return result;
}
int32_t GPSStateSerializer::serialize (OutPostcard * oc) {
    int32 result = 0;
    OutPostcard * card = oc;
    OutPostcard * ic = NULL;
    ENTRY_LOG();
    do {
        CREATE_INNER_CARD(GPSStateDataItem, GPSSTATE_DATA_ITEM_ID);
        BREAK_AND_DELETE_IF_NON_ZERO(
                5, card->addBool(GPSSTATE_FIELD_ENABLED, d->mEnabled), ic);
        ADD_INNER_CARD();
    } while (0);
    EXIT_LOG_WITH_ERROR("%d", result);
    return result;
}
int32_t NLPStatusSerializer::serialize (OutPostcard * oc) {
    int32 result = 0;
    OutPostcard * card = oc;
    OutPostcard * ic = NULL;
    ENTRY_LOG();
    do {
        CREATE_INNER_CARD(NLPStatusDataItem, NLPSTATUS_DATA_ITEM_ID);
        BREAK_AND_DELETE_IF_NON_ZERO(
                5, card->addBool(NLPSTATUS_FIELD_ENABLED, d->mEnabled), ic);
        ADD_INNER_CARD();
    } while (0);
    EXIT_LOG_WITH_ERROR("%d", result);
    return result;
}
int32_t WifiHardwareStateSerializer::serialize (OutPostcard * oc) {
    int32 result = 0;
    OutPostcard * card = oc;
    OutPostcard * ic = NULL;
    ENTRY_LOG();
    do {
        CREATE_INNER_CARD(WifiHardwareStateDataItem, WIFIHARDWARESTATE_DATA_ITEM_ID);
        BREAK_AND_DELETE_IF_NON_ZERO(
                5, card->addBool(WIFIHARDWARESTATE_FIELD_ENABLED, d->mEnabled), ic);
        ADD_INNER_CARD();
    } while (0);
    EXIT_LOG_WITH_ERROR("%d", result);
    return result;
}

int32_t ScreenStateSerializer::serialize (OutPostcard * oc) {
    int32 result = 0;
    OutPostcard * card = oc;
    OutPostcard * ic = NULL;
    ENTRY_LOG();
    do {
        CREATE_INNER_CARD(ScreenStateDataItem, SCREEN_STATE_DATA_ITEM_ID);
        BREAK_AND_DELETE_IF_NON_ZERO(
                5, card->addBool(SCREENSTATE_FIELD_ENABLED, d->mState), ic);
        ADD_INNER_CARD();
    } while (0);
    EXIT_LOG_WITH_ERROR("%d", result);
    return result;
}

int32_t PowerConnectStateSerializer::serialize (OutPostcard * oc) {
    int32 result = 0;
    OutPostcard * card = oc;
    OutPostcard * ic = NULL;
    ENTRY_LOG();
    do {
        CREATE_INNER_CARD(PowerConnectStateDataItem, POWER_CONNECTED_STATE_DATA_ITEM_ID);
        BREAK_AND_DELETE_IF_NON_ZERO(
                5, card->addBool(POWERCONNECTSTATE_FIELD_ENABLED, d->mState), ic);
        ADD_INNER_CARD();
    } while (0);
    EXIT_LOG_WITH_ERROR("%d", result);
    return result;
}

int32_t BatteryLevelSerializer::serialize (OutPostcard * oc) {
    int32 result = 0;
    OutPostcard * card = oc;
    OutPostcard * ic = NULL;
    ENTRY_LOG();
    do {
        CREATE_INNER_CARD(BatteryLevelDataItem, BATTERY_LEVEL_DATA_ITEM_ID);
        BREAK_AND_DELETE_IF_NON_ZERO(
                5, card->addUInt8(BATTERYLEVEL_FIELD_BATTERY_PCT, d->mBatteryPct), ic);
        ADD_INNER_CARD();
    } while (0);
    EXIT_LOG_WITH_ERROR("%d", result);
    return result;
}

int32_t TimeZoneChangeSerializer::serialize (OutPostcard * oc) {
    int32 result = 0;
    OutPostcard * card = oc;
    OutPostcard * ic = NULL;
    ENTRY_LOG();
    do {
        CREATE_INNER_CARD(TimeZoneChangeDataItem, TIMEZONE_CHANGE_DATA_ITEM_ID);
        BREAK_AND_DELETE_IF_NON_ZERO(
                5, card->addBool(TIMEZONECHANGE_FIELD_ENABLED, true), ic);
        BREAK_AND_DELETE_IF_NON_ZERO(
                7, card->addInt64(TIMECHANGE_FIELD_CURRENT_TIME_MILLIS, d->mCurrTimeMillis), ic);
        BREAK_AND_DELETE_IF_NON_ZERO(
                8, card->addInt64(TIMECHANGE_FIELD_RAW_OFFSET_TZ, d->mRawOffsetTZ), ic);
        BREAK_AND_DELETE_IF_NON_ZERO(
                9, card->addInt64(TIMECHANGE_FIELD_DST_OFFSET_TZ, d->mDstOffsetTZ), ic);
        ADD_INNER_CARD();
    } while (0);
    EXIT_LOG_WITH_ERROR("%d", result);
    return result;
}

int32_t TimeChangeSerializer::serialize (OutPostcard * oc) {
    int32 result = 0;
    OutPostcard * card = oc;
    OutPostcard * ic = NULL;
    ENTRY_LOG();
    do {
        CREATE_INNER_CARD(TimeChangeDataItem, TIME_CHANGE_DATA_ITEM_ID);
        BREAK_AND_DELETE_IF_NON_ZERO(
                5, card->addBool(TIMECHANGE_FIELD_ENABLED, true), ic);
        BREAK_AND_DELETE_IF_NON_ZERO(
                7, card->addInt64(TIMECHANGE_FIELD_CURRENT_TIME_MILLIS, d->mCurrTimeMillis), ic);
        BREAK_AND_DELETE_IF_NON_ZERO(
                8, card->addInt64(TIMECHANGE_FIELD_RAW_OFFSET_TZ, d->mRawOffsetTZ), ic);
        BREAK_AND_DELETE_IF_NON_ZERO(
                9, card->addInt64(TIMECHANGE_FIELD_DST_OFFSET_TZ, d->mDstOffsetTZ), ic);
        ADD_INNER_CARD();
    } while (0);
    EXIT_LOG_WITH_ERROR("%d", result);
    return result;
}

int32_t ShutdownStateSerializer::serialize (OutPostcard * oc) {
    int32 result = 0;
    OutPostcard * card = oc;
    OutPostcard * ic = NULL;
    ENTRY_LOG();
    do {
        CREATE_INNER_CARD(ShutdownStateDataItem, SHUTDOWN_STATE_DATA_ITEM_ID);
        BREAK_AND_DELETE_IF_NON_ZERO(
                5, card->addBool(SHUTDOWN_FIELD_ENABLED, d->mState), ic);
        ADD_INNER_CARD();
    } while (0);
    EXIT_LOG_WITH_ERROR("%d", result);
    return result;
}

int32_t AssistedGpsSerializer::serialize (OutPostcard * oc) {
    int32 result = 0;
    OutPostcard * card = oc;
    OutPostcard * ic = NULL;
    ENTRY_LOG();
    do {
        CREATE_INNER_CARD(AssistedGpsDataItem, ASSISTED_GPS_DATA_ITEM_ID);
        BREAK_AND_DELETE_IF_NON_ZERO(
                5, card->addBool(ASSISTEDGPS_FIELD_ENABLED, d->mEnabled), ic);
        ADD_INNER_CARD();
    } while (0);
    EXIT_LOG_WITH_ERROR("%d", result);
    return result;
}

int32_t NetworkInfoSerializer::serialize (OutPostcard * oc) {
    int32 result = 0;
    OutPostcard * card = oc;
    OutPostcard * ic = NULL;
    ENTRY_LOG();
    do {
        uint32_t type = (mDataItem.mAllTypes & (1<<TYPE_WIFI)) ? 100 : 201;
        CREATE_INNER_CARD(NetworkInfoDataItem, NETWORKINFO_DATA_ITEM_ID);
        BREAK_AND_DELETE_IF_NON_ZERO(
                6, oc->addBool(NETWORKINFO_FIELD_CONNECTED, d->mConnected), ic);
        BREAK_AND_DELETE_IF_NON_ZERO(
                7, card->addInt32(NETWORKINFO_FIELD_TYPE, type), ic);
        BREAK_AND_DELETE_IF_NON_ZERO(
                8, card->addString(NETWORKINFO_FIELD_TYPENAME, d->mTypeName.c_str()), ic);
        BREAK_AND_DELETE_IF_NON_ZERO(
                9, card->addString(NETWORKINFO_FIELD_SUBTYPENAME, d->mSubTypeName.c_str()), ic);
        BREAK_AND_DELETE_IF_NON_ZERO(
                10, card->addBool(NETWORKINFO_FIELD_AVAILABLE, d->mAvailable), ic);
        BREAK_AND_DELETE_IF_NON_ZERO(
                11, card->addBool(NETWORKINFO_FIELD_CONNECTED, d->mConnected), ic);
        BREAK_AND_DELETE_IF_NON_ZERO(12, card->addBool(NETWORKINFO_FIELD_ROAMING, d->mRoaming), ic);

        for (uint8_t handle_index = 0; handle_index < MAX_NETWORK_HANDLES; handle_index++) {
            std::string fieldName = "NETWORK_HANDLE_" + std::to_string(handle_index);
            BREAK_AND_DELETE_IF_NON_ZERO(13,
                    card->addUInt64(fieldName.c_str(),
                                    d->mAllNetworkHandles[handle_index].networkHandle), ic);
            fieldName = "NETWORK_TYPE_" + std::to_string(handle_index);
            BREAK_AND_DELETE_IF_NON_ZERO(14,
                    card->addUInt8(fieldName.c_str(),
                                   d->mAllNetworkHandles[handle_index].networkType), ic);
        }
        if (0 != result) break;
        BREAK_AND_DELETE_IF_NON_ZERO(15,
                card->addString(NETWORKINFO_FIELD_APN_NAME, d->mApn.c_str()), ic);

         ADD_INNER_CARD();
    } while (0);
    EXIT_LOG_WITH_ERROR("%d", result);
    return result;
}
int32_t ServiceStatusSerializer::serialize (OutPostcard * oc) {
    int32 result = 0;
    OutPostcard * card = oc;
    OutPostcard * ic = NULL;
    ENTRY_LOG();
    do {
        CREATE_INNER_CARD(ServiceStatusDataItem, SERVICESTATUS_DATA_ITEM_ID);
        if (d->mServiceState == LOC_NW_ROAMING) {
            BREAK_AND_DELETE_IF_NON_ZERO(
                    5, card->addString(SERVICESTATUS_FIELD_STATE, "ROAMING"), ic);
        } else if (d->mServiceState == LOC_NW_HOME) {
            BREAK_AND_DELETE_IF_NON_ZERO(
                    6, card->addString(SERVICESTATUS_FIELD_STATE, "HOME"), ic);
        } else {
            BREAK_AND_DELETE_IF_NON_ZERO(
                    7, card->addString(SERVICESTATUS_FIELD_STATE, "OOO"), ic);
        }
        ADD_INNER_CARD();
    } while (0);
    EXIT_LOG_WITH_ERROR("%d", result);
    return result;
}
int32_t ModelSerializer::serialize (OutPostcard * oc) {
    int32 result = 0;
    OutPostcard * card = oc;
    OutPostcard * ic = NULL;
    ENTRY_LOG();
    do {
        CREATE_INNER_CARD(ModelDataItem, MODEL_DATA_ITEM_ID);
        BREAK_AND_DELETE_IF_NON_ZERO(
                5, card->addString(MODEL_FIELD_NAME, d->mModel.c_str()), ic);
        ADD_INNER_CARD();
    } while (0);
    EXIT_LOG_WITH_ERROR("%d", result);
    return result;
}
int32_t ManufacturerSerializer::serialize (OutPostcard * oc) {
    int32 result = 0;
    OutPostcard * card = oc;
    OutPostcard * ic = NULL;
    ENTRY_LOG();
    do {
        CREATE_INNER_CARD(ManufacturerDataItem, MANUFACTURER_DATA_ITEM_ID);
        BREAK_AND_DELETE_IF_NON_ZERO(
                5, card->addString(MANUFACTURER_FIELD_NAME, d->mManufacturer.c_str()), ic);
        ADD_INNER_CARD();
    } while (0);
    EXIT_LOG_WITH_ERROR("%d", result);
    return result;
}
int32_t InEmergencyCallSerializer::serialize (OutPostcard * oc) {
    int32 result = 0;
    OutPostcard * card = oc;
    OutPostcard * ic = NULL;
    ENTRY_LOG();
    do {
        CREATE_INNER_CARD(InEmergencyCallDataItem, IN_EMERGENCY_CALL_DATA_ITEM_ID);
        BREAK_IF_NON_ZERO(5, card->addBool(IN_EMERGENCY_CALL_FIELD_NAME, d->mIsEmergency));
        ADD_INNER_CARD();
    } while (0);
    EXIT_LOG_WITH_ERROR("%d", result);
    return result;
}
int32_t RilServiceInfoSerializer::serialize (OutPostcard * oc) {
    int32 result = 0;
    OutPostcard * card = oc;
    OutPostcard * ic = NULL;
    ENTRY_LOG();
    do {
        CREATE_INNER_CARD(RilServiceInfoDataItemExt, RILSERVICEINFO_DATA_ITEM_ID);
        // check if the data we received is valid
        if (d->mServiceInfo->valid_mask) {
            if ((d->mServiceInfo->valid_mask & LOC_RIL_SERVICE_INFO_HAS_AIR_IF_TYPE) ==
                    LOC_RIL_SERVICE_INFO_HAS_AIR_IF_TYPE) {
                BREAK_AND_DELETE_IF_NON_ZERO(6, card->addUInt32(
                        RILSERVICEINFO_FIELD_ARIF_TYPE_MASK, d->mServiceInfo->airIf_type), ic);
            }
            if ((d->mServiceInfo->valid_mask & LOC_RIL_SERVICE_INFO_HAS_CARRIER_AIR_IF_TYPE) ==
                    LOC_RIL_SERVICE_INFO_HAS_CARRIER_AIR_IF_TYPE) {
                if (d->mServiceInfo->carrierAirIf_type == LOC_RILAIRIF_GSM) {
                    BREAK_AND_DELETE_IF_NON_ZERO(7,
                            card->addString(RILSERVICEINFO_FIELD_CARRIER_ARIF_TYPE, "GSM"), ic);
                } else if (d->mServiceInfo->carrierAirIf_type == LOC_RILAIRIF_CDMA) {
                    BREAK_AND_DELETE_IF_NON_ZERO(8,
                            card->addString(RILSERVICEINFO_FIELD_CARRIER_ARIF_TYPE, "CDMA"), ic);
                } else if (d->mServiceInfo->carrierAirIf_type == LOC_RILAIRIF_WCDMA) {
                    BREAK_AND_DELETE_IF_NON_ZERO(9,
                            card->addString(RILSERVICEINFO_FIELD_CARRIER_ARIF_TYPE, "WCDMA"), ic);
                } else if (d->mServiceInfo->carrierAirIf_type == LOC_RILAIRIF_LTE) {
                    BREAK_AND_DELETE_IF_NON_ZERO(10,
                            card->addString(RILSERVICEINFO_FIELD_CARRIER_ARIF_TYPE, "LTE"), ic);
                } else if (d->mServiceInfo->carrierAirIf_type == LOC_RILAIRIF_EVDO) {
                    BREAK_AND_DELETE_IF_NON_ZERO(11,
                            card->addString(RILSERVICEINFO_FIELD_CARRIER_ARIF_TYPE, "EVDO"), ic);
                } else if (d->mServiceInfo->carrierAirIf_type == LOC_RILAIRIF_WIFI) {
                    BREAK_AND_DELETE_IF_NON_ZERO(12,
                            card->addString(RILSERVICEINFO_FIELD_CARRIER_ARIF_TYPE, "WIFI"), ic);
                }
            }
            if ((d->mServiceInfo->valid_mask & LOC_RIL_SERVICE_INFO_HAS_CARRIER_MCC) ==
                LOC_RIL_SERVICE_INFO_HAS_CARRIER_MCC) {
                BREAK_AND_DELETE_IF_NON_ZERO(13, card->addUInt16(
                        RILSERVICEINFO_FIELD_CARRIER_MCC, d->mServiceInfo->carrierMcc), ic);
            }
            if ((d->mServiceInfo->valid_mask & LOC_RIL_SERVICE_INFO_HAS_CARRIER_MNC) ==
                LOC_RIL_SERVICE_INFO_HAS_CARRIER_MNC) {
                BREAK_AND_DELETE_IF_NON_ZERO(14, card->addUInt16(
                        RILSERVICEINFO_FIELD_CARRIER_MNC, d->mServiceInfo->carrierMnc), ic);
            }
            if ((d->mServiceInfo->valid_mask & LOC_RIL_SERVICE_INFO_HAS_CARRIER_NAME) ==
                LOC_RIL_SERVICE_INFO_HAS_CARRIER_NAME) {
                BREAK_AND_DELETE_IF_NON_ZERO(15, card->addString(
                        RILSERVICEINFO_FIELD_CARRIER_NAME, d->mServiceInfo->carrierName), ic);
            }
        }
        ADD_INNER_CARD();
    } while (0);
    EXIT_LOG_WITH_ERROR("%d", result);
    return result;
}
int32_t RilCellInfoSerializer::serialize (OutPostcard * oc) {
    int32 result = 0;
    OutPostcard * card = oc;
    OutPostcard * ic = NULL;
    ENTRY_LOG();
    do {
        CREATE_INNER_CARD(RilCellInfoDataItemExt, RILCELLINFO_DATA_ITEM_ID);
        // check if the data we received is valid
        if (d->mCellInfo->valid_mask) {
            if ((d->mCellInfo->valid_mask & LOC_RIL_CELLINFO_HAS_NW_STATUS) ==
                    LOC_RIL_CELLINFO_HAS_NW_STATUS) {
                if (d->mCellInfo->nwStatus == LOC_NW_ROAMING) {
                    BREAK_AND_DELETE_IF_NON_ZERO(
                            6, card->addString(RILCELLINFO_FIELD_NETWORK_STATUS, "ROAMING"), ic);
                } else if (d->mCellInfo->nwStatus == LOC_NW_HOME) {
                    BREAK_AND_DELETE_IF_NON_ZERO(
                            7, card->addString(RILCELLINFO_FIELD_NETWORK_STATUS, "HOME"), ic);
                } else {
                    BREAK_AND_DELETE_IF_NON_ZERO(
                            8, card->addString(RILCELLINFO_FIELD_NETWORK_STATUS, "OOO"), ic);
                }
            }
            if ((d->mCellInfo->valid_mask & LOC_RIL_CELLINFO_HAS_CELL_INFO) ==
                        LOC_RIL_CELLINFO_HAS_CELL_INFO) {
                if ((d->mCellInfo->valid_mask & LOC_RIL_CELLINFO_HAS_TECH_TYPE) ==
                        LOC_RIL_CELLINFO_HAS_TECH_TYPE) {
                    switch (d->mCellInfo->rtType) {
                        case LOC_RIL_TECH_CDMA:
                        BREAK_AND_DELETE_IF_NON_ZERO(
                                9, card->addString(RILCELLINFO_FIELD_RIL_TECH_TYPE, "CDMA"), ic);
                        if ((d->mCellInfo->valid_mask & LOC_RIL_TECH_CDMA_HAS_MCC) ==
                                LOC_RIL_TECH_CDMA_HAS_MCC) {
                            BREAK_AND_DELETE_IF_NON_ZERO(10, card->addUInt16(
                                    RILLCELLINFO_FIELD_MCC, d->mCellInfo->u.cdmaCinfo.mcc), ic);
                        }
                        if ((d->mCellInfo->valid_mask & LOC_RIL_TECH_CDMA_HAS_SID) ==
                                LOC_RIL_TECH_CDMA_HAS_SID) {
                            BREAK_AND_DELETE_IF_NON_ZERO(11, card->addUInt16(
                                    RILLCELLINFO_FIELD_SID, d->mCellInfo->u.cdmaCinfo.sid), ic);
                        }
                        if ((d->mCellInfo->valid_mask & LOC_RIL_TECH_CDMA_HAS_NID) ==
                                LOC_RIL_TECH_CDMA_HAS_NID) {
                            BREAK_AND_DELETE_IF_NON_ZERO(12, card->addUInt16(
                                    RILLCELLINFO_FIELD_NID, d->mCellInfo->u.cdmaCinfo.nid), ic);
                        }
                        if ((d->mCellInfo->valid_mask & LOC_RIL_TECH_CDMA_HAS_BSID) ==
                                LOC_RIL_TECH_CDMA_HAS_BSID) {
                            BREAK_AND_DELETE_IF_NON_ZERO(13, card->addUInt32(
                                    RILLCELLINFO_FIELD_BSID, d->mCellInfo->u.cdmaCinfo.bsid), ic);
                        }
                        if ((d->mCellInfo->valid_mask & LOC_RIL_TECH_CDMA_HAS_BSLAT) ==
                                LOC_RIL_TECH_CDMA_HAS_BSLAT) {
                            BREAK_AND_DELETE_IF_NON_ZERO(14, card->addInt32(
                                    RILLCELLINFO_FIELD_BSLAT, d->mCellInfo->u.cdmaCinfo.bslat), ic);
                        }
                        if ((d->mCellInfo->valid_mask & LOC_RIL_TECH_CDMA_HAS_BSLONG) ==
                                LOC_RIL_TECH_CDMA_HAS_BSLONG) {
                            BREAK_AND_DELETE_IF_NON_ZERO(15, card->addInt32(
                                    RILLCELLINFO_FIELD_BSLON, d->mCellInfo->u.cdmaCinfo.bslon), ic);
                        }
                        if ((d->mCellInfo->valid_mask & LOC_RIL_TECH_CDMA_HAS_TIMEZONE) ==
                                LOC_RIL_TECH_CDMA_HAS_TIMEZONE) {
                            BREAK_AND_DELETE_IF_NON_ZERO(
                                    16, card->addInt32(RILLCELLINFO_FIELD_UTC_TIME_OFFSET,
                                    d->mCellInfo->u.cdmaCinfo.local_timezone_offset_from_utc), ic);
                        }
                        if ((d->mCellInfo->valid_mask & LOC_RIL_TECH_CDMA_HAS_DAYLIGHT_SAVING) ==
                                LOC_RIL_TECH_CDMA_HAS_DAYLIGHT_SAVING) {
                            BREAK_AND_DELETE_IF_NON_ZERO(17, card->addBool(
                                    RILLCELLINFO_FIELD_DAYLIGHT_TIMEZONE,
                                    d->mCellInfo->u.cdmaCinfo.local_timezone_on_daylight_savings),
                                    ic);
                        }
                        break;
                        case LOC_RIL_TECH_GSM:
                        BREAK_AND_DELETE_IF_NON_ZERO(18, card->addString(
                                    RILCELLINFO_FIELD_RIL_TECH_TYPE, "GSM"), ic);
                        if ((d->mCellInfo->valid_mask & LOC_RIL_TECH_GW_HAS_MCC) ==
                                LOC_RIL_TECH_GW_HAS_MCC) {
                            BREAK_AND_DELETE_IF_NON_ZERO(19, card->addUInt16(
                                    RILLCELLINFO_FIELD_MCC, d->mCellInfo->u.gsmCinfo.mcc), ic);
                        }
                        if ((d->mCellInfo->valid_mask & LOC_RIL_TECH_GW_HAS_MNC) ==
                                LOC_RIL_TECH_GW_HAS_MNC) {
                            BREAK_AND_DELETE_IF_NON_ZERO(20, card->addUInt16(
                                    RILLCELLINFO_FIELD_MNC, d->mCellInfo->u.gsmCinfo.mnc), ic);
                        }
                        if ((d->mCellInfo->valid_mask & LOC_RIL_TECH_GW_HAS_LAC) ==
                                LOC_RIL_TECH_GW_HAS_LAC) {
                            BREAK_AND_DELETE_IF_NON_ZERO(21, card->addUInt16(
                                    RILLCELLINFO_FIELD_LAC, d->mCellInfo->u.gsmCinfo.lac), ic);
                        }
                        if ((d->mCellInfo->valid_mask & LOC_RIL_TECH_GW_HAS_CID) ==
                                LOC_RIL_TECH_GW_HAS_CID) {
                            BREAK_AND_DELETE_IF_NON_ZERO(22, card->addUInt32(
                                    RILLCELLINFO_FIELD_CID, d->mCellInfo->u.gsmCinfo.cid), ic);
                        }
                        break;
                        case LOC_RIL_TECH_WCDMA:
                            BREAK_AND_DELETE_IF_NON_ZERO(23, card->addString(
                                    RILCELLINFO_FIELD_RIL_TECH_TYPE, "WCDMA"), ic);
                            BREAK_AND_DELETE_IF_NON_ZERO(24, card->addUInt16(
                                    RILLCELLINFO_FIELD_MCC, d->mCellInfo->u.wcdmaCinfo.mcc), ic);
                            BREAK_AND_DELETE_IF_NON_ZERO(25, card->addUInt16(
                                    RILLCELLINFO_FIELD_MNC, d->mCellInfo->u.wcdmaCinfo.mnc), ic);
                            BREAK_AND_DELETE_IF_NON_ZERO(26, card->addUInt16(
                                    RILLCELLINFO_FIELD_LAC, d->mCellInfo->u.wcdmaCinfo.lac), ic);
                            BREAK_AND_DELETE_IF_NON_ZERO(27, card->addUInt32(
                                    RILLCELLINFO_FIELD_CID, d->mCellInfo->u.wcdmaCinfo.cid), ic);
                            BREAK_AND_DELETE_IF_NON_ZERO(40, card->addUInt16(
                                    RILLCELLINFO_FIELD_WCDMA_FCN,
                                    d->mCellInfo->u.wcdmaCinfo.frequency), ic);
                            BREAK_AND_DELETE_IF_NON_ZERO(41, card->addUInt16(
                                    RILLCELLINFO_FIELD_PSC,
                                    d->mCellInfo->u.wcdmaCinfo.primarySynchcode), ic);
                            BREAK_AND_DELETE_IF_NON_ZERO(45, card->addUInt32(
                                    RILLCELLINFO_FIELD_VALID_MASK,
                                    d->mCellInfo->valid_mask), ic);
                        break;
                        case LOC_RIL_TECH_LTE:
                            BREAK_AND_DELETE_IF_NON_ZERO(28, card->addString(
                                    RILCELLINFO_FIELD_RIL_TECH_TYPE, "LTE"), ic);
                            BREAK_AND_DELETE_IF_NON_ZERO(29, card->addUInt16(
                                    RILLCELLINFO_FIELD_MCC, d->mCellInfo->u.lteCinfo.mcc), ic);
                            BREAK_AND_DELETE_IF_NON_ZERO(30, card->addUInt16(
                                    RILLCELLINFO_FIELD_MNC, d->mCellInfo->u.lteCinfo.mnc), ic);
                            BREAK_AND_DELETE_IF_NON_ZERO(31, card->addUInt16(
                                    RILLCELLINFO_FIELD_TAC, d->mCellInfo->u.lteCinfo.tac), ic);
                            BREAK_AND_DELETE_IF_NON_ZERO(32, card->addUInt16(
                                    RILLCELLINFO_FIELD_PCI, d->mCellInfo->u.lteCinfo.pci), ic);
                            BREAK_AND_DELETE_IF_NON_ZERO(33, card->addUInt32(
                                    RILLCELLINFO_FIELD_CID, d->mCellInfo->u.lteCinfo.cid), ic);
                            BREAK_AND_DELETE_IF_NON_ZERO(42, card->addUInt32(
                                    RILLCELLINFO_FIELD_LTE_EARFCN,
                                    d->mCellInfo->u.lteCinfo.earfcn), ic);
                            BREAK_AND_DELETE_IF_NON_ZERO(46, card->addUInt32(
                                    RILLCELLINFO_FIELD_VALID_MASK,
                                    d->mCellInfo->valid_mask), ic);
                        break;
                        case LOC_RIL_TECH_NR:
                            BREAK_AND_DELETE_IF_NON_ZERO(34, card->addString(
                                    RILCELLINFO_FIELD_RIL_TECH_TYPE, "NR"), ic);
                            BREAK_AND_DELETE_IF_NON_ZERO(35, card->addUInt16(
                                    RILLCELLINFO_FIELD_MCC, d->mCellInfo->u.nrCinfo.mcc), ic);
                            BREAK_AND_DELETE_IF_NON_ZERO(36, card->addUInt16(
                                    RILLCELLINFO_FIELD_MNC, d->mCellInfo->u.nrCinfo.mnc), ic);
                            BREAK_AND_DELETE_IF_NON_ZERO(37, card->addInt32(
                                    RILLCELLINFO_FIELD_TAC, d->mCellInfo->u.nrCinfo.tac), ic);
                            BREAK_AND_DELETE_IF_NON_ZERO(39, card->addUInt64(
                                    RILLCELLINFO_FIELD_CID, d->mCellInfo->u.nrCinfo.nci), ic);
                            BREAK_AND_DELETE_IF_NON_ZERO(43, card->addUInt32(
                                    RILLCELLINFO_FIELD_PCI, d->mCellInfo->u.nrCinfo.pci), ic);
                            BREAK_AND_DELETE_IF_NON_ZERO(44, card->addInt32(
                                    RILLCELLINFO_FIELD_NRARFCN,
                                    d->mCellInfo->u.nrCinfo.nrarfcn), ic);
                            BREAK_AND_DELETE_IF_NON_ZERO(47, card->addUInt32(
                                    RILLCELLINFO_FIELD_VALID_MASK, 
                                    d->mCellInfo->valid_mask), ic);
                        break;
                        case LOC_RIL_TECH_TD_SCDMA:
                        case LOC_RIL_TECH_MAX:
                        default:
                        break;
                    }; // end switch
                }
            }
        }
        ADD_INNER_CARD();
    } while (0);
    EXIT_LOG_WITH_ERROR("%d", result);
    return result;
}

int32_t WifiSupplicantStatusSerializer::serialize (OutPostcard * oc) {
    int32 result = 0;
    OutPostcard * card = oc;
    OutPostcard * ic = NULL;
    ENTRY_LOG();
    do {
        CREATE_INNER_CARD(WifiSupplicantStatusDataItem, WIFI_SUPPLICANT_STATUS_DATA_ITEM_ID);
        BREAK_AND_DELETE_IF_NON_ZERO(
                5, card->addInt32(WIFI_SUPPLICANT_FIELD_STATE, d->mState), ic);
        // TODO: serialize remaining fields if required
        ADD_INNER_CARD();
    } while (0);
    EXIT_LOG_WITH_ERROR("%d", result);
    return result;
}
int32_t TacSerializer::serialize (OutPostcard * oc) {
    int32 result = 0;
    OutPostcard * card = oc;
    OutPostcard * ic = NULL;
    ENTRY_LOG();
    do {
        CREATE_INNER_CARD(TacDataItem, TAC_DATA_ITEM_ID);
        BREAK_IF_NON_ZERO(5, card->addString(TAC_FIELD_NAME, d->mValue.c_str()));
        ADD_INNER_CARD();
    } while (0);
    EXIT_LOG_WITH_ERROR("%d", result);
    return result;
}
int32_t MccmncSerializer::serialize (OutPostcard * oc) {
    int32 result = 0;
    OutPostcard * card = oc;
    OutPostcard * ic = NULL;
    ENTRY_LOG();
    do {
        CREATE_INNER_CARD(MccmncDataItem, MCCMNC_DATA_ITEM_ID);
        BREAK_IF_NON_ZERO(5, card->addString(MCCMNC_FIELD_NAME, d->mValue.c_str()));
        ADD_INNER_CARD();
    } while (0);
    EXIT_LOG_WITH_ERROR("%d", result);
    return result;
}
int32_t PreciseLocationEnabledSerializer::serialize(OutPostcard * oc) {
    int32 result = 0;
    OutPostcard * card = oc;
    OutPostcard * ic = NULL;
    ENTRY_LOG();
    do {
        CREATE_INNER_CARD(PreciseLocationEnabledDataItem, PRECISE_LOCATION_ENABLED_DATA_ITEM_ID);
        BREAK_IF_NON_ZERO(5, card->addBool(PRECISE_LOCATION_ENABLED_FIELD_NAME,
            d->mPreciseLocationEnabled));
        ADD_INNER_CARD();
    } while (0);
    EXIT_LOG_WITH_ERROR("%d", result);
    return result;
}

int32_t TrackingStartedSerializer::serialize(OutPostcard * oc) {
    int32 result = 0;
    OutPostcard * card = oc;
    OutPostcard * ic = NULL;
    ENTRY_LOG();
    do {
        CREATE_INNER_CARD(TrackingStartedDataItem, TRACKING_STARTED_DATA_ITEM_ID);
        BREAK_IF_NON_ZERO(5, card->addBool(TRACKING_STARTED_FIELD_NAME, d->mTrackingStarted));
        ADD_INNER_CARD();
    } while (0);
    EXIT_LOG_WITH_ERROR("%d", result);
    return result;
}

int32_t NtripStartedSerializer::serialize(OutPostcard * oc) {
    int32 result = 0;
    OutPostcard * card = oc;
    OutPostcard * ic = NULL;
    ENTRY_LOG();
    do {
        CREATE_INNER_CARD(NtripStartedDataItem, NTRIP_STARTED_DATA_ITEM_ID);
        BREAK_IF_NON_ZERO(5, card->addBool(NTRIP_STARTED_FIELD_NAME, d->mNtripStarted));
        ADD_INNER_CARD();
    } while (0);
    EXIT_LOG_WITH_ERROR("%d", result);
    return result;
}

int32_t LocFeatureStatusSerializer::serialize(OutPostcard * oc) {
    int32 result = 0;
    OutPostcard * card = oc;
    OutPostcard * ic = NULL;
    ENTRY_LOG();
    do {
        CREATE_INNER_CARD(LocFeatureStatusDataItem, LOC_FEATURE_STATUS_DATA_ITEM_ID);
        uint32_t len = d->mFids.size();
        BREAK_IF_NON_ZERO(5, card->addUInt32(LOC_FEATURE_ID_NUM_FIELD_NAME, len));
        if (len > 0) {
            uint32_t fids[len];
            uint32_t i = 0;
            for (int item : d->mFids) {
                fids[i] = item;
                i++;
            }
            BREAK_IF_NON_ZERO(6, card->addArrayUInt32(LOC_FEATURE_ID_ARRAY_FIELD_NAME, len, fids));
        }
        ADD_INNER_CARD();
    } while (0);
    EXIT_LOG_WITH_ERROR("%d", result);
    return result;
}

int32_t NlpSessionStartedSerializer::serialize(OutPostcard * oc) {
    int32 result = 0;
    OutPostcard * card = oc;
    OutPostcard * ic = NULL;
    ENTRY_LOG();
    do {
        CREATE_INNER_CARD(NlpSessionStartedDataItem, NETWORK_POSITIONING_STARTED_DATA_ITEM_ID);
        BREAK_IF_NON_ZERO(5, card->addBool(NLP_SESSION_STARTED_FIELD_NAME, d->mNlpStarted));
        ADD_INNER_CARD();
    } while (0);
    EXIT_LOG_WITH_ERROR("%d", result);
    return result;
}

// deSerialize TODO
int32_t AirplaneModeSerializer::deSerialize (InPostcard * ic) {
    int32 result = 0;
    ENTRY_LOG();
    do {
    } while (0);
    EXIT_LOG_WITH_ERROR("%d", result);
    return result;
}
int32_t ENHSerializer::deSerialize (InPostcard * ic) {
    int32 result = 0;
    ENTRY_LOG();
    do {
    } while (0);
    EXIT_LOG_WITH_ERROR("%d", result);
    return result;
}
int32_t GPSStateSerializer::deSerialize (InPostcard * ic) {
    int32 result = 0;
    ENTRY_LOG();
    do {
    } while (0);
    EXIT_LOG_WITH_ERROR("%d", result);
    return result;
}
int32_t NLPStatusSerializer::deSerialize (InPostcard * ic) {
    int32 result = 0;
    ENTRY_LOG();
    do {
    } while (0);
    EXIT_LOG_WITH_ERROR("%d", result);
    return result;
}
int32_t WifiHardwareStateSerializer::deSerialize (InPostcard * ic) {
    int32 result = 0;
    ENTRY_LOG();
    do {
    } while (0);
    EXIT_LOG_WITH_ERROR("%d", result);
    return result;
}
int32_t ScreenStateSerializer::deSerialize (InPostcard * ic) {
    int32 result = 0;
    ENTRY_LOG();
    do {
    } while (0);
    EXIT_LOG_WITH_ERROR("%d", result);
    return result;
}
int32_t PowerConnectStateSerializer::deSerialize (InPostcard * ic) {
    int32 result = 0;
    ENTRY_LOG();
    do {
    } while (0);
    EXIT_LOG_WITH_ERROR("%d", result);
    return result;
}
int32_t BatteryLevelSerializer::deSerialize (InPostcard * ic) {
    int32 result = 0;
    ENTRY_LOG();
    do {
    } while (0);
    EXIT_LOG_WITH_ERROR("%d", result);
    return result;
}
int32_t TimeZoneChangeSerializer::deSerialize (InPostcard * ic) {
    int32 result = 0;
    ENTRY_LOG();
    do {
    } while (0);
    EXIT_LOG_WITH_ERROR("%d", result);
    return result;
}
int32_t TimeChangeSerializer::deSerialize (InPostcard * ic) {
    int32 result = 0;
    ENTRY_LOG();
    do {
    } while (0);
    EXIT_LOG_WITH_ERROR("%d", result);
    return result;
}
int32_t ShutdownStateSerializer::deSerialize (InPostcard * ic) {
    int32 result = 0;
    ENTRY_LOG();
    do {
    } while (0);
    EXIT_LOG_WITH_ERROR("%d", result);
    return result;
}
int32_t AssistedGpsSerializer::deSerialize (InPostcard * ic) {
    int32 result = 0;
    ENTRY_LOG();
    do {
    } while (0);
    EXIT_LOG_WITH_ERROR("%d", result);
    return result;
}
int32_t NetworkInfoSerializer::deSerialize (InPostcard * ic) {
    int32 result = 0;
    ENTRY_LOG();
    do {
    } while (0);
    EXIT_LOG_WITH_ERROR("%d", result);
    return result;
}
int32_t ServiceStatusSerializer::deSerialize (InPostcard * ic) {
    int32 result = 0;
    ENTRY_LOG();
    do {
    } while (0);
    EXIT_LOG_WITH_ERROR("%d", result);
    return result;
}
int32_t ModelSerializer::deSerialize (InPostcard * ic) {
    int32 result = 0;
    ENTRY_LOG();
    do {
    } while (0);
    EXIT_LOG_WITH_ERROR("%d", result);
    return result;
}
int32_t ManufacturerSerializer::deSerialize (InPostcard * ic) {
    int32 result = 0;
    ENTRY_LOG();
    do {
    } while (0);
    EXIT_LOG_WITH_ERROR("%d", result);
    return result;
}
int32_t InEmergencyCallSerializer::deSerialize (InPostcard * ic) {
    int32 result = 0;
    ENTRY_LOG();
    do {
    } while (0);
    EXIT_LOG_WITH_ERROR("%d", result);
    return result;
}
int32_t RilCellInfoSerializer::deSerialize (InPostcard * ic) {
    int32 result = 0;
    ENTRY_LOG();
    do {
    } while (0);
    EXIT_LOG_WITH_ERROR("%d", result);
    return result;
}
int32_t RilServiceInfoSerializer::deSerialize (InPostcard * ic) {
    int32 result = 0;
    ENTRY_LOG();
    do {
    } while (0);
    EXIT_LOG_WITH_ERROR("%d", result);
    return result;
}
int32_t WifiSupplicantStatusSerializer::deSerialize (InPostcard * ic) {
    int32 result = 0;
    ENTRY_LOG();
    do {
    } while (0);
    EXIT_LOG_WITH_ERROR("%d", result);
    return result;
}
int32_t TacSerializer::deSerialize (InPostcard * ic) {
    int32 result = 0;
    ENTRY_LOG();
    do {
    } while (0);
    EXIT_LOG_WITH_ERROR("%d", result);
    return result;
}
int32_t MccmncSerializer::deSerialize (InPostcard * ic) {
    int32 result = 0;
    ENTRY_LOG();
    do {
    } while (0);
    EXIT_LOG_WITH_ERROR("%d", result);
    return result;
}
int32_t TrackingStartedSerializer::deSerialize(InPostcard * ic) {
     int32 result = 0;
     ENTRY_LOG();
     do {
     } while (0);
     EXIT_LOG_WITH_ERROR("%d", result);
     return result;
}

int32_t NtripStartedSerializer::deSerialize(InPostcard * ic) {
     int32 result = 0;
     ENTRY_LOG();
     do {
     } while (0);
     EXIT_LOG_WITH_ERROR("%d", result);
     return result;
}

int32_t PreciseLocationEnabledSerializer::deSerialize(InPostcard * ic) {
     int32 result = 0;
     ENTRY_LOG();
     do {
     } while (0);
     EXIT_LOG_WITH_ERROR("%d", result);
     return result;
}

int32_t LocFeatureStatusSerializer::deSerialize(InPostcard * ic) {
     int32 result = 0;
     ENTRY_LOG();
     do {
     } while (0);
     EXIT_LOG_WITH_ERROR("%d", result);
     return result;
}

int32_t NlpSessionStartedSerializer::deSerialize(InPostcard * ic) {
     int32 result = 0;
     ENTRY_LOG();
     do {
     } while (0);
     EXIT_LOG_WITH_ERROR("%d", result);
     return result;
}
