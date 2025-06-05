#ifndef HIDL_GENERATED_VENDOR_QTI_HARDWARE_RADIO_LPA_V1_3_TYPES_H
#define HIDL_GENERATED_VENDOR_QTI_HARDWARE_RADIO_LPA_V1_3_TYPES_H

#include <vendor/qti/hardware/radio/lpa/1.1/types.h>

#include <hidl/HidlSupport.h>
#include <hidl/MQDescriptor.h>
#include <utils/NativeHandle.h>
#include <utils/misc.h>

namespace vendor {
namespace qti {
namespace hardware {
namespace radio {
namespace lpa {
namespace V1_3 {

// Forward declaration for forward reference support:
enum class UimLpaUserConsentType : uint32_t;
struct UimLpaUserReq;
struct UimLpaAddProfileProgressIndV1_1;

enum class UimLpaUserConsentType : uint32_t {
    UIM_LPA_NO_CONFIRMATION_REQD = 0u,
    UIM_LPA_SIMPLE_CONFIRMATION_REQD = 1u,
    UIM_LPA_STRONG_CONFIRMATION_REQD = 2u,
};

struct UimLpaUserReq final {
    ::vendor::qti::hardware::radio::lpa::V1_1::UimLpaUserReq base __attribute__ ((aligned(8)));
    ::vendor::qti::hardware::radio::lpa::V1_3::UimLpaUserConsentType user_consent_type __attribute__ ((aligned(4)));
};

static_assert(offsetof(::vendor::qti::hardware::radio::lpa::V1_3::UimLpaUserReq, base) == 0, "wrong offset");
static_assert(offsetof(::vendor::qti::hardware::radio::lpa::V1_3::UimLpaUserReq, user_consent_type) == 112, "wrong offset");
static_assert(sizeof(::vendor::qti::hardware::radio::lpa::V1_3::UimLpaUserReq) == 120, "wrong size");
static_assert(__alignof(::vendor::qti::hardware::radio::lpa::V1_3::UimLpaUserReq) == 8, "wrong alignment");

struct UimLpaAddProfileProgressIndV1_1 final {
    ::vendor::qti::hardware::radio::lpa::V1_1::UimLpaAddProfileProgressIndV1_1 base __attribute__ ((aligned(8)));
    ::vendor::qti::hardware::radio::lpa::V1_3::UimLpaUserConsentType user_consent_type __attribute__ ((aligned(4)));
};

static_assert(offsetof(::vendor::qti::hardware::radio::lpa::V1_3::UimLpaAddProfileProgressIndV1_1, base) == 0, "wrong offset");
static_assert(offsetof(::vendor::qti::hardware::radio::lpa::V1_3::UimLpaAddProfileProgressIndV1_1, user_consent_type) == 40, "wrong offset");
static_assert(sizeof(::vendor::qti::hardware::radio::lpa::V1_3::UimLpaAddProfileProgressIndV1_1) == 48, "wrong size");
static_assert(__alignof(::vendor::qti::hardware::radio::lpa::V1_3::UimLpaAddProfileProgressIndV1_1) == 8, "wrong alignment");

//
// type declarations for package
//

template<typename>
static inline std::string toString(uint32_t o);
static inline std::string toString(::vendor::qti::hardware::radio::lpa::V1_3::UimLpaUserConsentType o);
static inline void PrintTo(::vendor::qti::hardware::radio::lpa::V1_3::UimLpaUserConsentType o, ::std::ostream* os);
constexpr uint32_t operator|(const ::vendor::qti::hardware::radio::lpa::V1_3::UimLpaUserConsentType lhs, const ::vendor::qti::hardware::radio::lpa::V1_3::UimLpaUserConsentType rhs) {
    return static_cast<uint32_t>(static_cast<uint32_t>(lhs) | static_cast<uint32_t>(rhs));
}
constexpr uint32_t operator|(const uint32_t lhs, const ::vendor::qti::hardware::radio::lpa::V1_3::UimLpaUserConsentType rhs) {
    return static_cast<uint32_t>(lhs | static_cast<uint32_t>(rhs));
}
constexpr uint32_t operator|(const ::vendor::qti::hardware::radio::lpa::V1_3::UimLpaUserConsentType lhs, const uint32_t rhs) {
    return static_cast<uint32_t>(static_cast<uint32_t>(lhs) | rhs);
}
constexpr uint32_t operator&(const ::vendor::qti::hardware::radio::lpa::V1_3::UimLpaUserConsentType lhs, const ::vendor::qti::hardware::radio::lpa::V1_3::UimLpaUserConsentType rhs) {
    return static_cast<uint32_t>(static_cast<uint32_t>(lhs) & static_cast<uint32_t>(rhs));
}
constexpr uint32_t operator&(const uint32_t lhs, const ::vendor::qti::hardware::radio::lpa::V1_3::UimLpaUserConsentType rhs) {
    return static_cast<uint32_t>(lhs & static_cast<uint32_t>(rhs));
}
constexpr uint32_t operator&(const ::vendor::qti::hardware::radio::lpa::V1_3::UimLpaUserConsentType lhs, const uint32_t rhs) {
    return static_cast<uint32_t>(static_cast<uint32_t>(lhs) & rhs);
}
constexpr uint32_t &operator|=(uint32_t& v, const ::vendor::qti::hardware::radio::lpa::V1_3::UimLpaUserConsentType e) {
    v |= static_cast<uint32_t>(e);
    return v;
}
constexpr uint32_t &operator&=(uint32_t& v, const ::vendor::qti::hardware::radio::lpa::V1_3::UimLpaUserConsentType e) {
    v &= static_cast<uint32_t>(e);
    return v;
}

static inline std::string toString(const ::vendor::qti::hardware::radio::lpa::V1_3::UimLpaUserReq& o);
static inline void PrintTo(const ::vendor::qti::hardware::radio::lpa::V1_3::UimLpaUserReq& o, ::std::ostream*);
static inline bool operator==(const ::vendor::qti::hardware::radio::lpa::V1_3::UimLpaUserReq& lhs, const ::vendor::qti::hardware::radio::lpa::V1_3::UimLpaUserReq& rhs);
static inline bool operator!=(const ::vendor::qti::hardware::radio::lpa::V1_3::UimLpaUserReq& lhs, const ::vendor::qti::hardware::radio::lpa::V1_3::UimLpaUserReq& rhs);

static inline std::string toString(const ::vendor::qti::hardware::radio::lpa::V1_3::UimLpaAddProfileProgressIndV1_1& o);
static inline void PrintTo(const ::vendor::qti::hardware::radio::lpa::V1_3::UimLpaAddProfileProgressIndV1_1& o, ::std::ostream*);
static inline bool operator==(const ::vendor::qti::hardware::radio::lpa::V1_3::UimLpaAddProfileProgressIndV1_1& lhs, const ::vendor::qti::hardware::radio::lpa::V1_3::UimLpaAddProfileProgressIndV1_1& rhs);
static inline bool operator!=(const ::vendor::qti::hardware::radio::lpa::V1_3::UimLpaAddProfileProgressIndV1_1& lhs, const ::vendor::qti::hardware::radio::lpa::V1_3::UimLpaAddProfileProgressIndV1_1& rhs);

//
// type header definitions for package
//

template<>
inline std::string toString<::vendor::qti::hardware::radio::lpa::V1_3::UimLpaUserConsentType>(uint32_t o) {
    using ::android::hardware::details::toHexString;
    std::string os;
    ::android::hardware::hidl_bitfield<::vendor::qti::hardware::radio::lpa::V1_3::UimLpaUserConsentType> flipped = 0;
    bool first = true;
    if ((o & ::vendor::qti::hardware::radio::lpa::V1_3::UimLpaUserConsentType::UIM_LPA_NO_CONFIRMATION_REQD) == static_cast<uint32_t>(::vendor::qti::hardware::radio::lpa::V1_3::UimLpaUserConsentType::UIM_LPA_NO_CONFIRMATION_REQD)) {
        os += (first ? "" : " | ");
        os += "UIM_LPA_NO_CONFIRMATION_REQD";
        first = false;
        flipped |= ::vendor::qti::hardware::radio::lpa::V1_3::UimLpaUserConsentType::UIM_LPA_NO_CONFIRMATION_REQD;
    }
    if ((o & ::vendor::qti::hardware::radio::lpa::V1_3::UimLpaUserConsentType::UIM_LPA_SIMPLE_CONFIRMATION_REQD) == static_cast<uint32_t>(::vendor::qti::hardware::radio::lpa::V1_3::UimLpaUserConsentType::UIM_LPA_SIMPLE_CONFIRMATION_REQD)) {
        os += (first ? "" : " | ");
        os += "UIM_LPA_SIMPLE_CONFIRMATION_REQD";
        first = false;
        flipped |= ::vendor::qti::hardware::radio::lpa::V1_3::UimLpaUserConsentType::UIM_LPA_SIMPLE_CONFIRMATION_REQD;
    }
    if ((o & ::vendor::qti::hardware::radio::lpa::V1_3::UimLpaUserConsentType::UIM_LPA_STRONG_CONFIRMATION_REQD) == static_cast<uint32_t>(::vendor::qti::hardware::radio::lpa::V1_3::UimLpaUserConsentType::UIM_LPA_STRONG_CONFIRMATION_REQD)) {
        os += (first ? "" : " | ");
        os += "UIM_LPA_STRONG_CONFIRMATION_REQD";
        first = false;
        flipped |= ::vendor::qti::hardware::radio::lpa::V1_3::UimLpaUserConsentType::UIM_LPA_STRONG_CONFIRMATION_REQD;
    }
    if (o != flipped) {
        os += (first ? "" : " | ");
        os += toHexString(o & (~flipped));
    }os += " (";
    os += toHexString(o);
    os += ")";
    return os;
}

static inline std::string toString(::vendor::qti::hardware::radio::lpa::V1_3::UimLpaUserConsentType o) {
    using ::android::hardware::details::toHexString;
    if (o == ::vendor::qti::hardware::radio::lpa::V1_3::UimLpaUserConsentType::UIM_LPA_NO_CONFIRMATION_REQD) {
        return "UIM_LPA_NO_CONFIRMATION_REQD";
    }
    if (o == ::vendor::qti::hardware::radio::lpa::V1_3::UimLpaUserConsentType::UIM_LPA_SIMPLE_CONFIRMATION_REQD) {
        return "UIM_LPA_SIMPLE_CONFIRMATION_REQD";
    }
    if (o == ::vendor::qti::hardware::radio::lpa::V1_3::UimLpaUserConsentType::UIM_LPA_STRONG_CONFIRMATION_REQD) {
        return "UIM_LPA_STRONG_CONFIRMATION_REQD";
    }
    std::string os;
    os += toHexString(static_cast<uint32_t>(o));
    return os;
}

static inline void PrintTo(::vendor::qti::hardware::radio::lpa::V1_3::UimLpaUserConsentType o, ::std::ostream* os) {
    *os << toString(o);
}

static inline std::string toString(const ::vendor::qti::hardware::radio::lpa::V1_3::UimLpaUserReq& o) {
    using ::android::hardware::toString;
    std::string os;
    os += "{";
    os += ".base = ";
    os += ::vendor::qti::hardware::radio::lpa::V1_1::toString(o.base);
    os += ", .user_consent_type = ";
    os += ::vendor::qti::hardware::radio::lpa::V1_3::toString(o.user_consent_type);
    os += "}"; return os;
}

static inline void PrintTo(const ::vendor::qti::hardware::radio::lpa::V1_3::UimLpaUserReq& o, ::std::ostream* os) {
    *os << toString(o);
}

static inline bool operator==(const ::vendor::qti::hardware::radio::lpa::V1_3::UimLpaUserReq& lhs, const ::vendor::qti::hardware::radio::lpa::V1_3::UimLpaUserReq& rhs) {
    if (lhs.base != rhs.base) {
        return false;
    }
    if (lhs.user_consent_type != rhs.user_consent_type) {
        return false;
    }
    return true;
}

static inline bool operator!=(const ::vendor::qti::hardware::radio::lpa::V1_3::UimLpaUserReq& lhs, const ::vendor::qti::hardware::radio::lpa::V1_3::UimLpaUserReq& rhs){
    return !(lhs == rhs);
}

static inline std::string toString(const ::vendor::qti::hardware::radio::lpa::V1_3::UimLpaAddProfileProgressIndV1_1& o) {
    using ::android::hardware::toString;
    std::string os;
    os += "{";
    os += ".base = ";
    os += ::vendor::qti::hardware::radio::lpa::V1_1::toString(o.base);
    os += ", .user_consent_type = ";
    os += ::vendor::qti::hardware::radio::lpa::V1_3::toString(o.user_consent_type);
    os += "}"; return os;
}

static inline void PrintTo(const ::vendor::qti::hardware::radio::lpa::V1_3::UimLpaAddProfileProgressIndV1_1& o, ::std::ostream* os) {
    *os << toString(o);
}

static inline bool operator==(const ::vendor::qti::hardware::radio::lpa::V1_3::UimLpaAddProfileProgressIndV1_1& lhs, const ::vendor::qti::hardware::radio::lpa::V1_3::UimLpaAddProfileProgressIndV1_1& rhs) {
    if (lhs.base != rhs.base) {
        return false;
    }
    if (lhs.user_consent_type != rhs.user_consent_type) {
        return false;
    }
    return true;
}

static inline bool operator!=(const ::vendor::qti::hardware::radio::lpa::V1_3::UimLpaAddProfileProgressIndV1_1& lhs, const ::vendor::qti::hardware::radio::lpa::V1_3::UimLpaAddProfileProgressIndV1_1& rhs){
    return !(lhs == rhs);
}


}  // namespace V1_3
}  // namespace lpa
}  // namespace radio
}  // namespace hardware
}  // namespace qti
}  // namespace vendor

//
// global type declarations for package
//

namespace android {
namespace hardware {
namespace details {
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wc++17-extensions"
template<> inline constexpr std::array<::vendor::qti::hardware::radio::lpa::V1_3::UimLpaUserConsentType, 3> hidl_enum_values<::vendor::qti::hardware::radio::lpa::V1_3::UimLpaUserConsentType> = {
    ::vendor::qti::hardware::radio::lpa::V1_3::UimLpaUserConsentType::UIM_LPA_NO_CONFIRMATION_REQD,
    ::vendor::qti::hardware::radio::lpa::V1_3::UimLpaUserConsentType::UIM_LPA_SIMPLE_CONFIRMATION_REQD,
    ::vendor::qti::hardware::radio::lpa::V1_3::UimLpaUserConsentType::UIM_LPA_STRONG_CONFIRMATION_REQD,
};
#pragma clang diagnostic pop
}  // namespace details
}  // namespace hardware
}  // namespace android


#endif  // HIDL_GENERATED_VENDOR_QTI_HARDWARE_RADIO_LPA_V1_3_TYPES_H
