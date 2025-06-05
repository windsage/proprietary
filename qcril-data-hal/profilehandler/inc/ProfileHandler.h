/******************************************************************************
#  Copyright (c) 2018, 2020, 2022 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#ifndef PROFILEHANDLER
#define PROFILEHANDLER

#include "DSDModemEndPoint.h"
#include "WDSModemEndPoint.h"
#include "modules/android/version.h"
#include "telephony/ril.h"
#include "request/SetInitialAttachApnRequestMessage.h"
#include "request/SetDataProfileRequestMessage.h"
#include "MessageCommon.h"
#include "LocalLogBuffer.h"
#include "Util.h"
#include "framework/TimeKeeper.h"

#ifndef QCRIL_RIL_VERSION
#error "undefined QCRIL_RIL_VERSION!"
#endif

using namespace rildata;

class ProfileHandler
{
public:
  struct RilRespParams
  {
    qcril_instance_id_e_type instance_id;
    qcril_modem_id_e_type modem_id;
    qcril_evt_e_type event_id;
    RIL_Token t;
  };
private:
  LocalLogBuffer& logBuffer;
  qdp::Util& util;
  bool mApAssistMode;
  bool mUpdateAllowed;
  uint8_t mIARetry;
  std::shared_ptr<Message> mPendingMessage;
  TimeKeeper::timer_id mIARetryAttachTimer;
  static constexpr TimeKeeper::millisec RETRY_ATTACH_TIMEOUT = 500;
  std::shared_ptr<WDSModemEndPoint> wds_endpoint;
  std::shared_ptr<DSDModemEndPoint> dsd_endpoint;
  std::shared_ptr<std::list<uint16_t>> attach_list;
  DataProfileInfo_t requestProfile;
  std::shared_ptr<qdp::IProfileKey> attachProfileKey;
  std::shared_ptr<std::function<void(std::shared_ptr<Message>, RIL_Errno)>> informIACompletedCb;
  enum Request
  {
   SET_INITIAL_ATTACH,
   SET_DATA_PROFILE
  };

  /**
   * @brief      Is action supported as part of the attach procedure
   */
  enum AttachResult
  {
    NO_DETACH_NEEDED,
    DETACH_NEEDED,
    GENERIC_FAILURE
  };

  enum AttachResponse
  {
    SUCCESS,
    FAILURE,
    INPROGRESS
  };

  static RilRespParams m_resp_params;

  /**
   * @brief      Common implementation agnostic of the version of RIL
   *
   * @return     { description_of_the_return_value }
   */
  bool setAttachListCommon();
  /**
   * @brief      If current profile is in profiles found from attach list, then no
   *             attach required.
   *
   * @param[in]  attach_list  The attach list
   *
   * @return     True if attach required, False otherwise.
   *
   *  NOTE: This uses uint16_t based on the QMI response type
   */
  AttachResponse matchAttachProfile(const std::list<uint16_t>& attach_list);

  /**
   * @brief      Sets the attach list.
   *             Additionally, sets action if it is supported.
   *
   * @param[in]  attach_list  The attach list
   *
   * @return     SUPPORTED if action is supported by QMI service
   *             UNSUPPORTED if action is not supported by QMI service
   */
  AttachResult setAttachList();

  /**
   * @brief      Sends a request resp.
   *
   * @return     { description_of_the_return_value }
   */
  void sendSetInitialAttachResponse(RIL_Errno e);
  void initialAttachHandler();

public:
  void handleInitialAttachRequest(std::shared_ptr<Message> m);
  static void handleWdsUnSolInd(const wds_set_lte_attach_pdn_list_action_result_ind_msg_v01 *res);
  void releaseQdpAttachProfile(void);
  void compareModemAttachParams();
  boolean matchModemProfileParams
  (
  const wds_get_lte_attach_params_resp_msg_v01 *modem_attach_params,
  std::string ril_apn,
  std::string ril_ipfamily
  );
  void init(bool apAssistMode, bool updateAllowed);
  void dump(string padding, ostream& os);
  ProfileHandler(LocalLogBuffer& logBuffer, qdp::Util& util, std::shared_ptr<std::function<void(std::shared_ptr<Message>, RIL_Errno)>> cb);
  ~ProfileHandler();
  void setUpdateAllowed(bool updateAllowed);
  bool inited = false;
  void handleGetModemAttachParamsRetryMessage();
  void setDefaultProfileNumIfRequired(const std::vector<DataProfileInfo_t>& profileList);
  void getAttachParamsTimeoutHander(void *);
  void handleSetDataProfileRequestMessage(std::shared_ptr<Message> msg);
};

#endif
