/******************************************************************************
#  Copyright (c) 2021 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#include <ril_socket_api.h>
#include <interfaces/uim/UimLpaReqMessage.h>
#include <interfaces/uim/qcril_uim_types.h>
#include "interfaces/lpa/lpa_service_types.h"
#include "interfaces/uim/qcril_uim_lpa.h"
#undef TAG
#define TAG "RILQ"
#include <framework/Log.h>

namespace ril {
namespace socket {
namespace api {

void socket_to_module(UimLpaUserReq& in, lpa_service_user_req_type& out)
{
    out.event  = (lpa_service_user_event_type)in.event;
    if(in.activationCode) {
       out.activationCode = in.activationCode;
    }
    else {
       out.activationCode="";
    }
    if(in.confirmationCode) {
       out.confirmationCode = in.confirmationCode;
    }
    else {
       out.confirmationCode = "";
    }
    if(in.nickname){
        out.nickname = in.nickname;
    }
    else {
        out.nickname = "";
    }
    if(in.iccid) {
        out.iccid = (uint8_t *)in.iccid;
        out.iccid_len = strlen(in.iccid);
    }
    else {
        out.iccid = (uint8_t *)"";
    }
    out.resetMask  = in.resetMask;
    out.userOk = in.userOk;
    out.nok_reason = in.nok_reason;
    out.srvOpReq.opCode = (lpa_service_srv_addr_op_type)in.srvOpReq.opCode;
    out.srvOpReq.smdpAddress = in.srvOpReq.smdpAddress;
}

uint8_t convert_iccid_to_byte
(
  uint8_t       * iccid_byte_ptr,
  uint8_t         iccid_byte_len,
  const uint8_t * iccid_ptr,
  uint8_t         len
)
{
  uint8_t  i = 0;
  uint8_t  j = 0;
  uint8_t ch = 0;

  if (iccid_ptr == NULL ||
      iccid_byte_ptr == NULL ||
      len == 0 ||
      len > 10 ||
      iccid_byte_len == 0 ||
      len > 2 * 10)
  {
      return 0;
  }

  for ( i = 0; i < len && j < iccid_byte_len; i++ )
  {
    /* copy first digit */
    ch = iccid_ptr[i] & 0x0F;
    /* iccid bcd octets can have padded 0xf, need to ignore the same */
    if (0x0F == ch)
    {
      continue;
    }

    if (ch > 9)
    {
      return 0;
    }
    iccid_byte_ptr[j] = ch + '0';
    j++;

    if (j < iccid_byte_len)
    {
      /* copy second digit */
      ch = iccid_ptr[i] >> 4;
      /* iccid bcd octets can have padded 0xf, need to ignore the same */
      if (0x0F == ch)
      {
        continue;
      }

      if (ch > 9)
      {
          return 0;
      }
      iccid_byte_ptr[j] = ch + '0';
      j++;
    }
  }
  return j;
} /* qcril_uim_lpa_iccid_to_byte */

// TODO - Find the right UIM module message not found in ril_service.cpp
void sendlpauserrequest(std::shared_ptr<SocketRequestContext> context, Marshal& p) {
    UimLpaUserReq params;
    int32_t token;
    if(p.read(token) == Marshal::Result::SUCCESS && p.read(params) == Marshal::Result::SUCCESS) {
        QCRIL_LOG_INFO("Dispatching Sim lpauser request.");
        lpa_service_user_req_type user_req_params;
        socket_to_module(params,user_req_params);
        QCRIL_LOG_INFO("Token is %d  Event is %d",token,user_req_params.event);
        std::shared_ptr<UimLpaReqMessage>   lpa_req_ptr      = nullptr;
        GenericCallback <qcril_uim_lpa_response_type>cb(
             [context] (std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
             std::shared_ptr<qcril_uim_lpa_response_type> resp) -> void {
                 QCRIL_LOG_INFO("Sim lpauser request response.");
                 RIL_Errno errorCode = RIL_E_INTERNAL_ERR;
                 qcril_uim_lpa_user_resp_type * usr_resp_ptr =
                                    (qcril_uim_lpa_user_resp_type *) resp->data_ptr;
                 qcril_uim_lpa_user_resp_type * out_ptr = nullptr;
                 out_ptr = new qcril_uim_lpa_user_resp_type;
                 if (out_ptr != nullptr)
                 {
                   memset(out_ptr, 0x00, sizeof(qcril_uim_lpa_user_resp_type));

                   out_ptr->event = (qcril_uim_lpa_user_req_event_type )usr_resp_ptr->event;

                   out_ptr->result = (qcril_uim_lpa_result_type )usr_resp_ptr->result;

                   out_ptr->eid_len = 0;
                   if (usr_resp_ptr->eid != nullptr && usr_resp_ptr->eid_len != 0)
                   {
                     out_ptr->eid = new uint8_t[usr_resp_ptr->eid_len];
                     if (out_ptr->eid != nullptr)
                     {
                       memcpy(out_ptr->eid, usr_resp_ptr->eid, usr_resp_ptr->eid_len);
                       out_ptr->eid_len = usr_resp_ptr->eid_len;
                     }
                   }
                   if (usr_resp_ptr->srvAddr.smdpAddress != nullptr)
                   {
                     uint32_t len = strlen(usr_resp_ptr->srvAddr.smdpAddress);

                     out_ptr->srvAddr.smdpAddress = new char[len + 1]{0};
                     if (out_ptr->srvAddr.smdpAddress != nullptr)
                     {
                       strlcpy(out_ptr->srvAddr.smdpAddress, usr_resp_ptr->srvAddr.smdpAddress,
                                                                   len + 1);
                     }
                   }
                   if (usr_resp_ptr->srvAddr.smdsAddress != nullptr)
                   {
                     uint32_t len = strlen(usr_resp_ptr->srvAddr.smdsAddress);

                     out_ptr->srvAddr.smdsAddress = new char[len + 1]{0};
                     if (out_ptr->srvAddr.smdsAddress != nullptr)
                     {
                       strlcpy(out_ptr->srvAddr.smdsAddress, usr_resp_ptr->srvAddr.smdsAddress,
                                                               len + 1);
                     }
                   }
                   out_ptr->no_of_profiles = 0;
                   if (usr_resp_ptr->no_of_profiles != 0 && usr_resp_ptr->profiles != nullptr)
                   {
                     out_ptr->profiles = new qcril_uim_lpa_profile_info_type[
                                                  usr_resp_ptr->no_of_profiles];
                     if (out_ptr->profiles != nullptr)
                     {
                       uint32_t i = 0;
                       out_ptr->no_of_profiles = usr_resp_ptr->no_of_profiles;
                       for (i = 0; i < usr_resp_ptr->no_of_profiles; i++)
                       {
                         memcpy(&out_ptr->profiles[i], &usr_resp_ptr->profiles[i],
                                            sizeof(qcril_uim_lpa_profile_info_type));
                         out_ptr->profiles[i].iccid_len = 0;
                         if (usr_resp_ptr->profiles[i].iccid != nullptr &&
                                                 usr_resp_ptr->profiles[i].iccid_len != 0)
                         {
                           out_ptr->profiles[i].iccid = new uint8_t[usr_resp_ptr->profiles[i].
                                                                    iccid_len * 2];
                           if (out_ptr->profiles[i].iccid != nullptr)
                           {
                             out_ptr->profiles[i].iccid_len = convert_iccid_to_byte(
                                                               out_ptr->profiles[i].iccid,
                                                               usr_resp_ptr->profiles[i].iccid_len * 2,
                                                               usr_resp_ptr->profiles[i].iccid,
                                                               usr_resp_ptr->profiles[i].iccid_len);
                           }
                        }
                        out_ptr->profiles[i].icon_len = 0;
                        if (usr_resp_ptr->profiles[i].icon != nullptr &&
                                         usr_resp_ptr->profiles[i].icon_len != 0)
                         {
                           out_ptr->profiles[i].icon = new uint8_t[usr_resp_ptr->profiles[i].icon_len];

                           if (out_ptr->profiles[i].icon != nullptr)
                           {
                             memcpy(out_ptr->profiles[i].icon, usr_resp_ptr->profiles[i].icon,
                                                          usr_resp_ptr->profiles[i].icon_len);
                             out_ptr->profiles[i].icon_len = usr_resp_ptr->profiles[i].icon_len;
                           }
                         }
                         if (usr_resp_ptr->profiles[i].profileName != nullptr)
                         {
                           uint32_t len = strlen(usr_resp_ptr->profiles[i].profileName);
                           out_ptr->profiles[i].profileName = new char[len + 1]{0};
                           if (out_ptr->profiles[i].profileName != nullptr)
                           {
                             strlcpy(out_ptr->profiles[i].profileName,
                                             usr_resp_ptr->profiles[i].profileName, len + 1);
                           }
                         }
                         if (usr_resp_ptr->profiles[i].nickName != nullptr)
                         {
                           uint32_t len = strlen(usr_resp_ptr->profiles[i].nickName);

                           out_ptr->profiles[i].nickName = new char[len + 1]{0};
                           if (out_ptr->profiles[i].nickName != nullptr)
                           {
                             strlcpy(out_ptr->profiles[i].nickName,
                                         usr_resp_ptr->profiles[i].nickName, len + 1);
                           }
                         }
                         if (usr_resp_ptr->profiles[i].spName != nullptr)
                         {
                           uint32_t len = strlen(usr_resp_ptr->profiles[i].spName);
                           out_ptr->profiles[i].spName = new char[len + 1]{0};
                           if (out_ptr->profiles[i].spName != nullptr)
                           {
                             strlcpy(out_ptr->profiles[i].spName,
                                         usr_resp_ptr->profiles[i].spName, len + 1);
                           }
                         }
                       }
                     }
                   }
                 }
                 std::shared_ptr<Marshal> p = nullptr;
                 if (status == Message::Callback::Status::SUCCESS && resp != nullptr) {
                     p = std::make_shared<Marshal>();
                     if (p) {
                          UimLpaUserResponse rsp;
                          int i=0;
                          rsp.event = (UimLpaUserEventId)out_ptr->event;
                          QCRIL_LOG_INFO("LpaUserResponse event is %d",rsp.event);
                          rsp.result = (UimLpaResult)out_ptr->result;
                          rsp.eid = out_ptr->eid;
                          rsp.eid_len = out_ptr->eid_len;
                          rsp.no_of_profiles = out_ptr->no_of_profiles;
                          QCRIL_LOG_INFO("LpaUserReponse no_of_profiles %d",rsp.no_of_profiles);
                          if(out_ptr->no_of_profiles > 0 && out_ptr->profiles!=NULL)
                          {
                              rsp.profiles = new UimLpaProfileInfo[out_ptr->no_of_profiles];

                              for (i=0;i<out_ptr->no_of_profiles;i++) {
                                 rsp.profiles[i].state = (UimLpaProfileState)
                                                                out_ptr->profiles[i].state;
                                 rsp.profiles[i].iccid = out_ptr->profiles[i].iccid;
                                 rsp.profiles[i].iccid_len = out_ptr->profiles[i].iccid_len;
                                 rsp.profiles[i].profileName = out_ptr->profiles[i].profileName;
                                 rsp.profiles[i].nickName = out_ptr->profiles[i].nickName;
                                 rsp.profiles[i].spName = out_ptr->profiles[i].spName;
                                 rsp.profiles[i].iconType =(UimLpaIconType)
                                                              out_ptr->profiles[i].iconType;
                                 rsp.profiles[i].icon = out_ptr->profiles[i].icon;
                                 rsp.profiles[i].icon_len = out_ptr->profiles[i].icon_len;
                                 rsp.profiles[i].profileClass = (UimLpaProfileClassType)
                                                                out_ptr->profiles[i].profileClass;
                                 rsp.profiles[i].profilePolicy = (UimLpaProfilePolicyMask)
                                                                   out_ptr->profiles[i].profilePolicy;
                              }
                          }
                          rsp.srvAddr.smdpAddress = out_ptr->srvAddr.smdpAddress;
                          rsp.srvAddr.smdsAddress = out_ptr->srvAddr.smdsAddress;
                         if (p->write(rsp) == Marshal::Result::SUCCESS) {
                              errorCode = static_cast<RIL_Errno>(rsp.result);
                         }
                     } else {
                         errorCode = RIL_E_NO_MEMORY;
                     }
                 }
                 sendResponse(context, errorCode, p);
             });
         lpa_req_ptr = std::make_shared<UimLpaReqMessage>(UIM_LPA_USER_REQUEST_ID ,&cb,
                                                           &user_req_params,token);
         if(lpa_req_ptr != nullptr) {
             lpa_req_ptr->setCallback(&cb);
             lpa_req_ptr->dispatch();
         }
         else {
             sendResponse(context,RIL_E_NO_MEMORY,nullptr);
         }
         p.release(params);
    } else {

          sendResponse(context,RIL_E_INTERNAL_ERR,nullptr);
    }
}
}  // namespace api
}  // namespace socket
}  // namespace ril
