/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

GENERAL DESCRIPTION
  DataItemsFactory Implementation

  Copyright (c) 2015-2018, 2020-2023 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/

#include <IDataItemCore.h>
#include <DataItemSerializers.h>
#include "DataItemConcreteTypesExt.h"

using namespace izat_manager;

// Macro to create data item and return the object of type requested.
// In case of cloneDataItem, we also need to copy from DI object passed.
#define CREATE_DI_AND_CAST(DICLASS) \
{ \
    DICLASS *dataItem = new DICLASS(); \
    if (nullptr != dataItem) { \
        if (nullptr != di) { \
            dataItem->copy (di); \
        } \
        diItemSerializer = static_cast<T *> (dataItem); \
    } \
} \

IDataItemSerialization* IDataItemSerialization::createSerializer(IDataItemCore* dataItem) {
    IDataItemSerialization* diItemSerializer = nullptr;
    DataItemId id = dataItem->getId();
    switch (id) {
    case AIRPLANEMODE_DATA_ITEM_ID:
        diItemSerializer = new AirplaneModeSerializer((AirplaneModeDataItem*)dataItem);
        break;
    case ENH_DATA_ITEM_ID:
        diItemSerializer = new ENHSerializer((ENHDataItem*)dataItem);
        break;
    case GPSSTATE_DATA_ITEM_ID:
        diItemSerializer = new GPSStateSerializer((GPSStateDataItem*)dataItem);
        break;
    case NLPSTATUS_DATA_ITEM_ID:
        diItemSerializer = new NLPStatusSerializer((NLPStatusDataItem*)dataItem);
        break;
    case WIFIHARDWARESTATE_DATA_ITEM_ID:
        diItemSerializer = new WifiHardwareStateSerializer((WifiHardwareStateDataItem*)dataItem);
        break;
    case NETWORKINFO_DATA_ITEM_ID:
        diItemSerializer = new NetworkInfoSerializer((NetworkInfoDataItem*)dataItem);
        break;
    case SERVICESTATUS_DATA_ITEM_ID:
       diItemSerializer = new ServiceStatusSerializer((ServiceStatusDataItem*)dataItem);
        break;
    case RILCELLINFO_DATA_ITEM_ID:
        diItemSerializer = new RilCellInfoSerializer((RilCellInfoDataItem*)dataItem);
        break;
    case RILSERVICEINFO_DATA_ITEM_ID:
        diItemSerializer = new RilServiceInfoSerializer((RilServiceInfoDataItem*)dataItem);
        break;
    case MODEL_DATA_ITEM_ID:
        diItemSerializer = new ModelSerializer((ModelDataItem*)dataItem);
        break;
    case MANUFACTURER_DATA_ITEM_ID:
        diItemSerializer = new ManufacturerSerializer((ManufacturerDataItem*)dataItem);
        break;
    case IN_EMERGENCY_CALL_DATA_ITEM_ID:
        diItemSerializer = new InEmergencyCallSerializer((InEmergencyCallDataItem*)dataItem);
        break;
    case ASSISTED_GPS_DATA_ITEM_ID:
        diItemSerializer = new AssistedGpsSerializer((AssistedGpsDataItem*)dataItem);
        break;
    case SCREEN_STATE_DATA_ITEM_ID:
        diItemSerializer = new ScreenStateSerializer((ScreenStateDataItem*)dataItem);
        break;
    case POWER_CONNECTED_STATE_DATA_ITEM_ID:
        diItemSerializer = new PowerConnectStateSerializer((PowerConnectStateDataItem*)dataItem);
        break;
    case TIMEZONE_CHANGE_DATA_ITEM_ID:
        diItemSerializer = new TimeZoneChangeSerializer((TimeZoneChangeDataItem*)dataItem);
        break;
    case TIME_CHANGE_DATA_ITEM_ID:
        diItemSerializer = new TimeChangeSerializer((TimeChangeDataItem*)dataItem);
        break;
    case WIFI_SUPPLICANT_STATUS_DATA_ITEM_ID:
        diItemSerializer = new WifiSupplicantStatusSerializer(
                (WifiSupplicantStatusDataItem*)dataItem);
        break;
    case SHUTDOWN_STATE_DATA_ITEM_ID:
        diItemSerializer = new ShutdownStateSerializer((ShutdownStateDataItem*)dataItem);
        break;
    case TAC_DATA_ITEM_ID:
        diItemSerializer = new TacSerializer((TacDataItem*)dataItem);
        break;
    case MCCMNC_DATA_ITEM_ID:
        diItemSerializer = new MccmncSerializer((MccmncDataItem*)dataItem);
        break;
    case BATTERY_LEVEL_DATA_ITEM_ID:
        diItemSerializer = new BatteryLevelSerializer((BatteryLevelDataItem*)dataItem);
        break;
    case TRACKING_STARTED_DATA_ITEM_ID:
        diItemSerializer = new TrackingStartedSerializer((TrackingStartedDataItem*)dataItem);
        break;
    case NTRIP_STARTED_DATA_ITEM_ID:
        diItemSerializer = new NtripStartedSerializer((NtripStartedDataItem*)dataItem);
        break;
    case PRECISE_LOCATION_ENABLED_DATA_ITEM_ID:
        diItemSerializer =
                new PreciseLocationEnabledSerializer((PreciseLocationEnabledDataItem*)dataItem);
        break;
    case LOC_FEATURE_STATUS_DATA_ITEM_ID:
        diItemSerializer =
                new LocFeatureStatusSerializer((LocFeatureStatusDataItem*)dataItem);
        break;
    case NETWORK_POSITIONING_STARTED_DATA_ITEM_ID:
        diItemSerializer =
                new NlpSessionStartedSerializer((NlpSessionStartedDataItem*)dataItem);
        break;
    case INVALID_DATA_ITEM_ID:
    case MAX_DATA_ITEM_ID:
    default:
        break;
    };
    return diItemSerializer;
}
