#pragma once
/*
 * Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
 * All rights reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

/**
 * @section IPFM_Overview IPFM Overview
 * IPFM is the interface to the Platform Feature Manager (PFM),
 * which provides access to a managed collection of feature licenses.
 * PFM also refers to an ASCII-encoded license format based on PEM.
 * A complete description of feature licenses is beyond the scope of
 * this document.  From a high level, a feature license is an X.509
 * certificate with extensions that define what features are enabled
 * and under what conditions.  The extensions provide a blacklist.
 * If a value specified by any extension does not match either the query or
 * the state of the device, license validation will fail, with an error
 * code that indicates the first extension where a mismatch was detected.
 * Technically, a Feature ID or FID is an integer that identifies a feature.
 * However, the terms feature, Feature ID, and FID are used somewhat
 * interchangeably in the rest of this document.
 *
 * @subsection IPFM_CBOR CBOR
 * Many of the IPFM methods use CBOR to encode inputs or outputs.  A full
 * description of CBOR is beyond the scope of this document.  One
 * reason it is used in the API is so responses can include additional
 * data in the future without requiring changes to client code or the .idl.
 * With that in mind, client code should not depend on the exact order
 * of items in a CBOR map in a response, or the exact number of items in
 * a CBOR structure/array.  Clients should skip unexpected trailing items in
 * arrays and unknown keys in maps, and should only fail if any expected
 * fields are NOT present.
 *
 * CBOR inputs and outputs are described using both CDDL and a CBOR diagnostic
 * format, which a JSON-like text version of the CBOR data.  Square braces
 * indicate a CBOR array, which may be heterogeneous.  Curly braces
 * indicate a CBOR map.  CBOR maps used by IPFM always use text strings as
 * map key values.
 *
 * @note CBOR permits both definite- and indefinite-length arrays and maps,
 * but IPFM methods only accept and produce definite-length arrays and maps.
 * Depending on the CBOR library used by the client, client code may have to
 * provide additional options to generate definite-length arrays and maps.
 *
 * @subsection IPFM_Distinguished_Names Distinguished Names
 * The normal purpose of an X.509 certificate is for a certificate Issuer
 * to assert that a public key is owned by a Subject.  The Issuer and Subject
 * are identified by Distinguished Names.  A Distingushed Name (DN) is a series
 * of well-known attribute types (keys) and values.  Most key / value pairs are
 * optional.  Several IPFM methods return
 * the Issuer and Subject DNs in a CBOR map.  The values in the CBOR map
 * have been converted from the raw DER encoding into a CBOR map.  Both the
 * keys and values in the map are TSTR values.  The following key values are
 * supported at a minimum.
 * @li @c "O" - Organization
 * @li @c "OU" - Organizational Unit
 * @li @c "CN" - Common Name
 *
 * @subsection IPFM_Validation_Error_Codes Common Validation Error Codes
 * A number of IPFM methods return errors that indicate which restriction
 * failed to match, as described above, or a more general problem with the
 * X.509 certificate structure.  These error codes all include _CERT in
 * their names and are listed here.
 *
 * @par Restriction Mismatch Errors
 * @li @ref ERROR_CERT_PKHASH
 * @li @ref ERROR_CERT_FEATUREID
 * @li @ref ERROR_CERT_EXPIRED
 * @li @ref ERROR_CERT_OEM
 * @li @ref ERROR_CERT_HWVERSION
 * @li @ref ERROR_CERT_LICENSEE_HASH
 * @li @ref ERROR_CERT_DEVICEID
 * @li @ref ERROR_CERT_NOTYETVALID
 * @li @ref ERROR_CERT_PRODUCTID
 *
 * @par Errors for corrupted or incorrectly generated licenses
 * @li @ref ERROR_INVALID_CERT
 * @li @ref ERROR_CERT_NOT_TRUSTED
 * @li @ref ERROR_CERT_GENERAL_ERR
 * @li @ref ERROR_CERT_LEAF_IS_CA
 *
 * @subsection IPFM_Common_Error_Codes Other Common Error Codes
 * @par Object_ERROR_SIZE_OUT
 * Many IPFM methods return variable-length data in CBOR format.  If the output
 * buffer provided by the client is not large enough to hold the data,
 * Object_ERROR_SIZE_OUT will be returned, and the output buffer length will
 * be set to a value large enough to contain the data.  Note that this is
 * an upper bound on the data length, and not necessarily the exact data length.
 * If the client allocates a new buffer of this length and calls the method again
 * with the new buffer, it will succeed, and the buffer length will be set to
 * the exact data size.
 *
 * @par Object_ERROR
 * Object_ERROR can be returned from most methods.  This indicates an
 * internal error of some kind.  It may indicate a system misconfiguration.
 * For example, it can be returned if PFM can not find the persistent storage
 * for licenses. The circumstances under which the error occurred and the
 * qsee_log should be reported to the maintainers of PFM.
 *
 * @par Other error codes
 * Note that while every effort is made to correctly list the errors that
 * each method can return, the list should not be considered exhaustive.
 * A method may return error codes in addition to those listed, either now
 * or in the future.
 */

// AUTOGENERATED FILE: DO NOT EDIT

#include <cstdint>
#include "object.h"
#include "proxy_base.hpp"
#include "IDeviceAttestation.hpp"
#include "IProvisioning.hpp"
#include "IPFMCallback.hpp"

class IIPFMUpdater {
   public:
    virtual ~IIPFMUpdater() {}

    virtual int32_t Update(const void* PFMLicense_ptr, size_t PFMLicense_len) = 0;
    virtual int32_t Finalize(void* TransactionInfo_ptr, size_t TransactionInfo_len, size_t* TransactionInfo_lenout) = 0;

   protected:
    static const ObjectOp OP_Update = 0;
    static const ObjectOp OP_Finalize = 1;
};

class IPFMUpdater : public IIPFMUpdater, public ProxyBase {
   public:
    IPFMUpdater() {}
    IPFMUpdater(Object impl) : ProxyBase(impl) {}
    virtual ~IPFMUpdater() {}

    virtual int32_t Update(const void* PFMLicense_ptr, size_t PFMLicense_len) {
        ObjectArg a[1]={{{0,0}}};
        a[0].bi = (ObjectBufIn) {PFMLicense_ptr, PFMLicense_len * 1};

        return invoke(OP_Update, a, ObjectCounts_pack(1, 0, 0, 0));
    }

    virtual int32_t Finalize(void* TransactionInfo_ptr, size_t TransactionInfo_len, size_t* TransactionInfo_lenout) {
        ObjectArg a[1]={{{0,0}}};
        a[0].b = (ObjectBuf) {TransactionInfo_ptr, TransactionInfo_len * 1};

        int32_t result = invoke(OP_Finalize, a, ObjectCounts_pack(0, 1, 0, 0));
        if (Object_OK != result) { return result; }

        *TransactionInfo_lenout = a[0].b.size / 1;

        return result;
    }

};
class IIPFM {
   public:
    static const uint64_t OPT_NO_OEMID_CHECK = UINT64_C(0x00000001);
    static const uint64_t OPT_NO_HWVERSION_CHECK = UINT64_C(0x00000002);
    static const uint64_t OPT_NO_FEATUREID_CHECK = UINT64_C(0x00000004);
    static const uint64_t OPT_NO_TIME_CHECK = UINT64_C(0x00000008);
    static const uint64_t OPT_CLOCK_NOT_SET_OK = UINT64_C(0x00000010);
    static const uint64_t OPT_SELECT_TEST_ROOT = UINT64_C(0x00000020);
    static const uint64_t OPT_USAGE_ENCRYPT = UINT64_C(0x00000040);
    static const uint64_t OPT_NO_PKHASH_CHECK = UINT64_C(0x00000080);
    static const uint64_t OPT_NO_DEVICE_ID_CHECK = UINT64_C(0x00000100);
    static const uint64_t OPT_NO_START_LICENSE_DATE_CHECK = UINT64_C(0x00000200);
    static const uint64_t OPT_TRUST_INSECURE_CLK = UINT64_C(0x00000400);
    static const uint64_t OPT_NO_PRODUCTID_CHECK = UINT64_C(0x00000800);
    static const uint64_t OPT_CONSUME_GRACE_COUNT = UINT64_C(0x00001000);
    static const uint64_t OPT_DO_NOT_USE_GRACE_COUNT_FEATURE = UINT64_C(0x00002000);
    static const uint64_t OPT_SELECT_TEST_LICENSEE_HASH = UINT64_C(0x00004000);
    static const uint64_t OPT_CLIENT_IS_LICENSEE = UINT64_C(0x00008000);
    static const uint64_t LICENSE_HAS_OEMID = UINT64_C(0x00000001);
    static const uint64_t LICENSE_HAS_HWVERSION = UINT64_C(0x00000002);
    static const uint64_t LICENSE_HAS_DEVICEID = UINT64_C(0x00000004);
    static const uint64_t LICENSE_HAS_PKHASH = UINT64_C(0x00000008);
    static const uint64_t LICENSE_HAS_LICENSEEHASH = UINT64_C(0x00000010);
    static const uint64_t LICENSE_HAS_VALIDTIME = UINT64_C(0x00000020);
    static const uint64_t LICENSE_HAS_PRODUCTID = UINT64_C(0x00000040);
    static const uint64_t LICENSE_HAS_GRACE_PERIOD = UINT64_C(0x00000080);
    static const uint64_t CAPS_ATOMIC_LICENSE_OP = UINT64_C(0x00000001);
    static const uint64_t FEATURE_STATUS_ALLOWED = UINT64_C(0x00000000);
    static const uint64_t FEATURE_STATUS_NOT_ALLOWED = UINT64_C(0x00000001);
    static const uint64_t FEATURE_STATUS_PROCESSING_ERROR = UINT64_C(0x00000003);
    static const uint64_t FEATURE_STATUS_NO_LICENSES = UINT64_C(0x00000004);
    static const uint64_t FEATURE_STATUS_GRACE_ACTIVATION_NEEDED = UINT64_C(0x00000005);
    static const uint64_t TRANSACTION_STATUS_SUCCESS = UINT64_C(0x00000000);
    static const uint64_t TRANSACTION_STATUS_INCOMPLETE = UINT64_C(0x00000001);
    static const uint64_t TRANSACTION_STATUS_FAIL = UINT64_C(0x00000002);
    static const uint64_t EVENT_TYPE_STATE_CHANGE = UINT64_C(0x00000000);

    static const int32_t ERROR_NOMEM = INT32_C(10);
    static const int32_t ERROR_INVALID_PFM_EXTENSION = INT32_C(11);
    static const int32_t ERROR_CBOR_ENCODE_ERR = INT32_C(12);
    static const int32_t ERROR_CERT_PKHASH = INT32_C(13);
    static const int32_t ERROR_INVALID_CERT = INT32_C(14);
    static const int32_t ERROR_CERT_FEATUREID = INT32_C(15);
    static const int32_t ERROR_CERT_EXPIRED = INT32_C(16);
    static const int32_t ERROR_CERT_OEM = INT32_C(17);
    static const int32_t ERROR_CERT_HWVERSION = INT32_C(18);
    static const int32_t ERROR_CERT_LICENSEE_HASH = INT32_C(19);
    static const int32_t ERROR_PFMFILER_FAILED = INT32_C(20);
    static const int32_t ERROR_HASH_GENERATION = INT32_C(21);
    static const int32_t ERROR_CERT_NOT_TRUSTED = INT32_C(22);
    static const int32_t ERROR_CERT_GENERAL_ERR = INT32_C(23);
    static const int32_t ERROR_CBOR_DECODE_ERR = INT32_C(24);
    static const int32_t ERROR_CBOR_DECODE_DATATYPE_ERR = INT32_C(25);
    static const int32_t ERROR_PFMFILER_FILE_REMOVE_FAILED = INT32_C(26);
    static const int32_t ERROR_PFMCACHE_REMOVE_FAILED = INT32_C(27);
    static const int32_t ERROR_RPMB_ERR = INT32_C(28);
    static const int32_t ERROR_FILE_NOT_FOUND = INT32_C(29);
    static const int32_t ERROR_PFMFILER_GETFILECONTENTS_FAILED = INT32_C(30);
    static const int32_t ERROR_LICENSE_TOO_BIG = INT32_C(31);
    static const int32_t ERROR_PRIVILEGE_ERR = INT32_C(32);
    static const int32_t ERROR_LICENSE_STORE_FULL = INT32_C(33);
    static const int32_t ERROR_CERT_FIELD_TOO_BIG = INT32_C(34);
    static const int32_t ERROR_INVALID_CURRENT_TIME = INT32_C(35);
    static const int32_t ERROR_FID_LIST_TRUNCATED = INT32_C(36);
    static const int32_t ERROR_BLOB_ENCAP_FAILED = INT32_C(37);
    static const int32_t ERROR_BLOB_DECAP_FAILED = INT32_C(38);
    static const int32_t ERROR_CERT_DEVICEID = INT32_C(39);
    static const int32_t ERROR_SFS_ERR = INT32_C(40);
    static const int32_t ERROR_CERT_NOTYETVALID = INT32_C(41);
    static const int32_t ERROR_OPTS_NOT_SUPPORTED = INT32_C(42);
    static const int32_t ERROR_OVERFLOW = INT32_C(43);
    static const int32_t ERROR_NOT_GRACE_BOUND = INT32_C(44);
    static const int32_t ERROR_CERT_PRODUCTID = INT32_C(45);
    static const int32_t ERROR_CERT_LEAF_IS_CA = INT32_C(46);
    static const int32_t ERROR_NO_FEATURE_CONFIG = INT32_C(47);
    static const int32_t ERROR_DUPLICATE = INT32_C(48);
    static const int32_t ERROR_NO_LICENSES = INT32_C(49);
    static const int32_t ERROR_INCORRECT_LICENSEE_HASH = INT32_C(50);
    static const int32_t ERROR_ACTIVATION_NOT_NEEDED = INT32_C(51);
    static const int32_t ERROR_QWESSTORE_MEMORY_FULL = INT32_C(52);
    static const int32_t ERROR_REVOCATION_LIST = INT32_C(53);
    static const int32_t ERROR_UNSUPPORTED_REVOCATION_FID = INT32_C(54);
    static const int32_t ERROR_REVOCATION_UNSUPPORTED = INT32_C(55);
    static const int32_t ERROR_REVOCATION_LIST_FULL = INT32_C(56);
    static const int32_t ERROR_REV_LIC_ALREADY_PROCESSED = INT32_C(57);
    static const int32_t ERROR_LICENSE_ALREADY_REVOKED = INT32_C(58);
    static const int32_t ERROR_INVALID_TRANSACTION_COUNT = INT32_C(59);
    static const int32_t ERROR_REVOCATION_FID_UNAVAILABLE = INT32_C(60);
    static const int32_t ERROR_REVOKE_LICENSE = INT32_C(61);
    static const int32_t ERROR_FLUSH = INT32_C(62);
    static const int32_t ERROR_PFM_SUBMOD_REVOCATION_SERIAL = INT32_C(63);
    static const int32_t ERROR_PFM_REPORT_REVOCATION_LICENSE = INT32_C(64);
    static const int32_t ERROR_INVALID_PARAM = INT32_C(65);
    static const int32_t ERROR_GPBTTA_GRACE_EXPIRED = INT32_C(66);
    static const int32_t ERROR_GRACE_COUNT_EXHAUSTED = INT32_C(67);

    virtual ~IIPFM() {}

    virtual int32_t InstallLicense(const void* PFMLicense_ptr, size_t PFMLicense_len, void* LicenseSerialNumber_ptr, size_t LicenseSerialNumber_len, size_t* LicenseSerialNumber_lenout, void* FeatureIDsCBOR_ptr, size_t FeatureIDsCBOR_len, size_t* FeatureIDsCBOR_lenout, uint64_t* LicenseRestrictions_ptr) = 0;
    virtual int32_t CheckInstalledLicense(uint32_t FeatureID_val, const void* LicenseeHash_ptr, size_t LicenseeHash_len, void* LicenseSerialNumber_ptr, size_t LicenseSerialNumber_len, size_t* LicenseSerialNumber_lenout) = 0;
    virtual int32_t GetInstalledLicenseInfo(const void* LicenseSerialNumber_ptr, size_t LicenseSerialNumber_len, void* LicenseInfoCBOR_ptr, size_t LicenseInfoCBOR_len, size_t* LicenseInfoCBOR_lenout) = 0;
    virtual int32_t CheckLicenseBuffer(const void* PFMLicense_ptr, size_t PFMLicense_len, uint32_t FeatureID_val, const void* LicenseeHash_ptr, size_t LicenseeHash_len, void* LicenseBufferCBOR_ptr, size_t LicenseBufferCBOR_len, size_t* LicenseBufferCBOR_lenout) = 0;
    virtual int32_t CheckSecured(const void* SecuredRequest_ptr, size_t SecuredRequest_len, void* SecuredResponse_ptr, size_t SecuredResponse_len, size_t* SecuredResponse_lenout) = 0;
    virtual int32_t GetAllInstalledFeatureIDs(void* FeatureIDsCBOR_ptr, size_t FeatureIDsCBOR_len, size_t* FeatureIDsCBOR_lenout) = 0;
    virtual int32_t GetAllInstalledSerialNumbers(void* SerialNumbersCBOR_ptr, size_t SerialNumbersCBOR_len, size_t* SerialNumbersCBOR_lenout) = 0;
    virtual int32_t CheckFeatureIds(const void* RequestCBOR_ptr, size_t RequestCBOR_len, void* ResponseCBOR_ptr, size_t ResponseCBOR_len, size_t* ResponseCBOR_lenout) = 0;
    virtual int32_t CheckFIDAndGetAllSerialNums(const void* RequestCBOR_ptr, size_t RequestCBOR_len, void* ResponseCBOR_ptr, size_t ResponseCBOR_len, size_t* ResponseCBOR_lenout) = 0;
    virtual int32_t SetOptions(uint64_t uOpts_val) = 0;
    virtual int32_t RemoveLicense(const void* LicenseSerialNumber_ptr, size_t LicenseSerialNumber_len) = 0;
    virtual int32_t RemoveLicenseExpired() = 0;
    virtual int32_t GetLicenseCertPFM(const void* LicenseSerialNumber_ptr, size_t LicenseSerialNumber_len, void* PFMLicense_ptr, size_t PFMLicense_len, size_t* PFMLicense_lenout) = 0;
    virtual int32_t GetFeatureConfig(uint64_t* fuses_ptr) = 0;
    virtual int32_t SetTrustedTime(uint64_t time_val, uint32_t trust_level_val) = 0;
    virtual int32_t GetNextExpiration(uint64_t* expiration_ptr) = 0;
    virtual int32_t CheckFIDAndGetAllWithGrace(const void* RequestCBOR_ptr, size_t RequestCBOR_len, void* ResponseCBOR_ptr, size_t ResponseCBOR_len, size_t* ResponseCBOR_lenout) = 0;
    virtual int32_t ActivateGracePeriod(const void* LicenseSerialNumber_ptr, size_t LicenseSerialNumber_len) = 0;
    virtual int32_t GeneratePFMReport(const void* license_ptr, size_t license_len, uint32_t options_val, IAttestationBuilder &attestation_builder_ref) = 0;
    virtual int32_t CreateProvisioning(IProvisioning &Provisioning_ref) = 0;
    virtual int32_t BeginUpdate(IPFMUpdater &Updater_ref) = 0;
    virtual int32_t GetAppCapabilities(uint64_t* caps_ptr) = 0;
    virtual int32_t RegisterCallback(const IPFMCallback &Callback_ref, const void* FidsCBOR_ptr, size_t FidsCBOR_len, void* ResponseCBOR_ptr, size_t ResponseCBOR_len, size_t* ResponseCBOR_lenout, ProxyBase &CallbackHandle_ref) = 0;

   protected:
    static const ObjectOp OP_InstallLicense = 0;
    static const ObjectOp OP_CheckInstalledLicense = 1;
    static const ObjectOp OP_GetInstalledLicenseInfo = 2;
    static const ObjectOp OP_CheckLicenseBuffer = 3;
    static const ObjectOp OP_CheckSecured = 4;
    static const ObjectOp OP_GetAllInstalledFeatureIDs = 5;
    static const ObjectOp OP_GetAllInstalledSerialNumbers = 6;
    static const ObjectOp OP_CheckFeatureIds = 7;
    static const ObjectOp OP_CheckFIDAndGetAllSerialNums = 8;
    static const ObjectOp OP_SetOptions = 9;
    static const ObjectOp OP_RemoveLicense = 10;
    static const ObjectOp OP_RemoveLicenseExpired = 11;
    static const ObjectOp OP_GetLicenseCertPFM = 12;
    static const ObjectOp OP_GetFeatureConfig = 13;
    static const ObjectOp OP_SetTrustedTime = 14;
    static const ObjectOp OP_GetNextExpiration = 15;
    static const ObjectOp OP_CheckFIDAndGetAllWithGrace = 16;
    static const ObjectOp OP_ActivateGracePeriod = 17;
    static const ObjectOp OP_GeneratePFMReport = 18;
    static const ObjectOp OP_CreateProvisioning = 19;
    static const ObjectOp OP_BeginUpdate = 20;
    static const ObjectOp OP_GetAppCapabilities = 21;
    static const ObjectOp OP_RegisterCallback = 22;
};

class IPFM : public IIPFM, public ProxyBase {
   public:
    IPFM() {}
    IPFM(Object impl) : ProxyBase(impl) {}
    virtual ~IPFM() {}

    virtual int32_t InstallLicense(const void* PFMLicense_ptr, size_t PFMLicense_len, void* LicenseSerialNumber_ptr, size_t LicenseSerialNumber_len, size_t* LicenseSerialNumber_lenout, void* FeatureIDsCBOR_ptr, size_t FeatureIDsCBOR_len, size_t* FeatureIDsCBOR_lenout, uint64_t* LicenseRestrictions_ptr) {
        ObjectArg a[4]={{{0,0}}};
        a[0].bi = (ObjectBufIn) {PFMLicense_ptr, PFMLicense_len * 1};
        a[1].b = (ObjectBuf) {LicenseSerialNumber_ptr, LicenseSerialNumber_len * 1};
        a[2].b = (ObjectBuf) {FeatureIDsCBOR_ptr, FeatureIDsCBOR_len * 1};
        a[3].b = (ObjectBuf) {LicenseRestrictions_ptr, sizeof(uint64_t)};

        int32_t result = invoke(OP_InstallLicense, a, ObjectCounts_pack(1, 3, 0, 0));
        if (Object_OK != result) { return result; }

        *LicenseSerialNumber_lenout = a[1].b.size / 1;
        *FeatureIDsCBOR_lenout = a[2].b.size / 1;

        return result;
    }

    virtual int32_t CheckInstalledLicense(uint32_t FeatureID_val, const void* LicenseeHash_ptr, size_t LicenseeHash_len, void* LicenseSerialNumber_ptr, size_t LicenseSerialNumber_len, size_t* LicenseSerialNumber_lenout) {
        ObjectArg a[3]={{{0,0}}};
        a[0].b = (ObjectBuf) {&FeatureID_val, sizeof(uint32_t)};
        a[1].bi = (ObjectBufIn) {LicenseeHash_ptr, LicenseeHash_len * 1};
        a[2].b = (ObjectBuf) {LicenseSerialNumber_ptr, LicenseSerialNumber_len * 1};

        int32_t result = invoke(OP_CheckInstalledLicense, a, ObjectCounts_pack(2, 1, 0, 0));
        if (Object_OK != result) { return result; }

        *LicenseSerialNumber_lenout = a[2].b.size / 1;

        return result;
    }

    virtual int32_t GetInstalledLicenseInfo(const void* LicenseSerialNumber_ptr, size_t LicenseSerialNumber_len, void* LicenseInfoCBOR_ptr, size_t LicenseInfoCBOR_len, size_t* LicenseInfoCBOR_lenout) {
        ObjectArg a[2]={{{0,0}}};
        a[0].bi = (ObjectBufIn) {LicenseSerialNumber_ptr, LicenseSerialNumber_len * 1};
        a[1].b = (ObjectBuf) {LicenseInfoCBOR_ptr, LicenseInfoCBOR_len * 1};

        int32_t result = invoke(OP_GetInstalledLicenseInfo, a, ObjectCounts_pack(1, 1, 0, 0));
        if (Object_OK != result) { return result; }

        *LicenseInfoCBOR_lenout = a[1].b.size / 1;

        return result;
    }

    virtual int32_t CheckLicenseBuffer(const void* PFMLicense_ptr, size_t PFMLicense_len, uint32_t FeatureID_val, const void* LicenseeHash_ptr, size_t LicenseeHash_len, void* LicenseBufferCBOR_ptr, size_t LicenseBufferCBOR_len, size_t* LicenseBufferCBOR_lenout) {
        ObjectArg a[4]={{{0,0}}};
        a[0].bi = (ObjectBufIn) {PFMLicense_ptr, PFMLicense_len * 1};
        a[1].b = (ObjectBuf) {&FeatureID_val, sizeof(uint32_t)};
        a[2].bi = (ObjectBufIn) {LicenseeHash_ptr, LicenseeHash_len * 1};
        a[3].b = (ObjectBuf) {LicenseBufferCBOR_ptr, LicenseBufferCBOR_len * 1};

        int32_t result = invoke(OP_CheckLicenseBuffer, a, ObjectCounts_pack(3, 1, 0, 0));
        if (Object_OK != result) { return result; }

        *LicenseBufferCBOR_lenout = a[3].b.size / 1;

        return result;
    }

    virtual int32_t CheckSecured(const void* SecuredRequest_ptr, size_t SecuredRequest_len, void* SecuredResponse_ptr, size_t SecuredResponse_len, size_t* SecuredResponse_lenout) {
        ObjectArg a[2]={{{0,0}}};
        a[0].bi = (ObjectBufIn) {SecuredRequest_ptr, SecuredRequest_len * 1};
        a[1].b = (ObjectBuf) {SecuredResponse_ptr, SecuredResponse_len * 1};

        int32_t result = invoke(OP_CheckSecured, a, ObjectCounts_pack(1, 1, 0, 0));
        if (Object_OK != result) { return result; }

        *SecuredResponse_lenout = a[1].b.size / 1;

        return result;
    }

    virtual int32_t GetAllInstalledFeatureIDs(void* FeatureIDsCBOR_ptr, size_t FeatureIDsCBOR_len, size_t* FeatureIDsCBOR_lenout) {
        ObjectArg a[1]={{{0,0}}};
        a[0].b = (ObjectBuf) {FeatureIDsCBOR_ptr, FeatureIDsCBOR_len * 1};

        int32_t result = invoke(OP_GetAllInstalledFeatureIDs, a, ObjectCounts_pack(0, 1, 0, 0));
        if (Object_OK != result) { return result; }

        *FeatureIDsCBOR_lenout = a[0].b.size / 1;

        return result;
    }

    virtual int32_t GetAllInstalledSerialNumbers(void* SerialNumbersCBOR_ptr, size_t SerialNumbersCBOR_len, size_t* SerialNumbersCBOR_lenout) {
        ObjectArg a[1]={{{0,0}}};
        a[0].b = (ObjectBuf) {SerialNumbersCBOR_ptr, SerialNumbersCBOR_len * 1};

        int32_t result = invoke(OP_GetAllInstalledSerialNumbers, a, ObjectCounts_pack(0, 1, 0, 0));
        if (Object_OK != result) { return result; }

        *SerialNumbersCBOR_lenout = a[0].b.size / 1;

        return result;
    }

    virtual int32_t CheckFeatureIds(const void* RequestCBOR_ptr, size_t RequestCBOR_len, void* ResponseCBOR_ptr, size_t ResponseCBOR_len, size_t* ResponseCBOR_lenout) {
        ObjectArg a[2]={{{0,0}}};
        a[0].bi = (ObjectBufIn) {RequestCBOR_ptr, RequestCBOR_len * 1};
        a[1].b = (ObjectBuf) {ResponseCBOR_ptr, ResponseCBOR_len * 1};

        int32_t result = invoke(OP_CheckFeatureIds, a, ObjectCounts_pack(1, 1, 0, 0));
        if (Object_OK != result) { return result; }

        *ResponseCBOR_lenout = a[1].b.size / 1;

        return result;
    }

    virtual int32_t CheckFIDAndGetAllSerialNums(const void* RequestCBOR_ptr, size_t RequestCBOR_len, void* ResponseCBOR_ptr, size_t ResponseCBOR_len, size_t* ResponseCBOR_lenout) {
        ObjectArg a[2]={{{0,0}}};
        a[0].bi = (ObjectBufIn) {RequestCBOR_ptr, RequestCBOR_len * 1};
        a[1].b = (ObjectBuf) {ResponseCBOR_ptr, ResponseCBOR_len * 1};

        int32_t result = invoke(OP_CheckFIDAndGetAllSerialNums, a, ObjectCounts_pack(1, 1, 0, 0));
        if (Object_OK != result) { return result; }

        *ResponseCBOR_lenout = a[1].b.size / 1;

        return result;
    }

    virtual int32_t SetOptions(uint64_t uOpts_val) {
        ObjectArg a[1]={{{0,0}}};
        a[0].b = (ObjectBuf) {&uOpts_val, sizeof(uint64_t)};

        return invoke(OP_SetOptions, a, ObjectCounts_pack(1, 0, 0, 0));
    }

    virtual int32_t RemoveLicense(const void* LicenseSerialNumber_ptr, size_t LicenseSerialNumber_len) {
        ObjectArg a[1]={{{0,0}}};
        a[0].bi = (ObjectBufIn) {LicenseSerialNumber_ptr, LicenseSerialNumber_len * 1};

        return invoke(OP_RemoveLicense, a, ObjectCounts_pack(1, 0, 0, 0));
    }

    virtual int32_t RemoveLicenseExpired() {
        return invoke(OP_RemoveLicenseExpired, 0, 0);
    }

    virtual int32_t GetLicenseCertPFM(const void* LicenseSerialNumber_ptr, size_t LicenseSerialNumber_len, void* PFMLicense_ptr, size_t PFMLicense_len, size_t* PFMLicense_lenout) {
        ObjectArg a[2]={{{0,0}}};
        a[0].bi = (ObjectBufIn) {LicenseSerialNumber_ptr, LicenseSerialNumber_len * 1};
        a[1].b = (ObjectBuf) {PFMLicense_ptr, PFMLicense_len * 1};

        int32_t result = invoke(OP_GetLicenseCertPFM, a, ObjectCounts_pack(1, 1, 0, 0));
        if (Object_OK != result) { return result; }

        *PFMLicense_lenout = a[1].b.size / 1;

        return result;
    }

    virtual int32_t GetFeatureConfig(uint64_t* fuses_ptr) {
        ObjectArg a[1]={{{0,0}}};
        a[0].b = (ObjectBuf) {fuses_ptr, sizeof(uint64_t)};

        return invoke(OP_GetFeatureConfig, a, ObjectCounts_pack(0, 1, 0, 0));
    }

    virtual int32_t SetTrustedTime(uint64_t time_val, uint32_t trust_level_val) {
        ObjectArg a[1]={{{0,0}}};
        struct s1 {
            uint64_t m_time;
            uint32_t m_trust_level;
        } i;
        a[0].b = (ObjectBuf) {&i, 12};
        i.m_time = time_val;
        i.m_trust_level = trust_level_val;

        return invoke(OP_SetTrustedTime, a, ObjectCounts_pack(1, 0, 0, 0));
    }

    virtual int32_t GetNextExpiration(uint64_t* expiration_ptr) {
        ObjectArg a[1]={{{0,0}}};
        a[0].b = (ObjectBuf) {expiration_ptr, sizeof(uint64_t)};

        return invoke(OP_GetNextExpiration, a, ObjectCounts_pack(0, 1, 0, 0));
    }

    virtual int32_t CheckFIDAndGetAllWithGrace(const void* RequestCBOR_ptr, size_t RequestCBOR_len, void* ResponseCBOR_ptr, size_t ResponseCBOR_len, size_t* ResponseCBOR_lenout) {
        ObjectArg a[2]={{{0,0}}};
        a[0].bi = (ObjectBufIn) {RequestCBOR_ptr, RequestCBOR_len * 1};
        a[1].b = (ObjectBuf) {ResponseCBOR_ptr, ResponseCBOR_len * 1};

        int32_t result = invoke(OP_CheckFIDAndGetAllWithGrace, a, ObjectCounts_pack(1, 1, 0, 0));
        if (Object_OK != result) { return result; }

        *ResponseCBOR_lenout = a[1].b.size / 1;

        return result;
    }

    virtual int32_t ActivateGracePeriod(const void* LicenseSerialNumber_ptr, size_t LicenseSerialNumber_len) {
        ObjectArg a[1]={{{0,0}}};
        a[0].bi = (ObjectBufIn) {LicenseSerialNumber_ptr, LicenseSerialNumber_len * 1};

        return invoke(OP_ActivateGracePeriod, a, ObjectCounts_pack(1, 0, 0, 0));
    }

    virtual int32_t GeneratePFMReport(const void* license_ptr, size_t license_len, uint32_t options_val, IAttestationBuilder &attestation_builder_ref) {
        ObjectArg a[3]={{{0,0}}};
        a[0].bi = (ObjectBufIn) {license_ptr, license_len * 1};
        a[1].b = (ObjectBuf) {&options_val, sizeof(uint32_t)};

        int32_t result = invoke(OP_GeneratePFMReport, a, ObjectCounts_pack(2, 0, 0, 1));
        if (Object_OK != result) { return result; }

        attestation_builder_ref.consume(a[2].o);

        return result;
    }

    virtual int32_t CreateProvisioning(IProvisioning &Provisioning_ref) {
        ObjectArg a[1]={{{0,0}}};

        int32_t result = invoke(OP_CreateProvisioning, a, ObjectCounts_pack(0, 0, 0, 1));
        if (Object_OK != result) { return result; }

        Provisioning_ref.consume(a[0].o);

        return result;
    }

    virtual int32_t BeginUpdate(IPFMUpdater &Updater_ref) {
        ObjectArg a[1]={{{0,0}}};

        int32_t result = invoke(OP_BeginUpdate, a, ObjectCounts_pack(0, 0, 0, 1));
        if (Object_OK != result) { return result; }

        Updater_ref.consume(a[0].o);

        return result;
    }

    virtual int32_t GetAppCapabilities(uint64_t* caps_ptr) {
        ObjectArg a[1]={{{0,0}}};
        a[0].b = (ObjectBuf) {caps_ptr, sizeof(uint64_t)};

        return invoke(OP_GetAppCapabilities, a, ObjectCounts_pack(0, 1, 0, 0));
    }

    virtual int32_t RegisterCallback(const IPFMCallback &Callback_ref, const void* FidsCBOR_ptr, size_t FidsCBOR_len, void* ResponseCBOR_ptr, size_t ResponseCBOR_len, size_t* ResponseCBOR_lenout, ProxyBase &CallbackHandle_ref) {
        ObjectArg a[4]={{{0,0}}};
        a[2].o = Callback_ref.get();
        a[0].bi = (ObjectBufIn) {FidsCBOR_ptr, FidsCBOR_len * 1};
        a[1].b = (ObjectBuf) {ResponseCBOR_ptr, ResponseCBOR_len * 1};

        int32_t result = invoke(OP_RegisterCallback, a, ObjectCounts_pack(1, 1, 1, 1));
        if (Object_OK != result) { return result; }

        *ResponseCBOR_lenout = a[1].b.size / 1;
        CallbackHandle_ref.consume(a[3].o);

        return result;
    }

};

