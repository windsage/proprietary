/******************************************************************************
#  Copyright (c) 2018 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#pragma once

#include <framework/MessageContext.h>
#include <utils/StrongPointer.h>

template <typename T>
class RadioContextClass: public MessageContextBase<RadioContextClass<T>>
{
    private:
        int32_t serial;
        ::android::sp<T> service;
    public:
        RadioContextClass(::android::sp<T> &&service, int32_t serial): MessageContextBase<RadioContextClass>(service? service->getInstanceId() : QCRIL_DEFAULT_INSTANCE_ID), serial(serial) {}
        std::string toString() {
            return std::string("IRadio(") + std::to_string(serial) + std::string(")");
        }
        int32_t getSerial() { return serial; }
        ::android::sp<T> getService() { return service; }


};

class RadioServiceContext : public MessageContext {
 private:
 public:
  RadioServiceContext(qcril_instance_id_e_type instance_id, uint32_t serial)
      : MessageContext(instance_id, serial) {}

  ~RadioServiceContext() = default;

  std::string toString() {
    return std::string("Radio(") + std::to_string(serial) + std::string(")");
  }
};
