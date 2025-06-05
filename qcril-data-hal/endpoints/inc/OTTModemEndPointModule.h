/******************************************************************************

  Copyright (c) 2019-2020 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

#******************************************************************************/
#ifdef FEATURE_DATA_LQE
#ifndef OTTMODEMENDPOINTMODULE
#define OTTMODEMENDPOINTMODULE
#include "over_the_top_v01.h"
#include "modules/qmi/ModemEndPoint.h"
#include "modules/qmi/ModemEndPointModule.h"
#include "MessageCommon.h"
#define QCRIL_DATA_QMI_TIMEOUT 10000

using namespace rildata;

class OTTModemEndPointModule : public ModemEndPointModule
{
private:

  qmi_idl_service_object_type getServiceObject() override;
  bool handleQmiBinding(qcril_instance_id_e_type instanceId, int8_t stackId) override;
  bool lqeInited;
  uint32_t downlinkCapacityKbps;
  uint32_t uplinkCapacityKbps;
  void handleLceInitMessage(std::shared_ptr<Message> msg);
  void handleToggleLqeIndicationSyncMessage (std::shared_ptr<Message> msg);
  void handleLceDeInitMessage(std::shared_ptr<Message> msg);
  void handleOTTQmiIndMessage(std::shared_ptr<Message> msg);
  void handleSetCapReportCriteriaSyncMessage(std::shared_ptr<Message> msg);
  void OTTUnsolicitedIndHdlr(unsigned int msg_id, unsigned char *decoded_payload);
  void OTTUnsolDownLCEHandler(unsigned char *decoded_payload);
  void OTTUnsolUpLCEHandler(unsigned char *decoded_payload);
  bool registerForIndication(LqeDataDirection direction);
public:
  OTTModemEndPointModule(string name, ModemEndPoint& owner);
  virtual ~OTTModemEndPointModule();
  void init();
};
#endif /* FEATURE_DATA_LQE */

#endif
