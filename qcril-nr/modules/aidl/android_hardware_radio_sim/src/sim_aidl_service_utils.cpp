/******************************************************************************
#  Copyright (c) 2022 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#define TAG "RILQ"
#include "sim_aidl_service_utils.h"

aidlradio::RadioError convertGetIccCardStatusResponse(aidlsim::CardStatus& cardStatus,
                                                      std::shared_ptr<RIL_UIM_CardStatus> p_cur) {
  aidlradio::RadioError ret = aidlradio::RadioError::INVALID_RESPONSE;
  if (p_cur) {
    ret = static_cast<aidlradio::RadioError>(p_cur->err);
    if (p_cur->gsm_umts_subscription_app_index >= p_cur->num_applications ||
        p_cur->cdma_subscription_app_index >= p_cur->num_applications ||
        p_cur->ims_subscription_app_index >= p_cur->num_applications) {
      //      RLOGE("Invalid response");
      if (p_cur->err == RIL_UIM_E_SUCCESS) {
        ret = aidlradio::RadioError::INVALID_RESPONSE;
      }
    } else {
      cardStatus.cardState = static_cast<int>(p_cur->card_state);
      cardStatus.universalPinState = static_cast<aidlsim::PinState>(p_cur->universal_pin_state);
      cardStatus.gsmUmtsSubscriptionAppIndex = p_cur->gsm_umts_subscription_app_index;
      cardStatus.cdmaSubscriptionAppIndex = p_cur->cdma_subscription_app_index;
      cardStatus.imsSubscriptionAppIndex = p_cur->ims_subscription_app_index;

      cardStatus.atr            = p_cur->atr;
      cardStatus.iccid          = p_cur->iccid;

      if (!p_cur->eid.empty()) {
          cardStatus.eid = p_cur->eid;
      }

      cardStatus.slotMap.physicalSlotId          = p_cur->physical_slot_id;
      cardStatus.slotMap.portId =
          p_cur->port_id;

      cardStatus.supportedMepMode =
          static_cast<aidlconfig::MultipleEnabledProfilesMode>(
              p_cur->negotiated_mep_mode);
      QCRIL_LOG_DEBUG("IRadio Status_change_indication  %d",
                      cardStatus.slotMap.portId);
      RIL_UIM_AppStatus* rilAppStatus = p_cur->applications;
      cardStatus.applications.resize(p_cur->num_applications);
      aidlsim::AppStatus* appStatus = cardStatus.applications.data();

      for (int i = 0; i < p_cur->num_applications; i++) {
        appStatus[i].appType = static_cast<int>(rilAppStatus[i].app_type);  // FIXME
        appStatus[i].appState = static_cast<int>(rilAppStatus[i].app_state);
        appStatus[i].persoSubstate =
            static_cast<aidlsim::PersoSubstate>(rilAppStatus[i].perso_substate);
        appStatus[i].aidPtr = rilAppStatus[i].aid_ptr;
        appStatus[i].appLabelPtr = rilAppStatus[i].app_label_ptr;
        appStatus[i].pin1Replaced = rilAppStatus[i].pin1_replaced;
        appStatus[i].pin1 = static_cast<aidlsim::PinState>(rilAppStatus[i].pin1);
        appStatus[i].pin2 = static_cast<aidlsim::PinState>(rilAppStatus[i].pin2);
      }
    }
  }
  return ret;
}

qcril::interfaces::FacilityType convertFacilityType(std::string in) {
  if (in == "AO") {
    return qcril::interfaces::FacilityType::BAOC;
  } else if (in == "OI") {
    return qcril::interfaces::FacilityType::BAOIC;
  } else if (in == "OX") {
    return qcril::interfaces::FacilityType::BAOICxH;
  } else if (in == "AI") {
    return qcril::interfaces::FacilityType::BAIC;
  } else if (in == "IR") {
    return qcril::interfaces::FacilityType::BAICr;
  } else if (in == "AB") {
    return qcril::interfaces::FacilityType::BA_ALL;
  } else if (in == "AG") {
    return qcril::interfaces::FacilityType::BA_MO;
  } else if (in == "AC") {
    return qcril::interfaces::FacilityType::BA_MT;
  }
  return qcril::interfaces::FacilityType::UNKNOWN;
}

void convertRilPbCapacity(const std::shared_ptr<qcril::interfaces::qcril_pbm_adn_count_info>& in,
                          aidlsim::PhonebookCapacity& out) {
  out.maxAdnRecords = in->max_adn_num;
  out.usedAdnRecords = in->valid_adn_num;
  out.maxEmailRecords = in->max_email_num;
  out.usedEmailRecords = in->valid_email_num;
  out.maxAdditionalNumberRecords = in->max_ad_num;
  out.usedAdditionalNumberRecords = in->valid_ad_num;
  out.maxNameLen = in->max_name_len;
  out.maxNumberLen = in->max_number_len;
  out.maxEmailLen = in->max_email_len;
  out.maxAdditionalNumberLen = in->max_anr_len;
}

RIL_UIM_PersoSubstate convertHidlToRilPersoType(aidlsim::PersoSubstate persoType) {
  QCRIL_LOG_INFO("convertHidlToRilPersoType: persoType %d ", persoType);
  switch (persoType) {
    case aidlsim::PersoSubstate::SIM_SPN:
      return RIL_UIM_PERSOSUBSTATE_SIM_SPN;
    case aidlsim::PersoSubstate::SIM_SPN_PUK:
      return RIL_UIM_PERSOSUBSTATE_SIM_SPN_PUK;
    case aidlsim::PersoSubstate::SIM_SP_EHPLMN:
      return RIL_UIM_PERSOSUBSTATE_SIM_SP_EHPLMN;
    case aidlsim::PersoSubstate::SIM_SP_EHPLMN_PUK:
      return RIL_UIM_PERSOSUBSTATE_SIM_SP_EHPLMN_PUK;
    case aidlsim::PersoSubstate::SIM_ICCID:
      return RIL_UIM_PERSOSUBSTATE_SIM_ICCID;
    case aidlsim::PersoSubstate::SIM_ICCID_PUK:
      return RIL_UIM_PERSOSUBSTATE_SIM_ICCID_PUK;
    case aidlsim::PersoSubstate::SIM_IMPI:
      return RIL_UIM_PERSOSUBSTATE_SIM_IMPI;
    case aidlsim::PersoSubstate::SIM_IMPI_PUK:
      return RIL_UIM_PERSOSUBSTATE_SIM_IMPI_PUK;
    case aidlsim::PersoSubstate::SIM_NS_SP:
      return RIL_UIM_PERSOSUBSTATE_SIM_NS_SP;
    case aidlsim::PersoSubstate::SIM_NS_SP_PUK:
      return RIL_UIM_PERSOSUBSTATE_SIM_NS_SP_PUK;
    default:
      return (RIL_UIM_PersoSubstate)persoType;
      /* Only above values require conversion, others are 1:1 mapped */
  }
}

RIL_Errno convertHidlPhonebookRecords(const aidlsim::PhonebookRecordInfo& in,
                                 qcril::interfaces::AdnRecordInfo& out) {
  if (in.emails.size() > RIL_MAX_NUM_EMAIL_COUNT) {
     QCRIL_LOG_ERROR("MAX emails exceeded");
     return RIL_E_INVALID_ARGUMENTS;
  }

  if (in.additionalNumbers.size() > RIL_MAX_NUM_AD_COUNT) {
     QCRIL_LOG_ERROR("MAX additionalNumbers exceeded");
     return RIL_E_INVALID_ARGUMENTS;
  }

  uint16_t email_index, ad_index;
  out.record_id = in.recordId;
  out.name = in.name;
  out.number = in.number;
  out.email_elements = in.emails.size();
  for (email_index = 0; email_index < in.emails.size(); email_index++) {
    out.email[email_index] = in.emails[email_index];
  }
  out.anr_elements = in.additionalNumbers.size();
  for (ad_index = 0; ad_index < in.additionalNumbers.size(); ad_index++) {
    out.ad_number[ad_index] = in.additionalNumbers[ad_index];
  }
  return RIL_E_SUCCESS;
}

void convertRilPhonebookRecords(const qcril::interfaces::AdnRecords* in,
                                std::vector<aidlsim::PhonebookRecordInfo>& out) {
  uint16_t index, email_index, ad_index;
  out.resize(in->record_elements);
  for (index = 0; index < in->record_elements; index++) {
    const qcril::interfaces::AdnRecordInfo* adnRecordInfo = &in->adn_record_info[index];
    if (adnRecordInfo != nullptr) {
      out[index].recordId = adnRecordInfo->record_id;
      out[index].name = adnRecordInfo->name;
      out[index].number = adnRecordInfo->number;
      if (adnRecordInfo->email_elements > 0) {
        out[index].emails.resize(adnRecordInfo->email_elements);
        for (email_index = 0; email_index < adnRecordInfo->email_elements; email_index++) {
          out[index].emails[email_index] = adnRecordInfo->email[email_index];
        }
      }

      if (adnRecordInfo->anr_elements > 0) {
        out[index].additionalNumbers.resize(adnRecordInfo->anr_elements);
        for (ad_index = 0; ad_index < adnRecordInfo->anr_elements; ad_index++) {
          out[index].additionalNumbers[ad_index] = adnRecordInfo->ad_number[ad_index];
        }
      }
    }
  }
}

aidlsim::PbReceivedStatus convertRilPbReceivedStatus(int seq_num) {
  aidlsim::PbReceivedStatus status = aidlsim::PbReceivedStatus::PB_RECEIVED_ERROR;
  if (seq_num == 65535) {
    status = aidlsim::PbReceivedStatus::PB_RECEIVED_FINAL;
  } else if (seq_num > 0) {
    status = aidlsim::PbReceivedStatus::PB_RECEIVED_OK;
  }
  return status;
}
