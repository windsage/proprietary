/**
* Copyright (c) 2018-2020, 2023 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
**/

#ifndef INITTRACKER
#define INITTRACKER

namespace rildata {

 enum IWLANOperationMode {
   LEGACY = 1,
   AP_ASSISTED
 };
 class InitTracker {
 private:
    bool mDsdServiceReady;
    bool mWdsServiceReady;
    IWLANOperationMode mOperMode;
    bool mIWLANEnabled;
    bool mPartialRetryEnabled;
    bool mModemCapability;
 public:
    InitTracker() : mDsdServiceReady(false),
      mWdsServiceReady(false), mIWLANEnabled(false),
      mPartialRetryEnabled(true), mModemCapability(false)
      {
#ifdef RIL_FOR_MDM_LE
        mOperMode = LEGACY;
#else
        mOperMode = AP_ASSISTED;
#endif
      }
    bool allServicesReady() {
      return mDsdServiceReady && mWdsServiceReady;
    }
    bool isIWLANEnabled () {
      return mIWLANEnabled;
    }
    void setIWLANEnabled(bool enabled) {
      mIWLANEnabled = enabled;
    }
    bool isPartialRetryEnabled () {
      return mPartialRetryEnabled;
    }
    void setPartialRetryEnabled(bool enabled) {
      mPartialRetryEnabled = enabled;
    }
    bool isAPAssistMode() {
      return mOperMode == AP_ASSISTED;
    }
    void setIWLANMode(IWLANOperationMode mode)
    {
#ifndef RIL_FOR_MDM_LE
      mOperMode = mode;
#endif
    }
    void setModemCapability(bool capability) {
      mModemCapability = capability;
    }
    bool getModemCapability() {
      return mModemCapability;
    }
    bool getDsdServiceReady() {
      return mDsdServiceReady;
    }
    bool getWdsServiceReady() {
      return mWdsServiceReady;
    }
    void setDsdServiceReady(bool ready) {
      mDsdServiceReady = ready;
    }
    void setWdsServiceReady(bool ready) {
      mWdsServiceReady = ready;
    }
    void dump(std::string padding, std::ostream& os) {
      os << padding << std::boolalpha << "DsdServiceReady=" << mDsdServiceReady << endl;
      os << padding << std::boolalpha << "WdsServiceReady=" << mWdsServiceReady << endl;
      os << padding << "OperMode=" << mOperMode << endl;
      os << padding << std::boolalpha << "IWLANEnabled=" << mIWLANEnabled << endl;
      os << padding << std::boolalpha << "ModemCapability=" << mModemCapability << endl;
    }
  };

} //namespace

#endif
