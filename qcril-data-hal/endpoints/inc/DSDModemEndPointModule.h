/******************************************************************************
#  Copyright (c) 2018-2024 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#ifndef DSDMODEMENDPOINTMODULE
#define DSDMODEMENDPOINTMODULE
#include "data_system_determination_v01.h"
#include "qmi_client.h"
#include "common_v01.h"
#include "modules/qmi/ModemEndPoint.h"
#include "modules/qmi/ModemEndPointModule.h"
#include "modules/qmi/QmiServiceUpIndMessage.h"
#include "modules/qmi/QmiSetupRequest.h"
#include "request/SetRatPrefMessage.h"
#include "request/SetNrUCIconConfigureMessage.h"

#include "SegmentTracker.h"

#define QCRIL_DATA_QMI_TIMEOUT 10000

class DSDModemEndPointModule : public ModemEndPointModule
{
private:
  qmi_idl_service_object_type getServiceObject() override;
  bool handleQmiBinding(qcril_instance_id_e_type instanceId, int8_t stackId) override;
  void handleQmiDsdIndMessage(std::shared_ptr<Message> msg);
  void handleGetDsdSystemStatus(std::shared_ptr<Message> msg);
  void handleSetRatPref(std::shared_ptr<Message> msg);
  void indicationHandler(unsigned int msg_id, unsigned char *decoded_payload, uint32_t decoded_payload_len);
  void processSystemStatusInd(dsd_system_status_ind_msg_v01 *ind_data);
  void processQmiDdsSwitchInd(dsd_switch_dds_ind_msg_v01 *ind);
  void processQmiCurrentDdsInd(dsd_current_dds_ind_msg_v01 *ind);
  void processRoamingStatusChangeInd(dsd_roaming_status_change_ind_msg_v01 *ind_data);
  void handleGetDsdSystemStatusV1(std::shared_ptr<Message> msg);
  void handleCurrentRoamingStatusMessage(std::shared_ptr<Message> msg);
  rildata::RatPreference homeRatPref;
  rildata::RatPreference roamingRatPref;
  bool ratPrefReceived;
  bool isRoaming;
#if (!defined(RIL_FOR_LOW_RAM) || defined(RIL_FOR_MDM_LE))
  void handleChangeStackIdDsdSyncMessage(std::shared_ptr<Message> msg);
  void handleSetVowifiConfiguration(std::shared_ptr<Message> msg);
#endif

  rildata::SegmentTracker segmentTracker;
  std::unordered_map<rildata::SegmentTracker::KeyType_t, std::shared_ptr<Message>> pendingSegmentRequests;
  void processDsdSystemStatusIndComplete(rildata::SegmentationStatus_t, uint16_t, std::vector<std::shared_ptr<Message>>);
  void processDsdSystemStatusResultComplete(rildata::SegmentationStatus_t, uint16_t, std::vector<std::shared_ptr<Message>>);
  void processDsdUiInfoIndComplete(rildata::SegmentationStatus_t, uint16_t, std::vector<std::shared_ptr<Message>>);
  void processDsdUiInfoResultComplete(rildata::SegmentationStatus_t, uint16_t, std::vector<std::shared_ptr<Message>>);
  void processSystemStatusInd(dsd_system_status_v2_ind_msg_v01 *ind_data);
  void processSystemStatusInd(dsd_get_system_status_v2_result_ind_msg_v01 *ind_data);
  void processPrefApnUpdatedInd(dsd_preferred_apn_updated_ind_msg_v01 *ind_data);
  void processUiInfoInd(dsd_ui_info_ind_msg_v01 *ind_data);
  void processUiInfoInd(dsd_get_ui_info_v2_result_ind_msg_v01 *ind_data);
  void processDdsRecommendationCapabilityInd(dsd_temp_dds_rec_capability_ind_v01 *ind_data);
  void processDdsRecommendationInd(dsd_dds_recommendation_ind_v01 *ind_data);
  void processDsdCIWlanCapabilityInd(dsd_ciwlan_capability_ind_v01 * ind_data);
  void convertBandInfo(rildata::NrUCIconBandInfo_t bandInfo, dsd_ui_icon_band_info_type_v01 & qmiBandInfo);
  dsd_ui_icon_mode_type_enum_v01 convertToQmiMode(rildata::NrUCIconMode_t mode);
  void handleSetNrUCIconConfigure(std::shared_ptr<Message> m);
  void handleSegmentTimeout(std::shared_ptr<Message> m);
  void handleGetDataNrIconType(std::shared_ptr<Message> msg);
  void getUiInfo(std::shared_ptr<Message> msg);
  void handleGetDsdSystemStatusV2(std::shared_ptr<Message> msg);
  void registerForUiChangeInd();
  void processIntentToChangeApnPrefSysInd(dsd_intent_to_change_apn_pref_sys_ind_msg_v01 *ind_data);
  void processApAsstApnPrefSysResultInd(dsd_ap_asst_apn_pref_sys_result_ind_msg_v01 *ind_data);
  bool sendNotifyDataSettings(rildata::RatPreference ratPref);

public:
  DSDModemEndPointModule(string name, ModemEndPoint& owner);
  virtual ~DSDModemEndPointModule();
  void init();
#ifdef QMI_RIL_UTF
  void cleanUpQmiSvcClient();
#endif
};

#endif
