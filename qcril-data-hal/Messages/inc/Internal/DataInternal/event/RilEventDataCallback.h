/**
* Copyright (c) 2017-2020, 2022 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
**/

#ifndef RILEVENTDATACALLBACK
#define RILEVENTDATACALLBACK
#include "framework/Message.h"
#include "framework/UnSolicitedMessage.h"
#include <framework/add_message_id.h>
#include "dsi_netctrl.h"

namespace rildata {

struct dsiInfo {
  dsi_dev_info_t connectInfo;
  bool isConnectInfoValid;
  dsi_disconnect_info_t disconnectInfo;
  bool isDisconnectInfoValid;
  bool isMtuValid;
};

typedef struct
{
  dsi_net_evt_t         evt;
  void                 *data;
  int                   data_len;
  void                 *self;
  dsi_hndl_t            hndl;
  dsi_evt_payload_t     payload;
  dsiInfo               payload_ex;
} EventDataType;

class RilEventDataCallback : public UnSolicitedMessage,
                             public add_message_id<RilEventDataCallback> {
private:
  EventDataType eventData;
  int mCid;

public:
  static constexpr const char *MESSAGE_NAME = "QCRIL_EVT_DATA_EVENT_CALLBACK";
  ~RilEventDataCallback() = default;

  inline RilEventDataCallback(EventDataType p): UnSolicitedMessage(get_class_message_id())
  {
    eventData = p;
    mName = MESSAGE_NAME;
  }

  inline std::shared_ptr<UnSolicitedMessage> clone()
  {
    return std::static_pointer_cast<UnSolicitedMessage>(std::make_shared<RilEventDataCallback>(eventData));
  }

  inline std::string getEventName()
  {
    switch (eventData.evt) {
      case DSI_EVT_INVALID:
        return "INVALID";
      case DSI_EVT_NET_IS_CONN:
        return "NET_IS_CONN";
      case DSI_EVT_NET_NO_NET:
        return "NET_NO_NET";
      case DSI_EVT_PHYSLINK_DOWN_STATE:
        return "PHYSLINK_DOWN_STATE";
      case DSI_EVT_PHYSLINK_UP_STATE:
        return "PHYSLINK_UP_STATE";
      case DSI_EVT_NET_RECONFIGURED:
        return "NET_RECONFIGURED";
      case DSI_EVT_QOS_STATUS_IND:
        return "QOS_STATUS_IND";
      case DSI_EVT_NET_NEWADDR:
        return "NET_NEWADDR";
      case DSI_EVT_NET_DELADDR:
        return "NET_DELADDR";
      case DSI_EVT_NET_PARTIAL_CONN:
        return "NET_PARTIAL_CONN";
      case DSI_EVT_NET_HANDOFF:
        return "NET_HANDOFF";
      case DSI_EVT_WDS_CONNECTED:
        return "WDS_CONNECTED";
      case DSI_EVT_NET_NEWMTU:
        return "NET_NEWMTU";
#ifndef RIL_FOR_MDM_LE
      case DSI_EVT_NET_ERR_CALL_EXISTS:
        return "NET_ERR_CALL_EXISTS";
#endif
      default:
        return "UNKNOWN";
    }
  }

  inline string dump()
  {
    return mName + " {cid=" + std::to_string(mCid) + " " + getEventName() + "}";
  }

  inline EventDataType *getEventData() {
    return &eventData;
  }

  inline int getCid() {
    return mCid;
  }

  inline void setCid(int cid) {
    mCid = cid;
  }
};

} //namespace

#endif