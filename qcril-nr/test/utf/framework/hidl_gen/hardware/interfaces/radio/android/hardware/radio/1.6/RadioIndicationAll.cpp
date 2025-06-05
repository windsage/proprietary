#define LOG_TAG "android.hardware.radio@1.6::RadioIndication"
#undef UNUSED

#include <android/hardware/radio/1.6/IRadioIndication.h>
#include <log/log.h>
#include <cutils/trace.h>
#include "ril_utf_hidl_services.h"

namespace android {
namespace hardware {
namespace radio {
namespace V1_6 {

const char* IRadioIndication::descriptor("android.hardware.radio@1.6::IRadioIndication");

// Methods from ::android::hardware::radio::V1_0::IRadioIndication follow.
// no default implementation for: ::android::hardware::Return<void> IRadioIndication::radioStateChanged(::android::hardware::radio::V1_0::RadioIndicationType type, ::android::hardware::radio::V1_0::RadioState radioState)
// no default implementation for: ::android::hardware::Return<void> IRadioIndication::callStateChanged(::android::hardware::radio::V1_0::RadioIndicationType type)
// no default implementation for: ::android::hardware::Return<void> IRadioIndication::networkStateChanged(::android::hardware::radio::V1_0::RadioIndicationType type)
// no default implementation for: ::android::hardware::Return<void> IRadioIndication::newSms(::android::hardware::radio::V1_0::RadioIndicationType type, const ::android::hardware::hidl_vec<uint8_t>& pdu)
// no default implementation for: ::android::hardware::Return<void> IRadioIndication::newSmsStatusReport(::android::hardware::radio::V1_0::RadioIndicationType type, const ::android::hardware::hidl_vec<uint8_t>& pdu)
// no default implementation for: ::android::hardware::Return<void> IRadioIndication::newSmsOnSim(::android::hardware::radio::V1_0::RadioIndicationType type, int32_t recordNumber)
// no default implementation for: ::android::hardware::Return<void> IRadioIndication::onUssd(::android::hardware::radio::V1_0::RadioIndicationType type, ::android::hardware::radio::V1_0::UssdModeType modeType, const ::android::hardware::hidl_string& msg)
// no default implementation for: ::android::hardware::Return<void> IRadioIndication::nitzTimeReceived(::android::hardware::radio::V1_0::RadioIndicationType type, const ::android::hardware::hidl_string& nitzTime, uint64_t receivedTime)
// no default implementation for: ::android::hardware::Return<void> IRadioIndication::currentSignalStrength(::android::hardware::radio::V1_0::RadioIndicationType type, const ::android::hardware::radio::V1_0::SignalStrength& signalStrength)
// no default implementation for: ::android::hardware::Return<void> IRadioIndication::dataCallListChanged(::android::hardware::radio::V1_0::RadioIndicationType type, const ::android::hardware::hidl_vec<::android::hardware::radio::V1_0::SetupDataCallResult>& dcList)
// no default implementation for: ::android::hardware::Return<void> IRadioIndication::suppSvcNotify(::android::hardware::radio::V1_0::RadioIndicationType type, const ::android::hardware::radio::V1_0::SuppSvcNotification& suppSvc)
// no default implementation for: ::android::hardware::Return<void> IRadioIndication::stkSessionEnd(::android::hardware::radio::V1_0::RadioIndicationType type)
// no default implementation for: ::android::hardware::Return<void> IRadioIndication::stkProactiveCommand(::android::hardware::radio::V1_0::RadioIndicationType type, const ::android::hardware::hidl_string& cmd)
// no default implementation for: ::android::hardware::Return<void> IRadioIndication::stkEventNotify(::android::hardware::radio::V1_0::RadioIndicationType type, const ::android::hardware::hidl_string& cmd)
// no default implementation for: ::android::hardware::Return<void> IRadioIndication::stkCallSetup(::android::hardware::radio::V1_0::RadioIndicationType type, int64_t timeout)
// no default implementation for: ::android::hardware::Return<void> IRadioIndication::simSmsStorageFull(::android::hardware::radio::V1_0::RadioIndicationType type)
// no default implementation for: ::android::hardware::Return<void> IRadioIndication::simRefresh(::android::hardware::radio::V1_0::RadioIndicationType type, const ::android::hardware::radio::V1_0::SimRefreshResult& refreshResult)
// no default implementation for: ::android::hardware::Return<void> IRadioIndication::callRing(::android::hardware::radio::V1_0::RadioIndicationType type, bool isGsm, const ::android::hardware::radio::V1_0::CdmaSignalInfoRecord& record)
// no default implementation for: ::android::hardware::Return<void> IRadioIndication::simStatusChanged(::android::hardware::radio::V1_0::RadioIndicationType type)
// no default implementation for: ::android::hardware::Return<void> IRadioIndication::cdmaNewSms(::android::hardware::radio::V1_0::RadioIndicationType type, const ::android::hardware::radio::V1_0::CdmaSmsMessage& msg)
// no default implementation for: ::android::hardware::Return<void> IRadioIndication::newBroadcastSms(::android::hardware::radio::V1_0::RadioIndicationType type, const ::android::hardware::hidl_vec<uint8_t>& data)
// no default implementation for: ::android::hardware::Return<void> IRadioIndication::cdmaRuimSmsStorageFull(::android::hardware::radio::V1_0::RadioIndicationType type)
// no default implementation for: ::android::hardware::Return<void> IRadioIndication::restrictedStateChanged(::android::hardware::radio::V1_0::RadioIndicationType type, ::android::hardware::radio::V1_0::PhoneRestrictedState state)
// no default implementation for: ::android::hardware::Return<void> IRadioIndication::enterEmergencyCallbackMode(::android::hardware::radio::V1_0::RadioIndicationType type)
// no default implementation for: ::android::hardware::Return<void> IRadioIndication::cdmaCallWaiting(::android::hardware::radio::V1_0::RadioIndicationType type, const ::android::hardware::radio::V1_0::CdmaCallWaiting& callWaitingRecord)
// no default implementation for: ::android::hardware::Return<void> IRadioIndication::cdmaOtaProvisionStatus(::android::hardware::radio::V1_0::RadioIndicationType type, ::android::hardware::radio::V1_0::CdmaOtaProvisionStatus status)
// no default implementation for: ::android::hardware::Return<void> IRadioIndication::cdmaInfoRec(::android::hardware::radio::V1_0::RadioIndicationType type, const ::android::hardware::radio::V1_0::CdmaInformationRecords& records)
// no default implementation for: ::android::hardware::Return<void> IRadioIndication::indicateRingbackTone(::android::hardware::radio::V1_0::RadioIndicationType type, bool start)
// no default implementation for: ::android::hardware::Return<void> IRadioIndication::resendIncallMute(::android::hardware::radio::V1_0::RadioIndicationType type)
// no default implementation for: ::android::hardware::Return<void> IRadioIndication::cdmaSubscriptionSourceChanged(::android::hardware::radio::V1_0::RadioIndicationType type, ::android::hardware::radio::V1_0::CdmaSubscriptionSource cdmaSource)
// no default implementation for: ::android::hardware::Return<void> IRadioIndication::cdmaPrlChanged(::android::hardware::radio::V1_0::RadioIndicationType type, int32_t version)
// no default implementation for: ::android::hardware::Return<void> IRadioIndication::exitEmergencyCallbackMode(::android::hardware::radio::V1_0::RadioIndicationType type)
// no default implementation for: ::android::hardware::Return<void> IRadioIndication::rilConnected(::android::hardware::radio::V1_0::RadioIndicationType type)
// no default implementation for: ::android::hardware::Return<void> IRadioIndication::voiceRadioTechChanged(::android::hardware::radio::V1_0::RadioIndicationType type, ::android::hardware::radio::V1_0::RadioTechnology rat)
// no default implementation for: ::android::hardware::Return<void> IRadioIndication::cellInfoList(::android::hardware::radio::V1_0::RadioIndicationType type, const ::android::hardware::hidl_vec<::android::hardware::radio::V1_0::CellInfo>& records)
// no default implementation for: ::android::hardware::Return<void> IRadioIndication::imsNetworkStateChanged(::android::hardware::radio::V1_0::RadioIndicationType type)
// no default implementation for: ::android::hardware::Return<void> IRadioIndication::subscriptionStatusChanged(::android::hardware::radio::V1_0::RadioIndicationType type, bool activate)
// no default implementation for: ::android::hardware::Return<void> IRadioIndication::srvccStateNotify(::android::hardware::radio::V1_0::RadioIndicationType type, ::android::hardware::radio::V1_0::SrvccState state)
// no default implementation for: ::android::hardware::Return<void> IRadioIndication::hardwareConfigChanged(::android::hardware::radio::V1_0::RadioIndicationType type, const ::android::hardware::hidl_vec<::android::hardware::radio::V1_0::HardwareConfig>& configs)
// no default implementation for: ::android::hardware::Return<void> IRadioIndication::radioCapabilityIndication(::android::hardware::radio::V1_0::RadioIndicationType type, const ::android::hardware::radio::V1_0::RadioCapability& rc)
// no default implementation for: ::android::hardware::Return<void> IRadioIndication::onSupplementaryServiceIndication(::android::hardware::radio::V1_0::RadioIndicationType type, const ::android::hardware::radio::V1_0::StkCcUnsolSsResult& ss)
// no default implementation for: ::android::hardware::Return<void> IRadioIndication::stkCallControlAlphaNotify(::android::hardware::radio::V1_0::RadioIndicationType type, const ::android::hardware::hidl_string& alpha)
// no default implementation for: ::android::hardware::Return<void> IRadioIndication::lceData(::android::hardware::radio::V1_0::RadioIndicationType type, const ::android::hardware::radio::V1_0::LceDataInfo& lce)
// no default implementation for: ::android::hardware::Return<void> IRadioIndication::pcoData(::android::hardware::radio::V1_0::RadioIndicationType type, const ::android::hardware::radio::V1_0::PcoDataInfo& pco)
// no default implementation for: ::android::hardware::Return<void> IRadioIndication::modemReset(::android::hardware::radio::V1_0::RadioIndicationType type, const ::android::hardware::hidl_string& reason)

// Methods from ::android::hardware::radio::V1_1::IRadioIndication follow.
// no default implementation for: ::android::hardware::Return<void> IRadioIndication::carrierInfoForImsiEncryption(::android::hardware::radio::V1_0::RadioIndicationType info)
// no default implementation for: ::android::hardware::Return<void> IRadioIndication::networkScanResult(::android::hardware::radio::V1_0::RadioIndicationType type, const ::android::hardware::radio::V1_1::NetworkScanResult& result)
// no default implementation for: ::android::hardware::Return<void> IRadioIndication::keepaliveStatus(::android::hardware::radio::V1_0::RadioIndicationType type, const ::android::hardware::radio::V1_1::KeepaliveStatus& status)

// Methods from ::android::hardware::radio::V1_2::IRadioIndication follow.
// no default implementation for: ::android::hardware::Return<void> IRadioIndication::networkScanResult_1_2(::android::hardware::radio::V1_0::RadioIndicationType type, const ::android::hardware::radio::V1_2::NetworkScanResult& result)
// no default implementation for: ::android::hardware::Return<void> IRadioIndication::cellInfoList_1_2(::android::hardware::radio::V1_0::RadioIndicationType type, const ::android::hardware::hidl_vec<::android::hardware::radio::V1_2::CellInfo>& records)
// no default implementation for: ::android::hardware::Return<void> IRadioIndication::currentLinkCapacityEstimate(::android::hardware::radio::V1_0::RadioIndicationType type, const ::android::hardware::radio::V1_2::LinkCapacityEstimate& lce)
// no default implementation for: ::android::hardware::Return<void> IRadioIndication::currentPhysicalChannelConfigs(::android::hardware::radio::V1_0::RadioIndicationType type, const ::android::hardware::hidl_vec<::android::hardware::radio::V1_2::PhysicalChannelConfig>& configs)
// no default implementation for: ::android::hardware::Return<void> IRadioIndication::currentSignalStrength_1_2(::android::hardware::radio::V1_0::RadioIndicationType type, const ::android::hardware::radio::V1_2::SignalStrength& signalStrength)

// Methods from ::android::hardware::radio::V1_4::IRadioIndication follow.
// no default implementation for: ::android::hardware::Return<void> IRadioIndication::currentEmergencyNumberList(::android::hardware::radio::V1_0::RadioIndicationType type, const ::android::hardware::hidl_vec<::android::hardware::radio::V1_4::EmergencyNumber>& emergencyNumberList)
// no default implementation for: ::android::hardware::Return<void> IRadioIndication::cellInfoList_1_4(::android::hardware::radio::V1_0::RadioIndicationType type, const ::android::hardware::hidl_vec<::android::hardware::radio::V1_4::CellInfo>& records)
// no default implementation for: ::android::hardware::Return<void> IRadioIndication::networkScanResult_1_4(::android::hardware::radio::V1_0::RadioIndicationType type, const ::android::hardware::radio::V1_4::NetworkScanResult& result)
// no default implementation for: ::android::hardware::Return<void> IRadioIndication::currentPhysicalChannelConfigs_1_4(::android::hardware::radio::V1_0::RadioIndicationType type, const ::android::hardware::hidl_vec<::android::hardware::radio::V1_4::PhysicalChannelConfig>& configs)
// no default implementation for: ::android::hardware::Return<void> IRadioIndication::dataCallListChanged_1_4(::android::hardware::radio::V1_0::RadioIndicationType type, const ::android::hardware::hidl_vec<::android::hardware::radio::V1_4::SetupDataCallResult>& dcList)
// no default implementation for: ::android::hardware::Return<void> IRadioIndication::currentSignalStrength_1_4(::android::hardware::radio::V1_0::RadioIndicationType type, const ::android::hardware::radio::V1_4::SignalStrength& signalStrength)

// Methods from ::android::hardware::radio::V1_5::IRadioIndication follow.
// no default implementation for: ::android::hardware::Return<void> IRadioIndication::uiccApplicationsEnablementChanged(::android::hardware::radio::V1_0::RadioIndicationType type, bool enabled)
// no default implementation for: ::android::hardware::Return<void> IRadioIndication::registrationFailed(::android::hardware::radio::V1_0::RadioIndicationType type, const ::android::hardware::radio::V1_5::CellIdentity& cellIdentity, const ::android::hardware::hidl_string& chosenPlmn, ::android::hardware::hidl_bitfield<::android::hardware::radio::V1_5::Domain> domain, int32_t causeCode, int32_t additionalCauseCode)
// no default implementation for: ::android::hardware::Return<void> IRadioIndication::barringInfoChanged(::android::hardware::radio::V1_0::RadioIndicationType type, const ::android::hardware::radio::V1_5::CellIdentity& cellIdentity, const ::android::hardware::hidl_vec<::android::hardware::radio::V1_5::BarringInfo>& barringInfos)
// no default implementation for: ::android::hardware::Return<void> IRadioIndication::cellInfoList_1_5(::android::hardware::radio::V1_0::RadioIndicationType type, const ::android::hardware::hidl_vec<::android::hardware::radio::V1_5::CellInfo>& records)
// no default implementation for: ::android::hardware::Return<void> IRadioIndication::networkScanResult_1_5(::android::hardware::radio::V1_0::RadioIndicationType type, const ::android::hardware::radio::V1_5::NetworkScanResult& result)
// no default implementation for: ::android::hardware::Return<void> IRadioIndication::dataCallListChanged_1_5(::android::hardware::radio::V1_0::RadioIndicationType type, const ::android::hardware::hidl_vec<::android::hardware::radio::V1_5::SetupDataCallResult>& dcList)

// Methods from ::android::hardware::radio::V1_6::IRadioIndication follow.
// no default implementation for: ::android::hardware::Return<void> IRadioIndication::dataCallListChanged_1_6(::android::hardware::radio::V1_0::RadioIndicationType type, const ::android::hardware::hidl_vec<::android::hardware::radio::V1_6::SetupDataCallResult>& dcList)
// no default implementation for: ::android::hardware::Return<void> IRadioIndication::unthrottleApn(::android::hardware::radio::V1_0::RadioIndicationType type, const ::android::hardware::hidl_string& apn)
// no default implementation for: ::android::hardware::Return<void> IRadioIndication::currentLinkCapacityEstimate_1_6(::android::hardware::radio::V1_0::RadioIndicationType type, const ::android::hardware::radio::V1_6::LinkCapacityEstimate& lce)

// Methods from ::android::hidl::base::V1_0::IBase follow.
::android::hardware::Return<void> IRadioIndication::interfaceChain(interfaceChain_cb _hidl_cb){
    return ::android::hardware::Void();
}

::android::hardware::Return<void> IRadioIndication::debug(const ::android::hardware::hidl_handle& fd, const ::android::hardware::hidl_vec<::android::hardware::hidl_string>& options){
    (void)fd;
    (void)options;
    return ::android::hardware::Void();
}

::android::hardware::Return<void> IRadioIndication::interfaceDescriptor(interfaceDescriptor_cb _hidl_cb){
    _hidl_cb(::android::hardware::radio::V1_6::IRadioIndication::descriptor);
    return ::android::hardware::Void();
}

::android::hardware::Return<void> IRadioIndication::getHashChain(getHashChain_cb _hidl_cb){
    _hidl_cb({
        (uint8_t[32]){0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0} /* 0000000000000000000000000000000000000000000000000000000000000000 */,
        (uint8_t[32]){252,187,7,66,168,130,21,238,122,109,124,224,130,93,37,62,178,181,3,145,252,108,140,72,102,127,159,215,246,212,84,158} /* fcbb0742a88215ee7a6d7ce0825d253eb2b50391fc6c8c48667f9fd7f6d4549e */,
        (uint8_t[32]){51,217,230,137,92,202,152,170,86,41,107,176,23,32,209,139,138,205,14,77,228,150,11,235,113,46,99,173,20,116,56,165} /* 33d9e6895cca98aa56296bb01720d18b8acd0e4de4960beb712e63ad147438a5 */,
        (uint8_t[32]){233,208,241,26,82,113,95,90,41,216,158,45,142,46,33,219,30,22,164,49,116,175,107,157,81,166,45,112,92,218,20,85} /* e9d0f11a52715f5a29d89e2d8e2e21db1e16a43174af6b9d51a62d705cda1455 */,
        (uint8_t[32]){205,167,82,174,171,170,188,32,72,106,130,172,87,163,221,16,119,133,192,6,9,74,52,155,197,226,36,232,170,34,161,124} /* cda752aeabaabc20486a82ac57a3dd107785c006094a349bc5e224e8aa22a17c */,
        (uint8_t[32]){252,197,200,200,139,133,169,246,63,186,103,217,230,116,218,70,108,114,169,140,162,135,243,67,251,87,33,208,152,113,63,134} /* fcc5c8c88b85a9f63fba67d9e674da466c72a98ca287f343fb5721d098713f86 */,
        (uint8_t[32]){137,215,143,164,155,9,226,243,24,18,187,99,225,191,172,43,243,24,169,86,20,115,198,176,237,105,4,206,24,55,125,84} /* 89d78fa49b09e2f31812bb63e1bfac2bf318a9561473c6b0ed6904ce18377d54 */,
        (uint8_t[32]){236,127,215,158,208,45,250,133,188,73,148,38,173,174,62,190,35,239,5,36,243,205,105,87,19,147,36,184,59,24,202,76} /* ec7fd79ed02dfa85bc499426adae3ebe23ef0524f3cd6957139324b83b18ca4c */});
    return ::android::hardware::Void();
}

::android::hardware::Return<void> IRadioIndication::setHALInstrumentation(){
    return ::android::hardware::Void();
}

::android::hardware::Return<bool> IRadioIndication::linkToDeath(const ::android::sp<::android::hardware::hidl_death_recipient>& recipient, uint64_t cookie){
    (void)cookie;
    return (recipient != nullptr);
}

::android::hardware::Return<void> IRadioIndication::ping(){
    return ::android::hardware::Void();
}

::android::hardware::Return<void> IRadioIndication::getDebugInfo(getDebugInfo_cb _hidl_cb){
    ::android::hidl::base::V1_0::DebugInfo info = {};
    info.pid = -1;
    info.ptr = 0;
    info.arch = 
    #if defined(__LP64__)
    ::android::hidl::base::V1_0::DebugInfo::Architecture::IS_64BIT
    #else
    ::android::hidl::base::V1_0::DebugInfo::Architecture::IS_32BIT
    #endif
    ;
    _hidl_cb(info);
    return ::android::hardware::Void();
}

::android::hardware::Return<void> IRadioIndication::notifySyspropsChanged(){
    return ::android::hardware::Void();
}

::android::hardware::Return<bool> IRadioIndication::unlinkToDeath(const ::android::sp<::android::hardware::hidl_death_recipient>& recipient){
    return (recipient != nullptr);
}


::android::hardware::Return<::android::sp<::android::hardware::radio::V1_6::IRadioIndication>> IRadioIndication::castFrom(const ::android::sp<::android::hardware::radio::V1_6::IRadioIndication>& parent, bool /* emitError */) {
    return parent;
}

::android::hardware::Return<::android::sp<::android::hardware::radio::V1_6::IRadioIndication>> IRadioIndication::castFrom(const ::android::sp<::android::hardware::radio::V1_5::IRadioIndication>& parent, bool emitError) {
    return ::android::sp<::android::hardware::radio::V1_6::IRadioIndication>(static_cast<::android::hardware::radio::V1_6::IRadioIndication*>(parent.get()));
}

::android::hardware::Return<::android::sp<::android::hardware::radio::V1_6::IRadioIndication>> IRadioIndication::castFrom(const ::android::sp<::android::hardware::radio::V1_4::IRadioIndication>& parent, bool emitError) {
    return ::android::sp<::android::hardware::radio::V1_6::IRadioIndication>(static_cast<::android::hardware::radio::V1_6::IRadioIndication*>(parent.get()));
}

::android::hardware::Return<::android::sp<::android::hardware::radio::V1_6::IRadioIndication>> IRadioIndication::castFrom(const ::android::sp<::android::hardware::radio::V1_3::IRadioIndication>& parent, bool emitError) {
    return ::android::sp<::android::hardware::radio::V1_6::IRadioIndication>(static_cast<::android::hardware::radio::V1_6::IRadioIndication*>(parent.get()));
}

::android::hardware::Return<::android::sp<::android::hardware::radio::V1_6::IRadioIndication>> IRadioIndication::castFrom(const ::android::sp<::android::hardware::radio::V1_2::IRadioIndication>& parent, bool emitError) {
    return ::android::sp<::android::hardware::radio::V1_6::IRadioIndication>(static_cast<::android::hardware::radio::V1_6::IRadioIndication*>(parent.get()));
}

::android::hardware::Return<::android::sp<::android::hardware::radio::V1_6::IRadioIndication>> IRadioIndication::castFrom(const ::android::sp<::android::hardware::radio::V1_1::IRadioIndication>& parent, bool emitError) {
    return ::android::sp<::android::hardware::radio::V1_6::IRadioIndication>(static_cast<::android::hardware::radio::V1_6::IRadioIndication*>(parent.get()));
}

::android::hardware::Return<::android::sp<::android::hardware::radio::V1_6::IRadioIndication>> IRadioIndication::castFrom(const ::android::sp<::android::hardware::radio::V1_0::IRadioIndication>& parent, bool emitError) {
    return ::android::sp<::android::hardware::radio::V1_6::IRadioIndication>(static_cast<::android::hardware::radio::V1_6::IRadioIndication*>(parent.get()));
}

::android::hardware::Return<::android::sp<::android::hardware::radio::V1_6::IRadioIndication>> IRadioIndication::castFrom(const ::android::sp<::android::hidl::base::V1_0::IBase>& parent, bool emitError) {
    return ::android::sp<::android::hardware::radio::V1_6::IRadioIndication>(static_cast<::android::hardware::radio::V1_6::IRadioIndication*>(parent.get()));
}

::android::sp<IRadioIndication> IRadioIndication::tryGetService(const std::string &serviceName, const bool getStub) {
    return nullptr;
}

::android::sp<IRadioIndication> IRadioIndication::getService(const std::string &serviceName, const bool getStub) {
    return nullptr;
}

::android::status_t IRadioIndication::registerAsService(const std::string &serviceName) {
  return ::android::OK;
}

bool IRadioIndication::registerForNotifications(
        const std::string &serviceName,
        const ::android::sp<::android::hidl::manager::V1_0::IServiceNotification> &notification) {
    return true;
}

static_assert(sizeof(::android::hardware::MQDescriptor<char, ::android::hardware::kSynchronizedReadWrite>) == 32, "wrong size");
static_assert(sizeof(::android::hardware::hidl_handle) == 16, "wrong size");
static_assert(sizeof(::android::hardware::hidl_memory) == 40, "wrong size");
static_assert(sizeof(::android::hardware::hidl_string) == 16, "wrong size");
static_assert(sizeof(::android::hardware::hidl_vec<char>) == 16, "wrong size");

}  // namespace V1_6
}  // namespace radio
}  // namespace hardware
}  // namespace android
