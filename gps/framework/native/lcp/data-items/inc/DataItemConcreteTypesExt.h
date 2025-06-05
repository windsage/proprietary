/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

GENERAL DESCRIPTION
  DataItemConcreteTypesExt

  Copyright (c) 2020 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/

#ifndef IZAT_MANAGER_DATAITEMCONCRETETYPESEXT_H
#define IZAT_MANAGER_DATAITEMCONCRETETYPESEXT_H

#include <comdef.h>
#include <string>
#include <loc_ril.h>
#include <DataItemConcreteTypeDefaultValues.h>
#include <DataItemConcreteTypeFieldNames.h>
#include <DataItemConcreteTypes.h>

namespace izat_manager {

using namespace std;
using namespace loc_core;

class RilServiceInfoDataItemExt : public RilServiceInfoDataItem {
public:
    RilServiceInfoDataItemExt(struct LOC_RilServiceInfo * serviceInfo = NULL);
    virtual ~RilServiceInfoDataItemExt();
    virtual void stringify (string & valueStr);
    virtual int32_t copyFrom(IDataItemCore * src) override;
    virtual bool operator==(const RilServiceInfoDataItem& other) const override;
    virtual void setPeerData(RilServiceInfoDataItem& peer) const override;
// Data members
    struct LOC_RilServiceInfo * mServiceInfo;
protected:
    string mCardName;
private:
    bool mDestroy;
};

class RilCellInfoDataItemExt : public RilCellInfoDataItem {
public:
    RilCellInfoDataItemExt(struct LOC_RilCellInfo * cellInfo = NULL);
    virtual ~RilCellInfoDataItemExt();
    virtual void stringify (string & valueStr);
    virtual int32_t copyFrom(IDataItemCore * src) override;
    virtual bool operator==(const RilCellInfoDataItem& other) const override;
    virtual void setPeerData(RilCellInfoDataItem& peer) const override;
// Data members
    struct LOC_RilCellInfo * mCellInfo;
protected:
    string mCardName;
private:
    bool mDestroy;
};

} // namespace izat_manager

#endif // #ifndef IZAT_MANAGER_DATAITEMCONCRETETYPESEXT_H
