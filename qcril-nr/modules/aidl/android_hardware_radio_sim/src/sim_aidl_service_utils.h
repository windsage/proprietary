/******************************************************************************
#  Copyright (c) 2022 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#pragma once

#include "BnRadioSimDefault.h"
#include "aidl/android/hardware/radio/sim/CardStatus.h"

#include "interfaces/dms/dms_types.h"
#include "interfaces/nas/nas_types.h"
#include "interfaces/pbm/pbm.h"
#include "interfaces/sms/qcril_qmi_sms_types.h"
#include "interfaces/uim/qcril_uim_types.h"
#include "interfaces/voice/voice.h"
#include "MessageCommon.h"

#undef TAG
#define TAG "RILQ"

namespace aidlradio {
using namespace aidl::android::hardware::radio;
}

namespace aidlsim {
using namespace aidl::android::hardware::radio::sim;
}

namespace aidlconfig {
using namespace aidl::android::hardware::radio::config;
}

aidlradio::RadioError convertGetIccCardStatusResponse(aidlsim::CardStatus& cardStatus,
                                                      std::shared_ptr<RIL_UIM_CardStatus> p_cur);
qcril::interfaces::FacilityType convertFacilityType(std::string in);

void convertRilPbCapacity(const std::shared_ptr<qcril::interfaces::qcril_pbm_adn_count_info>& in,
                          aidlsim::PhonebookCapacity& out);
RIL_UIM_PersoSubstate convertHidlToRilPersoType(aidlsim::PersoSubstate persoType);

RIL_Errno convertHidlPhonebookRecords(const aidlsim::PhonebookRecordInfo& in,
                                 qcril::interfaces::AdnRecordInfo& out);
void convertRilPhonebookRecords(const qcril::interfaces::AdnRecords* in,
                                std::vector<aidlsim::PhonebookRecordInfo>& out);
aidlsim::PbReceivedStatus convertRilPbReceivedStatus(int seq_num);
