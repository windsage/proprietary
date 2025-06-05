/*===========================================================================
 *
 *    Copyright (c) 2018-2023 Qualcomm Technologies, Inc.
 *    All Rights Reserved.
 *    Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 *===========================================================================*/
#include "android/hardware/radio/1.2/types.h"
#include "platform/android/AndroidIRadio.h"
#include "platform/android/SmsAidlUtil.h"
#include "platform/android/NasHidlUtil.h"
#include "platform/android/VoiceHidlUtil.h"
#include "platform/android/SimHidlUtil.h"

#include "platform/android/IRadioConfigResponseImpl.h"
#include "platform/android/IRadioConfigIndicationImpl.h"
#include "platform/android/IRadioMessagingResponseImpl.h"
#include "platform/android/IRadioMessagingIndicationImpl.h"
#include "platform/android/IRadioModemResponseImpl.h"
#include "platform/android/IRadioModemIndicationImpl.h"
#include "platform/android/IRadioNetworkResponseImpl.h"
#include "platform/android/IRadioNetworkIndicationImpl.h"
#include "platform/android/IRadioSimResponseImpl.h"
#include "platform/android/IRadioSimIndicationImpl.h"
#include "platform/android/IRadioVoiceResponseImpl.h"
#include "platform/android/IRadioVoiceIndicationImpl.h"
#include "platform/android/VoiceAidlUtil.h"
#include "platform/android/IQtiRadioConfigResponseImpl.h"
#include "platform/android/IQtiRadioConfigIndicationImpl.h"

#include "ibinder_internal.h"
#include <binder/IBinder.h>
#include <android/binder_status.h>
#include <android/binder_ibinder.h>

#include <typeinfo>

#include <android/binder_manager.h>

using namespace android::hardware::radio;
using namespace android::hardware::radio::V1_0;

void qcril_qmi_radio_service_init(int instanceId);

void AndroidIRadio::setDataAPIs(::android::sp<IRadioResponseClientImpl> respClient,
        ::android::sp<IRadioIndicationClientImpl> indClient, 
        std::function<int(::android::sp<::android::hardware::radio::V1_6::IRadio> utfIRadioData,
        int, void *, size_t, RIL_Token)> onRequest) {
    dataRespClient = respClient;
    dataIndClient = indClient;
    dataOnRequest = onRequest;
}

void AndroidIRadio::Register(RIL_RadioFunctions *callbacks) {
    if (!bFirstCall) return;
    /* Sleep to allow RIL time to register IRadio Services
     * Without this sleep race condtion is possible, causing
     * some or all of the IRadio services to fail to bind */
#ifdef RIL_DATA_UTF
    usleep(10000); //sleep for 10 msec
#else
    usleep(50000); // Sleep for 50 msec
#endif
    qcril_qmi_radio_service_init(0);
    ::radio::registerService(callbacks, nullptr);
    utfIRadio = ::android::hardware::radio::V1_6::IRadio::getService("slot1", false);
    if (utfIRadio != nullptr) {
      RIL_UTF_DEBUG("AndroidIRadio::Register - Successfully got IRadio HIDL service.");
      // create the HIDL clients
      if (dataRespClient != nullptr && dataIndClient != nullptr) {
        RIL_UTF_DEBUG("AndroidIRadio::setting data response functions");
        utfIRadio->setResponseFunctions(dataRespClient, dataIndClient);
        } else {
            ::android::sp<IRadioResponseClientImpl> respClient
                = new IRadioResponseClientImpl();

            ::android::sp<IRadioIndicationClientImpl> indClient
                = new IRadioIndicationClientImpl();

            utfIRadio->setResponseFunctions(respClient, indClient);
        }
    } else {
      RIL_UTF_DEBUG("AndroidIRadio::Register - Failed to get IRadio HIDL service.");
    }
    bFirstCall = false;

    // Create the AIDL clients

    // IRadioVoice
    utfIRadioVoice = ::aidlvoice::IRadioVoice::fromBinder(
            ::ndk::SpAIBinder(AServiceManager_getService(
                "android.hardware.radio.voice.IRadioVoice/slot1")));

    if (utfIRadioVoice) {
        RIL_UTF_DEBUG("AndroidIRadio::Register - Successfully got IRadioVoice service.");
        std::shared_ptr<::aidlvoice::IRadioVoiceResponseImpl>
            IRadioVoiceResponseClient =
                ndk::SharedRefBase::make<::aidlvoice::IRadioVoiceResponseImpl>();

        std::shared_ptr<::aidlvoice::IRadioVoiceIndicationImpl>
            IRadioVoiceIndicationClient =
                ndk::SharedRefBase::make<::aidlvoice::IRadioVoiceIndicationImpl>();

        utfIRadioVoice->setResponseFunctions
            (IRadioVoiceResponseClient, IRadioVoiceIndicationClient);
    } else {
        RIL_UTF_DEBUG("AndroidIRadio::Register - Failed to get IRadioVoice service.");
    }

    // IRadioConfig
    utfIRadioConfig = ::aidlconfig::IRadioConfig::fromBinder(
            ::ndk::SpAIBinder(AServiceManager_getService(
                "android.hardware.radio.config.IRadioConfig/default")));

    if (utfIRadioConfig) {
        RIL_UTF_DEBUG("AndroidIRadio::Register - Successfully got IRadioConfig service.");
        std::shared_ptr<::aidlconfig::IRadioConfigResponseImpl>
            IRadioConfigResponseClient =
                ndk::SharedRefBase::make<::aidlconfig::IRadioConfigResponseImpl>();

        std::shared_ptr<::aidlconfig::IRadioConfigIndicationImpl>
            IRadioConfigIndicationClient =
                ndk::SharedRefBase::make<::aidlconfig::IRadioConfigIndicationImpl>();

        utfIRadioConfig->setResponseFunctions
            (IRadioConfigResponseClient, IRadioConfigIndicationClient);
    } else {
        RIL_UTF_DEBUG("AndroidIRadio::Register - Failed to get IRadioConfig service.");
    }

    // IRadioMessaging
    utfIRadioMessaging = ::aidlmessaging::IRadioMessaging::fromBinder(
            ::ndk::SpAIBinder(AServiceManager_getService(
                "android.hardware.radio.messaging.IRadioMessaging/slot1")));

    if (utfIRadioMessaging) {
        RIL_UTF_DEBUG("AndroidIRadio::Register - Successfully got IRadioMessaging service.");
        std::shared_ptr<::aidlmessaging::IRadioMessagingResponseImpl>
            IRadioMessagingResponseClient =
                ndk::SharedRefBase::make<::aidlmessaging::IRadioMessagingResponseImpl>();

        std::shared_ptr<::aidlmessaging::IRadioMessagingIndicationImpl>
            IRadioMessagingIndicationClient =
                ndk::SharedRefBase::make<::aidlmessaging::IRadioMessagingIndicationImpl>();

        utfIRadioMessaging->setResponseFunctions
            (IRadioMessagingResponseClient, IRadioMessagingIndicationClient);
    } else {
        RIL_UTF_DEBUG("AndroidIRadio::Register - Failed to get IRadioMessaging service.");
    }

    // IRadioModem
    utfIRadioModem = ::aidlmodem::IRadioModem::fromBinder(
            ::ndk::SpAIBinder(AServiceManager_getService(
                "android.hardware.radio.modem.IRadioModem/slot1")));

    if (utfIRadioModem) {
        RIL_UTF_DEBUG("AndroidIRadio::Register - Successfully got IRadioModem service.");
        std::shared_ptr<::aidlmodem::IRadioModemResponseImpl>
            IRadioModemResponseClient =
                ndk::SharedRefBase::make<::aidlmodem::IRadioModemResponseImpl>();

        std::shared_ptr<::aidlmodem::IRadioModemIndicationImpl>
            IRadioModemIndicationClient =
                ndk::SharedRefBase::make<::aidlmodem::IRadioModemIndicationImpl>();

        utfIRadioModem->setResponseFunctions
            (IRadioModemResponseClient, IRadioModemIndicationClient);
    } else {
        RIL_UTF_DEBUG("AndroidIRadio::Register - Failed to get IRadioModem service.");
    }

    // IRadioNetwork
    utfIRadioNetwork = ::aidlnetwork::IRadioNetwork::fromBinder(
            ::ndk::SpAIBinder(AServiceManager_getService(
                "android.hardware.radio.network.IRadioNetwork/slot1")));

    if (utfIRadioNetwork) {
        RIL_UTF_DEBUG("AndroidIRadio::Register - Successfully got IRadioNetwork service.");
        std::shared_ptr<::aidlnetwork::IRadioNetworkResponseImpl>
            IRadioNetworkResponseClient =
                ndk::SharedRefBase::make<::aidlnetwork::IRadioNetworkResponseImpl>();

        std::shared_ptr<::aidlnetwork::IRadioNetworkIndicationImpl>
            IRadioNetworkIndicationClient =
                ndk::SharedRefBase::make<::aidlnetwork::IRadioNetworkIndicationImpl>();

        utfIRadioNetwork->setResponseFunctions
            (IRadioNetworkResponseClient, IRadioNetworkIndicationClient);
    } else {
        RIL_UTF_DEBUG("AndroidIRadio::Register - Failed to get IRadioNetwork service.");
    }

    // IRadioSim
    utfIRadioSim = ::aidlsim::IRadioSim::fromBinder(
            ::ndk::SpAIBinder(AServiceManager_getService(
                "android.hardware.radio.sim.IRadioSim/slot1")));

    if (utfIRadioSim) {
        RIL_UTF_DEBUG("AndroidIRadio::Register - Successfully got IRadioSim service.");
        std::shared_ptr<::aidlsim::IRadioSimResponseImpl>
            IRadioSimResponseClient =
                ndk::SharedRefBase::make<::aidlsim::IRadioSimResponseImpl>();

        std::shared_ptr<::aidlsim::IRadioSimIndicationImpl>
            IRadioSimIndicationClient =
                ndk::SharedRefBase::make<::aidlsim::IRadioSimIndicationImpl>();

        utfIRadioSim->setResponseFunctions
            (IRadioSimResponseClient, IRadioSimIndicationClient);
    } else {
        RIL_UTF_DEBUG("AndroidIRadio::Register - Failed to get IRadioSim service.");
    }

    // QtiRadioConfig
    utfQtiRadioConfig = ::aidlqtiradioconfig::IQtiRadioConfig::fromBinder(
        ::ndk::SpAIBinder(AServiceManager_getService(
        "vendor.qti.hardware.radio.qtiradioconfig.IQtiRadioConfig/default")));

    if (utfQtiRadioConfig) {
      RIL_UTF_DEBUG("AndroidIRadio::Register - Successfully got IQtiRadioConfig service.");
      std::shared_ptr<::aidlqtiradioconfig::IQtiRadioConfigResponseImpl> IQtiRadioConfigResponseClient =
          ndk::SharedRefBase::make<::aidlqtiradioconfig::IQtiRadioConfigResponseImpl>();

      std::shared_ptr<::aidlqtiradioconfig::IQtiRadioConfigIndicationImpl>
          IQtiRadioConfigIndicationClient =
              ndk::SharedRefBase::make<::aidlqtiradioconfig::IQtiRadioConfigIndicationImpl>();

      utfQtiRadioConfig->setCallbacks(IQtiRadioConfigResponseClient,
                                      IQtiRadioConfigIndicationClient);
    } else {
      RIL_UTF_DEBUG("AndroidIRadio::Register - Failed to get IQtiRadioConfig service.");
    }
}

int AndroidIRadio::OnRequest(int request, void *data, size_t datalen,
        RIL_Token t) {
    switch (request) {
    case RIL_REQUEST_SEND_SMS:
      RIL_UTF_DEBUG("Calling RIL_REQUEST_CDMA_SEND_SMS");
      if (utfIRadioMessaging != nullptr) {
        RIL_UTF_DEBUG("Calling sendSms");
        aidlmessaging::GsmSmsMessage sms;
        convertGsmSmsPayloadToAidl(data, datalen, sms);
        utfIRadioMessaging->sendSms(*static_cast<int32_t*>(t), sms);
        return 0;
        } else {
            return 1;
        }
    case  RIL_REQUEST_CDMA_SEND_SMS:
        RIL_UTF_DEBUG("Calling RIL_REQUEST_CDMA_SEND_SMS");
        if (utfIRadioMessaging != nullptr) {
          RIL_UTF_DEBUG("Calling sendCdmaSms");
          aidlmessaging::CdmaSmsMessage sms;
          RIL_CDMA_SMS_Message* payload = static_cast<RIL_CDMA_SMS_Message*>(data);
          convertRilCdmaSmsToAidl(*payload, sms);
          utfIRadioMessaging->sendCdmaSms(*static_cast<int32_t*>(t), sms);
          return 0;
        } else {
            return 1;
        }
    case  RIL_REQUEST_CDMA_SEND_SMS_EXPECT_MORE:
        RIL_UTF_DEBUG("Calling RIL_REQUEST_CDMA_SEND_SMS_EXPECT_MORE");
        if (utfIRadioMessaging != nullptr) {
          RIL_UTF_DEBUG("Calling sendCdmaSmsExpectMore");
          aidlmessaging::CdmaSmsMessage sms;
          RIL_CDMA_SMS_Message* payload = static_cast<RIL_CDMA_SMS_Message*>(data);
          convertRilCdmaSmsToAidl(*payload, sms);
          utfIRadioMessaging->sendCdmaSmsExpectMore(*static_cast<int32_t*>(t), sms);
          return 0;
        } else {
            return 1;
        }
    case RIL_REQUEST_SMS_ACKNOWLEDGE:
      RIL_UTF_DEBUG("Calling RIL_REQUEST_SMS_ACKNOWLEDGE");
      if (utfIRadioMessaging != nullptr) {
        RIL_UTF_DEBUG("Calling acknowledgeLastIncomingGsmSms");
        auto ackInfo = static_cast<int*>(data);
        utfIRadioMessaging->acknowledgeLastIncomingGsmSms(
            *static_cast<int32_t*>(t),
            !!ackInfo[0],
            static_cast<aidlmessaging::SmsAcknowledgeFailCause>(ackInfo[1]));
        return 0;
        } else {
            return 1;
        }
    case RIL_REQUEST_IMS_REGISTRATION_STATE:
        if (utfIRadio != nullptr) {
            utfIRadio->getImsRegistrationState(*static_cast<int32_t*>(t));
            return 0;
        } else {
            return 1;
        }
    case RIL_REQUEST_SET_SMSC_ADDRESS:
      RIL_UTF_DEBUG("Calling RIL_REQUEST_SET_SMSC_ADDRESS");
      if (utfIRadioMessaging != nullptr) {
        RIL_UTF_DEBUG("Calling setSmscAddress");
        std::string addr = std::string((char*)data);
        utfIRadioMessaging->setSmscAddress(*static_cast<int32_t*>(t), std::move(addr));
        return 0;
        } else {
            return 1;
        }
    case RIL_REQUEST_DELETE_SMS_ON_SIM:
      RIL_UTF_DEBUG("Calling RIL_REQUEST_DELETE_SMS_ON_SIM");
      if (utfIRadioMessaging != nullptr) {
        RIL_UTF_DEBUG("Calling deleteSmsOnSim");
        int index = ((ril_request_delete_sms_on_sim_t*)data)->record_index;
        utfIRadioMessaging->deleteSmsOnSim(*static_cast<int32_t*>(t), index);
        return 0;
        } else {
            return 1;
        }
    case RIL_REQUEST_CDMA_DELETE_SMS_ON_RUIM:
      RIL_UTF_DEBUG("Calling RIL_REQUEST_CDMA_DELETE_SMS_ON_RUIM");
      if (utfIRadioMessaging != nullptr) {
        RIL_UTF_DEBUG("Calling deleteSmsOnRuim");
        int index = ((ril_request_delete_sms_on_sim_t*)data)->record_index;
        utfIRadioMessaging->deleteSmsOnRuim(*static_cast<int32_t*>(t), index);
        return 0;
        } else {
            return 1;
        }
    case RIL_REQUEST_VOICE_REGISTRATION_STATE:
        if (utfIRadio != nullptr) {
            utfIRadio->getVoiceRegistrationState_1_5(*static_cast<int32_t*>(t));
            return 0;
        } else {
            return 1;
        }
        break;
    case RIL_REQUEST_DATA_REGISTRATION_STATE:
        if (utfIRadio != nullptr) {
            utfIRadio->getDataRegistrationState_1_5(*static_cast<int32_t*>(t));
            return 0;
        } else {
            return 1;
        }
        break;
    case RIL_REQUEST_OPERATOR:
        if (utfIRadio != nullptr) {
            utfIRadio->getOperator(*static_cast<int32_t*>(t));
            return 0;
        } else {
            return 1;
        }
        break;
    case RIL_REQUEST_QUERY_AVAILABLE_NETWORKS:
        if (utfIRadioNetwork != nullptr) {
            utfIRadioNetwork->getAvailableNetworks(*static_cast<int32_t*>(t));
            return 0;
        } else {
            return 1;
        }
    case RIL_REQUEST_SET_PREFERRED_NETWORK_TYPE:
        if (utfIRadio != nullptr) {
            int networkType = *static_cast<int*>(data);
            utfIRadio->setPreferredNetworkType(*static_cast<int32_t*>(t),
                    (V1_0::PreferredNetworkType)networkType);
            return 0;
        } else {
            return 1;
        }
    case RIL_REQUEST_GET_PREFERRED_NETWORK_TYPE:
        if (utfIRadio != nullptr) {
            utfIRadio->getPreferredNetworkType(*static_cast<int32_t*>(t));
            return 0;
        } else {
            return 1;
        }
    case RIL_REQUEST_VOICE_RADIO_TECH:
      RIL_UTF_DEBUG("Calling RIL_REQUEST_VOICE_RADIO_TECH");
      if (utfIRadioNetwork != nullptr) {
        RIL_UTF_DEBUG("Calling getVoiceRadioTechnology");
        utfIRadioNetwork->getVoiceRadioTechnology(*static_cast<int32_t*>(t));
        return 0;
      } else {
        return 1;
      }
    case RIL_REQUEST_CDMA_SET_SUBSCRIPTION_SOURCE:
        if (utfIRadio != nullptr) {
            auto source = *static_cast<int*>(data);
            utfIRadio->setCdmaSubscriptionSource(*static_cast<int32_t*>(t),
                    static_cast<CdmaSubscriptionSource>(source));
            return 0;
        } else {
            return 1;
        }
    case RIL_REQUEST_CDMA_SET_ROAMING_PREFERENCE:
        if (utfIRadio != nullptr) {
            auto type = *static_cast<int*>(data);
            utfIRadio->setCdmaRoamingPreference(*static_cast<int32_t*>(t),
                    static_cast<CdmaRoamingType>(type));
            return 0;
        } else {
            return 1;
        }
    case RIL_REQUEST_CDMA_QUERY_ROAMING_PREFERENCE:
        if (utfIRadio != nullptr) {
            utfIRadio->getCdmaRoamingPreference(*static_cast<int32_t*>(t));
            return 0;
        } else {
            return 1;
        }
    case RIL_REQUEST_CDMA_GET_SUBSCRIPTION_SOURCE:
        if (utfIRadio != nullptr) {
            utfIRadio->getCdmaSubscriptionSource(*static_cast<int32_t*>(t));
            return 0;
        } else {
            return 1;
        }
    case RIL_REQUEST_SHUTDOWN:
        RIL_UTF_DEBUG("Calling RIL_REQUEST_SHUTDOWN");
        if (utfIRadioModem != nullptr) {
            RIL_UTF_DEBUG("Calling requestShutdown");
            utfIRadioModem->requestShutdown(*static_cast<int32_t*>(t));
            return 0;
        } else {
            return 1;
        }
    case RIL_REQUEST_DIAL:
      if (utfIRadioVoice != nullptr) {
        ::aidlvoice::Dial dialInfo = {};
        convertDialInfotoAidl(data, datalen, dialInfo);
        utfIRadioVoice->dial(*static_cast<int32_t*>(t), dialInfo);
        return 0;
      } else {
        return 1;
      }
    case RIL_REQUEST_GET_CURRENT_CALLS:
      if (utfIRadioVoice != nullptr) {
        utfIRadioVoice->getCurrentCalls(*static_cast<int32_t*>(t));
        return 0;
      } else {
        return 1;
      }
    case RIL_REQUEST_HANGUP:
      if (utfIRadioVoice != nullptr) {
        int index = *static_cast<int*>(data);
        utfIRadioVoice->hangup(*static_cast<int32_t*>(t), index);
        return 0;
      } else {
        return 1;
      }
    case RIL_REQUEST_ANSWER:
      if (utfIRadioVoice != nullptr) {
        utfIRadioVoice->acceptCall(*static_cast<int32_t*>(t));
        return 0;
      } else {
        return 1;
      }
    case RIL_REQUEST_HANGUP_WAITING_OR_BACKGROUND:
      if (utfIRadioVoice != nullptr) {
        utfIRadioVoice->hangupWaitingOrBackground(*static_cast<int32_t*>(t));
        return 0;
      } else {
        return 1;
      }
    case RIL_REQUEST_HANGUP_FOREGROUND_RESUME_BACKGROUND:
      if (utfIRadioVoice != nullptr) {
        utfIRadioVoice->hangupForegroundResumeBackground(*static_cast<int32_t*>(t));
        return 0;
      } else {
        return 1;
      }
    case RIL_REQUEST_UDUB:
      if (utfIRadioVoice != nullptr) {
        utfIRadioVoice->rejectCall(*static_cast<int32_t*>(t));
        return 0;
      } else {
        return 1;
      }
    case RIL_REQUEST_SWITCH_WAITING_OR_HOLDING_AND_ACTIVE:
      if (utfIRadioVoice != nullptr) {
        utfIRadioVoice->switchWaitingOrHoldingAndActive(*static_cast<int32_t*>(t));
        return 0;
      } else {
        return 1;
      }
    case RIL_REQUEST_CONFERENCE:
      if (utfIRadioVoice != nullptr) {
        utfIRadioVoice->conference(*static_cast<int32_t*>(t));
        return 0;
      } else {
        return 1;
      }
    case RIL_REQUEST_DTMF:
      if (utfIRadioVoice != nullptr) {
        auto dtmf_req = (ril_request_dtmf_t*)data;
        std::string s = dtmf_req->dtmf;
        utfIRadioVoice->sendDtmf(*static_cast<int32_t*>(t), s);
        return 0;
      } else {
        return 1;
      }
    case RIL_REQUEST_DTMF_START:
      if (utfIRadioVoice != nullptr) {
        auto dtmf_req = (ril_request_dtmf_t*)data;
        std::string s = dtmf_req->dtmf;
        utfIRadioVoice->startDtmf(*static_cast<int32_t*>(t), s);
        return 0;
      } else {
        return 1;
      }
    case RIL_REQUEST_DTMF_STOP:
      if (utfIRadioVoice != nullptr) {
        utfIRadioVoice->stopDtmf(*static_cast<int32_t*>(t));
        return 0;
      } else {
        return 1;
      }
    case RIL_REQUEST_GET_CLIR:
      if (utfIRadioVoice != nullptr) {
        utfIRadioVoice->getClir(*static_cast<int32_t*>(t));
        return 0;
      } else {
        return 1;
      }
    case RIL_REQUEST_SET_CLIR:
      if (utfIRadioVoice != nullptr) {
        auto req = (ril_request_set_clir_t*)data;
        utfIRadioVoice->setClir(*static_cast<int32_t*>(t), req->n_parameter);
        return 0;
      } else {
        return 1;
      }
    case RIL_REQUEST_GET_MUTE:
      if (utfIRadioVoice != nullptr) {
        utfIRadioVoice->getMute(*static_cast<int32_t*>(t));
        return 0;
      }
      return 1;
    case RIL_REQUEST_SET_MUTE:
      if (utfIRadioVoice != nullptr) {
        auto req = (ril_request_mute_t*)data;
        utfIRadioVoice->setMute(*static_cast<int32_t*>(t), req->enable);
        return 0;
      }
      return 1;
    case RIL_REQUEST_SET_UNSOL_CELL_INFO_LIST_RATE:
        if (utfIRadio != nullptr) {
            auto req = (ril_request_unsol_cell_info_rate_t *)data;
            utfIRadio->setCellInfoListRate(*static_cast<int32_t*>(t), req->rate);
            return 0;
        }
        return 1;
    case RIL_REQUEST_QUERY_CLIP:
      if (utfIRadioVoice != nullptr) {
        utfIRadioVoice->getClip(*static_cast<int32_t*>(t));
        return 0;
      } else {
        return 1;
      }
    case RIL_REQUEST_GET_CELL_INFO_LIST:
        if (utfIRadio != nullptr) {
            utfIRadio->getCellInfoList(*static_cast<int32_t*>(t));
            return 0;
        } else {
            return 1;
        }
    case RIL_REQUEST_EXPLICIT_CALL_TRANSFER:
      if (utfIRadioVoice != nullptr) {
        utfIRadioVoice->explicitCallTransfer(*static_cast<int32_t*>(t));
        return 0;
      } else {
        return 1;
      }
    case RIL_REQUEST_SEPARATE_CONNECTION:
      if (utfIRadioVoice != nullptr) {
        int index = *static_cast<int*>(data);
        utfIRadioVoice->separateConnection(*static_cast<int32_t*>(t), index);
        return 0;
      } else {
        return 1;
      }
    case RIL_REQUEST_LAST_CALL_FAIL_CAUSE:
      if (utfIRadioVoice != nullptr) {
        utfIRadioVoice->getLastCallFailCause(*static_cast<int32_t*>(t));
        return 0;
      } else {
        return 1;
      }
    case RIL_REQUEST_SEND_USSD:
        if (utfIRadio != nullptr) {
            char *ussd_str = static_cast<char *>(data);
            hidl_string ussd = ussd_str;
            utfIRadio->sendUssd(*static_cast<int32_t*>(t), ussd);
            return 0;
        } else {
            return 1;
        }
    case RIL_REQUEST_CANCEL_USSD:
        if (utfIRadio != nullptr) {
            utfIRadio->cancelPendingUssd(*static_cast<int32_t*>(t));
            return 0;
        } else {
            return 1;
        }
    case RIL_REQUEST_QUERY_CALL_FORWARD_STATUS:
      if (utfIRadioVoice != nullptr) {
        aidlvoice::CallForwardInfo cfInfo = {};
        convertCallForwardInfotoAidl(data, datalen, cfInfo);
        utfIRadioVoice->getCallForwardStatus(*static_cast<int32_t*>(t), cfInfo);
        return 0;
      } else {
        return 1;
      }
    case RIL_REQUEST_SET_CALL_FORWARD:
      if (utfIRadioVoice != nullptr) {
        aidlvoice::CallForwardInfo cfInfo = {};
        convertCallForwardInfotoAidl(data, datalen, cfInfo);
        utfIRadioVoice->setCallForward(*static_cast<int32_t*>(t), cfInfo);
        return 0;
      } else {
        return 1;
      }
    case RIL_REQUEST_QUERY_CALL_WAITING:
      if (utfIRadioVoice != nullptr) {
        auto req = (ril_request_query_call_waiting_t*)data;
        utfIRadioVoice->getCallWaiting(*static_cast<int32_t*>(t), req->service_class);
        return 0;
      } else {
        return 1;
      }
    case RIL_REQUEST_SET_CALL_WAITING:
      if (utfIRadioVoice != nullptr) {
        auto req = (ril_request_set_call_waiting_t*)data;
        bool enable = req->status ? true : false;
        int32_t serviceClass = req->service_class;
        utfIRadioVoice->setCallWaiting(*static_cast<int32_t*>(t), enable, serviceClass);
        return 0;
      } else {
        return 1;
      }
    case RIL_REQUEST_QUERY_FACILITY_LOCK:
        if (utfIRadio != nullptr) {
            char **req = (char**)data;
            string facility = req[0] ? req[0] : "";
            string password = req[1] ? req[1] : "";
            int32_t serviceClass = req[2] ? atoi(req[2]) : 0;
            string appId = req[3] ? req[3] : "";
            utfIRadio->getFacilityLockForApp(*static_cast<int32_t*>(t), facility, password, serviceClass, appId);
            return 0;
        } else {
            return 1;
        }
    case RIL_REQUEST_SET_FACILITY_LOCK:
        if (utfIRadio != nullptr) {
            char **req = (char**)data;
            string facility = req[0] ? req[0] : "";
            bool lockState = req[1] ? (req[1][0] == '1' ? true : false) : false;
            string password = req[2] ? req[2] : "";
            int32_t serviceClass = req[3] ? atoi(req[3]) : 0;
            string appId = req[4] ? req[4] : "";
            utfIRadio->setFacilityLockForApp(*static_cast<int32_t*>(t), facility, lockState, password, serviceClass, appId);
            return 0;
        } else {
            return 1;
        }
    case RIL_REQUEST_CHANGE_BARRING_PASSWORD:
        if (utfIRadioNetwork != nullptr) {
            auto req = (ril_request_change_barring_password_t *)data;
            utfIRadioNetwork->setBarringPassword(*static_cast<int32_t*>(t), req->facility_string_code, req->old_password, req->new_password);
            return 0;
        } else {
            return 1;
        }
    case RIL_REQUEST_SET_SUPP_SVC_NOTIFICATION:
        if (utfIRadioNetwork != nullptr) {
            auto req = (ril_request_set_supp_svc_notification_t *)data;
            bool enable = req->notification_status ? true : false;
            utfIRadioNetwork->setSuppServiceNotifications(*static_cast<int32_t*>(t), enable);
            return 0;
        } else {
            return 1;
        }
    case RIL_REQUEST_SET_TTY_MODE:
      if (utfIRadioVoice != nullptr) {
        auto req = (ril_request_set_tty_mode_t*)data;
        aidlvoice::TtyMode mode = (aidlvoice::TtyMode)req->tty_mode;
        switch (req->tty_mode) {
          case TTY_Off:
            mode = aidlvoice::TtyMode::OFF;
            break;
          case TTY_Full:
            mode = aidlvoice::TtyMode::FULL;
            break;
          case TTY_HCO:
            mode = aidlvoice::TtyMode::HCO;
            break;
          case TTY_VCO:
            mode = aidlvoice::TtyMode::VCO;
            break;
        }
        utfIRadioVoice->setTtyMode(*static_cast<int32_t*>(t), mode);
        return 0;
      } else {
        return 1;
      }
    case RIL_REQUEST_QUERY_TTY_MODE:
      if (utfIRadioVoice != nullptr) {
        utfIRadioVoice->getTtyMode(*static_cast<int32_t*>(t));
        return 0;
      } else {
        return 1;
      }
    case RIL_REQUEST_CDMA_SET_PREFERRED_VOICE_PRIVACY_MODE:
      if (utfIRadioVoice != nullptr) {
        auto req = (ril_request_cdma_set_preferred_voice_privacy_mode_t*)data;
        bool enable = req->voice_privacy_mode ? true : false;
        utfIRadioVoice->setPreferredVoicePrivacy(*static_cast<int32_t*>(t), enable);
        return 0;
      } else {
        return 1;
      }
    case RIL_REQUEST_CDMA_QUERY_PREFERRED_VOICE_PRIVACY_MODE:
      if (utfIRadioVoice != nullptr) {
        utfIRadioVoice->getPreferredVoicePrivacy(*static_cast<int32_t*>(t));
        return 0;
      } else {
        return 1;
      }
    case RIL_REQUEST_CDMA_FLASH:
      if (utfIRadioVoice != nullptr) {
        auto req = (ril_request_cdma_flash_t*)data;
        std::string featureCode = req->flash_string;
        utfIRadioVoice->sendCdmaFeatureCode(*static_cast<int32_t*>(t), featureCode);
        return 0;
      } else {
        return 1;
      }
    case RIL_REQUEST_CDMA_BURST_DTMF:
      if (utfIRadioVoice != nullptr) {
        auto req = (ril_request_cdma_burst_dtmf_t*)data;
        std::string dtmf = req->DTMF_string;
        int32_t on = atoi(req->DTMF_ON_length);
        int32_t off = atoi(req->DTMF_OFF_length);
        utfIRadioVoice->sendBurstDtmf(*static_cast<int32_t*>(t), dtmf, on, off);
        return 0;
      } else {
        return 1;
      }
    case RIL_REQUEST_RADIO_POWER:
        if (utfIRadioModem != nullptr) {
            auto req = static_cast<ril_request_radio_power_t*>(data);
            bool on = req->power ? true : false;
            bool emergency = req->reasonEmergency ? true : false;
            bool preferredSub = req->isPreferredSub ? true : false;
            utfIRadioModem->setRadioPower(*static_cast<int32_t*>(t), on, emergency, preferredSub);
            return 0;
        } else {
            return 1;
        }
    case RIL_REQUEST_BASEBAND_VERSION:
        RIL_UTF_DEBUG("Calling RIL_REQUEST_BASEBAND_VERSION");
        if (utfIRadioModem != nullptr) {
            RIL_UTF_DEBUG("Calling getBasebandVersion");
            utfIRadioModem->getBasebandVersion(*static_cast<int32_t*>(t));
            return 0;
        } else {
            return 1;
        }
    case RIL_REQUEST_DEVICE_IDENTITY:
        RIL_UTF_DEBUG("Calling RIL_REQUEST_DEVICE_IDENTITY");
        if (utfIRadioModem != nullptr) {
            RIL_UTF_DEBUG("Calling getDeviceIdentity");
            utfIRadioModem->getDeviceIdentity(*static_cast<int32_t*>(t));
            return 0;
        } else {
            return 1;
        }
    case RIL_REQUEST_GET_ACTIVITY_INFO:
        RIL_UTF_DEBUG("Calling RIL_REQUEST_GET_ACTIVITY_INFO");
        if (utfIRadioModem != nullptr) {
            RIL_UTF_DEBUG("Calling getModemActivityInfo");
            utfIRadioModem->getModemActivityInfo(*static_cast<int32_t*>(t));
            return 0;
        } else {
            return 1;
        }
    case RIL_REQUEST_SIM_IO:
        if (utfIRadio != nullptr) {
            V1_0::IccIo iccIoData;
            /* Construct the SIM IO request data */
            convertSimIoPayloadtoHidl(data, datalen, iccIoData);
            RIL_UTF_DEBUG("Calling iccIOForApp HIDL request");
            utfIRadio->iccIOForApp(*static_cast<int32_t*>(t), iccIoData);
            return 0;
        } else {
            return 1;
        }
    case RIL_REQUEST_GET_SIM_STATUS:
        if (utfIRadio != nullptr) {
            RIL_UTF_DEBUG("Calling getIccCardStatus HIDL request");
            utfIRadio->getIccCardStatus(*static_cast<int32_t*>(t));
            return 0;
        } else {
            return 1;
        }
    case RIL_REQUEST_QUERY_NETWORK_SELECTION_MODE:
        if (utfIRadio != nullptr) {
            RIL_UTF_DEBUG("Calling getNetworkSelectionMode HIDL request");
            utfIRadio->getNetworkSelectionMode(*static_cast<int32_t*>(t));
            return 0;
        } else {
            return 1;
        }
    case RIL_REQUEST_SET_BAND_MODE:
        if (utfIRadioNetwork != nullptr) {
            aidlnetwork::RadioBandMode mode = *static_cast<aidlnetwork::RadioBandMode*>(data);
            RIL_UTF_DEBUG("Calling setBandMode AIDL request");
            utfIRadioNetwork->setBandMode(*static_cast<int32_t*>(t), mode);
            return 0;
        } else {
            return 1;
        }
     case RIL_REQUEST_QUERY_AVAILABLE_BAND_MODE:
        if (utfIRadioNetwork != nullptr) {
            RIL_UTF_DEBUG("Calling getAvailableBandModes AIDL request");
            utfIRadioNetwork->getAvailableBandModes(*static_cast<int32_t*>(t));
            return 0;
        } else {
            return 1;
        }
    case RIL_REQUEST_ENABLE_MODEM:
         RIL_UTF_DEBUG("Calling RIL_REQUEST_ENABLE_MODEM");
         if (utfIRadioModem != nullptr) {
            RIL_UTF_DEBUG("Calling enableModem");
            int state = *static_cast<int*>(data);
            bool on = state ? true : false;
            utfIRadioModem->enableModem(*static_cast<int32_t*>(t), on);
            return 0;
        } else {
            return 1;
        }
    case RIL_REQUEST_GET_MODEM_STACK_STATUS:
         RIL_UTF_DEBUG("Calling RIL_REQUEST_GET_MODEM_STACK_STATUS");
         if (utfIRadioModem != nullptr) {
            RIL_UTF_DEBUG("Calling getModemStackStatus");
            utfIRadioModem->getModemStackStatus(*static_cast<int32_t*>(t));
            return 0;
        } else {
            return 1;
        }
    case RIL_REQUEST_GET_SMSC_ADDRESS:
      RIL_UTF_DEBUG("Calling RIL_REQUEST_GET_SMSC_ADDRESS");
      if (utfIRadioMessaging != nullptr) {
        RIL_UTF_DEBUG("Calling getSmscAddress");
        utfIRadioMessaging->getSmscAddress(*static_cast<int32_t*>(t));
        return 0;
        } else {
            return 1;
        }
    case RIL_REQUEST_WRITE_SMS_TO_SIM:
         RIL_UTF_DEBUG("Calling RIL_REQUEST_WRITE_SMS_TO_SIM");
         if (utfIRadioMessaging != nullptr) {
           RIL_UTF_DEBUG("Calling writeSmsToSim");
           aidlmessaging::SmsWriteArgs smswrite;
           convertGsmSmsWritePayloadToAidl(data, datalen, smswrite);
           utfIRadioMessaging->writeSmsToSim(*static_cast<int32_t*>(t), smswrite);
           return 0;
        } else {
            return 1;
        }
    case RIL_REQUEST_CDMA_WRITE_SMS_TO_RUIM:
         RIL_UTF_DEBUG("Calling RIL_REQUEST_CDMA_WRITE_SMS_TO_RUIM");
         if (utfIRadioMessaging != nullptr) {
           RIL_UTF_DEBUG("Calling writeSmsToRuim");
           aidlmessaging::CdmaSmsWriteArgs smswrite;
           convertCdmaSmsWritePayloadToAidl(data, datalen, smswrite);
           utfIRadioMessaging->writeSmsToRuim(*static_cast<int32_t*>(t), smswrite);
           return 0;
        } else {
            return 1;
        }
    case RIL_REQUEST_CDMA_SMS_ACKNOWLEDGE:
         RIL_UTF_DEBUG("Calling RIL_REQUEST_CDMA_SMS_ACKNOWLEDGE");
         if (utfIRadioMessaging != nullptr) {
           RIL_UTF_DEBUG("Calling acknowledgeLastIncomingCdmaSms");
           aidlmessaging::CdmaSmsAck smsack;
           convertCdmaSmsAckToAidl(data, datalen, smsack);
           utfIRadioMessaging->acknowledgeLastIncomingCdmaSms(*static_cast<int32_t*>(t), smsack);
           return 0;
        } else {
            return 1;
        }
    case RIL_REQUEST_CDMA_SET_BROADCAST_SMS_CONFIG:
         RIL_UTF_DEBUG("Calling RIL_REQUEST_CDMA_SET_BROADCAST_SMS_CONFIG");
         if (utfIRadioMessaging != nullptr) {
           RIL_UTF_DEBUG("Calling setCdmaBroadcastConfig");
           std::vector<aidlmessaging::CdmaBroadcastSmsConfigInfo> bconfigs;
           convertCdmaBcConfigToAidl(data, datalen, bconfigs);
           utfIRadioMessaging->setCdmaBroadcastConfig(*static_cast<int32_t*>(t), bconfigs);
           return 0;
        } else {
            return 1;
        }
    case RIL_REQUEST_GSM_SET_BROADCAST_SMS_CONFIG:
         RIL_UTF_DEBUG("Calling RIL_REQUEST_GSM_SET_BROADCAST_SMS_CONFIG");
         if (utfIRadioMessaging != nullptr) {
           RIL_UTF_DEBUG("Calling setGsmBroadcastConfig");
           std::vector<aidlmessaging::GsmBroadcastSmsConfigInfo> bconfigs;
           convertGsmBcConfigToAidl(data, datalen, bconfigs);
           utfIRadioMessaging->setGsmBroadcastConfig(*static_cast<int32_t*>(t), bconfigs);
           return 0;
        } else {
            return 1;
        }
    case RIL_REQUEST_GSM_GET_BROADCAST_SMS_CONFIG:
         RIL_UTF_DEBUG("Calling RIL_REQUEST_GSM_GET_BROADCAST_SMS_CONFIG");
         if (utfIRadioMessaging != nullptr) {
           RIL_UTF_DEBUG("Calling getGsmBroadcastConfig");
           utfIRadioMessaging->getGsmBroadcastConfig(*static_cast<int32_t*>(t));
           return 0;
        } else {
            return 1;
        }
    case RIL_REQUEST_CDMA_GET_BROADCAST_SMS_CONFIG:
         RIL_UTF_DEBUG("Calling RIL_REQUEST_CDMA_GET_BROADCAST_SMS_CONFIG");
         if (utfIRadioMessaging != nullptr) {
           RIL_UTF_DEBUG("Calling getCdmaBroadcastConfig");
           utfIRadioMessaging->getCdmaBroadcastConfig(*static_cast<int32_t*>(t));
           return 0;
        } else {
            return 1;
        }
    case RIL_REQUEST_REPORT_SMS_MEMORY_STATUS:
        RIL_UTF_DEBUG("Calling RIL_REQUEST_REPORT_SMS_MEMORY_STATUS");
        if (utfIRadioMessaging != nullptr) {
          RIL_UTF_DEBUG("Calling reportSmsMemoryStatus");
          bool memavail = (((static_cast<int*>(data))[0] == 0) ? false : true);
          utfIRadioMessaging->reportSmsMemoryStatus(*static_cast<int32_t*>(t), memavail);
          return 0;
        } else {
            return 1;
        }
    case RIL_REQUEST_GSM_SMS_BROADCAST_ACTIVATION:
        RIL_UTF_DEBUG("Calling RIL_REQUEST_GSM_SMS_BROADCAST_ACTIVATION");
        if (utfIRadioMessaging != nullptr) {
          RIL_UTF_DEBUG("Calling setGsmBroadcastActivation");
          bool activate = (((static_cast<int*>(data))[0] == 0) ? false : true);
          utfIRadioMessaging->setGsmBroadcastActivation(*static_cast<int32_t*>(t), activate);
          return 0;
        } else {
            return 1;
        }
    case RIL_REQUEST_CDMA_SMS_BROADCAST_ACTIVATION:
        RIL_UTF_DEBUG("Calling RIL_REQUEST_CDMA_SMS_BROADCAST_ACTIVATION");
        if (utfIRadioMessaging != nullptr) {
          RIL_UTF_DEBUG("Calling setCdmaBroadcastActivation");
          bool activate = (((static_cast<int*>(data))[0] == 0) ? false : true);
          utfIRadioMessaging->setCdmaBroadcastActivation(*static_cast<int32_t*>(t), activate);
          return 0;
        } else {
            return 1;
        }
    case RIL_REQUEST_START_NETWORK_SCAN:
        RIL_UTF_DEBUG("Calling RIL_REQUEST_START_NETWORK_SCAN");
        if (utfIRadioNetwork != nullptr) {
            RIL_UTF_DEBUG("Calling startNetworkScan");
            aidlnetwork::NetworkScanRequest nsRequest = {};
            RIL_NetworkScanRequest *rilReq = static_cast<RIL_NetworkScanRequest*>(data);
            convertNetworkScanRequestToAidl(*rilReq, nsRequest);
            utfIRadioNetwork->startNetworkScan(*static_cast<int32_t*>(t), nsRequest);
            return 0;
        }
        return 1;
    case RIL_REQUEST_STOP_NETWORK_SCAN:
        RIL_UTF_DEBUG("Calling RIL_REQUEST_STOP_NETWORK_SCAN");
        if (utfIRadioNetwork != nullptr) {
            RIL_UTF_DEBUG("Calling stopNetworkScan");
            utfIRadioNetwork->stopNetworkScan(*static_cast<int32_t*>(t));
            return 0;
        }
        return 1;
    case RIL_REQUEST_GET_NEIGHBORING_CELL_IDS:
        RIL_UTF_DEBUG("Calling RIL_REQUEST_GET_NEIGHBORING_CELL_IDS");
        if (utfIRadio != nullptr) {
            RIL_UTF_DEBUG("Calling getNeighboringCids");
            utfIRadio->getNeighboringCids(*static_cast<int32_t*>(t));
            return 0;
        }
        return 1;
    case RIL_REQUEST_SET_SYSTEM_SELECTION_CHANNELS:
        RIL_UTF_DEBUG("Calling RIL_REQUEST_SET_SYSTEM_SELECTION_CHANNELS");
        if (utfIRadioNetwork != nullptr) {
            RIL_UTF_DEBUG("Calling setSystemSelectionChannels");
            std::vector<aidlnetwork::RadioAccessSpecifier> raspecifiers;
            auto rilReq = static_cast<ril_request_set_sys_channels_t*>(data);
            bool specifyChannels = rilReq->specifyChannels;
            RIL_SysSelChannels in = rilReq->specifiers_list;
            raspecifiers = convertAidlRadioAccessSpecifier(in.specifiers_length, in.specifiers);
            utfIRadioNetwork->setSystemSelectionChannels(*static_cast<int32_t*>(t),specifyChannels, raspecifiers);
            return 0;
        }
        return 1;
    case RIL_REQUEST_GET_SYSTEM_SELECTION_CHANNELS:
        RIL_UTF_DEBUG("Calling RIL_REQUEST_GET_SYSTEM_SELECTION_CHANNELS");
        if (utfIRadio != nullptr) {
            RIL_UTF_DEBUG("Calling getSystemSelectionChannels");
            utfIRadio->getSystemSelectionChannels(*static_cast<int32_t*>(t));
            return 0;
        } else {
            return 1;
        }
    case RIL_REQUEST_CONFIG_GET_PHONE_CAPABILITY:
        RIL_UTF_DEBUG("Calling RIL_REQUEST_CONFIG_GET_PHONE_CAPABILITY");
        if (utfIRadioConfig != nullptr) {
            RIL_UTF_DEBUG("Calling getPhoneCapability");
            utfIRadioConfig->getPhoneCapability(*static_cast<int32_t*>(t));
            return 0;
        } else {
            return 1;
        }
    case RIL_REQUEST_GET_IMEI:
      RIL_UTF_DEBUG("Calling RIL_REQUEST_GET_IMEI");
      if (utfIRadioModem != nullptr) {
        RIL_UTF_DEBUG("Calling getImei");
        utfIRadioModem->getImei(*static_cast<int32_t*>(t));
        return 0;
      } else {
        return 1;
      }
    case RIL_REQUEST_SET_UNSOLICITED_RESPONSE_FILTER:
        RIL_UTF_DEBUG("Calling RIL_REQUEST_SET_UNSOLICITED_RESPONSE_FILTER");
        if (utfIRadioNetwork != nullptr) {
            auto rilReq = static_cast<ril_request_set_indication_filter_t*>(data);
            uint32_t indicationFilter = rilReq->indicationFilter;
            utfIRadioNetwork->setIndicationFilter(*static_cast<int32_t*>(t), indicationFilter);
            return 0;
        } else {
            return 1;
        }
    case RIL_REQUEST_NV_RESET_CONFIG:
      RIL_UTF_DEBUG("Calling RIL_REQUEST_NV_RESET_CONFIG");
      if (utfIRadioModem != nullptr) {
        RIL_UTF_DEBUG("Calling nvResetConfig. Data = %s", data == nullptr ? "null" : "non-null");
        utfIRadioModem->nvResetConfig(*static_cast<int32_t*>(t),
                                      *static_cast<aidlmodem::ResetNvType*>(data));
        return 0;
      }
      return 1;
    case RIL_REQUEST_SET_LOCATION_UPDATES:
      RIL_UTF_DEBUG("Calling RIL_REQUEST_SET_LOCATION_UPDATES");
      if (utfIRadioNetwork != nullptr) {
        RIL_UTF_DEBUG("Calling setLocationUpdates");
        bool in_enable = *(static_cast<bool*>(data));
        utfIRadioNetwork->setLocationUpdates(*static_cast<int32_t*>(t), in_enable);
        return 0;
      }
      return 1;
    case RIL_REQUEST_SEND_DEVICE_STATE:
        RIL_UTF_DEBUG("Calling RIL_REQUEST_SEND_DEVICE_STATE");
        if (utfIRadioModem != nullptr) {
            auto rilReq = static_cast<ril_request_send_device_state_t*>(data);
            bool in_enable = rilReq->mValue;
            ::aidl::android::hardware::radio::modem::DeviceStateType in_deviceStateType {};
            RIL_DeviceStateType ril_deviceState = rilReq->mType;
            convertRILDeviceStateToAidl(in_deviceStateType, ril_deviceState);
            utfIRadioModem->sendDeviceState(*static_cast<int32_t*>(t), in_deviceStateType,
                                            in_enable);
            return 0;
        } else {
            return 1;
        }
    case RIL_REQUEST_GET_USAGE_SETTING:
      RIL_UTF_DEBUG("Calling RIL_REQUEST_GET_USAGE_SETTING");
      if (utfIRadioNetwork != nullptr) {
        RIL_UTF_DEBUG("Calling getUsageSetting");
        utfIRadioNetwork->getUsageSetting(*static_cast<int32_t*>(t));
        return 0;
      }
      return 1;
    case RIL_REQUEST_SET_USAGE_SETTING:
      RIL_UTF_DEBUG("Calling RIL_REQUEST_SET_USAGE_SETTING");
      if (utfIRadioNetwork != nullptr) {
        RIL_UTF_DEBUG("Calling setUsageSetting");
        aidlnetwork::UsageSetting* mode = static_cast<aidlnetwork::UsageSetting*>(data);
        utfIRadioNetwork->setUsageSetting(*static_cast<int32_t*>(t), *mode);
        return 0;
      }
      return 1;
    case RIL_REQUEST_SET_MSIM_PREFERENCE:
      RIL_UTF_DEBUG("Calling RIL_REQUEST_SET_MSIM_PREFERENCE");
      if (utfQtiRadioConfig != nullptr) {
        RIL_UTF_DEBUG("Calling setMsimPreference");
        aidlqtiradioconfig::MsimPreference pref;
        if (convertMsimPreference(*static_cast<Msim_Preference*>(data), pref)) {
          utfQtiRadioConfig->setMsimPreference(*static_cast<int32_t*>(t), pref);
          return 0;
        }
      }
      return 1;
    case RIL_REQUEST_SIGNAL_STRENGTH:
      RIL_UTF_DEBUG("Calling RIL_REQUEST_SIGNAL_STRENGTH");
      if (utfIRadioNetwork != nullptr) {
        RIL_UTF_DEBUG("Calling getSystemSelectionChannels");
        utfIRadioNetwork->getSignalStrength(*static_cast<int32_t*>(t));
        return 0;
      } else {
        return 1;
      }
    case RIL_REQUEST_GET_CELL_BARRING_INFO:
      RIL_UTF_DEBUG("Calling RIL_REQUEST_GET_CELL_BARRING_INFO;");
      if (utfIRadioNetwork != nullptr) {
          RIL_UTF_DEBUG("Calling getBarringInfo");
          utfIRadioNetwork->getBarringInfo(*static_cast<int32_t*>(t));
          return 0;
      }  else {
          return 1;
      }
    case RIL_REQUEST_CDMA_SUBSCRIPTION:
      RIL_UTF_DEBUG("Calling RIL_REQUEST_CDMA_SUBSCRIPTION");
      if (utfIRadioSim != nullptr) {
        RIL_UTF_DEBUG("Calling getCdmaSubscription");
        utfIRadioSim->getCdmaSubscription(*static_cast<int32_t*>(t));
        return 0;
      } else {
        return 1;
      }
    case RIL_REQUEST_GET_UICC_APPLICATIONS_ENABLEMENT:
      RIL_UTF_DEBUG("Calling RIL_REQUEST_GET_UICC_APPLICATIONS_ENABLEMENT");
      if (utfIRadioSim != nullptr) {
        RIL_UTF_DEBUG("Calling areUiccApplicationsEnabled");
        utfIRadioSim->areUiccApplicationsEnabled(*static_cast<int32_t*>(t));
        return 0;
      } else {
        return 1;
      }
    case RIL_REQUEST_SET_UICC_SUBSCRIPTION:
      RIL_UTF_DEBUG("Calling RIL_REQUEST_SET_UICC_SUBSCRIPTION");
      if (utfIRadioSim != nullptr) {
        RIL_UTF_DEBUG("Calling setUiccSubscription");

        RIL_SelectUiccSub* in_data = static_cast<RIL_SelectUiccSub*>(data);
        size_t payload_len = sizeof(aidl::android::hardware::radio::sim::SelectUiccSub);
        aidl::android::hardware::radio::sim::SelectUiccSub* payload =
            (aidl::android::hardware::radio::sim::SelectUiccSub*)malloc(payload_len);
        memset(payload, 0x0, payload_len);

        payload->slot = in_data->slot;
        payload->appIndex = in_data->app_index;
        payload->subType = (in_data)->sub_type;
        payload->actStatus = (in_data)->act_status;

        utfIRadioSim->setUiccSubscription(*static_cast<int32_t*>(t), *payload);
        return 0;
      } else {
        return 1;
      }
    case RIL_REQUEST_GET_RADIO_CAPABILITY:
      RIL_UTF_DEBUG("Calling RIL_REQUEST_GET_RADIO_CAPABILITY");
      if (utfIRadioModem != nullptr) {
        RIL_UTF_DEBUG("Calling getRadioCapability");
        utfIRadioModem->getRadioCapability(*static_cast<int32_t*>(t));
        return 0;
      }
      return 1;
    case RIL_REQUEST_SET_RADIO_CAPABILITY:
      RIL_UTF_DEBUG("Calling RIL_REQUEST_SET_RADIO_CAPABILITY");
      if (utfIRadioModem != nullptr) {
        RIL_UTF_DEBUG("Calling setRadioCapability");
        aidlmodem::RadioCapability* aidlReq =
            (aidlmodem::RadioCapability*)malloc(sizeof(aidlmodem::RadioCapability));

        if (convertToAidlRadioCapability(*(static_cast<RIL_RadioCapability*>(data)), *aidlReq)) {
          utfIRadioModem->setRadioCapability(*static_cast<int32_t*>(t), *aidlReq);
          return 0;
        }
      }
      return 1;
    case RIL_REQUEST_ENABLE_VONR:
        RIL_UTF_DEBUG("Calling RIL_REQUEST_ENABLE_VONR;");
        if (utfIRadioVoice != nullptr) {
            auto rilReq = static_cast<ril_request_set_vo_nr_enabled_t*>(data);
            bool enabled = rilReq->mEnable;
            utfIRadioVoice->setVoNrEnabled(*static_cast<int32_t*>(t), enabled);
            return 0;
        }  else {
            return 1;
        }
    case RIL_REQUEST_IS_VONR_ENABLED:
        RIL_UTF_DEBUG("Calling RIL_REQUEST_IS_VONR_ENABLED;");
        if (utfIRadioVoice != nullptr) {
            utfIRadioVoice->isVoNrEnabled(*static_cast<int32_t*>(t));
            return 0;
        }  else {
            return 1;
        }
    case RIL_REQUEST_IS_FEATURE_SUPPORTED:
        RIL_UTF_DEBUG("Calling RIL_REQUEST_IS_FEATURE_SUPPORTED;");
        if (utfQtiRadioConfig != nullptr) {
            RIL_UTF_DEBUG("Calling isFeatureSupported");
            int feature = 1;
            bool *ptr = (bool*)malloc(sizeof(bool));
            utfQtiRadioConfig->isFeatureSupported(feature, ptr);
            handle_sync_response(ptr, sizeof(bool), RIL_REQUEST_IS_FEATURE_SUPPORTED,
                *static_cast<int32_t*>(t), RIL_E_SUCCESS, ril_utf_ril_response);
            return 0;
        } else {
            return 1;
        }
    default:
        return dataOnRequest(utfIRadio, request, data, datalen, t);
    }

    return 1;
}

AndroidIRadio& getAndroidIRadio() {
    static AndroidIRadio iradio;
    return iradio;
}

void AndroidIRadio::handle_sync_response(void *payload, int payload_len, unsigned long msg_id,
        int token_id, RIL_Errno e, enum ril_utf_q_element_t type) {
    int slot = acquire_expectation_table();
    enqueue_ril_expect(payload,
            payload_len,
            msg_id, // msg_id
            token_id, //token_id
            e, //RIL error
            type);
    release_expectation_table(slot);
}

static void __attribute__((constructor)) registerPlatform() {
    setPlatform(&getAndroidIRadio());
    RIL_UTF_DEBUG("AndroidIRadio::registerPlatform()");
}
