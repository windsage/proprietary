/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

GENERAL DESCRIPTION
  DataItemSerializers

Copyright (c) 2020-2023 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/

#ifndef IZAT_MANAGER_DATAITEMSERIALIZERS_H
#define IZAT_MANAGER_DATAITEMSERIALIZERS_H

#include <comdef.h>
#include <string>
#include <DataItemConcreteTypeDefaultValues.h>
#include <DataItemConcreteTypeFieldNames.h>
#include <DataItemConcreteTypes.h>
#include <DataItemConcreteTypesExt.h>
#include <loc_ril.h>

namespace qc_loc_fw {
    class OutPostcard;
    class InPostcard;
}

namespace izat_manager {

using namespace ::qc_loc_fw;
using namespace std;
using namespace loc_core;

/**
 * @brief IDataItemSerialization interface.
 * @details IDataItemSerialization interface.
 *
 * @param out Pointer to an instance of OutPostcard.
 * @return Zero for success or non zero for failure.
 */
class IDataItemSerialization {
public:
    static IDataItemSerialization* createSerializer(IDataItemCore* dataItem);
    /**
     * @brief Serialize.
     * @details Serialize.
     *
     * @param out Pointer to an instance of OutPostcard.
     * @return Zero for success or non zero for failure.
     */
    virtual int32 serialize (OutPostcard * out) = 0;
    /**
     * @brief Deserialize.
     * @details Deserialize.
     *
     * @param in Pointer to an instance of InPostcard.
     * @return Zero for success or non zero for failure.
     */
    virtual int32 deSerialize (InPostcard * in) = 0;

    /**
     * @brief Destructor.
     * @details Destructor.
     */
    virtual ~IDataItemSerialization () {}
    inline const string & getCardName () const {
        return mCardName;
    }
    inline bool hasCardName () const {
        return !mCardName.empty ();
    }
    IDataItemSerialization(const string& str): mCardName(str) {}
protected:
    const string mCardName;
};


class AirplaneModeSerializer: public IDataItemSerialization {
public:
    AirplaneModeDataItem mDataItem;
    AirplaneModeSerializer(AirplaneModeDataItem* di): IDataItemSerialization(AIRPLANEMODE_CARD),
            mDataItem(*di) {}
    virtual ~AirplaneModeSerializer() {}
    virtual int32 serialize (OutPostcard * out);
    virtual int32 deSerialize (InPostcard * in);
};

class ENHSerializer: public IDataItemSerialization {
public:
    ENHDataItem mDataItem;
    ENHSerializer(ENHDataItem* di): IDataItemSerialization(ENH_CARD), mDataItem(*di) {}
    virtual ~ENHSerializer() {}
    virtual int32 serialize (OutPostcard * out);
    virtual int32 deSerialize (InPostcard * in);
};

class GPSStateSerializer: public IDataItemSerialization {
public:
    GPSStateDataItem mDataItem;
    GPSStateSerializer(GPSStateDataItem* di): IDataItemSerialization(GPSSTATE_CARD),
            mDataItem(*di) {}
    virtual ~GPSStateSerializer() {}
    virtual int32 serialize (OutPostcard * out);
    virtual int32 deSerialize (InPostcard * in);
};

class NLPStatusSerializer: public IDataItemSerialization {
public:
    NLPStatusDataItem mDataItem;
    NLPStatusSerializer(NLPStatusDataItem* di): IDataItemSerialization(NLPSTATUS_CARD),
            mDataItem(*di) {}
    virtual ~NLPStatusSerializer() {}
    virtual int32 serialize (OutPostcard * out);
    virtual int32 deSerialize (InPostcard * in);
};

class WifiHardwareStateSerializer: public IDataItemSerialization {
public:
    WifiHardwareStateDataItem mDataItem;
    WifiHardwareStateSerializer(WifiHardwareStateDataItem* di):
            IDataItemSerialization(WIFIHARDWARESTATE_CARD), mDataItem(*di) {}
    virtual ~WifiHardwareStateSerializer() {}
    virtual int32 serialize (OutPostcard * out);
    virtual int32 deSerialize (InPostcard * in);
};

class ScreenStateSerializer: public IDataItemSerialization {
public:
    ScreenStateDataItem mDataItem;
    ScreenStateSerializer(ScreenStateDataItem* di): IDataItemSerialization(SCREENSTATE_CARD),
            mDataItem(*di) {}
    virtual ~ScreenStateSerializer() {}
    virtual int32 serialize (OutPostcard * out);
    virtual int32 deSerialize (InPostcard * in);
};

class PowerConnectStateSerializer: public IDataItemSerialization {
public:
    PowerConnectStateDataItem mDataItem;
    PowerConnectStateSerializer(PowerConnectStateDataItem* di):
            IDataItemSerialization(POWERCONNECTSTATE_CARD), mDataItem(*di) {}
    virtual ~PowerConnectStateSerializer() {}
    virtual int32 serialize (OutPostcard * out);
    virtual int32 deSerialize (InPostcard * in);
};

class TimeZoneChangeSerializer: public IDataItemSerialization {
public:
    TimeZoneChangeDataItem mDataItem;
    TimeZoneChangeSerializer(TimeZoneChangeDataItem* di):
            IDataItemSerialization(TIMEZONECHANGE_CARD), mDataItem(*di) {}
    virtual ~TimeZoneChangeSerializer() {}
    virtual int32 serialize (OutPostcard * out);
    virtual int32 deSerialize (InPostcard * in);
};

class TimeChangeSerializer: public IDataItemSerialization {
public:
    TimeChangeDataItem mDataItem;
    TimeChangeSerializer(TimeChangeDataItem* di): IDataItemSerialization(TIMECHANGE_CARD),
            mDataItem(*di) {}
    virtual ~TimeChangeSerializer() {}
    virtual int32 serialize (OutPostcard * out);
    virtual int32 deSerialize (InPostcard * in);
};

class ShutdownStateSerializer: public IDataItemSerialization {
public:
    ShutdownStateDataItem mDataItem;
    ShutdownStateSerializer(ShutdownStateDataItem* di): IDataItemSerialization(SHUTDOWN_CARD),
            mDataItem(*di) {}
    virtual ~ShutdownStateSerializer() {}
    virtual int32 serialize (OutPostcard * out);
    virtual int32 deSerialize (InPostcard * in);
};

class AssistedGpsSerializer: public IDataItemSerialization {
public:
    AssistedGpsDataItem mDataItem;
    AssistedGpsSerializer(AssistedGpsDataItem* di): IDataItemSerialization(ASSISTEDGPS_CARD),
            mDataItem(*di) {}
    virtual ~AssistedGpsSerializer() {}
    virtual int32 serialize (OutPostcard * out);
    virtual int32 deSerialize (InPostcard * in);
};

class NetworkInfoSerializer: public IDataItemSerialization {
public:
    NetworkInfoDataItem mDataItem;
    NetworkInfoSerializer(NetworkInfoDataItem* di): IDataItemSerialization(NETWORKINFO_CARD),
            mDataItem(*di) {}
    virtual ~NetworkInfoSerializer() {}
    virtual int32 serialize (OutPostcard * out);
    virtual int32 deSerialize (InPostcard * in);
};

class ServiceStatusSerializer: public IDataItemSerialization {
public:
    ServiceStatusDataItem mDataItem;
    ServiceStatusSerializer(ServiceStatusDataItem* di): IDataItemSerialization(SERVICESTATUS_CARD),
            mDataItem(*di) {}
    virtual ~ServiceStatusSerializer() {}
    virtual int32 serialize (OutPostcard * out);
    virtual int32 deSerialize (InPostcard * in);
};

class ModelSerializer: public IDataItemSerialization {
public:
    ModelDataItem mDataItem;
    ModelSerializer(ModelDataItem* di): IDataItemSerialization(MODEL_CARD),
            mDataItem(*di) {}
    virtual ~ModelSerializer() {}
    virtual int32 serialize (OutPostcard * out);
    virtual int32 deSerialize (InPostcard * in);
};

class ManufacturerSerializer: public IDataItemSerialization {
public:
    ManufacturerDataItem mDataItem;
    ManufacturerSerializer(ManufacturerDataItem* di): IDataItemSerialization(MANUFACTURER_CARD),
            mDataItem(*di) {}
    virtual ~ManufacturerSerializer() {}
    virtual int32 serialize (OutPostcard * out);
    virtual int32 deSerialize (InPostcard * in);
};

class InEmergencyCallSerializer: public IDataItemSerialization {
public:
    InEmergencyCallDataItem mDataItem;
    InEmergencyCallSerializer(InEmergencyCallDataItem* di):
            IDataItemSerialization(IN_EMERGENCY_CALL_CARD), mDataItem(*di) {}
    virtual ~InEmergencyCallSerializer() {}
    virtual int32 serialize (OutPostcard * out);
    virtual int32 deSerialize (InPostcard * in);
};

class RilServiceInfoSerializer: public IDataItemSerialization {
public:
    RilServiceInfoDataItemExt mDataItem;
    RilServiceInfoSerializer(RilServiceInfoDataItem* di):
            IDataItemSerialization(RILSERVICEINFO_CARD),
            mDataItem((struct LOC_RilServiceInfo *)di->mData) {}
    virtual ~RilServiceInfoSerializer() {}
    virtual int32 serialize (OutPostcard * out);
    virtual int32 deSerialize (InPostcard * in);
// Data members
    struct LOC_RilServiceInfo * mServiceInfo;
private:
    bool mDestroy;
};

class RilCellInfoSerializer: public IDataItemSerialization {
public:
    RilCellInfoDataItemExt mDataItem;
    RilCellInfoSerializer(RilCellInfoDataItem* di): IDataItemSerialization(RILCELLINFO_CARD),
            mDataItem((struct LOC_RilCellInfo *)di->mData) {}
    virtual ~RilCellInfoSerializer() {}
    virtual int32 serialize (OutPostcard * out);
    virtual int32 deSerialize (InPostcard * in);
// Data members
    struct LOC_RilCellInfo * mCellInfo;
private:
    bool mDestroy;
};

class WifiSupplicantStatusSerializer: public IDataItemSerialization {
public:
    WifiSupplicantStatusDataItem mDataItem;
    WifiSupplicantStatusSerializer(WifiSupplicantStatusDataItem* di):
            IDataItemSerialization(WIFI_SUPPLICANT_STATUS_CARD), mDataItem(*di) {}
    virtual ~WifiSupplicantStatusSerializer() {}
    virtual int32 serialize (OutPostcard * out);
    virtual int32 deSerialize (InPostcard * in);
};

class TacSerializer: public IDataItemSerialization {
public:
    TacDataItem mDataItem;
    TacSerializer(TacDataItem* di): IDataItemSerialization(TAC_CARD),
            mDataItem(*di) {}
    virtual ~TacSerializer() {}
    virtual int32 serialize (OutPostcard * out);
    virtual int32 deSerialize (InPostcard * in);
};

class MccmncSerializer: public IDataItemSerialization {
public:
    MccmncDataItem mDataItem;
    MccmncSerializer(MccmncDataItem* di): IDataItemSerialization(MCCMNC_CARD),
            mDataItem(*di) {}
    virtual ~MccmncSerializer() {}
    virtual int32 serialize (OutPostcard * out);
    virtual int32 deSerialize (InPostcard * in);
};

class BatteryLevelSerializer: public IDataItemSerialization {
public:
    BatteryLevelDataItem mDataItem;
    BatteryLevelSerializer(BatteryLevelDataItem* di): IDataItemSerialization(BATTERYLEVEL_CARD),
            mDataItem(*di) {}
    virtual ~BatteryLevelSerializer() {}
    virtual int32 serialize (OutPostcard * out);
    virtual int32 deSerialize (InPostcard * in);
};

class PreciseLocationEnabledSerializer: public IDataItemSerialization {
public:
    PreciseLocationEnabledDataItem mDataItem;
    PreciseLocationEnabledSerializer(PreciseLocationEnabledDataItem* di):
            IDataItemSerialization(PRECISE_LOCATION_ENABLED_CARD), mDataItem(*di) {}
    virtual ~PreciseLocationEnabledSerializer() {}
    virtual int32 serialize (OutPostcard * out);
    virtual int32 deSerialize (InPostcard * in);
};

class TrackingStartedSerializer: public IDataItemSerialization {
public:
    TrackingStartedDataItem mDataItem;
    TrackingStartedSerializer(TrackingStartedDataItem* di):
            IDataItemSerialization(TRACKING_STARTED_CARD), mDataItem(*di) {}
    virtual ~TrackingStartedSerializer() {}
    virtual int32 serialize (OutPostcard * out);
    virtual int32 deSerialize (InPostcard * in);
};

class NtripStartedSerializer: public IDataItemSerialization {
public:
    NtripStartedDataItem mDataItem;
    NtripStartedSerializer(NtripStartedDataItem* di):
            IDataItemSerialization(NTRIP_STARTED_CARD), mDataItem(*di) {}
    virtual ~NtripStartedSerializer() {}
    virtual int32 serialize (OutPostcard * out);
    virtual int32 deSerialize (InPostcard * in);
};

class LocFeatureStatusSerializer: public IDataItemSerialization {
public:
    LocFeatureStatusDataItem mDataItem;
    LocFeatureStatusSerializer(LocFeatureStatusDataItem* di):
            IDataItemSerialization(LOC_FEATURE_STATUS_CARD), mDataItem(*di) {}
    virtual ~LocFeatureStatusSerializer() {}
    virtual int32 serialize (OutPostcard * out);
    virtual int32 deSerialize (InPostcard * in);
};

class NlpSessionStartedSerializer: public IDataItemSerialization {
public:
    NlpSessionStartedDataItem mDataItem;
    NlpSessionStartedSerializer(NlpSessionStartedDataItem* di):
            IDataItemSerialization(NLP_SESSION_STARTED_CARD), mDataItem(*di) {}
    virtual ~NlpSessionStartedSerializer() {}
    virtual int32 serialize (OutPostcard * out);
    virtual int32 deSerialize (InPostcard * in);
};

} //namespace izat_manager

#endif // #ifndef IZAT_MANAGER_DATAITEMSERIALIZERS_H
