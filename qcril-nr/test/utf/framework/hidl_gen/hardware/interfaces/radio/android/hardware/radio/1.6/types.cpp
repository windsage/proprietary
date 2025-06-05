#define LOG_TAG "android.hardware.radio@1.6::types"

#include <android/hardware/radio/1.6/types.h>
#include <log/log.h>
#include <cutils/trace.h>
#ifndef QMI_RIL_UTF
#include <hidl/HidlTransportSupport.h>

#include <hidl/Static.h>
#include <hwbinder/ProcessState.h>
#include <utils/Trace.h>
#include <android/hardware/radio/1.6/types.h>
#include <android/hardware/radio/1.6/hwtypes.h>
#endif

namespace android {
namespace hardware {
namespace radio {
namespace V1_6 {

::android::hardware::radio::V1_6::Qos::Qos() {
    static_assert(offsetof(::android::hardware::radio::V1_6::Qos, hidl_d) == 0, "wrong offset");
    static_assert(offsetof(::android::hardware::radio::V1_6::Qos, hidl_u) == 4, "wrong offset");

    ::std::memset(&hidl_u, 0, sizeof(hidl_u));
    ::std::memset(reinterpret_cast<uint8_t*>(this) + 1, 0, 3);
    // no padding to zero starting at offset 28

    hidl_d = hidl_discriminator::noinit;
    new (&hidl_u.noinit) ::android::hidl::safe_union::V1_0::Monostate();
}

::android::hardware::radio::V1_6::Qos::~Qos() {
    hidl_destructUnion();
}

::android::hardware::radio::V1_6::Qos::Qos(Qos&& other) : ::android::hardware::radio::V1_6::Qos() {
    *this = std::move(other);
}

::android::hardware::radio::V1_6::Qos::Qos(const Qos& other) : ::android::hardware::radio::V1_6::Qos() {
    *this = other;
}

::android::hardware::radio::V1_6::Qos& (::android::hardware::radio::V1_6::Qos::operator=)(Qos&& other) {
    if (this == &other) { return *this; }

    switch (other.hidl_d) {
        case hidl_discriminator::noinit: {
            noinit(std::move(other.hidl_u.noinit));
            break;
        }
        case hidl_discriminator::eps: {
            eps(std::move(other.hidl_u.eps));
            break;
        }
        case hidl_discriminator::nr: {
            nr(std::move(other.hidl_u.nr));
            break;
        }
        default: {
            ::android::hardware::details::logAlwaysFatal((
                    "Unknown union discriminator (value: " +
                    std::to_string((uint8_t) other.hidl_d) + ").").c_str());
        }
    }
    return *this;
}

::android::hardware::radio::V1_6::Qos& (::android::hardware::radio::V1_6::Qos::operator=)(const Qos& other) {
    if (this == &other) { return *this; }

    switch (other.hidl_d) {
        case hidl_discriminator::noinit: {
            noinit(other.hidl_u.noinit);
            break;
        }
        case hidl_discriminator::eps: {
            eps(other.hidl_u.eps);
            break;
        }
        case hidl_discriminator::nr: {
            nr(other.hidl_u.nr);
            break;
        }
        default: {
            ::android::hardware::details::logAlwaysFatal((
                    "Unknown union discriminator (value: " +
                    std::to_string((uint8_t) other.hidl_d) + ").").c_str());
        }
    }
    return *this;
}

void ::android::hardware::radio::V1_6::Qos::hidl_destructUnion() {
    switch (hidl_d) {
        case hidl_discriminator::noinit: {
            ::android::hardware::details::destructElement(&(hidl_u.noinit));
            break;
        }
        case hidl_discriminator::eps: {
            ::android::hardware::details::destructElement(&(hidl_u.eps));
            break;
        }
        case hidl_discriminator::nr: {
            ::android::hardware::details::destructElement(&(hidl_u.nr));
            break;
        }
        default: {
            ::android::hardware::details::logAlwaysFatal((
                    "Unknown union discriminator (value: " +
                    std::to_string((uint8_t) hidl_d) + ").").c_str());
        }
    }

}

void ::android::hardware::radio::V1_6::Qos::noinit(const ::android::hidl::safe_union::V1_0::Monostate& o) {
    if (hidl_d != hidl_discriminator::noinit) {
        hidl_destructUnion();
        ::std::memset(&hidl_u, 0, sizeof(hidl_u));

        new (&hidl_u.noinit) ::android::hidl::safe_union::V1_0::Monostate(o);
        hidl_d = hidl_discriminator::noinit;
    }
    else if (&(hidl_u.noinit) != &o) {
        hidl_u.noinit = o;
    }
}

void ::android::hardware::radio::V1_6::Qos::noinit(::android::hidl::safe_union::V1_0::Monostate&& o) {
    if (hidl_d != hidl_discriminator::noinit) {
        hidl_destructUnion();
        ::std::memset(&hidl_u, 0, sizeof(hidl_u));

        new (&hidl_u.noinit) ::android::hidl::safe_union::V1_0::Monostate(std::move(o));
        hidl_d = hidl_discriminator::noinit;
    }
    else if (&(hidl_u.noinit) != &o) {
        hidl_u.noinit = std::move(o);
    }
}

::android::hidl::safe_union::V1_0::Monostate& (::android::hardware::radio::V1_6::Qos::noinit)() {
    if (CC_UNLIKELY(hidl_d != hidl_discriminator::noinit)) {
        LOG_ALWAYS_FATAL("Bad safe_union access: safe_union has discriminator %" PRIu64 " but discriminator %" PRIu64 " was accessed.",
                static_cast<uint64_t>(hidl_d), static_cast<uint64_t>(hidl_discriminator::noinit));
    }

    return hidl_u.noinit;
}

const ::android::hidl::safe_union::V1_0::Monostate& (::android::hardware::radio::V1_6::Qos::noinit)() const {
    if (CC_UNLIKELY(hidl_d != hidl_discriminator::noinit)) {
        LOG_ALWAYS_FATAL("Bad safe_union access: safe_union has discriminator %" PRIu64 " but discriminator %" PRIu64 " was accessed.",
                static_cast<uint64_t>(hidl_d), static_cast<uint64_t>(hidl_discriminator::noinit));
    }

    return hidl_u.noinit;
}

void ::android::hardware::radio::V1_6::Qos::eps(const ::android::hardware::radio::V1_6::EpsQos& o) {
    if (hidl_d != hidl_discriminator::eps) {
        hidl_destructUnion();
        ::std::memset(&hidl_u, 0, sizeof(hidl_u));

        new (&hidl_u.eps) ::android::hardware::radio::V1_6::EpsQos(o);
        hidl_d = hidl_discriminator::eps;
    }
    else if (&(hidl_u.eps) != &o) {
        hidl_u.eps = o;
    }
}

void ::android::hardware::radio::V1_6::Qos::eps(::android::hardware::radio::V1_6::EpsQos&& o) {
    if (hidl_d != hidl_discriminator::eps) {
        hidl_destructUnion();
        ::std::memset(&hidl_u, 0, sizeof(hidl_u));

        new (&hidl_u.eps) ::android::hardware::radio::V1_6::EpsQos(std::move(o));
        hidl_d = hidl_discriminator::eps;
    }
    else if (&(hidl_u.eps) != &o) {
        hidl_u.eps = std::move(o);
    }
}

::android::hardware::radio::V1_6::EpsQos& (::android::hardware::radio::V1_6::Qos::eps)() {
    if (CC_UNLIKELY(hidl_d != hidl_discriminator::eps)) {
        LOG_ALWAYS_FATAL("Bad safe_union access: safe_union has discriminator %" PRIu64 " but discriminator %" PRIu64 " was accessed.",
                static_cast<uint64_t>(hidl_d), static_cast<uint64_t>(hidl_discriminator::eps));
    }

    return hidl_u.eps;
}

const ::android::hardware::radio::V1_6::EpsQos& (::android::hardware::radio::V1_6::Qos::eps)() const {
    if (CC_UNLIKELY(hidl_d != hidl_discriminator::eps)) {
        LOG_ALWAYS_FATAL("Bad safe_union access: safe_union has discriminator %" PRIu64 " but discriminator %" PRIu64 " was accessed.",
                static_cast<uint64_t>(hidl_d), static_cast<uint64_t>(hidl_discriminator::eps));
    }

    return hidl_u.eps;
}

void ::android::hardware::radio::V1_6::Qos::nr(const ::android::hardware::radio::V1_6::NrQos& o) {
    if (hidl_d != hidl_discriminator::nr) {
        hidl_destructUnion();
        ::std::memset(&hidl_u, 0, sizeof(hidl_u));

        new (&hidl_u.nr) ::android::hardware::radio::V1_6::NrQos(o);
        hidl_d = hidl_discriminator::nr;
    }
    else if (&(hidl_u.nr) != &o) {
        hidl_u.nr = o;
    }
}

void ::android::hardware::radio::V1_6::Qos::nr(::android::hardware::radio::V1_6::NrQos&& o) {
    if (hidl_d != hidl_discriminator::nr) {
        hidl_destructUnion();
        ::std::memset(&hidl_u, 0, sizeof(hidl_u));

        new (&hidl_u.nr) ::android::hardware::radio::V1_6::NrQos(std::move(o));
        hidl_d = hidl_discriminator::nr;
    }
    else if (&(hidl_u.nr) != &o) {
        hidl_u.nr = std::move(o);
    }
}

::android::hardware::radio::V1_6::NrQos& (::android::hardware::radio::V1_6::Qos::nr)() {
    if (CC_UNLIKELY(hidl_d != hidl_discriminator::nr)) {
        LOG_ALWAYS_FATAL("Bad safe_union access: safe_union has discriminator %" PRIu64 " but discriminator %" PRIu64 " was accessed.",
                static_cast<uint64_t>(hidl_d), static_cast<uint64_t>(hidl_discriminator::nr));
    }

    return hidl_u.nr;
}

const ::android::hardware::radio::V1_6::NrQos& (::android::hardware::radio::V1_6::Qos::nr)() const {
    if (CC_UNLIKELY(hidl_d != hidl_discriminator::nr)) {
        LOG_ALWAYS_FATAL("Bad safe_union access: safe_union has discriminator %" PRIu64 " but discriminator %" PRIu64 " was accessed.",
                static_cast<uint64_t>(hidl_d), static_cast<uint64_t>(hidl_discriminator::nr));
    }

    return hidl_u.nr;
}

::android::hardware::radio::V1_6::Qos::hidl_union::hidl_union() {}

::android::hardware::radio::V1_6::Qos::hidl_union::~hidl_union() {}

::android::hardware::radio::V1_6::Qos::hidl_discriminator (::android::hardware::radio::V1_6::Qos::getDiscriminator)() const {
    return hidl_d;
}

::android::hardware::radio::V1_6::MaybePort::MaybePort() {
    static_assert(offsetof(::android::hardware::radio::V1_6::MaybePort, hidl_d) == 0, "wrong offset");
    static_assert(offsetof(::android::hardware::radio::V1_6::MaybePort, hidl_u) == 4, "wrong offset");

    ::std::memset(&hidl_u, 0, sizeof(hidl_u));
    ::std::memset(reinterpret_cast<uint8_t*>(this) + 1, 0, 3);
    // no padding to zero starting at offset 12

    hidl_d = hidl_discriminator::noinit;
    new (&hidl_u.noinit) ::android::hidl::safe_union::V1_0::Monostate();
}

::android::hardware::radio::V1_6::MaybePort::~MaybePort() {
    hidl_destructUnion();
}

::android::hardware::radio::V1_6::MaybePort::MaybePort(MaybePort&& other) : ::android::hardware::radio::V1_6::MaybePort() {
    *this = std::move(other);
}

::android::hardware::radio::V1_6::MaybePort::MaybePort(const MaybePort& other) : ::android::hardware::radio::V1_6::MaybePort() {
    *this = other;
}

::android::hardware::radio::V1_6::MaybePort& (::android::hardware::radio::V1_6::MaybePort::operator=)(MaybePort&& other) {
    if (this == &other) { return *this; }

    switch (other.hidl_d) {
        case hidl_discriminator::noinit: {
            noinit(std::move(other.hidl_u.noinit));
            break;
        }
        case hidl_discriminator::range: {
            range(std::move(other.hidl_u.range));
            break;
        }
        default: {
            ::android::hardware::details::logAlwaysFatal((
                    "Unknown union discriminator (value: " +
                    std::to_string((uint8_t) other.hidl_d) + ").").c_str());
        }
    }
    return *this;
}

::android::hardware::radio::V1_6::MaybePort& (::android::hardware::radio::V1_6::MaybePort::operator=)(const MaybePort& other) {
    if (this == &other) { return *this; }

    switch (other.hidl_d) {
        case hidl_discriminator::noinit: {
            noinit(other.hidl_u.noinit);
            break;
        }
        case hidl_discriminator::range: {
            range(other.hidl_u.range);
            break;
        }
        default: {
            ::android::hardware::details::logAlwaysFatal((
                    "Unknown union discriminator (value: " +
                    std::to_string((uint8_t) other.hidl_d) + ").").c_str());
        }
    }
    return *this;
}

void ::android::hardware::radio::V1_6::MaybePort::hidl_destructUnion() {
    switch (hidl_d) {
        case hidl_discriminator::noinit: {
            ::android::hardware::details::destructElement(&(hidl_u.noinit));
            break;
        }
        case hidl_discriminator::range: {
            ::android::hardware::details::destructElement(&(hidl_u.range));
            break;
        }
        default: {
            ::android::hardware::details::logAlwaysFatal((
                    "Unknown union discriminator (value: " +
                    std::to_string((uint8_t) hidl_d) + ").").c_str());
        }
    }

}

void ::android::hardware::radio::V1_6::MaybePort::noinit(const ::android::hidl::safe_union::V1_0::Monostate& o) {
    if (hidl_d != hidl_discriminator::noinit) {
        hidl_destructUnion();
        ::std::memset(&hidl_u, 0, sizeof(hidl_u));

        new (&hidl_u.noinit) ::android::hidl::safe_union::V1_0::Monostate(o);
        hidl_d = hidl_discriminator::noinit;
    }
    else if (&(hidl_u.noinit) != &o) {
        hidl_u.noinit = o;
    }
}

void ::android::hardware::radio::V1_6::MaybePort::noinit(::android::hidl::safe_union::V1_0::Monostate&& o) {
    if (hidl_d != hidl_discriminator::noinit) {
        hidl_destructUnion();
        ::std::memset(&hidl_u, 0, sizeof(hidl_u));

        new (&hidl_u.noinit) ::android::hidl::safe_union::V1_0::Monostate(std::move(o));
        hidl_d = hidl_discriminator::noinit;
    }
    else if (&(hidl_u.noinit) != &o) {
        hidl_u.noinit = std::move(o);
    }
}

::android::hidl::safe_union::V1_0::Monostate& (::android::hardware::radio::V1_6::MaybePort::noinit)() {
    if (CC_UNLIKELY(hidl_d != hidl_discriminator::noinit)) {
        LOG_ALWAYS_FATAL("Bad safe_union access: safe_union has discriminator %" PRIu64 " but discriminator %" PRIu64 " was accessed.",
                static_cast<uint64_t>(hidl_d), static_cast<uint64_t>(hidl_discriminator::noinit));
    }

    return hidl_u.noinit;
}

const ::android::hidl::safe_union::V1_0::Monostate& (::android::hardware::radio::V1_6::MaybePort::noinit)() const {
    if (CC_UNLIKELY(hidl_d != hidl_discriminator::noinit)) {
        LOG_ALWAYS_FATAL("Bad safe_union access: safe_union has discriminator %" PRIu64 " but discriminator %" PRIu64 " was accessed.",
                static_cast<uint64_t>(hidl_d), static_cast<uint64_t>(hidl_discriminator::noinit));
    }

    return hidl_u.noinit;
}

void ::android::hardware::radio::V1_6::MaybePort::range(const ::android::hardware::radio::V1_6::PortRange& o) {
    if (hidl_d != hidl_discriminator::range) {
        hidl_destructUnion();
        ::std::memset(&hidl_u, 0, sizeof(hidl_u));

        new (&hidl_u.range) ::android::hardware::radio::V1_6::PortRange(o);
        hidl_d = hidl_discriminator::range;
    }
    else if (&(hidl_u.range) != &o) {
        hidl_u.range = o;
    }
}

void ::android::hardware::radio::V1_6::MaybePort::range(::android::hardware::radio::V1_6::PortRange&& o) {
    if (hidl_d != hidl_discriminator::range) {
        hidl_destructUnion();
        ::std::memset(&hidl_u, 0, sizeof(hidl_u));

        new (&hidl_u.range) ::android::hardware::radio::V1_6::PortRange(std::move(o));
        hidl_d = hidl_discriminator::range;
    }
    else if (&(hidl_u.range) != &o) {
        hidl_u.range = std::move(o);
    }
}

::android::hardware::radio::V1_6::PortRange& (::android::hardware::radio::V1_6::MaybePort::range)() {
    if (CC_UNLIKELY(hidl_d != hidl_discriminator::range)) {
        LOG_ALWAYS_FATAL("Bad safe_union access: safe_union has discriminator %" PRIu64 " but discriminator %" PRIu64 " was accessed.",
                static_cast<uint64_t>(hidl_d), static_cast<uint64_t>(hidl_discriminator::range));
    }

    return hidl_u.range;
}

const ::android::hardware::radio::V1_6::PortRange& (::android::hardware::radio::V1_6::MaybePort::range)() const {
    if (CC_UNLIKELY(hidl_d != hidl_discriminator::range)) {
        LOG_ALWAYS_FATAL("Bad safe_union access: safe_union has discriminator %" PRIu64 " but discriminator %" PRIu64 " was accessed.",
                static_cast<uint64_t>(hidl_d), static_cast<uint64_t>(hidl_discriminator::range));
    }

    return hidl_u.range;
}

::android::hardware::radio::V1_6::MaybePort::hidl_union::hidl_union() {}

::android::hardware::radio::V1_6::MaybePort::hidl_union::~hidl_union() {}

::android::hardware::radio::V1_6::MaybePort::hidl_discriminator (::android::hardware::radio::V1_6::MaybePort::getDiscriminator)() const {
    return hidl_d;
}

::android::hardware::radio::V1_6::QosFilter::TypeOfService::TypeOfService() {
    static_assert(offsetof(::android::hardware::radio::V1_6::QosFilter::TypeOfService, hidl_d) == 0, "wrong offset");
    static_assert(offsetof(::android::hardware::radio::V1_6::QosFilter::TypeOfService, hidl_u) == 1, "wrong offset");

    ::std::memset(&hidl_u, 0, sizeof(hidl_u));
    // no padding to zero starting at offset 1
    // no padding to zero starting at offset 2

    hidl_d = hidl_discriminator::noinit;
    new (&hidl_u.noinit) ::android::hidl::safe_union::V1_0::Monostate();
}

::android::hardware::radio::V1_6::QosFilter::TypeOfService::~TypeOfService() {
    hidl_destructUnion();
}

::android::hardware::radio::V1_6::QosFilter::TypeOfService::TypeOfService(TypeOfService&& other) : ::android::hardware::radio::V1_6::QosFilter::TypeOfService() {
    *this = std::move(other);
}

::android::hardware::radio::V1_6::QosFilter::TypeOfService::TypeOfService(const TypeOfService& other) : ::android::hardware::radio::V1_6::QosFilter::TypeOfService() {
    *this = other;
}

::android::hardware::radio::V1_6::QosFilter::TypeOfService& (::android::hardware::radio::V1_6::QosFilter::TypeOfService::operator=)(TypeOfService&& other) {
    if (this == &other) { return *this; }

    switch (other.hidl_d) {
        case hidl_discriminator::noinit: {
            noinit(std::move(other.hidl_u.noinit));
            break;
        }
        case hidl_discriminator::value: {
            value(std::move(other.hidl_u.value));
            break;
        }
        default: {
            ::android::hardware::details::logAlwaysFatal((
                    "Unknown union discriminator (value: " +
                    std::to_string((uint8_t) other.hidl_d) + ").").c_str());
        }
    }
    return *this;
}

::android::hardware::radio::V1_6::QosFilter::TypeOfService& (::android::hardware::radio::V1_6::QosFilter::TypeOfService::operator=)(const TypeOfService& other) {
    if (this == &other) { return *this; }

    switch (other.hidl_d) {
        case hidl_discriminator::noinit: {
            noinit(other.hidl_u.noinit);
            break;
        }
        case hidl_discriminator::value: {
            value(other.hidl_u.value);
            break;
        }
        default: {
            ::android::hardware::details::logAlwaysFatal((
                    "Unknown union discriminator (value: " +
                    std::to_string((uint8_t) other.hidl_d) + ").").c_str());
        }
    }
    return *this;
}

void ::android::hardware::radio::V1_6::QosFilter::TypeOfService::hidl_destructUnion() {
    switch (hidl_d) {
        case hidl_discriminator::noinit: {
            ::android::hardware::details::destructElement(&(hidl_u.noinit));
            break;
        }
        case hidl_discriminator::value: {
            ::android::hardware::details::destructElement(&(hidl_u.value));
            break;
        }
        default: {
            ::android::hardware::details::logAlwaysFatal((
                    "Unknown union discriminator (value: " +
                    std::to_string((uint8_t) hidl_d) + ").").c_str());
        }
    }

}

void ::android::hardware::radio::V1_6::QosFilter::TypeOfService::noinit(const ::android::hidl::safe_union::V1_0::Monostate& o) {
    if (hidl_d != hidl_discriminator::noinit) {
        hidl_destructUnion();
        ::std::memset(&hidl_u, 0, sizeof(hidl_u));

        new (&hidl_u.noinit) ::android::hidl::safe_union::V1_0::Monostate(o);
        hidl_d = hidl_discriminator::noinit;
    }
    else if (&(hidl_u.noinit) != &o) {
        hidl_u.noinit = o;
    }
}

void ::android::hardware::radio::V1_6::QosFilter::TypeOfService::noinit(::android::hidl::safe_union::V1_0::Monostate&& o) {
    if (hidl_d != hidl_discriminator::noinit) {
        hidl_destructUnion();
        ::std::memset(&hidl_u, 0, sizeof(hidl_u));

        new (&hidl_u.noinit) ::android::hidl::safe_union::V1_0::Monostate(std::move(o));
        hidl_d = hidl_discriminator::noinit;
    }
    else if (&(hidl_u.noinit) != &o) {
        hidl_u.noinit = std::move(o);
    }
}

::android::hidl::safe_union::V1_0::Monostate& (::android::hardware::radio::V1_6::QosFilter::TypeOfService::noinit)() {
    if (CC_UNLIKELY(hidl_d != hidl_discriminator::noinit)) {
        LOG_ALWAYS_FATAL("Bad safe_union access: safe_union has discriminator %" PRIu64 " but discriminator %" PRIu64 " was accessed.",
                static_cast<uint64_t>(hidl_d), static_cast<uint64_t>(hidl_discriminator::noinit));
    }

    return hidl_u.noinit;
}

const ::android::hidl::safe_union::V1_0::Monostate& (::android::hardware::radio::V1_6::QosFilter::TypeOfService::noinit)() const {
    if (CC_UNLIKELY(hidl_d != hidl_discriminator::noinit)) {
        LOG_ALWAYS_FATAL("Bad safe_union access: safe_union has discriminator %" PRIu64 " but discriminator %" PRIu64 " was accessed.",
                static_cast<uint64_t>(hidl_d), static_cast<uint64_t>(hidl_discriminator::noinit));
    }

    return hidl_u.noinit;
}

void ::android::hardware::radio::V1_6::QosFilter::TypeOfService::value(uint8_t o) {
    if (hidl_d != hidl_discriminator::value) {
        hidl_destructUnion();
        ::std::memset(&hidl_u, 0, sizeof(hidl_u));

        new (&hidl_u.value) uint8_t(o);
        hidl_d = hidl_discriminator::value;
    }
    else if (&(hidl_u.value) != &o) {
        hidl_u.value = o;
    }
}

uint8_t& (::android::hardware::radio::V1_6::QosFilter::TypeOfService::value)() {
    if (CC_UNLIKELY(hidl_d != hidl_discriminator::value)) {
        LOG_ALWAYS_FATAL("Bad safe_union access: safe_union has discriminator %" PRIu64 " but discriminator %" PRIu64 " was accessed.",
                static_cast<uint64_t>(hidl_d), static_cast<uint64_t>(hidl_discriminator::value));
    }

    return hidl_u.value;
}

uint8_t (::android::hardware::radio::V1_6::QosFilter::TypeOfService::value)() const {
    if (CC_UNLIKELY(hidl_d != hidl_discriminator::value)) {
        LOG_ALWAYS_FATAL("Bad safe_union access: safe_union has discriminator %" PRIu64 " but discriminator %" PRIu64 " was accessed.",
                static_cast<uint64_t>(hidl_d), static_cast<uint64_t>(hidl_discriminator::value));
    }

    return hidl_u.value;
}

::android::hardware::radio::V1_6::QosFilter::TypeOfService::hidl_union::hidl_union() {}

::android::hardware::radio::V1_6::QosFilter::TypeOfService::hidl_union::~hidl_union() {}

::android::hardware::radio::V1_6::QosFilter::TypeOfService::hidl_discriminator (::android::hardware::radio::V1_6::QosFilter::TypeOfService::getDiscriminator)() const {
    return hidl_d;
}

::android::hardware::radio::V1_6::QosFilter::Ipv6FlowLabel::Ipv6FlowLabel() {
    static_assert(offsetof(::android::hardware::radio::V1_6::QosFilter::Ipv6FlowLabel, hidl_d) == 0, "wrong offset");
    static_assert(offsetof(::android::hardware::radio::V1_6::QosFilter::Ipv6FlowLabel, hidl_u) == 4, "wrong offset");

    ::std::memset(&hidl_u, 0, sizeof(hidl_u));
    ::std::memset(reinterpret_cast<uint8_t*>(this) + 1, 0, 3);
    // no padding to zero starting at offset 8

    hidl_d = hidl_discriminator::noinit;
    new (&hidl_u.noinit) ::android::hidl::safe_union::V1_0::Monostate();
}

::android::hardware::radio::V1_6::QosFilter::Ipv6FlowLabel::~Ipv6FlowLabel() {
    hidl_destructUnion();
}

::android::hardware::radio::V1_6::QosFilter::Ipv6FlowLabel::Ipv6FlowLabel(Ipv6FlowLabel&& other) : ::android::hardware::radio::V1_6::QosFilter::Ipv6FlowLabel() {
    *this = std::move(other);
}

::android::hardware::radio::V1_6::QosFilter::Ipv6FlowLabel::Ipv6FlowLabel(const Ipv6FlowLabel& other) : ::android::hardware::radio::V1_6::QosFilter::Ipv6FlowLabel() {
    *this = other;
}

::android::hardware::radio::V1_6::QosFilter::Ipv6FlowLabel& (::android::hardware::radio::V1_6::QosFilter::Ipv6FlowLabel::operator=)(Ipv6FlowLabel&& other) {
    if (this == &other) { return *this; }

    switch (other.hidl_d) {
        case hidl_discriminator::noinit: {
            noinit(std::move(other.hidl_u.noinit));
            break;
        }
        case hidl_discriminator::value: {
            value(std::move(other.hidl_u.value));
            break;
        }
        default: {
            ::android::hardware::details::logAlwaysFatal((
                    "Unknown union discriminator (value: " +
                    std::to_string((uint8_t) other.hidl_d) + ").").c_str());
        }
    }
    return *this;
}

::android::hardware::radio::V1_6::QosFilter::Ipv6FlowLabel& (::android::hardware::radio::V1_6::QosFilter::Ipv6FlowLabel::operator=)(const Ipv6FlowLabel& other) {
    if (this == &other) { return *this; }

    switch (other.hidl_d) {
        case hidl_discriminator::noinit: {
            noinit(other.hidl_u.noinit);
            break;
        }
        case hidl_discriminator::value: {
            value(other.hidl_u.value);
            break;
        }
        default: {
            ::android::hardware::details::logAlwaysFatal((
                    "Unknown union discriminator (value: " +
                    std::to_string((uint8_t) other.hidl_d) + ").").c_str());
        }
    }
    return *this;
}

void ::android::hardware::radio::V1_6::QosFilter::Ipv6FlowLabel::hidl_destructUnion() {
    switch (hidl_d) {
        case hidl_discriminator::noinit: {
            ::android::hardware::details::destructElement(&(hidl_u.noinit));
            break;
        }
        case hidl_discriminator::value: {
            ::android::hardware::details::destructElement(&(hidl_u.value));
            break;
        }
        default: {
            ::android::hardware::details::logAlwaysFatal((
                    "Unknown union discriminator (value: " +
                    std::to_string((uint8_t) hidl_d) + ").").c_str());
        }
    }

}

void ::android::hardware::radio::V1_6::QosFilter::Ipv6FlowLabel::noinit(const ::android::hidl::safe_union::V1_0::Monostate& o) {
    if (hidl_d != hidl_discriminator::noinit) {
        hidl_destructUnion();
        ::std::memset(&hidl_u, 0, sizeof(hidl_u));

        new (&hidl_u.noinit) ::android::hidl::safe_union::V1_0::Monostate(o);
        hidl_d = hidl_discriminator::noinit;
    }
    else if (&(hidl_u.noinit) != &o) {
        hidl_u.noinit = o;
    }
}

void ::android::hardware::radio::V1_6::QosFilter::Ipv6FlowLabel::noinit(::android::hidl::safe_union::V1_0::Monostate&& o) {
    if (hidl_d != hidl_discriminator::noinit) {
        hidl_destructUnion();
        ::std::memset(&hidl_u, 0, sizeof(hidl_u));

        new (&hidl_u.noinit) ::android::hidl::safe_union::V1_0::Monostate(std::move(o));
        hidl_d = hidl_discriminator::noinit;
    }
    else if (&(hidl_u.noinit) != &o) {
        hidl_u.noinit = std::move(o);
    }
}

::android::hidl::safe_union::V1_0::Monostate& (::android::hardware::radio::V1_6::QosFilter::Ipv6FlowLabel::noinit)() {
    if (CC_UNLIKELY(hidl_d != hidl_discriminator::noinit)) {
        LOG_ALWAYS_FATAL("Bad safe_union access: safe_union has discriminator %" PRIu64 " but discriminator %" PRIu64 " was accessed.",
                static_cast<uint64_t>(hidl_d), static_cast<uint64_t>(hidl_discriminator::noinit));
    }

    return hidl_u.noinit;
}

const ::android::hidl::safe_union::V1_0::Monostate& (::android::hardware::radio::V1_6::QosFilter::Ipv6FlowLabel::noinit)() const {
    if (CC_UNLIKELY(hidl_d != hidl_discriminator::noinit)) {
        LOG_ALWAYS_FATAL("Bad safe_union access: safe_union has discriminator %" PRIu64 " but discriminator %" PRIu64 " was accessed.",
                static_cast<uint64_t>(hidl_d), static_cast<uint64_t>(hidl_discriminator::noinit));
    }

    return hidl_u.noinit;
}

void ::android::hardware::radio::V1_6::QosFilter::Ipv6FlowLabel::value(uint32_t o) {
    if (hidl_d != hidl_discriminator::value) {
        hidl_destructUnion();
        ::std::memset(&hidl_u, 0, sizeof(hidl_u));

        new (&hidl_u.value) uint32_t(o);
        hidl_d = hidl_discriminator::value;
    }
    else if (&(hidl_u.value) != &o) {
        hidl_u.value = o;
    }
}

uint32_t& (::android::hardware::radio::V1_6::QosFilter::Ipv6FlowLabel::value)() {
    if (CC_UNLIKELY(hidl_d != hidl_discriminator::value)) {
        LOG_ALWAYS_FATAL("Bad safe_union access: safe_union has discriminator %" PRIu64 " but discriminator %" PRIu64 " was accessed.",
                static_cast<uint64_t>(hidl_d), static_cast<uint64_t>(hidl_discriminator::value));
    }

    return hidl_u.value;
}

uint32_t (::android::hardware::radio::V1_6::QosFilter::Ipv6FlowLabel::value)() const {
    if (CC_UNLIKELY(hidl_d != hidl_discriminator::value)) {
        LOG_ALWAYS_FATAL("Bad safe_union access: safe_union has discriminator %" PRIu64 " but discriminator %" PRIu64 " was accessed.",
                static_cast<uint64_t>(hidl_d), static_cast<uint64_t>(hidl_discriminator::value));
    }

    return hidl_u.value;
}

::android::hardware::radio::V1_6::QosFilter::Ipv6FlowLabel::hidl_union::hidl_union() {}

::android::hardware::radio::V1_6::QosFilter::Ipv6FlowLabel::hidl_union::~hidl_union() {}

::android::hardware::radio::V1_6::QosFilter::Ipv6FlowLabel::hidl_discriminator (::android::hardware::radio::V1_6::QosFilter::Ipv6FlowLabel::getDiscriminator)() const {
    return hidl_d;
}

::android::hardware::radio::V1_6::QosFilter::IpsecSpi::IpsecSpi() {
    static_assert(offsetof(::android::hardware::radio::V1_6::QosFilter::IpsecSpi, hidl_d) == 0, "wrong offset");
    static_assert(offsetof(::android::hardware::radio::V1_6::QosFilter::IpsecSpi, hidl_u) == 4, "wrong offset");

    ::std::memset(&hidl_u, 0, sizeof(hidl_u));
    ::std::memset(reinterpret_cast<uint8_t*>(this) + 1, 0, 3);
    // no padding to zero starting at offset 8

    hidl_d = hidl_discriminator::noinit;
    new (&hidl_u.noinit) ::android::hidl::safe_union::V1_0::Monostate();
}

::android::hardware::radio::V1_6::QosFilter::IpsecSpi::~IpsecSpi() {
    hidl_destructUnion();
}

::android::hardware::radio::V1_6::QosFilter::IpsecSpi::IpsecSpi(IpsecSpi&& other) : ::android::hardware::radio::V1_6::QosFilter::IpsecSpi() {
    *this = std::move(other);
}

::android::hardware::radio::V1_6::QosFilter::IpsecSpi::IpsecSpi(const IpsecSpi& other) : ::android::hardware::radio::V1_6::QosFilter::IpsecSpi() {
    *this = other;
}

::android::hardware::radio::V1_6::QosFilter::IpsecSpi& (::android::hardware::radio::V1_6::QosFilter::IpsecSpi::operator=)(IpsecSpi&& other) {
    if (this == &other) { return *this; }

    switch (other.hidl_d) {
        case hidl_discriminator::noinit: {
            noinit(std::move(other.hidl_u.noinit));
            break;
        }
        case hidl_discriminator::value: {
            value(std::move(other.hidl_u.value));
            break;
        }
        default: {
            ::android::hardware::details::logAlwaysFatal((
                    "Unknown union discriminator (value: " +
                    std::to_string((uint8_t) other.hidl_d) + ").").c_str());
        }
    }
    return *this;
}

::android::hardware::radio::V1_6::QosFilter::IpsecSpi& (::android::hardware::radio::V1_6::QosFilter::IpsecSpi::operator=)(const IpsecSpi& other) {
    if (this == &other) { return *this; }

    switch (other.hidl_d) {
        case hidl_discriminator::noinit: {
            noinit(other.hidl_u.noinit);
            break;
        }
        case hidl_discriminator::value: {
            value(other.hidl_u.value);
            break;
        }
        default: {
            ::android::hardware::details::logAlwaysFatal((
                    "Unknown union discriminator (value: " +
                    std::to_string((uint8_t) other.hidl_d) + ").").c_str());
        }
    }
    return *this;
}

void ::android::hardware::radio::V1_6::QosFilter::IpsecSpi::hidl_destructUnion() {
    switch (hidl_d) {
        case hidl_discriminator::noinit: {
            ::android::hardware::details::destructElement(&(hidl_u.noinit));
            break;
        }
        case hidl_discriminator::value: {
            ::android::hardware::details::destructElement(&(hidl_u.value));
            break;
        }
        default: {
            ::android::hardware::details::logAlwaysFatal((
                    "Unknown union discriminator (value: " +
                    std::to_string((uint8_t) hidl_d) + ").").c_str());
        }
    }

}

void ::android::hardware::radio::V1_6::QosFilter::IpsecSpi::noinit(const ::android::hidl::safe_union::V1_0::Monostate& o) {
    if (hidl_d != hidl_discriminator::noinit) {
        hidl_destructUnion();
        ::std::memset(&hidl_u, 0, sizeof(hidl_u));

        new (&hidl_u.noinit) ::android::hidl::safe_union::V1_0::Monostate(o);
        hidl_d = hidl_discriminator::noinit;
    }
    else if (&(hidl_u.noinit) != &o) {
        hidl_u.noinit = o;
    }
}

void ::android::hardware::radio::V1_6::QosFilter::IpsecSpi::noinit(::android::hidl::safe_union::V1_0::Monostate&& o) {
    if (hidl_d != hidl_discriminator::noinit) {
        hidl_destructUnion();
        ::std::memset(&hidl_u, 0, sizeof(hidl_u));

        new (&hidl_u.noinit) ::android::hidl::safe_union::V1_0::Monostate(std::move(o));
        hidl_d = hidl_discriminator::noinit;
    }
    else if (&(hidl_u.noinit) != &o) {
        hidl_u.noinit = std::move(o);
    }
}

::android::hidl::safe_union::V1_0::Monostate& (::android::hardware::radio::V1_6::QosFilter::IpsecSpi::noinit)() {
    if (CC_UNLIKELY(hidl_d != hidl_discriminator::noinit)) {
        LOG_ALWAYS_FATAL("Bad safe_union access: safe_union has discriminator %" PRIu64 " but discriminator %" PRIu64 " was accessed.",
                static_cast<uint64_t>(hidl_d), static_cast<uint64_t>(hidl_discriminator::noinit));
    }

    return hidl_u.noinit;
}

const ::android::hidl::safe_union::V1_0::Monostate& (::android::hardware::radio::V1_6::QosFilter::IpsecSpi::noinit)() const {
    if (CC_UNLIKELY(hidl_d != hidl_discriminator::noinit)) {
        LOG_ALWAYS_FATAL("Bad safe_union access: safe_union has discriminator %" PRIu64 " but discriminator %" PRIu64 " was accessed.",
                static_cast<uint64_t>(hidl_d), static_cast<uint64_t>(hidl_discriminator::noinit));
    }

    return hidl_u.noinit;
}

void ::android::hardware::radio::V1_6::QosFilter::IpsecSpi::value(uint32_t o) {
    if (hidl_d != hidl_discriminator::value) {
        hidl_destructUnion();
        ::std::memset(&hidl_u, 0, sizeof(hidl_u));

        new (&hidl_u.value) uint32_t(o);
        hidl_d = hidl_discriminator::value;
    }
    else if (&(hidl_u.value) != &o) {
        hidl_u.value = o;
    }
}

uint32_t& (::android::hardware::radio::V1_6::QosFilter::IpsecSpi::value)() {
    if (CC_UNLIKELY(hidl_d != hidl_discriminator::value)) {
        LOG_ALWAYS_FATAL("Bad safe_union access: safe_union has discriminator %" PRIu64 " but discriminator %" PRIu64 " was accessed.",
                static_cast<uint64_t>(hidl_d), static_cast<uint64_t>(hidl_discriminator::value));
    }

    return hidl_u.value;
}

uint32_t (::android::hardware::radio::V1_6::QosFilter::IpsecSpi::value)() const {
    if (CC_UNLIKELY(hidl_d != hidl_discriminator::value)) {
        LOG_ALWAYS_FATAL("Bad safe_union access: safe_union has discriminator %" PRIu64 " but discriminator %" PRIu64 " was accessed.",
                static_cast<uint64_t>(hidl_d), static_cast<uint64_t>(hidl_discriminator::value));
    }

    return hidl_u.value;
}

::android::hardware::radio::V1_6::QosFilter::IpsecSpi::hidl_union::hidl_union() {}

::android::hardware::radio::V1_6::QosFilter::IpsecSpi::hidl_union::~hidl_union() {}

::android::hardware::radio::V1_6::QosFilter::IpsecSpi::hidl_discriminator (::android::hardware::radio::V1_6::QosFilter::IpsecSpi::getDiscriminator)() const {
    return hidl_d;
}

::android::hardware::radio::V1_6::OptionalSliceInfo::OptionalSliceInfo() {
    static_assert(offsetof(::android::hardware::radio::V1_6::OptionalSliceInfo, hidl_d) == 0, "wrong offset");
    static_assert(offsetof(::android::hardware::radio::V1_6::OptionalSliceInfo, hidl_u) == 4, "wrong offset");

    ::std::memset(&hidl_u, 0, sizeof(hidl_u));
    ::std::memset(reinterpret_cast<uint8_t*>(this) + 1, 0, 3);
    // no padding to zero starting at offset 24

    hidl_d = hidl_discriminator::noinit;
    new (&hidl_u.noinit) ::android::hidl::safe_union::V1_0::Monostate();
}

::android::hardware::radio::V1_6::OptionalSliceInfo::~OptionalSliceInfo() {
    hidl_destructUnion();
}

::android::hardware::radio::V1_6::OptionalSliceInfo::OptionalSliceInfo(OptionalSliceInfo&& other) : ::android::hardware::radio::V1_6::OptionalSliceInfo() {
    *this = std::move(other);
}

::android::hardware::radio::V1_6::OptionalSliceInfo::OptionalSliceInfo(const OptionalSliceInfo& other) : ::android::hardware::radio::V1_6::OptionalSliceInfo() {
    *this = other;
}

::android::hardware::radio::V1_6::OptionalSliceInfo& (::android::hardware::radio::V1_6::OptionalSliceInfo::operator=)(OptionalSliceInfo&& other) {
    if (this == &other) { return *this; }

    switch (other.hidl_d) {
        case hidl_discriminator::noinit: {
            noinit(std::move(other.hidl_u.noinit));
            break;
        }
        case hidl_discriminator::value: {
            value(std::move(other.hidl_u.value));
            break;
        }
        default: {
            ::android::hardware::details::logAlwaysFatal((
                    "Unknown union discriminator (value: " +
                    std::to_string((uint8_t) other.hidl_d) + ").").c_str());
        }
    }
    return *this;
}

::android::hardware::radio::V1_6::OptionalSliceInfo& (::android::hardware::radio::V1_6::OptionalSliceInfo::operator=)(const OptionalSliceInfo& other) {
    if (this == &other) { return *this; }

    switch (other.hidl_d) {
        case hidl_discriminator::noinit: {
            noinit(other.hidl_u.noinit);
            break;
        }
        case hidl_discriminator::value: {
            value(other.hidl_u.value);
            break;
        }
        default: {
            ::android::hardware::details::logAlwaysFatal((
                    "Unknown union discriminator (value: " +
                    std::to_string((uint8_t) other.hidl_d) + ").").c_str());
        }
    }
    return *this;
}

void ::android::hardware::radio::V1_6::OptionalSliceInfo::hidl_destructUnion() {
    switch (hidl_d) {
        case hidl_discriminator::noinit: {
            ::android::hardware::details::destructElement(&(hidl_u.noinit));
            break;
        }
        case hidl_discriminator::value: {
            ::android::hardware::details::destructElement(&(hidl_u.value));
            break;
        }
        default: {
            ::android::hardware::details::logAlwaysFatal((
                    "Unknown union discriminator (value: " +
                    std::to_string((uint8_t) hidl_d) + ").").c_str());
        }
    }

}

void ::android::hardware::radio::V1_6::OptionalSliceInfo::noinit(const ::android::hidl::safe_union::V1_0::Monostate& o) {
    if (hidl_d != hidl_discriminator::noinit) {
        hidl_destructUnion();
        ::std::memset(&hidl_u, 0, sizeof(hidl_u));

        new (&hidl_u.noinit) ::android::hidl::safe_union::V1_0::Monostate(o);
        hidl_d = hidl_discriminator::noinit;
    }
    else if (&(hidl_u.noinit) != &o) {
        hidl_u.noinit = o;
    }
}

void ::android::hardware::radio::V1_6::OptionalSliceInfo::noinit(::android::hidl::safe_union::V1_0::Monostate&& o) {
    if (hidl_d != hidl_discriminator::noinit) {
        hidl_destructUnion();
        ::std::memset(&hidl_u, 0, sizeof(hidl_u));

        new (&hidl_u.noinit) ::android::hidl::safe_union::V1_0::Monostate(std::move(o));
        hidl_d = hidl_discriminator::noinit;
    }
    else if (&(hidl_u.noinit) != &o) {
        hidl_u.noinit = std::move(o);
    }
}

::android::hidl::safe_union::V1_0::Monostate& (::android::hardware::radio::V1_6::OptionalSliceInfo::noinit)() {
    if (CC_UNLIKELY(hidl_d != hidl_discriminator::noinit)) {
        LOG_ALWAYS_FATAL("Bad safe_union access: safe_union has discriminator %" PRIu64 " but discriminator %" PRIu64 " was accessed.",
                static_cast<uint64_t>(hidl_d), static_cast<uint64_t>(hidl_discriminator::noinit));
    }

    return hidl_u.noinit;
}

const ::android::hidl::safe_union::V1_0::Monostate& (::android::hardware::radio::V1_6::OptionalSliceInfo::noinit)() const {
    if (CC_UNLIKELY(hidl_d != hidl_discriminator::noinit)) {
        LOG_ALWAYS_FATAL("Bad safe_union access: safe_union has discriminator %" PRIu64 " but discriminator %" PRIu64 " was accessed.",
                static_cast<uint64_t>(hidl_d), static_cast<uint64_t>(hidl_discriminator::noinit));
    }

    return hidl_u.noinit;
}

void ::android::hardware::radio::V1_6::OptionalSliceInfo::value(const ::android::hardware::radio::V1_6::SliceInfo& o) {
    if (hidl_d != hidl_discriminator::value) {
        hidl_destructUnion();
        ::std::memset(&hidl_u, 0, sizeof(hidl_u));

        new (&hidl_u.value) ::android::hardware::radio::V1_6::SliceInfo(o);
        hidl_d = hidl_discriminator::value;
    }
    else if (&(hidl_u.value) != &o) {
        hidl_u.value = o;
    }
}

void ::android::hardware::radio::V1_6::OptionalSliceInfo::value(::android::hardware::radio::V1_6::SliceInfo&& o) {
    if (hidl_d != hidl_discriminator::value) {
        hidl_destructUnion();
        ::std::memset(&hidl_u, 0, sizeof(hidl_u));

        new (&hidl_u.value) ::android::hardware::radio::V1_6::SliceInfo(std::move(o));
        hidl_d = hidl_discriminator::value;
    }
    else if (&(hidl_u.value) != &o) {
        hidl_u.value = std::move(o);
    }
}

::android::hardware::radio::V1_6::SliceInfo& (::android::hardware::radio::V1_6::OptionalSliceInfo::value)() {
    if (CC_UNLIKELY(hidl_d != hidl_discriminator::value)) {
        LOG_ALWAYS_FATAL("Bad safe_union access: safe_union has discriminator %" PRIu64 " but discriminator %" PRIu64 " was accessed.",
                static_cast<uint64_t>(hidl_d), static_cast<uint64_t>(hidl_discriminator::value));
    }

    return hidl_u.value;
}

const ::android::hardware::radio::V1_6::SliceInfo& (::android::hardware::radio::V1_6::OptionalSliceInfo::value)() const {
    if (CC_UNLIKELY(hidl_d != hidl_discriminator::value)) {
        LOG_ALWAYS_FATAL("Bad safe_union access: safe_union has discriminator %" PRIu64 " but discriminator %" PRIu64 " was accessed.",
                static_cast<uint64_t>(hidl_d), static_cast<uint64_t>(hidl_discriminator::value));
    }

    return hidl_u.value;
}

::android::hardware::radio::V1_6::OptionalSliceInfo::hidl_union::hidl_union() {}

::android::hardware::radio::V1_6::OptionalSliceInfo::hidl_union::~hidl_union() {}

::android::hardware::radio::V1_6::OptionalSliceInfo::hidl_discriminator (::android::hardware::radio::V1_6::OptionalSliceInfo::getDiscriminator)() const {
    return hidl_d;
}

::android::hardware::radio::V1_6::CellInfo::CellInfoRatSpecificInfo::CellInfoRatSpecificInfo() {
    static_assert(offsetof(::android::hardware::radio::V1_6::CellInfo::CellInfoRatSpecificInfo, hidl_d) == 0, "wrong offset");
    static_assert(offsetof(::android::hardware::radio::V1_6::CellInfo::CellInfoRatSpecificInfo, hidl_u) == 8, "wrong offset");

    ::std::memset(&hidl_u, 0, sizeof(hidl_u));
    ::std::memset(reinterpret_cast<uint8_t*>(this) + 1, 0, 7);
    // no padding to zero starting at offset 200

    hidl_d = hidl_discriminator::gsm;
    new (&hidl_u.gsm) ::android::hardware::radio::V1_5::CellInfoGsm();
}

::android::hardware::radio::V1_6::CellInfo::CellInfoRatSpecificInfo::~CellInfoRatSpecificInfo() {
    hidl_destructUnion();
}

::android::hardware::radio::V1_6::CellInfo::CellInfoRatSpecificInfo::CellInfoRatSpecificInfo(CellInfoRatSpecificInfo&& other) : ::android::hardware::radio::V1_6::CellInfo::CellInfoRatSpecificInfo() {
    *this = std::move(other);
}

::android::hardware::radio::V1_6::CellInfo::CellInfoRatSpecificInfo::CellInfoRatSpecificInfo(const CellInfoRatSpecificInfo& other) : ::android::hardware::radio::V1_6::CellInfo::CellInfoRatSpecificInfo() {
    *this = other;
}

::android::hardware::radio::V1_6::CellInfo::CellInfoRatSpecificInfo& (::android::hardware::radio::V1_6::CellInfo::CellInfoRatSpecificInfo::operator=)(CellInfoRatSpecificInfo&& other) {
    if (this == &other) { return *this; }

    switch (other.hidl_d) {
        case hidl_discriminator::gsm: {
            gsm(std::move(other.hidl_u.gsm));
            break;
        }
        case hidl_discriminator::wcdma: {
            wcdma(std::move(other.hidl_u.wcdma));
            break;
        }
        case hidl_discriminator::tdscdma: {
            tdscdma(std::move(other.hidl_u.tdscdma));
            break;
        }
        case hidl_discriminator::lte: {
            lte(std::move(other.hidl_u.lte));
            break;
        }
        case hidl_discriminator::nr: {
            nr(std::move(other.hidl_u.nr));
            break;
        }
        case hidl_discriminator::cdma: {
            cdma(std::move(other.hidl_u.cdma));
            break;
        }
        default: {
            ::android::hardware::details::logAlwaysFatal((
                    "Unknown union discriminator (value: " +
                    std::to_string((uint8_t) other.hidl_d) + ").").c_str());
        }
    }
    return *this;
}

::android::hardware::radio::V1_6::CellInfo::CellInfoRatSpecificInfo& (::android::hardware::radio::V1_6::CellInfo::CellInfoRatSpecificInfo::operator=)(const CellInfoRatSpecificInfo& other) {
    if (this == &other) { return *this; }

    switch (other.hidl_d) {
        case hidl_discriminator::gsm: {
            gsm(other.hidl_u.gsm);
            break;
        }
        case hidl_discriminator::wcdma: {
            wcdma(other.hidl_u.wcdma);
            break;
        }
        case hidl_discriminator::tdscdma: {
            tdscdma(other.hidl_u.tdscdma);
            break;
        }
        case hidl_discriminator::lte: {
            lte(other.hidl_u.lte);
            break;
        }
        case hidl_discriminator::nr: {
            nr(other.hidl_u.nr);
            break;
        }
        case hidl_discriminator::cdma: {
            cdma(other.hidl_u.cdma);
            break;
        }
        default: {
            ::android::hardware::details::logAlwaysFatal((
                    "Unknown union discriminator (value: " +
                    std::to_string((uint8_t) other.hidl_d) + ").").c_str());
        }
    }
    return *this;
}

void ::android::hardware::radio::V1_6::CellInfo::CellInfoRatSpecificInfo::hidl_destructUnion() {
    switch (hidl_d) {
        case hidl_discriminator::gsm: {
            ::android::hardware::details::destructElement(&(hidl_u.gsm));
            break;
        }
        case hidl_discriminator::wcdma: {
            ::android::hardware::details::destructElement(&(hidl_u.wcdma));
            break;
        }
        case hidl_discriminator::tdscdma: {
            ::android::hardware::details::destructElement(&(hidl_u.tdscdma));
            break;
        }
        case hidl_discriminator::lte: {
            ::android::hardware::details::destructElement(&(hidl_u.lte));
            break;
        }
        case hidl_discriminator::nr: {
            ::android::hardware::details::destructElement(&(hidl_u.nr));
            break;
        }
        case hidl_discriminator::cdma: {
            ::android::hardware::details::destructElement(&(hidl_u.cdma));
            break;
        }
        default: {
            ::android::hardware::details::logAlwaysFatal((
                    "Unknown union discriminator (value: " +
                    std::to_string((uint8_t) hidl_d) + ").").c_str());
        }
    }

}

void ::android::hardware::radio::V1_6::CellInfo::CellInfoRatSpecificInfo::gsm(const ::android::hardware::radio::V1_5::CellInfoGsm& o) {
    if (hidl_d != hidl_discriminator::gsm) {
        hidl_destructUnion();
        ::std::memset(&hidl_u, 0, sizeof(hidl_u));

        new (&hidl_u.gsm) ::android::hardware::radio::V1_5::CellInfoGsm(o);
        hidl_d = hidl_discriminator::gsm;
    }
    else if (&(hidl_u.gsm) != &o) {
        hidl_u.gsm = o;
    }
}

void ::android::hardware::radio::V1_6::CellInfo::CellInfoRatSpecificInfo::gsm(::android::hardware::radio::V1_5::CellInfoGsm&& o) {
    if (hidl_d != hidl_discriminator::gsm) {
        hidl_destructUnion();
        ::std::memset(&hidl_u, 0, sizeof(hidl_u));

        new (&hidl_u.gsm) ::android::hardware::radio::V1_5::CellInfoGsm(std::move(o));
        hidl_d = hidl_discriminator::gsm;
    }
    else if (&(hidl_u.gsm) != &o) {
        hidl_u.gsm = std::move(o);
    }
}

::android::hardware::radio::V1_5::CellInfoGsm& (::android::hardware::radio::V1_6::CellInfo::CellInfoRatSpecificInfo::gsm)() {
    if (CC_UNLIKELY(hidl_d != hidl_discriminator::gsm)) {
        LOG_ALWAYS_FATAL("Bad safe_union access: safe_union has discriminator %" PRIu64 " but discriminator %" PRIu64 " was accessed.",
                static_cast<uint64_t>(hidl_d), static_cast<uint64_t>(hidl_discriminator::gsm));
    }

    return hidl_u.gsm;
}

const ::android::hardware::radio::V1_5::CellInfoGsm& (::android::hardware::radio::V1_6::CellInfo::CellInfoRatSpecificInfo::gsm)() const {
    if (CC_UNLIKELY(hidl_d != hidl_discriminator::gsm)) {
        LOG_ALWAYS_FATAL("Bad safe_union access: safe_union has discriminator %" PRIu64 " but discriminator %" PRIu64 " was accessed.",
                static_cast<uint64_t>(hidl_d), static_cast<uint64_t>(hidl_discriminator::gsm));
    }

    return hidl_u.gsm;
}

void ::android::hardware::radio::V1_6::CellInfo::CellInfoRatSpecificInfo::wcdma(const ::android::hardware::radio::V1_5::CellInfoWcdma& o) {
    if (hidl_d != hidl_discriminator::wcdma) {
        hidl_destructUnion();
        ::std::memset(&hidl_u, 0, sizeof(hidl_u));

        new (&hidl_u.wcdma) ::android::hardware::radio::V1_5::CellInfoWcdma(o);
        hidl_d = hidl_discriminator::wcdma;
    }
    else if (&(hidl_u.wcdma) != &o) {
        hidl_u.wcdma = o;
    }
}

void ::android::hardware::radio::V1_6::CellInfo::CellInfoRatSpecificInfo::wcdma(::android::hardware::radio::V1_5::CellInfoWcdma&& o) {
    if (hidl_d != hidl_discriminator::wcdma) {
        hidl_destructUnion();
        ::std::memset(&hidl_u, 0, sizeof(hidl_u));

        new (&hidl_u.wcdma) ::android::hardware::radio::V1_5::CellInfoWcdma(std::move(o));
        hidl_d = hidl_discriminator::wcdma;
    }
    else if (&(hidl_u.wcdma) != &o) {
        hidl_u.wcdma = std::move(o);
    }
}

::android::hardware::radio::V1_5::CellInfoWcdma& (::android::hardware::radio::V1_6::CellInfo::CellInfoRatSpecificInfo::wcdma)() {
    if (CC_UNLIKELY(hidl_d != hidl_discriminator::wcdma)) {
        LOG_ALWAYS_FATAL("Bad safe_union access: safe_union has discriminator %" PRIu64 " but discriminator %" PRIu64 " was accessed.",
                static_cast<uint64_t>(hidl_d), static_cast<uint64_t>(hidl_discriminator::wcdma));
    }

    return hidl_u.wcdma;
}

const ::android::hardware::radio::V1_5::CellInfoWcdma& (::android::hardware::radio::V1_6::CellInfo::CellInfoRatSpecificInfo::wcdma)() const {
    if (CC_UNLIKELY(hidl_d != hidl_discriminator::wcdma)) {
        LOG_ALWAYS_FATAL("Bad safe_union access: safe_union has discriminator %" PRIu64 " but discriminator %" PRIu64 " was accessed.",
                static_cast<uint64_t>(hidl_d), static_cast<uint64_t>(hidl_discriminator::wcdma));
    }

    return hidl_u.wcdma;
}

void ::android::hardware::radio::V1_6::CellInfo::CellInfoRatSpecificInfo::tdscdma(const ::android::hardware::radio::V1_5::CellInfoTdscdma& o) {
    if (hidl_d != hidl_discriminator::tdscdma) {
        hidl_destructUnion();
        ::std::memset(&hidl_u, 0, sizeof(hidl_u));

        new (&hidl_u.tdscdma) ::android::hardware::radio::V1_5::CellInfoTdscdma(o);
        hidl_d = hidl_discriminator::tdscdma;
    }
    else if (&(hidl_u.tdscdma) != &o) {
        hidl_u.tdscdma = o;
    }
}

void ::android::hardware::radio::V1_6::CellInfo::CellInfoRatSpecificInfo::tdscdma(::android::hardware::radio::V1_5::CellInfoTdscdma&& o) {
    if (hidl_d != hidl_discriminator::tdscdma) {
        hidl_destructUnion();
        ::std::memset(&hidl_u, 0, sizeof(hidl_u));

        new (&hidl_u.tdscdma) ::android::hardware::radio::V1_5::CellInfoTdscdma(std::move(o));
        hidl_d = hidl_discriminator::tdscdma;
    }
    else if (&(hidl_u.tdscdma) != &o) {
        hidl_u.tdscdma = std::move(o);
    }
}

::android::hardware::radio::V1_5::CellInfoTdscdma& (::android::hardware::radio::V1_6::CellInfo::CellInfoRatSpecificInfo::tdscdma)() {
    if (CC_UNLIKELY(hidl_d != hidl_discriminator::tdscdma)) {
        LOG_ALWAYS_FATAL("Bad safe_union access: safe_union has discriminator %" PRIu64 " but discriminator %" PRIu64 " was accessed.",
                static_cast<uint64_t>(hidl_d), static_cast<uint64_t>(hidl_discriminator::tdscdma));
    }

    return hidl_u.tdscdma;
}

const ::android::hardware::radio::V1_5::CellInfoTdscdma& (::android::hardware::radio::V1_6::CellInfo::CellInfoRatSpecificInfo::tdscdma)() const {
    if (CC_UNLIKELY(hidl_d != hidl_discriminator::tdscdma)) {
        LOG_ALWAYS_FATAL("Bad safe_union access: safe_union has discriminator %" PRIu64 " but discriminator %" PRIu64 " was accessed.",
                static_cast<uint64_t>(hidl_d), static_cast<uint64_t>(hidl_discriminator::tdscdma));
    }

    return hidl_u.tdscdma;
}

void ::android::hardware::radio::V1_6::CellInfo::CellInfoRatSpecificInfo::lte(const ::android::hardware::radio::V1_6::CellInfoLte& o) {
    if (hidl_d != hidl_discriminator::lte) {
        hidl_destructUnion();
        ::std::memset(&hidl_u, 0, sizeof(hidl_u));

        new (&hidl_u.lte) ::android::hardware::radio::V1_6::CellInfoLte(o);
        hidl_d = hidl_discriminator::lte;
    }
    else if (&(hidl_u.lte) != &o) {
        hidl_u.lte = o;
    }
}

void ::android::hardware::radio::V1_6::CellInfo::CellInfoRatSpecificInfo::lte(::android::hardware::radio::V1_6::CellInfoLte&& o) {
    if (hidl_d != hidl_discriminator::lte) {
        hidl_destructUnion();
        ::std::memset(&hidl_u, 0, sizeof(hidl_u));

        new (&hidl_u.lte) ::android::hardware::radio::V1_6::CellInfoLte(std::move(o));
        hidl_d = hidl_discriminator::lte;
    }
    else if (&(hidl_u.lte) != &o) {
        hidl_u.lte = std::move(o);
    }
}

::android::hardware::radio::V1_6::CellInfoLte& (::android::hardware::radio::V1_6::CellInfo::CellInfoRatSpecificInfo::lte)() {
    if (CC_UNLIKELY(hidl_d != hidl_discriminator::lte)) {
        LOG_ALWAYS_FATAL("Bad safe_union access: safe_union has discriminator %" PRIu64 " but discriminator %" PRIu64 " was accessed.",
                static_cast<uint64_t>(hidl_d), static_cast<uint64_t>(hidl_discriminator::lte));
    }

    return hidl_u.lte;
}

const ::android::hardware::radio::V1_6::CellInfoLte& (::android::hardware::radio::V1_6::CellInfo::CellInfoRatSpecificInfo::lte)() const {
    if (CC_UNLIKELY(hidl_d != hidl_discriminator::lte)) {
        LOG_ALWAYS_FATAL("Bad safe_union access: safe_union has discriminator %" PRIu64 " but discriminator %" PRIu64 " was accessed.",
                static_cast<uint64_t>(hidl_d), static_cast<uint64_t>(hidl_discriminator::lte));
    }

    return hidl_u.lte;
}

void ::android::hardware::radio::V1_6::CellInfo::CellInfoRatSpecificInfo::nr(const ::android::hardware::radio::V1_6::CellInfoNr& o) {
    if (hidl_d != hidl_discriminator::nr) {
        hidl_destructUnion();
        ::std::memset(&hidl_u, 0, sizeof(hidl_u));

        new (&hidl_u.nr) ::android::hardware::radio::V1_6::CellInfoNr(o);
        hidl_d = hidl_discriminator::nr;
    }
    else if (&(hidl_u.nr) != &o) {
        hidl_u.nr = o;
    }
}

void ::android::hardware::radio::V1_6::CellInfo::CellInfoRatSpecificInfo::nr(::android::hardware::radio::V1_6::CellInfoNr&& o) {
    if (hidl_d != hidl_discriminator::nr) {
        hidl_destructUnion();
        ::std::memset(&hidl_u, 0, sizeof(hidl_u));

        new (&hidl_u.nr) ::android::hardware::radio::V1_6::CellInfoNr(std::move(o));
        hidl_d = hidl_discriminator::nr;
    }
    else if (&(hidl_u.nr) != &o) {
        hidl_u.nr = std::move(o);
    }
}

::android::hardware::radio::V1_6::CellInfoNr& (::android::hardware::radio::V1_6::CellInfo::CellInfoRatSpecificInfo::nr)() {
    if (CC_UNLIKELY(hidl_d != hidl_discriminator::nr)) {
        LOG_ALWAYS_FATAL("Bad safe_union access: safe_union has discriminator %" PRIu64 " but discriminator %" PRIu64 " was accessed.",
                static_cast<uint64_t>(hidl_d), static_cast<uint64_t>(hidl_discriminator::nr));
    }

    return hidl_u.nr;
}

const ::android::hardware::radio::V1_6::CellInfoNr& (::android::hardware::radio::V1_6::CellInfo::CellInfoRatSpecificInfo::nr)() const {
    if (CC_UNLIKELY(hidl_d != hidl_discriminator::nr)) {
        LOG_ALWAYS_FATAL("Bad safe_union access: safe_union has discriminator %" PRIu64 " but discriminator %" PRIu64 " was accessed.",
                static_cast<uint64_t>(hidl_d), static_cast<uint64_t>(hidl_discriminator::nr));
    }

    return hidl_u.nr;
}

void ::android::hardware::radio::V1_6::CellInfo::CellInfoRatSpecificInfo::cdma(const ::android::hardware::radio::V1_2::CellInfoCdma& o) {
    if (hidl_d != hidl_discriminator::cdma) {
        hidl_destructUnion();
        ::std::memset(&hidl_u, 0, sizeof(hidl_u));

        new (&hidl_u.cdma) ::android::hardware::radio::V1_2::CellInfoCdma(o);
        hidl_d = hidl_discriminator::cdma;
    }
    else if (&(hidl_u.cdma) != &o) {
        hidl_u.cdma = o;
    }
}

void ::android::hardware::radio::V1_6::CellInfo::CellInfoRatSpecificInfo::cdma(::android::hardware::radio::V1_2::CellInfoCdma&& o) {
    if (hidl_d != hidl_discriminator::cdma) {
        hidl_destructUnion();
        ::std::memset(&hidl_u, 0, sizeof(hidl_u));

        new (&hidl_u.cdma) ::android::hardware::radio::V1_2::CellInfoCdma(std::move(o));
        hidl_d = hidl_discriminator::cdma;
    }
    else if (&(hidl_u.cdma) != &o) {
        hidl_u.cdma = std::move(o);
    }
}

::android::hardware::radio::V1_2::CellInfoCdma& (::android::hardware::radio::V1_6::CellInfo::CellInfoRatSpecificInfo::cdma)() {
    if (CC_UNLIKELY(hidl_d != hidl_discriminator::cdma)) {
        LOG_ALWAYS_FATAL("Bad safe_union access: safe_union has discriminator %" PRIu64 " but discriminator %" PRIu64 " was accessed.",
                static_cast<uint64_t>(hidl_d), static_cast<uint64_t>(hidl_discriminator::cdma));
    }

    return hidl_u.cdma;
}

const ::android::hardware::radio::V1_2::CellInfoCdma& (::android::hardware::radio::V1_6::CellInfo::CellInfoRatSpecificInfo::cdma)() const {
    if (CC_UNLIKELY(hidl_d != hidl_discriminator::cdma)) {
        LOG_ALWAYS_FATAL("Bad safe_union access: safe_union has discriminator %" PRIu64 " but discriminator %" PRIu64 " was accessed.",
                static_cast<uint64_t>(hidl_d), static_cast<uint64_t>(hidl_discriminator::cdma));
    }

    return hidl_u.cdma;
}

::android::hardware::radio::V1_6::CellInfo::CellInfoRatSpecificInfo::hidl_union::hidl_union() {}

::android::hardware::radio::V1_6::CellInfo::CellInfoRatSpecificInfo::hidl_union::~hidl_union() {}

::android::hardware::radio::V1_6::CellInfo::CellInfoRatSpecificInfo::hidl_discriminator (::android::hardware::radio::V1_6::CellInfo::CellInfoRatSpecificInfo::getDiscriminator)() const {
    return hidl_d;
}

::android::hardware::radio::V1_6::RegStateResult::AccessTechnologySpecificInfo::AccessTechnologySpecificInfo() {
    static_assert(offsetof(::android::hardware::radio::V1_6::RegStateResult::AccessTechnologySpecificInfo, hidl_d) == 0, "wrong offset");
    static_assert(offsetof(::android::hardware::radio::V1_6::RegStateResult::AccessTechnologySpecificInfo, hidl_u) == 4, "wrong offset");

    ::std::memset(&hidl_u, 0, sizeof(hidl_u));
    ::std::memset(reinterpret_cast<uint8_t*>(this) + 1, 0, 3);
    // no padding to zero starting at offset 20

    hidl_d = hidl_discriminator::noinit;
    new (&hidl_u.noinit) ::android::hidl::safe_union::V1_0::Monostate();
}

::android::hardware::radio::V1_6::RegStateResult::AccessTechnologySpecificInfo::~AccessTechnologySpecificInfo() {
    hidl_destructUnion();
}

::android::hardware::radio::V1_6::RegStateResult::AccessTechnologySpecificInfo::AccessTechnologySpecificInfo(AccessTechnologySpecificInfo&& other) : ::android::hardware::radio::V1_6::RegStateResult::AccessTechnologySpecificInfo() {
    *this = std::move(other);
}

::android::hardware::radio::V1_6::RegStateResult::AccessTechnologySpecificInfo::AccessTechnologySpecificInfo(const AccessTechnologySpecificInfo& other) : ::android::hardware::radio::V1_6::RegStateResult::AccessTechnologySpecificInfo() {
    *this = other;
}

::android::hardware::radio::V1_6::RegStateResult::AccessTechnologySpecificInfo& (::android::hardware::radio::V1_6::RegStateResult::AccessTechnologySpecificInfo::operator=)(AccessTechnologySpecificInfo&& other) {
    if (this == &other) { return *this; }

    switch (other.hidl_d) {
        case hidl_discriminator::noinit: {
            noinit(std::move(other.hidl_u.noinit));
            break;
        }
        case hidl_discriminator::cdmaInfo: {
            cdmaInfo(std::move(other.hidl_u.cdmaInfo));
            break;
        }
        case hidl_discriminator::eutranInfo: {
            eutranInfo(std::move(other.hidl_u.eutranInfo));
            break;
        }
        case hidl_discriminator::ngranNrVopsInfo: {
            ngranNrVopsInfo(std::move(other.hidl_u.ngranNrVopsInfo));
            break;
        }
        case hidl_discriminator::geranDtmSupported: {
            geranDtmSupported(std::move(other.hidl_u.geranDtmSupported));
            break;
        }
        default: {
            ::android::hardware::details::logAlwaysFatal((
                    "Unknown union discriminator (value: " +
                    std::to_string((uint8_t) other.hidl_d) + ").").c_str());
        }
    }
    return *this;
}

::android::hardware::radio::V1_6::RegStateResult::AccessTechnologySpecificInfo& (::android::hardware::radio::V1_6::RegStateResult::AccessTechnologySpecificInfo::operator=)(const AccessTechnologySpecificInfo& other) {
    if (this == &other) { return *this; }

    switch (other.hidl_d) {
        case hidl_discriminator::noinit: {
            noinit(other.hidl_u.noinit);
            break;
        }
        case hidl_discriminator::cdmaInfo: {
            cdmaInfo(other.hidl_u.cdmaInfo);
            break;
        }
        case hidl_discriminator::eutranInfo: {
            eutranInfo(other.hidl_u.eutranInfo);
            break;
        }
        case hidl_discriminator::ngranNrVopsInfo: {
            ngranNrVopsInfo(other.hidl_u.ngranNrVopsInfo);
            break;
        }
        case hidl_discriminator::geranDtmSupported: {
            geranDtmSupported(other.hidl_u.geranDtmSupported);
            break;
        }
        default: {
            ::android::hardware::details::logAlwaysFatal((
                    "Unknown union discriminator (value: " +
                    std::to_string((uint8_t) other.hidl_d) + ").").c_str());
        }
    }
    return *this;
}

void ::android::hardware::radio::V1_6::RegStateResult::AccessTechnologySpecificInfo::hidl_destructUnion() {
    switch (hidl_d) {
        case hidl_discriminator::noinit: {
            ::android::hardware::details::destructElement(&(hidl_u.noinit));
            break;
        }
        case hidl_discriminator::cdmaInfo: {
            ::android::hardware::details::destructElement(&(hidl_u.cdmaInfo));
            break;
        }
        case hidl_discriminator::eutranInfo: {
            ::android::hardware::details::destructElement(&(hidl_u.eutranInfo));
            break;
        }
        case hidl_discriminator::ngranNrVopsInfo: {
            ::android::hardware::details::destructElement(&(hidl_u.ngranNrVopsInfo));
            break;
        }
        case hidl_discriminator::geranDtmSupported: {
            ::android::hardware::details::destructElement(&(hidl_u.geranDtmSupported));
            break;
        }
        default: {
            ::android::hardware::details::logAlwaysFatal((
                    "Unknown union discriminator (value: " +
                    std::to_string((uint8_t) hidl_d) + ").").c_str());
        }
    }

}

void ::android::hardware::radio::V1_6::RegStateResult::AccessTechnologySpecificInfo::noinit(const ::android::hidl::safe_union::V1_0::Monostate& o) {
    if (hidl_d != hidl_discriminator::noinit) {
        hidl_destructUnion();
        ::std::memset(&hidl_u, 0, sizeof(hidl_u));

        new (&hidl_u.noinit) ::android::hidl::safe_union::V1_0::Monostate(o);
        hidl_d = hidl_discriminator::noinit;
    }
    else if (&(hidl_u.noinit) != &o) {
        hidl_u.noinit = o;
    }
}

void ::android::hardware::radio::V1_6::RegStateResult::AccessTechnologySpecificInfo::noinit(::android::hidl::safe_union::V1_0::Monostate&& o) {
    if (hidl_d != hidl_discriminator::noinit) {
        hidl_destructUnion();
        ::std::memset(&hidl_u, 0, sizeof(hidl_u));

        new (&hidl_u.noinit) ::android::hidl::safe_union::V1_0::Monostate(std::move(o));
        hidl_d = hidl_discriminator::noinit;
    }
    else if (&(hidl_u.noinit) != &o) {
        hidl_u.noinit = std::move(o);
    }
}

::android::hidl::safe_union::V1_0::Monostate& (::android::hardware::radio::V1_6::RegStateResult::AccessTechnologySpecificInfo::noinit)() {
    if (CC_UNLIKELY(hidl_d != hidl_discriminator::noinit)) {
        LOG_ALWAYS_FATAL("Bad safe_union access: safe_union has discriminator %" PRIu64 " but discriminator %" PRIu64 " was accessed.",
                static_cast<uint64_t>(hidl_d), static_cast<uint64_t>(hidl_discriminator::noinit));
    }

    return hidl_u.noinit;
}

const ::android::hidl::safe_union::V1_0::Monostate& (::android::hardware::radio::V1_6::RegStateResult::AccessTechnologySpecificInfo::noinit)() const {
    if (CC_UNLIKELY(hidl_d != hidl_discriminator::noinit)) {
        LOG_ALWAYS_FATAL("Bad safe_union access: safe_union has discriminator %" PRIu64 " but discriminator %" PRIu64 " was accessed.",
                static_cast<uint64_t>(hidl_d), static_cast<uint64_t>(hidl_discriminator::noinit));
    }

    return hidl_u.noinit;
}

void ::android::hardware::radio::V1_6::RegStateResult::AccessTechnologySpecificInfo::cdmaInfo(const ::android::hardware::radio::V1_5::RegStateResult::AccessTechnologySpecificInfo::Cdma2000RegistrationInfo& o) {
    if (hidl_d != hidl_discriminator::cdmaInfo) {
        hidl_destructUnion();
        ::std::memset(&hidl_u, 0, sizeof(hidl_u));

        new (&hidl_u.cdmaInfo) ::android::hardware::radio::V1_5::RegStateResult::AccessTechnologySpecificInfo::Cdma2000RegistrationInfo(o);
        hidl_d = hidl_discriminator::cdmaInfo;
    }
    else if (&(hidl_u.cdmaInfo) != &o) {
        hidl_u.cdmaInfo = o;
    }
}

void ::android::hardware::radio::V1_6::RegStateResult::AccessTechnologySpecificInfo::cdmaInfo(::android::hardware::radio::V1_5::RegStateResult::AccessTechnologySpecificInfo::Cdma2000RegistrationInfo&& o) {
    if (hidl_d != hidl_discriminator::cdmaInfo) {
        hidl_destructUnion();
        ::std::memset(&hidl_u, 0, sizeof(hidl_u));

        new (&hidl_u.cdmaInfo) ::android::hardware::radio::V1_5::RegStateResult::AccessTechnologySpecificInfo::Cdma2000RegistrationInfo(std::move(o));
        hidl_d = hidl_discriminator::cdmaInfo;
    }
    else if (&(hidl_u.cdmaInfo) != &o) {
        hidl_u.cdmaInfo = std::move(o);
    }
}

::android::hardware::radio::V1_5::RegStateResult::AccessTechnologySpecificInfo::Cdma2000RegistrationInfo& (::android::hardware::radio::V1_6::RegStateResult::AccessTechnologySpecificInfo::cdmaInfo)() {
    if (CC_UNLIKELY(hidl_d != hidl_discriminator::cdmaInfo)) {
        LOG_ALWAYS_FATAL("Bad safe_union access: safe_union has discriminator %" PRIu64 " but discriminator %" PRIu64 " was accessed.",
                static_cast<uint64_t>(hidl_d), static_cast<uint64_t>(hidl_discriminator::cdmaInfo));
    }

    return hidl_u.cdmaInfo;
}

const ::android::hardware::radio::V1_5::RegStateResult::AccessTechnologySpecificInfo::Cdma2000RegistrationInfo& (::android::hardware::radio::V1_6::RegStateResult::AccessTechnologySpecificInfo::cdmaInfo)() const {
    if (CC_UNLIKELY(hidl_d != hidl_discriminator::cdmaInfo)) {
        LOG_ALWAYS_FATAL("Bad safe_union access: safe_union has discriminator %" PRIu64 " but discriminator %" PRIu64 " was accessed.",
                static_cast<uint64_t>(hidl_d), static_cast<uint64_t>(hidl_discriminator::cdmaInfo));
    }

    return hidl_u.cdmaInfo;
}

void ::android::hardware::radio::V1_6::RegStateResult::AccessTechnologySpecificInfo::eutranInfo(const ::android::hardware::radio::V1_5::RegStateResult::AccessTechnologySpecificInfo::EutranRegistrationInfo& o) {
    if (hidl_d != hidl_discriminator::eutranInfo) {
        hidl_destructUnion();
        ::std::memset(&hidl_u, 0, sizeof(hidl_u));

        new (&hidl_u.eutranInfo) ::android::hardware::radio::V1_5::RegStateResult::AccessTechnologySpecificInfo::EutranRegistrationInfo(o);
        hidl_d = hidl_discriminator::eutranInfo;
    }
    else if (&(hidl_u.eutranInfo) != &o) {
        hidl_u.eutranInfo = o;
    }
}

void ::android::hardware::radio::V1_6::RegStateResult::AccessTechnologySpecificInfo::eutranInfo(::android::hardware::radio::V1_5::RegStateResult::AccessTechnologySpecificInfo::EutranRegistrationInfo&& o) {
    if (hidl_d != hidl_discriminator::eutranInfo) {
        hidl_destructUnion();
        ::std::memset(&hidl_u, 0, sizeof(hidl_u));

        new (&hidl_u.eutranInfo) ::android::hardware::radio::V1_5::RegStateResult::AccessTechnologySpecificInfo::EutranRegistrationInfo(std::move(o));
        hidl_d = hidl_discriminator::eutranInfo;
    }
    else if (&(hidl_u.eutranInfo) != &o) {
        hidl_u.eutranInfo = std::move(o);
    }
}

::android::hardware::radio::V1_5::RegStateResult::AccessTechnologySpecificInfo::EutranRegistrationInfo& (::android::hardware::radio::V1_6::RegStateResult::AccessTechnologySpecificInfo::eutranInfo)() {
    if (CC_UNLIKELY(hidl_d != hidl_discriminator::eutranInfo)) {
        LOG_ALWAYS_FATAL("Bad safe_union access: safe_union has discriminator %" PRIu64 " but discriminator %" PRIu64 " was accessed.",
                static_cast<uint64_t>(hidl_d), static_cast<uint64_t>(hidl_discriminator::eutranInfo));
    }

    return hidl_u.eutranInfo;
}

const ::android::hardware::radio::V1_5::RegStateResult::AccessTechnologySpecificInfo::EutranRegistrationInfo& (::android::hardware::radio::V1_6::RegStateResult::AccessTechnologySpecificInfo::eutranInfo)() const {
    if (CC_UNLIKELY(hidl_d != hidl_discriminator::eutranInfo)) {
        LOG_ALWAYS_FATAL("Bad safe_union access: safe_union has discriminator %" PRIu64 " but discriminator %" PRIu64 " was accessed.",
                static_cast<uint64_t>(hidl_d), static_cast<uint64_t>(hidl_discriminator::eutranInfo));
    }

    return hidl_u.eutranInfo;
}

void ::android::hardware::radio::V1_6::RegStateResult::AccessTechnologySpecificInfo::ngranNrVopsInfo(const ::android::hardware::radio::V1_6::NrVopsInfo& o) {
    if (hidl_d != hidl_discriminator::ngranNrVopsInfo) {
        hidl_destructUnion();
        ::std::memset(&hidl_u, 0, sizeof(hidl_u));

        new (&hidl_u.ngranNrVopsInfo) ::android::hardware::radio::V1_6::NrVopsInfo(o);
        hidl_d = hidl_discriminator::ngranNrVopsInfo;
    }
    else if (&(hidl_u.ngranNrVopsInfo) != &o) {
        hidl_u.ngranNrVopsInfo = o;
    }
}

void ::android::hardware::radio::V1_6::RegStateResult::AccessTechnologySpecificInfo::ngranNrVopsInfo(::android::hardware::radio::V1_6::NrVopsInfo&& o) {
    if (hidl_d != hidl_discriminator::ngranNrVopsInfo) {
        hidl_destructUnion();
        ::std::memset(&hidl_u, 0, sizeof(hidl_u));

        new (&hidl_u.ngranNrVopsInfo) ::android::hardware::radio::V1_6::NrVopsInfo(std::move(o));
        hidl_d = hidl_discriminator::ngranNrVopsInfo;
    }
    else if (&(hidl_u.ngranNrVopsInfo) != &o) {
        hidl_u.ngranNrVopsInfo = std::move(o);
    }
}

::android::hardware::radio::V1_6::NrVopsInfo& (::android::hardware::radio::V1_6::RegStateResult::AccessTechnologySpecificInfo::ngranNrVopsInfo)() {
    if (CC_UNLIKELY(hidl_d != hidl_discriminator::ngranNrVopsInfo)) {
        LOG_ALWAYS_FATAL("Bad safe_union access: safe_union has discriminator %" PRIu64 " but discriminator %" PRIu64 " was accessed.",
                static_cast<uint64_t>(hidl_d), static_cast<uint64_t>(hidl_discriminator::ngranNrVopsInfo));
    }

    return hidl_u.ngranNrVopsInfo;
}

const ::android::hardware::radio::V1_6::NrVopsInfo& (::android::hardware::radio::V1_6::RegStateResult::AccessTechnologySpecificInfo::ngranNrVopsInfo)() const {
    if (CC_UNLIKELY(hidl_d != hidl_discriminator::ngranNrVopsInfo)) {
        LOG_ALWAYS_FATAL("Bad safe_union access: safe_union has discriminator %" PRIu64 " but discriminator %" PRIu64 " was accessed.",
                static_cast<uint64_t>(hidl_d), static_cast<uint64_t>(hidl_discriminator::ngranNrVopsInfo));
    }

    return hidl_u.ngranNrVopsInfo;
}

void ::android::hardware::radio::V1_6::RegStateResult::AccessTechnologySpecificInfo::geranDtmSupported(bool o) {
    if (hidl_d != hidl_discriminator::geranDtmSupported) {
        hidl_destructUnion();
        ::std::memset(&hidl_u, 0, sizeof(hidl_u));

        new (&hidl_u.geranDtmSupported) bool(o);
        hidl_d = hidl_discriminator::geranDtmSupported;
    }
    else if (&(hidl_u.geranDtmSupported) != &o) {
        hidl_u.geranDtmSupported = o;
    }
}

bool& (::android::hardware::radio::V1_6::RegStateResult::AccessTechnologySpecificInfo::geranDtmSupported)() {
    if (CC_UNLIKELY(hidl_d != hidl_discriminator::geranDtmSupported)) {
        LOG_ALWAYS_FATAL("Bad safe_union access: safe_union has discriminator %" PRIu64 " but discriminator %" PRIu64 " was accessed.",
                static_cast<uint64_t>(hidl_d), static_cast<uint64_t>(hidl_discriminator::geranDtmSupported));
    }

    return hidl_u.geranDtmSupported;
}

bool (::android::hardware::radio::V1_6::RegStateResult::AccessTechnologySpecificInfo::geranDtmSupported)() const {
    if (CC_UNLIKELY(hidl_d != hidl_discriminator::geranDtmSupported)) {
        LOG_ALWAYS_FATAL("Bad safe_union access: safe_union has discriminator %" PRIu64 " but discriminator %" PRIu64 " was accessed.",
                static_cast<uint64_t>(hidl_d), static_cast<uint64_t>(hidl_discriminator::geranDtmSupported));
    }

    return hidl_u.geranDtmSupported;
}

::android::hardware::radio::V1_6::RegStateResult::AccessTechnologySpecificInfo::hidl_union::hidl_union() {}

::android::hardware::radio::V1_6::RegStateResult::AccessTechnologySpecificInfo::hidl_union::~hidl_union() {}

::android::hardware::radio::V1_6::RegStateResult::AccessTechnologySpecificInfo::hidl_discriminator (::android::hardware::radio::V1_6::RegStateResult::AccessTechnologySpecificInfo::getDiscriminator)() const {
    return hidl_d;
}

::android::hardware::radio::V1_6::PhysicalChannelConfig::Band::Band() {
    static_assert(offsetof(::android::hardware::radio::V1_6::PhysicalChannelConfig::Band, hidl_d) == 0, "wrong offset");
    static_assert(offsetof(::android::hardware::radio::V1_6::PhysicalChannelConfig::Band, hidl_u) == 4, "wrong offset");

    ::std::memset(&hidl_u, 0, sizeof(hidl_u));
    ::std::memset(reinterpret_cast<uint8_t*>(this) + 1, 0, 3);
    // no padding to zero starting at offset 8

    hidl_d = hidl_discriminator::geranBand;
    new (&hidl_u.geranBand) ::android::hardware::radio::V1_1::GeranBands();
}

::android::hardware::radio::V1_6::PhysicalChannelConfig::Band::~Band() {
    hidl_destructUnion();
}

::android::hardware::radio::V1_6::PhysicalChannelConfig::Band::Band(Band&& other) : ::android::hardware::radio::V1_6::PhysicalChannelConfig::Band() {
    *this = std::move(other);
}

::android::hardware::radio::V1_6::PhysicalChannelConfig::Band::Band(const Band& other) : ::android::hardware::radio::V1_6::PhysicalChannelConfig::Band() {
    *this = other;
}

::android::hardware::radio::V1_6::PhysicalChannelConfig::Band& (::android::hardware::radio::V1_6::PhysicalChannelConfig::Band::operator=)(Band&& other) {
    if (this == &other) { return *this; }

    switch (other.hidl_d) {
        case hidl_discriminator::geranBand: {
            geranBand(std::move(other.hidl_u.geranBand));
            break;
        }
        case hidl_discriminator::utranBand: {
            utranBand(std::move(other.hidl_u.utranBand));
            break;
        }
        case hidl_discriminator::eutranBand: {
            eutranBand(std::move(other.hidl_u.eutranBand));
            break;
        }
        case hidl_discriminator::ngranBand: {
            ngranBand(std::move(other.hidl_u.ngranBand));
            break;
        }
        default: {
            ::android::hardware::details::logAlwaysFatal((
                    "Unknown union discriminator (value: " +
                    std::to_string((uint8_t) other.hidl_d) + ").").c_str());
        }
    }
    return *this;
}

::android::hardware::radio::V1_6::PhysicalChannelConfig::Band& (::android::hardware::radio::V1_6::PhysicalChannelConfig::Band::operator=)(const Band& other) {
    if (this == &other) { return *this; }

    switch (other.hidl_d) {
        case hidl_discriminator::geranBand: {
            geranBand(other.hidl_u.geranBand);
            break;
        }
        case hidl_discriminator::utranBand: {
            utranBand(other.hidl_u.utranBand);
            break;
        }
        case hidl_discriminator::eutranBand: {
            eutranBand(other.hidl_u.eutranBand);
            break;
        }
        case hidl_discriminator::ngranBand: {
            ngranBand(other.hidl_u.ngranBand);
            break;
        }
        default: {
            ::android::hardware::details::logAlwaysFatal((
                    "Unknown union discriminator (value: " +
                    std::to_string((uint8_t) other.hidl_d) + ").").c_str());
        }
    }
    return *this;
}

void ::android::hardware::radio::V1_6::PhysicalChannelConfig::Band::hidl_destructUnion() {
    switch (hidl_d) {
        case hidl_discriminator::geranBand: {
            ::android::hardware::details::destructElement(&(hidl_u.geranBand));
            break;
        }
        case hidl_discriminator::utranBand: {
            ::android::hardware::details::destructElement(&(hidl_u.utranBand));
            break;
        }
        case hidl_discriminator::eutranBand: {
            ::android::hardware::details::destructElement(&(hidl_u.eutranBand));
            break;
        }
        case hidl_discriminator::ngranBand: {
            ::android::hardware::details::destructElement(&(hidl_u.ngranBand));
            break;
        }
        default: {
            ::android::hardware::details::logAlwaysFatal((
                    "Unknown union discriminator (value: " +
                    std::to_string((uint8_t) hidl_d) + ").").c_str());
        }
    }

}

void ::android::hardware::radio::V1_6::PhysicalChannelConfig::Band::geranBand(::android::hardware::radio::V1_1::GeranBands o) {
    if (hidl_d != hidl_discriminator::geranBand) {
        hidl_destructUnion();
        ::std::memset(&hidl_u, 0, sizeof(hidl_u));

        new (&hidl_u.geranBand) ::android::hardware::radio::V1_1::GeranBands(o);
        hidl_d = hidl_discriminator::geranBand;
    }
    else if (&(hidl_u.geranBand) != &o) {
        hidl_u.geranBand = o;
    }
}

::android::hardware::radio::V1_1::GeranBands& (::android::hardware::radio::V1_6::PhysicalChannelConfig::Band::geranBand)() {
    if (CC_UNLIKELY(hidl_d != hidl_discriminator::geranBand)) {
        LOG_ALWAYS_FATAL("Bad safe_union access: safe_union has discriminator %" PRIu64 " but discriminator %" PRIu64 " was accessed.",
                static_cast<uint64_t>(hidl_d), static_cast<uint64_t>(hidl_discriminator::geranBand));
    }

    return hidl_u.geranBand;
}

::android::hardware::radio::V1_1::GeranBands (::android::hardware::radio::V1_6::PhysicalChannelConfig::Band::geranBand)() const {
    if (CC_UNLIKELY(hidl_d != hidl_discriminator::geranBand)) {
        LOG_ALWAYS_FATAL("Bad safe_union access: safe_union has discriminator %" PRIu64 " but discriminator %" PRIu64 " was accessed.",
                static_cast<uint64_t>(hidl_d), static_cast<uint64_t>(hidl_discriminator::geranBand));
    }

    return hidl_u.geranBand;
}

void ::android::hardware::radio::V1_6::PhysicalChannelConfig::Band::utranBand(::android::hardware::radio::V1_1::UtranBands o) {
    if (hidl_d != hidl_discriminator::utranBand) {
        hidl_destructUnion();
        ::std::memset(&hidl_u, 0, sizeof(hidl_u));

        new (&hidl_u.utranBand) ::android::hardware::radio::V1_1::UtranBands(o);
        hidl_d = hidl_discriminator::utranBand;
    }
    else if (&(hidl_u.utranBand) != &o) {
        hidl_u.utranBand = o;
    }
}

::android::hardware::radio::V1_1::UtranBands& (::android::hardware::radio::V1_6::PhysicalChannelConfig::Band::utranBand)() {
    if (CC_UNLIKELY(hidl_d != hidl_discriminator::utranBand)) {
        LOG_ALWAYS_FATAL("Bad safe_union access: safe_union has discriminator %" PRIu64 " but discriminator %" PRIu64 " was accessed.",
                static_cast<uint64_t>(hidl_d), static_cast<uint64_t>(hidl_discriminator::utranBand));
    }

    return hidl_u.utranBand;
}

::android::hardware::radio::V1_1::UtranBands (::android::hardware::radio::V1_6::PhysicalChannelConfig::Band::utranBand)() const {
    if (CC_UNLIKELY(hidl_d != hidl_discriminator::utranBand)) {
        LOG_ALWAYS_FATAL("Bad safe_union access: safe_union has discriminator %" PRIu64 " but discriminator %" PRIu64 " was accessed.",
                static_cast<uint64_t>(hidl_d), static_cast<uint64_t>(hidl_discriminator::utranBand));
    }

    return hidl_u.utranBand;
}

void ::android::hardware::radio::V1_6::PhysicalChannelConfig::Band::eutranBand(::android::hardware::radio::V1_1::EutranBands o) {
    if (hidl_d != hidl_discriminator::eutranBand) {
        hidl_destructUnion();
        ::std::memset(&hidl_u, 0, sizeof(hidl_u));

        new (&hidl_u.eutranBand) ::android::hardware::radio::V1_1::EutranBands(o);
        hidl_d = hidl_discriminator::eutranBand;
    }
    else if (&(hidl_u.eutranBand) != &o) {
        hidl_u.eutranBand = o;
    }
}

::android::hardware::radio::V1_1::EutranBands& (::android::hardware::radio::V1_6::PhysicalChannelConfig::Band::eutranBand)() {
    if (CC_UNLIKELY(hidl_d != hidl_discriminator::eutranBand)) {
        LOG_ALWAYS_FATAL("Bad safe_union access: safe_union has discriminator %" PRIu64 " but discriminator %" PRIu64 " was accessed.",
                static_cast<uint64_t>(hidl_d), static_cast<uint64_t>(hidl_discriminator::eutranBand));
    }

    return hidl_u.eutranBand;
}

::android::hardware::radio::V1_1::EutranBands (::android::hardware::radio::V1_6::PhysicalChannelConfig::Band::eutranBand)() const {
    if (CC_UNLIKELY(hidl_d != hidl_discriminator::eutranBand)) {
        LOG_ALWAYS_FATAL("Bad safe_union access: safe_union has discriminator %" PRIu64 " but discriminator %" PRIu64 " was accessed.",
                static_cast<uint64_t>(hidl_d), static_cast<uint64_t>(hidl_discriminator::eutranBand));
    }

    return hidl_u.eutranBand;
}

void ::android::hardware::radio::V1_6::PhysicalChannelConfig::Band::ngranBand(::android::hardware::radio::V1_6::NgranBands o) {
    if (hidl_d != hidl_discriminator::ngranBand) {
        hidl_destructUnion();
        ::std::memset(&hidl_u, 0, sizeof(hidl_u));

        new (&hidl_u.ngranBand) ::android::hardware::radio::V1_6::NgranBands(o);
        hidl_d = hidl_discriminator::ngranBand;
    }
    else if (&(hidl_u.ngranBand) != &o) {
        hidl_u.ngranBand = o;
    }
}

::android::hardware::radio::V1_6::NgranBands& (::android::hardware::radio::V1_6::PhysicalChannelConfig::Band::ngranBand)() {
    if (CC_UNLIKELY(hidl_d != hidl_discriminator::ngranBand)) {
        LOG_ALWAYS_FATAL("Bad safe_union access: safe_union has discriminator %" PRIu64 " but discriminator %" PRIu64 " was accessed.",
                static_cast<uint64_t>(hidl_d), static_cast<uint64_t>(hidl_discriminator::ngranBand));
    }

    return hidl_u.ngranBand;
}

::android::hardware::radio::V1_6::NgranBands (::android::hardware::radio::V1_6::PhysicalChannelConfig::Band::ngranBand)() const {
    if (CC_UNLIKELY(hidl_d != hidl_discriminator::ngranBand)) {
        LOG_ALWAYS_FATAL("Bad safe_union access: safe_union has discriminator %" PRIu64 " but discriminator %" PRIu64 " was accessed.",
                static_cast<uint64_t>(hidl_d), static_cast<uint64_t>(hidl_discriminator::ngranBand));
    }

    return hidl_u.ngranBand;
}

::android::hardware::radio::V1_6::PhysicalChannelConfig::Band::hidl_union::hidl_union() {}

::android::hardware::radio::V1_6::PhysicalChannelConfig::Band::hidl_union::~hidl_union() {}

::android::hardware::radio::V1_6::PhysicalChannelConfig::Band::hidl_discriminator (::android::hardware::radio::V1_6::PhysicalChannelConfig::Band::getDiscriminator)() const {
    return hidl_d;
}

::android::hardware::radio::V1_6::OptionalDnn::OptionalDnn() {
    static_assert(offsetof(::android::hardware::radio::V1_6::OptionalDnn, hidl_d) == 0, "wrong offset");
    static_assert(offsetof(::android::hardware::radio::V1_6::OptionalDnn, hidl_u) == 8, "wrong offset");

    ::std::memset(&hidl_u, 0, sizeof(hidl_u));
    ::std::memset(reinterpret_cast<uint8_t*>(this) + 1, 0, 7);
    // no padding to zero starting at offset 24

    hidl_d = hidl_discriminator::noinit;
    new (&hidl_u.noinit) ::android::hidl::safe_union::V1_0::Monostate();
}

::android::hardware::radio::V1_6::OptionalDnn::~OptionalDnn() {
    hidl_destructUnion();
}

::android::hardware::radio::V1_6::OptionalDnn::OptionalDnn(OptionalDnn&& other) : ::android::hardware::radio::V1_6::OptionalDnn() {
    *this = std::move(other);
}

::android::hardware::radio::V1_6::OptionalDnn::OptionalDnn(const OptionalDnn& other) : ::android::hardware::radio::V1_6::OptionalDnn() {
    *this = other;
}

::android::hardware::radio::V1_6::OptionalDnn& (::android::hardware::radio::V1_6::OptionalDnn::operator=)(OptionalDnn&& other) {
    if (this == &other) { return *this; }

    switch (other.hidl_d) {
        case hidl_discriminator::noinit: {
            noinit(std::move(other.hidl_u.noinit));
            break;
        }
        case hidl_discriminator::value: {
            value(std::move(other.hidl_u.value));
            break;
        }
        default: {
            ::android::hardware::details::logAlwaysFatal((
                    "Unknown union discriminator (value: " +
                    std::to_string((uint8_t) other.hidl_d) + ").").c_str());
        }
    }
    return *this;
}

::android::hardware::radio::V1_6::OptionalDnn& (::android::hardware::radio::V1_6::OptionalDnn::operator=)(const OptionalDnn& other) {
    if (this == &other) { return *this; }

    switch (other.hidl_d) {
        case hidl_discriminator::noinit: {
            noinit(other.hidl_u.noinit);
            break;
        }
        case hidl_discriminator::value: {
            value(other.hidl_u.value);
            break;
        }
        default: {
            ::android::hardware::details::logAlwaysFatal((
                    "Unknown union discriminator (value: " +
                    std::to_string((uint8_t) other.hidl_d) + ").").c_str());
        }
    }
    return *this;
}

void ::android::hardware::radio::V1_6::OptionalDnn::hidl_destructUnion() {
    switch (hidl_d) {
        case hidl_discriminator::noinit: {
            ::android::hardware::details::destructElement(&(hidl_u.noinit));
            break;
        }
        case hidl_discriminator::value: {
            ::android::hardware::details::destructElement(&(hidl_u.value));
            break;
        }
        default: {
            ::android::hardware::details::logAlwaysFatal((
                    "Unknown union discriminator (value: " +
                    std::to_string((uint8_t) hidl_d) + ").").c_str());
        }
    }

}

void ::android::hardware::radio::V1_6::OptionalDnn::noinit(const ::android::hidl::safe_union::V1_0::Monostate& o) {
    if (hidl_d != hidl_discriminator::noinit) {
        hidl_destructUnion();
        ::std::memset(&hidl_u, 0, sizeof(hidl_u));

        new (&hidl_u.noinit) ::android::hidl::safe_union::V1_0::Monostate(o);
        hidl_d = hidl_discriminator::noinit;
    }
    else if (&(hidl_u.noinit) != &o) {
        hidl_u.noinit = o;
    }
}

void ::android::hardware::radio::V1_6::OptionalDnn::noinit(::android::hidl::safe_union::V1_0::Monostate&& o) {
    if (hidl_d != hidl_discriminator::noinit) {
        hidl_destructUnion();
        ::std::memset(&hidl_u, 0, sizeof(hidl_u));

        new (&hidl_u.noinit) ::android::hidl::safe_union::V1_0::Monostate(std::move(o));
        hidl_d = hidl_discriminator::noinit;
    }
    else if (&(hidl_u.noinit) != &o) {
        hidl_u.noinit = std::move(o);
    }
}

::android::hidl::safe_union::V1_0::Monostate& (::android::hardware::radio::V1_6::OptionalDnn::noinit)() {
    if (CC_UNLIKELY(hidl_d != hidl_discriminator::noinit)) {
        LOG_ALWAYS_FATAL("Bad safe_union access: safe_union has discriminator %" PRIu64 " but discriminator %" PRIu64 " was accessed.",
                static_cast<uint64_t>(hidl_d), static_cast<uint64_t>(hidl_discriminator::noinit));
    }

    return hidl_u.noinit;
}

const ::android::hidl::safe_union::V1_0::Monostate& (::android::hardware::radio::V1_6::OptionalDnn::noinit)() const {
    if (CC_UNLIKELY(hidl_d != hidl_discriminator::noinit)) {
        LOG_ALWAYS_FATAL("Bad safe_union access: safe_union has discriminator %" PRIu64 " but discriminator %" PRIu64 " was accessed.",
                static_cast<uint64_t>(hidl_d), static_cast<uint64_t>(hidl_discriminator::noinit));
    }

    return hidl_u.noinit;
}

void ::android::hardware::radio::V1_6::OptionalDnn::value(const ::android::hardware::hidl_string& o) {
    if (hidl_d != hidl_discriminator::value) {
        hidl_destructUnion();
        ::std::memset(&hidl_u, 0, sizeof(hidl_u));

        new (&hidl_u.value) ::android::hardware::hidl_string(o);
        hidl_d = hidl_discriminator::value;
    }
    else if (&(hidl_u.value) != &o) {
        hidl_u.value = o;
    }
}

void ::android::hardware::radio::V1_6::OptionalDnn::value(::android::hardware::hidl_string&& o) {
    if (hidl_d != hidl_discriminator::value) {
        hidl_destructUnion();
        ::std::memset(&hidl_u, 0, sizeof(hidl_u));

        new (&hidl_u.value) ::android::hardware::hidl_string(std::move(o));
        hidl_d = hidl_discriminator::value;
    }
    else if (&(hidl_u.value) != &o) {
        hidl_u.value = std::move(o);
    }
}

::android::hardware::hidl_string& (::android::hardware::radio::V1_6::OptionalDnn::value)() {
    if (CC_UNLIKELY(hidl_d != hidl_discriminator::value)) {
        LOG_ALWAYS_FATAL("Bad safe_union access: safe_union has discriminator %" PRIu64 " but discriminator %" PRIu64 " was accessed.",
                static_cast<uint64_t>(hidl_d), static_cast<uint64_t>(hidl_discriminator::value));
    }

    return hidl_u.value;
}

const ::android::hardware::hidl_string& (::android::hardware::radio::V1_6::OptionalDnn::value)() const {
    if (CC_UNLIKELY(hidl_d != hidl_discriminator::value)) {
        LOG_ALWAYS_FATAL("Bad safe_union access: safe_union has discriminator %" PRIu64 " but discriminator %" PRIu64 " was accessed.",
                static_cast<uint64_t>(hidl_d), static_cast<uint64_t>(hidl_discriminator::value));
    }

    return hidl_u.value;
}

::android::hardware::radio::V1_6::OptionalDnn::hidl_union::hidl_union() {}

::android::hardware::radio::V1_6::OptionalDnn::hidl_union::~hidl_union() {}

::android::hardware::radio::V1_6::OptionalDnn::hidl_discriminator (::android::hardware::radio::V1_6::OptionalDnn::getDiscriminator)() const {
    return hidl_d;
}

::android::hardware::radio::V1_6::OptionalOsAppId::OptionalOsAppId() {
    static_assert(offsetof(::android::hardware::radio::V1_6::OptionalOsAppId, hidl_d) == 0, "wrong offset");
    static_assert(offsetof(::android::hardware::radio::V1_6::OptionalOsAppId, hidl_u) == 8, "wrong offset");

    ::std::memset(&hidl_u, 0, sizeof(hidl_u));
    ::std::memset(reinterpret_cast<uint8_t*>(this) + 1, 0, 7);
    // no padding to zero starting at offset 24

    hidl_d = hidl_discriminator::noinit;
    new (&hidl_u.noinit) ::android::hidl::safe_union::V1_0::Monostate();
}

::android::hardware::radio::V1_6::OptionalOsAppId::~OptionalOsAppId() {
    hidl_destructUnion();
}

::android::hardware::radio::V1_6::OptionalOsAppId::OptionalOsAppId(OptionalOsAppId&& other) : ::android::hardware::radio::V1_6::OptionalOsAppId() {
    *this = std::move(other);
}

::android::hardware::radio::V1_6::OptionalOsAppId::OptionalOsAppId(const OptionalOsAppId& other) : ::android::hardware::radio::V1_6::OptionalOsAppId() {
    *this = other;
}

::android::hardware::radio::V1_6::OptionalOsAppId& (::android::hardware::radio::V1_6::OptionalOsAppId::operator=)(OptionalOsAppId&& other) {
    if (this == &other) { return *this; }

    switch (other.hidl_d) {
        case hidl_discriminator::noinit: {
            noinit(std::move(other.hidl_u.noinit));
            break;
        }
        case hidl_discriminator::value: {
            value(std::move(other.hidl_u.value));
            break;
        }
        default: {
            ::android::hardware::details::logAlwaysFatal((
                    "Unknown union discriminator (value: " +
                    std::to_string((uint8_t) other.hidl_d) + ").").c_str());
        }
    }
    return *this;
}

::android::hardware::radio::V1_6::OptionalOsAppId& (::android::hardware::radio::V1_6::OptionalOsAppId::operator=)(const OptionalOsAppId& other) {
    if (this == &other) { return *this; }

    switch (other.hidl_d) {
        case hidl_discriminator::noinit: {
            noinit(other.hidl_u.noinit);
            break;
        }
        case hidl_discriminator::value: {
            value(other.hidl_u.value);
            break;
        }
        default: {
            ::android::hardware::details::logAlwaysFatal((
                    "Unknown union discriminator (value: " +
                    std::to_string((uint8_t) other.hidl_d) + ").").c_str());
        }
    }
    return *this;
}

void ::android::hardware::radio::V1_6::OptionalOsAppId::hidl_destructUnion() {
    switch (hidl_d) {
        case hidl_discriminator::noinit: {
            ::android::hardware::details::destructElement(&(hidl_u.noinit));
            break;
        }
        case hidl_discriminator::value: {
            ::android::hardware::details::destructElement(&(hidl_u.value));
            break;
        }
        default: {
            ::android::hardware::details::logAlwaysFatal((
                    "Unknown union discriminator (value: " +
                    std::to_string((uint8_t) hidl_d) + ").").c_str());
        }
    }

}

void ::android::hardware::radio::V1_6::OptionalOsAppId::noinit(const ::android::hidl::safe_union::V1_0::Monostate& o) {
    if (hidl_d != hidl_discriminator::noinit) {
        hidl_destructUnion();
        ::std::memset(&hidl_u, 0, sizeof(hidl_u));

        new (&hidl_u.noinit) ::android::hidl::safe_union::V1_0::Monostate(o);
        hidl_d = hidl_discriminator::noinit;
    }
    else if (&(hidl_u.noinit) != &o) {
        hidl_u.noinit = o;
    }
}

void ::android::hardware::radio::V1_6::OptionalOsAppId::noinit(::android::hidl::safe_union::V1_0::Monostate&& o) {
    if (hidl_d != hidl_discriminator::noinit) {
        hidl_destructUnion();
        ::std::memset(&hidl_u, 0, sizeof(hidl_u));

        new (&hidl_u.noinit) ::android::hidl::safe_union::V1_0::Monostate(std::move(o));
        hidl_d = hidl_discriminator::noinit;
    }
    else if (&(hidl_u.noinit) != &o) {
        hidl_u.noinit = std::move(o);
    }
}

::android::hidl::safe_union::V1_0::Monostate& (::android::hardware::radio::V1_6::OptionalOsAppId::noinit)() {
    if (CC_UNLIKELY(hidl_d != hidl_discriminator::noinit)) {
        LOG_ALWAYS_FATAL("Bad safe_union access: safe_union has discriminator %" PRIu64 " but discriminator %" PRIu64 " was accessed.",
                static_cast<uint64_t>(hidl_d), static_cast<uint64_t>(hidl_discriminator::noinit));
    }

    return hidl_u.noinit;
}

const ::android::hidl::safe_union::V1_0::Monostate& (::android::hardware::radio::V1_6::OptionalOsAppId::noinit)() const {
    if (CC_UNLIKELY(hidl_d != hidl_discriminator::noinit)) {
        LOG_ALWAYS_FATAL("Bad safe_union access: safe_union has discriminator %" PRIu64 " but discriminator %" PRIu64 " was accessed.",
                static_cast<uint64_t>(hidl_d), static_cast<uint64_t>(hidl_discriminator::noinit));
    }

    return hidl_u.noinit;
}

void ::android::hardware::radio::V1_6::OptionalOsAppId::value(const ::android::hardware::radio::V1_6::OsAppId& o) {
    if (hidl_d != hidl_discriminator::value) {
        hidl_destructUnion();
        ::std::memset(&hidl_u, 0, sizeof(hidl_u));

        new (&hidl_u.value) ::android::hardware::radio::V1_6::OsAppId(o);
        hidl_d = hidl_discriminator::value;
    }
    else if (&(hidl_u.value) != &o) {
        hidl_u.value = o;
    }
}

void ::android::hardware::radio::V1_6::OptionalOsAppId::value(::android::hardware::radio::V1_6::OsAppId&& o) {
    if (hidl_d != hidl_discriminator::value) {
        hidl_destructUnion();
        ::std::memset(&hidl_u, 0, sizeof(hidl_u));

        new (&hidl_u.value) ::android::hardware::radio::V1_6::OsAppId(std::move(o));
        hidl_d = hidl_discriminator::value;
    }
    else if (&(hidl_u.value) != &o) {
        hidl_u.value = std::move(o);
    }
}

::android::hardware::radio::V1_6::OsAppId& (::android::hardware::radio::V1_6::OptionalOsAppId::value)() {
    if (CC_UNLIKELY(hidl_d != hidl_discriminator::value)) {
        LOG_ALWAYS_FATAL("Bad safe_union access: safe_union has discriminator %" PRIu64 " but discriminator %" PRIu64 " was accessed.",
                static_cast<uint64_t>(hidl_d), static_cast<uint64_t>(hidl_discriminator::value));
    }

    return hidl_u.value;
}

const ::android::hardware::radio::V1_6::OsAppId& (::android::hardware::radio::V1_6::OptionalOsAppId::value)() const {
    if (CC_UNLIKELY(hidl_d != hidl_discriminator::value)) {
        LOG_ALWAYS_FATAL("Bad safe_union access: safe_union has discriminator %" PRIu64 " but discriminator %" PRIu64 " was accessed.",
                static_cast<uint64_t>(hidl_d), static_cast<uint64_t>(hidl_discriminator::value));
    }

    return hidl_u.value;
}

::android::hardware::radio::V1_6::OptionalOsAppId::hidl_union::hidl_union() {}

::android::hardware::radio::V1_6::OptionalOsAppId::hidl_union::~hidl_union() {}

::android::hardware::radio::V1_6::OptionalOsAppId::hidl_discriminator (::android::hardware::radio::V1_6::OptionalOsAppId::getDiscriminator)() const {
    return hidl_d;
}

::android::hardware::radio::V1_6::OptionalTrafficDescriptor::OptionalTrafficDescriptor() {
    static_assert(offsetof(::android::hardware::radio::V1_6::OptionalTrafficDescriptor, hidl_d) == 0, "wrong offset");
    static_assert(offsetof(::android::hardware::radio::V1_6::OptionalTrafficDescriptor, hidl_u) == 8, "wrong offset");

    ::std::memset(&hidl_u, 0, sizeof(hidl_u));
    ::std::memset(reinterpret_cast<uint8_t*>(this) + 1, 0, 7);
    // no padding to zero starting at offset 56

    hidl_d = hidl_discriminator::noinit;
    new (&hidl_u.noinit) ::android::hidl::safe_union::V1_0::Monostate();
}

::android::hardware::radio::V1_6::OptionalTrafficDescriptor::~OptionalTrafficDescriptor() {
    hidl_destructUnion();
}

::android::hardware::radio::V1_6::OptionalTrafficDescriptor::OptionalTrafficDescriptor(OptionalTrafficDescriptor&& other) : ::android::hardware::radio::V1_6::OptionalTrafficDescriptor() {
    *this = std::move(other);
}

::android::hardware::radio::V1_6::OptionalTrafficDescriptor::OptionalTrafficDescriptor(const OptionalTrafficDescriptor& other) : ::android::hardware::radio::V1_6::OptionalTrafficDescriptor() {
    *this = other;
}

::android::hardware::radio::V1_6::OptionalTrafficDescriptor& (::android::hardware::radio::V1_6::OptionalTrafficDescriptor::operator=)(OptionalTrafficDescriptor&& other) {
    if (this == &other) { return *this; }

    switch (other.hidl_d) {
        case hidl_discriminator::noinit: {
            noinit(std::move(other.hidl_u.noinit));
            break;
        }
        case hidl_discriminator::value: {
            value(std::move(other.hidl_u.value));
            break;
        }
        default: {
            ::android::hardware::details::logAlwaysFatal((
                    "Unknown union discriminator (value: " +
                    std::to_string((uint8_t) other.hidl_d) + ").").c_str());
        }
    }
    return *this;
}

::android::hardware::radio::V1_6::OptionalTrafficDescriptor& (::android::hardware::radio::V1_6::OptionalTrafficDescriptor::operator=)(const OptionalTrafficDescriptor& other) {
    if (this == &other) { return *this; }

    switch (other.hidl_d) {
        case hidl_discriminator::noinit: {
            noinit(other.hidl_u.noinit);
            break;
        }
        case hidl_discriminator::value: {
            value(other.hidl_u.value);
            break;
        }
        default: {
            ::android::hardware::details::logAlwaysFatal((
                    "Unknown union discriminator (value: " +
                    std::to_string((uint8_t) other.hidl_d) + ").").c_str());
        }
    }
    return *this;
}

void ::android::hardware::radio::V1_6::OptionalTrafficDescriptor::hidl_destructUnion() {
    switch (hidl_d) {
        case hidl_discriminator::noinit: {
            ::android::hardware::details::destructElement(&(hidl_u.noinit));
            break;
        }
        case hidl_discriminator::value: {
            ::android::hardware::details::destructElement(&(hidl_u.value));
            break;
        }
        default: {
            ::android::hardware::details::logAlwaysFatal((
                    "Unknown union discriminator (value: " +
                    std::to_string((uint8_t) hidl_d) + ").").c_str());
        }
    }

}

void ::android::hardware::radio::V1_6::OptionalTrafficDescriptor::noinit(const ::android::hidl::safe_union::V1_0::Monostate& o) {
    if (hidl_d != hidl_discriminator::noinit) {
        hidl_destructUnion();
        ::std::memset(&hidl_u, 0, sizeof(hidl_u));

        new (&hidl_u.noinit) ::android::hidl::safe_union::V1_0::Monostate(o);
        hidl_d = hidl_discriminator::noinit;
    }
    else if (&(hidl_u.noinit) != &o) {
        hidl_u.noinit = o;
    }
}

void ::android::hardware::radio::V1_6::OptionalTrafficDescriptor::noinit(::android::hidl::safe_union::V1_0::Monostate&& o) {
    if (hidl_d != hidl_discriminator::noinit) {
        hidl_destructUnion();
        ::std::memset(&hidl_u, 0, sizeof(hidl_u));

        new (&hidl_u.noinit) ::android::hidl::safe_union::V1_0::Monostate(std::move(o));
        hidl_d = hidl_discriminator::noinit;
    }
    else if (&(hidl_u.noinit) != &o) {
        hidl_u.noinit = std::move(o);
    }
}

::android::hidl::safe_union::V1_0::Monostate& (::android::hardware::radio::V1_6::OptionalTrafficDescriptor::noinit)() {
    if (CC_UNLIKELY(hidl_d != hidl_discriminator::noinit)) {
        LOG_ALWAYS_FATAL("Bad safe_union access: safe_union has discriminator %" PRIu64 " but discriminator %" PRIu64 " was accessed.",
                static_cast<uint64_t>(hidl_d), static_cast<uint64_t>(hidl_discriminator::noinit));
    }

    return hidl_u.noinit;
}

const ::android::hidl::safe_union::V1_0::Monostate& (::android::hardware::radio::V1_6::OptionalTrafficDescriptor::noinit)() const {
    if (CC_UNLIKELY(hidl_d != hidl_discriminator::noinit)) {
        LOG_ALWAYS_FATAL("Bad safe_union access: safe_union has discriminator %" PRIu64 " but discriminator %" PRIu64 " was accessed.",
                static_cast<uint64_t>(hidl_d), static_cast<uint64_t>(hidl_discriminator::noinit));
    }

    return hidl_u.noinit;
}

void ::android::hardware::radio::V1_6::OptionalTrafficDescriptor::value(const ::android::hardware::radio::V1_6::TrafficDescriptor& o) {
    if (hidl_d != hidl_discriminator::value) {
        hidl_destructUnion();
        ::std::memset(&hidl_u, 0, sizeof(hidl_u));

        new (&hidl_u.value) ::android::hardware::radio::V1_6::TrafficDescriptor(o);
        hidl_d = hidl_discriminator::value;
    }
    else if (&(hidl_u.value) != &o) {
        hidl_u.value = o;
    }
}

void ::android::hardware::radio::V1_6::OptionalTrafficDescriptor::value(::android::hardware::radio::V1_6::TrafficDescriptor&& o) {
    if (hidl_d != hidl_discriminator::value) {
        hidl_destructUnion();
        ::std::memset(&hidl_u, 0, sizeof(hidl_u));

        new (&hidl_u.value) ::android::hardware::radio::V1_6::TrafficDescriptor(std::move(o));
        hidl_d = hidl_discriminator::value;
    }
    else if (&(hidl_u.value) != &o) {
        hidl_u.value = std::move(o);
    }
}

::android::hardware::radio::V1_6::TrafficDescriptor& (::android::hardware::radio::V1_6::OptionalTrafficDescriptor::value)() {
    if (CC_UNLIKELY(hidl_d != hidl_discriminator::value)) {
        LOG_ALWAYS_FATAL("Bad safe_union access: safe_union has discriminator %" PRIu64 " but discriminator %" PRIu64 " was accessed.",
                static_cast<uint64_t>(hidl_d), static_cast<uint64_t>(hidl_discriminator::value));
    }

    return hidl_u.value;
}

const ::android::hardware::radio::V1_6::TrafficDescriptor& (::android::hardware::radio::V1_6::OptionalTrafficDescriptor::value)() const {
    if (CC_UNLIKELY(hidl_d != hidl_discriminator::value)) {
        LOG_ALWAYS_FATAL("Bad safe_union access: safe_union has discriminator %" PRIu64 " but discriminator %" PRIu64 " was accessed.",
                static_cast<uint64_t>(hidl_d), static_cast<uint64_t>(hidl_discriminator::value));
    }

    return hidl_u.value;
}

::android::hardware::radio::V1_6::OptionalTrafficDescriptor::hidl_union::hidl_union() {}

::android::hardware::radio::V1_6::OptionalTrafficDescriptor::hidl_union::~hidl_union() {}

::android::hardware::radio::V1_6::OptionalTrafficDescriptor::hidl_discriminator (::android::hardware::radio::V1_6::OptionalTrafficDescriptor::getDiscriminator)() const {
    return hidl_d;
}

::android::hardware::radio::V1_6::OptionalPdpProtocolType::OptionalPdpProtocolType() {
    static_assert(offsetof(::android::hardware::radio::V1_6::OptionalPdpProtocolType, hidl_d) == 0, "wrong offset");
    static_assert(offsetof(::android::hardware::radio::V1_6::OptionalPdpProtocolType, hidl_u) == 4, "wrong offset");

    ::std::memset(&hidl_u, 0, sizeof(hidl_u));
    ::std::memset(reinterpret_cast<uint8_t*>(this) + 1, 0, 3);
    // no padding to zero starting at offset 8

    hidl_d = hidl_discriminator::noinit;
    new (&hidl_u.noinit) ::android::hidl::safe_union::V1_0::Monostate();
}

::android::hardware::radio::V1_6::OptionalPdpProtocolType::~OptionalPdpProtocolType() {
    hidl_destructUnion();
}

::android::hardware::radio::V1_6::OptionalPdpProtocolType::OptionalPdpProtocolType(OptionalPdpProtocolType&& other) : ::android::hardware::radio::V1_6::OptionalPdpProtocolType() {
    *this = std::move(other);
}

::android::hardware::radio::V1_6::OptionalPdpProtocolType::OptionalPdpProtocolType(const OptionalPdpProtocolType& other) : ::android::hardware::radio::V1_6::OptionalPdpProtocolType() {
    *this = other;
}

::android::hardware::radio::V1_6::OptionalPdpProtocolType& (::android::hardware::radio::V1_6::OptionalPdpProtocolType::operator=)(OptionalPdpProtocolType&& other) {
    if (this == &other) { return *this; }

    switch (other.hidl_d) {
        case hidl_discriminator::noinit: {
            noinit(std::move(other.hidl_u.noinit));
            break;
        }
        case hidl_discriminator::value: {
            value(std::move(other.hidl_u.value));
            break;
        }
        default: {
            ::android::hardware::details::logAlwaysFatal((
                    "Unknown union discriminator (value: " +
                    std::to_string((uint8_t) other.hidl_d) + ").").c_str());
        }
    }
    return *this;
}

::android::hardware::radio::V1_6::OptionalPdpProtocolType& (::android::hardware::radio::V1_6::OptionalPdpProtocolType::operator=)(const OptionalPdpProtocolType& other) {
    if (this == &other) { return *this; }

    switch (other.hidl_d) {
        case hidl_discriminator::noinit: {
            noinit(other.hidl_u.noinit);
            break;
        }
        case hidl_discriminator::value: {
            value(other.hidl_u.value);
            break;
        }
        default: {
            ::android::hardware::details::logAlwaysFatal((
                    "Unknown union discriminator (value: " +
                    std::to_string((uint8_t) other.hidl_d) + ").").c_str());
        }
    }
    return *this;
}

void ::android::hardware::radio::V1_6::OptionalPdpProtocolType::hidl_destructUnion() {
    switch (hidl_d) {
        case hidl_discriminator::noinit: {
            ::android::hardware::details::destructElement(&(hidl_u.noinit));
            break;
        }
        case hidl_discriminator::value: {
            ::android::hardware::details::destructElement(&(hidl_u.value));
            break;
        }
        default: {
            ::android::hardware::details::logAlwaysFatal((
                    "Unknown union discriminator (value: " +
                    std::to_string((uint8_t) hidl_d) + ").").c_str());
        }
    }

}

void ::android::hardware::radio::V1_6::OptionalPdpProtocolType::noinit(const ::android::hidl::safe_union::V1_0::Monostate& o) {
    if (hidl_d != hidl_discriminator::noinit) {
        hidl_destructUnion();
        ::std::memset(&hidl_u, 0, sizeof(hidl_u));

        new (&hidl_u.noinit) ::android::hidl::safe_union::V1_0::Monostate(o);
        hidl_d = hidl_discriminator::noinit;
    }
    else if (&(hidl_u.noinit) != &o) {
        hidl_u.noinit = o;
    }
}

void ::android::hardware::radio::V1_6::OptionalPdpProtocolType::noinit(::android::hidl::safe_union::V1_0::Monostate&& o) {
    if (hidl_d != hidl_discriminator::noinit) {
        hidl_destructUnion();
        ::std::memset(&hidl_u, 0, sizeof(hidl_u));

        new (&hidl_u.noinit) ::android::hidl::safe_union::V1_0::Monostate(std::move(o));
        hidl_d = hidl_discriminator::noinit;
    }
    else if (&(hidl_u.noinit) != &o) {
        hidl_u.noinit = std::move(o);
    }
}

::android::hidl::safe_union::V1_0::Monostate& (::android::hardware::radio::V1_6::OptionalPdpProtocolType::noinit)() {
    if (CC_UNLIKELY(hidl_d != hidl_discriminator::noinit)) {
        LOG_ALWAYS_FATAL("Bad safe_union access: safe_union has discriminator %" PRIu64 " but discriminator %" PRIu64 " was accessed.",
                static_cast<uint64_t>(hidl_d), static_cast<uint64_t>(hidl_discriminator::noinit));
    }

    return hidl_u.noinit;
}

const ::android::hidl::safe_union::V1_0::Monostate& (::android::hardware::radio::V1_6::OptionalPdpProtocolType::noinit)() const {
    if (CC_UNLIKELY(hidl_d != hidl_discriminator::noinit)) {
        LOG_ALWAYS_FATAL("Bad safe_union access: safe_union has discriminator %" PRIu64 " but discriminator %" PRIu64 " was accessed.",
                static_cast<uint64_t>(hidl_d), static_cast<uint64_t>(hidl_discriminator::noinit));
    }

    return hidl_u.noinit;
}

void ::android::hardware::radio::V1_6::OptionalPdpProtocolType::value(::android::hardware::radio::V1_4::PdpProtocolType o) {
    if (hidl_d != hidl_discriminator::value) {
        hidl_destructUnion();
        ::std::memset(&hidl_u, 0, sizeof(hidl_u));

        new (&hidl_u.value) ::android::hardware::radio::V1_4::PdpProtocolType(o);
        hidl_d = hidl_discriminator::value;
    }
    else if (&(hidl_u.value) != &o) {
        hidl_u.value = o;
    }
}

::android::hardware::radio::V1_4::PdpProtocolType& (::android::hardware::radio::V1_6::OptionalPdpProtocolType::value)() {
    if (CC_UNLIKELY(hidl_d != hidl_discriminator::value)) {
        LOG_ALWAYS_FATAL("Bad safe_union access: safe_union has discriminator %" PRIu64 " but discriminator %" PRIu64 " was accessed.",
                static_cast<uint64_t>(hidl_d), static_cast<uint64_t>(hidl_discriminator::value));
    }

    return hidl_u.value;
}

::android::hardware::radio::V1_4::PdpProtocolType (::android::hardware::radio::V1_6::OptionalPdpProtocolType::value)() const {
    if (CC_UNLIKELY(hidl_d != hidl_discriminator::value)) {
        LOG_ALWAYS_FATAL("Bad safe_union access: safe_union has discriminator %" PRIu64 " but discriminator %" PRIu64 " was accessed.",
                static_cast<uint64_t>(hidl_d), static_cast<uint64_t>(hidl_discriminator::value));
    }

    return hidl_u.value;
}

::android::hardware::radio::V1_6::OptionalPdpProtocolType::hidl_union::hidl_union() {}

::android::hardware::radio::V1_6::OptionalPdpProtocolType::hidl_union::~hidl_union() {}

::android::hardware::radio::V1_6::OptionalPdpProtocolType::hidl_discriminator (::android::hardware::radio::V1_6::OptionalPdpProtocolType::getDiscriminator)() const {
    return hidl_d;
}

::android::hardware::radio::V1_6::OptionalSscMode::OptionalSscMode() {
    static_assert(offsetof(::android::hardware::radio::V1_6::OptionalSscMode, hidl_d) == 0, "wrong offset");
    static_assert(offsetof(::android::hardware::radio::V1_6::OptionalSscMode, hidl_u) == 1, "wrong offset");

    ::std::memset(&hidl_u, 0, sizeof(hidl_u));
    // no padding to zero starting at offset 1
    // no padding to zero starting at offset 2

    hidl_d = hidl_discriminator::noinit;
    new (&hidl_u.noinit) ::android::hidl::safe_union::V1_0::Monostate();
}

::android::hardware::radio::V1_6::OptionalSscMode::~OptionalSscMode() {
    hidl_destructUnion();
}

::android::hardware::radio::V1_6::OptionalSscMode::OptionalSscMode(OptionalSscMode&& other) : ::android::hardware::radio::V1_6::OptionalSscMode() {
    *this = std::move(other);
}

::android::hardware::radio::V1_6::OptionalSscMode::OptionalSscMode(const OptionalSscMode& other) : ::android::hardware::radio::V1_6::OptionalSscMode() {
    *this = other;
}

::android::hardware::radio::V1_6::OptionalSscMode& (::android::hardware::radio::V1_6::OptionalSscMode::operator=)(OptionalSscMode&& other) {
    if (this == &other) { return *this; }

    switch (other.hidl_d) {
        case hidl_discriminator::noinit: {
            noinit(std::move(other.hidl_u.noinit));
            break;
        }
        case hidl_discriminator::value: {
            value(std::move(other.hidl_u.value));
            break;
        }
        default: {
            ::android::hardware::details::logAlwaysFatal((
                    "Unknown union discriminator (value: " +
                    std::to_string((uint8_t) other.hidl_d) + ").").c_str());
        }
    }
    return *this;
}

::android::hardware::radio::V1_6::OptionalSscMode& (::android::hardware::radio::V1_6::OptionalSscMode::operator=)(const OptionalSscMode& other) {
    if (this == &other) { return *this; }

    switch (other.hidl_d) {
        case hidl_discriminator::noinit: {
            noinit(other.hidl_u.noinit);
            break;
        }
        case hidl_discriminator::value: {
            value(other.hidl_u.value);
            break;
        }
        default: {
            ::android::hardware::details::logAlwaysFatal((
                    "Unknown union discriminator (value: " +
                    std::to_string((uint8_t) other.hidl_d) + ").").c_str());
        }
    }
    return *this;
}

void ::android::hardware::radio::V1_6::OptionalSscMode::hidl_destructUnion() {
    switch (hidl_d) {
        case hidl_discriminator::noinit: {
            ::android::hardware::details::destructElement(&(hidl_u.noinit));
            break;
        }
        case hidl_discriminator::value: {
            ::android::hardware::details::destructElement(&(hidl_u.value));
            break;
        }
        default: {
            ::android::hardware::details::logAlwaysFatal((
                    "Unknown union discriminator (value: " +
                    std::to_string((uint8_t) hidl_d) + ").").c_str());
        }
    }

}

void ::android::hardware::radio::V1_6::OptionalSscMode::noinit(const ::android::hidl::safe_union::V1_0::Monostate& o) {
    if (hidl_d != hidl_discriminator::noinit) {
        hidl_destructUnion();
        ::std::memset(&hidl_u, 0, sizeof(hidl_u));

        new (&hidl_u.noinit) ::android::hidl::safe_union::V1_0::Monostate(o);
        hidl_d = hidl_discriminator::noinit;
    }
    else if (&(hidl_u.noinit) != &o) {
        hidl_u.noinit = o;
    }
}

void ::android::hardware::radio::V1_6::OptionalSscMode::noinit(::android::hidl::safe_union::V1_0::Monostate&& o) {
    if (hidl_d != hidl_discriminator::noinit) {
        hidl_destructUnion();
        ::std::memset(&hidl_u, 0, sizeof(hidl_u));

        new (&hidl_u.noinit) ::android::hidl::safe_union::V1_0::Monostate(std::move(o));
        hidl_d = hidl_discriminator::noinit;
    }
    else if (&(hidl_u.noinit) != &o) {
        hidl_u.noinit = std::move(o);
    }
}

::android::hidl::safe_union::V1_0::Monostate& (::android::hardware::radio::V1_6::OptionalSscMode::noinit)() {
    if (CC_UNLIKELY(hidl_d != hidl_discriminator::noinit)) {
        LOG_ALWAYS_FATAL("Bad safe_union access: safe_union has discriminator %" PRIu64 " but discriminator %" PRIu64 " was accessed.",
                static_cast<uint64_t>(hidl_d), static_cast<uint64_t>(hidl_discriminator::noinit));
    }

    return hidl_u.noinit;
}

const ::android::hidl::safe_union::V1_0::Monostate& (::android::hardware::radio::V1_6::OptionalSscMode::noinit)() const {
    if (CC_UNLIKELY(hidl_d != hidl_discriminator::noinit)) {
        LOG_ALWAYS_FATAL("Bad safe_union access: safe_union has discriminator %" PRIu64 " but discriminator %" PRIu64 " was accessed.",
                static_cast<uint64_t>(hidl_d), static_cast<uint64_t>(hidl_discriminator::noinit));
    }

    return hidl_u.noinit;
}

void ::android::hardware::radio::V1_6::OptionalSscMode::value(::android::hardware::radio::V1_6::SscMode o) {
    if (hidl_d != hidl_discriminator::value) {
        hidl_destructUnion();
        ::std::memset(&hidl_u, 0, sizeof(hidl_u));

        new (&hidl_u.value) ::android::hardware::radio::V1_6::SscMode(o);
        hidl_d = hidl_discriminator::value;
    }
    else if (&(hidl_u.value) != &o) {
        hidl_u.value = o;
    }
}

::android::hardware::radio::V1_6::SscMode& (::android::hardware::radio::V1_6::OptionalSscMode::value)() {
    if (CC_UNLIKELY(hidl_d != hidl_discriminator::value)) {
        LOG_ALWAYS_FATAL("Bad safe_union access: safe_union has discriminator %" PRIu64 " but discriminator %" PRIu64 " was accessed.",
                static_cast<uint64_t>(hidl_d), static_cast<uint64_t>(hidl_discriminator::value));
    }

    return hidl_u.value;
}

::android::hardware::radio::V1_6::SscMode (::android::hardware::radio::V1_6::OptionalSscMode::value)() const {
    if (CC_UNLIKELY(hidl_d != hidl_discriminator::value)) {
        LOG_ALWAYS_FATAL("Bad safe_union access: safe_union has discriminator %" PRIu64 " but discriminator %" PRIu64 " was accessed.",
                static_cast<uint64_t>(hidl_d), static_cast<uint64_t>(hidl_discriminator::value));
    }

    return hidl_u.value;
}

::android::hardware::radio::V1_6::OptionalSscMode::hidl_union::hidl_union() {}

::android::hardware::radio::V1_6::OptionalSscMode::hidl_union::~hidl_union() {}

::android::hardware::radio::V1_6::OptionalSscMode::hidl_discriminator (::android::hardware::radio::V1_6::OptionalSscMode::getDiscriminator)() const {
    return hidl_d;
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
