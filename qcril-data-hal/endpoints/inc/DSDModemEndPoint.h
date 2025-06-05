/******************************************************************************
#  Copyright (c) 2018-2023 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#ifndef DSDMODEMENDPOINT
#define DSDMODEMENDPOINT
#include <vector>
#include "modules/qmi/ModemEndPoint.h"
#include "DSDModemEndPointModule.h"
#include "framework/Log.h"
#include "DataCommon.h"
#include "MessageCommon.h"

class DSDModemEndPoint : public ModemEndPoint
{
private:
  bool mReportLimitedSysIndicationChange;
  rildata::ScreenState_t mScreenState;
  bool mUiFilterRegistered;
  std::optional<bool> mDdsSwitchRecCapEnabled;
  static bool mUserDdsSwitchSelection;
  bool mCIWlanCapability;
  rildata::CIWlanCapabilityType mCIWlanCapType;
  bool mCIWlanUISelection;
  bool mUsingSystemStatusV2;
  bool mUsingUiInfoV2;
  bool mNrIconReportDisabled;
public:
  static constexpr const char *NAME = "DSDModemEndPoint";
  DSDModemEndPoint() : ModemEndPoint(NAME)
  {
    mModule = new DSDModemEndPointModule("DSDModemEndPointModule", *this);
    mModule->init();
    mReportLimitedSysIndicationChange = false;
    mUiFilterRegistered = false;
    mCIWlanCapability = false;
    mCIWlanCapType = rildata::CIWlanCapabilityType::None;
    mCIWlanUISelection = false;
    mUsingSystemStatusV2 = false;
    mUsingUiInfoV2 = false;
    mNrIconReportDisabled = false;
    mScreenState = rildata::ScreenState_t::SCREEN_ON;
    Log::getInstance().d("[DSDModemEndPoint]: xtor");
  }
  ~DSDModemEndPoint()
  {
    Log::getInstance().d("[DSDModemEndPoint]: destructor");
    //mModule->killLooper();
    delete mModule;
    mModule = nullptr;
  }
  void requestSetup(string clientToken, qcril_instance_id_e_type id, GenericCallback<string>* cb);
  /**
   * @brief      Sets the apn information synchronously
   *
   * @param[in]  apn_name       The apn name
   * @param[in]  apn_type_mask  The apn type mask
   *
   * @return     { description_of_the_return_value }
   */
  boolean isApnTypeFound(const rildata::ApnTypes_t &apn_type, const rildata::ApnTypes_t &apn_types_mask);

  void sendApnInfoQmi( const std::string &apn_name, dsd_apn_type_enum_v01 apn_type);

  Message::Callback::Status setApnInfoSync(const std::string apn_name,
   const rildata::ApnTypes_t apnTypesBitmap);

  Message::Callback::Status sendAPAssistIWLANSupported( );

  Message::Callback::Status registerForCurrentDDSInd( );

  Message::Callback::Status registerForSmartTempDDSInd( );

  Message::Callback::Status registerForRoamingStatusChanged( );

  Message::Callback::Status registerForAPAsstIWlanInd(bool toRegister );

  Message::Callback::Status registerForCIWLAN();

  RIL_Errno registerForDataDuringVoiceCall(bool enable);

  Message::Callback::Status setV2Capabilities( );

  Message::Callback::Status setApnPreferredSystemChangeSync(const std::string apnName, const int32_t prefRat);

  void generateDsdSystemStatusInd();

  Message::Callback::Status getCurrentDDSSync(DDSSubIdInfo &subId);

  Message::Callback::Status triggerDDSSwitch(int subId, int &error, dsd_dds_switch_type_enum_v01 switch_type);

  Message::Callback::Status getDataSystemCapabilitySync();

  Message::Callback::Status getMasterRILSpecificCapability();

  int reportSystemStatusChange(bool report_null_bearer, bool report);

  void handleSegmentTimeout(std::shared_ptr<Message> m);

  Message::Callback::Status handleScreenStateChangeInd(bool);

  Message::Callback::Status sendRegisterForUiChangeInd(uint8_t isRegister);

  void registerForUiChangeInd();

  void updateInitialUiInfo();
#ifdef RIL_FOR_MDM_LE
  bool uiChangeInfoRegistrationRequest(bool regValue);
#endif
#ifdef QMI_RIL_UTF
  void cleanup();
#endif

  RIL_Errno setIsDataRoamingEnabled( bool );

  RIL_Errno setIsDataEnabled( bool );

  RIL_Errno setCIWlanUIOptionSelection(bool enabled);

  bool getCIWlanUIOptionSelected() {return mCIWlanUISelection;}

  bool deviceShutdownRequest();

  qmi_response_type_v01 setQualityMeasurement(dsd_set_quality_measurement_info_req_msg_v01 info);

  std::optional<bool> getDdsSwitchRecCapEnabled() {return mDdsSwitchRecCapEnabled;}

  void setDdsSwitchRecCapEnabled(bool cap) {mDdsSwitchRecCapEnabled = cap;}

  void resetDdsSwitchRecCapEnabled() {mDdsSwitchRecCapEnabled.reset();mUserDdsSwitchSelection = false;}

  void setDsdCIWlanCapability(bool cap) {mCIWlanCapability = cap;}

  void setSystemStatusV2(bool ver) {mUsingSystemStatusV2 = ver;}

  void setUiInfoV2(bool uiInfo) {mUsingUiInfoV2 = uiInfo;}

  void setNrIconReportDisable(bool val) {mNrIconReportDisabled = val;}

  bool getDsdCIWlanCapability() {return mCIWlanCapability;}

  void setDsdCIWlanCapType(rildata::CIWlanCapabilityType type) {mCIWlanCapType = type;}

  rildata::CIWlanCapabilityType getDsdCIWlanCapType() {return mCIWlanCapType;}

  bool getUserDdsSwitchSelection() {return mUserDdsSwitchSelection;}

  bool getSystemStatusV2() {return mUsingSystemStatusV2;}

  bool getUiInfoV2() {return mUsingUiInfoV2;}

  bool getNrIconReportDisable() {return mNrIconReportDisabled;}
};
#endif
