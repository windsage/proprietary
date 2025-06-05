/******************************************************************************
#  Copyright (c) 2020 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#ifndef PDCMODEMENDPOINTMODULE
#define PDCMODEMENDPOINTMODULE
#include "qmi_client.h"
#include "common_v01.h"
#include "persistent_device_configuration_v01.h"

#include "modules/qmi/ModemEndPoint.h"
#include "modules/qmi/ModemEndPointModule.h"

namespace rildata {

class PdcModemEndPointModule : public ModemEndPointModule {
private:
    qmi_idl_service_object_type getServiceObject() override;
    void handleQmiPdcIndMessage(std::shared_ptr<Message>);
    void handleEndPointStatusIndMessage(std::shared_ptr<Message>);
    void registerForPdcIndication();
    void handlePdcRefreshInd(const pdc_refresh_ind_msg_v01& ind);
    void sendPdcNotificationAck(const pdc_refresh_ind_msg_v01& ind);
    void indicationHandler(
        unsigned int   msg_id,
        unsigned char *decoded_payload,
        uint32_t       decoded_payload_len
    );

public:
    PdcModemEndPointModule(string name, ModemEndPoint& owner);
    virtual ~PdcModemEndPointModule();

    void init();
};

}

#endif