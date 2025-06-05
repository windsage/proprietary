/******************************************************************************
#  Copyright (c) 2020 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#include <ril_socket_api.h>
#include <interfaces/ims/QcRilRequestImsGeoLocationInfoMessage.h>
#include <interfaces/ims/ims.h>
#include <ril_socket_api.h>
#include <telephony/ril_ims.h>
#undef TAG
#define TAG "RILQ"

namespace ril {
namespace socket {
namespace api {

static void freeGeolocationInfoBuffer(RIL_IMS_GeolocationInfo& geolocationInfo) {
    delete[] geolocationInfo.countryCode;
    delete[] geolocationInfo.country;
    delete[] geolocationInfo.state;
    delete[] geolocationInfo.city;
    delete[] geolocationInfo.postalCode;
    delete[] geolocationInfo.street;
    delete[] geolocationInfo.houseNumber;
    geolocationInfo.countryCode = nullptr;
    geolocationInfo.country = nullptr;
    geolocationInfo.state = nullptr;
    geolocationInfo.city = nullptr;
    geolocationInfo.postalCode = nullptr;
    geolocationInfo.street = nullptr;
    geolocationInfo.houseNumber = nullptr;
}

void dispatchImsSendGeolocationInfo(std::shared_ptr<SocketRequestContext> context, Marshal& p) {
    QCRIL_LOG_FUNC_ENTRY();
    RIL_IMS_GeolocationInfo geolocationInfo{};

    if (p.read(geolocationInfo) != Marshal::Result::SUCCESS) {
        QCRIL_LOG_ERROR("Ims Geolocation Info is empty");
        sendResponse(context, RIL_E_INTERNAL_ERR, nullptr);
        QCRIL_LOG_FUNC_RETURN();
        return;
    }

    auto msg = std::make_shared<QcRilRequestImsGeoLocationInfoMessage>(context);
    if(msg == nullptr){
        QCRIL_LOG_ERROR("msg is nullptr");
        sendResponse(context, RIL_E_NO_MEMORY, nullptr);
        freeGeolocationInfoBuffer(geolocationInfo);
        QCRIL_LOG_FUNC_RETURN();
        return;
    }

    // Check for nullptrs
    bool invalid_arg = false;
    do {
        if (!geolocationInfo.countryCode) {
            invalid_arg = true;
            QCRIL_LOG_ERROR("geolocationInfo.countryCode is a nullptr.");
            break;
        }
        if (!geolocationInfo.country) {
            invalid_arg = true;
            QCRIL_LOG_ERROR("geolocationInfo.country is a nullptr.");
            break;
        }
        if (!geolocationInfo.state) {
            invalid_arg = true;
            QCRIL_LOG_ERROR("geolocationInfo.state is a nullptr.");
            break;
        }
        if (!geolocationInfo.city) {
            invalid_arg = true;
            QCRIL_LOG_ERROR("geolocationInfo.city is a nullptr.");
            break;
        }
        if (!geolocationInfo.postalCode) {
            invalid_arg = true;
            QCRIL_LOG_ERROR("geolocationInfo.postalCode is a nullptr.");
            break;
        }
        if (!geolocationInfo.street) {
            invalid_arg = true;
            QCRIL_LOG_ERROR("geolocationInfo.street is a nullptr.");
            break;
        }
        if (!geolocationInfo.houseNumber) {
            invalid_arg = true;
            QCRIL_LOG_ERROR("geolocationInfo.houseNumber is a nullptr.");
            break;
        }
    } while (0);

    if (invalid_arg) {
        sendResponse(context, RIL_E_INVALID_ARGUMENTS, nullptr);
        freeGeolocationInfoBuffer(geolocationInfo);
        QCRIL_LOG_FUNC_RETURN();
        return;
    }

    msg->setLatitude(geolocationInfo.latitude);
    msg->setLongitude(geolocationInfo.longitude);
    msg->setCountryCode(geolocationInfo.countryCode);
    msg->setCountry(geolocationInfo.country);
    msg->setState(geolocationInfo.state);
    msg->setCity(geolocationInfo.city);
    msg->setPostalCode(geolocationInfo.postalCode);
    msg->setStreet(geolocationInfo.street);
    msg->setHouseNumber(geolocationInfo.houseNumber);

    GenericCallback<QcRilRequestMessageCallbackPayload> cb(
        [context](std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
                  std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
          RIL_Errno errorCode = dispatchStatusToRilErrno(status);
          if (status == Message::Callback::Status::SUCCESS && resp != nullptr) {
              errorCode = resp->errorCode;
          }
          sendResponse(context, errorCode, nullptr);
    });

    msg->setCallback(&cb);
    msg->dispatch();
    freeGeolocationInfoBuffer(geolocationInfo);
    QCRIL_LOG_FUNC_RETURN();
}

}  // namespace api
}  // namespace socket
}  // namespace ril
