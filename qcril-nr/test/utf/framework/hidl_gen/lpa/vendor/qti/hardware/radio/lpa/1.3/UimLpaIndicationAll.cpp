#define LOG_TAG "vendor.qti.hardware.radio.lpa@1.3::UimLpaIndication"

#include <log/log.h>
#include <cutils/trace.h>
#include <vendor/qti/hardware/radio/lpa/1.3/IUimLpaIndication.h>
namespace vendor {
namespace qti {
namespace hardware {
namespace radio {
namespace lpa {
namespace V1_3 {

const char* IUimLpaIndication::descriptor("vendor.qti.hardware.radio.lpa@1.3::IUimLpaIndication");

// Methods from ::vendor::qti::hardware::radio::lpa::V1_0::IUimLpaIndication follow.
// no default implementation for: ::android::hardware::Return<void> IUimLpaIndication::UimLpaHttpTxnIndication(const ::vendor::qti::hardware::radio::lpa::V1_0::UimLpaHttpTransactionInd& txnInd)
// no default implementation for: ::android::hardware::Return<void> IUimLpaIndication::UimLpaAddProfileProgressIndication(const ::vendor::qti::hardware::radio::lpa::V1_0::UimLpaAddProfileProgressInd& progressInd)

// Methods from ::vendor::qti::hardware::radio::lpa::V1_1::IUimLpaIndication follow.
// no default implementation for: ::android::hardware::Return<void> IUimLpaIndication::UimLpaAddProfileProgressIndication_1_1(const ::vendor::qti::hardware::radio::lpa::V1_1::UimLpaAddProfileProgressIndV1_1& progressInd)

// Methods from ::vendor::qti::hardware::radio::lpa::V1_3::IUimLpaIndication follow.
// no default implementation for: ::android::hardware::Return<void> IUimLpaIndication::UimLpaAddProfileProgressIndication_1_3(const ::vendor::qti::hardware::radio::lpa::V1_3::UimLpaAddProfileProgressIndV1_1& progressInd)
// no default implementation for: ::android::hardware::Return<void> IUimLpaIndication::UimLpaEndUserConsentIndication(::vendor::qti::hardware::radio::lpa::V1_3::UimLpaUserConsentType UserConsent)

// Methods from ::android::hidl::base::V1_0::IBase follow.
::android::hardware::Return<void> IUimLpaIndication::interfaceChain(interfaceChain_cb _hidl_cb){
    return ::android::hardware::Void();
}

::android::hardware::Return<void> IUimLpaIndication::debug(const ::android::hardware::hidl_handle& fd, const ::android::hardware::hidl_vec<::android::hardware::hidl_string>& options){
    (void)fd;
    (void)options;
    return ::android::hardware::Void();
}

::android::hardware::Return<void> IUimLpaIndication::interfaceDescriptor(interfaceDescriptor_cb _hidl_cb){
    _hidl_cb(::vendor::qti::hardware::radio::lpa::V1_3::IUimLpaIndication::descriptor);
    return ::android::hardware::Void();
}

::android::hardware::Return<void> IUimLpaIndication::getHashChain(getHashChain_cb _hidl_cb){
    _hidl_cb({
        (uint8_t[32]){138,71,187,8,250,166,189,240,53,196,250,193,8,141,158,146,188,212,82,136,45,250,121,198,23,144,6,79,250,105,202,95} /* 8a47bb08faa6bdf035c4fac1088d9e92bcd452882dfa79c61790064ffa69ca5f */,
        (uint8_t[32]){58,80,234,88,60,234,203,238,64,206,117,6,134,150,99,26,33,32,236,160,8,110,250,246,40,3,72,28,46,203,122,150} /* 3a50ea583ceacbee40ce75068696631a2120eca0086efaf62803481c2ecb7a96 */,
        (uint8_t[32]){254,217,233,191,179,141,251,24,240,220,86,77,113,223,215,47,164,51,81,199,114,38,250,12,160,26,190,170,197,94,212,122} /* fed9e9bfb38dfb18f0dc564d71dfd72fa43351c77226fa0ca01abeaac55ed47a */,
        (uint8_t[32]){236,127,215,158,208,45,250,133,188,73,148,38,173,174,62,190,35,239,5,36,243,205,105,87,19,147,36,184,59,24,202,76} /* ec7fd79ed02dfa85bc499426adae3ebe23ef0524f3cd6957139324b83b18ca4c */});
    return ::android::hardware::Void();
}

::android::hardware::Return<void> IUimLpaIndication::setHALInstrumentation(){
    return ::android::hardware::Void();
}

::android::hardware::Return<bool> IUimLpaIndication::linkToDeath(const ::android::sp<::android::hardware::hidl_death_recipient>& recipient, uint64_t cookie){
    (void)cookie;
    return (recipient != nullptr);
}

::android::hardware::Return<void> IUimLpaIndication::ping(){
    return ::android::hardware::Void();
}

::android::hardware::Return<void> IUimLpaIndication::getDebugInfo(getDebugInfo_cb _hidl_cb){
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

::android::hardware::Return<void> IUimLpaIndication::notifySyspropsChanged(){
    return ::android::hardware::Void();
}

::android::hardware::Return<bool> IUimLpaIndication::unlinkToDeath(const ::android::sp<::android::hardware::hidl_death_recipient>& recipient){
    return (recipient != nullptr);
}


::android::hardware::Return<::android::sp<::vendor::qti::hardware::radio::lpa::V1_3::IUimLpaIndication>> IUimLpaIndication::castFrom(const ::android::sp<::vendor::qti::hardware::radio::lpa::V1_3::IUimLpaIndication>& parent, bool /* emitError */) {
    return parent;
}

::android::hardware::Return<::android::sp<::vendor::qti::hardware::radio::lpa::V1_3::IUimLpaIndication>> IUimLpaIndication::castFrom(const ::android::sp<::vendor::qti::hardware::radio::lpa::V1_1::IUimLpaIndication>& parent, bool emitError) {
    return nullptr;
}

::android::hardware::Return<::android::sp<::vendor::qti::hardware::radio::lpa::V1_3::IUimLpaIndication>> IUimLpaIndication::castFrom(const ::android::sp<::vendor::qti::hardware::radio::lpa::V1_0::IUimLpaIndication>& parent, bool emitError) {
    return nullptr;
}

::android::hardware::Return<::android::sp<::vendor::qti::hardware::radio::lpa::V1_3::IUimLpaIndication>> IUimLpaIndication::castFrom(const ::android::sp<::android::hidl::base::V1_0::IBase>& parent, bool emitError) {
    return nullptr;
}

::android::sp<IUimLpaIndication> IUimLpaIndication::tryGetService(const std::string &serviceName, const bool getStub) {
    return nullptr;
}

::android::sp<IUimLpaIndication> IUimLpaIndication::getService(const std::string &serviceName, const bool getStub) {
    return nullptr;
}

::android::status_t IUimLpaIndication::registerAsService(const std::string &serviceName) {
    return ::android::OK;
}

bool IUimLpaIndication::registerForNotifications(
        const std::string &serviceName,
        const ::android::sp<::android::hidl::manager::V1_0::IServiceNotification> &notification) {
    return true;
}

static_assert(sizeof(::android::hardware::MQDescriptor<char, ::android::hardware::kSynchronizedReadWrite>) == 32, "wrong size");
static_assert(sizeof(::android::hardware::hidl_handle) == 16, "wrong size");
static_assert(sizeof(::android::hardware::hidl_memory) == 40, "wrong size");
static_assert(sizeof(::android::hardware::hidl_string) == 16, "wrong size");
static_assert(sizeof(::android::hardware::hidl_vec<char>) == 16, "wrong size");

}  // namespace V1_3
}  // namespace lpa
}  // namespace radio
}  // namespace hardware
}  // namespace qti
}  // namespace vendor
