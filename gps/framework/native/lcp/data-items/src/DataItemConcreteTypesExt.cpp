/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

GENERAL DESCRIPTION
  DataItemConcreteTypes Implementation

  Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
  All rights reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/

#define LOG_TAG "DataItemConcreteTypesExt"

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
        BREAK_IF_NON_ZERO(3,oc->addCard(getCardName().c_str(), card)); \
    }

*/

// Ctors
RilServiceInfoDataItemExt::RilServiceInfoDataItemExt(LOC_RilServiceInfo * serviceInfo):
        RilServiceInfoDataItem(), mCardName(RILSERVICEINFO_CARD), mServiceInfo (serviceInfo),
        mDestroy (false) {
    if (mServiceInfo == NULL) {
        mServiceInfo = new (std :: nothrow) LOC_RilServiceInfo;
        if (mServiceInfo) {
            mDestroy = true;
            // Initialize to default values
            mServiceInfo->valid_mask = RTLLSERVICEINFO_DEFAULT_VALID_MASK;
            mServiceInfo->airIf_type = RTLLSERVICEINFO_DEFAULT_AIRIF_TYPE;
            mServiceInfo->carrierAirIf_type = RTLLSERVICEINFO_DEFAULT_CARRIER_AIRIF_TYPE;
            mServiceInfo->carrierMcc = RTLLSERVICEINFO_DEFAULT_CARRIER_MCC;
            mServiceInfo->carrierMnc = RTLLSERVICEINFO_DEFAULT_CARRIER_MNC;
            mServiceInfo->carrierNameLen = RTLLSERVICEINFO_DEFAULT_CARRIER_NAMELEN;
            strlcpy(mServiceInfo->carrierName, RTLLSERVICEINFO_DEFAULT_CARRIER_NAME,
                    LOC_RIL_CARRIER_NAME_MAX_LEN);
        }
    }
    mLength = sizeof(*mServiceInfo);
    mData = mServiceInfo;
}

RilCellInfoDataItemExt::RilCellInfoDataItemExt(LOC_RilCellInfo * cellInfo): RilCellInfoDataItem(),
        mCellInfo (cellInfo), mCardName(RILCELLINFO_CARD), mDestroy (false) {
    if (mCellInfo == NULL) {
        mCellInfo = new (std :: nothrow) LOC_RilCellInfo;
        if (mCellInfo) {
            mDestroy = true;
            // Initialize to default values
            mCellInfo->valid_mask = RILLCELLINFO_DEFAULT_VALID_MASK;
            mCellInfo->nwStatus = static_cast<LOC_NWstatus> (RILLCELLINFO_DEFAULT_NETWORK_STATUS);
            mCellInfo->rtType = static_cast<LOC_RilTechType> (RILLCELLINFO_DEFAULT_RIL_TECH_TYPE);
            mCellInfo->u.cdmaCinfo.mcc = RILLCELLINFO_DEFAULT_CDMA_CINFO_MCC;
            mCellInfo->u.cdmaCinfo.sid = RILLCELLINFO_DEFAULT_CDMA_CINFO_SID;
            mCellInfo->u.cdmaCinfo.nid = RILLCELLINFO_DEFAULT_CDMA_CINFO_NID;
            mCellInfo->u.cdmaCinfo.bsid = RILLCELLINFO_DEFAULT_CDMA_CINFO_BSID;
            mCellInfo->u.cdmaCinfo.bslat = RILLCELLINFO_DEFAULT_CDMA_CINFO_BSLAT;
            mCellInfo->u.cdmaCinfo.bslon = RILLCELLINFO_DEFAULT_CDMA_CINFO_BSLON;
            mCellInfo->u.cdmaCinfo.local_timezone_offset_from_utc =
                RILLCELLINFO_DEFAULT_CDMA_CINFO_LOCAL_TIME_ZONE_OFFSET_FROM_UTC;
            mCellInfo->u.cdmaCinfo.local_timezone_on_daylight_savings =
                RILLCELLINFO_DEFAULT_CDMA_CINFO_LOCAL_TIME_ZONE_ON_DAYLIGHT_SAVINGS;
        }
    }
    mLength = sizeof(*mCellInfo);
    mData = mCellInfo;
}

// Dtors
RilServiceInfoDataItemExt::~RilServiceInfoDataItemExt () {
    if (  (mDestroy) &&  (mServiceInfo) ) { delete mServiceInfo; }
    mData = nullptr;
}
RilCellInfoDataItemExt::~RilCellInfoDataItemExt () {
    if (  (mDestroy) &&  (mCellInfo) ) { delete mCellInfo; }
    mData = nullptr;
}

bool RilServiceInfoDataItemExt::operator==(const RilServiceInfoDataItem& other) const {
    LOC_RilServiceInfo* data = reinterpret_cast<LOC_RilServiceInfo*>(other.mData);
    return (data == mServiceInfo ||
            (nullptr != data && nullptr != mServiceInfo &&
             data->valid_mask == mServiceInfo->valid_mask &&
             data->airIf_type == mServiceInfo->airIf_type &&
             data->carrierAirIf_type == mServiceInfo->carrierAirIf_type &&
             data->carrierMcc == mServiceInfo->carrierMcc &&
             data->carrierMnc == mServiceInfo->carrierMnc &&
             data->carrierNameLen == mServiceInfo->carrierNameLen &&
             strncmp(data->carrierName, mServiceInfo->carrierName,
                     std::min(data->carrierNameLen,
                              (decltype(data->carrierNameLen))LOC_RIL_CARRIER_NAME_MAX_LEN))));
}

void RilServiceInfoDataItemExt::stringify(string& valueStr) {
    int32_t result = 0;
    ENTRY_LOG();
    do {
        STRINGIFY_ERROR_CHECK_AND_DOWN_CAST(RilServiceInfoDataItemExt, RILSERVICEINFO_DATA_ITEM_ID);
        valueStr.clear ();
        valueStr += RILSERVICEINFO_CARD;
        if (d->mServiceInfo->valid_mask) {
            valueStr += ":";

            valueStr += RILSERVICEINFO_FIELD_ARIF_TYPE_MASK;
            valueStr += ": ";
            char t[10];
            memset (t, '\0', 10);
            snprintf (t, 10, "%d", d->mServiceInfo->airIf_type);
            valueStr += t;


            valueStr += ", ";
            valueStr += RILSERVICEINFO_FIELD_CARRIER_ARIF_TYPE;
            valueStr += "(CDMA-1, GSM -2, WCDMA-4, LTA-8, EVDO-16, WIFI-32):";
            memset (t, '\0', 10);
            snprintf (t, 10, "%d", d->mServiceInfo->carrierAirIf_type);
            valueStr += t;

            valueStr += ", ";
            valueStr += RILSERVICEINFO_FIELD_CARRIER_MCC;
            valueStr += ": ";
            memset (t, '\0', 10);
            snprintf (t, 10, "%d", d->mServiceInfo->carrierMcc);
            valueStr += t;

            valueStr += ", ";
            valueStr += RILSERVICEINFO_FIELD_CARRIER_MNC;
            valueStr += ": ";
            memset (t, '\0', 10);
            snprintf (t, 10, "%d", d->mServiceInfo->carrierMcc);
            valueStr += t;

            valueStr += ", ";
            valueStr += RILSERVICEINFO_FIELD_CARRIER_NAME;
            valueStr += ": ";
            valueStr += d->mServiceInfo->carrierName;
        } else {
            valueStr += "Invalid";
        }
    } while (0);
    EXIT_LOG_WITH_ERROR("%d", result);

}
int32_t RilServiceInfoDataItemExt::copyFrom(IDataItemCore* src) {
    int32_t result = -1;
    ENTRY_LOG();
    do {
        COPIER_ERROR_CHECK_AND_DOWN_CAST(RilServiceInfoDataItemExt, RILSERVICEINFO_DATA_ITEM_ID);
        if (*s == *d) {
            result =0;
            break;
        }
        if (nullptr != d->mData) {
            memcpy(s->mServiceInfo, d->mData, sizeof(*(s->mServiceInfo)));
        }
        s->mData = s->mServiceInfo;
        result = 0;
    } while (0);
    EXIT_LOG_WITH_ERROR("%d", result);
    return result;
}
int32_t RilCellInfoDataItemExt::copyFrom(IDataItemCore* src) {
    int32_t result = -1;
    ENTRY_LOG();
    do {
        COPIER_ERROR_CHECK_AND_DOWN_CAST(RilCellInfoDataItemExt, RILCELLINFO_DATA_ITEM_ID);
        if (*s == *d) {
            result =0;
            break;
        }
        if (nullptr != d->mData) {
            memcpy(s->mCellInfo, d->mData, sizeof(*(s->mCellInfo)));
        }
        s->mData = s->mCellInfo;
        result = 0;
    } while (0);
    EXIT_LOG_WITH_ERROR("%d", result);
    return result;
}
void RilCellInfoDataItemExt::stringify(string& valueStr) {
    int32_t result = 0;
    ENTRY_LOG();
    do {
        STRINGIFY_ERROR_CHECK_AND_DOWN_CAST(RilCellInfoDataItemExt, RILCELLINFO_DATA_ITEM_ID);
        valueStr.clear ();
        valueStr += RILCELLINFO_CARD;
        if (d->mCellInfo->valid_mask) {
            if ((d->mCellInfo->valid_mask & LOC_RIL_CELLINFO_HAS_NW_STATUS) ==
                    LOC_RIL_CELLINFO_HAS_NW_STATUS) {
                valueStr += ": ";
                valueStr += RILCELLINFO_FIELD_NETWORK_STATUS;
                valueStr += ": ";
                if (d->mCellInfo->nwStatus == LOC_NW_ROAMING) {
                    valueStr += "ROAMING";
                } else if (d->mCellInfo->nwStatus == LOC_NW_HOME) {
                    valueStr += "ROAMING";
                } else {
                    valueStr += "OOO";
                }
            }
            if ((d->mCellInfo->valid_mask & LOC_RIL_CELLINFO_HAS_CELL_INFO) ==
                    LOC_RIL_CELLINFO_HAS_CELL_INFO) {
                if ((d->mCellInfo->valid_mask & LOC_RIL_CELLINFO_HAS_TECH_TYPE) ==
                        LOC_RIL_CELLINFO_HAS_TECH_TYPE) {

                        valueStr += ", ";
                        valueStr += RILCELLINFO_FIELD_RIL_TECH_TYPE;
                        valueStr += ": ";

                    switch (d->mCellInfo->rtType) {
                        case LOC_RIL_TECH_CDMA:
                        valueStr += "CDMA";
                        if ((d->mCellInfo->valid_mask & LOC_RIL_TECH_CDMA_HAS_MCC) ==
                                LOC_RIL_TECH_CDMA_HAS_MCC) {
                            valueStr += ", ";
                            valueStr += RILLCELLINFO_FIELD_MCC;
                            valueStr += ": ";
                            char t[20];
                            memset (t, '\0', 20);
                            snprintf (t, 20, "%d", d->mCellInfo->u.cdmaCinfo.mcc);
                            valueStr += t;
                        }
                        if ((d->mCellInfo->valid_mask & LOC_RIL_TECH_CDMA_HAS_SID) ==
                                LOC_RIL_TECH_CDMA_HAS_SID) {
                            valueStr += ", ";
                            valueStr += RILLCELLINFO_FIELD_SID;
                            valueStr += ": ";
                            char t[20];
                            memset (t, '\0', 20);
                            snprintf (t, 20, "%d", d->mCellInfo->u.cdmaCinfo.sid);
                            valueStr += t;
                        }
                        if ((d->mCellInfo->valid_mask & LOC_RIL_TECH_CDMA_HAS_NID) ==
                                LOC_RIL_TECH_CDMA_HAS_NID) {
                            valueStr += ", ";
                            valueStr += RILLCELLINFO_FIELD_NID;
                            valueStr += ": ";
                            char t[20];
                            memset (t, '\0', 20);
                            snprintf (t, 20, "%d", d->mCellInfo->u.cdmaCinfo.nid);
                            valueStr += t;
                        }
                        if ((d->mCellInfo->valid_mask & LOC_RIL_TECH_CDMA_HAS_BSID) ==
                                LOC_RIL_TECH_CDMA_HAS_BSID) {
                            valueStr += ", ";
                            valueStr += RILLCELLINFO_FIELD_BSID;
                            valueStr += ": ";
                            char t[20];
                            memset (t, '\0', 20);
                            snprintf (t, 20, "%d", d->mCellInfo->u.cdmaCinfo.bsid);
                            valueStr += t;
                        }
                        if ((d->mCellInfo->valid_mask & LOC_RIL_TECH_CDMA_HAS_BSLAT) ==
                                LOC_RIL_TECH_CDMA_HAS_BSLAT) {
                            valueStr += ", ";
                            valueStr += RILLCELLINFO_FIELD_BSLAT;
                            valueStr += ": ";
                            char t[20];
                            memset (t, '\0', 20);
                            snprintf (t, 20, "%d", d->mCellInfo->u.cdmaCinfo.bslat);
                            valueStr += t;
                        }
                        if ((d->mCellInfo->valid_mask & LOC_RIL_TECH_CDMA_HAS_BSLONG) ==
                                LOC_RIL_TECH_CDMA_HAS_BSLONG) {
                            valueStr += ", ";
                            valueStr += RILLCELLINFO_FIELD_BSLON;
                            valueStr += ": ";
                            char t[20];
                            memset (t, '\0', 20);
                            snprintf (t, 20, "%d", d->mCellInfo->u.cdmaCinfo.bslon);
                            valueStr += t;
                        }
                        if ((d->mCellInfo->valid_mask & LOC_RIL_TECH_CDMA_HAS_TIMEZONE) ==
                                LOC_RIL_TECH_CDMA_HAS_TIMEZONE) {
                            valueStr += ", ";
                            valueStr += RILLCELLINFO_FIELD_UTC_TIME_OFFSET;
                            valueStr += ": ";
                            char t[20];
                            memset (t, '\0', 20);
                            snprintf(t, 20, "%d",
                                    d->mCellInfo->u.cdmaCinfo.local_timezone_offset_from_utc);
                            valueStr += t;
                        }
                        if ((d->mCellInfo->valid_mask & LOC_RIL_TECH_CDMA_HAS_DAYLIGHT_SAVING) ==
                                LOC_RIL_TECH_CDMA_HAS_DAYLIGHT_SAVING) {
                            valueStr += ", ";
                            valueStr += RILLCELLINFO_FIELD_DAYLIGHT_TIMEZONE;
                            valueStr += ": ";
                            char t[20];
                            memset (t, '\0', 20);
                            snprintf(t, 20, "%d",
                                    d->mCellInfo->u.cdmaCinfo.local_timezone_on_daylight_savings);
                            valueStr += t;
                        }
                        break;
                        case LOC_RIL_TECH_GSM:
                        valueStr += "GSM";
                        if ((d->mCellInfo->valid_mask & LOC_RIL_TECH_GW_HAS_MCC) ==
                                LOC_RIL_TECH_GW_HAS_MCC) {
                            valueStr += ", ";
                            valueStr += RILLCELLINFO_FIELD_MCC;
                            valueStr += ": ";
                            char t[20];
                            memset (t, '\0', 20);
                            snprintf (t, 20, "%d", d->mCellInfo->u.gsmCinfo.mcc);
                            valueStr += t;
                        }
                        if ((d->mCellInfo->valid_mask & LOC_RIL_TECH_GW_HAS_MNC) ==
                                LOC_RIL_TECH_GW_HAS_MNC) {
                            valueStr += ", ";
                            valueStr += RILLCELLINFO_FIELD_MNC;
                            valueStr += ": ";
                            char t[20];
                            memset (t, '\0', 20);
                            snprintf (t, 20, "%d", d->mCellInfo->u.gsmCinfo.mnc);
                            valueStr += t;
                        }
                        if ((d->mCellInfo->valid_mask & LOC_RIL_TECH_GW_HAS_LAC) ==
                                LOC_RIL_TECH_GW_HAS_LAC) {
                            valueStr += ", ";
                            valueStr += RILLCELLINFO_FIELD_LAC;
                            valueStr += ": ";
                            char t[20];
                            memset (t, '\0', 20);
                            snprintf (t, 20, "%d", d->mCellInfo->u.gsmCinfo.lac);
                            valueStr += t;
                        }
                        if ((d->mCellInfo->valid_mask & LOC_RIL_TECH_GW_HAS_CID) ==
                                LOC_RIL_TECH_GW_HAS_CID) {
                            valueStr += ", ";
                            valueStr += RILLCELLINFO_FIELD_CID;
                            valueStr += ": ";
                            char t[20];
                            memset (t, '\0', 20);
                            snprintf (t, 20, "%d", d->mCellInfo->u.gsmCinfo.cid);
                            valueStr += t;
                        }
                        break;
                        case LOC_RIL_TECH_WCDMA:
                        valueStr += "WCDMA";
                        if ((d->mCellInfo->valid_mask & LOC_RIL_TECH_GW_HAS_MCC) ==
                                LOC_RIL_TECH_GW_HAS_MCC) {
                            valueStr += ", ";
                            valueStr += RILLCELLINFO_FIELD_MCC;
                            valueStr += ": ";
                            char t[20];
                            memset (t, '\0', 20);
                            snprintf (t, 20, "%d", d->mCellInfo->u.wcdmaCinfo.mcc);
                            valueStr += t;
                        }
                        if ((d->mCellInfo->valid_mask & LOC_RIL_TECH_GW_HAS_MNC) ==
                                LOC_RIL_TECH_GW_HAS_MNC) {
                            valueStr += ", ";
                            valueStr += RILLCELLINFO_FIELD_MNC;
                            valueStr += ": ";
                            char t[20];
                            memset (t, '\0', 20);
                            snprintf (t, 20, "%d", d->mCellInfo->u.wcdmaCinfo.mnc);
                            valueStr += t;
                        }
                        if ((d->mCellInfo->valid_mask & LOC_RIL_TECH_GW_HAS_LAC) ==
                                LOC_RIL_TECH_GW_HAS_LAC) {
                            valueStr += ", ";
                            valueStr += RILLCELLINFO_FIELD_LAC;
                            valueStr += ": ";
                            char t[20];
                            memset (t, '\0', 20);
                            snprintf (t, 20, "%d", d->mCellInfo->u.wcdmaCinfo.lac);
                            valueStr += t;
                        }
                        if ((d->mCellInfo->valid_mask & LOC_RIL_TECH_GW_HAS_CID) ==
                                LOC_RIL_TECH_GW_HAS_CID) {
                            valueStr += ", ";
                            valueStr += RILLCELLINFO_FIELD_CID;
                            valueStr += ": ";
                            char t[20];
                            memset (t, '\0', 20);
                            snprintf (t, 20, "%d", d->mCellInfo->u.wcdmaCinfo.cid);
                            valueStr += t;
                        }
                        if ((d->mCellInfo->valid_mask & LOC_RIL_TECH_WCDMA_HAS_FREQUENCY) ==
                                LOC_RIL_TECH_WCDMA_HAS_FREQUENCY) {
                            valueStr += ", ";
                            valueStr += RILLCELLINFO_FIELD_WCDMA_FCN;
                            valueStr += ": ";
                            char t[20];
                            memset (t, '\0', 20);
                            snprintf (t, 20, "%d", d->mCellInfo->u.wcdmaCinfo.frequency);
                            valueStr += t;
                        }
                        if ((d->mCellInfo->valid_mask & LOC_RIL_TECH_WCDMA_HAS_PRIMARY_SYNCHCODE) ==
                                LOC_RIL_TECH_WCDMA_HAS_PRIMARY_SYNCHCODE) {
                            valueStr += ", ";
                            valueStr += RILLCELLINFO_FIELD_PSC;
                            valueStr += ": ";
                            char t[20];
                            memset (t, '\0', 20);
                            snprintf (t, 20, "%d", d->mCellInfo->u.wcdmaCinfo.primarySynchcode);
                            valueStr += t;
                        }
                        break;
                        case LOC_RIL_TECH_LTE:
                        valueStr += "LTE";
                        if ((d->mCellInfo->valid_mask & LOC_RIL_TECH_LTE_HAS_MCC) ==
                                LOC_RIL_TECH_LTE_HAS_MCC) {
                            valueStr += ", ";
                            valueStr += RILLCELLINFO_FIELD_MCC;
                            valueStr += ": ";
                            char t[20];
                            memset (t, '\0', 20);
                            snprintf (t, 20, "%d", d->mCellInfo->u.lteCinfo.mcc);
                            valueStr += t;
                        }
                        if ((d->mCellInfo->valid_mask & LOC_RIL_TECH_LTE_HAS_MNC) ==
                                LOC_RIL_TECH_LTE_HAS_MNC) {
                            valueStr += ", ";
                            valueStr += RILLCELLINFO_FIELD_MNC;
                            valueStr += ": ";
                            char t[20];
                            memset (t, '\0', 20);
                            snprintf (t, 20, "%d", d->mCellInfo->u.lteCinfo.mnc);
                            valueStr += t;
                        }
                        if ((d->mCellInfo->valid_mask & LOC_RIL_TECH_LTE_HAS_TAC) ==
                                LOC_RIL_TECH_LTE_HAS_TAC) {
                            valueStr += ", ";
                            valueStr += RILLCELLINFO_FIELD_TAC;
                            valueStr += ": ";
                            char t[20];
                            memset (t, '\0', 20);
                            snprintf (t, 20, "%d", d->mCellInfo->u.lteCinfo.tac);
                            valueStr += t;
                        }
                        if ((d->mCellInfo->valid_mask & LOC_RIL_TECH_LTE_HAS_PCI) ==
                                LOC_RIL_TECH_LTE_HAS_PCI) {
                            valueStr += ", ";
                            valueStr += RILLCELLINFO_FIELD_PCI;
                            valueStr += ": ";
                            char t[20];
                            memset (t, '\0', 20);
                            snprintf (t, 20, "%d", d->mCellInfo->u.lteCinfo.pci);
                            valueStr += t;
                        }
                        if ((d->mCellInfo->valid_mask & LOC_RIL_TECH_LTE_HAS_CID) ==
                                LOC_RIL_TECH_LTE_HAS_CID) {
                            valueStr += ", ";
                            valueStr += RILLCELLINFO_FIELD_CID;
                            valueStr += ": ";
                            char t[20];
                            memset (t, '\0', 20);
                            snprintf (t, 20, "%d", d->mCellInfo->u.lteCinfo.cid);
                            valueStr += t;
                        }
                        if ((d->mCellInfo->valid_mask & LOC_RIL_TECH_LTE_HAS_EARFCN) ==
                                LOC_RIL_TECH_LTE_HAS_EARFCN) {
                            valueStr += ", ";
                            valueStr += RILLCELLINFO_FIELD_LTE_EARFCN;
                            valueStr += ": ";
                            char t[20];
                            memset (t, '\0', 20);
                            snprintf (t, 20, "%d", d->mCellInfo->u.lteCinfo.earfcn);
                            valueStr += t;
                        }
                        break;
                        case LOC_RIL_TECH_NR:
                        valueStr += "NR";
                        if ((d->mCellInfo->valid_mask & LOC_RIL_TECH_NR_HAS_MCC) ==
                                LOC_RIL_TECH_NR_HAS_MCC) {
                            valueStr += ", ";
                            valueStr += RILLCELLINFO_FIELD_MCC;
                            valueStr += ": ";
                            char t[20];
                            memset (t, '\0', 20);
                            snprintf (t, 20, "%d", d->mCellInfo->u.nrCinfo.mcc);
                            valueStr += t;
                        }
                        if ((d->mCellInfo->valid_mask & LOC_RIL_TECH_NR_HAS_MNC) ==
                                LOC_RIL_TECH_NR_HAS_MNC) {
                            valueStr += ", ";
                            valueStr += RILLCELLINFO_FIELD_MNC;
                            valueStr += ": ";
                            char t[20];
                            memset (t, '\0', 20);
                            snprintf (t, 20, "%d", d->mCellInfo->u.nrCinfo.mnc);
                            valueStr += t;
                        }
                        if ((d->mCellInfo->valid_mask & LOC_RIL_TECH_NR_HAS_TAC) ==
                                LOC_RIL_TECH_NR_HAS_TAC) {
                            valueStr += ", ";
                            valueStr += RILLCELLINFO_FIELD_TAC;
                            valueStr += ": ";
                            char t[20];
                            memset (t, '\0', 20);
                            snprintf (t, 20, "%d", d->mCellInfo->u.nrCinfo.tac);
                            valueStr += t;
                        }
                        if ((d->mCellInfo->valid_mask & LOC_RIL_TECH_NR_HAS_NCI) ==
                                LOC_RIL_TECH_NR_HAS_NCI) {
                            valueStr += ", ";
                            valueStr += RILLCELLINFO_FIELD_CID;
                            valueStr += ": ";
                            char t[20];
                            memset (t, '\0', 20);
                            snprintf (t, 20, "%" PRIu64, d->mCellInfo->u.nrCinfo.nci);
                            valueStr += t;
                        }
                        if ((d->mCellInfo->valid_mask & LOC_RIL_TECH_NR_HAS_PCI) ==
                                LOC_RIL_TECH_NR_HAS_PCI) {
                            valueStr += ", ";
                            valueStr += RILLCELLINFO_FIELD_PCI;
                            valueStr += ": ";
                            char t[20];
                            memset (t, '\0', 20);
                            snprintf (t, 20, "%d", d->mCellInfo->u.nrCinfo.pci);
                            valueStr += t;
                        }
                        if ((d->mCellInfo->valid_mask & LOC_RIL_TECH_NR_HAS_NRARFCN) ==
                                LOC_RIL_TECH_NR_HAS_NRARFCN) {
                            valueStr += ", ";
                            valueStr += RILLCELLINFO_FIELD_NRARFCN;
                            valueStr += ": ";
                            char t[20];
                            memset (t, '\0', 20);
                            snprintf (t, 20, "%d", d->mCellInfo->u.nrCinfo.nrarfcn);
                            valueStr += t;
                        }
                        break;
                        case LOC_RIL_TECH_TD_SCDMA:
                        case LOC_RIL_TECH_MAX:
                        default:
                        break;
                    }; // end switch
                }
            }
        }
    } while (0);
    EXIT_LOG_WITH_ERROR("%d", result);

}
bool RilCellInfoDataItemExt::operator==(const RilCellInfoDataItem& other) const {
    LOC_RilCellInfo* data = reinterpret_cast<LOC_RilCellInfo*>(other.mData);
    if (data == mCellInfo) return true;
    bool equal = (nullptr != data && nullptr != mCellInfo &&
                  data->valid_mask == mCellInfo->valid_mask &&
                  data->nwStatus == mCellInfo->nwStatus &&
                  data->rtType == mCellInfo->rtType);
    if (equal) {
        switch (data->rtType) {
        case LOC_RIL_TECH_CDMA:
            return (data->u.cdmaCinfo.mcc == mCellInfo->u.cdmaCinfo.mcc &&
                    data->u.cdmaCinfo.sid == mCellInfo->u.cdmaCinfo.sid &&
                    data->u.cdmaCinfo.nid == mCellInfo->u.cdmaCinfo.nid &&
                    data->u.cdmaCinfo.bsid == mCellInfo->u.cdmaCinfo.bsid &&
                    data->u.cdmaCinfo.bslat == mCellInfo->u.cdmaCinfo.bslat &&
                    data->u.cdmaCinfo.bslon == mCellInfo->u.cdmaCinfo.bslon &&
                    data->u.cdmaCinfo.local_timezone_offset_from_utc ==
                            mCellInfo->u.cdmaCinfo.local_timezone_offset_from_utc &&
                    data->u.cdmaCinfo.local_timezone_on_daylight_savings ==
                            mCellInfo->u.cdmaCinfo.local_timezone_on_daylight_savings);
        case LOC_RIL_TECH_GSM:
            return (data->u.gsmCinfo.mcc == mCellInfo->u.gsmCinfo.mcc &&
                    data->u.gsmCinfo.mnc == mCellInfo->u.gsmCinfo.mnc &&
                    data->u.gsmCinfo.lac == mCellInfo->u.gsmCinfo.lac &&
                    data->u.gsmCinfo.cid == mCellInfo->u.gsmCinfo.cid);
        case LOC_RIL_TECH_WCDMA:
            return (data->u.wcdmaCinfo.mcc == mCellInfo->u.wcdmaCinfo.mcc &&
                    data->u.wcdmaCinfo.mnc == mCellInfo->u.wcdmaCinfo.mnc &&
                    data->u.wcdmaCinfo.lac == mCellInfo->u.wcdmaCinfo.lac &&
                    data->u.wcdmaCinfo.cid == mCellInfo->u.wcdmaCinfo.cid);
        case LOC_RIL_TECH_LTE:
            return (data->u.lteCinfo.mcc == mCellInfo->u.lteCinfo.mcc &&
                    data->u.lteCinfo.mnc == mCellInfo->u.lteCinfo.mnc &&
                    data->u.lteCinfo.tac == mCellInfo->u.lteCinfo.tac &&
                    data->u.lteCinfo.pci == mCellInfo->u.lteCinfo.pci &&
                    data->u.lteCinfo.cid == mCellInfo->u.lteCinfo.cid);
        case LOC_RIL_TECH_NR:
            return (data->u.nrCinfo.mcc == mCellInfo->u.nrCinfo.mcc &&
                    data->u.nrCinfo.mnc == mCellInfo->u.nrCinfo.mnc &&
                    data->u.nrCinfo.tac == mCellInfo->u.nrCinfo.tac &&
                    data->u.nrCinfo.nci == mCellInfo->u.nrCinfo.nci);
        default:
            break;
        }
    }
    return equal;
}

void RilServiceInfoDataItemExt::setPeerData(RilServiceInfoDataItem& peer) const {
    if (nullptr == peer.mData) {
        peer.mData = malloc(sizeof(*mServiceInfo));
        if (NULL != peer.mData) {
            *(decltype(mServiceInfo))peer.mData = *mServiceInfo;
        } else {
            LOC_LOGe("malloc failed !");
        }
    }
}

void RilCellInfoDataItemExt::setPeerData(RilCellInfoDataItem& peer) const {
    if (nullptr == peer.mData) {
        peer.mData = malloc(sizeof(*mCellInfo));
        if (NULL != peer.mData) {
            *(decltype(mCellInfo))peer.mData = *mCellInfo;
        } else {
            LOC_LOGe("malloc failed !");
        }
    }
}
