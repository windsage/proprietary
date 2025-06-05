/******************************************************************************
#  Copyright (c) 2018-2023 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#ifndef WDSMODEMENDPOINT
#define WDSMODEMENDPOINT
#include "modules/qmi/ModemEndPoint.h"
#include "WDSModemEndPointModule.h"
#include "framework/Log.h"
#include "MessageCommon.h"
#include "CommonDefs.h"
#include "request/GetSlicingConfigRequestMessage.h"
#include "UnSolMessages/GetSlicingConfigInternalMessage.h"

class WDSModemEndPoint : public ModemEndPoint
{
private:
  enum AttachAction
  {
    ATTACH_PDN_ACTION_NOT_SUPPORTED =0,
    ATTACH_PDN_ACTION_SUPPORTED
  };
  bool mIsDataRegistered;
  bool mIsReportDataRegistrationRejectCause;
  bool mUsingPdnThrottleV2;

public:
  static constexpr const char *NAME = "WDSModemEndPoint";
  WDSModemEndPoint() : ModemEndPoint(NAME) {
    mModule = new WDSModemEndPointModule("WDSModemEndPointModule", *this);
    mModule->init();
    mIsDataRegistered = false;
    mIsReportDataRegistrationRejectCause = false;
    mUsingPdnThrottleV2 = false;
    Log::getInstance().d("[WDSModemEndPoint]: xtor");
  }
  ~WDSModemEndPoint() {
      Log::getInstance().d("[WDSModemEndPoint]: destructor");
    //mModule->killLooper();
    delete mModule;
    mModule = nullptr;
  }

  Message::Callback::Status getLteAttachParams(wds_get_lte_attach_params_resp_msg_v01& attach_param);
  void requestSetup(string clientToken, qcril_instance_id_e_type id, GenericCallback<string>* cb);
  /**
   * @brief      Gets the attach list.
   *
   * @param      attach_list  The attach list
   *
   * @return     Status
   */
  Message::Callback::Status getAttachList(
    std::shared_ptr<std::list<uint16_t>>& attach_list
  );

  /**
   * @brief      Set Attach list with desired action
   *
   * @param[in]  attach_list  The attach list
   * @param[in]  action       The action
   */
  Message::Callback::Status setAttachList(
    const std::shared_ptr<std::list<uint16_t>>& attach_list,
    const rildata::AttachListAction action);

  Message::Callback::Status getAttachListCapability(
    rildata::AttachListCap& cap);

  Message::Callback::Status getCallBringUpCapability(
  rildata::BringUpCapability& callBringUpCapability
  );

  Message::Callback::Status setDefaultProfileNum(qdp::TechType techType, qdp::ProfileId index);

  bool getReportingStatus();

  Message::Callback::Status setV2Capabilities(bool nswo);

  void getPdnThrottleTime(std::string apn, DataProfileInfoType_t techType, std::string ipType, int cid);

  bool getDataRegistrationState();

  void registerforWdsIndication();

  void updateDataRegistrationState(bool registered);

  Message::Callback::Status registerDataRegistrationRejectCause(bool enable);
  
  Message::Callback::Status getPduSessionParamLookup(uint16_t txId, ApnTypes_t apnType,
                                                     std::optional<TrafficDescriptor_t> td,
                                                     bool matchAllRuleAllowed = true);

  void getSlicingConfigRequest(std::shared_ptr<GetSlicingConfigRequestMessage>);

  void setPdnThrottleV2(bool val) {mUsingPdnThrottleV2 = val;}

  bool getPdnThrottleV2() {return mUsingPdnThrottleV2;}
};

#endif
