#ifndef HIDL_GENERATED_ANDROID_HARDWARE_RADIO_V1_6_TYPES_H
#define HIDL_GENERATED_ANDROID_HARDWARE_RADIO_V1_6_TYPES_H

#include <android/hardware/radio/1.0/types.h>
#include <android/hardware/radio/1.1/types.h>
#include <android/hardware/radio/1.2/types.h>
#include <android/hardware/radio/1.4/types.h>
#include <android/hardware/radio/1.5/types.h>
#include <android/hidl/safe_union/1.0/types.h>

#include <hidl/HidlSupport.h>
#include <hidl/MQDescriptor.h>
#include <utils/NativeHandle.h>
#include <utils/misc.h>

namespace android {
namespace hardware {
namespace radio {
namespace V1_6 {

// Forward declaration for forward reference support:
struct QosBandwidth;
struct EpsQos;
struct NrQos;
enum class QosFlowIdRange : uint8_t;
struct Qos;
enum class QosProtocol : int8_t;
enum class QosFilterDirection : int8_t;
enum class QosPortRange : uint16_t;
enum class RadioError : int32_t;
struct RadioResponseInfo;
struct PortRange;
struct MaybePort;
struct QosFilter;
struct QosSession;
enum class HandoverFailureMode : int8_t;
enum class DataCallFailCause : int32_t;
enum class SliceServiceType : uint8_t;
enum class SliceStatus : int8_t;
struct SliceInfo;
struct OptionalSliceInfo;
struct SetupDataCallResult;
enum class NrDualConnectivityState : int8_t;
struct LinkCapacityEstimate;
enum class DataThrottlingAction : int8_t;
enum class VopsIndicator : uint8_t;
enum class EmcIndicator : uint8_t;
enum class EmfIndicator : uint8_t;
struct NrVopsInfo;
struct LteSignalStrength;
struct NrSignalStrength;
struct SignalStrength;
struct CellInfoLte;
struct CellInfoNr;
struct CellInfo;
struct NetworkScanResult;
struct RegStateResult;
struct Call;
enum class NgranBands : int32_t;
struct PhysicalChannelConfig;
struct OptionalDnn;
struct OsAppId;
struct OptionalOsAppId;
struct TrafficDescriptor;
struct OptionalTrafficDescriptor;
struct SlicingConfig;
struct UrspRule;
struct OptionalPdpProtocolType;
enum class SscMode : int8_t;
struct OptionalSscMode;
struct RouteSelectionDescriptor;
enum class PublicKeyType : int8_t;
struct ImsiEncryptionInfo;

// Order of inner types was changed for forward reference support.
struct PhonebookCapacity;
struct PhonebookRecordInfo;
enum class PbReceivedStatus : int8_t;

struct QosBandwidth final {
    /**
     * Maximum bit rate possible on the bearer
     */
    uint32_t maxBitrateKbps __attribute__ ((aligned(4)));
    /**
     * Minimum bit rate that is guaranteed to be provided by the network
     */
    uint32_t guaranteedBitrateKbps __attribute__ ((aligned(4)));
};

static_assert(offsetof(::android::hardware::radio::V1_6::QosBandwidth, maxBitrateKbps) == 0, "wrong offset");
static_assert(offsetof(::android::hardware::radio::V1_6::QosBandwidth, guaranteedBitrateKbps) == 4, "wrong offset");
static_assert(sizeof(::android::hardware::radio::V1_6::QosBandwidth) == 8, "wrong size");
static_assert(__alignof(::android::hardware::radio::V1_6::QosBandwidth) == 4, "wrong alignment");

/**
 * LTE/EPS Quality of Service parameters as per 3gpp spec 24.301 sec 9.9.4.3.
 */
struct EpsQos final {
    /**
     * Quality of Service Class Identifier (QCI), see 3GPP TS 23.203 and 29.212.
     * The allowed values are standard values(1-9, 65-68, 69-70, 75, 79-80, 82-85)
     * defined in the spec and operator specific values in the range 128-254.
     */
    uint16_t qci __attribute__ ((aligned(2)));
    ::android::hardware::radio::V1_6::QosBandwidth downlink __attribute__ ((aligned(4)));
    ::android::hardware::radio::V1_6::QosBandwidth uplink __attribute__ ((aligned(4)));
};

static_assert(offsetof(::android::hardware::radio::V1_6::EpsQos, qci) == 0, "wrong offset");
static_assert(offsetof(::android::hardware::radio::V1_6::EpsQos, downlink) == 4, "wrong offset");
static_assert(offsetof(::android::hardware::radio::V1_6::EpsQos, uplink) == 12, "wrong offset");
static_assert(sizeof(::android::hardware::radio::V1_6::EpsQos) == 20, "wrong size");
static_assert(__alignof(::android::hardware::radio::V1_6::EpsQos) == 4, "wrong alignment");

/**
 * 5G Quality of Service parameters as per 3gpp spec 24.501 sec 9.11.4.12
 */
struct NrQos final {
    /**
     * 5G QOS Identifier (5QI), see 3GPP TS 24.501 and 23.501.
     * The allowed values are standard values(1-9, 65-68, 69-70, 75, 79-80, 82-85)
     * defined in the spec and operator specific values in the range 128-254.
     */
    uint16_t fiveQi __attribute__ ((aligned(2)));
    ::android::hardware::radio::V1_6::QosBandwidth downlink __attribute__ ((aligned(4)));
    ::android::hardware::radio::V1_6::QosBandwidth uplink __attribute__ ((aligned(4)));
    /**
     * QOS flow identifier of the QOS flow description in the
     * range of QosFlowIdRange::MIN to QosFlowIdRange::MAX
     */
    uint8_t qfi __attribute__ ((aligned(1)));
    uint16_t averagingWindowMs __attribute__ ((aligned(2)));
};

static_assert(offsetof(::android::hardware::radio::V1_6::NrQos, fiveQi) == 0, "wrong offset");
static_assert(offsetof(::android::hardware::radio::V1_6::NrQos, downlink) == 4, "wrong offset");
static_assert(offsetof(::android::hardware::radio::V1_6::NrQos, uplink) == 12, "wrong offset");
static_assert(offsetof(::android::hardware::radio::V1_6::NrQos, qfi) == 20, "wrong offset");
static_assert(offsetof(::android::hardware::radio::V1_6::NrQos, averagingWindowMs) == 22, "wrong offset");
static_assert(sizeof(::android::hardware::radio::V1_6::NrQos) == 24, "wrong size");
static_assert(__alignof(::android::hardware::radio::V1_6::NrQos) == 4, "wrong alignment");

/**
 * Allowed values for 5G QOS flow identifier
 */
enum class QosFlowIdRange : uint8_t {
    MIN = 1,
    MAX = 63,
};

/**
 * EPS or NR QOS parameters
 */
struct Qos final {
    enum class hidl_discriminator : uint8_t {
        noinit = 0,  // ::android::hidl::safe_union::V1_0::Monostate
        eps = 1,  // ::android::hardware::radio::V1_6::EpsQos
        nr = 2,  // ::android::hardware::radio::V1_6::NrQos
    };

    Qos();
    ~Qos();
    Qos(Qos&&);
    Qos(const Qos&);
    Qos& operator=(Qos&&);
    Qos& operator=(const Qos&);

    void noinit(const ::android::hidl::safe_union::V1_0::Monostate&);
    void noinit(::android::hidl::safe_union::V1_0::Monostate&&);
    ::android::hidl::safe_union::V1_0::Monostate& noinit();
    const ::android::hidl::safe_union::V1_0::Monostate& noinit() const;

    void eps(const ::android::hardware::radio::V1_6::EpsQos&);
    void eps(::android::hardware::radio::V1_6::EpsQos&&);
    ::android::hardware::radio::V1_6::EpsQos& eps();
    const ::android::hardware::radio::V1_6::EpsQos& eps() const;

    void nr(const ::android::hardware::radio::V1_6::NrQos&);
    void nr(::android::hardware::radio::V1_6::NrQos&&);
    ::android::hardware::radio::V1_6::NrQos& nr();
    const ::android::hardware::radio::V1_6::NrQos& nr() const;

    // Utility methods
    hidl_discriminator getDiscriminator() const;

    constexpr size_t hidl_getUnionOffset() const {
        return offsetof(::android::hardware::radio::V1_6::Qos, hidl_u);
    }

private:
    void hidl_destructUnion();

    hidl_discriminator hidl_d __attribute__ ((aligned(1))) ;
    union hidl_union final {
        ::android::hidl::safe_union::V1_0::Monostate noinit __attribute__ ((aligned(1)));
        ::android::hardware::radio::V1_6::EpsQos eps __attribute__ ((aligned(4)));
        ::android::hardware::radio::V1_6::NrQos nr __attribute__ ((aligned(4)));

        hidl_union();
        ~hidl_union();
    } hidl_u;

    static_assert(sizeof(::android::hardware::radio::V1_6::Qos::hidl_union) == 24, "wrong size");
    static_assert(__alignof(::android::hardware::radio::V1_6::Qos::hidl_union) == 4, "wrong alignment");
    static_assert(sizeof(::android::hardware::radio::V1_6::Qos::hidl_discriminator) == 1, "wrong size");
    static_assert(__alignof(::android::hardware::radio::V1_6::Qos::hidl_discriminator) == 1, "wrong alignment");
};

static_assert(sizeof(::android::hardware::radio::V1_6::Qos) == 28, "wrong size");
static_assert(__alignof(::android::hardware::radio::V1_6::Qos) == 4, "wrong alignment");

/**
 * Next header protocol numbers defined by IANA, RFC 5237
 */
enum class QosProtocol : int8_t {
    /**
     * No protocol specified
     */
    UNSPECIFIED = -1 /* -1 */,
    /**
     * Transmission Control Protocol
     */
    TCP = 6,
    /**
     * User Datagram Protocol
     */
    UDP = 17,
    /**
     * Encapsulating Security Payload Protocol
     */
    ESP = 50,
    /**
     * Authentication Header
     */
    AH = 51,
};

enum class QosFilterDirection : int8_t {
    DOWNLINK = 0,
    UPLINK = 1,
    BIDIRECTIONAL = 2,
};

/**
 * Allowed port numbers
 */
enum class QosPortRange : uint16_t {
    MIN = 20,
    MAX = 65535,
};

enum class RadioError : int32_t {
    NONE = 0,
    RADIO_NOT_AVAILABLE = 1,
    GENERIC_FAILURE = 2,
    PASSWORD_INCORRECT = 3,
    SIM_PIN2 = 4,
    SIM_PUK2 = 5,
    REQUEST_NOT_SUPPORTED = 6,
    CANCELLED = 7,
    OP_NOT_ALLOWED_DURING_VOICE_CALL = 8,
    OP_NOT_ALLOWED_BEFORE_REG_TO_NW = 9,
    SMS_SEND_FAIL_RETRY = 10,
    SIM_ABSENT = 11,
    SUBSCRIPTION_NOT_AVAILABLE = 12,
    MODE_NOT_SUPPORTED = 13,
    FDN_CHECK_FAILURE = 14,
    ILLEGAL_SIM_OR_ME = 15,
    MISSING_RESOURCE = 16,
    NO_SUCH_ELEMENT = 17,
    DIAL_MODIFIED_TO_USSD = 18,
    DIAL_MODIFIED_TO_SS = 19,
    DIAL_MODIFIED_TO_DIAL = 20,
    USSD_MODIFIED_TO_DIAL = 21,
    USSD_MODIFIED_TO_SS = 22,
    USSD_MODIFIED_TO_USSD = 23,
    SS_MODIFIED_TO_DIAL = 24,
    SS_MODIFIED_TO_USSD = 25,
    SUBSCRIPTION_NOT_SUPPORTED = 26,
    SS_MODIFIED_TO_SS = 27,
    LCE_NOT_SUPPORTED = 36,
    NO_MEMORY = 37,
    INTERNAL_ERR = 38,
    SYSTEM_ERR = 39,
    MODEM_ERR = 40,
    INVALID_STATE = 41,
    NO_RESOURCES = 42,
    SIM_ERR = 43,
    INVALID_ARGUMENTS = 44,
    INVALID_SIM_STATE = 45,
    INVALID_MODEM_STATE = 46,
    INVALID_CALL_ID = 47,
    NO_SMS_TO_ACK = 48,
    NETWORK_ERR = 49,
    REQUEST_RATE_LIMITED = 50,
    SIM_BUSY = 51,
    SIM_FULL = 52,
    NETWORK_REJECT = 53,
    OPERATION_NOT_ALLOWED = 54,
    EMPTY_RECORD = 55,
    INVALID_SMS_FORMAT = 56,
    ENCODING_ERR = 57,
    INVALID_SMSC_ADDRESS = 58,
    NO_SUCH_ENTRY = 59,
    NETWORK_NOT_READY = 60,
    NOT_PROVISIONED = 61,
    NO_SUBSCRIPTION = 62,
    NO_NETWORK_FOUND = 63,
    DEVICE_IN_USE = 64,
    ABORTED = 65,
    INVALID_RESPONSE = 66,
    OEM_ERROR_1 = 501,
    OEM_ERROR_2 = 502,
    OEM_ERROR_3 = 503,
    OEM_ERROR_4 = 504,
    OEM_ERROR_5 = 505,
    OEM_ERROR_6 = 506,
    OEM_ERROR_7 = 507,
    OEM_ERROR_8 = 508,
    OEM_ERROR_9 = 509,
    OEM_ERROR_10 = 510,
    OEM_ERROR_11 = 511,
    OEM_ERROR_12 = 512,
    OEM_ERROR_13 = 513,
    OEM_ERROR_14 = 514,
    OEM_ERROR_15 = 515,
    OEM_ERROR_16 = 516,
    OEM_ERROR_17 = 517,
    OEM_ERROR_18 = 518,
    OEM_ERROR_19 = 519,
    OEM_ERROR_20 = 520,
    OEM_ERROR_21 = 521,
    OEM_ERROR_22 = 522,
    OEM_ERROR_23 = 523,
    OEM_ERROR_24 = 524,
    OEM_ERROR_25 = 525,
    /**
     * 1X voice and SMS are not allowed simulteneously.
     */
    SIMULTANEOUS_SMS_AND_CALL_NOT_ALLOWED = 67,
    /**
     * Access is barred.
     */
    ACCESS_BARRED = 68,
    /**
     * SMS is blocked due to call control, e.g., resource unavailable
     * in the SMR entity.
     */
    BLOCKED_DUE_TO_CALL = 69,
    /**
     * Returned from setRadioPowerResponse when detecting RF HW issues. Some RF
     * Front-End(RFFE) components like antenna are considered critical for modem
     * to provide telephony service. This RadioError is used when modem detect
     * such RFFE problem.
     */
    RF_HARDWARE_ISSUE = 70,
    /**
     * Returned from setRadioPowerResponse when detecting no RF calibration
     * issue. Unlike RF_HARDWARE_ISSUE, this is a SW problem and no HW repair is
     * needed.
     */
    NO_RF_CALIBRATION_INFO = 71,
};

/**
 * Overwritten from @1.0::RadioResponseInfo in order to update the RadioError to 1.6 version.
 */
struct RadioResponseInfo final {
    ::android::hardware::radio::V1_0::RadioResponseType type __attribute__ ((aligned(4)));
    int32_t serial __attribute__ ((aligned(4)));
    ::android::hardware::radio::V1_6::RadioError error __attribute__ ((aligned(4)));
};

static_assert(offsetof(::android::hardware::radio::V1_6::RadioResponseInfo, type) == 0, "wrong offset");
static_assert(offsetof(::android::hardware::radio::V1_6::RadioResponseInfo, serial) == 4, "wrong offset");
static_assert(offsetof(::android::hardware::radio::V1_6::RadioResponseInfo, error) == 8, "wrong offset");
static_assert(sizeof(::android::hardware::radio::V1_6::RadioResponseInfo) == 12, "wrong size");
static_assert(__alignof(::android::hardware::radio::V1_6::RadioResponseInfo) == 4, "wrong alignment");

/**
 * Defines range of ports. start and end are the first and last port numbers
 * (inclusive) in the range. Both start and end are in QosPortRange.MIN to
 * QosPortRange.MAX range. A single port shall be represented by the same
 * start and end value.
 */
struct PortRange final {
    int32_t start __attribute__ ((aligned(4)));
    int32_t end __attribute__ ((aligned(4)));
};

static_assert(offsetof(::android::hardware::radio::V1_6::PortRange, start) == 0, "wrong offset");
static_assert(offsetof(::android::hardware::radio::V1_6::PortRange, end) == 4, "wrong offset");
static_assert(sizeof(::android::hardware::radio::V1_6::PortRange) == 8, "wrong size");
static_assert(__alignof(::android::hardware::radio::V1_6::PortRange) == 4, "wrong alignment");

/**
 * Port is optional, contains either single port or range of ports
 */
struct MaybePort final {
    enum class hidl_discriminator : uint8_t {
        noinit = 0,  // ::android::hidl::safe_union::V1_0::Monostate
        range = 1,  // ::android::hardware::radio::V1_6::PortRange
    };

    MaybePort();
    ~MaybePort();
    MaybePort(MaybePort&&);
    MaybePort(const MaybePort&);
    MaybePort& operator=(MaybePort&&);
    MaybePort& operator=(const MaybePort&);

    void noinit(const ::android::hidl::safe_union::V1_0::Monostate&);
    void noinit(::android::hidl::safe_union::V1_0::Monostate&&);
    ::android::hidl::safe_union::V1_0::Monostate& noinit();
    const ::android::hidl::safe_union::V1_0::Monostate& noinit() const;

    void range(const ::android::hardware::radio::V1_6::PortRange&);
    void range(::android::hardware::radio::V1_6::PortRange&&);
    ::android::hardware::radio::V1_6::PortRange& range();
    const ::android::hardware::radio::V1_6::PortRange& range() const;

    // Utility methods
    hidl_discriminator getDiscriminator() const;

    constexpr size_t hidl_getUnionOffset() const {
        return offsetof(::android::hardware::radio::V1_6::MaybePort, hidl_u);
    }

private:
    void hidl_destructUnion();

    hidl_discriminator hidl_d __attribute__ ((aligned(1))) ;
    union hidl_union final {
        ::android::hidl::safe_union::V1_0::Monostate noinit __attribute__ ((aligned(1)));
        ::android::hardware::radio::V1_6::PortRange range __attribute__ ((aligned(4)));

        hidl_union();
        ~hidl_union();
    } hidl_u;

    static_assert(sizeof(::android::hardware::radio::V1_6::MaybePort::hidl_union) == 8, "wrong size");
    static_assert(__alignof(::android::hardware::radio::V1_6::MaybePort::hidl_union) == 4, "wrong alignment");
    static_assert(sizeof(::android::hardware::radio::V1_6::MaybePort::hidl_discriminator) == 1, "wrong size");
    static_assert(__alignof(::android::hardware::radio::V1_6::MaybePort::hidl_discriminator) == 1, "wrong alignment");
};

static_assert(sizeof(::android::hardware::radio::V1_6::MaybePort) == 12, "wrong size");
static_assert(__alignof(::android::hardware::radio::V1_6::MaybePort) == 4, "wrong alignment");

/**
 * See 3gpp 24.008 10.5.6.12 and 3gpp 24.501 9.11.4.13
 */
struct QosFilter final {
    // Forward declaration for forward reference support:
    struct TypeOfService;
    struct Ipv6FlowLabel;
    struct IpsecSpi;

    struct TypeOfService final {
        enum class hidl_discriminator : uint8_t {
            noinit = 0,  // ::android::hidl::safe_union::V1_0::Monostate
            value = 1,  // uint8_t
        };

        TypeOfService();
        ~TypeOfService();
        TypeOfService(TypeOfService&&);
        TypeOfService(const TypeOfService&);
        TypeOfService& operator=(TypeOfService&&);
        TypeOfService& operator=(const TypeOfService&);

        void noinit(const ::android::hidl::safe_union::V1_0::Monostate&);
        void noinit(::android::hidl::safe_union::V1_0::Monostate&&);
        ::android::hidl::safe_union::V1_0::Monostate& noinit();
        const ::android::hidl::safe_union::V1_0::Monostate& noinit() const;

        void value(uint8_t);
        uint8_t& value();
        uint8_t value() const;

        // Utility methods
        hidl_discriminator getDiscriminator() const;

        constexpr size_t hidl_getUnionOffset() const {
            return offsetof(::android::hardware::radio::V1_6::QosFilter::TypeOfService, hidl_u);
        }

    private:
        void hidl_destructUnion();

        hidl_discriminator hidl_d __attribute__ ((aligned(1))) ;
        union hidl_union final {
            ::android::hidl::safe_union::V1_0::Monostate noinit __attribute__ ((aligned(1)));
            uint8_t value __attribute__ ((aligned(1)));

            hidl_union();
            ~hidl_union();
        } hidl_u;

        static_assert(sizeof(::android::hardware::radio::V1_6::QosFilter::TypeOfService::hidl_union) == 1, "wrong size");
        static_assert(__alignof(::android::hardware::radio::V1_6::QosFilter::TypeOfService::hidl_union) == 1, "wrong alignment");
        static_assert(sizeof(::android::hardware::radio::V1_6::QosFilter::TypeOfService::hidl_discriminator) == 1, "wrong size");
        static_assert(__alignof(::android::hardware::radio::V1_6::QosFilter::TypeOfService::hidl_discriminator) == 1, "wrong alignment");
    };

    static_assert(sizeof(::android::hardware::radio::V1_6::QosFilter::TypeOfService) == 2, "wrong size");
    static_assert(__alignof(::android::hardware::radio::V1_6::QosFilter::TypeOfService) == 1, "wrong alignment");

    struct Ipv6FlowLabel final {
        enum class hidl_discriminator : uint8_t {
            noinit = 0,  // ::android::hidl::safe_union::V1_0::Monostate
            value = 1,  // uint32_t
        };

        Ipv6FlowLabel();
        ~Ipv6FlowLabel();
        Ipv6FlowLabel(Ipv6FlowLabel&&);
        Ipv6FlowLabel(const Ipv6FlowLabel&);
        Ipv6FlowLabel& operator=(Ipv6FlowLabel&&);
        Ipv6FlowLabel& operator=(const Ipv6FlowLabel&);

        void noinit(const ::android::hidl::safe_union::V1_0::Monostate&);
        void noinit(::android::hidl::safe_union::V1_0::Monostate&&);
        ::android::hidl::safe_union::V1_0::Monostate& noinit();
        const ::android::hidl::safe_union::V1_0::Monostate& noinit() const;

        void value(uint32_t);
        uint32_t& value();
        uint32_t value() const;

        // Utility methods
        hidl_discriminator getDiscriminator() const;

        constexpr size_t hidl_getUnionOffset() const {
            return offsetof(::android::hardware::radio::V1_6::QosFilter::Ipv6FlowLabel, hidl_u);
        }

    private:
        void hidl_destructUnion();

        hidl_discriminator hidl_d __attribute__ ((aligned(1))) ;
        union hidl_union final {
            ::android::hidl::safe_union::V1_0::Monostate noinit __attribute__ ((aligned(1)));
            uint32_t value __attribute__ ((aligned(4)));

            hidl_union();
            ~hidl_union();
        } hidl_u;

        static_assert(sizeof(::android::hardware::radio::V1_6::QosFilter::Ipv6FlowLabel::hidl_union) == 4, "wrong size");
        static_assert(__alignof(::android::hardware::radio::V1_6::QosFilter::Ipv6FlowLabel::hidl_union) == 4, "wrong alignment");
        static_assert(sizeof(::android::hardware::radio::V1_6::QosFilter::Ipv6FlowLabel::hidl_discriminator) == 1, "wrong size");
        static_assert(__alignof(::android::hardware::radio::V1_6::QosFilter::Ipv6FlowLabel::hidl_discriminator) == 1, "wrong alignment");
    };

    static_assert(sizeof(::android::hardware::radio::V1_6::QosFilter::Ipv6FlowLabel) == 8, "wrong size");
    static_assert(__alignof(::android::hardware::radio::V1_6::QosFilter::Ipv6FlowLabel) == 4, "wrong alignment");

    struct IpsecSpi final {
        enum class hidl_discriminator : uint8_t {
            noinit = 0,  // ::android::hidl::safe_union::V1_0::Monostate
            value = 1,  // uint32_t
        };

        IpsecSpi();
        ~IpsecSpi();
        IpsecSpi(IpsecSpi&&);
        IpsecSpi(const IpsecSpi&);
        IpsecSpi& operator=(IpsecSpi&&);
        IpsecSpi& operator=(const IpsecSpi&);

        void noinit(const ::android::hidl::safe_union::V1_0::Monostate&);
        void noinit(::android::hidl::safe_union::V1_0::Monostate&&);
        ::android::hidl::safe_union::V1_0::Monostate& noinit();
        const ::android::hidl::safe_union::V1_0::Monostate& noinit() const;

        void value(uint32_t);
        uint32_t& value();
        uint32_t value() const;

        // Utility methods
        hidl_discriminator getDiscriminator() const;

        constexpr size_t hidl_getUnionOffset() const {
            return offsetof(::android::hardware::radio::V1_6::QosFilter::IpsecSpi, hidl_u);
        }

    private:
        void hidl_destructUnion();

        hidl_discriminator hidl_d __attribute__ ((aligned(1))) ;
        union hidl_union final {
            ::android::hidl::safe_union::V1_0::Monostate noinit __attribute__ ((aligned(1)));
            uint32_t value __attribute__ ((aligned(4)));

            hidl_union();
            ~hidl_union();
        } hidl_u;

        static_assert(sizeof(::android::hardware::radio::V1_6::QosFilter::IpsecSpi::hidl_union) == 4, "wrong size");
        static_assert(__alignof(::android::hardware::radio::V1_6::QosFilter::IpsecSpi::hidl_union) == 4, "wrong alignment");
        static_assert(sizeof(::android::hardware::radio::V1_6::QosFilter::IpsecSpi::hidl_discriminator) == 1, "wrong size");
        static_assert(__alignof(::android::hardware::radio::V1_6::QosFilter::IpsecSpi::hidl_discriminator) == 1, "wrong alignment");
    };

    static_assert(sizeof(::android::hardware::radio::V1_6::QosFilter::IpsecSpi) == 8, "wrong size");
    static_assert(__alignof(::android::hardware::radio::V1_6::QosFilter::IpsecSpi) == 4, "wrong alignment");

    /**
     * Local and remote IP addresses, typically one IPv4 or one IPv6
     * or one of each. Addresses could be with optional "/" prefix
     * length, e.g.,"192.0.1.3" or "192.0.1.11/16 2001:db8::1/64".
     * If the prefix length is absent the addresses are assumed to be
     * point to point with IPv4 having a prefix length of 32 and
     * IPv6 128.
     */
    ::android::hardware::hidl_vec<::android::hardware::hidl_string> localAddresses __attribute__ ((aligned(8)));
    ::android::hardware::hidl_vec<::android::hardware::hidl_string> remoteAddresses __attribute__ ((aligned(8)));
    /**
     * Local and remote port/ranges
     */
    ::android::hardware::radio::V1_6::MaybePort localPort __attribute__ ((aligned(4)));
    ::android::hardware::radio::V1_6::MaybePort remotePort __attribute__ ((aligned(4)));
    /**
     * QoS protocol
     */
    ::android::hardware::radio::V1_6::QosProtocol protocol __attribute__ ((aligned(1)));
    /**
     * Type of service value or mask as defined in RFC 1349
     */
    ::android::hardware::radio::V1_6::QosFilter::TypeOfService tos __attribute__ ((aligned(1)));
    /**
     * IPv6 flow label as defined in RFC 6437
     */
    ::android::hardware::radio::V1_6::QosFilter::Ipv6FlowLabel flowLabel __attribute__ ((aligned(4)));
    /**
     * IPSec security parameter index
     */
    ::android::hardware::radio::V1_6::QosFilter::IpsecSpi spi __attribute__ ((aligned(4)));
    /**
     * Filter direction
     */
    ::android::hardware::radio::V1_6::QosFilterDirection direction __attribute__ ((aligned(1)));
    /**
     * Specifies the order in which the filter needs to be matched.
     * A lower numerical(positive) value has a higher precedence.
     * Set -1 when unspecified.
     */
    int32_t precedence __attribute__ ((aligned(4)));
};

static_assert(offsetof(::android::hardware::radio::V1_6::QosFilter, localAddresses) == 0, "wrong offset");
static_assert(offsetof(::android::hardware::radio::V1_6::QosFilter, remoteAddresses) == 16, "wrong offset");
static_assert(offsetof(::android::hardware::radio::V1_6::QosFilter, localPort) == 32, "wrong offset");
static_assert(offsetof(::android::hardware::radio::V1_6::QosFilter, remotePort) == 44, "wrong offset");
static_assert(offsetof(::android::hardware::radio::V1_6::QosFilter, protocol) == 56, "wrong offset");
static_assert(offsetof(::android::hardware::radio::V1_6::QosFilter, tos) == 57, "wrong offset");
static_assert(offsetof(::android::hardware::radio::V1_6::QosFilter, flowLabel) == 60, "wrong offset");
static_assert(offsetof(::android::hardware::radio::V1_6::QosFilter, spi) == 68, "wrong offset");
static_assert(offsetof(::android::hardware::radio::V1_6::QosFilter, direction) == 76, "wrong offset");
static_assert(offsetof(::android::hardware::radio::V1_6::QosFilter, precedence) == 80, "wrong offset");
static_assert(sizeof(::android::hardware::radio::V1_6::QosFilter) == 88, "wrong size");
static_assert(__alignof(::android::hardware::radio::V1_6::QosFilter) == 8, "wrong alignment");

/**
 * QOS session associated with a dedicated bearer
 */
struct QosSession final {
    /**
     * Unique ID of the QoS session within the data call
     */
    int32_t qosSessionId __attribute__ ((aligned(4)));
    /**
     * QOS attributes
     */
    ::android::hardware::radio::V1_6::Qos qos __attribute__ ((aligned(4)));
    /**
     * List of QOS filters associated with this session
     */
    ::android::hardware::hidl_vec<::android::hardware::radio::V1_6::QosFilter> qosFilters __attribute__ ((aligned(8)));
};

static_assert(offsetof(::android::hardware::radio::V1_6::QosSession, qosSessionId) == 0, "wrong offset");
static_assert(offsetof(::android::hardware::radio::V1_6::QosSession, qos) == 4, "wrong offset");
static_assert(offsetof(::android::hardware::radio::V1_6::QosSession, qosFilters) == 32, "wrong offset");
static_assert(sizeof(::android::hardware::radio::V1_6::QosSession) == 48, "wrong size");
static_assert(__alignof(::android::hardware::radio::V1_6::QosSession) == 8, "wrong alignment");

/**
 * The allowed failure modes on an IWLAN handover failure.
 */
enum class HandoverFailureMode : int8_t {
    /**
     * On data handover failure, fallback to the source data transport when the
     * fail cause is due to a hand off preference change.
     */
    LEGACY = 0,
    /**
     * On data handover failure, fallback to the source data transport.
     */
    DO_FALLBACK = 1,
    /**
     * On data handover failure, retry the handover instead of falling back to
     * the source data transport.
     */
    NO_FALLBACK_RETRY_HANDOVER = 2,
    /**
     * On data handover failure, setup a new data connection by sending a normal
     * request to the underlying data service.
     */
    NO_FALLBACK_RETRY_SETUP_NORMAL = 3,
};

/**
 * Expose more setup data call failures.
 */
enum class DataCallFailCause : int32_t {
    NONE = 0,
    OPERATOR_BARRED = 8 /* 0x08 */,
    NAS_SIGNALLING = 14 /* 0x0E */,
    INSUFFICIENT_RESOURCES = 26 /* 0x1A */,
    MISSING_UKNOWN_APN = 27 /* 0x1B */,
    UNKNOWN_PDP_ADDRESS_TYPE = 28 /* 0x1C */,
    USER_AUTHENTICATION = 29 /* 0x1D */,
    ACTIVATION_REJECT_GGSN = 30 /* 0x1E */,
    ACTIVATION_REJECT_UNSPECIFIED = 31 /* 0x1F */,
    SERVICE_OPTION_NOT_SUPPORTED = 32 /* 0x20 */,
    SERVICE_OPTION_NOT_SUBSCRIBED = 33 /* 0x21 */,
    SERVICE_OPTION_OUT_OF_ORDER = 34 /* 0x22 */,
    NSAPI_IN_USE = 35 /* 0x23 */,
    REGULAR_DEACTIVATION = 36 /* 0x24 */,
    QOS_NOT_ACCEPTED = 37 /* 0x25 */,
    NETWORK_FAILURE = 38 /* 0x26 */,
    UMTS_REACTIVATION_REQ = 39 /* 0x27 */,
    FEATURE_NOT_SUPP = 40 /* 0x28 */,
    TFT_SEMANTIC_ERROR = 41 /* 0x29 */,
    TFT_SYTAX_ERROR = 42 /* 0x2A */,
    UNKNOWN_PDP_CONTEXT = 43 /* 0x2B */,
    FILTER_SEMANTIC_ERROR = 44 /* 0x2C */,
    FILTER_SYTAX_ERROR = 45 /* 0x2D */,
    PDP_WITHOUT_ACTIVE_TFT = 46 /* 0x2E */,
    ONLY_IPV4_ALLOWED = 50 /* 0x32 */,
    ONLY_IPV6_ALLOWED = 51 /* 0x33 */,
    ONLY_SINGLE_BEARER_ALLOWED = 52 /* 0x34 */,
    ESM_INFO_NOT_RECEIVED = 53 /* 0x35 */,
    PDN_CONN_DOES_NOT_EXIST = 54 /* 0x36 */,
    MULTI_CONN_TO_SAME_PDN_NOT_ALLOWED = 55 /* 0x37 */,
    MAX_ACTIVE_PDP_CONTEXT_REACHED = 65 /* 0x41 */,
    UNSUPPORTED_APN_IN_CURRENT_PLMN = 66 /* 0x42 */,
    INVALID_TRANSACTION_ID = 81 /* 0x51 */,
    MESSAGE_INCORRECT_SEMANTIC = 95 /* 0x5F */,
    INVALID_MANDATORY_INFO = 96 /* 0x60 */,
    MESSAGE_TYPE_UNSUPPORTED = 97 /* 0x61 */,
    MSG_TYPE_NONCOMPATIBLE_STATE = 98 /* 0x62 */,
    UNKNOWN_INFO_ELEMENT = 99 /* 0x63 */,
    CONDITIONAL_IE_ERROR = 100 /* 0x64 */,
    MSG_AND_PROTOCOL_STATE_UNCOMPATIBLE = 101 /* 0x65 */,
    PROTOCOL_ERRORS = 111 /* 0x6F */,
    APN_TYPE_CONFLICT = 112 /* 0x70 */,
    INVALID_PCSCF_ADDR = 113 /* 0x71 */,
    INTERNAL_CALL_PREEMPT_BY_HIGH_PRIO_APN = 114 /* 0x72 */,
    EMM_ACCESS_BARRED = 115 /* 0x73 */,
    EMERGENCY_IFACE_ONLY = 116 /* 0x74 */,
    IFACE_MISMATCH = 117 /* 0x75 */,
    COMPANION_IFACE_IN_USE = 118 /* 0x76 */,
    IP_ADDRESS_MISMATCH = 119 /* 0x77 */,
    IFACE_AND_POL_FAMILY_MISMATCH = 120 /* 0x78 */,
    EMM_ACCESS_BARRED_INFINITE_RETRY = 121 /* 0x79 */,
    AUTH_FAILURE_ON_EMERGENCY_CALL = 122 /* 0x7A */,
    OEM_DCFAILCAUSE_1 = 4097 /* 0x1001 */,
    OEM_DCFAILCAUSE_2 = 4098 /* 0x1002 */,
    OEM_DCFAILCAUSE_3 = 4099 /* 0x1003 */,
    OEM_DCFAILCAUSE_4 = 4100 /* 0x1004 */,
    OEM_DCFAILCAUSE_5 = 4101 /* 0x1005 */,
    OEM_DCFAILCAUSE_6 = 4102 /* 0x1006 */,
    OEM_DCFAILCAUSE_7 = 4103 /* 0x1007 */,
    OEM_DCFAILCAUSE_8 = 4104 /* 0x1008 */,
    OEM_DCFAILCAUSE_9 = 4105 /* 0x1009 */,
    OEM_DCFAILCAUSE_10 = 4106 /* 0x100A */,
    OEM_DCFAILCAUSE_11 = 4107 /* 0x100B */,
    OEM_DCFAILCAUSE_12 = 4108 /* 0x100C */,
    OEM_DCFAILCAUSE_13 = 4109 /* 0x100D */,
    OEM_DCFAILCAUSE_14 = 4110 /* 0x100E */,
    OEM_DCFAILCAUSE_15 = 4111 /* 0x100F */,
    VOICE_REGISTRATION_FAIL = -1 /* -1 */,
    DATA_REGISTRATION_FAIL = -2 /* -2 */,
    SIGNAL_LOST = -3 /* -3 */,
    PREF_RADIO_TECH_CHANGED = -4 /* -4 */,
    RADIO_POWER_OFF = -5 /* -5 */,
    TETHERED_CALL_ACTIVE = -6 /* -6 */,
    ERROR_UNSPECIFIED = 65535 /* 0xffff */,
    /**
     * Network cannot provide the requested service and PDP context is deactivated because of LLC
     * or SNDCP failure.
     */
    LLC_SNDCP = 25 /* 0x19 */,
    /**
     * UE requested to modify QoS parameters or the bearer control mode, which is not compatible
     * with the selected bearer control mode.
     */
    ACTIVATION_REJECTED_BCM_VIOLATION = 48 /* 0x30 */,
    /**
     * Network has already initiated the activation, modification, or deactivation of bearer
     * resources that was requested by the UE.
     */
    COLLISION_WITH_NETWORK_INITIATED_REQUEST = 56 /* 0x38 */,
    /**
     * Network supports IPv4v6 PDP type only. Non-IP type is not allowed. In LTE mode of operation,
     * this is a PDN throttling cause code, meaning the UE may throttle further requests to the
     * same APN.
     */
    ONLY_IPV4V6_ALLOWED = 57 /* 0x39 */,
    /**
     * Network supports non-IP PDP type only. IPv4, IPv6 and IPv4v6 is not allowed. In LTE mode of
     * operation, this is a PDN throttling cause code, meaning the UE can throttle further requests
     * to the same APN.
     */
    ONLY_NON_IP_ALLOWED = 58 /* 0x3A */,
    /**
     * QCI (QoS Class Identifier) indicated in the UE request cannot be supported.
     */
    UNSUPPORTED_QCI_VALUE = 59 /* 0x3B */,
    /**
     * Procedure requested by the UE was rejected because the bearer handling is not supported.
     */
    BEARER_HANDLING_NOT_SUPPORTED = 60 /* 0x3C */,
    /**
     * Not receiving a DNS address that was mandatory.
     */
    INVALID_DNS_ADDR = 123 /* 0x7B */,
    /**
     * Not receiving either a PCSCF or a DNS address, one of them being mandatory.
     */
    INVALID_PCSCF_OR_DNS_ADDRESS = 124 /* 0x7C */,
    /**
     * Emergency call bring up on a different ePDG.
     */
    CALL_PREEMPT_BY_EMERGENCY_APN = 127 /* 0x7F */,
    /**
     * UE performs a detach or disconnect PDN action based on TE requirements.
     */
    UE_INITIATED_DETACH_OR_DISCONNECT = 128 /* 0x80 */,
    /**
     * Reason unspecified for foreign agent rejected MIP (Mobile IP) registration.
     */
    MIP_FA_REASON_UNSPECIFIED = 2000 /* 0x7D0 */,
    /**
     * Foreign agent administratively prohibited MIP (Mobile IP) registration.
     */
    MIP_FA_ADMIN_PROHIBITED = 2001 /* 0x7D1 */,
    /**
     * Foreign agent rejected MIP (Mobile IP) registration because of insufficient resources.
     */
    MIP_FA_INSUFFICIENT_RESOURCES = 2002 /* 0x7D2 */,
    /**
     * Foreign agent rejected MIP (Mobile IP) registration because of MN-AAA authenticator was
     * wrong.
     */
    MIP_FA_MOBILE_NODE_AUTHENTICATION_FAILURE = 2003 /* 0x7D3 */,
    /**
     * Foreign agent rejected MIP (Mobile IP) registration because of home agent authentication
     * failure.
     */
    MIP_FA_HOME_AGENT_AUTHENTICATION_FAILURE = 2004 /* 0x7D4 */,
    /**
     * Foreign agent rejected MIP (Mobile IP) registration because of requested lifetime was too
     * long.
     */
    MIP_FA_REQUESTED_LIFETIME_TOO_LONG = 2005 /* 0x7D5 */,
    /**
     * Foreign agent rejected MIP (Mobile IP) registration because of malformed request.
     */
    MIP_FA_MALFORMED_REQUEST = 2006 /* 0x7D6 */,
    /**
     * Foreign agent rejected MIP (Mobile IP) registration because of malformed reply.
     */
    MIP_FA_MALFORMED_REPLY = 2007 /* 0x7D7 */,
    /**
     * Foreign agent rejected MIP (Mobile IP) registration because of requested encapsulation was
     * unavailable.
     */
    MIP_FA_ENCAPSULATION_UNAVAILABLE = 2008 /* 0x7D8 */,
    /**
     * Foreign agent rejected MIP (Mobile IP) registration of VJ Header Compression was unavailable.
     */
    MIP_FA_VJ_HEADER_COMPRESSION_UNAVAILABLE = 2009 /* 0x7D9 */,
    /**
     * Foreign agent rejected MIP (Mobile IP) registration because of reverse tunnel was
     * unavailable.
     */
    MIP_FA_REVERSE_TUNNEL_UNAVAILABLE = 2010 /* 0x7DA */,
    /**
     * Foreign agent rejected MIP (Mobile IP) registration because of reverse tunnel was mandatory
     * but not requested by device.
     */
    MIP_FA_REVERSE_TUNNEL_IS_MANDATORY = 2011 /* 0x7DB */,
    /**
     * Foreign agent rejected MIP (Mobile IP) registration because of delivery style was not
     * supported.
     */
    MIP_FA_DELIVERY_STYLE_NOT_SUPPORTED = 2012 /* 0x7DC */,
    /**
     * Foreign agent rejected MIP (Mobile IP) registration because of missing NAI (Network Access
     * Identifier).
     */
    MIP_FA_MISSING_NAI = 2013 /* 0x7DD */,
    /**
     * Foreign agent rejected MIP (Mobile IP) registration because of missing Home Agent.
     */
    MIP_FA_MISSING_HOME_AGENT = 2014 /* 0x7DE */,
    /**
     * Foreign agent rejected MIP (Mobile IP) registration because of missing Home Address.
     */
    MIP_FA_MISSING_HOME_ADDRESS = 2015 /* 0x7DF */,
    /**
     * Foreign agent rejected MIP (Mobile IP) registration because of unknown challenge.
     */
    MIP_FA_UNKNOWN_CHALLENGE = 2016 /* 0x7E0 */,
    /**
     * Foreign agent rejected MIP (Mobile IP) registration because of missing challenge.
     */
    MIP_FA_MISSING_CHALLENGE = 2017 /* 0x7E1 */,
    /**
     * Foreign agent rejected MIP (Mobile IP) registration because of stale challenge.
     */
    MIP_FA_STALE_CHALLENGE = 2018 /* 0x7E2 */,
    /**
     * Reason unspecified for home agent rejected MIP (Mobile IP) registration.
     */
    MIP_HA_REASON_UNSPECIFIED = 2019 /* 0x7E3 */,
    /**
     * Home agent administratively prohibited MIP (Mobile IP) registration.
     */
    MIP_HA_ADMIN_PROHIBITED = 2020 /* 0x7E4 */,
    /**
     * Home agent rejected MIP (Mobile IP) registration because of insufficient resources.
     */
    MIP_HA_INSUFFICIENT_RESOURCES = 2021 /* 0x7E5 */,
    /**
     * Home agent rejected MIP (Mobile IP) registration because of MN-HA authenticator was wrong.
     */
    MIP_HA_MOBILE_NODE_AUTHENTICATION_FAILURE = 2022 /* 0x7E6 */,
    /**
     * Home agent rejected MIP (Mobile IP) registration because of foreign agent authentication
     * failure.
     */
    MIP_HA_FOREIGN_AGENT_AUTHENTICATION_FAILURE = 2023 /* 0x7E7 */,
    /**
     * Home agent rejected MIP (Mobile IP) registration because of registration id mismatch.
     */
    MIP_HA_REGISTRATION_ID_MISMATCH = 2024 /* 0x7E8 */,
    /**
     * Home agent rejected MIP (Mobile IP) registration because of malformed request.
     */
    MIP_HA_MALFORMED_REQUEST = 2025 /* 0x7E9 */,
    /**
     * Home agent rejected MIP (Mobile IP) registration because of unknown home agent address.
     */
    MIP_HA_UNKNOWN_HOME_AGENT_ADDRESS = 2026 /* 0x7EA */,
    /**
     * Home agent rejected MIP (Mobile IP) registration because of reverse tunnel was unavailable.
     */
    MIP_HA_REVERSE_TUNNEL_UNAVAILABLE = 2027 /* 0x7EB */,
    /**
     * Home agent rejected MIP (Mobile IP) registration because of reverse tunnel is mandatory but
     * not requested by device.
     */
    MIP_HA_REVERSE_TUNNEL_IS_MANDATORY = 2028 /* 0x7EC */,
    /**
     * Home agent rejected MIP (Mobile IP) registration because of encapsulation unavailable.
     */
    MIP_HA_ENCAPSULATION_UNAVAILABLE = 2029 /* 0x7ED */,
    /**
     * Tearing down is in progress.
     */
    CLOSE_IN_PROGRESS = 2030 /* 0x7EE */,
    /**
     * Brought down by the network.
     */
    NETWORK_INITIATED_TERMINATION = 2031 /* 0x7EF */,
    /**
     * Another application in modem preempts the data call.
     */
    MODEM_APP_PREEMPTED = 2032 /* 0x7F0 */,
    /**
     * IPV4 PDN is in throttled state due to network providing only IPV6 address during the previous
     * VSNCP bringup (subs_limited_to_v6).
     */
    PDN_IPV4_CALL_DISALLOWED = 2033 /* 0x7F1 */,
    /**
     * IPV4 PDN is in throttled state due to previous VSNCP bringup failure(s).
     */
    PDN_IPV4_CALL_THROTTLED = 2034 /* 0x7F2 */,
    /**
     * IPV6 PDN is in throttled state due to network providing only IPV4 address during the previous
     * VSNCP bringup (subs_limited_to_v4).
     */
    PDN_IPV6_CALL_DISALLOWED = 2035 /* 0x7F3 */,
    /**
     * IPV6 PDN is in throttled state due to previous VSNCP bringup failure(s).
     */
    PDN_IPV6_CALL_THROTTLED = 2036 /* 0x7F4 */,
    /**
     * Modem restart.
     */
    MODEM_RESTART = 2037 /* 0x7F5 */,
    /**
     * PDP PPP calls are not supported.
     */
    PDP_PPP_NOT_SUPPORTED = 2038 /* 0x7F6 */,
    /**
     * RAT on which the data call is attempted/connected is no longer the preferred RAT.
     */
    UNPREFERRED_RAT = 2039 /* 0x7F7 */,
    /**
     * Physical link is in the process of cleanup.
     */
    PHYSICAL_LINK_CLOSE_IN_PROGRESS = 2040 /* 0x7F8 */,
    /**
     * Interface bring up is attempted for an APN that is yet to be handed over to target RAT.
     */
    APN_PENDING_HANDOVER = 2041 /* 0x7F9 */,
    /**
     * APN bearer type in the profile does not match preferred network mode.
     */
    PROFILE_BEARER_INCOMPATIBLE = 2042 /* 0x7FA */,
    /**
     * Card was refreshed or removed.
     */
    SIM_CARD_CHANGED = 2043 /* 0x7FB */,
    /**
     * Device is going into lower power mode or powering down.
     */
    LOW_POWER_MODE_OR_POWERING_DOWN = 2044 /* 0x7FC */,
    /**
     * APN has been disabled.
     */
    APN_DISABLED = 2045 /* 0x7FD */,
    /**
     * Maximum PPP inactivity timer expired.
     */
    MAX_PPP_INACTIVITY_TIMER_EXPIRED = 2046 /* 0x7FE */,
    /**
     * IPv6 address transfer failed.
     */
    IPV6_ADDRESS_TRANSFER_FAILED = 2047 /* 0x7FF */,
    /**
     * Target RAT swap failed.
     */
    TRAT_SWAP_FAILED = 2048 /* 0x800 */,
    /**
     * Device falls back from eHRPD to HRPD.
     */
    EHRPD_TO_HRPD_FALLBACK = 2049 /* 0x801 */,
    /**
     * UE is in MIP-only configuration but the MIP configuration fails on call bring up due to
     * incorrect provisioning.
     */
    MIP_CONFIG_FAILURE = 2050 /* 0x802 */,
    /**
     * PDN inactivity timer expired due to no data transmission in a configurable duration of time.
     */
    PDN_INACTIVITY_TIMER_EXPIRED = 2051 /* 0x803 */,
    /**
     * IPv4 data call bring up is rejected because the UE already maintains the allotted maximum
     * number of IPv4 data connections.
     */
    MAX_IPV4_CONNECTIONS = 2052 /* 0x804 */,
    /**
     * IPv6 data call bring up is rejected because the UE already maintains the allotted maximum
     * number of IPv6 data connections.
     */
    MAX_IPV6_CONNECTIONS = 2053 /* 0x805 */,
    /**
     * New PDN bring up is rejected during interface selection because the UE has already allotted
     * the available interfaces for other PDNs.
     */
    APN_MISMATCH = 2054 /* 0x806 */,
    /**
     * New call bring up is rejected since the existing data call IP type doesn't match the
     * requested IP.
     */
    IP_VERSION_MISMATCH = 2055 /* 0x807 */,
    /**
     * Dial up networking (DUN) call bring up is rejected since UE is in eHRPD RAT.
     */
    DUN_CALL_DISALLOWED = 2056 /* 0x808 */,
    /**
     * Rejected/Brought down since UE is transition between EPC and NONEPC RAT.
     */
    INTERNAL_EPC_NONEPC_TRANSITION = 2057 /* 0x809 */,
    /**
     * The current interface is being in use.
     */
    INTERFACE_IN_USE = 2058 /* 0x80A */,
    /**
     * PDN connection to the APN is disallowed on the roaming network.
     */
    APN_DISALLOWED_ON_ROAMING = 2059 /* 0x80B */,
    /**
     * APN-related parameters are changed.
     */
    APN_PARAMETERS_CHANGED = 2060 /* 0x80C */,
    /**
     * PDN is attempted to be brought up with NULL APN but NULL APN is not supported.
     */
    NULL_APN_DISALLOWED = 2061 /* 0x80D */,
    /**
     * Thermal level increases and causes calls to be torn down when normal mode of operation is
     * not allowed.
     */
    THERMAL_MITIGATION = 2062 /* 0x80E */,
    /**
     * PDN Connection to a given APN is disallowed because data is disabled from the device user
     * interface settings.
     */
    DATA_SETTINGS_DISABLED = 2063 /* 0x80F */,
    /**
     * PDN Connection to a given APN is disallowed because data roaming is disabled from the device
     * user interface settings and the UE is roaming.
     */
    DATA_ROAMING_SETTINGS_DISABLED = 2064 /* 0x810 */,
    /**
     * DDS (Default data subscription) switch occurs.
     */
    DDS_SWITCHED = 2065 /* 0x811 */,
    /**
     * PDN being brought up with an APN that is part of forbidden APN Name list.
     */
    FORBIDDEN_APN_NAME = 2066 /* 0x812 */,
    /**
     * Default data subscription switch is in progress.
     */
    DDS_SWITCH_IN_PROGRESS = 2067 /* 0x813 */,
    /**
     * Roaming is disallowed during call bring up.
     */
    CALL_DISALLOWED_IN_ROAMING = 2068 /* 0x814 */,
    /**
     * UE is unable to bring up a non-IP data call because the device is not camped on a NB1 cell.
     */
    NON_IP_NOT_SUPPORTED = 2069 /* 0x815 */,
    /**
     * Non-IP PDN is in throttled state due to previous VSNCP bringup failure(s).
     */
    PDN_NON_IP_CALL_THROTTLED = 2070 /* 0x816 */,
    /**
     * Non-IP PDN is in disallowed state due to the network providing only an IP address.
     */
    PDN_NON_IP_CALL_DISALLOWED = 2071 /* 0x817 */,
    /**
     * Device in CDMA locked state.
     */
    CDMA_LOCK = 2072 /* 0x818 */,
    /**
     * Received an intercept order from the base station.
     */
    CDMA_INTERCEPT = 2073 /* 0x819 */,
    /**
     * Receiving a reorder from the base station.
     */
    CDMA_REORDER = 2074 /* 0x81A */,
    /**
     * Receiving a release from the base station with a SO (Service Option) Reject reason.
     */
    CDMA_RELEASE_DUE_TO_SO_REJECTION = 2075 /* 0x81B */,
    /**
     * Receiving an incoming call from the base station.
     */
    CDMA_INCOMING_CALL = 2076 /* 0x81C */,
    /**
     * Received an alert stop from the base station due to incoming only.
     */
    CDMA_ALERT_STOP = 2077 /* 0x81D */,
    /**
     * Channel acquisition failures. This indicates that device has failed acquiring all the
     * channels in the PRL.
     */
    CHANNEL_ACQUISITION_FAILURE = 2078 /* 0x81E */,
    /**
     * Maximum access probes transmitted.
     */
    MAX_ACCESS_PROBE = 2079 /* 0x81F */,
    /**
     * Concurrent service is not supported by base station.
     */
    CONCURRENT_SERVICE_NOT_SUPPORTED_BY_BASE_STATION = 2080 /* 0x820 */,
    /**
     * There was no response received from the base station.
     */
    NO_RESPONSE_FROM_BASE_STATION = 2081 /* 0x821 */,
    /**
     * The base station rejecting the call.
     */
    REJECTED_BY_BASE_STATION = 2082 /* 0x822 */,
    /**
     * The concurrent services requested were not compatible.
     */
    CONCURRENT_SERVICES_INCOMPATIBLE = 2083 /* 0x823 */,
    /**
     * Device does not have CDMA service.
     */
    NO_CDMA_SERVICE = 2084 /* 0x824 */,
    /**
     * RUIM not being present.
     */
    RUIM_NOT_PRESENT = 2085 /* 0x825 */,
    /**
     * Receiving a retry order from the base station.
     */
    CDMA_RETRY_ORDER = 2086 /* 0x826 */,
    /**
     * Access blocked by the base station.
     */
    ACCESS_BLOCK = 2087 /* 0x827 */,
    /**
     * Access blocked by the base station for all mobile devices.
     */
    ACCESS_BLOCK_ALL = 2088 /* 0x828 */,
    /**
     * Maximum access probes for the IS-707B call.
     */
    IS707B_MAX_ACCESS_PROBES = 2089 /* 0x829 */,
    /**
     * Put device in thermal emergency.
     */
    THERMAL_EMERGENCY = 2090 /* 0x82A */,
    /**
     * In favor of a voice call or SMS when concurrent voice and data are not supported.
     */
    CONCURRENT_SERVICES_NOT_ALLOWED = 2091 /* 0x82B */,
    /**
     * The other clients rejected incoming call.
     */
    INCOMING_CALL_REJECTED = 2092 /* 0x82C */,
    /**
     * No service on the gateway.
     */
    NO_SERVICE_ON_GATEWAY = 2093 /* 0x82D */,
    /**
     * GPRS context is not available.
     */
    NO_GPRS_CONTEXT = 2094 /* 0x82E */,
    /**
     * Network refuses service to the MS because either an identity of the MS is not acceptable to
     * the network or the MS does not pass the authentication check.
     */
    ILLEGAL_MS = 2095 /* 0x82F */,
    /**
     * ME could not be authenticated and the ME used is not acceptable to the network.
     */
    ILLEGAL_ME = 2096 /* 0x830 */,
    /**
     * Not allowed to operate either GPRS or non-GPRS services.
     */
    GPRS_SERVICES_AND_NON_GPRS_SERVICES_NOT_ALLOWED = 2097 /* 0x831 */,
    /**
     * MS is not allowed to operate GPRS services.
     */
    GPRS_SERVICES_NOT_ALLOWED = 2098 /* 0x832 */,
    /**
     * No matching identity or context could be found in the network.
     */
    MS_IDENTITY_CANNOT_BE_DERIVED_BY_THE_NETWORK = 2099 /* 0x833 */,
    /**
     * Mobile reachable timer has expired, or the GMM context data related to the subscription does
     * not exist in the SGSN.
     */
    IMPLICITLY_DETACHED = 2100 /* 0x834 */,
    /**
     * UE requests GPRS service, or the network initiates a detach request in a PLMN which does not
     * offer roaming for GPRS services to that MS.
     */
    PLMN_NOT_ALLOWED = 2101 /* 0x835 */,
    /**
     * MS requests service, or the network initiates a detach request, in a location area where the
     * HPLMN determines that the MS, by subscription, is not allowed to operate.
     */
    LOCATION_AREA_NOT_ALLOWED = 2102 /* 0x836 */,
    /**
     * UE requests GPRS service or the network initiates a detach request in a PLMN that does not
     * offer roaming for GPRS services.
     */
    GPRS_SERVICES_NOT_ALLOWED_IN_THIS_PLMN = 2103 /* 0x837 */,
    /**
     * PDP context already exists.
     */
    PDP_DUPLICATE = 2104 /* 0x838 */,
    /**
     * RAT change on the UE.
     */
    UE_RAT_CHANGE = 2105 /* 0x839 */,
    /**
     * Network cannot serve a request from the MS due to congestion.
     */
    CONGESTION = 2106 /* 0x83A */,
    /**
     * MS requests an establishment of the radio access bearers for all active PDP contexts by
     * sending a service request message indicating data to the network, but the SGSN does not have
     * any active PDP context.
     */
    NO_PDP_CONTEXT_ACTIVATED = 2107 /* 0x83B */,
    /**
     * Access class blocking restrictions for the current camped cell.
     */
    ACCESS_CLASS_DSAC_REJECTION = 2108 /* 0x83C */,
    /**
     * SM attempts PDP activation for a maximum of four attempts.
     */
    PDP_ACTIVATE_MAX_RETRY_FAILED = 2109 /* 0x83D */,
    /**
     * Radio access bearer failure.
     */
    RADIO_ACCESS_BEARER_FAILURE = 2110 /* 0x83E */,
    /**
     * Invalid EPS bearer identity in the request.
     */
    ESM_UNKNOWN_EPS_BEARER_CONTEXT = 2111 /* 0x83F */,
    /**
     * Data radio bearer is released by the RRC.
     */
    DRB_RELEASED_BY_RRC = 2112 /* 0x840 */,
    /**
     * Indicate the connection was released.
     */
    CONNECTION_RELEASED = 2113 /* 0x841 */,
    /**
     * UE is detached.
     */
    EMM_DETACHED = 2114 /* 0x842 */,
    /**
     * Attach procedure is rejected by the network.
     */
    EMM_ATTACH_FAILED = 2115 /* 0x843 */,
    /**
     * Attach procedure is started for EMC purposes.
     */
    EMM_ATTACH_STARTED = 2116 /* 0x844 */,
    /**
     * Service request procedure failure.
     */
    LTE_NAS_SERVICE_REQUEST_FAILED = 2117 /* 0x845 */,
    /**
     * Active dedicated bearer was requested using the same default bearer ID.
     */
    DUPLICATE_BEARER_ID = 2118 /* 0x846 */,
    /**
     * Collision scenarios for the UE and network-initiated procedures.
     */
    ESM_COLLISION_SCENARIOS = 2119 /* 0x847 */,
    /**
     * Bearer must be deactivated to synchronize with the network.
     */
    ESM_BEARER_DEACTIVATED_TO_SYNC_WITH_NETWORK = 2120 /* 0x848 */,
    /**
     * Active dedicated bearer was requested for an existing default bearer.
     */
    ESM_NW_ACTIVATED_DED_BEARER_WITH_ID_OF_DEF_BEARER = 2121 /* 0x849 */,
    /**
     * Bad OTA message is received from the network.
     */
    ESM_BAD_OTA_MESSAGE = 2122 /* 0x84A */,
    /**
     * Download server rejected the call.
     */
    ESM_DOWNLOAD_SERVER_REJECTED_THE_CALL = 2123 /* 0x84B */,
    /**
     * PDN was disconnected by the downlaod server due to IRAT.
     */
    ESM_CONTEXT_TRANSFERRED_DUE_TO_IRAT = 2124 /* 0x84C */,
    /**
     * Dedicated bearer will be deactivated regardless of the network response.
     */
    DS_EXPLICIT_DEACTIVATION = 2125 /* 0x84D */,
    /**
     * No specific local cause is mentioned, usually a valid OTA cause.
     */
    ESM_LOCAL_CAUSE_NONE = 2126 /* 0x84E */,
    /**
     * Throttling is not needed for this service request failure.
     */
    LTE_THROTTLING_NOT_REQUIRED = 2127 /* 0x84F */,
    /**
     * Access control list check failure at the lower layer.
     */
    ACCESS_CONTROL_LIST_CHECK_FAILURE = 2128 /* 0x850 */,
    /**
     * Service is not allowed on the requested PLMN.
     */
    SERVICE_NOT_ALLOWED_ON_PLMN = 2129 /* 0x851 */,
    /**
     * T3417 timer expiration of the service request procedure.
     */
    EMM_T3417_EXPIRED = 2130 /* 0x852 */,
    /**
     * Extended service request fails due to expiration of the T3417 EXT timer.
     */
    EMM_T3417_EXT_EXPIRED = 2131 /* 0x853 */,
    /**
     * Transmission failure of radio resource control (RRC) uplink data.
     */
    RRC_UPLINK_DATA_TRANSMISSION_FAILURE = 2132 /* 0x854 */,
    /**
     * Radio resource control (RRC) uplink data delivery failed due to a handover.
     */
    RRC_UPLINK_DELIVERY_FAILED_DUE_TO_HANDOVER = 2133 /* 0x855 */,
    /**
     * Radio resource control (RRC) uplink data delivery failed due to a connection release.
     */
    RRC_UPLINK_CONNECTION_RELEASE = 2134 /* 0x856 */,
    /**
     * Radio resource control (RRC) uplink data delivery failed due to a radio link failure.
     */
    RRC_UPLINK_RADIO_LINK_FAILURE = 2135 /* 0x857 */,
    /**
     * Radio resource control (RRC) is not connected but the non-access stratum (NAS) sends an
     * uplink data request.
     */
    RRC_UPLINK_ERROR_REQUEST_FROM_NAS = 2136 /* 0x858 */,
    /**
     * Radio resource control (RRC) connection failure at access stratum.
     */
    RRC_CONNECTION_ACCESS_STRATUM_FAILURE = 2137 /* 0x859 */,
    /**
     * Radio resource control (RRC) connection establishment is aborted due to another procedure.
     */
    RRC_CONNECTION_ANOTHER_PROCEDURE_IN_PROGRESS = 2138 /* 0x85A */,
    /**
     * Radio resource control (RRC) connection establishment failed due to access barrred.
     */
    RRC_CONNECTION_ACCESS_BARRED = 2139 /* 0x85B */,
    /**
     * Radio resource control (RRC) connection establishment failed due to cell reselection at
     * access stratum.
     */
    RRC_CONNECTION_CELL_RESELECTION = 2140 /* 0x85C */,
    /**
     * Connection establishment failed due to configuration failure at the radio resource control
     * (RRC).
     */
    RRC_CONNECTION_CONFIG_FAILURE = 2141 /* 0x85D */,
    /**
     * Radio resource control (RRC) connection could not be established in the time limit.
     */
    RRC_CONNECTION_TIMER_EXPIRED = 2142 /* 0x85E */,
    /**
     * Connection establishment failed due to a link failure at the radio resource control (RRC).
     */
    RRC_CONNECTION_LINK_FAILURE = 2143 /* 0x85F */,
    /**
     * Connection establishment failed as the radio resource control (RRC) is not camped on any
     * cell.
     */
    RRC_CONNECTION_CELL_NOT_CAMPED = 2144 /* 0x860 */,
    /**
     * Connection establishment failed due to a service interval failure at the radio resource
     * control (RRC).
     */
    RRC_CONNECTION_SYSTEM_INTERVAL_FAILURE = 2145 /* 0x861 */,
    /**
     * Radio resource control (RRC) connection establishment failed due to the network rejecting the
     * UE connection request.
     */
    RRC_CONNECTION_REJECT_BY_NETWORK = 2146 /* 0x862 */,
    /**
     * Normal radio resource control (RRC) connection release.
     */
    RRC_CONNECTION_NORMAL_RELEASE = 2147 /* 0x863 */,
    /**
     * Radio resource control (RRC) connection release failed due to radio link failure conditions.
     */
    RRC_CONNECTION_RADIO_LINK_FAILURE = 2148 /* 0x864 */,
    /**
     * Radio resource control (RRC) connection re-establishment failure.
     */
    RRC_CONNECTION_REESTABLISHMENT_FAILURE = 2149 /* 0x865 */,
    /**
     * UE is out of service during the call register.
     */
    RRC_CONNECTION_OUT_OF_SERVICE_DURING_CELL_REGISTER = 2150 /* 0x866 */,
    /**
     * Connection has been released by the radio resource control (RRC) due to an abort request.
     */
    RRC_CONNECTION_ABORT_REQUEST = 2151 /* 0x867 */,
    /**
     * Radio resource control (RRC) connection released due to a system information block read
     * error.
     */
    RRC_CONNECTION_SYSTEM_INFORMATION_BLOCK_READ_ERROR = 2152 /* 0x868 */,
    /**
     * Network-initiated detach with reattach.
     */
    NETWORK_INITIATED_DETACH_WITH_AUTO_REATTACH = 2153 /* 0x869 */,
    /**
     * Network-initiated detach without reattach.
     */
    NETWORK_INITIATED_DETACH_NO_AUTO_REATTACH = 2154 /* 0x86A */,
    /**
     * ESM procedure maximum attempt timeout failure.
     */
    ESM_PROCEDURE_TIME_OUT = 2155 /* 0x86B */,
    /**
     * No PDP exists with the given connection ID while modifying or deactivating or activation for
     * an already active PDP.
     */
    INVALID_CONNECTION_ID = 2156 /* 0x86C */,
    /**
     * Maximum NSAPIs have been exceeded during PDP activation.
     */
    MAXIMIUM_NSAPIS_EXCEEDED = 2157 /* 0x86D */,
    /**
     * Primary context for NSAPI does not exist.
     */
    INVALID_PRIMARY_NSAPI = 2158 /* 0x86E */,
    /**
     * Unable to encode the OTA message for MT PDP or deactivate PDP.
     */
    CANNOT_ENCODE_OTA_MESSAGE = 2159 /* 0x86F */,
    /**
     * Radio access bearer is not established by the lower layers during activation, modification,
     * or deactivation.
     */
    RADIO_ACCESS_BEARER_SETUP_FAILURE = 2160 /* 0x870 */,
    /**
     * Expiration of the PDP establish timer with a maximum of five retries.
     */
    PDP_ESTABLISH_TIMEOUT_EXPIRED = 2161 /* 0x871 */,
    /**
     * Expiration of the PDP modify timer with a maximum of four retries.
     */
    PDP_MODIFY_TIMEOUT_EXPIRED = 2162 /* 0x872 */,
    /**
     * Expiration of the PDP deactivate timer with a maximum of four retries.
     */
    PDP_INACTIVE_TIMEOUT_EXPIRED = 2163 /* 0x873 */,
    /**
     * PDP activation failed due to RRC_ABORT or a forbidden PLMN.
     */
    PDP_LOWERLAYER_ERROR = 2164 /* 0x874 */,
    /**
     * MO PDP modify collision when the MT PDP is already in progress.
     */
    PDP_MODIFY_COLLISION = 2165 /* 0x875 */,
    /**
     * Maximum size of the L2 message was exceeded.
     */
    MAXINUM_SIZE_OF_L2_MESSAGE_EXCEEDED = 2166 /* 0x876 */,
    /**
     * Non-access stratum (NAS) request was rejected by the network.
     */
    NAS_REQUEST_REJECTED_BY_NETWORK = 2167 /* 0x877 */,
    /**
     * Radio resource control (RRC) connection establishment failure due to an error in the request
     * message.
     */
    RRC_CONNECTION_INVALID_REQUEST = 2168 /* 0x878 */,
    /**
     * Radio resource control (RRC) connection establishment failure due to a change in the tracking
     * area ID.
     */
    RRC_CONNECTION_TRACKING_AREA_ID_CHANGED = 2169 /* 0x879 */,
    /**
     * Radio resource control (RRC) connection establishment failure due to the RF was unavailable.
     */
    RRC_CONNECTION_RF_UNAVAILABLE = 2170 /* 0x87A */,
    /**
     * Radio resource control (RRC) connection was aborted before deactivating the LTE stack due to
     * a successful LTE to WCDMA/GSM/TD-SCDMA IRAT change.
     */
    RRC_CONNECTION_ABORTED_DUE_TO_IRAT_CHANGE = 2171 /* 0x87B */,
    /**
     * If the UE has an LTE radio link failure before security is established, the radio resource
     * control (RRC) connection must be released and the UE must return to idle.
     */
    RRC_CONNECTION_RELEASED_SECURITY_NOT_ACTIVE = 2172 /* 0x87C */,
    /**
     * Radio resource control (RRC) connection was aborted by the non-access stratum (NAS) after an
     * IRAT to LTE IRAT handover.
     */
    RRC_CONNECTION_ABORTED_AFTER_HANDOVER = 2173 /* 0x87D */,
    /**
     * Radio resource control (RRC) connection was aborted before deactivating the LTE stack after a
     * successful LTE to GSM/EDGE IRAT cell change order procedure.
     */
    RRC_CONNECTION_ABORTED_AFTER_IRAT_CELL_CHANGE = 2174 /* 0x87E */,
    /**
     * Radio resource control (RRC) connection was aborted in the middle of a LTE to GSM IRAT cell
     * change order procedure.
     */
    RRC_CONNECTION_ABORTED_DURING_IRAT_CELL_CHANGE = 2175 /* 0x87F */,
    /**
     * IMSI present in the UE is unknown in the home subscriber server.
     */
    IMSI_UNKNOWN_IN_HOME_SUBSCRIBER_SERVER = 2176 /* 0x880 */,
    /**
     * IMEI of the UE is not accepted by the network.
     */
    IMEI_NOT_ACCEPTED = 2177 /* 0x881 */,
    /**
     * EPS and non-EPS services are not allowed by the network.
     */
    EPS_SERVICES_AND_NON_EPS_SERVICES_NOT_ALLOWED = 2178 /* 0x882 */,
    /**
     * EPS services are not allowed in the PLMN.
     */
    EPS_SERVICES_NOT_ALLOWED_IN_PLMN = 2179 /* 0x883 */,
    /**
     * Mobile switching center is temporarily unreachable.
     */
    MSC_TEMPORARILY_NOT_REACHABLE = 2180 /* 0x884 */,
    /**
     * CS domain is not available.
     */
    CS_DOMAIN_NOT_AVAILABLE = 2181 /* 0x885 */,
    /**
     * ESM level failure.
     */
    ESM_FAILURE = 2182 /* 0x886 */,
    /**
     * MAC level failure.
     */
    MAC_FAILURE = 2183 /* 0x887 */,
    /**
     * Synchronization failure.
     */
    SYNCHRONIZATION_FAILURE = 2184 /* 0x888 */,
    /**
     * UE security capabilities mismatch.
     */
    UE_SECURITY_CAPABILITIES_MISMATCH = 2185 /* 0x889 */,
    /**
     * Unspecified security mode reject.
     */
    SECURITY_MODE_REJECTED = 2186 /* 0x88A */,
    /**
     * Unacceptable non-EPS authentication.
     */
    UNACCEPTABLE_NON_EPS_AUTHENTICATION = 2187 /* 0x88B */,
    /**
     * CS fallback call establishment is not allowed.
     */
    CS_FALLBACK_CALL_ESTABLISHMENT_NOT_ALLOWED = 2188 /* 0x88C */,
    /**
     * No EPS bearer context was activated.
     */
    NO_EPS_BEARER_CONTEXT_ACTIVATED = 2189 /* 0x88D */,
    /**
     * Invalid EMM state.
     */
    INVALID_EMM_STATE = 2190 /* 0x88E */,
    /**
     * Non-Access Spectrum layer failure.
     */
    NAS_LAYER_FAILURE = 2191 /* 0x88F */,
    /**
     * Multiple PDP call feature is disabled.
     */
    MULTIPLE_PDP_CALL_NOT_ALLOWED = 2192 /* 0x890 */,
    /**
     * Data call was unsuccessfully transferred during the IRAT handover.
     */
    IRAT_HANDOVER_FAILED = 2194 /* 0x892 */,
    /**
     * Test loop-back data call has been successfully brought down.
     */
    TEST_LOOPBACK_REGULAR_DEACTIVATION = 2196 /* 0x894 */,
    /**
     * Lower layer registration failure.
     */
    LOWER_LAYER_REGISTRATION_FAILURE = 2197 /* 0x895 */,
    /**
     * Network initiates a detach on LTE with error cause "data plan has been replenished or has
     * expired".
     */
    DATA_PLAN_EXPIRED = 2198 /* 0x896 */,
    /**
     * UMTS interface is brought down due to handover from UMTS to iWLAN.
     */
    UMTS_HANDOVER_TO_IWLAN = 2199 /* 0x897 */,
    /**
     * Received a connection deny due to general or network busy on EVDO network.
     */
    EVDO_CONNECTION_DENY_BY_GENERAL_OR_NETWORK_BUSY = 2200 /* 0x898 */,
    /**
     * Received a connection deny due to billing or authentication failure on EVDO network.
     */
    EVDO_CONNECTION_DENY_BY_BILLING_OR_AUTHENTICATION_FAILURE = 2201 /* 0x899 */,
    /**
     * HDR system has been changed due to redirection or the PRL was not preferred.
     */
    EVDO_HDR_CHANGED = 2202 /* 0x89A */,
    /**
     * Device exited HDR due to redirection or the PRL was not preferred.
     */
    EVDO_HDR_EXITED = 2203 /* 0x89B */,
    /**
     * Device does not have an HDR session.
     */
    EVDO_HDR_NO_SESSION = 2204 /* 0x89C */,
    /**
     * It is ending an HDR call origination in favor of a GPS fix.
     */
    EVDO_USING_GPS_FIX_INSTEAD_OF_HDR_CALL = 2205 /* 0x89D */,
    /**
     * Connection setup on the HDR system was time out.
     */
    EVDO_HDR_CONNECTION_SETUP_TIMEOUT = 2206 /* 0x89E */,
    /**
     * Device failed to acquire a co-located HDR for origination.
     */
    FAILED_TO_ACQUIRE_COLOCATED_HDR = 2207 /* 0x89F */,
    /**
     * OTASP commit is in progress.
     */
    OTASP_COMMIT_IN_PROGRESS = 2208 /* 0x8A0 */,
    /**
     * Device has no hybrid HDR service.
     */
    NO_HYBRID_HDR_SERVICE = 2209 /* 0x8A1 */,
    /**
     * HDR module could not be obtained because of the RF locked.
     */
    HDR_NO_LOCK_GRANTED = 2210 /* 0x8A2 */,
    /**
     * DBM or SMS is in progress.
     */
    DBM_OR_SMS_IN_PROGRESS = 2211 /* 0x8A3 */,
    /**
     * HDR module released the call due to fade.
     */
    HDR_FADE = 2212 /* 0x8A4 */,
    /**
     * HDR system access failure.
     */
    HDR_ACCESS_FAILURE = 2213 /* 0x8A5 */,
    /**
     * P_rev supported by 1 base station is less than 6, which is not supported for a 1X data call.
     * The UE must be in the footprint of BS which has p_rev >= 6 to support this SO33 call.
     */
    UNSUPPORTED_1X_PREV = 2214 /* 0x8A6 */,
    /**
     * Client ended the data call.
     */
    LOCAL_END = 2215 /* 0x8A7 */,
    /**
     * Device has no service.
     */
    NO_SERVICE = 2216 /* 0x8A8 */,
    /**
     * Device lost the system due to fade.
     */
    FADE = 2217 /* 0x8A9 */,
    /**
     * Receiving a release from the base station with no reason.
     */
    NORMAL_RELEASE = 2218 /* 0x8AA */,
    /**
     * Access attempt is already in progress.
     */
    ACCESS_ATTEMPT_ALREADY_IN_PROGRESS = 2219 /* 0x8AB */,
    /**
     * Device is in the process of redirecting or handing off to a different target system.
     */
    REDIRECTION_OR_HANDOFF_IN_PROGRESS = 2220 /* 0x8AC */,
    /**
     * Device is operating in Emergency mode.
     */
    EMERGENCY_MODE = 2221 /* 0x8AD */,
    /**
     * Device is in use (e.g., voice call).
     */
    PHONE_IN_USE = 2222 /* 0x8AE */,
    /**
     * Device operational mode is different from the mode requested in the traffic channel bring up.
     */
    INVALID_MODE = 2223 /* 0x8AF */,
    /**
     * SIM was marked by the network as invalid for the circuit and/or packet service domain.
     */
    INVALID_SIM_STATE = 2224 /* 0x8B0 */,
    /**
     * There is no co-located HDR.
     */
    NO_COLLOCATED_HDR = 2225 /* 0x8B1 */,
    /**
     * UE is entering power save mode.
     */
    UE_IS_ENTERING_POWERSAVE_MODE = 2226 /* 0x8B2 */,
    /**
     * Dual switch from single standby to dual standby is in progress.
     */
    DUAL_SWITCH = 2227 /* 0x8B3 */,
    /**
     * Data call bring up fails in the PPP setup due to a timeout. (e.g., an LCP conf ack was not
     * received from the network)
     */
    PPP_TIMEOUT = 2228 /* 0x8B4 */,
    /**
     * Data call bring up fails in the PPP setup due to an authorization failure.
     * (e.g., authorization is required, but not negotiated with the network during an LCP phase)
     */
    PPP_AUTH_FAILURE = 2229 /* 0x8B5 */,
    /**
     * Data call bring up fails in the PPP setup due to an option mismatch.
     */
    PPP_OPTION_MISMATCH = 2230 /* 0x8B6 */,
    /**
     * Data call bring up fails in the PPP setup due to a PAP failure.
     */
    PPP_PAP_FAILURE = 2231 /* 0x8B7 */,
    /**
     * Data call bring up fails in the PPP setup due to a CHAP failure.
     */
    PPP_CHAP_FAILURE = 2232 /* 0x8B8 */,
    /**
     * Data call bring up fails in the PPP setup because the PPP is in the process of cleaning the
     * previous PPP session.
     */
    PPP_CLOSE_IN_PROGRESS = 2233 /* 0x8B9 */,
    /**
     * IPv6 interface bring up fails because the network provided only the IPv4 address for the
     * upcoming PDN permanent client can reattempt a IPv6 call bring up after the IPv4 interface is
     * also brought down. However, there is no guarantee that the network will provide a IPv6
     * address.
     */
    LIMITED_TO_IPV4 = 2234 /* 0x8BA */,
    /**
     * IPv4 interface bring up fails because the network provided only the IPv6 address for the
     * upcoming PDN permanent client can reattempt a IPv4 call bring up after the IPv6 interface is
     * also brought down. However there is no guarantee that the network will provide a IPv4
     * address.
     */
    LIMITED_TO_IPV6 = 2235 /* 0x8BB */,
    /**
     * Data call bring up fails in the VSNCP phase due to a VSNCP timeout error.
     */
    VSNCP_TIMEOUT = 2236 /* 0x8BC */,
    /**
     * Data call bring up fails in the VSNCP phase due to a general error. It's used when there is
     * no other specific error code available to report the failure.
     */
    VSNCP_GEN_ERROR = 2237 /* 0x8BD */,
    /**
     * Data call bring up fails in the VSNCP phase due to a network rejection of the VSNCP
     * configuration request because the requested APN is unauthorized.
     */
    VSNCP_APN_UNATHORIZED = 2238 /* 0x8BE */,
    /**
     * Data call bring up fails in the VSNCP phase due to a network rejection of the VSNCP
     * configuration request because the PDN limit has been exceeded.
     */
    VSNCP_PDN_LIMIT_EXCEEDED = 2239 /* 0x8BF */,
    /**
     * Data call bring up fails in the VSNCP phase due to the network rejected the VSNCP
     * configuration request due to no PDN gateway address.
     */
    VSNCP_NO_PDN_GATEWAY_ADDRESS = 2240 /* 0x8C0 */,
    /**
     * Data call bring up fails in the VSNCP phase due to a network rejection of the VSNCP
     * configuration request because the PDN gateway is unreachable.
     */
    VSNCP_PDN_GATEWAY_UNREACHABLE = 2241 /* 0x8C1 */,
    /**
     * Data call bring up fails in the VSNCP phase due to a network rejection of the VSNCP
     * configuration request due to a PDN gateway reject.
     */
    VSNCP_PDN_GATEWAY_REJECT = 2242 /* 0x8C2 */,
    /**
     * Data call bring up fails in the VSNCP phase due to a network rejection of the VSNCP
     * configuration request with the reason of insufficient parameter.
     */
    VSNCP_INSUFFICIENT_PARAMETERS = 2243 /* 0x8C3 */,
    /**
     * Data call bring up fails in the VSNCP phase due to a network rejection of the VSNCP
     * configuration request with the reason of resource unavailable.
     */
    VSNCP_RESOURCE_UNAVAILABLE = 2244 /* 0x8C4 */,
    /**
     * Data call bring up fails in the VSNCP phase due to a network rejection of the VSNCP
     * configuration request with the reason of administratively prohibited at the HSGW.
     */
    VSNCP_ADMINISTRATIVELY_PROHIBITED = 2245 /* 0x8C5 */,
    /**
     * Data call bring up fails in the VSNCP phase due to a network rejection of PDN ID in use, or
     * all existing PDNs are brought down with this end reason because one of the PDN bring up was
     * rejected by the network with the reason of PDN ID in use.
     */
    VSNCP_PDN_ID_IN_USE = 2246 /* 0x8C6 */,
    /**
     * Data call bring up fails in the VSNCP phase due to a network rejection of the VSNCP
     * configuration request for the reason of subscriber limitation.
     */
    VSNCP_SUBSCRIBER_LIMITATION = 2247 /* 0x8C7 */,
    /**
     * Data call bring up fails in the VSNCP phase due to a network rejection of the VSNCP
     * configuration request because the PDN exists for this APN.
     */
    VSNCP_PDN_EXISTS_FOR_THIS_APN = 2248 /* 0x8C8 */,
    /**
     * Data call bring up fails in the VSNCP phase due to a network rejection of the VSNCP
     * configuration request with reconnect to this PDN not allowed, or an active data call is
     * terminated by the network because reconnection to this PDN is not allowed. Upon receiving
     * this error code from the network, the modem infinitely throttles the PDN until the next power
     * cycle.
     */
    VSNCP_RECONNECT_NOT_ALLOWED = 2249 /* 0x8C9 */,
    /**
     * Device failure to obtain the prefix from the network.
     */
    IPV6_PREFIX_UNAVAILABLE = 2250 /* 0x8CA */,
    /**
     * System preference change back to SRAT during handoff
     */
    HANDOFF_PREFERENCE_CHANGED = 2251 /* 0x8CB */,
    /**
     * Data call fail due to the slice not being allowed for the data call.
     */
    SLICE_REJECTED = 2252 /* 0x8CC */,
    /**
     * No matching rule available for the request, and match-all rule is not allowed for it.
     */
    MATCH_ALL_RULE_NOT_ALLOWED = 2253 /* 0x8CD */,
    /**
     * If connection failed for all matching URSP rules.
     */
    ALL_MATCHING_RULES_FAILED = 2254 /* 0x8CE */,
};

/**
 * Slice/Service Type as defined in 3GPP TS 23.501.
 */
enum class SliceServiceType : uint8_t {
    /*
     * Not specified
     */
    NONE = 0,
    /*
     * Slice suitable for the handling of 5G enhanced Mobile Broadband
     */
    EMBB = 1,
    /**
     * Slice suitable for the handling of ultra-reliable low latency
     * communications
     */
    URLLC = 2,
    /*
     * Slice suitable for the handling of massive IoT
     */
    MIOT = 3,
};

enum class SliceStatus : int8_t {
    UNKNOWN = 0,
    CONFIGURED = 1 /* ::android::hardware::radio::V1_6::SliceStatus.UNKNOWN implicitly + 1 */,
    ALLOWED = 2 /* ::android::hardware::radio::V1_6::SliceStatus.CONFIGURED implicitly + 1 */,
    REJECTED_NOT_AVAILABLE_IN_PLMN = 3 /* ::android::hardware::radio::V1_6::SliceStatus.ALLOWED implicitly + 1 */,
    REJECTED_NOT_AVAILABLE_IN_REG_AREA = 4 /* ::android::hardware::radio::V1_6::SliceStatus.REJECTED_NOT_AVAILABLE_IN_PLMN implicitly + 1 */,
    DEFAULT_CONFIGURED = 5 /* ::android::hardware::radio::V1_6::SliceStatus.REJECTED_NOT_AVAILABLE_IN_REG_AREA implicitly + 1 */,
};

/**
 * This struct represents a S-NSSAI as defined in 3GPP TS 24.501.
 */
struct SliceInfo final {
    /**
     * The type of service provided by the slice.
     *
     * see: 3GPP TS 24.501 Section 9.11.2.8.
     */
    ::android::hardware::radio::V1_6::SliceServiceType sst __attribute__ ((aligned(1)));
    /**
     * Slice differentiator is the identifier of a slice that has
     * SliceServiceType as SST. A value of -1 indicates that there is
     * no corresponding SliceInfo of the HPLMN.
     *
     * see: 3GPP TS 24.501 Section 9.11.2.8.
     */
    int32_t sliceDifferentiator __attribute__ ((aligned(4)));
    /**
     * This SST corresponds to a SliceInfo (S-NSSAI) of the HPLMN; the SST is
     * mapped to this value.
     *
     * see: 3GPP TS 24.501 Section 9.11.2.8.
     */
    ::android::hardware::radio::V1_6::SliceServiceType mappedHplmnSst __attribute__ ((aligned(1)));
    /**
     * Present only if both sliceDifferentiator and mappedHplmnSst are also
     * present. This SD corresponds to a SliceInfo (S-NSSAI) of the HPLMN;
     * sliceDifferentiator is mapped to this value. A value of -1 indicates that
     * there is no corresponding SliceInfo of the HPLMN.
     *
     * see: 3GPP TS 24.501 Section 9.11.2.8.
     */
    int32_t mappedHplmnSD __attribute__ ((aligned(4)));
    /**
     * Field to indicate the current status of the slice.
     */
    ::android::hardware::radio::V1_6::SliceStatus status __attribute__ ((aligned(1)));
};

static_assert(offsetof(::android::hardware::radio::V1_6::SliceInfo, sst) == 0, "wrong offset");
static_assert(offsetof(::android::hardware::radio::V1_6::SliceInfo, sliceDifferentiator) == 4, "wrong offset");
static_assert(offsetof(::android::hardware::radio::V1_6::SliceInfo, mappedHplmnSst) == 8, "wrong offset");
static_assert(offsetof(::android::hardware::radio::V1_6::SliceInfo, mappedHplmnSD) == 12, "wrong offset");
static_assert(offsetof(::android::hardware::radio::V1_6::SliceInfo, status) == 16, "wrong offset");
static_assert(sizeof(::android::hardware::radio::V1_6::SliceInfo) == 20, "wrong size");
static_assert(__alignof(::android::hardware::radio::V1_6::SliceInfo) == 4, "wrong alignment");

/**
 * This safe_union represents an optional slice info
 */
struct OptionalSliceInfo final {
    enum class hidl_discriminator : uint8_t {
        noinit = 0,  // ::android::hidl::safe_union::V1_0::Monostate
        value = 1,  // ::android::hardware::radio::V1_6::SliceInfo
    };

    OptionalSliceInfo();
    ~OptionalSliceInfo();
    OptionalSliceInfo(OptionalSliceInfo&&);
    OptionalSliceInfo(const OptionalSliceInfo&);
    OptionalSliceInfo& operator=(OptionalSliceInfo&&);
    OptionalSliceInfo& operator=(const OptionalSliceInfo&);

    void noinit(const ::android::hidl::safe_union::V1_0::Monostate&);
    void noinit(::android::hidl::safe_union::V1_0::Monostate&&);
    ::android::hidl::safe_union::V1_0::Monostate& noinit();
    const ::android::hidl::safe_union::V1_0::Monostate& noinit() const;

    void value(const ::android::hardware::radio::V1_6::SliceInfo&);
    void value(::android::hardware::radio::V1_6::SliceInfo&&);
    ::android::hardware::radio::V1_6::SliceInfo& value();
    const ::android::hardware::radio::V1_6::SliceInfo& value() const;

    // Utility methods
    hidl_discriminator getDiscriminator() const;

    constexpr size_t hidl_getUnionOffset() const {
        return offsetof(::android::hardware::radio::V1_6::OptionalSliceInfo, hidl_u);
    }

private:
    void hidl_destructUnion();

    hidl_discriminator hidl_d __attribute__ ((aligned(1))) ;
    union hidl_union final {
        ::android::hidl::safe_union::V1_0::Monostate noinit __attribute__ ((aligned(1)));
        ::android::hardware::radio::V1_6::SliceInfo value __attribute__ ((aligned(4)));

        hidl_union();
        ~hidl_union();
    } hidl_u;

    static_assert(sizeof(::android::hardware::radio::V1_6::OptionalSliceInfo::hidl_union) == 20, "wrong size");
    static_assert(__alignof(::android::hardware::radio::V1_6::OptionalSliceInfo::hidl_union) == 4, "wrong alignment");
    static_assert(sizeof(::android::hardware::radio::V1_6::OptionalSliceInfo::hidl_discriminator) == 1, "wrong size");
    static_assert(__alignof(::android::hardware::radio::V1_6::OptionalSliceInfo::hidl_discriminator) == 1, "wrong alignment");
};

static_assert(sizeof(::android::hardware::radio::V1_6::OptionalSliceInfo) == 24, "wrong size");
static_assert(__alignof(::android::hardware::radio::V1_6::OptionalSliceInfo) == 4, "wrong alignment");

/**
 * Overwritten from @1.5::SetupDataCallResult in order to change the suggestedRetryTime
 * to 64-bit value. In the future, this must be extended instead of overwritten.
 * Also added defaultQos, qosSessions, handoverFailureMode, pduSessionId, sliceInfo,
 * and traffic descriptors in this version.
 */
struct SetupDataCallResult final {
    /**
     * Data call fail cause. DataCallFailCause.NONE if no error.
     */
    ::android::hardware::radio::V1_6::DataCallFailCause cause __attribute__ ((aligned(4)));
    /**
     * If cause is not DataCallFailCause.NONE, this field indicates the network suggested data
     * retry back-off time in milliseconds. Negative value indicates network does not give any
     * suggestion. 0 indicates retry should be performed immediately. 0x7fffffffffffffff indicates
     * the device should not retry data setup anymore.
     *
     * During this time, no calls to IRadio@1.6::SetupDataCall for this APN will be made unless
     * IRadioIndication@1.6::unthrottleApn is sent with the same APN.
     */
    int64_t suggestedRetryTime __attribute__ ((aligned(8)));
    /**
     * Context ID, uniquely identifies this data connection.
     */
    int32_t cid __attribute__ ((aligned(4)));
    /**
     * Data connection active status.
     */
    ::android::hardware::radio::V1_4::DataConnActiveStatus active __attribute__ ((aligned(4)));
    /**
     * PDP protocol type. If cause is DataCallFailCause.ONLY_SINGLE_BEARER_ALLOWED, this is the
     * protocol type supported, such as "IP" or "IPV6".
     */
    ::android::hardware::radio::V1_4::PdpProtocolType type __attribute__ ((aligned(4)));
    /**
     * The network interface name.
     */
    ::android::hardware::hidl_string ifname __attribute__ ((aligned(8)));
    /**
     * List of link address.
     */
    ::android::hardware::hidl_vec<::android::hardware::radio::V1_5::LinkAddress> addresses __attribute__ ((aligned(8)));
    /**
     * List of DNS server addresses, e.g., "192.0.1.3" or "192.0.1.11 2001:db8::1". Empty if no dns
     * server addresses returned.
     */
    ::android::hardware::hidl_vec<::android::hardware::hidl_string> dnses __attribute__ ((aligned(8)));
    /**
     * List of default gateway addresses, e.g., "192.0.1.3" or "192.0.1.11 2001:db8::1".
     * When empty, the addresses represent point to point connections.
     */
    ::android::hardware::hidl_vec<::android::hardware::hidl_string> gateways __attribute__ ((aligned(8)));
    /**
     * List of P-CSCF(Proxy Call State Control Function) addresses via PCO(Protocol Configuration
     * Option), e.g., "2001:db8::1 2001:db8::2 2001:db8::3". Empty if not IMS client.
     */
    ::android::hardware::hidl_vec<::android::hardware::hidl_string> pcscf __attribute__ ((aligned(8)));
    /**
     * MTU received from network for IPv4.
     * Value <= 0 means network has either not sent a value or sent an invalid value.
     */
    int32_t mtuV4 __attribute__ ((aligned(4)));
    /**
     * MTU received from network for IPv6.
     * Value <= 0 means network has either not sent a value or sent an invalid value.
     */
    int32_t mtuV6 __attribute__ ((aligned(4)));
    /**
     * Default bearer QoS. Applicable to LTE and NR
     */
    ::android::hardware::radio::V1_6::Qos defaultQos __attribute__ ((aligned(4)));
    /**
     * Active QOS sessions of the dedicated bearers. Applicable to
     * PDNs that support dedicated bearers.
     */
    ::android::hardware::hidl_vec<::android::hardware::radio::V1_6::QosSession> qosSessions __attribute__ ((aligned(8)));
    /**
     * Specifies the fallback mode on an IWLAN handover failure.
     */
    ::android::hardware::radio::V1_6::HandoverFailureMode handoverFailureMode __attribute__ ((aligned(1)));
    /**
     * The allocated pdu session id for this data call.
     * A value of 0 means no pdu session id was attached to this call.
     *
     * Reference: 3GPP TS 24.007 section 11.2.3.1b
     */
    int32_t pduSessionId __attribute__ ((aligned(4)));
    /**
     * Slice used for this data call. It is valid only when this data call is on
     * AccessNetwork:NGRAN.
     */
    ::android::hardware::radio::V1_6::OptionalSliceInfo sliceInfo __attribute__ ((aligned(4)));
    /**
     * TrafficDescriptors for which this data call must be used. It only includes
     * the TDs for which a data call has been requested so far; it is not an
     * exhaustive list.
     */
    ::android::hardware::hidl_vec<::android::hardware::radio::V1_6::TrafficDescriptor> trafficDescriptors __attribute__ ((aligned(8)));
};

static_assert(offsetof(::android::hardware::radio::V1_6::SetupDataCallResult, cause) == 0, "wrong offset");
static_assert(offsetof(::android::hardware::radio::V1_6::SetupDataCallResult, suggestedRetryTime) == 8, "wrong offset");
static_assert(offsetof(::android::hardware::radio::V1_6::SetupDataCallResult, cid) == 16, "wrong offset");
static_assert(offsetof(::android::hardware::radio::V1_6::SetupDataCallResult, active) == 20, "wrong offset");
static_assert(offsetof(::android::hardware::radio::V1_6::SetupDataCallResult, type) == 24, "wrong offset");
static_assert(offsetof(::android::hardware::radio::V1_6::SetupDataCallResult, ifname) == 32, "wrong offset");
static_assert(offsetof(::android::hardware::radio::V1_6::SetupDataCallResult, addresses) == 48, "wrong offset");
static_assert(offsetof(::android::hardware::radio::V1_6::SetupDataCallResult, dnses) == 64, "wrong offset");
static_assert(offsetof(::android::hardware::radio::V1_6::SetupDataCallResult, gateways) == 80, "wrong offset");
static_assert(offsetof(::android::hardware::radio::V1_6::SetupDataCallResult, pcscf) == 96, "wrong offset");
static_assert(offsetof(::android::hardware::radio::V1_6::SetupDataCallResult, mtuV4) == 112, "wrong offset");
static_assert(offsetof(::android::hardware::radio::V1_6::SetupDataCallResult, mtuV6) == 116, "wrong offset");
static_assert(offsetof(::android::hardware::radio::V1_6::SetupDataCallResult, defaultQos) == 120, "wrong offset");
static_assert(offsetof(::android::hardware::radio::V1_6::SetupDataCallResult, qosSessions) == 152, "wrong offset");
static_assert(offsetof(::android::hardware::radio::V1_6::SetupDataCallResult, handoverFailureMode) == 168, "wrong offset");
static_assert(offsetof(::android::hardware::radio::V1_6::SetupDataCallResult, pduSessionId) == 172, "wrong offset");
static_assert(offsetof(::android::hardware::radio::V1_6::SetupDataCallResult, sliceInfo) == 176, "wrong offset");
static_assert(offsetof(::android::hardware::radio::V1_6::SetupDataCallResult, trafficDescriptors) == 200, "wrong offset");
static_assert(sizeof(::android::hardware::radio::V1_6::SetupDataCallResult) == 216, "wrong size");
static_assert(__alignof(::android::hardware::radio::V1_6::SetupDataCallResult) == 8, "wrong alignment");

/**
 * NR Dual connectivity state
 */
enum class NrDualConnectivityState : int8_t {
    /**
     * Enable NR dual connectivity. Enabled state does not mean dual connectivity
     * is active. It means device is allowed to connect to both primary and secondary.
     */
    ENABLE = 1,
    /**
     * Disable NR dual connectivity. Disabled state does not mean secondary cell is released.
     * Modem will release it only if current bearer is released to avoid radio link failure.
     */
    DISABLE = 2,
    /**
     * Disable NR dual connectivity and force secondary cell to be released if dual connectivity
     * was active. This may result in radio link failure.
     */
    DISABLE_IMMEDIATE = 3,
};

/**
 * Overwritten from @1.2::LinkCapacityEstimate to update LinkCapacityEstimate to 1.6 version.
 */
struct LinkCapacityEstimate final {
    /**
     * Estimated downlink capacity in kbps. In case of a dual connected network,
     * this includes capacity of both primary and secondary. This bandwidth estimate shall be
     * the estimated maximum sustainable link bandwidth (as would be measured
     * at the Upper PDCP or SNDCP SAP). If the DL Aggregate Maximum Bit Rate is known,
     * this value shall not exceed the DL-AMBR for the Internet PDN connection.
     * This must be filled with 0 if network is not connected.
     */
    uint32_t downlinkCapacityKbps __attribute__ ((aligned(4)));
    /**
     * Estimated uplink capacity in kbps. In case of a dual connected network,
     * this includes capacity of both primary and secondary. This bandwidth estimate shall be the
     * estimated maximum sustainable link bandwidth (as would be measured at the
     * Upper PDCP or SNDCP SAP). If the UL Aggregate Maximum Bit Rate is known,
     * this value shall not exceed the UL-AMBR for the Internet PDN connection.
     * This must be filled with 0 if network is not connected.
     */
    uint32_t uplinkCapacityKbps __attribute__ ((aligned(4)));
    /**
     * Estimated downlink capacity of secondary carrier in a dual connected NR mode in kbps.
     * This bandwidth estimate shall be the estimated maximum sustainable link bandwidth
     * (as would be measured at the Upper PDCP or SNDCP SAP). This is valid only
     * in if device is connected to both primary and secodary in dual connected
     * mode. This must be filled with 0 if secondary is not connected or if
     * modem does not support this feature.
     */
    uint32_t secondaryDownlinkCapacityKbps __attribute__ ((aligned(4)));
    /**
     * Estimated uplink capacity secondary carrier in a dual connected NR mode in kbps.
     * This bandwidth estimate shall be the estimated
     * maximum sustainable link bandwidth (as would be measured at the Upper PDCP or SNDCP SAP).
     * This is valid only in if device is connected to both primary and secodary in dual connected
     * mode.This must be filled with 0 if secondary is not connected or if modem
     * does not support this feature.
     */
    uint32_t secondaryUplinkCapacityKbps __attribute__ ((aligned(4)));
};

static_assert(offsetof(::android::hardware::radio::V1_6::LinkCapacityEstimate, downlinkCapacityKbps) == 0, "wrong offset");
static_assert(offsetof(::android::hardware::radio::V1_6::LinkCapacityEstimate, uplinkCapacityKbps) == 4, "wrong offset");
static_assert(offsetof(::android::hardware::radio::V1_6::LinkCapacityEstimate, secondaryDownlinkCapacityKbps) == 8, "wrong offset");
static_assert(offsetof(::android::hardware::radio::V1_6::LinkCapacityEstimate, secondaryUplinkCapacityKbps) == 12, "wrong offset");
static_assert(sizeof(::android::hardware::radio::V1_6::LinkCapacityEstimate) == 16, "wrong size");
static_assert(__alignof(::android::hardware::radio::V1_6::LinkCapacityEstimate) == 4, "wrong alignment");

enum class DataThrottlingAction : int8_t {
    /*
     * Clear all existing data throttling.
     */
    NO_DATA_THROTTLING = 0,
    /**
     * Enact secondary carrier data throttling and remove any existing data
     * throttling on anchor carrier.
     */
    THROTTLE_SECONDARY_CARRIER = 1,
    /**
     * Enact anchor carrier data throttling and disable data on secondary
     * carrier if currently enabled.
     */
    THROTTLE_ANCHOR_CARRIER = 2,
    /**
     * Immediately hold on to current level of throttling.
     */
    HOLD = 3,
};

/**
 * Defines the values for VoPS indicator of NR as per 3gpp spec 24.501 sec 9.10.3.5
 */
enum class VopsIndicator : uint8_t {
    /**
     * IMS voice over PS session not supported
     */
    VOPS_NOT_SUPPORTED = 0,
    /**
     * IMS voice over PS session supported over 3GPP access
     */
    VOPS_OVER_3GPP = 1,
    /**
     * IMS voice over PS session supported over non-3GPP access
     */
    VOPS_OVER_NON_3GPP = 2,
};

/**
 * Defines the values for emergency service indicator of NR
 * as per 3gpp spec 24.501 sec 9.10.3.5
 */
enum class EmcIndicator : uint8_t {
    /**
     * Emergency services not supported
     */
    EMC_NOT_SUPPORTED = 0,
    /**
     * Emergency services supported in NR connected to 5GCN only
     */
    EMC_NR_CONNECTED_TO_5GCN = 1,
    /**
     * Emergency services supported in E-UTRA connected to 5GCN only
     */
    EMC_EUTRA_CONNECTED_TO_5GCN = 2,
    /**
     * Emergency services supported in NR connected to 5GCN and E-UTRA connected to 5GCN
     */
    EMC_BOTH_NR_EUTRA_CONNECTED_TO_5GCN = 3,
};

/**
 * Defines the values for emergency service fallback indicator of NR
 * as per 3gpp spec 24.501 sec 9.10.3.5
 */
enum class EmfIndicator : uint8_t {
    /**
     * Emergency services fallback not supported
     */
    EMF_NOT_SUPPORTED = 0,
    /**
     * Emergency services fallback supported in NR connected to 5GCN only
     */
    EMF_NR_CONNECTED_TO_5GCN = 1,
    /**
     * Emergency services fallback supported in E-UTRA connected to 5GCN only
     */
    EMF_EUTRA_CONNECTED_TO_5GCN = 2,
    /**
     * Emergency services fallback supported in NR connected to 5GCN and E-UTRA
     * connected to 5GCN.
     */
    EMF_BOTH_NR_EUTRA_CONNECTED_TO_5GCN = 3,
};

/**
 * Type to define the NR specific network capabilities for voice over PS including
 * emergency and normal voice calls.
 */
struct NrVopsInfo final {
    /**
     * This indicates if the camped network supports VoNR services, and what kind of services
     * it supports. This information is received from NR network during NR NAS registration
     * procedure through NR REGISTRATION ACCEPT.
     * Refer 3GPP 24.501 EPS 5GS network feature support -> IMS VoPS
     */
    ::android::hardware::radio::V1_6::VopsIndicator vopsSupported __attribute__ ((aligned(1)));
    /**
     * This indicates if the camped network supports VoNR emergency service. This information
     * is received from NR network through two sources:
     * a. During NR NAS registration procedure through NR REGISTRATION ACCEPT.
     *    Refer 3GPP 24.501 EPS 5GS network feature support -> EMC
     * b. In case the device is not registered on the network.
     *    Refer 3GPP 38.331 SIB1 : ims-EmergencySupport
     *    If device is registered on NR, then this field indicates whether the cell
     *    supports IMS emergency bearer services for UEs in limited service mode.
     */
    ::android::hardware::radio::V1_6::EmcIndicator emcSupported __attribute__ ((aligned(1)));
    /**
     * This indicates if the camped network supports VoNR emergency service fallback. This
     * information is received from NR network during NR NAS registration procedure through
     * NR REGISTRATION ACCEPT.
     * Refer 3GPP 24.501 EPS 5GS network feature support -> EMF
     */
    ::android::hardware::radio::V1_6::EmfIndicator emfSupported __attribute__ ((aligned(1)));
};

static_assert(offsetof(::android::hardware::radio::V1_6::NrVopsInfo, vopsSupported) == 0, "wrong offset");
static_assert(offsetof(::android::hardware::radio::V1_6::NrVopsInfo, emcSupported) == 1, "wrong offset");
static_assert(offsetof(::android::hardware::radio::V1_6::NrVopsInfo, emfSupported) == 2, "wrong offset");
static_assert(sizeof(::android::hardware::radio::V1_6::NrVopsInfo) == 3, "wrong size");
static_assert(__alignof(::android::hardware::radio::V1_6::NrVopsInfo) == 1, "wrong alignment");

struct LteSignalStrength final {
    ::android::hardware::radio::V1_0::LteSignalStrength base __attribute__ ((aligned(4)));
    /**
     * CSI channel quality indicator (CQI) table index. There are multiple CQI tables.
     * The definition of CQI in each table is different.
     *
     * Reference: 3GPP TS 136.213 section 7.2.3.
     *
     * Range [1, 6], INT_MAX means invalid/unreported.
     */
    uint32_t cqiTableIndex __attribute__ ((aligned(4)));
};

static_assert(offsetof(::android::hardware::radio::V1_6::LteSignalStrength, base) == 0, "wrong offset");
static_assert(offsetof(::android::hardware::radio::V1_6::LteSignalStrength, cqiTableIndex) == 24, "wrong offset");
static_assert(sizeof(::android::hardware::radio::V1_6::LteSignalStrength) == 28, "wrong size");
static_assert(__alignof(::android::hardware::radio::V1_6::LteSignalStrength) == 4, "wrong alignment");

struct NrSignalStrength final {
    ::android::hardware::radio::V1_4::NrSignalStrength base __attribute__ ((aligned(4)));
    /**
     * CSI channel quality indicator (CQI) table index. There are multiple CQI tables.
     * The definition of CQI in each table is different.
     *
     * Reference: 3GPP TS 138.214 section 5.2.2.1.
     *
     * Range [1, 3], INT_MAX means invalid/unreported.
     */
    uint32_t csiCqiTableIndex __attribute__ ((aligned(4)));
    /**
     * CSI channel quality indicator (CQI) for all subbands.
     *
     * If the CQI report is for the entire wideband, a single CQI index is provided.
     * If the CQI report is for all subbands, one CQI index is provided for each subband,
     * in ascending order of subband index.
     * If CQI is not available, the CQI report is empty.
     *
     * Reference: 3GPP TS 138.214 section 5.2.2.1.
     *
     * Range [0, 15], 0xFF means invalid/unreported.
     */
    ::android::hardware::hidl_vec<uint8_t> csiCqiReport __attribute__ ((aligned(8)));
};

static_assert(offsetof(::android::hardware::radio::V1_6::NrSignalStrength, base) == 0, "wrong offset");
static_assert(offsetof(::android::hardware::radio::V1_6::NrSignalStrength, csiCqiTableIndex) == 24, "wrong offset");
static_assert(offsetof(::android::hardware::radio::V1_6::NrSignalStrength, csiCqiReport) == 32, "wrong offset");
static_assert(sizeof(::android::hardware::radio::V1_6::NrSignalStrength) == 48, "wrong size");
static_assert(__alignof(::android::hardware::radio::V1_6::NrSignalStrength) == 8, "wrong alignment");

/**
 * Overwritten from @1.4::SignalStrength in order to update LteSignalStrength and NrSignalStrength.
 */
struct SignalStrength final {
    /**
     * If GSM measurements are provided, this structure must contain valid measurements; otherwise
     * all fields should be set to INT_MAX to mark them as invalid.
     */
    ::android::hardware::radio::V1_0::GsmSignalStrength gsm __attribute__ ((aligned(4)));
    /**
     * If CDMA measurements are provided, this structure must contain valid measurements; otherwise
     * all fields should be set to INT_MAX to mark them as invalid.
     */
    ::android::hardware::radio::V1_0::CdmaSignalStrength cdma __attribute__ ((aligned(4)));
    /**
     * If EvDO measurements are provided, this structure must contain valid measurements; otherwise
     * all fields should be set to INT_MAX to mark them as invalid.
     */
    ::android::hardware::radio::V1_0::EvdoSignalStrength evdo __attribute__ ((aligned(4)));
    /**
     * If LTE measurements are provided, this structure must contain valid measurements; otherwise
     * all fields should be set to INT_MAX to mark them as invalid.
     */
    ::android::hardware::radio::V1_6::LteSignalStrength lte __attribute__ ((aligned(4)));
    /**
     * If TD-SCDMA measurements are provided, this structure must contain valid measurements;
     * otherwise all fields should be set to INT_MAX to mark them as invalid.
     */
    ::android::hardware::radio::V1_2::TdscdmaSignalStrength tdscdma __attribute__ ((aligned(4)));
    /**
     * If WCDMA measurements are provided, this structure must contain valid measurements; otherwise
     * all fields should be set to INT_MAX to mark them as invalid.
     */
    ::android::hardware::radio::V1_2::WcdmaSignalStrength wcdma __attribute__ ((aligned(4)));
    /**
     * If NR 5G measurements are provided, this structure must contain valid measurements; otherwise
     * all fields should be set to INT_MAX to mark them as invalid.
     */
    ::android::hardware::radio::V1_6::NrSignalStrength nr __attribute__ ((aligned(8)));
};

static_assert(offsetof(::android::hardware::radio::V1_6::SignalStrength, gsm) == 0, "wrong offset");
static_assert(offsetof(::android::hardware::radio::V1_6::SignalStrength, cdma) == 12, "wrong offset");
static_assert(offsetof(::android::hardware::radio::V1_6::SignalStrength, evdo) == 20, "wrong offset");
static_assert(offsetof(::android::hardware::radio::V1_6::SignalStrength, lte) == 32, "wrong offset");
static_assert(offsetof(::android::hardware::radio::V1_6::SignalStrength, tdscdma) == 60, "wrong offset");
static_assert(offsetof(::android::hardware::radio::V1_6::SignalStrength, wcdma) == 72, "wrong offset");
static_assert(offsetof(::android::hardware::radio::V1_6::SignalStrength, nr) == 88, "wrong offset");
static_assert(sizeof(::android::hardware::radio::V1_6::SignalStrength) == 136, "wrong size");
static_assert(__alignof(::android::hardware::radio::V1_6::SignalStrength) == 8, "wrong alignment");

/**
 * Overwritten from @1.5::CellInfoLte in order to update LteSignalStrength.
 */
struct CellInfoLte final {
    ::android::hardware::radio::V1_5::CellIdentityLte cellIdentityLte __attribute__ ((aligned(8)));
    ::android::hardware::radio::V1_6::LteSignalStrength signalStrengthLte __attribute__ ((aligned(4)));
};

static_assert(offsetof(::android::hardware::radio::V1_6::CellInfoLte, cellIdentityLte) == 0, "wrong offset");
static_assert(offsetof(::android::hardware::radio::V1_6::CellInfoLte, signalStrengthLte) == 160, "wrong offset");
static_assert(sizeof(::android::hardware::radio::V1_6::CellInfoLte) == 192, "wrong size");
static_assert(__alignof(::android::hardware::radio::V1_6::CellInfoLte) == 8, "wrong alignment");

/**
 * Overwritten from @1.5::CellInfoNr in order to update NrSignalStrength.
 */
struct CellInfoNr final {
    ::android::hardware::radio::V1_5::CellIdentityNr cellIdentityNr __attribute__ ((aligned(8)));
    ::android::hardware::radio::V1_6::NrSignalStrength signalStrengthNr __attribute__ ((aligned(8)));
};

static_assert(offsetof(::android::hardware::radio::V1_6::CellInfoNr, cellIdentityNr) == 0, "wrong offset");
static_assert(offsetof(::android::hardware::radio::V1_6::CellInfoNr, signalStrengthNr) == 120, "wrong offset");
static_assert(sizeof(::android::hardware::radio::V1_6::CellInfoNr) == 168, "wrong size");
static_assert(__alignof(::android::hardware::radio::V1_6::CellInfoNr) == 8, "wrong alignment");

/**
 * Overwritten from @1.5::CellInfo in order to update LteSignalStrength and NrSignalStrength.
 */
struct CellInfo final {
    // Forward declaration for forward reference support:
    struct CellInfoRatSpecificInfo;

    struct CellInfoRatSpecificInfo final {
        enum class hidl_discriminator : uint8_t {
            /**
             * 3gpp CellInfo types.
             */
            gsm = 0,  // ::android::hardware::radio::V1_5::CellInfoGsm
            wcdma = 1,  // ::android::hardware::radio::V1_5::CellInfoWcdma
            tdscdma = 2,  // ::android::hardware::radio::V1_5::CellInfoTdscdma
            lte = 3,  // ::android::hardware::radio::V1_6::CellInfoLte
            nr = 4,  // ::android::hardware::radio::V1_6::CellInfoNr
            /**
             * 3gpp2 CellInfo types;
             */
            cdma = 5,  // ::android::hardware::radio::V1_2::CellInfoCdma
        };

        CellInfoRatSpecificInfo();
        ~CellInfoRatSpecificInfo();
        CellInfoRatSpecificInfo(CellInfoRatSpecificInfo&&);
        CellInfoRatSpecificInfo(const CellInfoRatSpecificInfo&);
        CellInfoRatSpecificInfo& operator=(CellInfoRatSpecificInfo&&);
        CellInfoRatSpecificInfo& operator=(const CellInfoRatSpecificInfo&);

        void gsm(const ::android::hardware::radio::V1_5::CellInfoGsm&);
        void gsm(::android::hardware::radio::V1_5::CellInfoGsm&&);
        ::android::hardware::radio::V1_5::CellInfoGsm& gsm();
        const ::android::hardware::radio::V1_5::CellInfoGsm& gsm() const;

        void wcdma(const ::android::hardware::radio::V1_5::CellInfoWcdma&);
        void wcdma(::android::hardware::radio::V1_5::CellInfoWcdma&&);
        ::android::hardware::radio::V1_5::CellInfoWcdma& wcdma();
        const ::android::hardware::radio::V1_5::CellInfoWcdma& wcdma() const;

        void tdscdma(const ::android::hardware::radio::V1_5::CellInfoTdscdma&);
        void tdscdma(::android::hardware::radio::V1_5::CellInfoTdscdma&&);
        ::android::hardware::radio::V1_5::CellInfoTdscdma& tdscdma();
        const ::android::hardware::radio::V1_5::CellInfoTdscdma& tdscdma() const;

        void lte(const ::android::hardware::radio::V1_6::CellInfoLte&);
        void lte(::android::hardware::radio::V1_6::CellInfoLte&&);
        ::android::hardware::radio::V1_6::CellInfoLte& lte();
        const ::android::hardware::radio::V1_6::CellInfoLte& lte() const;

        void nr(const ::android::hardware::radio::V1_6::CellInfoNr&);
        void nr(::android::hardware::radio::V1_6::CellInfoNr&&);
        ::android::hardware::radio::V1_6::CellInfoNr& nr();
        const ::android::hardware::radio::V1_6::CellInfoNr& nr() const;

        void cdma(const ::android::hardware::radio::V1_2::CellInfoCdma&);
        void cdma(::android::hardware::radio::V1_2::CellInfoCdma&&);
        ::android::hardware::radio::V1_2::CellInfoCdma& cdma();
        const ::android::hardware::radio::V1_2::CellInfoCdma& cdma() const;

        // Utility methods
        hidl_discriminator getDiscriminator() const;

        constexpr size_t hidl_getUnionOffset() const {
            return offsetof(::android::hardware::radio::V1_6::CellInfo::CellInfoRatSpecificInfo, hidl_u);
        }

    private:
        void hidl_destructUnion();

        hidl_discriminator hidl_d __attribute__ ((aligned(1))) ;
        union hidl_union final {
            ::android::hardware::radio::V1_5::CellInfoGsm gsm __attribute__ ((aligned(8)));
            ::android::hardware::radio::V1_5::CellInfoWcdma wcdma __attribute__ ((aligned(8)));
            ::android::hardware::radio::V1_5::CellInfoTdscdma tdscdma __attribute__ ((aligned(8)));
            ::android::hardware::radio::V1_6::CellInfoLte lte __attribute__ ((aligned(8)));
            ::android::hardware::radio::V1_6::CellInfoNr nr __attribute__ ((aligned(8)));
            ::android::hardware::radio::V1_2::CellInfoCdma cdma __attribute__ ((aligned(8)));

            hidl_union();
            ~hidl_union();
        } hidl_u;

        static_assert(sizeof(::android::hardware::radio::V1_6::CellInfo::CellInfoRatSpecificInfo::hidl_union) == 192, "wrong size");
        static_assert(__alignof(::android::hardware::radio::V1_6::CellInfo::CellInfoRatSpecificInfo::hidl_union) == 8, "wrong alignment");
        static_assert(sizeof(::android::hardware::radio::V1_6::CellInfo::CellInfoRatSpecificInfo::hidl_discriminator) == 1, "wrong size");
        static_assert(__alignof(::android::hardware::radio::V1_6::CellInfo::CellInfoRatSpecificInfo::hidl_discriminator) == 1, "wrong alignment");
    };

    static_assert(sizeof(::android::hardware::radio::V1_6::CellInfo::CellInfoRatSpecificInfo) == 200, "wrong size");
    static_assert(__alignof(::android::hardware::radio::V1_6::CellInfo::CellInfoRatSpecificInfo) == 8, "wrong alignment");

    /**
     * True if this cell is registered false if not registered.
     */
    bool registered __attribute__ ((aligned(1)));
    /**
     * Connection status for the cell.
     */
    ::android::hardware::radio::V1_2::CellConnectionStatus connectionStatus __attribute__ ((aligned(4)));
    ::android::hardware::radio::V1_6::CellInfo::CellInfoRatSpecificInfo ratSpecificInfo __attribute__ ((aligned(8)));
};

static_assert(offsetof(::android::hardware::radio::V1_6::CellInfo, registered) == 0, "wrong offset");
static_assert(offsetof(::android::hardware::radio::V1_6::CellInfo, connectionStatus) == 4, "wrong offset");
static_assert(offsetof(::android::hardware::radio::V1_6::CellInfo, ratSpecificInfo) == 8, "wrong offset");
static_assert(sizeof(::android::hardware::radio::V1_6::CellInfo) == 208, "wrong size");
static_assert(__alignof(::android::hardware::radio::V1_6::CellInfo) == 8, "wrong alignment");

/**
 * Overwritten from @1.5::NetworkScanResult in order to update the CellInfo to 1.6 version.
 */
struct NetworkScanResult final {
    /**
     * The status of the scan.
     */
    ::android::hardware::radio::V1_1::ScanStatus status __attribute__ ((aligned(4)));
    /**
     * The error code of the incremental result.
     */
    ::android::hardware::radio::V1_6::RadioError error __attribute__ ((aligned(4)));
    /**
     * List of network information as CellInfo.
     */
    ::android::hardware::hidl_vec<::android::hardware::radio::V1_6::CellInfo> networkInfos __attribute__ ((aligned(8)));
};

static_assert(offsetof(::android::hardware::radio::V1_6::NetworkScanResult, status) == 0, "wrong offset");
static_assert(offsetof(::android::hardware::radio::V1_6::NetworkScanResult, error) == 4, "wrong offset");
static_assert(offsetof(::android::hardware::radio::V1_6::NetworkScanResult, networkInfos) == 8, "wrong offset");
static_assert(sizeof(::android::hardware::radio::V1_6::NetworkScanResult) == 24, "wrong size");
static_assert(__alignof(::android::hardware::radio::V1_6::NetworkScanResult) == 8, "wrong alignment");

/**
 * Overwritten from @1.5::RegStateResult to 1.6 to support NrRegistrationInfo
 * version.
 */
struct RegStateResult final {
    // Forward declaration for forward reference support:
    struct AccessTechnologySpecificInfo;

    struct AccessTechnologySpecificInfo final {
        enum class hidl_discriminator : uint8_t {
            noinit = 0,  // ::android::hidl::safe_union::V1_0::Monostate
            cdmaInfo = 1,  // ::android::hardware::radio::V1_5::RegStateResult::AccessTechnologySpecificInfo::Cdma2000RegistrationInfo
            eutranInfo = 2,  // ::android::hardware::radio::V1_5::RegStateResult::AccessTechnologySpecificInfo::EutranRegistrationInfo
            /**
             * Network capabilities for voice over PS services. This info is valid only on NR
             * network and must be present when the device is camped on NR. VopsInfo must be
             * empty when the device is not camped on NR.
             */
            ngranNrVopsInfo = 3,  // ::android::hardware::radio::V1_6::NrVopsInfo
            /**
             * True if the dual transfer mode is supported.
             * Refer to 3GPP TS 44.108 section 3.4.25.3
             */
            geranDtmSupported = 4,  // bool
        };

        AccessTechnologySpecificInfo();
        ~AccessTechnologySpecificInfo();
        AccessTechnologySpecificInfo(AccessTechnologySpecificInfo&&);
        AccessTechnologySpecificInfo(const AccessTechnologySpecificInfo&);
        AccessTechnologySpecificInfo& operator=(AccessTechnologySpecificInfo&&);
        AccessTechnologySpecificInfo& operator=(const AccessTechnologySpecificInfo&);

        void noinit(const ::android::hidl::safe_union::V1_0::Monostate&);
        void noinit(::android::hidl::safe_union::V1_0::Monostate&&);
        ::android::hidl::safe_union::V1_0::Monostate& noinit();
        const ::android::hidl::safe_union::V1_0::Monostate& noinit() const;

        void cdmaInfo(const ::android::hardware::radio::V1_5::RegStateResult::AccessTechnologySpecificInfo::Cdma2000RegistrationInfo&);
        void cdmaInfo(::android::hardware::radio::V1_5::RegStateResult::AccessTechnologySpecificInfo::Cdma2000RegistrationInfo&&);
        ::android::hardware::radio::V1_5::RegStateResult::AccessTechnologySpecificInfo::Cdma2000RegistrationInfo& cdmaInfo();
        const ::android::hardware::radio::V1_5::RegStateResult::AccessTechnologySpecificInfo::Cdma2000RegistrationInfo& cdmaInfo() const;

        void eutranInfo(const ::android::hardware::radio::V1_5::RegStateResult::AccessTechnologySpecificInfo::EutranRegistrationInfo&);
        void eutranInfo(::android::hardware::radio::V1_5::RegStateResult::AccessTechnologySpecificInfo::EutranRegistrationInfo&&);
        ::android::hardware::radio::V1_5::RegStateResult::AccessTechnologySpecificInfo::EutranRegistrationInfo& eutranInfo();
        const ::android::hardware::radio::V1_5::RegStateResult::AccessTechnologySpecificInfo::EutranRegistrationInfo& eutranInfo() const;

        void ngranNrVopsInfo(const ::android::hardware::radio::V1_6::NrVopsInfo&);
        void ngranNrVopsInfo(::android::hardware::radio::V1_6::NrVopsInfo&&);
        ::android::hardware::radio::V1_6::NrVopsInfo& ngranNrVopsInfo();
        const ::android::hardware::radio::V1_6::NrVopsInfo& ngranNrVopsInfo() const;

        void geranDtmSupported(bool);
        bool& geranDtmSupported();
        bool geranDtmSupported() const;

        // Utility methods
        hidl_discriminator getDiscriminator() const;

        constexpr size_t hidl_getUnionOffset() const {
            return offsetof(::android::hardware::radio::V1_6::RegStateResult::AccessTechnologySpecificInfo, hidl_u);
        }

    private:
        void hidl_destructUnion();

        hidl_discriminator hidl_d __attribute__ ((aligned(1))) ;
        union hidl_union final {
            ::android::hidl::safe_union::V1_0::Monostate noinit __attribute__ ((aligned(1)));
            ::android::hardware::radio::V1_5::RegStateResult::AccessTechnologySpecificInfo::Cdma2000RegistrationInfo cdmaInfo __attribute__ ((aligned(4)));
            ::android::hardware::radio::V1_5::RegStateResult::AccessTechnologySpecificInfo::EutranRegistrationInfo eutranInfo __attribute__ ((aligned(1)));
            ::android::hardware::radio::V1_6::NrVopsInfo ngranNrVopsInfo __attribute__ ((aligned(1)));
            bool geranDtmSupported __attribute__ ((aligned(1)));

            hidl_union();
            ~hidl_union();
        } hidl_u;

        static_assert(sizeof(::android::hardware::radio::V1_6::RegStateResult::AccessTechnologySpecificInfo::hidl_union) == 16, "wrong size");
        static_assert(__alignof(::android::hardware::radio::V1_6::RegStateResult::AccessTechnologySpecificInfo::hidl_union) == 4, "wrong alignment");
        static_assert(sizeof(::android::hardware::radio::V1_6::RegStateResult::AccessTechnologySpecificInfo::hidl_discriminator) == 1, "wrong size");
        static_assert(__alignof(::android::hardware::radio::V1_6::RegStateResult::AccessTechnologySpecificInfo::hidl_discriminator) == 1, "wrong alignment");
    };

    static_assert(sizeof(::android::hardware::radio::V1_6::RegStateResult::AccessTechnologySpecificInfo) == 20, "wrong size");
    static_assert(__alignof(::android::hardware::radio::V1_6::RegStateResult::AccessTechnologySpecificInfo) == 4, "wrong alignment");

    /**
     * Registration state
     *
     * If the RAT is indicated as a GERAN, UTRAN, or CDMA2000 technology, this value reports
     * registration in the Circuit-switched domain.
     * If the RAT is indicated as an EUTRAN, NGRAN, or another technology that does not support
     * circuit-switched services, this value reports registration in the Packet-switched domain.
     */
    ::android::hardware::radio::V1_0::RegState regState __attribute__ ((aligned(4)));
    /**
     * Indicates the available voice radio technology, valid values as defined by RadioTechnology,
     * except LTE_CA, which is no longer a valid value on 1.5 or above. When the device is on
     * carrier aggregation, vendor RIL service should properly report multiple PhysicalChannelConfig
     * elements through IRadio::currentPhysicalChannelConfigs_1_6.
     */
    ::android::hardware::radio::V1_4::RadioTechnology rat __attribute__ ((aligned(4)));
    /**
     * Cause code reported by the network in case registration fails. This will be a mobility
     * management cause code defined for MM, GMM, MME or equivalent as appropriate for the RAT.
     */
    ::android::hardware::radio::V1_5::RegistrationFailCause reasonForDenial __attribute__ ((aligned(4)));
    /**
     * CellIdentity
     */
    ::android::hardware::radio::V1_5::CellIdentity cellIdentity __attribute__ ((aligned(8)));
    /**
     * The most-recent PLMN-ID upon which the UE registered (or attempted to register if a failure
     * is reported in the reasonForDenial field). This PLMN shall be in standard format consisting
     * of a 3 digit MCC concatenated with a 2 or 3 digit MNC.
     */
    ::android::hardware::hidl_string registeredPlmn __attribute__ ((aligned(8)));
    /**
     * Access-technology-specific registration information, such as for CDMA2000.
     */
    ::android::hardware::radio::V1_6::RegStateResult::AccessTechnologySpecificInfo accessTechnologySpecificInfo __attribute__ ((aligned(4)));
};

static_assert(offsetof(::android::hardware::radio::V1_6::RegStateResult, regState) == 0, "wrong offset");
static_assert(offsetof(::android::hardware::radio::V1_6::RegStateResult, rat) == 4, "wrong offset");
static_assert(offsetof(::android::hardware::radio::V1_6::RegStateResult, reasonForDenial) == 8, "wrong offset");
static_assert(offsetof(::android::hardware::radio::V1_6::RegStateResult, cellIdentity) == 16, "wrong offset");
static_assert(offsetof(::android::hardware::radio::V1_6::RegStateResult, registeredPlmn) == 184, "wrong offset");
static_assert(offsetof(::android::hardware::radio::V1_6::RegStateResult, accessTechnologySpecificInfo) == 200, "wrong offset");
static_assert(sizeof(::android::hardware::radio::V1_6::RegStateResult) == 224, "wrong size");
static_assert(__alignof(::android::hardware::radio::V1_6::RegStateResult) == 8, "wrong alignment");

struct Call final {
    ::android::hardware::radio::V1_2::Call base __attribute__ ((aligned(8)));
    /**
     * Forwarded number. It can set only one forwarded number based on 3GPP rule of the CS.
     * Reference: 3GPP TS 24.008 section 10.5.4.21b
     */
    ::android::hardware::hidl_string forwardedNumber __attribute__ ((aligned(8)));
};

static_assert(offsetof(::android::hardware::radio::V1_6::Call, base) == 0, "wrong offset");
static_assert(offsetof(::android::hardware::radio::V1_6::Call, forwardedNumber) == 96, "wrong offset");
static_assert(sizeof(::android::hardware::radio::V1_6::Call) == 112, "wrong size");
static_assert(__alignof(::android::hardware::radio::V1_6::Call) == 8, "wrong alignment");

/**
 * Extended from @1.5 NgranBands
 * IRadio 1.6 supports NGRAN bands up to V16.5.0
 */
enum class NgranBands : int32_t {
    /**
     * 3GPP TS 38.101-1, Table 5.2-1: FR1 bands
     */
    BAND_1 = 1,
    BAND_2 = 2,
    BAND_3 = 3,
    BAND_5 = 5,
    BAND_7 = 7,
    BAND_8 = 8,
    BAND_12 = 12,
    BAND_14 = 14,
    BAND_18 = 18,
    BAND_20 = 20,
    BAND_25 = 25,
    BAND_28 = 28,
    BAND_29 = 29,
    BAND_30 = 30,
    BAND_34 = 34,
    BAND_38 = 38,
    BAND_39 = 39,
    BAND_40 = 40,
    BAND_41 = 41,
    BAND_48 = 48,
    BAND_50 = 50,
    BAND_51 = 51,
    BAND_65 = 65,
    BAND_66 = 66,
    BAND_70 = 70,
    BAND_71 = 71,
    BAND_74 = 74,
    BAND_75 = 75,
    BAND_76 = 76,
    BAND_77 = 77,
    BAND_78 = 78,
    BAND_79 = 79,
    BAND_80 = 80,
    BAND_81 = 81,
    BAND_82 = 82,
    BAND_83 = 83,
    BAND_84 = 84,
    BAND_86 = 86,
    BAND_89 = 89,
    BAND_90 = 90,
    BAND_91 = 91,
    BAND_92 = 92,
    BAND_93 = 93,
    BAND_94 = 94,
    BAND_95 = 95,
    /**
     * 3GPP TS 38.101-2, Table 5.2-1: FR2 bands
     */
    BAND_257 = 257,
    BAND_258 = 258,
    BAND_260 = 260,
    BAND_261 = 261,
    /**
     * 3GPP TS 38.101-1, Table 5.2-1: FR1 bands
     */
    BAND_26 = 26,
    BAND_46 = 46,
    BAND_53 = 53,
    BAND_96 = 96,
};

struct PhysicalChannelConfig final {
    // Forward declaration for forward reference support:
    struct Band;

    struct Band final {
        enum class hidl_discriminator : uint8_t {
            /**
             * Valid only if radioAccessNetwork = GERAN.
             */
            geranBand = 0,  // ::android::hardware::radio::V1_1::GeranBands
            /**
             * Valid only if radioAccessNetwork = UTRAN.
             */
            utranBand = 1,  // ::android::hardware::radio::V1_1::UtranBands
            /**
             * Valid only if radioAccessNetwork = EUTRAN.
             */
            eutranBand = 2,  // ::android::hardware::radio::V1_1::EutranBands
            /**
             * Valid only if radioAccessNetwork = NGRAN.
             */
            ngranBand = 3,  // ::android::hardware::radio::V1_6::NgranBands
        };

        Band();
        ~Band();
        Band(Band&&);
        Band(const Band&);
        Band& operator=(Band&&);
        Band& operator=(const Band&);

        void geranBand(::android::hardware::radio::V1_1::GeranBands);
        ::android::hardware::radio::V1_1::GeranBands& geranBand();
        ::android::hardware::radio::V1_1::GeranBands geranBand() const;

        void utranBand(::android::hardware::radio::V1_1::UtranBands);
        ::android::hardware::radio::V1_1::UtranBands& utranBand();
        ::android::hardware::radio::V1_1::UtranBands utranBand() const;

        void eutranBand(::android::hardware::radio::V1_1::EutranBands);
        ::android::hardware::radio::V1_1::EutranBands& eutranBand();
        ::android::hardware::radio::V1_1::EutranBands eutranBand() const;

        void ngranBand(::android::hardware::radio::V1_6::NgranBands);
        ::android::hardware::radio::V1_6::NgranBands& ngranBand();
        ::android::hardware::radio::V1_6::NgranBands ngranBand() const;

        // Utility methods
        hidl_discriminator getDiscriminator() const;

        constexpr size_t hidl_getUnionOffset() const {
            return offsetof(::android::hardware::radio::V1_6::PhysicalChannelConfig::Band, hidl_u);
        }

    private:
        void hidl_destructUnion();

        hidl_discriminator hidl_d __attribute__ ((aligned(1))) ;
        union hidl_union final {
            ::android::hardware::radio::V1_1::GeranBands geranBand __attribute__ ((aligned(4)));
            ::android::hardware::radio::V1_1::UtranBands utranBand __attribute__ ((aligned(4)));
            ::android::hardware::radio::V1_1::EutranBands eutranBand __attribute__ ((aligned(4)));
            ::android::hardware::radio::V1_6::NgranBands ngranBand __attribute__ ((aligned(4)));

            hidl_union();
            ~hidl_union();
        } hidl_u;

        static_assert(sizeof(::android::hardware::radio::V1_6::PhysicalChannelConfig::Band::hidl_union) == 4, "wrong size");
        static_assert(__alignof(::android::hardware::radio::V1_6::PhysicalChannelConfig::Band::hidl_union) == 4, "wrong alignment");
        static_assert(sizeof(::android::hardware::radio::V1_6::PhysicalChannelConfig::Band::hidl_discriminator) == 1, "wrong size");
        static_assert(__alignof(::android::hardware::radio::V1_6::PhysicalChannelConfig::Band::hidl_discriminator) == 1, "wrong alignment");
    };

    static_assert(sizeof(::android::hardware::radio::V1_6::PhysicalChannelConfig::Band) == 8, "wrong size");
    static_assert(__alignof(::android::hardware::radio::V1_6::PhysicalChannelConfig::Band) == 4, "wrong alignment");

    /**
     * Connection status for cell. Valid values are PRIMARY_SERVING and SECONDARY_SERVING
     */
    ::android::hardware::radio::V1_2::CellConnectionStatus status __attribute__ ((aligned(4)));
    /**
     * The radio technology for this physical channel
     */
    ::android::hardware::radio::V1_4::RadioTechnology rat __attribute__ ((aligned(4)));
    /**
     * Downlink Absolute Radio Frequency Channel Number
     */
    int32_t downlinkChannelNumber __attribute__ ((aligned(4)));
    /**
     * Uplink Absolute Radio Frequency Channel Number
     */
    int32_t uplinkChannelNumber __attribute__ ((aligned(4)));
    /**
     * Downlink cell bandwidth, in kHz
     */
    int32_t cellBandwidthDownlinkKhz __attribute__ ((aligned(4)));
    /**
     * Uplink cell bandwidth, in kHz
     */
    int32_t cellBandwidthUplinkKhz __attribute__ ((aligned(4)));
    /**
     * A list of data calls mapped to this physical channel. The context id must match the cid of
     * @1.5::SetupDataCallResult. An empty list means the physical channel has no data call mapped
     * to it.
     */
    ::android::hardware::hidl_vec<int32_t> contextIds __attribute__ ((aligned(8)));
    /**
     * The physical cell identifier for this cell.
     *
     * In UTRAN, this value is primary scrambling code. The range is [0, 511].
     * Reference: 3GPP TS 25.213 section 5.2.2.
     *
     * In EUTRAN, this value is physical layer cell identity. The range is [0, 503].
     * Reference: 3GPP TS 36.211 section 6.11.
     *
     * In 5G RAN, this value is physical layer cell identity. The range is [0, 1007].
     * Reference: 3GPP TS 38.211 section 7.4.2.1.
     */
    uint32_t physicalCellId __attribute__ ((aligned(4)));
    /**
     * The frequency band to scan.
     */
    ::android::hardware::radio::V1_6::PhysicalChannelConfig::Band band __attribute__ ((aligned(4)));
};

static_assert(offsetof(::android::hardware::radio::V1_6::PhysicalChannelConfig, status) == 0, "wrong offset");
static_assert(offsetof(::android::hardware::radio::V1_6::PhysicalChannelConfig, rat) == 4, "wrong offset");
static_assert(offsetof(::android::hardware::radio::V1_6::PhysicalChannelConfig, downlinkChannelNumber) == 8, "wrong offset");
static_assert(offsetof(::android::hardware::radio::V1_6::PhysicalChannelConfig, uplinkChannelNumber) == 12, "wrong offset");
static_assert(offsetof(::android::hardware::radio::V1_6::PhysicalChannelConfig, cellBandwidthDownlinkKhz) == 16, "wrong offset");
static_assert(offsetof(::android::hardware::radio::V1_6::PhysicalChannelConfig, cellBandwidthUplinkKhz) == 20, "wrong offset");
static_assert(offsetof(::android::hardware::radio::V1_6::PhysicalChannelConfig, contextIds) == 24, "wrong offset");
static_assert(offsetof(::android::hardware::radio::V1_6::PhysicalChannelConfig, physicalCellId) == 40, "wrong offset");
static_assert(offsetof(::android::hardware::radio::V1_6::PhysicalChannelConfig, band) == 44, "wrong offset");
static_assert(sizeof(::android::hardware::radio::V1_6::PhysicalChannelConfig) == 56, "wrong size");
static_assert(__alignof(::android::hardware::radio::V1_6::PhysicalChannelConfig) == 8, "wrong alignment");

/**
 * This safe_union represents an optional DNN. DNN stands for Data Network Name
 * and represents an APN as defined in 3GPP TS 23.003.
 */
struct OptionalDnn final {
    enum class hidl_discriminator : uint8_t {
        noinit = 0,  // ::android::hidl::safe_union::V1_0::Monostate
        value = 1,  // ::android::hardware::hidl_string
    };

    OptionalDnn();
    ~OptionalDnn();
    OptionalDnn(OptionalDnn&&);
    OptionalDnn(const OptionalDnn&);
    OptionalDnn& operator=(OptionalDnn&&);
    OptionalDnn& operator=(const OptionalDnn&);

    void noinit(const ::android::hidl::safe_union::V1_0::Monostate&);
    void noinit(::android::hidl::safe_union::V1_0::Monostate&&);
    ::android::hidl::safe_union::V1_0::Monostate& noinit();
    const ::android::hidl::safe_union::V1_0::Monostate& noinit() const;

    void value(const ::android::hardware::hidl_string&);
    void value(::android::hardware::hidl_string&&);
    ::android::hardware::hidl_string& value();
    const ::android::hardware::hidl_string& value() const;

    // Utility methods
    hidl_discriminator getDiscriminator() const;

    constexpr size_t hidl_getUnionOffset() const {
        return offsetof(::android::hardware::radio::V1_6::OptionalDnn, hidl_u);
    }

private:
    void hidl_destructUnion();

    hidl_discriminator hidl_d __attribute__ ((aligned(1))) ;
    union hidl_union final {
        ::android::hidl::safe_union::V1_0::Monostate noinit __attribute__ ((aligned(1)));
        ::android::hardware::hidl_string value __attribute__ ((aligned(8)));

        hidl_union();
        ~hidl_union();
    } hidl_u;

    static_assert(sizeof(::android::hardware::radio::V1_6::OptionalDnn::hidl_union) == 16, "wrong size");
    static_assert(__alignof(::android::hardware::radio::V1_6::OptionalDnn::hidl_union) == 8, "wrong alignment");
    static_assert(sizeof(::android::hardware::radio::V1_6::OptionalDnn::hidl_discriminator) == 1, "wrong size");
    static_assert(__alignof(::android::hardware::radio::V1_6::OptionalDnn::hidl_discriminator) == 1, "wrong alignment");
};

static_assert(sizeof(::android::hardware::radio::V1_6::OptionalDnn) == 24, "wrong size");
static_assert(__alignof(::android::hardware::radio::V1_6::OptionalDnn) == 8, "wrong alignment");

/**
 * This struct represents the OsId + OsAppId as defined in TS 24.526 Section 5.2
 */
struct OsAppId final {
    /**
     * Byte array representing OsId + OsAppId. The minimum length of the array is
     * 18 and maximum length is 272 (16 bytes for OsId + 1 byte for OsAppId length
     * + up to 255 bytes for OsAppId).
     */
    ::android::hardware::hidl_vec<uint8_t> osAppId __attribute__ ((aligned(8)));
};

static_assert(offsetof(::android::hardware::radio::V1_6::OsAppId, osAppId) == 0, "wrong offset");
static_assert(sizeof(::android::hardware::radio::V1_6::OsAppId) == 16, "wrong size");
static_assert(__alignof(::android::hardware::radio::V1_6::OsAppId) == 8, "wrong alignment");

/**
 * This safe_union represents an optional OsAppId.
 */
struct OptionalOsAppId final {
    enum class hidl_discriminator : uint8_t {
        noinit = 0,  // ::android::hidl::safe_union::V1_0::Monostate
        value = 1,  // ::android::hardware::radio::V1_6::OsAppId
    };

    OptionalOsAppId();
    ~OptionalOsAppId();
    OptionalOsAppId(OptionalOsAppId&&);
    OptionalOsAppId(const OptionalOsAppId&);
    OptionalOsAppId& operator=(OptionalOsAppId&&);
    OptionalOsAppId& operator=(const OptionalOsAppId&);

    void noinit(const ::android::hidl::safe_union::V1_0::Monostate&);
    void noinit(::android::hidl::safe_union::V1_0::Monostate&&);
    ::android::hidl::safe_union::V1_0::Monostate& noinit();
    const ::android::hidl::safe_union::V1_0::Monostate& noinit() const;

    void value(const ::android::hardware::radio::V1_6::OsAppId&);
    void value(::android::hardware::radio::V1_6::OsAppId&&);
    ::android::hardware::radio::V1_6::OsAppId& value();
    const ::android::hardware::radio::V1_6::OsAppId& value() const;

    // Utility methods
    hidl_discriminator getDiscriminator() const;

    constexpr size_t hidl_getUnionOffset() const {
        return offsetof(::android::hardware::radio::V1_6::OptionalOsAppId, hidl_u);
    }

private:
    void hidl_destructUnion();

    hidl_discriminator hidl_d __attribute__ ((aligned(1))) ;
    union hidl_union final {
        ::android::hidl::safe_union::V1_0::Monostate noinit __attribute__ ((aligned(1)));
        ::android::hardware::radio::V1_6::OsAppId value __attribute__ ((aligned(8)));

        hidl_union();
        ~hidl_union();
    } hidl_u;

    static_assert(sizeof(::android::hardware::radio::V1_6::OptionalOsAppId::hidl_union) == 16, "wrong size");
    static_assert(__alignof(::android::hardware::radio::V1_6::OptionalOsAppId::hidl_union) == 8, "wrong alignment");
    static_assert(sizeof(::android::hardware::radio::V1_6::OptionalOsAppId::hidl_discriminator) == 1, "wrong size");
    static_assert(__alignof(::android::hardware::radio::V1_6::OptionalOsAppId::hidl_discriminator) == 1, "wrong alignment");
};

static_assert(sizeof(::android::hardware::radio::V1_6::OptionalOsAppId) == 24, "wrong size");
static_assert(__alignof(::android::hardware::radio::V1_6::OptionalOsAppId) == 8, "wrong alignment");

/**
 * This struct represents a traffic descriptor. A valid struct must have at least
 * one of the optional values present. This is based on the definition of traffic
 * descriptor in TS 24.526 Section 5.2.
 */
struct TrafficDescriptor final {
    /**
     * DNN stands for Data Network Name and represents an APN as defined in
     * 3GPP TS 23.003.
     */
    ::android::hardware::radio::V1_6::OptionalDnn dnn __attribute__ ((aligned(8)));
    /**
     * Indicates the OsId + OsAppId (used as category in Android).
     */
    ::android::hardware::radio::V1_6::OptionalOsAppId osAppId __attribute__ ((aligned(8)));
};

static_assert(offsetof(::android::hardware::radio::V1_6::TrafficDescriptor, dnn) == 0, "wrong offset");
static_assert(offsetof(::android::hardware::radio::V1_6::TrafficDescriptor, osAppId) == 24, "wrong offset");
static_assert(sizeof(::android::hardware::radio::V1_6::TrafficDescriptor) == 48, "wrong size");
static_assert(__alignof(::android::hardware::radio::V1_6::TrafficDescriptor) == 8, "wrong alignment");

/**
 * This safe_union represents an optional TrafficDescriptor.
 */
struct OptionalTrafficDescriptor final {
    enum class hidl_discriminator : uint8_t {
        noinit = 0,  // ::android::hidl::safe_union::V1_0::Monostate
        value = 1,  // ::android::hardware::radio::V1_6::TrafficDescriptor
    };

    OptionalTrafficDescriptor();
    ~OptionalTrafficDescriptor();
    OptionalTrafficDescriptor(OptionalTrafficDescriptor&&);
    OptionalTrafficDescriptor(const OptionalTrafficDescriptor&);
    OptionalTrafficDescriptor& operator=(OptionalTrafficDescriptor&&);
    OptionalTrafficDescriptor& operator=(const OptionalTrafficDescriptor&);

    void noinit(const ::android::hidl::safe_union::V1_0::Monostate&);
    void noinit(::android::hidl::safe_union::V1_0::Monostate&&);
    ::android::hidl::safe_union::V1_0::Monostate& noinit();
    const ::android::hidl::safe_union::V1_0::Monostate& noinit() const;

    void value(const ::android::hardware::radio::V1_6::TrafficDescriptor&);
    void value(::android::hardware::radio::V1_6::TrafficDescriptor&&);
    ::android::hardware::radio::V1_6::TrafficDescriptor& value();
    const ::android::hardware::radio::V1_6::TrafficDescriptor& value() const;

    // Utility methods
    hidl_discriminator getDiscriminator() const;

    constexpr size_t hidl_getUnionOffset() const {
        return offsetof(::android::hardware::radio::V1_6::OptionalTrafficDescriptor, hidl_u);
    }

private:
    void hidl_destructUnion();

    hidl_discriminator hidl_d __attribute__ ((aligned(1))) ;
    union hidl_union final {
        ::android::hidl::safe_union::V1_0::Monostate noinit __attribute__ ((aligned(1)));
        ::android::hardware::radio::V1_6::TrafficDescriptor value __attribute__ ((aligned(8)));

        hidl_union();
        ~hidl_union();
    } hidl_u;

    static_assert(sizeof(::android::hardware::radio::V1_6::OptionalTrafficDescriptor::hidl_union) == 48, "wrong size");
    static_assert(__alignof(::android::hardware::radio::V1_6::OptionalTrafficDescriptor::hidl_union) == 8, "wrong alignment");
    static_assert(sizeof(::android::hardware::radio::V1_6::OptionalTrafficDescriptor::hidl_discriminator) == 1, "wrong size");
    static_assert(__alignof(::android::hardware::radio::V1_6::OptionalTrafficDescriptor::hidl_discriminator) == 1, "wrong alignment");
};

static_assert(sizeof(::android::hardware::radio::V1_6::OptionalTrafficDescriptor) == 56, "wrong size");
static_assert(__alignof(::android::hardware::radio::V1_6::OptionalTrafficDescriptor) == 8, "wrong alignment");

/**
 * This struct represents the current slicing configuration.
 */
struct SlicingConfig final {
    /**
     * This vector contains the current URSP rules. Empty vector represents that no
     * rules are configured.
     */
    ::android::hardware::hidl_vec<::android::hardware::radio::V1_6::UrspRule> urspRules __attribute__ ((aligned(8)));
    /**
     * List of all slices.
     */
    ::android::hardware::hidl_vec<::android::hardware::radio::V1_6::SliceInfo> sliceInfo __attribute__ ((aligned(8)));
};

static_assert(offsetof(::android::hardware::radio::V1_6::SlicingConfig, urspRules) == 0, "wrong offset");
static_assert(offsetof(::android::hardware::radio::V1_6::SlicingConfig, sliceInfo) == 16, "wrong offset");
static_assert(sizeof(::android::hardware::radio::V1_6::SlicingConfig) == 32, "wrong size");
static_assert(__alignof(::android::hardware::radio::V1_6::SlicingConfig) == 8, "wrong alignment");

/**
 * This struct represents a single URSP rule as defined in 3GPP TS 24.526.
 */
struct UrspRule final {
    /**
     * Precedence value in the range of 0 to 255. Higher value has lower
     * precedence.
     */
    uint8_t precedence __attribute__ ((aligned(1)));
    /**
     * Used as a matcher for network requests.
     */
    ::android::hardware::hidl_vec<::android::hardware::radio::V1_6::TrafficDescriptor> trafficDescriptors __attribute__ ((aligned(8)));
    /**
     * List of routes (connection parameters) that must be used for requests
     * matching a trafficDescriptor.
     */
    ::android::hardware::hidl_vec<::android::hardware::radio::V1_6::RouteSelectionDescriptor> routeSelectionDescriptor __attribute__ ((aligned(8)));
};

static_assert(offsetof(::android::hardware::radio::V1_6::UrspRule, precedence) == 0, "wrong offset");
static_assert(offsetof(::android::hardware::radio::V1_6::UrspRule, trafficDescriptors) == 8, "wrong offset");
static_assert(offsetof(::android::hardware::radio::V1_6::UrspRule, routeSelectionDescriptor) == 24, "wrong offset");
static_assert(sizeof(::android::hardware::radio::V1_6::UrspRule) == 40, "wrong size");
static_assert(__alignof(::android::hardware::radio::V1_6::UrspRule) == 8, "wrong alignment");

/**
 * This safe_union represents an optional PdpProtocolType.
 */
struct OptionalPdpProtocolType final {
    enum class hidl_discriminator : uint8_t {
        noinit = 0,  // ::android::hidl::safe_union::V1_0::Monostate
        value = 1,  // ::android::hardware::radio::V1_4::PdpProtocolType
    };

    OptionalPdpProtocolType();
    ~OptionalPdpProtocolType();
    OptionalPdpProtocolType(OptionalPdpProtocolType&&);
    OptionalPdpProtocolType(const OptionalPdpProtocolType&);
    OptionalPdpProtocolType& operator=(OptionalPdpProtocolType&&);
    OptionalPdpProtocolType& operator=(const OptionalPdpProtocolType&);

    void noinit(const ::android::hidl::safe_union::V1_0::Monostate&);
    void noinit(::android::hidl::safe_union::V1_0::Monostate&&);
    ::android::hidl::safe_union::V1_0::Monostate& noinit();
    const ::android::hidl::safe_union::V1_0::Monostate& noinit() const;

    void value(::android::hardware::radio::V1_4::PdpProtocolType);
    ::android::hardware::radio::V1_4::PdpProtocolType& value();
    ::android::hardware::radio::V1_4::PdpProtocolType value() const;

    // Utility methods
    hidl_discriminator getDiscriminator() const;

    constexpr size_t hidl_getUnionOffset() const {
        return offsetof(::android::hardware::radio::V1_6::OptionalPdpProtocolType, hidl_u);
    }

private:
    void hidl_destructUnion();

    hidl_discriminator hidl_d __attribute__ ((aligned(1))) ;
    union hidl_union final {
        ::android::hidl::safe_union::V1_0::Monostate noinit __attribute__ ((aligned(1)));
        ::android::hardware::radio::V1_4::PdpProtocolType value __attribute__ ((aligned(4)));

        hidl_union();
        ~hidl_union();
    } hidl_u;

    static_assert(sizeof(::android::hardware::radio::V1_6::OptionalPdpProtocolType::hidl_union) == 4, "wrong size");
    static_assert(__alignof(::android::hardware::radio::V1_6::OptionalPdpProtocolType::hidl_union) == 4, "wrong alignment");
    static_assert(sizeof(::android::hardware::radio::V1_6::OptionalPdpProtocolType::hidl_discriminator) == 1, "wrong size");
    static_assert(__alignof(::android::hardware::radio::V1_6::OptionalPdpProtocolType::hidl_discriminator) == 1, "wrong alignment");
};

static_assert(sizeof(::android::hardware::radio::V1_6::OptionalPdpProtocolType) == 8, "wrong size");
static_assert(__alignof(::android::hardware::radio::V1_6::OptionalPdpProtocolType) == 4, "wrong alignment");

/**
 * Enum representing session and service continuity mode as defined in
 * 3GPP TS 23.501.
 */
enum class SscMode : int8_t {
    MODE_1 = 1,
    MODE_2 = 2,
    MODE_3 = 3,
};

/**
 * This safe_union represents an optional SscMode.
 */
struct OptionalSscMode final {
    enum class hidl_discriminator : uint8_t {
        noinit = 0,  // ::android::hidl::safe_union::V1_0::Monostate
        value = 1,  // ::android::hardware::radio::V1_6::SscMode
    };

    OptionalSscMode();
    ~OptionalSscMode();
    OptionalSscMode(OptionalSscMode&&);
    OptionalSscMode(const OptionalSscMode&);
    OptionalSscMode& operator=(OptionalSscMode&&);
    OptionalSscMode& operator=(const OptionalSscMode&);

    void noinit(const ::android::hidl::safe_union::V1_0::Monostate&);
    void noinit(::android::hidl::safe_union::V1_0::Monostate&&);
    ::android::hidl::safe_union::V1_0::Monostate& noinit();
    const ::android::hidl::safe_union::V1_0::Monostate& noinit() const;

    void value(::android::hardware::radio::V1_6::SscMode);
    ::android::hardware::radio::V1_6::SscMode& value();
    ::android::hardware::radio::V1_6::SscMode value() const;

    // Utility methods
    hidl_discriminator getDiscriminator() const;

    constexpr size_t hidl_getUnionOffset() const {
        return offsetof(::android::hardware::radio::V1_6::OptionalSscMode, hidl_u);
    }

private:
    void hidl_destructUnion();

    hidl_discriminator hidl_d __attribute__ ((aligned(1))) ;
    union hidl_union final {
        ::android::hidl::safe_union::V1_0::Monostate noinit __attribute__ ((aligned(1)));
        ::android::hardware::radio::V1_6::SscMode value __attribute__ ((aligned(1)));

        hidl_union();
        ~hidl_union();
    } hidl_u;

    static_assert(sizeof(::android::hardware::radio::V1_6::OptionalSscMode::hidl_union) == 1, "wrong size");
    static_assert(__alignof(::android::hardware::radio::V1_6::OptionalSscMode::hidl_union) == 1, "wrong alignment");
    static_assert(sizeof(::android::hardware::radio::V1_6::OptionalSscMode::hidl_discriminator) == 1, "wrong size");
    static_assert(__alignof(::android::hardware::radio::V1_6::OptionalSscMode::hidl_discriminator) == 1, "wrong alignment");
};

static_assert(sizeof(::android::hardware::radio::V1_6::OptionalSscMode) == 2, "wrong size");
static_assert(__alignof(::android::hardware::radio::V1_6::OptionalSscMode) == 1, "wrong alignment");

/**
 * This struct represents a single route selection descriptor as defined in
 * 3GPP TS 24.526.
 */
struct RouteSelectionDescriptor final {
    /**
     * Precedence value in the range of 0 to 255. Higher value has lower
     * precedence.
     */
    uint8_t precedence __attribute__ ((aligned(1)));
    /**
     * Valid values are IP, IPV6 and IPV4V6.
     */
    ::android::hardware::radio::V1_6::OptionalPdpProtocolType sessionType __attribute__ ((aligned(4)));
    ::android::hardware::radio::V1_6::OptionalSscMode sscMode __attribute__ ((aligned(1)));
    /**
     * There can be 0 or more SliceInfo specified in a route descriptor.
     */
    ::android::hardware::hidl_vec<::android::hardware::radio::V1_6::SliceInfo> sliceInfo __attribute__ ((aligned(8)));
    /**
     * DNN stands for Data Network Name and represents an APN as defined in
     * 3GPP TS 23.003. There can be 0 or more DNNs specified in a route
     * descriptor.
     */
    ::android::hardware::hidl_vec<::android::hardware::hidl_string> dnn __attribute__ ((aligned(8)));
};

static_assert(offsetof(::android::hardware::radio::V1_6::RouteSelectionDescriptor, precedence) == 0, "wrong offset");
static_assert(offsetof(::android::hardware::radio::V1_6::RouteSelectionDescriptor, sessionType) == 4, "wrong offset");
static_assert(offsetof(::android::hardware::radio::V1_6::RouteSelectionDescriptor, sscMode) == 12, "wrong offset");
static_assert(offsetof(::android::hardware::radio::V1_6::RouteSelectionDescriptor, sliceInfo) == 16, "wrong offset");
static_assert(offsetof(::android::hardware::radio::V1_6::RouteSelectionDescriptor, dnn) == 32, "wrong offset");
static_assert(sizeof(::android::hardware::radio::V1_6::RouteSelectionDescriptor) == 48, "wrong size");
static_assert(__alignof(::android::hardware::radio::V1_6::RouteSelectionDescriptor) == 8, "wrong alignment");

/**
 * Public key type from carrier certificate.
 */
enum class PublicKeyType : int8_t {
    EPDG = 1,
    WLAN = 2,
};

/**
 * Carrier specific Information sent by the carrier,
 * which will be used to encrypt the IMSI and IMPI.
 */
struct ImsiEncryptionInfo final {
    ::android::hardware::radio::V1_1::ImsiEncryptionInfo base __attribute__ ((aligned(8)));
    ::android::hardware::radio::V1_6::PublicKeyType keyType __attribute__ ((aligned(1)));
};

static_assert(offsetof(::android::hardware::radio::V1_6::ImsiEncryptionInfo, base) == 0, "wrong offset");
static_assert(offsetof(::android::hardware::radio::V1_6::ImsiEncryptionInfo, keyType) == 72, "wrong offset");
static_assert(sizeof(::android::hardware::radio::V1_6::ImsiEncryptionInfo) == 80, "wrong size");
static_assert(__alignof(::android::hardware::radio::V1_6::ImsiEncryptionInfo) == 8, "wrong alignment");

/**
 * Phonebook-record-information specified by EF_ADN(Abbreviated dialing numbers)
 * record of SIM as per 3GPP spec 31.102 v15 Section-4.4.2.3.
 */
struct PhonebookRecordInfo final {
    /**
     * Record index. 0 is used to insert a record
     */
    uint32_t recordId __attribute__ ((aligned(4)));
    /**
     * Alpha identifier, empty string if no value
     */
    ::android::hardware::hidl_string name __attribute__ ((aligned(8)));
    /**
     * Dialling number, empty string if no value
     */
    ::android::hardware::hidl_string number __attribute__ ((aligned(8)));
    /**
     * Email addresses
     */
    ::android::hardware::hidl_vec<::android::hardware::hidl_string> emails __attribute__ ((aligned(8)));
    /**
     * Additional numbers
     */
    ::android::hardware::hidl_vec<::android::hardware::hidl_string> additionalNumbers __attribute__ ((aligned(8)));
};

static_assert(offsetof(::android::hardware::radio::V1_6::PhonebookRecordInfo, recordId) == 0, "wrong offset");
static_assert(offsetof(::android::hardware::radio::V1_6::PhonebookRecordInfo, name) == 8, "wrong offset");
static_assert(offsetof(::android::hardware::radio::V1_6::PhonebookRecordInfo, number) == 24, "wrong offset");
static_assert(offsetof(::android::hardware::radio::V1_6::PhonebookRecordInfo, emails) == 40, "wrong offset");
static_assert(offsetof(::android::hardware::radio::V1_6::PhonebookRecordInfo, additionalNumbers) == 56, "wrong offset");
static_assert(sizeof(::android::hardware::radio::V1_6::PhonebookRecordInfo) == 72, "wrong size");
static_assert(__alignof(::android::hardware::radio::V1_6::PhonebookRecordInfo) == 8, "wrong alignment");

struct PhonebookCapacity final {
    /**
     * Maximum number of ADN records possible in the SIM phonebook
     * Needs to be non-negative
     */
    int32_t maxAdnRecords __attribute__ ((aligned(4)));
    /**
     * Used ADN records in the SIM phonebook
     * Needs to be non-negative
     */
    int32_t usedAdnRecords __attribute__ ((aligned(4)));
    /**
     * Maximum email records possible in the SIM phonebook
     * Needs to be non-negative
     */
    int32_t maxEmailRecords __attribute__ ((aligned(4)));
    /**
     * Used email records in the SIM phonebook
     * Needs to be non-negative
     */
    int32_t usedEmailRecords __attribute__ ((aligned(4)));
    /**
     * Maximum additional number records possible in the SIM phonebook
     * Needs to be non-negative
     */
    int32_t maxAdditionalNumberRecords __attribute__ ((aligned(4)));
    /**
     * Used additional number records in the SIM phonebook
     * Needs to be non-negative
     */
    int32_t usedAdditionalNumberRecords __attribute__ ((aligned(4)));
    /**
     * Maximum name length possible in the SIM phonebook
     * Needs to be non-negative
     */
    int32_t maxNameLen __attribute__ ((aligned(4)));
    /**
     * Maximum number length possible in the SIM phonebook
     * Needs to be non-negative
     */
    int32_t maxNumberLen __attribute__ ((aligned(4)));
    /**
     * Maximum email length possible in the SIM phonebook
     * Needs to be non-negative
     */
    int32_t maxEmailLen __attribute__ ((aligned(4)));
    /**
     * Maximum additional number length possible in the SIM phonebook
     * Needs to be non-negative
     */
    int32_t maxAdditionalNumberLen __attribute__ ((aligned(4)));
};

static_assert(offsetof(::android::hardware::radio::V1_6::PhonebookCapacity, maxAdnRecords) == 0, "wrong offset");
static_assert(offsetof(::android::hardware::radio::V1_6::PhonebookCapacity, usedAdnRecords) == 4, "wrong offset");
static_assert(offsetof(::android::hardware::radio::V1_6::PhonebookCapacity, maxEmailRecords) == 8, "wrong offset");
static_assert(offsetof(::android::hardware::radio::V1_6::PhonebookCapacity, usedEmailRecords) == 12, "wrong offset");
static_assert(offsetof(::android::hardware::radio::V1_6::PhonebookCapacity, maxAdditionalNumberRecords) == 16, "wrong offset");
static_assert(offsetof(::android::hardware::radio::V1_6::PhonebookCapacity, usedAdditionalNumberRecords) == 20, "wrong offset");
static_assert(offsetof(::android::hardware::radio::V1_6::PhonebookCapacity, maxNameLen) == 24, "wrong offset");
static_assert(offsetof(::android::hardware::radio::V1_6::PhonebookCapacity, maxNumberLen) == 28, "wrong offset");
static_assert(offsetof(::android::hardware::radio::V1_6::PhonebookCapacity, maxEmailLen) == 32, "wrong offset");
static_assert(offsetof(::android::hardware::radio::V1_6::PhonebookCapacity, maxAdditionalNumberLen) == 36, "wrong offset");
static_assert(sizeof(::android::hardware::radio::V1_6::PhonebookCapacity) == 40, "wrong size");
static_assert(__alignof(::android::hardware::radio::V1_6::PhonebookCapacity) == 4, "wrong alignment");

/**
 * Enum representing the status of the received PB indication,
 * PB_RECEIVED_OK indicates this retrieval is fine
 * PB_RECEIVED_ERROR indicates one error happens, in general, the process
 *   can't be restored soon.
 * PB_RECEIVED_ABORT indicates the process is interrupted, in this case,
 *   modem might need resources and interrupt the current process, or it is
 *   timed out to receive all indications, and client can retry soon.
 * PB_RECEIVED_FINAL indicates the whole process is finished with a full
 *   chunk of phonebook data, means this is a last indication with the left
 *   data.
 */
enum class PbReceivedStatus : int8_t {
    PB_RECEIVED_OK = 1,
    PB_RECEIVED_ERROR = 2,
    PB_RECEIVED_ABORT = 3,
    PB_RECEIVED_FINAL = 4,
};

//
// type declarations for package
//

static inline std::string toString(const ::android::hardware::radio::V1_6::QosBandwidth& o);
static inline void PrintTo(const ::android::hardware::radio::V1_6::QosBandwidth& o, ::std::ostream*);
static inline bool operator==(const ::android::hardware::radio::V1_6::QosBandwidth& lhs, const ::android::hardware::radio::V1_6::QosBandwidth& rhs);
static inline bool operator!=(const ::android::hardware::radio::V1_6::QosBandwidth& lhs, const ::android::hardware::radio::V1_6::QosBandwidth& rhs);

static inline std::string toString(const ::android::hardware::radio::V1_6::EpsQos& o);
static inline void PrintTo(const ::android::hardware::radio::V1_6::EpsQos& o, ::std::ostream*);
static inline bool operator==(const ::android::hardware::radio::V1_6::EpsQos& lhs, const ::android::hardware::radio::V1_6::EpsQos& rhs);
static inline bool operator!=(const ::android::hardware::radio::V1_6::EpsQos& lhs, const ::android::hardware::radio::V1_6::EpsQos& rhs);

static inline std::string toString(const ::android::hardware::radio::V1_6::NrQos& o);
static inline void PrintTo(const ::android::hardware::radio::V1_6::NrQos& o, ::std::ostream*);
static inline bool operator==(const ::android::hardware::radio::V1_6::NrQos& lhs, const ::android::hardware::radio::V1_6::NrQos& rhs);
static inline bool operator!=(const ::android::hardware::radio::V1_6::NrQos& lhs, const ::android::hardware::radio::V1_6::NrQos& rhs);

template<typename>
static inline std::string toString(uint8_t o);
static inline std::string toString(::android::hardware::radio::V1_6::QosFlowIdRange o);
static inline void PrintTo(::android::hardware::radio::V1_6::QosFlowIdRange o, ::std::ostream* os);
constexpr uint8_t operator|(const ::android::hardware::radio::V1_6::QosFlowIdRange lhs, const ::android::hardware::radio::V1_6::QosFlowIdRange rhs) {
    return static_cast<uint8_t>(static_cast<uint8_t>(lhs) | static_cast<uint8_t>(rhs));
}
constexpr uint8_t operator|(const uint8_t lhs, const ::android::hardware::radio::V1_6::QosFlowIdRange rhs) {
    return static_cast<uint8_t>(lhs | static_cast<uint8_t>(rhs));
}
constexpr uint8_t operator|(const ::android::hardware::radio::V1_6::QosFlowIdRange lhs, const uint8_t rhs) {
    return static_cast<uint8_t>(static_cast<uint8_t>(lhs) | rhs);
}
constexpr uint8_t operator&(const ::android::hardware::radio::V1_6::QosFlowIdRange lhs, const ::android::hardware::radio::V1_6::QosFlowIdRange rhs) {
    return static_cast<uint8_t>(static_cast<uint8_t>(lhs) & static_cast<uint8_t>(rhs));
}
constexpr uint8_t operator&(const uint8_t lhs, const ::android::hardware::radio::V1_6::QosFlowIdRange rhs) {
    return static_cast<uint8_t>(lhs & static_cast<uint8_t>(rhs));
}
constexpr uint8_t operator&(const ::android::hardware::radio::V1_6::QosFlowIdRange lhs, const uint8_t rhs) {
    return static_cast<uint8_t>(static_cast<uint8_t>(lhs) & rhs);
}
constexpr uint8_t &operator|=(uint8_t& v, const ::android::hardware::radio::V1_6::QosFlowIdRange e) {
    v |= static_cast<uint8_t>(e);
    return v;
}
constexpr uint8_t &operator&=(uint8_t& v, const ::android::hardware::radio::V1_6::QosFlowIdRange e) {
    v &= static_cast<uint8_t>(e);
    return v;
}

static inline std::string toString(const ::android::hardware::radio::V1_6::Qos& o);
static inline void PrintTo(const ::android::hardware::radio::V1_6::Qos& o, ::std::ostream*);
static inline bool operator==(const ::android::hardware::radio::V1_6::Qos& lhs, const ::android::hardware::radio::V1_6::Qos& rhs);
static inline bool operator!=(const ::android::hardware::radio::V1_6::Qos& lhs, const ::android::hardware::radio::V1_6::Qos& rhs);

template<typename>
static inline std::string toString(int8_t o);
static inline std::string toString(::android::hardware::radio::V1_6::QosProtocol o);
static inline void PrintTo(::android::hardware::radio::V1_6::QosProtocol o, ::std::ostream* os);
constexpr int8_t operator|(const ::android::hardware::radio::V1_6::QosProtocol lhs, const ::android::hardware::radio::V1_6::QosProtocol rhs) {
    return static_cast<int8_t>(static_cast<int8_t>(lhs) | static_cast<int8_t>(rhs));
}
constexpr int8_t operator|(const int8_t lhs, const ::android::hardware::radio::V1_6::QosProtocol rhs) {
    return static_cast<int8_t>(lhs | static_cast<int8_t>(rhs));
}
constexpr int8_t operator|(const ::android::hardware::radio::V1_6::QosProtocol lhs, const int8_t rhs) {
    return static_cast<int8_t>(static_cast<int8_t>(lhs) | rhs);
}
constexpr int8_t operator&(const ::android::hardware::radio::V1_6::QosProtocol lhs, const ::android::hardware::radio::V1_6::QosProtocol rhs) {
    return static_cast<int8_t>(static_cast<int8_t>(lhs) & static_cast<int8_t>(rhs));
}
constexpr int8_t operator&(const int8_t lhs, const ::android::hardware::radio::V1_6::QosProtocol rhs) {
    return static_cast<int8_t>(lhs & static_cast<int8_t>(rhs));
}
constexpr int8_t operator&(const ::android::hardware::radio::V1_6::QosProtocol lhs, const int8_t rhs) {
    return static_cast<int8_t>(static_cast<int8_t>(lhs) & rhs);
}
constexpr int8_t &operator|=(int8_t& v, const ::android::hardware::radio::V1_6::QosProtocol e) {
    v |= static_cast<int8_t>(e);
    return v;
}
constexpr int8_t &operator&=(int8_t& v, const ::android::hardware::radio::V1_6::QosProtocol e) {
    v &= static_cast<int8_t>(e);
    return v;
}

template<typename>
static inline std::string toString(int8_t o);
static inline std::string toString(::android::hardware::radio::V1_6::QosFilterDirection o);
static inline void PrintTo(::android::hardware::radio::V1_6::QosFilterDirection o, ::std::ostream* os);
constexpr int8_t operator|(const ::android::hardware::radio::V1_6::QosFilterDirection lhs, const ::android::hardware::radio::V1_6::QosFilterDirection rhs) {
    return static_cast<int8_t>(static_cast<int8_t>(lhs) | static_cast<int8_t>(rhs));
}
constexpr int8_t operator|(const int8_t lhs, const ::android::hardware::radio::V1_6::QosFilterDirection rhs) {
    return static_cast<int8_t>(lhs | static_cast<int8_t>(rhs));
}
constexpr int8_t operator|(const ::android::hardware::radio::V1_6::QosFilterDirection lhs, const int8_t rhs) {
    return static_cast<int8_t>(static_cast<int8_t>(lhs) | rhs);
}
constexpr int8_t operator&(const ::android::hardware::radio::V1_6::QosFilterDirection lhs, const ::android::hardware::radio::V1_6::QosFilterDirection rhs) {
    return static_cast<int8_t>(static_cast<int8_t>(lhs) & static_cast<int8_t>(rhs));
}
constexpr int8_t operator&(const int8_t lhs, const ::android::hardware::radio::V1_6::QosFilterDirection rhs) {
    return static_cast<int8_t>(lhs & static_cast<int8_t>(rhs));
}
constexpr int8_t operator&(const ::android::hardware::radio::V1_6::QosFilterDirection lhs, const int8_t rhs) {
    return static_cast<int8_t>(static_cast<int8_t>(lhs) & rhs);
}
constexpr int8_t &operator|=(int8_t& v, const ::android::hardware::radio::V1_6::QosFilterDirection e) {
    v |= static_cast<int8_t>(e);
    return v;
}
constexpr int8_t &operator&=(int8_t& v, const ::android::hardware::radio::V1_6::QosFilterDirection e) {
    v &= static_cast<int8_t>(e);
    return v;
}

template<typename>
static inline std::string toString(uint16_t o);
static inline std::string toString(::android::hardware::radio::V1_6::QosPortRange o);
static inline void PrintTo(::android::hardware::radio::V1_6::QosPortRange o, ::std::ostream* os);
constexpr uint16_t operator|(const ::android::hardware::radio::V1_6::QosPortRange lhs, const ::android::hardware::radio::V1_6::QosPortRange rhs) {
    return static_cast<uint16_t>(static_cast<uint16_t>(lhs) | static_cast<uint16_t>(rhs));
}
constexpr uint16_t operator|(const uint16_t lhs, const ::android::hardware::radio::V1_6::QosPortRange rhs) {
    return static_cast<uint16_t>(lhs | static_cast<uint16_t>(rhs));
}
constexpr uint16_t operator|(const ::android::hardware::radio::V1_6::QosPortRange lhs, const uint16_t rhs) {
    return static_cast<uint16_t>(static_cast<uint16_t>(lhs) | rhs);
}
constexpr uint16_t operator&(const ::android::hardware::radio::V1_6::QosPortRange lhs, const ::android::hardware::radio::V1_6::QosPortRange rhs) {
    return static_cast<uint16_t>(static_cast<uint16_t>(lhs) & static_cast<uint16_t>(rhs));
}
constexpr uint16_t operator&(const uint16_t lhs, const ::android::hardware::radio::V1_6::QosPortRange rhs) {
    return static_cast<uint16_t>(lhs & static_cast<uint16_t>(rhs));
}
constexpr uint16_t operator&(const ::android::hardware::radio::V1_6::QosPortRange lhs, const uint16_t rhs) {
    return static_cast<uint16_t>(static_cast<uint16_t>(lhs) & rhs);
}
constexpr uint16_t &operator|=(uint16_t& v, const ::android::hardware::radio::V1_6::QosPortRange e) {
    v |= static_cast<uint16_t>(e);
    return v;
}
constexpr uint16_t &operator&=(uint16_t& v, const ::android::hardware::radio::V1_6::QosPortRange e) {
    v &= static_cast<uint16_t>(e);
    return v;
}

template<typename>
static inline std::string toString(int32_t o);
static inline std::string toString(::android::hardware::radio::V1_6::RadioError o);
static inline void PrintTo(::android::hardware::radio::V1_6::RadioError o, ::std::ostream* os);
constexpr int32_t operator|(const ::android::hardware::radio::V1_6::RadioError lhs, const ::android::hardware::radio::V1_6::RadioError rhs) {
    return static_cast<int32_t>(static_cast<int32_t>(lhs) | static_cast<int32_t>(rhs));
}
constexpr int32_t operator|(const int32_t lhs, const ::android::hardware::radio::V1_6::RadioError rhs) {
    return static_cast<int32_t>(lhs | static_cast<int32_t>(rhs));
}
constexpr int32_t operator|(const ::android::hardware::radio::V1_6::RadioError lhs, const int32_t rhs) {
    return static_cast<int32_t>(static_cast<int32_t>(lhs) | rhs);
}
constexpr int32_t operator&(const ::android::hardware::radio::V1_6::RadioError lhs, const ::android::hardware::radio::V1_6::RadioError rhs) {
    return static_cast<int32_t>(static_cast<int32_t>(lhs) & static_cast<int32_t>(rhs));
}
constexpr int32_t operator&(const int32_t lhs, const ::android::hardware::radio::V1_6::RadioError rhs) {
    return static_cast<int32_t>(lhs & static_cast<int32_t>(rhs));
}
constexpr int32_t operator&(const ::android::hardware::radio::V1_6::RadioError lhs, const int32_t rhs) {
    return static_cast<int32_t>(static_cast<int32_t>(lhs) & rhs);
}
constexpr int32_t &operator|=(int32_t& v, const ::android::hardware::radio::V1_6::RadioError e) {
    v |= static_cast<int32_t>(e);
    return v;
}
constexpr int32_t &operator&=(int32_t& v, const ::android::hardware::radio::V1_6::RadioError e) {
    v &= static_cast<int32_t>(e);
    return v;
}

static inline std::string toString(const ::android::hardware::radio::V1_6::RadioResponseInfo& o);
static inline void PrintTo(const ::android::hardware::radio::V1_6::RadioResponseInfo& o, ::std::ostream*);
static inline bool operator==(const ::android::hardware::radio::V1_6::RadioResponseInfo& lhs, const ::android::hardware::radio::V1_6::RadioResponseInfo& rhs);
static inline bool operator!=(const ::android::hardware::radio::V1_6::RadioResponseInfo& lhs, const ::android::hardware::radio::V1_6::RadioResponseInfo& rhs);

static inline std::string toString(const ::android::hardware::radio::V1_6::PortRange& o);
static inline void PrintTo(const ::android::hardware::radio::V1_6::PortRange& o, ::std::ostream*);
static inline bool operator==(const ::android::hardware::radio::V1_6::PortRange& lhs, const ::android::hardware::radio::V1_6::PortRange& rhs);
static inline bool operator!=(const ::android::hardware::radio::V1_6::PortRange& lhs, const ::android::hardware::radio::V1_6::PortRange& rhs);

static inline std::string toString(const ::android::hardware::radio::V1_6::MaybePort& o);
static inline void PrintTo(const ::android::hardware::radio::V1_6::MaybePort& o, ::std::ostream*);
static inline bool operator==(const ::android::hardware::radio::V1_6::MaybePort& lhs, const ::android::hardware::radio::V1_6::MaybePort& rhs);
static inline bool operator!=(const ::android::hardware::radio::V1_6::MaybePort& lhs, const ::android::hardware::radio::V1_6::MaybePort& rhs);

static inline std::string toString(const ::android::hardware::radio::V1_6::QosFilter::TypeOfService& o);
static inline void PrintTo(const ::android::hardware::radio::V1_6::QosFilter::TypeOfService& o, ::std::ostream*);
static inline bool operator==(const ::android::hardware::radio::V1_6::QosFilter::TypeOfService& lhs, const ::android::hardware::radio::V1_6::QosFilter::TypeOfService& rhs);
static inline bool operator!=(const ::android::hardware::radio::V1_6::QosFilter::TypeOfService& lhs, const ::android::hardware::radio::V1_6::QosFilter::TypeOfService& rhs);

static inline std::string toString(const ::android::hardware::radio::V1_6::QosFilter::Ipv6FlowLabel& o);
static inline void PrintTo(const ::android::hardware::radio::V1_6::QosFilter::Ipv6FlowLabel& o, ::std::ostream*);
static inline bool operator==(const ::android::hardware::radio::V1_6::QosFilter::Ipv6FlowLabel& lhs, const ::android::hardware::radio::V1_6::QosFilter::Ipv6FlowLabel& rhs);
static inline bool operator!=(const ::android::hardware::radio::V1_6::QosFilter::Ipv6FlowLabel& lhs, const ::android::hardware::radio::V1_6::QosFilter::Ipv6FlowLabel& rhs);

static inline std::string toString(const ::android::hardware::radio::V1_6::QosFilter::IpsecSpi& o);
static inline void PrintTo(const ::android::hardware::radio::V1_6::QosFilter::IpsecSpi& o, ::std::ostream*);
static inline bool operator==(const ::android::hardware::radio::V1_6::QosFilter::IpsecSpi& lhs, const ::android::hardware::radio::V1_6::QosFilter::IpsecSpi& rhs);
static inline bool operator!=(const ::android::hardware::radio::V1_6::QosFilter::IpsecSpi& lhs, const ::android::hardware::radio::V1_6::QosFilter::IpsecSpi& rhs);

static inline std::string toString(const ::android::hardware::radio::V1_6::QosFilter& o);
static inline void PrintTo(const ::android::hardware::radio::V1_6::QosFilter& o, ::std::ostream*);
static inline bool operator==(const ::android::hardware::radio::V1_6::QosFilter& lhs, const ::android::hardware::radio::V1_6::QosFilter& rhs);
static inline bool operator!=(const ::android::hardware::radio::V1_6::QosFilter& lhs, const ::android::hardware::radio::V1_6::QosFilter& rhs);

static inline std::string toString(const ::android::hardware::radio::V1_6::QosSession& o);
static inline void PrintTo(const ::android::hardware::radio::V1_6::QosSession& o, ::std::ostream*);
static inline bool operator==(const ::android::hardware::radio::V1_6::QosSession& lhs, const ::android::hardware::radio::V1_6::QosSession& rhs);
static inline bool operator!=(const ::android::hardware::radio::V1_6::QosSession& lhs, const ::android::hardware::radio::V1_6::QosSession& rhs);

template<typename>
static inline std::string toString(int8_t o);
static inline std::string toString(::android::hardware::radio::V1_6::HandoverFailureMode o);
static inline void PrintTo(::android::hardware::radio::V1_6::HandoverFailureMode o, ::std::ostream* os);
constexpr int8_t operator|(const ::android::hardware::radio::V1_6::HandoverFailureMode lhs, const ::android::hardware::radio::V1_6::HandoverFailureMode rhs) {
    return static_cast<int8_t>(static_cast<int8_t>(lhs) | static_cast<int8_t>(rhs));
}
constexpr int8_t operator|(const int8_t lhs, const ::android::hardware::radio::V1_6::HandoverFailureMode rhs) {
    return static_cast<int8_t>(lhs | static_cast<int8_t>(rhs));
}
constexpr int8_t operator|(const ::android::hardware::radio::V1_6::HandoverFailureMode lhs, const int8_t rhs) {
    return static_cast<int8_t>(static_cast<int8_t>(lhs) | rhs);
}
constexpr int8_t operator&(const ::android::hardware::radio::V1_6::HandoverFailureMode lhs, const ::android::hardware::radio::V1_6::HandoverFailureMode rhs) {
    return static_cast<int8_t>(static_cast<int8_t>(lhs) & static_cast<int8_t>(rhs));
}
constexpr int8_t operator&(const int8_t lhs, const ::android::hardware::radio::V1_6::HandoverFailureMode rhs) {
    return static_cast<int8_t>(lhs & static_cast<int8_t>(rhs));
}
constexpr int8_t operator&(const ::android::hardware::radio::V1_6::HandoverFailureMode lhs, const int8_t rhs) {
    return static_cast<int8_t>(static_cast<int8_t>(lhs) & rhs);
}
constexpr int8_t &operator|=(int8_t& v, const ::android::hardware::radio::V1_6::HandoverFailureMode e) {
    v |= static_cast<int8_t>(e);
    return v;
}
constexpr int8_t &operator&=(int8_t& v, const ::android::hardware::radio::V1_6::HandoverFailureMode e) {
    v &= static_cast<int8_t>(e);
    return v;
}

template<typename>
static inline std::string toString(int32_t o);
static inline std::string toString(::android::hardware::radio::V1_6::DataCallFailCause o);
static inline void PrintTo(::android::hardware::radio::V1_6::DataCallFailCause o, ::std::ostream* os);
constexpr int32_t operator|(const ::android::hardware::radio::V1_6::DataCallFailCause lhs, const ::android::hardware::radio::V1_6::DataCallFailCause rhs) {
    return static_cast<int32_t>(static_cast<int32_t>(lhs) | static_cast<int32_t>(rhs));
}
constexpr int32_t operator|(const int32_t lhs, const ::android::hardware::radio::V1_6::DataCallFailCause rhs) {
    return static_cast<int32_t>(lhs | static_cast<int32_t>(rhs));
}
constexpr int32_t operator|(const ::android::hardware::radio::V1_6::DataCallFailCause lhs, const int32_t rhs) {
    return static_cast<int32_t>(static_cast<int32_t>(lhs) | rhs);
}
constexpr int32_t operator&(const ::android::hardware::radio::V1_6::DataCallFailCause lhs, const ::android::hardware::radio::V1_6::DataCallFailCause rhs) {
    return static_cast<int32_t>(static_cast<int32_t>(lhs) & static_cast<int32_t>(rhs));
}
constexpr int32_t operator&(const int32_t lhs, const ::android::hardware::radio::V1_6::DataCallFailCause rhs) {
    return static_cast<int32_t>(lhs & static_cast<int32_t>(rhs));
}
constexpr int32_t operator&(const ::android::hardware::radio::V1_6::DataCallFailCause lhs, const int32_t rhs) {
    return static_cast<int32_t>(static_cast<int32_t>(lhs) & rhs);
}
constexpr int32_t &operator|=(int32_t& v, const ::android::hardware::radio::V1_6::DataCallFailCause e) {
    v |= static_cast<int32_t>(e);
    return v;
}
constexpr int32_t &operator&=(int32_t& v, const ::android::hardware::radio::V1_6::DataCallFailCause e) {
    v &= static_cast<int32_t>(e);
    return v;
}

template<typename>
static inline std::string toString(uint8_t o);
static inline std::string toString(::android::hardware::radio::V1_6::SliceServiceType o);
static inline void PrintTo(::android::hardware::radio::V1_6::SliceServiceType o, ::std::ostream* os);
constexpr uint8_t operator|(const ::android::hardware::radio::V1_6::SliceServiceType lhs, const ::android::hardware::radio::V1_6::SliceServiceType rhs) {
    return static_cast<uint8_t>(static_cast<uint8_t>(lhs) | static_cast<uint8_t>(rhs));
}
constexpr uint8_t operator|(const uint8_t lhs, const ::android::hardware::radio::V1_6::SliceServiceType rhs) {
    return static_cast<uint8_t>(lhs | static_cast<uint8_t>(rhs));
}
constexpr uint8_t operator|(const ::android::hardware::radio::V1_6::SliceServiceType lhs, const uint8_t rhs) {
    return static_cast<uint8_t>(static_cast<uint8_t>(lhs) | rhs);
}
constexpr uint8_t operator&(const ::android::hardware::radio::V1_6::SliceServiceType lhs, const ::android::hardware::radio::V1_6::SliceServiceType rhs) {
    return static_cast<uint8_t>(static_cast<uint8_t>(lhs) & static_cast<uint8_t>(rhs));
}
constexpr uint8_t operator&(const uint8_t lhs, const ::android::hardware::radio::V1_6::SliceServiceType rhs) {
    return static_cast<uint8_t>(lhs & static_cast<uint8_t>(rhs));
}
constexpr uint8_t operator&(const ::android::hardware::radio::V1_6::SliceServiceType lhs, const uint8_t rhs) {
    return static_cast<uint8_t>(static_cast<uint8_t>(lhs) & rhs);
}
constexpr uint8_t &operator|=(uint8_t& v, const ::android::hardware::radio::V1_6::SliceServiceType e) {
    v |= static_cast<uint8_t>(e);
    return v;
}
constexpr uint8_t &operator&=(uint8_t& v, const ::android::hardware::radio::V1_6::SliceServiceType e) {
    v &= static_cast<uint8_t>(e);
    return v;
}

template<typename>
static inline std::string toString(int8_t o);
static inline std::string toString(::android::hardware::radio::V1_6::SliceStatus o);
static inline void PrintTo(::android::hardware::radio::V1_6::SliceStatus o, ::std::ostream* os);
constexpr int8_t operator|(const ::android::hardware::radio::V1_6::SliceStatus lhs, const ::android::hardware::radio::V1_6::SliceStatus rhs) {
    return static_cast<int8_t>(static_cast<int8_t>(lhs) | static_cast<int8_t>(rhs));
}
constexpr int8_t operator|(const int8_t lhs, const ::android::hardware::radio::V1_6::SliceStatus rhs) {
    return static_cast<int8_t>(lhs | static_cast<int8_t>(rhs));
}
constexpr int8_t operator|(const ::android::hardware::radio::V1_6::SliceStatus lhs, const int8_t rhs) {
    return static_cast<int8_t>(static_cast<int8_t>(lhs) | rhs);
}
constexpr int8_t operator&(const ::android::hardware::radio::V1_6::SliceStatus lhs, const ::android::hardware::radio::V1_6::SliceStatus rhs) {
    return static_cast<int8_t>(static_cast<int8_t>(lhs) & static_cast<int8_t>(rhs));
}
constexpr int8_t operator&(const int8_t lhs, const ::android::hardware::radio::V1_6::SliceStatus rhs) {
    return static_cast<int8_t>(lhs & static_cast<int8_t>(rhs));
}
constexpr int8_t operator&(const ::android::hardware::radio::V1_6::SliceStatus lhs, const int8_t rhs) {
    return static_cast<int8_t>(static_cast<int8_t>(lhs) & rhs);
}
constexpr int8_t &operator|=(int8_t& v, const ::android::hardware::radio::V1_6::SliceStatus e) {
    v |= static_cast<int8_t>(e);
    return v;
}
constexpr int8_t &operator&=(int8_t& v, const ::android::hardware::radio::V1_6::SliceStatus e) {
    v &= static_cast<int8_t>(e);
    return v;
}

static inline std::string toString(const ::android::hardware::radio::V1_6::SliceInfo& o);
static inline void PrintTo(const ::android::hardware::radio::V1_6::SliceInfo& o, ::std::ostream*);
static inline bool operator==(const ::android::hardware::radio::V1_6::SliceInfo& lhs, const ::android::hardware::radio::V1_6::SliceInfo& rhs);
static inline bool operator!=(const ::android::hardware::radio::V1_6::SliceInfo& lhs, const ::android::hardware::radio::V1_6::SliceInfo& rhs);

static inline std::string toString(const ::android::hardware::radio::V1_6::OptionalSliceInfo& o);
static inline void PrintTo(const ::android::hardware::radio::V1_6::OptionalSliceInfo& o, ::std::ostream*);
static inline bool operator==(const ::android::hardware::radio::V1_6::OptionalSliceInfo& lhs, const ::android::hardware::radio::V1_6::OptionalSliceInfo& rhs);
static inline bool operator!=(const ::android::hardware::radio::V1_6::OptionalSliceInfo& lhs, const ::android::hardware::radio::V1_6::OptionalSliceInfo& rhs);

static inline std::string toString(const ::android::hardware::radio::V1_6::SetupDataCallResult& o);
static inline void PrintTo(const ::android::hardware::radio::V1_6::SetupDataCallResult& o, ::std::ostream*);
static inline bool operator==(const ::android::hardware::radio::V1_6::SetupDataCallResult& lhs, const ::android::hardware::radio::V1_6::SetupDataCallResult& rhs);
static inline bool operator!=(const ::android::hardware::radio::V1_6::SetupDataCallResult& lhs, const ::android::hardware::radio::V1_6::SetupDataCallResult& rhs);

template<typename>
static inline std::string toString(int8_t o);
static inline std::string toString(::android::hardware::radio::V1_6::NrDualConnectivityState o);
static inline void PrintTo(::android::hardware::radio::V1_6::NrDualConnectivityState o, ::std::ostream* os);
constexpr int8_t operator|(const ::android::hardware::radio::V1_6::NrDualConnectivityState lhs, const ::android::hardware::radio::V1_6::NrDualConnectivityState rhs) {
    return static_cast<int8_t>(static_cast<int8_t>(lhs) | static_cast<int8_t>(rhs));
}
constexpr int8_t operator|(const int8_t lhs, const ::android::hardware::radio::V1_6::NrDualConnectivityState rhs) {
    return static_cast<int8_t>(lhs | static_cast<int8_t>(rhs));
}
constexpr int8_t operator|(const ::android::hardware::radio::V1_6::NrDualConnectivityState lhs, const int8_t rhs) {
    return static_cast<int8_t>(static_cast<int8_t>(lhs) | rhs);
}
constexpr int8_t operator&(const ::android::hardware::radio::V1_6::NrDualConnectivityState lhs, const ::android::hardware::radio::V1_6::NrDualConnectivityState rhs) {
    return static_cast<int8_t>(static_cast<int8_t>(lhs) & static_cast<int8_t>(rhs));
}
constexpr int8_t operator&(const int8_t lhs, const ::android::hardware::radio::V1_6::NrDualConnectivityState rhs) {
    return static_cast<int8_t>(lhs & static_cast<int8_t>(rhs));
}
constexpr int8_t operator&(const ::android::hardware::radio::V1_6::NrDualConnectivityState lhs, const int8_t rhs) {
    return static_cast<int8_t>(static_cast<int8_t>(lhs) & rhs);
}
constexpr int8_t &operator|=(int8_t& v, const ::android::hardware::radio::V1_6::NrDualConnectivityState e) {
    v |= static_cast<int8_t>(e);
    return v;
}
constexpr int8_t &operator&=(int8_t& v, const ::android::hardware::radio::V1_6::NrDualConnectivityState e) {
    v &= static_cast<int8_t>(e);
    return v;
}

static inline std::string toString(const ::android::hardware::radio::V1_6::LinkCapacityEstimate& o);
static inline void PrintTo(const ::android::hardware::radio::V1_6::LinkCapacityEstimate& o, ::std::ostream*);
static inline bool operator==(const ::android::hardware::radio::V1_6::LinkCapacityEstimate& lhs, const ::android::hardware::radio::V1_6::LinkCapacityEstimate& rhs);
static inline bool operator!=(const ::android::hardware::radio::V1_6::LinkCapacityEstimate& lhs, const ::android::hardware::radio::V1_6::LinkCapacityEstimate& rhs);

template<typename>
static inline std::string toString(int8_t o);
static inline std::string toString(::android::hardware::radio::V1_6::DataThrottlingAction o);
static inline void PrintTo(::android::hardware::radio::V1_6::DataThrottlingAction o, ::std::ostream* os);
constexpr int8_t operator|(const ::android::hardware::radio::V1_6::DataThrottlingAction lhs, const ::android::hardware::radio::V1_6::DataThrottlingAction rhs) {
    return static_cast<int8_t>(static_cast<int8_t>(lhs) | static_cast<int8_t>(rhs));
}
constexpr int8_t operator|(const int8_t lhs, const ::android::hardware::radio::V1_6::DataThrottlingAction rhs) {
    return static_cast<int8_t>(lhs | static_cast<int8_t>(rhs));
}
constexpr int8_t operator|(const ::android::hardware::radio::V1_6::DataThrottlingAction lhs, const int8_t rhs) {
    return static_cast<int8_t>(static_cast<int8_t>(lhs) | rhs);
}
constexpr int8_t operator&(const ::android::hardware::radio::V1_6::DataThrottlingAction lhs, const ::android::hardware::radio::V1_6::DataThrottlingAction rhs) {
    return static_cast<int8_t>(static_cast<int8_t>(lhs) & static_cast<int8_t>(rhs));
}
constexpr int8_t operator&(const int8_t lhs, const ::android::hardware::radio::V1_6::DataThrottlingAction rhs) {
    return static_cast<int8_t>(lhs & static_cast<int8_t>(rhs));
}
constexpr int8_t operator&(const ::android::hardware::radio::V1_6::DataThrottlingAction lhs, const int8_t rhs) {
    return static_cast<int8_t>(static_cast<int8_t>(lhs) & rhs);
}
constexpr int8_t &operator|=(int8_t& v, const ::android::hardware::radio::V1_6::DataThrottlingAction e) {
    v |= static_cast<int8_t>(e);
    return v;
}
constexpr int8_t &operator&=(int8_t& v, const ::android::hardware::radio::V1_6::DataThrottlingAction e) {
    v &= static_cast<int8_t>(e);
    return v;
}

template<typename>
static inline std::string toString(uint8_t o);
static inline std::string toString(::android::hardware::radio::V1_6::VopsIndicator o);
static inline void PrintTo(::android::hardware::radio::V1_6::VopsIndicator o, ::std::ostream* os);
constexpr uint8_t operator|(const ::android::hardware::radio::V1_6::VopsIndicator lhs, const ::android::hardware::radio::V1_6::VopsIndicator rhs) {
    return static_cast<uint8_t>(static_cast<uint8_t>(lhs) | static_cast<uint8_t>(rhs));
}
constexpr uint8_t operator|(const uint8_t lhs, const ::android::hardware::radio::V1_6::VopsIndicator rhs) {
    return static_cast<uint8_t>(lhs | static_cast<uint8_t>(rhs));
}
constexpr uint8_t operator|(const ::android::hardware::radio::V1_6::VopsIndicator lhs, const uint8_t rhs) {
    return static_cast<uint8_t>(static_cast<uint8_t>(lhs) | rhs);
}
constexpr uint8_t operator&(const ::android::hardware::radio::V1_6::VopsIndicator lhs, const ::android::hardware::radio::V1_6::VopsIndicator rhs) {
    return static_cast<uint8_t>(static_cast<uint8_t>(lhs) & static_cast<uint8_t>(rhs));
}
constexpr uint8_t operator&(const uint8_t lhs, const ::android::hardware::radio::V1_6::VopsIndicator rhs) {
    return static_cast<uint8_t>(lhs & static_cast<uint8_t>(rhs));
}
constexpr uint8_t operator&(const ::android::hardware::radio::V1_6::VopsIndicator lhs, const uint8_t rhs) {
    return static_cast<uint8_t>(static_cast<uint8_t>(lhs) & rhs);
}
constexpr uint8_t &operator|=(uint8_t& v, const ::android::hardware::radio::V1_6::VopsIndicator e) {
    v |= static_cast<uint8_t>(e);
    return v;
}
constexpr uint8_t &operator&=(uint8_t& v, const ::android::hardware::radio::V1_6::VopsIndicator e) {
    v &= static_cast<uint8_t>(e);
    return v;
}

template<typename>
static inline std::string toString(uint8_t o);
static inline std::string toString(::android::hardware::radio::V1_6::EmcIndicator o);
static inline void PrintTo(::android::hardware::radio::V1_6::EmcIndicator o, ::std::ostream* os);
constexpr uint8_t operator|(const ::android::hardware::radio::V1_6::EmcIndicator lhs, const ::android::hardware::radio::V1_6::EmcIndicator rhs) {
    return static_cast<uint8_t>(static_cast<uint8_t>(lhs) | static_cast<uint8_t>(rhs));
}
constexpr uint8_t operator|(const uint8_t lhs, const ::android::hardware::radio::V1_6::EmcIndicator rhs) {
    return static_cast<uint8_t>(lhs | static_cast<uint8_t>(rhs));
}
constexpr uint8_t operator|(const ::android::hardware::radio::V1_6::EmcIndicator lhs, const uint8_t rhs) {
    return static_cast<uint8_t>(static_cast<uint8_t>(lhs) | rhs);
}
constexpr uint8_t operator&(const ::android::hardware::radio::V1_6::EmcIndicator lhs, const ::android::hardware::radio::V1_6::EmcIndicator rhs) {
    return static_cast<uint8_t>(static_cast<uint8_t>(lhs) & static_cast<uint8_t>(rhs));
}
constexpr uint8_t operator&(const uint8_t lhs, const ::android::hardware::radio::V1_6::EmcIndicator rhs) {
    return static_cast<uint8_t>(lhs & static_cast<uint8_t>(rhs));
}
constexpr uint8_t operator&(const ::android::hardware::radio::V1_6::EmcIndicator lhs, const uint8_t rhs) {
    return static_cast<uint8_t>(static_cast<uint8_t>(lhs) & rhs);
}
constexpr uint8_t &operator|=(uint8_t& v, const ::android::hardware::radio::V1_6::EmcIndicator e) {
    v |= static_cast<uint8_t>(e);
    return v;
}
constexpr uint8_t &operator&=(uint8_t& v, const ::android::hardware::radio::V1_6::EmcIndicator e) {
    v &= static_cast<uint8_t>(e);
    return v;
}

template<typename>
static inline std::string toString(uint8_t o);
static inline std::string toString(::android::hardware::radio::V1_6::EmfIndicator o);
static inline void PrintTo(::android::hardware::radio::V1_6::EmfIndicator o, ::std::ostream* os);
constexpr uint8_t operator|(const ::android::hardware::radio::V1_6::EmfIndicator lhs, const ::android::hardware::radio::V1_6::EmfIndicator rhs) {
    return static_cast<uint8_t>(static_cast<uint8_t>(lhs) | static_cast<uint8_t>(rhs));
}
constexpr uint8_t operator|(const uint8_t lhs, const ::android::hardware::radio::V1_6::EmfIndicator rhs) {
    return static_cast<uint8_t>(lhs | static_cast<uint8_t>(rhs));
}
constexpr uint8_t operator|(const ::android::hardware::radio::V1_6::EmfIndicator lhs, const uint8_t rhs) {
    return static_cast<uint8_t>(static_cast<uint8_t>(lhs) | rhs);
}
constexpr uint8_t operator&(const ::android::hardware::radio::V1_6::EmfIndicator lhs, const ::android::hardware::radio::V1_6::EmfIndicator rhs) {
    return static_cast<uint8_t>(static_cast<uint8_t>(lhs) & static_cast<uint8_t>(rhs));
}
constexpr uint8_t operator&(const uint8_t lhs, const ::android::hardware::radio::V1_6::EmfIndicator rhs) {
    return static_cast<uint8_t>(lhs & static_cast<uint8_t>(rhs));
}
constexpr uint8_t operator&(const ::android::hardware::radio::V1_6::EmfIndicator lhs, const uint8_t rhs) {
    return static_cast<uint8_t>(static_cast<uint8_t>(lhs) & rhs);
}
constexpr uint8_t &operator|=(uint8_t& v, const ::android::hardware::radio::V1_6::EmfIndicator e) {
    v |= static_cast<uint8_t>(e);
    return v;
}
constexpr uint8_t &operator&=(uint8_t& v, const ::android::hardware::radio::V1_6::EmfIndicator e) {
    v &= static_cast<uint8_t>(e);
    return v;
}

static inline std::string toString(const ::android::hardware::radio::V1_6::NrVopsInfo& o);
static inline void PrintTo(const ::android::hardware::radio::V1_6::NrVopsInfo& o, ::std::ostream*);
static inline bool operator==(const ::android::hardware::radio::V1_6::NrVopsInfo& lhs, const ::android::hardware::radio::V1_6::NrVopsInfo& rhs);
static inline bool operator!=(const ::android::hardware::radio::V1_6::NrVopsInfo& lhs, const ::android::hardware::radio::V1_6::NrVopsInfo& rhs);

static inline std::string toString(const ::android::hardware::radio::V1_6::LteSignalStrength& o);
static inline void PrintTo(const ::android::hardware::radio::V1_6::LteSignalStrength& o, ::std::ostream*);
static inline bool operator==(const ::android::hardware::radio::V1_6::LteSignalStrength& lhs, const ::android::hardware::radio::V1_6::LteSignalStrength& rhs);
static inline bool operator!=(const ::android::hardware::radio::V1_6::LteSignalStrength& lhs, const ::android::hardware::radio::V1_6::LteSignalStrength& rhs);

static inline std::string toString(const ::android::hardware::radio::V1_6::NrSignalStrength& o);
static inline void PrintTo(const ::android::hardware::radio::V1_6::NrSignalStrength& o, ::std::ostream*);
static inline bool operator==(const ::android::hardware::radio::V1_6::NrSignalStrength& lhs, const ::android::hardware::radio::V1_6::NrSignalStrength& rhs);
static inline bool operator!=(const ::android::hardware::radio::V1_6::NrSignalStrength& lhs, const ::android::hardware::radio::V1_6::NrSignalStrength& rhs);

static inline std::string toString(const ::android::hardware::radio::V1_6::SignalStrength& o);
static inline void PrintTo(const ::android::hardware::radio::V1_6::SignalStrength& o, ::std::ostream*);
static inline bool operator==(const ::android::hardware::radio::V1_6::SignalStrength& lhs, const ::android::hardware::radio::V1_6::SignalStrength& rhs);
static inline bool operator!=(const ::android::hardware::radio::V1_6::SignalStrength& lhs, const ::android::hardware::radio::V1_6::SignalStrength& rhs);

static inline std::string toString(const ::android::hardware::radio::V1_6::CellInfoLte& o);
static inline void PrintTo(const ::android::hardware::radio::V1_6::CellInfoLte& o, ::std::ostream*);
static inline bool operator==(const ::android::hardware::radio::V1_6::CellInfoLte& lhs, const ::android::hardware::radio::V1_6::CellInfoLte& rhs);
static inline bool operator!=(const ::android::hardware::radio::V1_6::CellInfoLte& lhs, const ::android::hardware::radio::V1_6::CellInfoLte& rhs);

static inline std::string toString(const ::android::hardware::radio::V1_6::CellInfoNr& o);
static inline void PrintTo(const ::android::hardware::radio::V1_6::CellInfoNr& o, ::std::ostream*);
static inline bool operator==(const ::android::hardware::radio::V1_6::CellInfoNr& lhs, const ::android::hardware::radio::V1_6::CellInfoNr& rhs);
static inline bool operator!=(const ::android::hardware::radio::V1_6::CellInfoNr& lhs, const ::android::hardware::radio::V1_6::CellInfoNr& rhs);

static inline std::string toString(const ::android::hardware::radio::V1_6::CellInfo::CellInfoRatSpecificInfo& o);
static inline void PrintTo(const ::android::hardware::radio::V1_6::CellInfo::CellInfoRatSpecificInfo& o, ::std::ostream*);
static inline bool operator==(const ::android::hardware::radio::V1_6::CellInfo::CellInfoRatSpecificInfo& lhs, const ::android::hardware::radio::V1_6::CellInfo::CellInfoRatSpecificInfo& rhs);
static inline bool operator!=(const ::android::hardware::radio::V1_6::CellInfo::CellInfoRatSpecificInfo& lhs, const ::android::hardware::radio::V1_6::CellInfo::CellInfoRatSpecificInfo& rhs);

static inline std::string toString(const ::android::hardware::radio::V1_6::CellInfo& o);
static inline void PrintTo(const ::android::hardware::radio::V1_6::CellInfo& o, ::std::ostream*);
static inline bool operator==(const ::android::hardware::radio::V1_6::CellInfo& lhs, const ::android::hardware::radio::V1_6::CellInfo& rhs);
static inline bool operator!=(const ::android::hardware::radio::V1_6::CellInfo& lhs, const ::android::hardware::radio::V1_6::CellInfo& rhs);

static inline std::string toString(const ::android::hardware::radio::V1_6::NetworkScanResult& o);
static inline void PrintTo(const ::android::hardware::radio::V1_6::NetworkScanResult& o, ::std::ostream*);
static inline bool operator==(const ::android::hardware::radio::V1_6::NetworkScanResult& lhs, const ::android::hardware::radio::V1_6::NetworkScanResult& rhs);
static inline bool operator!=(const ::android::hardware::radio::V1_6::NetworkScanResult& lhs, const ::android::hardware::radio::V1_6::NetworkScanResult& rhs);

static inline std::string toString(const ::android::hardware::radio::V1_6::RegStateResult::AccessTechnologySpecificInfo& o);
static inline void PrintTo(const ::android::hardware::radio::V1_6::RegStateResult::AccessTechnologySpecificInfo& o, ::std::ostream*);
static inline bool operator==(const ::android::hardware::radio::V1_6::RegStateResult::AccessTechnologySpecificInfo& lhs, const ::android::hardware::radio::V1_6::RegStateResult::AccessTechnologySpecificInfo& rhs);
static inline bool operator!=(const ::android::hardware::radio::V1_6::RegStateResult::AccessTechnologySpecificInfo& lhs, const ::android::hardware::radio::V1_6::RegStateResult::AccessTechnologySpecificInfo& rhs);

static inline std::string toString(const ::android::hardware::radio::V1_6::RegStateResult& o);
static inline void PrintTo(const ::android::hardware::radio::V1_6::RegStateResult& o, ::std::ostream*);
static inline bool operator==(const ::android::hardware::radio::V1_6::RegStateResult& lhs, const ::android::hardware::radio::V1_6::RegStateResult& rhs);
static inline bool operator!=(const ::android::hardware::radio::V1_6::RegStateResult& lhs, const ::android::hardware::radio::V1_6::RegStateResult& rhs);

static inline std::string toString(const ::android::hardware::radio::V1_6::Call& o);
static inline void PrintTo(const ::android::hardware::radio::V1_6::Call& o, ::std::ostream*);
static inline bool operator==(const ::android::hardware::radio::V1_6::Call& lhs, const ::android::hardware::radio::V1_6::Call& rhs);
static inline bool operator!=(const ::android::hardware::radio::V1_6::Call& lhs, const ::android::hardware::radio::V1_6::Call& rhs);

template<typename>
static inline std::string toString(int32_t o);
static inline std::string toString(::android::hardware::radio::V1_6::NgranBands o);
static inline void PrintTo(::android::hardware::radio::V1_6::NgranBands o, ::std::ostream* os);
constexpr int32_t operator|(const ::android::hardware::radio::V1_6::NgranBands lhs, const ::android::hardware::radio::V1_6::NgranBands rhs) {
    return static_cast<int32_t>(static_cast<int32_t>(lhs) | static_cast<int32_t>(rhs));
}
constexpr int32_t operator|(const int32_t lhs, const ::android::hardware::radio::V1_6::NgranBands rhs) {
    return static_cast<int32_t>(lhs | static_cast<int32_t>(rhs));
}
constexpr int32_t operator|(const ::android::hardware::radio::V1_6::NgranBands lhs, const int32_t rhs) {
    return static_cast<int32_t>(static_cast<int32_t>(lhs) | rhs);
}
constexpr int32_t operator&(const ::android::hardware::radio::V1_6::NgranBands lhs, const ::android::hardware::radio::V1_6::NgranBands rhs) {
    return static_cast<int32_t>(static_cast<int32_t>(lhs) & static_cast<int32_t>(rhs));
}
constexpr int32_t operator&(const int32_t lhs, const ::android::hardware::radio::V1_6::NgranBands rhs) {
    return static_cast<int32_t>(lhs & static_cast<int32_t>(rhs));
}
constexpr int32_t operator&(const ::android::hardware::radio::V1_6::NgranBands lhs, const int32_t rhs) {
    return static_cast<int32_t>(static_cast<int32_t>(lhs) & rhs);
}
constexpr int32_t &operator|=(int32_t& v, const ::android::hardware::radio::V1_6::NgranBands e) {
    v |= static_cast<int32_t>(e);
    return v;
}
constexpr int32_t &operator&=(int32_t& v, const ::android::hardware::radio::V1_6::NgranBands e) {
    v &= static_cast<int32_t>(e);
    return v;
}

static inline std::string toString(const ::android::hardware::radio::V1_6::PhysicalChannelConfig::Band& o);
static inline void PrintTo(const ::android::hardware::radio::V1_6::PhysicalChannelConfig::Band& o, ::std::ostream*);
static inline bool operator==(const ::android::hardware::radio::V1_6::PhysicalChannelConfig::Band& lhs, const ::android::hardware::radio::V1_6::PhysicalChannelConfig::Band& rhs);
static inline bool operator!=(const ::android::hardware::radio::V1_6::PhysicalChannelConfig::Band& lhs, const ::android::hardware::radio::V1_6::PhysicalChannelConfig::Band& rhs);

static inline std::string toString(const ::android::hardware::radio::V1_6::PhysicalChannelConfig& o);
static inline void PrintTo(const ::android::hardware::radio::V1_6::PhysicalChannelConfig& o, ::std::ostream*);
static inline bool operator==(const ::android::hardware::radio::V1_6::PhysicalChannelConfig& lhs, const ::android::hardware::radio::V1_6::PhysicalChannelConfig& rhs);
static inline bool operator!=(const ::android::hardware::radio::V1_6::PhysicalChannelConfig& lhs, const ::android::hardware::radio::V1_6::PhysicalChannelConfig& rhs);

static inline std::string toString(const ::android::hardware::radio::V1_6::OptionalDnn& o);
static inline void PrintTo(const ::android::hardware::radio::V1_6::OptionalDnn& o, ::std::ostream*);
static inline bool operator==(const ::android::hardware::radio::V1_6::OptionalDnn& lhs, const ::android::hardware::radio::V1_6::OptionalDnn& rhs);
static inline bool operator!=(const ::android::hardware::radio::V1_6::OptionalDnn& lhs, const ::android::hardware::radio::V1_6::OptionalDnn& rhs);

static inline std::string toString(const ::android::hardware::radio::V1_6::OsAppId& o);
static inline void PrintTo(const ::android::hardware::radio::V1_6::OsAppId& o, ::std::ostream*);
static inline bool operator==(const ::android::hardware::radio::V1_6::OsAppId& lhs, const ::android::hardware::radio::V1_6::OsAppId& rhs);
static inline bool operator!=(const ::android::hardware::radio::V1_6::OsAppId& lhs, const ::android::hardware::radio::V1_6::OsAppId& rhs);

static inline std::string toString(const ::android::hardware::radio::V1_6::OptionalOsAppId& o);
static inline void PrintTo(const ::android::hardware::radio::V1_6::OptionalOsAppId& o, ::std::ostream*);
static inline bool operator==(const ::android::hardware::radio::V1_6::OptionalOsAppId& lhs, const ::android::hardware::radio::V1_6::OptionalOsAppId& rhs);
static inline bool operator!=(const ::android::hardware::radio::V1_6::OptionalOsAppId& lhs, const ::android::hardware::radio::V1_6::OptionalOsAppId& rhs);

static inline std::string toString(const ::android::hardware::radio::V1_6::TrafficDescriptor& o);
static inline void PrintTo(const ::android::hardware::radio::V1_6::TrafficDescriptor& o, ::std::ostream*);
static inline bool operator==(const ::android::hardware::radio::V1_6::TrafficDescriptor& lhs, const ::android::hardware::radio::V1_6::TrafficDescriptor& rhs);
static inline bool operator!=(const ::android::hardware::radio::V1_6::TrafficDescriptor& lhs, const ::android::hardware::radio::V1_6::TrafficDescriptor& rhs);

static inline std::string toString(const ::android::hardware::radio::V1_6::OptionalTrafficDescriptor& o);
static inline void PrintTo(const ::android::hardware::radio::V1_6::OptionalTrafficDescriptor& o, ::std::ostream*);
static inline bool operator==(const ::android::hardware::radio::V1_6::OptionalTrafficDescriptor& lhs, const ::android::hardware::radio::V1_6::OptionalTrafficDescriptor& rhs);
static inline bool operator!=(const ::android::hardware::radio::V1_6::OptionalTrafficDescriptor& lhs, const ::android::hardware::radio::V1_6::OptionalTrafficDescriptor& rhs);

static inline std::string toString(const ::android::hardware::radio::V1_6::SlicingConfig& o);
static inline void PrintTo(const ::android::hardware::radio::V1_6::SlicingConfig& o, ::std::ostream*);
static inline bool operator==(const ::android::hardware::radio::V1_6::SlicingConfig& lhs, const ::android::hardware::radio::V1_6::SlicingConfig& rhs);
static inline bool operator!=(const ::android::hardware::radio::V1_6::SlicingConfig& lhs, const ::android::hardware::radio::V1_6::SlicingConfig& rhs);

static inline std::string toString(const ::android::hardware::radio::V1_6::UrspRule& o);
static inline void PrintTo(const ::android::hardware::radio::V1_6::UrspRule& o, ::std::ostream*);
static inline bool operator==(const ::android::hardware::radio::V1_6::UrspRule& lhs, const ::android::hardware::radio::V1_6::UrspRule& rhs);
static inline bool operator!=(const ::android::hardware::radio::V1_6::UrspRule& lhs, const ::android::hardware::radio::V1_6::UrspRule& rhs);

static inline std::string toString(const ::android::hardware::radio::V1_6::OptionalPdpProtocolType& o);
static inline void PrintTo(const ::android::hardware::radio::V1_6::OptionalPdpProtocolType& o, ::std::ostream*);
static inline bool operator==(const ::android::hardware::radio::V1_6::OptionalPdpProtocolType& lhs, const ::android::hardware::radio::V1_6::OptionalPdpProtocolType& rhs);
static inline bool operator!=(const ::android::hardware::radio::V1_6::OptionalPdpProtocolType& lhs, const ::android::hardware::radio::V1_6::OptionalPdpProtocolType& rhs);

template<typename>
static inline std::string toString(int8_t o);
static inline std::string toString(::android::hardware::radio::V1_6::SscMode o);
static inline void PrintTo(::android::hardware::radio::V1_6::SscMode o, ::std::ostream* os);
constexpr int8_t operator|(const ::android::hardware::radio::V1_6::SscMode lhs, const ::android::hardware::radio::V1_6::SscMode rhs) {
    return static_cast<int8_t>(static_cast<int8_t>(lhs) | static_cast<int8_t>(rhs));
}
constexpr int8_t operator|(const int8_t lhs, const ::android::hardware::radio::V1_6::SscMode rhs) {
    return static_cast<int8_t>(lhs | static_cast<int8_t>(rhs));
}
constexpr int8_t operator|(const ::android::hardware::radio::V1_6::SscMode lhs, const int8_t rhs) {
    return static_cast<int8_t>(static_cast<int8_t>(lhs) | rhs);
}
constexpr int8_t operator&(const ::android::hardware::radio::V1_6::SscMode lhs, const ::android::hardware::radio::V1_6::SscMode rhs) {
    return static_cast<int8_t>(static_cast<int8_t>(lhs) & static_cast<int8_t>(rhs));
}
constexpr int8_t operator&(const int8_t lhs, const ::android::hardware::radio::V1_6::SscMode rhs) {
    return static_cast<int8_t>(lhs & static_cast<int8_t>(rhs));
}
constexpr int8_t operator&(const ::android::hardware::radio::V1_6::SscMode lhs, const int8_t rhs) {
    return static_cast<int8_t>(static_cast<int8_t>(lhs) & rhs);
}
constexpr int8_t &operator|=(int8_t& v, const ::android::hardware::radio::V1_6::SscMode e) {
    v |= static_cast<int8_t>(e);
    return v;
}
constexpr int8_t &operator&=(int8_t& v, const ::android::hardware::radio::V1_6::SscMode e) {
    v &= static_cast<int8_t>(e);
    return v;
}

static inline std::string toString(const ::android::hardware::radio::V1_6::OptionalSscMode& o);
static inline void PrintTo(const ::android::hardware::radio::V1_6::OptionalSscMode& o, ::std::ostream*);
static inline bool operator==(const ::android::hardware::radio::V1_6::OptionalSscMode& lhs, const ::android::hardware::radio::V1_6::OptionalSscMode& rhs);
static inline bool operator!=(const ::android::hardware::radio::V1_6::OptionalSscMode& lhs, const ::android::hardware::radio::V1_6::OptionalSscMode& rhs);

static inline std::string toString(const ::android::hardware::radio::V1_6::RouteSelectionDescriptor& o);
static inline void PrintTo(const ::android::hardware::radio::V1_6::RouteSelectionDescriptor& o, ::std::ostream*);
static inline bool operator==(const ::android::hardware::radio::V1_6::RouteSelectionDescriptor& lhs, const ::android::hardware::radio::V1_6::RouteSelectionDescriptor& rhs);
static inline bool operator!=(const ::android::hardware::radio::V1_6::RouteSelectionDescriptor& lhs, const ::android::hardware::radio::V1_6::RouteSelectionDescriptor& rhs);

template<typename>
static inline std::string toString(int8_t o);
static inline std::string toString(::android::hardware::radio::V1_6::PublicKeyType o);
static inline void PrintTo(::android::hardware::radio::V1_6::PublicKeyType o, ::std::ostream* os);
constexpr int8_t operator|(const ::android::hardware::radio::V1_6::PublicKeyType lhs, const ::android::hardware::radio::V1_6::PublicKeyType rhs) {
    return static_cast<int8_t>(static_cast<int8_t>(lhs) | static_cast<int8_t>(rhs));
}
constexpr int8_t operator|(const int8_t lhs, const ::android::hardware::radio::V1_6::PublicKeyType rhs) {
    return static_cast<int8_t>(lhs | static_cast<int8_t>(rhs));
}
constexpr int8_t operator|(const ::android::hardware::radio::V1_6::PublicKeyType lhs, const int8_t rhs) {
    return static_cast<int8_t>(static_cast<int8_t>(lhs) | rhs);
}
constexpr int8_t operator&(const ::android::hardware::radio::V1_6::PublicKeyType lhs, const ::android::hardware::radio::V1_6::PublicKeyType rhs) {
    return static_cast<int8_t>(static_cast<int8_t>(lhs) & static_cast<int8_t>(rhs));
}
constexpr int8_t operator&(const int8_t lhs, const ::android::hardware::radio::V1_6::PublicKeyType rhs) {
    return static_cast<int8_t>(lhs & static_cast<int8_t>(rhs));
}
constexpr int8_t operator&(const ::android::hardware::radio::V1_6::PublicKeyType lhs, const int8_t rhs) {
    return static_cast<int8_t>(static_cast<int8_t>(lhs) & rhs);
}
constexpr int8_t &operator|=(int8_t& v, const ::android::hardware::radio::V1_6::PublicKeyType e) {
    v |= static_cast<int8_t>(e);
    return v;
}
constexpr int8_t &operator&=(int8_t& v, const ::android::hardware::radio::V1_6::PublicKeyType e) {
    v &= static_cast<int8_t>(e);
    return v;
}

static inline std::string toString(const ::android::hardware::radio::V1_6::ImsiEncryptionInfo& o);
static inline void PrintTo(const ::android::hardware::radio::V1_6::ImsiEncryptionInfo& o, ::std::ostream*);
static inline bool operator==(const ::android::hardware::radio::V1_6::ImsiEncryptionInfo& lhs, const ::android::hardware::radio::V1_6::ImsiEncryptionInfo& rhs);
static inline bool operator!=(const ::android::hardware::radio::V1_6::ImsiEncryptionInfo& lhs, const ::android::hardware::radio::V1_6::ImsiEncryptionInfo& rhs);

static inline std::string toString(const ::android::hardware::radio::V1_6::PhonebookRecordInfo& o);
static inline void PrintTo(const ::android::hardware::radio::V1_6::PhonebookRecordInfo& o, ::std::ostream*);
static inline bool operator==(const ::android::hardware::radio::V1_6::PhonebookRecordInfo& lhs, const ::android::hardware::radio::V1_6::PhonebookRecordInfo& rhs);
static inline bool operator!=(const ::android::hardware::radio::V1_6::PhonebookRecordInfo& lhs, const ::android::hardware::radio::V1_6::PhonebookRecordInfo& rhs);

static inline std::string toString(const ::android::hardware::radio::V1_6::PhonebookCapacity& o);
static inline void PrintTo(const ::android::hardware::radio::V1_6::PhonebookCapacity& o, ::std::ostream*);
static inline bool operator==(const ::android::hardware::radio::V1_6::PhonebookCapacity& lhs, const ::android::hardware::radio::V1_6::PhonebookCapacity& rhs);
static inline bool operator!=(const ::android::hardware::radio::V1_6::PhonebookCapacity& lhs, const ::android::hardware::radio::V1_6::PhonebookCapacity& rhs);

template<typename>
static inline std::string toString(int8_t o);
static inline std::string toString(::android::hardware::radio::V1_6::PbReceivedStatus o);
static inline void PrintTo(::android::hardware::radio::V1_6::PbReceivedStatus o, ::std::ostream* os);
constexpr int8_t operator|(const ::android::hardware::radio::V1_6::PbReceivedStatus lhs, const ::android::hardware::radio::V1_6::PbReceivedStatus rhs) {
    return static_cast<int8_t>(static_cast<int8_t>(lhs) | static_cast<int8_t>(rhs));
}
constexpr int8_t operator|(const int8_t lhs, const ::android::hardware::radio::V1_6::PbReceivedStatus rhs) {
    return static_cast<int8_t>(lhs | static_cast<int8_t>(rhs));
}
constexpr int8_t operator|(const ::android::hardware::radio::V1_6::PbReceivedStatus lhs, const int8_t rhs) {
    return static_cast<int8_t>(static_cast<int8_t>(lhs) | rhs);
}
constexpr int8_t operator&(const ::android::hardware::radio::V1_6::PbReceivedStatus lhs, const ::android::hardware::radio::V1_6::PbReceivedStatus rhs) {
    return static_cast<int8_t>(static_cast<int8_t>(lhs) & static_cast<int8_t>(rhs));
}
constexpr int8_t operator&(const int8_t lhs, const ::android::hardware::radio::V1_6::PbReceivedStatus rhs) {
    return static_cast<int8_t>(lhs & static_cast<int8_t>(rhs));
}
constexpr int8_t operator&(const ::android::hardware::radio::V1_6::PbReceivedStatus lhs, const int8_t rhs) {
    return static_cast<int8_t>(static_cast<int8_t>(lhs) & rhs);
}
constexpr int8_t &operator|=(int8_t& v, const ::android::hardware::radio::V1_6::PbReceivedStatus e) {
    v |= static_cast<int8_t>(e);
    return v;
}
constexpr int8_t &operator&=(int8_t& v, const ::android::hardware::radio::V1_6::PbReceivedStatus e) {
    v &= static_cast<int8_t>(e);
    return v;
}

//
// type header definitions for package
//

static inline std::string toString(const ::android::hardware::radio::V1_6::QosBandwidth& o) {
    using ::android::hardware::toString;
    std::string os;
    os += "{";
    os += ".maxBitrateKbps = ";
    os += ::android::hardware::toString(o.maxBitrateKbps);
    os += ", .guaranteedBitrateKbps = ";
    os += ::android::hardware::toString(o.guaranteedBitrateKbps);
    os += "}"; return os;
}

static inline void PrintTo(const ::android::hardware::radio::V1_6::QosBandwidth& o, ::std::ostream* os) {
    *os << toString(o);
}

static inline bool operator==(const ::android::hardware::radio::V1_6::QosBandwidth& lhs, const ::android::hardware::radio::V1_6::QosBandwidth& rhs) {
    if (lhs.maxBitrateKbps != rhs.maxBitrateKbps) {
        return false;
    }
    if (lhs.guaranteedBitrateKbps != rhs.guaranteedBitrateKbps) {
        return false;
    }
    return true;
}

static inline bool operator!=(const ::android::hardware::radio::V1_6::QosBandwidth& lhs, const ::android::hardware::radio::V1_6::QosBandwidth& rhs){
    return !(lhs == rhs);
}

static inline std::string toString(const ::android::hardware::radio::V1_6::EpsQos& o) {
    using ::android::hardware::toString;
    std::string os;
    os += "{";
    os += ".qci = ";
    os += ::android::hardware::toString(o.qci);
    os += ", .downlink = ";
    os += ::android::hardware::radio::V1_6::toString(o.downlink);
    os += ", .uplink = ";
    os += ::android::hardware::radio::V1_6::toString(o.uplink);
    os += "}"; return os;
}

static inline void PrintTo(const ::android::hardware::radio::V1_6::EpsQos& o, ::std::ostream* os) {
    *os << toString(o);
}

static inline bool operator==(const ::android::hardware::radio::V1_6::EpsQos& lhs, const ::android::hardware::radio::V1_6::EpsQos& rhs) {
    if (lhs.qci != rhs.qci) {
        return false;
    }
    if (lhs.downlink != rhs.downlink) {
        return false;
    }
    if (lhs.uplink != rhs.uplink) {
        return false;
    }
    return true;
}

static inline bool operator!=(const ::android::hardware::radio::V1_6::EpsQos& lhs, const ::android::hardware::radio::V1_6::EpsQos& rhs){
    return !(lhs == rhs);
}

static inline std::string toString(const ::android::hardware::radio::V1_6::NrQos& o) {
    using ::android::hardware::toString;
    std::string os;
    os += "{";
    os += ".fiveQi = ";
    os += ::android::hardware::toString(o.fiveQi);
    os += ", .downlink = ";
    os += ::android::hardware::radio::V1_6::toString(o.downlink);
    os += ", .uplink = ";
    os += ::android::hardware::radio::V1_6::toString(o.uplink);
    os += ", .qfi = ";
    os += ::android::hardware::toString(o.qfi);
    os += ", .averagingWindowMs = ";
    os += ::android::hardware::toString(o.averagingWindowMs);
    os += "}"; return os;
}

static inline void PrintTo(const ::android::hardware::radio::V1_6::NrQos& o, ::std::ostream* os) {
    *os << toString(o);
}

static inline bool operator==(const ::android::hardware::radio::V1_6::NrQos& lhs, const ::android::hardware::radio::V1_6::NrQos& rhs) {
    if (lhs.fiveQi != rhs.fiveQi) {
        return false;
    }
    if (lhs.downlink != rhs.downlink) {
        return false;
    }
    if (lhs.uplink != rhs.uplink) {
        return false;
    }
    if (lhs.qfi != rhs.qfi) {
        return false;
    }
    if (lhs.averagingWindowMs != rhs.averagingWindowMs) {
        return false;
    }
    return true;
}

static inline bool operator!=(const ::android::hardware::radio::V1_6::NrQos& lhs, const ::android::hardware::radio::V1_6::NrQos& rhs){
    return !(lhs == rhs);
}

template<>
inline std::string toString<::android::hardware::radio::V1_6::QosFlowIdRange>(uint8_t o) {
    using ::android::hardware::details::toHexString;
    std::string os;
    ::android::hardware::hidl_bitfield<::android::hardware::radio::V1_6::QosFlowIdRange> flipped = 0;
    bool first = true;
    if ((o & ::android::hardware::radio::V1_6::QosFlowIdRange::MIN) == static_cast<uint8_t>(::android::hardware::radio::V1_6::QosFlowIdRange::MIN)) {
        os += (first ? "" : " | ");
        os += "MIN";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::QosFlowIdRange::MIN;
    }
    if ((o & ::android::hardware::radio::V1_6::QosFlowIdRange::MAX) == static_cast<uint8_t>(::android::hardware::radio::V1_6::QosFlowIdRange::MAX)) {
        os += (first ? "" : " | ");
        os += "MAX";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::QosFlowIdRange::MAX;
    }
    if (o != flipped) {
        os += (first ? "" : " | ");
        os += toHexString(o & (~flipped));
    }os += " (";
    os += toHexString(o);
    os += ")";
    return os;
}

static inline std::string toString(::android::hardware::radio::V1_6::QosFlowIdRange o) {
    using ::android::hardware::details::toHexString;
    if (o == ::android::hardware::radio::V1_6::QosFlowIdRange::MIN) {
        return "MIN";
    }
    if (o == ::android::hardware::radio::V1_6::QosFlowIdRange::MAX) {
        return "MAX";
    }
    std::string os;
    os += toHexString(static_cast<uint8_t>(o));
    return os;
}

static inline void PrintTo(::android::hardware::radio::V1_6::QosFlowIdRange o, ::std::ostream* os) {
    *os << toString(o);
}

static inline std::string toString(const ::android::hardware::radio::V1_6::Qos& o) {
    using ::android::hardware::toString;
    std::string os;
    os += "{";

    switch (o.getDiscriminator()) {
        case ::android::hardware::radio::V1_6::Qos::hidl_discriminator::noinit: {
            os += ".noinit = ";
            os += toString(o.noinit());
            break;
        }
        case ::android::hardware::radio::V1_6::Qos::hidl_discriminator::eps: {
            os += ".eps = ";
            os += toString(o.eps());
            break;
        }
        case ::android::hardware::radio::V1_6::Qos::hidl_discriminator::nr: {
            os += ".nr = ";
            os += toString(o.nr());
            break;
        }
        default: {
            ::android::hardware::details::logAlwaysFatal((
                    "Unknown union discriminator (value: " +
                    std::to_string((uint8_t) o.getDiscriminator()) + ").").c_str());
        }
    }
    os += "}"; return os;
}

static inline void PrintTo(const ::android::hardware::radio::V1_6::Qos& o, ::std::ostream* os) {
    *os << toString(o);
}

static inline bool operator==(const ::android::hardware::radio::V1_6::Qos& lhs, const ::android::hardware::radio::V1_6::Qos& rhs) {
    if (lhs.getDiscriminator() != rhs.getDiscriminator()) {
        return false;
    }
    switch (lhs.getDiscriminator()) {
        case ::android::hardware::radio::V1_6::Qos::hidl_discriminator::noinit: {
            return (lhs.noinit() == rhs.noinit());
        }
        case ::android::hardware::radio::V1_6::Qos::hidl_discriminator::eps: {
            return (lhs.eps() == rhs.eps());
        }
        case ::android::hardware::radio::V1_6::Qos::hidl_discriminator::nr: {
            return (lhs.nr() == rhs.nr());
        }
        default: {
            ::android::hardware::details::logAlwaysFatal((
                    "Unknown union discriminator (value: " +
                    std::to_string((uint8_t) lhs.getDiscriminator()) + ").").c_str());
        }
    }
    return true;
}

static inline bool operator!=(const ::android::hardware::radio::V1_6::Qos& lhs, const ::android::hardware::radio::V1_6::Qos& rhs){
    return !(lhs == rhs);
}

template<>
inline std::string toString<::android::hardware::radio::V1_6::QosProtocol>(int8_t o) {
    using ::android::hardware::details::toHexString;
    std::string os;
    ::android::hardware::hidl_bitfield<::android::hardware::radio::V1_6::QosProtocol> flipped = 0;
    bool first = true;
    if ((o & ::android::hardware::radio::V1_6::QosProtocol::UNSPECIFIED) == static_cast<int8_t>(::android::hardware::radio::V1_6::QosProtocol::UNSPECIFIED)) {
        os += (first ? "" : " | ");
        os += "UNSPECIFIED";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::QosProtocol::UNSPECIFIED;
    }
    if ((o & ::android::hardware::radio::V1_6::QosProtocol::TCP) == static_cast<int8_t>(::android::hardware::radio::V1_6::QosProtocol::TCP)) {
        os += (first ? "" : " | ");
        os += "TCP";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::QosProtocol::TCP;
    }
    if ((o & ::android::hardware::radio::V1_6::QosProtocol::UDP) == static_cast<int8_t>(::android::hardware::radio::V1_6::QosProtocol::UDP)) {
        os += (first ? "" : " | ");
        os += "UDP";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::QosProtocol::UDP;
    }
    if ((o & ::android::hardware::radio::V1_6::QosProtocol::ESP) == static_cast<int8_t>(::android::hardware::radio::V1_6::QosProtocol::ESP)) {
        os += (first ? "" : " | ");
        os += "ESP";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::QosProtocol::ESP;
    }
    if ((o & ::android::hardware::radio::V1_6::QosProtocol::AH) == static_cast<int8_t>(::android::hardware::radio::V1_6::QosProtocol::AH)) {
        os += (first ? "" : " | ");
        os += "AH";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::QosProtocol::AH;
    }
    if (o != flipped) {
        os += (first ? "" : " | ");
        os += toHexString(o & (~flipped));
    }os += " (";
    os += toHexString(o);
    os += ")";
    return os;
}

static inline std::string toString(::android::hardware::radio::V1_6::QosProtocol o) {
    using ::android::hardware::details::toHexString;
    if (o == ::android::hardware::radio::V1_6::QosProtocol::UNSPECIFIED) {
        return "UNSPECIFIED";
    }
    if (o == ::android::hardware::radio::V1_6::QosProtocol::TCP) {
        return "TCP";
    }
    if (o == ::android::hardware::radio::V1_6::QosProtocol::UDP) {
        return "UDP";
    }
    if (o == ::android::hardware::radio::V1_6::QosProtocol::ESP) {
        return "ESP";
    }
    if (o == ::android::hardware::radio::V1_6::QosProtocol::AH) {
        return "AH";
    }
    std::string os;
    os += toHexString(static_cast<int8_t>(o));
    return os;
}

static inline void PrintTo(::android::hardware::radio::V1_6::QosProtocol o, ::std::ostream* os) {
    *os << toString(o);
}

template<>
inline std::string toString<::android::hardware::radio::V1_6::QosFilterDirection>(int8_t o) {
    using ::android::hardware::details::toHexString;
    std::string os;
    ::android::hardware::hidl_bitfield<::android::hardware::radio::V1_6::QosFilterDirection> flipped = 0;
    bool first = true;
    if ((o & ::android::hardware::radio::V1_6::QosFilterDirection::DOWNLINK) == static_cast<int8_t>(::android::hardware::radio::V1_6::QosFilterDirection::DOWNLINK)) {
        os += (first ? "" : " | ");
        os += "DOWNLINK";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::QosFilterDirection::DOWNLINK;
    }
    if ((o & ::android::hardware::radio::V1_6::QosFilterDirection::UPLINK) == static_cast<int8_t>(::android::hardware::radio::V1_6::QosFilterDirection::UPLINK)) {
        os += (first ? "" : " | ");
        os += "UPLINK";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::QosFilterDirection::UPLINK;
    }
    if ((o & ::android::hardware::radio::V1_6::QosFilterDirection::BIDIRECTIONAL) == static_cast<int8_t>(::android::hardware::radio::V1_6::QosFilterDirection::BIDIRECTIONAL)) {
        os += (first ? "" : " | ");
        os += "BIDIRECTIONAL";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::QosFilterDirection::BIDIRECTIONAL;
    }
    if (o != flipped) {
        os += (first ? "" : " | ");
        os += toHexString(o & (~flipped));
    }os += " (";
    os += toHexString(o);
    os += ")";
    return os;
}

static inline std::string toString(::android::hardware::radio::V1_6::QosFilterDirection o) {
    using ::android::hardware::details::toHexString;
    if (o == ::android::hardware::radio::V1_6::QosFilterDirection::DOWNLINK) {
        return "DOWNLINK";
    }
    if (o == ::android::hardware::radio::V1_6::QosFilterDirection::UPLINK) {
        return "UPLINK";
    }
    if (o == ::android::hardware::radio::V1_6::QosFilterDirection::BIDIRECTIONAL) {
        return "BIDIRECTIONAL";
    }
    std::string os;
    os += toHexString(static_cast<int8_t>(o));
    return os;
}

static inline void PrintTo(::android::hardware::radio::V1_6::QosFilterDirection o, ::std::ostream* os) {
    *os << toString(o);
}

template<>
inline std::string toString<::android::hardware::radio::V1_6::QosPortRange>(uint16_t o) {
    using ::android::hardware::details::toHexString;
    std::string os;
    ::android::hardware::hidl_bitfield<::android::hardware::radio::V1_6::QosPortRange> flipped = 0;
    bool first = true;
    if ((o & ::android::hardware::radio::V1_6::QosPortRange::MIN) == static_cast<uint16_t>(::android::hardware::radio::V1_6::QosPortRange::MIN)) {
        os += (first ? "" : " | ");
        os += "MIN";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::QosPortRange::MIN;
    }
    if ((o & ::android::hardware::radio::V1_6::QosPortRange::MAX) == static_cast<uint16_t>(::android::hardware::radio::V1_6::QosPortRange::MAX)) {
        os += (first ? "" : " | ");
        os += "MAX";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::QosPortRange::MAX;
    }
    if (o != flipped) {
        os += (first ? "" : " | ");
        os += toHexString(o & (~flipped));
    }os += " (";
    os += toHexString(o);
    os += ")";
    return os;
}

static inline std::string toString(::android::hardware::radio::V1_6::QosPortRange o) {
    using ::android::hardware::details::toHexString;
    if (o == ::android::hardware::radio::V1_6::QosPortRange::MIN) {
        return "MIN";
    }
    if (o == ::android::hardware::radio::V1_6::QosPortRange::MAX) {
        return "MAX";
    }
    std::string os;
    os += toHexString(static_cast<uint16_t>(o));
    return os;
}

static inline void PrintTo(::android::hardware::radio::V1_6::QosPortRange o, ::std::ostream* os) {
    *os << toString(o);
}

template<>
inline std::string toString<::android::hardware::radio::V1_6::RadioError>(int32_t o) {
    using ::android::hardware::details::toHexString;
    std::string os;
    ::android::hardware::hidl_bitfield<::android::hardware::radio::V1_6::RadioError> flipped = 0;
    bool first = true;
    if ((o & ::android::hardware::radio::V1_6::RadioError::NONE) == static_cast<int32_t>(::android::hardware::radio::V1_6::RadioError::NONE)) {
        os += (first ? "" : " | ");
        os += "NONE";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::RadioError::NONE;
    }
    if ((o & ::android::hardware::radio::V1_6::RadioError::RADIO_NOT_AVAILABLE) == static_cast<int32_t>(::android::hardware::radio::V1_6::RadioError::RADIO_NOT_AVAILABLE)) {
        os += (first ? "" : " | ");
        os += "RADIO_NOT_AVAILABLE";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::RadioError::RADIO_NOT_AVAILABLE;
    }
    if ((o & ::android::hardware::radio::V1_6::RadioError::GENERIC_FAILURE) == static_cast<int32_t>(::android::hardware::radio::V1_6::RadioError::GENERIC_FAILURE)) {
        os += (first ? "" : " | ");
        os += "GENERIC_FAILURE";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::RadioError::GENERIC_FAILURE;
    }
    if ((o & ::android::hardware::radio::V1_6::RadioError::PASSWORD_INCORRECT) == static_cast<int32_t>(::android::hardware::radio::V1_6::RadioError::PASSWORD_INCORRECT)) {
        os += (first ? "" : " | ");
        os += "PASSWORD_INCORRECT";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::RadioError::PASSWORD_INCORRECT;
    }
    if ((o & ::android::hardware::radio::V1_6::RadioError::SIM_PIN2) == static_cast<int32_t>(::android::hardware::radio::V1_6::RadioError::SIM_PIN2)) {
        os += (first ? "" : " | ");
        os += "SIM_PIN2";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::RadioError::SIM_PIN2;
    }
    if ((o & ::android::hardware::radio::V1_6::RadioError::SIM_PUK2) == static_cast<int32_t>(::android::hardware::radio::V1_6::RadioError::SIM_PUK2)) {
        os += (first ? "" : " | ");
        os += "SIM_PUK2";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::RadioError::SIM_PUK2;
    }
    if ((o & ::android::hardware::radio::V1_6::RadioError::REQUEST_NOT_SUPPORTED) == static_cast<int32_t>(::android::hardware::radio::V1_6::RadioError::REQUEST_NOT_SUPPORTED)) {
        os += (first ? "" : " | ");
        os += "REQUEST_NOT_SUPPORTED";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::RadioError::REQUEST_NOT_SUPPORTED;
    }
    if ((o & ::android::hardware::radio::V1_6::RadioError::CANCELLED) == static_cast<int32_t>(::android::hardware::radio::V1_6::RadioError::CANCELLED)) {
        os += (first ? "" : " | ");
        os += "CANCELLED";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::RadioError::CANCELLED;
    }
    if ((o & ::android::hardware::radio::V1_6::RadioError::OP_NOT_ALLOWED_DURING_VOICE_CALL) == static_cast<int32_t>(::android::hardware::radio::V1_6::RadioError::OP_NOT_ALLOWED_DURING_VOICE_CALL)) {
        os += (first ? "" : " | ");
        os += "OP_NOT_ALLOWED_DURING_VOICE_CALL";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::RadioError::OP_NOT_ALLOWED_DURING_VOICE_CALL;
    }
    if ((o & ::android::hardware::radio::V1_6::RadioError::OP_NOT_ALLOWED_BEFORE_REG_TO_NW) == static_cast<int32_t>(::android::hardware::radio::V1_6::RadioError::OP_NOT_ALLOWED_BEFORE_REG_TO_NW)) {
        os += (first ? "" : " | ");
        os += "OP_NOT_ALLOWED_BEFORE_REG_TO_NW";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::RadioError::OP_NOT_ALLOWED_BEFORE_REG_TO_NW;
    }
    if ((o & ::android::hardware::radio::V1_6::RadioError::SMS_SEND_FAIL_RETRY) == static_cast<int32_t>(::android::hardware::radio::V1_6::RadioError::SMS_SEND_FAIL_RETRY)) {
        os += (first ? "" : " | ");
        os += "SMS_SEND_FAIL_RETRY";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::RadioError::SMS_SEND_FAIL_RETRY;
    }
    if ((o & ::android::hardware::radio::V1_6::RadioError::SIM_ABSENT) == static_cast<int32_t>(::android::hardware::radio::V1_6::RadioError::SIM_ABSENT)) {
        os += (first ? "" : " | ");
        os += "SIM_ABSENT";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::RadioError::SIM_ABSENT;
    }
    if ((o & ::android::hardware::radio::V1_6::RadioError::SUBSCRIPTION_NOT_AVAILABLE) == static_cast<int32_t>(::android::hardware::radio::V1_6::RadioError::SUBSCRIPTION_NOT_AVAILABLE)) {
        os += (first ? "" : " | ");
        os += "SUBSCRIPTION_NOT_AVAILABLE";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::RadioError::SUBSCRIPTION_NOT_AVAILABLE;
    }
    if ((o & ::android::hardware::radio::V1_6::RadioError::MODE_NOT_SUPPORTED) == static_cast<int32_t>(::android::hardware::radio::V1_6::RadioError::MODE_NOT_SUPPORTED)) {
        os += (first ? "" : " | ");
        os += "MODE_NOT_SUPPORTED";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::RadioError::MODE_NOT_SUPPORTED;
    }
    if ((o & ::android::hardware::radio::V1_6::RadioError::FDN_CHECK_FAILURE) == static_cast<int32_t>(::android::hardware::radio::V1_6::RadioError::FDN_CHECK_FAILURE)) {
        os += (first ? "" : " | ");
        os += "FDN_CHECK_FAILURE";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::RadioError::FDN_CHECK_FAILURE;
    }
    if ((o & ::android::hardware::radio::V1_6::RadioError::ILLEGAL_SIM_OR_ME) == static_cast<int32_t>(::android::hardware::radio::V1_6::RadioError::ILLEGAL_SIM_OR_ME)) {
        os += (first ? "" : " | ");
        os += "ILLEGAL_SIM_OR_ME";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::RadioError::ILLEGAL_SIM_OR_ME;
    }
    if ((o & ::android::hardware::radio::V1_6::RadioError::MISSING_RESOURCE) == static_cast<int32_t>(::android::hardware::radio::V1_6::RadioError::MISSING_RESOURCE)) {
        os += (first ? "" : " | ");
        os += "MISSING_RESOURCE";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::RadioError::MISSING_RESOURCE;
    }
    if ((o & ::android::hardware::radio::V1_6::RadioError::NO_SUCH_ELEMENT) == static_cast<int32_t>(::android::hardware::radio::V1_6::RadioError::NO_SUCH_ELEMENT)) {
        os += (first ? "" : " | ");
        os += "NO_SUCH_ELEMENT";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::RadioError::NO_SUCH_ELEMENT;
    }
    if ((o & ::android::hardware::radio::V1_6::RadioError::DIAL_MODIFIED_TO_USSD) == static_cast<int32_t>(::android::hardware::radio::V1_6::RadioError::DIAL_MODIFIED_TO_USSD)) {
        os += (first ? "" : " | ");
        os += "DIAL_MODIFIED_TO_USSD";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::RadioError::DIAL_MODIFIED_TO_USSD;
    }
    if ((o & ::android::hardware::radio::V1_6::RadioError::DIAL_MODIFIED_TO_SS) == static_cast<int32_t>(::android::hardware::radio::V1_6::RadioError::DIAL_MODIFIED_TO_SS)) {
        os += (first ? "" : " | ");
        os += "DIAL_MODIFIED_TO_SS";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::RadioError::DIAL_MODIFIED_TO_SS;
    }
    if ((o & ::android::hardware::radio::V1_6::RadioError::DIAL_MODIFIED_TO_DIAL) == static_cast<int32_t>(::android::hardware::radio::V1_6::RadioError::DIAL_MODIFIED_TO_DIAL)) {
        os += (first ? "" : " | ");
        os += "DIAL_MODIFIED_TO_DIAL";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::RadioError::DIAL_MODIFIED_TO_DIAL;
    }
    if ((o & ::android::hardware::radio::V1_6::RadioError::USSD_MODIFIED_TO_DIAL) == static_cast<int32_t>(::android::hardware::radio::V1_6::RadioError::USSD_MODIFIED_TO_DIAL)) {
        os += (first ? "" : " | ");
        os += "USSD_MODIFIED_TO_DIAL";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::RadioError::USSD_MODIFIED_TO_DIAL;
    }
    if ((o & ::android::hardware::radio::V1_6::RadioError::USSD_MODIFIED_TO_SS) == static_cast<int32_t>(::android::hardware::radio::V1_6::RadioError::USSD_MODIFIED_TO_SS)) {
        os += (first ? "" : " | ");
        os += "USSD_MODIFIED_TO_SS";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::RadioError::USSD_MODIFIED_TO_SS;
    }
    if ((o & ::android::hardware::radio::V1_6::RadioError::USSD_MODIFIED_TO_USSD) == static_cast<int32_t>(::android::hardware::radio::V1_6::RadioError::USSD_MODIFIED_TO_USSD)) {
        os += (first ? "" : " | ");
        os += "USSD_MODIFIED_TO_USSD";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::RadioError::USSD_MODIFIED_TO_USSD;
    }
    if ((o & ::android::hardware::radio::V1_6::RadioError::SS_MODIFIED_TO_DIAL) == static_cast<int32_t>(::android::hardware::radio::V1_6::RadioError::SS_MODIFIED_TO_DIAL)) {
        os += (first ? "" : " | ");
        os += "SS_MODIFIED_TO_DIAL";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::RadioError::SS_MODIFIED_TO_DIAL;
    }
    if ((o & ::android::hardware::radio::V1_6::RadioError::SS_MODIFIED_TO_USSD) == static_cast<int32_t>(::android::hardware::radio::V1_6::RadioError::SS_MODIFIED_TO_USSD)) {
        os += (first ? "" : " | ");
        os += "SS_MODIFIED_TO_USSD";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::RadioError::SS_MODIFIED_TO_USSD;
    }
    if ((o & ::android::hardware::radio::V1_6::RadioError::SUBSCRIPTION_NOT_SUPPORTED) == static_cast<int32_t>(::android::hardware::radio::V1_6::RadioError::SUBSCRIPTION_NOT_SUPPORTED)) {
        os += (first ? "" : " | ");
        os += "SUBSCRIPTION_NOT_SUPPORTED";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::RadioError::SUBSCRIPTION_NOT_SUPPORTED;
    }
    if ((o & ::android::hardware::radio::V1_6::RadioError::SS_MODIFIED_TO_SS) == static_cast<int32_t>(::android::hardware::radio::V1_6::RadioError::SS_MODIFIED_TO_SS)) {
        os += (first ? "" : " | ");
        os += "SS_MODIFIED_TO_SS";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::RadioError::SS_MODIFIED_TO_SS;
    }
    if ((o & ::android::hardware::radio::V1_6::RadioError::LCE_NOT_SUPPORTED) == static_cast<int32_t>(::android::hardware::radio::V1_6::RadioError::LCE_NOT_SUPPORTED)) {
        os += (first ? "" : " | ");
        os += "LCE_NOT_SUPPORTED";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::RadioError::LCE_NOT_SUPPORTED;
    }
    if ((o & ::android::hardware::radio::V1_6::RadioError::NO_MEMORY) == static_cast<int32_t>(::android::hardware::radio::V1_6::RadioError::NO_MEMORY)) {
        os += (first ? "" : " | ");
        os += "NO_MEMORY";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::RadioError::NO_MEMORY;
    }
    if ((o & ::android::hardware::radio::V1_6::RadioError::INTERNAL_ERR) == static_cast<int32_t>(::android::hardware::radio::V1_6::RadioError::INTERNAL_ERR)) {
        os += (first ? "" : " | ");
        os += "INTERNAL_ERR";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::RadioError::INTERNAL_ERR;
    }
    if ((o & ::android::hardware::radio::V1_6::RadioError::SYSTEM_ERR) == static_cast<int32_t>(::android::hardware::radio::V1_6::RadioError::SYSTEM_ERR)) {
        os += (first ? "" : " | ");
        os += "SYSTEM_ERR";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::RadioError::SYSTEM_ERR;
    }
    if ((o & ::android::hardware::radio::V1_6::RadioError::MODEM_ERR) == static_cast<int32_t>(::android::hardware::radio::V1_6::RadioError::MODEM_ERR)) {
        os += (first ? "" : " | ");
        os += "MODEM_ERR";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::RadioError::MODEM_ERR;
    }
    if ((o & ::android::hardware::radio::V1_6::RadioError::INVALID_STATE) == static_cast<int32_t>(::android::hardware::radio::V1_6::RadioError::INVALID_STATE)) {
        os += (first ? "" : " | ");
        os += "INVALID_STATE";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::RadioError::INVALID_STATE;
    }
    if ((o & ::android::hardware::radio::V1_6::RadioError::NO_RESOURCES) == static_cast<int32_t>(::android::hardware::radio::V1_6::RadioError::NO_RESOURCES)) {
        os += (first ? "" : " | ");
        os += "NO_RESOURCES";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::RadioError::NO_RESOURCES;
    }
    if ((o & ::android::hardware::radio::V1_6::RadioError::SIM_ERR) == static_cast<int32_t>(::android::hardware::radio::V1_6::RadioError::SIM_ERR)) {
        os += (first ? "" : " | ");
        os += "SIM_ERR";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::RadioError::SIM_ERR;
    }
    if ((o & ::android::hardware::radio::V1_6::RadioError::INVALID_ARGUMENTS) == static_cast<int32_t>(::android::hardware::radio::V1_6::RadioError::INVALID_ARGUMENTS)) {
        os += (first ? "" : " | ");
        os += "INVALID_ARGUMENTS";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::RadioError::INVALID_ARGUMENTS;
    }
    if ((o & ::android::hardware::radio::V1_6::RadioError::INVALID_SIM_STATE) == static_cast<int32_t>(::android::hardware::radio::V1_6::RadioError::INVALID_SIM_STATE)) {
        os += (first ? "" : " | ");
        os += "INVALID_SIM_STATE";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::RadioError::INVALID_SIM_STATE;
    }
    if ((o & ::android::hardware::radio::V1_6::RadioError::INVALID_MODEM_STATE) == static_cast<int32_t>(::android::hardware::radio::V1_6::RadioError::INVALID_MODEM_STATE)) {
        os += (first ? "" : " | ");
        os += "INVALID_MODEM_STATE";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::RadioError::INVALID_MODEM_STATE;
    }
    if ((o & ::android::hardware::radio::V1_6::RadioError::INVALID_CALL_ID) == static_cast<int32_t>(::android::hardware::radio::V1_6::RadioError::INVALID_CALL_ID)) {
        os += (first ? "" : " | ");
        os += "INVALID_CALL_ID";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::RadioError::INVALID_CALL_ID;
    }
    if ((o & ::android::hardware::radio::V1_6::RadioError::NO_SMS_TO_ACK) == static_cast<int32_t>(::android::hardware::radio::V1_6::RadioError::NO_SMS_TO_ACK)) {
        os += (first ? "" : " | ");
        os += "NO_SMS_TO_ACK";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::RadioError::NO_SMS_TO_ACK;
    }
    if ((o & ::android::hardware::radio::V1_6::RadioError::NETWORK_ERR) == static_cast<int32_t>(::android::hardware::radio::V1_6::RadioError::NETWORK_ERR)) {
        os += (first ? "" : " | ");
        os += "NETWORK_ERR";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::RadioError::NETWORK_ERR;
    }
    if ((o & ::android::hardware::radio::V1_6::RadioError::REQUEST_RATE_LIMITED) == static_cast<int32_t>(::android::hardware::radio::V1_6::RadioError::REQUEST_RATE_LIMITED)) {
        os += (first ? "" : " | ");
        os += "REQUEST_RATE_LIMITED";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::RadioError::REQUEST_RATE_LIMITED;
    }
    if ((o & ::android::hardware::radio::V1_6::RadioError::SIM_BUSY) == static_cast<int32_t>(::android::hardware::radio::V1_6::RadioError::SIM_BUSY)) {
        os += (first ? "" : " | ");
        os += "SIM_BUSY";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::RadioError::SIM_BUSY;
    }
    if ((o & ::android::hardware::radio::V1_6::RadioError::SIM_FULL) == static_cast<int32_t>(::android::hardware::radio::V1_6::RadioError::SIM_FULL)) {
        os += (first ? "" : " | ");
        os += "SIM_FULL";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::RadioError::SIM_FULL;
    }
    if ((o & ::android::hardware::radio::V1_6::RadioError::NETWORK_REJECT) == static_cast<int32_t>(::android::hardware::radio::V1_6::RadioError::NETWORK_REJECT)) {
        os += (first ? "" : " | ");
        os += "NETWORK_REJECT";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::RadioError::NETWORK_REJECT;
    }
    if ((o & ::android::hardware::radio::V1_6::RadioError::OPERATION_NOT_ALLOWED) == static_cast<int32_t>(::android::hardware::radio::V1_6::RadioError::OPERATION_NOT_ALLOWED)) {
        os += (first ? "" : " | ");
        os += "OPERATION_NOT_ALLOWED";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::RadioError::OPERATION_NOT_ALLOWED;
    }
    if ((o & ::android::hardware::radio::V1_6::RadioError::EMPTY_RECORD) == static_cast<int32_t>(::android::hardware::radio::V1_6::RadioError::EMPTY_RECORD)) {
        os += (first ? "" : " | ");
        os += "EMPTY_RECORD";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::RadioError::EMPTY_RECORD;
    }
    if ((o & ::android::hardware::radio::V1_6::RadioError::INVALID_SMS_FORMAT) == static_cast<int32_t>(::android::hardware::radio::V1_6::RadioError::INVALID_SMS_FORMAT)) {
        os += (first ? "" : " | ");
        os += "INVALID_SMS_FORMAT";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::RadioError::INVALID_SMS_FORMAT;
    }
    if ((o & ::android::hardware::radio::V1_6::RadioError::ENCODING_ERR) == static_cast<int32_t>(::android::hardware::radio::V1_6::RadioError::ENCODING_ERR)) {
        os += (first ? "" : " | ");
        os += "ENCODING_ERR";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::RadioError::ENCODING_ERR;
    }
    if ((o & ::android::hardware::radio::V1_6::RadioError::INVALID_SMSC_ADDRESS) == static_cast<int32_t>(::android::hardware::radio::V1_6::RadioError::INVALID_SMSC_ADDRESS)) {
        os += (first ? "" : " | ");
        os += "INVALID_SMSC_ADDRESS";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::RadioError::INVALID_SMSC_ADDRESS;
    }
    if ((o & ::android::hardware::radio::V1_6::RadioError::NO_SUCH_ENTRY) == static_cast<int32_t>(::android::hardware::radio::V1_6::RadioError::NO_SUCH_ENTRY)) {
        os += (first ? "" : " | ");
        os += "NO_SUCH_ENTRY";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::RadioError::NO_SUCH_ENTRY;
    }
    if ((o & ::android::hardware::radio::V1_6::RadioError::NETWORK_NOT_READY) == static_cast<int32_t>(::android::hardware::radio::V1_6::RadioError::NETWORK_NOT_READY)) {
        os += (first ? "" : " | ");
        os += "NETWORK_NOT_READY";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::RadioError::NETWORK_NOT_READY;
    }
    if ((o & ::android::hardware::radio::V1_6::RadioError::NOT_PROVISIONED) == static_cast<int32_t>(::android::hardware::radio::V1_6::RadioError::NOT_PROVISIONED)) {
        os += (first ? "" : " | ");
        os += "NOT_PROVISIONED";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::RadioError::NOT_PROVISIONED;
    }
    if ((o & ::android::hardware::radio::V1_6::RadioError::NO_SUBSCRIPTION) == static_cast<int32_t>(::android::hardware::radio::V1_6::RadioError::NO_SUBSCRIPTION)) {
        os += (first ? "" : " | ");
        os += "NO_SUBSCRIPTION";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::RadioError::NO_SUBSCRIPTION;
    }
    if ((o & ::android::hardware::radio::V1_6::RadioError::NO_NETWORK_FOUND) == static_cast<int32_t>(::android::hardware::radio::V1_6::RadioError::NO_NETWORK_FOUND)) {
        os += (first ? "" : " | ");
        os += "NO_NETWORK_FOUND";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::RadioError::NO_NETWORK_FOUND;
    }
    if ((o & ::android::hardware::radio::V1_6::RadioError::DEVICE_IN_USE) == static_cast<int32_t>(::android::hardware::radio::V1_6::RadioError::DEVICE_IN_USE)) {
        os += (first ? "" : " | ");
        os += "DEVICE_IN_USE";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::RadioError::DEVICE_IN_USE;
    }
    if ((o & ::android::hardware::radio::V1_6::RadioError::ABORTED) == static_cast<int32_t>(::android::hardware::radio::V1_6::RadioError::ABORTED)) {
        os += (first ? "" : " | ");
        os += "ABORTED";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::RadioError::ABORTED;
    }
    if ((o & ::android::hardware::radio::V1_6::RadioError::INVALID_RESPONSE) == static_cast<int32_t>(::android::hardware::radio::V1_6::RadioError::INVALID_RESPONSE)) {
        os += (first ? "" : " | ");
        os += "INVALID_RESPONSE";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::RadioError::INVALID_RESPONSE;
    }
    if ((o & ::android::hardware::radio::V1_6::RadioError::OEM_ERROR_1) == static_cast<int32_t>(::android::hardware::radio::V1_6::RadioError::OEM_ERROR_1)) {
        os += (first ? "" : " | ");
        os += "OEM_ERROR_1";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::RadioError::OEM_ERROR_1;
    }
    if ((o & ::android::hardware::radio::V1_6::RadioError::OEM_ERROR_2) == static_cast<int32_t>(::android::hardware::radio::V1_6::RadioError::OEM_ERROR_2)) {
        os += (first ? "" : " | ");
        os += "OEM_ERROR_2";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::RadioError::OEM_ERROR_2;
    }
    if ((o & ::android::hardware::radio::V1_6::RadioError::OEM_ERROR_3) == static_cast<int32_t>(::android::hardware::radio::V1_6::RadioError::OEM_ERROR_3)) {
        os += (first ? "" : " | ");
        os += "OEM_ERROR_3";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::RadioError::OEM_ERROR_3;
    }
    if ((o & ::android::hardware::radio::V1_6::RadioError::OEM_ERROR_4) == static_cast<int32_t>(::android::hardware::radio::V1_6::RadioError::OEM_ERROR_4)) {
        os += (first ? "" : " | ");
        os += "OEM_ERROR_4";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::RadioError::OEM_ERROR_4;
    }
    if ((o & ::android::hardware::radio::V1_6::RadioError::OEM_ERROR_5) == static_cast<int32_t>(::android::hardware::radio::V1_6::RadioError::OEM_ERROR_5)) {
        os += (first ? "" : " | ");
        os += "OEM_ERROR_5";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::RadioError::OEM_ERROR_5;
    }
    if ((o & ::android::hardware::radio::V1_6::RadioError::OEM_ERROR_6) == static_cast<int32_t>(::android::hardware::radio::V1_6::RadioError::OEM_ERROR_6)) {
        os += (first ? "" : " | ");
        os += "OEM_ERROR_6";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::RadioError::OEM_ERROR_6;
    }
    if ((o & ::android::hardware::radio::V1_6::RadioError::OEM_ERROR_7) == static_cast<int32_t>(::android::hardware::radio::V1_6::RadioError::OEM_ERROR_7)) {
        os += (first ? "" : " | ");
        os += "OEM_ERROR_7";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::RadioError::OEM_ERROR_7;
    }
    if ((o & ::android::hardware::radio::V1_6::RadioError::OEM_ERROR_8) == static_cast<int32_t>(::android::hardware::radio::V1_6::RadioError::OEM_ERROR_8)) {
        os += (first ? "" : " | ");
        os += "OEM_ERROR_8";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::RadioError::OEM_ERROR_8;
    }
    if ((o & ::android::hardware::radio::V1_6::RadioError::OEM_ERROR_9) == static_cast<int32_t>(::android::hardware::radio::V1_6::RadioError::OEM_ERROR_9)) {
        os += (first ? "" : " | ");
        os += "OEM_ERROR_9";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::RadioError::OEM_ERROR_9;
    }
    if ((o & ::android::hardware::radio::V1_6::RadioError::OEM_ERROR_10) == static_cast<int32_t>(::android::hardware::radio::V1_6::RadioError::OEM_ERROR_10)) {
        os += (first ? "" : " | ");
        os += "OEM_ERROR_10";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::RadioError::OEM_ERROR_10;
    }
    if ((o & ::android::hardware::radio::V1_6::RadioError::OEM_ERROR_11) == static_cast<int32_t>(::android::hardware::radio::V1_6::RadioError::OEM_ERROR_11)) {
        os += (first ? "" : " | ");
        os += "OEM_ERROR_11";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::RadioError::OEM_ERROR_11;
    }
    if ((o & ::android::hardware::radio::V1_6::RadioError::OEM_ERROR_12) == static_cast<int32_t>(::android::hardware::radio::V1_6::RadioError::OEM_ERROR_12)) {
        os += (first ? "" : " | ");
        os += "OEM_ERROR_12";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::RadioError::OEM_ERROR_12;
    }
    if ((o & ::android::hardware::radio::V1_6::RadioError::OEM_ERROR_13) == static_cast<int32_t>(::android::hardware::radio::V1_6::RadioError::OEM_ERROR_13)) {
        os += (first ? "" : " | ");
        os += "OEM_ERROR_13";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::RadioError::OEM_ERROR_13;
    }
    if ((o & ::android::hardware::radio::V1_6::RadioError::OEM_ERROR_14) == static_cast<int32_t>(::android::hardware::radio::V1_6::RadioError::OEM_ERROR_14)) {
        os += (first ? "" : " | ");
        os += "OEM_ERROR_14";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::RadioError::OEM_ERROR_14;
    }
    if ((o & ::android::hardware::radio::V1_6::RadioError::OEM_ERROR_15) == static_cast<int32_t>(::android::hardware::radio::V1_6::RadioError::OEM_ERROR_15)) {
        os += (first ? "" : " | ");
        os += "OEM_ERROR_15";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::RadioError::OEM_ERROR_15;
    }
    if ((o & ::android::hardware::radio::V1_6::RadioError::OEM_ERROR_16) == static_cast<int32_t>(::android::hardware::radio::V1_6::RadioError::OEM_ERROR_16)) {
        os += (first ? "" : " | ");
        os += "OEM_ERROR_16";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::RadioError::OEM_ERROR_16;
    }
    if ((o & ::android::hardware::radio::V1_6::RadioError::OEM_ERROR_17) == static_cast<int32_t>(::android::hardware::radio::V1_6::RadioError::OEM_ERROR_17)) {
        os += (first ? "" : " | ");
        os += "OEM_ERROR_17";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::RadioError::OEM_ERROR_17;
    }
    if ((o & ::android::hardware::radio::V1_6::RadioError::OEM_ERROR_18) == static_cast<int32_t>(::android::hardware::radio::V1_6::RadioError::OEM_ERROR_18)) {
        os += (first ? "" : " | ");
        os += "OEM_ERROR_18";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::RadioError::OEM_ERROR_18;
    }
    if ((o & ::android::hardware::radio::V1_6::RadioError::OEM_ERROR_19) == static_cast<int32_t>(::android::hardware::radio::V1_6::RadioError::OEM_ERROR_19)) {
        os += (first ? "" : " | ");
        os += "OEM_ERROR_19";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::RadioError::OEM_ERROR_19;
    }
    if ((o & ::android::hardware::radio::V1_6::RadioError::OEM_ERROR_20) == static_cast<int32_t>(::android::hardware::radio::V1_6::RadioError::OEM_ERROR_20)) {
        os += (first ? "" : " | ");
        os += "OEM_ERROR_20";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::RadioError::OEM_ERROR_20;
    }
    if ((o & ::android::hardware::radio::V1_6::RadioError::OEM_ERROR_21) == static_cast<int32_t>(::android::hardware::radio::V1_6::RadioError::OEM_ERROR_21)) {
        os += (first ? "" : " | ");
        os += "OEM_ERROR_21";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::RadioError::OEM_ERROR_21;
    }
    if ((o & ::android::hardware::radio::V1_6::RadioError::OEM_ERROR_22) == static_cast<int32_t>(::android::hardware::radio::V1_6::RadioError::OEM_ERROR_22)) {
        os += (first ? "" : " | ");
        os += "OEM_ERROR_22";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::RadioError::OEM_ERROR_22;
    }
    if ((o & ::android::hardware::radio::V1_6::RadioError::OEM_ERROR_23) == static_cast<int32_t>(::android::hardware::radio::V1_6::RadioError::OEM_ERROR_23)) {
        os += (first ? "" : " | ");
        os += "OEM_ERROR_23";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::RadioError::OEM_ERROR_23;
    }
    if ((o & ::android::hardware::radio::V1_6::RadioError::OEM_ERROR_24) == static_cast<int32_t>(::android::hardware::radio::V1_6::RadioError::OEM_ERROR_24)) {
        os += (first ? "" : " | ");
        os += "OEM_ERROR_24";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::RadioError::OEM_ERROR_24;
    }
    if ((o & ::android::hardware::radio::V1_6::RadioError::OEM_ERROR_25) == static_cast<int32_t>(::android::hardware::radio::V1_6::RadioError::OEM_ERROR_25)) {
        os += (first ? "" : " | ");
        os += "OEM_ERROR_25";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::RadioError::OEM_ERROR_25;
    }
    if ((o & ::android::hardware::radio::V1_6::RadioError::SIMULTANEOUS_SMS_AND_CALL_NOT_ALLOWED) == static_cast<int32_t>(::android::hardware::radio::V1_6::RadioError::SIMULTANEOUS_SMS_AND_CALL_NOT_ALLOWED)) {
        os += (first ? "" : " | ");
        os += "SIMULTANEOUS_SMS_AND_CALL_NOT_ALLOWED";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::RadioError::SIMULTANEOUS_SMS_AND_CALL_NOT_ALLOWED;
    }
    if ((o & ::android::hardware::radio::V1_6::RadioError::ACCESS_BARRED) == static_cast<int32_t>(::android::hardware::radio::V1_6::RadioError::ACCESS_BARRED)) {
        os += (first ? "" : " | ");
        os += "ACCESS_BARRED";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::RadioError::ACCESS_BARRED;
    }
    if ((o & ::android::hardware::radio::V1_6::RadioError::BLOCKED_DUE_TO_CALL) == static_cast<int32_t>(::android::hardware::radio::V1_6::RadioError::BLOCKED_DUE_TO_CALL)) {
        os += (first ? "" : " | ");
        os += "BLOCKED_DUE_TO_CALL";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::RadioError::BLOCKED_DUE_TO_CALL;
    }
    if ((o & ::android::hardware::radio::V1_6::RadioError::RF_HARDWARE_ISSUE) == static_cast<int32_t>(::android::hardware::radio::V1_6::RadioError::RF_HARDWARE_ISSUE)) {
        os += (first ? "" : " | ");
        os += "RF_HARDWARE_ISSUE";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::RadioError::RF_HARDWARE_ISSUE;
    }
    if ((o & ::android::hardware::radio::V1_6::RadioError::NO_RF_CALIBRATION_INFO) == static_cast<int32_t>(::android::hardware::radio::V1_6::RadioError::NO_RF_CALIBRATION_INFO)) {
        os += (first ? "" : " | ");
        os += "NO_RF_CALIBRATION_INFO";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::RadioError::NO_RF_CALIBRATION_INFO;
    }
    if (o != flipped) {
        os += (first ? "" : " | ");
        os += toHexString(o & (~flipped));
    }os += " (";
    os += toHexString(o);
    os += ")";
    return os;
}

static inline std::string toString(::android::hardware::radio::V1_6::RadioError o) {
    using ::android::hardware::details::toHexString;
    if (o == ::android::hardware::radio::V1_6::RadioError::NONE) {
        return "NONE";
    }
    if (o == ::android::hardware::radio::V1_6::RadioError::RADIO_NOT_AVAILABLE) {
        return "RADIO_NOT_AVAILABLE";
    }
    if (o == ::android::hardware::radio::V1_6::RadioError::GENERIC_FAILURE) {
        return "GENERIC_FAILURE";
    }
    if (o == ::android::hardware::radio::V1_6::RadioError::PASSWORD_INCORRECT) {
        return "PASSWORD_INCORRECT";
    }
    if (o == ::android::hardware::radio::V1_6::RadioError::SIM_PIN2) {
        return "SIM_PIN2";
    }
    if (o == ::android::hardware::radio::V1_6::RadioError::SIM_PUK2) {
        return "SIM_PUK2";
    }
    if (o == ::android::hardware::radio::V1_6::RadioError::REQUEST_NOT_SUPPORTED) {
        return "REQUEST_NOT_SUPPORTED";
    }
    if (o == ::android::hardware::radio::V1_6::RadioError::CANCELLED) {
        return "CANCELLED";
    }
    if (o == ::android::hardware::radio::V1_6::RadioError::OP_NOT_ALLOWED_DURING_VOICE_CALL) {
        return "OP_NOT_ALLOWED_DURING_VOICE_CALL";
    }
    if (o == ::android::hardware::radio::V1_6::RadioError::OP_NOT_ALLOWED_BEFORE_REG_TO_NW) {
        return "OP_NOT_ALLOWED_BEFORE_REG_TO_NW";
    }
    if (o == ::android::hardware::radio::V1_6::RadioError::SMS_SEND_FAIL_RETRY) {
        return "SMS_SEND_FAIL_RETRY";
    }
    if (o == ::android::hardware::radio::V1_6::RadioError::SIM_ABSENT) {
        return "SIM_ABSENT";
    }
    if (o == ::android::hardware::radio::V1_6::RadioError::SUBSCRIPTION_NOT_AVAILABLE) {
        return "SUBSCRIPTION_NOT_AVAILABLE";
    }
    if (o == ::android::hardware::radio::V1_6::RadioError::MODE_NOT_SUPPORTED) {
        return "MODE_NOT_SUPPORTED";
    }
    if (o == ::android::hardware::radio::V1_6::RadioError::FDN_CHECK_FAILURE) {
        return "FDN_CHECK_FAILURE";
    }
    if (o == ::android::hardware::radio::V1_6::RadioError::ILLEGAL_SIM_OR_ME) {
        return "ILLEGAL_SIM_OR_ME";
    }
    if (o == ::android::hardware::radio::V1_6::RadioError::MISSING_RESOURCE) {
        return "MISSING_RESOURCE";
    }
    if (o == ::android::hardware::radio::V1_6::RadioError::NO_SUCH_ELEMENT) {
        return "NO_SUCH_ELEMENT";
    }
    if (o == ::android::hardware::radio::V1_6::RadioError::DIAL_MODIFIED_TO_USSD) {
        return "DIAL_MODIFIED_TO_USSD";
    }
    if (o == ::android::hardware::radio::V1_6::RadioError::DIAL_MODIFIED_TO_SS) {
        return "DIAL_MODIFIED_TO_SS";
    }
    if (o == ::android::hardware::radio::V1_6::RadioError::DIAL_MODIFIED_TO_DIAL) {
        return "DIAL_MODIFIED_TO_DIAL";
    }
    if (o == ::android::hardware::radio::V1_6::RadioError::USSD_MODIFIED_TO_DIAL) {
        return "USSD_MODIFIED_TO_DIAL";
    }
    if (o == ::android::hardware::radio::V1_6::RadioError::USSD_MODIFIED_TO_SS) {
        return "USSD_MODIFIED_TO_SS";
    }
    if (o == ::android::hardware::radio::V1_6::RadioError::USSD_MODIFIED_TO_USSD) {
        return "USSD_MODIFIED_TO_USSD";
    }
    if (o == ::android::hardware::radio::V1_6::RadioError::SS_MODIFIED_TO_DIAL) {
        return "SS_MODIFIED_TO_DIAL";
    }
    if (o == ::android::hardware::radio::V1_6::RadioError::SS_MODIFIED_TO_USSD) {
        return "SS_MODIFIED_TO_USSD";
    }
    if (o == ::android::hardware::radio::V1_6::RadioError::SUBSCRIPTION_NOT_SUPPORTED) {
        return "SUBSCRIPTION_NOT_SUPPORTED";
    }
    if (o == ::android::hardware::radio::V1_6::RadioError::SS_MODIFIED_TO_SS) {
        return "SS_MODIFIED_TO_SS";
    }
    if (o == ::android::hardware::radio::V1_6::RadioError::LCE_NOT_SUPPORTED) {
        return "LCE_NOT_SUPPORTED";
    }
    if (o == ::android::hardware::radio::V1_6::RadioError::NO_MEMORY) {
        return "NO_MEMORY";
    }
    if (o == ::android::hardware::radio::V1_6::RadioError::INTERNAL_ERR) {
        return "INTERNAL_ERR";
    }
    if (o == ::android::hardware::radio::V1_6::RadioError::SYSTEM_ERR) {
        return "SYSTEM_ERR";
    }
    if (o == ::android::hardware::radio::V1_6::RadioError::MODEM_ERR) {
        return "MODEM_ERR";
    }
    if (o == ::android::hardware::radio::V1_6::RadioError::INVALID_STATE) {
        return "INVALID_STATE";
    }
    if (o == ::android::hardware::radio::V1_6::RadioError::NO_RESOURCES) {
        return "NO_RESOURCES";
    }
    if (o == ::android::hardware::radio::V1_6::RadioError::SIM_ERR) {
        return "SIM_ERR";
    }
    if (o == ::android::hardware::radio::V1_6::RadioError::INVALID_ARGUMENTS) {
        return "INVALID_ARGUMENTS";
    }
    if (o == ::android::hardware::radio::V1_6::RadioError::INVALID_SIM_STATE) {
        return "INVALID_SIM_STATE";
    }
    if (o == ::android::hardware::radio::V1_6::RadioError::INVALID_MODEM_STATE) {
        return "INVALID_MODEM_STATE";
    }
    if (o == ::android::hardware::radio::V1_6::RadioError::INVALID_CALL_ID) {
        return "INVALID_CALL_ID";
    }
    if (o == ::android::hardware::radio::V1_6::RadioError::NO_SMS_TO_ACK) {
        return "NO_SMS_TO_ACK";
    }
    if (o == ::android::hardware::radio::V1_6::RadioError::NETWORK_ERR) {
        return "NETWORK_ERR";
    }
    if (o == ::android::hardware::radio::V1_6::RadioError::REQUEST_RATE_LIMITED) {
        return "REQUEST_RATE_LIMITED";
    }
    if (o == ::android::hardware::radio::V1_6::RadioError::SIM_BUSY) {
        return "SIM_BUSY";
    }
    if (o == ::android::hardware::radio::V1_6::RadioError::SIM_FULL) {
        return "SIM_FULL";
    }
    if (o == ::android::hardware::radio::V1_6::RadioError::NETWORK_REJECT) {
        return "NETWORK_REJECT";
    }
    if (o == ::android::hardware::radio::V1_6::RadioError::OPERATION_NOT_ALLOWED) {
        return "OPERATION_NOT_ALLOWED";
    }
    if (o == ::android::hardware::radio::V1_6::RadioError::EMPTY_RECORD) {
        return "EMPTY_RECORD";
    }
    if (o == ::android::hardware::radio::V1_6::RadioError::INVALID_SMS_FORMAT) {
        return "INVALID_SMS_FORMAT";
    }
    if (o == ::android::hardware::radio::V1_6::RadioError::ENCODING_ERR) {
        return "ENCODING_ERR";
    }
    if (o == ::android::hardware::radio::V1_6::RadioError::INVALID_SMSC_ADDRESS) {
        return "INVALID_SMSC_ADDRESS";
    }
    if (o == ::android::hardware::radio::V1_6::RadioError::NO_SUCH_ENTRY) {
        return "NO_SUCH_ENTRY";
    }
    if (o == ::android::hardware::radio::V1_6::RadioError::NETWORK_NOT_READY) {
        return "NETWORK_NOT_READY";
    }
    if (o == ::android::hardware::radio::V1_6::RadioError::NOT_PROVISIONED) {
        return "NOT_PROVISIONED";
    }
    if (o == ::android::hardware::radio::V1_6::RadioError::NO_SUBSCRIPTION) {
        return "NO_SUBSCRIPTION";
    }
    if (o == ::android::hardware::radio::V1_6::RadioError::NO_NETWORK_FOUND) {
        return "NO_NETWORK_FOUND";
    }
    if (o == ::android::hardware::radio::V1_6::RadioError::DEVICE_IN_USE) {
        return "DEVICE_IN_USE";
    }
    if (o == ::android::hardware::radio::V1_6::RadioError::ABORTED) {
        return "ABORTED";
    }
    if (o == ::android::hardware::radio::V1_6::RadioError::INVALID_RESPONSE) {
        return "INVALID_RESPONSE";
    }
    if (o == ::android::hardware::radio::V1_6::RadioError::OEM_ERROR_1) {
        return "OEM_ERROR_1";
    }
    if (o == ::android::hardware::radio::V1_6::RadioError::OEM_ERROR_2) {
        return "OEM_ERROR_2";
    }
    if (o == ::android::hardware::radio::V1_6::RadioError::OEM_ERROR_3) {
        return "OEM_ERROR_3";
    }
    if (o == ::android::hardware::radio::V1_6::RadioError::OEM_ERROR_4) {
        return "OEM_ERROR_4";
    }
    if (o == ::android::hardware::radio::V1_6::RadioError::OEM_ERROR_5) {
        return "OEM_ERROR_5";
    }
    if (o == ::android::hardware::radio::V1_6::RadioError::OEM_ERROR_6) {
        return "OEM_ERROR_6";
    }
    if (o == ::android::hardware::radio::V1_6::RadioError::OEM_ERROR_7) {
        return "OEM_ERROR_7";
    }
    if (o == ::android::hardware::radio::V1_6::RadioError::OEM_ERROR_8) {
        return "OEM_ERROR_8";
    }
    if (o == ::android::hardware::radio::V1_6::RadioError::OEM_ERROR_9) {
        return "OEM_ERROR_9";
    }
    if (o == ::android::hardware::radio::V1_6::RadioError::OEM_ERROR_10) {
        return "OEM_ERROR_10";
    }
    if (o == ::android::hardware::radio::V1_6::RadioError::OEM_ERROR_11) {
        return "OEM_ERROR_11";
    }
    if (o == ::android::hardware::radio::V1_6::RadioError::OEM_ERROR_12) {
        return "OEM_ERROR_12";
    }
    if (o == ::android::hardware::radio::V1_6::RadioError::OEM_ERROR_13) {
        return "OEM_ERROR_13";
    }
    if (o == ::android::hardware::radio::V1_6::RadioError::OEM_ERROR_14) {
        return "OEM_ERROR_14";
    }
    if (o == ::android::hardware::radio::V1_6::RadioError::OEM_ERROR_15) {
        return "OEM_ERROR_15";
    }
    if (o == ::android::hardware::radio::V1_6::RadioError::OEM_ERROR_16) {
        return "OEM_ERROR_16";
    }
    if (o == ::android::hardware::radio::V1_6::RadioError::OEM_ERROR_17) {
        return "OEM_ERROR_17";
    }
    if (o == ::android::hardware::radio::V1_6::RadioError::OEM_ERROR_18) {
        return "OEM_ERROR_18";
    }
    if (o == ::android::hardware::radio::V1_6::RadioError::OEM_ERROR_19) {
        return "OEM_ERROR_19";
    }
    if (o == ::android::hardware::radio::V1_6::RadioError::OEM_ERROR_20) {
        return "OEM_ERROR_20";
    }
    if (o == ::android::hardware::radio::V1_6::RadioError::OEM_ERROR_21) {
        return "OEM_ERROR_21";
    }
    if (o == ::android::hardware::radio::V1_6::RadioError::OEM_ERROR_22) {
        return "OEM_ERROR_22";
    }
    if (o == ::android::hardware::radio::V1_6::RadioError::OEM_ERROR_23) {
        return "OEM_ERROR_23";
    }
    if (o == ::android::hardware::radio::V1_6::RadioError::OEM_ERROR_24) {
        return "OEM_ERROR_24";
    }
    if (o == ::android::hardware::radio::V1_6::RadioError::OEM_ERROR_25) {
        return "OEM_ERROR_25";
    }
    if (o == ::android::hardware::radio::V1_6::RadioError::SIMULTANEOUS_SMS_AND_CALL_NOT_ALLOWED) {
        return "SIMULTANEOUS_SMS_AND_CALL_NOT_ALLOWED";
    }
    if (o == ::android::hardware::radio::V1_6::RadioError::ACCESS_BARRED) {
        return "ACCESS_BARRED";
    }
    if (o == ::android::hardware::radio::V1_6::RadioError::BLOCKED_DUE_TO_CALL) {
        return "BLOCKED_DUE_TO_CALL";
    }
    if (o == ::android::hardware::radio::V1_6::RadioError::RF_HARDWARE_ISSUE) {
        return "RF_HARDWARE_ISSUE";
    }
    if (o == ::android::hardware::radio::V1_6::RadioError::NO_RF_CALIBRATION_INFO) {
        return "NO_RF_CALIBRATION_INFO";
    }
    std::string os;
    os += toHexString(static_cast<int32_t>(o));
    return os;
}

static inline void PrintTo(::android::hardware::radio::V1_6::RadioError o, ::std::ostream* os) {
    *os << toString(o);
}

static inline std::string toString(const ::android::hardware::radio::V1_6::RadioResponseInfo& o) {
    using ::android::hardware::toString;
    std::string os;
    os += "{";
    os += ".type = ";
    os += ::android::hardware::radio::V1_0::toString(o.type);
    os += ", .serial = ";
    os += ::android::hardware::toString(o.serial);
    os += ", .error = ";
    os += ::android::hardware::radio::V1_6::toString(o.error);
    os += "}"; return os;
}

static inline void PrintTo(const ::android::hardware::radio::V1_6::RadioResponseInfo& o, ::std::ostream* os) {
    *os << toString(o);
}

static inline bool operator==(const ::android::hardware::radio::V1_6::RadioResponseInfo& lhs, const ::android::hardware::radio::V1_6::RadioResponseInfo& rhs) {
    if (lhs.type != rhs.type) {
        return false;
    }
    if (lhs.serial != rhs.serial) {
        return false;
    }
    if (lhs.error != rhs.error) {
        return false;
    }
    return true;
}

static inline bool operator!=(const ::android::hardware::radio::V1_6::RadioResponseInfo& lhs, const ::android::hardware::radio::V1_6::RadioResponseInfo& rhs){
    return !(lhs == rhs);
}

static inline std::string toString(const ::android::hardware::radio::V1_6::PortRange& o) {
    using ::android::hardware::toString;
    std::string os;
    os += "{";
    os += ".start = ";
    os += ::android::hardware::toString(o.start);
    os += ", .end = ";
    os += ::android::hardware::toString(o.end);
    os += "}"; return os;
}

static inline void PrintTo(const ::android::hardware::radio::V1_6::PortRange& o, ::std::ostream* os) {
    *os << toString(o);
}

static inline bool operator==(const ::android::hardware::radio::V1_6::PortRange& lhs, const ::android::hardware::radio::V1_6::PortRange& rhs) {
    if (lhs.start != rhs.start) {
        return false;
    }
    if (lhs.end != rhs.end) {
        return false;
    }
    return true;
}

static inline bool operator!=(const ::android::hardware::radio::V1_6::PortRange& lhs, const ::android::hardware::radio::V1_6::PortRange& rhs){
    return !(lhs == rhs);
}

static inline std::string toString(const ::android::hardware::radio::V1_6::MaybePort& o) {
    using ::android::hardware::toString;
    std::string os;
    os += "{";

    switch (o.getDiscriminator()) {
        case ::android::hardware::radio::V1_6::MaybePort::hidl_discriminator::noinit: {
            os += ".noinit = ";
            os += toString(o.noinit());
            break;
        }
        case ::android::hardware::radio::V1_6::MaybePort::hidl_discriminator::range: {
            os += ".range = ";
            os += toString(o.range());
            break;
        }
        default: {
            ::android::hardware::details::logAlwaysFatal((
                    "Unknown union discriminator (value: " +
                    std::to_string((uint8_t) o.getDiscriminator()) + ").").c_str());
        }
    }
    os += "}"; return os;
}

static inline void PrintTo(const ::android::hardware::radio::V1_6::MaybePort& o, ::std::ostream* os) {
    *os << toString(o);
}

static inline bool operator==(const ::android::hardware::radio::V1_6::MaybePort& lhs, const ::android::hardware::radio::V1_6::MaybePort& rhs) {
    if (lhs.getDiscriminator() != rhs.getDiscriminator()) {
        return false;
    }
    switch (lhs.getDiscriminator()) {
        case ::android::hardware::radio::V1_6::MaybePort::hidl_discriminator::noinit: {
            return (lhs.noinit() == rhs.noinit());
        }
        case ::android::hardware::radio::V1_6::MaybePort::hidl_discriminator::range: {
            return (lhs.range() == rhs.range());
        }
        default: {
            ::android::hardware::details::logAlwaysFatal((
                    "Unknown union discriminator (value: " +
                    std::to_string((uint8_t) lhs.getDiscriminator()) + ").").c_str());
        }
    }
    return true;
}

static inline bool operator!=(const ::android::hardware::radio::V1_6::MaybePort& lhs, const ::android::hardware::radio::V1_6::MaybePort& rhs){
    return !(lhs == rhs);
}

static inline std::string toString(const ::android::hardware::radio::V1_6::QosFilter::TypeOfService& o) {
    using ::android::hardware::toString;
    std::string os;
    os += "{";

    switch (o.getDiscriminator()) {
        case ::android::hardware::radio::V1_6::QosFilter::TypeOfService::hidl_discriminator::noinit: {
            os += ".noinit = ";
            os += toString(o.noinit());
            break;
        }
        case ::android::hardware::radio::V1_6::QosFilter::TypeOfService::hidl_discriminator::value: {
            os += ".value = ";
            os += toString(o.value());
            break;
        }
        default: {
            ::android::hardware::details::logAlwaysFatal((
                    "Unknown union discriminator (value: " +
                    std::to_string((uint8_t) o.getDiscriminator()) + ").").c_str());
        }
    }
    os += "}"; return os;
}

static inline void PrintTo(const ::android::hardware::radio::V1_6::QosFilter::TypeOfService& o, ::std::ostream* os) {
    *os << toString(o);
}

static inline bool operator==(const ::android::hardware::radio::V1_6::QosFilter::TypeOfService& lhs, const ::android::hardware::radio::V1_6::QosFilter::TypeOfService& rhs) {
    if (lhs.getDiscriminator() != rhs.getDiscriminator()) {
        return false;
    }
    switch (lhs.getDiscriminator()) {
        case ::android::hardware::radio::V1_6::QosFilter::TypeOfService::hidl_discriminator::noinit: {
            return (lhs.noinit() == rhs.noinit());
        }
        case ::android::hardware::radio::V1_6::QosFilter::TypeOfService::hidl_discriminator::value: {
            return (lhs.value() == rhs.value());
        }
        default: {
            ::android::hardware::details::logAlwaysFatal((
                    "Unknown union discriminator (value: " +
                    std::to_string((uint8_t) lhs.getDiscriminator()) + ").").c_str());
        }
    }
    return true;
}

static inline bool operator!=(const ::android::hardware::radio::V1_6::QosFilter::TypeOfService& lhs, const ::android::hardware::radio::V1_6::QosFilter::TypeOfService& rhs){
    return !(lhs == rhs);
}

static inline std::string toString(const ::android::hardware::radio::V1_6::QosFilter::Ipv6FlowLabel& o) {
    using ::android::hardware::toString;
    std::string os;
    os += "{";

    switch (o.getDiscriminator()) {
        case ::android::hardware::radio::V1_6::QosFilter::Ipv6FlowLabel::hidl_discriminator::noinit: {
            os += ".noinit = ";
            os += toString(o.noinit());
            break;
        }
        case ::android::hardware::radio::V1_6::QosFilter::Ipv6FlowLabel::hidl_discriminator::value: {
            os += ".value = ";
            os += toString(o.value());
            break;
        }
        default: {
            ::android::hardware::details::logAlwaysFatal((
                    "Unknown union discriminator (value: " +
                    std::to_string((uint8_t) o.getDiscriminator()) + ").").c_str());
        }
    }
    os += "}"; return os;
}

static inline void PrintTo(const ::android::hardware::radio::V1_6::QosFilter::Ipv6FlowLabel& o, ::std::ostream* os) {
    *os << toString(o);
}

static inline bool operator==(const ::android::hardware::radio::V1_6::QosFilter::Ipv6FlowLabel& lhs, const ::android::hardware::radio::V1_6::QosFilter::Ipv6FlowLabel& rhs) {
    if (lhs.getDiscriminator() != rhs.getDiscriminator()) {
        return false;
    }
    switch (lhs.getDiscriminator()) {
        case ::android::hardware::radio::V1_6::QosFilter::Ipv6FlowLabel::hidl_discriminator::noinit: {
            return (lhs.noinit() == rhs.noinit());
        }
        case ::android::hardware::radio::V1_6::QosFilter::Ipv6FlowLabel::hidl_discriminator::value: {
            return (lhs.value() == rhs.value());
        }
        default: {
            ::android::hardware::details::logAlwaysFatal((
                    "Unknown union discriminator (value: " +
                    std::to_string((uint8_t) lhs.getDiscriminator()) + ").").c_str());
        }
    }
    return true;
}

static inline bool operator!=(const ::android::hardware::radio::V1_6::QosFilter::Ipv6FlowLabel& lhs, const ::android::hardware::radio::V1_6::QosFilter::Ipv6FlowLabel& rhs){
    return !(lhs == rhs);
}

static inline std::string toString(const ::android::hardware::radio::V1_6::QosFilter::IpsecSpi& o) {
    using ::android::hardware::toString;
    std::string os;
    os += "{";

    switch (o.getDiscriminator()) {
        case ::android::hardware::radio::V1_6::QosFilter::IpsecSpi::hidl_discriminator::noinit: {
            os += ".noinit = ";
            os += toString(o.noinit());
            break;
        }
        case ::android::hardware::radio::V1_6::QosFilter::IpsecSpi::hidl_discriminator::value: {
            os += ".value = ";
            os += toString(o.value());
            break;
        }
        default: {
            ::android::hardware::details::logAlwaysFatal((
                    "Unknown union discriminator (value: " +
                    std::to_string((uint8_t) o.getDiscriminator()) + ").").c_str());
        }
    }
    os += "}"; return os;
}

static inline void PrintTo(const ::android::hardware::radio::V1_6::QosFilter::IpsecSpi& o, ::std::ostream* os) {
    *os << toString(o);
}

static inline bool operator==(const ::android::hardware::radio::V1_6::QosFilter::IpsecSpi& lhs, const ::android::hardware::radio::V1_6::QosFilter::IpsecSpi& rhs) {
    if (lhs.getDiscriminator() != rhs.getDiscriminator()) {
        return false;
    }
    switch (lhs.getDiscriminator()) {
        case ::android::hardware::radio::V1_6::QosFilter::IpsecSpi::hidl_discriminator::noinit: {
            return (lhs.noinit() == rhs.noinit());
        }
        case ::android::hardware::radio::V1_6::QosFilter::IpsecSpi::hidl_discriminator::value: {
            return (lhs.value() == rhs.value());
        }
        default: {
            ::android::hardware::details::logAlwaysFatal((
                    "Unknown union discriminator (value: " +
                    std::to_string((uint8_t) lhs.getDiscriminator()) + ").").c_str());
        }
    }
    return true;
}

static inline bool operator!=(const ::android::hardware::radio::V1_6::QosFilter::IpsecSpi& lhs, const ::android::hardware::radio::V1_6::QosFilter::IpsecSpi& rhs){
    return !(lhs == rhs);
}

static inline std::string toString(const ::android::hardware::radio::V1_6::QosFilter& o) {
    using ::android::hardware::toString;
    std::string os;
    os += "{";
    os += ".localAddresses = ";
    os += ::android::hardware::toString(o.localAddresses);
    os += ", .remoteAddresses = ";
    os += ::android::hardware::toString(o.remoteAddresses);
    os += ", .localPort = ";
    os += ::android::hardware::radio::V1_6::toString(o.localPort);
    os += ", .remotePort = ";
    os += ::android::hardware::radio::V1_6::toString(o.remotePort);
    os += ", .protocol = ";
    os += ::android::hardware::radio::V1_6::toString(o.protocol);
    os += ", .tos = ";
    os += ::android::hardware::radio::V1_6::toString(o.tos);
    os += ", .flowLabel = ";
    os += ::android::hardware::radio::V1_6::toString(o.flowLabel);
    os += ", .spi = ";
    os += ::android::hardware::radio::V1_6::toString(o.spi);
    os += ", .direction = ";
    os += ::android::hardware::radio::V1_6::toString(o.direction);
    os += ", .precedence = ";
    os += ::android::hardware::toString(o.precedence);
    os += "}"; return os;
}

static inline void PrintTo(const ::android::hardware::radio::V1_6::QosFilter& o, ::std::ostream* os) {
    *os << toString(o);
}

static inline bool operator==(const ::android::hardware::radio::V1_6::QosFilter& lhs, const ::android::hardware::radio::V1_6::QosFilter& rhs) {
    if (lhs.localAddresses != rhs.localAddresses) {
        return false;
    }
    if (lhs.remoteAddresses != rhs.remoteAddresses) {
        return false;
    }
    if (lhs.localPort != rhs.localPort) {
        return false;
    }
    if (lhs.remotePort != rhs.remotePort) {
        return false;
    }
    if (lhs.protocol != rhs.protocol) {
        return false;
    }
    if (lhs.tos != rhs.tos) {
        return false;
    }
    if (lhs.flowLabel != rhs.flowLabel) {
        return false;
    }
    if (lhs.spi != rhs.spi) {
        return false;
    }
    if (lhs.direction != rhs.direction) {
        return false;
    }
    if (lhs.precedence != rhs.precedence) {
        return false;
    }
    return true;
}

static inline bool operator!=(const ::android::hardware::radio::V1_6::QosFilter& lhs, const ::android::hardware::radio::V1_6::QosFilter& rhs){
    return !(lhs == rhs);
}

static inline std::string toString(const ::android::hardware::radio::V1_6::QosSession& o) {
    using ::android::hardware::toString;
    std::string os;
    os += "{";
    os += ".qosSessionId = ";
    os += ::android::hardware::toString(o.qosSessionId);
    os += ", .qos = ";
    os += ::android::hardware::radio::V1_6::toString(o.qos);
    os += ", .qosFilters = ";
    os += ::android::hardware::toString(o.qosFilters);
    os += "}"; return os;
}

static inline void PrintTo(const ::android::hardware::radio::V1_6::QosSession& o, ::std::ostream* os) {
    *os << toString(o);
}

static inline bool operator==(const ::android::hardware::radio::V1_6::QosSession& lhs, const ::android::hardware::radio::V1_6::QosSession& rhs) {
    if (lhs.qosSessionId != rhs.qosSessionId) {
        return false;
    }
    if (lhs.qos != rhs.qos) {
        return false;
    }
    if (lhs.qosFilters != rhs.qosFilters) {
        return false;
    }
    return true;
}

static inline bool operator!=(const ::android::hardware::radio::V1_6::QosSession& lhs, const ::android::hardware::radio::V1_6::QosSession& rhs){
    return !(lhs == rhs);
}

template<>
inline std::string toString<::android::hardware::radio::V1_6::HandoverFailureMode>(int8_t o) {
    using ::android::hardware::details::toHexString;
    std::string os;
    ::android::hardware::hidl_bitfield<::android::hardware::radio::V1_6::HandoverFailureMode> flipped = 0;
    bool first = true;
    if ((o & ::android::hardware::radio::V1_6::HandoverFailureMode::LEGACY) == static_cast<int8_t>(::android::hardware::radio::V1_6::HandoverFailureMode::LEGACY)) {
        os += (first ? "" : " | ");
        os += "LEGACY";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::HandoverFailureMode::LEGACY;
    }
    if ((o & ::android::hardware::radio::V1_6::HandoverFailureMode::DO_FALLBACK) == static_cast<int8_t>(::android::hardware::radio::V1_6::HandoverFailureMode::DO_FALLBACK)) {
        os += (first ? "" : " | ");
        os += "DO_FALLBACK";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::HandoverFailureMode::DO_FALLBACK;
    }
    if ((o & ::android::hardware::radio::V1_6::HandoverFailureMode::NO_FALLBACK_RETRY_HANDOVER) == static_cast<int8_t>(::android::hardware::radio::V1_6::HandoverFailureMode::NO_FALLBACK_RETRY_HANDOVER)) {
        os += (first ? "" : " | ");
        os += "NO_FALLBACK_RETRY_HANDOVER";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::HandoverFailureMode::NO_FALLBACK_RETRY_HANDOVER;
    }
    if ((o & ::android::hardware::radio::V1_6::HandoverFailureMode::NO_FALLBACK_RETRY_SETUP_NORMAL) == static_cast<int8_t>(::android::hardware::radio::V1_6::HandoverFailureMode::NO_FALLBACK_RETRY_SETUP_NORMAL)) {
        os += (first ? "" : " | ");
        os += "NO_FALLBACK_RETRY_SETUP_NORMAL";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::HandoverFailureMode::NO_FALLBACK_RETRY_SETUP_NORMAL;
    }
    if (o != flipped) {
        os += (first ? "" : " | ");
        os += toHexString(o & (~flipped));
    }os += " (";
    os += toHexString(o);
    os += ")";
    return os;
}

static inline std::string toString(::android::hardware::radio::V1_6::HandoverFailureMode o) {
    using ::android::hardware::details::toHexString;
    if (o == ::android::hardware::radio::V1_6::HandoverFailureMode::LEGACY) {
        return "LEGACY";
    }
    if (o == ::android::hardware::radio::V1_6::HandoverFailureMode::DO_FALLBACK) {
        return "DO_FALLBACK";
    }
    if (o == ::android::hardware::radio::V1_6::HandoverFailureMode::NO_FALLBACK_RETRY_HANDOVER) {
        return "NO_FALLBACK_RETRY_HANDOVER";
    }
    if (o == ::android::hardware::radio::V1_6::HandoverFailureMode::NO_FALLBACK_RETRY_SETUP_NORMAL) {
        return "NO_FALLBACK_RETRY_SETUP_NORMAL";
    }
    std::string os;
    os += toHexString(static_cast<int8_t>(o));
    return os;
}

static inline void PrintTo(::android::hardware::radio::V1_6::HandoverFailureMode o, ::std::ostream* os) {
    *os << toString(o);
}

template<>
inline std::string toString<::android::hardware::radio::V1_6::DataCallFailCause>(int32_t o) {
    using ::android::hardware::details::toHexString;
    std::string os;
    ::android::hardware::hidl_bitfield<::android::hardware::radio::V1_6::DataCallFailCause> flipped = 0;
    bool first = true;
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::NONE) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::NONE)) {
        os += (first ? "" : " | ");
        os += "NONE";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::NONE;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::OPERATOR_BARRED) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::OPERATOR_BARRED)) {
        os += (first ? "" : " | ");
        os += "OPERATOR_BARRED";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::OPERATOR_BARRED;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::NAS_SIGNALLING) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::NAS_SIGNALLING)) {
        os += (first ? "" : " | ");
        os += "NAS_SIGNALLING";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::NAS_SIGNALLING;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::INSUFFICIENT_RESOURCES) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::INSUFFICIENT_RESOURCES)) {
        os += (first ? "" : " | ");
        os += "INSUFFICIENT_RESOURCES";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::INSUFFICIENT_RESOURCES;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::MISSING_UKNOWN_APN) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::MISSING_UKNOWN_APN)) {
        os += (first ? "" : " | ");
        os += "MISSING_UKNOWN_APN";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::MISSING_UKNOWN_APN;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::UNKNOWN_PDP_ADDRESS_TYPE) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::UNKNOWN_PDP_ADDRESS_TYPE)) {
        os += (first ? "" : " | ");
        os += "UNKNOWN_PDP_ADDRESS_TYPE";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::UNKNOWN_PDP_ADDRESS_TYPE;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::USER_AUTHENTICATION) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::USER_AUTHENTICATION)) {
        os += (first ? "" : " | ");
        os += "USER_AUTHENTICATION";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::USER_AUTHENTICATION;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::ACTIVATION_REJECT_GGSN) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::ACTIVATION_REJECT_GGSN)) {
        os += (first ? "" : " | ");
        os += "ACTIVATION_REJECT_GGSN";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::ACTIVATION_REJECT_GGSN;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::ACTIVATION_REJECT_UNSPECIFIED) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::ACTIVATION_REJECT_UNSPECIFIED)) {
        os += (first ? "" : " | ");
        os += "ACTIVATION_REJECT_UNSPECIFIED";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::ACTIVATION_REJECT_UNSPECIFIED;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::SERVICE_OPTION_NOT_SUPPORTED) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::SERVICE_OPTION_NOT_SUPPORTED)) {
        os += (first ? "" : " | ");
        os += "SERVICE_OPTION_NOT_SUPPORTED";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::SERVICE_OPTION_NOT_SUPPORTED;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::SERVICE_OPTION_NOT_SUBSCRIBED) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::SERVICE_OPTION_NOT_SUBSCRIBED)) {
        os += (first ? "" : " | ");
        os += "SERVICE_OPTION_NOT_SUBSCRIBED";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::SERVICE_OPTION_NOT_SUBSCRIBED;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::SERVICE_OPTION_OUT_OF_ORDER) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::SERVICE_OPTION_OUT_OF_ORDER)) {
        os += (first ? "" : " | ");
        os += "SERVICE_OPTION_OUT_OF_ORDER";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::SERVICE_OPTION_OUT_OF_ORDER;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::NSAPI_IN_USE) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::NSAPI_IN_USE)) {
        os += (first ? "" : " | ");
        os += "NSAPI_IN_USE";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::NSAPI_IN_USE;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::REGULAR_DEACTIVATION) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::REGULAR_DEACTIVATION)) {
        os += (first ? "" : " | ");
        os += "REGULAR_DEACTIVATION";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::REGULAR_DEACTIVATION;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::QOS_NOT_ACCEPTED) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::QOS_NOT_ACCEPTED)) {
        os += (first ? "" : " | ");
        os += "QOS_NOT_ACCEPTED";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::QOS_NOT_ACCEPTED;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::NETWORK_FAILURE) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::NETWORK_FAILURE)) {
        os += (first ? "" : " | ");
        os += "NETWORK_FAILURE";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::NETWORK_FAILURE;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::UMTS_REACTIVATION_REQ) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::UMTS_REACTIVATION_REQ)) {
        os += (first ? "" : " | ");
        os += "UMTS_REACTIVATION_REQ";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::UMTS_REACTIVATION_REQ;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::FEATURE_NOT_SUPP) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::FEATURE_NOT_SUPP)) {
        os += (first ? "" : " | ");
        os += "FEATURE_NOT_SUPP";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::FEATURE_NOT_SUPP;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::TFT_SEMANTIC_ERROR) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::TFT_SEMANTIC_ERROR)) {
        os += (first ? "" : " | ");
        os += "TFT_SEMANTIC_ERROR";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::TFT_SEMANTIC_ERROR;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::TFT_SYTAX_ERROR) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::TFT_SYTAX_ERROR)) {
        os += (first ? "" : " | ");
        os += "TFT_SYTAX_ERROR";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::TFT_SYTAX_ERROR;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::UNKNOWN_PDP_CONTEXT) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::UNKNOWN_PDP_CONTEXT)) {
        os += (first ? "" : " | ");
        os += "UNKNOWN_PDP_CONTEXT";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::UNKNOWN_PDP_CONTEXT;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::FILTER_SEMANTIC_ERROR) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::FILTER_SEMANTIC_ERROR)) {
        os += (first ? "" : " | ");
        os += "FILTER_SEMANTIC_ERROR";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::FILTER_SEMANTIC_ERROR;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::FILTER_SYTAX_ERROR) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::FILTER_SYTAX_ERROR)) {
        os += (first ? "" : " | ");
        os += "FILTER_SYTAX_ERROR";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::FILTER_SYTAX_ERROR;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::PDP_WITHOUT_ACTIVE_TFT) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::PDP_WITHOUT_ACTIVE_TFT)) {
        os += (first ? "" : " | ");
        os += "PDP_WITHOUT_ACTIVE_TFT";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::PDP_WITHOUT_ACTIVE_TFT;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::ONLY_IPV4_ALLOWED) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::ONLY_IPV4_ALLOWED)) {
        os += (first ? "" : " | ");
        os += "ONLY_IPV4_ALLOWED";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::ONLY_IPV4_ALLOWED;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::ONLY_IPV6_ALLOWED) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::ONLY_IPV6_ALLOWED)) {
        os += (first ? "" : " | ");
        os += "ONLY_IPV6_ALLOWED";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::ONLY_IPV6_ALLOWED;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::ONLY_SINGLE_BEARER_ALLOWED) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::ONLY_SINGLE_BEARER_ALLOWED)) {
        os += (first ? "" : " | ");
        os += "ONLY_SINGLE_BEARER_ALLOWED";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::ONLY_SINGLE_BEARER_ALLOWED;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::ESM_INFO_NOT_RECEIVED) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::ESM_INFO_NOT_RECEIVED)) {
        os += (first ? "" : " | ");
        os += "ESM_INFO_NOT_RECEIVED";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::ESM_INFO_NOT_RECEIVED;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::PDN_CONN_DOES_NOT_EXIST) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::PDN_CONN_DOES_NOT_EXIST)) {
        os += (first ? "" : " | ");
        os += "PDN_CONN_DOES_NOT_EXIST";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::PDN_CONN_DOES_NOT_EXIST;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::MULTI_CONN_TO_SAME_PDN_NOT_ALLOWED) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::MULTI_CONN_TO_SAME_PDN_NOT_ALLOWED)) {
        os += (first ? "" : " | ");
        os += "MULTI_CONN_TO_SAME_PDN_NOT_ALLOWED";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::MULTI_CONN_TO_SAME_PDN_NOT_ALLOWED;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::MAX_ACTIVE_PDP_CONTEXT_REACHED) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::MAX_ACTIVE_PDP_CONTEXT_REACHED)) {
        os += (first ? "" : " | ");
        os += "MAX_ACTIVE_PDP_CONTEXT_REACHED";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::MAX_ACTIVE_PDP_CONTEXT_REACHED;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::UNSUPPORTED_APN_IN_CURRENT_PLMN) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::UNSUPPORTED_APN_IN_CURRENT_PLMN)) {
        os += (first ? "" : " | ");
        os += "UNSUPPORTED_APN_IN_CURRENT_PLMN";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::UNSUPPORTED_APN_IN_CURRENT_PLMN;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::INVALID_TRANSACTION_ID) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::INVALID_TRANSACTION_ID)) {
        os += (first ? "" : " | ");
        os += "INVALID_TRANSACTION_ID";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::INVALID_TRANSACTION_ID;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::MESSAGE_INCORRECT_SEMANTIC) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::MESSAGE_INCORRECT_SEMANTIC)) {
        os += (first ? "" : " | ");
        os += "MESSAGE_INCORRECT_SEMANTIC";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::MESSAGE_INCORRECT_SEMANTIC;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::INVALID_MANDATORY_INFO) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::INVALID_MANDATORY_INFO)) {
        os += (first ? "" : " | ");
        os += "INVALID_MANDATORY_INFO";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::INVALID_MANDATORY_INFO;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::MESSAGE_TYPE_UNSUPPORTED) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::MESSAGE_TYPE_UNSUPPORTED)) {
        os += (first ? "" : " | ");
        os += "MESSAGE_TYPE_UNSUPPORTED";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::MESSAGE_TYPE_UNSUPPORTED;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::MSG_TYPE_NONCOMPATIBLE_STATE) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::MSG_TYPE_NONCOMPATIBLE_STATE)) {
        os += (first ? "" : " | ");
        os += "MSG_TYPE_NONCOMPATIBLE_STATE";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::MSG_TYPE_NONCOMPATIBLE_STATE;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::UNKNOWN_INFO_ELEMENT) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::UNKNOWN_INFO_ELEMENT)) {
        os += (first ? "" : " | ");
        os += "UNKNOWN_INFO_ELEMENT";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::UNKNOWN_INFO_ELEMENT;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::CONDITIONAL_IE_ERROR) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::CONDITIONAL_IE_ERROR)) {
        os += (first ? "" : " | ");
        os += "CONDITIONAL_IE_ERROR";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::CONDITIONAL_IE_ERROR;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::MSG_AND_PROTOCOL_STATE_UNCOMPATIBLE) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::MSG_AND_PROTOCOL_STATE_UNCOMPATIBLE)) {
        os += (first ? "" : " | ");
        os += "MSG_AND_PROTOCOL_STATE_UNCOMPATIBLE";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::MSG_AND_PROTOCOL_STATE_UNCOMPATIBLE;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::PROTOCOL_ERRORS) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::PROTOCOL_ERRORS)) {
        os += (first ? "" : " | ");
        os += "PROTOCOL_ERRORS";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::PROTOCOL_ERRORS;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::APN_TYPE_CONFLICT) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::APN_TYPE_CONFLICT)) {
        os += (first ? "" : " | ");
        os += "APN_TYPE_CONFLICT";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::APN_TYPE_CONFLICT;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::INVALID_PCSCF_ADDR) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::INVALID_PCSCF_ADDR)) {
        os += (first ? "" : " | ");
        os += "INVALID_PCSCF_ADDR";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::INVALID_PCSCF_ADDR;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::INTERNAL_CALL_PREEMPT_BY_HIGH_PRIO_APN) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::INTERNAL_CALL_PREEMPT_BY_HIGH_PRIO_APN)) {
        os += (first ? "" : " | ");
        os += "INTERNAL_CALL_PREEMPT_BY_HIGH_PRIO_APN";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::INTERNAL_CALL_PREEMPT_BY_HIGH_PRIO_APN;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::EMM_ACCESS_BARRED) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::EMM_ACCESS_BARRED)) {
        os += (first ? "" : " | ");
        os += "EMM_ACCESS_BARRED";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::EMM_ACCESS_BARRED;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::EMERGENCY_IFACE_ONLY) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::EMERGENCY_IFACE_ONLY)) {
        os += (first ? "" : " | ");
        os += "EMERGENCY_IFACE_ONLY";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::EMERGENCY_IFACE_ONLY;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::IFACE_MISMATCH) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::IFACE_MISMATCH)) {
        os += (first ? "" : " | ");
        os += "IFACE_MISMATCH";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::IFACE_MISMATCH;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::COMPANION_IFACE_IN_USE) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::COMPANION_IFACE_IN_USE)) {
        os += (first ? "" : " | ");
        os += "COMPANION_IFACE_IN_USE";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::COMPANION_IFACE_IN_USE;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::IP_ADDRESS_MISMATCH) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::IP_ADDRESS_MISMATCH)) {
        os += (first ? "" : " | ");
        os += "IP_ADDRESS_MISMATCH";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::IP_ADDRESS_MISMATCH;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::IFACE_AND_POL_FAMILY_MISMATCH) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::IFACE_AND_POL_FAMILY_MISMATCH)) {
        os += (first ? "" : " | ");
        os += "IFACE_AND_POL_FAMILY_MISMATCH";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::IFACE_AND_POL_FAMILY_MISMATCH;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::EMM_ACCESS_BARRED_INFINITE_RETRY) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::EMM_ACCESS_BARRED_INFINITE_RETRY)) {
        os += (first ? "" : " | ");
        os += "EMM_ACCESS_BARRED_INFINITE_RETRY";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::EMM_ACCESS_BARRED_INFINITE_RETRY;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::AUTH_FAILURE_ON_EMERGENCY_CALL) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::AUTH_FAILURE_ON_EMERGENCY_CALL)) {
        os += (first ? "" : " | ");
        os += "AUTH_FAILURE_ON_EMERGENCY_CALL";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::AUTH_FAILURE_ON_EMERGENCY_CALL;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::OEM_DCFAILCAUSE_1) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::OEM_DCFAILCAUSE_1)) {
        os += (first ? "" : " | ");
        os += "OEM_DCFAILCAUSE_1";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::OEM_DCFAILCAUSE_1;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::OEM_DCFAILCAUSE_2) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::OEM_DCFAILCAUSE_2)) {
        os += (first ? "" : " | ");
        os += "OEM_DCFAILCAUSE_2";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::OEM_DCFAILCAUSE_2;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::OEM_DCFAILCAUSE_3) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::OEM_DCFAILCAUSE_3)) {
        os += (first ? "" : " | ");
        os += "OEM_DCFAILCAUSE_3";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::OEM_DCFAILCAUSE_3;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::OEM_DCFAILCAUSE_4) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::OEM_DCFAILCAUSE_4)) {
        os += (first ? "" : " | ");
        os += "OEM_DCFAILCAUSE_4";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::OEM_DCFAILCAUSE_4;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::OEM_DCFAILCAUSE_5) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::OEM_DCFAILCAUSE_5)) {
        os += (first ? "" : " | ");
        os += "OEM_DCFAILCAUSE_5";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::OEM_DCFAILCAUSE_5;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::OEM_DCFAILCAUSE_6) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::OEM_DCFAILCAUSE_6)) {
        os += (first ? "" : " | ");
        os += "OEM_DCFAILCAUSE_6";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::OEM_DCFAILCAUSE_6;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::OEM_DCFAILCAUSE_7) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::OEM_DCFAILCAUSE_7)) {
        os += (first ? "" : " | ");
        os += "OEM_DCFAILCAUSE_7";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::OEM_DCFAILCAUSE_7;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::OEM_DCFAILCAUSE_8) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::OEM_DCFAILCAUSE_8)) {
        os += (first ? "" : " | ");
        os += "OEM_DCFAILCAUSE_8";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::OEM_DCFAILCAUSE_8;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::OEM_DCFAILCAUSE_9) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::OEM_DCFAILCAUSE_9)) {
        os += (first ? "" : " | ");
        os += "OEM_DCFAILCAUSE_9";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::OEM_DCFAILCAUSE_9;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::OEM_DCFAILCAUSE_10) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::OEM_DCFAILCAUSE_10)) {
        os += (first ? "" : " | ");
        os += "OEM_DCFAILCAUSE_10";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::OEM_DCFAILCAUSE_10;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::OEM_DCFAILCAUSE_11) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::OEM_DCFAILCAUSE_11)) {
        os += (first ? "" : " | ");
        os += "OEM_DCFAILCAUSE_11";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::OEM_DCFAILCAUSE_11;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::OEM_DCFAILCAUSE_12) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::OEM_DCFAILCAUSE_12)) {
        os += (first ? "" : " | ");
        os += "OEM_DCFAILCAUSE_12";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::OEM_DCFAILCAUSE_12;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::OEM_DCFAILCAUSE_13) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::OEM_DCFAILCAUSE_13)) {
        os += (first ? "" : " | ");
        os += "OEM_DCFAILCAUSE_13";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::OEM_DCFAILCAUSE_13;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::OEM_DCFAILCAUSE_14) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::OEM_DCFAILCAUSE_14)) {
        os += (first ? "" : " | ");
        os += "OEM_DCFAILCAUSE_14";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::OEM_DCFAILCAUSE_14;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::OEM_DCFAILCAUSE_15) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::OEM_DCFAILCAUSE_15)) {
        os += (first ? "" : " | ");
        os += "OEM_DCFAILCAUSE_15";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::OEM_DCFAILCAUSE_15;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::VOICE_REGISTRATION_FAIL) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::VOICE_REGISTRATION_FAIL)) {
        os += (first ? "" : " | ");
        os += "VOICE_REGISTRATION_FAIL";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::VOICE_REGISTRATION_FAIL;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::DATA_REGISTRATION_FAIL) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::DATA_REGISTRATION_FAIL)) {
        os += (first ? "" : " | ");
        os += "DATA_REGISTRATION_FAIL";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::DATA_REGISTRATION_FAIL;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::SIGNAL_LOST) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::SIGNAL_LOST)) {
        os += (first ? "" : " | ");
        os += "SIGNAL_LOST";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::SIGNAL_LOST;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::PREF_RADIO_TECH_CHANGED) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::PREF_RADIO_TECH_CHANGED)) {
        os += (first ? "" : " | ");
        os += "PREF_RADIO_TECH_CHANGED";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::PREF_RADIO_TECH_CHANGED;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::RADIO_POWER_OFF) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::RADIO_POWER_OFF)) {
        os += (first ? "" : " | ");
        os += "RADIO_POWER_OFF";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::RADIO_POWER_OFF;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::TETHERED_CALL_ACTIVE) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::TETHERED_CALL_ACTIVE)) {
        os += (first ? "" : " | ");
        os += "TETHERED_CALL_ACTIVE";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::TETHERED_CALL_ACTIVE;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::ERROR_UNSPECIFIED) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::ERROR_UNSPECIFIED)) {
        os += (first ? "" : " | ");
        os += "ERROR_UNSPECIFIED";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::ERROR_UNSPECIFIED;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::LLC_SNDCP) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::LLC_SNDCP)) {
        os += (first ? "" : " | ");
        os += "LLC_SNDCP";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::LLC_SNDCP;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::ACTIVATION_REJECTED_BCM_VIOLATION) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::ACTIVATION_REJECTED_BCM_VIOLATION)) {
        os += (first ? "" : " | ");
        os += "ACTIVATION_REJECTED_BCM_VIOLATION";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::ACTIVATION_REJECTED_BCM_VIOLATION;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::COLLISION_WITH_NETWORK_INITIATED_REQUEST) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::COLLISION_WITH_NETWORK_INITIATED_REQUEST)) {
        os += (first ? "" : " | ");
        os += "COLLISION_WITH_NETWORK_INITIATED_REQUEST";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::COLLISION_WITH_NETWORK_INITIATED_REQUEST;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::ONLY_IPV4V6_ALLOWED) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::ONLY_IPV4V6_ALLOWED)) {
        os += (first ? "" : " | ");
        os += "ONLY_IPV4V6_ALLOWED";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::ONLY_IPV4V6_ALLOWED;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::ONLY_NON_IP_ALLOWED) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::ONLY_NON_IP_ALLOWED)) {
        os += (first ? "" : " | ");
        os += "ONLY_NON_IP_ALLOWED";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::ONLY_NON_IP_ALLOWED;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::UNSUPPORTED_QCI_VALUE) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::UNSUPPORTED_QCI_VALUE)) {
        os += (first ? "" : " | ");
        os += "UNSUPPORTED_QCI_VALUE";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::UNSUPPORTED_QCI_VALUE;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::BEARER_HANDLING_NOT_SUPPORTED) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::BEARER_HANDLING_NOT_SUPPORTED)) {
        os += (first ? "" : " | ");
        os += "BEARER_HANDLING_NOT_SUPPORTED";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::BEARER_HANDLING_NOT_SUPPORTED;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::INVALID_DNS_ADDR) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::INVALID_DNS_ADDR)) {
        os += (first ? "" : " | ");
        os += "INVALID_DNS_ADDR";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::INVALID_DNS_ADDR;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::INVALID_PCSCF_OR_DNS_ADDRESS) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::INVALID_PCSCF_OR_DNS_ADDRESS)) {
        os += (first ? "" : " | ");
        os += "INVALID_PCSCF_OR_DNS_ADDRESS";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::INVALID_PCSCF_OR_DNS_ADDRESS;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::CALL_PREEMPT_BY_EMERGENCY_APN) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::CALL_PREEMPT_BY_EMERGENCY_APN)) {
        os += (first ? "" : " | ");
        os += "CALL_PREEMPT_BY_EMERGENCY_APN";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::CALL_PREEMPT_BY_EMERGENCY_APN;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::UE_INITIATED_DETACH_OR_DISCONNECT) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::UE_INITIATED_DETACH_OR_DISCONNECT)) {
        os += (first ? "" : " | ");
        os += "UE_INITIATED_DETACH_OR_DISCONNECT";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::UE_INITIATED_DETACH_OR_DISCONNECT;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::MIP_FA_REASON_UNSPECIFIED) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::MIP_FA_REASON_UNSPECIFIED)) {
        os += (first ? "" : " | ");
        os += "MIP_FA_REASON_UNSPECIFIED";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::MIP_FA_REASON_UNSPECIFIED;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::MIP_FA_ADMIN_PROHIBITED) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::MIP_FA_ADMIN_PROHIBITED)) {
        os += (first ? "" : " | ");
        os += "MIP_FA_ADMIN_PROHIBITED";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::MIP_FA_ADMIN_PROHIBITED;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::MIP_FA_INSUFFICIENT_RESOURCES) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::MIP_FA_INSUFFICIENT_RESOURCES)) {
        os += (first ? "" : " | ");
        os += "MIP_FA_INSUFFICIENT_RESOURCES";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::MIP_FA_INSUFFICIENT_RESOURCES;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::MIP_FA_MOBILE_NODE_AUTHENTICATION_FAILURE) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::MIP_FA_MOBILE_NODE_AUTHENTICATION_FAILURE)) {
        os += (first ? "" : " | ");
        os += "MIP_FA_MOBILE_NODE_AUTHENTICATION_FAILURE";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::MIP_FA_MOBILE_NODE_AUTHENTICATION_FAILURE;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::MIP_FA_HOME_AGENT_AUTHENTICATION_FAILURE) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::MIP_FA_HOME_AGENT_AUTHENTICATION_FAILURE)) {
        os += (first ? "" : " | ");
        os += "MIP_FA_HOME_AGENT_AUTHENTICATION_FAILURE";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::MIP_FA_HOME_AGENT_AUTHENTICATION_FAILURE;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::MIP_FA_REQUESTED_LIFETIME_TOO_LONG) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::MIP_FA_REQUESTED_LIFETIME_TOO_LONG)) {
        os += (first ? "" : " | ");
        os += "MIP_FA_REQUESTED_LIFETIME_TOO_LONG";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::MIP_FA_REQUESTED_LIFETIME_TOO_LONG;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::MIP_FA_MALFORMED_REQUEST) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::MIP_FA_MALFORMED_REQUEST)) {
        os += (first ? "" : " | ");
        os += "MIP_FA_MALFORMED_REQUEST";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::MIP_FA_MALFORMED_REQUEST;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::MIP_FA_MALFORMED_REPLY) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::MIP_FA_MALFORMED_REPLY)) {
        os += (first ? "" : " | ");
        os += "MIP_FA_MALFORMED_REPLY";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::MIP_FA_MALFORMED_REPLY;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::MIP_FA_ENCAPSULATION_UNAVAILABLE) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::MIP_FA_ENCAPSULATION_UNAVAILABLE)) {
        os += (first ? "" : " | ");
        os += "MIP_FA_ENCAPSULATION_UNAVAILABLE";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::MIP_FA_ENCAPSULATION_UNAVAILABLE;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::MIP_FA_VJ_HEADER_COMPRESSION_UNAVAILABLE) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::MIP_FA_VJ_HEADER_COMPRESSION_UNAVAILABLE)) {
        os += (first ? "" : " | ");
        os += "MIP_FA_VJ_HEADER_COMPRESSION_UNAVAILABLE";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::MIP_FA_VJ_HEADER_COMPRESSION_UNAVAILABLE;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::MIP_FA_REVERSE_TUNNEL_UNAVAILABLE) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::MIP_FA_REVERSE_TUNNEL_UNAVAILABLE)) {
        os += (first ? "" : " | ");
        os += "MIP_FA_REVERSE_TUNNEL_UNAVAILABLE";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::MIP_FA_REVERSE_TUNNEL_UNAVAILABLE;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::MIP_FA_REVERSE_TUNNEL_IS_MANDATORY) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::MIP_FA_REVERSE_TUNNEL_IS_MANDATORY)) {
        os += (first ? "" : " | ");
        os += "MIP_FA_REVERSE_TUNNEL_IS_MANDATORY";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::MIP_FA_REVERSE_TUNNEL_IS_MANDATORY;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::MIP_FA_DELIVERY_STYLE_NOT_SUPPORTED) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::MIP_FA_DELIVERY_STYLE_NOT_SUPPORTED)) {
        os += (first ? "" : " | ");
        os += "MIP_FA_DELIVERY_STYLE_NOT_SUPPORTED";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::MIP_FA_DELIVERY_STYLE_NOT_SUPPORTED;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::MIP_FA_MISSING_NAI) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::MIP_FA_MISSING_NAI)) {
        os += (first ? "" : " | ");
        os += "MIP_FA_MISSING_NAI";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::MIP_FA_MISSING_NAI;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::MIP_FA_MISSING_HOME_AGENT) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::MIP_FA_MISSING_HOME_AGENT)) {
        os += (first ? "" : " | ");
        os += "MIP_FA_MISSING_HOME_AGENT";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::MIP_FA_MISSING_HOME_AGENT;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::MIP_FA_MISSING_HOME_ADDRESS) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::MIP_FA_MISSING_HOME_ADDRESS)) {
        os += (first ? "" : " | ");
        os += "MIP_FA_MISSING_HOME_ADDRESS";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::MIP_FA_MISSING_HOME_ADDRESS;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::MIP_FA_UNKNOWN_CHALLENGE) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::MIP_FA_UNKNOWN_CHALLENGE)) {
        os += (first ? "" : " | ");
        os += "MIP_FA_UNKNOWN_CHALLENGE";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::MIP_FA_UNKNOWN_CHALLENGE;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::MIP_FA_MISSING_CHALLENGE) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::MIP_FA_MISSING_CHALLENGE)) {
        os += (first ? "" : " | ");
        os += "MIP_FA_MISSING_CHALLENGE";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::MIP_FA_MISSING_CHALLENGE;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::MIP_FA_STALE_CHALLENGE) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::MIP_FA_STALE_CHALLENGE)) {
        os += (first ? "" : " | ");
        os += "MIP_FA_STALE_CHALLENGE";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::MIP_FA_STALE_CHALLENGE;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::MIP_HA_REASON_UNSPECIFIED) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::MIP_HA_REASON_UNSPECIFIED)) {
        os += (first ? "" : " | ");
        os += "MIP_HA_REASON_UNSPECIFIED";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::MIP_HA_REASON_UNSPECIFIED;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::MIP_HA_ADMIN_PROHIBITED) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::MIP_HA_ADMIN_PROHIBITED)) {
        os += (first ? "" : " | ");
        os += "MIP_HA_ADMIN_PROHIBITED";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::MIP_HA_ADMIN_PROHIBITED;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::MIP_HA_INSUFFICIENT_RESOURCES) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::MIP_HA_INSUFFICIENT_RESOURCES)) {
        os += (first ? "" : " | ");
        os += "MIP_HA_INSUFFICIENT_RESOURCES";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::MIP_HA_INSUFFICIENT_RESOURCES;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::MIP_HA_MOBILE_NODE_AUTHENTICATION_FAILURE) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::MIP_HA_MOBILE_NODE_AUTHENTICATION_FAILURE)) {
        os += (first ? "" : " | ");
        os += "MIP_HA_MOBILE_NODE_AUTHENTICATION_FAILURE";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::MIP_HA_MOBILE_NODE_AUTHENTICATION_FAILURE;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::MIP_HA_FOREIGN_AGENT_AUTHENTICATION_FAILURE) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::MIP_HA_FOREIGN_AGENT_AUTHENTICATION_FAILURE)) {
        os += (first ? "" : " | ");
        os += "MIP_HA_FOREIGN_AGENT_AUTHENTICATION_FAILURE";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::MIP_HA_FOREIGN_AGENT_AUTHENTICATION_FAILURE;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::MIP_HA_REGISTRATION_ID_MISMATCH) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::MIP_HA_REGISTRATION_ID_MISMATCH)) {
        os += (first ? "" : " | ");
        os += "MIP_HA_REGISTRATION_ID_MISMATCH";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::MIP_HA_REGISTRATION_ID_MISMATCH;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::MIP_HA_MALFORMED_REQUEST) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::MIP_HA_MALFORMED_REQUEST)) {
        os += (first ? "" : " | ");
        os += "MIP_HA_MALFORMED_REQUEST";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::MIP_HA_MALFORMED_REQUEST;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::MIP_HA_UNKNOWN_HOME_AGENT_ADDRESS) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::MIP_HA_UNKNOWN_HOME_AGENT_ADDRESS)) {
        os += (first ? "" : " | ");
        os += "MIP_HA_UNKNOWN_HOME_AGENT_ADDRESS";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::MIP_HA_UNKNOWN_HOME_AGENT_ADDRESS;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::MIP_HA_REVERSE_TUNNEL_UNAVAILABLE) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::MIP_HA_REVERSE_TUNNEL_UNAVAILABLE)) {
        os += (first ? "" : " | ");
        os += "MIP_HA_REVERSE_TUNNEL_UNAVAILABLE";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::MIP_HA_REVERSE_TUNNEL_UNAVAILABLE;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::MIP_HA_REVERSE_TUNNEL_IS_MANDATORY) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::MIP_HA_REVERSE_TUNNEL_IS_MANDATORY)) {
        os += (first ? "" : " | ");
        os += "MIP_HA_REVERSE_TUNNEL_IS_MANDATORY";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::MIP_HA_REVERSE_TUNNEL_IS_MANDATORY;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::MIP_HA_ENCAPSULATION_UNAVAILABLE) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::MIP_HA_ENCAPSULATION_UNAVAILABLE)) {
        os += (first ? "" : " | ");
        os += "MIP_HA_ENCAPSULATION_UNAVAILABLE";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::MIP_HA_ENCAPSULATION_UNAVAILABLE;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::CLOSE_IN_PROGRESS) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::CLOSE_IN_PROGRESS)) {
        os += (first ? "" : " | ");
        os += "CLOSE_IN_PROGRESS";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::CLOSE_IN_PROGRESS;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::NETWORK_INITIATED_TERMINATION) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::NETWORK_INITIATED_TERMINATION)) {
        os += (first ? "" : " | ");
        os += "NETWORK_INITIATED_TERMINATION";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::NETWORK_INITIATED_TERMINATION;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::MODEM_APP_PREEMPTED) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::MODEM_APP_PREEMPTED)) {
        os += (first ? "" : " | ");
        os += "MODEM_APP_PREEMPTED";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::MODEM_APP_PREEMPTED;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::PDN_IPV4_CALL_DISALLOWED) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::PDN_IPV4_CALL_DISALLOWED)) {
        os += (first ? "" : " | ");
        os += "PDN_IPV4_CALL_DISALLOWED";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::PDN_IPV4_CALL_DISALLOWED;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::PDN_IPV4_CALL_THROTTLED) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::PDN_IPV4_CALL_THROTTLED)) {
        os += (first ? "" : " | ");
        os += "PDN_IPV4_CALL_THROTTLED";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::PDN_IPV4_CALL_THROTTLED;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::PDN_IPV6_CALL_DISALLOWED) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::PDN_IPV6_CALL_DISALLOWED)) {
        os += (first ? "" : " | ");
        os += "PDN_IPV6_CALL_DISALLOWED";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::PDN_IPV6_CALL_DISALLOWED;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::PDN_IPV6_CALL_THROTTLED) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::PDN_IPV6_CALL_THROTTLED)) {
        os += (first ? "" : " | ");
        os += "PDN_IPV6_CALL_THROTTLED";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::PDN_IPV6_CALL_THROTTLED;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::MODEM_RESTART) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::MODEM_RESTART)) {
        os += (first ? "" : " | ");
        os += "MODEM_RESTART";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::MODEM_RESTART;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::PDP_PPP_NOT_SUPPORTED) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::PDP_PPP_NOT_SUPPORTED)) {
        os += (first ? "" : " | ");
        os += "PDP_PPP_NOT_SUPPORTED";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::PDP_PPP_NOT_SUPPORTED;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::UNPREFERRED_RAT) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::UNPREFERRED_RAT)) {
        os += (first ? "" : " | ");
        os += "UNPREFERRED_RAT";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::UNPREFERRED_RAT;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::PHYSICAL_LINK_CLOSE_IN_PROGRESS) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::PHYSICAL_LINK_CLOSE_IN_PROGRESS)) {
        os += (first ? "" : " | ");
        os += "PHYSICAL_LINK_CLOSE_IN_PROGRESS";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::PHYSICAL_LINK_CLOSE_IN_PROGRESS;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::APN_PENDING_HANDOVER) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::APN_PENDING_HANDOVER)) {
        os += (first ? "" : " | ");
        os += "APN_PENDING_HANDOVER";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::APN_PENDING_HANDOVER;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::PROFILE_BEARER_INCOMPATIBLE) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::PROFILE_BEARER_INCOMPATIBLE)) {
        os += (first ? "" : " | ");
        os += "PROFILE_BEARER_INCOMPATIBLE";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::PROFILE_BEARER_INCOMPATIBLE;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::SIM_CARD_CHANGED) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::SIM_CARD_CHANGED)) {
        os += (first ? "" : " | ");
        os += "SIM_CARD_CHANGED";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::SIM_CARD_CHANGED;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::LOW_POWER_MODE_OR_POWERING_DOWN) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::LOW_POWER_MODE_OR_POWERING_DOWN)) {
        os += (first ? "" : " | ");
        os += "LOW_POWER_MODE_OR_POWERING_DOWN";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::LOW_POWER_MODE_OR_POWERING_DOWN;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::APN_DISABLED) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::APN_DISABLED)) {
        os += (first ? "" : " | ");
        os += "APN_DISABLED";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::APN_DISABLED;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::MAX_PPP_INACTIVITY_TIMER_EXPIRED) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::MAX_PPP_INACTIVITY_TIMER_EXPIRED)) {
        os += (first ? "" : " | ");
        os += "MAX_PPP_INACTIVITY_TIMER_EXPIRED";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::MAX_PPP_INACTIVITY_TIMER_EXPIRED;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::IPV6_ADDRESS_TRANSFER_FAILED) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::IPV6_ADDRESS_TRANSFER_FAILED)) {
        os += (first ? "" : " | ");
        os += "IPV6_ADDRESS_TRANSFER_FAILED";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::IPV6_ADDRESS_TRANSFER_FAILED;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::TRAT_SWAP_FAILED) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::TRAT_SWAP_FAILED)) {
        os += (first ? "" : " | ");
        os += "TRAT_SWAP_FAILED";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::TRAT_SWAP_FAILED;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::EHRPD_TO_HRPD_FALLBACK) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::EHRPD_TO_HRPD_FALLBACK)) {
        os += (first ? "" : " | ");
        os += "EHRPD_TO_HRPD_FALLBACK";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::EHRPD_TO_HRPD_FALLBACK;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::MIP_CONFIG_FAILURE) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::MIP_CONFIG_FAILURE)) {
        os += (first ? "" : " | ");
        os += "MIP_CONFIG_FAILURE";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::MIP_CONFIG_FAILURE;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::PDN_INACTIVITY_TIMER_EXPIRED) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::PDN_INACTIVITY_TIMER_EXPIRED)) {
        os += (first ? "" : " | ");
        os += "PDN_INACTIVITY_TIMER_EXPIRED";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::PDN_INACTIVITY_TIMER_EXPIRED;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::MAX_IPV4_CONNECTIONS) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::MAX_IPV4_CONNECTIONS)) {
        os += (first ? "" : " | ");
        os += "MAX_IPV4_CONNECTIONS";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::MAX_IPV4_CONNECTIONS;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::MAX_IPV6_CONNECTIONS) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::MAX_IPV6_CONNECTIONS)) {
        os += (first ? "" : " | ");
        os += "MAX_IPV6_CONNECTIONS";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::MAX_IPV6_CONNECTIONS;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::APN_MISMATCH) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::APN_MISMATCH)) {
        os += (first ? "" : " | ");
        os += "APN_MISMATCH";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::APN_MISMATCH;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::IP_VERSION_MISMATCH) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::IP_VERSION_MISMATCH)) {
        os += (first ? "" : " | ");
        os += "IP_VERSION_MISMATCH";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::IP_VERSION_MISMATCH;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::DUN_CALL_DISALLOWED) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::DUN_CALL_DISALLOWED)) {
        os += (first ? "" : " | ");
        os += "DUN_CALL_DISALLOWED";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::DUN_CALL_DISALLOWED;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::INTERNAL_EPC_NONEPC_TRANSITION) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::INTERNAL_EPC_NONEPC_TRANSITION)) {
        os += (first ? "" : " | ");
        os += "INTERNAL_EPC_NONEPC_TRANSITION";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::INTERNAL_EPC_NONEPC_TRANSITION;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::INTERFACE_IN_USE) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::INTERFACE_IN_USE)) {
        os += (first ? "" : " | ");
        os += "INTERFACE_IN_USE";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::INTERFACE_IN_USE;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::APN_DISALLOWED_ON_ROAMING) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::APN_DISALLOWED_ON_ROAMING)) {
        os += (first ? "" : " | ");
        os += "APN_DISALLOWED_ON_ROAMING";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::APN_DISALLOWED_ON_ROAMING;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::APN_PARAMETERS_CHANGED) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::APN_PARAMETERS_CHANGED)) {
        os += (first ? "" : " | ");
        os += "APN_PARAMETERS_CHANGED";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::APN_PARAMETERS_CHANGED;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::NULL_APN_DISALLOWED) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::NULL_APN_DISALLOWED)) {
        os += (first ? "" : " | ");
        os += "NULL_APN_DISALLOWED";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::NULL_APN_DISALLOWED;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::THERMAL_MITIGATION) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::THERMAL_MITIGATION)) {
        os += (first ? "" : " | ");
        os += "THERMAL_MITIGATION";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::THERMAL_MITIGATION;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::DATA_SETTINGS_DISABLED) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::DATA_SETTINGS_DISABLED)) {
        os += (first ? "" : " | ");
        os += "DATA_SETTINGS_DISABLED";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::DATA_SETTINGS_DISABLED;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::DATA_ROAMING_SETTINGS_DISABLED) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::DATA_ROAMING_SETTINGS_DISABLED)) {
        os += (first ? "" : " | ");
        os += "DATA_ROAMING_SETTINGS_DISABLED";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::DATA_ROAMING_SETTINGS_DISABLED;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::DDS_SWITCHED) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::DDS_SWITCHED)) {
        os += (first ? "" : " | ");
        os += "DDS_SWITCHED";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::DDS_SWITCHED;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::FORBIDDEN_APN_NAME) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::FORBIDDEN_APN_NAME)) {
        os += (first ? "" : " | ");
        os += "FORBIDDEN_APN_NAME";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::FORBIDDEN_APN_NAME;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::DDS_SWITCH_IN_PROGRESS) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::DDS_SWITCH_IN_PROGRESS)) {
        os += (first ? "" : " | ");
        os += "DDS_SWITCH_IN_PROGRESS";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::DDS_SWITCH_IN_PROGRESS;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::CALL_DISALLOWED_IN_ROAMING) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::CALL_DISALLOWED_IN_ROAMING)) {
        os += (first ? "" : " | ");
        os += "CALL_DISALLOWED_IN_ROAMING";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::CALL_DISALLOWED_IN_ROAMING;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::NON_IP_NOT_SUPPORTED) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::NON_IP_NOT_SUPPORTED)) {
        os += (first ? "" : " | ");
        os += "NON_IP_NOT_SUPPORTED";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::NON_IP_NOT_SUPPORTED;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::PDN_NON_IP_CALL_THROTTLED) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::PDN_NON_IP_CALL_THROTTLED)) {
        os += (first ? "" : " | ");
        os += "PDN_NON_IP_CALL_THROTTLED";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::PDN_NON_IP_CALL_THROTTLED;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::PDN_NON_IP_CALL_DISALLOWED) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::PDN_NON_IP_CALL_DISALLOWED)) {
        os += (first ? "" : " | ");
        os += "PDN_NON_IP_CALL_DISALLOWED";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::PDN_NON_IP_CALL_DISALLOWED;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::CDMA_LOCK) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::CDMA_LOCK)) {
        os += (first ? "" : " | ");
        os += "CDMA_LOCK";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::CDMA_LOCK;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::CDMA_INTERCEPT) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::CDMA_INTERCEPT)) {
        os += (first ? "" : " | ");
        os += "CDMA_INTERCEPT";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::CDMA_INTERCEPT;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::CDMA_REORDER) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::CDMA_REORDER)) {
        os += (first ? "" : " | ");
        os += "CDMA_REORDER";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::CDMA_REORDER;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::CDMA_RELEASE_DUE_TO_SO_REJECTION) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::CDMA_RELEASE_DUE_TO_SO_REJECTION)) {
        os += (first ? "" : " | ");
        os += "CDMA_RELEASE_DUE_TO_SO_REJECTION";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::CDMA_RELEASE_DUE_TO_SO_REJECTION;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::CDMA_INCOMING_CALL) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::CDMA_INCOMING_CALL)) {
        os += (first ? "" : " | ");
        os += "CDMA_INCOMING_CALL";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::CDMA_INCOMING_CALL;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::CDMA_ALERT_STOP) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::CDMA_ALERT_STOP)) {
        os += (first ? "" : " | ");
        os += "CDMA_ALERT_STOP";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::CDMA_ALERT_STOP;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::CHANNEL_ACQUISITION_FAILURE) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::CHANNEL_ACQUISITION_FAILURE)) {
        os += (first ? "" : " | ");
        os += "CHANNEL_ACQUISITION_FAILURE";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::CHANNEL_ACQUISITION_FAILURE;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::MAX_ACCESS_PROBE) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::MAX_ACCESS_PROBE)) {
        os += (first ? "" : " | ");
        os += "MAX_ACCESS_PROBE";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::MAX_ACCESS_PROBE;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::CONCURRENT_SERVICE_NOT_SUPPORTED_BY_BASE_STATION) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::CONCURRENT_SERVICE_NOT_SUPPORTED_BY_BASE_STATION)) {
        os += (first ? "" : " | ");
        os += "CONCURRENT_SERVICE_NOT_SUPPORTED_BY_BASE_STATION";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::CONCURRENT_SERVICE_NOT_SUPPORTED_BY_BASE_STATION;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::NO_RESPONSE_FROM_BASE_STATION) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::NO_RESPONSE_FROM_BASE_STATION)) {
        os += (first ? "" : " | ");
        os += "NO_RESPONSE_FROM_BASE_STATION";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::NO_RESPONSE_FROM_BASE_STATION;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::REJECTED_BY_BASE_STATION) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::REJECTED_BY_BASE_STATION)) {
        os += (first ? "" : " | ");
        os += "REJECTED_BY_BASE_STATION";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::REJECTED_BY_BASE_STATION;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::CONCURRENT_SERVICES_INCOMPATIBLE) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::CONCURRENT_SERVICES_INCOMPATIBLE)) {
        os += (first ? "" : " | ");
        os += "CONCURRENT_SERVICES_INCOMPATIBLE";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::CONCURRENT_SERVICES_INCOMPATIBLE;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::NO_CDMA_SERVICE) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::NO_CDMA_SERVICE)) {
        os += (first ? "" : " | ");
        os += "NO_CDMA_SERVICE";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::NO_CDMA_SERVICE;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::RUIM_NOT_PRESENT) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::RUIM_NOT_PRESENT)) {
        os += (first ? "" : " | ");
        os += "RUIM_NOT_PRESENT";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::RUIM_NOT_PRESENT;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::CDMA_RETRY_ORDER) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::CDMA_RETRY_ORDER)) {
        os += (first ? "" : " | ");
        os += "CDMA_RETRY_ORDER";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::CDMA_RETRY_ORDER;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::ACCESS_BLOCK) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::ACCESS_BLOCK)) {
        os += (first ? "" : " | ");
        os += "ACCESS_BLOCK";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::ACCESS_BLOCK;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::ACCESS_BLOCK_ALL) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::ACCESS_BLOCK_ALL)) {
        os += (first ? "" : " | ");
        os += "ACCESS_BLOCK_ALL";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::ACCESS_BLOCK_ALL;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::IS707B_MAX_ACCESS_PROBES) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::IS707B_MAX_ACCESS_PROBES)) {
        os += (first ? "" : " | ");
        os += "IS707B_MAX_ACCESS_PROBES";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::IS707B_MAX_ACCESS_PROBES;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::THERMAL_EMERGENCY) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::THERMAL_EMERGENCY)) {
        os += (first ? "" : " | ");
        os += "THERMAL_EMERGENCY";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::THERMAL_EMERGENCY;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::CONCURRENT_SERVICES_NOT_ALLOWED) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::CONCURRENT_SERVICES_NOT_ALLOWED)) {
        os += (first ? "" : " | ");
        os += "CONCURRENT_SERVICES_NOT_ALLOWED";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::CONCURRENT_SERVICES_NOT_ALLOWED;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::INCOMING_CALL_REJECTED) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::INCOMING_CALL_REJECTED)) {
        os += (first ? "" : " | ");
        os += "INCOMING_CALL_REJECTED";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::INCOMING_CALL_REJECTED;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::NO_SERVICE_ON_GATEWAY) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::NO_SERVICE_ON_GATEWAY)) {
        os += (first ? "" : " | ");
        os += "NO_SERVICE_ON_GATEWAY";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::NO_SERVICE_ON_GATEWAY;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::NO_GPRS_CONTEXT) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::NO_GPRS_CONTEXT)) {
        os += (first ? "" : " | ");
        os += "NO_GPRS_CONTEXT";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::NO_GPRS_CONTEXT;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::ILLEGAL_MS) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::ILLEGAL_MS)) {
        os += (first ? "" : " | ");
        os += "ILLEGAL_MS";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::ILLEGAL_MS;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::ILLEGAL_ME) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::ILLEGAL_ME)) {
        os += (first ? "" : " | ");
        os += "ILLEGAL_ME";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::ILLEGAL_ME;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::GPRS_SERVICES_AND_NON_GPRS_SERVICES_NOT_ALLOWED) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::GPRS_SERVICES_AND_NON_GPRS_SERVICES_NOT_ALLOWED)) {
        os += (first ? "" : " | ");
        os += "GPRS_SERVICES_AND_NON_GPRS_SERVICES_NOT_ALLOWED";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::GPRS_SERVICES_AND_NON_GPRS_SERVICES_NOT_ALLOWED;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::GPRS_SERVICES_NOT_ALLOWED) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::GPRS_SERVICES_NOT_ALLOWED)) {
        os += (first ? "" : " | ");
        os += "GPRS_SERVICES_NOT_ALLOWED";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::GPRS_SERVICES_NOT_ALLOWED;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::MS_IDENTITY_CANNOT_BE_DERIVED_BY_THE_NETWORK) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::MS_IDENTITY_CANNOT_BE_DERIVED_BY_THE_NETWORK)) {
        os += (first ? "" : " | ");
        os += "MS_IDENTITY_CANNOT_BE_DERIVED_BY_THE_NETWORK";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::MS_IDENTITY_CANNOT_BE_DERIVED_BY_THE_NETWORK;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::IMPLICITLY_DETACHED) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::IMPLICITLY_DETACHED)) {
        os += (first ? "" : " | ");
        os += "IMPLICITLY_DETACHED";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::IMPLICITLY_DETACHED;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::PLMN_NOT_ALLOWED) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::PLMN_NOT_ALLOWED)) {
        os += (first ? "" : " | ");
        os += "PLMN_NOT_ALLOWED";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::PLMN_NOT_ALLOWED;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::LOCATION_AREA_NOT_ALLOWED) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::LOCATION_AREA_NOT_ALLOWED)) {
        os += (first ? "" : " | ");
        os += "LOCATION_AREA_NOT_ALLOWED";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::LOCATION_AREA_NOT_ALLOWED;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::GPRS_SERVICES_NOT_ALLOWED_IN_THIS_PLMN) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::GPRS_SERVICES_NOT_ALLOWED_IN_THIS_PLMN)) {
        os += (first ? "" : " | ");
        os += "GPRS_SERVICES_NOT_ALLOWED_IN_THIS_PLMN";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::GPRS_SERVICES_NOT_ALLOWED_IN_THIS_PLMN;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::PDP_DUPLICATE) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::PDP_DUPLICATE)) {
        os += (first ? "" : " | ");
        os += "PDP_DUPLICATE";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::PDP_DUPLICATE;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::UE_RAT_CHANGE) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::UE_RAT_CHANGE)) {
        os += (first ? "" : " | ");
        os += "UE_RAT_CHANGE";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::UE_RAT_CHANGE;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::CONGESTION) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::CONGESTION)) {
        os += (first ? "" : " | ");
        os += "CONGESTION";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::CONGESTION;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::NO_PDP_CONTEXT_ACTIVATED) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::NO_PDP_CONTEXT_ACTIVATED)) {
        os += (first ? "" : " | ");
        os += "NO_PDP_CONTEXT_ACTIVATED";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::NO_PDP_CONTEXT_ACTIVATED;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::ACCESS_CLASS_DSAC_REJECTION) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::ACCESS_CLASS_DSAC_REJECTION)) {
        os += (first ? "" : " | ");
        os += "ACCESS_CLASS_DSAC_REJECTION";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::ACCESS_CLASS_DSAC_REJECTION;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::PDP_ACTIVATE_MAX_RETRY_FAILED) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::PDP_ACTIVATE_MAX_RETRY_FAILED)) {
        os += (first ? "" : " | ");
        os += "PDP_ACTIVATE_MAX_RETRY_FAILED";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::PDP_ACTIVATE_MAX_RETRY_FAILED;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::RADIO_ACCESS_BEARER_FAILURE) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::RADIO_ACCESS_BEARER_FAILURE)) {
        os += (first ? "" : " | ");
        os += "RADIO_ACCESS_BEARER_FAILURE";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::RADIO_ACCESS_BEARER_FAILURE;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::ESM_UNKNOWN_EPS_BEARER_CONTEXT) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::ESM_UNKNOWN_EPS_BEARER_CONTEXT)) {
        os += (first ? "" : " | ");
        os += "ESM_UNKNOWN_EPS_BEARER_CONTEXT";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::ESM_UNKNOWN_EPS_BEARER_CONTEXT;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::DRB_RELEASED_BY_RRC) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::DRB_RELEASED_BY_RRC)) {
        os += (first ? "" : " | ");
        os += "DRB_RELEASED_BY_RRC";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::DRB_RELEASED_BY_RRC;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::CONNECTION_RELEASED) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::CONNECTION_RELEASED)) {
        os += (first ? "" : " | ");
        os += "CONNECTION_RELEASED";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::CONNECTION_RELEASED;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::EMM_DETACHED) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::EMM_DETACHED)) {
        os += (first ? "" : " | ");
        os += "EMM_DETACHED";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::EMM_DETACHED;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::EMM_ATTACH_FAILED) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::EMM_ATTACH_FAILED)) {
        os += (first ? "" : " | ");
        os += "EMM_ATTACH_FAILED";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::EMM_ATTACH_FAILED;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::EMM_ATTACH_STARTED) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::EMM_ATTACH_STARTED)) {
        os += (first ? "" : " | ");
        os += "EMM_ATTACH_STARTED";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::EMM_ATTACH_STARTED;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::LTE_NAS_SERVICE_REQUEST_FAILED) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::LTE_NAS_SERVICE_REQUEST_FAILED)) {
        os += (first ? "" : " | ");
        os += "LTE_NAS_SERVICE_REQUEST_FAILED";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::LTE_NAS_SERVICE_REQUEST_FAILED;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::DUPLICATE_BEARER_ID) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::DUPLICATE_BEARER_ID)) {
        os += (first ? "" : " | ");
        os += "DUPLICATE_BEARER_ID";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::DUPLICATE_BEARER_ID;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::ESM_COLLISION_SCENARIOS) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::ESM_COLLISION_SCENARIOS)) {
        os += (first ? "" : " | ");
        os += "ESM_COLLISION_SCENARIOS";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::ESM_COLLISION_SCENARIOS;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::ESM_BEARER_DEACTIVATED_TO_SYNC_WITH_NETWORK) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::ESM_BEARER_DEACTIVATED_TO_SYNC_WITH_NETWORK)) {
        os += (first ? "" : " | ");
        os += "ESM_BEARER_DEACTIVATED_TO_SYNC_WITH_NETWORK";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::ESM_BEARER_DEACTIVATED_TO_SYNC_WITH_NETWORK;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::ESM_NW_ACTIVATED_DED_BEARER_WITH_ID_OF_DEF_BEARER) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::ESM_NW_ACTIVATED_DED_BEARER_WITH_ID_OF_DEF_BEARER)) {
        os += (first ? "" : " | ");
        os += "ESM_NW_ACTIVATED_DED_BEARER_WITH_ID_OF_DEF_BEARER";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::ESM_NW_ACTIVATED_DED_BEARER_WITH_ID_OF_DEF_BEARER;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::ESM_BAD_OTA_MESSAGE) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::ESM_BAD_OTA_MESSAGE)) {
        os += (first ? "" : " | ");
        os += "ESM_BAD_OTA_MESSAGE";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::ESM_BAD_OTA_MESSAGE;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::ESM_DOWNLOAD_SERVER_REJECTED_THE_CALL) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::ESM_DOWNLOAD_SERVER_REJECTED_THE_CALL)) {
        os += (first ? "" : " | ");
        os += "ESM_DOWNLOAD_SERVER_REJECTED_THE_CALL";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::ESM_DOWNLOAD_SERVER_REJECTED_THE_CALL;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::ESM_CONTEXT_TRANSFERRED_DUE_TO_IRAT) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::ESM_CONTEXT_TRANSFERRED_DUE_TO_IRAT)) {
        os += (first ? "" : " | ");
        os += "ESM_CONTEXT_TRANSFERRED_DUE_TO_IRAT";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::ESM_CONTEXT_TRANSFERRED_DUE_TO_IRAT;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::DS_EXPLICIT_DEACTIVATION) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::DS_EXPLICIT_DEACTIVATION)) {
        os += (first ? "" : " | ");
        os += "DS_EXPLICIT_DEACTIVATION";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::DS_EXPLICIT_DEACTIVATION;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::ESM_LOCAL_CAUSE_NONE) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::ESM_LOCAL_CAUSE_NONE)) {
        os += (first ? "" : " | ");
        os += "ESM_LOCAL_CAUSE_NONE";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::ESM_LOCAL_CAUSE_NONE;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::LTE_THROTTLING_NOT_REQUIRED) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::LTE_THROTTLING_NOT_REQUIRED)) {
        os += (first ? "" : " | ");
        os += "LTE_THROTTLING_NOT_REQUIRED";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::LTE_THROTTLING_NOT_REQUIRED;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::ACCESS_CONTROL_LIST_CHECK_FAILURE) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::ACCESS_CONTROL_LIST_CHECK_FAILURE)) {
        os += (first ? "" : " | ");
        os += "ACCESS_CONTROL_LIST_CHECK_FAILURE";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::ACCESS_CONTROL_LIST_CHECK_FAILURE;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::SERVICE_NOT_ALLOWED_ON_PLMN) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::SERVICE_NOT_ALLOWED_ON_PLMN)) {
        os += (first ? "" : " | ");
        os += "SERVICE_NOT_ALLOWED_ON_PLMN";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::SERVICE_NOT_ALLOWED_ON_PLMN;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::EMM_T3417_EXPIRED) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::EMM_T3417_EXPIRED)) {
        os += (first ? "" : " | ");
        os += "EMM_T3417_EXPIRED";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::EMM_T3417_EXPIRED;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::EMM_T3417_EXT_EXPIRED) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::EMM_T3417_EXT_EXPIRED)) {
        os += (first ? "" : " | ");
        os += "EMM_T3417_EXT_EXPIRED";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::EMM_T3417_EXT_EXPIRED;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::RRC_UPLINK_DATA_TRANSMISSION_FAILURE) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::RRC_UPLINK_DATA_TRANSMISSION_FAILURE)) {
        os += (first ? "" : " | ");
        os += "RRC_UPLINK_DATA_TRANSMISSION_FAILURE";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::RRC_UPLINK_DATA_TRANSMISSION_FAILURE;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::RRC_UPLINK_DELIVERY_FAILED_DUE_TO_HANDOVER) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::RRC_UPLINK_DELIVERY_FAILED_DUE_TO_HANDOVER)) {
        os += (first ? "" : " | ");
        os += "RRC_UPLINK_DELIVERY_FAILED_DUE_TO_HANDOVER";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::RRC_UPLINK_DELIVERY_FAILED_DUE_TO_HANDOVER;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::RRC_UPLINK_CONNECTION_RELEASE) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::RRC_UPLINK_CONNECTION_RELEASE)) {
        os += (first ? "" : " | ");
        os += "RRC_UPLINK_CONNECTION_RELEASE";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::RRC_UPLINK_CONNECTION_RELEASE;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::RRC_UPLINK_RADIO_LINK_FAILURE) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::RRC_UPLINK_RADIO_LINK_FAILURE)) {
        os += (first ? "" : " | ");
        os += "RRC_UPLINK_RADIO_LINK_FAILURE";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::RRC_UPLINK_RADIO_LINK_FAILURE;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::RRC_UPLINK_ERROR_REQUEST_FROM_NAS) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::RRC_UPLINK_ERROR_REQUEST_FROM_NAS)) {
        os += (first ? "" : " | ");
        os += "RRC_UPLINK_ERROR_REQUEST_FROM_NAS";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::RRC_UPLINK_ERROR_REQUEST_FROM_NAS;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::RRC_CONNECTION_ACCESS_STRATUM_FAILURE) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::RRC_CONNECTION_ACCESS_STRATUM_FAILURE)) {
        os += (first ? "" : " | ");
        os += "RRC_CONNECTION_ACCESS_STRATUM_FAILURE";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::RRC_CONNECTION_ACCESS_STRATUM_FAILURE;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::RRC_CONNECTION_ANOTHER_PROCEDURE_IN_PROGRESS) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::RRC_CONNECTION_ANOTHER_PROCEDURE_IN_PROGRESS)) {
        os += (first ? "" : " | ");
        os += "RRC_CONNECTION_ANOTHER_PROCEDURE_IN_PROGRESS";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::RRC_CONNECTION_ANOTHER_PROCEDURE_IN_PROGRESS;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::RRC_CONNECTION_ACCESS_BARRED) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::RRC_CONNECTION_ACCESS_BARRED)) {
        os += (first ? "" : " | ");
        os += "RRC_CONNECTION_ACCESS_BARRED";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::RRC_CONNECTION_ACCESS_BARRED;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::RRC_CONNECTION_CELL_RESELECTION) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::RRC_CONNECTION_CELL_RESELECTION)) {
        os += (first ? "" : " | ");
        os += "RRC_CONNECTION_CELL_RESELECTION";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::RRC_CONNECTION_CELL_RESELECTION;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::RRC_CONNECTION_CONFIG_FAILURE) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::RRC_CONNECTION_CONFIG_FAILURE)) {
        os += (first ? "" : " | ");
        os += "RRC_CONNECTION_CONFIG_FAILURE";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::RRC_CONNECTION_CONFIG_FAILURE;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::RRC_CONNECTION_TIMER_EXPIRED) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::RRC_CONNECTION_TIMER_EXPIRED)) {
        os += (first ? "" : " | ");
        os += "RRC_CONNECTION_TIMER_EXPIRED";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::RRC_CONNECTION_TIMER_EXPIRED;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::RRC_CONNECTION_LINK_FAILURE) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::RRC_CONNECTION_LINK_FAILURE)) {
        os += (first ? "" : " | ");
        os += "RRC_CONNECTION_LINK_FAILURE";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::RRC_CONNECTION_LINK_FAILURE;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::RRC_CONNECTION_CELL_NOT_CAMPED) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::RRC_CONNECTION_CELL_NOT_CAMPED)) {
        os += (first ? "" : " | ");
        os += "RRC_CONNECTION_CELL_NOT_CAMPED";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::RRC_CONNECTION_CELL_NOT_CAMPED;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::RRC_CONNECTION_SYSTEM_INTERVAL_FAILURE) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::RRC_CONNECTION_SYSTEM_INTERVAL_FAILURE)) {
        os += (first ? "" : " | ");
        os += "RRC_CONNECTION_SYSTEM_INTERVAL_FAILURE";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::RRC_CONNECTION_SYSTEM_INTERVAL_FAILURE;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::RRC_CONNECTION_REJECT_BY_NETWORK) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::RRC_CONNECTION_REJECT_BY_NETWORK)) {
        os += (first ? "" : " | ");
        os += "RRC_CONNECTION_REJECT_BY_NETWORK";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::RRC_CONNECTION_REJECT_BY_NETWORK;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::RRC_CONNECTION_NORMAL_RELEASE) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::RRC_CONNECTION_NORMAL_RELEASE)) {
        os += (first ? "" : " | ");
        os += "RRC_CONNECTION_NORMAL_RELEASE";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::RRC_CONNECTION_NORMAL_RELEASE;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::RRC_CONNECTION_RADIO_LINK_FAILURE) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::RRC_CONNECTION_RADIO_LINK_FAILURE)) {
        os += (first ? "" : " | ");
        os += "RRC_CONNECTION_RADIO_LINK_FAILURE";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::RRC_CONNECTION_RADIO_LINK_FAILURE;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::RRC_CONNECTION_REESTABLISHMENT_FAILURE) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::RRC_CONNECTION_REESTABLISHMENT_FAILURE)) {
        os += (first ? "" : " | ");
        os += "RRC_CONNECTION_REESTABLISHMENT_FAILURE";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::RRC_CONNECTION_REESTABLISHMENT_FAILURE;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::RRC_CONNECTION_OUT_OF_SERVICE_DURING_CELL_REGISTER) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::RRC_CONNECTION_OUT_OF_SERVICE_DURING_CELL_REGISTER)) {
        os += (first ? "" : " | ");
        os += "RRC_CONNECTION_OUT_OF_SERVICE_DURING_CELL_REGISTER";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::RRC_CONNECTION_OUT_OF_SERVICE_DURING_CELL_REGISTER;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::RRC_CONNECTION_ABORT_REQUEST) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::RRC_CONNECTION_ABORT_REQUEST)) {
        os += (first ? "" : " | ");
        os += "RRC_CONNECTION_ABORT_REQUEST";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::RRC_CONNECTION_ABORT_REQUEST;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::RRC_CONNECTION_SYSTEM_INFORMATION_BLOCK_READ_ERROR) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::RRC_CONNECTION_SYSTEM_INFORMATION_BLOCK_READ_ERROR)) {
        os += (first ? "" : " | ");
        os += "RRC_CONNECTION_SYSTEM_INFORMATION_BLOCK_READ_ERROR";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::RRC_CONNECTION_SYSTEM_INFORMATION_BLOCK_READ_ERROR;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::NETWORK_INITIATED_DETACH_WITH_AUTO_REATTACH) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::NETWORK_INITIATED_DETACH_WITH_AUTO_REATTACH)) {
        os += (first ? "" : " | ");
        os += "NETWORK_INITIATED_DETACH_WITH_AUTO_REATTACH";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::NETWORK_INITIATED_DETACH_WITH_AUTO_REATTACH;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::NETWORK_INITIATED_DETACH_NO_AUTO_REATTACH) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::NETWORK_INITIATED_DETACH_NO_AUTO_REATTACH)) {
        os += (first ? "" : " | ");
        os += "NETWORK_INITIATED_DETACH_NO_AUTO_REATTACH";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::NETWORK_INITIATED_DETACH_NO_AUTO_REATTACH;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::ESM_PROCEDURE_TIME_OUT) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::ESM_PROCEDURE_TIME_OUT)) {
        os += (first ? "" : " | ");
        os += "ESM_PROCEDURE_TIME_OUT";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::ESM_PROCEDURE_TIME_OUT;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::INVALID_CONNECTION_ID) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::INVALID_CONNECTION_ID)) {
        os += (first ? "" : " | ");
        os += "INVALID_CONNECTION_ID";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::INVALID_CONNECTION_ID;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::MAXIMIUM_NSAPIS_EXCEEDED) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::MAXIMIUM_NSAPIS_EXCEEDED)) {
        os += (first ? "" : " | ");
        os += "MAXIMIUM_NSAPIS_EXCEEDED";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::MAXIMIUM_NSAPIS_EXCEEDED;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::INVALID_PRIMARY_NSAPI) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::INVALID_PRIMARY_NSAPI)) {
        os += (first ? "" : " | ");
        os += "INVALID_PRIMARY_NSAPI";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::INVALID_PRIMARY_NSAPI;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::CANNOT_ENCODE_OTA_MESSAGE) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::CANNOT_ENCODE_OTA_MESSAGE)) {
        os += (first ? "" : " | ");
        os += "CANNOT_ENCODE_OTA_MESSAGE";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::CANNOT_ENCODE_OTA_MESSAGE;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::RADIO_ACCESS_BEARER_SETUP_FAILURE) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::RADIO_ACCESS_BEARER_SETUP_FAILURE)) {
        os += (first ? "" : " | ");
        os += "RADIO_ACCESS_BEARER_SETUP_FAILURE";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::RADIO_ACCESS_BEARER_SETUP_FAILURE;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::PDP_ESTABLISH_TIMEOUT_EXPIRED) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::PDP_ESTABLISH_TIMEOUT_EXPIRED)) {
        os += (first ? "" : " | ");
        os += "PDP_ESTABLISH_TIMEOUT_EXPIRED";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::PDP_ESTABLISH_TIMEOUT_EXPIRED;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::PDP_MODIFY_TIMEOUT_EXPIRED) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::PDP_MODIFY_TIMEOUT_EXPIRED)) {
        os += (first ? "" : " | ");
        os += "PDP_MODIFY_TIMEOUT_EXPIRED";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::PDP_MODIFY_TIMEOUT_EXPIRED;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::PDP_INACTIVE_TIMEOUT_EXPIRED) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::PDP_INACTIVE_TIMEOUT_EXPIRED)) {
        os += (first ? "" : " | ");
        os += "PDP_INACTIVE_TIMEOUT_EXPIRED";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::PDP_INACTIVE_TIMEOUT_EXPIRED;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::PDP_LOWERLAYER_ERROR) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::PDP_LOWERLAYER_ERROR)) {
        os += (first ? "" : " | ");
        os += "PDP_LOWERLAYER_ERROR";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::PDP_LOWERLAYER_ERROR;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::PDP_MODIFY_COLLISION) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::PDP_MODIFY_COLLISION)) {
        os += (first ? "" : " | ");
        os += "PDP_MODIFY_COLLISION";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::PDP_MODIFY_COLLISION;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::MAXINUM_SIZE_OF_L2_MESSAGE_EXCEEDED) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::MAXINUM_SIZE_OF_L2_MESSAGE_EXCEEDED)) {
        os += (first ? "" : " | ");
        os += "MAXINUM_SIZE_OF_L2_MESSAGE_EXCEEDED";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::MAXINUM_SIZE_OF_L2_MESSAGE_EXCEEDED;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::NAS_REQUEST_REJECTED_BY_NETWORK) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::NAS_REQUEST_REJECTED_BY_NETWORK)) {
        os += (first ? "" : " | ");
        os += "NAS_REQUEST_REJECTED_BY_NETWORK";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::NAS_REQUEST_REJECTED_BY_NETWORK;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::RRC_CONNECTION_INVALID_REQUEST) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::RRC_CONNECTION_INVALID_REQUEST)) {
        os += (first ? "" : " | ");
        os += "RRC_CONNECTION_INVALID_REQUEST";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::RRC_CONNECTION_INVALID_REQUEST;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::RRC_CONNECTION_TRACKING_AREA_ID_CHANGED) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::RRC_CONNECTION_TRACKING_AREA_ID_CHANGED)) {
        os += (first ? "" : " | ");
        os += "RRC_CONNECTION_TRACKING_AREA_ID_CHANGED";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::RRC_CONNECTION_TRACKING_AREA_ID_CHANGED;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::RRC_CONNECTION_RF_UNAVAILABLE) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::RRC_CONNECTION_RF_UNAVAILABLE)) {
        os += (first ? "" : " | ");
        os += "RRC_CONNECTION_RF_UNAVAILABLE";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::RRC_CONNECTION_RF_UNAVAILABLE;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::RRC_CONNECTION_ABORTED_DUE_TO_IRAT_CHANGE) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::RRC_CONNECTION_ABORTED_DUE_TO_IRAT_CHANGE)) {
        os += (first ? "" : " | ");
        os += "RRC_CONNECTION_ABORTED_DUE_TO_IRAT_CHANGE";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::RRC_CONNECTION_ABORTED_DUE_TO_IRAT_CHANGE;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::RRC_CONNECTION_RELEASED_SECURITY_NOT_ACTIVE) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::RRC_CONNECTION_RELEASED_SECURITY_NOT_ACTIVE)) {
        os += (first ? "" : " | ");
        os += "RRC_CONNECTION_RELEASED_SECURITY_NOT_ACTIVE";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::RRC_CONNECTION_RELEASED_SECURITY_NOT_ACTIVE;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::RRC_CONNECTION_ABORTED_AFTER_HANDOVER) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::RRC_CONNECTION_ABORTED_AFTER_HANDOVER)) {
        os += (first ? "" : " | ");
        os += "RRC_CONNECTION_ABORTED_AFTER_HANDOVER";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::RRC_CONNECTION_ABORTED_AFTER_HANDOVER;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::RRC_CONNECTION_ABORTED_AFTER_IRAT_CELL_CHANGE) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::RRC_CONNECTION_ABORTED_AFTER_IRAT_CELL_CHANGE)) {
        os += (first ? "" : " | ");
        os += "RRC_CONNECTION_ABORTED_AFTER_IRAT_CELL_CHANGE";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::RRC_CONNECTION_ABORTED_AFTER_IRAT_CELL_CHANGE;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::RRC_CONNECTION_ABORTED_DURING_IRAT_CELL_CHANGE) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::RRC_CONNECTION_ABORTED_DURING_IRAT_CELL_CHANGE)) {
        os += (first ? "" : " | ");
        os += "RRC_CONNECTION_ABORTED_DURING_IRAT_CELL_CHANGE";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::RRC_CONNECTION_ABORTED_DURING_IRAT_CELL_CHANGE;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::IMSI_UNKNOWN_IN_HOME_SUBSCRIBER_SERVER) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::IMSI_UNKNOWN_IN_HOME_SUBSCRIBER_SERVER)) {
        os += (first ? "" : " | ");
        os += "IMSI_UNKNOWN_IN_HOME_SUBSCRIBER_SERVER";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::IMSI_UNKNOWN_IN_HOME_SUBSCRIBER_SERVER;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::IMEI_NOT_ACCEPTED) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::IMEI_NOT_ACCEPTED)) {
        os += (first ? "" : " | ");
        os += "IMEI_NOT_ACCEPTED";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::IMEI_NOT_ACCEPTED;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::EPS_SERVICES_AND_NON_EPS_SERVICES_NOT_ALLOWED) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::EPS_SERVICES_AND_NON_EPS_SERVICES_NOT_ALLOWED)) {
        os += (first ? "" : " | ");
        os += "EPS_SERVICES_AND_NON_EPS_SERVICES_NOT_ALLOWED";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::EPS_SERVICES_AND_NON_EPS_SERVICES_NOT_ALLOWED;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::EPS_SERVICES_NOT_ALLOWED_IN_PLMN) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::EPS_SERVICES_NOT_ALLOWED_IN_PLMN)) {
        os += (first ? "" : " | ");
        os += "EPS_SERVICES_NOT_ALLOWED_IN_PLMN";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::EPS_SERVICES_NOT_ALLOWED_IN_PLMN;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::MSC_TEMPORARILY_NOT_REACHABLE) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::MSC_TEMPORARILY_NOT_REACHABLE)) {
        os += (first ? "" : " | ");
        os += "MSC_TEMPORARILY_NOT_REACHABLE";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::MSC_TEMPORARILY_NOT_REACHABLE;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::CS_DOMAIN_NOT_AVAILABLE) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::CS_DOMAIN_NOT_AVAILABLE)) {
        os += (first ? "" : " | ");
        os += "CS_DOMAIN_NOT_AVAILABLE";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::CS_DOMAIN_NOT_AVAILABLE;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::ESM_FAILURE) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::ESM_FAILURE)) {
        os += (first ? "" : " | ");
        os += "ESM_FAILURE";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::ESM_FAILURE;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::MAC_FAILURE) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::MAC_FAILURE)) {
        os += (first ? "" : " | ");
        os += "MAC_FAILURE";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::MAC_FAILURE;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::SYNCHRONIZATION_FAILURE) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::SYNCHRONIZATION_FAILURE)) {
        os += (first ? "" : " | ");
        os += "SYNCHRONIZATION_FAILURE";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::SYNCHRONIZATION_FAILURE;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::UE_SECURITY_CAPABILITIES_MISMATCH) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::UE_SECURITY_CAPABILITIES_MISMATCH)) {
        os += (first ? "" : " | ");
        os += "UE_SECURITY_CAPABILITIES_MISMATCH";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::UE_SECURITY_CAPABILITIES_MISMATCH;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::SECURITY_MODE_REJECTED) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::SECURITY_MODE_REJECTED)) {
        os += (first ? "" : " | ");
        os += "SECURITY_MODE_REJECTED";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::SECURITY_MODE_REJECTED;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::UNACCEPTABLE_NON_EPS_AUTHENTICATION) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::UNACCEPTABLE_NON_EPS_AUTHENTICATION)) {
        os += (first ? "" : " | ");
        os += "UNACCEPTABLE_NON_EPS_AUTHENTICATION";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::UNACCEPTABLE_NON_EPS_AUTHENTICATION;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::CS_FALLBACK_CALL_ESTABLISHMENT_NOT_ALLOWED) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::CS_FALLBACK_CALL_ESTABLISHMENT_NOT_ALLOWED)) {
        os += (first ? "" : " | ");
        os += "CS_FALLBACK_CALL_ESTABLISHMENT_NOT_ALLOWED";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::CS_FALLBACK_CALL_ESTABLISHMENT_NOT_ALLOWED;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::NO_EPS_BEARER_CONTEXT_ACTIVATED) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::NO_EPS_BEARER_CONTEXT_ACTIVATED)) {
        os += (first ? "" : " | ");
        os += "NO_EPS_BEARER_CONTEXT_ACTIVATED";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::NO_EPS_BEARER_CONTEXT_ACTIVATED;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::INVALID_EMM_STATE) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::INVALID_EMM_STATE)) {
        os += (first ? "" : " | ");
        os += "INVALID_EMM_STATE";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::INVALID_EMM_STATE;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::NAS_LAYER_FAILURE) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::NAS_LAYER_FAILURE)) {
        os += (first ? "" : " | ");
        os += "NAS_LAYER_FAILURE";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::NAS_LAYER_FAILURE;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::MULTIPLE_PDP_CALL_NOT_ALLOWED) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::MULTIPLE_PDP_CALL_NOT_ALLOWED)) {
        os += (first ? "" : " | ");
        os += "MULTIPLE_PDP_CALL_NOT_ALLOWED";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::MULTIPLE_PDP_CALL_NOT_ALLOWED;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::IRAT_HANDOVER_FAILED) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::IRAT_HANDOVER_FAILED)) {
        os += (first ? "" : " | ");
        os += "IRAT_HANDOVER_FAILED";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::IRAT_HANDOVER_FAILED;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::TEST_LOOPBACK_REGULAR_DEACTIVATION) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::TEST_LOOPBACK_REGULAR_DEACTIVATION)) {
        os += (first ? "" : " | ");
        os += "TEST_LOOPBACK_REGULAR_DEACTIVATION";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::TEST_LOOPBACK_REGULAR_DEACTIVATION;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::LOWER_LAYER_REGISTRATION_FAILURE) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::LOWER_LAYER_REGISTRATION_FAILURE)) {
        os += (first ? "" : " | ");
        os += "LOWER_LAYER_REGISTRATION_FAILURE";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::LOWER_LAYER_REGISTRATION_FAILURE;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::DATA_PLAN_EXPIRED) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::DATA_PLAN_EXPIRED)) {
        os += (first ? "" : " | ");
        os += "DATA_PLAN_EXPIRED";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::DATA_PLAN_EXPIRED;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::UMTS_HANDOVER_TO_IWLAN) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::UMTS_HANDOVER_TO_IWLAN)) {
        os += (first ? "" : " | ");
        os += "UMTS_HANDOVER_TO_IWLAN";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::UMTS_HANDOVER_TO_IWLAN;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::EVDO_CONNECTION_DENY_BY_GENERAL_OR_NETWORK_BUSY) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::EVDO_CONNECTION_DENY_BY_GENERAL_OR_NETWORK_BUSY)) {
        os += (first ? "" : " | ");
        os += "EVDO_CONNECTION_DENY_BY_GENERAL_OR_NETWORK_BUSY";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::EVDO_CONNECTION_DENY_BY_GENERAL_OR_NETWORK_BUSY;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::EVDO_CONNECTION_DENY_BY_BILLING_OR_AUTHENTICATION_FAILURE) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::EVDO_CONNECTION_DENY_BY_BILLING_OR_AUTHENTICATION_FAILURE)) {
        os += (first ? "" : " | ");
        os += "EVDO_CONNECTION_DENY_BY_BILLING_OR_AUTHENTICATION_FAILURE";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::EVDO_CONNECTION_DENY_BY_BILLING_OR_AUTHENTICATION_FAILURE;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::EVDO_HDR_CHANGED) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::EVDO_HDR_CHANGED)) {
        os += (first ? "" : " | ");
        os += "EVDO_HDR_CHANGED";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::EVDO_HDR_CHANGED;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::EVDO_HDR_EXITED) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::EVDO_HDR_EXITED)) {
        os += (first ? "" : " | ");
        os += "EVDO_HDR_EXITED";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::EVDO_HDR_EXITED;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::EVDO_HDR_NO_SESSION) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::EVDO_HDR_NO_SESSION)) {
        os += (first ? "" : " | ");
        os += "EVDO_HDR_NO_SESSION";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::EVDO_HDR_NO_SESSION;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::EVDO_USING_GPS_FIX_INSTEAD_OF_HDR_CALL) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::EVDO_USING_GPS_FIX_INSTEAD_OF_HDR_CALL)) {
        os += (first ? "" : " | ");
        os += "EVDO_USING_GPS_FIX_INSTEAD_OF_HDR_CALL";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::EVDO_USING_GPS_FIX_INSTEAD_OF_HDR_CALL;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::EVDO_HDR_CONNECTION_SETUP_TIMEOUT) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::EVDO_HDR_CONNECTION_SETUP_TIMEOUT)) {
        os += (first ? "" : " | ");
        os += "EVDO_HDR_CONNECTION_SETUP_TIMEOUT";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::EVDO_HDR_CONNECTION_SETUP_TIMEOUT;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::FAILED_TO_ACQUIRE_COLOCATED_HDR) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::FAILED_TO_ACQUIRE_COLOCATED_HDR)) {
        os += (first ? "" : " | ");
        os += "FAILED_TO_ACQUIRE_COLOCATED_HDR";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::FAILED_TO_ACQUIRE_COLOCATED_HDR;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::OTASP_COMMIT_IN_PROGRESS) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::OTASP_COMMIT_IN_PROGRESS)) {
        os += (first ? "" : " | ");
        os += "OTASP_COMMIT_IN_PROGRESS";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::OTASP_COMMIT_IN_PROGRESS;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::NO_HYBRID_HDR_SERVICE) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::NO_HYBRID_HDR_SERVICE)) {
        os += (first ? "" : " | ");
        os += "NO_HYBRID_HDR_SERVICE";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::NO_HYBRID_HDR_SERVICE;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::HDR_NO_LOCK_GRANTED) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::HDR_NO_LOCK_GRANTED)) {
        os += (first ? "" : " | ");
        os += "HDR_NO_LOCK_GRANTED";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::HDR_NO_LOCK_GRANTED;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::DBM_OR_SMS_IN_PROGRESS) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::DBM_OR_SMS_IN_PROGRESS)) {
        os += (first ? "" : " | ");
        os += "DBM_OR_SMS_IN_PROGRESS";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::DBM_OR_SMS_IN_PROGRESS;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::HDR_FADE) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::HDR_FADE)) {
        os += (first ? "" : " | ");
        os += "HDR_FADE";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::HDR_FADE;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::HDR_ACCESS_FAILURE) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::HDR_ACCESS_FAILURE)) {
        os += (first ? "" : " | ");
        os += "HDR_ACCESS_FAILURE";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::HDR_ACCESS_FAILURE;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::UNSUPPORTED_1X_PREV) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::UNSUPPORTED_1X_PREV)) {
        os += (first ? "" : " | ");
        os += "UNSUPPORTED_1X_PREV";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::UNSUPPORTED_1X_PREV;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::LOCAL_END) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::LOCAL_END)) {
        os += (first ? "" : " | ");
        os += "LOCAL_END";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::LOCAL_END;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::NO_SERVICE) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::NO_SERVICE)) {
        os += (first ? "" : " | ");
        os += "NO_SERVICE";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::NO_SERVICE;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::FADE) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::FADE)) {
        os += (first ? "" : " | ");
        os += "FADE";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::FADE;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::NORMAL_RELEASE) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::NORMAL_RELEASE)) {
        os += (first ? "" : " | ");
        os += "NORMAL_RELEASE";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::NORMAL_RELEASE;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::ACCESS_ATTEMPT_ALREADY_IN_PROGRESS) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::ACCESS_ATTEMPT_ALREADY_IN_PROGRESS)) {
        os += (first ? "" : " | ");
        os += "ACCESS_ATTEMPT_ALREADY_IN_PROGRESS";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::ACCESS_ATTEMPT_ALREADY_IN_PROGRESS;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::REDIRECTION_OR_HANDOFF_IN_PROGRESS) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::REDIRECTION_OR_HANDOFF_IN_PROGRESS)) {
        os += (first ? "" : " | ");
        os += "REDIRECTION_OR_HANDOFF_IN_PROGRESS";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::REDIRECTION_OR_HANDOFF_IN_PROGRESS;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::EMERGENCY_MODE) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::EMERGENCY_MODE)) {
        os += (first ? "" : " | ");
        os += "EMERGENCY_MODE";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::EMERGENCY_MODE;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::PHONE_IN_USE) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::PHONE_IN_USE)) {
        os += (first ? "" : " | ");
        os += "PHONE_IN_USE";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::PHONE_IN_USE;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::INVALID_MODE) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::INVALID_MODE)) {
        os += (first ? "" : " | ");
        os += "INVALID_MODE";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::INVALID_MODE;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::INVALID_SIM_STATE) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::INVALID_SIM_STATE)) {
        os += (first ? "" : " | ");
        os += "INVALID_SIM_STATE";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::INVALID_SIM_STATE;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::NO_COLLOCATED_HDR) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::NO_COLLOCATED_HDR)) {
        os += (first ? "" : " | ");
        os += "NO_COLLOCATED_HDR";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::NO_COLLOCATED_HDR;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::UE_IS_ENTERING_POWERSAVE_MODE) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::UE_IS_ENTERING_POWERSAVE_MODE)) {
        os += (first ? "" : " | ");
        os += "UE_IS_ENTERING_POWERSAVE_MODE";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::UE_IS_ENTERING_POWERSAVE_MODE;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::DUAL_SWITCH) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::DUAL_SWITCH)) {
        os += (first ? "" : " | ");
        os += "DUAL_SWITCH";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::DUAL_SWITCH;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::PPP_TIMEOUT) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::PPP_TIMEOUT)) {
        os += (first ? "" : " | ");
        os += "PPP_TIMEOUT";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::PPP_TIMEOUT;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::PPP_AUTH_FAILURE) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::PPP_AUTH_FAILURE)) {
        os += (first ? "" : " | ");
        os += "PPP_AUTH_FAILURE";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::PPP_AUTH_FAILURE;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::PPP_OPTION_MISMATCH) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::PPP_OPTION_MISMATCH)) {
        os += (first ? "" : " | ");
        os += "PPP_OPTION_MISMATCH";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::PPP_OPTION_MISMATCH;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::PPP_PAP_FAILURE) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::PPP_PAP_FAILURE)) {
        os += (first ? "" : " | ");
        os += "PPP_PAP_FAILURE";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::PPP_PAP_FAILURE;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::PPP_CHAP_FAILURE) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::PPP_CHAP_FAILURE)) {
        os += (first ? "" : " | ");
        os += "PPP_CHAP_FAILURE";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::PPP_CHAP_FAILURE;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::PPP_CLOSE_IN_PROGRESS) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::PPP_CLOSE_IN_PROGRESS)) {
        os += (first ? "" : " | ");
        os += "PPP_CLOSE_IN_PROGRESS";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::PPP_CLOSE_IN_PROGRESS;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::LIMITED_TO_IPV4) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::LIMITED_TO_IPV4)) {
        os += (first ? "" : " | ");
        os += "LIMITED_TO_IPV4";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::LIMITED_TO_IPV4;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::LIMITED_TO_IPV6) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::LIMITED_TO_IPV6)) {
        os += (first ? "" : " | ");
        os += "LIMITED_TO_IPV6";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::LIMITED_TO_IPV6;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::VSNCP_TIMEOUT) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::VSNCP_TIMEOUT)) {
        os += (first ? "" : " | ");
        os += "VSNCP_TIMEOUT";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::VSNCP_TIMEOUT;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::VSNCP_GEN_ERROR) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::VSNCP_GEN_ERROR)) {
        os += (first ? "" : " | ");
        os += "VSNCP_GEN_ERROR";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::VSNCP_GEN_ERROR;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::VSNCP_APN_UNATHORIZED) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::VSNCP_APN_UNATHORIZED)) {
        os += (first ? "" : " | ");
        os += "VSNCP_APN_UNATHORIZED";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::VSNCP_APN_UNATHORIZED;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::VSNCP_PDN_LIMIT_EXCEEDED) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::VSNCP_PDN_LIMIT_EXCEEDED)) {
        os += (first ? "" : " | ");
        os += "VSNCP_PDN_LIMIT_EXCEEDED";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::VSNCP_PDN_LIMIT_EXCEEDED;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::VSNCP_NO_PDN_GATEWAY_ADDRESS) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::VSNCP_NO_PDN_GATEWAY_ADDRESS)) {
        os += (first ? "" : " | ");
        os += "VSNCP_NO_PDN_GATEWAY_ADDRESS";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::VSNCP_NO_PDN_GATEWAY_ADDRESS;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::VSNCP_PDN_GATEWAY_UNREACHABLE) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::VSNCP_PDN_GATEWAY_UNREACHABLE)) {
        os += (first ? "" : " | ");
        os += "VSNCP_PDN_GATEWAY_UNREACHABLE";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::VSNCP_PDN_GATEWAY_UNREACHABLE;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::VSNCP_PDN_GATEWAY_REJECT) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::VSNCP_PDN_GATEWAY_REJECT)) {
        os += (first ? "" : " | ");
        os += "VSNCP_PDN_GATEWAY_REJECT";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::VSNCP_PDN_GATEWAY_REJECT;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::VSNCP_INSUFFICIENT_PARAMETERS) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::VSNCP_INSUFFICIENT_PARAMETERS)) {
        os += (first ? "" : " | ");
        os += "VSNCP_INSUFFICIENT_PARAMETERS";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::VSNCP_INSUFFICIENT_PARAMETERS;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::VSNCP_RESOURCE_UNAVAILABLE) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::VSNCP_RESOURCE_UNAVAILABLE)) {
        os += (first ? "" : " | ");
        os += "VSNCP_RESOURCE_UNAVAILABLE";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::VSNCP_RESOURCE_UNAVAILABLE;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::VSNCP_ADMINISTRATIVELY_PROHIBITED) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::VSNCP_ADMINISTRATIVELY_PROHIBITED)) {
        os += (first ? "" : " | ");
        os += "VSNCP_ADMINISTRATIVELY_PROHIBITED";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::VSNCP_ADMINISTRATIVELY_PROHIBITED;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::VSNCP_PDN_ID_IN_USE) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::VSNCP_PDN_ID_IN_USE)) {
        os += (first ? "" : " | ");
        os += "VSNCP_PDN_ID_IN_USE";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::VSNCP_PDN_ID_IN_USE;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::VSNCP_SUBSCRIBER_LIMITATION) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::VSNCP_SUBSCRIBER_LIMITATION)) {
        os += (first ? "" : " | ");
        os += "VSNCP_SUBSCRIBER_LIMITATION";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::VSNCP_SUBSCRIBER_LIMITATION;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::VSNCP_PDN_EXISTS_FOR_THIS_APN) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::VSNCP_PDN_EXISTS_FOR_THIS_APN)) {
        os += (first ? "" : " | ");
        os += "VSNCP_PDN_EXISTS_FOR_THIS_APN";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::VSNCP_PDN_EXISTS_FOR_THIS_APN;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::VSNCP_RECONNECT_NOT_ALLOWED) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::VSNCP_RECONNECT_NOT_ALLOWED)) {
        os += (first ? "" : " | ");
        os += "VSNCP_RECONNECT_NOT_ALLOWED";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::VSNCP_RECONNECT_NOT_ALLOWED;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::IPV6_PREFIX_UNAVAILABLE) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::IPV6_PREFIX_UNAVAILABLE)) {
        os += (first ? "" : " | ");
        os += "IPV6_PREFIX_UNAVAILABLE";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::IPV6_PREFIX_UNAVAILABLE;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::HANDOFF_PREFERENCE_CHANGED) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::HANDOFF_PREFERENCE_CHANGED)) {
        os += (first ? "" : " | ");
        os += "HANDOFF_PREFERENCE_CHANGED";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::HANDOFF_PREFERENCE_CHANGED;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::SLICE_REJECTED) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::SLICE_REJECTED)) {
        os += (first ? "" : " | ");
        os += "SLICE_REJECTED";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::SLICE_REJECTED;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::MATCH_ALL_RULE_NOT_ALLOWED) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::MATCH_ALL_RULE_NOT_ALLOWED)) {
        os += (first ? "" : " | ");
        os += "MATCH_ALL_RULE_NOT_ALLOWED";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::MATCH_ALL_RULE_NOT_ALLOWED;
    }
    if ((o & ::android::hardware::radio::V1_6::DataCallFailCause::ALL_MATCHING_RULES_FAILED) == static_cast<int32_t>(::android::hardware::radio::V1_6::DataCallFailCause::ALL_MATCHING_RULES_FAILED)) {
        os += (first ? "" : " | ");
        os += "ALL_MATCHING_RULES_FAILED";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataCallFailCause::ALL_MATCHING_RULES_FAILED;
    }
    if (o != flipped) {
        os += (first ? "" : " | ");
        os += toHexString(o & (~flipped));
    }os += " (";
    os += toHexString(o);
    os += ")";
    return os;
}

static inline std::string toString(::android::hardware::radio::V1_6::DataCallFailCause o) {
    using ::android::hardware::details::toHexString;
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::NONE) {
        return "NONE";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::OPERATOR_BARRED) {
        return "OPERATOR_BARRED";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::NAS_SIGNALLING) {
        return "NAS_SIGNALLING";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::INSUFFICIENT_RESOURCES) {
        return "INSUFFICIENT_RESOURCES";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::MISSING_UKNOWN_APN) {
        return "MISSING_UKNOWN_APN";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::UNKNOWN_PDP_ADDRESS_TYPE) {
        return "UNKNOWN_PDP_ADDRESS_TYPE";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::USER_AUTHENTICATION) {
        return "USER_AUTHENTICATION";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::ACTIVATION_REJECT_GGSN) {
        return "ACTIVATION_REJECT_GGSN";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::ACTIVATION_REJECT_UNSPECIFIED) {
        return "ACTIVATION_REJECT_UNSPECIFIED";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::SERVICE_OPTION_NOT_SUPPORTED) {
        return "SERVICE_OPTION_NOT_SUPPORTED";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::SERVICE_OPTION_NOT_SUBSCRIBED) {
        return "SERVICE_OPTION_NOT_SUBSCRIBED";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::SERVICE_OPTION_OUT_OF_ORDER) {
        return "SERVICE_OPTION_OUT_OF_ORDER";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::NSAPI_IN_USE) {
        return "NSAPI_IN_USE";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::REGULAR_DEACTIVATION) {
        return "REGULAR_DEACTIVATION";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::QOS_NOT_ACCEPTED) {
        return "QOS_NOT_ACCEPTED";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::NETWORK_FAILURE) {
        return "NETWORK_FAILURE";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::UMTS_REACTIVATION_REQ) {
        return "UMTS_REACTIVATION_REQ";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::FEATURE_NOT_SUPP) {
        return "FEATURE_NOT_SUPP";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::TFT_SEMANTIC_ERROR) {
        return "TFT_SEMANTIC_ERROR";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::TFT_SYTAX_ERROR) {
        return "TFT_SYTAX_ERROR";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::UNKNOWN_PDP_CONTEXT) {
        return "UNKNOWN_PDP_CONTEXT";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::FILTER_SEMANTIC_ERROR) {
        return "FILTER_SEMANTIC_ERROR";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::FILTER_SYTAX_ERROR) {
        return "FILTER_SYTAX_ERROR";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::PDP_WITHOUT_ACTIVE_TFT) {
        return "PDP_WITHOUT_ACTIVE_TFT";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::ONLY_IPV4_ALLOWED) {
        return "ONLY_IPV4_ALLOWED";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::ONLY_IPV6_ALLOWED) {
        return "ONLY_IPV6_ALLOWED";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::ONLY_SINGLE_BEARER_ALLOWED) {
        return "ONLY_SINGLE_BEARER_ALLOWED";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::ESM_INFO_NOT_RECEIVED) {
        return "ESM_INFO_NOT_RECEIVED";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::PDN_CONN_DOES_NOT_EXIST) {
        return "PDN_CONN_DOES_NOT_EXIST";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::MULTI_CONN_TO_SAME_PDN_NOT_ALLOWED) {
        return "MULTI_CONN_TO_SAME_PDN_NOT_ALLOWED";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::MAX_ACTIVE_PDP_CONTEXT_REACHED) {
        return "MAX_ACTIVE_PDP_CONTEXT_REACHED";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::UNSUPPORTED_APN_IN_CURRENT_PLMN) {
        return "UNSUPPORTED_APN_IN_CURRENT_PLMN";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::INVALID_TRANSACTION_ID) {
        return "INVALID_TRANSACTION_ID";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::MESSAGE_INCORRECT_SEMANTIC) {
        return "MESSAGE_INCORRECT_SEMANTIC";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::INVALID_MANDATORY_INFO) {
        return "INVALID_MANDATORY_INFO";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::MESSAGE_TYPE_UNSUPPORTED) {
        return "MESSAGE_TYPE_UNSUPPORTED";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::MSG_TYPE_NONCOMPATIBLE_STATE) {
        return "MSG_TYPE_NONCOMPATIBLE_STATE";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::UNKNOWN_INFO_ELEMENT) {
        return "UNKNOWN_INFO_ELEMENT";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::CONDITIONAL_IE_ERROR) {
        return "CONDITIONAL_IE_ERROR";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::MSG_AND_PROTOCOL_STATE_UNCOMPATIBLE) {
        return "MSG_AND_PROTOCOL_STATE_UNCOMPATIBLE";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::PROTOCOL_ERRORS) {
        return "PROTOCOL_ERRORS";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::APN_TYPE_CONFLICT) {
        return "APN_TYPE_CONFLICT";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::INVALID_PCSCF_ADDR) {
        return "INVALID_PCSCF_ADDR";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::INTERNAL_CALL_PREEMPT_BY_HIGH_PRIO_APN) {
        return "INTERNAL_CALL_PREEMPT_BY_HIGH_PRIO_APN";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::EMM_ACCESS_BARRED) {
        return "EMM_ACCESS_BARRED";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::EMERGENCY_IFACE_ONLY) {
        return "EMERGENCY_IFACE_ONLY";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::IFACE_MISMATCH) {
        return "IFACE_MISMATCH";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::COMPANION_IFACE_IN_USE) {
        return "COMPANION_IFACE_IN_USE";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::IP_ADDRESS_MISMATCH) {
        return "IP_ADDRESS_MISMATCH";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::IFACE_AND_POL_FAMILY_MISMATCH) {
        return "IFACE_AND_POL_FAMILY_MISMATCH";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::EMM_ACCESS_BARRED_INFINITE_RETRY) {
        return "EMM_ACCESS_BARRED_INFINITE_RETRY";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::AUTH_FAILURE_ON_EMERGENCY_CALL) {
        return "AUTH_FAILURE_ON_EMERGENCY_CALL";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::OEM_DCFAILCAUSE_1) {
        return "OEM_DCFAILCAUSE_1";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::OEM_DCFAILCAUSE_2) {
        return "OEM_DCFAILCAUSE_2";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::OEM_DCFAILCAUSE_3) {
        return "OEM_DCFAILCAUSE_3";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::OEM_DCFAILCAUSE_4) {
        return "OEM_DCFAILCAUSE_4";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::OEM_DCFAILCAUSE_5) {
        return "OEM_DCFAILCAUSE_5";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::OEM_DCFAILCAUSE_6) {
        return "OEM_DCFAILCAUSE_6";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::OEM_DCFAILCAUSE_7) {
        return "OEM_DCFAILCAUSE_7";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::OEM_DCFAILCAUSE_8) {
        return "OEM_DCFAILCAUSE_8";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::OEM_DCFAILCAUSE_9) {
        return "OEM_DCFAILCAUSE_9";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::OEM_DCFAILCAUSE_10) {
        return "OEM_DCFAILCAUSE_10";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::OEM_DCFAILCAUSE_11) {
        return "OEM_DCFAILCAUSE_11";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::OEM_DCFAILCAUSE_12) {
        return "OEM_DCFAILCAUSE_12";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::OEM_DCFAILCAUSE_13) {
        return "OEM_DCFAILCAUSE_13";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::OEM_DCFAILCAUSE_14) {
        return "OEM_DCFAILCAUSE_14";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::OEM_DCFAILCAUSE_15) {
        return "OEM_DCFAILCAUSE_15";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::VOICE_REGISTRATION_FAIL) {
        return "VOICE_REGISTRATION_FAIL";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::DATA_REGISTRATION_FAIL) {
        return "DATA_REGISTRATION_FAIL";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::SIGNAL_LOST) {
        return "SIGNAL_LOST";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::PREF_RADIO_TECH_CHANGED) {
        return "PREF_RADIO_TECH_CHANGED";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::RADIO_POWER_OFF) {
        return "RADIO_POWER_OFF";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::TETHERED_CALL_ACTIVE) {
        return "TETHERED_CALL_ACTIVE";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::ERROR_UNSPECIFIED) {
        return "ERROR_UNSPECIFIED";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::LLC_SNDCP) {
        return "LLC_SNDCP";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::ACTIVATION_REJECTED_BCM_VIOLATION) {
        return "ACTIVATION_REJECTED_BCM_VIOLATION";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::COLLISION_WITH_NETWORK_INITIATED_REQUEST) {
        return "COLLISION_WITH_NETWORK_INITIATED_REQUEST";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::ONLY_IPV4V6_ALLOWED) {
        return "ONLY_IPV4V6_ALLOWED";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::ONLY_NON_IP_ALLOWED) {
        return "ONLY_NON_IP_ALLOWED";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::UNSUPPORTED_QCI_VALUE) {
        return "UNSUPPORTED_QCI_VALUE";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::BEARER_HANDLING_NOT_SUPPORTED) {
        return "BEARER_HANDLING_NOT_SUPPORTED";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::INVALID_DNS_ADDR) {
        return "INVALID_DNS_ADDR";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::INVALID_PCSCF_OR_DNS_ADDRESS) {
        return "INVALID_PCSCF_OR_DNS_ADDRESS";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::CALL_PREEMPT_BY_EMERGENCY_APN) {
        return "CALL_PREEMPT_BY_EMERGENCY_APN";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::UE_INITIATED_DETACH_OR_DISCONNECT) {
        return "UE_INITIATED_DETACH_OR_DISCONNECT";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::MIP_FA_REASON_UNSPECIFIED) {
        return "MIP_FA_REASON_UNSPECIFIED";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::MIP_FA_ADMIN_PROHIBITED) {
        return "MIP_FA_ADMIN_PROHIBITED";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::MIP_FA_INSUFFICIENT_RESOURCES) {
        return "MIP_FA_INSUFFICIENT_RESOURCES";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::MIP_FA_MOBILE_NODE_AUTHENTICATION_FAILURE) {
        return "MIP_FA_MOBILE_NODE_AUTHENTICATION_FAILURE";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::MIP_FA_HOME_AGENT_AUTHENTICATION_FAILURE) {
        return "MIP_FA_HOME_AGENT_AUTHENTICATION_FAILURE";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::MIP_FA_REQUESTED_LIFETIME_TOO_LONG) {
        return "MIP_FA_REQUESTED_LIFETIME_TOO_LONG";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::MIP_FA_MALFORMED_REQUEST) {
        return "MIP_FA_MALFORMED_REQUEST";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::MIP_FA_MALFORMED_REPLY) {
        return "MIP_FA_MALFORMED_REPLY";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::MIP_FA_ENCAPSULATION_UNAVAILABLE) {
        return "MIP_FA_ENCAPSULATION_UNAVAILABLE";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::MIP_FA_VJ_HEADER_COMPRESSION_UNAVAILABLE) {
        return "MIP_FA_VJ_HEADER_COMPRESSION_UNAVAILABLE";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::MIP_FA_REVERSE_TUNNEL_UNAVAILABLE) {
        return "MIP_FA_REVERSE_TUNNEL_UNAVAILABLE";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::MIP_FA_REVERSE_TUNNEL_IS_MANDATORY) {
        return "MIP_FA_REVERSE_TUNNEL_IS_MANDATORY";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::MIP_FA_DELIVERY_STYLE_NOT_SUPPORTED) {
        return "MIP_FA_DELIVERY_STYLE_NOT_SUPPORTED";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::MIP_FA_MISSING_NAI) {
        return "MIP_FA_MISSING_NAI";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::MIP_FA_MISSING_HOME_AGENT) {
        return "MIP_FA_MISSING_HOME_AGENT";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::MIP_FA_MISSING_HOME_ADDRESS) {
        return "MIP_FA_MISSING_HOME_ADDRESS";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::MIP_FA_UNKNOWN_CHALLENGE) {
        return "MIP_FA_UNKNOWN_CHALLENGE";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::MIP_FA_MISSING_CHALLENGE) {
        return "MIP_FA_MISSING_CHALLENGE";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::MIP_FA_STALE_CHALLENGE) {
        return "MIP_FA_STALE_CHALLENGE";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::MIP_HA_REASON_UNSPECIFIED) {
        return "MIP_HA_REASON_UNSPECIFIED";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::MIP_HA_ADMIN_PROHIBITED) {
        return "MIP_HA_ADMIN_PROHIBITED";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::MIP_HA_INSUFFICIENT_RESOURCES) {
        return "MIP_HA_INSUFFICIENT_RESOURCES";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::MIP_HA_MOBILE_NODE_AUTHENTICATION_FAILURE) {
        return "MIP_HA_MOBILE_NODE_AUTHENTICATION_FAILURE";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::MIP_HA_FOREIGN_AGENT_AUTHENTICATION_FAILURE) {
        return "MIP_HA_FOREIGN_AGENT_AUTHENTICATION_FAILURE";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::MIP_HA_REGISTRATION_ID_MISMATCH) {
        return "MIP_HA_REGISTRATION_ID_MISMATCH";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::MIP_HA_MALFORMED_REQUEST) {
        return "MIP_HA_MALFORMED_REQUEST";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::MIP_HA_UNKNOWN_HOME_AGENT_ADDRESS) {
        return "MIP_HA_UNKNOWN_HOME_AGENT_ADDRESS";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::MIP_HA_REVERSE_TUNNEL_UNAVAILABLE) {
        return "MIP_HA_REVERSE_TUNNEL_UNAVAILABLE";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::MIP_HA_REVERSE_TUNNEL_IS_MANDATORY) {
        return "MIP_HA_REVERSE_TUNNEL_IS_MANDATORY";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::MIP_HA_ENCAPSULATION_UNAVAILABLE) {
        return "MIP_HA_ENCAPSULATION_UNAVAILABLE";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::CLOSE_IN_PROGRESS) {
        return "CLOSE_IN_PROGRESS";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::NETWORK_INITIATED_TERMINATION) {
        return "NETWORK_INITIATED_TERMINATION";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::MODEM_APP_PREEMPTED) {
        return "MODEM_APP_PREEMPTED";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::PDN_IPV4_CALL_DISALLOWED) {
        return "PDN_IPV4_CALL_DISALLOWED";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::PDN_IPV4_CALL_THROTTLED) {
        return "PDN_IPV4_CALL_THROTTLED";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::PDN_IPV6_CALL_DISALLOWED) {
        return "PDN_IPV6_CALL_DISALLOWED";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::PDN_IPV6_CALL_THROTTLED) {
        return "PDN_IPV6_CALL_THROTTLED";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::MODEM_RESTART) {
        return "MODEM_RESTART";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::PDP_PPP_NOT_SUPPORTED) {
        return "PDP_PPP_NOT_SUPPORTED";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::UNPREFERRED_RAT) {
        return "UNPREFERRED_RAT";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::PHYSICAL_LINK_CLOSE_IN_PROGRESS) {
        return "PHYSICAL_LINK_CLOSE_IN_PROGRESS";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::APN_PENDING_HANDOVER) {
        return "APN_PENDING_HANDOVER";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::PROFILE_BEARER_INCOMPATIBLE) {
        return "PROFILE_BEARER_INCOMPATIBLE";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::SIM_CARD_CHANGED) {
        return "SIM_CARD_CHANGED";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::LOW_POWER_MODE_OR_POWERING_DOWN) {
        return "LOW_POWER_MODE_OR_POWERING_DOWN";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::APN_DISABLED) {
        return "APN_DISABLED";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::MAX_PPP_INACTIVITY_TIMER_EXPIRED) {
        return "MAX_PPP_INACTIVITY_TIMER_EXPIRED";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::IPV6_ADDRESS_TRANSFER_FAILED) {
        return "IPV6_ADDRESS_TRANSFER_FAILED";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::TRAT_SWAP_FAILED) {
        return "TRAT_SWAP_FAILED";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::EHRPD_TO_HRPD_FALLBACK) {
        return "EHRPD_TO_HRPD_FALLBACK";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::MIP_CONFIG_FAILURE) {
        return "MIP_CONFIG_FAILURE";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::PDN_INACTIVITY_TIMER_EXPIRED) {
        return "PDN_INACTIVITY_TIMER_EXPIRED";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::MAX_IPV4_CONNECTIONS) {
        return "MAX_IPV4_CONNECTIONS";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::MAX_IPV6_CONNECTIONS) {
        return "MAX_IPV6_CONNECTIONS";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::APN_MISMATCH) {
        return "APN_MISMATCH";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::IP_VERSION_MISMATCH) {
        return "IP_VERSION_MISMATCH";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::DUN_CALL_DISALLOWED) {
        return "DUN_CALL_DISALLOWED";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::INTERNAL_EPC_NONEPC_TRANSITION) {
        return "INTERNAL_EPC_NONEPC_TRANSITION";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::INTERFACE_IN_USE) {
        return "INTERFACE_IN_USE";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::APN_DISALLOWED_ON_ROAMING) {
        return "APN_DISALLOWED_ON_ROAMING";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::APN_PARAMETERS_CHANGED) {
        return "APN_PARAMETERS_CHANGED";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::NULL_APN_DISALLOWED) {
        return "NULL_APN_DISALLOWED";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::THERMAL_MITIGATION) {
        return "THERMAL_MITIGATION";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::DATA_SETTINGS_DISABLED) {
        return "DATA_SETTINGS_DISABLED";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::DATA_ROAMING_SETTINGS_DISABLED) {
        return "DATA_ROAMING_SETTINGS_DISABLED";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::DDS_SWITCHED) {
        return "DDS_SWITCHED";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::FORBIDDEN_APN_NAME) {
        return "FORBIDDEN_APN_NAME";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::DDS_SWITCH_IN_PROGRESS) {
        return "DDS_SWITCH_IN_PROGRESS";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::CALL_DISALLOWED_IN_ROAMING) {
        return "CALL_DISALLOWED_IN_ROAMING";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::NON_IP_NOT_SUPPORTED) {
        return "NON_IP_NOT_SUPPORTED";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::PDN_NON_IP_CALL_THROTTLED) {
        return "PDN_NON_IP_CALL_THROTTLED";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::PDN_NON_IP_CALL_DISALLOWED) {
        return "PDN_NON_IP_CALL_DISALLOWED";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::CDMA_LOCK) {
        return "CDMA_LOCK";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::CDMA_INTERCEPT) {
        return "CDMA_INTERCEPT";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::CDMA_REORDER) {
        return "CDMA_REORDER";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::CDMA_RELEASE_DUE_TO_SO_REJECTION) {
        return "CDMA_RELEASE_DUE_TO_SO_REJECTION";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::CDMA_INCOMING_CALL) {
        return "CDMA_INCOMING_CALL";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::CDMA_ALERT_STOP) {
        return "CDMA_ALERT_STOP";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::CHANNEL_ACQUISITION_FAILURE) {
        return "CHANNEL_ACQUISITION_FAILURE";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::MAX_ACCESS_PROBE) {
        return "MAX_ACCESS_PROBE";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::CONCURRENT_SERVICE_NOT_SUPPORTED_BY_BASE_STATION) {
        return "CONCURRENT_SERVICE_NOT_SUPPORTED_BY_BASE_STATION";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::NO_RESPONSE_FROM_BASE_STATION) {
        return "NO_RESPONSE_FROM_BASE_STATION";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::REJECTED_BY_BASE_STATION) {
        return "REJECTED_BY_BASE_STATION";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::CONCURRENT_SERVICES_INCOMPATIBLE) {
        return "CONCURRENT_SERVICES_INCOMPATIBLE";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::NO_CDMA_SERVICE) {
        return "NO_CDMA_SERVICE";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::RUIM_NOT_PRESENT) {
        return "RUIM_NOT_PRESENT";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::CDMA_RETRY_ORDER) {
        return "CDMA_RETRY_ORDER";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::ACCESS_BLOCK) {
        return "ACCESS_BLOCK";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::ACCESS_BLOCK_ALL) {
        return "ACCESS_BLOCK_ALL";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::IS707B_MAX_ACCESS_PROBES) {
        return "IS707B_MAX_ACCESS_PROBES";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::THERMAL_EMERGENCY) {
        return "THERMAL_EMERGENCY";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::CONCURRENT_SERVICES_NOT_ALLOWED) {
        return "CONCURRENT_SERVICES_NOT_ALLOWED";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::INCOMING_CALL_REJECTED) {
        return "INCOMING_CALL_REJECTED";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::NO_SERVICE_ON_GATEWAY) {
        return "NO_SERVICE_ON_GATEWAY";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::NO_GPRS_CONTEXT) {
        return "NO_GPRS_CONTEXT";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::ILLEGAL_MS) {
        return "ILLEGAL_MS";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::ILLEGAL_ME) {
        return "ILLEGAL_ME";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::GPRS_SERVICES_AND_NON_GPRS_SERVICES_NOT_ALLOWED) {
        return "GPRS_SERVICES_AND_NON_GPRS_SERVICES_NOT_ALLOWED";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::GPRS_SERVICES_NOT_ALLOWED) {
        return "GPRS_SERVICES_NOT_ALLOWED";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::MS_IDENTITY_CANNOT_BE_DERIVED_BY_THE_NETWORK) {
        return "MS_IDENTITY_CANNOT_BE_DERIVED_BY_THE_NETWORK";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::IMPLICITLY_DETACHED) {
        return "IMPLICITLY_DETACHED";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::PLMN_NOT_ALLOWED) {
        return "PLMN_NOT_ALLOWED";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::LOCATION_AREA_NOT_ALLOWED) {
        return "LOCATION_AREA_NOT_ALLOWED";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::GPRS_SERVICES_NOT_ALLOWED_IN_THIS_PLMN) {
        return "GPRS_SERVICES_NOT_ALLOWED_IN_THIS_PLMN";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::PDP_DUPLICATE) {
        return "PDP_DUPLICATE";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::UE_RAT_CHANGE) {
        return "UE_RAT_CHANGE";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::CONGESTION) {
        return "CONGESTION";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::NO_PDP_CONTEXT_ACTIVATED) {
        return "NO_PDP_CONTEXT_ACTIVATED";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::ACCESS_CLASS_DSAC_REJECTION) {
        return "ACCESS_CLASS_DSAC_REJECTION";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::PDP_ACTIVATE_MAX_RETRY_FAILED) {
        return "PDP_ACTIVATE_MAX_RETRY_FAILED";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::RADIO_ACCESS_BEARER_FAILURE) {
        return "RADIO_ACCESS_BEARER_FAILURE";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::ESM_UNKNOWN_EPS_BEARER_CONTEXT) {
        return "ESM_UNKNOWN_EPS_BEARER_CONTEXT";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::DRB_RELEASED_BY_RRC) {
        return "DRB_RELEASED_BY_RRC";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::CONNECTION_RELEASED) {
        return "CONNECTION_RELEASED";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::EMM_DETACHED) {
        return "EMM_DETACHED";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::EMM_ATTACH_FAILED) {
        return "EMM_ATTACH_FAILED";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::EMM_ATTACH_STARTED) {
        return "EMM_ATTACH_STARTED";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::LTE_NAS_SERVICE_REQUEST_FAILED) {
        return "LTE_NAS_SERVICE_REQUEST_FAILED";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::DUPLICATE_BEARER_ID) {
        return "DUPLICATE_BEARER_ID";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::ESM_COLLISION_SCENARIOS) {
        return "ESM_COLLISION_SCENARIOS";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::ESM_BEARER_DEACTIVATED_TO_SYNC_WITH_NETWORK) {
        return "ESM_BEARER_DEACTIVATED_TO_SYNC_WITH_NETWORK";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::ESM_NW_ACTIVATED_DED_BEARER_WITH_ID_OF_DEF_BEARER) {
        return "ESM_NW_ACTIVATED_DED_BEARER_WITH_ID_OF_DEF_BEARER";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::ESM_BAD_OTA_MESSAGE) {
        return "ESM_BAD_OTA_MESSAGE";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::ESM_DOWNLOAD_SERVER_REJECTED_THE_CALL) {
        return "ESM_DOWNLOAD_SERVER_REJECTED_THE_CALL";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::ESM_CONTEXT_TRANSFERRED_DUE_TO_IRAT) {
        return "ESM_CONTEXT_TRANSFERRED_DUE_TO_IRAT";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::DS_EXPLICIT_DEACTIVATION) {
        return "DS_EXPLICIT_DEACTIVATION";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::ESM_LOCAL_CAUSE_NONE) {
        return "ESM_LOCAL_CAUSE_NONE";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::LTE_THROTTLING_NOT_REQUIRED) {
        return "LTE_THROTTLING_NOT_REQUIRED";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::ACCESS_CONTROL_LIST_CHECK_FAILURE) {
        return "ACCESS_CONTROL_LIST_CHECK_FAILURE";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::SERVICE_NOT_ALLOWED_ON_PLMN) {
        return "SERVICE_NOT_ALLOWED_ON_PLMN";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::EMM_T3417_EXPIRED) {
        return "EMM_T3417_EXPIRED";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::EMM_T3417_EXT_EXPIRED) {
        return "EMM_T3417_EXT_EXPIRED";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::RRC_UPLINK_DATA_TRANSMISSION_FAILURE) {
        return "RRC_UPLINK_DATA_TRANSMISSION_FAILURE";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::RRC_UPLINK_DELIVERY_FAILED_DUE_TO_HANDOVER) {
        return "RRC_UPLINK_DELIVERY_FAILED_DUE_TO_HANDOVER";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::RRC_UPLINK_CONNECTION_RELEASE) {
        return "RRC_UPLINK_CONNECTION_RELEASE";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::RRC_UPLINK_RADIO_LINK_FAILURE) {
        return "RRC_UPLINK_RADIO_LINK_FAILURE";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::RRC_UPLINK_ERROR_REQUEST_FROM_NAS) {
        return "RRC_UPLINK_ERROR_REQUEST_FROM_NAS";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::RRC_CONNECTION_ACCESS_STRATUM_FAILURE) {
        return "RRC_CONNECTION_ACCESS_STRATUM_FAILURE";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::RRC_CONNECTION_ANOTHER_PROCEDURE_IN_PROGRESS) {
        return "RRC_CONNECTION_ANOTHER_PROCEDURE_IN_PROGRESS";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::RRC_CONNECTION_ACCESS_BARRED) {
        return "RRC_CONNECTION_ACCESS_BARRED";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::RRC_CONNECTION_CELL_RESELECTION) {
        return "RRC_CONNECTION_CELL_RESELECTION";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::RRC_CONNECTION_CONFIG_FAILURE) {
        return "RRC_CONNECTION_CONFIG_FAILURE";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::RRC_CONNECTION_TIMER_EXPIRED) {
        return "RRC_CONNECTION_TIMER_EXPIRED";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::RRC_CONNECTION_LINK_FAILURE) {
        return "RRC_CONNECTION_LINK_FAILURE";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::RRC_CONNECTION_CELL_NOT_CAMPED) {
        return "RRC_CONNECTION_CELL_NOT_CAMPED";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::RRC_CONNECTION_SYSTEM_INTERVAL_FAILURE) {
        return "RRC_CONNECTION_SYSTEM_INTERVAL_FAILURE";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::RRC_CONNECTION_REJECT_BY_NETWORK) {
        return "RRC_CONNECTION_REJECT_BY_NETWORK";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::RRC_CONNECTION_NORMAL_RELEASE) {
        return "RRC_CONNECTION_NORMAL_RELEASE";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::RRC_CONNECTION_RADIO_LINK_FAILURE) {
        return "RRC_CONNECTION_RADIO_LINK_FAILURE";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::RRC_CONNECTION_REESTABLISHMENT_FAILURE) {
        return "RRC_CONNECTION_REESTABLISHMENT_FAILURE";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::RRC_CONNECTION_OUT_OF_SERVICE_DURING_CELL_REGISTER) {
        return "RRC_CONNECTION_OUT_OF_SERVICE_DURING_CELL_REGISTER";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::RRC_CONNECTION_ABORT_REQUEST) {
        return "RRC_CONNECTION_ABORT_REQUEST";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::RRC_CONNECTION_SYSTEM_INFORMATION_BLOCK_READ_ERROR) {
        return "RRC_CONNECTION_SYSTEM_INFORMATION_BLOCK_READ_ERROR";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::NETWORK_INITIATED_DETACH_WITH_AUTO_REATTACH) {
        return "NETWORK_INITIATED_DETACH_WITH_AUTO_REATTACH";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::NETWORK_INITIATED_DETACH_NO_AUTO_REATTACH) {
        return "NETWORK_INITIATED_DETACH_NO_AUTO_REATTACH";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::ESM_PROCEDURE_TIME_OUT) {
        return "ESM_PROCEDURE_TIME_OUT";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::INVALID_CONNECTION_ID) {
        return "INVALID_CONNECTION_ID";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::MAXIMIUM_NSAPIS_EXCEEDED) {
        return "MAXIMIUM_NSAPIS_EXCEEDED";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::INVALID_PRIMARY_NSAPI) {
        return "INVALID_PRIMARY_NSAPI";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::CANNOT_ENCODE_OTA_MESSAGE) {
        return "CANNOT_ENCODE_OTA_MESSAGE";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::RADIO_ACCESS_BEARER_SETUP_FAILURE) {
        return "RADIO_ACCESS_BEARER_SETUP_FAILURE";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::PDP_ESTABLISH_TIMEOUT_EXPIRED) {
        return "PDP_ESTABLISH_TIMEOUT_EXPIRED";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::PDP_MODIFY_TIMEOUT_EXPIRED) {
        return "PDP_MODIFY_TIMEOUT_EXPIRED";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::PDP_INACTIVE_TIMEOUT_EXPIRED) {
        return "PDP_INACTIVE_TIMEOUT_EXPIRED";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::PDP_LOWERLAYER_ERROR) {
        return "PDP_LOWERLAYER_ERROR";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::PDP_MODIFY_COLLISION) {
        return "PDP_MODIFY_COLLISION";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::MAXINUM_SIZE_OF_L2_MESSAGE_EXCEEDED) {
        return "MAXINUM_SIZE_OF_L2_MESSAGE_EXCEEDED";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::NAS_REQUEST_REJECTED_BY_NETWORK) {
        return "NAS_REQUEST_REJECTED_BY_NETWORK";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::RRC_CONNECTION_INVALID_REQUEST) {
        return "RRC_CONNECTION_INVALID_REQUEST";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::RRC_CONNECTION_TRACKING_AREA_ID_CHANGED) {
        return "RRC_CONNECTION_TRACKING_AREA_ID_CHANGED";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::RRC_CONNECTION_RF_UNAVAILABLE) {
        return "RRC_CONNECTION_RF_UNAVAILABLE";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::RRC_CONNECTION_ABORTED_DUE_TO_IRAT_CHANGE) {
        return "RRC_CONNECTION_ABORTED_DUE_TO_IRAT_CHANGE";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::RRC_CONNECTION_RELEASED_SECURITY_NOT_ACTIVE) {
        return "RRC_CONNECTION_RELEASED_SECURITY_NOT_ACTIVE";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::RRC_CONNECTION_ABORTED_AFTER_HANDOVER) {
        return "RRC_CONNECTION_ABORTED_AFTER_HANDOVER";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::RRC_CONNECTION_ABORTED_AFTER_IRAT_CELL_CHANGE) {
        return "RRC_CONNECTION_ABORTED_AFTER_IRAT_CELL_CHANGE";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::RRC_CONNECTION_ABORTED_DURING_IRAT_CELL_CHANGE) {
        return "RRC_CONNECTION_ABORTED_DURING_IRAT_CELL_CHANGE";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::IMSI_UNKNOWN_IN_HOME_SUBSCRIBER_SERVER) {
        return "IMSI_UNKNOWN_IN_HOME_SUBSCRIBER_SERVER";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::IMEI_NOT_ACCEPTED) {
        return "IMEI_NOT_ACCEPTED";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::EPS_SERVICES_AND_NON_EPS_SERVICES_NOT_ALLOWED) {
        return "EPS_SERVICES_AND_NON_EPS_SERVICES_NOT_ALLOWED";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::EPS_SERVICES_NOT_ALLOWED_IN_PLMN) {
        return "EPS_SERVICES_NOT_ALLOWED_IN_PLMN";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::MSC_TEMPORARILY_NOT_REACHABLE) {
        return "MSC_TEMPORARILY_NOT_REACHABLE";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::CS_DOMAIN_NOT_AVAILABLE) {
        return "CS_DOMAIN_NOT_AVAILABLE";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::ESM_FAILURE) {
        return "ESM_FAILURE";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::MAC_FAILURE) {
        return "MAC_FAILURE";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::SYNCHRONIZATION_FAILURE) {
        return "SYNCHRONIZATION_FAILURE";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::UE_SECURITY_CAPABILITIES_MISMATCH) {
        return "UE_SECURITY_CAPABILITIES_MISMATCH";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::SECURITY_MODE_REJECTED) {
        return "SECURITY_MODE_REJECTED";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::UNACCEPTABLE_NON_EPS_AUTHENTICATION) {
        return "UNACCEPTABLE_NON_EPS_AUTHENTICATION";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::CS_FALLBACK_CALL_ESTABLISHMENT_NOT_ALLOWED) {
        return "CS_FALLBACK_CALL_ESTABLISHMENT_NOT_ALLOWED";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::NO_EPS_BEARER_CONTEXT_ACTIVATED) {
        return "NO_EPS_BEARER_CONTEXT_ACTIVATED";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::INVALID_EMM_STATE) {
        return "INVALID_EMM_STATE";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::NAS_LAYER_FAILURE) {
        return "NAS_LAYER_FAILURE";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::MULTIPLE_PDP_CALL_NOT_ALLOWED) {
        return "MULTIPLE_PDP_CALL_NOT_ALLOWED";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::IRAT_HANDOVER_FAILED) {
        return "IRAT_HANDOVER_FAILED";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::TEST_LOOPBACK_REGULAR_DEACTIVATION) {
        return "TEST_LOOPBACK_REGULAR_DEACTIVATION";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::LOWER_LAYER_REGISTRATION_FAILURE) {
        return "LOWER_LAYER_REGISTRATION_FAILURE";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::DATA_PLAN_EXPIRED) {
        return "DATA_PLAN_EXPIRED";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::UMTS_HANDOVER_TO_IWLAN) {
        return "UMTS_HANDOVER_TO_IWLAN";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::EVDO_CONNECTION_DENY_BY_GENERAL_OR_NETWORK_BUSY) {
        return "EVDO_CONNECTION_DENY_BY_GENERAL_OR_NETWORK_BUSY";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::EVDO_CONNECTION_DENY_BY_BILLING_OR_AUTHENTICATION_FAILURE) {
        return "EVDO_CONNECTION_DENY_BY_BILLING_OR_AUTHENTICATION_FAILURE";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::EVDO_HDR_CHANGED) {
        return "EVDO_HDR_CHANGED";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::EVDO_HDR_EXITED) {
        return "EVDO_HDR_EXITED";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::EVDO_HDR_NO_SESSION) {
        return "EVDO_HDR_NO_SESSION";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::EVDO_USING_GPS_FIX_INSTEAD_OF_HDR_CALL) {
        return "EVDO_USING_GPS_FIX_INSTEAD_OF_HDR_CALL";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::EVDO_HDR_CONNECTION_SETUP_TIMEOUT) {
        return "EVDO_HDR_CONNECTION_SETUP_TIMEOUT";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::FAILED_TO_ACQUIRE_COLOCATED_HDR) {
        return "FAILED_TO_ACQUIRE_COLOCATED_HDR";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::OTASP_COMMIT_IN_PROGRESS) {
        return "OTASP_COMMIT_IN_PROGRESS";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::NO_HYBRID_HDR_SERVICE) {
        return "NO_HYBRID_HDR_SERVICE";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::HDR_NO_LOCK_GRANTED) {
        return "HDR_NO_LOCK_GRANTED";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::DBM_OR_SMS_IN_PROGRESS) {
        return "DBM_OR_SMS_IN_PROGRESS";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::HDR_FADE) {
        return "HDR_FADE";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::HDR_ACCESS_FAILURE) {
        return "HDR_ACCESS_FAILURE";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::UNSUPPORTED_1X_PREV) {
        return "UNSUPPORTED_1X_PREV";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::LOCAL_END) {
        return "LOCAL_END";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::NO_SERVICE) {
        return "NO_SERVICE";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::FADE) {
        return "FADE";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::NORMAL_RELEASE) {
        return "NORMAL_RELEASE";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::ACCESS_ATTEMPT_ALREADY_IN_PROGRESS) {
        return "ACCESS_ATTEMPT_ALREADY_IN_PROGRESS";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::REDIRECTION_OR_HANDOFF_IN_PROGRESS) {
        return "REDIRECTION_OR_HANDOFF_IN_PROGRESS";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::EMERGENCY_MODE) {
        return "EMERGENCY_MODE";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::PHONE_IN_USE) {
        return "PHONE_IN_USE";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::INVALID_MODE) {
        return "INVALID_MODE";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::INVALID_SIM_STATE) {
        return "INVALID_SIM_STATE";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::NO_COLLOCATED_HDR) {
        return "NO_COLLOCATED_HDR";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::UE_IS_ENTERING_POWERSAVE_MODE) {
        return "UE_IS_ENTERING_POWERSAVE_MODE";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::DUAL_SWITCH) {
        return "DUAL_SWITCH";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::PPP_TIMEOUT) {
        return "PPP_TIMEOUT";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::PPP_AUTH_FAILURE) {
        return "PPP_AUTH_FAILURE";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::PPP_OPTION_MISMATCH) {
        return "PPP_OPTION_MISMATCH";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::PPP_PAP_FAILURE) {
        return "PPP_PAP_FAILURE";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::PPP_CHAP_FAILURE) {
        return "PPP_CHAP_FAILURE";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::PPP_CLOSE_IN_PROGRESS) {
        return "PPP_CLOSE_IN_PROGRESS";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::LIMITED_TO_IPV4) {
        return "LIMITED_TO_IPV4";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::LIMITED_TO_IPV6) {
        return "LIMITED_TO_IPV6";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::VSNCP_TIMEOUT) {
        return "VSNCP_TIMEOUT";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::VSNCP_GEN_ERROR) {
        return "VSNCP_GEN_ERROR";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::VSNCP_APN_UNATHORIZED) {
        return "VSNCP_APN_UNATHORIZED";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::VSNCP_PDN_LIMIT_EXCEEDED) {
        return "VSNCP_PDN_LIMIT_EXCEEDED";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::VSNCP_NO_PDN_GATEWAY_ADDRESS) {
        return "VSNCP_NO_PDN_GATEWAY_ADDRESS";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::VSNCP_PDN_GATEWAY_UNREACHABLE) {
        return "VSNCP_PDN_GATEWAY_UNREACHABLE";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::VSNCP_PDN_GATEWAY_REJECT) {
        return "VSNCP_PDN_GATEWAY_REJECT";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::VSNCP_INSUFFICIENT_PARAMETERS) {
        return "VSNCP_INSUFFICIENT_PARAMETERS";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::VSNCP_RESOURCE_UNAVAILABLE) {
        return "VSNCP_RESOURCE_UNAVAILABLE";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::VSNCP_ADMINISTRATIVELY_PROHIBITED) {
        return "VSNCP_ADMINISTRATIVELY_PROHIBITED";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::VSNCP_PDN_ID_IN_USE) {
        return "VSNCP_PDN_ID_IN_USE";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::VSNCP_SUBSCRIBER_LIMITATION) {
        return "VSNCP_SUBSCRIBER_LIMITATION";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::VSNCP_PDN_EXISTS_FOR_THIS_APN) {
        return "VSNCP_PDN_EXISTS_FOR_THIS_APN";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::VSNCP_RECONNECT_NOT_ALLOWED) {
        return "VSNCP_RECONNECT_NOT_ALLOWED";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::IPV6_PREFIX_UNAVAILABLE) {
        return "IPV6_PREFIX_UNAVAILABLE";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::HANDOFF_PREFERENCE_CHANGED) {
        return "HANDOFF_PREFERENCE_CHANGED";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::SLICE_REJECTED) {
        return "SLICE_REJECTED";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::MATCH_ALL_RULE_NOT_ALLOWED) {
        return "MATCH_ALL_RULE_NOT_ALLOWED";
    }
    if (o == ::android::hardware::radio::V1_6::DataCallFailCause::ALL_MATCHING_RULES_FAILED) {
        return "ALL_MATCHING_RULES_FAILED";
    }
    std::string os;
    os += toHexString(static_cast<int32_t>(o));
    return os;
}

static inline void PrintTo(::android::hardware::radio::V1_6::DataCallFailCause o, ::std::ostream* os) {
    *os << toString(o);
}

template<>
inline std::string toString<::android::hardware::radio::V1_6::SliceServiceType>(uint8_t o) {
    using ::android::hardware::details::toHexString;
    std::string os;
    ::android::hardware::hidl_bitfield<::android::hardware::radio::V1_6::SliceServiceType> flipped = 0;
    bool first = true;
    if ((o & ::android::hardware::radio::V1_6::SliceServiceType::NONE) == static_cast<uint8_t>(::android::hardware::radio::V1_6::SliceServiceType::NONE)) {
        os += (first ? "" : " | ");
        os += "NONE";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::SliceServiceType::NONE;
    }
    if ((o & ::android::hardware::radio::V1_6::SliceServiceType::EMBB) == static_cast<uint8_t>(::android::hardware::radio::V1_6::SliceServiceType::EMBB)) {
        os += (first ? "" : " | ");
        os += "EMBB";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::SliceServiceType::EMBB;
    }
    if ((o & ::android::hardware::radio::V1_6::SliceServiceType::URLLC) == static_cast<uint8_t>(::android::hardware::radio::V1_6::SliceServiceType::URLLC)) {
        os += (first ? "" : " | ");
        os += "URLLC";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::SliceServiceType::URLLC;
    }
    if ((o & ::android::hardware::radio::V1_6::SliceServiceType::MIOT) == static_cast<uint8_t>(::android::hardware::radio::V1_6::SliceServiceType::MIOT)) {
        os += (first ? "" : " | ");
        os += "MIOT";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::SliceServiceType::MIOT;
    }
    if (o != flipped) {
        os += (first ? "" : " | ");
        os += toHexString(o & (~flipped));
    }os += " (";
    os += toHexString(o);
    os += ")";
    return os;
}

static inline std::string toString(::android::hardware::radio::V1_6::SliceServiceType o) {
    using ::android::hardware::details::toHexString;
    if (o == ::android::hardware::radio::V1_6::SliceServiceType::NONE) {
        return "NONE";
    }
    if (o == ::android::hardware::radio::V1_6::SliceServiceType::EMBB) {
        return "EMBB";
    }
    if (o == ::android::hardware::radio::V1_6::SliceServiceType::URLLC) {
        return "URLLC";
    }
    if (o == ::android::hardware::radio::V1_6::SliceServiceType::MIOT) {
        return "MIOT";
    }
    std::string os;
    os += toHexString(static_cast<uint8_t>(o));
    return os;
}

static inline void PrintTo(::android::hardware::radio::V1_6::SliceServiceType o, ::std::ostream* os) {
    *os << toString(o);
}

template<>
inline std::string toString<::android::hardware::radio::V1_6::SliceStatus>(int8_t o) {
    using ::android::hardware::details::toHexString;
    std::string os;
    ::android::hardware::hidl_bitfield<::android::hardware::radio::V1_6::SliceStatus> flipped = 0;
    bool first = true;
    if ((o & ::android::hardware::radio::V1_6::SliceStatus::UNKNOWN) == static_cast<int8_t>(::android::hardware::radio::V1_6::SliceStatus::UNKNOWN)) {
        os += (first ? "" : " | ");
        os += "UNKNOWN";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::SliceStatus::UNKNOWN;
    }
    if ((o & ::android::hardware::radio::V1_6::SliceStatus::CONFIGURED) == static_cast<int8_t>(::android::hardware::radio::V1_6::SliceStatus::CONFIGURED)) {
        os += (first ? "" : " | ");
        os += "CONFIGURED";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::SliceStatus::CONFIGURED;
    }
    if ((o & ::android::hardware::radio::V1_6::SliceStatus::ALLOWED) == static_cast<int8_t>(::android::hardware::radio::V1_6::SliceStatus::ALLOWED)) {
        os += (first ? "" : " | ");
        os += "ALLOWED";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::SliceStatus::ALLOWED;
    }
    if ((o & ::android::hardware::radio::V1_6::SliceStatus::REJECTED_NOT_AVAILABLE_IN_PLMN) == static_cast<int8_t>(::android::hardware::radio::V1_6::SliceStatus::REJECTED_NOT_AVAILABLE_IN_PLMN)) {
        os += (first ? "" : " | ");
        os += "REJECTED_NOT_AVAILABLE_IN_PLMN";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::SliceStatus::REJECTED_NOT_AVAILABLE_IN_PLMN;
    }
    if ((o & ::android::hardware::radio::V1_6::SliceStatus::REJECTED_NOT_AVAILABLE_IN_REG_AREA) == static_cast<int8_t>(::android::hardware::radio::V1_6::SliceStatus::REJECTED_NOT_AVAILABLE_IN_REG_AREA)) {
        os += (first ? "" : " | ");
        os += "REJECTED_NOT_AVAILABLE_IN_REG_AREA";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::SliceStatus::REJECTED_NOT_AVAILABLE_IN_REG_AREA;
    }
    if ((o & ::android::hardware::radio::V1_6::SliceStatus::DEFAULT_CONFIGURED) == static_cast<int8_t>(::android::hardware::radio::V1_6::SliceStatus::DEFAULT_CONFIGURED)) {
        os += (first ? "" : " | ");
        os += "DEFAULT_CONFIGURED";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::SliceStatus::DEFAULT_CONFIGURED;
    }
    if (o != flipped) {
        os += (first ? "" : " | ");
        os += toHexString(o & (~flipped));
    }os += " (";
    os += toHexString(o);
    os += ")";
    return os;
}

static inline std::string toString(::android::hardware::radio::V1_6::SliceStatus o) {
    using ::android::hardware::details::toHexString;
    if (o == ::android::hardware::radio::V1_6::SliceStatus::UNKNOWN) {
        return "UNKNOWN";
    }
    if (o == ::android::hardware::radio::V1_6::SliceStatus::CONFIGURED) {
        return "CONFIGURED";
    }
    if (o == ::android::hardware::radio::V1_6::SliceStatus::ALLOWED) {
        return "ALLOWED";
    }
    if (o == ::android::hardware::radio::V1_6::SliceStatus::REJECTED_NOT_AVAILABLE_IN_PLMN) {
        return "REJECTED_NOT_AVAILABLE_IN_PLMN";
    }
    if (o == ::android::hardware::radio::V1_6::SliceStatus::REJECTED_NOT_AVAILABLE_IN_REG_AREA) {
        return "REJECTED_NOT_AVAILABLE_IN_REG_AREA";
    }
    if (o == ::android::hardware::radio::V1_6::SliceStatus::DEFAULT_CONFIGURED) {
        return "DEFAULT_CONFIGURED";
    }
    std::string os;
    os += toHexString(static_cast<int8_t>(o));
    return os;
}

static inline void PrintTo(::android::hardware::radio::V1_6::SliceStatus o, ::std::ostream* os) {
    *os << toString(o);
}

static inline std::string toString(const ::android::hardware::radio::V1_6::SliceInfo& o) {
    using ::android::hardware::toString;
    std::string os;
    os += "{";
    os += ".sst = ";
    os += ::android::hardware::radio::V1_6::toString(o.sst);
    os += ", .sliceDifferentiator = ";
    os += ::android::hardware::toString(o.sliceDifferentiator);
    os += ", .mappedHplmnSst = ";
    os += ::android::hardware::radio::V1_6::toString(o.mappedHplmnSst);
    os += ", .mappedHplmnSD = ";
    os += ::android::hardware::toString(o.mappedHplmnSD);
    os += ", .status = ";
    os += ::android::hardware::radio::V1_6::toString(o.status);
    os += "}"; return os;
}

static inline void PrintTo(const ::android::hardware::radio::V1_6::SliceInfo& o, ::std::ostream* os) {
    *os << toString(o);
}

static inline bool operator==(const ::android::hardware::radio::V1_6::SliceInfo& lhs, const ::android::hardware::radio::V1_6::SliceInfo& rhs) {
    if (lhs.sst != rhs.sst) {
        return false;
    }
    if (lhs.sliceDifferentiator != rhs.sliceDifferentiator) {
        return false;
    }
    if (lhs.mappedHplmnSst != rhs.mappedHplmnSst) {
        return false;
    }
    if (lhs.mappedHplmnSD != rhs.mappedHplmnSD) {
        return false;
    }
    if (lhs.status != rhs.status) {
        return false;
    }
    return true;
}

static inline bool operator!=(const ::android::hardware::radio::V1_6::SliceInfo& lhs, const ::android::hardware::radio::V1_6::SliceInfo& rhs){
    return !(lhs == rhs);
}

static inline std::string toString(const ::android::hardware::radio::V1_6::OptionalSliceInfo& o) {
    using ::android::hardware::toString;
    std::string os;
    os += "{";

    switch (o.getDiscriminator()) {
        case ::android::hardware::radio::V1_6::OptionalSliceInfo::hidl_discriminator::noinit: {
            os += ".noinit = ";
            os += toString(o.noinit());
            break;
        }
        case ::android::hardware::radio::V1_6::OptionalSliceInfo::hidl_discriminator::value: {
            os += ".value = ";
            os += toString(o.value());
            break;
        }
        default: {
            ::android::hardware::details::logAlwaysFatal((
                    "Unknown union discriminator (value: " +
                    std::to_string((uint8_t) o.getDiscriminator()) + ").").c_str());
        }
    }
    os += "}"; return os;
}

static inline void PrintTo(const ::android::hardware::radio::V1_6::OptionalSliceInfo& o, ::std::ostream* os) {
    *os << toString(o);
}

static inline bool operator==(const ::android::hardware::radio::V1_6::OptionalSliceInfo& lhs, const ::android::hardware::radio::V1_6::OptionalSliceInfo& rhs) {
    if (lhs.getDiscriminator() != rhs.getDiscriminator()) {
        return false;
    }
    switch (lhs.getDiscriminator()) {
        case ::android::hardware::radio::V1_6::OptionalSliceInfo::hidl_discriminator::noinit: {
            return (lhs.noinit() == rhs.noinit());
        }
        case ::android::hardware::radio::V1_6::OptionalSliceInfo::hidl_discriminator::value: {
            return (lhs.value() == rhs.value());
        }
        default: {
            ::android::hardware::details::logAlwaysFatal((
                    "Unknown union discriminator (value: " +
                    std::to_string((uint8_t) lhs.getDiscriminator()) + ").").c_str());
        }
    }
    return true;
}

static inline bool operator!=(const ::android::hardware::radio::V1_6::OptionalSliceInfo& lhs, const ::android::hardware::radio::V1_6::OptionalSliceInfo& rhs){
    return !(lhs == rhs);
}

static inline std::string toString(const ::android::hardware::radio::V1_6::SetupDataCallResult& o) {
    using ::android::hardware::toString;
    std::string os;
    os += "{";
    os += ".cause = ";
    os += ::android::hardware::radio::V1_6::toString(o.cause);
    os += ", .suggestedRetryTime = ";
    os += ::android::hardware::toString(o.suggestedRetryTime);
    os += ", .cid = ";
    os += ::android::hardware::toString(o.cid);
    os += ", .active = ";
    os += ::android::hardware::radio::V1_4::toString(o.active);
    os += ", .type = ";
    os += ::android::hardware::radio::V1_4::toString(o.type);
    os += ", .ifname = ";
    os += ::android::hardware::toString(o.ifname);
    os += ", .addresses = ";
    os += ::android::hardware::toString(o.addresses);
    os += ", .dnses = ";
    os += ::android::hardware::toString(o.dnses);
    os += ", .gateways = ";
    os += ::android::hardware::toString(o.gateways);
    os += ", .pcscf = ";
    os += ::android::hardware::toString(o.pcscf);
    os += ", .mtuV4 = ";
    os += ::android::hardware::toString(o.mtuV4);
    os += ", .mtuV6 = ";
    os += ::android::hardware::toString(o.mtuV6);
    os += ", .defaultQos = ";
    os += ::android::hardware::radio::V1_6::toString(o.defaultQos);
    os += ", .qosSessions = ";
    os += ::android::hardware::toString(o.qosSessions);
    os += ", .handoverFailureMode = ";
    os += ::android::hardware::radio::V1_6::toString(o.handoverFailureMode);
    os += ", .pduSessionId = ";
    os += ::android::hardware::toString(o.pduSessionId);
    os += ", .sliceInfo = ";
    os += ::android::hardware::radio::V1_6::toString(o.sliceInfo);
    os += ", .trafficDescriptors = ";
    os += ::android::hardware::toString(o.trafficDescriptors);
    os += "}"; return os;
}

static inline void PrintTo(const ::android::hardware::radio::V1_6::SetupDataCallResult& o, ::std::ostream* os) {
    *os << toString(o);
}

static inline bool operator==(const ::android::hardware::radio::V1_6::SetupDataCallResult& lhs, const ::android::hardware::radio::V1_6::SetupDataCallResult& rhs) {
    if (lhs.cause != rhs.cause) {
        return false;
    }
    if (lhs.suggestedRetryTime != rhs.suggestedRetryTime) {
        return false;
    }
    if (lhs.cid != rhs.cid) {
        return false;
    }
    if (lhs.active != rhs.active) {
        return false;
    }
    if (lhs.type != rhs.type) {
        return false;
    }
    if (lhs.ifname != rhs.ifname) {
        return false;
    }
    if (lhs.addresses != rhs.addresses) {
        return false;
    }
    if (lhs.dnses != rhs.dnses) {
        return false;
    }
    if (lhs.gateways != rhs.gateways) {
        return false;
    }
    if (lhs.pcscf != rhs.pcscf) {
        return false;
    }
    if (lhs.mtuV4 != rhs.mtuV4) {
        return false;
    }
    if (lhs.mtuV6 != rhs.mtuV6) {
        return false;
    }
    if (lhs.defaultQos != rhs.defaultQos) {
        return false;
    }
    if (lhs.qosSessions != rhs.qosSessions) {
        return false;
    }
    if (lhs.handoverFailureMode != rhs.handoverFailureMode) {
        return false;
    }
    if (lhs.pduSessionId != rhs.pduSessionId) {
        return false;
    }
    if (lhs.sliceInfo != rhs.sliceInfo) {
        return false;
    }
    if (lhs.trafficDescriptors != rhs.trafficDescriptors) {
        return false;
    }
    return true;
}

static inline bool operator!=(const ::android::hardware::radio::V1_6::SetupDataCallResult& lhs, const ::android::hardware::radio::V1_6::SetupDataCallResult& rhs){
    return !(lhs == rhs);
}

template<>
inline std::string toString<::android::hardware::radio::V1_6::NrDualConnectivityState>(int8_t o) {
    using ::android::hardware::details::toHexString;
    std::string os;
    ::android::hardware::hidl_bitfield<::android::hardware::radio::V1_6::NrDualConnectivityState> flipped = 0;
    bool first = true;
    if ((o & ::android::hardware::radio::V1_6::NrDualConnectivityState::ENABLE) == static_cast<int8_t>(::android::hardware::radio::V1_6::NrDualConnectivityState::ENABLE)) {
        os += (first ? "" : " | ");
        os += "ENABLE";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::NrDualConnectivityState::ENABLE;
    }
    if ((o & ::android::hardware::radio::V1_6::NrDualConnectivityState::DISABLE) == static_cast<int8_t>(::android::hardware::radio::V1_6::NrDualConnectivityState::DISABLE)) {
        os += (first ? "" : " | ");
        os += "DISABLE";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::NrDualConnectivityState::DISABLE;
    }
    if ((o & ::android::hardware::radio::V1_6::NrDualConnectivityState::DISABLE_IMMEDIATE) == static_cast<int8_t>(::android::hardware::radio::V1_6::NrDualConnectivityState::DISABLE_IMMEDIATE)) {
        os += (first ? "" : " | ");
        os += "DISABLE_IMMEDIATE";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::NrDualConnectivityState::DISABLE_IMMEDIATE;
    }
    if (o != flipped) {
        os += (first ? "" : " | ");
        os += toHexString(o & (~flipped));
    }os += " (";
    os += toHexString(o);
    os += ")";
    return os;
}

static inline std::string toString(::android::hardware::radio::V1_6::NrDualConnectivityState o) {
    using ::android::hardware::details::toHexString;
    if (o == ::android::hardware::radio::V1_6::NrDualConnectivityState::ENABLE) {
        return "ENABLE";
    }
    if (o == ::android::hardware::radio::V1_6::NrDualConnectivityState::DISABLE) {
        return "DISABLE";
    }
    if (o == ::android::hardware::radio::V1_6::NrDualConnectivityState::DISABLE_IMMEDIATE) {
        return "DISABLE_IMMEDIATE";
    }
    std::string os;
    os += toHexString(static_cast<int8_t>(o));
    return os;
}

static inline void PrintTo(::android::hardware::radio::V1_6::NrDualConnectivityState o, ::std::ostream* os) {
    *os << toString(o);
}

static inline std::string toString(const ::android::hardware::radio::V1_6::LinkCapacityEstimate& o) {
    using ::android::hardware::toString;
    std::string os;
    os += "{";
    os += ".downlinkCapacityKbps = ";
    os += ::android::hardware::toString(o.downlinkCapacityKbps);
    os += ", .uplinkCapacityKbps = ";
    os += ::android::hardware::toString(o.uplinkCapacityKbps);
    os += ", .secondaryDownlinkCapacityKbps = ";
    os += ::android::hardware::toString(o.secondaryDownlinkCapacityKbps);
    os += ", .secondaryUplinkCapacityKbps = ";
    os += ::android::hardware::toString(o.secondaryUplinkCapacityKbps);
    os += "}"; return os;
}

static inline void PrintTo(const ::android::hardware::radio::V1_6::LinkCapacityEstimate& o, ::std::ostream* os) {
    *os << toString(o);
}

static inline bool operator==(const ::android::hardware::radio::V1_6::LinkCapacityEstimate& lhs, const ::android::hardware::radio::V1_6::LinkCapacityEstimate& rhs) {
    if (lhs.downlinkCapacityKbps != rhs.downlinkCapacityKbps) {
        return false;
    }
    if (lhs.uplinkCapacityKbps != rhs.uplinkCapacityKbps) {
        return false;
    }
    if (lhs.secondaryDownlinkCapacityKbps != rhs.secondaryDownlinkCapacityKbps) {
        return false;
    }
    if (lhs.secondaryUplinkCapacityKbps != rhs.secondaryUplinkCapacityKbps) {
        return false;
    }
    return true;
}

static inline bool operator!=(const ::android::hardware::radio::V1_6::LinkCapacityEstimate& lhs, const ::android::hardware::radio::V1_6::LinkCapacityEstimate& rhs){
    return !(lhs == rhs);
}

template<>
inline std::string toString<::android::hardware::radio::V1_6::DataThrottlingAction>(int8_t o) {
    using ::android::hardware::details::toHexString;
    std::string os;
    ::android::hardware::hidl_bitfield<::android::hardware::radio::V1_6::DataThrottlingAction> flipped = 0;
    bool first = true;
    if ((o & ::android::hardware::radio::V1_6::DataThrottlingAction::NO_DATA_THROTTLING) == static_cast<int8_t>(::android::hardware::radio::V1_6::DataThrottlingAction::NO_DATA_THROTTLING)) {
        os += (first ? "" : " | ");
        os += "NO_DATA_THROTTLING";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataThrottlingAction::NO_DATA_THROTTLING;
    }
    if ((o & ::android::hardware::radio::V1_6::DataThrottlingAction::THROTTLE_SECONDARY_CARRIER) == static_cast<int8_t>(::android::hardware::radio::V1_6::DataThrottlingAction::THROTTLE_SECONDARY_CARRIER)) {
        os += (first ? "" : " | ");
        os += "THROTTLE_SECONDARY_CARRIER";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataThrottlingAction::THROTTLE_SECONDARY_CARRIER;
    }
    if ((o & ::android::hardware::radio::V1_6::DataThrottlingAction::THROTTLE_ANCHOR_CARRIER) == static_cast<int8_t>(::android::hardware::radio::V1_6::DataThrottlingAction::THROTTLE_ANCHOR_CARRIER)) {
        os += (first ? "" : " | ");
        os += "THROTTLE_ANCHOR_CARRIER";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataThrottlingAction::THROTTLE_ANCHOR_CARRIER;
    }
    if ((o & ::android::hardware::radio::V1_6::DataThrottlingAction::HOLD) == static_cast<int8_t>(::android::hardware::radio::V1_6::DataThrottlingAction::HOLD)) {
        os += (first ? "" : " | ");
        os += "HOLD";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::DataThrottlingAction::HOLD;
    }
    if (o != flipped) {
        os += (first ? "" : " | ");
        os += toHexString(o & (~flipped));
    }os += " (";
    os += toHexString(o);
    os += ")";
    return os;
}

static inline std::string toString(::android::hardware::radio::V1_6::DataThrottlingAction o) {
    using ::android::hardware::details::toHexString;
    if (o == ::android::hardware::radio::V1_6::DataThrottlingAction::NO_DATA_THROTTLING) {
        return "NO_DATA_THROTTLING";
    }
    if (o == ::android::hardware::radio::V1_6::DataThrottlingAction::THROTTLE_SECONDARY_CARRIER) {
        return "THROTTLE_SECONDARY_CARRIER";
    }
    if (o == ::android::hardware::radio::V1_6::DataThrottlingAction::THROTTLE_ANCHOR_CARRIER) {
        return "THROTTLE_ANCHOR_CARRIER";
    }
    if (o == ::android::hardware::radio::V1_6::DataThrottlingAction::HOLD) {
        return "HOLD";
    }
    std::string os;
    os += toHexString(static_cast<int8_t>(o));
    return os;
}

static inline void PrintTo(::android::hardware::radio::V1_6::DataThrottlingAction o, ::std::ostream* os) {
    *os << toString(o);
}

template<>
inline std::string toString<::android::hardware::radio::V1_6::VopsIndicator>(uint8_t o) {
    using ::android::hardware::details::toHexString;
    std::string os;
    ::android::hardware::hidl_bitfield<::android::hardware::radio::V1_6::VopsIndicator> flipped = 0;
    bool first = true;
    if ((o & ::android::hardware::radio::V1_6::VopsIndicator::VOPS_NOT_SUPPORTED) == static_cast<uint8_t>(::android::hardware::radio::V1_6::VopsIndicator::VOPS_NOT_SUPPORTED)) {
        os += (first ? "" : " | ");
        os += "VOPS_NOT_SUPPORTED";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::VopsIndicator::VOPS_NOT_SUPPORTED;
    }
    if ((o & ::android::hardware::radio::V1_6::VopsIndicator::VOPS_OVER_3GPP) == static_cast<uint8_t>(::android::hardware::radio::V1_6::VopsIndicator::VOPS_OVER_3GPP)) {
        os += (first ? "" : " | ");
        os += "VOPS_OVER_3GPP";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::VopsIndicator::VOPS_OVER_3GPP;
    }
    if ((o & ::android::hardware::radio::V1_6::VopsIndicator::VOPS_OVER_NON_3GPP) == static_cast<uint8_t>(::android::hardware::radio::V1_6::VopsIndicator::VOPS_OVER_NON_3GPP)) {
        os += (first ? "" : " | ");
        os += "VOPS_OVER_NON_3GPP";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::VopsIndicator::VOPS_OVER_NON_3GPP;
    }
    if (o != flipped) {
        os += (first ? "" : " | ");
        os += toHexString(o & (~flipped));
    }os += " (";
    os += toHexString(o);
    os += ")";
    return os;
}

static inline std::string toString(::android::hardware::radio::V1_6::VopsIndicator o) {
    using ::android::hardware::details::toHexString;
    if (o == ::android::hardware::radio::V1_6::VopsIndicator::VOPS_NOT_SUPPORTED) {
        return "VOPS_NOT_SUPPORTED";
    }
    if (o == ::android::hardware::radio::V1_6::VopsIndicator::VOPS_OVER_3GPP) {
        return "VOPS_OVER_3GPP";
    }
    if (o == ::android::hardware::radio::V1_6::VopsIndicator::VOPS_OVER_NON_3GPP) {
        return "VOPS_OVER_NON_3GPP";
    }
    std::string os;
    os += toHexString(static_cast<uint8_t>(o));
    return os;
}

static inline void PrintTo(::android::hardware::radio::V1_6::VopsIndicator o, ::std::ostream* os) {
    *os << toString(o);
}

template<>
inline std::string toString<::android::hardware::radio::V1_6::EmcIndicator>(uint8_t o) {
    using ::android::hardware::details::toHexString;
    std::string os;
    ::android::hardware::hidl_bitfield<::android::hardware::radio::V1_6::EmcIndicator> flipped = 0;
    bool first = true;
    if ((o & ::android::hardware::radio::V1_6::EmcIndicator::EMC_NOT_SUPPORTED) == static_cast<uint8_t>(::android::hardware::radio::V1_6::EmcIndicator::EMC_NOT_SUPPORTED)) {
        os += (first ? "" : " | ");
        os += "EMC_NOT_SUPPORTED";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::EmcIndicator::EMC_NOT_SUPPORTED;
    }
    if ((o & ::android::hardware::radio::V1_6::EmcIndicator::EMC_NR_CONNECTED_TO_5GCN) == static_cast<uint8_t>(::android::hardware::radio::V1_6::EmcIndicator::EMC_NR_CONNECTED_TO_5GCN)) {
        os += (first ? "" : " | ");
        os += "EMC_NR_CONNECTED_TO_5GCN";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::EmcIndicator::EMC_NR_CONNECTED_TO_5GCN;
    }
    if ((o & ::android::hardware::radio::V1_6::EmcIndicator::EMC_EUTRA_CONNECTED_TO_5GCN) == static_cast<uint8_t>(::android::hardware::radio::V1_6::EmcIndicator::EMC_EUTRA_CONNECTED_TO_5GCN)) {
        os += (first ? "" : " | ");
        os += "EMC_EUTRA_CONNECTED_TO_5GCN";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::EmcIndicator::EMC_EUTRA_CONNECTED_TO_5GCN;
    }
    if ((o & ::android::hardware::radio::V1_6::EmcIndicator::EMC_BOTH_NR_EUTRA_CONNECTED_TO_5GCN) == static_cast<uint8_t>(::android::hardware::radio::V1_6::EmcIndicator::EMC_BOTH_NR_EUTRA_CONNECTED_TO_5GCN)) {
        os += (first ? "" : " | ");
        os += "EMC_BOTH_NR_EUTRA_CONNECTED_TO_5GCN";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::EmcIndicator::EMC_BOTH_NR_EUTRA_CONNECTED_TO_5GCN;
    }
    if (o != flipped) {
        os += (first ? "" : " | ");
        os += toHexString(o & (~flipped));
    }os += " (";
    os += toHexString(o);
    os += ")";
    return os;
}

static inline std::string toString(::android::hardware::radio::V1_6::EmcIndicator o) {
    using ::android::hardware::details::toHexString;
    if (o == ::android::hardware::radio::V1_6::EmcIndicator::EMC_NOT_SUPPORTED) {
        return "EMC_NOT_SUPPORTED";
    }
    if (o == ::android::hardware::radio::V1_6::EmcIndicator::EMC_NR_CONNECTED_TO_5GCN) {
        return "EMC_NR_CONNECTED_TO_5GCN";
    }
    if (o == ::android::hardware::radio::V1_6::EmcIndicator::EMC_EUTRA_CONNECTED_TO_5GCN) {
        return "EMC_EUTRA_CONNECTED_TO_5GCN";
    }
    if (o == ::android::hardware::radio::V1_6::EmcIndicator::EMC_BOTH_NR_EUTRA_CONNECTED_TO_5GCN) {
        return "EMC_BOTH_NR_EUTRA_CONNECTED_TO_5GCN";
    }
    std::string os;
    os += toHexString(static_cast<uint8_t>(o));
    return os;
}

static inline void PrintTo(::android::hardware::radio::V1_6::EmcIndicator o, ::std::ostream* os) {
    *os << toString(o);
}

template<>
inline std::string toString<::android::hardware::radio::V1_6::EmfIndicator>(uint8_t o) {
    using ::android::hardware::details::toHexString;
    std::string os;
    ::android::hardware::hidl_bitfield<::android::hardware::radio::V1_6::EmfIndicator> flipped = 0;
    bool first = true;
    if ((o & ::android::hardware::radio::V1_6::EmfIndicator::EMF_NOT_SUPPORTED) == static_cast<uint8_t>(::android::hardware::radio::V1_6::EmfIndicator::EMF_NOT_SUPPORTED)) {
        os += (first ? "" : " | ");
        os += "EMF_NOT_SUPPORTED";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::EmfIndicator::EMF_NOT_SUPPORTED;
    }
    if ((o & ::android::hardware::radio::V1_6::EmfIndicator::EMF_NR_CONNECTED_TO_5GCN) == static_cast<uint8_t>(::android::hardware::radio::V1_6::EmfIndicator::EMF_NR_CONNECTED_TO_5GCN)) {
        os += (first ? "" : " | ");
        os += "EMF_NR_CONNECTED_TO_5GCN";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::EmfIndicator::EMF_NR_CONNECTED_TO_5GCN;
    }
    if ((o & ::android::hardware::radio::V1_6::EmfIndicator::EMF_EUTRA_CONNECTED_TO_5GCN) == static_cast<uint8_t>(::android::hardware::radio::V1_6::EmfIndicator::EMF_EUTRA_CONNECTED_TO_5GCN)) {
        os += (first ? "" : " | ");
        os += "EMF_EUTRA_CONNECTED_TO_5GCN";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::EmfIndicator::EMF_EUTRA_CONNECTED_TO_5GCN;
    }
    if ((o & ::android::hardware::radio::V1_6::EmfIndicator::EMF_BOTH_NR_EUTRA_CONNECTED_TO_5GCN) == static_cast<uint8_t>(::android::hardware::radio::V1_6::EmfIndicator::EMF_BOTH_NR_EUTRA_CONNECTED_TO_5GCN)) {
        os += (first ? "" : " | ");
        os += "EMF_BOTH_NR_EUTRA_CONNECTED_TO_5GCN";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::EmfIndicator::EMF_BOTH_NR_EUTRA_CONNECTED_TO_5GCN;
    }
    if (o != flipped) {
        os += (first ? "" : " | ");
        os += toHexString(o & (~flipped));
    }os += " (";
    os += toHexString(o);
    os += ")";
    return os;
}

static inline std::string toString(::android::hardware::radio::V1_6::EmfIndicator o) {
    using ::android::hardware::details::toHexString;
    if (o == ::android::hardware::radio::V1_6::EmfIndicator::EMF_NOT_SUPPORTED) {
        return "EMF_NOT_SUPPORTED";
    }
    if (o == ::android::hardware::radio::V1_6::EmfIndicator::EMF_NR_CONNECTED_TO_5GCN) {
        return "EMF_NR_CONNECTED_TO_5GCN";
    }
    if (o == ::android::hardware::radio::V1_6::EmfIndicator::EMF_EUTRA_CONNECTED_TO_5GCN) {
        return "EMF_EUTRA_CONNECTED_TO_5GCN";
    }
    if (o == ::android::hardware::radio::V1_6::EmfIndicator::EMF_BOTH_NR_EUTRA_CONNECTED_TO_5GCN) {
        return "EMF_BOTH_NR_EUTRA_CONNECTED_TO_5GCN";
    }
    std::string os;
    os += toHexString(static_cast<uint8_t>(o));
    return os;
}

static inline void PrintTo(::android::hardware::radio::V1_6::EmfIndicator o, ::std::ostream* os) {
    *os << toString(o);
}

static inline std::string toString(const ::android::hardware::radio::V1_6::NrVopsInfo& o) {
    using ::android::hardware::toString;
    std::string os;
    os += "{";
    os += ".vopsSupported = ";
    os += ::android::hardware::radio::V1_6::toString(o.vopsSupported);
    os += ", .emcSupported = ";
    os += ::android::hardware::radio::V1_6::toString(o.emcSupported);
    os += ", .emfSupported = ";
    os += ::android::hardware::radio::V1_6::toString(o.emfSupported);
    os += "}"; return os;
}

static inline void PrintTo(const ::android::hardware::radio::V1_6::NrVopsInfo& o, ::std::ostream* os) {
    *os << toString(o);
}

static inline bool operator==(const ::android::hardware::radio::V1_6::NrVopsInfo& lhs, const ::android::hardware::radio::V1_6::NrVopsInfo& rhs) {
    if (lhs.vopsSupported != rhs.vopsSupported) {
        return false;
    }
    if (lhs.emcSupported != rhs.emcSupported) {
        return false;
    }
    if (lhs.emfSupported != rhs.emfSupported) {
        return false;
    }
    return true;
}

static inline bool operator!=(const ::android::hardware::radio::V1_6::NrVopsInfo& lhs, const ::android::hardware::radio::V1_6::NrVopsInfo& rhs){
    return !(lhs == rhs);
}

static inline std::string toString(const ::android::hardware::radio::V1_6::LteSignalStrength& o) {
    using ::android::hardware::toString;
    std::string os;
    os += "{";
    os += ".base = ";
    os += ::android::hardware::radio::V1_0::toString(o.base);
    os += ", .cqiTableIndex = ";
    os += ::android::hardware::toString(o.cqiTableIndex);
    os += "}"; return os;
}

static inline void PrintTo(const ::android::hardware::radio::V1_6::LteSignalStrength& o, ::std::ostream* os) {
    *os << toString(o);
}

static inline bool operator==(const ::android::hardware::radio::V1_6::LteSignalStrength& lhs, const ::android::hardware::radio::V1_6::LteSignalStrength& rhs) {
    if (lhs.base != rhs.base) {
        return false;
    }
    if (lhs.cqiTableIndex != rhs.cqiTableIndex) {
        return false;
    }
    return true;
}

static inline bool operator!=(const ::android::hardware::radio::V1_6::LteSignalStrength& lhs, const ::android::hardware::radio::V1_6::LteSignalStrength& rhs){
    return !(lhs == rhs);
}

static inline std::string toString(const ::android::hardware::radio::V1_6::NrSignalStrength& o) {
    using ::android::hardware::toString;
    std::string os;
    os += "{";
    os += ".base = ";
    os += ::android::hardware::radio::V1_4::toString(o.base);
    os += ", .csiCqiTableIndex = ";
    os += ::android::hardware::toString(o.csiCqiTableIndex);
    os += ", .csiCqiReport = ";
    os += ::android::hardware::toString(o.csiCqiReport);
    os += "}"; return os;
}

static inline void PrintTo(const ::android::hardware::radio::V1_6::NrSignalStrength& o, ::std::ostream* os) {
    *os << toString(o);
}

static inline bool operator==(const ::android::hardware::radio::V1_6::NrSignalStrength& lhs, const ::android::hardware::radio::V1_6::NrSignalStrength& rhs) {
    if (lhs.base != rhs.base) {
        return false;
    }
    if (lhs.csiCqiTableIndex != rhs.csiCqiTableIndex) {
        return false;
    }
    if (lhs.csiCqiReport != rhs.csiCqiReport) {
        return false;
    }
    return true;
}

static inline bool operator!=(const ::android::hardware::radio::V1_6::NrSignalStrength& lhs, const ::android::hardware::radio::V1_6::NrSignalStrength& rhs){
    return !(lhs == rhs);
}

static inline std::string toString(const ::android::hardware::radio::V1_6::SignalStrength& o) {
    using ::android::hardware::toString;
    std::string os;
    os += "{";
    os += ".gsm = ";
    os += ::android::hardware::radio::V1_0::toString(o.gsm);
    os += ", .cdma = ";
    os += ::android::hardware::radio::V1_0::toString(o.cdma);
    os += ", .evdo = ";
    os += ::android::hardware::radio::V1_0::toString(o.evdo);
    os += ", .lte = ";
    os += ::android::hardware::radio::V1_6::toString(o.lte);
    os += ", .tdscdma = ";
    os += ::android::hardware::radio::V1_2::toString(o.tdscdma);
    os += ", .wcdma = ";
    os += ::android::hardware::radio::V1_2::toString(o.wcdma);
    os += ", .nr = ";
    os += ::android::hardware::radio::V1_6::toString(o.nr);
    os += "}"; return os;
}

static inline void PrintTo(const ::android::hardware::radio::V1_6::SignalStrength& o, ::std::ostream* os) {
    *os << toString(o);
}

static inline bool operator==(const ::android::hardware::radio::V1_6::SignalStrength& lhs, const ::android::hardware::radio::V1_6::SignalStrength& rhs) {
    if (lhs.gsm != rhs.gsm) {
        return false;
    }
    if (lhs.cdma != rhs.cdma) {
        return false;
    }
    if (lhs.evdo != rhs.evdo) {
        return false;
    }
    if (lhs.lte != rhs.lte) {
        return false;
    }
    if (lhs.tdscdma != rhs.tdscdma) {
        return false;
    }
    if (lhs.wcdma != rhs.wcdma) {
        return false;
    }
    if (lhs.nr != rhs.nr) {
        return false;
    }
    return true;
}

static inline bool operator!=(const ::android::hardware::radio::V1_6::SignalStrength& lhs, const ::android::hardware::radio::V1_6::SignalStrength& rhs){
    return !(lhs == rhs);
}

static inline std::string toString(const ::android::hardware::radio::V1_6::CellInfoLte& o) {
    using ::android::hardware::toString;
    std::string os;
    os += "{";
    os += ".cellIdentityLte = ";
    os += ::android::hardware::radio::V1_5::toString(o.cellIdentityLte);
    os += ", .signalStrengthLte = ";
    os += ::android::hardware::radio::V1_6::toString(o.signalStrengthLte);
    os += "}"; return os;
}

static inline void PrintTo(const ::android::hardware::radio::V1_6::CellInfoLte& o, ::std::ostream* os) {
    *os << toString(o);
}

static inline bool operator==(const ::android::hardware::radio::V1_6::CellInfoLte& lhs, const ::android::hardware::radio::V1_6::CellInfoLte& rhs) {
    if (lhs.cellIdentityLte != rhs.cellIdentityLte) {
        return false;
    }
    if (lhs.signalStrengthLte != rhs.signalStrengthLte) {
        return false;
    }
    return true;
}

static inline bool operator!=(const ::android::hardware::radio::V1_6::CellInfoLte& lhs, const ::android::hardware::radio::V1_6::CellInfoLte& rhs){
    return !(lhs == rhs);
}

static inline std::string toString(const ::android::hardware::radio::V1_6::CellInfoNr& o) {
    using ::android::hardware::toString;
    std::string os;
    os += "{";
    os += ".cellIdentityNr = ";
    os += ::android::hardware::radio::V1_5::toString(o.cellIdentityNr);
    os += ", .signalStrengthNr = ";
    os += ::android::hardware::radio::V1_6::toString(o.signalStrengthNr);
    os += "}"; return os;
}

static inline void PrintTo(const ::android::hardware::radio::V1_6::CellInfoNr& o, ::std::ostream* os) {
    *os << toString(o);
}

static inline bool operator==(const ::android::hardware::radio::V1_6::CellInfoNr& lhs, const ::android::hardware::radio::V1_6::CellInfoNr& rhs) {
    if (lhs.cellIdentityNr != rhs.cellIdentityNr) {
        return false;
    }
    if (lhs.signalStrengthNr != rhs.signalStrengthNr) {
        return false;
    }
    return true;
}

static inline bool operator!=(const ::android::hardware::radio::V1_6::CellInfoNr& lhs, const ::android::hardware::radio::V1_6::CellInfoNr& rhs){
    return !(lhs == rhs);
}

static inline std::string toString(const ::android::hardware::radio::V1_6::CellInfo::CellInfoRatSpecificInfo& o) {
    using ::android::hardware::toString;
    std::string os;
    os += "{";

    switch (o.getDiscriminator()) {
        case ::android::hardware::radio::V1_6::CellInfo::CellInfoRatSpecificInfo::hidl_discriminator::gsm: {
            os += ".gsm = ";
            os += toString(o.gsm());
            break;
        }
        case ::android::hardware::radio::V1_6::CellInfo::CellInfoRatSpecificInfo::hidl_discriminator::wcdma: {
            os += ".wcdma = ";
            os += toString(o.wcdma());
            break;
        }
        case ::android::hardware::radio::V1_6::CellInfo::CellInfoRatSpecificInfo::hidl_discriminator::tdscdma: {
            os += ".tdscdma = ";
            os += toString(o.tdscdma());
            break;
        }
        case ::android::hardware::radio::V1_6::CellInfo::CellInfoRatSpecificInfo::hidl_discriminator::lte: {
            os += ".lte = ";
            os += toString(o.lte());
            break;
        }
        case ::android::hardware::radio::V1_6::CellInfo::CellInfoRatSpecificInfo::hidl_discriminator::nr: {
            os += ".nr = ";
            os += toString(o.nr());
            break;
        }
        case ::android::hardware::radio::V1_6::CellInfo::CellInfoRatSpecificInfo::hidl_discriminator::cdma: {
            os += ".cdma = ";
            os += toString(o.cdma());
            break;
        }
        default: {
            ::android::hardware::details::logAlwaysFatal((
                    "Unknown union discriminator (value: " +
                    std::to_string((uint8_t) o.getDiscriminator()) + ").").c_str());
        }
    }
    os += "}"; return os;
}

static inline void PrintTo(const ::android::hardware::radio::V1_6::CellInfo::CellInfoRatSpecificInfo& o, ::std::ostream* os) {
    *os << toString(o);
}

static inline bool operator==(const ::android::hardware::radio::V1_6::CellInfo::CellInfoRatSpecificInfo& lhs, const ::android::hardware::radio::V1_6::CellInfo::CellInfoRatSpecificInfo& rhs) {
    if (lhs.getDiscriminator() != rhs.getDiscriminator()) {
        return false;
    }
    switch (lhs.getDiscriminator()) {
        case ::android::hardware::radio::V1_6::CellInfo::CellInfoRatSpecificInfo::hidl_discriminator::gsm: {
            return (lhs.gsm() == rhs.gsm());
        }
        case ::android::hardware::radio::V1_6::CellInfo::CellInfoRatSpecificInfo::hidl_discriminator::wcdma: {
            return (lhs.wcdma() == rhs.wcdma());
        }
        case ::android::hardware::radio::V1_6::CellInfo::CellInfoRatSpecificInfo::hidl_discriminator::tdscdma: {
            return (lhs.tdscdma() == rhs.tdscdma());
        }
        case ::android::hardware::radio::V1_6::CellInfo::CellInfoRatSpecificInfo::hidl_discriminator::lte: {
            return (lhs.lte() == rhs.lte());
        }
        case ::android::hardware::radio::V1_6::CellInfo::CellInfoRatSpecificInfo::hidl_discriminator::nr: {
            return (lhs.nr() == rhs.nr());
        }
        case ::android::hardware::radio::V1_6::CellInfo::CellInfoRatSpecificInfo::hidl_discriminator::cdma: {
            return (lhs.cdma() == rhs.cdma());
        }
        default: {
            ::android::hardware::details::logAlwaysFatal((
                    "Unknown union discriminator (value: " +
                    std::to_string((uint8_t) lhs.getDiscriminator()) + ").").c_str());
        }
    }
    return true;
}

static inline bool operator!=(const ::android::hardware::radio::V1_6::CellInfo::CellInfoRatSpecificInfo& lhs, const ::android::hardware::radio::V1_6::CellInfo::CellInfoRatSpecificInfo& rhs){
    return !(lhs == rhs);
}

static inline std::string toString(const ::android::hardware::radio::V1_6::CellInfo& o) {
    using ::android::hardware::toString;
    std::string os;
    os += "{";
    os += ".registered = ";
    os += ::android::hardware::toString(o.registered);
    os += ", .connectionStatus = ";
    os += ::android::hardware::radio::V1_2::toString(o.connectionStatus);
    os += ", .ratSpecificInfo = ";
    os += ::android::hardware::radio::V1_6::toString(o.ratSpecificInfo);
    os += "}"; return os;
}

static inline void PrintTo(const ::android::hardware::radio::V1_6::CellInfo& o, ::std::ostream* os) {
    *os << toString(o);
}

static inline bool operator==(const ::android::hardware::radio::V1_6::CellInfo& lhs, const ::android::hardware::radio::V1_6::CellInfo& rhs) {
    if (lhs.registered != rhs.registered) {
        return false;
    }
    if (lhs.connectionStatus != rhs.connectionStatus) {
        return false;
    }
    if (lhs.ratSpecificInfo != rhs.ratSpecificInfo) {
        return false;
    }
    return true;
}

static inline bool operator!=(const ::android::hardware::radio::V1_6::CellInfo& lhs, const ::android::hardware::radio::V1_6::CellInfo& rhs){
    return !(lhs == rhs);
}

static inline std::string toString(const ::android::hardware::radio::V1_6::NetworkScanResult& o) {
    using ::android::hardware::toString;
    std::string os;
    os += "{";
    os += ".status = ";
    os += ::android::hardware::radio::V1_1::toString(o.status);
    os += ", .error = ";
    os += ::android::hardware::radio::V1_6::toString(o.error);
    os += ", .networkInfos = ";
    os += ::android::hardware::toString(o.networkInfos);
    os += "}"; return os;
}

static inline void PrintTo(const ::android::hardware::radio::V1_6::NetworkScanResult& o, ::std::ostream* os) {
    *os << toString(o);
}

static inline bool operator==(const ::android::hardware::radio::V1_6::NetworkScanResult& lhs, const ::android::hardware::radio::V1_6::NetworkScanResult& rhs) {
    if (lhs.status != rhs.status) {
        return false;
    }
    if (lhs.error != rhs.error) {
        return false;
    }
    if (lhs.networkInfos != rhs.networkInfos) {
        return false;
    }
    return true;
}

static inline bool operator!=(const ::android::hardware::radio::V1_6::NetworkScanResult& lhs, const ::android::hardware::radio::V1_6::NetworkScanResult& rhs){
    return !(lhs == rhs);
}

static inline std::string toString(const ::android::hardware::radio::V1_6::RegStateResult::AccessTechnologySpecificInfo& o) {
    using ::android::hardware::toString;
    std::string os;
    os += "{";

    switch (o.getDiscriminator()) {
        case ::android::hardware::radio::V1_6::RegStateResult::AccessTechnologySpecificInfo::hidl_discriminator::noinit: {
            os += ".noinit = ";
            os += toString(o.noinit());
            break;
        }
        case ::android::hardware::radio::V1_6::RegStateResult::AccessTechnologySpecificInfo::hidl_discriminator::cdmaInfo: {
            os += ".cdmaInfo = ";
            os += toString(o.cdmaInfo());
            break;
        }
        case ::android::hardware::radio::V1_6::RegStateResult::AccessTechnologySpecificInfo::hidl_discriminator::eutranInfo: {
            os += ".eutranInfo = ";
            os += toString(o.eutranInfo());
            break;
        }
        case ::android::hardware::radio::V1_6::RegStateResult::AccessTechnologySpecificInfo::hidl_discriminator::ngranNrVopsInfo: {
            os += ".ngranNrVopsInfo = ";
            os += toString(o.ngranNrVopsInfo());
            break;
        }
        case ::android::hardware::radio::V1_6::RegStateResult::AccessTechnologySpecificInfo::hidl_discriminator::geranDtmSupported: {
            os += ".geranDtmSupported = ";
            os += toString(o.geranDtmSupported());
            break;
        }
        default: {
            ::android::hardware::details::logAlwaysFatal((
                    "Unknown union discriminator (value: " +
                    std::to_string((uint8_t) o.getDiscriminator()) + ").").c_str());
        }
    }
    os += "}"; return os;
}

static inline void PrintTo(const ::android::hardware::radio::V1_6::RegStateResult::AccessTechnologySpecificInfo& o, ::std::ostream* os) {
    *os << toString(o);
}

static inline bool operator==(const ::android::hardware::radio::V1_6::RegStateResult::AccessTechnologySpecificInfo& lhs, const ::android::hardware::radio::V1_6::RegStateResult::AccessTechnologySpecificInfo& rhs) {
    if (lhs.getDiscriminator() != rhs.getDiscriminator()) {
        return false;
    }
    switch (lhs.getDiscriminator()) {
        case ::android::hardware::radio::V1_6::RegStateResult::AccessTechnologySpecificInfo::hidl_discriminator::noinit: {
            return (lhs.noinit() == rhs.noinit());
        }
        case ::android::hardware::radio::V1_6::RegStateResult::AccessTechnologySpecificInfo::hidl_discriminator::cdmaInfo: {
            return (lhs.cdmaInfo() == rhs.cdmaInfo());
        }
        case ::android::hardware::radio::V1_6::RegStateResult::AccessTechnologySpecificInfo::hidl_discriminator::eutranInfo: {
            return (lhs.eutranInfo() == rhs.eutranInfo());
        }
        case ::android::hardware::radio::V1_6::RegStateResult::AccessTechnologySpecificInfo::hidl_discriminator::ngranNrVopsInfo: {
            return (lhs.ngranNrVopsInfo() == rhs.ngranNrVopsInfo());
        }
        case ::android::hardware::radio::V1_6::RegStateResult::AccessTechnologySpecificInfo::hidl_discriminator::geranDtmSupported: {
            return (lhs.geranDtmSupported() == rhs.geranDtmSupported());
        }
        default: {
            ::android::hardware::details::logAlwaysFatal((
                    "Unknown union discriminator (value: " +
                    std::to_string((uint8_t) lhs.getDiscriminator()) + ").").c_str());
        }
    }
    return true;
}

static inline bool operator!=(const ::android::hardware::radio::V1_6::RegStateResult::AccessTechnologySpecificInfo& lhs, const ::android::hardware::radio::V1_6::RegStateResult::AccessTechnologySpecificInfo& rhs){
    return !(lhs == rhs);
}

static inline std::string toString(const ::android::hardware::radio::V1_6::RegStateResult& o) {
    using ::android::hardware::toString;
    std::string os;
    os += "{";
    os += ".regState = ";
    os += ::android::hardware::radio::V1_0::toString(o.regState);
    os += ", .rat = ";
    os += ::android::hardware::radio::V1_4::toString(o.rat);
    os += ", .reasonForDenial = ";
    os += ::android::hardware::radio::V1_5::toString(o.reasonForDenial);
    os += ", .cellIdentity = ";
    os += ::android::hardware::radio::V1_5::toString(o.cellIdentity);
    os += ", .registeredPlmn = ";
    os += ::android::hardware::toString(o.registeredPlmn);
    os += ", .accessTechnologySpecificInfo = ";
    os += ::android::hardware::radio::V1_6::toString(o.accessTechnologySpecificInfo);
    os += "}"; return os;
}

static inline void PrintTo(const ::android::hardware::radio::V1_6::RegStateResult& o, ::std::ostream* os) {
    *os << toString(o);
}

static inline bool operator==(const ::android::hardware::radio::V1_6::RegStateResult& lhs, const ::android::hardware::radio::V1_6::RegStateResult& rhs) {
    if (lhs.regState != rhs.regState) {
        return false;
    }
    if (lhs.rat != rhs.rat) {
        return false;
    }
    if (lhs.reasonForDenial != rhs.reasonForDenial) {
        return false;
    }
    if (lhs.cellIdentity != rhs.cellIdentity) {
        return false;
    }
    if (lhs.registeredPlmn != rhs.registeredPlmn) {
        return false;
    }
    if (lhs.accessTechnologySpecificInfo != rhs.accessTechnologySpecificInfo) {
        return false;
    }
    return true;
}

static inline bool operator!=(const ::android::hardware::radio::V1_6::RegStateResult& lhs, const ::android::hardware::radio::V1_6::RegStateResult& rhs){
    return !(lhs == rhs);
}

static inline std::string toString(const ::android::hardware::radio::V1_6::Call& o) {
    using ::android::hardware::toString;
    std::string os;
    os += "{";
    os += ".base = ";
    os += ::android::hardware::radio::V1_2::toString(o.base);
    os += ", .forwardedNumber = ";
    os += ::android::hardware::toString(o.forwardedNumber);
    os += "}"; return os;
}

static inline void PrintTo(const ::android::hardware::radio::V1_6::Call& o, ::std::ostream* os) {
    *os << toString(o);
}

static inline bool operator==(const ::android::hardware::radio::V1_6::Call& lhs, const ::android::hardware::radio::V1_6::Call& rhs) {
    if (lhs.base != rhs.base) {
        return false;
    }
    if (lhs.forwardedNumber != rhs.forwardedNumber) {
        return false;
    }
    return true;
}

static inline bool operator!=(const ::android::hardware::radio::V1_6::Call& lhs, const ::android::hardware::radio::V1_6::Call& rhs){
    return !(lhs == rhs);
}

template<>
inline std::string toString<::android::hardware::radio::V1_6::NgranBands>(int32_t o) {
    using ::android::hardware::details::toHexString;
    std::string os;
    ::android::hardware::hidl_bitfield<::android::hardware::radio::V1_6::NgranBands> flipped = 0;
    bool first = true;
    if ((o & ::android::hardware::radio::V1_6::NgranBands::BAND_1) == static_cast<int32_t>(::android::hardware::radio::V1_6::NgranBands::BAND_1)) {
        os += (first ? "" : " | ");
        os += "BAND_1";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::NgranBands::BAND_1;
    }
    if ((o & ::android::hardware::radio::V1_6::NgranBands::BAND_2) == static_cast<int32_t>(::android::hardware::radio::V1_6::NgranBands::BAND_2)) {
        os += (first ? "" : " | ");
        os += "BAND_2";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::NgranBands::BAND_2;
    }
    if ((o & ::android::hardware::radio::V1_6::NgranBands::BAND_3) == static_cast<int32_t>(::android::hardware::radio::V1_6::NgranBands::BAND_3)) {
        os += (first ? "" : " | ");
        os += "BAND_3";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::NgranBands::BAND_3;
    }
    if ((o & ::android::hardware::radio::V1_6::NgranBands::BAND_5) == static_cast<int32_t>(::android::hardware::radio::V1_6::NgranBands::BAND_5)) {
        os += (first ? "" : " | ");
        os += "BAND_5";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::NgranBands::BAND_5;
    }
    if ((o & ::android::hardware::radio::V1_6::NgranBands::BAND_7) == static_cast<int32_t>(::android::hardware::radio::V1_6::NgranBands::BAND_7)) {
        os += (first ? "" : " | ");
        os += "BAND_7";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::NgranBands::BAND_7;
    }
    if ((o & ::android::hardware::radio::V1_6::NgranBands::BAND_8) == static_cast<int32_t>(::android::hardware::radio::V1_6::NgranBands::BAND_8)) {
        os += (first ? "" : " | ");
        os += "BAND_8";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::NgranBands::BAND_8;
    }
    if ((o & ::android::hardware::radio::V1_6::NgranBands::BAND_12) == static_cast<int32_t>(::android::hardware::radio::V1_6::NgranBands::BAND_12)) {
        os += (first ? "" : " | ");
        os += "BAND_12";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::NgranBands::BAND_12;
    }
    if ((o & ::android::hardware::radio::V1_6::NgranBands::BAND_14) == static_cast<int32_t>(::android::hardware::radio::V1_6::NgranBands::BAND_14)) {
        os += (first ? "" : " | ");
        os += "BAND_14";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::NgranBands::BAND_14;
    }
    if ((o & ::android::hardware::radio::V1_6::NgranBands::BAND_18) == static_cast<int32_t>(::android::hardware::radio::V1_6::NgranBands::BAND_18)) {
        os += (first ? "" : " | ");
        os += "BAND_18";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::NgranBands::BAND_18;
    }
    if ((o & ::android::hardware::radio::V1_6::NgranBands::BAND_20) == static_cast<int32_t>(::android::hardware::radio::V1_6::NgranBands::BAND_20)) {
        os += (first ? "" : " | ");
        os += "BAND_20";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::NgranBands::BAND_20;
    }
    if ((o & ::android::hardware::radio::V1_6::NgranBands::BAND_25) == static_cast<int32_t>(::android::hardware::radio::V1_6::NgranBands::BAND_25)) {
        os += (first ? "" : " | ");
        os += "BAND_25";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::NgranBands::BAND_25;
    }
    if ((o & ::android::hardware::radio::V1_6::NgranBands::BAND_28) == static_cast<int32_t>(::android::hardware::radio::V1_6::NgranBands::BAND_28)) {
        os += (first ? "" : " | ");
        os += "BAND_28";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::NgranBands::BAND_28;
    }
    if ((o & ::android::hardware::radio::V1_6::NgranBands::BAND_29) == static_cast<int32_t>(::android::hardware::radio::V1_6::NgranBands::BAND_29)) {
        os += (first ? "" : " | ");
        os += "BAND_29";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::NgranBands::BAND_29;
    }
    if ((o & ::android::hardware::radio::V1_6::NgranBands::BAND_30) == static_cast<int32_t>(::android::hardware::radio::V1_6::NgranBands::BAND_30)) {
        os += (first ? "" : " | ");
        os += "BAND_30";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::NgranBands::BAND_30;
    }
    if ((o & ::android::hardware::radio::V1_6::NgranBands::BAND_34) == static_cast<int32_t>(::android::hardware::radio::V1_6::NgranBands::BAND_34)) {
        os += (first ? "" : " | ");
        os += "BAND_34";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::NgranBands::BAND_34;
    }
    if ((o & ::android::hardware::radio::V1_6::NgranBands::BAND_38) == static_cast<int32_t>(::android::hardware::radio::V1_6::NgranBands::BAND_38)) {
        os += (first ? "" : " | ");
        os += "BAND_38";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::NgranBands::BAND_38;
    }
    if ((o & ::android::hardware::radio::V1_6::NgranBands::BAND_39) == static_cast<int32_t>(::android::hardware::radio::V1_6::NgranBands::BAND_39)) {
        os += (first ? "" : " | ");
        os += "BAND_39";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::NgranBands::BAND_39;
    }
    if ((o & ::android::hardware::radio::V1_6::NgranBands::BAND_40) == static_cast<int32_t>(::android::hardware::radio::V1_6::NgranBands::BAND_40)) {
        os += (first ? "" : " | ");
        os += "BAND_40";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::NgranBands::BAND_40;
    }
    if ((o & ::android::hardware::radio::V1_6::NgranBands::BAND_41) == static_cast<int32_t>(::android::hardware::radio::V1_6::NgranBands::BAND_41)) {
        os += (first ? "" : " | ");
        os += "BAND_41";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::NgranBands::BAND_41;
    }
    if ((o & ::android::hardware::radio::V1_6::NgranBands::BAND_48) == static_cast<int32_t>(::android::hardware::radio::V1_6::NgranBands::BAND_48)) {
        os += (first ? "" : " | ");
        os += "BAND_48";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::NgranBands::BAND_48;
    }
    if ((o & ::android::hardware::radio::V1_6::NgranBands::BAND_50) == static_cast<int32_t>(::android::hardware::radio::V1_6::NgranBands::BAND_50)) {
        os += (first ? "" : " | ");
        os += "BAND_50";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::NgranBands::BAND_50;
    }
    if ((o & ::android::hardware::radio::V1_6::NgranBands::BAND_51) == static_cast<int32_t>(::android::hardware::radio::V1_6::NgranBands::BAND_51)) {
        os += (first ? "" : " | ");
        os += "BAND_51";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::NgranBands::BAND_51;
    }
    if ((o & ::android::hardware::radio::V1_6::NgranBands::BAND_65) == static_cast<int32_t>(::android::hardware::radio::V1_6::NgranBands::BAND_65)) {
        os += (first ? "" : " | ");
        os += "BAND_65";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::NgranBands::BAND_65;
    }
    if ((o & ::android::hardware::radio::V1_6::NgranBands::BAND_66) == static_cast<int32_t>(::android::hardware::radio::V1_6::NgranBands::BAND_66)) {
        os += (first ? "" : " | ");
        os += "BAND_66";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::NgranBands::BAND_66;
    }
    if ((o & ::android::hardware::radio::V1_6::NgranBands::BAND_70) == static_cast<int32_t>(::android::hardware::radio::V1_6::NgranBands::BAND_70)) {
        os += (first ? "" : " | ");
        os += "BAND_70";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::NgranBands::BAND_70;
    }
    if ((o & ::android::hardware::radio::V1_6::NgranBands::BAND_71) == static_cast<int32_t>(::android::hardware::radio::V1_6::NgranBands::BAND_71)) {
        os += (first ? "" : " | ");
        os += "BAND_71";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::NgranBands::BAND_71;
    }
    if ((o & ::android::hardware::radio::V1_6::NgranBands::BAND_74) == static_cast<int32_t>(::android::hardware::radio::V1_6::NgranBands::BAND_74)) {
        os += (first ? "" : " | ");
        os += "BAND_74";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::NgranBands::BAND_74;
    }
    if ((o & ::android::hardware::radio::V1_6::NgranBands::BAND_75) == static_cast<int32_t>(::android::hardware::radio::V1_6::NgranBands::BAND_75)) {
        os += (first ? "" : " | ");
        os += "BAND_75";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::NgranBands::BAND_75;
    }
    if ((o & ::android::hardware::radio::V1_6::NgranBands::BAND_76) == static_cast<int32_t>(::android::hardware::radio::V1_6::NgranBands::BAND_76)) {
        os += (first ? "" : " | ");
        os += "BAND_76";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::NgranBands::BAND_76;
    }
    if ((o & ::android::hardware::radio::V1_6::NgranBands::BAND_77) == static_cast<int32_t>(::android::hardware::radio::V1_6::NgranBands::BAND_77)) {
        os += (first ? "" : " | ");
        os += "BAND_77";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::NgranBands::BAND_77;
    }
    if ((o & ::android::hardware::radio::V1_6::NgranBands::BAND_78) == static_cast<int32_t>(::android::hardware::radio::V1_6::NgranBands::BAND_78)) {
        os += (first ? "" : " | ");
        os += "BAND_78";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::NgranBands::BAND_78;
    }
    if ((o & ::android::hardware::radio::V1_6::NgranBands::BAND_79) == static_cast<int32_t>(::android::hardware::radio::V1_6::NgranBands::BAND_79)) {
        os += (first ? "" : " | ");
        os += "BAND_79";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::NgranBands::BAND_79;
    }
    if ((o & ::android::hardware::radio::V1_6::NgranBands::BAND_80) == static_cast<int32_t>(::android::hardware::radio::V1_6::NgranBands::BAND_80)) {
        os += (first ? "" : " | ");
        os += "BAND_80";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::NgranBands::BAND_80;
    }
    if ((o & ::android::hardware::radio::V1_6::NgranBands::BAND_81) == static_cast<int32_t>(::android::hardware::radio::V1_6::NgranBands::BAND_81)) {
        os += (first ? "" : " | ");
        os += "BAND_81";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::NgranBands::BAND_81;
    }
    if ((o & ::android::hardware::radio::V1_6::NgranBands::BAND_82) == static_cast<int32_t>(::android::hardware::radio::V1_6::NgranBands::BAND_82)) {
        os += (first ? "" : " | ");
        os += "BAND_82";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::NgranBands::BAND_82;
    }
    if ((o & ::android::hardware::radio::V1_6::NgranBands::BAND_83) == static_cast<int32_t>(::android::hardware::radio::V1_6::NgranBands::BAND_83)) {
        os += (first ? "" : " | ");
        os += "BAND_83";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::NgranBands::BAND_83;
    }
    if ((o & ::android::hardware::radio::V1_6::NgranBands::BAND_84) == static_cast<int32_t>(::android::hardware::radio::V1_6::NgranBands::BAND_84)) {
        os += (first ? "" : " | ");
        os += "BAND_84";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::NgranBands::BAND_84;
    }
    if ((o & ::android::hardware::radio::V1_6::NgranBands::BAND_86) == static_cast<int32_t>(::android::hardware::radio::V1_6::NgranBands::BAND_86)) {
        os += (first ? "" : " | ");
        os += "BAND_86";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::NgranBands::BAND_86;
    }
    if ((o & ::android::hardware::radio::V1_6::NgranBands::BAND_89) == static_cast<int32_t>(::android::hardware::radio::V1_6::NgranBands::BAND_89)) {
        os += (first ? "" : " | ");
        os += "BAND_89";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::NgranBands::BAND_89;
    }
    if ((o & ::android::hardware::radio::V1_6::NgranBands::BAND_90) == static_cast<int32_t>(::android::hardware::radio::V1_6::NgranBands::BAND_90)) {
        os += (first ? "" : " | ");
        os += "BAND_90";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::NgranBands::BAND_90;
    }
    if ((o & ::android::hardware::radio::V1_6::NgranBands::BAND_91) == static_cast<int32_t>(::android::hardware::radio::V1_6::NgranBands::BAND_91)) {
        os += (first ? "" : " | ");
        os += "BAND_91";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::NgranBands::BAND_91;
    }
    if ((o & ::android::hardware::radio::V1_6::NgranBands::BAND_92) == static_cast<int32_t>(::android::hardware::radio::V1_6::NgranBands::BAND_92)) {
        os += (first ? "" : " | ");
        os += "BAND_92";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::NgranBands::BAND_92;
    }
    if ((o & ::android::hardware::radio::V1_6::NgranBands::BAND_93) == static_cast<int32_t>(::android::hardware::radio::V1_6::NgranBands::BAND_93)) {
        os += (first ? "" : " | ");
        os += "BAND_93";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::NgranBands::BAND_93;
    }
    if ((o & ::android::hardware::radio::V1_6::NgranBands::BAND_94) == static_cast<int32_t>(::android::hardware::radio::V1_6::NgranBands::BAND_94)) {
        os += (first ? "" : " | ");
        os += "BAND_94";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::NgranBands::BAND_94;
    }
    if ((o & ::android::hardware::radio::V1_6::NgranBands::BAND_95) == static_cast<int32_t>(::android::hardware::radio::V1_6::NgranBands::BAND_95)) {
        os += (first ? "" : " | ");
        os += "BAND_95";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::NgranBands::BAND_95;
    }
    if ((o & ::android::hardware::radio::V1_6::NgranBands::BAND_257) == static_cast<int32_t>(::android::hardware::radio::V1_6::NgranBands::BAND_257)) {
        os += (first ? "" : " | ");
        os += "BAND_257";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::NgranBands::BAND_257;
    }
    if ((o & ::android::hardware::radio::V1_6::NgranBands::BAND_258) == static_cast<int32_t>(::android::hardware::radio::V1_6::NgranBands::BAND_258)) {
        os += (first ? "" : " | ");
        os += "BAND_258";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::NgranBands::BAND_258;
    }
    if ((o & ::android::hardware::radio::V1_6::NgranBands::BAND_260) == static_cast<int32_t>(::android::hardware::radio::V1_6::NgranBands::BAND_260)) {
        os += (first ? "" : " | ");
        os += "BAND_260";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::NgranBands::BAND_260;
    }
    if ((o & ::android::hardware::radio::V1_6::NgranBands::BAND_261) == static_cast<int32_t>(::android::hardware::radio::V1_6::NgranBands::BAND_261)) {
        os += (first ? "" : " | ");
        os += "BAND_261";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::NgranBands::BAND_261;
    }
    if ((o & ::android::hardware::radio::V1_6::NgranBands::BAND_26) == static_cast<int32_t>(::android::hardware::radio::V1_6::NgranBands::BAND_26)) {
        os += (first ? "" : " | ");
        os += "BAND_26";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::NgranBands::BAND_26;
    }
    if ((o & ::android::hardware::radio::V1_6::NgranBands::BAND_46) == static_cast<int32_t>(::android::hardware::radio::V1_6::NgranBands::BAND_46)) {
        os += (first ? "" : " | ");
        os += "BAND_46";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::NgranBands::BAND_46;
    }
    if ((o & ::android::hardware::radio::V1_6::NgranBands::BAND_53) == static_cast<int32_t>(::android::hardware::radio::V1_6::NgranBands::BAND_53)) {
        os += (first ? "" : " | ");
        os += "BAND_53";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::NgranBands::BAND_53;
    }
    if ((o & ::android::hardware::radio::V1_6::NgranBands::BAND_96) == static_cast<int32_t>(::android::hardware::radio::V1_6::NgranBands::BAND_96)) {
        os += (first ? "" : " | ");
        os += "BAND_96";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::NgranBands::BAND_96;
    }
    if (o != flipped) {
        os += (first ? "" : " | ");
        os += toHexString(o & (~flipped));
    }os += " (";
    os += toHexString(o);
    os += ")";
    return os;
}

static inline std::string toString(::android::hardware::radio::V1_6::NgranBands o) {
    using ::android::hardware::details::toHexString;
    if (o == ::android::hardware::radio::V1_6::NgranBands::BAND_1) {
        return "BAND_1";
    }
    if (o == ::android::hardware::radio::V1_6::NgranBands::BAND_2) {
        return "BAND_2";
    }
    if (o == ::android::hardware::radio::V1_6::NgranBands::BAND_3) {
        return "BAND_3";
    }
    if (o == ::android::hardware::radio::V1_6::NgranBands::BAND_5) {
        return "BAND_5";
    }
    if (o == ::android::hardware::radio::V1_6::NgranBands::BAND_7) {
        return "BAND_7";
    }
    if (o == ::android::hardware::radio::V1_6::NgranBands::BAND_8) {
        return "BAND_8";
    }
    if (o == ::android::hardware::radio::V1_6::NgranBands::BAND_12) {
        return "BAND_12";
    }
    if (o == ::android::hardware::radio::V1_6::NgranBands::BAND_14) {
        return "BAND_14";
    }
    if (o == ::android::hardware::radio::V1_6::NgranBands::BAND_18) {
        return "BAND_18";
    }
    if (o == ::android::hardware::radio::V1_6::NgranBands::BAND_20) {
        return "BAND_20";
    }
    if (o == ::android::hardware::radio::V1_6::NgranBands::BAND_25) {
        return "BAND_25";
    }
    if (o == ::android::hardware::radio::V1_6::NgranBands::BAND_28) {
        return "BAND_28";
    }
    if (o == ::android::hardware::radio::V1_6::NgranBands::BAND_29) {
        return "BAND_29";
    }
    if (o == ::android::hardware::radio::V1_6::NgranBands::BAND_30) {
        return "BAND_30";
    }
    if (o == ::android::hardware::radio::V1_6::NgranBands::BAND_34) {
        return "BAND_34";
    }
    if (o == ::android::hardware::radio::V1_6::NgranBands::BAND_38) {
        return "BAND_38";
    }
    if (o == ::android::hardware::radio::V1_6::NgranBands::BAND_39) {
        return "BAND_39";
    }
    if (o == ::android::hardware::radio::V1_6::NgranBands::BAND_40) {
        return "BAND_40";
    }
    if (o == ::android::hardware::radio::V1_6::NgranBands::BAND_41) {
        return "BAND_41";
    }
    if (o == ::android::hardware::radio::V1_6::NgranBands::BAND_48) {
        return "BAND_48";
    }
    if (o == ::android::hardware::radio::V1_6::NgranBands::BAND_50) {
        return "BAND_50";
    }
    if (o == ::android::hardware::radio::V1_6::NgranBands::BAND_51) {
        return "BAND_51";
    }
    if (o == ::android::hardware::radio::V1_6::NgranBands::BAND_65) {
        return "BAND_65";
    }
    if (o == ::android::hardware::radio::V1_6::NgranBands::BAND_66) {
        return "BAND_66";
    }
    if (o == ::android::hardware::radio::V1_6::NgranBands::BAND_70) {
        return "BAND_70";
    }
    if (o == ::android::hardware::radio::V1_6::NgranBands::BAND_71) {
        return "BAND_71";
    }
    if (o == ::android::hardware::radio::V1_6::NgranBands::BAND_74) {
        return "BAND_74";
    }
    if (o == ::android::hardware::radio::V1_6::NgranBands::BAND_75) {
        return "BAND_75";
    }
    if (o == ::android::hardware::radio::V1_6::NgranBands::BAND_76) {
        return "BAND_76";
    }
    if (o == ::android::hardware::radio::V1_6::NgranBands::BAND_77) {
        return "BAND_77";
    }
    if (o == ::android::hardware::radio::V1_6::NgranBands::BAND_78) {
        return "BAND_78";
    }
    if (o == ::android::hardware::radio::V1_6::NgranBands::BAND_79) {
        return "BAND_79";
    }
    if (o == ::android::hardware::radio::V1_6::NgranBands::BAND_80) {
        return "BAND_80";
    }
    if (o == ::android::hardware::radio::V1_6::NgranBands::BAND_81) {
        return "BAND_81";
    }
    if (o == ::android::hardware::radio::V1_6::NgranBands::BAND_82) {
        return "BAND_82";
    }
    if (o == ::android::hardware::radio::V1_6::NgranBands::BAND_83) {
        return "BAND_83";
    }
    if (o == ::android::hardware::radio::V1_6::NgranBands::BAND_84) {
        return "BAND_84";
    }
    if (o == ::android::hardware::radio::V1_6::NgranBands::BAND_86) {
        return "BAND_86";
    }
    if (o == ::android::hardware::radio::V1_6::NgranBands::BAND_89) {
        return "BAND_89";
    }
    if (o == ::android::hardware::radio::V1_6::NgranBands::BAND_90) {
        return "BAND_90";
    }
    if (o == ::android::hardware::radio::V1_6::NgranBands::BAND_91) {
        return "BAND_91";
    }
    if (o == ::android::hardware::radio::V1_6::NgranBands::BAND_92) {
        return "BAND_92";
    }
    if (o == ::android::hardware::radio::V1_6::NgranBands::BAND_93) {
        return "BAND_93";
    }
    if (o == ::android::hardware::radio::V1_6::NgranBands::BAND_94) {
        return "BAND_94";
    }
    if (o == ::android::hardware::radio::V1_6::NgranBands::BAND_95) {
        return "BAND_95";
    }
    if (o == ::android::hardware::radio::V1_6::NgranBands::BAND_257) {
        return "BAND_257";
    }
    if (o == ::android::hardware::radio::V1_6::NgranBands::BAND_258) {
        return "BAND_258";
    }
    if (o == ::android::hardware::radio::V1_6::NgranBands::BAND_260) {
        return "BAND_260";
    }
    if (o == ::android::hardware::radio::V1_6::NgranBands::BAND_261) {
        return "BAND_261";
    }
    if (o == ::android::hardware::radio::V1_6::NgranBands::BAND_26) {
        return "BAND_26";
    }
    if (o == ::android::hardware::radio::V1_6::NgranBands::BAND_46) {
        return "BAND_46";
    }
    if (o == ::android::hardware::radio::V1_6::NgranBands::BAND_53) {
        return "BAND_53";
    }
    if (o == ::android::hardware::radio::V1_6::NgranBands::BAND_96) {
        return "BAND_96";
    }
    std::string os;
    os += toHexString(static_cast<int32_t>(o));
    return os;
}

static inline void PrintTo(::android::hardware::radio::V1_6::NgranBands o, ::std::ostream* os) {
    *os << toString(o);
}

static inline std::string toString(const ::android::hardware::radio::V1_6::PhysicalChannelConfig::Band& o) {
    using ::android::hardware::toString;
    std::string os;
    os += "{";

    switch (o.getDiscriminator()) {
        case ::android::hardware::radio::V1_6::PhysicalChannelConfig::Band::hidl_discriminator::geranBand: {
            os += ".geranBand = ";
            os += toString(o.geranBand());
            break;
        }
        case ::android::hardware::radio::V1_6::PhysicalChannelConfig::Band::hidl_discriminator::utranBand: {
            os += ".utranBand = ";
            os += toString(o.utranBand());
            break;
        }
        case ::android::hardware::radio::V1_6::PhysicalChannelConfig::Band::hidl_discriminator::eutranBand: {
            os += ".eutranBand = ";
            os += toString(o.eutranBand());
            break;
        }
        case ::android::hardware::radio::V1_6::PhysicalChannelConfig::Band::hidl_discriminator::ngranBand: {
            os += ".ngranBand = ";
            os += toString(o.ngranBand());
            break;
        }
        default: {
            ::android::hardware::details::logAlwaysFatal((
                    "Unknown union discriminator (value: " +
                    std::to_string((uint8_t) o.getDiscriminator()) + ").").c_str());
        }
    }
    os += "}"; return os;
}

static inline void PrintTo(const ::android::hardware::radio::V1_6::PhysicalChannelConfig::Band& o, ::std::ostream* os) {
    *os << toString(o);
}

static inline bool operator==(const ::android::hardware::radio::V1_6::PhysicalChannelConfig::Band& lhs, const ::android::hardware::radio::V1_6::PhysicalChannelConfig::Band& rhs) {
    if (lhs.getDiscriminator() != rhs.getDiscriminator()) {
        return false;
    }
    switch (lhs.getDiscriminator()) {
        case ::android::hardware::radio::V1_6::PhysicalChannelConfig::Band::hidl_discriminator::geranBand: {
            return (lhs.geranBand() == rhs.geranBand());
        }
        case ::android::hardware::radio::V1_6::PhysicalChannelConfig::Band::hidl_discriminator::utranBand: {
            return (lhs.utranBand() == rhs.utranBand());
        }
        case ::android::hardware::radio::V1_6::PhysicalChannelConfig::Band::hidl_discriminator::eutranBand: {
            return (lhs.eutranBand() == rhs.eutranBand());
        }
        case ::android::hardware::radio::V1_6::PhysicalChannelConfig::Band::hidl_discriminator::ngranBand: {
            return (lhs.ngranBand() == rhs.ngranBand());
        }
        default: {
            ::android::hardware::details::logAlwaysFatal((
                    "Unknown union discriminator (value: " +
                    std::to_string((uint8_t) lhs.getDiscriminator()) + ").").c_str());
        }
    }
    return true;
}

static inline bool operator!=(const ::android::hardware::radio::V1_6::PhysicalChannelConfig::Band& lhs, const ::android::hardware::radio::V1_6::PhysicalChannelConfig::Band& rhs){
    return !(lhs == rhs);
}

static inline std::string toString(const ::android::hardware::radio::V1_6::PhysicalChannelConfig& o) {
    using ::android::hardware::toString;
    std::string os;
    os += "{";
    os += ".status = ";
    os += ::android::hardware::radio::V1_2::toString(o.status);
    os += ", .rat = ";
    os += ::android::hardware::radio::V1_4::toString(o.rat);
    os += ", .downlinkChannelNumber = ";
    os += ::android::hardware::toString(o.downlinkChannelNumber);
    os += ", .uplinkChannelNumber = ";
    os += ::android::hardware::toString(o.uplinkChannelNumber);
    os += ", .cellBandwidthDownlinkKhz = ";
    os += ::android::hardware::toString(o.cellBandwidthDownlinkKhz);
    os += ", .cellBandwidthUplinkKhz = ";
    os += ::android::hardware::toString(o.cellBandwidthUplinkKhz);
    os += ", .contextIds = ";
    os += ::android::hardware::toString(o.contextIds);
    os += ", .physicalCellId = ";
    os += ::android::hardware::toString(o.physicalCellId);
    os += ", .band = ";
    os += ::android::hardware::radio::V1_6::toString(o.band);
    os += "}"; return os;
}

static inline void PrintTo(const ::android::hardware::radio::V1_6::PhysicalChannelConfig& o, ::std::ostream* os) {
    *os << toString(o);
}

static inline bool operator==(const ::android::hardware::radio::V1_6::PhysicalChannelConfig& lhs, const ::android::hardware::radio::V1_6::PhysicalChannelConfig& rhs) {
    if (lhs.status != rhs.status) {
        return false;
    }
    if (lhs.rat != rhs.rat) {
        return false;
    }
    if (lhs.downlinkChannelNumber != rhs.downlinkChannelNumber) {
        return false;
    }
    if (lhs.uplinkChannelNumber != rhs.uplinkChannelNumber) {
        return false;
    }
    if (lhs.cellBandwidthDownlinkKhz != rhs.cellBandwidthDownlinkKhz) {
        return false;
    }
    if (lhs.cellBandwidthUplinkKhz != rhs.cellBandwidthUplinkKhz) {
        return false;
    }
    if (lhs.contextIds != rhs.contextIds) {
        return false;
    }
    if (lhs.physicalCellId != rhs.physicalCellId) {
        return false;
    }
    if (lhs.band != rhs.band) {
        return false;
    }
    return true;
}

static inline bool operator!=(const ::android::hardware::radio::V1_6::PhysicalChannelConfig& lhs, const ::android::hardware::radio::V1_6::PhysicalChannelConfig& rhs){
    return !(lhs == rhs);
}

static inline std::string toString(const ::android::hardware::radio::V1_6::OptionalDnn& o) {
    using ::android::hardware::toString;
    std::string os;
    os += "{";

    switch (o.getDiscriminator()) {
        case ::android::hardware::radio::V1_6::OptionalDnn::hidl_discriminator::noinit: {
            os += ".noinit = ";
            os += toString(o.noinit());
            break;
        }
        case ::android::hardware::radio::V1_6::OptionalDnn::hidl_discriminator::value: {
            os += ".value = ";
            os += toString(o.value());
            break;
        }
        default: {
            ::android::hardware::details::logAlwaysFatal((
                    "Unknown union discriminator (value: " +
                    std::to_string((uint8_t) o.getDiscriminator()) + ").").c_str());
        }
    }
    os += "}"; return os;
}

static inline void PrintTo(const ::android::hardware::radio::V1_6::OptionalDnn& o, ::std::ostream* os) {
    *os << toString(o);
}

static inline bool operator==(const ::android::hardware::radio::V1_6::OptionalDnn& lhs, const ::android::hardware::radio::V1_6::OptionalDnn& rhs) {
    if (lhs.getDiscriminator() != rhs.getDiscriminator()) {
        return false;
    }
    switch (lhs.getDiscriminator()) {
        case ::android::hardware::radio::V1_6::OptionalDnn::hidl_discriminator::noinit: {
            return (lhs.noinit() == rhs.noinit());
        }
        case ::android::hardware::radio::V1_6::OptionalDnn::hidl_discriminator::value: {
            return (lhs.value() == rhs.value());
        }
        default: {
            ::android::hardware::details::logAlwaysFatal((
                    "Unknown union discriminator (value: " +
                    std::to_string((uint8_t) lhs.getDiscriminator()) + ").").c_str());
        }
    }
    return true;
}

static inline bool operator!=(const ::android::hardware::radio::V1_6::OptionalDnn& lhs, const ::android::hardware::radio::V1_6::OptionalDnn& rhs){
    return !(lhs == rhs);
}

static inline std::string toString(const ::android::hardware::radio::V1_6::OsAppId& o) {
    using ::android::hardware::toString;
    std::string os;
    os += "{";
    os += ".osAppId = ";
    os += ::android::hardware::toString(o.osAppId);
    os += "}"; return os;
}

static inline void PrintTo(const ::android::hardware::radio::V1_6::OsAppId& o, ::std::ostream* os) {
    *os << toString(o);
}

static inline bool operator==(const ::android::hardware::radio::V1_6::OsAppId& lhs, const ::android::hardware::radio::V1_6::OsAppId& rhs) {
    if (lhs.osAppId != rhs.osAppId) {
        return false;
    }
    return true;
}

static inline bool operator!=(const ::android::hardware::radio::V1_6::OsAppId& lhs, const ::android::hardware::radio::V1_6::OsAppId& rhs){
    return !(lhs == rhs);
}

static inline std::string toString(const ::android::hardware::radio::V1_6::OptionalOsAppId& o) {
    using ::android::hardware::toString;
    std::string os;
    os += "{";

    switch (o.getDiscriminator()) {
        case ::android::hardware::radio::V1_6::OptionalOsAppId::hidl_discriminator::noinit: {
            os += ".noinit = ";
            os += toString(o.noinit());
            break;
        }
        case ::android::hardware::radio::V1_6::OptionalOsAppId::hidl_discriminator::value: {
            os += ".value = ";
            os += toString(o.value());
            break;
        }
        default: {
            ::android::hardware::details::logAlwaysFatal((
                    "Unknown union discriminator (value: " +
                    std::to_string((uint8_t) o.getDiscriminator()) + ").").c_str());
        }
    }
    os += "}"; return os;
}

static inline void PrintTo(const ::android::hardware::radio::V1_6::OptionalOsAppId& o, ::std::ostream* os) {
    *os << toString(o);
}

static inline bool operator==(const ::android::hardware::radio::V1_6::OptionalOsAppId& lhs, const ::android::hardware::radio::V1_6::OptionalOsAppId& rhs) {
    if (lhs.getDiscriminator() != rhs.getDiscriminator()) {
        return false;
    }
    switch (lhs.getDiscriminator()) {
        case ::android::hardware::radio::V1_6::OptionalOsAppId::hidl_discriminator::noinit: {
            return (lhs.noinit() == rhs.noinit());
        }
        case ::android::hardware::radio::V1_6::OptionalOsAppId::hidl_discriminator::value: {
            return (lhs.value() == rhs.value());
        }
        default: {
            ::android::hardware::details::logAlwaysFatal((
                    "Unknown union discriminator (value: " +
                    std::to_string((uint8_t) lhs.getDiscriminator()) + ").").c_str());
        }
    }
    return true;
}

static inline bool operator!=(const ::android::hardware::radio::V1_6::OptionalOsAppId& lhs, const ::android::hardware::radio::V1_6::OptionalOsAppId& rhs){
    return !(lhs == rhs);
}

static inline std::string toString(const ::android::hardware::radio::V1_6::TrafficDescriptor& o) {
    using ::android::hardware::toString;
    std::string os;
    os += "{";
    os += ".dnn = ";
    os += ::android::hardware::radio::V1_6::toString(o.dnn);
    os += ", .osAppId = ";
    os += ::android::hardware::radio::V1_6::toString(o.osAppId);
    os += "}"; return os;
}

static inline void PrintTo(const ::android::hardware::radio::V1_6::TrafficDescriptor& o, ::std::ostream* os) {
    *os << toString(o);
}

static inline bool operator==(const ::android::hardware::radio::V1_6::TrafficDescriptor& lhs, const ::android::hardware::radio::V1_6::TrafficDescriptor& rhs) {
    if (lhs.dnn != rhs.dnn) {
        return false;
    }
    if (lhs.osAppId != rhs.osAppId) {
        return false;
    }
    return true;
}

static inline bool operator!=(const ::android::hardware::radio::V1_6::TrafficDescriptor& lhs, const ::android::hardware::radio::V1_6::TrafficDescriptor& rhs){
    return !(lhs == rhs);
}

static inline std::string toString(const ::android::hardware::radio::V1_6::OptionalTrafficDescriptor& o) {
    using ::android::hardware::toString;
    std::string os;
    os += "{";

    switch (o.getDiscriminator()) {
        case ::android::hardware::radio::V1_6::OptionalTrafficDescriptor::hidl_discriminator::noinit: {
            os += ".noinit = ";
            os += toString(o.noinit());
            break;
        }
        case ::android::hardware::radio::V1_6::OptionalTrafficDescriptor::hidl_discriminator::value: {
            os += ".value = ";
            os += toString(o.value());
            break;
        }
        default: {
            ::android::hardware::details::logAlwaysFatal((
                    "Unknown union discriminator (value: " +
                    std::to_string((uint8_t) o.getDiscriminator()) + ").").c_str());
        }
    }
    os += "}"; return os;
}

static inline void PrintTo(const ::android::hardware::radio::V1_6::OptionalTrafficDescriptor& o, ::std::ostream* os) {
    *os << toString(o);
}

static inline bool operator==(const ::android::hardware::radio::V1_6::OptionalTrafficDescriptor& lhs, const ::android::hardware::radio::V1_6::OptionalTrafficDescriptor& rhs) {
    if (lhs.getDiscriminator() != rhs.getDiscriminator()) {
        return false;
    }
    switch (lhs.getDiscriminator()) {
        case ::android::hardware::radio::V1_6::OptionalTrafficDescriptor::hidl_discriminator::noinit: {
            return (lhs.noinit() == rhs.noinit());
        }
        case ::android::hardware::radio::V1_6::OptionalTrafficDescriptor::hidl_discriminator::value: {
            return (lhs.value() == rhs.value());
        }
        default: {
            ::android::hardware::details::logAlwaysFatal((
                    "Unknown union discriminator (value: " +
                    std::to_string((uint8_t) lhs.getDiscriminator()) + ").").c_str());
        }
    }
    return true;
}

static inline bool operator!=(const ::android::hardware::radio::V1_6::OptionalTrafficDescriptor& lhs, const ::android::hardware::radio::V1_6::OptionalTrafficDescriptor& rhs){
    return !(lhs == rhs);
}

static inline std::string toString(const ::android::hardware::radio::V1_6::SlicingConfig& o) {
    using ::android::hardware::toString;
    std::string os;
    os += "{";
    os += ".urspRules = ";
    os += ::android::hardware::toString(o.urspRules);
    os += ", .sliceInfo = ";
    os += ::android::hardware::toString(o.sliceInfo);
    os += "}"; return os;
}

static inline void PrintTo(const ::android::hardware::radio::V1_6::SlicingConfig& o, ::std::ostream* os) {
    *os << toString(o);
}

static inline bool operator==(const ::android::hardware::radio::V1_6::SlicingConfig& lhs, const ::android::hardware::radio::V1_6::SlicingConfig& rhs) {
    if (lhs.urspRules != rhs.urspRules) {
        return false;
    }
    if (lhs.sliceInfo != rhs.sliceInfo) {
        return false;
    }
    return true;
}

static inline bool operator!=(const ::android::hardware::radio::V1_6::SlicingConfig& lhs, const ::android::hardware::radio::V1_6::SlicingConfig& rhs){
    return !(lhs == rhs);
}

static inline std::string toString(const ::android::hardware::radio::V1_6::UrspRule& o) {
    using ::android::hardware::toString;
    std::string os;
    os += "{";
    os += ".precedence = ";
    os += ::android::hardware::toString(o.precedence);
    os += ", .trafficDescriptors = ";
    os += ::android::hardware::toString(o.trafficDescriptors);
    os += ", .routeSelectionDescriptor = ";
    os += ::android::hardware::toString(o.routeSelectionDescriptor);
    os += "}"; return os;
}

static inline void PrintTo(const ::android::hardware::radio::V1_6::UrspRule& o, ::std::ostream* os) {
    *os << toString(o);
}

static inline bool operator==(const ::android::hardware::radio::V1_6::UrspRule& lhs, const ::android::hardware::radio::V1_6::UrspRule& rhs) {
    if (lhs.precedence != rhs.precedence) {
        return false;
    }
    if (lhs.trafficDescriptors != rhs.trafficDescriptors) {
        return false;
    }
    if (lhs.routeSelectionDescriptor != rhs.routeSelectionDescriptor) {
        return false;
    }
    return true;
}

static inline bool operator!=(const ::android::hardware::radio::V1_6::UrspRule& lhs, const ::android::hardware::radio::V1_6::UrspRule& rhs){
    return !(lhs == rhs);
}

static inline std::string toString(const ::android::hardware::radio::V1_6::OptionalPdpProtocolType& o) {
    using ::android::hardware::toString;
    std::string os;
    os += "{";

    switch (o.getDiscriminator()) {
        case ::android::hardware::radio::V1_6::OptionalPdpProtocolType::hidl_discriminator::noinit: {
            os += ".noinit = ";
            os += toString(o.noinit());
            break;
        }
        case ::android::hardware::radio::V1_6::OptionalPdpProtocolType::hidl_discriminator::value: {
            os += ".value = ";
            os += toString(o.value());
            break;
        }
        default: {
            ::android::hardware::details::logAlwaysFatal((
                    "Unknown union discriminator (value: " +
                    std::to_string((uint8_t) o.getDiscriminator()) + ").").c_str());
        }
    }
    os += "}"; return os;
}

static inline void PrintTo(const ::android::hardware::radio::V1_6::OptionalPdpProtocolType& o, ::std::ostream* os) {
    *os << toString(o);
}

static inline bool operator==(const ::android::hardware::radio::V1_6::OptionalPdpProtocolType& lhs, const ::android::hardware::radio::V1_6::OptionalPdpProtocolType& rhs) {
    if (lhs.getDiscriminator() != rhs.getDiscriminator()) {
        return false;
    }
    switch (lhs.getDiscriminator()) {
        case ::android::hardware::radio::V1_6::OptionalPdpProtocolType::hidl_discriminator::noinit: {
            return (lhs.noinit() == rhs.noinit());
        }
        case ::android::hardware::radio::V1_6::OptionalPdpProtocolType::hidl_discriminator::value: {
            return (lhs.value() == rhs.value());
        }
        default: {
            ::android::hardware::details::logAlwaysFatal((
                    "Unknown union discriminator (value: " +
                    std::to_string((uint8_t) lhs.getDiscriminator()) + ").").c_str());
        }
    }
    return true;
}

static inline bool operator!=(const ::android::hardware::radio::V1_6::OptionalPdpProtocolType& lhs, const ::android::hardware::radio::V1_6::OptionalPdpProtocolType& rhs){
    return !(lhs == rhs);
}

template<>
inline std::string toString<::android::hardware::radio::V1_6::SscMode>(int8_t o) {
    using ::android::hardware::details::toHexString;
    std::string os;
    ::android::hardware::hidl_bitfield<::android::hardware::radio::V1_6::SscMode> flipped = 0;
    bool first = true;
    if ((o & ::android::hardware::radio::V1_6::SscMode::MODE_1) == static_cast<int8_t>(::android::hardware::radio::V1_6::SscMode::MODE_1)) {
        os += (first ? "" : " | ");
        os += "MODE_1";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::SscMode::MODE_1;
    }
    if ((o & ::android::hardware::radio::V1_6::SscMode::MODE_2) == static_cast<int8_t>(::android::hardware::radio::V1_6::SscMode::MODE_2)) {
        os += (first ? "" : " | ");
        os += "MODE_2";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::SscMode::MODE_2;
    }
    if ((o & ::android::hardware::radio::V1_6::SscMode::MODE_3) == static_cast<int8_t>(::android::hardware::radio::V1_6::SscMode::MODE_3)) {
        os += (first ? "" : " | ");
        os += "MODE_3";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::SscMode::MODE_3;
    }
    if (o != flipped) {
        os += (first ? "" : " | ");
        os += toHexString(o & (~flipped));
    }os += " (";
    os += toHexString(o);
    os += ")";
    return os;
}

static inline std::string toString(::android::hardware::radio::V1_6::SscMode o) {
    using ::android::hardware::details::toHexString;
    if (o == ::android::hardware::radio::V1_6::SscMode::MODE_1) {
        return "MODE_1";
    }
    if (o == ::android::hardware::radio::V1_6::SscMode::MODE_2) {
        return "MODE_2";
    }
    if (o == ::android::hardware::radio::V1_6::SscMode::MODE_3) {
        return "MODE_3";
    }
    std::string os;
    os += toHexString(static_cast<int8_t>(o));
    return os;
}

static inline void PrintTo(::android::hardware::radio::V1_6::SscMode o, ::std::ostream* os) {
    *os << toString(o);
}

static inline std::string toString(const ::android::hardware::radio::V1_6::OptionalSscMode& o) {
    using ::android::hardware::toString;
    std::string os;
    os += "{";

    switch (o.getDiscriminator()) {
        case ::android::hardware::radio::V1_6::OptionalSscMode::hidl_discriminator::noinit: {
            os += ".noinit = ";
            os += toString(o.noinit());
            break;
        }
        case ::android::hardware::radio::V1_6::OptionalSscMode::hidl_discriminator::value: {
            os += ".value = ";
            os += toString(o.value());
            break;
        }
        default: {
            ::android::hardware::details::logAlwaysFatal((
                    "Unknown union discriminator (value: " +
                    std::to_string((uint8_t) o.getDiscriminator()) + ").").c_str());
        }
    }
    os += "}"; return os;
}

static inline void PrintTo(const ::android::hardware::radio::V1_6::OptionalSscMode& o, ::std::ostream* os) {
    *os << toString(o);
}

static inline bool operator==(const ::android::hardware::radio::V1_6::OptionalSscMode& lhs, const ::android::hardware::radio::V1_6::OptionalSscMode& rhs) {
    if (lhs.getDiscriminator() != rhs.getDiscriminator()) {
        return false;
    }
    switch (lhs.getDiscriminator()) {
        case ::android::hardware::radio::V1_6::OptionalSscMode::hidl_discriminator::noinit: {
            return (lhs.noinit() == rhs.noinit());
        }
        case ::android::hardware::radio::V1_6::OptionalSscMode::hidl_discriminator::value: {
            return (lhs.value() == rhs.value());
        }
        default: {
            ::android::hardware::details::logAlwaysFatal((
                    "Unknown union discriminator (value: " +
                    std::to_string((uint8_t) lhs.getDiscriminator()) + ").").c_str());
        }
    }
    return true;
}

static inline bool operator!=(const ::android::hardware::radio::V1_6::OptionalSscMode& lhs, const ::android::hardware::radio::V1_6::OptionalSscMode& rhs){
    return !(lhs == rhs);
}

static inline std::string toString(const ::android::hardware::radio::V1_6::RouteSelectionDescriptor& o) {
    using ::android::hardware::toString;
    std::string os;
    os += "{";
    os += ".precedence = ";
    os += ::android::hardware::toString(o.precedence);
    os += ", .sessionType = ";
    os += ::android::hardware::radio::V1_6::toString(o.sessionType);
    os += ", .sscMode = ";
    os += ::android::hardware::radio::V1_6::toString(o.sscMode);
    os += ", .sliceInfo = ";
    os += ::android::hardware::toString(o.sliceInfo);
    os += ", .dnn = ";
    os += ::android::hardware::toString(o.dnn);
    os += "}"; return os;
}

static inline void PrintTo(const ::android::hardware::radio::V1_6::RouteSelectionDescriptor& o, ::std::ostream* os) {
    *os << toString(o);
}

static inline bool operator==(const ::android::hardware::radio::V1_6::RouteSelectionDescriptor& lhs, const ::android::hardware::radio::V1_6::RouteSelectionDescriptor& rhs) {
    if (lhs.precedence != rhs.precedence) {
        return false;
    }
    if (lhs.sessionType != rhs.sessionType) {
        return false;
    }
    if (lhs.sscMode != rhs.sscMode) {
        return false;
    }
    if (lhs.sliceInfo != rhs.sliceInfo) {
        return false;
    }
    if (lhs.dnn != rhs.dnn) {
        return false;
    }
    return true;
}

static inline bool operator!=(const ::android::hardware::radio::V1_6::RouteSelectionDescriptor& lhs, const ::android::hardware::radio::V1_6::RouteSelectionDescriptor& rhs){
    return !(lhs == rhs);
}

template<>
inline std::string toString<::android::hardware::radio::V1_6::PublicKeyType>(int8_t o) {
    using ::android::hardware::details::toHexString;
    std::string os;
    ::android::hardware::hidl_bitfield<::android::hardware::radio::V1_6::PublicKeyType> flipped = 0;
    bool first = true;
    if ((o & ::android::hardware::radio::V1_6::PublicKeyType::EPDG) == static_cast<int8_t>(::android::hardware::radio::V1_6::PublicKeyType::EPDG)) {
        os += (first ? "" : " | ");
        os += "EPDG";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::PublicKeyType::EPDG;
    }
    if ((o & ::android::hardware::radio::V1_6::PublicKeyType::WLAN) == static_cast<int8_t>(::android::hardware::radio::V1_6::PublicKeyType::WLAN)) {
        os += (first ? "" : " | ");
        os += "WLAN";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::PublicKeyType::WLAN;
    }
    if (o != flipped) {
        os += (first ? "" : " | ");
        os += toHexString(o & (~flipped));
    }os += " (";
    os += toHexString(o);
    os += ")";
    return os;
}

static inline std::string toString(::android::hardware::radio::V1_6::PublicKeyType o) {
    using ::android::hardware::details::toHexString;
    if (o == ::android::hardware::radio::V1_6::PublicKeyType::EPDG) {
        return "EPDG";
    }
    if (o == ::android::hardware::radio::V1_6::PublicKeyType::WLAN) {
        return "WLAN";
    }
    std::string os;
    os += toHexString(static_cast<int8_t>(o));
    return os;
}

static inline void PrintTo(::android::hardware::radio::V1_6::PublicKeyType o, ::std::ostream* os) {
    *os << toString(o);
}

static inline std::string toString(const ::android::hardware::radio::V1_6::ImsiEncryptionInfo& o) {
    using ::android::hardware::toString;
    std::string os;
    os += "{";
    os += ".base = ";
    os += ::android::hardware::radio::V1_1::toString(o.base);
    os += ", .keyType = ";
    os += ::android::hardware::radio::V1_6::toString(o.keyType);
    os += "}"; return os;
}

static inline void PrintTo(const ::android::hardware::radio::V1_6::ImsiEncryptionInfo& o, ::std::ostream* os) {
    *os << toString(o);
}

static inline bool operator==(const ::android::hardware::radio::V1_6::ImsiEncryptionInfo& lhs, const ::android::hardware::radio::V1_6::ImsiEncryptionInfo& rhs) {
    if (lhs.base != rhs.base) {
        return false;
    }
    if (lhs.keyType != rhs.keyType) {
        return false;
    }
    return true;
}

static inline bool operator!=(const ::android::hardware::radio::V1_6::ImsiEncryptionInfo& lhs, const ::android::hardware::radio::V1_6::ImsiEncryptionInfo& rhs){
    return !(lhs == rhs);
}

static inline std::string toString(const ::android::hardware::radio::V1_6::PhonebookRecordInfo& o) {
    using ::android::hardware::toString;
    std::string os;
    os += "{";
    os += ".recordId = ";
    os += ::android::hardware::toString(o.recordId);
    os += ", .name = ";
    os += ::android::hardware::toString(o.name);
    os += ", .number = ";
    os += ::android::hardware::toString(o.number);
    os += ", .emails = ";
    os += ::android::hardware::toString(o.emails);
    os += ", .additionalNumbers = ";
    os += ::android::hardware::toString(o.additionalNumbers);
    os += "}"; return os;
}

static inline void PrintTo(const ::android::hardware::radio::V1_6::PhonebookRecordInfo& o, ::std::ostream* os) {
    *os << toString(o);
}

static inline bool operator==(const ::android::hardware::radio::V1_6::PhonebookRecordInfo& lhs, const ::android::hardware::radio::V1_6::PhonebookRecordInfo& rhs) {
    if (lhs.recordId != rhs.recordId) {
        return false;
    }
    if (lhs.name != rhs.name) {
        return false;
    }
    if (lhs.number != rhs.number) {
        return false;
    }
    if (lhs.emails != rhs.emails) {
        return false;
    }
    if (lhs.additionalNumbers != rhs.additionalNumbers) {
        return false;
    }
    return true;
}

static inline bool operator!=(const ::android::hardware::radio::V1_6::PhonebookRecordInfo& lhs, const ::android::hardware::radio::V1_6::PhonebookRecordInfo& rhs){
    return !(lhs == rhs);
}

static inline std::string toString(const ::android::hardware::radio::V1_6::PhonebookCapacity& o) {
    using ::android::hardware::toString;
    std::string os;
    os += "{";
    os += ".maxAdnRecords = ";
    os += ::android::hardware::toString(o.maxAdnRecords);
    os += ", .usedAdnRecords = ";
    os += ::android::hardware::toString(o.usedAdnRecords);
    os += ", .maxEmailRecords = ";
    os += ::android::hardware::toString(o.maxEmailRecords);
    os += ", .usedEmailRecords = ";
    os += ::android::hardware::toString(o.usedEmailRecords);
    os += ", .maxAdditionalNumberRecords = ";
    os += ::android::hardware::toString(o.maxAdditionalNumberRecords);
    os += ", .usedAdditionalNumberRecords = ";
    os += ::android::hardware::toString(o.usedAdditionalNumberRecords);
    os += ", .maxNameLen = ";
    os += ::android::hardware::toString(o.maxNameLen);
    os += ", .maxNumberLen = ";
    os += ::android::hardware::toString(o.maxNumberLen);
    os += ", .maxEmailLen = ";
    os += ::android::hardware::toString(o.maxEmailLen);
    os += ", .maxAdditionalNumberLen = ";
    os += ::android::hardware::toString(o.maxAdditionalNumberLen);
    os += "}"; return os;
}

static inline void PrintTo(const ::android::hardware::radio::V1_6::PhonebookCapacity& o, ::std::ostream* os) {
    *os << toString(o);
}

static inline bool operator==(const ::android::hardware::radio::V1_6::PhonebookCapacity& lhs, const ::android::hardware::radio::V1_6::PhonebookCapacity& rhs) {
    if (lhs.maxAdnRecords != rhs.maxAdnRecords) {
        return false;
    }
    if (lhs.usedAdnRecords != rhs.usedAdnRecords) {
        return false;
    }
    if (lhs.maxEmailRecords != rhs.maxEmailRecords) {
        return false;
    }
    if (lhs.usedEmailRecords != rhs.usedEmailRecords) {
        return false;
    }
    if (lhs.maxAdditionalNumberRecords != rhs.maxAdditionalNumberRecords) {
        return false;
    }
    if (lhs.usedAdditionalNumberRecords != rhs.usedAdditionalNumberRecords) {
        return false;
    }
    if (lhs.maxNameLen != rhs.maxNameLen) {
        return false;
    }
    if (lhs.maxNumberLen != rhs.maxNumberLen) {
        return false;
    }
    if (lhs.maxEmailLen != rhs.maxEmailLen) {
        return false;
    }
    if (lhs.maxAdditionalNumberLen != rhs.maxAdditionalNumberLen) {
        return false;
    }
    return true;
}

static inline bool operator!=(const ::android::hardware::radio::V1_6::PhonebookCapacity& lhs, const ::android::hardware::radio::V1_6::PhonebookCapacity& rhs){
    return !(lhs == rhs);
}

template<>
inline std::string toString<::android::hardware::radio::V1_6::PbReceivedStatus>(int8_t o) {
    using ::android::hardware::details::toHexString;
    std::string os;
    ::android::hardware::hidl_bitfield<::android::hardware::radio::V1_6::PbReceivedStatus> flipped = 0;
    bool first = true;
    if ((o & ::android::hardware::radio::V1_6::PbReceivedStatus::PB_RECEIVED_OK) == static_cast<int8_t>(::android::hardware::radio::V1_6::PbReceivedStatus::PB_RECEIVED_OK)) {
        os += (first ? "" : " | ");
        os += "PB_RECEIVED_OK";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::PbReceivedStatus::PB_RECEIVED_OK;
    }
    if ((o & ::android::hardware::radio::V1_6::PbReceivedStatus::PB_RECEIVED_ERROR) == static_cast<int8_t>(::android::hardware::radio::V1_6::PbReceivedStatus::PB_RECEIVED_ERROR)) {
        os += (first ? "" : " | ");
        os += "PB_RECEIVED_ERROR";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::PbReceivedStatus::PB_RECEIVED_ERROR;
    }
    if ((o & ::android::hardware::radio::V1_6::PbReceivedStatus::PB_RECEIVED_ABORT) == static_cast<int8_t>(::android::hardware::radio::V1_6::PbReceivedStatus::PB_RECEIVED_ABORT)) {
        os += (first ? "" : " | ");
        os += "PB_RECEIVED_ABORT";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::PbReceivedStatus::PB_RECEIVED_ABORT;
    }
    if ((o & ::android::hardware::radio::V1_6::PbReceivedStatus::PB_RECEIVED_FINAL) == static_cast<int8_t>(::android::hardware::radio::V1_6::PbReceivedStatus::PB_RECEIVED_FINAL)) {
        os += (first ? "" : " | ");
        os += "PB_RECEIVED_FINAL";
        first = false;
        flipped |= ::android::hardware::radio::V1_6::PbReceivedStatus::PB_RECEIVED_FINAL;
    }
    if (o != flipped) {
        os += (first ? "" : " | ");
        os += toHexString(o & (~flipped));
    }os += " (";
    os += toHexString(o);
    os += ")";
    return os;
}

static inline std::string toString(::android::hardware::radio::V1_6::PbReceivedStatus o) {
    using ::android::hardware::details::toHexString;
    if (o == ::android::hardware::radio::V1_6::PbReceivedStatus::PB_RECEIVED_OK) {
        return "PB_RECEIVED_OK";
    }
    if (o == ::android::hardware::radio::V1_6::PbReceivedStatus::PB_RECEIVED_ERROR) {
        return "PB_RECEIVED_ERROR";
    }
    if (o == ::android::hardware::radio::V1_6::PbReceivedStatus::PB_RECEIVED_ABORT) {
        return "PB_RECEIVED_ABORT";
    }
    if (o == ::android::hardware::radio::V1_6::PbReceivedStatus::PB_RECEIVED_FINAL) {
        return "PB_RECEIVED_FINAL";
    }
    std::string os;
    os += toHexString(static_cast<int8_t>(o));
    return os;
}

static inline void PrintTo(::android::hardware::radio::V1_6::PbReceivedStatus o, ::std::ostream* os) {
    *os << toString(o);
}


}  // namespace V1_6
}  // namespace radio
}  // namespace hardware
}  // namespace android

//
// global type declarations for package
//

namespace android {
namespace hardware {
namespace details {
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wc++17-extensions"
template<> inline constexpr std::array<::android::hardware::radio::V1_6::QosFlowIdRange, 2> hidl_enum_values<::android::hardware::radio::V1_6::QosFlowIdRange> = {
    ::android::hardware::radio::V1_6::QosFlowIdRange::MIN,
    ::android::hardware::radio::V1_6::QosFlowIdRange::MAX,
};
#pragma clang diagnostic pop
}  // namespace details
}  // namespace hardware
}  // namespace android

namespace android {
namespace hardware {
namespace details {
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wc++17-extensions"
template<> inline constexpr std::array<::android::hardware::radio::V1_6::QosProtocol, 5> hidl_enum_values<::android::hardware::radio::V1_6::QosProtocol> = {
    ::android::hardware::radio::V1_6::QosProtocol::UNSPECIFIED,
    ::android::hardware::radio::V1_6::QosProtocol::TCP,
    ::android::hardware::radio::V1_6::QosProtocol::UDP,
    ::android::hardware::radio::V1_6::QosProtocol::ESP,
    ::android::hardware::radio::V1_6::QosProtocol::AH,
};
#pragma clang diagnostic pop
}  // namespace details
}  // namespace hardware
}  // namespace android

namespace android {
namespace hardware {
namespace details {
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wc++17-extensions"
template<> inline constexpr std::array<::android::hardware::radio::V1_6::QosFilterDirection, 3> hidl_enum_values<::android::hardware::radio::V1_6::QosFilterDirection> = {
    ::android::hardware::radio::V1_6::QosFilterDirection::DOWNLINK,
    ::android::hardware::radio::V1_6::QosFilterDirection::UPLINK,
    ::android::hardware::radio::V1_6::QosFilterDirection::BIDIRECTIONAL,
};
#pragma clang diagnostic pop
}  // namespace details
}  // namespace hardware
}  // namespace android

namespace android {
namespace hardware {
namespace details {
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wc++17-extensions"
template<> inline constexpr std::array<::android::hardware::radio::V1_6::QosPortRange, 2> hidl_enum_values<::android::hardware::radio::V1_6::QosPortRange> = {
    ::android::hardware::radio::V1_6::QosPortRange::MIN,
    ::android::hardware::radio::V1_6::QosPortRange::MAX,
};
#pragma clang diagnostic pop
}  // namespace details
}  // namespace hardware
}  // namespace android

namespace android {
namespace hardware {
namespace details {
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wc++17-extensions"
template<> inline constexpr std::array<::android::hardware::radio::V1_6::RadioError, 89> hidl_enum_values<::android::hardware::radio::V1_6::RadioError> = {
    ::android::hardware::radio::V1_6::RadioError::NONE,
    ::android::hardware::radio::V1_6::RadioError::RADIO_NOT_AVAILABLE,
    ::android::hardware::radio::V1_6::RadioError::GENERIC_FAILURE,
    ::android::hardware::radio::V1_6::RadioError::PASSWORD_INCORRECT,
    ::android::hardware::radio::V1_6::RadioError::SIM_PIN2,
    ::android::hardware::radio::V1_6::RadioError::SIM_PUK2,
    ::android::hardware::radio::V1_6::RadioError::REQUEST_NOT_SUPPORTED,
    ::android::hardware::radio::V1_6::RadioError::CANCELLED,
    ::android::hardware::radio::V1_6::RadioError::OP_NOT_ALLOWED_DURING_VOICE_CALL,
    ::android::hardware::radio::V1_6::RadioError::OP_NOT_ALLOWED_BEFORE_REG_TO_NW,
    ::android::hardware::radio::V1_6::RadioError::SMS_SEND_FAIL_RETRY,
    ::android::hardware::radio::V1_6::RadioError::SIM_ABSENT,
    ::android::hardware::radio::V1_6::RadioError::SUBSCRIPTION_NOT_AVAILABLE,
    ::android::hardware::radio::V1_6::RadioError::MODE_NOT_SUPPORTED,
    ::android::hardware::radio::V1_6::RadioError::FDN_CHECK_FAILURE,
    ::android::hardware::radio::V1_6::RadioError::ILLEGAL_SIM_OR_ME,
    ::android::hardware::radio::V1_6::RadioError::MISSING_RESOURCE,
    ::android::hardware::radio::V1_6::RadioError::NO_SUCH_ELEMENT,
    ::android::hardware::radio::V1_6::RadioError::DIAL_MODIFIED_TO_USSD,
    ::android::hardware::radio::V1_6::RadioError::DIAL_MODIFIED_TO_SS,
    ::android::hardware::radio::V1_6::RadioError::DIAL_MODIFIED_TO_DIAL,
    ::android::hardware::radio::V1_6::RadioError::USSD_MODIFIED_TO_DIAL,
    ::android::hardware::radio::V1_6::RadioError::USSD_MODIFIED_TO_SS,
    ::android::hardware::radio::V1_6::RadioError::USSD_MODIFIED_TO_USSD,
    ::android::hardware::radio::V1_6::RadioError::SS_MODIFIED_TO_DIAL,
    ::android::hardware::radio::V1_6::RadioError::SS_MODIFIED_TO_USSD,
    ::android::hardware::radio::V1_6::RadioError::SUBSCRIPTION_NOT_SUPPORTED,
    ::android::hardware::radio::V1_6::RadioError::SS_MODIFIED_TO_SS,
    ::android::hardware::radio::V1_6::RadioError::LCE_NOT_SUPPORTED,
    ::android::hardware::radio::V1_6::RadioError::NO_MEMORY,
    ::android::hardware::radio::V1_6::RadioError::INTERNAL_ERR,
    ::android::hardware::radio::V1_6::RadioError::SYSTEM_ERR,
    ::android::hardware::radio::V1_6::RadioError::MODEM_ERR,
    ::android::hardware::radio::V1_6::RadioError::INVALID_STATE,
    ::android::hardware::radio::V1_6::RadioError::NO_RESOURCES,
    ::android::hardware::radio::V1_6::RadioError::SIM_ERR,
    ::android::hardware::radio::V1_6::RadioError::INVALID_ARGUMENTS,
    ::android::hardware::radio::V1_6::RadioError::INVALID_SIM_STATE,
    ::android::hardware::radio::V1_6::RadioError::INVALID_MODEM_STATE,
    ::android::hardware::radio::V1_6::RadioError::INVALID_CALL_ID,
    ::android::hardware::radio::V1_6::RadioError::NO_SMS_TO_ACK,
    ::android::hardware::radio::V1_6::RadioError::NETWORK_ERR,
    ::android::hardware::radio::V1_6::RadioError::REQUEST_RATE_LIMITED,
    ::android::hardware::radio::V1_6::RadioError::SIM_BUSY,
    ::android::hardware::radio::V1_6::RadioError::SIM_FULL,
    ::android::hardware::radio::V1_6::RadioError::NETWORK_REJECT,
    ::android::hardware::radio::V1_6::RadioError::OPERATION_NOT_ALLOWED,
    ::android::hardware::radio::V1_6::RadioError::EMPTY_RECORD,
    ::android::hardware::radio::V1_6::RadioError::INVALID_SMS_FORMAT,
    ::android::hardware::radio::V1_6::RadioError::ENCODING_ERR,
    ::android::hardware::radio::V1_6::RadioError::INVALID_SMSC_ADDRESS,
    ::android::hardware::radio::V1_6::RadioError::NO_SUCH_ENTRY,
    ::android::hardware::radio::V1_6::RadioError::NETWORK_NOT_READY,
    ::android::hardware::radio::V1_6::RadioError::NOT_PROVISIONED,
    ::android::hardware::radio::V1_6::RadioError::NO_SUBSCRIPTION,
    ::android::hardware::radio::V1_6::RadioError::NO_NETWORK_FOUND,
    ::android::hardware::radio::V1_6::RadioError::DEVICE_IN_USE,
    ::android::hardware::radio::V1_6::RadioError::ABORTED,
    ::android::hardware::radio::V1_6::RadioError::INVALID_RESPONSE,
    ::android::hardware::radio::V1_6::RadioError::OEM_ERROR_1,
    ::android::hardware::radio::V1_6::RadioError::OEM_ERROR_2,
    ::android::hardware::radio::V1_6::RadioError::OEM_ERROR_3,
    ::android::hardware::radio::V1_6::RadioError::OEM_ERROR_4,
    ::android::hardware::radio::V1_6::RadioError::OEM_ERROR_5,
    ::android::hardware::radio::V1_6::RadioError::OEM_ERROR_6,
    ::android::hardware::radio::V1_6::RadioError::OEM_ERROR_7,
    ::android::hardware::radio::V1_6::RadioError::OEM_ERROR_8,
    ::android::hardware::radio::V1_6::RadioError::OEM_ERROR_9,
    ::android::hardware::radio::V1_6::RadioError::OEM_ERROR_10,
    ::android::hardware::radio::V1_6::RadioError::OEM_ERROR_11,
    ::android::hardware::radio::V1_6::RadioError::OEM_ERROR_12,
    ::android::hardware::radio::V1_6::RadioError::OEM_ERROR_13,
    ::android::hardware::radio::V1_6::RadioError::OEM_ERROR_14,
    ::android::hardware::radio::V1_6::RadioError::OEM_ERROR_15,
    ::android::hardware::radio::V1_6::RadioError::OEM_ERROR_16,
    ::android::hardware::radio::V1_6::RadioError::OEM_ERROR_17,
    ::android::hardware::radio::V1_6::RadioError::OEM_ERROR_18,
    ::android::hardware::radio::V1_6::RadioError::OEM_ERROR_19,
    ::android::hardware::radio::V1_6::RadioError::OEM_ERROR_20,
    ::android::hardware::radio::V1_6::RadioError::OEM_ERROR_21,
    ::android::hardware::radio::V1_6::RadioError::OEM_ERROR_22,
    ::android::hardware::radio::V1_6::RadioError::OEM_ERROR_23,
    ::android::hardware::radio::V1_6::RadioError::OEM_ERROR_24,
    ::android::hardware::radio::V1_6::RadioError::OEM_ERROR_25,
    ::android::hardware::radio::V1_6::RadioError::SIMULTANEOUS_SMS_AND_CALL_NOT_ALLOWED,
    ::android::hardware::radio::V1_6::RadioError::ACCESS_BARRED,
    ::android::hardware::radio::V1_6::RadioError::BLOCKED_DUE_TO_CALL,
    ::android::hardware::radio::V1_6::RadioError::RF_HARDWARE_ISSUE,
    ::android::hardware::radio::V1_6::RadioError::NO_RF_CALIBRATION_INFO,
};
#pragma clang diagnostic pop
}  // namespace details
}  // namespace hardware
}  // namespace android

namespace android {
namespace hardware {
namespace details {
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wc++17-extensions"
template<> inline constexpr std::array<::android::hardware::radio::V1_6::HandoverFailureMode, 4> hidl_enum_values<::android::hardware::radio::V1_6::HandoverFailureMode> = {
    ::android::hardware::radio::V1_6::HandoverFailureMode::LEGACY,
    ::android::hardware::radio::V1_6::HandoverFailureMode::DO_FALLBACK,
    ::android::hardware::radio::V1_6::HandoverFailureMode::NO_FALLBACK_RETRY_HANDOVER,
    ::android::hardware::radio::V1_6::HandoverFailureMode::NO_FALLBACK_RETRY_SETUP_NORMAL,
};
#pragma clang diagnostic pop
}  // namespace details
}  // namespace hardware
}  // namespace android

namespace android {
namespace hardware {
namespace details {
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wc++17-extensions"
template<> inline constexpr std::array<::android::hardware::radio::V1_6::DataCallFailCause, 340> hidl_enum_values<::android::hardware::radio::V1_6::DataCallFailCause> = {
    ::android::hardware::radio::V1_6::DataCallFailCause::NONE,
    ::android::hardware::radio::V1_6::DataCallFailCause::OPERATOR_BARRED,
    ::android::hardware::radio::V1_6::DataCallFailCause::NAS_SIGNALLING,
    ::android::hardware::radio::V1_6::DataCallFailCause::INSUFFICIENT_RESOURCES,
    ::android::hardware::radio::V1_6::DataCallFailCause::MISSING_UKNOWN_APN,
    ::android::hardware::radio::V1_6::DataCallFailCause::UNKNOWN_PDP_ADDRESS_TYPE,
    ::android::hardware::radio::V1_6::DataCallFailCause::USER_AUTHENTICATION,
    ::android::hardware::radio::V1_6::DataCallFailCause::ACTIVATION_REJECT_GGSN,
    ::android::hardware::radio::V1_6::DataCallFailCause::ACTIVATION_REJECT_UNSPECIFIED,
    ::android::hardware::radio::V1_6::DataCallFailCause::SERVICE_OPTION_NOT_SUPPORTED,
    ::android::hardware::radio::V1_6::DataCallFailCause::SERVICE_OPTION_NOT_SUBSCRIBED,
    ::android::hardware::radio::V1_6::DataCallFailCause::SERVICE_OPTION_OUT_OF_ORDER,
    ::android::hardware::radio::V1_6::DataCallFailCause::NSAPI_IN_USE,
    ::android::hardware::radio::V1_6::DataCallFailCause::REGULAR_DEACTIVATION,
    ::android::hardware::radio::V1_6::DataCallFailCause::QOS_NOT_ACCEPTED,
    ::android::hardware::radio::V1_6::DataCallFailCause::NETWORK_FAILURE,
    ::android::hardware::radio::V1_6::DataCallFailCause::UMTS_REACTIVATION_REQ,
    ::android::hardware::radio::V1_6::DataCallFailCause::FEATURE_NOT_SUPP,
    ::android::hardware::radio::V1_6::DataCallFailCause::TFT_SEMANTIC_ERROR,
    ::android::hardware::radio::V1_6::DataCallFailCause::TFT_SYTAX_ERROR,
    ::android::hardware::radio::V1_6::DataCallFailCause::UNKNOWN_PDP_CONTEXT,
    ::android::hardware::radio::V1_6::DataCallFailCause::FILTER_SEMANTIC_ERROR,
    ::android::hardware::radio::V1_6::DataCallFailCause::FILTER_SYTAX_ERROR,
    ::android::hardware::radio::V1_6::DataCallFailCause::PDP_WITHOUT_ACTIVE_TFT,
    ::android::hardware::radio::V1_6::DataCallFailCause::ONLY_IPV4_ALLOWED,
    ::android::hardware::radio::V1_6::DataCallFailCause::ONLY_IPV6_ALLOWED,
    ::android::hardware::radio::V1_6::DataCallFailCause::ONLY_SINGLE_BEARER_ALLOWED,
    ::android::hardware::radio::V1_6::DataCallFailCause::ESM_INFO_NOT_RECEIVED,
    ::android::hardware::radio::V1_6::DataCallFailCause::PDN_CONN_DOES_NOT_EXIST,
    ::android::hardware::radio::V1_6::DataCallFailCause::MULTI_CONN_TO_SAME_PDN_NOT_ALLOWED,
    ::android::hardware::radio::V1_6::DataCallFailCause::MAX_ACTIVE_PDP_CONTEXT_REACHED,
    ::android::hardware::radio::V1_6::DataCallFailCause::UNSUPPORTED_APN_IN_CURRENT_PLMN,
    ::android::hardware::radio::V1_6::DataCallFailCause::INVALID_TRANSACTION_ID,
    ::android::hardware::radio::V1_6::DataCallFailCause::MESSAGE_INCORRECT_SEMANTIC,
    ::android::hardware::radio::V1_6::DataCallFailCause::INVALID_MANDATORY_INFO,
    ::android::hardware::radio::V1_6::DataCallFailCause::MESSAGE_TYPE_UNSUPPORTED,
    ::android::hardware::radio::V1_6::DataCallFailCause::MSG_TYPE_NONCOMPATIBLE_STATE,
    ::android::hardware::radio::V1_6::DataCallFailCause::UNKNOWN_INFO_ELEMENT,
    ::android::hardware::radio::V1_6::DataCallFailCause::CONDITIONAL_IE_ERROR,
    ::android::hardware::radio::V1_6::DataCallFailCause::MSG_AND_PROTOCOL_STATE_UNCOMPATIBLE,
    ::android::hardware::radio::V1_6::DataCallFailCause::PROTOCOL_ERRORS,
    ::android::hardware::radio::V1_6::DataCallFailCause::APN_TYPE_CONFLICT,
    ::android::hardware::radio::V1_6::DataCallFailCause::INVALID_PCSCF_ADDR,
    ::android::hardware::radio::V1_6::DataCallFailCause::INTERNAL_CALL_PREEMPT_BY_HIGH_PRIO_APN,
    ::android::hardware::radio::V1_6::DataCallFailCause::EMM_ACCESS_BARRED,
    ::android::hardware::radio::V1_6::DataCallFailCause::EMERGENCY_IFACE_ONLY,
    ::android::hardware::radio::V1_6::DataCallFailCause::IFACE_MISMATCH,
    ::android::hardware::radio::V1_6::DataCallFailCause::COMPANION_IFACE_IN_USE,
    ::android::hardware::radio::V1_6::DataCallFailCause::IP_ADDRESS_MISMATCH,
    ::android::hardware::radio::V1_6::DataCallFailCause::IFACE_AND_POL_FAMILY_MISMATCH,
    ::android::hardware::radio::V1_6::DataCallFailCause::EMM_ACCESS_BARRED_INFINITE_RETRY,
    ::android::hardware::radio::V1_6::DataCallFailCause::AUTH_FAILURE_ON_EMERGENCY_CALL,
    ::android::hardware::radio::V1_6::DataCallFailCause::OEM_DCFAILCAUSE_1,
    ::android::hardware::radio::V1_6::DataCallFailCause::OEM_DCFAILCAUSE_2,
    ::android::hardware::radio::V1_6::DataCallFailCause::OEM_DCFAILCAUSE_3,
    ::android::hardware::radio::V1_6::DataCallFailCause::OEM_DCFAILCAUSE_4,
    ::android::hardware::radio::V1_6::DataCallFailCause::OEM_DCFAILCAUSE_5,
    ::android::hardware::radio::V1_6::DataCallFailCause::OEM_DCFAILCAUSE_6,
    ::android::hardware::radio::V1_6::DataCallFailCause::OEM_DCFAILCAUSE_7,
    ::android::hardware::radio::V1_6::DataCallFailCause::OEM_DCFAILCAUSE_8,
    ::android::hardware::radio::V1_6::DataCallFailCause::OEM_DCFAILCAUSE_9,
    ::android::hardware::radio::V1_6::DataCallFailCause::OEM_DCFAILCAUSE_10,
    ::android::hardware::radio::V1_6::DataCallFailCause::OEM_DCFAILCAUSE_11,
    ::android::hardware::radio::V1_6::DataCallFailCause::OEM_DCFAILCAUSE_12,
    ::android::hardware::radio::V1_6::DataCallFailCause::OEM_DCFAILCAUSE_13,
    ::android::hardware::radio::V1_6::DataCallFailCause::OEM_DCFAILCAUSE_14,
    ::android::hardware::radio::V1_6::DataCallFailCause::OEM_DCFAILCAUSE_15,
    ::android::hardware::radio::V1_6::DataCallFailCause::VOICE_REGISTRATION_FAIL,
    ::android::hardware::radio::V1_6::DataCallFailCause::DATA_REGISTRATION_FAIL,
    ::android::hardware::radio::V1_6::DataCallFailCause::SIGNAL_LOST,
    ::android::hardware::radio::V1_6::DataCallFailCause::PREF_RADIO_TECH_CHANGED,
    ::android::hardware::radio::V1_6::DataCallFailCause::RADIO_POWER_OFF,
    ::android::hardware::radio::V1_6::DataCallFailCause::TETHERED_CALL_ACTIVE,
    ::android::hardware::radio::V1_6::DataCallFailCause::ERROR_UNSPECIFIED,
    ::android::hardware::radio::V1_6::DataCallFailCause::LLC_SNDCP,
    ::android::hardware::radio::V1_6::DataCallFailCause::ACTIVATION_REJECTED_BCM_VIOLATION,
    ::android::hardware::radio::V1_6::DataCallFailCause::COLLISION_WITH_NETWORK_INITIATED_REQUEST,
    ::android::hardware::radio::V1_6::DataCallFailCause::ONLY_IPV4V6_ALLOWED,
    ::android::hardware::radio::V1_6::DataCallFailCause::ONLY_NON_IP_ALLOWED,
    ::android::hardware::radio::V1_6::DataCallFailCause::UNSUPPORTED_QCI_VALUE,
    ::android::hardware::radio::V1_6::DataCallFailCause::BEARER_HANDLING_NOT_SUPPORTED,
    ::android::hardware::radio::V1_6::DataCallFailCause::INVALID_DNS_ADDR,
    ::android::hardware::radio::V1_6::DataCallFailCause::INVALID_PCSCF_OR_DNS_ADDRESS,
    ::android::hardware::radio::V1_6::DataCallFailCause::CALL_PREEMPT_BY_EMERGENCY_APN,
    ::android::hardware::radio::V1_6::DataCallFailCause::UE_INITIATED_DETACH_OR_DISCONNECT,
    ::android::hardware::radio::V1_6::DataCallFailCause::MIP_FA_REASON_UNSPECIFIED,
    ::android::hardware::radio::V1_6::DataCallFailCause::MIP_FA_ADMIN_PROHIBITED,
    ::android::hardware::radio::V1_6::DataCallFailCause::MIP_FA_INSUFFICIENT_RESOURCES,
    ::android::hardware::radio::V1_6::DataCallFailCause::MIP_FA_MOBILE_NODE_AUTHENTICATION_FAILURE,
    ::android::hardware::radio::V1_6::DataCallFailCause::MIP_FA_HOME_AGENT_AUTHENTICATION_FAILURE,
    ::android::hardware::radio::V1_6::DataCallFailCause::MIP_FA_REQUESTED_LIFETIME_TOO_LONG,
    ::android::hardware::radio::V1_6::DataCallFailCause::MIP_FA_MALFORMED_REQUEST,
    ::android::hardware::radio::V1_6::DataCallFailCause::MIP_FA_MALFORMED_REPLY,
    ::android::hardware::radio::V1_6::DataCallFailCause::MIP_FA_ENCAPSULATION_UNAVAILABLE,
    ::android::hardware::radio::V1_6::DataCallFailCause::MIP_FA_VJ_HEADER_COMPRESSION_UNAVAILABLE,
    ::android::hardware::radio::V1_6::DataCallFailCause::MIP_FA_REVERSE_TUNNEL_UNAVAILABLE,
    ::android::hardware::radio::V1_6::DataCallFailCause::MIP_FA_REVERSE_TUNNEL_IS_MANDATORY,
    ::android::hardware::radio::V1_6::DataCallFailCause::MIP_FA_DELIVERY_STYLE_NOT_SUPPORTED,
    ::android::hardware::radio::V1_6::DataCallFailCause::MIP_FA_MISSING_NAI,
    ::android::hardware::radio::V1_6::DataCallFailCause::MIP_FA_MISSING_HOME_AGENT,
    ::android::hardware::radio::V1_6::DataCallFailCause::MIP_FA_MISSING_HOME_ADDRESS,
    ::android::hardware::radio::V1_6::DataCallFailCause::MIP_FA_UNKNOWN_CHALLENGE,
    ::android::hardware::radio::V1_6::DataCallFailCause::MIP_FA_MISSING_CHALLENGE,
    ::android::hardware::radio::V1_6::DataCallFailCause::MIP_FA_STALE_CHALLENGE,
    ::android::hardware::radio::V1_6::DataCallFailCause::MIP_HA_REASON_UNSPECIFIED,
    ::android::hardware::radio::V1_6::DataCallFailCause::MIP_HA_ADMIN_PROHIBITED,
    ::android::hardware::radio::V1_6::DataCallFailCause::MIP_HA_INSUFFICIENT_RESOURCES,
    ::android::hardware::radio::V1_6::DataCallFailCause::MIP_HA_MOBILE_NODE_AUTHENTICATION_FAILURE,
    ::android::hardware::radio::V1_6::DataCallFailCause::MIP_HA_FOREIGN_AGENT_AUTHENTICATION_FAILURE,
    ::android::hardware::radio::V1_6::DataCallFailCause::MIP_HA_REGISTRATION_ID_MISMATCH,
    ::android::hardware::radio::V1_6::DataCallFailCause::MIP_HA_MALFORMED_REQUEST,
    ::android::hardware::radio::V1_6::DataCallFailCause::MIP_HA_UNKNOWN_HOME_AGENT_ADDRESS,
    ::android::hardware::radio::V1_6::DataCallFailCause::MIP_HA_REVERSE_TUNNEL_UNAVAILABLE,
    ::android::hardware::radio::V1_6::DataCallFailCause::MIP_HA_REVERSE_TUNNEL_IS_MANDATORY,
    ::android::hardware::radio::V1_6::DataCallFailCause::MIP_HA_ENCAPSULATION_UNAVAILABLE,
    ::android::hardware::radio::V1_6::DataCallFailCause::CLOSE_IN_PROGRESS,
    ::android::hardware::radio::V1_6::DataCallFailCause::NETWORK_INITIATED_TERMINATION,
    ::android::hardware::radio::V1_6::DataCallFailCause::MODEM_APP_PREEMPTED,
    ::android::hardware::radio::V1_6::DataCallFailCause::PDN_IPV4_CALL_DISALLOWED,
    ::android::hardware::radio::V1_6::DataCallFailCause::PDN_IPV4_CALL_THROTTLED,
    ::android::hardware::radio::V1_6::DataCallFailCause::PDN_IPV6_CALL_DISALLOWED,
    ::android::hardware::radio::V1_6::DataCallFailCause::PDN_IPV6_CALL_THROTTLED,
    ::android::hardware::radio::V1_6::DataCallFailCause::MODEM_RESTART,
    ::android::hardware::radio::V1_6::DataCallFailCause::PDP_PPP_NOT_SUPPORTED,
    ::android::hardware::radio::V1_6::DataCallFailCause::UNPREFERRED_RAT,
    ::android::hardware::radio::V1_6::DataCallFailCause::PHYSICAL_LINK_CLOSE_IN_PROGRESS,
    ::android::hardware::radio::V1_6::DataCallFailCause::APN_PENDING_HANDOVER,
    ::android::hardware::radio::V1_6::DataCallFailCause::PROFILE_BEARER_INCOMPATIBLE,
    ::android::hardware::radio::V1_6::DataCallFailCause::SIM_CARD_CHANGED,
    ::android::hardware::radio::V1_6::DataCallFailCause::LOW_POWER_MODE_OR_POWERING_DOWN,
    ::android::hardware::radio::V1_6::DataCallFailCause::APN_DISABLED,
    ::android::hardware::radio::V1_6::DataCallFailCause::MAX_PPP_INACTIVITY_TIMER_EXPIRED,
    ::android::hardware::radio::V1_6::DataCallFailCause::IPV6_ADDRESS_TRANSFER_FAILED,
    ::android::hardware::radio::V1_6::DataCallFailCause::TRAT_SWAP_FAILED,
    ::android::hardware::radio::V1_6::DataCallFailCause::EHRPD_TO_HRPD_FALLBACK,
    ::android::hardware::radio::V1_6::DataCallFailCause::MIP_CONFIG_FAILURE,
    ::android::hardware::radio::V1_6::DataCallFailCause::PDN_INACTIVITY_TIMER_EXPIRED,
    ::android::hardware::radio::V1_6::DataCallFailCause::MAX_IPV4_CONNECTIONS,
    ::android::hardware::radio::V1_6::DataCallFailCause::MAX_IPV6_CONNECTIONS,
    ::android::hardware::radio::V1_6::DataCallFailCause::APN_MISMATCH,
    ::android::hardware::radio::V1_6::DataCallFailCause::IP_VERSION_MISMATCH,
    ::android::hardware::radio::V1_6::DataCallFailCause::DUN_CALL_DISALLOWED,
    ::android::hardware::radio::V1_6::DataCallFailCause::INTERNAL_EPC_NONEPC_TRANSITION,
    ::android::hardware::radio::V1_6::DataCallFailCause::INTERFACE_IN_USE,
    ::android::hardware::radio::V1_6::DataCallFailCause::APN_DISALLOWED_ON_ROAMING,
    ::android::hardware::radio::V1_6::DataCallFailCause::APN_PARAMETERS_CHANGED,
    ::android::hardware::radio::V1_6::DataCallFailCause::NULL_APN_DISALLOWED,
    ::android::hardware::radio::V1_6::DataCallFailCause::THERMAL_MITIGATION,
    ::android::hardware::radio::V1_6::DataCallFailCause::DATA_SETTINGS_DISABLED,
    ::android::hardware::radio::V1_6::DataCallFailCause::DATA_ROAMING_SETTINGS_DISABLED,
    ::android::hardware::radio::V1_6::DataCallFailCause::DDS_SWITCHED,
    ::android::hardware::radio::V1_6::DataCallFailCause::FORBIDDEN_APN_NAME,
    ::android::hardware::radio::V1_6::DataCallFailCause::DDS_SWITCH_IN_PROGRESS,
    ::android::hardware::radio::V1_6::DataCallFailCause::CALL_DISALLOWED_IN_ROAMING,
    ::android::hardware::radio::V1_6::DataCallFailCause::NON_IP_NOT_SUPPORTED,
    ::android::hardware::radio::V1_6::DataCallFailCause::PDN_NON_IP_CALL_THROTTLED,
    ::android::hardware::radio::V1_6::DataCallFailCause::PDN_NON_IP_CALL_DISALLOWED,
    ::android::hardware::radio::V1_6::DataCallFailCause::CDMA_LOCK,
    ::android::hardware::radio::V1_6::DataCallFailCause::CDMA_INTERCEPT,
    ::android::hardware::radio::V1_6::DataCallFailCause::CDMA_REORDER,
    ::android::hardware::radio::V1_6::DataCallFailCause::CDMA_RELEASE_DUE_TO_SO_REJECTION,
    ::android::hardware::radio::V1_6::DataCallFailCause::CDMA_INCOMING_CALL,
    ::android::hardware::radio::V1_6::DataCallFailCause::CDMA_ALERT_STOP,
    ::android::hardware::radio::V1_6::DataCallFailCause::CHANNEL_ACQUISITION_FAILURE,
    ::android::hardware::radio::V1_6::DataCallFailCause::MAX_ACCESS_PROBE,
    ::android::hardware::radio::V1_6::DataCallFailCause::CONCURRENT_SERVICE_NOT_SUPPORTED_BY_BASE_STATION,
    ::android::hardware::radio::V1_6::DataCallFailCause::NO_RESPONSE_FROM_BASE_STATION,
    ::android::hardware::radio::V1_6::DataCallFailCause::REJECTED_BY_BASE_STATION,
    ::android::hardware::radio::V1_6::DataCallFailCause::CONCURRENT_SERVICES_INCOMPATIBLE,
    ::android::hardware::radio::V1_6::DataCallFailCause::NO_CDMA_SERVICE,
    ::android::hardware::radio::V1_6::DataCallFailCause::RUIM_NOT_PRESENT,
    ::android::hardware::radio::V1_6::DataCallFailCause::CDMA_RETRY_ORDER,
    ::android::hardware::radio::V1_6::DataCallFailCause::ACCESS_BLOCK,
    ::android::hardware::radio::V1_6::DataCallFailCause::ACCESS_BLOCK_ALL,
    ::android::hardware::radio::V1_6::DataCallFailCause::IS707B_MAX_ACCESS_PROBES,
    ::android::hardware::radio::V1_6::DataCallFailCause::THERMAL_EMERGENCY,
    ::android::hardware::radio::V1_6::DataCallFailCause::CONCURRENT_SERVICES_NOT_ALLOWED,
    ::android::hardware::radio::V1_6::DataCallFailCause::INCOMING_CALL_REJECTED,
    ::android::hardware::radio::V1_6::DataCallFailCause::NO_SERVICE_ON_GATEWAY,
    ::android::hardware::radio::V1_6::DataCallFailCause::NO_GPRS_CONTEXT,
    ::android::hardware::radio::V1_6::DataCallFailCause::ILLEGAL_MS,
    ::android::hardware::radio::V1_6::DataCallFailCause::ILLEGAL_ME,
    ::android::hardware::radio::V1_6::DataCallFailCause::GPRS_SERVICES_AND_NON_GPRS_SERVICES_NOT_ALLOWED,
    ::android::hardware::radio::V1_6::DataCallFailCause::GPRS_SERVICES_NOT_ALLOWED,
    ::android::hardware::radio::V1_6::DataCallFailCause::MS_IDENTITY_CANNOT_BE_DERIVED_BY_THE_NETWORK,
    ::android::hardware::radio::V1_6::DataCallFailCause::IMPLICITLY_DETACHED,
    ::android::hardware::radio::V1_6::DataCallFailCause::PLMN_NOT_ALLOWED,
    ::android::hardware::radio::V1_6::DataCallFailCause::LOCATION_AREA_NOT_ALLOWED,
    ::android::hardware::radio::V1_6::DataCallFailCause::GPRS_SERVICES_NOT_ALLOWED_IN_THIS_PLMN,
    ::android::hardware::radio::V1_6::DataCallFailCause::PDP_DUPLICATE,
    ::android::hardware::radio::V1_6::DataCallFailCause::UE_RAT_CHANGE,
    ::android::hardware::radio::V1_6::DataCallFailCause::CONGESTION,
    ::android::hardware::radio::V1_6::DataCallFailCause::NO_PDP_CONTEXT_ACTIVATED,
    ::android::hardware::radio::V1_6::DataCallFailCause::ACCESS_CLASS_DSAC_REJECTION,
    ::android::hardware::radio::V1_6::DataCallFailCause::PDP_ACTIVATE_MAX_RETRY_FAILED,
    ::android::hardware::radio::V1_6::DataCallFailCause::RADIO_ACCESS_BEARER_FAILURE,
    ::android::hardware::radio::V1_6::DataCallFailCause::ESM_UNKNOWN_EPS_BEARER_CONTEXT,
    ::android::hardware::radio::V1_6::DataCallFailCause::DRB_RELEASED_BY_RRC,
    ::android::hardware::radio::V1_6::DataCallFailCause::CONNECTION_RELEASED,
    ::android::hardware::radio::V1_6::DataCallFailCause::EMM_DETACHED,
    ::android::hardware::radio::V1_6::DataCallFailCause::EMM_ATTACH_FAILED,
    ::android::hardware::radio::V1_6::DataCallFailCause::EMM_ATTACH_STARTED,
    ::android::hardware::radio::V1_6::DataCallFailCause::LTE_NAS_SERVICE_REQUEST_FAILED,
    ::android::hardware::radio::V1_6::DataCallFailCause::DUPLICATE_BEARER_ID,
    ::android::hardware::radio::V1_6::DataCallFailCause::ESM_COLLISION_SCENARIOS,
    ::android::hardware::radio::V1_6::DataCallFailCause::ESM_BEARER_DEACTIVATED_TO_SYNC_WITH_NETWORK,
    ::android::hardware::radio::V1_6::DataCallFailCause::ESM_NW_ACTIVATED_DED_BEARER_WITH_ID_OF_DEF_BEARER,
    ::android::hardware::radio::V1_6::DataCallFailCause::ESM_BAD_OTA_MESSAGE,
    ::android::hardware::radio::V1_6::DataCallFailCause::ESM_DOWNLOAD_SERVER_REJECTED_THE_CALL,
    ::android::hardware::radio::V1_6::DataCallFailCause::ESM_CONTEXT_TRANSFERRED_DUE_TO_IRAT,
    ::android::hardware::radio::V1_6::DataCallFailCause::DS_EXPLICIT_DEACTIVATION,
    ::android::hardware::radio::V1_6::DataCallFailCause::ESM_LOCAL_CAUSE_NONE,
    ::android::hardware::radio::V1_6::DataCallFailCause::LTE_THROTTLING_NOT_REQUIRED,
    ::android::hardware::radio::V1_6::DataCallFailCause::ACCESS_CONTROL_LIST_CHECK_FAILURE,
    ::android::hardware::radio::V1_6::DataCallFailCause::SERVICE_NOT_ALLOWED_ON_PLMN,
    ::android::hardware::radio::V1_6::DataCallFailCause::EMM_T3417_EXPIRED,
    ::android::hardware::radio::V1_6::DataCallFailCause::EMM_T3417_EXT_EXPIRED,
    ::android::hardware::radio::V1_6::DataCallFailCause::RRC_UPLINK_DATA_TRANSMISSION_FAILURE,
    ::android::hardware::radio::V1_6::DataCallFailCause::RRC_UPLINK_DELIVERY_FAILED_DUE_TO_HANDOVER,
    ::android::hardware::radio::V1_6::DataCallFailCause::RRC_UPLINK_CONNECTION_RELEASE,
    ::android::hardware::radio::V1_6::DataCallFailCause::RRC_UPLINK_RADIO_LINK_FAILURE,
    ::android::hardware::radio::V1_6::DataCallFailCause::RRC_UPLINK_ERROR_REQUEST_FROM_NAS,
    ::android::hardware::radio::V1_6::DataCallFailCause::RRC_CONNECTION_ACCESS_STRATUM_FAILURE,
    ::android::hardware::radio::V1_6::DataCallFailCause::RRC_CONNECTION_ANOTHER_PROCEDURE_IN_PROGRESS,
    ::android::hardware::radio::V1_6::DataCallFailCause::RRC_CONNECTION_ACCESS_BARRED,
    ::android::hardware::radio::V1_6::DataCallFailCause::RRC_CONNECTION_CELL_RESELECTION,
    ::android::hardware::radio::V1_6::DataCallFailCause::RRC_CONNECTION_CONFIG_FAILURE,
    ::android::hardware::radio::V1_6::DataCallFailCause::RRC_CONNECTION_TIMER_EXPIRED,
    ::android::hardware::radio::V1_6::DataCallFailCause::RRC_CONNECTION_LINK_FAILURE,
    ::android::hardware::radio::V1_6::DataCallFailCause::RRC_CONNECTION_CELL_NOT_CAMPED,
    ::android::hardware::radio::V1_6::DataCallFailCause::RRC_CONNECTION_SYSTEM_INTERVAL_FAILURE,
    ::android::hardware::radio::V1_6::DataCallFailCause::RRC_CONNECTION_REJECT_BY_NETWORK,
    ::android::hardware::radio::V1_6::DataCallFailCause::RRC_CONNECTION_NORMAL_RELEASE,
    ::android::hardware::radio::V1_6::DataCallFailCause::RRC_CONNECTION_RADIO_LINK_FAILURE,
    ::android::hardware::radio::V1_6::DataCallFailCause::RRC_CONNECTION_REESTABLISHMENT_FAILURE,
    ::android::hardware::radio::V1_6::DataCallFailCause::RRC_CONNECTION_OUT_OF_SERVICE_DURING_CELL_REGISTER,
    ::android::hardware::radio::V1_6::DataCallFailCause::RRC_CONNECTION_ABORT_REQUEST,
    ::android::hardware::radio::V1_6::DataCallFailCause::RRC_CONNECTION_SYSTEM_INFORMATION_BLOCK_READ_ERROR,
    ::android::hardware::radio::V1_6::DataCallFailCause::NETWORK_INITIATED_DETACH_WITH_AUTO_REATTACH,
    ::android::hardware::radio::V1_6::DataCallFailCause::NETWORK_INITIATED_DETACH_NO_AUTO_REATTACH,
    ::android::hardware::radio::V1_6::DataCallFailCause::ESM_PROCEDURE_TIME_OUT,
    ::android::hardware::radio::V1_6::DataCallFailCause::INVALID_CONNECTION_ID,
    ::android::hardware::radio::V1_6::DataCallFailCause::MAXIMIUM_NSAPIS_EXCEEDED,
    ::android::hardware::radio::V1_6::DataCallFailCause::INVALID_PRIMARY_NSAPI,
    ::android::hardware::radio::V1_6::DataCallFailCause::CANNOT_ENCODE_OTA_MESSAGE,
    ::android::hardware::radio::V1_6::DataCallFailCause::RADIO_ACCESS_BEARER_SETUP_FAILURE,
    ::android::hardware::radio::V1_6::DataCallFailCause::PDP_ESTABLISH_TIMEOUT_EXPIRED,
    ::android::hardware::radio::V1_6::DataCallFailCause::PDP_MODIFY_TIMEOUT_EXPIRED,
    ::android::hardware::radio::V1_6::DataCallFailCause::PDP_INACTIVE_TIMEOUT_EXPIRED,
    ::android::hardware::radio::V1_6::DataCallFailCause::PDP_LOWERLAYER_ERROR,
    ::android::hardware::radio::V1_6::DataCallFailCause::PDP_MODIFY_COLLISION,
    ::android::hardware::radio::V1_6::DataCallFailCause::MAXINUM_SIZE_OF_L2_MESSAGE_EXCEEDED,
    ::android::hardware::radio::V1_6::DataCallFailCause::NAS_REQUEST_REJECTED_BY_NETWORK,
    ::android::hardware::radio::V1_6::DataCallFailCause::RRC_CONNECTION_INVALID_REQUEST,
    ::android::hardware::radio::V1_6::DataCallFailCause::RRC_CONNECTION_TRACKING_AREA_ID_CHANGED,
    ::android::hardware::radio::V1_6::DataCallFailCause::RRC_CONNECTION_RF_UNAVAILABLE,
    ::android::hardware::radio::V1_6::DataCallFailCause::RRC_CONNECTION_ABORTED_DUE_TO_IRAT_CHANGE,
    ::android::hardware::radio::V1_6::DataCallFailCause::RRC_CONNECTION_RELEASED_SECURITY_NOT_ACTIVE,
    ::android::hardware::radio::V1_6::DataCallFailCause::RRC_CONNECTION_ABORTED_AFTER_HANDOVER,
    ::android::hardware::radio::V1_6::DataCallFailCause::RRC_CONNECTION_ABORTED_AFTER_IRAT_CELL_CHANGE,
    ::android::hardware::radio::V1_6::DataCallFailCause::RRC_CONNECTION_ABORTED_DURING_IRAT_CELL_CHANGE,
    ::android::hardware::radio::V1_6::DataCallFailCause::IMSI_UNKNOWN_IN_HOME_SUBSCRIBER_SERVER,
    ::android::hardware::radio::V1_6::DataCallFailCause::IMEI_NOT_ACCEPTED,
    ::android::hardware::radio::V1_6::DataCallFailCause::EPS_SERVICES_AND_NON_EPS_SERVICES_NOT_ALLOWED,
    ::android::hardware::radio::V1_6::DataCallFailCause::EPS_SERVICES_NOT_ALLOWED_IN_PLMN,
    ::android::hardware::radio::V1_6::DataCallFailCause::MSC_TEMPORARILY_NOT_REACHABLE,
    ::android::hardware::radio::V1_6::DataCallFailCause::CS_DOMAIN_NOT_AVAILABLE,
    ::android::hardware::radio::V1_6::DataCallFailCause::ESM_FAILURE,
    ::android::hardware::radio::V1_6::DataCallFailCause::MAC_FAILURE,
    ::android::hardware::radio::V1_6::DataCallFailCause::SYNCHRONIZATION_FAILURE,
    ::android::hardware::radio::V1_6::DataCallFailCause::UE_SECURITY_CAPABILITIES_MISMATCH,
    ::android::hardware::radio::V1_6::DataCallFailCause::SECURITY_MODE_REJECTED,
    ::android::hardware::radio::V1_6::DataCallFailCause::UNACCEPTABLE_NON_EPS_AUTHENTICATION,
    ::android::hardware::radio::V1_6::DataCallFailCause::CS_FALLBACK_CALL_ESTABLISHMENT_NOT_ALLOWED,
    ::android::hardware::radio::V1_6::DataCallFailCause::NO_EPS_BEARER_CONTEXT_ACTIVATED,
    ::android::hardware::radio::V1_6::DataCallFailCause::INVALID_EMM_STATE,
    ::android::hardware::radio::V1_6::DataCallFailCause::NAS_LAYER_FAILURE,
    ::android::hardware::radio::V1_6::DataCallFailCause::MULTIPLE_PDP_CALL_NOT_ALLOWED,
    ::android::hardware::radio::V1_6::DataCallFailCause::IRAT_HANDOVER_FAILED,
    ::android::hardware::radio::V1_6::DataCallFailCause::TEST_LOOPBACK_REGULAR_DEACTIVATION,
    ::android::hardware::radio::V1_6::DataCallFailCause::LOWER_LAYER_REGISTRATION_FAILURE,
    ::android::hardware::radio::V1_6::DataCallFailCause::DATA_PLAN_EXPIRED,
    ::android::hardware::radio::V1_6::DataCallFailCause::UMTS_HANDOVER_TO_IWLAN,
    ::android::hardware::radio::V1_6::DataCallFailCause::EVDO_CONNECTION_DENY_BY_GENERAL_OR_NETWORK_BUSY,
    ::android::hardware::radio::V1_6::DataCallFailCause::EVDO_CONNECTION_DENY_BY_BILLING_OR_AUTHENTICATION_FAILURE,
    ::android::hardware::radio::V1_6::DataCallFailCause::EVDO_HDR_CHANGED,
    ::android::hardware::radio::V1_6::DataCallFailCause::EVDO_HDR_EXITED,
    ::android::hardware::radio::V1_6::DataCallFailCause::EVDO_HDR_NO_SESSION,
    ::android::hardware::radio::V1_6::DataCallFailCause::EVDO_USING_GPS_FIX_INSTEAD_OF_HDR_CALL,
    ::android::hardware::radio::V1_6::DataCallFailCause::EVDO_HDR_CONNECTION_SETUP_TIMEOUT,
    ::android::hardware::radio::V1_6::DataCallFailCause::FAILED_TO_ACQUIRE_COLOCATED_HDR,
    ::android::hardware::radio::V1_6::DataCallFailCause::OTASP_COMMIT_IN_PROGRESS,
    ::android::hardware::radio::V1_6::DataCallFailCause::NO_HYBRID_HDR_SERVICE,
    ::android::hardware::radio::V1_6::DataCallFailCause::HDR_NO_LOCK_GRANTED,
    ::android::hardware::radio::V1_6::DataCallFailCause::DBM_OR_SMS_IN_PROGRESS,
    ::android::hardware::radio::V1_6::DataCallFailCause::HDR_FADE,
    ::android::hardware::radio::V1_6::DataCallFailCause::HDR_ACCESS_FAILURE,
    ::android::hardware::radio::V1_6::DataCallFailCause::UNSUPPORTED_1X_PREV,
    ::android::hardware::radio::V1_6::DataCallFailCause::LOCAL_END,
    ::android::hardware::radio::V1_6::DataCallFailCause::NO_SERVICE,
    ::android::hardware::radio::V1_6::DataCallFailCause::FADE,
    ::android::hardware::radio::V1_6::DataCallFailCause::NORMAL_RELEASE,
    ::android::hardware::radio::V1_6::DataCallFailCause::ACCESS_ATTEMPT_ALREADY_IN_PROGRESS,
    ::android::hardware::radio::V1_6::DataCallFailCause::REDIRECTION_OR_HANDOFF_IN_PROGRESS,
    ::android::hardware::radio::V1_6::DataCallFailCause::EMERGENCY_MODE,
    ::android::hardware::radio::V1_6::DataCallFailCause::PHONE_IN_USE,
    ::android::hardware::radio::V1_6::DataCallFailCause::INVALID_MODE,
    ::android::hardware::radio::V1_6::DataCallFailCause::INVALID_SIM_STATE,
    ::android::hardware::radio::V1_6::DataCallFailCause::NO_COLLOCATED_HDR,
    ::android::hardware::radio::V1_6::DataCallFailCause::UE_IS_ENTERING_POWERSAVE_MODE,
    ::android::hardware::radio::V1_6::DataCallFailCause::DUAL_SWITCH,
    ::android::hardware::radio::V1_6::DataCallFailCause::PPP_TIMEOUT,
    ::android::hardware::radio::V1_6::DataCallFailCause::PPP_AUTH_FAILURE,
    ::android::hardware::radio::V1_6::DataCallFailCause::PPP_OPTION_MISMATCH,
    ::android::hardware::radio::V1_6::DataCallFailCause::PPP_PAP_FAILURE,
    ::android::hardware::radio::V1_6::DataCallFailCause::PPP_CHAP_FAILURE,
    ::android::hardware::radio::V1_6::DataCallFailCause::PPP_CLOSE_IN_PROGRESS,
    ::android::hardware::radio::V1_6::DataCallFailCause::LIMITED_TO_IPV4,
    ::android::hardware::radio::V1_6::DataCallFailCause::LIMITED_TO_IPV6,
    ::android::hardware::radio::V1_6::DataCallFailCause::VSNCP_TIMEOUT,
    ::android::hardware::radio::V1_6::DataCallFailCause::VSNCP_GEN_ERROR,
    ::android::hardware::radio::V1_6::DataCallFailCause::VSNCP_APN_UNATHORIZED,
    ::android::hardware::radio::V1_6::DataCallFailCause::VSNCP_PDN_LIMIT_EXCEEDED,
    ::android::hardware::radio::V1_6::DataCallFailCause::VSNCP_NO_PDN_GATEWAY_ADDRESS,
    ::android::hardware::radio::V1_6::DataCallFailCause::VSNCP_PDN_GATEWAY_UNREACHABLE,
    ::android::hardware::radio::V1_6::DataCallFailCause::VSNCP_PDN_GATEWAY_REJECT,
    ::android::hardware::radio::V1_6::DataCallFailCause::VSNCP_INSUFFICIENT_PARAMETERS,
    ::android::hardware::radio::V1_6::DataCallFailCause::VSNCP_RESOURCE_UNAVAILABLE,
    ::android::hardware::radio::V1_6::DataCallFailCause::VSNCP_ADMINISTRATIVELY_PROHIBITED,
    ::android::hardware::radio::V1_6::DataCallFailCause::VSNCP_PDN_ID_IN_USE,
    ::android::hardware::radio::V1_6::DataCallFailCause::VSNCP_SUBSCRIBER_LIMITATION,
    ::android::hardware::radio::V1_6::DataCallFailCause::VSNCP_PDN_EXISTS_FOR_THIS_APN,
    ::android::hardware::radio::V1_6::DataCallFailCause::VSNCP_RECONNECT_NOT_ALLOWED,
    ::android::hardware::radio::V1_6::DataCallFailCause::IPV6_PREFIX_UNAVAILABLE,
    ::android::hardware::radio::V1_6::DataCallFailCause::HANDOFF_PREFERENCE_CHANGED,
    ::android::hardware::radio::V1_6::DataCallFailCause::SLICE_REJECTED,
    ::android::hardware::radio::V1_6::DataCallFailCause::MATCH_ALL_RULE_NOT_ALLOWED,
    ::android::hardware::radio::V1_6::DataCallFailCause::ALL_MATCHING_RULES_FAILED,
};
#pragma clang diagnostic pop
}  // namespace details
}  // namespace hardware
}  // namespace android

namespace android {
namespace hardware {
namespace details {
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wc++17-extensions"
template<> inline constexpr std::array<::android::hardware::radio::V1_6::SliceServiceType, 4> hidl_enum_values<::android::hardware::radio::V1_6::SliceServiceType> = {
    ::android::hardware::radio::V1_6::SliceServiceType::NONE,
    ::android::hardware::radio::V1_6::SliceServiceType::EMBB,
    ::android::hardware::radio::V1_6::SliceServiceType::URLLC,
    ::android::hardware::radio::V1_6::SliceServiceType::MIOT,
};
#pragma clang diagnostic pop
}  // namespace details
}  // namespace hardware
}  // namespace android

namespace android {
namespace hardware {
namespace details {
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wc++17-extensions"
template<> inline constexpr std::array<::android::hardware::radio::V1_6::SliceStatus, 6> hidl_enum_values<::android::hardware::radio::V1_6::SliceStatus> = {
    ::android::hardware::radio::V1_6::SliceStatus::UNKNOWN,
    ::android::hardware::radio::V1_6::SliceStatus::CONFIGURED,
    ::android::hardware::radio::V1_6::SliceStatus::ALLOWED,
    ::android::hardware::radio::V1_6::SliceStatus::REJECTED_NOT_AVAILABLE_IN_PLMN,
    ::android::hardware::radio::V1_6::SliceStatus::REJECTED_NOT_AVAILABLE_IN_REG_AREA,
    ::android::hardware::radio::V1_6::SliceStatus::DEFAULT_CONFIGURED,
};
#pragma clang diagnostic pop
}  // namespace details
}  // namespace hardware
}  // namespace android

namespace android {
namespace hardware {
namespace details {
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wc++17-extensions"
template<> inline constexpr std::array<::android::hardware::radio::V1_6::NrDualConnectivityState, 3> hidl_enum_values<::android::hardware::radio::V1_6::NrDualConnectivityState> = {
    ::android::hardware::radio::V1_6::NrDualConnectivityState::ENABLE,
    ::android::hardware::radio::V1_6::NrDualConnectivityState::DISABLE,
    ::android::hardware::radio::V1_6::NrDualConnectivityState::DISABLE_IMMEDIATE,
};
#pragma clang diagnostic pop
}  // namespace details
}  // namespace hardware
}  // namespace android

namespace android {
namespace hardware {
namespace details {
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wc++17-extensions"
template<> inline constexpr std::array<::android::hardware::radio::V1_6::DataThrottlingAction, 4> hidl_enum_values<::android::hardware::radio::V1_6::DataThrottlingAction> = {
    ::android::hardware::radio::V1_6::DataThrottlingAction::NO_DATA_THROTTLING,
    ::android::hardware::radio::V1_6::DataThrottlingAction::THROTTLE_SECONDARY_CARRIER,
    ::android::hardware::radio::V1_6::DataThrottlingAction::THROTTLE_ANCHOR_CARRIER,
    ::android::hardware::radio::V1_6::DataThrottlingAction::HOLD,
};
#pragma clang diagnostic pop
}  // namespace details
}  // namespace hardware
}  // namespace android

namespace android {
namespace hardware {
namespace details {
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wc++17-extensions"
template<> inline constexpr std::array<::android::hardware::radio::V1_6::VopsIndicator, 3> hidl_enum_values<::android::hardware::radio::V1_6::VopsIndicator> = {
    ::android::hardware::radio::V1_6::VopsIndicator::VOPS_NOT_SUPPORTED,
    ::android::hardware::radio::V1_6::VopsIndicator::VOPS_OVER_3GPP,
    ::android::hardware::radio::V1_6::VopsIndicator::VOPS_OVER_NON_3GPP,
};
#pragma clang diagnostic pop
}  // namespace details
}  // namespace hardware
}  // namespace android

namespace android {
namespace hardware {
namespace details {
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wc++17-extensions"
template<> inline constexpr std::array<::android::hardware::radio::V1_6::EmcIndicator, 4> hidl_enum_values<::android::hardware::radio::V1_6::EmcIndicator> = {
    ::android::hardware::radio::V1_6::EmcIndicator::EMC_NOT_SUPPORTED,
    ::android::hardware::radio::V1_6::EmcIndicator::EMC_NR_CONNECTED_TO_5GCN,
    ::android::hardware::radio::V1_6::EmcIndicator::EMC_EUTRA_CONNECTED_TO_5GCN,
    ::android::hardware::radio::V1_6::EmcIndicator::EMC_BOTH_NR_EUTRA_CONNECTED_TO_5GCN,
};
#pragma clang diagnostic pop
}  // namespace details
}  // namespace hardware
}  // namespace android

namespace android {
namespace hardware {
namespace details {
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wc++17-extensions"
template<> inline constexpr std::array<::android::hardware::radio::V1_6::EmfIndicator, 4> hidl_enum_values<::android::hardware::radio::V1_6::EmfIndicator> = {
    ::android::hardware::radio::V1_6::EmfIndicator::EMF_NOT_SUPPORTED,
    ::android::hardware::radio::V1_6::EmfIndicator::EMF_NR_CONNECTED_TO_5GCN,
    ::android::hardware::radio::V1_6::EmfIndicator::EMF_EUTRA_CONNECTED_TO_5GCN,
    ::android::hardware::radio::V1_6::EmfIndicator::EMF_BOTH_NR_EUTRA_CONNECTED_TO_5GCN,
};
#pragma clang diagnostic pop
}  // namespace details
}  // namespace hardware
}  // namespace android

namespace android {
namespace hardware {
namespace details {
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wc++17-extensions"
template<> inline constexpr std::array<::android::hardware::radio::V1_6::NgranBands, 53> hidl_enum_values<::android::hardware::radio::V1_6::NgranBands> = {
    ::android::hardware::radio::V1_6::NgranBands::BAND_1,
    ::android::hardware::radio::V1_6::NgranBands::BAND_2,
    ::android::hardware::radio::V1_6::NgranBands::BAND_3,
    ::android::hardware::radio::V1_6::NgranBands::BAND_5,
    ::android::hardware::radio::V1_6::NgranBands::BAND_7,
    ::android::hardware::radio::V1_6::NgranBands::BAND_8,
    ::android::hardware::radio::V1_6::NgranBands::BAND_12,
    ::android::hardware::radio::V1_6::NgranBands::BAND_14,
    ::android::hardware::radio::V1_6::NgranBands::BAND_18,
    ::android::hardware::radio::V1_6::NgranBands::BAND_20,
    ::android::hardware::radio::V1_6::NgranBands::BAND_25,
    ::android::hardware::radio::V1_6::NgranBands::BAND_28,
    ::android::hardware::radio::V1_6::NgranBands::BAND_29,
    ::android::hardware::radio::V1_6::NgranBands::BAND_30,
    ::android::hardware::radio::V1_6::NgranBands::BAND_34,
    ::android::hardware::radio::V1_6::NgranBands::BAND_38,
    ::android::hardware::radio::V1_6::NgranBands::BAND_39,
    ::android::hardware::radio::V1_6::NgranBands::BAND_40,
    ::android::hardware::radio::V1_6::NgranBands::BAND_41,
    ::android::hardware::radio::V1_6::NgranBands::BAND_48,
    ::android::hardware::radio::V1_6::NgranBands::BAND_50,
    ::android::hardware::radio::V1_6::NgranBands::BAND_51,
    ::android::hardware::radio::V1_6::NgranBands::BAND_65,
    ::android::hardware::radio::V1_6::NgranBands::BAND_66,
    ::android::hardware::radio::V1_6::NgranBands::BAND_70,
    ::android::hardware::radio::V1_6::NgranBands::BAND_71,
    ::android::hardware::radio::V1_6::NgranBands::BAND_74,
    ::android::hardware::radio::V1_6::NgranBands::BAND_75,
    ::android::hardware::radio::V1_6::NgranBands::BAND_76,
    ::android::hardware::radio::V1_6::NgranBands::BAND_77,
    ::android::hardware::radio::V1_6::NgranBands::BAND_78,
    ::android::hardware::radio::V1_6::NgranBands::BAND_79,
    ::android::hardware::radio::V1_6::NgranBands::BAND_80,
    ::android::hardware::radio::V1_6::NgranBands::BAND_81,
    ::android::hardware::radio::V1_6::NgranBands::BAND_82,
    ::android::hardware::radio::V1_6::NgranBands::BAND_83,
    ::android::hardware::radio::V1_6::NgranBands::BAND_84,
    ::android::hardware::radio::V1_6::NgranBands::BAND_86,
    ::android::hardware::radio::V1_6::NgranBands::BAND_89,
    ::android::hardware::radio::V1_6::NgranBands::BAND_90,
    ::android::hardware::radio::V1_6::NgranBands::BAND_91,
    ::android::hardware::radio::V1_6::NgranBands::BAND_92,
    ::android::hardware::radio::V1_6::NgranBands::BAND_93,
    ::android::hardware::radio::V1_6::NgranBands::BAND_94,
    ::android::hardware::radio::V1_6::NgranBands::BAND_95,
    ::android::hardware::radio::V1_6::NgranBands::BAND_257,
    ::android::hardware::radio::V1_6::NgranBands::BAND_258,
    ::android::hardware::radio::V1_6::NgranBands::BAND_260,
    ::android::hardware::radio::V1_6::NgranBands::BAND_261,
    ::android::hardware::radio::V1_6::NgranBands::BAND_26,
    ::android::hardware::radio::V1_6::NgranBands::BAND_46,
    ::android::hardware::radio::V1_6::NgranBands::BAND_53,
    ::android::hardware::radio::V1_6::NgranBands::BAND_96,
};
#pragma clang diagnostic pop
}  // namespace details
}  // namespace hardware
}  // namespace android

namespace android {
namespace hardware {
namespace details {
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wc++17-extensions"
template<> inline constexpr std::array<::android::hardware::radio::V1_6::SscMode, 3> hidl_enum_values<::android::hardware::radio::V1_6::SscMode> = {
    ::android::hardware::radio::V1_6::SscMode::MODE_1,
    ::android::hardware::radio::V1_6::SscMode::MODE_2,
    ::android::hardware::radio::V1_6::SscMode::MODE_3,
};
#pragma clang diagnostic pop
}  // namespace details
}  // namespace hardware
}  // namespace android

namespace android {
namespace hardware {
namespace details {
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wc++17-extensions"
template<> inline constexpr std::array<::android::hardware::radio::V1_6::PublicKeyType, 2> hidl_enum_values<::android::hardware::radio::V1_6::PublicKeyType> = {
    ::android::hardware::radio::V1_6::PublicKeyType::EPDG,
    ::android::hardware::radio::V1_6::PublicKeyType::WLAN,
};
#pragma clang diagnostic pop
}  // namespace details
}  // namespace hardware
}  // namespace android

namespace android {
namespace hardware {
namespace details {
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wc++17-extensions"
template<> inline constexpr std::array<::android::hardware::radio::V1_6::PbReceivedStatus, 4> hidl_enum_values<::android::hardware::radio::V1_6::PbReceivedStatus> = {
    ::android::hardware::radio::V1_6::PbReceivedStatus::PB_RECEIVED_OK,
    ::android::hardware::radio::V1_6::PbReceivedStatus::PB_RECEIVED_ERROR,
    ::android::hardware::radio::V1_6::PbReceivedStatus::PB_RECEIVED_ABORT,
    ::android::hardware::radio::V1_6::PbReceivedStatus::PB_RECEIVED_FINAL,
};
#pragma clang diagnostic pop
}  // namespace details
}  // namespace hardware
}  // namespace android


#endif  // HIDL_GENERATED_ANDROID_HARDWARE_RADIO_V1_6_TYPES_H
