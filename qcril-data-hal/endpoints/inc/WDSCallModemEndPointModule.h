/******************************************************************************
#  Copyright (c) 2018-2022 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#ifndef WDSCALLMODEMENDPOINTMODULE
#define WDSCALLMODEMENDPOINTMODULE
#include "qmi_client.h"
#include "common_v01.h"
#include <list>
#include "wireless_data_service_v01.h"
#include "MessageCommon.h"
#include "SegmentTracker.h"

#include "modules/qmi/ModemEndPoint.h"
#include "modules/qmi/ModemEndPointModule.h"
#include "UnSolMessages/InformPhysicalChannelConfigReportStatus.h"

using namespace rildata;

class WDSCallModemEndPointModule : public ModemEndPointModule
{
private:
  rildata::IpFamilyType_t mIpType;
  SegmentTracker segmentTracker;
  std::unordered_map<SegmentTracker::KeyType_t, std::shared_ptr<Message>> pendingSegmentRequests;
  bool mReportPhysicalChannelConfig;

  qmi_idl_service_object_type getServiceObject() override;
  bool handleQmiBinding(qcril_instance_id_e_type instanceId, int8_t stackId) override;
  void processExtendedIpConfigIndication(wds_req_settings_mask_v01 extMask);
  void handleWdsQmiIndMessage(std::shared_ptr<Message> msg);
  void handleEndPointStatusIndMessage(std::shared_ptr<Message> msg);
  void handleQmiServiceDown(std::shared_ptr<QmiServiceDownIndMessage> shared_srvDownMsg);
  void WdsUnsolicitedIndHdlr(unsigned int msg_id, unsigned char *decoded_payload, uint32_t decoded_payload_len);
  Message::Callback::Status queryAllDataBearerType(void);
  void handleSegmentTimeout(std::shared_ptr<Message> m);
  void processWdsRouteChangeIndComplete(rildata::SegmentationStatus_t, uint16_t,
                                        std::vector<std::shared_ptr<Message>>);
  void handlePhysicalChannelConfigReportStatus(std::shared_ptr<InformPhysicalChannelConfigReportStatus> msg);

public:
  WDSCallModemEndPointModule(string name, ModemEndPoint& owner, rildata::IpFamilyType_t iptype,
      message_id_ref qmi_id, message_id_ref ep_id, bool reportPhysicalChannelConfig);
  virtual ~WDSCallModemEndPointModule();
  void init();
  void releaseWDSCallEPModuleQMIClient();
  void queryNewPrimaryPduInfo();
};

#endif
