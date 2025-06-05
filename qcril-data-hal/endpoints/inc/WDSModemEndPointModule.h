/******************************************************************************
#  Copyright (c) 2018-2023 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#ifndef WDSMODEMENDPOINTMODULE
#define WDSMODEMENDPOINTMODULE
#include "wireless_data_service_v01.h"
#include "qmi_client.h"
#include "common_v01.h"
#include <list>
#include "modules/qmi/ModemEndPoint.h"
#include "modules/qmi/ModemEndPointModule.h"
#include "modules/qmi/QmiServiceUpIndMessage.h"
#include "modules/qmi/QmiSetupRequest.h"
#include "UnSolMessages/DataRegistrationFailureCauseMessage.h"
#include "UnSolMessages/WdsThrottleInfoFragmentMessage.h"
#include "MessageCommon.h"
#include "SegmentTracker.h"

#ifndef QCRIL_DATA_QMI_TIMEOUT
#define QCRIL_DATA_QMI_TIMEOUT 10000
#endif

using namespace rildata;

class WDSModemEndPointModule : public ModemEndPointModule
{
private:
  qmi_idl_service_object_type getServiceObject() override;
  bool handleQmiBinding(qcril_instance_id_e_type instanceId,
                        int8_t stackId) override;
  void handleGetAttachListCap(std::shared_ptr<Message> msg);
  void handleGetCallBringUpCapabilitySync(std::shared_ptr<Message> msg);
  void handleSetCapabilities(std::shared_ptr<Message> msg);
  void handleGetPdnThrottleTime(std::shared_ptr<Message> msg);
  int64_t getPdnThrottleTime(std::list<WdsThrottleInfo> throttleInfo, std::shared_ptr<Message> m);
  void handleWdsQmiIndMessage(std::shared_ptr<Message> msg);
  void handleChangeStackIdWdsSyncMessage(std::shared_ptr<Message> msg);
  void processGetPdnThrottleInfoResultComplete(rildata::SegmentationStatus_t, uint16_t, std::vector<std::shared_ptr<Message>>);
  void processPdnThrottleInfoIndComplete(rildata::SegmentationStatus_t, uint16_t, std::vector<std::shared_ptr<Message>>);
  void processTdInfoIndComplete(SegmentationStatus_t status, uint16_t txId, std::vector<std::shared_ptr<Message>> segments);
  void handleGetSlicingConfigInternalMessage(std::shared_ptr<Message> m);
  void handleSegmentTimeout(std::shared_ptr<Message> m);
  void WdsUnsolicitedIndHdlr(unsigned int   msg_id,
  unsigned char *decoded_payload,
  uint32_t       decoded_payload_len);
  bool isDefaultProfile
  (
    int32_t supportedApnTypesBitmap
  );
  uint16_t allocateTxId() {return (mTxId++ % 0x7FFF);}

  PLMN_t convertPlmn(uint8_t plmn_id[]);

  #ifdef SET_LOCAL_URSP_CONFIG
  void handleSetLocalUrspConfigurationRequestMessage(std::shared_ptr<Message> m);
  #endif

  SegmentTracker segmentTracker;
  std::unordered_map<SegmentTracker::KeyType_t, std::shared_ptr<Message>> pendingSegmentRequests;
  uint16_t mTxId;
  std::unordered_map<uint32_t, std::shared_ptr<Message>> getSlicingConfigTxId;

public:
  WDSModemEndPointModule(string name, ModemEndPoint& owner);
  virtual ~WDSModemEndPointModule();
  void init();
  static wds_apn_type_mask_v01 convertToApnTypeMask
  (
    int32_t supportedApnTypesBitmap
  );
  static vector<TrafficDescriptor_t> constructTDsFromFragments(std::vector<std::shared_ptr<Message>> fragments);
};

#endif
