/******************************************************************************
#  Copyright (c) 2018, 2020-2022 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#ifndef WDSCALLMODEMENDPOINT
#define WDSCALLMODEMENDPOINT
#include <string>
#include "modules/qmi/ModemEndPoint.h"
#include "WDSCallModemEndPointModule.h"
#include "framework/Log.h"

class WDSCallModemEndPoint : public ModemEndPoint
{
private:
  std::string mModuleName;
  rildata::IpFamilyType_t mIpType;
  int mEpType;
  int mEpId;
  int mMuxId;
  int mCid;
  std::string mApnName;
  std::string mIfaceName;
  bool mReportLinkStateChange;
  uint32_t mSecPduId;

public:
  WDSCallModemEndPoint(std::string moduleName, int cid, std::string apnName,
    std::string ifaceName, rildata::IpFamilyType_t iptype, bool reportPhysicalChannelConfig) : ModemEndPoint(moduleName) {
    mModuleName = moduleName;
    mCid = cid;
    mApnName = apnName;
    mIfaceName = ifaceName;
    mIpType = iptype;
    std::string qmiInd = moduleName+"_QMI_IND";
    std::string endpointStatusInd = moduleName+"_ENDPOINT_STATUS_IND";
    //Query the Dispatcher for existing ID
    message_id_info* qmi_ind_id = REG_MSG_Q(qmiInd);
    message_id_info* ep_ind_id = REG_MSG_Q(endpointStatusInd);

    // Create ID and register for Message
    if(nullptr == qmi_ind_id){
        qmi_ind_id = new message_id_info(qmiInd);
    }
    if(nullptr == ep_ind_id){
        ep_ind_id = new message_id_info(endpointStatusInd);
    }
    if((nullptr != qmi_ind_id) && (nullptr != ep_ind_id)) {
        mModule = new WDSCallModemEndPointModule(mModuleName, *this, mIpType, REG_MSG_N(qmiInd, *qmi_ind_id), REG_MSG_N(endpointStatusInd, *ep_ind_id), reportPhysicalChannelConfig);
        mModule->init();
    }
    Log::getInstance().d("["+mModuleName+"]: xtor with name " + mModuleName);
  }
  ~WDSCallModemEndPoint() {
    Log::getInstance().d("["+mModuleName+"]: destructor");
    delete mModule;
    mModule = nullptr;
  }
  void requestSetup(string clientToken, qcril_instance_id_e_type id, GenericCallback<string>* cb);
  rildata::IpFamilyType_t getIpType(void){return mIpType;};

  int getCid(void){return mCid;};

  std::string getApnName(void){return mApnName;};

  std::string getIfaceName(void){return mIfaceName;};

  void setBindMuxDataPortParams(int eptype, int epid, int muxid);

  void getBindMuxDataPortParams(int *eptype, int *epid, int *muxid);

  void releaseWDSCallEPModuleQMIClient();

  void setLinkStateChangeReport(bool report, uint32_t secPduId);

  void updateLinkStateChangeReport(void);

  void queryNewPrimaryPduInfo(void);

  bool goDormant(uint32_t pduId);
  bool setDataInActivityPeriod(uint32_t setDataInActivityPeriod);
};

#endif
