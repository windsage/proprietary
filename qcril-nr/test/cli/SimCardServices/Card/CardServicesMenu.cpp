/*
 * Copyright (c) 2019 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 * Copyright (c) 2019, The Linux Foundation. All rights reserved
 */

/**
 * CardServicesMenu provides menu options to invoke Card Services such as Transmit APDU.
 */

#include <chrono>
#include <iostream>
#include <limits>
//#include "Utils.hpp"
//#include <telux/tel/PhoneFactory.hpp>
#include<string.h>
#include "CardServicesMenu.hpp"

#define PRINT_CB std::cout << "\033[1;35mCallback: \033[0m"

// void ChangeCardPinResponseCb(int retryCount, telux::common::ErrorCode error) {
//   if(error != telux::common::ErrorCode::SUCCESS) {
//      PRINT_CB << "Change Card Pin Request failed with errorCode: " << static_cast<int>(error)
//               << ":" << Utils::getErrorCodeAsString(error) << std::endl;
//   } else {
//      PRINT_CB << "Change Card Pin Request successful retryCount:" << retryCount << std::endl;
//   }
//}
//
// void unlockCardByPinResponseCb(int retryCount, telux::common::ErrorCode error) {
//   if(error != telux::common::ErrorCode::SUCCESS) {
//      PRINT_CB << "Unlock Card By Pin Request failed with errorCode: " << static_cast<int>(error)
//               << ":" << Utils::getErrorCodeAsString(error) << std::endl;
//   } else {
//      PRINT_CB << "Unlock Card By Pin Request successful retryCount:" << retryCount << std::endl;
//   }
//}
//
// void unlockCardByPukResponseCb(int retryCount, telux::common::ErrorCode error) {
//   if(error != telux::common::ErrorCode::SUCCESS) {
//      PRINT_CB << "Unlock Card By Puk Request failed with errorCode: " << static_cast<int>(error)
//               << ":" << Utils::getErrorCodeAsString(error) << std::endl;
//   } else {
//      PRINT_CB << "Unlock Card By Puk request successful retryCount:" << retryCount << std::endl;
//   }
//}
//
// void setCardLockResponseCb(int retryCount, telux::common::ErrorCode error) {
//   if(error != telux::common::ErrorCode::SUCCESS) {
//      PRINT_CB << "Set Card Lock Request failed with errorCode: " << static_cast<int>(error) << ":"
//               << Utils::getErrorCodeAsString(error) << std::endl;
//   } else {
//      PRINT_CB << "Set Card Lock request successful retryCount:" << retryCount << std::endl;
//   }
//}
//
// void queryFdnLockResponseCb(bool isAvailable, bool isEnabled, telux::common::ErrorCode error) {
//   if(error != telux::common::ErrorCode::SUCCESS) {
//      PRINT_CB << "Query FDN lock state request failed with errorCode: " << static_cast<int>(error)
//               << ":" << Utils::getErrorCodeAsString(error) << std::endl;
//   } else {
//      PRINT_CB << "Query FDN lock state is successful" << std::endl;
//      if(isAvailable) {
//         PRINT_CB << "FDN lock is available and ";
//         if(isEnabled) {
//            std::cout << "enabled" << std::endl;
//         } else {
//            std::cout << "disabled" << std::endl;
//         }
//      } else {
//         PRINT_CB << "FDN lock not available" << std::endl;
//      }
//   }
//}
//
// void queryPin1LockResponseCb(bool state, telux::common::ErrorCode error) {
//   if(error != telux::common::ErrorCode::SUCCESS) {
//      PRINT_CB << "Query Pin1 Lock Request failed with errorCode: " << static_cast<int>(error)
//               << ":" << Utils::getErrorCodeAsString(error) << std::endl;
//   } else {
//      PRINT_CB << "Query Pin1 Lock Request successful state:" << state << std::endl;
//   }
//}

CardServicesMenu::CardServicesMenu(std::string appName, std::string cursor,
                                   RilApiSession& rilSession)
    : ConsoleApp(appName, cursor), rilSession(rilSession) {
  std::chrono::time_point<std::chrono::system_clock> startTime, endTime;
  startTime = std::chrono::system_clock::now();
  //  Get the PhoneFactory and PhoneManager instances.
  //   auto &phoneFactory = telux::tel::PhoneFactory::getInstance();
  //   cardManager_ = phoneFactory.getCardManager();
  //
  //   //  Check if telephony subsystem is ready
  //   bool subSystemStatus = cardManager_->isSubsystemReady();
  bool subSystemStatus = true;
  //   //  If telephony subsystem is not ready, wait for it to be ready
  //   if(!subSystemStatus) {
  //      std::cout << "Telephony subsystem is not ready, Please wait" << std::endl;
  //      std::future<bool> f = cardManager_->onSubsystemReady();
  //      // If we want to wait unconditionally for telephony subsystem to be ready
  //      subSystemStatus = f.get();
  //   }

  //   //  Exit the application, if SDK is unable to initialize telephony subsystems
  //   if(subSystemStatus) {
  //      endTime = std::chrono::system_clock::now();
  //      std::chrono::duration<double> elapsedTime = endTime - startTime;
  //      std::cout << "Elapsed Time for Subsystems to ready : " << elapsedTime.count() << "s\n"
  //                << std::endl;
  //   } else {
  //      std::cout << "ERROR - Unable to initialize subSystem" << std::endl;
  //      exit(0);
  //   }
  //
  //   if(subSystemStatus) {
  //      std::vector<int> slotIds;
  //      cardManager_->getSlotIds(slotIds);
  //
  //      // get the default card object
  //      card_ = cardManager_->getCard();
  //
  //      // listener
  //      cardListener_ = std::make_shared<MyCardListener>();
  //
  //      // callbacks
  //      myOpenLogicalChannelCb_ = std::make_shared<MyOpenLogicalChannelCallback>();
  //      myTransmitApduCb_ = std::make_shared<MyTransmitApduResponseCallback>();
  //      myCloseLogicalChannelCb_ = std::make_shared<MyCardCommandResponseCallback>();
  //
  //      // registering Listener
  //      telux::common::Status status = cardManager_->registerListener(cardListener_);
  //      if(status != telux::common::Status::SUCCESS) {
  //         std::cout << "Unable to registerListener" << std::endl;
  //      }
  //   }
}

CardServicesMenu::~CardServicesMenu() {
  //   if(cardListener_) {
  //      cardManager_->removeListener(cardListener_);
  //      cardListener_ = nullptr;
  //   }
  //   myOpenLogicalChannelCb_ = nullptr;
  //   myTransmitApduCb_ = nullptr;
  //   myCloseLogicalChannelCb_ = nullptr;
}

void CardServicesMenu::init() {
    std::vector<std::shared_ptr<ConsoleAppCommand>> commandsListCardServicesSubMenu = {
      CREATE_COMMAND(CardServicesMenu::getCardState, "Get_card_state"),
      CREATE_COMMAND(CardServicesMenu::getSupportedApps, "Get_supported_apps"),
      CREATE_COMMAND(CardServicesMenu::openLogicalChannel, "Open_logical_channel", "aid", "p2"),
      CREATE_COMMAND(CardServicesMenu::closeLogicalChannel, "Close_logical_channel", "channel"),
      CREATE_COMMAND(CardServicesMenu::transmitApdu, "Transmit_APDU", "session-id", "cla", "ins(Hexvalue)", "p1", "p2", "p3", "data"),
      CREATE_COMMAND(CardServicesMenu::basicTransmitApdu, "Basic_transmit_APDU", "session-id", "cla", "ins(Hexvalue)", "p1", "p2", "p3", "data"),
      CREATE_COMMAND(CardServicesMenu::changeCardPin, "Change_card_pin", "oldpin", "newpin", "aid"),
      CREATE_COMMAND(CardServicesMenu::changeCardPin2, "Change_card_pin2", "oldpin","newpin","aid"),
      CREATE_COMMAND(CardServicesMenu::unlockCardByPin, "Unlock_card_by_pin", "pin","aid"),
      CREATE_COMMAND(CardServicesMenu::unlockCardByPin2, "Unlock_card_by_pin2", "pin2","aid"),
      CREATE_COMMAND(CardServicesMenu::unlockCardByPuk, "Unlock_card_by_puk", "puk code","pin","aid"),
      CREATE_COMMAND(CardServicesMenu::unlockCardByPuk2, "Unlock_card_by_puk2", "pukcode","pin2","aid"),
      CREATE_COMMAND(CardServicesMenu::queryPin1LockState, "Query_pin1_lockState"),
      CREATE_COMMAND(CardServicesMenu::queryFdnLockState, "Query_fdn_lockState", "facility","password","service-class","aid"),
      CREATE_COMMAND(CardServicesMenu::setCardLock, "Set_card_lock", "facility","lock_status","password","service-class","aid"),
      CREATE_COMMAND(CardServicesMenu::getImsirequest, "Get_imsi_command", "aid"),
      CREATE_COMMAND(CardServicesMenu::setSimIoReq, "Sim_Io_req", "command(Hexvalue)","fileid(Hexvalue)","path","p1","p2","p3","data","pin2","aidPtr"),
      CREATE_COMMAND(CardServicesMenu::enveloperequest, "Envelope_Req", "command"),
      CREATE_COMMAND(CardServicesMenu::envelopecommand, "Envelope_command", "command"),
      CREATE_COMMAND(CardServicesMenu::Terminalresp_command, "TerminalResp_command", "command"),
      CREATE_COMMAND(CardServicesMenu::CallSetup_command, "CallSetupRequest_command", "command(hexvalue)"),
      CREATE_COMMAND(CardServicesMenu::ServiceRunning_command, "ServiceIsRunning_command", "command"),
      CREATE_COMMAND(CardServicesMenu::IsimAuthenticate_command, "IsimAuth_command", "command"),
      CREATE_COMMAND(CardServicesMenu::simAuthenticate_command, "simAuth_command", "authContext(Hexvalue)","authData","aid"),
      CREATE_COMMAND(CardServicesMenu::NwDeperso_command, "NetworkDeperso_command", "command"),
      CREATE_COMMAND(CardServicesMenu::registerForIndications, "Unsol_Registration"),
      CREATE_COMMAND(CardServicesMenu::GetAtr, "Get_atr", "slot-id"),
      CREATE_COMMAND(CardServicesMenu::GetAdnRecord, "GetAdnRecord_command"),
      CREATE_COMMAND(CardServicesMenu::UpdateAdnRecord, "UpdateAdnRecord_command"),
      CREATE_COMMAND(CardServicesMenu::lpauserreq, "token","eventype","activation_code",
                                "confirmation_code","nicname" ,"iccid" ,"resetmask",
                                 "userOk","nok_reason","opCode","smdpaddress"),
 };
  addCommands(commandsListCardServicesSubMenu);
  ConsoleApp::displayMenu();
}

// std::string CardServicesMenu::cardStateToString(telux::tel::CardState state) {
//   std::string cardState;
//   switch(state) {
//      case telux::tel::CardState::CARDSTATE_ABSENT:
//         cardState = "Absent";
//         break;
//      case telux::tel::CardState::CARDSTATE_PRESENT:
//         cardState = "Present";
//         break;
//      case telux::tel::CardState::CARDSTATE_ERROR:
//         cardState = "Either error or absent";
//         break;
//      case telux::tel::CardState::CARDSTATE_RESTRICTED:
//         cardState = "Restricted";
//         break;
//      default:
//         cardState = "Unknown card state";
//         break;
//   }
//   return cardState;
//}
static const char *const HEX_DIGITS = "0123456789ABCDEF";
static const char *const BASE64_DIGITS =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
static const char BASE64_PADDING = '=';
typedef struct {
    size_t size;
    unsigned char *data;
} bytestring;

static unsigned char hex_value(char digit)
{
    return (unsigned char) (strchr(HEX_DIGITS, digit) - HEX_DIGITS);
}

char bin_to_hexchar
(
  uint8_t ch
)
{

  if (ch < 0x0a)
  {
    return (ch + '0');
  }
  return (ch + 'a' - 10);
}

void bin_to_hexstring
(
 char*  buffer_ptr,
  uint16_t        buffer_size,
  char*           string_ptr,
  uint32_t        string_size
)
{
  int    i = 0;

  if(buffer_ptr == NULL || string_ptr == NULL)
  {
    return;
  }

  memset(string_ptr, 0, string_size);

  if (string_size < 3) {return;}
  if ((buffer_size*2 + 1) > string_size) {
    buffer_size = (string_size-1)/2;
  }

  for (i = 0; i < buffer_size; i++)
  {
    string_ptr[i * 2] = bin_to_hexchar((buffer_ptr[i] >> 4) & 0x0F);
    string_ptr[i * 2 + 1] = bin_to_hexchar(buffer_ptr[i] & 0x0F);
  }
  string_ptr[buffer_size * 2] = 0x0;
}

char* mem_alloc_bin_to_hexstring
(
  char*  buffer_ptr,
  uint16_t        buffer_size
)
{
  char* out_ptr    = NULL;
  uint32_t string_len = 0;

  if(buffer_ptr == NULL)
  {
    return NULL;
  }

  if(buffer_size == 0)
  {
    return NULL;
  }

  string_len = (buffer_size * 2) + sizeof(char);

  out_ptr = (char *)malloc(string_len);

  if (out_ptr != NULL)
  {
      bin_to_hexstring(buffer_ptr, buffer_size, out_ptr, string_len);
  }

  return out_ptr;
}

static bool bytes_from_hex(const char *hex, bytestring *bytes_ptr)
{
    size_t num_digits = 0;
    for (const char *ptr = hex; *ptr != '\0'; ptr++) {
        if (strchr(HEX_DIGITS, *ptr) == NULL) {
            return false;
        }
        num_digits++;
    }

    if (num_digits % 2 == 1) {
        return false;
    }

    bytes_ptr->size = num_digits / 2;
    if ((bytes_ptr->data = (unsigned char *)malloc(bytes_ptr->size)) == NULL) {
        return false;
    }

    for (size_t i = 0; i < bytes_ptr->size; i++) {
        unsigned char value_a = hex_value(hex[2*i + 0]);
        unsigned char value_b = hex_value(hex[2*i + 1]);
        bytes_ptr->data[i] = (unsigned char) (value_a << 4 | value_b);
    }
    return true;
}

static uint8_t find_base64_values
(
  bool  find_char,
  char     input_char,
  uint8_t  input_index
)
{
  const char base64_table[] =
    {"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/"};
  const char * value_ptr = NULL;

  if (find_char)
  {
    value_ptr = strchr(base64_table, input_char);
    if (value_ptr)
    {
      return value_ptr - base64_table;
    }
  }
  else
  {
    if (input_index < (sizeof(base64_table)/sizeof(base64_table[0])))
    {
      return (uint8_t)base64_table[input_index];
    }
  }

  return 0;
}

char *mem_alloc_base64string_to_bin
(
   const char   * input_ptr,
   uint16_t     * output_len_ptr
)
{
  uint8_t   i           = 0;
  uint8_t   j           = 0;
  uint16_t  input_len   = 0;
  uint16_t  output_len  = 0;
  char * output_ptr  = NULL;

  if (input_ptr == NULL || output_len_ptr == NULL)
  {
    return NULL;
  }

  /* Base64 string have to be multiples of 4 byte-blocks and
     possibly padded to a 4 bytes block */
  input_len = strlen(input_ptr);

  if ((input_len == 0) || (input_len % 4) != 0)
  {
    return NULL;
  }

  output_len = (input_len / 4) * 3;

  /* Update output buffer size if input was padded with '='s
     Only 2 '=' padded bytes are allowed per 4 byte-block */
  if (input_ptr[input_len-1] == '=')
  {
    output_len--;
    if (input_ptr[input_len-2] == '=')
    {
      output_len--;
    }
  }

  output_ptr = (char *)malloc(output_len);
  if (output_ptr == NULL)
  {
    return NULL;
  }

  memset(output_ptr, 0, output_len);

  /* Decode the string & convert to binary */
  while (i <= (input_len - 4))
  {
    /* Decode each block of 4 Base64 bytes to 3 binary bytes */
    uint32_t first  = find_base64_values(1, input_ptr[i++], 0);
    uint32_t second = find_base64_values(1, input_ptr[i++], 0);
    uint32_t third  = find_base64_values(1, input_ptr[i++], 0);
    uint32_t fourth = find_base64_values(1, input_ptr[i++], 0);

    uint64_t all_three = (first  << (3 * 6)) +
                         (second << (2 * 6)) +
                         (third  << (1 * 6)) +
                         fourth;

    /* 3 binary bytes */
    if (j >= output_len)
    {
      break;
    }
    output_ptr[j++] = (all_three >> (2 * 8)) & 0xFF;

    if (j >= output_len)
    {
      break;
    }
    output_ptr[j++] = (all_three >> (1 * 8)) & 0xFF;

    if (j >= output_len)
    {
      break;
    }
    output_ptr[j++] = all_three & 0xFF;
  }

  *output_len_ptr = output_len;
  return output_ptr;
}


char* mem_alloc_bin_to_base64string
(
  unsigned char   * input_ptr,
  uint16_t          input_len
)
{
  uint8_t   i                = 0;
  uint8_t   j                = 0;
  uint8_t   extra_bytes      = 0;
  uint16_t  output_len       = 0;
  char*     output_ptr       = NULL;

  if ((input_ptr == NULL) || (input_len == 0))
  {
    return NULL;
  }

  /* Calculate the max buffer size needed for the encoded Base64 string,
     3 binary bytes make 4 Base64 bytes */
  output_len =  sizeof(char) + ((((input_len % 3 > 0) ? 1 : 0) +
                                  (input_len / 3 )) * 4);
  output_ptr = (char *)malloc(output_len);


  if (output_ptr == NULL)
  {
    return NULL;
  }

  memset(output_ptr, 0, output_len);

  /* Now encode the ASCII string to Base64 string */
  while (i < input_len)
  {
    /* Encode each block of 4 bytes from 3 ASCII bytes */
    uint32_t first  = i < input_len ? input_ptr[i++] : 0;
    uint32_t second = i < input_len ? input_ptr[i++] : 0;
    uint32_t third  = i < input_len ? input_ptr[i++] : 0;

    uint32_t all_three = (first  << (2 * 8)) +
                         (second << (1 * 8)) +
                         third;

    /* 4 Base64 bytes */
    if ((j+3) < output_len)
    {
      output_ptr[j++] = find_base64_values(
                          0, 0, (all_three >> (3 * 6)) & 0x3F);
      output_ptr[j++] = find_base64_values(
                          0, 0, (all_three >> (2 * 6)) & 0x3F);
      output_ptr[j++] = find_base64_values(
                          0, 0, (all_three >> (1 * 6)) & 0x3F);
      output_ptr[j++] = find_base64_values(
                          0, 0, all_three & 0x3F);
    }
  }

  /* Update pading if required. It is needed if ASCII string's
     last group has either 1 or 2 bytes */
  extra_bytes = input_len % 3;
  if (extra_bytes)
  {
    uint8_t bytes_to_fill = (extra_bytes == 1) ? 2 : 1;
    for (i = 1; i < 3 && bytes_to_fill; i++, --bytes_to_fill)
    {
      output_ptr[output_len - 1 - i] = '=';
    }
  }
  return output_ptr;
}

void CardServicesMenu::getCardState(std::vector<std::string> userInput) {
  //   if(card_) {
  //      telux::tel::CardState cardState;
  //      card_->getState(cardState);
  //      std::cout << "CardState : " << cardStateToString(cardState) << std::endl;
  //   }
  Status s = rilSession.simGetSimStatusReq([](RIL_Errno err,const RIL_CardStatus_v6 *cardStatusInfo) -> void {
    std::cout << "Got response for get card state " << err << std::endl;
    if(cardStatusInfo != nullptr)
    {
     std::cout << "cardStatusInfo.card_state "<<cardStatusInfo->card_state<<std::endl;
     std::cout << "cardStatusInfo.physical_slot_id "<<cardStatusInfo->physical_slot_id<<std::endl;
     if(cardStatusInfo->atr && cardStatusInfo->iccid) {
         std::cout << "cardStatusInfo.atr "<<cardStatusInfo->atr <<std::endl;
         std::cout << "cardStatusInfo.iccid "<<cardStatusInfo->iccid <<std::endl;
     }
     std::cout << "cardStatusInfo.universal_pin_state "<<cardStatusInfo->universal_pin_state <<std::endl;
     std::cout << "cardStatusInfo.gsm_umts_subscription_app_index "<<cardStatusInfo->gsm_umts_subscription_app_index <<std::endl;
     std::cout << "cardStatusInfo.cdma_subscription_app_index "<<cardStatusInfo->cdma_subscription_app_index <<std::endl;
     std::cout << "cardStatusInfo.ims_subscription_app_index "<<cardStatusInfo->ims_subscription_app_index <<std::endl;
     std::cout << "cardStatusInfo.num_applications "<<cardStatusInfo->num_applications <<std::endl;
     for(uint32_t cnt=0 ;cnt< cardStatusInfo->num_applications;cnt++)
     {
         std::cout << "cardStatusInfo.applications[cnt].app_type "<<cardStatusInfo->applications[cnt].app_type <<std::endl;
         std::cout << "cardStatusInfo.applications[cnt].app_state "<<cardStatusInfo->applications[cnt].app_state <<std::endl;
         std::cout << "cardStatusInfo.applications[cnt].perso_substate "<<cardStatusInfo->applications[cnt].perso_substate <<std::endl;
         std::cout << "cardStatusInfo.applications[cnt].aid_ptr "<<cardStatusInfo->applications[cnt].aid_ptr <<std::endl;
         std::cout << "cardStatusInfo.applications[cnt].app_label_ptr "<<cardStatusInfo->applications[cnt].app_label_ptr <<std::endl;
         std::cout << "cardStatusInfo.applications[cnt].pin1_replaced "<<cardStatusInfo->applications[cnt].pin1_replaced <<std::endl;
         std::cout << "cardStatusInfo.applications[cnt].pin1 "<<cardStatusInfo->applications[cnt].pin1 <<std::endl;
         std::cout << "cardStatusInfo.applications[cnt].pin2 "<<cardStatusInfo->applications[cnt].pin2<<std::endl;

     }
     if(cardStatusInfo->eid !=NULL) {
         std::cout << "cardStatusInfo.eid" << cardStatusInfo->eid <<std::endl;
     }
    }
  });
  std::cout << ((s == Status::SUCCESS) ? "Received Sim status response succesfully"
                                       : "Sim status response received with error")
            << std::endl;
  for (auto& arg : userInput) std::cout << __func__ << ":" << arg << std::endl;
}

// std::string CardServicesMenu::appTypeToString(telux::tel::AppType appType) {
//   std::string applicationType;
//   switch(appType) {
//      case telux::tel::AppType::APPTYPE_SIM:
//         applicationType = "SIM";
//         break;
//      case telux::tel::AppType::APPTYPE_USIM:
//         applicationType = "USIM";
//         break;
//      case telux::tel::AppType::APPTYPE_RUIM:
//         applicationType = "RUIM";
//         break;
//      case telux::tel::AppType::APPTYPE_CSIM:
//         applicationType = "CSIM";
//         break;
//      case telux::tel::AppType::APPTYPE_ISIM:
//         applicationType = "ISIM";
//         break;
//      default:
//         applicationType = "Unknown";
//         break;
//   }
//   return applicationType;
//}
//
// std::string CardServicesMenu::appStateToString(telux::tel::AppState appState) {
//   std::string applicationState;
//   switch(appState) {
//      case telux::tel::AppState::APPSTATE_DETECTED:
//         applicationState = "Detected";
//         break;
//      case telux::tel::AppState::APPSTATE_PIN:
//         applicationState = "PIN";
//         break;
//      case telux::tel::AppState::APPSTATE_PUK:
//         applicationState = "PUK";
//         break;
//      case telux::tel::AppState::APPSTATE_SUBSCRIPTION_PERSO:
//         applicationState = "Subscription Perso";
//         break;
//      case telux::tel::AppState::APPSTATE_READY:
//         applicationState = "Ready";
//         break;
//      default:
//         applicationState = "Unknown";
//         break;
//   }
//   return applicationState;
//}

void CardServicesMenu::getSupportedApps(std::vector<std::string> userInput) {
  //   if(card_) {
  //      std::vector<std::shared_ptr<telux::tel::ICardApp>> applications;
  //      applications = card_->getApplications();
  //      if(applications.size() != 0)  {
  //         for(auto cardApp : applications) {
  //            std::cout << "App type: " << appTypeToString(cardApp->getAppType()) << std::endl;
  //            std::cout << "App state: " << appStateToString(cardApp->getAppState()) << std::endl;
  //            std::cout << "AppId : " << cardApp->getAppId() << std::endl;
  //         }
  //      } else {
  //         std::cout <<"No supported applications"<< std::endl;
  //         telux::tel::CardState cardState;
  //         card_->getState(cardState);
  //         std::cout << "Card State : " << cardStateToString(cardState) << std::endl;
  //      }
  //   }  else {
  //      std::cout << "ERROR: Unable to get card instance";
  //   }
//  Status s = rilSession.simGetSimStatusReq([](RIL_Errno err) -> void {
//    std::cout << "Got response for get card state " << err << std::endl;
//  });
  for (auto& arg : userInput) std::cout << __func__ << ":" << arg << std::endl;
}

void CardServicesMenu::openLogicalChannel(std::vector<std::string> userInput) {
  //   if(card_) {
  //      std::string aid = userInput[1];
  //      std::cout << "Open logical channel with aid:" << aid << std::endl;
  //      auto ret = card_->openLogicalChannel(aid, myOpenLogicalChannelCb_);
  //      if(ret == telux::common::Status::SUCCESS) {
  //         std::cout << "Open logical channel request sent successfully \n";
  //      } else {
  //         std::cout << "Open logical channel request failed \n";
  //      }
  //   }  else {
  //      std::cout << "ERROR: Unable to get card instance";
  //   }

  RIL_OpenChannelParams open_chnl;

  open_chnl.aidPtr =  const_cast<char *>(userInput[1].c_str());
  if(!(strncmp(open_chnl.aidPtr,"0000",4))) {
      open_chnl.aidPtr = "";
  }
  open_chnl.p2      = std::stoi(userInput[2]);
  Status s = rilSession.simOpenChannelReq(open_chnl, [](RIL_Errno err ,const int *aid ,size_t len) -> void {
    std::cout << "Got response for open channel: " << err << std::endl;
    if(aid != nullptr)
    {
       std::cout << "Response Aid: " << std::to_string(*aid) << std::endl;
       std::cout << "Response len: " << len << std::endl;
    }
  });
  std::cout << ((s == Status::SUCCESS) ? "open channel request  success"
                                       : "open channel request failed  with error")
            << std::endl;
  for (auto& arg : userInput) std::cout << __func__ << ":" << arg << std::endl;
}

void CardServicesMenu::transmitApdu(std::vector<std::string> userInput) {
  //   if(card_) {
  //      int channel;
  //      int cla, instruction, p1, p2, p3;
  //      std::vector<uint8_t> data;
  //
  //      cla = 0;
  //      instruction = 0;
  //      p1 = 0;
  //      p2 = 0;
  //      p3 = 0;
  //
  //      std::cout << std::endl;
  //      std::cout << "Enter the channel : ";
  //      std::cin >> channel;
  //      Utils::validateInput(channel);
  //      std::cout << "Enter CLA : ";
  //      std::cin >> cla;
  //      Utils::validateInput(cla);
  //      std::cout << "Enter INS : ";
  //      std::cin >> instruction;
  //      Utils::validateInput(instruction);
  //      std::cout << "Enter P1 : ";
  //      std::cin >> p1;
  //      Utils::validateInput(p1);
  //      std::cout << "Enter P2 : ";
  //      std::cin >> p2;
  //      Utils::validateInput(p2);
  //      std::cout << "Enter P3 : ";
  //      std::cin >> p3;
  //      Utils::validateInput(p3);
  //      int dataInput;
  //      for(int i = 0; i < p3; i++) {
  //         std::cout << "Enter DATA (" << i + 1 << ") :";
  //         std::cin >> dataInput;
  //         Utils::validateInput(dataInput);
  //         data.emplace_back((uint8_t)dataInput);
  //      }
  //
  //      auto ret = card_->transmitApduLogicalChannel(channel, (uint8_t)cla, (uint8_t)instruction,
  //                                                   (uint8_t)p1, (uint8_t)p2, (uint8_t)p3, data,
  //                                                   myTransmitApduCb_);
  //      std::cout << (ret == telux::common::Status::SUCCESS ?"Transmit APDU request sent successfully"
  //                                                          : "Transmit APDU request failed")
  //                << '\n';
  //   }  else {
  //      std::cout << "ERROR: Unable to get card instance";
  //   }
  RIL_SIM_APDU sim_apdu;
  sim_apdu.sessionid = std::stoi(userInput[1]);
  sim_apdu.cla       = std::stoi(userInput[2]);
  sim_apdu.instruction = std::stoi(userInput[3],NULL,16);
  sim_apdu.p1          = std::stoi(userInput[4]);
  sim_apdu.p2        = std::stoi(userInput[5]);
  sim_apdu.p3        = std::stoi(userInput[6]);
  sim_apdu.data      = const_cast<char *>(userInput[7].c_str());
  Status s = rilSession.simTransmitApduChannelReq(sim_apdu, [](RIL_Errno err, const RIL_SIM_IO_Response *res) -> void {
    std::cout << "Got response for Transmit apdu channel: " << err << std::endl;
      if(res != nullptr) {
          std::cout<<"StatusWord1: "<<res->sw1<<std::endl;
          std::cout<<"Statusword2: "<<res->sw2<<std::endl;
          if(res->simResponse != nullptr)
          {
             std::cout<<"simresponse"<<std::string(res->simResponse)<<std::endl;
          }
      }
  });
  std::cout << ((s == Status::SUCCESS) ? "Received success  for Transmit apdu request"
                                       : "Received failure for Transmit apdu request")
            << std::endl;

  for (auto& arg : userInput) std::cout << __func__ << ":" << arg << std::endl;
}

void CardServicesMenu::basicTransmitApdu(std::vector<std::string> userInput) {
  //   if(card_) {
  //      int cla, instruction, p1, p2, p3;
  //      std::vector<uint8_t> data;
  //
  //      cla = 0;
  //      instruction = 0;
  //      p1 = 0;
  //      p2 = 0;
  //      p3 = 0;
  //
  //      std::string user_input;
  //      std::cout << std::endl;
  //      std::cout << "Enter CLA : ";
  //      std::cin >> cla;
  //      Utils::validateInput(cla);
  //      std::cout << "Enter INS : ";
  //      std::cin >> instruction;
  //      Utils::validateInput(instruction);
  //      std::cout << "Enter P1 : ";
  //      std::cin >> p1;
  //      Utils::validateInput(p1);
  //      std::cout << "Enter P2 : ";
  //      std::cin >> p2;
  //      Utils::validateInput(p2);
  //      std::cout << "Enter P3 : ";
  //      std::cin >> p3;
  //      Utils::validateInput(p3);
  //      int tmpInp;
  //      for(int i = 0; i < p3; i++) {
  //         std::cout << "Enter DATA (" << i + 1 << ") :";
  //         std::cin >> tmpInp;
  //         Utils::validateInput(tmpInp);
  //         data.emplace_back((uint8_t)tmpInp);
  //      }
  //      auto ret = card_->transmitApduBasicChannel((uint8_t)cla, (uint8_t)instruction, (uint8_t)p1,
  //                                                 (uint8_t)p2, (uint8_t)p3, data, myTransmitApduCb_);
  //      if(ret == telux::common::Status::SUCCESS) {
  //         std::cout << "Basic transmit APDU request sent successfully\n";
  //      } else {
  //         std::cout << "Basic transmit APDU request failed\n";
  //      }
  //   }  else {
  //      std::cout << "ERROR: Unable to get card instance";
  //   }

  RIL_SIM_APDU sim_apdu;
  sim_apdu.sessionid = std::stoi(userInput[1]);
  sim_apdu.cla       = std::stoi(userInput[2]);
  sim_apdu.instruction = std::stoi(userInput[3],NULL,16);
  sim_apdu.p1          = std::stoi(userInput[4]);
  sim_apdu.p2        = std::stoi(userInput[5]);
  sim_apdu.p3        = std::stoi(userInput[6]);
  sim_apdu.data      = const_cast<char *>(userInput[7].c_str());
  Status s = rilSession.simTransmitApduBasicReq(sim_apdu, [](RIL_Errno err,const RIL_SIM_IO_Response *res) -> void {
    std::cout << "Got response for basic transmit apdu: "  << err << std::endl;
      if(res != nullptr) {
          std::cout<<"StatusWord1: "<<res->sw1<<std::endl;
          std::cout<<"Statusword2: "<<res->sw2<<std::endl;
          if(res->simResponse != nullptr)
          {
             std::cout<<"simresponse"<<std::string(res->simResponse)<<std::endl;
          }
      }
  });

  std::cout << ((s == Status::SUCCESS) ? "Transmit apdu request success"
                                       : "Transmit apdu request failed")
            << std::endl;
  for (auto& arg : userInput) std::cout << __func__ << ":" << arg << std::endl;
}

void CardServicesMenu::closeLogicalChannel(std::vector<std::string> userInput) {
  //   if(card_) {
  //      int channel = std::stoi(userInput[1]);
  //      std::cout << "Close logical channel with channel:" << channel << std::endl;
  //      auto ret = card_->closeLogicalChannel(channel, myCloseLogicalChannelCb_);
  //      if(ret == telux::common::Status::SUCCESS) {
  //         std::cout << "Close logical channel request sent successfully \n";
  //      } else {
  //         std::cout << "Close logical channel request failed \n";
  //      }
  //   }  else {
  //      std::cout << "ERROR: Unable to get card instance";
  //   }
  int session_id;
  session_id = std::stoi(userInput[1]);
  Status s = rilSession.simCloseChannelReq(session_id, [](RIL_Errno err) -> void {
    std::cout << "Got response for close channel: " << err << std::endl;
  });
  std::cout << ((s == Status::SUCCESS) ? "Received Success for close channel request"
                                       : "Received Failure for close channel request")
            << std::endl;
  for (auto& arg : userInput) std::cout << __func__ << ":" << arg << std::endl;
}

void CardServicesMenu::changeCardPin(std::vector<std::string> userInput) {
  //   std::string oldPin, newPin, lockType;
  //   telux::tel::CardLockType cardLockType;
  //   char delimiter = '\n';
  //
  //   if(!card_) {
  //      std::cout << "ERROR: Unable to get card instance";
  //      return;
  //   }
  //   std::cout << "Enter 1-PIN1/2-PIN2 : ";
  //   std::getline(std::cin, lockType, delimiter);
  //   // Validating user input
  //   if(!lockType.empty()) {
  //      try {
  //         std::stoi(lockType);
  //      } catch(const std::exception &e) {
  //         std::cout << "ERROR: invalid input, please enter numerical values " << lockType
  //                   << std::endl;
  //      }
  //   } else {
  //      std::cout << "empty input going with default PIN1\n";
  //      cardLockType = telux::tel::CardLockType::PIN1;
  //   }
  //
  //   if(lockType.compare("1") == 0) {
  //      cardLockType = telux::tel::CardLockType::PIN1;
  //   } else if(lockType.compare("2") == 0) {
  //      cardLockType = telux::tel::CardLockType::PIN2;
  //   } else {
  //      std::cout << "wrong input, going with default PIN1\n";
  //      cardLockType = telux::tel::CardLockType::PIN1;
  //   }
  //   std::cout << "Enter old PIN: ";
  //   std::getline(std::cin, oldPin, delimiter);
  //
  //   std::cout << "Enter new PIN: ";
  //   std::getline(std::cin, newPin, delimiter);
  //
  //   std::vector<std::shared_ptr<telux::tel::ICardApp>> applications;
  //   applications = card_->getApplications();
  //   if(applications.size() != 0)  {
  //      for(auto cardApp : applications) {
  //         if((cardApp->getAppType() == telux::tel::AppType::APPTYPE_USIM)
  //            && (cardApp->getAppState() == telux::tel::AppState::APPSTATE_READY)) {
  //            auto ret
  //               = cardApp->changeCardPassword(cardLockType, oldPin, newPin,
  //                  &ChangeCardPinResponseCb);
  //            if(ret == telux::common::Status::SUCCESS) {
  //               std::cout << "Change card PIN request sent successfully\n";
  //            } else {
  //               std::cout << "Change card PIN request failed\n";
  //            }
  //         }
  //      }
  //   } else {
  //         std::cout <<"Change card PIN request failed"<< std::endl;
  //         telux::tel::CardState cardState;
  //         card_->getState(cardState);
  //         std::cout << "Card State : " << cardStateToString(cardState) << std::endl;
  //   }

  const char *new_pinp[3] = {};
  new_pinp[0] =const_cast<char*>(userInput[1].c_str());
  new_pinp[1] =const_cast<char*>(userInput[2].c_str());
  new_pinp[2] =const_cast<char*>(userInput[3].c_str());
  if(!(strncmp(new_pinp[2],"0000",4))) {
      new_pinp[2] = "";
  }
  Status s = rilSession.simChangePinReq(new_pinp, [](RIL_Errno err, int *res) -> void {
    std::cout << "Got response for change pin : " << err << std::endl;
    if(res!=NULL)
    {
       std::cout << "Response for Change Pin "<< std::to_string(*res)<<std::endl;
    }
  });
  std::cout << ((s == Status::SUCCESS) ? "Received Success for change pin request"
                                       : "change pin request received with error")
            << std::endl;
  for (auto& arg : userInput) std::cout << __func__ << ":" << arg << std::endl;
}

void CardServicesMenu::changeCardPin2(std::vector<std::string> userInput) {

  const char *new_pinp[3] = {};
  new_pinp[0] =const_cast<char*>(userInput[1].c_str());
  new_pinp[1] =const_cast<char*>(userInput[2].c_str());
  new_pinp[2] =const_cast<char*>(userInput[3].c_str());
  if(!(strncmp(new_pinp[2],"0000",4))) {
      new_pinp[2] = "";
  }
  Status s = rilSession.simChangePin2Req(new_pinp, [](RIL_Errno err, int *res) -> void {
    std::cout << "Got response for change pin2 : " << err << std::endl;
    if(res!=NULL)
    {
       std::cout << "Response for Change Pin2 "<< std::to_string(*res)<<std::endl;
    }
  });
  std::cout << ((s == Status::SUCCESS) ? "Received Success forchange pin2 request"
                                       : "Received Failure response for change pin2 request")
            << std::endl;
  for (auto& arg : userInput) std::cout << __func__ << ":" << arg << std::endl;
}

void CardServicesMenu::unlockCardByPuk(std::vector<std::string> userInput) {
  //   std::string puk, newPin, lockType;
  //   telux::tel::CardLockType cardLockType;
  //   char delimiter = '\n';
  //
  //   if(!card_) {
  //      std::cout << "ERROR: Unable to get card instance";
  //      return;
  //   }
  //   std::cout << "Enter 1-PUK1/2-PUK2 : ";
  //   std::getline(std::cin, lockType, delimiter);
  //   if(!lockType.empty()) {
  //      try {
  //         std::stoi(lockType);
  //      } catch(const std::exception &e) {
  //         std::cout << "ERROR: invalid input, please enter numerical values " << lockType
  //                   << std::endl;
  //      }
  //   } else {
  //      std::cout << "empty input going with default PUK1\n";
  //      cardLockType = telux::tel::CardLockType::PUK1;
  //   }
  //
  //   if(lockType.compare("1") == 0) {
  //      cardLockType = telux::tel::CardLockType::PUK1;
  //   } else if(lockType.compare("2") == 0) {
  //      cardLockType = telux::tel::CardLockType::PUK2;
  //   } else {
  //      std::cout << "As the user entered wrong input, setting default lock type value 'PUK1'\n";
  //      cardLockType = telux::tel::CardLockType::PUK1;
  //   }
  //   std::cout << "Enter PUK: ";
  //   std::getline(std::cin, puk, delimiter);
  //
  //   std::cout << "Enter New PIN: ";
  //   std::getline(std::cin, newPin, delimiter);
  //
  //   std::vector<std::shared_ptr<telux::tel::ICardApp>> applications;
  //   applications = card_->getApplications();
  //   if(applications.size() != 0)  {
  //      for(auto cardApp : applications) {
  //         if((cardApp->getAppType() == telux::tel::AppType::APPTYPE_USIM)
  //            && (cardApp->getAppState() == telux::tel::AppState::APPSTATE_PUK)) {
  //            auto ret = cardApp->unlockCardByPuk(cardLockType, puk, newPin,
  //               &unlockCardByPukResponseCb);
  //            if(ret == telux::common::Status::SUCCESS) {
  //               std::cout << "Unlock card by PUK request sent successfully\n";
  //            } else {
  //               std::cout << "Unlock card by PUK request failed\n";
  //            }
  //         }
  //      }
  //   } else {
  //         std::cout <<"Unlock card by PUK request failed"<< std::endl;
  //         telux::tel::CardState cardState;
  //         card_->getState(cardState);
  //         std::cout << "Card State : " << cardStateToString(cardState) << std::endl;
  //   }
  const char *new_puk[3] = {};
  new_puk[0] = const_cast<char*>(userInput[1].c_str());
  new_puk[1] = const_cast<char*>(userInput[2].c_str());
  new_puk[2] = const_cast<char*>(userInput[3].c_str());
  if(!strncmp(new_puk[2],"0000",4)) {
      new_puk[2] = "";
  }
  Status s = rilSession.simEnterPukReq(new_puk, [](RIL_Errno err ,int *res) -> void {
    std::cout << "Got response for enter puk req : " << err << std::endl;
    if(res!=NULL)
    {
       std::cout << "Response for Enter Puk "<< std::to_string(*res)<<std::endl;
    }
  });
  std::cout << ((s == Status::SUCCESS) ? "Received Success for unblock card by puk request"
                                       : "Received Failure for unblock card by puk request")
            << std::endl;
  for (auto& arg : userInput) std::cout << __func__ << ":" << arg << std::endl;
}

void CardServicesMenu::unlockCardByPuk2(std::vector<std::string> userInput) {
  const char *new_puk[3] = {};
  new_puk[0] = const_cast<char*>(userInput[1].c_str());
  new_puk[1] = const_cast<char*>(userInput[2].c_str());
  new_puk[2] = const_cast<char*>(userInput[3].c_str());
  if(!strncmp(new_puk[2],"0000",4)){
      new_puk[2] = "";
  }
  Status s = rilSession.simEnterPuk2Req(new_puk, [](RIL_Errno err ,int *res) -> void {
    std::cout << "Got response for enter puk2 : " << err << std::endl;
    if(res!=NULL)
    {
       std::cout << "Response for Enter Puk2 "<< std::to_string(*res)<<std::endl;
    }
  });
  std::cout << ((s == Status::SUCCESS) ? "Received Success for unblock card by puk2 request"
                                       : "Received Success for unblock card by puk request")
            << std::endl;
  for (auto& arg : userInput) std::cout << __func__ << ":" << arg << std::endl;
}
void CardServicesMenu::unlockCardByPin(std::vector<std::string> userInput) {
  //   std::string newPin, lockType;
  //   telux::tel::CardLockType cardLockType;
  //   char delimiter = '\n';
  //
  //   if(!card_) {
  //      std::cout << "ERROR: Unable to get card instance";
  //      return;
  //   }
  //   std::cout << "Enter the 1-PIN1/2-PIN2: ";
  //   std::getline(std::cin, lockType, delimiter);
  //   if(!lockType.empty()) {
  //      try {
  //         std::stoi(lockType);
  //      } catch(const std::exception &e) {
  //         std::cout << "ERROR: invalid input, please enter numerical values " << lockType
  //                   << std::endl;
  //      }
  //   } else {
  //      std::cout << "empty input going with default PIN1\n";
  //      cardLockType = telux::tel::CardLockType::PIN1;
  //   }
  //
  //   if(lockType.compare("1") == 0) {
  //      cardLockType = telux::tel::CardLockType::PIN1;
  //   } else if(lockType.compare("2") == 0) {
  //      cardLockType = telux::tel::CardLockType::PIN2;
  //   } else {
  //      std::cout << "As the user entered wrong option, setting default lock type value 'PIN1'\n";
  //      cardLockType = telux::tel::CardLockType::PIN1;
  //   }
  //
  //   std::cout << "Enter PIN: ";
  //   std::getline(std::cin, newPin, delimiter);
  //
  //   std::vector<std::shared_ptr<telux::tel::ICardApp>> applications;
  //   applications = card_->getApplications();
  //   if(applications.size() != 0)  {
  //      for(auto cardApp : applications) {
  //         if(cardApp->getAppType() == telux::tel::AppType::APPTYPE_USIM) {
  //            auto ret = cardApp->unlockCardByPin(cardLockType, newPin,
  //            &unlockCardByPinResponseCb); if(ret == telux::common::Status::SUCCESS) {
  //               std::cout << "Unlock card by pin request sent successfully\n";
  //            } else {
  //               std::cout << "Unlock card by pin request failed\n";
  //            }
  //         }
  //      }
  //   } else {
  //         std::cout <<"Unlock card by PIN request failed\n"<< std::endl;
  //         telux::tel::CardState cardState;
  //         card_->getState(cardState);
  //         std::cout << "Card State : " << cardStateToString(cardState) << std::endl;
  //   }
  const char *unblock_pin[2] = {};
  unblock_pin[0] = const_cast<char*>(userInput[1].c_str());
  unblock_pin[1] = const_cast<char*>(userInput[2].c_str());
  if(!(strncmp(unblock_pin[1],"0000",4))) {
     unblock_pin[1] = "";
  }
  Status s = rilSession.simEnterPinReq(unblock_pin, [](RIL_Errno err ,int *res) -> void {
    std::cout << "Got response for unblock pin : " << err << std::endl;
    if(res!=NULL)
    {
       std::cout << "Response for Unblock Pin "<< std::to_string(*res)<<std::endl;
    }
  });
  std::cout << ((s == Status::SUCCESS) ? "Received Sucess for unblock_pin request"
                                       : "Received Failure for unblock_pin request")
            << std::endl;
  for (auto& arg : userInput) std::cout << __func__ << ":" << arg << std::endl;
}

void CardServicesMenu::unlockCardByPin2(std::vector<std::string> userInput) {
  const char *unblock_pin2[2] = {};
  unblock_pin2[0] = const_cast<char*>(userInput[1].c_str());
  unblock_pin2[1] = const_cast<char*>(userInput[2].c_str());
  if(!strncmp(unblock_pin2[1],"0000",4)) {
      unblock_pin2[1] = "";
  }
  Status s = rilSession.simEnterPin2Req(unblock_pin2, [](RIL_Errno err ,int *res) -> void {
    std::cout << "Got response for unblock pin2 : " << err << std::endl;
    if(res!=NULL)
    {
       std::cout << "Response for Unblock Pin2 "<< std::to_string(*res)<<std::endl;
    }
  });
  std::cout << ((s == Status::SUCCESS) ? "Received Sucess for unblock_pin2 request"
                                       : "Received Failure for unblock_pin2 request")
            << std::endl;
  for (auto& arg : userInput) std::cout << __func__ << ":" << arg << std::endl;
}
void CardServicesMenu::queryPin1LockState(std::vector<std::string> userInput) {
  //   if(card_) {
  //      std::vector<std::shared_ptr<telux::tel::ICardApp>> applications;
  //      applications = card_->getApplications();
  //      if(applications.size() != 0)  {
  //         for(auto cardApp : applications) {
  //            if(cardApp->getAppType() == telux::tel::AppType::APPTYPE_USIM) {
  //               auto ret = cardApp->queryPin1LockState(queryPin1LockResponseCb);
  //               if(ret == telux::common::Status::SUCCESS) {
  //                  std::cout << "Query pin1 lock state request sent successfully\n";
  //               } else {
  //                  std::cout << "Query pin1 lock state request failed\n";
  //               }
  //            }
  //         }
  //      } else {
  //         std::cout <<"Query pin1 lock state request failed"<< std::endl;
  //         telux::tel::CardState cardState;
  //         card_->getState(cardState);
  //         std::cout << "Card State : " << cardStateToString(cardState) << std::endl;
  //      }
  //   } else {
  //      std::cout << "ERROR: Unable to get card instance";
  //   }
  for (auto& arg : userInput) std::cout << __func__ << ":" << arg << std::endl;
}

void CardServicesMenu::queryFdnLockState(std::vector<std::string> userInput) {
  //   if(card_) {
  //      std::vector<std::shared_ptr<telux::tel::ICardApp>> applications;
  //      applications = card_->getApplications();
  //      if(applications.size() != 0)  {
  //         for(auto cardApp : applications) {
  //            if(cardApp->getAppType() == telux::tel::AppType::APPTYPE_USIM) {
  //               auto ret = cardApp->queryFdnLockState(&queryFdnLockResponseCb);
  //               if(ret == telux::common::Status::SUCCESS) {
  //                  std::cout << "Query FDN lock state request sent successfully\n";
  //               } else {
  //                  std::cout << "Query FDN lock state request failed\n";
  //               }
  //            }
  //         }
  //      } else {
  //         std::cout <<"Query FDN lock state request failed"<< std::endl;
  //         telux::tel::CardState cardState;
  //         card_->getState(cardState);
  //         std::cout << "Card State : " << cardStateToString(cardState) << std::endl;
  //      }
  //   } else {
  //      std::cout << "ERROR: Unable to get card instance";
  //   }

  const char *quert_fci_lock[4] ={};
  quert_fci_lock[0] = const_cast<char*>(userInput[1].c_str());
  quert_fci_lock[1] = const_cast<char*>(userInput[2].c_str());
  quert_fci_lock[2] = const_cast<char*>(userInput[3].c_str());
  quert_fci_lock[3] = const_cast<char*>(userInput[4].c_str());
  if(!strncmp(quert_fci_lock[3],"0000",4)) {
     quert_fci_lock[3] = "";
  }
  Status s = rilSession.simQueryFacilityLockReq(quert_fci_lock, [](RIL_Errno err, int32_t *res) -> void {
    std::cout << "Got response for Query Fdn lock req : " << err << std::endl;
    if(res!=NULL)
    {
       std::cout << "Response for queryFdn lock "<< std::to_string(*res)<<std::endl;
    }
  });
  std::cout << ((s == Status::SUCCESS) ? "Received Success for query facility lock request"
                                       : "Received Failure for query facility lockn request")
            << std::endl;
  for (auto& arg : userInput) std::cout << __func__ << ":" << arg << std::endl;
}

void CardServicesMenu::setCardLock(std::vector<std::string> userInput) {
  //   std::string pwd, isEnable, lockType;
  //   telux::tel::CardLockType cardLockType;
  //   char delimiter = '\n';
  //
  //   if(!card_) {
  //      std::cout << "ERROR: Unable to get card instance";
  //      return;
  //   }
  //   std::cout << "Enter the 1-PIN1/2-FDN : ";
  //   std::getline(std::cin, lockType, delimiter);
  //   if(!lockType.empty()) {
  //      try {
  //         std::stoi(lockType);
  //      } catch(const std::exception &e) {
  //         std::cout << "ERROR: invalid input, please enter numerical values " << lockType
  //                   << std::endl;
  //      }
  //   } else {
  //      std::cout << "empty input going with default PIN1\n";
  //      cardLockType = telux::tel::CardLockType::PIN1;
  //   }
  //
  //   if(lockType.compare("1") == 0) {
  //      cardLockType = telux::tel::CardLockType::PIN1;
  //   } else if(lockType.compare("2") == 0) {
  //      cardLockType = telux::tel::CardLockType::FDN;
  //   } else {
  //      std::cout << "As the user entered wrong option, going with default card lock as PIN1\n";
  //      cardLockType = telux::tel::CardLockType::PIN1;
  //   }
  //
  //   std::cout << "Enter PIN: ";
  //   std::getline(std::cin, pwd, delimiter);
  //
  //   std::cout << "Enter 1-Enable/0-Disable: ";
  //   std::getline(std::cin, isEnable, delimiter);
  //   bool opt = false;
  //   if(!isEnable.empty()) {
  //      try {
  //         int value = std::stoi(isEnable);
  //         if(value != 0 && value != 1) {
  //            std::cout << "As user entered wrong input, taking default input as 'enable'\n";
  //            opt = true;
  //         } else {
  //            opt = value;
  //         }
  //      } catch(const std::exception &e) {
  //         std::cout << "ERROR: invalid input, please enter numerical values " << opt << std::endl;
  //      }
  //   } else {
  //      std::cout << "As user entered empty input, taking default input as 'enable'\n";
  //      opt = true;
  //   }
  //
  //   std::vector<std::shared_ptr<telux::tel::ICardApp>> applications;
  //   applications = card_->getApplications();
  //   if(applications.size() != 0)  {
  //      for(auto cardApp : applications) {
  //         if(cardApp->getAppType() == telux::tel::AppType::APPTYPE_USIM) {
  //            auto ret = cardApp->setCardLock(cardLockType, pwd, opt, &setCardLockResponseCb);
  //            if(ret == telux::common::Status::SUCCESS) {
  //               std::cout << "Set card lock request sent successfully\n";
  //            } else {
  //               std::cout << "Set card lock request failed\n";
  //            }
  //         }
  //      }
  //   } else {
  //         std::cout <<"Set card lock request failed"<< std::endl;
  //         telux::tel::CardState cardState;
  //         card_->getState(cardState);
  //         std::cout << "Card State : " << cardStateToString(cardState) << std::endl;
  //   }
  const char *set_fci_lock[5] ={};
  set_fci_lock[0] = const_cast<char *>(userInput[1].c_str());
  set_fci_lock[1] = const_cast<char *>(userInput[2].c_str());
  set_fci_lock[2] = const_cast<char *>(userInput[3].c_str());
  set_fci_lock[3] = const_cast<char *>(userInput[4].c_str());
  set_fci_lock[4] = const_cast<char *>(userInput[5].c_str());
  if(!strncmp(set_fci_lock[4],"0000",4)) {
      set_fci_lock[4] = "";
  }
  Status s = rilSession.simSetFacilityLockReq(set_fci_lock, [](RIL_Errno err ,int *res) -> void {
    std::cout << "Got response for set facility lock : " << err << std::endl;
    if(res!=NULL)
    {
       std::cout << "Response for set facility req "<< std::to_string(*res)<<std::endl;
    }
  });
  std::cout << ((s == Status::SUCCESS) ? "Received Success for set facility lock request"
                                       : "Received Failure for set facility lock request")
            << std::endl;
  for (auto& arg : userInput) std::cout << __func__ << ":" << arg << std::endl;
}

void CardServicesMenu::getImsirequest(std::vector<std::string> userInput) {
    const char *aid;
    aid = const_cast<char *>(userInput[1].c_str());
    if(!strncmp(aid,"0000",4)) {
        aid = "";
    }
    Status s = rilSession.simGetImsiReq(aid, [](RIL_Errno err ,const char *res) -> void {
      std::cout << "Got response for get imsi request : " << err << std::endl;
      if(res!=NULL)
      {
         std::cout << "IMSI value: "<< std::string(res)<<std::endl;
      }
    });
    std::cout << ((s == Status::SUCCESS) ? "Received Sucess response for imsi request"
                                         : "Received Failure response for imsi request")
              << std::endl;
}

void CardServicesMenu::setSimIoReq(std::vector<std::string> userInput) {

  RIL_SIM_IO_v6 Io_Req_params;

  Io_Req_params.command = std::stoi(userInput[1],NULL,16);
  Io_Req_params.fileid  = std::stoi(userInput[2],NULL,16);
  Io_Req_params.path    = const_cast<char *>(userInput[3].c_str());
  Io_Req_params.p1      = std::stoi(userInput[4]);
  Io_Req_params.p2      = std::stoi(userInput[5]);
  Io_Req_params.p3      = std::stoi(userInput[6]);
  Io_Req_params.data    = const_cast<char *>(userInput[7].c_str());
  Io_Req_params.pin2 = const_cast<char*>(userInput[8].c_str());
  Io_Req_params.aidPtr = const_cast<char*>(userInput[9].c_str());
  if(!strncmp(Io_Req_params.aidPtr,"0000",4)) {
      Io_Req_params.aidPtr = "";
  }
  Status s = rilSession.simIOReq(Io_Req_params, [](RIL_Errno err ,const RIL_SIM_IO_Response *res) -> void {
    std::cout << "Got response for get simIO request : " <<std::endl;
      if(res != nullptr) {
          std::cout<<"StatusWord1: "<<res->sw1<<std::endl;
          std::cout<<"Statusword2: "<<res->sw2<<std::endl;
          if(res->simResponse != nullptr)
          {
             std::cout<<"simresponse: "<<std::string(res->simResponse)<<std::endl;
          }
      }
  });
  std::cout << ((s == Status::SUCCESS) ? "Received Sucess response for SimIo request"
                                       : "Received Failure response for SimIo request")
            << std::endl;
}
void CardServicesMenu::enveloperequest(std::vector<std::string> userInput) {
    const char *aid;
    aid = const_cast<char *>(userInput[1].c_str());
    if(!strncmp(aid,"0000",4)) {
        aid = "";
    }
    Status s = rilSession.simStkSendEnvelopeWithStatusReq(aid, [](RIL_Errno err ,const RIL_SIM_IO_Response *res) -> void {
      std::cout << "Got response for envelope status request : " << err << std::endl;
      if(res != nullptr) {
          std::cout<<"StatusWord1: "<<res->sw1<<std::endl;
          std::cout<<"Statusword2: "<<res->sw2<<std::endl;
          if(res->simResponse != nullptr)
          {
             std::cout<<"\nsimresponse: "<<std::string(res->simResponse)<<std::endl;
          }
      }
    });
    std::cout << ((s == Status::SUCCESS) ? "Received Sucess response for envelope status request"
                                         : "Received Failure response for envelope status request")
              << std::endl;
}
void CardServicesMenu::envelopecommand(std::vector<std::string> userInput) {
    const char *command;
    size_t len;
    command = const_cast<char *>(userInput[1].c_str());
    len = strlen(command) + 1;
    Status s = rilSession.StkSendEnvelopeCommand(command,len, [](RIL_Errno err ,const char *res,size_t size) -> void {
      std::cout << "Got response for envelope command request : " << err << std::endl;
      if((res != NULL)&&(size > 0))
      {
          std::cout<<"Response data :"<<std::string(res)<<std::endl;
          std::cout<<"Length of the response: "<<size<<std::endl;
      }
    });
    std::cout << ((s == Status::SUCCESS) ? "Received Sucess response for envelope command request"
                                         : "Received Failure response for envelope command request")
              << std::endl;
}
void CardServicesMenu::Terminalresp_command(std::vector<std::string> userInput) {
    const char *command;
    size_t len;
    command = const_cast<char *>(userInput[1].c_str());
    len = strlen(command) + 1;
    Status s = rilSession.StkSendTerminalResponse(command,len, [](RIL_Errno err ) -> void {
      std::cout << "Got response for Terminal response command request : " << err << std::endl;
    });
    std::cout << ((s == Status::SUCCESS) ? "Received Sucess response for Terminal Response command request"
                                         : "Received Failure response for Terminal command request")
              << std::endl;
}
void CardServicesMenu::CallSetup_command(std::vector<std::string> userInput) {
    int command;
    command = std::stoi(userInput[1],NULL,16);
    Status s = rilSession.StkHandleCallSetupRequestedFromSim(command, [](RIL_Errno err ) -> void {
      std::cout << "Got response for callsetuprequestedrequest : " << err << std::endl;
    });
    std::cout << ((s == Status::SUCCESS) ? "Received Sucess response for CallSetupRequested from sim request"
                                         : "Received Failure response for Callsetuprequestedfromsim request")
              << std::endl;
}
void CardServicesMenu::ServiceRunning_command(std::vector<std::string> userInput) {
  Status s = rilSession.ReportStkServiceIsRunning([](RIL_Errno err) -> void {
     std::cout << "Got response for Stk service running req " << err << std::endl;
  });
  std::cout << ((s == Status::SUCCESS) ? "Received Stk service running  response succesfully"
                                       : "Received Stk service running with  error")
            << std::endl;
  for (auto& arg : userInput) std::cout << __func__ << ":" << arg << std::endl;
}
void CardServicesMenu::IsimAuthenticate_command(std::vector<std::string> userInput) {
  bytestring bytes;
  const char *encoded_data = const_cast<char *>(userInput[1].c_str());
  uint16_t str_len = strlen(encoded_data)/2;
  char *command;
  bytes_from_hex(encoded_data, &bytes);
  command = mem_alloc_bin_to_base64string(bytes.data,str_len);
  Status s = rilSession.simIsimAuthenticationReq(command,[](RIL_Errno err,const char *res) -> void {
     std::cout << "Got response for IsimAuthenticate req " << err << std::endl;
     if(res!=NULL && (strlen(res)>0))
     {
         char *decoded_data;
         char *decoded_bin_data;
         uint16_t dec_len;
         decoded_bin_data = mem_alloc_base64string_to_bin(res,&dec_len);
         decoded_data = mem_alloc_bin_to_hexstring(decoded_bin_data,dec_len);
         if(decoded_data != nullptr) {
             std::cout<<"simresponse: "<<std::string(decoded_data)<<std::endl;
         }
     }
  });
  std::cout << ((s == Status::SUCCESS) ? "Received  Success for Isimauthenticate"
                                       : "Received Failure for Isimauthticate")
            << std::endl;
  for (auto& arg : userInput) std::cout << __func__ << ":" << arg << std::endl;
}
void CardServicesMenu::simAuthenticate_command(std::vector<std::string> userInput) {
  RIL_SimAuthentication params;
  const char *raw_data;
  bytestring bytes;
  params.authContext = std::stoi(userInput[1],NULL,16);
  raw_data= const_cast<char *>(userInput[2].c_str());
  params.aid = const_cast<char *>(userInput[3].c_str());
  if(!strncmp(params.aid,"0000",4)) {
      params.aid = "";
  }
  uint16_t str_len = strlen(raw_data)/2;
  bytes_from_hex(raw_data,&bytes);
  params.authData = mem_alloc_bin_to_base64string(bytes.data,str_len);
  if(params.authData != nullptr) {
      printf("encoded data is %s",params.authData);
  }
  Status s = rilSession.simAuthenticationReq(params,[](RIL_Errno err, const RIL_SIM_IO_Response *res) -> void {
     std::cout << "Got response for simAuthenticate req " << std::endl;
      if(res != nullptr) {
          std::cout<<"StatusWord1: "<<res->sw1<<std::endl;
          std::cout<<"Statusword2: "<<res->sw2<<std::endl;
          if(res->simResponse != nullptr && (strlen(res->simResponse) > 0))
          {
             uint16_t dec_len ;
             char *decoded_bin_data ;
             char *decoded_data;
             decoded_bin_data = mem_alloc_base64string_to_bin(res->simResponse,&dec_len);
             decoded_data = mem_alloc_bin_to_hexstring(decoded_bin_data,dec_len);
             if(decoded_data != nullptr) {
                 std::cout<<"simresponse: "<<std::string(decoded_data)<<std::endl;
             }
          }
      }
  });
  std::cout << ((s == Status::SUCCESS) ? "Received success for  simauthenticate request"
                                       : "Received failure simauthticate request")
            << std::endl;
  for (auto& arg : userInput) std::cout << __func__ << ":" << arg << std::endl;
}
void CardServicesMenu::NwDeperso_command(std::vector<std::string> userInput) {
  const char *command[1] = {};
  command[0] = const_cast<char *>(userInput[1].c_str());
  Status s = rilSession.EnterNetworkDepersonalization(command,[](RIL_Errno err,int32_t *res) -> void {
     std::cout << "Got response for Network Deperso req " << err << std::endl;
     if(res != nullptr)
     {
         std::cout << "Response for Network Personalization " << std::to_string(*res)<< std::endl;
     }
  });
  std::cout << ((s == Status::SUCCESS) ? "Received  Success for Networkdeperso request"
                                       : "Received Failure for Networkdeperso request")
            << std::endl;
  for (auto& arg : userInput) std::cout << __func__ << ":" << arg << std::endl;
}
void CardServicesMenu::registerForIndications(std::vector<std::string> userInput) {
   rilSession.registerStkUnsolSessionEnd(
      [] () {
          std::cout<<"[CardServicesMenu]: Received Stk SessionEnd Indications";
      }
  );
  rilSession.registerStkUnsolProactiveCommand(
      [](const char *res) {
        std::cout << "Received Proactive Commanfd Notification." << std::endl;
        if (res) {
          std::cout << std::string(res)<<std::endl;
        }
      });

  rilSession.registerStkEventNotify(
      [] (const char *res ) {
          std::cout<<"[CardServicesMenu]: Received Stk Event Notifiy Indications";
          if(res)
          {
            std::cout<<std::string(res)<<std::endl;
          }
      }
  );
  rilSession.registerStkCallSetup(
      [] (const int &res ) {
          std::cout<<"[CardServicesMenu]: Received Stk Call Set Up Indications"<< "message"<<res<<std::endl;
      }
  );
  rilSession.registerStkCcAlphaNotify(
      [] (const char *res ) {
          std::cout<<"[CardServicesMenu]: Received Stk  Event AlphaNotify Indications"<<std::endl;
          if(res)
          {
            std::cout<<std::string(res)<<std::endl;
          }
      }
  );
  rilSession.registerSimRefresh(
      [] (const RIL_SimRefreshResponse_v7 *res ) {
          std::cout<<"[CardServicesMenu]: Received Sim Refresh Indications"<<std::endl;
          if(res)
          {
            std::cout<<"result"<<res->result<<std::endl;
            std::cout<<"ef_id"<<std::to_string(res->ef_id)<<std::endl;
            std::cout<<"aid" << std::string(res->aid)<<std::endl;
          }
      }
  );
  rilSession.registerSimStatusChanged(
      [] () {
          std::cout<<"[CardServicesMenu]: Received Sim Status Changed Indication"<<std::endl;
      }
  );

  rilSession.registerAdnInitDone(
      [] () {
          std::cout<<"[CardServicesMenu]: Received Adn Init Done Indication"<<std::endl;
      }
  );

  rilSession.registerAdnRecords(
      [] () {
          std::cout<<"[CardServicesMenu]: Received Adn Records Indication" << std::endl;
      }
  );

  rilSession.registerAddProfProgressIndicationHandler(
              [](const UimLpaAddProfProgressInd *ind) {
          std::cout<<"[CardServicesMenu]: Received Add profile Indications"<<std::endl;
          if(ind != nullptr) {
              std::cout<<"[CardServicesMenu]: Profile status"<<ind->status<<std::endl;
              std::cout<<"[CardServicesMenu]: Profile cause"<<ind->cause<<std::endl;
              std::cout<<"[CardServicesMenu]: Profile progress "<<ind->progress<<std::endl;
              std::cout<<"[CardServicesMenu]: Profile policyMask "<<ind->policyMask<<std::endl;
              if(ind->profileName){
                  std::cout<<"[CardServicesMenu]: Profile pfile_name "<<ind->profileName<<std::endl;
              }
          }
      }
  );
  rilSession.registerLpaHttpsTxnIndicationHandler(
              [](const UimLpaHttpTxnIndype *ind) {
          std::cout<<"[CardServicesMenu]: Received Https Indications"<<std::endl;
          char *dummy_payload,*payload_data;
          std::string postresponse;
          int i,httpResult;
          if(ind != nullptr){
              std::cout<<"[CardServicesMenu]: Https indication tokenId"<<ind->tokenId<<std::endl;
              dummy_payload = (char *)ind->payload;
              payload_data = mem_alloc_bin_to_hexstring(dummy_payload,ind->payload_len);
              if (payload_data != NULL) {
                std::cout << "[CardServicesMenu]: Https indication payload"
                          << std::string(payload_data) << std::endl;
              }
              std::cout<<"[CardServicesMenu]: Https indication payload_len"<<ind->payload_len<<std::endl;
              std::cout<<"[CardServicesMenu]: Https indication contentType"<<ind->contentType<<std::endl;
              for(i=0;i<ind->no_of_headers;i++) {
                  std::cout<<"[CardServicesMenu]: Https indication customHeader->headerName:"<<i<<":"<<ind->customHeaders[i].headerName<<std::endl;
                  std::cout<<"[CardServicesMenu]: Https indication customHeader->headerValue:"<<i<<":"<<ind->customHeaders[i].headerValue<<std::endl;
              }
              std::cout<<"[CardServicesMenu]: Https indication Url "<<ind->url<<std::endl;
          }
     }
  );

}

void CardServicesMenu::GetAtr(std::vector<std::string> userInput) {
    int slot_id;

    slot_id = std::stoi(userInput[1]);

    std::cout << "slot_id" << slot_id << std::endl;

    Status s = rilSession.GetAtr(slot_id,[](RIL_Errno err, const char *atr ,int len) -> void {
       std::cout << "Got response for getAtr req " << std::endl;
        if(len > 0 && atr != NULL) {
            std::cout << "Atr : " << atr << std::endl;
            std::cout << "Atrlength : " << len << std::endl;
        }
    });
    std::cout << ((s == Status::SUCCESS) ? "Received Atr values  succesfully"
                                         : "Error in retrieving Atr values")
              << std::endl;
    for (auto& arg : userInput) std::cout << __func__ << ":" << arg << std::endl;
}

void CardServicesMenu::GetAdnRecord(std::vector<std::string> userInput) {

    Status s = rilSession.GetAdnRecord([] (RIL_Errno err, const RIL_AdnCountInfo* countInfo) -> void {
        std::cout << "Got response for GetAdnRecord req: " << err << std::endl;
        if (countInfo != nullptr && err == RIL_E_SUCCESS) {
            std::cout << "max_adn_num" << countInfo->max_adn_num << std::endl
                      << "valid_adn_num" << countInfo->valid_adn_num << std::endl
                      << "max_email_num" << countInfo->max_email_num << std::endl
                      << "valid_email_num" << countInfo->valid_email_num << std::endl
                      << "max_ad_num" << countInfo->max_ad_num << std::endl
                      << "valid_ad_num" << countInfo->valid_ad_num << std::endl
                      << "max_name_len" << countInfo->max_name_len << std::endl
                      << "max_number_len" << countInfo->max_number_len << std::endl
                      << "max_email_len" << countInfo->max_email_len << std::endl
                      << "max_anr_len" << countInfo->max_anr_len << std::endl;
        }
    });
    std::cout << ((s == Status::SUCCESS) ? "Succeeded to send getAdnRecord req"
                                         : "Failed to send getAdnRecord req");
}

std::istream& operator >>(std::istream& in, RIL_AdnRecordInfo& adnRecordInfo) {

    while ((std::cout << "Enter Record Id")
        && !(in >> adnRecordInfo.record_id)){
        std::cout << "Enter a valid positive number ";
        in.clear();
        in.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }

    std::string recordName;
    while ((std::cout << "Enter record name")
        && !(in >> recordName)){
        in.clear();
    }
    adnRecordInfo.name = new char[recordName.size()+1]();
    if (adnRecordInfo.name != nullptr) {
        std::copy(recordName.begin(), recordName.end(), adnRecordInfo.name);
        adnRecordInfo.name[recordName.size()] = '\0';
    }

    std::string recordNumber;
    while ((std::cout << "Enter record number")
        && !(in >> recordNumber)){
        in.clear();
    }
    adnRecordInfo.number = new char[recordNumber.size()+1]();
    if (adnRecordInfo.number != nullptr) {
        std::copy(recordNumber.begin(), recordNumber.end(), adnRecordInfo.number);
        adnRecordInfo.number[recordNumber.size()] = '\0';
    }

    while ((std::cout << "Enter number of email elements")
        && (!(in >> adnRecordInfo.email_elements)
           || adnRecordInfo.email_elements > RIL_MAX_NUM_EMAIL_COUNT)) {
        std::cout << "Max allowed elements are 2";
        in.clear();
    }

    if (adnRecordInfo.email_elements > 0 &&
        adnRecordInfo.email_elements <= RIL_MAX_NUM_EMAIL_COUNT){
        std::string email;
            for (int i = 0; i < adnRecordInfo.email_elements; i++){
                std::cout << "Enter email" << std::endl;
                in >> email;
                adnRecordInfo.email[i] = new char[email.size()+1]();
                if (adnRecordInfo.email[i] != nullptr) {
                    std::copy(email.begin(), email.end(), adnRecordInfo.email[i]);
                    adnRecordInfo.email[i][email.size()] = '\0';
                }
                email.clear();
            }
    }

    while ((std::cout << "Enter number of anr elements")
        && (!(in >> adnRecordInfo.anr_elements)
            || adnRecordInfo.anr_elements > RIL_MAX_NUM_AD_COUNT)) {
        std::cout << "Max allowed elements are 4 ";
        in.clear();
    }
    if (adnRecordInfo.anr_elements > 0 &&
        adnRecordInfo.anr_elements <= RIL_MAX_NUM_AD_COUNT){
        std::string anr;
            for (int i = 0; i < adnRecordInfo.anr_elements; i++){
                std::cout << "Enter ad_number" << std::endl;
                in >> anr;
                adnRecordInfo.ad_number[i] = new char[anr.size()+1]();
                if (adnRecordInfo.ad_number[i] != nullptr) {
                    std::copy(anr.begin(), anr.end(), adnRecordInfo.ad_number[i]);
                    adnRecordInfo.ad_number[i][anr.size()] = '\0';
                }
                anr.clear();
            }
    }
    return in;
}


void CardServicesMenu::UpdateAdnRecord(std::vector<std::string> userInput) {

    RIL_AdnRecordInfo recordInfo{};
    std::cin >> recordInfo;
    Status s = rilSession.UpdateAdnRecord(&recordInfo, [](RIL_Errno err)->void {
        std::cout << "Received response for UpdateAdnRecord - error code: " << err << std::endl;
    });

    if (s != Status::SUCCESS){
        std::cout << "Updating adn record FAILED" << std::endl;
    }

    if (recordInfo.name != nullptr) {
        delete [] recordInfo.name;
    }

    if (recordInfo.number != nullptr) {
        delete [] recordInfo.number;
    }

    for (unsigned int i = 0; i < RIL_MAX_NUM_EMAIL_COUNT; i++){
        if (recordInfo.email[i] != nullptr) {
            delete [] recordInfo.email[i];
        }
    }

    for (unsigned int i = 0; i < RIL_MAX_NUM_AD_COUNT; i++){
        if (recordInfo.ad_number[i] != nullptr) {
            delete [] recordInfo.ad_number[i];
        }
    }
}

void CardServicesMenu::lpauserreq(std::vector<std::string> userInput) {
    int token,event,opcode;
    UimLpaUserReq user_req;
    std::cout<<"Sending lpauserrequest"<<std::endl;
    token = std::stoi(userInput[1]);
    event = std::stoi(userInput[2]);
    user_req.event = (UimLpaUserEventId)event;
    user_req.activationCode = const_cast<char *>(userInput[3].c_str());
    user_req.confirmationCode = const_cast<char *>(userInput[4].c_str());
    user_req.nickname = const_cast<char *>(userInput[5].c_str());
    user_req.iccid = const_cast<char *>(userInput[6].c_str());

    user_req.resetMask = std::stoi(userInput[7]);
    user_req.userOk = std::stoi(userInput[8]);
    user_req.nok_reason = std::stoi(userInput[9]);
    opcode = std::stoi(userInput[10]);
    user_req.srvOpReq.opCode = (UimLpaSrvAddrOp)opcode;
    user_req.srvOpReq.smdpAddress = const_cast<char *>(userInput[11].c_str());
    Status s = rilSession.SendLpaUserRequest(token,user_req,[](RIL_Errno err,const UimLpaUserResponse *resp ) -> void {
       std::cout << "Got response for LpaUserreq " << std::endl;
        int i=0,len = 0;
        char *dummy_eid_data,*eid_data,*dummy_iccid_data,*iccid_data;
        if(resp != nullptr) {
           std::cout << "Event :" << resp->event<<std::endl;
           std::cout <<"Result :" << resp->result<<std::endl;
           if(resp->eid != nullptr){
              dummy_eid_data = (char *)resp->eid;
              eid_data = mem_alloc_bin_to_hexstring(dummy_eid_data,resp->eid_len);
              if (eid_data != NULL) {
                std::cout << "Eid value is : " << std::string(eid_data) << std::endl;
              }
           }
           for(i=0;i<resp->no_of_profiles;i++)
           {
              std::cout<<"========== Profile : "<<i<<"========"<<std::endl;
              std::cout << "ProfileState : " << resp->profiles[i].state<<std::endl;
              if(resp->profiles[i].iccid != nullptr) {
              dummy_iccid_data = (char *)resp->profiles[i].iccid;
              std::cout<<"Iccid : " ;
              for(len = 0;len<resp->profiles[i].iccid_len;len++) {
                  std::cout<<resp->profiles[i].iccid[len];
              }
              std::cout<<std::endl;
              std::cout << "Iccid_len : " << resp->profiles[i].iccid_len<<std::endl;
              }
              if(resp->profiles[i].profileName){
              std::cout << "profileName : "<< std::string(resp->profiles[i].profileName)<<std::endl;
              }
              if(resp->profiles[i].nickName){
              std::cout << "NickName : "<< std::string(resp->profiles[i].nickName)<<std::endl;
              }
              if(resp->profiles[i].spName){
              std::cout << "spName : " << std::string(resp->profiles[i].spName)<<std::endl;
              }
              std::cout << "Icontype : " << resp->profiles[i].iconType<<std::endl;
              if(resp->profiles[i].icon){
              std::cout << "Icon : " << resp->profiles[i].icon<<std::endl;
              }
              std::cout << "Icon_len : " << resp->profiles[i].icon_len<<std::endl;
              std::cout << "profileClass : " << resp->profiles[i].profileClass<<std::endl;
              std::cout << "profilePolicy : " << resp->profiles[i].profilePolicy<<std::endl;

              std::cout<<"=========================================="<<std::endl;
           }
           if(resp->no_of_profiles == 0) {
              std::cout<<"No Profiles installed"<<std::endl;
           }
           if(resp->srvAddr.smdpAddress){
               std::cout<<"smdpAddress : "<<std::string(resp->srvAddr.smdpAddress)<<std::endl;
           }
           else {
                std::cout<<"smdpAddress: "<<std::endl;
           }
           if(resp->srvAddr.smdsAddress){
               std::cout<<"smdsAddress : "<<std::string(resp->srvAddr.smdsAddress)<<std::endl;
           }
        }
    });
    std::cout << ((s == Status::SUCCESS) ? "Received success for lpa request"
                                         : "Received failure for lpa request")
              << std::endl;
    for (auto& arg : userInput) std::cout << __func__ << ":" << arg << std::endl;
}
