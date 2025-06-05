/*
 * Copyright (c) 2019-2021 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 * Not a Contribution.
 */

/*
 * Copyright (C) 2014 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef HARDWARE_KEYMASTER_DEFS_H
#define HARDWARE_KEYMASTER_DEFS_H

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

/**
 * Authorization tags each have an associated type.  This enumeration facilitates tagging each with
 * a type, by using the high four bits (of an implied 32-bit unsigned enum value) to specify up to
 * 16 data types.  These values are ORed with tag IDs to generate the final tag ID values.
 */
typedef enum {
    KM_INVALID = 0 << 28, /* Invalid type, used to designate a tag as uninitialized */
    KM_ENUM = 1 << 28,
    KM_ENUM_REP = 2 << 28, /* Repeatable enumeration value. */
    KM_UINT = 3 << 28,
    KM_UINT_REP = 4 << 28, /* Repeatable integer value */
    KM_ULONG = 5 << 28,
    KM_DATE = 6 << 28,
    KM_BOOL = 7 << 28,
    KM_BIGNUM = 8 << 28,
    KM_BYTES = 9 << 28,
    KM_ULONG_REP = 10 << 28, /* Repeatable long value */
} keymaster_tag_type_t;

typedef enum {
    KM_TAG_INVALID = KM_INVALID | 0,

    /*
     * Tags that must be semantically enforced by hardware and software implementations.
     */

    /* Crypto parameters */
    KM_TAG_PURPOSE = KM_ENUM_REP | 1,    /* keymaster_purpose_t. */
    KM_TAG_ALGORITHM = KM_ENUM | 2,      /* keymaster_algorithm_t. */
    KM_TAG_KEY_SIZE = KM_UINT | 3,       /* Key size in bits. */
    KM_TAG_BLOCK_MODE = KM_ENUM_REP | 4, /* keymaster_block_mode_t. */
    KM_TAG_DIGEST = KM_ENUM_REP | 5,     /* keymaster_digest_t. */
    KM_TAG_PADDING = KM_ENUM_REP | 6,    /* keymaster_padding_t. */
    KM_TAG_CALLER_NONCE = KM_BOOL | 7,   /* Allow caller to specify nonce or IV. */
    KM_TAG_MIN_MAC_LENGTH = KM_UINT | 8, /* Minimum length of MAC or AEAD authentication tag in
                                          * bits. */
    KM_TAG_EC_CURVE = KM_ENUM | 10,      /* keymaster_ec_curve_t (keymaster2) */

    /* Algorithm-specific. */
    KM_TAG_RSA_PUBLIC_EXPONENT = KM_ULONG | 200,

    /*
     * Tags that should be semantically enforced by hardware if possible and will otherwise be
     * enforced by software (keystore).
     */

    /* Key validity period */
    KM_TAG_ACTIVE_DATETIME = KM_DATE | 400,             /* Start of validity */
    KM_TAG_ORIGINATION_EXPIRE_DATETIME = KM_DATE | 401, /* Date when new "messages" should no
                                                           longer be created. */
    KM_TAG_USAGE_EXPIRE_DATETIME = KM_DATE | 402,       /* Date when existing "messages" should no
                                                           longer be trusted. */
    KM_TAG_MIN_SECONDS_BETWEEN_OPS = KM_UINT | 403,     /* Minimum elapsed time between
                                                           cryptographic operations with the key. */
    KM_TAG_MAX_USES_PER_BOOT = KM_UINT | 404,           /* Number of times the key can be used per
                                                           boot. */

    /* Application access control */
    KM_TAG_APPLICATION_ID = KM_BYTES | 601,  /* Byte string identifying the authorized
                                              * application. */
    KM_TAG_APPLICATION_DATA = KM_BYTES | 700,      /* Data provided by authorized application. */

    /*
     * Semantically unenforceable tags, either because they have no specific meaning or because
     * they're informational only.
     */
    KM_TAG_CREATION_DATETIME = KM_DATE | 701,      /* Key creation time */
    KM_TAG_ORIGIN = KM_ENUM | 702,                 /* keymaster_key_origin_t. */
    KM_TAG_ROOT_OF_TRUST = KM_BYTES | 704,         /* Root of trust ID. */
    KM_TAG_OS_VERSION = KM_UINT | 705,             /* Version of system (keymaster2) */
    KM_TAG_OS_PATCHLEVEL = KM_UINT | 706,          /* Patch level of system (keymaster2) */
    KM_TAG_UNIQUE_ID = KM_BYTES | 707,             /* Used to provide unique ID in attestation */
    KM_TAG_ATTESTATION_CHALLENGE = KM_BYTES | 708, /* Used to provide challenge in attestation */
    KM_TAG_ATTESTATION_APPLICATION_ID = KM_BYTES | 709, /* Used to identify the set of possible
                                                         * applications of which one has initiated
                                                         * a key attestation */
    KM_TAG_ATTESTATION_ID_BRAND = KM_BYTES | 710,  /* Used to provide the device's brand name to be
                                                      included in attestation */
    KM_TAG_ATTESTATION_ID_DEVICE = KM_BYTES | 711, /* Used to provide the device's device name to be
                                                      included in attestation */
    KM_TAG_ATTESTATION_ID_PRODUCT = KM_BYTES | 712, /* Used to provide the device's product name to
                                                       be included in attestation */
    KM_TAG_ATTESTATION_ID_SERIAL = KM_BYTES | 713, /* Used to provide the device's serial number to
                                                      be included in attestation */
    KM_TAG_ATTESTATION_ID_IMEI = KM_BYTES | 714,   /* Used to provide the device's IMEI to be
                                                      included in attestation */
    KM_TAG_ATTESTATION_ID_MEID = KM_BYTES | 715,   /* Used to provide the device's MEID to be
                                                      included in attestation */
    KM_ATTESTATION_ID_MANUFACTURER = KM_BYTES | 716, /* Used to provide the device's manufacturer
                                                          name to be included in attestation */
    KM_ATTESTATION_ID_MODEL = KM_BYTES | 717,  /* Used to provide the device's model name to be
                                                    included in attestation */
    /**
     * Patch level of vendor image.  The value is an integer of the form YYYYMM, where YYYY is the
     * four-digit year when the vendor image was released and MM is the two-digit month.  During
     * each boot, the bootloader must provide the patch level of the vendor image to keymaser
     * (mechanism is implemntation-defined).  When keymaster keys are created or updated, the
     * VENDOR_PATCHLEVEL tag must be cryptographically bound to the keys, with the current value as
     * provided by the bootloader.  When keys are used, keymaster must verify that the
     * VENDOR_PATCHLEVEL bound to the key matches the current value.  If they do not match,
     * keymaster must return ErrorCode::KEY_REQUIRES_UPGRADE.  The client must then call upgradeKey.
     */
    KM_VENDOR_PATCHLEVEL = KM_UINT | 718,
    /**
     * Patch level of boot image.  The value is an integer of the form YYYYMM, where YYYY is the
     * four-digit year when the boot image was released and MM is the two-digit month.  During each
     * boot, the bootloader must provide the patch level of the boot image to keymaser (mechanism is
     * implemntation-defined).  When keymaster keys are created or updated, the BOOT_PATCHLEVEL tag
     * must be cryptographically bound to the keys, with the current value as provided by the
     * bootloader.  When keys are used, keymaster must verify that the BOOT_PATCHLEVEL bound to the
     * key matches the current value.  If they do not match, keymaster must return
     * ErrorCode::KEY_REQUIRES_UPGRADE.  The client must then call upgradeKey.
     */
    KM_BOOT_PATCHLEVEL = KM_UINT | 719,


    /* Tags used only to provide data to or receive data from operations */
    KM_TAG_ASSOCIATED_DATA = KM_BYTES | 1000, /* Used to provide associated data for AEAD modes. */
    KM_TAG_NONCE = KM_BYTES | 1001,           /* Nonce or Initialization Vector */
    KM_TAG_MAC_LENGTH = KM_UINT | 1003,       /* MAC or AEAD authentication tag length in
                                               * bits. */

    /* The following defines are kept for backward compatibility and are not supported */
    KM_TAG_INCLUDE_UNIQUE_ID = KM_BOOL | 202,
    KM_TAG_BLOB_USAGE_REQUIREMENTS = KM_ENUM | 301,
    KM_TAG_BOOTLOADER_ONLY = KM_BOOL | 302,
    KM_TAG_ROLLBACK_RESISTANCE = KM_BOOL | 303, /* Whether key is rollback-resistant. */
    KM_TAG_ROLLBACK_RESISTANT = KM_BOOL | 303,   /* Whether key is rollback-resistant. */

    /* HARDWARE_TYPE specifies the type of the secure hardware that is requested for the key
     * generation / import. See the SecurityLevel enum. In the absence of this tag, keystore will
     * use TRUSTED_ENVIRONMENT. If this tag is present and the requested hardware type is not
     * available, Keymaster returns HARDWARE_TYPE_UNAVAILABLE. This tag is not included in
     * attestations, but hardware type will be reflected in the keymaster SecurityLevel of the
     * attestation header. */
    KM_HARDWARE_TYPE = KM_ENUM | 304,
    KM_TAG_ALL_USERS = KM_BOOL | 500,
    KM_TAG_USER_ID = KM_UINT | 501,
    KM_TAG_USER_SECURE_ID = KM_ULONG_REP | 502,
    KM_TAG_NO_AUTH_REQUIRED = KM_BOOL | 503,
    KM_TAG_USER_AUTH_TYPE = KM_ENUM | 504,
    KM_TAG_AUTH_TIMEOUT = KM_UINT | 505,
    KM_TAG_ALLOW_WHILE_ON_BODY = KM_BOOL | 506,

    /**
     * TRUSTED_USER_PRESENCE_REQUIRED is an optional feature that specifies that this key may only
     * be used when the user has provided proof of physical presence. Proof of physical presence
     * must be a signal that cannot be provided or spoofed by any attacker who does not have
     * physical control of the device and has not compromised the secure environment. For instance,
     * proof of user identity may be considered proof of presence if it meets the requirements
     * above. However, proof of identity established in one security domain (e.g. TEE) does not
     * constitute proof of presence in another security domain (e.g. StrongBox), and no mechanism
     * analogous to the authentication token is defined for communicating proof of presence across
     * security domains.
     *
     * Some examples:
     *
     *     A hardware button hardwired to a PIN on a StrongBox device in such a way that nothing
     *     other than a button press can trigger the signal constitutes proof of physical presence
     *     for StrongBox keys.
     *
     *     Fingerprint authentication provides proof of presence (and identity) for TEE keys if the
     *     TEE has exclusive control of the fingerprint scanner and performs fingerprint matching.
     *
     *     Password authentication does not provide proof of presence to either TEE or StrongBox,
     *     even if TEE or StrongBox does the password matching, because password input is handled by
     *     the non-secure world, which means an attacker who has compromised Android can spoof
     *     password authentication.
     *
     * Finally, note that no mechanism is defined for delivering proof of presence to Keymaster,
     * except perhaps as implied by an auth token. This means that Keymaster must be able to check
     * proof of presence some other way. Further, the proof of presence must be performed between
     * begin() and the first call to update() or finish(). If the first update() or the finish()
     * call is made without proof of presence, the keymaster method must return
     * ErrorCode:PROOF_OF_PRESENCE_REQUIRED and abort the operation. The caller must delay the
     * update() or finish() call until proof of presence has been provided, which means the caller
     * must also have some mechanism for verifying that the proof has been provided.
     */
    KM_TRUSTED_USER_PRESENCE_REQUIRED = KM_BOOL | 507,
    /* TRUSTED_CONFIRMATION_REQUIRED is only applicable to keys with KeyPurpose SIGN, and specifies
     * that this key may only be used when the user provides confirmation of the data to be
     * signed. Confirmation is proven to keymaster via an approval token. See APPROVAL_TOKEN, as
     * well as the Trusted UI HAL.
     *
     * If an attempt to use a key with this tag does not have a cryptographically valid
     * APPROVAL_TOKEN provided to begin(), or if the data provided to update()/final() does not
     * match the data described in the token, keymaster must return NO_USER_CONFIRMATION. */
    KM_TRUSTED_CONFIRMATION_REQUIRED = KM_BOOL | 508,
    KM_UNLOCKED_DEVICE_REQUIRED = KM_BOOL | 509, /* Require the device screen to be unlocked if
                                                  * the key is used. */
    KM_TAG_ALL_APPLICATIONS = KM_BOOL | 600,
    KM_TAG_EXPORTABLE = KM_BOOL | 602,
    KM_STORAGE_KEY = KM_BOOL | 722,
    KM_TAG_AUTH_TOKEN = KM_BYTES | 1002,

    KM_TAG_KEY_TYPE = KM_UINT | 16202,

    KM_TAG_SHARED_KEY = KM_BOOL | 16301,
    KM_TAG_SHARED_VM1 = KM_UINT | 16302,
    KM_TAG_SHARED_VM2 = KM_UINT | 16303,
    KM_TAG_SHARED_VM3 = KM_UINT | 16304,
    KM_TAG_SHARED_VM4 = KM_UINT | 16305,
    KM_TAG_SHARED_VM5 = KM_UINT | 16306,
    KM_TAG_SHARED_VM6 = KM_UINT | 16307,
    KM_TAG_SHARED_VM7 = KM_UINT | 16308,
    KM_TAG_SHARED_VM8 = KM_UINT | 16309,
    KM_TAG_PURPOSE_VM1 = KM_ENUM_REP | 16311,
    KM_TAG_PURPOSE_VM2 = KM_ENUM_REP | 16312,
    KM_TAG_PURPOSE_VM3 = KM_ENUM_REP | 16313,
    KM_TAG_PURPOSE_VM4 = KM_ENUM_REP | 16314,
    KM_TAG_PURPOSE_VM5 = KM_ENUM_REP | 16315,
    KM_TAG_PURPOSE_VM6 = KM_ENUM_REP | 16316,
    KM_TAG_PURPOSE_VM7 = KM_ENUM_REP | 16317,
    KM_TAG_PURPOSE_VM8 = KM_ENUM_REP | 16318,

} keymaster_tag_t;

/**
 * Algorithms that may be provided by keymaster implementations.  Those that must be provided by all
 * implementations are tagged as "required".
 */
typedef enum {
    /* Asymmetric algorithms. */
    KM_ALGORITHM_RSA = 1,
    KM_ALGORITHM_EC = 3,
    /* Block ciphers algorithms */
    KM_ALGORITHM_AES = 32,
    /* MAC algorithms */
    KM_ALGORITHM_HMAC = 128,
} keymaster_algorithm_t;

/**
 * Symmetric block cipher modes provided by keymaster implementations.
 */
typedef enum {
    /* Unauthenticated modes, usable only for encryption/decryption and not generally recommended
     * except for compatibility with existing other protocols. */
    KM_MODE_ECB = 1,
    KM_MODE_CBC = 2,
    KM_MODE_CTR = 3,

    /* Authenticated modes, usable for encryption/decryption and signing/verification.  Recommended
     * over unauthenticated modes for all purposes. */
    KM_MODE_GCM = 32,
} keymaster_block_mode_t;

/**
 * Padding modes that may be applied to plaintext for encryption operations.  This list includes
 * padding modes for both symmetric and asymmetric algorithms.  Note that implementations should not
 * provide all possible combinations of algorithm and padding, only the
 * cryptographically-appropriate pairs.
 */
typedef enum {
    KM_PAD_NONE = 1,
    KM_PAD_RSA_OAEP = 2,
    KM_PAD_RSA_PSS = 3,
    KM_PAD_RSA_PKCS1_1_5_ENCRYPT = 4,
    KM_PAD_RSA_PKCS1_1_5_SIGN = 5,
    KM_PAD_PKCS7 = 64,
} keymaster_padding_t;

/**
 * Digests provided by keymaster implementations.
 */
typedef enum {
    KM_DIGEST_NONE = 0,
/* Optional, may not be implemented in hardware, will be handled
   in software if needed.*/
    KM_DIGEST_MD5 = 1,
    KM_DIGEST_SHA1 = 2,
    KM_DIGEST_SHA_2_224 = 3,
    KM_DIGEST_SHA_2_256 = 4,
    KM_DIGEST_SHA_2_384 = 5,
    KM_DIGEST_SHA_2_512 = 6,
} keymaster_digest_t;

/**
 * Supported EC curves, used in ECDSA/ECIES.
 */
typedef enum {
    KM_EC_CURVE_P_224 = 0,
    KM_EC_CURVE_P_256 = 1,
    KM_EC_CURVE_P_384 = 2,
    KM_EC_CURVE_P_521 = 3,
} keymaster_ec_curve_t;

/**
 * The origin of a key (or pair), i.e. where it was generated.  Note that KM_TAG_ORIGIN can be found
 * in either the hardware-enforced or software-enforced list for a key, indicating whether the key
 * is hardware or software-based.  Specifically, a key with KM_ORIGIN_GENERATED in the
 * hardware-enforced list is guaranteed never to have existed outide the secure hardware.
 */
typedef enum {
    KM_ORIGIN_GENERATED = 0, /* Generated in keymaster.  Should not exist outside the TEE. */
    KM_ORIGIN_IMPORTED = 2,  /* Imported into keymaster.  Existed as cleartext in Android. */
    KM_ORIGIN_UNKNOWN = 3,   /* Keymaster did not record origin.  This value can only be seen on
                              * keys in a keymaster0 implementation.  The keymaster0 adapter uses
                              * this value to document the fact that it is unkown whether the key
                              * was generated inside or imported into keymaster. */
} keymaster_key_origin_t;

/**
 * Possible purposes of a key (or pair).
 */
typedef enum {
    KM_PURPOSE_ENCRYPT = 0,    /* Usable with RSA, EC and AES keys. */
    KM_PURPOSE_DECRYPT = 1,    /* Usable with RSA, EC and AES keys. */
    KM_PURPOSE_SIGN = 2,       /* Usable with RSA, EC and HMAC keys. */
    KM_PURPOSE_VERIFY = 3,     /* Usable with RSA, EC and HMAC keys. */
} keymaster_purpose_t;

typedef struct {
    const uint8_t* data;
    size_t data_length;
} keymaster_blob_t;

typedef struct {
    keymaster_tag_t tag;
    union {
        uint32_t enumerated;   /* KM_ENUM and KM_ENUM_REP */
        bool boolean;          /* KM_BOOL */
        uint32_t integer;      /* KM_INT and KM_INT_REP */
        uint64_t long_integer; /* KM_LONG */
        uint64_t date_time;    /* KM_DATE */
        keymaster_blob_t blob; /* KM_BIGNUM and KM_BYTES*/
    };
} keymaster_key_param_t;

typedef struct {
    keymaster_key_param_t* params; /* may be NULL if length == 0 */
    size_t length;
} keymaster_key_param_set_t;

/**
 * Parameters that define a key's characteristics, including authorized modes of usage and access
 * control restrictions.  The parameters are divided into two categories, those that are enforced by
 * secure hardware, and those that are not.  For a software-only keymaster implementation the
 * enforced array must NULL.  Hardware implementations must enforce everything in the enforced
 * array.
 */
typedef struct {
    keymaster_key_param_set_t hw_enforced;
    keymaster_key_param_set_t sw_enforced;
} keymaster_key_characteristics_t;

typedef struct {
    const uint8_t* key_material;
    size_t key_material_size;
} keymaster_key_blob_t;

typedef struct {
    keymaster_blob_t* entries;
    size_t entry_count;
} keymaster_cert_chain_t;


typedef enum {
    KM_SECURITY_LEVEL_SOFTWARE = 0,
    KM_SECURITY_LEVEL_TRUSTED_ENVIRONMENT = 1,
} keymaster_security_level_t;

/**
 * Formats for key import and export.
 */
typedef enum {
    KM_KEY_FORMAT_X509 = 0,  /* for public key export */
    KM_KEY_FORMAT_PKCS8 = 1, /* for asymmetric key pair import */
    KM_KEY_FORMAT_RAW = 3,   /* for symmetric key import and export*/
    KM_KEY_FORMAT_SHARED = 7,   /* for shared key import and export*/
} keymaster_key_format_t;

/**
 * The keymaster operation API consists of begin, update, finish and abort. This is the type of the
 * handle used to tie the sequence of calls together.  A 64-bit value is used because it's important
 * that handles not be predictable.  Implementations must use strong random numbers for handle
 * values.
 */
typedef uint64_t keymaster_operation_handle_t;

typedef enum {
    KM_ERROR_OK = 0,
    KM_ERROR_ROOT_OF_TRUST_ALREADY_SET = -1,
    KM_ERROR_UNSUPPORTED_PURPOSE = -2,
    KM_ERROR_INCOMPATIBLE_PURPOSE = -3,
    KM_ERROR_UNSUPPORTED_ALGORITHM = -4,
    KM_ERROR_INCOMPATIBLE_ALGORITHM = -5,
    KM_ERROR_UNSUPPORTED_KEY_SIZE = -6,
    KM_ERROR_UNSUPPORTED_BLOCK_MODE = -7,
    KM_ERROR_INCOMPATIBLE_BLOCK_MODE = -8,
    KM_ERROR_UNSUPPORTED_MAC_LENGTH = -9,
    KM_ERROR_UNSUPPORTED_PADDING_MODE = -10,
    KM_ERROR_INCOMPATIBLE_PADDING_MODE = -11,
    KM_ERROR_UNSUPPORTED_DIGEST = -12,
    KM_ERROR_INCOMPATIBLE_DIGEST = -13,
    KM_ERROR_INVALID_EXPIRATION_TIME = -14,
    KM_ERROR_INVALID_USER_ID = -15,
    KM_ERROR_INVALID_AUTHORIZATION_TIMEOUT = -16,
    KM_ERROR_UNSUPPORTED_KEY_FORMAT = -17,
    KM_ERROR_INCOMPATIBLE_KEY_FORMAT = -18,
    KM_ERROR_UNSUPPORTED_KEY_ENCRYPTION_ALGORITHM = -19,   /* For PKCS8 & PKCS12 */
    KM_ERROR_UNSUPPORTED_KEY_VERIFICATION_ALGORITHM = -20, /* For PKCS8 & PKCS12 */
    KM_ERROR_INVALID_INPUT_LENGTH = -21,
    KM_ERROR_KEY_EXPORT_OPTIONS_INVALID = -22,
    KM_ERROR_DELEGATION_NOT_ALLOWED = -23,
    KM_ERROR_KEY_NOT_YET_VALID = -24,
    KM_ERROR_KEY_EXPIRED = -25,
    KM_ERROR_KEY_USER_NOT_AUTHENTICATED = -26,
    KM_ERROR_OUTPUT_PARAMETER_NULL = -27,
    KM_ERROR_INVALID_OPERATION_HANDLE = -28,
    KM_ERROR_INSUFFICIENT_BUFFER_SPACE = -29,
    KM_ERROR_VERIFICATION_FAILED = -30,
    KM_ERROR_TOO_MANY_OPERATIONS = -31,
    KM_ERROR_UNEXPECTED_NULL_POINTER = -32,
    KM_ERROR_INVALID_KEY_BLOB = -33,
    KM_ERROR_IMPORTED_KEY_NOT_ENCRYPTED = -34,
    KM_ERROR_IMPORTED_KEY_DECRYPTION_FAILED = -35,
    KM_ERROR_IMPORTED_KEY_NOT_SIGNED = -36,
    KM_ERROR_IMPORTED_KEY_VERIFICATION_FAILED = -37,
    KM_ERROR_INVALID_ARGUMENT = -38,
    KM_ERROR_UNSUPPORTED_TAG = -39,
    KM_ERROR_INVALID_TAG = -40,
    KM_ERROR_MEMORY_ALLOCATION_FAILED = -41,
    KM_ERROR_IMPORT_PARAMETER_MISMATCH = -44,
    KM_ERROR_SECURE_HW_ACCESS_DENIED = -45,
    KM_ERROR_OPERATION_CANCELLED = -46,
    KM_ERROR_CONCURRENT_ACCESS_CONFLICT = -47,
    KM_ERROR_SECURE_HW_BUSY = -48,
    KM_ERROR_SECURE_HW_COMMUNICATION_FAILED = -49,
    KM_ERROR_UNSUPPORTED_EC_FIELD = -50,
    KM_ERROR_MISSING_NONCE = -51,
    KM_ERROR_INVALID_NONCE = -52,
    KM_ERROR_MISSING_MAC_LENGTH = -53,
    KM_ERROR_KEY_RATE_LIMIT_EXCEEDED = -54,
    KM_ERROR_CALLER_NONCE_PROHIBITED = -55,
    KM_ERROR_KEY_MAX_OPS_EXCEEDED = -56,
    KM_ERROR_INVALID_MAC_LENGTH = -57,
    KM_ERROR_MISSING_MIN_MAC_LENGTH = -58,
    KM_ERROR_UNSUPPORTED_MIN_MAC_LENGTH = -59,
    KM_ERROR_UNSUPPORTED_KDF = -60,
    KM_ERROR_UNSUPPORTED_EC_CURVE = -61,
    KM_ERROR_KEY_REQUIRES_UPGRADE = -62,
    KM_ERROR_ATTESTATION_CHALLENGE_MISSING = -63,
    KM_ERROR_KEYMASTER_NOT_CONFIGURED = -64,
    KM_ERROR_ATTESTATION_APPLICATION_ID_MISSING = -65,
    KM_ERROR_CANNOT_ATTEST_IDS = -66,
    KM_ERROR_ROLLBACK_RESISTANCE_UNAVAILABLE = -67,
    KM_ERROR_UNIMPLEMENTED = -100,
    KM_ERROR_VERSION_MISMATCH = -101,

    KM_ERROR_UNKNOWN_ERROR = -1000,

    KM_ERROR_SOTER_ERROR = -10000,
} keymaster_error_t;

#ifdef __cplusplus
}  // extern "C"
#endif  // __cplusplus

#endif  // HARDWARE_KEYMASTER_DEFS_H
