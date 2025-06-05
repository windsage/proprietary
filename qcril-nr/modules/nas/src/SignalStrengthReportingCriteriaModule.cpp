/******************************************************************************
#  Copyright (c) 2018-2020 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#define TAG "RILQ_SSRepCritMod"
#include <functional>
#include <limits>
#include <framework/Module.h>
#include <framework/QcrilInitMessage.h>
#include <framework/Dispatcher.h>
#include <modules/qmi/QmiIndMessage.h>
#include <modules/qmi/QmiAsyncResponseMessage.h>
#include <interfaces/nas/NasSetSignalStrengthCriteria.h>
#include <interfaces/nas/NasSetSignalStrengthCriteriaAllRans.h>
#include <modules/nas/qcril_qmi_nas.h>
#include <modules/qmi/NasModemEndPoint.h>
#include <modules/qmi/ModemEndPointFactory.h>
#include <modules/qmi/EndpointStatusIndMessage.h>
#include <network_access_service_v01.h>
#include <modules/nas/qcril_nas_legacy.h>
#include <qcril_qmi_client.h>
#include <modules/dms/RadioPowerStatusMessage.h>
#include <modules/android/ClientConnectedMessage.h>
#include "qcril_qmi_err_utils.h"

using std::placeholders::_1;

DECLARE_MSG_ID_INFO(NAS_QMI_IND);
DECLARE_MSG_ID_INFO(NAS_ENDPOINT_STATUS_IND);


class SignalStrengthReportingCriteriaModule : public Module {
    public:
        SignalStrengthReportingCriteriaModule();
        ~SignalStrengthReportingCriteriaModule() {}
        void init();

    private:
        bool mReady = false;
        nas_config_sig_info2_req_msg_v01 req{};

        void handleQcrilInit(std::shared_ptr<QcrilInitMessage> msg);
        void handleNasQmiIndMessage(std::shared_ptr<QmiIndMessage> msg);
        void handleQmiAsyncRespMessage(std::shared_ptr<QmiAsyncResponseMessage> msg);
        void handleNasEndpointStatusIndMessage(std::shared_ptr<EndpointStatusIndMessage> msg);
        void handleNasSetSignalStrengthCriteria(std::shared_ptr<NasSetSignalStrengthCriteria> msg);
        void handleNasSetSignalStrengthCriteriaAllRans(std::shared_ptr<NasSetSignalStrengthCriteriaAllRans> msg);
        void handleClientConnectedMessage(std::shared_ptr<ClientConnectedMessage> msg);
        void handleRadioPowerStatusMessage(std::shared_ptr<RadioPowerStatusMessage> msg);
        void cleanUp() {
            memset(&req, 0, sizeof(req));
        }
};

static load_module<SignalStrengthReportingCriteriaModule> the_module;

SignalStrengthReportingCriteriaModule &getSignalStrengthReportingCriteriaModule() {
    return the_module.get_module();
}

SignalStrengthReportingCriteriaModule::SignalStrengthReportingCriteriaModule() {
    mName = "SignalStrengthReportingCriteriaModule";
    ModemEndPointFactory<NasModemEndPoint>::getInstance().buildEndPoint();

    mMessageHandler = {
        HANDLER(QcrilInitMessage, SignalStrengthReportingCriteriaModule::handleQcrilInit),
        HANDLER_MULTI(QmiIndMessage, NAS_QMI_IND, SignalStrengthReportingCriteriaModule::handleNasQmiIndMessage),
        // End Point Status Indication
        HANDLER_MULTI(EndpointStatusIndMessage, NAS_ENDPOINT_STATUS_IND, SignalStrengthReportingCriteriaModule::handleNasEndpointStatusIndMessage),
        // Qmi Async Response
        HANDLER(QmiAsyncResponseMessage, SignalStrengthReportingCriteriaModule::handleQmiAsyncRespMessage),
        HANDLER(NasSetSignalStrengthCriteria, SignalStrengthReportingCriteriaModule::handleNasSetSignalStrengthCriteria),
        HANDLER(NasSetSignalStrengthCriteriaAllRans, SignalStrengthReportingCriteriaModule::handleNasSetSignalStrengthCriteriaAllRans),
        HANDLER(ClientConnectedMessage, SignalStrengthReportingCriteriaModule::handleClientConnectedMessage),
        HANDLER(RadioPowerStatusMessage, SignalStrengthReportingCriteriaModule::handleRadioPowerStatusMessage),
    };
}

void SignalStrengthReportingCriteriaModule::handleQcrilInit(std::shared_ptr<QcrilInitMessage> msg) {
    QCRIL_LOG_DEBUG("[%s]: Handling msg = %s", mName.c_str(), msg->dump().c_str());
    /* Init QMI services.*/
    GenericCallback<string> gcb([](std::shared_ptr<Message> msg, Message::Callback::Status status, std::shared_ptr<void> payload) -> void {
            (void)msg;
            (void)status;
            (void)payload;
            });
    ModemEndPointFactory<NasModemEndPoint>::getInstance().buildEndPoint()->requestSetup(
            "", msg->get_instance_id(), &gcb);
}

void SignalStrengthReportingCriteriaModule::init()
{
    QCRIL_LOG_DEBUG("init");
    Module::init();
}

void SignalStrengthReportingCriteriaModule::handleNasQmiIndMessage(std::shared_ptr<QmiIndMessage> msg)
{
    QCRIL_LOG_DEBUG("Handling msg = %s", msg->dump().c_str() );
}

void SignalStrengthReportingCriteriaModule::handleQmiAsyncRespMessage(std::shared_ptr<QmiAsyncResponseMessage> msg)
{
    QCRIL_LOG_DEBUG("Handling msg = %s", msg->dump().c_str() );
}

void SignalStrengthReportingCriteriaModule::handleNasEndpointStatusIndMessage(std::shared_ptr<EndpointStatusIndMessage> msg)
{
    QCRIL_LOG_DEBUG("Handling msg = %s", msg->dump().c_str() );
    if (msg->getState() == ModemEndPoint::State::OPERATIONAL) {
        mReady = true;
    } else {
        mReady = false;
    }
}

void SignalStrengthReportingCriteriaModule::handleClientConnectedMessage(std::shared_ptr<ClientConnectedMessage> msg)
{
    QCRIL_LOG_DEBUG("Handling msg = %s", msg->dump().c_str() );
}

void SignalStrengthReportingCriteriaModule::handleRadioPowerStatusMessage(std::shared_ptr<RadioPowerStatusMessage> msg)
{
    QCRIL_LOG_DEBUG("Handling msg = %s", msg->dump().c_str() );
}

#define MAX_THRESHOLDS 32

void convertThreshold(const int32_t th_in, int16_t &th_out) {
    const int16_t int16_min = std::numeric_limits<int16_t>::min();
    const int16_t int16_max = std::numeric_limits<int16_t>::max();

    if (th_in >= int16_min && th_in <= int16_max) {
        th_out = static_cast<int16_t>(th_in);
    } else {
        if (th_in > int16_max) {
            th_out = int16_max;
        } else {
            th_out = int16_min;
        }
    }
}

void fillGeran(nas_config_sig_info2_req_msg_v01 &config, const qcril::interfaces::SignalStrengthCriteriaEntry &entry) {
    unsigned sz = entry.thresholds.size();

    if (entry.type != MT_RSSI)
        QCRIL_LOG_WARN("Unexpected geran measure type: %d", entry.type);

    if (entry.isEnabled) {
        if(sz > 0) {
            if (sz >= MAX_THRESHOLDS) sz = MAX_THRESHOLDS;
            config.gsm_rssi_threshold_list_valid = TRUE;
            QCRIL_LOG_DEBUG("Setting gsm rssi threshold_list_len to %u", sz);
            config.gsm_rssi_threshold_list_len = sz;
            for (unsigned i = 0; i < sz; i++) {
                convertThreshold(entry.thresholds[i] * 10, config.gsm_rssi_threshold_list[i]);
            }
        } else { //threshold vector size is 0
            QCRIL_LOG_DEBUG("Disabling usage of thresholds for geran rssi"
                "reporting. Setting it to implementation defined value");
            config.gsm_rssi_delta_valid = TRUE;
            config.gsm_rssi_delta = NasSetSignalStrengthCriteria::RSSI_DELTA;
            return; // hysteresis values are valid only if thresholds are passed
        }

        if (entry.hysteresisDb >= 0) {
            QCRIL_LOG_DEBUG("Setting gsm rssi hysteresisDb");
            config.gsm_hysteresis_valid = 1;
            config.gsm_hysteresis[config.gsm_hysteresis_len].signal = NAS_SIG_TYPE_RSSI_V01;
            config.gsm_hysteresis[config.gsm_hysteresis_len].delta = entry.hysteresisDb * 10;
            config.gsm_hysteresis_len++;
        }
    } else { //isEnabled is false
        QCRIL_LOG_DEBUG("gsm_rssi disabled from reporting");
        config.gsm_rssi_delta_valid = TRUE;
        config.gsm_rssi_delta = 65535;
    }
}

void fillUtran(nas_config_sig_info2_req_msg_v01 &config, const qcril::interfaces::SignalStrengthCriteriaEntry &entry) {
    unsigned sz = entry.thresholds.size();

    if (entry.type == MT_RSCP) {
        if (entry.isEnabled) {
            if (sz > 0) {
                if (sz >= MAX_THRESHOLDS) sz = MAX_THRESHOLDS;
                config.wcdma_rscp_threshold_list_valid = TRUE;
                QCRIL_LOG_DEBUG("Setting wcdma rscp threshold_list_len to %u", sz);
                config.wcdma_rscp_threshold_list_len = sz;
                for (unsigned i = 0; i < sz; i++) {
                    convertThreshold(entry.thresholds[i] * 10, config.wcdma_rscp_threshold_list[i]);
                }
            } else { //threshold vector size is 0
                QCRIL_LOG_DEBUG("Disabling usage of thresholds for wcdma rscp"
                    "reporting, setting it to implementation defined value");
                config.wcdma_rscp_delta_valid = TRUE;
                config.wcdma_rscp_delta = NasSetSignalStrengthCriteria::RSCP_DELTA;
                return;
            }
            if (entry.hysteresisDb >= 0) {
                QCRIL_LOG_DEBUG("Setting wcdma rscp hysteresisDb");
                config.wcdma_hysteresis_valid = 1;
                config.wcdma_hysteresis[config.wcdma_hysteresis_len].signal = NAS_SIG_TYPE_RSCP_V01;
                config.wcdma_hysteresis[config.wcdma_hysteresis_len].delta = entry.hysteresisDb * 10;
                config.wcdma_hysteresis_len++;
            }
        } else { //isenabled is false
            QCRIL_LOG_DEBUG(" wcdma rscp disabled from reporting");
            config.wcdma_rscp_delta_valid = TRUE;
            config.wcdma_rscp_delta = 65535;
        }
    } else if (entry.type == MT_RSSI) {
        if (entry.isEnabled) {
            if (sz > 0) {
                if (sz >= MAX_THRESHOLDS) sz = MAX_THRESHOLDS;
                config.wcdma_rssi_threshold_list_valid = TRUE;
                QCRIL_LOG_DEBUG("Setting wcdma rssi threshold_list_len to %u", sz);
                config.wcdma_rssi_threshold_list_len = sz;
                for (unsigned i = 0; i < sz; i++) {
                    convertThreshold(entry.thresholds[i] * 10, config.wcdma_rssi_threshold_list[i]);
                }
            } else { //threshold size is 0
                QCRIL_LOG_DEBUG("Disabling usage of thresholds for wcdma rssi "
                    "reporting, setting it to implementation defined value");
                config.wcdma_rssi_delta_valid = TRUE;
                config.wcdma_rssi_delta = NasSetSignalStrengthCriteria::RSSI_DELTA;
                return;
            }

            if (entry.hysteresisDb >= 0) {
                QCRIL_LOG_DEBUG("Setting wcdma rssi hysteresisDb");
                config.wcdma_hysteresis_valid = 1;
                config.wcdma_hysteresis[config.wcdma_hysteresis_len].signal = NAS_SIG_TYPE_RSSI_V01;
                config.wcdma_hysteresis[config.wcdma_hysteresis_len].delta = entry.hysteresisDb * 10;
                config.wcdma_hysteresis_len++;
            }
        } else { //isenabled is false
            QCRIL_LOG_DEBUG(" wcdma rssi disabled from reporting");
            config.wcdma_rssi_delta_valid = TRUE;
            config.wcdma_rssi_delta = 65535;
        }
    } else {
        QCRIL_LOG_ERROR("Unexpected utran measure type: %d", entry.type);
    }
}

void setLteDeltaValues(nas_config_sig_info2_req_msg_v01 &config){
    config.lte_rsrp_delta_valid = TRUE;
    config.lte_rsrp_delta = NasSetSignalStrengthCriteria::RSRP_DELTA;
    config.lte_rsrq_delta_valid = TRUE;
    config.lte_rsrq_delta = NasSetSignalStrengthCriteria::RSRQ_DELTA;
    config.lte_snr_delta_valid = TRUE;
    config.lte_snr_delta = NasSetSignalStrengthCriteria::RSSNR_DELTA;
}

void setGeranDeltaValues(nas_config_sig_info2_req_msg_v01 &config){
    config.gsm_rssi_delta_valid = TRUE;
    config.gsm_rssi_delta = NasSetSignalStrengthCriteria::RSSI_DELTA;
}

void setUtranDeltaValues(nas_config_sig_info2_req_msg_v01 &config){
    config.wcdma_rscp_delta_valid = TRUE;
    config.wcdma_rscp_delta = NasSetSignalStrengthCriteria::RSCP_DELTA;
    config.wcdma_rssi_delta_valid = TRUE;
    config.wcdma_rssi_delta = NasSetSignalStrengthCriteria::RSSI_DELTA;
}

void setCdmaDeltaValues(nas_config_sig_info2_req_msg_v01 &config){
    config.cdma_rssi_delta_valid = TRUE;
    config.cdma_rssi_delta = NasSetSignalStrengthCriteria::RSSI_DELTA;
}


void setNr5gDeltaValues(nas_config_sig_info2_req_msg_v01 &config){
    config.nr5g_rsrp_delta_valid = TRUE;
    config.nr5g_rsrp_delta = NasSetSignalStrengthCriteria::RSRP_DELTA;
    config.nr5g_rsrq_delta_valid = TRUE;
    config.nr5g_rsrq_delta = NasSetSignalStrengthCriteria::RSRQ_DELTA;
    config.nr5g_snr_delta_valid = TRUE;
    config.nr5g_snr_delta = NasSetSignalStrengthCriteria::RSSNR_DELTA;
}

void fillEutranReportingRate(nas_config_sig_info2_req_msg_v01 &config){
    config.lte_sig_rpt_config_valid = TRUE;
    config.lte_sig_rpt_config = {
        .rpt_rate = NAS_LTE_SIG_RPT_RATE_3_SEC_V01,
        .avg_period = NAS_LTE_SIG_AVG_PRD_5_SEC_V01
    };
}

void fillEutran(nas_config_sig_info2_req_msg_v01 &config, const qcril::interfaces::SignalStrengthCriteriaEntry &entry) {
    unsigned sz = entry.thresholds.size();
    if (entry.type == MT_RSRP) {
        if (entry.isEnabled) {
            if (sz > 0){
                if (sz >= MAX_THRESHOLDS) sz = MAX_THRESHOLDS;
                config.lte_rsrp_threshold_list_valid = TRUE;
                QCRIL_LOG_DEBUG("Setting lte rsrp threshold_list_len to %u", sz);
                config.lte_rsrp_threshold_list_len = sz;
                for (unsigned i = 0; i < sz; i++) {
                    convertThreshold(entry.thresholds[i] * 10, config.lte_rsrp_threshold_list[i]);
                }
            } else { //threshold size is 0
                QCRIL_LOG_DEBUG("Disabling usage of thresholds for LTE rsrp "
                    "reporting, setting it to implementation defined value");
                config.lte_rsrp_delta_valid = TRUE;
                config.lte_rsrp_delta = NasSetSignalStrengthCriteria::RSRP_DELTA;
                fillEutranReportingRate(config);
                return;
            }
            if (entry.hysteresisDb >= 0) {
                QCRIL_LOG_DEBUG("Setting lte rsrp hysteresisDb");
                config.lte_hysteresis_valid = 1;
                config.lte_hysteresis[config.lte_hysteresis_len].signal = NAS_SIG_TYPE_RSRP_V01;
                config.lte_hysteresis[config.lte_hysteresis_len].delta = entry.hysteresisDb * 10;
                config.lte_hysteresis_len++;
            }
        } else {
            QCRIL_LOG_DEBUG("lte rsrp disabled from reporting");
            config.lte_rsrp_delta_valid = TRUE;
            config.lte_rsrp_delta = 65535;
        }
    } else if (entry.type == MT_RSRQ) {
        if (entry.isEnabled) {
            if (sz > 0){
                if (sz >= MAX_THRESHOLDS) sz = MAX_THRESHOLDS;
                config.lte_rsrq_threshold_list_valid = TRUE;
                QCRIL_LOG_DEBUG("Setting lte rsrq threshold_list_len to %u", sz);
                config.lte_rsrq_threshold_list_len = sz;
                for (unsigned i = 0; i < sz; i++) {
                    convertThreshold(entry.thresholds[i] * 10, config.lte_rsrq_threshold_list[i]);
                }
            } else {
                QCRIL_LOG_DEBUG("Disabling usage of thresholds for LTE rsrq "
                    "reporting, setting it to implementation defined value");
                config.lte_rsrq_delta_valid = TRUE;
                config.lte_rsrq_delta = NasSetSignalStrengthCriteria::RSRQ_DELTA;
                fillEutranReportingRate(config);
                return;
            }

            if (entry.hysteresisDb >= 0) {
                QCRIL_LOG_DEBUG("Setting lte rsrq hysteresisDb");
                config.lte_hysteresis_valid = 1;
                config.lte_hysteresis[config.lte_hysteresis_len].signal = NAS_SIG_TYPE_RSRQ_V01;
                config.lte_hysteresis[config.lte_hysteresis_len].delta = entry.hysteresisDb * 10;
                config.lte_hysteresis_len++;
            }
        } else {
            QCRIL_LOG_DEBUG("lte rsrq disabled from reporting");
            config.lte_rsrq_delta_valid = TRUE;
            config.lte_rsrq_delta = 65535;
        }
    } else if (entry.type == MT_RSSNR) {
        if (entry.isEnabled) {
            if(sz > 0){
                if (sz >= MAX_THRESHOLDS) sz = MAX_THRESHOLDS;
                config.lte_snr_threshold_list_valid = TRUE;
                QCRIL_LOG_DEBUG("Setting lte rssnr threshold_list_len to %u", sz);
                config.lte_snr_threshold_list_len = sz;
                for (unsigned i = 0; i < sz; i++) {
                    convertThreshold(entry.thresholds[i] * 10, config.lte_snr_threshold_list[i]);
                }
            } else {
                QCRIL_LOG_DEBUG("Disabling usage of thresholds for LTE rssnr "
                    "reporting, setting it to implementation defined value");
                config.lte_snr_delta_valid = TRUE;
                config.lte_snr_delta = NasSetSignalStrengthCriteria::RSSNR_DELTA;
                fillEutranReportingRate(config);
                return;
            }

            if (entry.hysteresisDb >= 0) {
                QCRIL_LOG_DEBUG("Setting lte rssnr hysteresisDb");
                config.lte_hysteresis_valid = 1;
                config.lte_hysteresis[config.lte_hysteresis_len].signal = NAS_SIG_TYPE_SNR_V01;
                config.lte_hysteresis[config.lte_hysteresis_len].delta = entry.hysteresisDb * 10;
                config.lte_hysteresis_len++;
            }
        } else {
            QCRIL_LOG_DEBUG("lte rssnr disabled from reporting");
            config.lte_snr_delta_valid = TRUE;
            config.lte_snr_delta = 65535;
        }
    } else {
        QCRIL_LOG_ERROR("Unexpected lte measure type: %d", entry.type);
    }

    fillEutranReportingRate(config);
}

void fillCdma(nas_config_sig_info2_req_msg_v01 &config, const qcril::interfaces::SignalStrengthCriteriaEntry &entry) {
    unsigned sz = entry.thresholds.size();

    if (entry.type != MT_RSSI)
        QCRIL_LOG_WARN("Unexpected cdma measure type: %d", entry.type);

    if (entry.isEnabled) {
        if (sz > 0) {
            if (sz >= MAX_THRESHOLDS) sz = MAX_THRESHOLDS;
            config.cdma_rssi_threshold_list_valid = TRUE;
            QCRIL_LOG_DEBUG("Setting cdma rssi threshold_list_len to %u", sz);
            config.cdma_rssi_threshold_list_len = sz;
            for (unsigned i = 0; i < sz; i++) {
                convertThreshold(entry.thresholds[i] * 10, config.cdma_rssi_threshold_list[i]);
            }
        } else {
            QCRIL_LOG_DEBUG("Disabling usage of thresholds for CDMA rssi "
                    "reporting, setting it to implementation defined value");
            config.cdma_rssi_delta_valid = TRUE;
            config.cdma_rssi_delta = NasSetSignalStrengthCriteria::RSSI_DELTA;
            return;
        }

        if (entry.hysteresisDb >= 0) {
            QCRIL_LOG_DEBUG("Setting cdma rssi hysteresisDb");
            config.cdma_hysteresis_valid = 1;;
            config.cdma_hysteresis[config.cdma_hysteresis_len].signal = NAS_SIG_TYPE_RSSI_V01;
            config.cdma_hysteresis[config.cdma_hysteresis_len].delta = entry.hysteresisDb * 10;
            config.cdma_hysteresis_len++;
        }
    } else {
        QCRIL_LOG_DEBUG("CDMA rssi disabled from reporting");
        config.cdma_rssi_delta_valid = TRUE;
        config.cdma_rssi_delta = 65535;
    }
}


void fillNgranReportingtRate(nas_config_sig_info2_req_msg_v01 &config){
    config.nr5g_sig_rpt_config_valid = TRUE;
    config.nr5g_sig_rpt_config = {
        .rpt_rate = NR5G_COMMON_RSSI_REPORTING_RATE_3_SEC_V01,
        .avg_period = NR5G_COMMON_RSSI_AVERAGING_PERIOD_5_SEC_V01
    };
}

void fillNgran(nas_config_sig_info2_req_msg_v01 &config, const qcril::interfaces::SignalStrengthCriteriaEntry &entry) {
    unsigned sz = entry.thresholds.size();
    if (entry.type == MT_SSRSRP) {
        if (entry.isEnabled) {
            if (sz > 0) {
                if (sz >= MAX_THRESHOLDS) sz = MAX_THRESHOLDS;
                config.nr5g_rsrp_threshold_list_valid = TRUE;
                QCRIL_LOG_DEBUG("Setting nr5g rsrp threshold_list_len to %u", sz);
                config.nr5g_rsrp_threshold_list_len = sz;
                for (unsigned i = 0; i < sz; i++) {
                    convertThreshold(entry.thresholds[i] * 10, config.nr5g_rsrp_threshold_list[i]);
                }
            } else {
                QCRIL_LOG_DEBUG("Disabling usage of thresholds for nr5g rsrp "
                    "reporting, setting it to implementation defined value");
                config.nr5g_rsrp_delta_valid = TRUE;
                config.nr5g_rsrp_delta = NasSetSignalStrengthCriteria::RSRP_DELTA;
                fillNgranReportingtRate(config);
                return;
            }

            if (entry.hysteresisDb >= 0) {
                QCRIL_LOG_DEBUG("Setting nr5g rsrp hysteresisDb");
                config.nr5g_hysteresis_valid = 1;
                config.nr5g_hysteresis[config.nr5g_hysteresis_len].signal = NAS_SIG_TYPE_RSRP_V01;
                config.nr5g_hysteresis[config.nr5g_hysteresis_len].delta = entry.hysteresisDb * 10;
                config.nr5g_hysteresis_len++;
            }
        } else {
            QCRIL_LOG_DEBUG("nr5g rsrp disabled from reporting");
            config.nr5g_rsrp_delta_valid = TRUE;
            config.nr5g_rsrp_delta = 65535;
        }
    } else if (entry.type == MT_SSRSRQ) {
        if (entry.isEnabled) {
            if (sz > 0) {
                if (sz >= MAX_THRESHOLDS) sz = MAX_THRESHOLDS;
                config.nr5g_rsrq_threshold_list_valid = TRUE;
                QCRIL_LOG_DEBUG("Setting nr5g rsrq threshold_list_len to %u", sz);
                config.nr5g_rsrq_threshold_list_len = sz;
                for (unsigned i = 0; i < sz; i++) {
                    convertThreshold(entry.thresholds[i] * 10, config.nr5g_rsrq_threshold_list[i]);
                }
            } else {
                QCRIL_LOG_DEBUG("Disabling usage of thresholds for nr5g rsrq "
                    "reporting, setting it to implementation defined value");
                config.nr5g_rsrq_delta_valid = TRUE;
                config.nr5g_rsrq_delta = NasSetSignalStrengthCriteria::RSRQ_DELTA;
                fillNgranReportingtRate(config);
                return;
            }

            if (entry.hysteresisDb >= 0) {
                QCRIL_LOG_DEBUG("Setting nr5g rsrq hysteresisDb");
                config.nr5g_hysteresis_valid = 1;
                config.nr5g_hysteresis[config.nr5g_hysteresis_len].signal = NAS_SIG_TYPE_RSRQ_V01;
                config.nr5g_hysteresis[config.nr5g_hysteresis_len].delta = entry.hysteresisDb * 10;
                config.nr5g_hysteresis_len++;
            }
        } else {
            QCRIL_LOG_DEBUG("nr5g rsrq disabled from reporting");
            config.nr5g_rsrq_delta_valid = TRUE;
            config.nr5g_rsrq_delta = 65535;
        }
    } else if (entry.type == MT_SSSINR) {
        if (entry.isEnabled) {
            if (sz > 0) {
                if (sz >= MAX_THRESHOLDS) sz = MAX_THRESHOLDS;
                config.nr5g_snr_threshold_list_valid = TRUE;
                QCRIL_LOG_DEBUG("Setting nr5g snr threshold_list_len to %u", sz);
                config.nr5g_snr_threshold_list_len = sz;
                for (unsigned i = 0; i < sz; i++) {
                    convertThreshold(entry.thresholds[i] * 10, config.nr5g_snr_threshold_list[i]);
                }
            } else {
                QCRIL_LOG_DEBUG("Disabling usage of thresholds for nr5g snr "
                    "reporting, setting it to implementation defined value");
               config.nr5g_snr_delta_valid = TRUE;
               config.nr5g_snr_delta = NasSetSignalStrengthCriteria::RSSNR_DELTA;
               fillNgranReportingtRate(config);
               return;
            }

            if (entry.hysteresisDb >= 0) {
                QCRIL_LOG_DEBUG("Setting nr5g snr hysteresisDb");
                config.nr5g_hysteresis_valid = 1;
                config.nr5g_hysteresis[config.nr5g_hysteresis_len].signal = NAS_SIG_TYPE_SNR_V01;
                config.nr5g_hysteresis[config.nr5g_hysteresis_len].delta = entry.hysteresisDb * 10;
                config.nr5g_hysteresis_len++;
            }
        } else {
            QCRIL_LOG_DEBUG("nr5g snr disabled from reporting");
            config.nr5g_snr_delta_valid = TRUE;
            config.nr5g_snr_delta = 65535;
        }
    } else {
        QCRIL_LOG_ERROR("Unexpected nr5g measure type: %d", entry.type);
    }
    fillNgranReportingtRate(config);
}

void updateCriteria(nas_config_sig_info2_req_msg_v01 &out, const std::vector<qcril::interfaces::SignalStrengthCriteriaEntry> &criteria, bool disableAllTypes) {
    QCRIL_LOG_FUNC_ENTRY();

    for (const qcril::interfaces::SignalStrengthCriteriaEntry &entry: criteria) {
        QCRIL_LOG_DEBUG("Found entry criteria");

        if ((entry.isEnabled) && (entry.hysteresisMs >= 0)) {
            QCRIL_LOG_DEBUG("Setting the global hysteresis timer");
            out.hysteresis_timer_valid = TRUE;
            out.hysteresis_timer = entry.hysteresisMs;
        }

        switch(entry.ran) {
            case GERAN:
                QCRIL_LOG_DEBUG("RAN: GERAN");
                fillGeran(out, entry);
                break;
            case UTRAN:
                QCRIL_LOG_DEBUG("RAN: UTRAN");
                fillUtran(out, entry);
                break;
            case EUTRAN:
                QCRIL_LOG_DEBUG("RAN: EUTRAN");
                if (disableAllTypes) {
                    QCRIL_LOG_DEBUG("All Lte types are disabled. Setting delta for all lte types to implementation defined values");
                    setLteDeltaValues(out);
                    return;
                }
                fillEutran(out, entry);
                break;
            case CDMA:
                QCRIL_LOG_DEBUG("RAN: CDMA");
                fillCdma(out, entry);
                break;
            case IWLAN:
                QCRIL_LOG_DEBUG("RAN: IWLAN Unsupported");
                break;
            case NGRAN:
                QCRIL_LOG_DEBUG("RAN: NGRAN");
                if (disableAllTypes){
                    QCRIL_LOG_DEBUG("All nr5g types are disabled. Setting delta for all nr5g types to implementation defined values");
                    setNr5gDeltaValues(out);
                    return;
                }
                fillNgran(out, entry);
                break;
            default:
                QCRIL_LOG_ERROR("RAN: Unknown value %d", entry.ran);
                break;
        }
    }
    QCRIL_LOG_FUNC_RETURN();
}

void updateCriteriaAllRans(nas_config_sig_info2_req_msg_v01 &out, const std::vector<qcril::interfaces::SignalStrengthCriteriaEntry> &criteria) {
    QCRIL_LOG_FUNC_ENTRY();
    bool setGeranDelta{true};
    bool setUtranDelta{true};
    bool setEutranDelta{true};
    bool setCdmaDelta{true};
    bool setNr5gDelta{true};
    for (const qcril::interfaces::SignalStrengthCriteriaEntry &entry: criteria) {
        QCRIL_LOG_DEBUG("Found entry criteria");

        if ((entry.isEnabled) && (entry.hysteresisMs >= 0)) {
            QCRIL_LOG_DEBUG("Setting the global hysteresis timer");
            out.hysteresis_timer_valid = TRUE;
            out.hysteresis_timer = entry.hysteresisMs;
        }

        switch(entry.ran) {
            case GERAN:
                QCRIL_LOG_DEBUG("RAN: GERAN");
                setGeranDelta = false;
                fillGeran(out, entry);
                break;
            case UTRAN:
                QCRIL_LOG_DEBUG("RAN: UTRAN");
                setUtranDelta = false;
                fillUtran(out, entry);
                break;
            case EUTRAN:
                QCRIL_LOG_DEBUG("RAN: EUTRAN");
                setEutranDelta = false;
                fillEutran(out, entry);
                break;
            case CDMA:
                QCRIL_LOG_DEBUG("RAN: CDMA");
                setCdmaDelta = false;
                fillCdma(out, entry);
                break;
            case IWLAN:
                QCRIL_LOG_DEBUG("RAN: IWLAN Unsupported");
                break;
            case NGRAN:
                QCRIL_LOG_DEBUG("RAN: NGRAN");
                setNr5gDelta = false;
                fillNgran(out, entry);
                break;
            default:
                QCRIL_LOG_ERROR("RAN: Unknown value %d", entry.ran);
                break;
        }
    }
    // Unset RANs will be set to implementation defined values.
    if (setGeranDelta) {
      setGeranDeltaValues(out);
    }

    if (setUtranDelta) {
      setUtranDeltaValues(out);
    }

    if (setEutranDelta) {
      setLteDeltaValues(out);
    }

    if (setCdmaDelta) {
      setCdmaDeltaValues(out);
    }

    if (setNr5gDelta) {
      setNr5gDeltaValues(out);
    }

    QCRIL_LOG_FUNC_RETURN();
}

void SignalStrengthReportingCriteriaModule::handleNasSetSignalStrengthCriteria(std::shared_ptr<NasSetSignalStrengthCriteria> msg)
{
    QCRIL_LOG_DEBUG("Handling msg = NasSetSignalStrengthCriteria");
    if (msg) {
        QCRIL_LOG_DEBUG("Contents: %s", msg->dump().c_str());
        std::shared_ptr<RIL_Errno> rsp = nullptr;
        Message::Callback::Status status = Message::Callback::Status::FAILURE;

        if (mReady) {
            QCRIL_LOG_DEBUG("Making shared_ptr");
            rsp = std::make_shared<RIL_Errno>(RIL_E_SUCCESS);
            QCRIL_LOG_DEBUG("Calling updateCriteria");
            SignalStrengthReportingCriteriaModule::cleanUp();
            updateCriteria(req, msg->getCriteria(), msg->getDisableAllTypes());
            QCRIL_LOG_DEBUG("Returned from updateCriteria");
            qmi_client_error_type err = qmi_client_nas_send_async(QMI_NAS_CONFIG_SIG_INFO2_REQ_MSG_V01,
                    &req,
                    sizeof(req),
                    sizeof(nas_config_sig_info2_resp_msg_v01),
                    [msg] (unsigned int, std::shared_ptr<void> qmi_response, unsigned int, void *, qmi_client_error_type qmi_client_error) -> void {
                        QCRIL_LOG_DEBUG("Received response from QMI");
                        RIL_Errno ril_req_res =
                            qcril_qmi_util_convert_qmi_response_codes_to_ril_result(
                                qmi_client_error,
                                &(std::static_pointer_cast<nas_config_sig_info2_resp_msg_v01>(qmi_response)->resp) );
                        std::shared_ptr<RIL_Errno> rsp = std::make_shared<RIL_Errno>(ril_req_res);
                        msg->sendResponse(msg, Message::Callback::Status::SUCCESS, rsp);
                    },
                    NULL);
            if (err) {
                QCRIL_LOG_DEBUG("Received QMI error: %d", err);
                rsp = std::make_shared<RIL_Errno>(RIL_E_SYSTEM_ERR);
                msg->sendResponse(msg, Message::Callback::Status::SUCCESS, rsp);
            }
        } else {
            QCRIL_LOG_DEBUG("Responding not ready");
            rsp = std::make_shared<RIL_Errno>(RIL_E_RADIO_NOT_AVAILABLE);
            msg->sendResponse(msg, status, rsp);
        }
    }
}

void SignalStrengthReportingCriteriaModule::handleNasSetSignalStrengthCriteriaAllRans(std::shared_ptr<NasSetSignalStrengthCriteriaAllRans> msg)
{
    QCRIL_LOG_DEBUG("Handling msg = NasSetSignalStrengthCriteriaAllRans");
    if (msg) {
        QCRIL_LOG_DEBUG("Contents: %s", msg->dump().c_str());
        std::shared_ptr<RIL_Errno> rsp = nullptr;
        Message::Callback::Status status = Message::Callback::Status::FAILURE;

        if (mReady) {
            rsp = std::make_shared<RIL_Errno>(RIL_E_SUCCESS);
            QCRIL_LOG_DEBUG("Calling updateCriteriaAllRans");
            SignalStrengthReportingCriteriaModule::cleanUp();
            updateCriteriaAllRans(req, msg->getCriteria());
            QCRIL_LOG_DEBUG("Returned from updateCriteriaAllRans");
            qmi_client_error_type err = qmi_client_nas_send_async(QMI_NAS_CONFIG_SIG_INFO2_REQ_MSG_V01,
                    &req,
                    sizeof(req),
                    sizeof(nas_config_sig_info2_resp_msg_v01),
                    [msg] (unsigned int, std::shared_ptr<void> qmi_response, unsigned int, void *, qmi_client_error_type qmi_client_error) -> void {
                        QCRIL_LOG_DEBUG("Received response from QMI");
                        RIL_Errno ril_req_res =
                            qcril_qmi_util_convert_qmi_response_codes_to_ril_result(
                                qmi_client_error,
                                &(std::static_pointer_cast<nas_config_sig_info2_resp_msg_v01>(qmi_response)->resp) );
                        std::shared_ptr<RIL_Errno> rsp = std::make_shared<RIL_Errno>(ril_req_res);
                        msg->sendResponse(msg, Message::Callback::Status::SUCCESS, rsp);
                    },
                    NULL);
            if (err) {
                QCRIL_LOG_DEBUG("Received QMI error: %d", err);
                rsp = std::make_shared<RIL_Errno>(RIL_E_SYSTEM_ERR);
                msg->sendResponse(msg, Message::Callback::Status::SUCCESS, rsp);
            }
        } else {
            QCRIL_LOG_DEBUG("Responding not ready");
            rsp = std::make_shared<RIL_Errno>(RIL_E_RADIO_NOT_AVAILABLE);
            msg->sendResponse(msg, status, rsp);
        }
    }
}
