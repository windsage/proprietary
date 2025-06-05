/**
 *  Copyright (c) 2020 Qualcomm Technologies, Inc.
 *  All Rights Reserved.
 *  Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include "modules/pbm/RilEmerNumFromPbm.h"

RilEmerNumFromPbm::~RilEmerNumFromPbm() {
  if (mCallback) {
    delete mCallback;
    mCallback = nullptr;
  }
}

std::string RilEmerNumFromPbm::getNumber() {
  return number;
}
void RilEmerNumFromPbm::setNumber(const std::string& num) {
  number = num;
}
string RilEmerNumFromPbm::dump() {
  return mName;
}
