/*
 * Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
 * All rights reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#ifndef SETUPDATACALLREQUESTMESSAGE
#define SETUPDATACALLREQUESTMESSAGE
#include "framework/SolicitedMessage.h"
#include "framework/add_message_id.h"
#include "MessageCommon.h"

using namespace std;
namespace rildata {

class SetupDataCallRadioResponseIndMessage : public UnSolicitedMessage,
                     public add_message_id<SetupDataCallRadioResponseIndMessage>  {
protected:
  SetupDataCallResponse_t response;
  int32_t serial;
  Message::Callback::Status status;

public:
  static constexpr const char *MESSAGE_NAME = "SetupDataCallRadioResponseIndMessage";
  ~SetupDataCallRadioResponseIndMessage() {};
  SetupDataCallRadioResponseIndMessage(const SetupDataCallResponse_t& setResponse,
    int32_t setSerial, Message::Callback::Status setStatus):
    UnSolicitedMessage(get_class_message_id())
  {
    mName = MESSAGE_NAME;
    response = setResponse;
    serial = setSerial;
    status = setStatus;
  }

  string dump() {
    stringstream ss;
    ss << mName << " Serial=[" << (int)getSerial() << "] ";
    response.dump("", ss);
    return ss.str();
  }

  SetupDataCallResponse_t getResponse() { return response; }
  int32_t getSerial() { return serial; }
  Message::Callback::Status getStatus() { return status; }
  std::shared_ptr<UnSolicitedMessage> clone() {
    return std::make_shared<SetupDataCallRadioResponseIndMessage>(response, serial, status);
  }
};

class SetupDataCallIWlanResponseIndMessage : public UnSolicitedMessage,
                          public add_message_id<SetupDataCallIWlanResponseIndMessage> {
private:
  SetupDataCallResponse_t response;
  int32_t serial;
  Message::Callback::Status status;

public:
  static constexpr const char *MESSAGE_NAME = "SetupDataCallIWlanResponseIndMessage";

  SetupDataCallIWlanResponseIndMessage() = delete;
  ~SetupDataCallIWlanResponseIndMessage() = default;
  SetupDataCallIWlanResponseIndMessage(const SetupDataCallResponse_t& setResponse, int32_t setSerial, Message::Callback::Status setStatus):
    UnSolicitedMessage(get_class_message_id()), response(setResponse), serial(setSerial), status(setStatus) {}

  string dump(){return mName;}

  std::shared_ptr<UnSolicitedMessage> clone() {
    return std::make_shared<SetupDataCallIWlanResponseIndMessage>(response, serial, status);
  }

  SetupDataCallResponse_t getResponse() { return response; }
  int32_t getSerial() { return serial; }
  Message::Callback::Status getStatus() { return status; }
};

class SetupDataCallRequestBase : public SolicitedMessage<SetupDataCallResponse_t> {
private:
  int32_t mSerial;
  RequestSource_t mRequestSource;
  AccessNetwork_t mAccessNetwork;
  bool mRoamingAllowed;
  DataRequestReason_t mReason;
  vector<string> mAddresses;
  vector<string> mDnses;
  shared_ptr<function<void(int32_t)>> mAcknowlegeRequestCb;

protected:
  DataProfileInfo_t mDataProfileInfo;

public:
  SetupDataCallRequestBase() = delete;
  SetupDataCallRequestBase(
    message_id_ref msgId,
    const int32_t serial,
    const RequestSource_t requestSource,
    const AccessNetwork_t accessNetwork,
    DataProfileInfo_t dataProfileInfo,
    const bool roamingAllowed,
    const DataRequestReason_t reason,
    const vector<string> addresses,
    const vector<string> dnses,
    const shared_ptr<function<void(int32_t)>> ackCb
  ):SolicitedMessage<SetupDataCallResponse_t>(msgId) {
    mSerial = serial;
    mRequestSource = requestSource;
    mAccessNetwork = accessNetwork;
    mDataProfileInfo = dataProfileInfo;
    mRoamingAllowed = roamingAllowed;
    mReason = reason;
    mAddresses = addresses;
    mDnses = dnses;
    mAcknowlegeRequestCb = ackCb;
  }
  ~SetupDataCallRequestBase() = default;

  virtual string dump(){
    std::stringstream ss;
    ss << mName << " Serial=[" << (int)getSerial() << "] Params=[";
    ss << (int)getRequestSource() << ",";
    ss << getAccessNetwork() << ",";
    getDataProfileInfo().dump("", ss);
    ss << ",";
    ss << std::boolalpha << getRoamingAllowed() << ",";
    ss << (int)getDataRequestReason() << ",";
    ss << "Addrs=<";
    std::vector<std::string> addr = getAddresses();
    for (unsigned long i=0 ; i<addr.size(); i++) {
      ss << addr[i];
    }
    ss << ">,Dnses=<";
    std::vector<std::string> dns = getDnses();
    for (unsigned long i=0 ; i<dns.size(); i++) {
      ss << dns[i];
    }
    ss << ">]";
    return ss.str();
  }

  int32_t getSerial() {return mSerial;}
  RequestSource_t getRequestSource() {return mRequestSource;}
  AccessNetwork_t getAccessNetwork() {return mAccessNetwork;}
  DataProfileInfo_t getDataProfileInfo() { return mDataProfileInfo; }
  DataProfileId_t getProfileId() {return mDataProfileInfo.profileId;}
  string getApn() {return mDataProfileInfo.apn;}
  string getProtocol() {return mDataProfileInfo.protocol;}
  string getRoamingProtocol() {return mDataProfileInfo.roamingProtocol;}
  ApnAuthType_t getAuthType() {return mDataProfileInfo.authType;}
  string getUsername() {return mDataProfileInfo.username;}
  string getPassword() {return mDataProfileInfo.password;}
  DataProfileInfoType_t getDataProfileInfoType() {return mDataProfileInfo.dataProfileInfoType;}
  int32_t getMaxConnsTime() {return mDataProfileInfo.maxConnsTime;}
  int32_t getMaxConns() {return mDataProfileInfo.maxConns;}
  int32_t getWaitTime() {return mDataProfileInfo.waitTime;}
  bool getEnableProfile() {return mDataProfileInfo.enableProfile;}
  ApnTypes_t getSupportedApnTypesBitmap() {return mDataProfileInfo.supportedApnTypesBitmap;}
  RadioAccessFamily_t getBearerBitmap() {return mDataProfileInfo.bearerBitmap;}
  int32_t getMtu() {return mDataProfileInfo.mtu;}
  bool getPreferred() {return mDataProfileInfo.preferred;}
  bool getPersistent() {return mDataProfileInfo.persistent;}
  bool getRoamingAllowed() {return mRoamingAllowed;}
  DataRequestReason_t getDataRequestReason() {return mReason;}
  vector<string> getAddresses() {return mAddresses;}
  vector<string> getDnses() {return mDnses;}
  shared_ptr<function<void(int32_t)>> getAcknowlegeRequestCb() {return mAcknowlegeRequestCb;}
};

class SetupDataCallRequestMessage : public SetupDataCallRequestBase,
                          public add_message_id<SetupDataCallRequestMessage> {
public:
  static constexpr const char *MESSAGE_NAME = "SetupDataCallRequestMessage";
  // IRadio 1.4-1.5
  SetupDataCallRequestMessage(
    const int32_t serial,
    const RequestSource_t requestSource,
    const AccessNetwork_t accessNetwork,
    DataProfileInfo_t dataProfileInfo,
    const bool roamingAllowed,
    const DataRequestReason_t reason,
    const vector<string> addresses,
    const vector<string> dnses,
    const shared_ptr<function<void(int32_t)>> ackCb
  ):SetupDataCallRequestBase(
    get_class_message_id(),
    serial,
    requestSource,
    accessNetwork,
    dataProfileInfo,
    roamingAllowed,
    reason,
    addresses,
    dnses,
    ackCb
  ) {
    mName = MESSAGE_NAME;
    mDataProfileInfo.originalapn = mDataProfileInfo.apn;
    transform(mDataProfileInfo.apn.begin(), mDataProfileInfo.apn.end(), mDataProfileInfo.apn.begin(), ::tolower);
  }
  ~SetupDataCallRequestMessage() = default;
  SetupDataCallRequestMessage(
  const int32_t serial,
  const RequestSource_t requestSource,
  const AccessNetwork_t accessNetwork,
  DataProfileInfo_t dataProfileInfo,
  const bool roamingAllowed,
  const DataRequestReason_t reason,
  const vector<string> addresses,
  const vector<string> dnses,
  const bool matchAllRuleAllowed,
  const shared_ptr<function<void(int32_t)>> ackCb
  ):SetupDataCallRequestBase(
    get_class_message_id(),
    serial,
    requestSource,
    accessNetwork,
    dataProfileInfo,
    roamingAllowed,
    reason,
    addresses,
    dnses,
    ackCb
  ) {
    mName = MESSAGE_NAME;
    std::ignore = matchAllRuleAllowed;
  }
  void setOptionalTrafficDescriptor(TrafficDescriptor_t) {}
};

class SetupDataCallRequestMessage_1_6 : public SetupDataCallRequestBase,
                          public add_message_id<SetupDataCallRequestMessage_1_6> {
private:
  std::optional<TrafficDescriptor_t> mTrafficDescriptor;
  bool mMatchAllRuleAllowed;

public:
  static constexpr const char *MESSAGE_NAME = "SetupDataCallRequestMessage_1_6";
  // IRadio 1.6
  SetupDataCallRequestMessage_1_6(
  const int32_t serial,
  const RequestSource_t requestSource,
  const AccessNetwork_t accessNetwork,
  DataProfileInfo_t dataProfileInfo,
  const bool roamingAllowed,
  const DataRequestReason_t reason,
  const vector<string> addresses,
  const vector<string> dnses,
  const bool matchAllRuleAllowed,
  const shared_ptr<function<void(int32_t)>> ackCb
  ):SetupDataCallRequestBase(
    get_class_message_id(),
    serial,
    requestSource,
    accessNetwork,
    dataProfileInfo,
    roamingAllowed,
    reason,
    addresses,
    dnses,
    ackCb
  ) {
    mName = MESSAGE_NAME;
    mMatchAllRuleAllowed = matchAllRuleAllowed;
    //cache it in DataProfileInfo for later use
    mDataProfileInfo.matchAllRuleAllowed = matchAllRuleAllowed;
    mDataProfileInfo.originalapn = mDataProfileInfo.apn;
    transform(mDataProfileInfo.apn.begin(), mDataProfileInfo.apn.end(), mDataProfileInfo.apn.begin(), ::tolower);
  }
  string dump() {
    std::stringstream ss;
    ss << std::boolalpha << getMatchAllRuleAllowed();
    ss << " Td=<";
    if(mTrafficDescriptor.has_value()) {
      mTrafficDescriptor.value().dump("", ss);
    }
    ss << ">,";
    return SetupDataCallRequestBase::dump() + ss.str();
  }

  std::vector<string> dumpLine() {
    std::vector<string> result;
    std::stringstream ss;
    ss << mName << "[";
    ss << "matchAllRuleAllowed=" << std::boolalpha << getMatchAllRuleAllowed();
    ss << " Td=<";
    if(mTrafficDescriptor.has_value()) {
      mTrafficDescriptor.value().dump("", ss);
    }
    ss << ">]";
    result.push_back(SetupDataCallRequestBase::dump());
    result.push_back(ss.str());
    return result;
  }

  void setOptionalTrafficDescriptor(TrafficDescriptor_t td) {
    mTrafficDescriptor = td;
    if (!mDataProfileInfo.trafficDescriptor.has_value()) {
      mDataProfileInfo.trafficDescriptor = mTrafficDescriptor;
    }
  }
  std::optional<TrafficDescriptor_t> getOptionalTrafficDescriptor() {return mTrafficDescriptor;}
  bool getMatchAllRuleAllowed() {return mMatchAllRuleAllowed;}
};

}

#endif
