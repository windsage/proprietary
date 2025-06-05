/***********************************************************************
 * Copyright (c) 2023-2024 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 **********************************************************************/
#include <openssl/sha.h>
#include <stdbool.h>

#include "HexUtils.h"
#include "ICredentials.h"
#include "ICredentials_invoke.h"
#include "MinkTypes.h"
#include "TUtils.h"
#include "heap.h"
#include "memscpy.h"
#include "object.h"
#include "osIndCredentials.h"
#include "qcbor.h"

/*****************************************************************************
 * osIndCredentials context
 ****************************************************************************/

#define CBORCredentials 32
#define defaultByteArraySize 32
#define SCRATCH_PAD_SIZE 64
#define NOT_USED 0
#define NOT_USED_POINTER NULL

// NOTE: _generate_uuid utils will need to be moved to securemsm-noship
#define MAX_UID_STR_LENGTH (10 + 1)
#define UUID_STRING_LENGTH 36
#define RFC_VARIANT_DCE 1
#define RFC_VARIANT_SHIFT 5
#define RFC_VARIANT_MASK 0x7
#define RFC_TYPE_DCE_SHA1 5
#define RFC_TYPE_SHIFT 4
#define RFC_TYPE_MASK 0xf

struct uuid_t {
    union {
        uint8_t val[VMUUID_MAX_SIZE];  // UUID is a 128 bit value
        struct {
            uint32_t timeLow;
            uint16_t timeMid;
            uint16_t timeHiAndVersion;
            uint8_t clockSeqAndNode[clockSeqAndNodeSize];
        } fields;
    };
};

// Define namespace UUID's to be used to generate UUID's
/* use command : "cat /proc/sys/kernel/random/uuid" to generate a random UUID as
 * the namespace. */
// AppUUIDNamespace = "119EB897-88A3-4F79-85E5-1C440F977ED8"
const uuid_t AppUUIDNamespace = {
    .fields = {0x119eb897,
               0x88a3,
               0x4f79,
               {0x85, 0xe5, 0x1c, 0x44, 0x0f, 0x97, 0x7e, 0xd8}}};
// "a061e17a-91e7-47f5-a990-9610cf4370f6" made using cat
// /proc/sys/kernel/random/uuid on Ubuntu
const uuid_t ProcessIDNamespace = {
    .fields = {0xa061e17a,
               0x91e7,
               0x47f5,
               {0xa9, 0x90, 0x96, 0x10, 0xcf, 0x43, 0x70, 0xf6}}};
// "800a8065-e93d-45c1-8be0-f2ac7679c0a0" made using cat
// /proc/sys/kernel/random/uuid on Ubuntu
const uuid_t UserIdNamespace = {
    .fields = {0x800a8065,
               0xe93d,
               0x45c1,
               {0x8b, 0xe0, 0xf2, 0xac, 0x76, 0x79, 0xc0, 0xa0}}};
// vmUUID will come from the hypervisor
// "3d28b89a-79c7-4f11-9703-b544c6085bfc" made using cat
// /proc/sys/kernel/random/uuid on Ubuntu
const uuid_t VM_uuid_t = {
    .fields = {0x3d28b89a,
               0x79c7,
               0x4f11,
               {0x97, 0x03, 0xb5, 0x44, 0xc6, 0x08, 0x5b, 0xfc}}};

typedef struct {
    int32_t refs;
    Object procCred;  // process
    Object envCred;   // env
} OSIndCredentials;

typedef struct {
    int32_t refs;
    int32_t flags;
    // process specific members
    char *appName;
    uuid_t appId;
    SHA256Hash appHash;
    uint32_t appDebug;
    uint32_t appVersion;
    uint8_t legacyCBOR[CBORCredentials];
    DistId distId;
    char *distName;
    char *domain;
    uuid_t pid;
    uuid_t uid;
    uint32_t securityLvl;
    uint64_t permissions;
} ProcessCred;

typedef struct {
    int32_t refs;
    int32_t flags;
    // env specific members
    char *osId;
    uuid_t vmUUID;
    DistId vmDID;
    SHA256Hash vmHash;
    char *vmDomain;
    char *vmDistUUID;
    uint32_t vmVersion;
} EnvCred;

// Definition of supported information keys
enum {
    // Sideloaded keys
    // Process
    eAppName = 'n',
    eAppID = ('a' << 24 | 'p' << 16 | 'i' << 8 | 'd'),
    eAppHash = ('a' << 24 | 'h' << 16 | 's' << 8 | 'h'),
    eAppDebug = ('a' << 24 | 'd' << 16 | 'b' << 8 | 'g'),
    eAppVersion = ('a' << 24 | 'v' << 16 | 'e' << 8 | 'r'),
    eLegacyCBOR = ('c' << 24 | 'c' << 16 | 'r' << 8 | 'd'),
    eDistinguishedID = ('d' << 16 | 'i' << 8 | 'd'),
    eDistName = ('d' << 8 | 'n'),
    eDomain = ('d' << 16 | 'm' << 8 | 'n'),
    eUID = ('u' << 16 | 'i' << 8 | 'd'),
    ePID = ('p' << 16 | 'i' << 8 | 'd'),
    eSecurityLevel = ('s' << 24 | 'e' << 16 | 'c' << 8 | 'l'),
    ePerm = ('p' << 24 | 'e' << 16 | 'r' << 8 | 'm'),
    // VM
    eOSid = ('o' << 8 | 's'),
    eVmUUID = ('v' << 8 | 'm'),
    eVmDID = ('v' << 24 | 'd' << 16 | 'i' << 8 | 'd'),
    eVmHash = ('v' << 24 | 'h' << 16 | 's' << 8 | 'h'),
    eVmDomain = ('v' << 16 | 'm' << 8 | 'd'),
    eVmDistUUID = ('v' << 24 | 'm' << 16 | 'd' << 8 | 'u'),
    eVmVersion = ('v' << 16 | 'm' << 8 | 'v'),

} OSind_eNumKeys;

// Bitmask position for OSIndCredentials->flags
enum {
    // index used for internal flags to check if set or not
    // process cred
    indAppName,
    indAppID,  // generated
    indAppHash,
    indAppDebug,
    indAppVersion,
    indLegacyCBOR,  // vendor: generated ------- system: input
    indDistID,      // generated
    indDistName,    // generated
    indDomain,      // vendor: hardcoded to OEM sign, ------  system: input
    indUID,
    indPID,
    indSecurityLevel,  // generated
    indPerm,

    // env
    indOSid,
    indVmUUID,  // input from hypervisor
    indVmDID,   // generated
    indVmHash,
    indVmDomain,
    indVmDistUUID,  // generated
    indVmVersion,
    endOfIndex,

    // define boundary of proc and enum
    firstProcCredIndex = indAppName,
    lastProcCredIndex = indPerm,
    firstEnvCredIndex = indOSid,
    lastEnvCredIndex = indVmVersion,

} OSind_FLAGS;

// used to convert index to key (string)
static const char *indToEKeyChar[] = {"n",    "apid", "ahsh", "adbg", "aver",
                                      "ccrd", "did",  "dn",   "dmn",  "uid",
                                      "pid",  "secl", "perm", "os",   "vm",
                                      "vdid", "vhsh", "vmd",  "vmdu", "vmv"};

// maps struct field to which test needs to run (one case for all uint32_t, one
// for all char *, etc.)
enum {
    caseCharPointer,
    case_uint32_t,
    case_uint64_t,
    case_memcmp,
};

#ifdef VERBOSE_DEBUG
// static map from index to switch case depending on type char[], uint8_t[],
// uin32_t, etc
static uint32_t mapIndexToCaseType[] = {
    caseCharPointer, case_memcmp,     case_memcmp,     case_uint32_t,
    case_uint32_t,   case_memcmp,     case_memcmp,     caseCharPointer,
    caseCharPointer, case_memcmp,     case_memcmp,     case_uint32_t,
    case_uint64_t,   caseCharPointer, case_memcmp,     case_memcmp,
    case_memcmp,     caseCharPointer, caseCharPointer, case_uint32_t};
#endif

/**
 * Description: converts a uuid (16 bytes) to a uuid string (hexadecimal)
 *
 * In:          uuid: uuid (16 bytes)
 *              len: size of uuidCharArray
 *
 *
 * Out:         uuidCharArray: uuid string (hexadecimal)
 */
static void convert_uuid_to_string(uuid_t *uuid, char *uuidCharArray,
                                   size_t len)
{
    if (!uuid || !uuidCharArray || UUID_STRING_LENGTH + 1 > len) {
        return;
    }

    snprintf(uuidCharArray, len,
             "%08X-%04X-%04X-%02hhX%02hhX-%02hhX%02hhX%02hhX%02hhX%02hhX%02hhX",
             uuid->fields.timeLow, uuid->fields.timeMid,
             uuid->fields.timeHiAndVersion, uuid->fields.clockSeqAndNode[0],
             uuid->fields.clockSeqAndNode[1], uuid->fields.clockSeqAndNode[2],
             uuid->fields.clockSeqAndNode[3], uuid->fields.clockSeqAndNode[4],
             uuid->fields.clockSeqAndNode[5], uuid->fields.clockSeqAndNode[6],
             uuid->fields.clockSeqAndNode[7]);

    return;
}

/**
 * Description: Generate a UUID that is compliant with the RFC4122. (from
 *              UIDLookupListener.cpp)
 *
 * In:          ns: namespace uuid
 *              name: string containing the name (ex. "10" for pid=10)
 *              len: length of name string
 *              me: OSIndCredentials struct pointer
 *              index: OSIndCredentials struct index to set in me->flags
 *
 * Out:         out: generated uuid (ex. &me->appId)
 *
 * Return:      Object_OK on success.
 *              All else on failure.
 */
static uint32_t _generate_uuid(uuid_t *out, const uuid_t *ns, const char *name,
                               size_t len, int32_t *flags, uint32_t index)
{
    SHA_CTX ctx;
    const unsigned char variant[] = {0x00, 0x04, 0x06, 0x07};
    uint8_t hash[SHA_DIGEST_LENGTH];

    if (!out || !ns || !name) {
        return ICredentials_ERROR_NOT_FOUND;
    }

    SHA1_Init(&ctx);
    SHA1_Update(&ctx, (const uint8_t *)ns, sizeof(uuid_t));
    SHA1_Update(&ctx, (const uint8_t *)name, len);
    SHA1_Final((unsigned char *)hash, &ctx);

    memcpy(out, hash, sizeof(uuid_t));

    ((uint8_t *)out)[6] &= ~(RFC_TYPE_MASK << RFC_TYPE_SHIFT);
    ((uint8_t *)out)[6] |= (RFC_TYPE_DCE_SHA1 << RFC_TYPE_SHIFT);
    ((uint8_t *)out)[8] &= ~(RFC_VARIANT_MASK << RFC_VARIANT_SHIFT);
    ((uint8_t *)out)[8] |= (variant[RFC_VARIANT_DCE] << RFC_VARIANT_SHIFT);

    // set UUID flag that was generated
    BIT_SET(*flags, index);
    return Object_OK;
}

/*****************************************************************************
 * local functions
 ****************************************************************************/

/**
 * Description: Convert a 1-4 letter string into a ID by using the character
 *              code shifted into a 32bit value based on the characters
 *              position.
 *
 * In:          key: String to convert
 *              length: Length of key
 *
 *  Return:     A positive integer or 0 on failure.
 */
static inline unsigned int _strKey(const char *key, size_t length)
{
    if (0 == length || sizeof(unsigned int) < length) {
        return 0;
    }

    size_t idx = 0;
    uint32_t key_val = 0;

    while (key[idx] && idx < length) {
        key_val <<= 8;

        key_val |= key[idx++];
    };

    return key_val;
}

#ifdef VERBOSE_DEBUG
/**
 * Description: prints a byte orray in hexadecimal values
 *
 * In:          arr: pointer to the byte array
 *              size: size of the array (number of bytes)
 *              index: the index of the struct field
 */
static void _printBytesAsHex(uint8_t *arr, int32_t size, int32_t index)
{
    char output[(size * 3) + 1];
    char *ptr = &output[0];
    for (int32_t i = 0; i < size; i++) {
        ptr += snprintf(ptr, sizeof("%02X:"), "%02X:", arr[i]);
    }
    LOG_MSG("\t\teKey: %s \tvalue: %s", indToEKeyChar[index], output);
}

/**
 * Description: prints the struct field by type (mapIndexToCaseType is used to
 *              determine the type)
 *
 * In:          index: OSIndCredentials struct field index
 *              pValue: value of the struct field
 */
static void _printAccordingToType(int index, void *pValue)
{
    switch (mapIndexToCaseType[index]) {
        case caseCharPointer:
            LOG_MSG("\teKey: %s, \tvalue: %s", indToEKeyChar[index],
                    (char *)pValue);
            break;
        case case_uint32_t:
            LOG_MSG("\teKey: %s, \tvalue: %u", indToEKeyChar[index],
                    *(uint32_t *)pValue);
            break;
        case case_uint64_t:
            LOG_MSG("\teKey: %s, \tvalue: %lu", indToEKeyChar[index],
                    *(uint64_t *)pValue);
            break;
        case case_memcmp:
            if (index == indAppID || index == indPID || index == indUID ||
                index == indVmUUID) {
                char uuid_str[UUID_STRING_LENGTH + 1];
                convert_uuid_to_string((uuid_t *)pValue, uuid_str,
                                       sizeof(uuid_str));
                LOG_MSG("\teKey: %s, \tvalue: %s", indToEKeyChar[index],
                        uuid_str);
            } else {
                _printBytesAsHex((uint8_t *)pValue, defaultByteArraySize,
                                 index);
            }
            break;

        default:
            break;
    }
}
#endif

#ifdef QCBOR_LA
/**
 * Description: Generates the CBOR blob (containing the credentials)
 *              this is only needed for LA
 *
 * In:          me: pointer to OSIndCredentials struct
 */
static int32_t _generate_CBOR(ProcessCred *me, uint32_t *encodedBufLen)
{
    QCBOREncodeContext ECtx;
    int32_t ret = Object_OK;

    uint8_t pEncoded[2000];
    size_t nEncodedLen = sizeof(pEncoded);

    QCBOREncode_Init(&ECtx, pEncoded, nEncodedLen);
    QCBOREncode_OpenMap(&ECtx);

    // proccess
    QCBOREncode_AddTextToMapN(&ECtx, indAppName,
                              SZLiteralToUsefulBufC(me->appName));
    QCBOREncode_AddBinaryUUIDToMapN(
        &ECtx, indAppID, ByteArrayLiteralToUsefulBufC(me->appId.val));
    QCBOREncode_AddBytesToMapN(
        &ECtx, indAppHash,
        ((UsefulBufC){me->appHash.val, sizeof(me->appHash)}));
    QCBOREncode_AddUInt64ToMapN(&ECtx, indAppDebug, me->appDebug);
    QCBOREncode_AddUInt64ToMapN(&ECtx, indAppVersion, me->appVersion);
    QCBOREncode_AddBytesToMapN(
        &ECtx, indDistID,
        ((UsefulBufC){me->distId.val, sizeof(me->distId.val)}));
    QCBOREncode_AddTextToMapN(&ECtx, indDistName,
                              SZLiteralToUsefulBufC(me->distName));
    QCBOREncode_AddTextToMapN(&ECtx, indDomain,
                              SZLiteralToUsefulBufC(me->domain));
    QCBOREncode_AddBinaryUUIDToMapN(&ECtx, indPID,
                                    ByteArrayLiteralToUsefulBufC(me->pid.val));
    QCBOREncode_AddBinaryUUIDToMapN(&ECtx, indUID,
                                    ByteArrayLiteralToUsefulBufC(me->uid.val));
    QCBOREncode_AddUInt64ToMapN(&ECtx, indSecurityLevel, me->securityLvl);
    QCBOREncode_AddUInt64ToMapN(&ECtx, indPerm, me->permissions);

    QCBOREncode_CloseMap(&ECtx);
    EncodedCBOR Enc;

    T_CHECK_ERR(QCBOREncode_Finish2(&ECtx, &Enc), QCBOR_ERR_BUFFER_TOO_SMALL);
    *encodedBufLen = Enc.Bytes.len;
exit:

    return ret;
}
#endif

/**
 * Description: Sets a char * field in the OSIndCredentials struct. Uses
 *              heap_zalloc to allocate memory, then memscpy to copy the
 *              valueIn to the field, and finishes by using BIT_SET to set
 *              the flag
 *
 * In:          me: OSIndCredentials struct pointer
 *              index: index of Char * field to set
 *              field: address of struct field (ex. &me->appName)
 *              valueIn: string to set the field to
 *
 * Return:      Object_OK on success.
 *              All else on failure.
 */
static int32_t _setCharField(int32_t *flags, uint32_t index, char **field,
                             char const *valueIn)
{
    int32_t ret = Object_OK;
    // Check to see if the flag is set and to see if there are any null pointers
    T_CHECK_ERR(!BIT_TST(*flags, index), Object_ERROR);
    T_CHECK_ERR(field, ICredentials_ERROR_NOT_FOUND);
    T_CHECK_ERR(valueIn, ICredentials_ERROR_NOT_FOUND);

    size_t words_len = strlen(valueIn) + 1;
    *field = (char *)heap_zalloc(words_len);
    T_CHECK_ERR(NULL != *field, Object_ERROR_MEM);

    words_len = words_len - 1;
    memscpy(*field, words_len, valueIn, words_len);
    char *ptr = *field;
    ptr[words_len] = '\0';

    BIT_SET(*flags, index);
exit:
    if (Object_isERROR(ret)) {
        BIT_CLR(*flags, index);
    }
    return ret;
}

/**
 * Description: Creates a Char* that points to the concatenation of values
 *
 * In:          me: OSIndCredentials struct pointer
 *              index_set: index of the struct field to set
 *              index_1: struct field index of first value
 *              val_1: pointer to string value
 *              index_2: struct field index of the second value
 *              val_2: pointer to the byte array
 *
 * Out:         addr_set: address of struct field to set (ex. &me->distName)
 *
 * Examples:    addr_set = &me->distName; or me->vmDistUUID; (result out)
 *              val_1   = &me->domain; or me->vmDomain;
 *              val_2   = &me->appId; or me->vmUUID;
 *
 *              distName = <domain>.<applicationid>
 *              vmDistUUID = <vmdomain>.<vmuuid>
 *              generic = <val_1>.<val_2>
 */
static int32_t _generateDistinguishedName(int32_t *flags, uint32_t index_set,
                                          char **addr_set, uint32_t index_1,
                                          char **val_1, uint32_t index_2,
                                          uint8_t *val_2)
{
    size_t len = 0;
    int32_t ret = Object_OK;
    // Check that <first>.<second> both exist.
    T_CHECK(BIT_TST(*flags, index_1) && BIT_TST(*flags, index_2));

    // Return if it is already set
    T_CHECK(!BIT_TST(*flags, index_set));

    bool trusted = false;

    // Processes from untrusted domains need to be fully qualified.
    if (!trusted) {
        len = strlen((const char *)*val_1);
        len++;  // '.' separator
    }
    // appID or VMUUID to uuid string
    char uuid_str[UUID_STRING_LENGTH + 1];
    convert_uuid_to_string((uuid_t *)val_2, uuid_str, sizeof(uuid_str));

    // Add name to the id hash
    len += sizeof(uuid_str);
    len++;  // +terminator

    *addr_set = HEAP_ZALLOC_ARRAY(char, len);
    T_CHECK(*addr_set);

    // distName = <domainid>.<applicationid>
    snprintf(*addr_set, len, "%s.%s", *val_1, uuid_str);
    // indDistName has a flag so we set it
    BIT_SET(*flags, index_set);

exit:
    return ret;
}

/**
 * Description: This is a helper function that generates a unique id (SHA256)
 *              based on textual data. Used for distId and vmDID
 *
 * In:          me: OSIndCredentials struct pointer
 *              index_set: index of the struct field to set
 *              index_1: struct field index of first value
 *              val_1: pointer to string value
 *              index_2: struct field index of the second value
 *              val_2: pointer to the byte array
 *
 * Out:         addr_set: address of struct field to set (ex. &me->distId)
 *
 * Examples:    distId = SHA256(me->distName)
 *              vmDID = SHA256(me->vmDistUUID)
 *              val_1 = &me->domain; or me->vmDomain;
 *              val_2 = &me->appId; or me->vmUUID;
 */
static int32_t _generateDistinguishedId(int32_t *flags, char *didName,
                                        uint32_t index_set, DistId **addr_set,
                                        uint32_t index_1, char **val_1,
                                        uint32_t index_2, uint8_t *val_2)
{
    uint8_t digest[SHA256_DIGEST_LENGTH] = {0};
    int32_t ret = Object_OK;

    if (didName) {
        if (0 == strlen(didName)) {  // no terminator
            return ret;
        }

        SHA256_CTX sha256;
        SHA256_Init(&sha256);
        SHA256_Update(&sha256, didName, strlen(didName));
        SHA256_Final(digest, &sha256);

        memscpy(addr_set, sizeof(**addr_set), digest, SHA256_DIGEST_LENGTH);
        BIT_SET(*flags, index_set);
    }

    return ret;
}

/*****************************************************************************
 * ICredentials interface functions for ProcessCred
 ****************************************************************************/

/**
 * Description: Given a name (key), return the corresponding value.
 *
 * In:          name: Name (key) of key/value pair.
 *              nameLen: Length (in bytes) of name.
 *              valueLen: Length (in bytes) of value.
 *
 * Out:         value: Value of key/value pair.
 *              valueLenOut: Actual length of value.
 * Return:      Object_OK on success.
 *              All else on failure.
 */
int32_t ProcessCred_getValueByName(ProcessCred *me, const void *name,
                                   size_t nameLen, void *value, size_t valueLen,
                                   size_t *valueLenOut)
{
    void *data = NULL;
    size_t data_size = 0;
    int32_t ret = ICredentials_ERROR_NOT_FOUND;
    *valueLenOut = 0;

    T_CHECK(me);

    switch (_strKey((const char *)name, nameLen)) {
        case eAppName:  // AppName
        {
            if (BIT_TST(me->flags, indAppName)) {
                data = me->appName;
                data_size = strlen(me->appName) + 1;
            }
            break;
        }
        case eAppID:  // appId
        {
            if (BIT_TST(me->flags, indAppID)) {
                data = &me->appId;
                data_size = sizeof(me->appId);
            }
            break;
        }
        case eAppHash:  // appHash
        {
            if (BIT_TST(me->flags, indAppHash)) {
                data = &me->appHash;
                data_size = sizeof(me->appHash);
            }
            break;
        }
        case eAppDebug:  // appDebug
        {
            if (BIT_TST(me->flags, indAppDebug)) {
                data = &me->appDebug;
                data_size = sizeof(me->appDebug);
            }
            break;
        }
        case eAppVersion:  // appVersion
        {
            if (BIT_TST(me->flags, indAppVersion)) {
                data = &me->appVersion;
                data_size = sizeof(me->appVersion);
            }
            break;
        }
        case eLegacyCBOR:  // legacyCBOR
        {
            if (BIT_TST(me->flags, indLegacyCBOR)) {
                data = &me->legacyCBOR;
                data_size = sizeof(me->legacyCBOR);
            }
            break;
        }
        case eDistinguishedID:  // distId
        {
            if (BIT_TST(me->flags, indDistID)) {
                data = &me->distId;
                data_size = sizeof(me->distId);
            }
            break;
        }
        case eDistName:  // distName
        {
            if (BIT_TST(me->flags, indDistName)) {
                data = me->distName;
                data_size = strlen(me->distName) + 1;
            }
            break;
        }
        case eDomain:  // domain
        {
            if (BIT_TST(me->flags, indDomain)) {
                data = me->domain;
                data_size = strlen(me->domain) + 1;
            }
            break;
        }
        case eUID:  // uid
        {
            if (BIT_TST(me->flags, indUID)) {
                data = &me->uid;
                data_size = sizeof(me->uid);
            }
            break;
        }
        case ePID:  // pid
        {
            if (BIT_TST(me->flags, indPID)) {
                data = &me->pid;
                data_size = sizeof(me->pid);
            }
            break;
        }
        case eSecurityLevel:  // securityLvl
        {
            if (BIT_TST(me->flags, indSecurityLevel)) {
                data = &me->securityLvl;
                data_size = sizeof(me->securityLvl);
            }
            break;
        }
        case ePerm:  // uid
        {
            if (BIT_TST(me->flags, indPerm)) {
                data = &me->permissions;
                data_size = sizeof(me->permissions);
            }
            break;
        }

        default: {
            break;
        }
    }

    // If the value is found in current credential, then we set the value.
    if (NULL != data && 0 != data_size) {
        if (memscpy(value, valueLen, data, data_size) == data_size) {
            *valueLenOut = data_size;
            ret = Object_OK;
        } else {
            // zero the value that was set with memscpy above
            memset(value, 0, valueLen);
            ret = ICredentials_ERROR_VALUE_SIZE;
        }
    }

exit:

    return ret;
}

/**
 * Description: Given an index, return the name (key) and value pair.
 *
 * In:          index: Index of key/value pair array.
 *              nameLen: Length (in bytes) of name.
 *              valueLen: Length (in bytes) of value.
 *
 * Out:         name: Name (key) of key/value pair.
 *              nameLenOut: Actual length of name.
 *              value: Value of key/value pair.
 *              valueLenOut: Actual length of value.
 *
 * Return:      Object_OK on success.
 *              All else on failure.
 */
int32_t ProcessCred_getPropertyByIndex(ProcessCred *me, uint32_t index,
                                       void *name, size_t nameLen,
                                       size_t *nameLenOut, void *value,
                                       size_t valueLen, size_t *valueLenOut)
{
    size_t data_size = 0;
    size_t nameLength = 0;
    const char *index_to_key_chars = NULL;
    int32_t ret = Object_OK;

    T_CHECK_ERR(me, ICredentials_ERROR_NOT_FOUND);
    // OSind_FLAGS covers both proc/env so that OSIndCredentials (which uses an
    // OSIndCredentials struct) can use the ICredentials interface
    T_CHECK_ERR(index >= firstProcCredIndex, ICredentials_ERROR_NOT_FOUND);
    T_CHECK_ERR(index <= lastProcCredIndex, ICredentials_ERROR_NOT_FOUND);

    index_to_key_chars = indToEKeyChar[index];
    nameLength = strlen(indToEKeyChar[index]);
    memscpy(name, nameLen, index_to_key_chars, nameLength);
    T_GUARD(ProcessCred_getValueByName(me, index_to_key_chars, nameLength,
                                       value, SCRATCH_PAD_SIZE, &data_size));

exit:
    if (Object_isERROR(ret)) {
        // clearing out values because of error
        memset(name, 0, nameLen);
        memset(value, 0, valueLen);
        *nameLenOut = 0;
        *valueLenOut = 0;
    }
    return ret;
}

/**
 * Description: This function will free the allocated memory used by the input
 *              OSIndCredentials struct (both heap pointers and objects)
 *
 * In:          me: OSIndCredentials struct pointer
 */
static void _destructProcessCreds(ProcessCred *me)
{
    if (!me) {
        return;
    }
    // Free the char * 's in the struct
    HEAP_FREE_PTR(me->appName);
    HEAP_FREE_PTR(me->distName);
    HEAP_FREE_PTR(me->domain);
    HEAP_FREE_PTR(me);
}

/**
 * Description: implements the ICredentials object release function.
 *              destructs the ProcessCred if no one else is using it.
 *              (me->refs = 0)
 *
 * In:          me: ProcessCred
 *
 * Return:      Object_OK on success.
 *              All else on failure.
 */
static int32_t ProcessCred_release(ProcessCred *me)
{
    // atomicAdd for thread safety
    if (atomicAdd(&me->refs, -1) == 0) {
        _destructProcessCreds(me);
    }
    return Object_OK;
}

/**
 * Description: implements the ICredentials object retain function.
 *              increments the refs counter
 *
 * In:          me: ProcessCred
 *
 * Return:      Object_OK on success.
 *              All else on failure.
 */
static int32_t ProcessCred_retain(ProcessCred *me)
{
    atomicAdd(&me->refs, 1);
    return Object_OK;
}

static ICredentials_DEFINE_INVOKE(ProcessCred_invoke, ProcessCred_,
                                  ProcessCred *);

/*****************************************************************************
 * ICredentials interface functions for EnvCred
 ****************************************************************************/

/**
 * Description: Given a name (key), return the corresponding value.
 *
 * In:          name: Name (key) of key/value pair.
 *              nameLen: Length (in bytes) of name.
 *              valueLen: Length (in bytes) of value.
 *
 * Out:         value: Value of key/value pair.
 *              valueLenOut: Actual length of value.
 * Return:      Object_OK on success.
 *              All else on failure.
 */
int32_t EnvCred_getValueByName(EnvCred *me, const void *name, size_t nameLen,
                               void *value, size_t valueLen,
                               size_t *valueLenOut)
{
    void *data = NULL;
    size_t data_size = 0;
    int32_t ret = ICredentials_ERROR_NOT_FOUND;
    *valueLenOut = 0;

    T_CHECK(me);

    switch (_strKey((const char *)name, nameLen)) {
        case eOSid:  // osId
        {
            if (BIT_TST(me->flags, indOSid)) {
                data = me->osId;
                data_size = strlen(me->osId) + 1;
            }
            break;
        }
        case eVmUUID:  // vmUUID
        {
            if (BIT_TST(me->flags, indVmUUID)) {
                data = &me->vmUUID;
                data_size = sizeof(me->vmUUID);
            }
            break;
        }
        case eVmDID:  // vmDID
        {
            if (BIT_TST(me->flags, indVmDID)) {
                data = &me->vmDID;
                data_size = sizeof(me->vmDID);
            }
            break;
        }
        case eVmHash:  // vmHash
        {
            if (BIT_TST(me->flags, indVmHash)) {
                data = &me->vmHash;
                data_size = sizeof(me->vmHash);
            }
            break;
        }
        case eVmDomain:  // vmDomain
        {
            if (BIT_TST(me->flags, indVmDomain)) {
                data = me->vmDomain;
                data_size = strlen(me->vmDomain) + 1;
            }
            break;
        }
        case eVmDistUUID:  // vmDistUUID
        {
            if (BIT_TST(me->flags, indVmDistUUID)) {
                data = me->vmDistUUID;
                data_size = strlen(me->vmDistUUID) + 1;
            }
            break;
        }
        case eVmVersion:  // vmVersion
        {
            if (BIT_TST(me->flags, indVmVersion)) {
                data = &me->vmVersion;
                data_size = sizeof(me->vmVersion);
            }
            break;
        }

        default: {
            break;
        }
    }

    // If the value is found in current credential, then we set the value.
    if (NULL != data && 0 != data_size) {
        if (memscpy(value, valueLen, data, data_size) == data_size) {
            *valueLenOut = data_size;
            ret = Object_OK;
        } else {
            // zero the value that was set with memscpy above
            memset(value, 0, valueLen);
            ret = ICredentials_ERROR_VALUE_SIZE;
        }
    }

exit:

    return ret;
}

/**
 * Description: Given an index, return the name (key) and value pair.
 *
 * In:          index: Index of key/value pair array.
 *              nameLen: Length (in bytes) of name.
 *              valueLen: Length (in bytes) of value.
 *
 * Out:         name: Name (key) of key/value pair.
 *              nameLenOut: Actual length of name.
 *              value: Value of key/value pair.
 *              valueLenOut: Actual length of value.
 *
 * Return:      Object_OK on success.
 *              All else on failure.
 */
int32_t EnvCred_getPropertyByIndex(EnvCred *me, uint32_t index, void *name,
                                   size_t nameLen, size_t *nameLenOut,
                                   void *value, size_t valueLen,
                                   size_t *valueLenOut)
{
    size_t data_size = 0;
    size_t nameLength = 0;
    const char *index_to_key_chars = NULL;
    int32_t ret = Object_OK;

    T_CHECK_ERR(me, ICredentials_ERROR_NOT_FOUND);
    // OSind_FLAGS covers both proc/env so that OSIndCredentials (which uses an
    // OSIndCredentials struct) can use the ICredentials interface
    // from indOSid to endOfIndex are each an env index
    T_CHECK_ERR(index >= firstEnvCredIndex, ICredentials_ERROR_NOT_FOUND);
    T_CHECK_ERR(index <= lastEnvCredIndex, ICredentials_ERROR_NOT_FOUND);

    index_to_key_chars = indToEKeyChar[index];
    nameLength = strlen(indToEKeyChar[index]);
    memscpy(name, nameLen, index_to_key_chars, nameLength);
    T_GUARD(EnvCred_getValueByName(me, index_to_key_chars, nameLength, value,
                                   SCRATCH_PAD_SIZE, &data_size));

exit:
    if (Object_isERROR(ret)) {
        // clearing out values because of error
        memset(name, 0, nameLen);
        memset(value, 0, valueLen);
        *nameLenOut = 0;
        *valueLenOut = 0;
    }
    return ret;
}

/**
 * Description: This function will free the allocated memory used by the input
 *              OSIndCredentials struct (both heap pointers and objects)
 *
 * In:          me: OSIndCredentials struct pointer
 */
static void _destructEnvCreds(EnvCred *me)
{
    if (!me) {
        return;
    }
    // Free the char * 's in the struct
    HEAP_FREE_PTR(me->osId);
    HEAP_FREE_PTR(me->vmDomain);
    HEAP_FREE_PTR(me->vmDistUUID);
    HEAP_FREE_PTR(me);
}

/**
 * Description: implements the ICredentials object release function.
 *              destructs the EnvCred if no one else is using it.
 *              (me->refs = 0)
 *
 * In:          me: EnvCred
 *
 * Return:      Object_OK on success.
 *              All else on failure.
 */
static int32_t EnvCred_release(EnvCred *me)
{
    // atomicAdd for thread safety
    if (atomicAdd(&me->refs, -1) == 0) {
        _destructEnvCreds(me);
    }
    return Object_OK;
}

/**
 * Description: implements the ICredentials object retain function.
 *              increments the refs counter
 *
 * In:          me: EnvCred
 *
 * Return:      Object_OK on success.
 *              All else on failure.
 */
static int32_t EnvCred_retain(EnvCred *me)
{
    atomicAdd(&me->refs, 1);
    return Object_OK;
}

static ICredentials_DEFINE_INVOKE(EnvCred_invoke, EnvCred_, EnvCred *);

/*****************************************************************************
 * ICredentials interface functions for OSIndCredentials
 ****************************************************************************/

/**
 * Description: Given a name (key), return the corresponding value.
 *
 * In:          name: Name (key) of key/value pair.
 *              nameLen: Length (in bytes) of name.
 *              valueLen: Length (in bytes) of value.
 *
 * Out:         value: Value of key/value pair.
 *              valueLenOut: Actual length of value.
 * Return:      Object_OK on success.
 *              All else on failure.
 */
int32_t OSIndCredentials_getValueByName(OSIndCredentials *me, const void *name,
                                        size_t nameLen, void *value,
                                        size_t valueLen, size_t *valueLenOut)
{
    int32_t ret = ICredentials_ERROR_NOT_FOUND;
    *valueLenOut = 0;

    T_CHECK(me);

    // OSIndCredentials wraps the procCred and the envCred so we check each
    // starting with envCred
    if (!Object_isNull(me->envCred)) {
        T_CALL_CHECK(ICredentials_getValueByName(
                     me->envCred, name, nameLen, value, valueLen, valueLenOut),
                     ret == ICredentials_ERROR_NOT_FOUND);
    }

    // If not in the envCred, Then we check the procCred
    if (!Object_isNull(me->procCred)) {
        ret = ICredentials_getValueByName(me->procCred, name, nameLen, value,
                                        valueLen, valueLenOut);
    }

exit:
    if (ret != Object_OK) {
        LOG_MSG("OSIndCredentials_getValueByName name=%s failed",
                (const char *)name);
    }

    return ret;
}

/**
 * Description: Given an index, return the name (key) and value pair.
 *
 * In:          index: Index of key/value pair array.
 *              nameLen: Length (in bytes) of name.
 *              valueLen: Length (in bytes) of value.
 *
 * Out:         name: Name (key) of key/value pair.
 *              nameLenOut: Actual length of name.
 *              value: Value of key/value pair.
 *              valueLenOut: Actual length of value.
 *
 * Return:      Object_OK on success.
 *              All else on failure.
 */
int32_t OSIndCredentials_getPropertyByIndex(OSIndCredentials *me,
                                            uint32_t index, void *name,
                                            size_t nameLen, size_t *nameLenOut,
                                            void *value, size_t valueLen,
                                            size_t *valueLenOut)
{
    size_t data_size = 0;
    size_t nameLength = 0;
    const char *index_to_key_chars = NULL;
    int32_t ret = Object_OK;

    T_CHECK_ERR(me, ICredentials_ERROR_NOT_FOUND);
    T_CHECK_ERR(index >= 0, ICredentials_ERROR_NOT_FOUND);
    T_CHECK_ERR(index < endOfIndex, ICredentials_ERROR_NOT_FOUND);

    index_to_key_chars = indToEKeyChar[index];
    nameLength = strlen(indToEKeyChar[index]);
    memscpy(name, nameLen, index_to_key_chars, nameLength);
    T_GUARD(OSIndCredentials_getValueByName(me, index_to_key_chars, nameLength,
                                            value, SCRATCH_PAD_SIZE,
                                            &data_size));

exit:
    if (Object_isERROR(ret)) {
        // clearing out values because of error
        memset(name, 0, nameLen);
        memset(value, 0, valueLen);
        *nameLenOut = 0;
        *valueLenOut = 0;
    }
    return ret;
}

#ifdef VERBOSE_DEBUG
/**
 * Description: Dump the credentials for debugging
 *
 * In:          me: pointer to OSIndCredentials struct
 */
static int32_t _dumpCredentials(OSIndCredentials *me)
{
    // for each field that is set, print the value
    int32_t ret = Object_ERROR;
    void *pValue = (void *)malloc(SCRATCH_PAD_SIZE * sizeof(char));
    void *pName = (void *)malloc(SCRATCH_PAD_SIZE * sizeof(char));
    size_t pNameLenOut, pValueLenOut;

    for (size_t i = 0; i < endOfIndex; i++) {
        OSIndCredentials_getPropertyByIndex(me, i, pName, SCRATCH_PAD_SIZE,
                                            &pNameLenOut, pValue,
                                            SCRATCH_PAD_SIZE, &pValueLenOut);
        _printAccordingToType(i, pValue);
        // reset scratch pads
        memset(pValue, 0, SCRATCH_PAD_SIZE * sizeof(char));
        memset(pName, 0, SCRATCH_PAD_SIZE * sizeof(char));
    }
    HEAP_FREE_PTR(pValue);
    HEAP_FREE_PTR(pName);
    ret = Object_OK;
    return ret;
}
#endif

/**
 * Description: This function will free the allocated memory used by the input
 *              OSIndCredentials struct (both heap pointers and objects)
 *
 * In:          me: OSIndCredentials struct pointer
 */
static void _destructCredentials(OSIndCredentials *me)
{
    if (!me) {
        return;
    }
    // dump values for for debugging
#ifdef VERBOSE_DEBUG
    _dumpCredentials(me);
#endif
    // Free the wrapped credentials in the struct
    Object_ASSIGN_NULL(me->procCred);
    Object_ASSIGN_NULL(me->envCred);
    HEAP_FREE_PTR(me);
}

/**
 * Description: implements the ICredentials object release function.
 *              destructs the OSIndCredentials if no one else is using it.
 *              (me->refs = 0)
 *
 * In:          me: OSIndCredential
 *
 * Return:      Object_OK on success.
 *              All else on failure.
 */
static int32_t OSIndCredentials_release(OSIndCredentials *me)
{
    // atomicAdd for thread safety
    if (atomicAdd(&me->refs, -1) == 0) {
        _destructCredentials(me);
    }
    return Object_OK;
}

/**
 * Description: implements the ICredentials object retain function.
 *              increments the refs counter
 *
 * In:          me: OSIndCredential
 *
 * Return:      Object_OK on success.
 *              All else on failure.
 */
static int32_t OSIndCredentials_retain(OSIndCredentials *me)
{
    atomicAdd(&me->refs, 1);
    return Object_OK;
}

static ICredentials_DEFINE_INVOKE(OSIndCredentials_invoke, OSIndCredentials_,
                                  OSIndCredentials *);

/*****************************************************************************
 * OSIndCredentials Constructors
 ****************************************************************************/

/**
 * Description: Generates an ICredentials object for the process
 *
 * In:          permissions: bitmask
 *              appName: Application name (optional)
 *              appHash: SHA256 of the application binary (optional)
 *              appDebug: debug value (optional)
 *              appVersion: application version (optional)
 *              legacyCBOR: client credentials as a CBOR blob (optional)
 *              legacyCBORCredentials_len: Length of CBOR blob (optional)
 *              domain: current "domain" in ICredentials (qti,oem,alt,isv,unk)
 *              pid: process id
 *              uid: user id
 *
 * Out:         outCredentials: ICredentials Object
 *
 * Return:      Object_OK on success.
 *              All else on failure.
 */
int32_t OSIndCredentials_newProcessCred(uint64_t permissions,
                                        const char *appName, SHA256Hash *appHash,
                                        uint32_t appDebug, uint32_t appVersion,
                                        uint8_t *legacyCBOR,
                                        uint32_t legacyCBORCredentials_len,
                                        const char *domain, uint32_t pid,
                                        uint32_t uid, Object *outCredentials)
{
    int32_t ret = Object_OK;
    ProcessCred *me = HEAP_ZALLOC_TYPE(ProcessCred);
    T_CHECK_ERR(me, Object_ERROR_MEM);
    me->refs = 1;
    uint32_t setSecurityLvl = 2;

    // set input values (others are generated)
    me->permissions = permissions;
    BIT_SET(me->flags, indPerm);
    if (appName) {
        T_GUARD(_setCharField(&me->flags, indAppName, &me->appName, appName));
    }

    if (appHash) {
        memscpy(&me->appHash, sizeof(me->appHash), appHash, sizeof(SHA256Hash));
        BIT_SET(me->flags, indAppHash);
    }
    me->appDebug = appDebug;
    BIT_SET(me->flags, indAppDebug);
    me->appVersion = appVersion;
    BIT_SET(me->flags, indAppVersion);
    if (legacyCBOR) {
        // system input
        memscpy(me->legacyCBOR, sizeof(me->legacyCBOR), legacyCBOR,
                legacyCBORCredentials_len);
        BIT_SET(me->flags, indLegacyCBOR);
    } else {
#ifdef QCBOR_LA
        uint32_t encodedBufLen = 0;
        _generate_CBOR(me, &encodedBufLen);
#endif
    }

    T_CHECK(domain);
    T_GUARD(_setCharField(&me->flags, indDomain, &me->domain, domain));

    me->securityLvl = setSecurityLvl;
    BIT_SET(me->flags, indSecurityLevel);

    // generate UUID's (appId, uid, pid)
    char uid_name[MAX_UID_STR_LENGTH];
    char pid_name[MAX_UID_STR_LENGTH];
    int uid_str_len =
        snprintf(uid_name, MAX_UID_STR_LENGTH, "%d", uid % 100000);
    int pid_str_len = snprintf(pid_name, MAX_UID_STR_LENGTH, "%d", pid);
    T_GUARD(_generate_uuid(&me->appId, &AppUUIDNamespace, uid_name, uid_str_len,
                           &me->flags, indAppID));
    T_GUARD(_generate_uuid(&me->pid, &ProcessIDNamespace, pid_name, pid_str_len,
                           &me->flags, indPID));
    T_GUARD(_generate_uuid(&me->uid, &UserIdNamespace, uid_name, uid_str_len,
                           &me->flags, indUID));

    // generate DistName and the distId
    T_GUARD(_generateDistinguishedName(
        &me->flags, indDistName, (char **)&me->distName, indDomain,
        (char **)&me->domain, indAppID, (uint8_t *)&me->appId.val));
    T_GUARD(_generateDistinguishedId(
        &me->flags, me->distName, indDistID, (DistId **)&me->distId, indDomain,
        &me->domain, indAppID, (uint8_t *)&me->appId.val));

    // set outCredentials using ICredentials_DEFINE_INVOKE
    *outCredentials = (Object){ProcessCred_invoke, me};

exit:
    if (Object_isERROR(ret)) {
        _destructProcessCreds(me);
    }
    return ret;
}

/**
 * Description: Generates an ICredentials object for the environment
 *
 * In:          osID: operating system ID (optional)
 *              vmUUID: uuid for the VM
 *              vmHash: SHA256 of the VM binary (optional)
 *              vmDomain: current "domain" in ICredentials (qti,oem,alt,isv,unk)
 *              vmVersion: version number (optional)
 *
 * Out:         outCredentials: an ICredentials object
 *
 * Return:      Object_OK on success.
 *              All else on failure.
 */
int32_t OSIndCredentials_newEnvCred(const char *osID, uint8_t *vmUUID,
                                    uint8_t *vmHash, const char *vmDomain,
                                    uint32_t vmVersion, Object *outCredentials)
{
    int32_t ret = Object_OK;

    EnvCred *me = HEAP_ZALLOC_TYPE(EnvCred);
    T_CHECK_ERR(me, Object_ERROR_MEM);
    me->refs = 1;

    // set input values (others are generated)
    if (osID) {
        T_GUARD(_setCharField(&me->flags, indOSid, &me->osId, osID));
    }
    memscpy(&me->vmUUID, sizeof(me->vmUUID), vmUUID, sizeof(uuid_t));
    if (vmHash) {
        memscpy(&me->vmHash, sizeof(me->vmHash), vmHash, sizeof(SHA256Hash));
        BIT_SET(me->flags, indVmHash);
    }
    T_GUARD(_setCharField(&me->flags, indVmDomain, &me->vmDomain, vmDomain));
    me->vmVersion = vmVersion;

    // set flags now because _generate checks if certain fields exist using
    // flags
    BIT_SET(me->flags, indOSid);
    BIT_SET(me->flags, indVmUUID);
    BIT_SET(me->flags, indVmDomain);
    BIT_SET(me->flags, indVmVersion);

    // generate values that need to be generated
    T_GUARD(_generateDistinguishedName(
        &me->flags, indVmDistUUID, (char **)&me->vmDistUUID, indVmDomain,
        (char **)&me->vmDomain, indVmUUID, (uint8_t *)&me->vmUUID.val));
    T_GUARD(_generateDistinguishedId(
        &me->flags, me->vmDistUUID, indVmDID, (DistId **)&me->vmDID,
        indVmDomain, &me->vmDomain, indVmUUID, (uint8_t *)&me->vmUUID));

    // set outCredentials using ICredentials_DEFINE_INVOKE
    *outCredentials = (Object){EnvCred_invoke, me};

exit:
    if (Object_isERROR(ret)) {
        _destructEnvCreds(me);
    }
    return ret;
}

/**
 * Description: Generates an ICredentials object that wraps the proc and env
 *              credentials
 *
 * In:          procCred: The process ICredentials object
 *              envCred: The env ICredentials
 *
 * Out:         outCredentials: an ICredentials object
 *
 * Return:      Object_OK on success.
 *              All else on failure.
 */
int32_t OSIndCredentials_WrapCredentials(Object *procCred, Object *envCred,
                                         Object *outCredentials)
{
    int32_t ret = Object_OK;
    OSIndCredentials *me = HEAP_ZALLOC_TYPE(OSIndCredentials);
    T_CHECK_ERR(me, Object_ERROR_MEM);

    T_CHECK(procCred);
    T_CHECK(envCred);

    me->refs = 1;

    Object_INIT(me->procCred, *procCred);
    Object_INIT(me->envCred, *envCred);

    // set outCredentials using ICredentials_DEFINE_INVOKE
    *outCredentials = (Object){OSIndCredentials_invoke, me};

exit:
    if (Object_isERROR(ret)) {
        _destructCredentials(me);
    }
    return ret;
}

/**
 * Description: Generates an ICredentials object for local vendor clients
 *
 * In:          appName: Application name
 *              pid: process id
 *              uid: user id
 *
 * Out:         outCredentials: an ICredentials object
 *
 * Return:      Object_OK on success.
 *              All else on failure.
 */
int32_t OSIndCredentials_newLAVendor(const char *appName, uint32_t pid,
                                     uint32_t uid, Object *outCredentials)
{
    // LAVendor specific values that are not passed
    uint64_t permissionsLAVendor = 0;
    uint32_t appDebugLAVendor = 0;
    uint32_t appVersionLAVendor = 0;
    char *domainLAVendor = "oem";

    return OSIndCredentials_newProcessCred(
        permissionsLAVendor, appName, NOT_USED_POINTER, appDebugLAVendor,
        appVersionLAVendor, NOT_USED_POINTER, NOT_USED, domainLAVendor, pid,
        uid, outCredentials);
}

/**
 * Description: Generates an ICredentials object for APK clients
 *
 * In:          appName: Application name
 *              appHash: SHA256 of the application binary
 *              appDebug: debug value (optional)
 *              appVersion: application version (optional)
 *              legacyCBOR: client credentials as a CBOR blob
 *              legacyCBORCredentials_len: Length of CBOR blob
 *              domain: current "domain" in ICredentials (qti,oem,alt,isv,unk)
 *              pid: process id
 *              uid: user id
 *
 * Out:         outCredentials: an ICredentials object
 *
 * Return:      Object_OK on success.
 *              All else on failure.
 */
int32_t OSIndCredentials_newLASystem(const char *appName, SHA256Hash *appHash,
                                     uint32_t appDebug, uint32_t appVersion,
                                     uint8_t *legacyCBOR,
                                     uint32_t legacyCBORCredentials_len,
                                     const char *domain, uint32_t pid,
                                     uint32_t uid, uint64_t permissions,
                                     Object *outCredentials)
{
    return OSIndCredentials_newProcessCred(
        permissions, appName, appHash, appDebug, appVersion, legacyCBOR,
        legacyCBORCredentials_len, domain, pid, uid, outCredentials);
}

