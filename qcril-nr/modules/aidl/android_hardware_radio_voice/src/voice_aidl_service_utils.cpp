/*
 * Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
 * All rights reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 * Not a Contribution.
 */
/*
 * Copyright (c) 2016 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#define TAG "RILQ"

#include "cutils/properties.h"

#include "aidl/android/hardware/radio/voice/EmergencyServiceCategory.h"
#include "voice_aidl_service_utils.h"
#include "qcril_config.h"

#define QCRIL_USSD_IMS_RUNTIME_SUPPORT "vendor.radio.ussd_support_at_runtime"

void convertRilCdmaSignalInfoRecord(aidlvoice::CdmaSignalInfoRecord& out,
    std::shared_ptr<RIL_CDMA_SignalInfoRecord> in) {
  if (in) {
    out.isPresent = in->isPresent;
    out.signalType = in->signalType;
    out.alertPitch = in->alertPitch;
    out.signal = in->signal;
  }
}

bool makeCdmaCallWaiting(aidlvoice::CdmaCallWaiting& callWaitingRecord,
    std::shared_ptr<QcRilUnsolCdmaCallWaitingMessage> msg) {
  if (msg) {
    if (msg->hasNumber() && !msg->getNumber().empty()) {
      callWaitingRecord.number = msg->getNumber();
    }
    if (msg->hasNumberPresentation()) {
      callWaitingRecord.numberPresentation =
          static_cast<int>(msg->getNumberPresentation());
    }
    if (msg->hasName() && !msg->getName().empty()) {
      callWaitingRecord.name = msg->getName();
    }
    if (msg->hasNumberType()) {
      callWaitingRecord.numberType =
          static_cast<int>(msg->getNumberType());
    }
    if (msg->hasNumberPlan()) {
      callWaitingRecord.numberPlan =
          static_cast<int>(msg->getNumberPlan());
    }
    if (msg->hasSignalInfoRecord() && msg->getSignalInfoRecord() != nullptr) {
      convertRilCdmaSignalInfoRecordToHal(callWaitingRecord.signalInfoRecord,
          msg->getSignalInfoRecord());
    }
    return true;
  }
  return false;
}

void convertRilCdmaSignalInfoRecordToHal(aidlvoice::CdmaSignalInfoRecord& out,
    std::shared_ptr<RIL_CDMA_SignalInfoRecord> in) {
  if (in) {
    out.isPresent = in->isPresent;
    out.signalType = in->signalType;
    out.alertPitch = in->alertPitch;
    out.signal = in->signal;
  }
}

bool makeCdmaInformationRecords(vector<aidlvoice::CdmaInformationRecord>& records,
    std::shared_ptr<QcRilUnsolCdmaInfoRecordMessage> msg) {
  if (msg && msg->hasCdmaInfoRecords() && msg->getCdmaInfoRecords() != nullptr) {
    std::shared_ptr<RIL_CDMA_InformationRecords> recordsRil = msg->getCdmaInfoRecords();

    char* string8 = NULL;
    int num = MIN(recordsRil->numberOfInfoRecs, RIL_CDMA_MAX_NUMBER_OF_INFO_RECS);
#if 0
    if (recordsRil->numberOfInfoRecs > RIL_CDMA_MAX_NUMBER_OF_INFO_RECS) {
      RLOGE(
          "cdmaInfoRecInd: received %d recs which is more than %d, dropping "
          "additional ones",
          recordsRil->numberOfInfoRecs, RIL_CDMA_MAX_NUMBER_OF_INFO_RECS);
    }
#endif
    records.resize(num);
    for (int i = 0; i < num; i++) {
      aidlvoice::CdmaInformationRecord* record = &records[i];
      RIL_CDMA_InformationRecord* infoRec = &recordsRil->infoRec[i];
      record->name = static_cast<int>(infoRec->name);
      // All vectors should be size 0 except one which will be size 1. Set everything to
      // size 0 initially.
      record->display.resize(0);
      record->number.resize(0);
      record->signal.resize(0);
      record->redir.resize(0);
      record->lineCtrl.resize(0);
      record->clir.resize(0);
      record->audioCtrl.resize(0);
      switch (infoRec->name) {
        case RIL_CDMA_DISPLAY_INFO_REC:
        case RIL_CDMA_EXTENDED_DISPLAY_INFO_REC: {
          if (infoRec->rec.display.alpha_len > CDMA_ALPHA_INFO_BUFFER_LENGTH) {
#if 0
            RLOGE(
                "cdmaInfoRecInd: invalid display info response length %d "
                "expected not more than %d",
                (int)infoRec->rec.display.alpha_len, CDMA_ALPHA_INFO_BUFFER_LENGTH);
#endif
            return 0;
          }
          string8 = (char*)malloc((infoRec->rec.display.alpha_len + 1) * sizeof(char));
          if (string8 == NULL) {
#if 0
            RLOGE(
                "cdmaInfoRecInd: Memory allocation failed for "
                "responseCdmaInformationRecords");
#endif
            return 0;
          }
          memcpy(string8, infoRec->rec.display.alpha_buf, infoRec->rec.display.alpha_len);
          string8[(int)infoRec->rec.display.alpha_len] = '\0';

          record->display.resize(1);
          record->display[0].alphaBuf = string8;
          free(string8);
          string8 = NULL;
          break;
        }

        case RIL_CDMA_CALLED_PARTY_NUMBER_INFO_REC:
        case RIL_CDMA_CALLING_PARTY_NUMBER_INFO_REC:
        case RIL_CDMA_CONNECTED_NUMBER_INFO_REC: {
          if (infoRec->rec.number.len > CDMA_NUMBER_INFO_BUFFER_LENGTH) {
#if 0
            RLOGE(
                "cdmaInfoRecInd: invalid display info response length %d "
                "expected not more than %d",
                (int)infoRec->rec.number.len, CDMA_NUMBER_INFO_BUFFER_LENGTH);
#endif
            return 0;
          }
          string8 = (char*)malloc((infoRec->rec.number.len + 1) * sizeof(char));
          if (string8 == NULL) {
#if 0
            RLOGE(
                "cdmaInfoRecInd: Memory allocation failed for "
                "responseCdmaInformationRecords");
#endif
            return 0;
          }
          memcpy(string8, infoRec->rec.number.buf, infoRec->rec.number.len);
          string8[(int)infoRec->rec.number.len] = '\0';

          record->number.resize(1);
          record->number[0].number = string8;
          free(string8);
          string8 = NULL;
          record->number[0].numberType = infoRec->rec.number.number_type;
          record->number[0].numberPlan = infoRec->rec.number.number_plan;
          record->number[0].pi = infoRec->rec.number.pi;
          record->number[0].si = infoRec->rec.number.si;
          break;
        }

        case RIL_CDMA_SIGNAL_INFO_REC: {
          record->signal.resize(1);
          record->signal[0].isPresent = infoRec->rec.signal.isPresent;
          record->signal[0].signalType = infoRec->rec.signal.signalType;
          record->signal[0].alertPitch = infoRec->rec.signal.alertPitch;
          record->signal[0].signal = infoRec->rec.signal.signal;
          break;
        }

        case RIL_CDMA_REDIRECTING_NUMBER_INFO_REC: {
          if (infoRec->rec.redir.redirectingNumber.len > CDMA_NUMBER_INFO_BUFFER_LENGTH) {
#if 0
            RLOGE(
                "cdmaInfoRecInd: invalid display info response length %d "
                "expected not more than %d\n",
                (int)infoRec->rec.redir.redirectingNumber.len, CDMA_NUMBER_INFO_BUFFER_LENGTH);
#endif
            return 0;
          }
          string8 = (char*)malloc((infoRec->rec.redir.redirectingNumber.len + 1) * sizeof(char));
          if (string8 == NULL) {
#if 0
            RLOGE(
                "cdmaInfoRecInd: Memory allocation failed for "
                "responseCdmaInformationRecords");
#endif
            return 0;
          }
          memcpy(string8, infoRec->rec.redir.redirectingNumber.buf,
                 infoRec->rec.redir.redirectingNumber.len);
          string8[(int)infoRec->rec.redir.redirectingNumber.len] = '\0';

          record->redir.resize(1);
          record->redir[0].redirectingNumber.number = string8;
          free(string8);
          string8 = NULL;
          record->redir[0].redirectingNumber.numberType =
              infoRec->rec.redir.redirectingNumber.number_type;
          record->redir[0].redirectingNumber.numberPlan =
              infoRec->rec.redir.redirectingNumber.number_plan;
          record->redir[0].redirectingNumber.pi = infoRec->rec.redir.redirectingNumber.pi;
          record->redir[0].redirectingNumber.si = infoRec->rec.redir.redirectingNumber.si;
          record->redir[0].redirectingReason =
              static_cast<int>(infoRec->rec.redir.redirectingReason);
          break;
        }

        case RIL_CDMA_LINE_CONTROL_INFO_REC: {
          record->lineCtrl.resize(1);
          record->lineCtrl[0].lineCtrlPolarityIncluded =
              infoRec->rec.lineCtrl.lineCtrlPolarityIncluded;
          record->lineCtrl[0].lineCtrlToggle = infoRec->rec.lineCtrl.lineCtrlToggle;
          record->lineCtrl[0].lineCtrlReverse = infoRec->rec.lineCtrl.lineCtrlReverse;
          record->lineCtrl[0].lineCtrlPowerDenial = infoRec->rec.lineCtrl.lineCtrlPowerDenial;
          break;
        }

        case RIL_CDMA_T53_CLIR_INFO_REC: {
          record->clir.resize(1);
          record->clir[0].cause = infoRec->rec.clir.cause;
          break;
        }

        case RIL_CDMA_T53_AUDIO_CONTROL_INFO_REC: {
          record->audioCtrl.resize(1);
          record->audioCtrl[0].upLink = infoRec->rec.audioCtrl.upLink;
          record->audioCtrl[0].downLink = infoRec->rec.audioCtrl.downLink;
          break;
        }

        case RIL_CDMA_T53_RELEASE_INFO_REC:
          // RLOGE("cdmaInfoRecInd: RIL_CDMA_T53_RELEASE_INFO_REC: INVALID");
          return 0;

        default:
          // RLOGE("cdmaInfoRecInd: Incorrect name value");
          return 0;
      }
    }
    return true;
  }
  return false;
}

int convertRilEccListToAidl(const qcril::interfaces::RIL_EccList& ril_ecc_list,
    vector<aidlvoice::EmergencyNumber>& ecc_list) {
  ecc_list.resize(ril_ecc_list.size());
  std::ostringstream logString{};
  for (size_t i = 0; i < ril_ecc_list.size(); i++) {
    ecc_list[i].number = ril_ecc_list[i]->number;
    logString << "number - " << ecc_list[i].number << " | ";
    ecc_list[i].mcc = ril_ecc_list[i]->mcc;
    logString << "mcc - " << ecc_list[i].mcc << " | ";
    ecc_list[i].mnc = ril_ecc_list[i]->mnc;
    logString << "mnc - " << ecc_list[i].mnc << " | ";
    ecc_list[i].categories = toSaidlCategories(static_cast<uint32_t>(ril_ecc_list[i]->categories));
    logString << "categories - " << ecc_list[i].categories << " | ";
    ecc_list[i].sources = toSaidlSources(static_cast<uint32_t>(ril_ecc_list[i]->sources));
    logString << "sources - " << ecc_list[i].sources << " | ";
    ecc_list[i].urns.resize(ril_ecc_list[i]->urns.size());
    logString << "urn - [";
    for (size_t j = 0; j < ril_ecc_list[i]->urns.size(); j++) {
      ecc_list[i].urns[j] = ril_ecc_list[i]->urns[j];
      logString << ecc_list[i].urns[j] << ",";
    }
    logString << "]";
    QCRIL_LOG_INFO("%s", logString.str().c_str());
    logString.str("");
  }
  return 0;
}

uint32_t toSaidlCategories(uint32_t categories) {
  uint32_t cat = (uint32_t)aidlvoice::EmergencyServiceCategory::UNSPECIFIED;
  if (categories & (uint32_t)qcril::interfaces::RIL_EccCategoryMask::POLICE) {
    cat |= (uint32_t)aidlvoice::EmergencyServiceCategory::POLICE;
  }
  if (categories & (uint32_t)qcril::interfaces::RIL_EccCategoryMask::AMBULANCE) {
    cat |= (uint32_t)aidlvoice::EmergencyServiceCategory::AMBULANCE;
  }
  if (categories & (uint32_t)qcril::interfaces::RIL_EccCategoryMask::FIRE_BRIGADE) {
    cat |= (uint32_t)aidlvoice::EmergencyServiceCategory::FIRE_BRIGADE;
  }
  if (categories & (uint32_t)qcril::interfaces::RIL_EccCategoryMask::MARINE_GUARD) {
    cat |= (uint32_t)aidlvoice::EmergencyServiceCategory::MARINE_GUARD;
  }
  if (categories & (uint32_t)qcril::interfaces::RIL_EccCategoryMask::MOUNTAIN_RESCUE) {
    cat |= (uint32_t)aidlvoice::EmergencyServiceCategory::MOUNTAIN_RESCUE;
  }
  if (categories & (uint32_t)qcril::interfaces::RIL_EccCategoryMask::MANUAL_ECALL) {
    cat |= (uint32_t)aidlvoice::EmergencyServiceCategory::MIEC;
  }
  if (categories & (uint32_t)qcril::interfaces::RIL_EccCategoryMask::AUTO_ECALL) {
    cat |= (uint32_t)aidlvoice::EmergencyServiceCategory::AIEC;
  }
  return cat;
}

uint32_t toSaidlSources(uint32_t sources) {
  uint32_t ecc_source = 0;
  if (sources & (uint32_t)qcril::interfaces::RIL_EccNumberSourceMask::NETWORK_SIGNALING) {
    ecc_source |= (uint32_t)aidlvoice::EmergencyNumber::SOURCE_NETWORK_SIGNALING ;
  }
  if (sources & (uint32_t)qcril::interfaces::RIL_EccNumberSourceMask::SIM) {
    ecc_source |= (uint32_t)aidlvoice::EmergencyNumber::SOURCE_SIM;
  }
  if (sources & (uint32_t)qcril::interfaces::RIL_EccNumberSourceMask::MODEM_CONFIG) {
    ecc_source |= (uint32_t)aidlvoice::EmergencyNumber::SOURCE_MODEM_CONFIG;
  }
  if (sources & (uint32_t)qcril::interfaces::RIL_EccNumberSourceMask::DEFAULT) {
    ecc_source |= (uint32_t)aidlvoice::EmergencyNumber::SOURCE_DEFAULT;
  }

  // If there is no ecc source, set it to modem config as this probably from RIL DB.
  if (ecc_source == 0) {
    ecc_source |= (uint32_t)aidlvoice::EmergencyNumber::SOURCE_MODEM_CONFIG;
  }
  return ecc_source;
}

bool makeStkCcUnsolSsResult(aidlvoice::StkCcUnsolSsResult& ss,
                            std::shared_ptr<QcRilUnsolSupplementaryServiceMessage> msg) {
  if (msg) {
    if (msg->hasServiceType()) {
      ss.serviceType = static_cast<int>(msg->getServiceType());
    }
    if (msg->hasRequestType()) {
      ss.requestType = static_cast<int>(msg->getRequestType());
    }
    if (msg->hasTeleserviceType()) {
      ss.teleserviceType = static_cast<int>(msg->getTeleserviceType());
    }
    if (msg->hasServiceClass()) {
      ss.serviceClass = msg->getServiceClass();
    }
    if (msg->hasResult()) {
      ss.result = static_cast<aidl::android::hardware::radio::RadioError>(msg->getResult());
    }
    if (isServiceTypeCf(msg->getServiceType(), msg->getRequestType()) &&
        msg->hasCallForwardInfoList() && !msg->getCallForwardInfoList().empty()) {
      std::vector<qcril::interfaces::CallForwardInfo> rilCfInfo = msg->getCallForwardInfoList();
      ss.cfData.resize(1);
      ss.ssInfo.resize(0);

      ss.cfData[0].cfInfo.resize(rilCfInfo.size());
      for (int i = 0; i < rilCfInfo.size() && i < NUM_SERVICE_CLASSES; i++) {
        qcril::interfaces::CallForwardInfo cf = rilCfInfo[i];
        aidlvoice::CallForwardInfo* cfInfo = &ss.cfData[0].cfInfo[i];
        if (cf.hasStatus()) {
          cfInfo->status = static_cast<int>(cf.getStatus());
        }
        if (cf.hasReason()) {
          cfInfo->reason = cf.getReason();
        }
        if (cf.hasServiceClass()) {
          cfInfo->serviceClass = cf.getServiceClass();
        }
        if (cf.hasToa()) {
          cfInfo->toa = cf.getToa();
        }
        if (!cf.getNumber().empty()) {
          cfInfo->number = cf.getNumber();
        }
        if (cf.hasTimeSeconds()) {
          cfInfo->timeSeconds = cf.getTimeSeconds();
        }
      }
    } else {
      ss.ssInfo.resize(1);
      ss.cfData.resize(0);
      if (msg->hasSuppSrvInfoList() && !msg->getSuppSrvInfoList().empty()) {

        ss.ssInfo[0].ssInfo.resize(SS_INFO_MAX);
        for (int i = 0; i < msg->getSuppSrvInfoList().size() && i < SS_INFO_MAX; i++) {
          ss.ssInfo[0].ssInfo[i] = msg->getSuppSrvInfoList()[i];
        }
      }
    }
    return true;
  }
  return false;
}

int convertCallForwardInfoStatus(int in) {
  switch (in) {
    case aidlvoice::CallForwardInfo::STATUS_DISABLE:
      return 0;  // disable
    case aidlvoice::CallForwardInfo::STATUS_ENABLE:
      return 1;  // enable
    case aidlvoice::CallForwardInfo::STATUS_INTERROGATE:
      return 2;  // interrogate
    case aidlvoice::CallForwardInfo::STATUS_REGISTRATION:
      return 3;  // registeration
    case aidlvoice::CallForwardInfo::STATUS_ERASURE:
      return 4;  // erasure
  }
  return 0;
}

bool isServiceTypeCf(RIL_SsServiceType serType, RIL_SsRequestType reqType) {
  if ((reqType == SS_INTERROGATION) &&
      (serType == SS_CFU ||
      serType == SS_CF_BUSY ||
      serType == SS_CF_NO_REPLY ||
      serType == SS_CF_NOT_REACHABLE ||
      serType == SS_CF_ALL ||
      serType == SS_CF_ALL_CONDITIONAL)) {
        return true;
  }
    return false;
}

int convertCallForwardInfoStatusToAidl(int in) {
  switch (in) {
    case 0:  // disable
      return aidlvoice::CallForwardInfo::STATUS_DISABLE;
    case 1:  // enable
      return aidlvoice::CallForwardInfo::STATUS_ENABLE;
    case 2:  // interrogate
      return aidlvoice::CallForwardInfo::STATUS_INTERROGATE;
    case 3:  // registeration
      return aidlvoice::CallForwardInfo::STATUS_REGISTRATION;
    case 4:  // erasure
      return aidlvoice::CallForwardInfo::STATUS_ERASURE;
  }
  return aidlvoice::CallForwardInfo::STATUS_DISABLE;
}

aidlvoice::AudioQuality convertAudioQualityToAidl(const RIL_AudioQuality rilAQ) {
  aidlvoice::AudioQuality aidlAQ;
  switch(rilAQ) {
    /** AMR (Narrowband) audio codec */
    case RIL_AUDIO_QUAL_AMR:
      aidlAQ = aidlvoice::AudioQuality::AMR;
      break;

    /** AMR (Wideband) audio codec */
    case RIL_AUDIO_QUAL_AMR_WB:
      aidlAQ = aidlvoice::AudioQuality::AMR_WB;
      break;

    /** GSM Enhanced Full-Rate audio codec */
    case RIL_AUDIO_QUAL_GSM_EFR:
      aidlAQ = aidlvoice::AudioQuality::GSM_EFR;
      break;

    /** GSM Full-Rate audio codec */
    case RIL_AUDIO_QUAL_GSM_FR:
      aidlAQ = aidlvoice::AudioQuality::GSM_FR;
      break;

    /** GSM Half-Rate audio codec */
    case RIL_AUDIO_QUAL_GSM_HR:
      aidlAQ = aidlvoice::AudioQuality::GSM_HR;
      break;

    /** Enhanced Variable rate codec */
    case RIL_AUDIO_QUAL_EVRC:
      aidlAQ = aidlvoice::AudioQuality::EVRC;
      break;

    /** Enhanced Variable rate codec revision B */
    case RIL_AUDIO_QUAL_EVRC_B:
      aidlAQ = aidlvoice::AudioQuality::EVRC_B;
      break;

    /** Enhanced Variable rate codec (Wideband) */
    case RIL_AUDIO_QUAL_EVRC_WB:
      aidlAQ = aidlvoice::AudioQuality::EVRC_WB;
      break;

    /** Enhanced Variable rate codec (Narrowband) */
    case RIL_AUDIO_QUAL_EVRC_NW:
      aidlAQ = aidlvoice::AudioQuality::EVRC_NW;
      break;

    /** Unspecified audio codec */
    case RIL_AUDIO_QUAL_UNSPECIFIED:
    default:
      aidlAQ = aidlvoice::AudioQuality::UNSPECIFIED;
      break;
  }

  return aidlAQ;
}

void convertToAidl(aidlvoice::Call& out, const qcril::interfaces::CallInfo& in) {
  out.state = convertCallStateToAidl(in.getCallState());
  out.index = in.getIndex();
  out.toa = in.getToa();
  out.isMpty = in.getIsMpty();
  out.isMT = in.getIsMt();
  out.als = in.getAls();
  out.isVoice = in.getIsVoice();
  out.isVoicePrivacy = in.getIsVoicePrivacy();
  out.number = in.getNumber();
  out.numberPresentation = convertCallPresentationToAidl(in.getNumberPresentation());
  out.name = in.getName();
  out.namePresentation = convertCallPresentationToAidl(in.getNamePresentation());
  out.forwardedNumber = in.getRedirNum();
  out.audioQuality = convertAudioQualityToAidl(in.getAudioQuality());

  if (in.hasUusInfo() && in.getUusInfo() != NULL) {
    std::shared_ptr<qcril::interfaces::UusInfo> uusInfo = in.getUusInfo();
    if (uusInfo->hasData() && !uusInfo->getData().empty()) {
      out.uusInfo.resize(1);
      out.uusInfo[0].uusType = static_cast<int>(uusInfo->getType());
      out.uusInfo[0].uusDcs = static_cast<int>(uusInfo->getDcs());
      out.uusInfo[0].uusData = uusInfo->getData();
    }
  }
}

int convertCallPresentationToAidl(uint32_t in) {
  switch (in) {
    case 0:
      return aidlvoice::Call::PRESENTATION_ALLOWED;
    case 1:
      return aidlvoice::Call::PRESENTATION_RESTRICTED;
    case 2:
    default:
      return aidlvoice::Call::PRESENTATION_UNKNOWN;
    case 3:
      return aidlvoice::Call::PRESENTATION_PAYPHONE;
  }
  return aidlvoice::Call::PRESENTATION_UNKNOWN;
}

int convertCallStateToAidl(qcril::interfaces::CallState in) {
  switch (in) {
    case qcril::interfaces::CallState::ACTIVE:
      return aidlvoice::Call::STATE_ACTIVE;
    case qcril::interfaces::CallState::HOLDING:
      return aidlvoice::Call::STATE_HOLDING;
    case qcril::interfaces::CallState::DIALING:
      return aidlvoice::Call::STATE_DIALING;
    case qcril::interfaces::CallState::ALERTING:
      return aidlvoice::Call::STATE_ALERTING;
    case qcril::interfaces::CallState::INCOMING:
      return aidlvoice::Call::STATE_INCOMING;
    case qcril::interfaces::CallState::WAITING:
      return aidlvoice::Call::STATE_WAITING;
    default:
      return aidlvoice::Call::STATE_ACTIVE;
  }
  return aidlvoice::Call::STATE_ACTIVE;
}

uint64_t toRilCategories(const int32_t categories) {
  uint64_t rilCategories = 0;

  if (categories & static_cast<int32_t>(aidlvoice::EmergencyServiceCategory::POLICE)) {
    rilCategories |= static_cast<uint64_t>(qcril::interfaces::RIL_EccCategoryMask::POLICE);
  }
  if (categories & static_cast<int32_t>(aidlvoice::EmergencyServiceCategory::AMBULANCE)) {
    rilCategories |= static_cast<uint64_t>(qcril::interfaces::RIL_EccCategoryMask::AMBULANCE);
  }
  if (categories & static_cast<int32_t>(aidlvoice::EmergencyServiceCategory::FIRE_BRIGADE)) {
    rilCategories |= static_cast<uint64_t>(qcril::interfaces::RIL_EccCategoryMask::FIRE_BRIGADE);
  }
  if (categories & static_cast<int32_t>(aidlvoice::EmergencyServiceCategory::MARINE_GUARD)) {
    rilCategories |= static_cast<uint64_t>(qcril::interfaces::RIL_EccCategoryMask::MARINE_GUARD);
  }
  if (categories & static_cast<int32_t>(aidlvoice::EmergencyServiceCategory::MOUNTAIN_RESCUE)) {
    rilCategories |=
        static_cast<uint64_t>(qcril::interfaces::RIL_EccCategoryMask::MOUNTAIN_RESCUE);
  }
  if (categories & static_cast<int32_t>(aidlvoice::EmergencyServiceCategory::MIEC)) {
    rilCategories |= static_cast<uint64_t>(qcril::interfaces::RIL_EccCategoryMask::MANUAL_ECALL);
  }
  if (categories & static_cast<int32_t>(aidlvoice::EmergencyServiceCategory::AIEC)) {
    rilCategories |= static_cast<uint64_t>(qcril::interfaces::RIL_EccCategoryMask::AUTO_ECALL);
  }

  if (rilCategories == 0) {
    rilCategories = static_cast<uint64_t>(qcril::interfaces::RIL_EccCategoryMask::UNKNOWN);
  }

  return rilCategories;
}

qcril::interfaces::EmergencyCallRouting toRilRouting(
    const aidlvoice::EmergencyCallRouting& routing) {
  qcril::interfaces::EmergencyCallRouting rilRouting =
      qcril::interfaces::EmergencyCallRouting::UNKNOWN;

  if (routing == aidlvoice::EmergencyCallRouting::EMERGENCY) {
    rilRouting = qcril::interfaces::EmergencyCallRouting::EMERGENCY;
  } else if (routing == aidlvoice::EmergencyCallRouting::NORMAL) {
    rilRouting = qcril::interfaces::EmergencyCallRouting::NORMAL;
  }
  return rilRouting;
}

bool isUssdOverImsSupported() {
  bool ussdOverImsSupported = false;
  bool bool_config;
  if (qcril_config_get(PERSIST_VENDOR_RADIO_IS_USSD_SUPPORTED, bool_config) == E_SUCCESS) {
    ussdOverImsSupported = bool_config;
  }
  bool ussdImsRuntimeSupport = property_get_bool(QCRIL_USSD_IMS_RUNTIME_SUPPORT, 1);
  return ussdOverImsSupported && ussdImsRuntimeSupport;
}

aidlvoice::UssdModeType convertUssdModeType(qcril::interfaces::UssdModeType in) {
  switch (in) {
    case qcril::interfaces::UssdModeType::NOTIFY:
      return aidlvoice::UssdModeType::NOTIFY;
    case qcril::interfaces::UssdModeType::REQUEST:
      return aidlvoice::UssdModeType::REQUEST;
    case qcril::interfaces::UssdModeType::NW_RELEASE:
      return aidlvoice::UssdModeType::NW_RELEASE;
    case qcril::interfaces::UssdModeType::LOCAL_CLIENT:
      return aidlvoice::UssdModeType::LOCAL_CLIENT;
    case qcril::interfaces::UssdModeType::NOT_SUPPORTED:
      return aidlvoice::UssdModeType::NOT_SUPPORTED;
    case qcril::interfaces::UssdModeType::NW_TIMEOUT:
      return aidlvoice::UssdModeType::NW_TIMEOUT;
    default:
      return aidlvoice::UssdModeType::NOTIFY;
  }
  return aidlvoice::UssdModeType::NOTIFY;
}

// Helper function to convert new internal RIL_Errno error codes
// Map the internal error codes to original error codes, as internal
// error codes cannot be mapped to a corresponding RadioError
RIL_Errno convertInternalErrorCode(RIL_Errno error) {
  switch(error) {
    case RIL_E_INTERNAL_NO_MEMORY:
      return RIL_E_NO_MEMORY;
    case RIL_E_INTERNAL_INVALID_STATE:
      return RIL_E_INVALID_STATE;
    case RIL_E_INTERNAL_INVALID_ARGUMENTS:
      return RIL_E_INVALID_ARGUMENTS;
    case RIL_E_INTERNAL_GENERIC_FAILURE:
      return RIL_E_GENERIC_FAILURE;
    default:
      return error;
  }
}
