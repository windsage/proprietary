/*===========================================================================

  Copyright (c) 2022 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

===========================================================================*/
#ifndef SETNRUCICONCONFIGUREMESSAGE
#define SETNRUCICONCONFIGUREMESSAGE
#include <vector>
#include <optional>
#include "framework/SolicitedMessage.h"
#include "framework/add_message_id.h"
#include "framework/GenericCallback.h"
#include "MessageCommon.h"

namespace rildata {

enum NrUCIconMode_t {
  MODE_NONE               = 0,
  MODE_CONNECTED          = 1,
  MODE_IDLE               = 2,
  MODE_CONNECTED_AND_IDLE = 3,
};

struct NrUCIconBandInfo_t {
  bool enabled;
  NrUCIconMode_t mode;
  std::vector<uint32_t> band;
};

struct NrUCIconBandwidthInfo_t {
  bool enabled;
  NrUCIconMode_t mode;
  uint32_t bandwidth;
};

enum NrUCIconRefreshTimer_t {
  SCG_TO_MCG_TIMER      = 0,
  IDLE_TO_CONNECT_TIMER = 1,
  IDLE_TIMER            = 2,
};

struct NrUCIconRefreshTime_t {
  NrUCIconRefreshTimer_t timerType;
  uint8_t timeValue;
};

class SetNrUCIconConfigureMessage : public SolicitedMessage<RIL_Errno>,
                                  public add_message_id<SetNrUCIconConfigureMessage> {
private:
  std::optional<uint8_t> mSibTwoValue;
  std::optional<NrUCIconBandInfo_t> mNsaBandList;
  std::optional<NrUCIconBandInfo_t> mSaBandList;
  std::vector<NrUCIconRefreshTime_t> mRefreshTimeList;
  std::optional<NrUCIconBandwidthInfo_t> mMinimumBandwidth;

public:
  static constexpr const char *MESSAGE_NAME = "SetNrUCIconConfigureMessage";
  SetNrUCIconConfigureMessage():SolicitedMessage<RIL_Errno>(get_class_message_id()) {
    mName = MESSAGE_NAME;
  }
  ~SetNrUCIconConfigureMessage() = default;

  void setSibTwoValue(uint8_t value) {
    mSibTwoValue = value;
  };
  void setNsaBandList(NrUCIconBandInfo_t bandinfo) {
    mNsaBandList = bandinfo;
  };
  void setSaBandList(NrUCIconBandInfo_t bandinfo) {
    mSaBandList = bandinfo;
  };
  void setRefreshTimeList(std::vector<NrUCIconRefreshTime_t> time) {
    mRefreshTimeList = time;
  }
  void setMinBandwidth(NrUCIconBandwidthInfo_t bwinfo) {
    mMinimumBandwidth = bwinfo;
  };
  uint8_t getSibTwoValue(void) {
    return *mSibTwoValue;
  };
  NrUCIconBandInfo_t getNsaBandList(void) {
    return *mNsaBandList;
  };
  NrUCIconBandInfo_t getSaBandList(void) {
    return *mSaBandList;
  };
  std::vector<NrUCIconRefreshTime_t> getRefreshTimeList(void) {
    return mRefreshTimeList;
  };
  NrUCIconBandwidthInfo_t getMinBandwidth(void) {
    return *mMinimumBandwidth;
  };
  bool hasSibTwoValue(void) {
    return mSibTwoValue ? true : false;
  };
  bool hasNsaBandList(void) {
    return mNsaBandList ? true : false;
  };
  bool hasSaBandList(void) {
    return mSaBandList ? true : false;
  };
  bool hasMinBandWidth(void) {
    return mMinimumBandwidth ? true : false;
  };


  string dump() {
    std::stringstream ss;
    ss << mName;
    return ss.str();
  }
};
} //namespace

#endif